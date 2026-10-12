/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_ssc.c
 * @brief: The EAL APIs for Safe State Controller - to configure safe state controller, force safe state , return to functional mode manually , checks the safe state controller status and alive counter readback
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_SSC_ENABLED

/*! \cond PRIVATE */
/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS RemoteRegRead(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nAddr, uint16_t nLength);
static void InvokeCbk(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SSC_CBK_EVT eEvt, void *poParam);
static void SetSAIFConfigurations(uint16_t *panAddr, uint32_t *panMask, uint32_t *panData,
                                  ADI_EAL_SSC_MONITOR_CONFIG *poSscSetupCfg);

/*================================= DEFINES ================================*/

/*================================= DATA TYPES ================================*/
/**
 * @brief Data structure to store remote data
 *
 */
typedef struct ADI_EAL_SSC_REMOTE_DATA
{
    bool bSafeMode;                /*!< Indicates if the node is in safe state */
    bool bReturntoFuncFailed;      /*!< Indicates if the node has failed to return to functional state */
} ADI_EAL_SSC_REMOTE_DATA;

/**
 * @brief Data structure for safe state controller
 */
typedef struct ADI_EAL_SSC_DATA
{
    ADI_EAL_SSC_CFG *poSscCfg;      /*!< SSC configuration data */
    ADI_EAL_SSC_REMOTE_DATA aoEalRemoteSscData[ADI_EAL_MAX_NUM_NODES]; /*!< SSC data of remote nodes */
} ADI_EAL_SSC_DATA;

ADI_SSC_DATA
static ADI_EAL_SSC_DATA gaoEalsscData[ADI_E2BCORE_MAX_INST];

/*! \endcond */
/*=================================== CODE ==================================*/
/**
 * @brief Initializes the config
 *
 * @param [in] nInstNum Instance number
 * @param [in] poSscCfg Pointer to config structure
 * @return     eEalStatus
 *             - #ADI_EAL_STATUS_SUCCESS            API successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL      Network buffer full
 *             - #ADI_EAL_STATUS_START_FAILED       Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_REGMAPIO_FAILED    Failed to add the register map message
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SSC_CODE
ADI_EAL_STATUS adi_eal_sscInit(uint32_t nInstNum, ADI_EAL_SSC_CFG *poSscCfg)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(poSscCfg != NULL, " Safe State Controller config param is NULL ");
    ADI_DBG_REQUIRE(poSscCfg->pfSscCallback != NULL, "Safe State Controlle callback function pointer is NULL");

    // Clear the instance memory
    (void) ADI_MEMSET(&gaoEalsscData[nInstNum], 0, sizeof(ADI_EAL_SSC_DATA));

    // Save the configurations
    gaoEalsscData[nInstNum].poSscCfg = poSscCfg;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Writes to the device to make sure it is not unattended, if the write is not received with in the timeout , it triggers the safe mode
 *
 * @param [in] nInstNum         Instance Number
 * @param [in] nRemoteNum       Remote Number
 * @return     eEalStatus
 *             - #ADI_EAL_STATUS_SUCCESS           API successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *             - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SSC_CODE
ADI_EAL_STATUS adi_eal_sscRun(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Set the device unattended check bit */
    uint16_t nAddr = IO_SSC_MISC_NW_ADDR;
    uint32_t nMask = BITM_IO_SSC_MISC_SSC_DEVICE_UNNATTENDED_CHECK;
    uint32_t nData = BITM_IO_SSC_MISC_SSC_DEVICE_UNNATTENDED_CHECK;

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Send the register write transaction */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief Sets the configuration required for the safe mode, which are enabling the monitoring of pins, polarity, configuration of SA_IF pins in safe mode, HW_ADC mux and configurations and auto return settings
 *
 * @param [in] nInstNum           Instance Number
 * @param [in] nRemoteNum         Remote Number
 * @param [in] poSscSetupCfg      The struture that contains the configurations
 * @return     eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SSC_CODE
ADI_EAL_STATUS adi_eal_sscSetConfig(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SSC_MONITOR_CONFIG *poSscSetupCfg)
{
    gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bSafeMode = false;
    gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bReturntoFuncFailed = false;

    uint16_t anAddr[15U];
    uint32_t anMask[15U];
    uint32_t anData[15U];

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 15U;
    oRegCfg.anStartAddr = &anAddr[0U];
    oRegCfg.anMask = &anMask[0U];
    oRegCfg.pBuffer = &anData[0U];

    /* Enabling the interrupts */
    anAddr[0U] = IO_TIC_ENABLE_34_GPIO1_REG0_NW_ADDR;
    anMask[0U] = IO_TIC_ENABLE_34_GPIO1_REG0_VOL_MASK;
    anData[0U] = SSC_MODE_CHANGE_FLAG_RISING_IRQ_TIC_FIELD_MASK |
                 SSC_MODE_CHANGE_FLAG_FALLING_IRQ_TIC_FIELD_MASK |
                 SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_TIC_FIELD_MASK;

    /* Setting monitor polarity */
    anAddr[1U] = IO_SSC_MONITOR_POLARITY_NW_ADDR;
    anMask[1U] = BITM_IO_SSC_MONITOR_POLARITY_SSC_MONITOR_POLARITY;
    anData[1U] = poSscSetupCfg->nMonitorPolarity;
    anAddr[2U] = IO_SSC_MONITOR_POLARITY_2_NW_ADDR;
    anMask[2U] = BITM_IO_SSC_MONITOR_POLARITY_2_SSC_MONITOR_POLARITY;
    anData[2U] = poSscSetupCfg->nMonitorPolarity >> 16U;

    /* General settings */
    anAddr[3U] = IO_SSC_CONTROLS_NW_ADDR;
    anMask[3U] = BITM_IO_SSC_CONTROLS_SSC_SAFE_MODE_ENABLE |
                 BITM_IO_SSC_CONTROLS_SSC_SQI_MON_YELLOW_RED |
                 BITM_IO_SSC_CONTROLS_SSC_DISABLE_HOIF_ON_SAFE_MODE |
                 BITM_IO_SSC_CONTROLS_SSC_BEACON_TIMEOUT_CHECK;
    anData[3U] = poSscSetupCfg->nSAIFSafeEn |
                 ((uint32_t)poSscSetupCfg->nSQISafeModeLvlTrigger << BITP_IO_SSC_CONTROLS_SSC_SQI_MON_YELLOW_RED) |
                 ((uint32_t)(poSscSetupCfg->bEnableHostInt == true ? 0U : 1U) <<
                  BITP_IO_SSC_CONTROLS_SSC_DISABLE_HOIF_ON_SAFE_MODE) |
                 ((uint32_t)(poSscSetupCfg->bDetectBeaconAsEthTraffic == true ? 1U : 0U) <<
                  BITP_IO_SSC_CONTROLS_SSC_BEACON_TIMEOUT_CHECK);

    /* Setting SA_IF configurations in safe mode */
    SetSAIFConfigurations(&anAddr[4U], &anMask[4U], &anData[4U], poSscSetupCfg);

    /* Setting the HW ADC configurations */
    anAddr[7U] = IO_SSC_HW_ADC_CONTROLS_NW_ADDR;
    anMask[7U] = IO_SSC_HW_ADC_CONTROLS_VOL_MASK;
    anData[7U] = ((uint32_t)poSscSetupCfg->nHwAdcThresholdFlag[1U] <<
                  BITP_IO_SSC_HW_ADC_CONTROLS_SSC_HW_ADC_MONITOR_GT_TH_1) |
                 ((uint32_t)poSscSetupCfg->eHwAdcConfig[1U] << BITP_IO_SSC_HW_ADC_CONTROLS_SSC_HW_ADC_MONITOR_MUX_1) |
                 ((uint32_t)poSscSetupCfg->nHwAdcThresholdFlag[0U] << BITP_IO_SSC_HW_ADC_CONTROLS_SSC_HW_ADC_MONITOR_GT_TH_0) |
                 ((uint32_t)poSscSetupCfg->eHwAdcConfig[0U]);
    anAddr[8U] = IO_SSC_HW_ADC_CONTROLS2_NW_ADDR;
    anMask[8U] = IO_SSC_HW_ADC_CONTROLS2_VOL_MASK;
    anData[8U] = ((uint32_t)poSscSetupCfg->nHwAdcThresholdFlag[3U] <<
                  BITP_IO_SSC_HW_ADC_CONTROLS2_SSC_HW_ADC_MONITOR_GT_TH_3) |
                 ((uint32_t)poSscSetupCfg->eHwAdcConfig[3U] << BITP_IO_SSC_HW_ADC_CONTROLS2_SSC_HW_ADC_MONITOR_MUX_3) |
                 ((uint32_t)poSscSetupCfg->nHwAdcThresholdFlag[2U] << BITP_IO_SSC_HW_ADC_CONTROLS2_SSC_HW_ADC_MONITOR_GT_TH_2) |
                 (uint32_t)(poSscSetupCfg->eHwAdcConfig[2U]);

    /* Setting Timeout for Ethernet Frame Check */
    anAddr[9U] = IO_SSC_TIMEOUT_0_TIME_NW_ADDR;
    anMask[9U] = BITM_IO_SSC_TIMEOUT_0_TIME_SSC_TIMEOUT_TIME_0;
    anData[9U] = poSscSetupCfg->nEthFrameCheckTimeout;

    /* Setting Timeout for Device unattended check */
    anAddr[10U] = IO_SSC_TIMEOUT_1_TIME_NW_ADDR;
    anMask[10U] = BITM_IO_SSC_TIMEOUT_1_TIME_SSC_TIMEOUT_TIME_1;
    anData[10U] = poSscSetupCfg->nDeviceUnattendedTimeout;

    /* Setting Auto Return option */
    anAddr[11U] = IO_SSC_AUTO_RETURN_TO_FUNCTIONAL_EN_NW_ADDR;
    anMask[11U] = IO_SSC_AUTO_RETURN_TO_FUNCTIONAL_EN_VOL_MASK;
    anData[11U] = poSscSetupCfg->nAutoReturnToFunc;
    anAddr[12U] = IO_SSC_AUTO_RETURN_TO_FUNCTIONAL_EN2_NW_ADDR;
    anMask[12U] = IO_SSC_AUTO_RETURN_TO_FUNCTIONAL_EN2_VOL_MASK;
    anData[12U] = (((poSscSetupCfg->nAutoReturnToFunc & ADI_SSC_GPTP_UNLOCKED) >> 20U) |
                   ((poSscSetupCfg->nAutoReturnToFunc & ADI_SSC_ETH_FRAME_CHECK) >> 29U));

    /* Setting Monitor Enable */
    anAddr[13U] = IO_SSC_SAFE_CHECKS_ENABLE_NW_ADDR;
    anMask[13U] = BITM_IO_SSC_SAFE_CHECKS_ENABLE_SSC_MONITOR_ENABLE;
    anData[13U] = poSscSetupCfg->nMonitorEnable;
    anAddr[14U] = IO_SSC_SAFE_CHECKS_ENABLE_2_NW_ADDR;
    anMask[14U] = BITM_IO_SSC_SAFE_CHECKS_ENABLE_2_SSC_MONITOR_ENABLE |
                  BITM_IO_SSC_SAFE_CHECKS_ENABLE_2_SSC_TIMEOUT_ENABLE_1 |
                  BITM_IO_SSC_SAFE_CHECKS_ENABLE_2_SSC_TIMEOUT_ENABLE_0;
    anData[14U] = poSscSetupCfg->nMonitorEnable >> 16U;

    /* Send the register write transaction */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief Forces the remote node to go into safe state by triggering manual monitored pin
 *
 * @param [in] nInstNum           Instance Number
 * @param [in] nRemoteNum         Remote Number
 * @param [in] bForceSafeState    If true, sets the node to safe mode
 * @return     eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SSC_CODE
ADI_EAL_STATUS adi_eal_sscForceSafeState(uint32_t nInstNum, uint32_t nRemoteNum, bool bForceSafeState)
{
    /* Force the remote node to go into safe state */
    uint16_t nAddr = IO_SSC_MONITOR_TRIGGERS_NW_ADDR;
    uint32_t nMask = BITM_IO_SSC_MONITOR_TRIGGERS_SSC_MANUAL_ENTER_SAFE_MODE;
    uint32_t nData = ((uint32_t)(bForceSafeState ? 1U : 0U)) << BITP_IO_SSC_MONITOR_TRIGGERS_SSC_MANUAL_ENTER_SAFE_MODE;

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Send the register write transaction */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief Readbacks the triggers that set the node to safe mode
 *
 * @param [in] nInstNum           Instance Number
 * @param [in] nRemoteNum         Remote Number
 * @return     eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SSC_CODE
ADI_EAL_STATUS adi_eal_sscCheckSafeModeStatus(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Check the safe mode status */
    ADI_EAL_STATUS eEalStatus = RemoteRegRead(nInstNum, nRemoteNum, IO_SSC_SAFE_CHECKS_TRIGGER_NW_ADDR, 3U);
    return eEalStatus;
}

/**
 * @brief Manual return to functional mode from safe mode
 *
 * @param [in] nInstNum            Instance Number
 * @param [in] nRemoteNum          Remote Number
 * @return     eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SSC_CODE
ADI_EAL_STATUS adi_eal_sscReturnToFunctional(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Manual return to functional mode from safe mode */
    uint16_t nAddr = IO_SSC_CONTROLS_NW_ADDR;
    uint32_t nMask = BITM_IO_SSC_CONTROLS_SSC_RETURN_TO_FUNCTIONAL_MODE;
    uint32_t nData = ((uint32_t)1U << BITP_IO_SSC_CONTROLS_SSC_RETURN_TO_FUNCTIONAL_MODE);

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Send the register write transaction */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief Alive Counter Readback
 *
 * @param [in] nInstNum         Instance Number
 * @param [in] nRemoteNum       Remote Number
 * @return     eEalStatus
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_SSC_CODE
ADI_EAL_STATUS adi_eal_sscAliveCounterRead(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Check the alive counter */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    eEalStatus = RemoteRegRead(nInstNum, nRemoteNum, IO_SSC_ALIVE_COUNTER_RB_NW_ADDR, 1U);
    return eEalStatus;
}

// --------------------- PRIVATE CODE ---------------------
// Remote node register read
ADI_EAL_SSC_CODE
static ADI_EAL_STATUS RemoteRegRead(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nAddr, uint16_t nLength)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    // Network frame configurations
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    // E2B message options
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox    = false;
    oMsgOpts.bSync       = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    // Read the MAC address register as a ping test
    uint16_t anRegAddr = (uint16_t)nAddr;

    // Configurations for the register map message
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void) ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG));
    oRegCfg.eType       = ADI_E2BCORE_BURST_READ,
    oRegCfg.nLength     = (uint8_t)nLength,
    oRegCfg.anStartAddr = &anRegAddr,

    // Set the notify flag
    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifySsc = true;

    // Send the register message
    eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);

    if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifySsc = false;
    }

    return eEalStatus;
}

/* Invoke the callback */
ADI_EAL_SSC_CODE
static void InvokeCbk(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SSC_CBK_EVT eEvt, void *poParam)
{
    /* Get the SSC configuration */
    ADI_EAL_SSC_CFG *poSscCfg = gaoEalsscData[nInstNum].poSscCfg;

    /* Get the callback function */
    ADI_EAL_SSC_CALLBACK_FUNC pfEalSscCbk = poSscCfg->pfSscCallback;

    /* Invoke the callback */
    if(pfEalSscCbk != NULL)
    {
        pfEalSscCbk(nInstNum, nRemoteNum, eEvt, poParam);
    }
}

/* Set the SAIF configurations */
ADI_EAL_SSC_CODE
static void SetSAIFConfigurations(uint16_t *panAddr, uint32_t *panMask, uint32_t *panData,
                                  ADI_EAL_SSC_MONITOR_CONFIG *poSscSetupCfg)
{
    /* Set the SA-IF[4:0] configurations */
    panAddr[0U] = IO_SSC_SAFE_SA_IF_CONTROLS_NW_ADDR;
    panMask[0U] = IO_SSC_SAFE_SA_IF_CONTROLS_VOL_MASK;
    panData[0U] = (poSscSetupCfg->nSAIFConfig[4U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_4_CONTROLS_MUX) |
                  (poSscSetupCfg->nSAIFConfig[3U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_3_CONTROLS_MUX) |
                  (poSscSetupCfg->nSAIFConfig[2U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_2_CONTROLS_MUX) |
                  (poSscSetupCfg->nSAIFConfig[1U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_1_CONTROLS_MUX) |
                  poSscSetupCfg->nSAIFConfig[0U];
    /* Set the SA-IF[9:5] configurations */
    panAddr[1U] = IO_SSC_SAFE_SA_IF_CONTROLS_2_NW_ADDR;
    panMask[1U] = IO_SSC_SAFE_SA_IF_CONTROLS_2_VOL_MASK;
    panData[1U] = (poSscSetupCfg->nSAIFConfig[9U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_9_CONTROLS_MUX) |
                  (poSscSetupCfg->nSAIFConfig[8U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_8_CONTROLS_MUX) |
                  (poSscSetupCfg->nSAIFConfig[7U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_7_CONTROLS_MUX) |
                  (poSscSetupCfg->nSAIFConfig[6U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_6_CONTROLS_MUX) |
                  poSscSetupCfg->nSAIFConfig[5U];
    /* Set the SA-IF[11:10] configurations */
    panAddr[2U] = IO_SSC_SAFE_SA_IF_CONTROLS_3_NW_ADDR;
    panMask[2U] = IO_SSC_SAFE_SA_IF_CONTROLS_3_VOL_MASK;
    panData[2U] = (poSscSetupCfg->nSAIFConfig[11U] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_3_SSC_SAFE_SA_IF_11_CONTROLS_MUX) |
                  poSscSetupCfg->nSAIFConfig[10U];
}

// ------------------------ EVENTS ------------------------
/**
 * @brief       This function allows the EAL top component to notify the
 *              SSC component of a register read event
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  poRegDat            Pointer to register read data
 */
ADI_EAL_SSC_CODE
void eal_notify_sscRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
    ADI_EAL_SSC_CFG *poSscCfg = gaoEalsscData[nInstNum].poSscCfg;

    if(poSscCfg != NULL)
    {
        // Response for SSC Status read
        if(poRegDat->poInfo->anStartAddr[0U] == IO_SSC_SAFE_CHECKS_TRIGGER_NW_ADDR)

        {
            ADI_EAL_SSC_CBK_EVT_SSC_STATUS_DATA oSscStatusData;
            adi_common_extractSscStatus(poRegDat->poInfo->pBuffer, &oSscStatusData);
            // Give a callback to the application layer with the SSC status data
            InvokeCbk(nInstNum, nRemoteNum, ADI_EAL_SSC_CBK_EVT_STATUS, &oSscStatusData);

            // Clear all the sticky readback once the callback is sent to the appplication layer
            ADI_E2BCORE_REGMAP_IO_CONFIG oRegMapCfg;
            (void) ADI_MEMSET(&oRegMapCfg, 0, sizeof(oRegMapCfg));

            // ~~~~~ Register map message configuration ~~~~~
            uint16_t nRegAddr = (uint16_t)IO_SSC_MISC_NW_ADDR;
            uint32_t nRegData = 1U << BITP_IO_SSC_MISC_SSC_CLEAR_ALL_TRIGGERS;
            uint32_t nMask = BITM_IO_SSC_MISC_SSC_CLEAR_ALL_TRIGGERS;

            oRegMapCfg.eType   = ADI_E2BCORE_RND_MASK_WRITE;
            oRegMapCfg.nLength = 1U;
            oRegMapCfg.anStartAddr = &nRegAddr;
            oRegMapCfg.pBuffer = &nRegData;
            oRegMapCfg.anMask = &nMask;

            // Send the regmap transaction
            if(adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegMapCfg) != ADI_EAL_STATUS_SUCCESS)
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to send reg transaction in ssc handler");
            }
        }

        // Response for Alive Counter Readback
        if(poRegDat->poInfo->anStartAddr[0U] == IO_SSC_ALIVE_COUNTER_RB_NW_ADDR)
        {
            uint16_t AliveCntrRb = (uint16_t)poRegDat->poInfo->pBuffer[0U];
            // Give a callback to the application layer with the alive counter readback value
            InvokeCbk(nInstNum, nRemoteNum, ADI_EAL_SSC_CBK_EVT_ALIVE_COUNTER_RB, &AliveCntrRb);
        }

    }

    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifySsc = false;
}

/**
 * @brief       This function allows the EAL top component to notify the
 *              ssc component of an interrupt arrival
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  eSscIntEvt          SSC interrupt event
 */
ADI_EAL_SSC_CODE
bool eal_notify_SscInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, EAL_SSC_INTERRUPT_EVT eSscIntEvt)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    /* Get the SSC configuration */
    ADI_EAL_SSC_CFG *poSscCfg = gaoEalsscData[nInstNum].poSscCfg;

    bool bInterruptProcessed = false;

    if(poSscCfg != NULL)
    {
        /* Process based on interrupt event */
        switch(eSscIntEvt)
        {
            /* Mode change to safe event */
            case EAL_SSC_MODE_CHANGE_TO_SAFE_EVT:
            {
                if(!gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bSafeMode)
                {
                    gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bSafeMode = true;
                    InvokeCbk(nInstNum, nRemoteNum, ADI_EAL_SSC_CBK_EVT_SAFE_MODE, NULL);
                }

                break;
            }

            /* Unsuccessful return to func event */
            case EAL_SSC_UNSUCCESSFUL_RETURN_TO_FUNC_EVT:
            {
                if(!gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bReturntoFuncFailed)
                {
                    gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bReturntoFuncFailed = true;
                    InvokeCbk(nInstNum, nRemoteNum, ADI_EAL_SSC_CBK_EVT_UNSUCCESSFUL_RETURN_TO_FUNC, NULL);
                }

                break;
            }

            /* Mode change to functional event */
            case EAL_SSC_MODE_CHANGE_TO_FUNC_EVT:
            {
                if(gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bSafeMode)
                {
                    gaoEalsscData[nInstNum].aoEalRemoteSscData[nRemoteNum].bSafeMode = false;
                    InvokeCbk(nInstNum, nRemoteNum, ADI_EAL_SSC_CBK_EVT_RETURN_TO_FUNC_MODE, NULL);
                }

                break;
            }

            /* Unknown event */
            default:
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Unknown SSC interrupt");
                ADI_DBG_ERROR();
                break;
            }
        }

        bInterruptProcessed = true;
    }

    return bInterruptProcessed;
}


#endif /* ADI_E2B_IFACE_SSC_ENABLED */
/** @} */

/*
 * EOF: www.analog.com
*/
