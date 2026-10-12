/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: demo_adpd.c
 * \brief: The ADPD demo implementation
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "demo_adpd.h"

#ifdef ADI_E2B_IFACE_SPI_ENABLED

#include "AdpdDrv.h"          /*!< The ADPD driver */
/*=================================== DATA ==================================*/
ADI_MEM_DATA_CRIT_CACHE
static DEMO_ADPD_DATA goDemoAdpdData;

/*!
 * The default configuration for the ADPD1080
 */
ADI_MEM_DATA_CONST
static const uint32_t dcfg_org_108[] =
{
    0x00060000U,
    0x001130A0U,
    0x00120050U,
    0x00140775U,
    0x00150110U,
    0x00181F00U,
    0x00193FFFU,
    0x001A3FFFU,
    0x001B3FFFU,
    0x001E1F00U,
    0x001F3FFFU,
    0x00203FFFU,
    0x00213FFFU,
    0x0022105FU,
    0x0023305FU,
    0x0024105FU,
    0x0025FFDFU,
    0x00300220U,
    0x0031020fU,
    0x00340000U,
    0x00350220U,
    0x0036040fU,
    0x00391af8U,
    0x003B1af8U,
    0x003C7006U,
    0x00421C35U,
    0x0043ADA5U,
    0x00441C35U,
    0x0045ADA5U,
    0x004E7040U,
    0x00540AA0U,
    0x003F0320U,
    0x00580000U,
    0x00590808U,
    0x005A0010U,
    0xFFFFFFFFU
};

/** Lookup table for the neopixel */
ADI_MEM_DATA_CONST
static const uint32_t ganLookUpNeo[] =
{
    [31U] = 1162U,
    [30U] = 1170U,
    [29U] = 1182U,
    [28U] = 1200U,
    [27U] = 1220U,
    [26U] = 1260U,
    [25U] = 1310U,
    [24U] = 1370U,
    [23U] = 1450U,
    [22U] = 1540U,
    [21U] = 1625U,
    [20U] = 1650U,
    [19U] = 1800U,
    [18U] = 2000U,
    [17U] = 2200U,
    [16U] = 2400U,
    [15U] = 3000U,
    [14U] = 4500U,
    [13U] = 5000U,
    [12U] = 6000U,
    [11U] = 9000U,
    [10U] = 12000U,
    [9U]  = 14250U,
    [8U]  = 16750U,
    [7U]  = 20000U,
    [6U]  = 24000U,
    [5U]  = 26750U,
    [4U]  = 28250U,
    [3U]  = 31750U,
    [2U]  = 34500U,
    [1U]  = 37250U,
    [0U]  = 40000U
};

/** Read data */
ADI_MEM_DATA_CRIT_CACHE
uint32_t gdnAdpdReadData[256U];

/* Read index */
ADI_MEM_DATA_CRIT_CACHE
uint32_t gdnAdpdReadIdx = 0U;

/* Buffer for the SPI periodic data (both TX and RX) */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganPeriodicTxRx[5U];

ADI_MEM_DATA_CRIT_CACHE
ADI_EAL_SPI_IO_DATA goSpiIOData;
/*================================ PROTOTYPES ===============================*/
static void _sendInit(void);
static void _sendCfgPeriodic(void);
static void _cbkBlockRead(uint32_t nNodeIdx, const uint8_t *pData, uint32_t nSize);
static void _loadDefaultConfig(const uint32_t *cfg);
static void _bringUp(uint8_t nSlotA, uint8_t nSlotB);
static double _movingAverage(void);
static uint32_t LookUpNeo(double fAverage);
/*=================================== CODE ==================================*/
/*!< Initializes the ADPD library */
ADI_MEM_CODE_NOCRIT
void demo_adpd_init(ADI_EAL_SPI_HANDLE hSpiHandle, DEMO_ADPD_CALLBACK_FUNC pfAdpdCbk)
{
    /* Clear the data */
    (void)ADI_MEMSET(&goDemoAdpdData, 0, sizeof(DEMO_ADPD_DATA));

    goDemoAdpdData.hProxSpiIntfHandle = hSpiHandle;

    ADI_DBG_REQUIRE(pfAdpdCbk != NULL, "Function pointer is NULL");
    goDemoAdpdData.pfAdpdCbk = pfAdpdCbk;

    /* Soft reset the ADPD device */
    ADI_DBG_FUNC((uint32_t)AdpdDrvSoftReset(), "Failed in AdpdDrvSoftReset");

    /* Initialize the ADPD driver*/
    ADI_DBG_FUNC((uint32_t)AdpdDrvOpenDriver(), "Failed in AdpdDrvOpenDriver");

    /* Load default configuration parameters */
    _loadDefaultConfig(dcfg_org_108);

    /* Write standard value of clock registers */
    ADI_DBG_FUNC((uint32_t)AdpdDrvRegWrite(0x004BU, 0x2695U), "Failed in AdpdDrvRegWrite");
    ADI_DBG_FUNC((uint32_t)AdpdDrvRegWrite(0x004DU, 0x4272U), "Failed in AdpdDrvRegWrite");

    /* Driver bring up with 16-bits output data and 8 channel mode */
    _bringUp(ADPDDrv_SLOT_OFF, ADPDDrv_SUM_32);

    /* Send the initializations */
    _sendInit();
}


ADI_MEM_CODE_NOCRIT
void demo_adpd_configurePeriodic(void)
{
    _sendCfgPeriodic();
}

/*! \brief   The routine function for the ADPD application */
ADI_MEM_CODE_NOCRIT
void demo_adpd_run(void)
{
    uint64_t nCurrentTime;

    /* Get the current time */
    adi_pal_getCurrTime(&nCurrentTime);

    /* If periodic has not yet been configured, wait for 1s since initialization and then send it */
    if((goDemoAdpdData.bPeriodicConfigured == false) && ((nCurrentTime - goDemoAdpdData.nInitTime) > SEC_TO_NS))
    {
        goDemoAdpdData.bPeriodicConfigured = true;
        goDemoAdpdData.nPeriodicCfgTime = nCurrentTime;
    }

    /* If periodic has not yet been started, wait for 10ms since periodic configuration and then send it */
    if((goDemoAdpdData.bPeriodicStarted == false)
            && (goDemoAdpdData.bPeriodicConfigured == true)
            && ((nCurrentTime - goDemoAdpdData.nPeriodicCfgTime) > (10ULL * MS_TO_NS)))
    {
        goDemoAdpdData.bPeriodicStarted = true;
    }
}

/*============================= Driver Callbacks ============================*/
/*! @brief   Handles the SPI transmit from the ADPD library */
ADI_MEM_CODE_NOCRIT
ADI_HAL_STATUS_t ADPD_SPI_Transmit(uint8_t *pTxData, uint32_t nLength)
{
    ADI_DBG_REQUIRE(nLength == 3U, "ADPD Spi transmit length not equal to 3");
    (void)ADI_MEMCPY(&goDemoAdpdData.anAdpdCtrlTx[goDemoAdpdData.nAdpdCtrlIdx++], pTxData, nLength);
    return ADI_HAL_OK;
}

/*! @brief   Handles the SPI transmit from the ADPD library */
ADI_MEM_CODE_NOCRIT
ADI_HAL_STATUS_t ADPD_SPI_TxRx(ADPDDrv_Recv_Event_t eEvent, uint8_t *pTxData, uint32_t nLength)
{
    /* Ignore the requests for TXRX from the library */
    return ADI_HAL_OK;
}

/** Function to process the sensor data */
ADI_MEM_CODE_CRIT
void demo_adpd_ProcessSensorData(uint8_t *pBuff, uint32_t nSize)
{
    _cbkBlockRead(0U, pBuff, 8U);
}

/*================================== STATIC =================================*/
/** Sends the initializations */
ADI_MEM_CODE_NOCRIT
static void _sendInit(void)
{
    ADI_EAL_STATUS eEalStatus;

    //ADPD SPI each transcation length should equal to 3
    ADI_EAL_SPI_IO_DATA oSpiIOData;
    oSpiIOData.pTxData = (uint8_t *)goDemoAdpdData.anAdpdCtrlTx;
    oSpiIOData.nWriteLength = 3U;
    oSpiIOData.apRxData = NULL;
    oSpiIOData.nReadLength = 0U;
    oSpiIOData.nChipSel = 0xFEU;

    eEalStatus = adi_eal_spiMultiWrite(goDemoAdpdData.hProxSpiIntfHandle, &oSpiIOData, goDemoAdpdData.nAdpdCtrlIdx, 0xABu);

    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Could not Write SPI transaction");
}

/** Sends the commands to start the periodic transfer */
ADI_MEM_CODE_NOCRIT
static void _sendCfgPeriodic(void)
{
    ADI_EAL_STATUS eEalStatus;

    /* Set the data */
    (void)ADI_MEMSET(ganPeriodicTxRx, 0, sizeof(ganPeriodicTxRx));
    ganPeriodicTxRx[0U] = REG_DATA_BUFFER << 1u;

    /* Array of pointers to RX data */
    uint8_t *apRxData[1U] =
    {
        &ganPeriodicTxRx[0U],
    };

    goSpiIOData.pTxData = ganPeriodicTxRx;
    goSpiIOData.nWriteLength = sizeof(ganPeriodicTxRx);
    goSpiIOData.apRxData = &apRxData[0U];
    goSpiIOData.nReadLength = sizeof(ganPeriodicTxRx);
    goSpiIOData.nChipSel = 0xFEU;

    eEalStatus = adi_eal_spiSetupPeriodicWriteRead(goDemoAdpdData.hProxSpiIntfHandle, &goSpiIOData, 0x01u);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Could not add SPI transaction configuration to the queue");
}

/*! Handles the block read callback from the E2B layer */
ADI_MEM_CODE_CRIT
static void _cbkBlockRead(uint32_t nNodeIdx, const uint8_t *pData, uint32_t nSize)
{
    uint32_t nIntensity;
    double fAverage;
    uint32_t nLC;

    /* Copy the data */
    for(nLC = 0u; nLC < ADPD_FIFO_THRESH; nLC += 4u)
    {
        /* Convert to 32 bit value
         * The proximity sends 15:8 7:0 31:24 23:16
         */
        nIntensity =
            (pData[nLC + 0u] << 8u)
            | (pData[nLC + 1u] << 0u)
            | (pData[nLC + 2u] << 24u)
            | (pData[nLC + 3u] << 16u);
        gdnAdpdReadData[gdnAdpdReadIdx] = nIntensity;
        gdnAdpdReadIdx = (gdnAdpdReadIdx + 1u) % 0xFFu;

        /* Check if the value is sane */
        if((nIntensity < 40000U) && (nIntensity > 500U))
        {
            /* Convert to proximity by scaling it and add it to the moving average filter */
            goDemoAdpdData.afAdpdMvnAvg[goDemoAdpdData.nProxValIdx % ADPD_AVG_WIND] = nIntensity;
            goDemoAdpdData.nProxValIdx++;

            if(goDemoAdpdData.nProxValCnt < ADPD_AVG_WIND)
            {
                goDemoAdpdData.nProxValCnt++;
            }
        }
    }

    /* If valid proximity values are there in the filter */
    if(goDemoAdpdData.nProxValCnt > 0u)
    {
        fAverage = _movingAverage();
        uint32_t nLength = LookUpNeo(fAverage);

        goDemoAdpdData.pfAdpdCbk(nLength, fAverage);

    }
}

/*  Computes a windowed moving average */
ADI_MEM_CODE_CRIT
static double _movingAverage(void)
{
    double fSortedArray[ADPD_AVG_WIND];
    double fResult = 0.0, fTemp = 0.0, fMedian = 0.0;
    uint32_t i = 0u, j = 0u, nCount = 0u;

    /* Make a copy */
    (void)ADI_MEMCPY(fSortedArray, goDemoAdpdData.afAdpdMvnAvg, sizeof(goDemoAdpdData.afAdpdMvnAvg));

    /* Sort the array */
    for(i = 0u; i < ADPD_AVG_WIND; i++)
    {
        for(j = i + 1U; j < ADPD_AVG_WIND; j++)
        {
            if(fSortedArray[j] < fSortedArray[i])
            {
                fTemp = fSortedArray[j];
                fSortedArray[j] = fSortedArray[i];
                fSortedArray[i] = fTemp;
            }
        }
    }

    /* Get the median */
    fMedian = fSortedArray[ADPD_AVG_WIND / 2u];

    /* Find the average after removing outliers */
    for(i = 0u; i < ADPD_AVG_WIND; i++)
    {
        if((goDemoAdpdData.afAdpdMvnAvg[i] < fMedian * 1.5f)
                && (goDemoAdpdData.afAdpdMvnAvg[i] > fMedian * 0.5f))
        {
            fResult += goDemoAdpdData.afAdpdMvnAvg[i];
            nCount++;
        }
    }

    fResult = fResult / (double)nCount;

    return fResult;
}

/**
 *  @brief    Load ADPD default configuration
 *  @param    uint32_t *cfg
 *  @retval   None
 */
ADI_MEM_CODE_NOCRIT
static void _loadDefaultConfig(const uint32_t *cfg)
{
    uint8_t nRegAddr, i = 0U;
    uint16_t nRegData;

    /* Preconditions */
    ADI_DBG_REQUIRE(cfg != NULL, "ADPD default configuration is null");

    while(1)
    {
        /* Read the address and data from the configuration */
        nRegAddr = (uint8_t)(cfg[i] >> 16U);
        nRegData = (uint16_t)(cfg[i]);

        i++;

        if(nRegAddr == 0xFFU)
        {
            break;
        }

        /* Load the data into the ADPD registers */
        if(AdpdDrvRegWrite(nRegAddr, nRegData) != ADPDDrv_SUCCESS)
        {
            break;
        }
    }
}

/**
 *  @brief  ADPD Driver bring up.
 *  @param  uint8_t nSlotA
 *  @param  uint8_t nSlotB
 *  @retval None
 */
ADI_MEM_CODE_NOCRIT
static void _bringUp(uint8_t nSlotA, uint8_t nSlotB)
{
    /* Set the slot modes for slot A and slot B */
    ADI_DBG_FUNC((uint32_t)AdpdDrvSetSlot(nSlotA, nSlotB), "Failed in AdpdDrvSetSlot");

    /* Set the device operation to sample mode. The data can be collected now */
    AdpdDrvSetOperationMode(ADPDDrv_MODE_SAMPLE);
}

/*! Looks up the proximity based on the intensity */
ADI_MEM_CODE_CRIT
static uint32_t LookUpNeo(double fAverage)
{
    uint32_t nLC;

    for(nLC = ADI_LOGO_NUM_OF_PIXELS; nLC >= 1U; nLC--)
    {
        if((uint32_t)fAverage < ganLookUpNeo[nLC - 1U])
        {
            return nLC;
        }
    }

    return 0U;
}

#endif /* ADI_E2B_IFACE_SPI_ENABLED */

/** @} */

/*
 * EOF: https://www.analog.com/
*/
