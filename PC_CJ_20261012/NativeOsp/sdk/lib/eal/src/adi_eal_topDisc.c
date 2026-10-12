/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_topDisc.c
 * @brief: The EAL APIs for topology discovery
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "eal_internal.h"   /* The EAL internal header */

#ifdef ADI_E2B_IFACE_TOPDISC_ENABLED

/*================================= DEFINES ================================*/
/*! \cond PRIVATE */
#define ADI_REG_READ_RESP_TIMEOUT_MS        (100U)                                       /*!< The timeout for reg read response in milliseconds */
#define MEAS_TIME                           ((uint64_t)4U * (uint64_t)MS_TO_US)          /*!< Measurement time */
#define REFN_INIT                           ((uint32_t)ENUM_PHY_OA_TD_CTRL_TD_REFN_ENUM001 << BITP_PHY_OA_TD_CTRL_TD_REFN)  /*!< Sets REF bit */
#define REFN_DLYM                           REFN_INIT | ((uint32_t)ENUM_PHY_OA_TD_CTRL_TD_EN_ENABLED << BITP_PHY_OA_TD_CTRL_TD_EN) | ((uint32_t)ENUM_PHY_OA_TD_CTRL_TD_DLYM_START_TRIGGER << BITP_PHY_OA_TD_CTRL_TD_DLYM_START)  /*!< Start Delay measurement on reference node */
#define REFN_DM                             REFN_INIT | ((uint32_t)ENUM_PHY_OA_TD_CTRL_TD_EN_ENABLED << BITP_PHY_OA_TD_CTRL_TD_EN) | ((uint32_t)ENUM_PHY_OA_TD_CTRL_TD_DM_START_TRIGGER << BITP_PHY_OA_TD_CTRL_TD_DM_START)    /*!< Start Distance measurement on reference node */
#define REFN_AUTO                           REFN_INIT | ((uint32_t)ENUM_PHY_OA_TD_CTRL_TD_EN_ENABLED << BITP_PHY_OA_TD_CTRL_TD_EN) | ((uint32_t)ENUM_PHY_OA_TD_CTRL_TD_AUTO_START_TRIGGER << BITP_PHY_OA_TD_CTRL_TD_AUTO_START) /*!< Automatic measurement start on reference node */
/*================================ DATA TYPES ===============================*/

/*! Structure to store topology discovery data for remote node */
typedef struct TOPO_DISC_REM_DATA
{
    double nTopoRemDistance;       /*!< Stores distance between reference node and remote node */
    double nTopoRemDelay;          /*!< Stores delay measurement of remote node in nanosec */
    double nTopoRemDistanceInNS;   /*!< Stores the propagation delay in ns */
} TOPO_DISC_REM_DATA;

/*! Structure to store Topology Discovery data */
typedef struct TOPO_DISC_INTERNAL_DATA
{
    uint8_t  nTimeDuration;                                      /*!< Time Duration for which the measurement takes place in msec */
    bool     bTopoDistMeasComplete;                              /*!< Flag to indicate distance measurement is done */
    bool     bTopoDelayMeasComplete;                             /*!< Flag to indicate delay measurement is done */
    bool     bTopoDelayErrStatus;                                /*!< Flag to indicate error in delay measurement  */
    bool     bTopoDistErrStatus;                                 /*!< Flag to indicate error in distance measurement  */
    double   nTopoRefDelayMeas;                                  /*!< Stores Refernce node's delay measurement in nanosec */
    bool     bTopoAutoErr;                                       /*!< Flag to indicate error in automatic mode measurement */
    uint32_t nRemoteIdx;                                         /*!< Temporary variable to store the remote number */
    bool bWaitForRegRead;                                        /*!< Flag for confirming register read */
    TOPO_DISC_REM_DATA aoTopoRemData[ADI_EAL_MAX_NUM_NODES];     /*!< Array of measurement for remote nodes */
} TOPO_DISC_INTERNAL_DATA;

/*=================================== DATA ==================================*/
/**
 * @brief Internal structure for topology discovery
 */
ADI_EAL_TOPO_DATA
static TOPO_DISC_INTERNAL_DATA gaoEalTopoDiscData[ADI_E2BCORE_MAX_INST];
/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS MeasDistance(uint32_t nInstNum, uint32_t nRemoteNum);
static ADI_EAL_STATUS RefNodeDelayMeas(uint32_t nInstNum);
static ADI_EAL_STATUS RemNodeMeasurements(uint32_t nInstNum);
static ADI_EAL_STATUS GetMeasurementResults(uint32_t nInstNum);
static ADI_EAL_STATUS ReadCtrlReg(uint32_t nInstNum, uint32_t nDevNum, uint8_t nLength);
static ADI_EAL_STATUS RemoteRegWrite(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nData,
                                     uint32_t nMask);
static ADI_EAL_STATUS RemoteRegRead(uint32_t nInstNum, uint32_t nRemoteNum);
static bool ExtractStatusBits(uint32_t nRegData);
static ADI_EAL_STATUS PerformAutoMeasurement(uint32_t nInstNum, uint32_t nDevNum, uint8_t nTimeDuration);
static void ExtractDelayAndDistMeasurement(ADI_NETWORK_EVT_READ_REG_DATA *poRegDat);
static void eal_network_cbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void *pData);
/*! \endcond */
/*=================================== CODE ==================================*/
/**
 * @brief Performs the topology discovery in manual mode
 *
 * @param [in]     nInstNum         Instance number
 * @param [in]     nTimeDuration    Duration for the which measurement should take place (in ms)
 * @param [out]    panTopoDiscovery Pointer to the array for discovered propagation delays between the measured and reference nodes (has to be passed by application)
 * @return         ADI_EAL_STATUS
 *                 - #ADI_EAL_STATUS_SUCCESS        Topology discovery success
 *                 - #ADI_EAL_STATUS_NTW_BUFF_FULL  Network buffer full
 */
ADI_EAL_TOPO_CODE
ADI_EAL_STATUS adi_eal_topoDiscDistanceMeasure(uint32_t nInstNum, uint8_t nTimeDuration,
        double *panTopoDiscovery)
{
    // Pre-conditions:
    //  * Validate the instance number
    //  * Validate the instance is in-use
    //  * Validate the time duration for measurement is less than or equal to 15

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use to terminate");
    ADI_DBG_REQUIRE(nTimeDuration < 16u, "Measurement duration is above 15ms");

    gaoEalTopoDiscData[nInstNum].nTimeDuration = nTimeDuration;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        // Set TD_EN in all the remote nodes
        eEalStatus = RemoteRegWrite(nInstNum, ADI_EAL_ALL_REMOTES,
                                    (uint32_t)ENUM_PHY_OA_TD_CTRL_TD_EN_ENABLED << BITP_PHY_OA_TD_CTRL_TD_EN, BITM_PHY_OA_TD_CTRL_TD_EN);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Internal delay measurement for reference node
        eEalStatus = RefNodeDelayMeas(nInstNum);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Internal delay and distance measurement for remote nodes
        eEalStatus = RemNodeMeasurements(nInstNum);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Clearing TD_EN on all remote nodes
        eEalStatus = RemoteRegWrite(nInstNum, ADI_EAL_ALL_REMOTES, PHY_OA_TD_CTRL_RESET_VAL, PHY_OA_TD_CTRL_VOL_MASK);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Read remote node internal delay measurement results
        eEalStatus = GetMeasurementResults(nInstNum);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Give back the result to application
        for(uint8_t i = 0; i < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes; i++)
        {
            uint32_t nRemoteNum = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[i].oId.nRemoteNum;
            panTopoDiscovery[nRemoteNum] =
                gaoEalTopoDiscData[nInstNum].aoTopoRemData[nRemoteNum].nTopoRemDistanceInNS;
        }

    }

    while(false);

    return eEalStatus;
}

/**
 * @brief Performs topology discovery in automatic mode
 *
 * @param [in]  nInstNum         Instance number
 * @param [in]  nTimeDuration    Duration for the which measurement should take place
 * @param [out] panTopoDiscovery Pointer to the array for discovered propagation delays between the measured and reference nodes (has to be passed by application)
 * @return      ADI_EAL_STATUS
 *              - #ADI_EAL_STATUS_SUCCESS        Topology discovery success
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL  Network buffer full
 */
ADI_EAL_TOPO_CODE
ADI_EAL_STATUS adi_eal_topoDiscAutoDistMeasure(uint32_t nInstNum, uint8_t nTimeDuration,
        double *panTopoDiscovery)
{
    // Pre-conditions:
    //  * Validate the instance number
    //  * Validate the instance is in-use
    //  * Validate the time duration for measurement is less than or equal to 15

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use to terminate");
    ADI_DBG_REQUIRE(nTimeDuration < 16u, "Measurement duration is above 15ms");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
    gaoEalTopoDiscData[nInstNum].nTimeDuration = nTimeDuration;

    do
    {
        // Set TD_EN in all the remote nodes
        eEalStatus = RemoteRegWrite(nInstNum, ADI_EAL_ALL_REMOTES,
                                    (uint32_t)ENUM_PHY_OA_TD_CTRL_TD_EN_ENABLED << BITP_PHY_OA_TD_CTRL_TD_EN, BITM_PHY_OA_TD_CTRL_TD_EN);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Perform automatic measurement on all nodes
        eEalStatus = PerformAutoMeasurement(nInstNum, nDevNum, nTimeDuration);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Clears TD_EN on remote nodes
        eEalStatus = RemoteRegWrite(nInstNum, ADI_EAL_ALL_REMOTES, PHY_OA_TD_CTRL_RESET_VAL, PHY_OA_TD_CTRL_VOL_MASK);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Read remote node internal delay measurement results
        eEalStatus = GetMeasurementResults(nInstNum);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Give back the result to application
        for(uint8_t i = 0; i < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes; i++)
        {
            uint32_t nRemoteNum = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[i].oId.nRemoteNum;
            panTopoDiscovery[nRemoteNum] =
                gaoEalTopoDiscData[nInstNum].aoTopoRemData[nRemoteNum].nTopoRemDistanceInNS;

        }

    } while(false);

    return eEalStatus;
}

/*=================================== STATIC ================================*/
/*! \cond PRIVATE */
/* Performs internal delay measurement for reference node */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS RefNodeDelayMeas(uint32_t nInstNum)
{
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        // Start the internal delay measurement
        uint32_t nRegData = REFN_DLYM | ((uint32_t)gaoEalTopoDiscData[nInstNum].nTimeDuration << BITP_PHY_OA_TD_CTRL_TD_DM_DUR);
        if(adi_network_writeControllerRegister(nDevNum, PHY_OA_TD_CTRL_ADDR, 1U, PHY_OA_TD_CTRL_MMS, &nRegData,
                &eal_network_cbk) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }
        adi_network_flushTxBuffer(nDevNum, 0ULL);

        // Wait for the expected measurement time - adding +1 as time duration can also take value of 0
        uint64_t nWaitingTime = ((uint64_t)gaoEalTopoDiscData[nInstNum].nTimeDuration + 1u) * MEAS_TIME;
        adi_pal_waitMicroSec(nWaitingTime);

        // Read the status bits for reference mode to verify DYLM_DONE and DYLM_ERR
        eEalStatus = ReadCtrlReg(nInstNum, nDevNum, 1u);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(gaoEalTopoDiscData[nInstNum].bTopoDelayErrStatus && !gaoEalTopoDiscData[nInstNum].bTopoDelayMeasComplete)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_DELAY_MEAS_FAILED;
            break;
        }

        // Clearing the TD_EN on reference node
        nRegData = REFN_INIT;
        if(adi_network_writeControllerRegister(nDevNum, PHY_OA_TD_CTRL_ADDR, 1U, PHY_OA_TD_CTRL_MMS, &nRegData,
                &eal_network_cbk) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
        }
        adi_network_flushTxBuffer(nDevNum, 0ULL);
    } while(false);

    return eEalStatus;
}

/* Performs internal delay and distance measurements for remote nodes */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS RemNodeMeasurements(uint32_t nInstNum)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    for(uint8_t i = 0; i < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes; i++)
    {
        uint32_t nRemoteNum = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[i].oId.nRemoteNum;

        // Delay measurement for the remote node
        eEalStatus = RemoteRegWrite(nInstNum, nRemoteNum,
                                    ((uint32_t)gaoEalTopoDiscData[nInstNum].nTimeDuration << BITP_PHY_OA_TD_CTRL_TD_DM_DUR) | ((uint32_t)1U <<
                                        BITP_PHY_OA_TD_CTRL_TD_DLYM_START),
                                    (BITM_PHY_OA_TD_CTRL_TD_DM_DUR | BITM_PHY_OA_TD_CTRL_TD_DLYM_START));

        if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            // Wait for the expected measurement time - adding +1 as time duration can also take value of 0
            uint64_t nWaitingTime = ((uint64_t)gaoEalTopoDiscData[nInstNum].nTimeDuration + 1u) * MEAS_TIME;
            adi_pal_waitMicroSec(nWaitingTime);
        }
        else
        {
            break;
        }

        gaoEalTopoDiscData[nInstNum].nRemoteIdx = nRemoteNum;

        // Distance measurement between reference node and remote node
        eEalStatus = MeasDistance(nInstNum, nRemoteNum);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }
    }

    return eEalStatus;

}

/* Read controller register */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS ReadCtrlReg(uint32_t nInstNum, uint32_t nDevNum, uint8_t nLength)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;

    // Set the wait for response flag
    gaoEalTopoDiscData[nInstNum].bWaitForRegRead  = true;

    /* Send the register read command */
    if(adi_network_readControllerRegister(nDevNum, PHY_OA_TD_STAT_ADDR, nLength, PHY_OA_TD_STAT_MMS,
            &eal_network_cbk) == ADI_NETWORK_OK)
    {
        /* Wait for the reg read response */
        eEalStatus = eal_utils_waitForTimeout(&gaoEalTopoDiscData[nInstNum].bWaitForRegRead,
                                    ADI_REG_READ_RESP_TIMEOUT_MS * MS_TO_NS);
    }
    else
    {
        /* Nothing to do */
    }

    // Clearing the wait for response flag
    gaoEalTopoDiscData[nInstNum].bWaitForRegRead = false;

    return eEalStatus;
}

/* Get the internal delay measurements from remote nodes */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS GetMeasurementResults(uint32_t nInstNum)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    for(uint8_t i = 0; i < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes; i++)
    {
        uint32_t nRemoteNum = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[i].oId.nRemoteNum;

        // Reading the internal delay measurement result of remote node
        eEalStatus = RemoteRegRead(nInstNum, nRemoteNum);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Check if delay measurement failed
        if(gaoEalTopoDiscData[nInstNum].bTopoDelayErrStatus && !gaoEalTopoDiscData[nInstNum].bTopoDelayMeasComplete)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_DELAY_MEAS_FAILED;
            break;
        }

        // Check if distance measurement failed
        else if(gaoEalTopoDiscData[nInstNum].bTopoDistErrStatus && !gaoEalTopoDiscData[nInstNum].bTopoDistMeasComplete)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_DIST_MEAS_FAILED;
            break;
        }

        // Check if automatic measurement failed
        else if(gaoEalTopoDiscData[nInstNum].bTopoAutoErr)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_AUTO_MEAS_FAILED;
            break;
        }

        else
        {
            //Do Nothing
        }

        // Calculation of distances after reading the measurement results
        if(!gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyTopoDisc)
        {
            gaoEalTopoDiscData[nInstNum].aoTopoRemData[nRemoteNum].nTopoRemDistanceInNS = (0.5f
                * (gaoEalTopoDiscData[nInstNum].aoTopoRemData[nRemoteNum].nTopoRemDistance -
                   gaoEalTopoDiscData[nInstNum].nTopoRefDelayMeas -
                   gaoEalTopoDiscData[nInstNum].aoTopoRemData[nRemoteNum].nTopoRemDelay));

        }
    }

    return eEalStatus;
}

/* Read remote node register(s) */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS RemoteRegRead(uint32_t nInstNum, uint32_t nRemoteNum)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /************************** Read the TD stats registers - contains delay, distance measurements and error status **************************/
    uint16_t nRegAddr = (uint16_t)PHY_OA_TD_STAT_NW_ADDR; /* Starting address of stats registers for topology discovery */

    // Configurations for the register map message
    static ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    uint32_t nRegVal[] = {0u, 0u, 0u, 0u, 0u};
    (void) ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG));
    oRegCfg.eType       = ADI_E2BCORE_BURST_READ,
    oRegCfg.nLength     = 5u,
    oRegCfg.anStartAddr = &nRegAddr,
    oRegCfg.pBuffer = &nRegVal[0u],

    // Set the notify flag to true, this will be set as false by EAL once the readback is received
    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyTopoDisc = true;

    // Send the register read transaction, done with timeout - essentially a synchronous call
    eEalStatus = adi_eal_readRegWithTimeout(nInstNum, nRemoteNum, &oRegCfg,
                                            ADI_REG_READ_RESP_TIMEOUT_MS * MS_TO_NS);

    return eEalStatus;
}

/* Write remote node register(s) */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS RemoteRegWrite(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nData,
                                     uint32_t nMask)
{
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void) ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG));
    // ~~~~~ Register map message configuration ~~~~~
    uint16_t nRegAddr = (uint16_t)PHY_OA_TD_CTRL_NW_ADDR;

    oRegCfg.eType   = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nRegAddr;
    oRegCfg.pBuffer = &nData;
    oRegCfg.anMask = &nMask;

    // Send the register write transaction
    eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        // Flush the buffer
        adi_network_flushTxBuffer(nDevNum, 0ULL);
    }

    /* To ensure packet is deliver to the remote */
    adi_network_flushTxTimed(50ULL * MS_TO_NS);

    return eEalStatus;
}

/* Performs distance measurement between the reference node and measured node */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS MeasDistance(uint32_t nInstNum, uint32_t nRemoteNum)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;

    do
    {
        // Start the measurement
        eEalStatus = RemoteRegWrite(nInstNum, nRemoteNum, (uint32_t)1U << BITP_PHY_OA_TD_CTRL_TD_DM_START,
                                    BITM_PHY_OA_TD_CTRL_TD_DM_START);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Set the time duration
        uint32_t nRegData = REFN_DM | ((uint32_t)gaoEalTopoDiscData[nInstNum].nTimeDuration <<
                                       BITP_PHY_OA_TD_CTRL_TD_DM_DUR);
        if(adi_network_writeControllerRegister(nDevNum, PHY_OA_TD_CTRL_ADDR, 1U, PHY_OA_TD_CTRL_MMS,
                &nRegData, &eal_network_cbk) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }
        adi_network_flushTxBuffer(nDevNum, 0ULL);

        // Wait for the expected measurement time - adding +1 as time duration can also take value of 0
        uint64_t nWaitingTime = ((uint64_t)gaoEalTopoDiscData[nInstNum].nTimeDuration + 1u) * MEAS_TIME;
        adi_pal_waitMicroSec(nWaitingTime);

        // Reading the Status bits and distance measurement
        eEalStatus = ReadCtrlReg(nInstNum, nDevNum, 5u);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        // Check if there is any status bits set
        if(gaoEalTopoDiscData[nInstNum].bTopoDistErrStatus && !gaoEalTopoDiscData[nInstNum].bTopoDistMeasComplete)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_DIST_MEAS_FAILED;
            break;
        }

        // Clearing the TD_EN on the reference node
        nRegData = REFN_INIT;
        if(adi_network_writeControllerRegister(nDevNum, PHY_OA_TD_CTRL_ADDR, 1U, PHY_OA_TD_CTRL_MMS, &nRegData,
                &eal_network_cbk) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
        }
        adi_network_flushTxBuffer(nDevNum, 0ULL);
    } while(false);

    return eEalStatus;
}

/*! Perform automatic measurement in all the remote nodes */
ADI_EAL_TOPO_CODE
static ADI_EAL_STATUS PerformAutoMeasurement(uint32_t nInstNum, uint32_t nDevNum, uint8_t nTimeDuration)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    for(uint8_t i = 0; i <  gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes; i++)
    {
        uint32_t nRemoteNum = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[i].oId.nRemoteNum;

        // Set automatic measurement in remote node
        eEalStatus =  RemoteRegWrite(nInstNum, nRemoteNum,
                                     (((uint32_t)1U << BITP_PHY_OA_TD_CTRL_TD_AUTO_START) | ((uint32_t)nTimeDuration << BITP_PHY_OA_TD_CTRL_TD_DM_DUR)),
                                     (BITM_PHY_OA_TD_CTRL_TD_AUTO_START | BITM_PHY_OA_TD_CTRL_TD_DM_DUR));

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        gaoEalTopoDiscData[nInstNum].nRemoteIdx = nRemoteNum;

        uint32_t nRegData = (REFN_AUTO | ((uint32_t)nTimeDuration << BITP_PHY_OA_TD_CTRL_TD_DM_DUR));
        // Set automatic measurement in reference node
        if(adi_network_writeControllerRegister(nDevNum, PHY_OA_TD_CTRL_ADDR, 1U, PHY_OA_TD_CTRL_MMS,
                &nRegData, &eal_network_cbk) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }
        adi_network_flushTxBuffer(nDevNum, 0ULL);

        // Wait for the expected measurement time - adding +1 as time duration can also take value of 0
        uint64_t nWaitingTime = ((uint64_t)gaoEalTopoDiscData[nInstNum].nTimeDuration + 1u) * MEAS_TIME;
        adi_pal_waitMicroSec(nWaitingTime);

        // Read the status bits for reference node to verify DYLM_DONE and DYLM_ERR
        eEalStatus = ReadCtrlReg(nInstNum, nDevNum, 5u);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(gaoEalTopoDiscData[nInstNum].bTopoDelayErrStatus && !gaoEalTopoDiscData[nInstNum].bTopoDelayMeasComplete)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_DELAY_MEAS_FAILED;
            break;
        }
        else if(gaoEalTopoDiscData[nInstNum].bTopoDistErrStatus && !gaoEalTopoDiscData[nInstNum].bTopoDistMeasComplete)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_DIST_MEAS_FAILED;
            break;
        }
        else if(gaoEalTopoDiscData[nInstNum].bTopoAutoErr)
        {
            eEalStatus = ADI_EAL_STATUS_TOPO_DISC_AUTO_MEAS_FAILED;
            break;
        }
        else
        {
            // Nothing to do
        }

        // Clearing the TD_EN on reference node
        nRegData = REFN_INIT;
        if(adi_network_writeControllerRegister(nDevNum, PHY_OA_TD_CTRL_ADDR, 1U, PHY_OA_TD_CTRL_MMS,
                &nRegData, &eal_network_cbk) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
        }
        adi_network_flushTxBuffer(nDevNum, 0ULL);
    }

    return eEalStatus;
}

/* Extract status bits from the register */
ADI_EAL_TOPO_CODE
static bool ExtractStatusBits(uint32_t nRegData)
{
    bool bErrored = false;

    // Topology delay measurement complete
    gaoEalTopoDiscData[0U].bTopoDelayMeasComplete =
        (((uint8_t)((BITM_PHY_OA_TD_STAT_TD_DLYM_DONE & nRegData) >> BITP_PHY_OA_TD_STAT_TD_DLYM_DONE)) == 1U);
    // Topology delay measurement error status
    gaoEalTopoDiscData[0U].bTopoDelayErrStatus =
        (((uint8_t)((BITM_PHY_OA_TD_STAT_TD_DLYM_ERR & nRegData) >> BITP_PHY_OA_TD_STAT_TD_DLYM_ERR)) == 1U);
    // Topology distance measurement complete
    gaoEalTopoDiscData[0U].bTopoDistMeasComplete =
        (((uint8_t)((BITM_PHY_OA_TD_STAT_TD_DM_DONE & nRegData) >> BITP_PHY_OA_TD_STAT_TD_DM_DONE)) == 1U);
    // Topology distance measurement error status
    gaoEalTopoDiscData[0U].bTopoDistErrStatus =
        (((uint8_t)((BITM_PHY_OA_TD_STAT_TD_DM_ERR & nRegData) >> BITP_PHY_OA_TD_STAT_TD_DM_ERR)) == 1U);
    // Automatic topology distance error status
    gaoEalTopoDiscData[0U].bTopoAutoErr =
        (((uint8_t)((BITM_PHY_OA_TD_STAT_TD_AUTO_ERR & nRegData) >> BITP_PHY_OA_TD_STAT_TD_AUTO_ERR)) == 1U);

    if(gaoEalTopoDiscData[0U].bTopoDelayErrStatus ||
            gaoEalTopoDiscData[0U].bTopoDistErrStatus ||
            gaoEalTopoDiscData[0U].bTopoAutoErr)
    {
        bErrored = true;
    }

    return bErrored;
}

/* Extract delay and distance measurement from register data */
ADI_EAL_TOPO_CODE
static void ExtractDelayAndDistMeasurement(ADI_NETWORK_EVT_READ_REG_DATA *poRegDat)
{
    ADI_DBG_ENSURE(poRegDat->nLength == 5U, "Incorrect assumption!");
    ADI_DBG_ENSURE(poRegDat->nStartAddr == PHY_OA_TD_STAT_ADDR, "Incorrect assumption!");

    // Calculate the delay
    uint32_t nDelayResLow = BITM_PHY_OA_TD_DLY_RES_LOW_TD_DLYM_MR & poRegDat->pRegData[3U];
    uint32_t nDelayResUp = BITM_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR & poRegDat->pRegData[4U];
    uint32_t nRefDelay = (nDelayResUp << BITL_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR) | nDelayResLow;

    // Converting delay to nanosec
    gaoEalTopoDiscData[0U].nTopoRefDelayMeas = (((double)gaoEalTopoDiscData[0U].nTimeDuration + 1.0f) *
        (double)MS_TO_NS) / (double)nRefDelay ;

    // Calculate the distance
    uint32_t nDistResLow = BITM_PHY_OA_TD_DIST_RES_LOW_TD_DIST_MR & poRegDat->pRegData[1U];
    uint32_t nDistResUp = BITM_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR & poRegDat->pRegData[2U];
    uint32_t nDistance = (nDistResUp << BITL_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR) | nDistResLow;

    // Convert distance to nano sec
    gaoEalTopoDiscData[0U].aoTopoRemData[gaoEalTopoDiscData[0U].nRemoteIdx].nTopoRemDistance =
        (((double)gaoEalTopoDiscData[0U].nTimeDuration + 1.0f) * (double)MS_TO_NS) / (double)nDistance;
}

/* Callback for controller write/read registers */
ADI_EAL_TOPO_CODE
static void eal_network_cbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void *pData)
{
    ADI_DBG_ENSURE(eEvent == ADI_NETWORK_EVT_READ_REG || eEvent == ADI_NETWORK_EVT_WRITE_REG, "Network Error");

    if(eEvent == ADI_NETWORK_EVT_READ_REG)
    {
        /* Clear the waiting for register read flag */
        gaoEalTopoDiscData[0u].bWaitForRegRead  = false;

        /* Process the register read data */
        ADI_NETWORK_EVT_READ_REG_DATA *poRegDat = (ADI_NETWORK_EVT_READ_REG_DATA *)pData;
        ADI_DBG_ENSURE(poRegDat->nLength >= 1U, "Invalid assumption");

        bool bErrored = ExtractStatusBits(poRegDat->pRegData[0U]);

        if(poRegDat->nLength == 5U && !bErrored)
        {
            ExtractDelayAndDistMeasurement(poRegDat);
        }
    }
}

/* EAL notification for remote node response */
ADI_EAL_TOPO_CODE
void eal_notify_TopoDiscRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
    uint32_t nDelayResLow = 0U;
    uint32_t nDelayResUp  = 0U;

    for(uint8_t i = 0u; i < poRegDat->poInfo->nLength; i++)
    {
        // Status bits
        if(*(poRegDat->poInfo->anStartAddr) + i == PHY_OA_TD_STAT_NW_ADDR)
        {
            if(ExtractStatusBits(poRegDat->poInfo->pBuffer[i]))
            {
                break;
            }
        }

        // Delay result in number of pulses received - lower word
        if(*(poRegDat->poInfo->anStartAddr) + i == PHY_OA_TD_DLY_RES_LOW_NW_ADDR)
        {
            nDelayResLow = poRegDat->poInfo->pBuffer[i] & BITM_PHY_OA_TD_DLY_RES_LOW_TD_DLYM_MR;
        }

        // Delay result in number of pulses received - upper word
        if(*(poRegDat->poInfo->anStartAddr) + i == PHY_OA_TD_DLY_RES_UP_NW_ADDR)
        {
            nDelayResUp =  poRegDat->poInfo->pBuffer[i] & BITM_PHY_OA_TD_DLY_RES_UP_TD_DLYM_MR;
            uint32_t nRemDelay = (nDelayResUp << BITL_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR) | nDelayResLow;
            // Converting into nanosec
            gaoEalTopoDiscData[nInstNum].aoTopoRemData[nRemoteNum].nTopoRemDelay = (((double)gaoEalTopoDiscData[0U].nTimeDuration +
                1.0f) * (double)MS_TO_NS) / (double)nRemDelay;
        }

    }

    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyTopoDisc = false;
}


#endif /* ADI_E2B_IFACE_TOPDISC_ENABLED */
/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
