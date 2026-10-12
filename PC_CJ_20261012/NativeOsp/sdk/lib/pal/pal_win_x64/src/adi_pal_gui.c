/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_pal_gui.c
 * \brief: The interface source for the PAL component port for the GUI
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup pal
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "pcap.h"
#include <windows.h>
#include <conio.h>
#include <stdarg.h>

#include "adi_pal.h" /*!< The class header */
#include "adi_dbg.h" /*!< Diagnostics */
#include "adi_memmap.h" /*! Memory map */

#include "ftd2xx.h"
#include "LibFT4222.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "dll_wrappers.h"
#include <assert.h>
/*================================= DEFINES =================================*/
/* Set the below flag if SPI MOSI, MISO data has to be logged in the terminal */
#define PAL_ENABLE_SPI_LOGS         (false)

/* Set the below flag to log every SPI transfer to "osp_spi_trace.log",
 * written next to this DLL's own location (NOT a hardcoded absolute path),
 * so the trace always follows wherever the project/DLL is deployed. */
#define PAL_ENABLE_SPI_FILE_TRACE   (true)

/**
 * Max length of the UART RX buffer.
 * If the commands are longer than 20 characters, increase this size!
 */
#define ADI_UART_MAX_COMMAND_LEN    (50u)

#define pipename "\\\\.\\pipe\\ADI.E2B.LogPipe" /*!< Name of the pipe for the GUI */

/*============================ SUPPRESS WARNING =============================*/
#if defined(_MSC_VER)
#pragma warning(disable:4191)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

/*================================ DATA TYPES ===============================*/
/*! Internal PAL structure */
typedef struct PAL_DATA
{
    bool bActive; /*!< Tracks if PAL is active */
    bool bIsPcapLoaded; /*!< Flag to see if Pcap library is loaded */
    bool bIsFtdiLoaded; /*!< Flag to see if FTDI dlls are loaded */
    bool bIsSpiDevOpened; /*!< Flag to see if SPI device is opened */
    bool bIsEthDevOpened; /*!< Flag to see if Ethernet device is opened */
    uint32_t nSpiDevIdx; /*!< Index of the SPI device (required in resetMacphy) */
    uint32_t nFtdiGpioIdx; /*!< Index of the FTDI B device (required in resetMacphy) */
    uint32_t nNumSpiDevsPresent; /*!< Number of SPI devices that are connected to the host */
    HANDLE hPipe;       /*!< Handle to the named pipe */
} PAL_DATA;

typedef struct FTDI_INFO
{
    uint64_t nLocId; /* Location ID of the device */
    char sDesc[ADI_PAL_DEV_NAME_MAX_SIZE]; /* Description of the device */
} FTDI_INFO;

/*================================ PROTOTYPES ===============================*/
static void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

static DWORD WINAPI RunFunction(LPVOID lpParam);

/*=================================== DATA ==================================*/
/*! Internal PAL structure */
ADI_MEM_DATA_NOCRIT_CACHE
static PAL_DATA goPalData;

/*! The handle for each FTDI device */
ADI_MEM_DATA_CRIT_CACHE
static FT_HANDLE gahSpiFtdi[ADI_PAL_MAX_NUM_SPI_DEV];
static FT_HANDLE gahGpioFtdi[ADI_PAL_MAX_NUM_SPI_DEV];

/*! The data for each FTDI device.
    It is multiplied by 2 because each FTDI device is made of 2 instances */
ADI_MEM_DATA_CRIT_CACHE
static FTDI_INFO gaoFtdiInfo[2u * ADI_PAL_MAX_NUM_SPI_DEV];

/*! The device list data for FTDI */
ADI_MEM_DATA_CRIT_CACHE
static ADI_PAL_SPIDEVINFO gaoFtdiDevData[ADI_PAL_MAX_NUM_SPI_DEV];

/*! The handle to the pcap device */
ADI_MEM_DATA_CRIT_CACHE
static pcap_t *gahPcapDev[ADI_PAL_MAX_NUM_ETH_DEV];

/*! The handle to the FTDI library */
ADI_FTDI_LIB goFtdiLib;
/*! The handle to the FT4222 library */
ADI_FT4222_LIB goFT4222Lib;
/*! The handle to the pcap library */
ADI_PCAP_LIB goPcapLib;

static HANDLE ghThread[ADI_PAL_MAX_NUM_SPI_DEV];
static bool   gbRunThread[ADI_PAL_MAX_NUM_SPI_DEV];
static bool   gbTodoSpiSend[ADI_PAL_MAX_NUM_SPI_DEV];
static bool   gbTodoSpiCallback[ADI_PAL_MAX_NUM_SPI_DEV];
static bool   gbSpiErr[ADI_PAL_MAX_NUM_SPI_DEV];
static bool   gbIrqSet[ADI_PAL_MAX_NUM_SPI_DEV];
static uint8_t *gpReadData[ADI_PAL_MAX_NUM_SPI_DEV];
static uint8_t *gpWriteData[ADI_PAL_MAX_NUM_SPI_DEV];
static uint16_t gnSize[ADI_PAL_MAX_NUM_SPI_DEV];

/* Buffer to store the UART TX Log data */
ADI_MEM_DATA_CRIT_NOCACHE
uint8_t ganUartLogData[UART_TX_LOG_MSG_SZ];

uint8_t ganPalMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /*!< Buffer for PAL log mutex */
PRJ_OSAL_MUTEX_CFG goPalMutexCfg;          /*!< PAL log mutex configuration */
PRJ_OSAL_MUTEX_HANDLE ghPalMutex;          /*!< Handle for the PAL log mutex */

uint8_t ganTimeMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /*!< Buffer for adi_pal_getCurrTime mutex */
PRJ_OSAL_MUTEX_CFG goTimeMutexCfg;          /*!< adi_pal_getCurrTime mutex configuration */
PRJ_OSAL_MUTEX_HANDLE ghTimeMutex;          /*!< Handle for the adi_pal_getCurrTime mutex */
bool gbSignalAsyncOperation;                /*!< Flag to indicate if an async operation needs to be notified  */

/*=================================== CODE ==================================*/
/**
 * @brief Initializes the pal
 */
ADI_MEM_CODE_NOCRIT
void adi_pal_init(void)
{
    /* Clear the data */
    (void)memset(&goPalData, 0, sizeof(PAL_DATA));

    /* Set the flag */
    goPalData.bActive = true;
    goPalData.bIsPcapLoaded = false;
    goPalData.bIsFtdiLoaded = false;

    /* Try to load wpcap library */
    if(InitPcapLib(&goPcapLib))
    {
        /* Library loaded */
        goPalData.bIsPcapLoaded = true;
    }

    /* Try to load FTDI DLLs */
    if(InitFtdiDll(&goFtdiLib) && InitFt4222Dll(&goFT4222Lib))
    {
        /* Libraries loaded */
        goPalData.bIsFtdiLoaded = true;
    }

#ifdef USE_CLI
    /* Open the pipe to get all the fields */
    goPalData.hPipe = CreateFile(
                          pipename,                       /**< Name of the pipe */
                          GENERIC_READ | GENERIC_WRITE,   /**< Pipe for both read and write */
                          0,                              /**< This pipe cannot be shared elsewhere after opening here */
                          NULL,                           /**< Security attributes -> NULL as there is no child process */
                          OPEN_EXISTING,                  /**< Opens the existing file */
                          0,                              /**< No flags / attributes set */
                          NULL                            /**< No Template file */
                      );

    ADI_DBG_ENSURE(goPalData.hPipe != NULL, "Failed to open named pipe");
#endif
    goPalMutexCfg.pMem = &ganPalMutexBuffer[0];
    goPalMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
    (void)prj_osal_createMutex(&goPalMutexCfg, &ghPalMutex);

    goTimeMutexCfg.pMem = &ganTimeMutexBuffer[0];
    goTimeMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
    (void)prj_osal_createMutex(&goTimeMutexCfg, &ghTimeMutex);
    gbSignalAsyncOperation = false;
}

/**
 * @brief Closes the PAL
 */
ADI_MEM_CODE_NOCRIT
void adi_pal_close(void)
{
    /* Clear the data */
    (void)memset(&goPalData, 0, sizeof(PAL_DATA));
    ClosePcapLib(&goPcapLib);
    CloseFtdiDll(&goFtdiLib);
    CloseFt4222Dll(&goFT4222Lib);
#ifdef USE_CLI
    CloseHandle(goPalData.hPipe);
#endif
}

/**
 * @brief Allows the application to pause for specified microseconds
 *
 * @param nMSec The number of microseconds to pause
 */
ADI_MEM_CODE_CRIT
void adi_pal_waitMicroSec(uint64_t nMSec)
{
    uint64_t nEndTime = 0ull;
    uint64_t nCurrTime = 0ull;

    /* Get the current time */
    adi_pal_getCurrTime(&nEndTime);
    nEndTime += ((uint64_t)nMSec * 1000ull);

    /* Wait for the end time */
    do
    {
        adi_pal_getCurrTime((uint64_t *)&nCurrTime);
    } while(nCurrTime < nEndTime);
}

ADI_MEM_CODE_CRIT
void adi_pal_getCurrTime(uint64_t *pnCurrTime)
{
    LARGE_INTEGER nCurrentTime, nFrequency;

    do
    {
        if(prj_osal_lockMutex(ghTimeMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Get the frequency and time */
        QueryPerformanceFrequency(&nFrequency);
        QueryPerformanceCounter(&nCurrentTime);

        nCurrentTime.QuadPart *= 1000000;
        nCurrentTime.QuadPart /= nFrequency.QuadPart;

        /* Convert to nanoseconds */
        nCurrentTime.QuadPart *= 1000;

        *pnCurrTime = nCurrentTime.QuadPart;

        if(prj_osal_unlockMutex(ghTimeMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            break;
        }
    } while(false);
}

ADI_MEM_CODE_NOCRIT
void adi_pal_ledSet(uint8_t nLedIdx, bool bState)
{
    /* Nothing to do */
}

ADI_MEM_CODE_NOCRIT
void adi_pal_platformLedSet(uint8_t nLed, bool bState)
{
    // TODO: Implement!
}

#ifdef MACPHY_EN

/**
 * @brief Get the list of all SPI devices
 *
 * @param pnNumDevices Returns the number of devices
 * @param aoDevInfo    This array is filled with the name and description of each device
 * @param nMaxNumSpiDevs Maximum number of SPI devices
 */
ADI_MEM_CODE_CRIT
void adi_pal_getSpiDevs(uint32_t *pnNumDevices, ADI_PAL_SPIDEVINFO *aoDevInfo, uint32_t nMaxNumSpiDevs)
{
    /* The return status for the create device info list */
    FT_STATUS ftStatus = 0;
    /* Device index */
    uint32_t nDevIdx = 0U;
    /* Device pair count */
    uint32_t nNumPairs = 0u;
    /* Location ID counter variable */
    uint32_t nLocIdCnt = 0u;
    /* The loop variable for the devices */
    uint32_t iDev;
    /* Check that the pointer to where the number of FTDI devices can be saved is not null */
    ADI_DBG_REQUIRE(pnNumDevices != NULL, "Pointer to number of FTDI devices is null");
    /* Check that the pointer to the devices info is not null */
    ADI_DBG_REQUIRE(aoDevInfo != NULL, "Pointer to the device info is null");

    /* If FTDI dlls are not loaded, return the number of devices as 0 */
    if(goPalData.bIsFtdiLoaded == false)
    {
        *pnNumDevices = 0U;
        return;
    }

    /* Find all the FTDI devices connected */
    ftStatus = goFtdiLib.getDeviceInfo((LPDWORD)pnNumDevices);
    ADI_DBG_REQUIRE(ftStatus == FT_OK, "FTDI device not connected");

    /* Clear the device info */
    (void) memset(gaoFtdiInfo, 0, sizeof(gaoFtdiInfo));

    /* Loop through all the available FTDI devices */
    for(iDev = 0; iDev < *pnNumDevices; ++iDev)
    {
        FT_DEVICE_LIST_INFO_NODE devInfo;
        (void)memset(&devInfo, 0, sizeof(FT_DEVICE_LIST_INFO_NODE));

        ftStatus = goFtdiLib.getDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
                   devInfo.SerialNumber,
                   devInfo.Description,
                   &devInfo.ftHandle);

        if(FT_OK == ftStatus)
        {
            /* The FTDI device info detail */
            if((strncmp(devInfo.Description, "FT4222 A", 8) == 0) || (strncmp(devInfo.Description, "FT4222 B", 8) == 0))
            {
                gaoFtdiInfo[nDevIdx].nLocId = (uint64_t)devInfo.LocId;
                (void) strncpy(gaoFtdiInfo[nDevIdx].sDesc, devInfo.Description, ADI_PAL_DEV_NAME_MAX_SIZE);
                nDevIdx++;

                if(nDevIdx == (2u * ADI_PAL_MAX_NUM_SPI_DEV))
                {
                    break;
                }
            }
        }
    }

    /* Cache the number of SPI devices in the PAL data structure */
    goPalData.nNumSpiDevsPresent = nDevIdx;

    /* Extract the FTDI device pairs */
    for(uint32_t i = 0u; i < nDevIdx; i++)
    {
        if(strncmp(gaoFtdiInfo[i].sDesc, "FT4222 A", 8) == 0)
        {
            gaoFtdiDevData[nNumPairs].nLocId = gaoFtdiInfo[i].nLocId / 0x10u;
            gaoFtdiDevData[nNumPairs].nGpioLocId = 0xffffu;
            gaoFtdiDevData[nNumPairs].nSpiLocId = gaoFtdiInfo[i].nLocId;
            snprintf(gaoFtdiDevData[nNumPairs].sDescription, ADI_PAL_DEV_NAME_MAX_SIZE, "FT4222_Loc%I64x",
                     gaoFtdiDevData[nNumPairs].nLocId);
            nNumPairs++;
        }
    }

    for(uint32_t i = 0u; i < nDevIdx; i++)
    {
        if(strncmp(gaoFtdiInfo[i].sDesc, "FT4222 B", 8) == 0)
        {
            for(uint32_t j = 0u; j < nNumPairs; j++)
            {
                if(gaoFtdiDevData[j].nLocId == (gaoFtdiInfo[i].nLocId / 0x10u))
                {
                    gaoFtdiDevData[j].nGpioLocId = gaoFtdiInfo[i].nLocId;
                    break;
                }
            }
        }
    }

    /* Set the return values for the application */
    for(uint32_t i = 0u; i < nNumPairs; i++)
    {
        if(i == nMaxNumSpiDevs)
        {
            break;
        }

        (void) memcpy(&aoDevInfo[i], &gaoFtdiDevData[i], sizeof(ADI_PAL_SPIDEVINFO));
    }

    *pnNumDevices = nNumPairs;
}

/**
 * @brief Open the chosen SPI device
 */
ADI_MEM_CODE_NOCRIT
ADI_PAL_STATUS adi_pal_spiOpen(uint32_t nId, uint32_t nSpiClk, uint8_t nSpiClkPol, uint8_t nSpiClkPha)
{
    uint64_t nSpiLocId = 0ull;
    uint64_t nGpioLocId = 0ull;
    uint32_t nSysClk;           /* Local variable to store system clock */
    uint32_t nClkDiv;           /* Local variable to store clock Division */
    uint8_t errFlg = 0;         /* Error flag set to 0 if no error */

    nSpiLocId = gaoFtdiDevData[nId].nSpiLocId;
    nGpioLocId = gaoFtdiDevData[nId].nGpioLocId;

    /* The return value for the setclock and open ftdi */
    FT_STATUS ftStatus;

    /* If FTDI dlls are not loaded, return */
    if(goPalData.bIsFtdiLoaded == false)
    {
        return ADI_PAL_ERR_LIB_NOT_LOADED;
    }

    /* Open the FTDI device */
    ftStatus = goFtdiLib.openDeviceEx((PVOID)nSpiLocId, FT_OPEN_BY_LOCATION, &gahSpiFtdi[nId]);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_OPEN_FAILED;
    }

    /* Set Timeout for FTDI device */
    ftStatus = goFtdiLib.setTimeouts(gahSpiFtdi[nId], 1000, 1000);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_OPEN_FAILED;
    }

    switch(nSpiClk)
    {
        case 5000000U:
        {
            nSysClk = SYS_CLK_80;
            nClkDiv = CLK_DIV_16;
            break;
        }

        case 6000000U:
        {
            nSysClk = SYS_CLK_24;
            nClkDiv = CLK_DIV_4;
            break;
        }

        case 7500000U:
        {
            nSysClk = SYS_CLK_60;
            nClkDiv = CLK_DIV_8;
            break;
        }

        case 10000000U:
        {
            nSysClk = SYS_CLK_80;
            nClkDiv = CLK_DIV_8;
            break;
        }

        case 12000000U:
        {
            nSysClk =  SYS_CLK_24;
            nClkDiv = CLK_DIV_2;
            break;
        }

        case 15000000U:
        {
            nSysClk = SYS_CLK_60;
            nClkDiv = CLK_DIV_4;
            break;
        }

        case 20000000U:
        {
            nSysClk = SYS_CLK_80;
            nClkDiv =  CLK_DIV_4;
            break;
        }

        default:
        {
            errFlg = 1;
            break;
        }
    }

    if(errFlg)
    {
        return ADI_PAL_ERR_DEV_SET_CLK_FAILED;
    }
    else
    {
        ftStatus = goFT4222Lib.setClock(gahSpiFtdi[nId], nSysClk);

        if(ftStatus != FT_OK)
        {
            return ADI_PAL_ERR_DEV_SET_CLK_FAILED;
        }

        ftStatus = goFT4222Lib.spiMasterInit(gahSpiFtdi[nId], SPI_IO_SINGLE, nClkDiv, nSpiClkPol, nSpiClkPha, 0x01U);
    }

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_INIT_FAILED;
    }

    /* Open the device in the next index (GPIO used for reset) */
    /* NOTE: Make sure the mode is 00 for FTDI for this to work! */
    ftStatus = goFtdiLib.openDeviceEx((PVOID)nGpioLocId, FT_OPEN_BY_LOCATION, &gahGpioFtdi[nId]);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_OPEN_FAILED;
    }

    /* Disable suspend out, it uses the same pin as GPIO2 */
    ftStatus = goFT4222Lib.setSuspendOut(gahGpioFtdi[nId], false);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_OPEN_FAILED;
    }

    /* Set the direction for the GPIO pins as output */
    GPIO_Dir gpioDir[4];
    gpioDir[0] = GPIO_OUTPUT;
    gpioDir[1] = GPIO_OUTPUT;
    gpioDir[2] = GPIO_OUTPUT;
    gpioDir[3] = GPIO_INPUT;

    /* Initialize the GPIO */
    ftStatus = goFT4222Lib.gpioInit(gahGpioFtdi[nId], gpioDir);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_INIT_FAILED;
    }

    /* Enable OA-SPI IRQ */
    ftStatus = goFT4222Lib.setWakeUpInterrupt(gahGpioFtdi[nId], true);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_INIT_FAILED;
    }

    /* Setup interrupt level */
    ftStatus = goFT4222Lib.setInterruptTrigger(gahGpioFtdi[nId], GPIO_TRIGGER_FALLING);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_INIT_FAILED;
    }

    /* Set the flag */
    goPalData.bIsSpiDevOpened = true;

    gbTodoSpiSend[nId] = false;
    gbTodoSpiCallback[nId] = false;
    gbSpiErr[nId] = false;

    gbRunThread[nId] = true;
    ghThread[nId] = CreateThread(
                        NULL,                   // default security attributes
                        0,                      // use default stack size
                        RunFunction,            // thread function name
                        (LPVOID)(uintptr_t)nId, // argument to thread function
                        0,                      // use default creation flags
                        NULL);                  // returns the thread identifier

    if(ghThread[nId] == NULL)
    {
        return ADI_PAL_ERR_DEV_INIT_FAILED;
    }

    return ADI_PAL_OK;
}

/**
 * @brief Sends the data via the SPI
 */
ADI_MEM_CODE_CRIT
ADI_PAL_STATUS adi_pal_spiSend(uint32_t nId, uint8_t *pWriteData, uint8_t *pReadData, uint32_t nSize)
{
    gpWriteData[nId] = pWriteData;
    gpReadData[nId] = pReadData;
    gnSize[nId] = nSize;
    gbTodoSpiSend[nId] = true;

    return ADI_PAL_OK;
}

/**
 * @brief Closes the SPI
 */
ADI_MEM_CODE_NOCRIT
ADI_PAL_STATUS adi_pal_spiClose(uint32_t nId)
{
    if(goPalData.bIsFtdiLoaded == false)
    {
        return ADI_PAL_ERR_LIB_NOT_LOADED;
    }

    gbRunThread[nId] = false;
    WaitForSingleObject(ghThread[nId], INFINITE);

    FT_STATUS ftStatus = goFT4222Lib.unInitialize(gahSpiFtdi[nId]);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_UNINIT_FAILED;
    }

    ftStatus = goFtdiLib.closeDevice(gahSpiFtdi[nId]);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_CLOSE_FAILED;
    }

    goPalData.bIsSpiDevOpened = false;

    ftStatus = goFT4222Lib.unInitialize(gahGpioFtdi[nId]);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_UNINIT_FAILED;
    }

    ftStatus = goFtdiLib.closeDevice(gahGpioFtdi[nId]);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_CLOSE_FAILED;
    }

    return ADI_PAL_OK;
}

ADI_MEM_CODE_CRIT
void adi_pal_spiRun(uint32_t nId)
{
    if(gbSpiErr[nId])
    {
        gbSpiErr[nId] = false;
        bool bError = true;
        adi_pal_networkCbk(nId, ADI_PAL_SPI_CBK, &bError);
    }

    if(gbIrqSet[nId])
    {
        gbIrqSet[nId] = false;
        adi_pal_networkCbk(nId, ADI_PAL_EVT_IRQ_CBK, NULL);
    }

    if(gbTodoSpiCallback[nId])
    {
        gbTodoSpiCallback[nId] = false;

        /* Notify completion of SPI */
        bool bError = false;
        adi_pal_networkCbk(nId, ADI_PAL_SPI_CBK, &bError);
    }
}

#endif /* MACPHY_EN */

#ifdef ETH_EN

/**
 * @brief Get the list of all ethernet adapters
 *
 * @param pnNumDevices Returns the number of devices
 * @param aoDevInfo    This array is filled with the name and description of each device
 * @param nMaxNumEthDevs Maximum number of ethernet devices
 */
ADI_MEM_CODE_CRIT
void adi_pal_getEthAdapters(uint32_t *pnNumDevices, ADI_PAL_ETHADAPTERSINFO *aoDevInfo, uint32_t nMaxNumEthDevs)
{
    /* Check that the pointer to the devices info is not null */
    ADI_DBG_REQUIRE(aoDevInfo != NULL, "Pointer to the device info is null");
    /* Check that the pointer to the number of devices is not null */
    ADI_DBG_REQUIRE(pnNumDevices != NULL, "Pointer to the number of devices is null");

    /* Check that pcap library is loaded */
    if(goPalData.bIsPcapLoaded == false)
    {
        *pnNumDevices = 0U;
        return;
    }

    /* Pointer to the devices */
    pcap_if_t *poAllDevs = NULL;

    /* Temp pcap device variable */
    pcap_if_t *d;

    /* Error buffer */
    char sErrBuf[PCAP_ERRBUF_SIZE];

    /* Initialize the number of devices */
    *pnNumDevices = 0U;

    /* Retrieve the device list from the local machine */
    if(goPcapLib.findAllDevs(PCAP_SRC_IF_STRING, NULL /* auth is not needed */, &poAllDevs, sErrBuf) == -1)
    {
        ADI_DBG_LOG(LOG_INFO, stdout, "Error in pcap_findalldevs_ex: %s\n", sErrBuf);
        *pnNumDevices = 0U;
        return;
    }

    for(d = poAllDevs; d != NULL; d = d->next)
    {
        aoDevInfo[*pnNumDevices].nId = *pnNumDevices;
        (void) strncpy(aoDevInfo[*pnNumDevices].sName, d->name, ADI_PAL_DEV_NAME_MAX_SIZE - 1U);
        aoDevInfo[*pnNumDevices].sName[ADI_PAL_DEV_NAME_MAX_SIZE - 1U] = '\0';

        if(d->description)
        {
            (void) strncpy(aoDevInfo[*pnNumDevices].sDesc, d->description, ADI_PAL_DEV_NAME_MAX_SIZE - 1U);
            aoDevInfo[*pnNumDevices].sDesc[ADI_PAL_DEV_NAME_MAX_SIZE - 1U] = '\0';
        }

        (*pnNumDevices)++;

        if(*pnNumDevices == nMaxNumEthDevs)
        {
            break;
        }
    }

    /* Free the device list */
    goPcapLib.freeAllDevs(poAllDevs);

    /* Set the flag */
    goPalData.bIsEthDevOpened = true;
}

/**
 * @brief Opens a particular ethernet adapter from the list of ethernet adapters
 *
 * @param nId The index of the adapter to be opened (This is the index from the list of devices added
 *                                                   by the getEthAdapters API)
 */
ADI_MEM_CODE_NOCRIT
ADI_PAL_STATUS adi_pal_openEthAdapter(uint32_t nId)
{
    /* Error buffer */
    char sErrBuf[PCAP_ERRBUF_SIZE];

    /* Temp loop variable */
    uint32_t i = 0U;

    /* Pointer to the devices */
    pcap_if_t *poAllDevs;

    /* Temp pcap device variable */
    pcap_if_t *d;

    /* Check that pcap library is loaded */
    if(goPalData.bIsPcapLoaded == false)
    {
        return ADI_PAL_ERR_LIB_NOT_LOADED;
    }

    /* Retrieve the device list from the local machine */
    if(goPcapLib.findAllDevs(PCAP_SRC_IF_STRING, NULL /* auth is not needed */, &poAllDevs, sErrBuf) == -1)
    {
        ADI_DBG_LOG(LOG_ERROR, stderr, "Error in pcap_findalldevs_ex: %s\n", sErrBuf);
        ADI_DBG_ERROR();
    }

    /* Loop through the list and get the device */
    for(d = poAllDevs; d != NULL; d = d->next)
    {
        if(i == nId)
        {
            break;
        }

        i++;
    }

    /* Ensure that the device is not null */
    ADI_DBG_ENSURE(d != NULL, "Failed device is null");

    /* Open the pcap device */
    gahPcapDev[nId] = goPcapLib.openDevice(
                          d->name,    /* The name of the ethernet adapter */
                          65536,      /* The portion of the packet to capture
                                   (65536 guarantees that the whole packet will be captured on all the link layers) */
                          PCAP_OPENFLAG_PROMISCUOUS | PCAP_OPENFLAG_NOCAPTURE_LOCAL, /* Open in promiscuous mode */
                          1000,       /* Read timeout */
                          NULL,       /* No authentication on the remote machine */
                          sErrBuf     /* Error buffer */
                      );

    if(gahPcapDev[nId] == NULL)
    {
        ADI_DBG_LOG(LOG_ERROR, stderr, "Unable to open %s adapter, because: %s\n", d->name, sErrBuf);
        return ADI_PAL_ERR_DEV_OPEN_FAILED;
    }

    /* Free the device list */
    goPcapLib.freeAllDevs(poAllDevs);

    /* Set non-blocking mode and start the capture */
    int nSetNonBlockStatus = goPcapLib.setNonBlock(gahPcapDev[nId], true, NULL);

    if(nSetNonBlockStatus == PCAP_ERROR)
    {
        return ADI_PAL_ERR_DEV_INIT_FAILED;
    }

    int nDispatchStatus = goPcapLib.dispatch(gahPcapDev[nId], 0, packet_handler, (uint8_t *)&nId);

    if(nDispatchStatus == PCAP_ERROR)
    {
        return ADI_PAL_ERR_DEV_INIT_FAILED;
    }

    return ADI_PAL_OK;
}

/**
 * @brief Sends a frame via the opened ethernet adapter
 *
 * @param nId   The index of the adapter
 * @param pData The pointer to the frame data
 * @param nSize The length of the frame
 * @param bCaptureTs Set this flag if egress timestamp has to be captured for this frame
 */
ADI_MEM_CODE_CRIT
ADI_PAL_STATUS adi_pal_ethSend(uint32_t nId, const uint8_t *pData, uint32_t nSize, bool bCaptureTs)
{
    /* Check that pcap library is loaded */
    if(goPalData.bIsPcapLoaded == false)
    {
        return ADI_PAL_ERR_LIB_NOT_LOADED;
    }

    /* Check that the pcap device handle is not NULL */
    if(gahPcapDev[nId] == NULL)
    {
        return ADI_PAL_ERR_DEV_WRITE_FAILED;
    }

    /* Send the packet to the opened ethernet adapter */
    goPcapLib.inject(gahPcapDev[nId], (void *)pData, (size_t)nSize);

    return ADI_PAL_OK;
}

ADI_MEM_CODE_NOCRIT
ADI_PAL_STATUS adi_pal_closeEthAdapter(uint32_t nId)
{
    /* Check that pcap library is loaded */
    if(goPalData.bIsPcapLoaded == false)
    {
        return ADI_PAL_ERR_LIB_NOT_LOADED;
    }

    /* Check that the pcap device handle is not NULL */
    if(gahPcapDev[nId] == NULL)
    {
        return ADI_PAL_ERR_DEV_CLOSE_FAILED;
    }

    /* Close the adapter */
    goPcapLib.closeDevice(gahPcapDev[nId]);

    /* Clear the flag */
    goPalData.bIsEthDevOpened = false;

    return ADI_PAL_OK;
}

ADI_MEM_CODE_CRIT
void adi_pal_ethRun(uint32_t nId)
{
    /* Check that pcap library is loaded */
    if(goPalData.bIsPcapLoaded)
    {
        goPcapLib.dispatch(gahPcapDev[nId], 0, packet_handler, (uint8_t *)&nId);
    }
}

ADI_MEM_CODE_CRIT
bool adi_pal_isEthTxEmpty(uint32_t nId)
{
    /* TODO (post 2.0.0): Implement */
    return true;
}

ADI_MEM_CODE_CRIT
void adi_pal_getEthMacStats(uint32_t nId)
{
    // /* Check that pcap library is loaded */
    // if(goPalData.bIsPcapLoaded == false)
    // {
    //     return;
    // }

    // struct pcap_stat stats;

    // uint32_t i = 6U;

    // ADI_PAL_STATSDAT oStatsDat;

    // /* Get the stats */
    // goPcapLib.pcapStats(gahPcapDev[nId], &stats);

    // /* Set the stats data with name */
    // /* Set all stats to 0xff, except the frame counts, which exist in pcap */
    // (void)memset(&oStatsDat, 0xFF, sizeof(ADI_PAL_STATSDAT));

    // oStatsDat.nTxFrmCnt = stats.ps_sent;

    // oStatsDat.nRxFrmCnt = stats.ps_recv;

    // adi_pal_networkCbk(nId, ADI_PAL_EVT_MAC_STATS, &oStatsDat);
}

ADI_MEM_CODE_CRIT
void adi_pal_getEthPhyStats(uint32_t nId)
{
    // TODO: Implement!
}

ADI_MEM_CODE_NOCRIT
void adi_pal_ethMacReset(uint32_t nId)
{
    // TODO: Implement!
}

ADI_MEM_CODE_NOCRIT
void adi_pal_ethSetPlca(uint32_t nId, bool bEnabled, uint32_t nNodeCount, uint32_t nNodeId)
{
    // TODO: Implement!
}

ADI_MEM_CODE_NOCRIT
void adi_pal_ethSetFilterTable(uint32_t nId, uint8_t panMacAddr[][6u], uint8_t nNumAddr)
{
    // TODO: Implement!
}

ADI_MEM_CODE_NOCRIT
void adi_pal_ethClearFilterTable(uint32_t nId)
{
    // TODO: Implement!
}

#endif /* ETH_EN */

/**
 * @brief       Trigger the wake-pin of the controller node
 *
 * @param [in]  nId         ID of the device to trigger the wake-pin
 * @return      Status
 *              - ADI_PAL_OK                    Triggered wake-pin successfully
 *              - ADI_PAL_ERR_DEV_WRITE_FAILED  Wake-pin GPIO triggering failed
 */
ADI_MEM_CODE_CRIT
ADI_PAL_STATUS adi_pal_triggerWakePin(uint32_t nId)
{
    FT_STATUS ftStatus;

    /* Set the GPIO2 pin as high */
    ftStatus = goFT4222Lib.gpioWrite(gahGpioFtdi[nId], GPIO_PORT2, true);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_WRITE_FAILED;
    }

    adi_pal_waitMicroSec(100);

    /* Set the GPIO2 pin as low */
    ftStatus = goFT4222Lib.gpioWrite(gahGpioFtdi[nId], GPIO_PORT2, false);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_WRITE_FAILED;
    }

    return ADI_PAL_OK;
}

/**
 * @brief       Trigger the TS_CAPT Pin
 *
 * @param [in]  nId         ID of the device to trigger the TS_CAPT Pin
 * @return      Status
 *              - ADI_PAL_OK                    Triggered TS_CAPT pin successfully
 *              - ADI_PAL_ERR_DEV_WRITE_FAILED  TS_CAPT GPIO triggering failed
 */
ADI_MEM_CODE_CRIT
ADI_PAL_STATUS adi_pal_triggerTsCaptPin(uint32_t nId)
{
    FT_STATUS ftStatus;

    /* Set the GPIO1 pin as high */
    ftStatus = goFT4222Lib.gpioWrite(gahGpioFtdi[nId], GPIO_PORT1, true);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_WRITE_FAILED;
    }

    adi_pal_waitMicroSec(100);

    /* Set the GPIO1 pin as low */
    ftStatus = goFT4222Lib.gpioWrite(gahGpioFtdi[nId], GPIO_PORT1, false);

    if(ftStatus != FT_OK)
    {
        return ADI_PAL_ERR_DEV_WRITE_FAILED;
    }

    return ADI_PAL_OK;
}

/**
 * Allows application to nofity a fatal error and crash
 */
ADI_MEM_CODE_NOCRIT
void adi_pal_fatalError(void)
{
    exit(1);
}

/** Allows any layer to notify a fatal error to the application */
ADI_MEM_CODE_CRIT
void adi_pal_setFatalError(void)
{
    adi_pal_appCbk(0U, ADI_PAL_EVT_FATAL_ERR, NULL);
}

/**
 * @brief       Logger function
 *
 * @param [in]  bIsPrompt   Should it add a prompt before the message (>>>>)
 * @param [in]  msg         Message to log
 * @param ...   varargs
 */
ADI_MEM_CODE_CRIT
void adi_pal_log(bool bIsPrompt, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    if(prj_osal_lockMutex(ghPalMutex) != PRJ_OSAL_STATUS_SUCCESS)
    {
        return;
    }

    if(bIsPrompt)
    {
        /* Create the log string */
        (void)snprintf((char *)&ganUartLogData[0U], UART_TX_LOG_MSG_SZ, ">>>> ");
        (void)vsnprintf((char *)&ganUartLogData[5u], UART_TX_LOG_MSG_SZ - 5U, msg, args);
    }
    else
    {
        /* Create the log string */
        (void)vsnprintf((char *)&ganUartLogData[0u], UART_TX_LOG_MSG_SZ, msg, args);
    }

    /* Send the data over UART */
    ADI_PAL_EVT_CLI_TXDATA oTxData;
    oTxData.pTxData = &ganUartLogData[0U];
    oTxData.nLength = (uint32_t)strlen((char *)ganUartLogData);
    adi_pal_appCbk(0U, ADI_PAL_EVT_CLI_TX, &oTxData);

    if(prj_osal_unlockMutex(ghPalMutex) != PRJ_OSAL_STATUS_SUCCESS)
    {
        return;
    }
}

ADI_MEM_CODE_CRIT
void adi_pal_cliRun(void)
{
    uint8_t buff[1000u] = {0u};
    uint8_t nRxByte = 0u;
    DWORD nBytesAvail;
    DWORD nBytesRead;
#ifdef USE_CLI
    ADI_DBG_REQUIRE(goPalData.hPipe != NULL, "Named pipe handle is null");
    PeekNamedPipe(goPalData.hPipe, &nRxByte, 1u, NULL, &nBytesAvail, NULL);

    if(nBytesAvail > 0U)
    {
        ReadFile(goPalData.hPipe, &buff[0u], 1000u, &nBytesRead, false);

        for(uint32_t i = 0; i < nBytesRead; i++)
        {
            adi_pal_appCbk(0U, ADI_PAL_EVT_CLI_RX, &buff[i]);
        }
    }

#endif

    // Check if a keyboard character is pressed and process it
    if(kbhit())
    {
        char ch = getch();
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "%c", ch);
        adi_pal_appCbk(0U, ADI_PAL_EVT_CLI_RX, &ch);
    }
}

ADI_MEM_CODE_CRIT
void adi_pal_sendToCLI(uint8_t *panTxData, uint32_t nSize)
{
#ifdef USE_CLI
    DWORD numWritten;
    ADI_DBG_REQUIRE(goPalData.hPipe != NULL, "Named pipe handle is null");
    WriteFile(goPalData.hPipe, panTxData, nSize, &numWritten, NULL);
#endif
}

/* Callback function invoked by libpcap for every incoming packet */
static void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
    uint32_t *pnId = (uint32_t *)param;
    ADI_PAL_EVT_ETHDATA oEthData;
    oEthData.anData = (uint8_t *)pkt_data;
    oEthData.nSize = header->caplen;
    adi_pal_networkCbk(*pnId, ADI_PAL_ETH_CBK, &oEthData);
}

#if PAL_ENABLE_SPI_FILE_TRACE == true
/**
 * @brief Appends one SPI transfer (TX/RX bytes) to "osp_spi_trace.log",
 *        located next to this DLL (whichever folder it was deployed/copied to),
 *        instead of any fixed absolute path baked in at build time.
 */
static void AdiPal_SpiTraceWrite(uint32_t nId, const uint8_t *pWriteData, const uint8_t *pReadData,
                                  uint32_t nSize, uint16_t nTransferred, FT_STATUS ftStatus)
{
    static bool bInit[ADI_PAL_MAX_NUM_SPI_DEV];
    static FILE *pFile[ADI_PAL_MAX_NUM_SPI_DEV];
    static uint32_t nSeq[ADI_PAL_MAX_NUM_SPI_DEV];

    if(!bInit[nId])
    {
        char szDllPath[MAX_PATH];
        char szLogPath[MAX_PATH];
        HMODULE hSelf = NULL;

        /* Resolve the folder this DLL is actually running from, using the
         * address of this very function as the lookup key. This makes the
         * trace path follow the DLL wherever the project copies it to. */
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCSTR)&AdiPal_SpiTraceWrite, &hSelf);

        szDllPath[0] = '\0';
        if(hSelf != NULL)
        {
            GetModuleFileNameA(hSelf, szDllPath, MAX_PATH);
        }

        {
            char *pLastSlash = strrchr(szDllPath, '\\');
            if(pLastSlash != NULL)
            {
                *(pLastSlash + 1) = '\0';

                /* Walk up from the DLL's own folder (typically bin\Debug\)
                 * to the project's top-level folder, two levels up, so the
                 * trace log lands next to LINMaster.csproj instead of deep
                 * inside bin\Debug\. */
                {
                    char szProjectRoot[MAX_PATH];
                    size_t nLen;
                    int i;

                    strncpy(szProjectRoot, szDllPath, MAX_PATH - 1U);
                    szProjectRoot[MAX_PATH - 1U] = '\0';

                    nLen = strlen(szProjectRoot);
                    if(nLen > 0U && szProjectRoot[nLen - 1U] == '\\')
                    {
                        szProjectRoot[nLen - 1U] = '\0';
                    }

                    for(i = 0; i < 2; i++)
                    {
                        char *pUp = strrchr(szProjectRoot, '\\');
                        if(pUp != NULL)
                        {
                            *pUp = '\0';
                        }
                    }

                    snprintf(szLogPath, MAX_PATH, "%s\\osp_spi_trace.log", szProjectRoot);
                }
            }
            else
            {
                /* Fallback: current working directory */
                snprintf(szLogPath, MAX_PATH, "osp_spi_trace.log");
            }
        }

        pFile[nId] = fopen(szLogPath, "a");

        /* Diagnostic-only marker written to the Windows temp folder, which is
         * essentially always writable regardless of where the DLL sits or
         * what permissions that folder has. This exists purely so we can
         * always confirm (a) this code path was reached at all, i.e. the
         * DLL currently loaded really is the rebuilt one, and (b) whether
         * the real trace log at szLogPath actually opened. */
        {
            char szTempPath[MAX_PATH];
            char szMarkerPath[MAX_PATH];
            if(GetTempPathA(MAX_PATH, szTempPath) > 0)
            {
                snprintf(szMarkerPath, MAX_PATH, "%sospbridge_trace_debug.txt", szTempPath);
                FILE *pMarker = fopen(szMarkerPath, "a");
                if(pMarker != NULL)
                {
                    fprintf(pMarker,
                            "[AdiPal_SpiTraceWrite] nId=%u resolved_dll_path=\"%s\" "
                            "target_log_path=\"%s\" fopen_result=%s\n",
                            nId, szDllPath, szLogPath,
                            (pFile[nId] != NULL) ? "OK" : "FAILED");
                    fclose(pMarker);
                }
            }
        }

        if(pFile[nId] != NULL)
        {
            fprintf(pFile[nId], "# ADI PC OspBridge raw SPI trace\n");
            fprintf(pFile[nId], "# Path: network_macphy -> adi_pal_spiSend -> FT4222_SPIMaster_SingleReadWrite\n");
            fprintf(pFile[nId], "# Log file location: %s (follows this DLL's own folder)\n\n", szLogPath);
            fflush(pFile[nId]);
        }

        nSeq[nId] = 0u;
        bInit[nId] = true;
    }

    if(pFile[nId] == NULL)
    {
        return;
    }

    nSeq[nId]++;

    fprintf(pFile[nId], "#%u dev=%u len=%u transferred=%u ftStatus=%d\n",
            nSeq[nId], nId, nSize, (unsigned)nTransferred, (int)ftStatus);

    fprintf(pFile[nId], "TX:");
    for(uint32_t i = 0u; i < nSize; i++)
    {
        fprintf(pFile[nId], " %02X", pWriteData[i]);
    }
    fprintf(pFile[nId], "\n");

    fprintf(pFile[nId], "RX:");
    for(uint32_t i = 0u; i < nSize; i++)
    {
        fprintf(pFile[nId], " %02X", pReadData[i]);
    }
    fprintf(pFile[nId], "\n\n");

    fflush(pFile[nId]);
}
#endif /* PAL_ENABLE_SPI_FILE_TRACE */

static DWORD WINAPI RunFunction(LPVOID lpParam)
{
    uint32_t nId = (uint32_t)(uintptr_t)lpParam;

    while(gbRunThread[nId])
    {
        if(goPalData.bIsFtdiLoaded)
        {
            BOOL value;

            if(goFT4222Lib.gpioRead(gahGpioFtdi[nId], (GPIO_Port)GPIO_PORT3, &value) == FT4222_OK)
            {
                if(value == 1)
                {
                    gbIrqSet[nId] = true;

                    if(gbSignalAsyncOperation == true)
                    {
                        adi_pal_networkCbk(nId, ADI_PAL_EVT_ASYNC_CBK, NULL);
                    }
                }
            }
        }


        if(gbTodoSpiSend[nId])
        {
            gbTodoSpiSend[nId] = false;
            uint16_t nBytesTransferred = 0U;
            FT_STATUS ftStatus;

            /* If FTDI dlls are not loaded, return */
            if(goPalData.bIsFtdiLoaded == false)
            {
                gbSpiErr[nId] = true;

                if(gbSignalAsyncOperation == true)
                {
                    adi_pal_networkCbk(nId, ADI_PAL_EVT_ASYNC_CBK, NULL);
                }

                continue;
            }

            ftStatus = goFT4222Lib.spiReadWrite(gahSpiFtdi[nId], gpReadData[nId], gpWriteData[nId], gnSize[nId], &nBytesTransferred,
                                                true);

            if(ftStatus != FT_OK)
            {
                // return ADI_PAL_ERR_DEV_WRITE_FAILED;
                // printf("Failed!\r\n");

                // while(1);
                gbSpiErr[nId] = true;

                if(gbSignalAsyncOperation == true)
                {
                    adi_pal_networkCbk(nId, ADI_PAL_EVT_ASYNC_CBK, NULL);
                }

                continue;
            }

#if PAL_ENABLE_SPI_FILE_TRACE == true
            AdiPal_SpiTraceWrite(nId, gpWriteData[nId], gpReadData[nId], gnSize[nId], nBytesTransferred, ftStatus);
#endif

#if PAL_ENABLE_SPI_LOGS == true
            printf("\nTX Data: ");

            for(uint32_t i = 0u; i < gnSize[nId]; i++)
            {
                printf("%02x ", gpWriteData[nId][i]);
            }

            printf("\nRX Data: ");

            for(uint32_t i = 0u; i < gnSize[nId]; i++)
            {
                printf("%02x ", gpReadData[nId][i]);
            }

            printf("\n");
#endif
            gbTodoSpiCallback[nId] = true;

            if(gbSignalAsyncOperation == true)
            {
                adi_pal_networkCbk(nId, ADI_PAL_EVT_ASYNC_CBK, NULL);
            }
        }
    }

    return 0;
}

/*============================ ENABLE WARNING =============================*/
#if defined(_MSC_VER)
#pragma warning(default:4191)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
/** @} */
/*
 * EOF: www.analog.com
*/
