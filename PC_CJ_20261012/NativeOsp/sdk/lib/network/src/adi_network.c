/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_network.c
 * \brief: Implementation of the Network layer component
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup network
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "network_internal.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/*! \cond PRIVATE */
/*================================ PROTOTYPES ===============================*/
#ifndef ADI_SERDES_MODE
static void transmitFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg);
#else
static ADI_NETWORK_ERR serdesTransmitFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg, uint8_t *pBuffer);
#endif
static bool areTxBuffersEmpty(uint32_t nDevNum);
static void resetNetworkData(void);
#ifndef ADI_SERDES_MODE
static bool IsQueueFull(uint16_t nWrIdx, uint32_t nDevNum);
#ifdef ADI_NETWORK_PRIO_Q_EN
static bool IsHpQueueFull(uint16_t nWrIdx, uint32_t nDevNum);
static bool IsHpRoute(uint32_t nInstNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg);
static uint16_t GetLpQueueSize(uint32_t nInstNum);
#endif
#endif
static void IncrementQIdx(volatile uint16_t *pnIdx, uint16_t nQSize);
static void fillVlanTags(uint8_t *pBuf, uint32_t *panVlanTags, uint8_t nNumCfgs, uint16_t *pnSize);
static ADI_NETWORK_ERR ValidateDeviceOpen(uint32_t nDevNum);
/*=================================== DATA ==================================*/
/*! Internal network structure */
ADI_MEM_DATA_CRIT_CACHE
ADI_NETWORK_DATA goNwData;

/* In SERDES mode, ethernet TX buffers are provided and managed by application layer */
#ifndef ADI_SERDES_MODE
/*! Buffer to store TX ethernet frames */
ADI_MEM_DATA_CRIT_CACHE
uint8_t ganEthBuff[MAX_MACPHY_INST][NETWORK_DATA_Q_SIZE][ADI_PAL_MAX_ETH_FRM];

#ifdef ADI_NETWORK_PRIO_Q_EN
/*! Buffer to store HP TX ethernet frames */
ADI_MEM_DATA_CRIT_CACHE
uint8_t ganEthBuffHp[MAX_MACPHY_INST][NETWORK_DATA_HP_Q_SIZE][ADI_PAL_MAX_ETH_FRM];
#endif

/*! Table to indicate if TX buffer for particular index is occupied or free */
ADI_MEM_DATA_CRIT_CACHE
volatile bool gabTransmitIdxTaken[NETWORK_DATA_Q_SIZE][ADI_NETWORK_MAX_DEVS];

#ifdef ADI_NETWORK_PRIO_Q_EN
/*! Table to indicate if HP TX buffer for particular index is occupied or free */
ADI_MEM_DATA_CRIT_CACHE
volatile bool gabTransmitIdxTakenHp[NETWORK_DATA_HP_Q_SIZE][ADI_NETWORK_MAX_DEVS];
#endif
#endif

/*! Buffer to store RX ethernet frames */
ADI_MEM_DATA_CRIT_CACHE
uint8_t ganEthRxBuff[MAX_MACPHY_INST][NETWORK_RX_Q_SIZE][ADI_PAL_MAX_ETH_FRM];

/*! Buffer for network_getTxBuffer and network_transmit mutex */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganNwMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];

/*! network_getTxBuffer and network_transmit mutex configuration */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_MUTEX_CFG goNwMutexCfg;

/*! Handle for the network_getTxBuffer and network_transmit mutex */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_MUTEX_HANDLE ghNwMutex;

/*! Buffer for network_run mutex */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganNwRunMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];

/*! network_run mutex configuration */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_MUTEX_CFG goNwRunMutexCfg;

/*! Handle for the network_run mutex */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_MUTEX_HANDLE ghNwRunMutex;

/*! Buffer for Transmit Index mutex */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganNwTxIdxMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];

/*! Transmit Index mutex configuration */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_MUTEX_CFG goNwTxIdxMutexCfg;

/* Transmit Index mutex handle */
ADI_MEM_DATA_CRIT_CACHE
PRJ_OSAL_MUTEX_HANDLE ghNwTxIdxMutex;

/*! Handle to the semaphore that has to be signalled on async event */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_SEMAPHORE_HANDLE ghAsyncSemaphore;


/*! \endcond */
/*=================================== CODE ==================================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Base APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Initializes the network layer
 */
ADI_NETWORK_CODE
void adi_network_init(void)
{
    /* Reset network data structure */
    resetNetworkData();

#ifdef MACPHY_EN
    /* Initialize MAC-PHY structures */
    network_macphy_Init();
#endif /* MACPHY_EN */

#ifdef ETH_EN
    /* Initialize eth structures */
    /* TODO: Uncomment below when it is used! */
    /* network_eth_Init(); */
#endif  /* ETH_EN */

    for(uint8_t i = 0; i < MAX_MACPHY_INST; i++)
    {
        goNwData.anEthWrIdx[i] = 0U;
#ifdef ADI_NETWORK_PRIO_Q_EN
        goNwData.anEthWrIdxHp[i] = 0U;
#endif
        goNwData.nRxWrIdx[i] = 0U;
        goNwData.nRxRdIdx[i] = 0U;
        goNwData.nCtrlWrIdx = 0U;
        goNwData.nCtrlRdIdx = 0U;
#ifdef ADI_SERDES_MODE
        goNwData.anDataDescWrIdx[i] = 0U;
        goNwData.anDataDescRdIdx[i] = 0U;
#endif
    }

#ifndef ADI_SERDES_MODE
    (void)ADI_MEMSET((void *)gabTransmitIdxTaken, 0, sizeof(gabTransmitIdxTaken));
#ifdef ADI_NETWORK_PRIO_Q_EN
    (void)ADI_MEMSET((void *)gabTransmitIdxTakenHp, 0, sizeof(gabTransmitIdxTakenHp));
#endif
#endif
    goNwMutexCfg.pMem = &ganNwMutexBuffer[0U];
    goNwMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
    (void)prj_osal_createMutex(&goNwMutexCfg, &ghNwMutex);

    goNwRunMutexCfg.pMem = &ganNwRunMutexBuffer[0U];
    goNwRunMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
    (void)prj_osal_createMutex(&goNwRunMutexCfg, &ghNwRunMutex);

    goNwTxIdxMutexCfg.pMem = &ganNwTxIdxMutexBuffer[0U];
    goNwTxIdxMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
    (void)prj_osal_createMutex(&goNwTxIdxMutexCfg, &ghNwTxIdxMutex);
}

/**
 * @brief       Terminates the network layer
 */
ADI_NETWORK_CODE
void adi_network_terminate(void)
{
    /* Mutex cleanup */
    (void)prj_osal_destroyMutex(ghNwMutex);
    (void)prj_osal_destroyMutex(ghNwRunMutex);
    (void)prj_osal_destroyMutex(ghNwTxIdxMutex);

    /* Reset network data structure */
    resetNetworkData();

#ifdef MACPHY_EN
    /* Terminate MAC-PHY structures */
    network_macphy_Terminate();
#endif /* MACPHY_EN */

#ifdef ETH_EN
    /* Terminate eth structures */
    /* TODO: Uncomment below when it is used! */
    /* network_eth_Terminate(); */
#endif  /* ETH_EN */
}

/**
 * @brief       Register a callback function
 *
 * @param [in]  pfCbk           Pointer to the callback function
 */
ADI_NETWORK_CODE
void adi_network_registerCallback(ADI_NETWORK_CBK pfCbk)
{
    /* Assign the pointer with the callback function passed */
    goNwData.pfCbk = pfCbk;
}

/**
 * @brief       Gets all the devices in the network
 *
 * @param [out] pnNumDevices    Network layer returns the number of host devices connected to the SW controller
 * @param [out] paoDevInfo      Network layer returns an array with the info of the devices
 */
ADI_NETWORK_CODE
void adi_network_getDevices(uint32_t **pnNumDevices, ADI_NETWORK_DEVINFO **paoDevInfo)
{
    goNwData.nNumDevices = 0U;

#ifdef MACPHY_EN
    /* Temporary variables to store the device names and descriptions */
    ADI_PAL_SPIDEVINFO aoSpiDevs[ADI_PAL_MAX_NUM_SPI_DEV];

    /* Get the name and description of all the SPI devices */
    adi_pal_getSpiDevs(&goNwData.nSpiDevs, &aoSpiDevs[0U], ADI_PAL_MAX_NUM_SPI_DEV);

    /* Set the name, description and ID of SPI devices to be returned */
    for(uint32_t i = 0U; i < goNwData.nSpiDevs; i++)
    {
        ADI_NETWORK_DEVINFO *poDevInfo = &goNwData.aoDevInfo[goNwData.nNumDevices];
        poDevInfo->nId = i;
        (void) ADI_STRNCPY(&poDevInfo->sName[0U], &aoSpiDevs[i].sDescription[0U], ADI_PAL_DEV_NAME_MAX_SIZE);
        poDevInfo->sName[ADI_PAL_DEV_NAME_MAX_SIZE - 1U] = '\0';
        (void) ADI_STRNCPY(&poDevInfo->sDesc[0U], &aoSpiDevs[i].sDescription[0U], ADI_PAL_DEV_NAME_MAX_SIZE);
        poDevInfo->sDesc[ADI_PAL_DEV_NAME_MAX_SIZE - 1U] = '\0';
        poDevInfo->eNwType = ADI_NETWORK_TYPE_MACPHY;
        poDevInfo->bWkslpHistoryCheckEn = false;
        goNwData.nNumDevices++;
    }

#endif /* MACPHY_EN */

#ifdef ETH_EN
    /* Temporary variables to store the device names and descriptions */
    ADI_PAL_ETHADAPTERSINFO aoEthDevs[ADI_PAL_MAX_NUM_ETH_DEV];

    /* Get the number, name and description of the npcap adapters */
    adi_pal_getEthAdapters(&goNwData.nEthDevs, &aoEthDevs[0U], ADI_PAL_MAX_NUM_ETH_DEV);

    /* Set the name, description and ID of npcap devices to be returned */
    for(uint32_t i = 0U; i < goNwData.nEthDevs; i++)
    {
        ADI_NETWORK_DEVINFO *poDevInfo = &goNwData.aoDevInfo[goNwData.nNumDevices];
        poDevInfo->nId = i;

        if(aoEthDevs[i].sName[0u] != '\0')
        {
            (void) ADI_STRNCPY(&poDevInfo->sName[0U], &aoEthDevs[i].sName[0U], ADI_NETWORK_DEV_NAME_MAX_SIZE);
            poDevInfo->sName[ADI_NETWORK_DEV_NAME_MAX_SIZE - 1U] = '\0';
        }
        else
        {
            char sName[ADI_NETWORK_DEV_NAME_MAX_SIZE];
            (void) ADI_SNPRINTF(sName, ADI_NETWORK_DEV_NAME_MAX_SIZE, "Ethernet Adapter "UINT32_FORMATTER"", i);
            (void) ADI_STRNCPY(&poDevInfo->sName[0U], sName, ADI_NETWORK_DEV_NAME_MAX_SIZE);
            poDevInfo->sName[ADI_NETWORK_DEV_NAME_MAX_SIZE - 1U] = '\0';
        }

        if(aoEthDevs[i].sDesc[0u] != '\0')
        {
            (void) ADI_STRNCPY(&poDevInfo->sDesc[0U], &aoEthDevs[i].sDesc[0U], ADI_NETWORK_DEV_NAME_MAX_SIZE);
            poDevInfo->sDesc[ADI_NETWORK_DEV_NAME_MAX_SIZE - 1U] = '\0';
        }
        else
        {
            char sDesc[ADI_NETWORK_DEV_NAME_MAX_SIZE];
            (void) ADI_SNPRINTF(sDesc, ADI_NETWORK_DEV_NAME_MAX_SIZE, "Ethernet Adapter "UINT32_FORMATTER"", i);
            (void) ADI_STRNCPY(&poDevInfo->sDesc[0U], sDesc, ADI_NETWORK_DEV_NAME_MAX_SIZE);
            poDevInfo->sDesc[ADI_NETWORK_DEV_NAME_MAX_SIZE - 1U] = '\0';
        }

        poDevInfo->eNwType = ADI_NETWORK_TYPE_ETHERNET;
        goNwData.nNumDevices++;
    }

#endif /* ETH_EN */

    /* Return the number of devices and their info */
    *pnNumDevices = &goNwData.nNumDevices;
    *paoDevInfo   = &goNwData.aoDevInfo[0U];
}

/**
 * @brief       Opens a device
 *
 * @param [in]  nDevNum         The number of the device to be opened. Use the
 *                              array index from the list obtained in adi_network_getDevices
 * @param [in]  poNwMacPhyCfg   Pointer to the MACPHY configuration structure
 * @note poNwMacPhyCfg Should be NULL incase the device type is ADI_NETWORK_TYPE_ETHERNET
 */
ADI_NETWORK_CODE
void adi_network_openDevice(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg)
{
    /* Assert that the device is not opened and that the index is within the list of devices */
    ADI_DBG_REQUIRE(nDevNum < goNwData.nNumDevices, "Device number exceeds total number of devices available");
    ADI_DBG_REQUIRE(goNwData.aoDevInfo[nDevNum].bIsOpened == false, "Device is already open");

    /* Initialize macphy layer */
    if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
    {
#ifdef MACPHY_EN
        goNwData.aoDevInfo[nDevNum].nCtrlIdx = goNwData.nSpiInst;
        goNwData.aoDevInfo[nDevNum].bIsOpened = true;
        network_macphy_Open(nDevNum, poNwMacPhyCfg);
        goNwData.nSpiInst++;
        goNwData.nNumDevOpened++;
#else
        ADI_DBG_ERROR();
#endif /* MACPHY_EN */
    }
    /* Open the network adapter */
    else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
    {
#ifdef ETH_EN
        goNwData.aoDevInfo[nDevNum].nCtrlIdx = goNwData.nEthInst;
        network_eth_Open(nDevNum);
        goNwData.nEthInst++;
        goNwData.aoDevInfo[nDevNum].bIsOpened = true;
        goNwData.nNumDevOpened++;
#else
        ADI_DBG_ERROR();
#endif /* ETH_EN */
    }
    else
    {
        /* Invalid device! */
        ADI_DBG_ERROR();
    }
}

/**
 * @brief       Closes a device
 *
 * @param [in]  nDevNum         The number of the device to be closed. Use the
 *                              array index from the list obtained in adi_network_getDevices
 */
ADI_NETWORK_CODE
void adi_network_closeDevice(uint32_t nDevNum)
{
    /* Assert that the device index is within the list of devices */
    ADI_DBG_REQUIRE(nDevNum < goNwData.nNumDevices, "Device number exceeds total number of devices available");

    /* Close the device only if it is open*/
    if(goNwData.aoDevInfo[nDevNum].bIsOpened == true)
    {
        /* Clear the flag */
        goNwData.aoDevInfo[nDevNum].bIsOpened = false;

        /* Decrement number of opened device */
        goNwData.nNumDevOpened--;

        /* Close the SPI device */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            network_macphy_Close(nDevNum);
            goNwData.nSpiInst--;
            /* Clear the mapping */
            goNwData.aoDevInfo[nDevNum].nCtrlIdx = 0xFFFFFFFFU;
#else
            ADI_DBG_ERROR();
#endif /* MACPHY_EN */
        }
        /* Close the network addapter */
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
#ifdef ETH_EN
            network_eth_Close(nDevNum);
            goNwData.nEthInst--;
            /* Clear the mapping */
            goNwData.aoDevInfo[nDevNum].nCtrlIdx = 0xFFFFFFFFU;
#else
            ADI_DBG_ERROR();
#endif /* ETH_EN */
        }
        else
        {
            /* Nothing to do */
            ADI_DBG_ERROR();
        }
    }
    else
    {
        ADI_DBG_ERROR();
    }
}

/**
 * @brief       Stores the MAC address of the controller node
 * @param [in]  panCtrlMacAddr  Pointer to the controller MAC address
 */
ADI_NETWORK_CODE
void adi_network_setControllerMacAddr(uint8_t *panCtrlMacAddr)
{
    /* Store the controller's MAC addr */
    (void) ADI_MEMCPY(goNwData.anControllerMacAddr, panCtrlMacAddr, 6u);
}

/**
 * @brief       Reconfigure the node
 *
 * @param [in]  nDevNum         The number of the device to reconfigure.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  poNwMacPhyCfg   Pointer to the MACPHY configuration structure
 * @return      Status
 *              - #ADI_NETWORK_OK           Successfully added the reconfigure request to queue
 *              - #ADI_NETWORK_NO_SPACE     No space available in queue to add the reconfigure request
 *              - #ADI_NETWORK_DEVICE_CLOSED Device is not opened
 *              - #ADI_NETWORK_INVALID_PARAM Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_reconfigure(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_Reconfigure(nDevNum, poNwMacPhyCfg);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* Nothing to do */
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Writes to the MAC-PHY (controller node) registers
 *
 * @param [in]  nDevNum         The number of the device to write register. Use the
 *                              array index from the list obtained in adi_network_getDevices
 * @param [in]  nStartAddr      Address to start writing from
 * @param [in]  nLength         Number of registers to write
 * @param [in]  nMMS            MMS of the registers
 * @param [in]  panRegData      Array of register data
 * @param [in]  pfCbk           Pointer to the callback function
 * @return      Status
 *              - #ADI_NETWORK_OK        Successfully added the register write request to queue
 *              - #ADI_NETWORK_NO_SPACE  No space available in queue
 *              - #ADI_NETWORK_DEVICE_CLOSED Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_writeControllerRegister(
    uint32_t nDevNum,
    uint16_t nStartAddr,
    uint8_t nLength,
    uint8_t nMMS,
    uint32_t *panRegData,
    ADI_NETWORK_CBK pfCbk
)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Controller register write is valid only for MAC-PHY devices */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_WriteRegister(
                         nDevNum,
                         nStartAddr,
                         nLength,
                         nMMS,
                         panRegData,
                         pfCbk
                     );
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* Invalid device type */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Reads from the MAC-PHY (controller node) registers
 *
 * @param [in]  nDevNum         The number of the device to read register. Use the
 *                              array index from the list obtained in adi_network_getDevices
 * @param [in]  nStartAddr      Address to start reading from
 * @param [in]  nLength         Number of registers to read
 * @param [in]  nMMS            MMS of registers
 * @param [in]  pfCbk           Pointer to the callback function
 * @return      Status
 *              - #ADI_NETWORK_OK        Successfully added the register read request to queue
 *              - #ADI_NETWORK_NO_SPACE  No space available in queue
 *              - #ADI_NETWORK_DEVICE_CLOSED Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_readControllerRegister(
    uint32_t nDevNum,
    uint16_t nStartAddr,
    uint8_t nLength,
    uint8_t nMMS,
    ADI_NETWORK_CBK pfCbk
)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Controller register read is valid only for MAC-PHY devices */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_ReadRegister(nDevNum, nStartAddr, nLength, nMMS, pfCbk);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* Invalid device type */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Performs (controller node) MAC-PHY register read and masked write at given bit postions
 *
 * @param [in]  nDevNum         The number of the device to write register. Use the
 *                              array index from the list obtained in adi_network_getDevices
 * @param [in]  nStartAddr      Address to start writing from
 * @param [in]  nLength         Number of registers to write
 * @param [in]  nMMS            MMS of the registers
 * @param [in]  panRegData      Pointer to array of register data
 * @param [in]  panRegMask      Pointer to array of register mask
 * @return      Status
 *              - #ADI_NETWORK_OK        Successfully added the register write request to queue
 *              - #ADI_NETWORK_NO_SPACE  No space available in queue
 *              - #ADI_NETWORK_DEVICE_CLOSED Device is not opened
 * @note  The possible errors to callback function is as follows:
 *                - #ADI_NETWORK_NO_SPACE               No space available in queue
 *                - #ADI_NETWORK_MACPHY_REG_READ_FAILED Register read failed
 *                - #ADI_NETWORK_MACPHY_REG_WRITE_FAILED Register write failed
 *                - #ADI_NETWORK_DEVICE_CLOSED Device is not opened
 *                - #ADI_NETWORK_INVALID_PARAM Invalid parameter
 *                - #ADI_NETWORK_MUTEX_FAILURE Mutex lock or unlock failed
 */
ADI_NETWORK_CODE
extern ADI_NETWORK_ERR adi_network_maskedWriteControllerRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    uint32_t  *panRegData,
    uint32_t  *panRegMask
)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        /* Validate pointer parameters */
        if((panRegData == NULL) || (panRegMask == NULL))
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* check for mutex lock success */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Controller register write is valid only for MAC-PHY devices */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_MaskedWriteRegister(nDevNum, nStartAddr, nLength, nMMS, panRegData, panRegMask);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* Invalid device type */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /*check for mutex unlock success*/
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Gets a buffer from a device to fill an ethernet frame
 *
 * @param [in]  nDevNum         The number of the device from which to get buffer.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [out] pBuffer         The location of the buffer from where the application can
 *                              fill the data is returned here
 * @param [in]  poNwFrameCfg    The pointer to the details of the frame, such as
 *                              ethertype, length, etc.
 * @return      Status
 *              -#ADI_NETWORK_OK        Successfully added the frame to queue
 *              -#ADI_NETWORK_NO_SPACE  No space available in queue
 *              -#ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 * @note   In SerDes mode, the request to get a new buffer is re-directed to the above layer
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getTxBuffer(
    uint32_t    nDevNum,
    uint8_t     **pBuffer,
    ADI_NETWORK_FRAME_CFG *poNwFrameCfg
)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

#ifndef ADI_SERDES_MODE
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    bool bMutexLocked = false;
    *pBuffer = NULL;

    do
    {
        if(prj_osal_lockMutex(ghNwMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        bMutexLocked = true;

#ifdef ADI_NETWORK_PRIO_Q_EN
        /* Determine if this is an HP route */
        bool bIsHp = IsHpRoute(nInstNum, poNwFrameCfg);

        if(bIsHp)
        {
            /* Check if space is available in HP queue */
            if(IsHpQueueFull(goNwData.anEthWrIdxHp[nInstNum], nDevNum))
            {
                eNwErr = ADI_NETWORK_NO_SPACE;
                break;
            }
        }
        else
        {
            /* Check if space is available in LP queue */
            if(IsQueueFull(goNwData.anEthWrIdx[nInstNum], nDevNum))
            {
                eNwErr = ADI_NETWORK_NO_SPACE;
                break;
            }
        }
#else
        /* Single queue mode: check if space is available */
        if(IsQueueFull(goNwData.anEthWrIdx[nInstNum], nDevNum))
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }
#endif

#ifdef MACPHY_EN

        /* Check if space is available in OA-SPI layer if device is MAC-PHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef ADI_NETWORK_PRIO_Q_EN
            const ADI_NETWORK_MACPHY_CFG *poMacPhyCfg = goNwData.paoNwMacPhyCfg[nInstNum];

            /* If priority mode is enabled, check space based on MAC frame routing */
            if(poMacPhyCfg->bEnablePrioFifos)
            {
                bool bTargetQFull;

                if(poNwFrameCfg->eMacRoutingInfo == ADI_MACPHY_ROUTE_PORT1_HP)
                {
                    bTargetQFull = adi_oaspi_IsTxDescQFull(nInstNum, true);
                }
                else if(poNwFrameCfg->eMacRoutingInfo == ADI_MACPHY_ROUTE_PORT0_LP)
                {
                    bTargetQFull = adi_oaspi_IsTxDescQFull(nInstNum, false);
                }
                else
                {
                    /* Invalid routing info, return error */
                    eNwErr = ADI_NETWORK_INVALID_PARAM;
                    break;
                }

                if(bTargetQFull)
                {
                    /* If target queue is full, return no space error */
                    eNwErr = ADI_NETWORK_NO_SPACE;
                    break;
                }
            }
            else
#endif /* ADI_NETWORK_PRIO_Q_EN */
            {
                /* In non-priority mode, check space based on default queue */
                if(adi_oaspi_IsTxDescQFull(nInstNum, false))
                {
                    eNwErr = ADI_NETWORK_NO_SPACE;
                    break;
                }
            }
        }

#endif /* MACPHY_EN */

#ifdef ADI_NETWORK_PRIO_Q_EN
        /* Return the pointer from the correct pool */
        if(bIsHp)
        {
            uint16_t nWrIdx = goNwData.anEthWrIdxHp[nInstNum];

            if(poNwFrameCfg->bLenInclMacHdr)
            {
                *pBuffer = &ganEthBuffHp[nInstNum][nWrIdx][0U];
            }
            else
            {
                *pBuffer = &ganEthBuffHp[nInstNum][nWrIdx][14U + 4U * poNwFrameCfg->nNumVlanTags];
            }
        }
        else
#endif /* ADI_NETWORK_PRIO_Q_EN */
        {
            uint16_t nWrIdx = goNwData.anEthWrIdx[nInstNum];

            if(poNwFrameCfg->bLenInclMacHdr)
            {
                *pBuffer = &ganEthBuff[nInstNum][nWrIdx][0U];
            }
            else
            {
                *pBuffer = &ganEthBuff[nInstNum][nWrIdx][14U + 4U * poNwFrameCfg->nNumVlanTags];
            }
        }
    } while(false);

    /* Cleanup: unlock mutex if it was successfully locked */
    if(bMutexLocked)
    {
        if(((eNwErr == ADI_NETWORK_NO_SPACE) || (eNwErr == ADI_NETWORK_INVALID_PARAM)) && (prj_osal_unlockMutex(ghNwMutex) != PRJ_OSAL_STATUS_SUCCESS))
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
        }
    }

#else

    ADI_NETWORK_EVT_APP_REQUEST_BUFFER_DATA oGetBufferData;
    oGetBufferData.pBuffer = NULL;
    oGetBufferData.nLength = poNwFrameCfg->nLenByte;
    oGetBufferData.panMacAddr = poNwFrameCfg->panPhysAddr;

    if(poNwFrameCfg->nNumVlanTags > 0U)
    {
        oGetBufferData.nVlanTag = poNwFrameCfg->panVlanTagArr[0U];
        oGetBufferData.bisVlanTagPresent =  true;
    }
    else
    {
        oGetBufferData.bisVlanTagPresent = false;
    }

    ADI_DBG_REQUIRE(goNwData.pfCbk != NULL, "Pointer to callback function is NULL");
    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_APP_REQUEST_BUFFER, &oGetBufferData);

    /* Check if we have got the buffer from above layer */
    if(oGetBufferData.pBuffer != NULL)
    {
        *pBuffer = oGetBufferData.pBuffer;
        poNwFrameCfg->nLenByte = oGetBufferData.nLength;
    }
    else
    {
        eNwErr = ADI_NETWORK_NO_SPACE;
    }

#endif
    return eNwErr;
}

/**
 * @brief       Transmits the filled buffer
 *
 * @param [in]  nDevNum         The number of the device from which to send the ethernet frame.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  poNwFrameCfg    The pointer to the details of the frame, such as
 *                              ethertype, length, etc.
 * @note   In SerDes mode, the request to transmit is re-directed to the above layer
 */
ADI_NETWORK_CODE
void adi_network_transmit(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg)
{
#ifndef ADI_SERDES_MODE
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
#ifdef ADI_NETWORK_PRIO_Q_EN
    bool bIsHp = IsHpRoute(nInstNum, poNwFrameCfg);
#endif
    uint16_t nEthWrIdx;

#ifdef ADI_NETWORK_PRIO_Q_EN
    if(bIsHp)
    {
        nEthWrIdx = goNwData.anEthWrIdxHp[nInstNum];
    }
    else
#endif
    {
        nEthWrIdx = goNwData.anEthWrIdx[nInstNum];
    }

    /* Construct the frame fully */
    if(!poNwFrameCfg->bLenInclMacHdr)
    {
        uint8_t *pFrameBuf;

#ifdef ADI_NETWORK_PRIO_Q_EN
        if(bIsHp)
        {
            pFrameBuf = &ganEthBuffHp[nInstNum][nEthWrIdx][0U];
        }
        else
#endif
        {
            pFrameBuf = &ganEthBuff[nInstNum][nEthWrIdx][0U];
        }

        /* Fill the destination address */
        (void) ADI_MEMCPY(&pFrameBuf[0U], poNwFrameCfg->panPhysAddr, 6U);

        /* Fill the source address */
        (void) ADI_MEMCPY(&pFrameBuf[6U], &goNwData.anControllerMacAddr[0U], 6U);

        /* Insert VLANs */
        uint16_t nVlanSize = 0U;
        fillVlanTags(&pFrameBuf[12U], poNwFrameCfg->panVlanTagArr, poNwFrameCfg->nNumVlanTags,
                     &nVlanSize);

        /* Fill the ethertype */
        pFrameBuf[12U + nVlanSize] = (uint8_t)((poNwFrameCfg->nFrameType & 0xFF00U) >> 8U);
        pFrameBuf[13U + nVlanSize] = (uint8_t)(poNwFrameCfg->nFrameType & 0x00FFU);

        /* Update the frame length to include SA, DA, Ethertype and VLAN tags */
        poNwFrameCfg->nLenByte += (14U + nVlanSize);
    }

    ADI_DBG_REQUIRE(prj_osal_lockMutex(ghNwTxIdxMutex) == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");

    /* If duplication is enabled, */
    /* loop through all the devices and transmit the frame out */
    if(poNwFrameCfg->bDuplicate)
    {
        for(uint32_t i = 0U; i < goNwData.nNumDevices; i++)
        {
            if(goNwData.aoDevInfo[i].bIsOpened)
            {
#ifdef ADI_NETWORK_PRIO_Q_EN
                if(bIsHp)
                {
                    gabTransmitIdxTakenHp[nEthWrIdx][i] = true;
                }
                else
#endif
                {
                    gabTransmitIdxTaken[nEthWrIdx][i] = true;
                }
            }
        }

        for(uint32_t i = 0U; i < goNwData.nNumDevices; i++)
        {
            if(goNwData.aoDevInfo[i].bIsOpened)
            {
                transmitFrame(i, poNwFrameCfg);
            }
        }
    }
    else
    {
#ifdef ADI_NETWORK_PRIO_Q_EN
        if(bIsHp)
        {
            gabTransmitIdxTakenHp[nEthWrIdx][nDevNum] = true;
        }
        else
#endif
        {
            gabTransmitIdxTaken[nEthWrIdx][nDevNum] = true;
        }

        transmitFrame(nDevNum, poNwFrameCfg);
    }

#ifdef ADI_NETWORK_PRIO_Q_EN
    if(bIsHp)
    {
        IncrementQIdx(&goNwData.anEthWrIdxHp[nInstNum], NETWORK_DATA_HP_Q_SIZE);
    }
    else
    {
        IncrementQIdx(&goNwData.anEthWrIdx[nInstNum], GetLpQueueSize(nInstNum));
    }
#else
    IncrementQIdx(&goNwData.anEthWrIdx[nInstNum], NETWORK_DATA_Q_SIZE);
#endif

    ADI_DBG_REQUIRE(prj_osal_unlockMutex(ghNwTxIdxMutex) == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    ADI_DBG_REQUIRE(prj_osal_unlockMutex(ghNwMutex) == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
#else

    ADI_NETWORK_EVT_APP_SERIALIZED_BUFFER_DATA oTransmitData;
    oTransmitData.panMacAddr = poNwFrameCfg->panPhysAddr;
    oTransmitData.nLength = poNwFrameCfg->nLenByte;

    ADI_DBG_REQUIRE(goNwData.pfCbk != NULL, "Pointer to callback function is NULL");
    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_APP_FINISHED_SERIALIZATION, &oTransmitData);

#endif
}

#ifdef ADI_SERDES_MODE
/**
 * @brief       Transmits the application buffers to either MACPHY port / Std Ethernet port
 *
 * @param [in]  nDevNum                 The number of the device from which to send the ethernet frame.
 *                                      Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  poNwTransmitFrameCfg    The pointer to the details of the frame that required by
 *                                      the lower layers to transmit the frame
 * @return      Status
 *              -#ADI_NETWORK_OK        Successfully added the frame to queue
 *              -#ADI_NETWORK_INVALID_PARAM Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_serdesTransmit(
    uint32_t  nDevNum,
    ADI_NETWORK_SERDES_TRANSMIT_CFG *poNwTransmitFrameCfg
)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(poNwTransmitFrameCfg == NULL)
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        uint8_t *pBuffer = poNwTransmitFrameCfg->pBuffer;

        /* The frame is assumed to be constructed fully by the above layer */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        oNwFrameCfg.nLenByte = poNwTransmitFrameCfg->nLenByte;
        oNwFrameCfg.eMacRoutingInfo = poNwTransmitFrameCfg->eMacRoutingInfo;
        oNwFrameCfg.eTsReg = poNwTransmitFrameCfg->eTsReg;

        eNwErr = serdesTransmitFrame(nDevNum, &oNwFrameCfg, pBuffer);

    } while(false);

    return eNwErr;

}

/**
 * @brief  Resets the OASPI and network macphy TX descriptors queue
 *
 * @param [in]  nDevNum                 The number of the device from which to send the ethernet frame.
 *                                      Use the array index from the list obtained in adi_network_getDevices
 */
ADI_NETWORK_CODE
void adi_network_serdesResetOaspiQueue(uint32_t nDevNum)
{
    ADI_DBG_REQUIRE(nDevNum < goNwData.nNumDevices, "Device number exceeds total number of devices available");
    ADI_DBG_REQUIRE(goNwData.aoDevInfo[nDevNum].bIsOpened == true,
                    "Device was not opened");

    if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
    {
#ifdef MACPHY_EN
        network_macphy_ResetOaspiQueue(nDevNum);
#else
        ADI_DBG_ERROR();
#endif /* MACPHY_EN */
    }
}
#endif /* ADI_SERDES_MODE */

/**
 * @brief       Function that calls the run function for all the open devices
 */
ADI_NETWORK_CODE
void adi_network_run(void)
{
    ADI_DBG_REQUIRE(prj_osal_lockMutex(ghNwRunMutex) == PRJ_OSAL_STATUS_SUCCESS,
                    "Failed to lock mutex in network_run function");

    /* Loop through all the opened network devices and invoke their run function */
    for(uint32_t i = 0U; i < goNwData.nNumDevices; i++)
    {
        if(goNwData.aoDevInfo[i].bIsOpened == true)
        {
            /* If the device type is MAC-PHY, invoke MAC-PHY driver run */
            if(goNwData.aoDevInfo[i].eNwType == ADI_NETWORK_TYPE_MACPHY)
            {
#ifdef MACPHY_EN
                network_macphy_Run(i);
#else
                ADI_DBG_ERROR();
#endif /* MACPHY_EN */
            }
            /* If the device type is ethernet, invoke ethernet driver run */
            else if(goNwData.aoDevInfo[i].eNwType == ADI_NETWORK_TYPE_ETHERNET)
            {
#ifdef ETH_EN
                network_eth_Run(i);
#else
                ADI_DBG_ERROR();
#endif /* ETH_EN */
            }
            else
            {
                /* Nothing to do */
                ADI_DBG_ERROR();
            }
        }
    }

    ADI_DBG_REQUIRE(prj_osal_unlockMutex(ghNwRunMutex) == PRJ_OSAL_STATUS_SUCCESS,
                    "Failed to unlock mutex in network_run function");
}

/**
 * @brief       Registers a semaphore to be signalled on async events
 *
 * @param [in]  hSemaphore      Handle to the semaphore to be signalled
 */
void adi_network_registerAsyncSemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hSemaphore != NULL, "Semaphore handle is null");
    ghAsyncSemaphore = hSemaphore;
    gbSignalAsyncOperation = true;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Status APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Gets the MAC status of the network device
 *
 * @details     Retrieves MAC layer status information by dispatching to the
 *              appropriate device-specific handler. Thread-safe operation is
 *              ensured by mutex protection during device access.
 *
 * @param [in]  nDevNum         The number of the device for which MAC status is to be returned.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved MAC status
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getMacStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to prevent concurrent access to device resources */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific handler */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetMacStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
#ifdef ETH_EN
            network_eth_GetMacStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* ETH_EN */
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex to allow other threads to access device */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Gets the MAC-PHY node's PLCA status
 *
 * @details     PLCA (Physical Layer Collision Avoidance) status provides information
 *              about the deterministic channel access mechanism. Mutex protection ensures
 *              atomic read of multi-register status information.
 *
 * @param [in]  nDevNum         The number of the device for which PLCA status is to be returned.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved PLCA status
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getPlcaStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic multi-register status read */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific handler */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetPlcaStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
#ifdef ETH_EN
            network_eth_GetPhyStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* ETH_EN */
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex to allow other threads to access device */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Gets PHY status
 *
 * @details     Retrieves physical layer status including link state, speed, duplex mode.
 *              Primarily applicable to MAC-PHY devices. Thread safety is ensured through
 *              mutex protection to prevent race conditions during register access.
 *
 * @param [in]  nDevNum         The number of the device to get PHY status
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved PHY status
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getPhyStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to prevent concurrent register access */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific handler - PHY status mainly for MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetPhyStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Ethernet controller doesn't have separate PHY status */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex to allow other threads to access device */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Gets PCS status
 *
 * @details     Retrieves Physical Coding Sublayer status which handles encoding/decoding
 *              between MAC and PHY layers. Applicable to MAC-PHY devices. Mutex ensures
 *              consistent multi-register snapshot.
 *
 * @param [in]  nDevNum         The number of the device to get PCS status
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved PCS status
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getPcsStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic multi-register status snapshot */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific handler - PCS applicable to MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetPcsStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Ethernet controller doesn't expose PCS status separately */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex to allow other threads to access device */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Gets PTP status
 *
 * @details     Retrieves Precision Time Protocol status used for time synchronization
 *              in distributed systems. Includes clock synchronization state and timestamp
 *              information. Mutex protection ensures consistent read of timing registers.
 *
 * @param [in]  nDevNum         The number of the device to get PTP status
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved PTP status
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getPtpStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic read of timing registers */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific handler - PTP mainly for MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetPtpStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Ethernet controller doesn't expose PTP status in this interface */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex to allow other threads to access device */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Reset APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Resets the MAC-PHY device
 *
 * @details     Issues a software reset to the MAC controller to restore it to default state.
 *              All MAC configuration registers are reset, but PHY layer configuration is preserved.
 *              Mutex ensures no other operations interfere during the reset sequence.
 *
 * @param [in]  nDevNum         The number of the device to reset.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully reset MAC controller
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_controllerMacReset(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to prevent operations during reset sequence */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific reset handler */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr =  network_macphy_MacReset(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
#ifdef ETH_EN
            network_eth_MacReset(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* ETH_EN */
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after reset completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Resets HV Die
 *
 * @param [in] nDevNum                  The number of the device to reset HV Die.
 *                                      Use the array index from the list obtained in adi_network_getDevices
 *
 * @return Status
 *              - #ADI_NETWORK_OK        Successfully added the frame to queue
 *              - #ADI_NETWORK_NO_SPACE  No space available in queue
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_controllerHVDieReset(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then reset the HV die */
#ifdef MACPHY_EN
            eNwErr =  network_macphy_HVDieReset(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;

}

/**
 * @brief Resets LV Die
 *
 * @param [in] nDevNum                  The number of the device to reset LV Die.
 *                                      Use the array index from the list obtained in adi_network_getDevices
 *
 * @return Status
 *              - #ADI_NETWORK_OK        Successfully added the frame to queue
 *              - #ADI_NETWORK_NO_SPACE  No space available in queue
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_controllerLVDieReset(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then reset the LV die */
#ifdef MACPHY_EN
            eNwErr =  network_macphy_LVDieReset(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Buffer flush APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Flush transmit buffers (Blocks till MAC-PHY transmit FIFO is empty)
 *
 * @param [in]  nDevNum         The device for which to flush out the transmit buffers.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  nTimeout        Timeout (in nanoseconds). If set to 0, default timeout is set as 300MS
 *
 * @note   In SerDes mode, the request to flush the buffers is re-direct to the above layers as a callback
 */
ADI_NETWORK_CODE
void adi_network_flushTxBuffer(uint32_t nDevNum, uint64_t nTimeout)
{
#ifdef ADI_SERDES_MODE
    /* In SerDes mode, give a callback to the application code for flushing out buffers in the application */
    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_APP_REQUEST_FLUSH_BUFFERS, NULL);
#endif

    uint64_t nCurrTime, nStartTime;
    /* Get the current time */
    adi_pal_getCurrTime(&nCurrTime);
    nStartTime = nCurrTime;

    /* Do a continuous run till TX buffer is fully flushed out */
    while(!areTxBuffersEmpty(nDevNum) && !goNwData.aoDevInfo[nDevNum].bInErrState)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);

        /* If timed-out, break out of the loop */
        if(((nTimeout != 0ULL) && (nCurrTime - nStartTime > nTimeout)) || ((nTimeout == 0ULL)
                && (nCurrTime - nStartTime > (300U * MS_TO_NS))))
        {
            break;
        }
    }
}

/**
 * @brief       Flush transmit buffers for the time specified
 *
 * @param [in]  nTimeout        Timeout (in nanoseconds)
 *
 * @note   In SerDes mode, the request to flush the buffers is re-direct to the above layers as a callback
 */
ADI_NETWORK_CODE
void adi_network_flushTxTimed(uint64_t nTimeout)
{
#ifdef ADI_SERDES_MODE
    /* In SerDes mode, give a callback to the application code for flushing out buffers in the application */
    goNwData.pfCbk(0U, ADI_NETWORK_EVT_APP_REQUEST_FLUSH_BUFFERS, NULL);
#endif

    uint64_t nEndTime, nCurrTime;
    /* Get the current time */
    adi_pal_getCurrTime(&nEndTime);
    nEndTime += nTimeout;

    /* Run continuously for the time given */
    do
    {
        adi_network_run();
        adi_pal_getCurrTime((uint64_t *)&nCurrTime);
    } while(nCurrTime < nEndTime);
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Filter table APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Sets the MAC address filtering in the MAC-PHY node
 *
 * @details     Configures the hardware MAC address filter table to accept frames only from
 *              specified source addresses. Maximum 16 addresses can be programmed. Filter is
 *              applied at hardware level for efficient packet filtering without CPU overhead.
 *
 * @param [in]  nDevNum         The number of the device to set MAC filter table.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  anMacAddr       Array of the MAC addresses to be entered in the filter table
 * @param [in]  nNumAddr        Number of entries to be added in the MAC address filter table (Max = 16)
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully configured filter table
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter (device num or nNumAddr > 16)
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setMacAddrFilterTable(uint32_t nDevNum, uint8_t anMacAddr[][6u], uint8_t nNumAddr)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Validate maximum filter table entries (hardware limit) */
        if(nNumAddr > 16u)
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        /* Lock mutex to ensure atomic filter table update */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific filter table programming */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetFilterTable(nDevNum, anMacAddr, nNumAddr);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
#ifdef ETH_EN
            network_eth_SetFilterTable(nDevNum, anMacAddr, nNumAddr);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* ETH_EN */
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after filter table update completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief       Clears MAC address filtering in the MAC-PHY node
 *
 * @details     Removes all entries from the hardware MAC address filter table, effectively
 *              disabling address filtering. After clearing, the device will accept frames
 *              from any source address (promiscuous mode).
 *
 * @param [in]  nDevNum         The number of the device to clear MAC filter table.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully cleared filter table
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_clearMacAddrFilterTable(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic filter table clear operation */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific filter table clear */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* Clears the MAC filter table to accept all addresses */
#ifdef MACPHY_EN
            eNwErr = network_macphy_ClearFilterTable(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
#ifdef ETH_EN
            network_eth_ClearFilterTable(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* ETH_EN */
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after filter table clear completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Enables VLAN filtering
 *
 * @param [in] nDevNum           The number of the device to set VLAN filtering
                                 Use the array index from the list obtained in adi_network_getDevices
 * @param [in] nVlanId1          VLAN ID entry 1
 * @param [in] nVlanId2          VLAN ID entry 2
 * @param [in] bDropFrames       If set true, the frames having VlanId1/VlanId2 are dropped , else they are passed in
 * @param [in] bEnable           if set true, enables the Vlan filtering
 * @return     Status
 *             - #ADI_NETWORK_OK                Successfully added the frame to queue
 *             - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *             - #ADI_NETWORK_NO_SPACE          No space available in queue
 *             - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *             - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
*/
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setVlanFilter(uint32_t nDevNum, uint16_t nVlanId1, uint16_t nVlanId2,
        bool bDropFrames, bool bEnable)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set the VLAN filter */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetVlanFilter(nDevNum, nVlanId1, nVlanId2, bDropFrames, bEnable);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Enables EtherType Filtering
 *
 * @param [in] nDevNum           The number of the device to set Ethertype filtering
                                 Use the array index from the list obtained in adi_network_getDevices
 * @param [in] nEtherType1       EtherType entry 1
 * @param [in] nEtherType2       EtherType entry 2
 * @param [in] bDropFrames       If set true, the frames having EtherType1/EtherType2 would be filtered out else they are passed in
 * @param [in] bEnable           if set true, enables the EtherType filtering
 * @return     Status
 *               - #ADI_NETWORK_OK                Successfully added the frame to queue
 *               - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *               - #ADI_NETWORK_NO_SPACE          No space available in queue
 *               - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *               - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setEtherTypeFilter(uint32_t nDevNum, uint16_t nEtherType1,
        uint16_t nEtherType2, bool bDropFrames, bool bEnable)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set the EtherType filter */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetEtherTypeFilter(nDevNum, nEtherType1, nEtherType2, bDropFrames, bEnable);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Wake-sleep APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Send the controller node to sleep mode
 *
 * @param [in]  nDevNum         The number of the device to send to sleep mode.
 *                              Use the array index from the list obtained in adi_network_getDevices
 *
 * @return      Status
 *              - #ADI_NETWORK_OK               Successfully added goto_sleep request
 *              - #ADI_NETWORK_NODE_NOT_AWAKE   Node is not awake to send goto_sleep request
 *
 * @note        This API doesn't block until the node goes to sleep. The application will
 *              be notified asynchronously via adi_network_cbk callback with appropriate event
 *              - #ADI_NETWORK_EVT_SLEEP            If node goes to sleep successfully
 *              - #ADI_NETWORK_EVT_SLEEP_FAILED     If node fails to go to sleep
 *              - #ADI_NETWORK_DEVICE_CLOSED        Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE        Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM        Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sleepControllerNode(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then send the node to sleep */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SleepNode(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Send the controller node to standby mode
 *
 * @param [in]  nDevNum         The number of the device to send to standby mode.
 *                              Use the array index from the list obtained in adi_network_getDevices
 *
 * @return      Status
 *              - #ADI_NETWORK_OK               Successfully added goto_standby request
 *              - #ADI_NETWORK_NODE_NOT_AWAKE   Node is not awake to send goto_standby request
 *
 * @note        This API doesn't block until the node goes to standby. The application will
 *              be notified asynchronously via registered callback with appropriate event
 *              - #ADI_NETWORK_EVT_STANDBY          If node goes to standby successfully
 *              - #ADI_NETWORK_EVT_STANDBY_FAILED   If node fails to go to standby
 *              - #ADI_NETWORK_DEVICE_CLOSED        Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE        Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM        Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_standbyControllerNode(uint32_t nDevNum)
{


    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then send the node to standby */
#ifdef MACPHY_EN
            eNwErr = network_macphy_StandbyNode(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Send a wake-up pulse from the controller node onto T1S bus
 *
 * @param [in]  nDevNum         The number of the device to send the wake-up pulse from.
 *                              Use the array index from the list obtained in adi_network_getDevices
 *
 * @return      Status
 *              - #ADI_NETWORK_OK               Successfully added WUP request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED    Device is not opened
 *              - #ADI_NETWORK_NODE_NOT_AWAKE   Node is not awake to send a WUP
 *              - #ADI_NETWORK_MUTEX_FAILURE    Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM    Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sendWakeUpPulse(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then send wake-up pulse */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SendWakeUpPulse(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Trigger wake-up pin of node
 *
 * @details     Activates the wake-up pin to bring a sleeping node back to active state.
 *              In low-power automotive networks, nodes enter sleep mode to conserve power.
 *              The wake-up pin provides a hardware mechanism to signal the node to wake up
 *              and resume network communication. This is typically used in wake-on-network
 *              scenarios where a master node needs to wake sleeping nodes.
 *
 * @param [in]  nDevNum         The number of the device to trigger wake-up pin.
 *                              Use the array index from the list obtained in adi_network_getDevices
 *
 * @return      Status
 *              - #ADI_NETWORK_OK                   Successfully triggered the wake pin
 *              - #ADI_NETWORK_DEVICE_CLOSED        Device is not opened
 *              - #ADI_NETWORK_GPIO_WRITE_FAILED    Failed to trigger the wake pin
 *              - #ADI_NETWORK_INVALID_PARAM        Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_triggerWakeUpPin(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Dispatch to device-type specific wake-up pin trigger */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            /* Trigger GPIO-based wake-up signal for MACPHY devices */
            eNwErr = network_macphy_TriggerWakeUpPin(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* Wake-up pin triggering not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Get the state of controller node (awake / standby / sleep)
 *
 * @param [in]  nDevNum         The number of the device to get the state from.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [out] peCtrlState     State of controller node
 *
 */
ADI_NETWORK_CODE
void adi_network_getControllerState(uint32_t nDevNum, ADI_NETWORK_CTRL_STATES **peCtrlState)
{
    /* The state of controller node (awake / standby / sleep) */
    *peCtrlState = &goNwData.aoDevInfo[nDevNum].oWkslpData.eNodeState;
}

/**
 * @brief       Notify network layer of node's state
 *
 * @param [in]  nDevNum         The number of the device to update the state.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  eCtrlState      State to set
 *
 */
ADI_NETWORK_CODE
void adi_network_notifyControllerNodeState(uint32_t nDevNum, ADI_NETWORK_CTRL_STATES eCtrlState)
{
    ADI_NETWORK_WKSLP_DATA *poWkslpData = &goNwData.aoDevInfo[nDevNum].oWkslpData;

    /* Clear wake-sleep data */
    (void)ADI_MEMSET(poWkslpData, 0, sizeof(ADI_NETWORK_WKSLP_DATA));

    /* Update the state of the node */
    poWkslpData->eNodeState = eCtrlState;
}

/**
 * @brief       Get reason for wake / sleep
 *
 * @param [in]  nDevNum         The number of the device to get the reason for sleep / wake.
 *                              Use the array index from the list obtained in adi_network_getDevices
 *
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully added the read request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the read request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getReasonForWkslp(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then get the reason for wake/sleep */
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetReasonForWkslp(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Safe State Controller APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Sets the configurations for safe state controller (SSC)
 *
 * @param  [in] nDevNum                     The number of the device to set the configuration for safe state controller.
 *                                          Use the array index from the list obtained in adi_network_getDevices
 * @param  [in] poSscSetupCfg               Pointer to the SSC configuration
 * @return Status
 *              - #ADI_NETWORK_OK                Successfully added the read request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the read request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sscSetConfig(uint32_t nDevNum,
        const ADI_NETWORK_MACPHY_SSC_MONITOR_CONFIG *poSscSetupCfg)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set the configuration for safe state controller */
#ifdef MACPHY_EN
            eNwErr = network_macphy_sscSetConfig(nDevNum, poSscSetupCfg);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Used to return to functional state manually, if tried to return to functional mode with the safe state triggers still present, throws an interrupt for unsuccessful return!
 *
 * @param [in] nDevNum                      The number of the device to return to functional state manually.
 *                                          Use the array index from the list obtained in adi_network_getDevices
 * @return Status
 *              - #ADI_NETWORK_OK                Successfully added the read request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the read request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sscReturnToFunctional(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then return to functional state */
#ifdef MACPHY_EN
            eNwErr = network_macphy_sscReturnToFunctional(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Forces safe state (Manual trigger)
 *
 * @param [in] nDevNum           The number of the device to force safe state.
 *                               Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bForceSafeState   if set true, sets Manual trigger, else resets it
 * @return Status
 *              - #ADI_NETWORK_OK                Successfully added the read request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the read request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sscForceSafeState(uint32_t nDevNum, bool bForceSafeState)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then force safe state */
#ifdef MACPHY_EN
            eNwErr = network_macphy_sscForceSafeState(nDevNum, bForceSafeState);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Gets the alive counter readback value, the readback doesn't update in safe mode
 *
 * @param [in] nDevNum      The number of the device to get the alive counter value.
 *                          Use the array index from the list obtained in adi_network_getDevices
 * @return Status
 *              - #ADI_NETWORK_OK                Successfully added the read request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the read request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sscAliveCounterRead(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then get the alive counter readback value */
#ifdef MACPHY_EN
            eNwErr = network_macphy_sscAliveCounterRead(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Gets the status of safe state controller (SSC)
 *
 * @param  [in] nDevNum         The number of the device to get the status of SSC.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return Status
 *              - #ADI_NETWORK_OK                Successfully added the read request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the read request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sscCheckSafeModeStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then get the status of SSC */
#ifdef MACPHY_EN
            eNwErr = network_macphy_sscCheckSafeModeStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Sets the device unattended bit, if not written within the timeout programmed periodically - triggers safe state
 *
 * @param [in] nDevNum      The number of the device to set the device unattended bit.
 *                          Use the array index from the list obtained in adi_network_getDevices
 * @return Status
 *              - #ADI_NETWORK_OK                Successfully added the read request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the read request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_sscMarkDeviceAttended(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set the device unattended bit */
#ifdef MACPHY_EN
            eNwErr = network_macphy_sscMarkDeviceAttended(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ PLCA APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief       Enables the PLCA for the controller node
 *
 * @param [in]  nDevNum         The number of the device to set PLCA.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  bEnabled        If PLCA is to be enabled / disabled
 * @param [in]  nNodeCount      The number of nodes in the network
 * @param [in]  nNodeId         The node ID of the controller
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully added the frame to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setPlca(uint32_t nDevNum, bool bEnabled, uint32_t nNodeCount, uint32_t nNodeId)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* If the device type is MACPHY , then proceed to set the PLCA */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set the PLCA */
#ifdef MACPHY_EN
            ADI_NETWORK_MACPHY_PLCA_CFG oPlcaCfg;
            oPlcaCfg.anPLCAID[0U] = (uint8_t)nNodeId;
            oPlcaCfg.bEnabled = bEnabled;
            oPlcaCfg.nNodeCount = nNodeCount;
            oPlcaCfg.nPlcaIdEn = ADI_NETWORK_MACPHY_EN_PLCA_ID_(0);
            eNwErr = network_macphy_SetPlcaMulti(nDevNum, &oPlcaCfg);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief       Enables the PLCA for the controller node
 *
 * @param [in]  nDevNum         The number of the device to set PLCA.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  poPlcaCfg       Pointer to the PLCA configurations
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully added the frame to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setPlcaMulti(uint32_t nDevNum, const ADI_NETWORK_MACPHY_PLCA_CFG *poPlcaCfg)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set the PLCA */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetPlcaMulti(nDevNum, poPlcaCfg);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Sets PLCA leader mode by performing MAC-PHY register read and masked write at given bit postions
 *
 * @param [in] nDevNum               The number of the device to enable PLCA leader mode.
 *                                    Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bPlcaLeader           If Set, elects the node as PLCA leader
 * @param [in] bPlcaLeaderModeEn     If Set, allows the node to elect as PLCA leader
 * @return     Status
 *               - #ADI_NETWORK_OK                Successfully added the frame to queue
 *               - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *               - #ADI_NETWORK_NO_SPACE          No space available in queue
 * @note  if elects the node as PLCA leader.
 *        Then configure the PLCA node count value for that particular node using (network_macphy_SetPlcaMulti) API.
 * @note  The possible errors to callback function is as follows:
 *                - #ADI_NETWORK_NO_SPACE               No space available in queue
 *                - #ADI_NETWORK_MACPHY_REG_READ_FAILED Register read failed
 *                - #ADI_NETWORK_MACPHY_REG_WRITE_FAILED Register write failed
 *                - #ADI_NETWORK_MUTEX_FAILURE    Mutex lock or unlock failed
 *                - #ADI_NETWORK_INVALID_PARAM    Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setPlcaLeaderMode(uint32_t nDevNum, bool bPlcaLeader, bool bPlcaLeaderModeEn)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* check for mutex lock success */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Check for network type of the device */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then Set PLCA leader mode */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetPlcaLeaderMode(nDevNum, bPlcaLeader, bPlcaLeaderModeEn);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* check for mutex unlock success */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Enables the PLCA precedence mode by performing MAC-PHY register read and masked write at given bit postions
 *
 * @param [in] nDevNum               The number of the device to enable PLCA precedence mode.
 *                                    Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bPlcaPrecedenceEn     If set, enables precedence mode
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully added the frame to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue
 * @note  if enabling , It has to be set on all nodes in network
 * @note  The possible errors to callback function is as follows:
 *                - #ADI_NETWORK_NO_SPACE                No space available in queue
 *                - #ADI_NETWORK_MACPHY_REG_READ_FAILED  Register read failed
 *                - #ADI_NETWORK_MACPHY_REG_WRITE_FAILED Register write failed
 *                - #ADI_NETWORK_MUTEX_FAILURE    Mutex lock or unlock failed
 *                - #ADI_NETWORK_INVALID_PARAM    Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_enablePlcaPrecedence(uint32_t nDevNum, bool bPlcaPrecedenceEn)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Check for network type of the device */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then enable PLCA precedence mode */
#ifdef MACPHY_EN
            eNwErr = network_macphy_EnablePlcaPrecedence(nDevNum, bPlcaPrecedenceEn);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Sets the PLCA transmit opportunity timer
 *
 * @details     Configures the maximum time a node can hold the transmit opportunity
 *              in PLCA (Physical Layer Collision Avoidance) mode. This timer ensures
 *              fair channel access by limiting transmission duration per node.
 *
 * @param [in] nDevNum         The number of the device for which the PLCA Transmit Opportunity timer has to be set
 *                             Use the array index from the list obtained in adi_network_getDevices
 * @param [in] nToTime         Transmit opportunity time (units is bit times)
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured TO timer
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setPlcaToTimer(uint32_t nDevNum, uint32_t nToTime)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic PLCA timer configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific handler - PLCA only for MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetPlcaToTimer(nDevNum, nToTime);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* PLCA not supported for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after timer configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);


    return eNwErr;
}

/**
 * @brief Sets the PLCA burst timer and max burst count
 *
 * @details     Configures burst mode parameters for PLCA operation. Burst mode allows
 *              a node to transmit multiple frames consecutively during its transmit
 *              opportunity, improving efficiency for bursty traffic patterns. The burst
 *              timer limits burst duration to maintain fair channel access.
 *
 * @param [in] nDevNum         The number of the device for which the PLCA Burst timer and Max Burst Count has to be set
 *                             Use the array index from the list obtained in adi_network_getDevices
 * @param [in] nMaxBurstCount  PLCA max burst count (maximum frames per burst)
 * @param [in] nBurstTime      PLCA burst timer (units in bit times)
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured burst parameters
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setPlcaBurstTimer(uint32_t nDevNum, uint8_t nMaxBurstCount, uint8_t nBurstTime)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic burst parameter configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific handler - burst mode only for MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetPlcaBurstTimer(nDevNum, nMaxBurstCount, nBurstTime);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* PLCA burst mode not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after burst configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Loopback APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Enables MAC Loopback
 *
 * @details     Configures MAC layer loopback mode for testing. In this mode, transmitted
 *              frames are internally looped back to the receiver at the MAC layer without
 *              reaching the PHY. Used for MAC-level diagnostics and testing.
 *
 * @param [in] nDevNum         The number of the device to enable MAC Loopback
 *                             Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bMacLoopback    if set 1, enables the MAC loopback
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured MAC loopback
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_enableMacLoopBack(uint32_t nDevNum, bool bMacLoopback)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic loopback configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific loopback configuration */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_EnableMacLoopBack(nDevNum, bMacLoopback);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Ethernet controller loopback not implemented in this interface */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after loopback configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Enables PMA Loopback
 *
 * @details     Configures Physical Medium Attachment (PMA) loopback for testing. In this mode,
 *              transmitted signals are looped back at the PMA sublayer, testing the digital
 *              portion of the PHY without requiring external physical medium. Useful for
 *              isolating PHY layer issues.
 *
 * @param [in] nDevNum         The number of the device to enable PMA Loopback
 *                             Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bPmaLoopback    if set 1, enables the PMA loopback
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured PMA loopback
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_enablePmaLoopBack(uint32_t nDevNum, bool bPmaLoopback)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic PMA configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific loopback - PMA only in MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_EnablePmaLoopBack(nDevNum, bPmaLoopback);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* PMA loopback not exposed for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after PMA configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Enables PCS Loopback
 *
 * @details     Configures Physical Coding Sublayer (PCS) loopback for testing. In this mode,
 *              transmitted data is looped back at the PCS layer between MAC and PMA. Useful
 *              for testing the PCS encoding/decoding logic without involving the analog PHY
 *              components.
 *
 * @param [in] nDevNum         The number of the device to enable PCS Loopback
 *                             Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bPcsLoopback    if set 1, enables the PCS loopback
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured PCS loopback
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_enablePcsLoopBack(uint32_t nDevNum, bool bPcsLoopback)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic PCS configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific loopback - PCS only in MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_EnablePcsLoopBack(nDevNum, bPcsLoopback);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* PCS loopback not exposed for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after PCS configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Timestamp / timer APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Captures external timestamp
 *
 * @details     Triggers a timestamp capture from an external input signal. The captured
 *              timestamp is synchronized with the device's internal timer and can be used
 *              for precise event timing correlation in time-sensitive networking applications.
 *
 * @param [in] nDevNum         The number of the device to capture external timestamp
 *                             Use the array index from the list obtained in adi_network_getDevices
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully captured timestamp
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_captureExtTimestamp(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic timestamp capture */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific timestamp capture */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_CaptureExtTimestamp(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* External timestamp capture not implemented for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after timestamp capture completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Enables TS_TIMER with the given frequency and dutycycle
 *
 * @details     Configures and starts the timestamp timer output signal with specified
 *              frequency, duty cycle, and start time. This timer can be used to generate
 *              synchronized periodic signals for time-sensitive applications or for clock
 *              distribution in networked systems.
 *
 * @param [in] nDevNum              The number of the device to enable TS_TIMER
 *                                  Use the array index from the list obtained in adi_network_getDevices
 * @param [in] nFreq                Frequency of the TS_TIMER output (in Hz)
 * @param [in] nDutyCycle           Duty cycle of the TS_TIMER output (in %)
 * @param [in] nStartTime           Point in time at which to start the TS_TIMER counter (in ns)
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured and started timer
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_enableTsTimer(uint32_t nDevNum, uint32_t nFreq,
        uint8_t nDutyCycle, uint32_t nStartTime)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic timer configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific timer configuration */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_EnableTsTimer(nDevNum, nFreq, nDutyCycle, nStartTime);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* TS_TIMER not available for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after timer configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Stops the toggling of TS_TIMER Output
 *
 * @details     Halts the timestamp timer output signal. The timer state is preserved and
 *              can be restarted later. Use this function to stop synchronized periodic
 *              signals when they are no longer needed.
 *
 * @param [in] nDevNum         The number of the device to stop TS_TIMER
 *                             Use the array index from the list obtained in adi_network_getDevices
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully stopped timer output
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_stopTsTimer(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic timer state change */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific timer stop */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_StopTsTimer(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* TS_TIMER not available for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after timer stop completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Enables 1 PPS output
 *
 * @details     Configures and enables the 1 Pulse Per Second (1PPS) output signal on the
 *              specified SA-IF pin. The 1PPS signal is synchronized with the device's internal
 *              timer and is used for precise time synchronization in PTP and other time-sensitive
 *              networking applications.
 *
 * @param [in]  nDevNum         The number of the device to enable 1PPS signal
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in]  nSAIF           SA-IF pin to output the 1PPS signal
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully enabled 1PPS output
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_enablePpsOutput(uint32_t nDevNum, uint8_t nSAIF)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic PPS configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific PPS output configuration */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_EnablePpsOutput(nDevNum, nSAIF);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* 1PPS output not available for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after PPS configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ SQI APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Configuration for SQI threshold and selection of PLCA ID to monitor
 *
 * @details     Configures Signal Quality Index (SQI) thresholds and monitoring parameters.
 *              SQI provides link quality indication based on PHY layer measurements.
 *              Threshold validation ensures proper hysteresis between good/warning/bad states
 *              to prevent oscillation. Settle range determines measurement window duration.
 *
 * @param [in] nDevNum             The number of the device to enable and configure SQI settings.
 *                                  Use the array index from the list obtained in adi_network_getDevices
 * @param [in] poSqiCfg            Configuration structure for SQI thresholds and parameters
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured SQI
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or threshold validation failed
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_configureSqi(uint32_t nDevNum, const ADI_NETWORK_MACPHY_SQI_CFG *poSqiCfg)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Validate configuration structure is not NULL */
        if(poSqiCfg == NULL)
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        /* Validate good-to-warning threshold has proper hysteresis */
        if(poSqiCfg->nGoodToWarningLvl < poSqiCfg->nWarningToGoodLvl)
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        /* Validate warning-to-bad threshold has proper hysteresis */
        if(poSqiCfg->nWarningToBadLvl < poSqiCfg->nBadToWarningLvl)
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        /* Ensure threshold ordering: good > warning > bad */
        if(poSqiCfg->nGoodToWarningLvl > poSqiCfg->nBadToWarningLvl)
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        /* Validate settle range is within supported values */
        if(poSqiCfg->eSqiSettleRange > ADI_SQI_SETTLE_RANGE_100K)
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
            break;
        }

        /* Lock mutex to ensure atomic SQI configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific SQI configuration */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* Configure SQI for MACPHY devices */
#ifdef MACPHY_EN
            eNwErr = network_macphy_ConfigureSqi(nDevNum, poSqiCfg);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* SQI not available for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after SQI configuration completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Gets SQI index
 *
 * @param [in] nDevNum          The number of the device to get SQI index.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return  Status
 *              - #ADI_NETWORK_OK        Successfully added the request to queue
 *              - #ADI_NETWORK_NO_SPACE  No space available in queue
 *              - #ADI_NETWORK_MUTEX_FAILURE    Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM    Invalid parameter
 *              - #ADI_NETWORK_DEVICE_CLOSED    Device is not opened
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getSqiIndex(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure consistent SQI measurement read */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific SQI retrieval - primarily for MACPHY */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* Get the SQI index from PHY measurements */
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetSqiIndex(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* SQI not available for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after SQI read completes */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Disable SQI
 *
 * @param [in] nDevNum          The number of the device to reset SQI.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return  Status
 *              - #ADI_NETWORK_OK                Successfully added the request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_disableSqi(uint32_t nDevNum)
{

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If the device is MACPHY, disable the SQI */
#ifdef MACPHY_EN
            eNwErr = network_macphy_disableSqi(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Topology Discovery APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Set measurement duration for topology discovery
 *
 * @param [in] nDevNum           The number of the device to set the measurement duration.
 *                               Use the array index from the list obtained in adi_network_getDevices
 * @param [in] nTimeDuration     Duration for which the measurement should take place (in ms) - takes value from 0 to 15
 */
ADI_NETWORK_CODE
void adi_network_setTopoDiscDuration(uint32_t nDevNum, uint8_t nTimeDuration)
{
    /* Check if the device is a MACPHY type device */
    if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
    {
#ifdef MACPHY_EN
        /* Validate measurement duration is within hardware limits (0-15ms) */
        ADI_DBG_REQUIRE(nTimeDuration <= 15U, "Measurement time duration cannot be greater than 15ms");

        /* Store the topology discovery measurement duration for this device */
        goNwData.aoDevInfo[nDevNum].oTopoDiscData.nTopoMeasDuration = nTimeDuration;
#else
        /* MACPHY support not enabled in build configuration */
        ADI_DBG_ERROR();
#endif /* MACPHY_EN */
    }
    /* Ethernet devices do not support topology discovery duration configuration */
    else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
    {
        ADI_DBG_ERROR();
    }
    /* Handle invalid or unsupported network device type */
    else
    {
        ADI_DBG_ERROR();
    }
}

/**
 * @brief Sets the node either as reference node or measured node, in case of measured node TD_EN bit is set which blocks TX path until cleared
 *
 * @details     Configures the node's role in topology discovery measurements. A reference node
 *              initiates measurements and keeps TX enabled, while a measured node blocks TX
 *              (via TD_EN bit) to ensure it doesn't interfere with the measurement process.
 *              This distinction is critical for accurate distance/delay measurements.
 *
 * @param [in] nDevNum       The number of the device to set the node either as reference/measured type
 *                           Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bIsRefNode    If true, MACPHY acts as reference node in the measurement else it acts as a measured node
 * @return  Status
 *          - #ADI_NETWORK_OK                Successfully configured node role
 *          - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *          - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *          - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setTopoDiscRefNode(uint32_t nDevNum, bool bIsRefNode)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic topology discovery configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Configure node role for topology discovery */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            goNwData.aoDevInfo[nDevNum].oTopoDiscData.bIsTopoRefNode = bIsRefNode;

            /* Block TX path for measured nodes to prevent interference during measurements */
            if(!bIsRefNode)
            {
                eNwErr = network_macphy_EnableTDBit(nDevNum, true);
            }

#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Topology discovery not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after node role configuration */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Starts internal delay measurement
 *
 * @details     Initiates measurement of the node's internal signal propagation delay through
 *              the PHY. This delay must be known for accurate distance calculations between
 *              nodes. PLCA must be disabled since topology discovery uses CSMA/CD mechanism.
 *
 * @param [in] nDevNum     The number of the device to start internal delay measurement
 *                         Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully started delay measurement
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter, unsupported device, or PLCA enabled
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_startDelayMeas(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic measurement start */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Start delay measurement - requires PLCA disabled */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN

            /* Ensure PLCA is disabled since topology discovery requires CSMA/CD */
            if(goNwData.aoDevInfo[nDevNum].bIsPlcaEn != true)
            {
                eNwErr = network_macphy_StartTopoDiscMeas(nDevNum, TOPO_DISC_INTERNAL_DELAY);
            }
            else
            {
                eNwErr = ADI_NETWORK_INVALID_PARAM;
            }

#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Topology discovery not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after measurement initiation */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Gets internal delay measurement result
 *
 * @details     Retrieves the measured internal delay value from the device. This delay
 *              represents the signal propagation time through the node's PHY circuitry.
 *              Must be called after startDelayMeas completes. Ensure TD_EN bit is cleared
 *              before reading to unblock TX path if needed.
 *
 * @param [in] nDevNum     The number of the device to get internal delay measurement result
 *                         Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved delay measurement
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 *
 * @note Application has to make sure that TD_EN bit is cleared when necessary before reading the results
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getDelayMeas(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic measurement read */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Retrieve delay measurement result */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            /* Flag indicates delay result (not distance) is being retrieved */
            goNwData.aoDevInfo[nDevNum].oTopoDiscData.bGetDelayResult = true;
            eNwErr = network_macphy_GetTopoDiscMeas(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Topology discovery not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after measurement read */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Starts distance measurement
 *
 * @details     Initiates measurement of the cable distance between two nodes. Uses
 *              round-trip time measurement principles. Distance calculation requires
 *              both nodes' internal delays to be known. PLCA must be disabled to use
 *              CSMA/CD-based measurement mechanism.
 *
 * @param [in] nDevNum     The number of the device to start distance measurement
 *                         Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully started distance measurement
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter, unsupported device, or PLCA enabled
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_startDistanceMeas(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic measurement start */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Start distance measurement - requires PLCA disabled */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN

            /* Ensure PLCA is disabled since topology discovery requires CSMA/CD */
            if(goNwData.aoDevInfo[nDevNum].bIsPlcaEn != true)
            {
                eNwErr = network_macphy_StartTopoDiscMeas(nDevNum, TOPO_DISC_DISTANCE);
            }
            else
            {
                eNwErr = ADI_NETWORK_INVALID_PARAM;
            }

#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Topology discovery not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after measurement initiation */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Gets distance measurement - This distance result is absolute translation of register value; the actual distance can be computed by subtracting their internal delays and dividing it by 2
 *
 * @details     Retrieves the raw distance measurement result. The returned value includes both
 *              cable propagation delay and internal delays of both nodes. To calculate actual
 *              cable distance: (raw_result - delay_node1 - delay_node2) / 2. Ensure TD_EN bit
 *              is cleared before reading to unblock TX path if needed.
 *
 * @param [in] nDevNum     The number of the device to get distance measurement
 *                         Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved distance measurement
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 *
 * @note Application has to make sure that TD_EN bit is cleared when necessary before reading the results
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getDistanceMeas(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic measurement read */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Retrieve distance measurement result */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            /* Flag indicates distance result (not delay) is being retrieved */
            goNwData.aoDevInfo[nDevNum].oTopoDiscData.bGetDistanceResult = true;
            eNwErr = network_macphy_GetTopoDiscMeas(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Topology discovery not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after measurement read */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Starts topology discovery in automatic mode
 *
 * @details     Initiates automatic topology discovery which performs both internal delay
 *              and distance measurements sequentially. This mode simplifies the measurement
 *              process by automating the sequence. PLCA must be disabled to use CSMA/CD-based
 *              measurement mechanism.
 *
 * @param [in] nDevNum     The number of the device to start automatic mode for topology discovery.
 *                         Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully started automatic measurements
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter, unsupported device, or PLCA enabled
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_startAutoMeas(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic automatic measurement start */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Start automatic topology discovery - requires PLCA disabled */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN

            /* Ensure PLCA is disabled since topology discovery requires CSMA/CD */
            if(goNwData.aoDevInfo[nDevNum].bIsPlcaEn != true)
            {
                eNwErr = network_macphy_StartTopoDiscMeas(nDevNum, TOPO_DISC_AUTOMATIC);
            }
            else
            {
                eNwErr = ADI_NETWORK_INVALID_PARAM;
            }

#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Topology discovery not supported for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after automatic measurement initiation */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Clear TD enable - used to unblock the TX path of the node
 *
 * @details     Clears the Topology Discovery enable bit (TD_EN) to re-enable the transmit
 *              path after topology discovery measurements complete. The TD_EN bit is
 *              automatically set for measured nodes to prevent TX interference during
 *              measurements and must be manually cleared when measurements finish.
 *
 * @param [in] nDevNum     The number of the device to clear TD enable bit.
 *                         Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully cleared TD bit and unblocked TX
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_clearTDBit(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic TD bit clear */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Clear TD bit to unblock TX path */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            /* Pass false to disable TD bit, enabling normal TX operation */
            eNwErr = network_macphy_EnableTDBit(nDevNum, false);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* TD bit not applicable for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after TD bit clear */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Misc APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Get Revision number
 *
 * @details     Retrieves the silicon revision ID from the device hardware. This information
 *              is used to identify the specific chip version and may be needed for errata
 *              workarounds or feature compatibility checks in the driver.
 *
 * @param [in]  nDevNum         The number of the device to get revision ID from
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully retrieved revision
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getRevision(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic revision register read */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific revision retrieval */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_GetRevision(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Revision retrieval not implemented for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after revision read */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Enables the test mode control
 *
 * @details     Configures the PHY test mode for compliance testing and diagnostics.
 *              Test modes allow verification of transmitter characteristics, signal
 *              integrity, and EMI compliance by generating predefined test patterns
 *              or placing the transmitter in specific states.
 *
 * @param [in] nDevNum          The number of the device to set Test mode
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in] eTestMode        Test mode selection:
 *                              0: Normal operation
 *                              1: Continuously transmit '1'
 *                              2: Square wave with high and low period=1.6us
 *                              3: Transmit pseudo-random sequence
 *                              4: Transmitter high impedance termination
 *                              5-6: Reserved
 *                              7: Transmit wake-up tone
 *
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured test mode
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_enableTestMode(uint32_t nDevNum, ADI_NETWORK_TESTMODE eTestMode)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic test mode configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific test mode configuration */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_EnableTestMode(nDevNum, eTestMode);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* Test mode control not exposed for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after test mode configuration */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;

}

/**
 * @brief Sets PMA control
 *
 * @details     Configures Physical Medium Attachment (PMA) layer control settings.
 *              PMA reset reinitializes the PMA sublayer, while transmit disable can be
 *              used to electrically silence the transmitter for power saving or testing.
 *
 * @param [in] nDevNum          The number of the device to set PMA Control
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bPmaReset        If true, resets PMA sublayer to default state
 * @param [in] bTransmitDisable If true, disables PMA transmitter output
 * @return     Status
 *              - #ADI_NETWORK_OK                Successfully configured PMA control
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setPmaControl(uint32_t nDevNum, bool bPmaReset, bool bTransmitDisable)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Lock mutex to ensure atomic PMA control configuration */
        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        /* Dispatch to device-type specific PMA control configuration */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetPmaControl(nDevNum, bPmaReset, bTransmitDisable);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
        {
            /* PMA control not exposed for standard Ethernet controllers */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
        else
        {
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        /* Release mutex after PMA control configuration */
        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Ping controller node to check if it is awake
 *
 * @param [in]  nDevNum         The number of the device to ping. Use the array index
 *                              from the list obtained in adi_network_getDevices
 *
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully added the ping request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue to add the ping request
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_pingControllerNode(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then ping controller node */
#ifdef MACPHY_EN
            eNwErr = network_macphy_PingControllerNode(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief Sets the MAC FIFO(s) size
 *
 * @param [in] nDevNum                     The number of the device to set FIFO Sizes.
 *                                         Use the array index from the list obtained
 *                                         in adi_network_getDevices
 * @param [in] poFIFOSize                  Pointer to FIFO size configuration structure
 * @return     Status
 *             - #ADI_NETWORK_OK                Successfully added the frame to queue
 *             - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *             - #ADI_NETWORK_NO_SPACE          No space available in queue
 *             - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *             - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setMacFIFOSize_Macphy(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_MACPHY_CONFIG *poFIFOSize)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set MAC FIFO size */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetMacFIFOSize_Macphy(nDevNum, poFIFOSize);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}


/**
 * @brief Sets the MAC FIFO(s) size in dual mode
 *
 * @param [in] nDevNum                        The number of the device to set FIFO Sizes
 *                                            in Dual Mode. Use the array index from the
 *                                            list obtained in adi_network_getDevices
 * @param [in] poFIFOSize                     Pointer to FIFO size configuration structure
 * @return     Status
 *             - #ADI_NETWORK_OK                Successfully added the frame to queue
 *             - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *             - #ADI_NETWORK_NO_SPACE          No space available in queue
 *             - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *             - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setMacFIFOSize_DualMode(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_DUAL_MODE_CONFIG *poFIFOSize)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
            /* If device type is MACPHY then set MAC FIFO size in dual mode */
#ifdef MACPHY_EN
            eNwErr = network_macphy_SetMacFIFOSize_DualMode(nDevNum, poFIFOSize);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }
    } while(false);

    return eNwErr;
}

/**
 * @brief       Gets Bootloader status for HV and LV Die blocks
 *
 * @param [in]  nDevNum         The number of the device to get bootloader status.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully added the request to queue
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_NO_SPACE          No space available in queue
 *              - #ADI_NETWORK_MUTEX_FAILURE     Mutex lock or unlock failed
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter
 *
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_getBootLoaderStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        if(prj_osal_lockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            /* If device type is MACPHY then get bootloader status */
            eNwErr = network_macphy_GetBootLoaderStatus(nDevNum);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* If the device is ETHERNET, return an error */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }

        if(prj_osal_unlockMutex(ghNwRunMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_MUTEX_FAILURE;
            break;
        }

    } while(false);

    return eNwErr;
}

/**
 * @brief Releases the mutex in the network layer
 *
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_releaseMutex(void)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
#ifndef ADI_SERDES_MODE

    if(prj_osal_unlockMutex(ghNwMutex) != PRJ_OSAL_STATUS_SUCCESS)
    {
        /* If failed to unlock the mutex, return error */
        eNwErr = ADI_NETWORK_MUTEX_FAILURE;
    }
    else
    {
        eNwErr = ADI_NETWORK_OK;
    }

#endif
    return eNwErr;
}

/**
 * @brief Enables/disables gPTP initial rate correction
 *
 * @details     Controls the initial rate correction feature for gPTP (generalized Precision
 *              Time Protocol). When enabled, the hardware applies initial frequency adjustment
 *              to accelerate clock synchronization at startup. This reduces time-to-sync in
 *              time-sensitive networking applications.
 *
 * @param [in] nDevNum      The number of the device to configure gPTP initial rate correction
 *                          Use the array index from the list obtained in adi_network_getDevices
 * @param [in] bEnable      If true, enables gPTP initial rate correction; if false, disables it
 *
 * @return      Status
 *              - #ADI_NETWORK_OK                Successfully configured initial rate correction
 *              - #ADI_NETWORK_DEVICE_CLOSED     Device is not opened
 *              - #ADI_NETWORK_INVALID_PARAM     Invalid parameter or unsupported device type
 */
ADI_NETWORK_CODE
ADI_NETWORK_ERR adi_network_setGPTPInitialRateCorrection(uint32_t nDevNum, bool bEnable)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Use do-while pattern to enable early exit on error via break */
    do
    {
        /* Verify device is initialized and opened */
        eNwErr = ValidateDeviceOpen(nDevNum);

        if(eNwErr != ADI_NETWORK_OK)
        {
            break;
        }

        /* Dispatch to device-type specific gPTP configuration */
        if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
        {
#ifdef MACPHY_EN
            /* Configure initial rate correction for gPTP synchronization */
            eNwErr = network_macphy_SetGPTPInitialRateCorrection(nDevNum, bEnable);
#else
            eNwErr = ADI_NETWORK_INVALID_PARAM;
#endif /* MACPHY_EN */
        }
        else
        {
            /* gPTP initial rate correction not available for standard Ethernet */
            eNwErr = ADI_NETWORK_INVALID_PARAM;
        }
    } while(false);

    return eNwErr;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Network layer VLAN APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief       Fills VLAN tag(s) in the provided buffer location
 *
 * @param [in]  pBuf        Pointer to the buffer where the VLAN tag(s) is to be inserted
 * @param [in]  pVlanCfgArr Array of VLAN tag(s) configs
 * @param [in]  nNumCfgs    Number of VLAN tags to fill
 * @param [out] pnSize      Number of bytes used by the VLAN tag(s)
 */
ADI_NETWORK_CODE
void adi_network_fillVlanTags(uint8_t *pBuf, ADI_VLAN_CFG *pVlanCfgArr, uint8_t nNumCfgs, uint16_t *pnSize)
{
    /* Pre-conditions: Check pointer to buffer is not NULL and the size pointer is not NULL */
    ADI_DBG_REQUIRE(pBuf != NULL, "Buffer is NULL");
    ADI_DBG_REQUIRE(pnSize != NULL, "Size pointer is NULL");

    uint8_t nFrmIdx = 0U;

    /* Loop through the number of vlan tags to be filled, and add it to the buffer provided */
    for(uint8_t i = 0U; i < nNumCfgs; i++)
    {
        /* Fill VLAN tag(s) in the provided buffer location for the number of configs specified */
        pBuf[nFrmIdx++] = (uint8_t)(pVlanCfgArr[i].nTPID >> 8U);
        pBuf[nFrmIdx++] = (uint8_t)(pVlanCfgArr[i].nTPID);
        pBuf[nFrmIdx++] = (uint8_t)(pVlanCfgArr[i].nPCP << 5U) |
                          (uint8_t)((pVlanCfgArr[i].bDEI ? 1U : 0U) << 4U) |
                          (uint8_t)(pVlanCfgArr[i].nVlanId >> 8U);
        pBuf[nFrmIdx++] = (uint8_t)(pVlanCfgArr[i].nVlanId);
    }

    *pnSize = nFrmIdx;
}

/**
 * @brief       Extracts VLAN tag(s) from the buffer
 *
 * @param [in]  pBuf        Pointer to the buffer from which to extract VLAN tag(s)
 * @param [in]  nMaxVlanCfgs Maximum number of VLAN configs (Indicates the size of pVlanCfgArr)
 * @param [out] pVlanCfgArr Pointer to the config array where the VLAN tag(s) should be filled
 * @param [out] pnNumCfgs   Number of VLAN tags extracted
 * @param [out] pnSize      Number of bytes used by the VLAN tag(s) in the buffer
 */
ADI_NETWORK_CODE
void adi_network_extractVlanTags(uint8_t *pBuf, uint8_t nMaxVlanCfgs, ADI_VLAN_CFG *pVlanCfgArr,
                                 uint8_t *pnNumCfgs, uint16_t *pnSize)
{
    /* Pre-conditions: Check pointer to buffer is not NULL, the VLAN cfg array is not NULL, and the number of configs pointer is not NULL */
    ADI_DBG_REQUIRE(pBuf != NULL, "Buffer is NULL");
    ADI_DBG_REQUIRE(pVlanCfgArr != NULL, "VLAN cfg array is NULL");
    ADI_DBG_REQUIRE(pnNumCfgs != NULL, "Number of configs pointer is NULL");
    ADI_DBG_REQUIRE(pnSize != NULL, "Size pointer is NULL");

    uint8_t nNumCfgs = 0U;
    uint16_t nFrmIdx = 0U;

    while(true)
    {
        /* Get the Ethernet type from the buffer */
        uint16_t nEthType = ((uint16_t)pBuf[nFrmIdx] << 8U) | (uint16_t)pBuf[nFrmIdx + 1U];

        /* Extract VLAN tag(s) from the buffer */
        if(nEthType == VLAN_C_TAG_TPID || nEthType == VLAN_S_TAG_TPID)
        {
            /* Fill the VLAN tag(s) in the provided buffer location */
            pVlanCfgArr[nNumCfgs].nTPID = ((uint16_t)pBuf[nFrmIdx] << 8U) |
                                          ((uint16_t)pBuf[nFrmIdx + 1U]);
            pVlanCfgArr[nNumCfgs].nPCP = ((pBuf[nFrmIdx + 2U] & 0xE0U) >> 5U);
            pVlanCfgArr[nNumCfgs].bDEI = ((pBuf[nFrmIdx + 2U] & 0x10U) == 0x10U) ? true : false;
            pVlanCfgArr[nNumCfgs].nVlanId = (((uint16_t)pBuf[nFrmIdx + 2U] & 0x0FU) << 8U) |
                                            ((uint16_t)pBuf[nFrmIdx + 3U]);
            nFrmIdx += 4U;
            nNumCfgs++;

            /* If the number of VLAN tags exceeds the maximum number of VLAN configs, break */
            if(nNumCfgs > nMaxVlanCfgs)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    *pnNumCfgs = nNumCfgs;
    *pnSize = (uint16_t)nNumCfgs * 4U;
}

/* ~~~~~~~~~~~~~~~~~~~~~~ Network layer measurement APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief       Start network layer measurement
 *
 * @param [in]  nDevNum         The number of the device to start the measurements.
 *                              Use the array index from the list obtained in adi_network_getDevices
 */
ADI_NETWORK_CODE
void adi_network_startMeasurement(uint32_t nDevNum)
{
    /* Start network layer measurement */
    goNwData.aoDevInfo[nDevNum].bEnMeas = true;
    (void)ADI_MEMSET(&goNwData.aoDevInfo[nDevNum].oMeas, 0, sizeof(ADI_NETWORK_IF_MEAS_DATA));
    /* Get the current time */
    adi_pal_getCurrTime(&goNwData.aoDevInfo[nDevNum].nMeasStartTime);
}

/**
 * @brief       Stop network layer measurement
 *
 * @param [in]  nDevNum         The number of the device to start the measurements.
 *                              Use the array index from the list obtained in adi_network_getDevices
 */
ADI_NETWORK_CODE
void adi_network_stopMeasurement(uint32_t nDevNum)
{
    /* Stop network layer measurement */
    goNwData.aoDevInfo[nDevNum].bEnMeas = false;
    adi_pal_getCurrTime(&goNwData.aoDevInfo[nDevNum].nMeasEndTime);
}

/**
 * @brief       Get network layer measurement
 *
 * @param [in]  nDevNum         The number of the device to start the measurements.
 *                              Use the array index from the list obtained in adi_network_getDevices
 * @param [out] pNetworkMeas    Network layer measurement object
 */
ADI_NETWORK_CODE
void adi_network_getMeasurement(uint32_t nDevNum, ADI_NETWORK_IF_MEAS_DATA **pNetworkMeas)
{
    ADI_NETWORK_DEVINFO *poDevInfo = &goNwData.aoDevInfo[nDevNum];
    ADI_NETWORK_IF_MEAS_DATA *poLibMeas = &poDevInfo->oMeas;
    uint64_t nEndTime = poDevInfo->nMeasEndTime;
    uint64_t nTxBits = (poLibMeas->nTxBytesCnt) * 8ULL;
    uint64_t nRxBits = (poLibMeas->nRxBytesCnt) * 8ULL;
    double fTxBits = (double)nTxBits;
    double fRxBits = (double)nRxBits;

    /* If measurement is still running, get the current time */
    if(poDevInfo->bEnMeas)
    {
        adi_pal_getCurrTime(&nEndTime);
    }

    /* Calculate the time difference */
    uint64_t nTimeDiff = (nEndTime - poDevInfo->nMeasStartTime);
    double fTimeDiff = (double)nTimeDiff;

    /* Calculate the bandwidth */
    poLibMeas->fTxBandwidth = (fTxBits / fTimeDiff) / ((double)1024.0 * (double)1024.0) * (double)1E9;
    poLibMeas->fRxBandwidth = (fRxBits / fTimeDiff) / ((double)1024.0 * (double)1024.0) * (double)1E9;
    *pNetworkMeas = poLibMeas;
}

/**
 * @brief       Log network layer measurement
 *
 * @param [in]  nDevNum         The number of the device to log the measurements.
 *                              Use the array index from the list obtained in adi_network_getDevices
 */
ADI_NETWORK_CODE
void adi_network_logMeasurement(uint32_t nDevNum)
{
    /* Get the measurement data */
    ADI_NETWORK_IF_MEAS_DATA *poLibMeas = &goNwData.aoDevInfo[nDevNum].oMeas;
    /* Log the measurement data */
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "TX Frame count: " UINT32_FORMATTER "\r\n", poLibMeas->nTxFrameCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "RX Frame count: " UINT32_FORMATTER "\r\n", poLibMeas->nRxFrameCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "TX Byte count : 0x" UINT32_HEX_FORMATTER UINT32_HEX_FORMATTER "\r\n",
                (uint32_t)(poLibMeas->nTxBytesCnt >> 32U), (uint32_t)poLibMeas->nTxBytesCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "RX Byte count : 0x" UINT32_HEX_FORMATTER UINT32_HEX_FORMATTER "\r\n",
                (uint32_t)(poLibMeas->nRxBytesCnt >> 32U), (uint32_t)poLibMeas->nRxBytesCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "TX Bandwidth (Mbps) : %f\r\n", poLibMeas->fTxBandwidth);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "RX Bandwidth (Mbps) : %f\r\n", poLibMeas->fRxBandwidth);
}

/*! \cond PRIVATE */
/*================================ CALLBACKS ===============================*/
/**
 * @brief       Callback from the pal when a packet is received from npcap
 *
 * @param [in]  nId             ID of the network device
 * @param [in]  eEvent          PAL event
 * @param [in]  pData           Data corresponding to the event
 */
ADI_NETWORK_CODE
void adi_pal_networkCbk(uint32_t nId, ADI_PAL_EVT eEvent, const void *pData)
{
    /* Switch based on the PAL callback event */
    switch(eEvent)
    {
        case ADI_PAL_ETH_CBK:
        {
            /* Received ethernet frame from the ethernet port */
            const ADI_PAL_EVT_ETHDATA *poEthData = (const ADI_PAL_EVT_ETHDATA *)pData;
            ADI_NETWORK_EVT_ETH_RX_RECEIVED_DATA oRxDat;
            uint32_t nDevNum;
#ifdef MACPHY_EN
            nDevNum = nId + goNwData.nSpiDevs;
#else
            nDevNum = nId;
#endif /* MACPHY_EN */

            /* Bandwidth measurement */
            if(goNwData.aoDevInfo[nDevNum].bEnMeas)
            {
                goNwData.aoDevInfo[nDevNum].oMeas.nRxFrameCnt++;
                goNwData.aoDevInfo[nDevNum].oMeas.nRxBytesCnt += poEthData->nSize;
            }

            oRxDat.nLength = (uint16_t)poEthData->nSize;
            oRxDat.pRxData = poEthData->anData;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ETH_RX_RECEIVED, &oRxDat);
            break;
        }

        case ADI_PAL_SPI_CBK:
        {
            const bool *pbError = (const bool *)pData;

            if(!*pbError)
            {
#ifdef MACPHY_EN
                uint32_t nInstNum = goNwData.aoDevInfo[nId].nCtrlIdx;
                adi_oaspi_NotifySpiCompletion(nInstNum);

                if(gbSignalAsyncOperation)
                {
                    (void)prj_osal_signalSemaphore(ghAsyncSemaphore);
                }

#endif /* MACPHY_EN */
            }
            else
            {
                goNwData.aoDevInfo[nId].bInErrState = true;
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_SPI_SEND_FAILED;
                goNwData.pfCbk(nId, ADI_NETWORK_EVT_ERR, &oErrDat);
            }

            break;
        }

        case ADI_PAL_TMR_CBK:
        {
            /* Nothing to do! */
            break;
        }

        case ADI_PAL_ETH_TS_CBK:
        {
            /* We are currently not using timestamping capability of uC */
#if 0
            /* Timestamp captured in ethernet interface callback */
            const ADI_PAL_EVT_ETH_TSDATA *poTsData = (const ADI_PAL_EVT_ETH_TSDATA *)pData;
            ADI_MACPHY_TS_DATA oMacTsData =
            {
                .nSec = poTsData->nSec,
                .nNanoSec = poTsData->nNanoSec,
            };
            ADI_NETWORK_EVT_TSCAPTD_DAT oNwTsData =
            {
                .poTimestamp = &oMacTsData,
            };
            uint32_t nDevNum = nId + goNwData.nSpiDevs;

            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TS_CAPTD, &oNwTsData);
#endif

            break;
        }

        case ADI_PAL_EVT_IRQ_CBK:
        {
#ifdef MACPHY_EN
            uint32_t nInstNum = goNwData.aoDevInfo[nId].nCtrlIdx;
            (void)adi_oaspi_ProcessIrq(nInstNum);

            if(gbSignalAsyncOperation)
            {
                (void)prj_osal_signalSemaphore(ghAsyncSemaphore);
            }

#endif /* MACPHY_EN */
            break;
        }

        case ADI_PAL_EVT_ASYNC_CBK:
        {
            (void)prj_osal_signalSemaphore(ghAsyncSemaphore);
            break;
        }

        default:
        {
            /* Nothing to do */
            break;
        }
    }
}

/*=============================== STATIC CODE ===============================*/
/*! Fill the vlan tags in the given buffer */
ADI_NETWORK_CODE
static void fillVlanTags(uint8_t *pBuf, uint32_t *panVlanTags, uint8_t nNumCfgs, uint16_t *pnSize)
{
    /* Pre-conditions: */
    ADI_DBG_REQUIRE(pBuf != NULL, "Buffer is NULL");
    ADI_DBG_REQUIRE(pnSize != NULL, "Size pointer is NULL");

    uint8_t nFrmIdx = 0U;

    for(uint8_t i = 0U; i < nNumCfgs; i++)
    {
        pBuf[nFrmIdx++] = (uint8_t)((panVlanTags[i] & 0xFF000000U) >> 24U); /* Extract the TPID */
        pBuf[nFrmIdx++] = (uint8_t)((panVlanTags[i] & 0x00FF0000U) >> 16U);
        pBuf[nFrmIdx++] = (uint8_t)((panVlanTags[i] & 0xFF00U) >> 8U); /* Extract PCP,DEI and VLAN ID */
        pBuf[nFrmIdx++] = (uint8_t)(panVlanTags[i]);
    }

    *pnSize = nFrmIdx;
}

#ifndef ADI_SERDES_MODE
/*! Submit ethernet frame for transmission */
ADI_NETWORK_CODE
static void transmitFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg)
{
    if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
    {
#ifdef MACPHY_EN
        network_macphy_SubmitTxFrame(nDevNum, poNwFrameCfg);
#else
        ADI_DBG_ERROR();
#endif /* MACPHY_EN */
    }
    else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
    {
#ifdef ETH_EN
        network_eth_SubmitTxFrame(nDevNum, poNwFrameCfg);
#else
        ADI_DBG_ERROR();
#endif /* ETH_EN */
    }
    else
    {
        ADI_DBG_ERROR();
    }
}
#else
/*! Submit ethernet frame for transmission in serdes mode */
ADI_NETWORK_CODE
static ADI_NETWORK_ERR serdesTransmitFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg, uint8_t *pBuffer)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
    {
#ifdef MACPHY_EN
        eNwErr = network_macphy_SubmitTxFrame(nDevNum, poNwFrameCfg, pBuffer);
#else
        ADI_DBG_ERROR();
#endif /* MACPHY_EN */
    }
    else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
    {
#ifdef ETH_EN
        /* Currently there is no return for this function, hence we are not capturing the network error status */
        network_eth_SubmitTxFrame(nDevNum, poNwFrameCfg, pBuffer);
#else
        ADI_DBG_ERROR();
#endif /* ETH_EN */
    }
    else
    {
        ADI_DBG_ERROR();
    }

    return eNwErr;
}
#endif /* ADI_SERDES_MODE */

/*! Checks if TX Buffers are empty */
ADI_NETWORK_CODE
static bool areTxBuffersEmpty(uint32_t nDevNum)
{
    bool bAreTxBuffersEmpty = false;

    if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_MACPHY)
    {
#ifdef MACPHY_EN
        bAreTxBuffersEmpty = network_macphy_AreTxBuffersEmpty(nDevNum);
#else
        ADI_DBG_ERROR();
#endif /* MACPHY_EN */
    }
    else if(goNwData.aoDevInfo[nDevNum].eNwType == ADI_NETWORK_TYPE_ETHERNET)
    {
#ifdef ETH_EN
        bAreTxBuffersEmpty = network_eth_AreTxBuffersEmpty(nDevNum);
#else
        ADI_DBG_ERROR();
#endif /* ETH_EN */
    }
    else
    {
        ADI_DBG_ERROR();
    }

    return bAreTxBuffersEmpty;
}

/*! Reset the network data structure */
ADI_NETWORK_CODE
static void resetNetworkData(void)
{
    uint32_t i;

    /* Clear the data */
    (void)ADI_MEMSET(&goNwData, 0, sizeof(ADI_NETWORK_DATA));

    /* Clear the controller index */
    for(i = 0U; i < ADI_NETWORK_MAX_DEVS; i++)
    {
        goNwData.aoDevInfo[i].nCtrlIdx = 0xFFFFFFFFU;
    }
}

#ifndef ADI_SERDES_MODE
/**
 * @brief Check if transmit queue slot is full at given index
 * @param[in] nWrIdx Write index to check in queue
 * @param[in] nDevNum Device number
 * @return true if queue slot is taken (full), false if available (not full)
 */
ADI_NETWORK_CODE
static bool IsQueueFull(uint16_t nWrIdx, uint32_t nDevNum)
{
    /* Assume queue is full unless proven otherwise */
    bool bIsQueueFull = true;

    /* Check if the transmit index slot is available */
    if(gabTransmitIdxTaken[nWrIdx][nDevNum] == false)
    {
        /* Slot is available - queue not full */
        bIsQueueFull = false;
    }

    /* Return queue full status */
    return bIsQueueFull;
}

#ifdef ADI_NETWORK_PRIO_Q_EN
/**
 * @brief Check if HP transmit queue slot is full at given index
 * @param[in] nWrIdx Write index to check in HP queue
 * @param[in] nDevNum Device number
 * @return true if HP queue slot is taken (full), false if available (not full)
 */
ADI_NETWORK_CODE
static bool IsHpQueueFull(uint16_t nWrIdx, uint32_t nDevNum)
{
    /* Assume queue is full unless proven otherwise */
    bool bIsQueueFull = true;

    /* Check if the HP transmit index slot is available */
    if(gabTransmitIdxTakenHp[nWrIdx][nDevNum] == false)
    {
        /* Slot is available - queue not full */
        bIsQueueFull = false;
    }

    /* Return queue full status */
    return bIsQueueFull;
}

/**
 * @brief Determine if the frame should be routed to the HP pool
 * @param[in] nInstNum  Controller instance number
 * @param[in] poNwFrameCfg  Frame configuration with routing info
 * @return true if frame should use HP pool, false for LP pool
 */
ADI_NETWORK_CODE
static bool IsHpRoute(uint32_t nInstNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg)
{
    bool bIsHp = false;

#ifdef MACPHY_EN
    if(goNwData.paoNwMacPhyCfg[nInstNum] != NULL)
    {
        if((goNwData.paoNwMacPhyCfg[nInstNum]->bEnablePrioFifos) &&
           (poNwFrameCfg->eMacRoutingInfo == ADI_MACPHY_ROUTE_PORT1_HP))
        {
            bIsHp = true;
        }
        else
        {
            /* Nothing to do - bIsHp remains false */
        }
    }
    else
    {
        /* Nothing to do - bIsHp remains false */
    }
#endif /* MACPHY_EN */

    return bIsHp;
}

/**
 * @brief Get effective LP queue size based on priority FIFO configuration
 * @param[in] nInstNum  Controller instance number
 * @return NETWORK_DATA_LP_PRIO_Q_SIZE (24) if priority FIFOs enabled, NETWORK_DATA_Q_SIZE (32) otherwise
 */
ADI_NETWORK_CODE
static uint16_t GetLpQueueSize(uint32_t nInstNum)
{
    uint16_t nLpQueueSize = NETWORK_DATA_Q_SIZE;

#ifdef MACPHY_EN
    if(goNwData.paoNwMacPhyCfg[nInstNum] != NULL)
    {
        if(goNwData.paoNwMacPhyCfg[nInstNum]->bEnablePrioFifos)
        {
            nLpQueueSize = NETWORK_DATA_LP_PRIO_Q_SIZE;
        }
        else
        {
            /* Nothing to do - nLpQueueSize remains 32 */
        }
    }
    else
    {
        /* Nothing to do - nLpQueueSize remains 32 */
    }
#endif /* MACPHY_EN */

    return nLpQueueSize;
}
#endif /* ADI_NETWORK_PRIO_Q_EN */
#endif /* ADI_SERDES_MODE */

/* Increment queue index with wraparound */
ADI_NETWORK_CODE
static void IncrementQIdx(volatile uint16_t *pnIdx, uint16_t nQSize)
{
    /* Increment the index and wrap around if necessary */
    uint16_t nTempIdx = *pnIdx + 1U;
    *pnIdx = (nTempIdx >= nQSize) ? (nTempIdx - nQSize) : nTempIdx;
}

/* Common runtime validation for all public network APIs */
ADI_NETWORK_CODE
static ADI_NETWORK_ERR ValidateDeviceOpen(uint32_t nDevNum)
{
    /* Initialize error status to success */
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Check if device number exceeds total available devices */
    if(nDevNum >= goNwData.nNumDevices)
    {
        /* Device number out of range - set invalid parameter error */
        eNwErr = ADI_NETWORK_INVALID_PARAM;
    }
    /* Check if the specified device is currently open */
    else if(goNwData.aoDevInfo[nDevNum].bIsOpened != true)
    {
        /* Device is not open - set device closed error */
        eNwErr = ADI_NETWORK_DEVICE_CLOSED;
    }
    else
    {
        /* Device is valid and open - no error */
    }

    /* Return validation result */
    return eNwErr;
}

/*! \endcond */

/** @} */

/**
 * EOF: www.analog.com
 */
