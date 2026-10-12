/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_adc.c
 * @brief: The EAL APIs for ADC interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_ADC_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalAdcData) \
ADI_DBG_REQUIRE((poEalAdcData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalAdcData)->bInUse == true, "Instance is not opened");

/*=================================== DATA ==================================*/

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendAdcTransaction(EAL_ADC_DATA *poEalAdcData, ADI_E2BCORE_ADC_TXN_CONFIG *poTxnCfg);
static void extractAdcStatus(uint32_t *anData, ADI_EAL_ADC_STATUS_DATA *poStatusData);
static void LockAdcMutex(EAL_ADC_DATA *poEalAdcData);
static void UnlockAdcMutex(EAL_ADC_DATA *poEalAdcData);
static bool IsResponseExpected(EAL_ADC_INST_DATA *poEalAdcData, ADI_E2BCORE_ADC_TXN_CONFIG *poTxnCfg);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void InvokeCbk(EAL_ADC_DATA *poEalAdcData, EAL_ADC_INST_DATA *poEalAdcInstData, ADI_EAL_ADC_CBK_EVT eEvt,
                      const void *pArg);
static bool IsSpaceAvblInEalTxnQueue(EAL_ADC_DATA *poEalAdcData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_ADC_DATA *poEalAdcData);
static void CopyTransactionConfigToQueue(EAL_ADC_DATA *poEalAdcData, ADI_E2BCORE_ADC_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_ADC_DATA *poEalAdcData, bool bIsMcast, uint8_t **apRxBuf);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_ADC_DATA *poEalAdcData, EAL_ADC_INST_DATA *poEalAdcInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoAdcTxnCfg);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an ADC instance
 *
 * @param [in]  poAdcCfg        Configuration pointer for EAL ADC instance
 * @param [out] phAdcHandle     Returns the ADC handle for use in other EAL ADC APIs
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: ADC instance was successfully opened.
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcOpen(ADI_EAL_ADC_CFG *poAdcCfg, ADI_EAL_ADC_HANDLE *phAdcHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poAdcCfg != NULL, "Pointer to the ADC module configuration is NULL");
    ADI_DBG_REQUIRE(phAdcHandle != NULL, "Pointer to the ADC handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poAdcCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poAdcCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poAdcCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poAdcCfg->nInstNum;
    uint32_t nRemoteNum = poAdcCfg->nRemoteNum;
    uint32_t nIntfNum = poAdcCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_ADC,
                    "The given pair of instance, remote and interface numbers is not configured as ADC interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poAdcCfg->pEalAdcMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poAdcCfg->nEalAdcMemorySize >= ADI_EAL_ADC_UNICAST_INST_MEM_SIZE(poAdcCfg->nAdcCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poAdcCfg->pEalAdcMemory;

    /* Allocate memory for the ADC transaction config queue */
    ADI_E2BCORE_ADC_TXN_CONFIG *paoAdcTxnCfgs = (ADI_E2BCORE_ADC_TXN_CONFIG *)((void *)pMem);
    pMem += (poAdcCfg->nAdcCfgQDepth * sizeof(ADI_E2BCORE_ADC_TXN_CONFIG));

    /* Allocate memory for the EAL ADC instance data */
    EAL_ADC_INST_DATA *poEalAdcInstData = (EAL_ADC_INST_DATA *)((void *)pMem);
    pMem += sizeof(EAL_ADC_INST_DATA);
    /* ~~~~~~~~~~~ ADC instance info ~~~~~~~~~~~ */
    poEalAdcInstData->oId.nInstNum      = nInstNum;
    poEalAdcInstData->oId.nRemoteNum    = nRemoteNum;
    poEalAdcInstData->oId.nIntfNum      = nIntfNum;
    poEalAdcInstData->pnIfRegVlanTag    = &poIfConfig->nIfRegVlanTag;
    poEalAdcInstData->pnFifoRegVlanTag  = &poIfConfig->nFifoRegVlanTag;
    poEalAdcInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;
    /* ~~~~~~~~~~~ ADC state info ~~~~~~~~~~~ */
    poEalAdcInstData->bIgnoreErr        = false;
    poEalAdcInstData->eMode             = ADI_EAL_USAGE_MODE_NORMAL;
    poEalAdcInstData->pfunCbk           = NULL; /* Will be filled in adi_eal_adcRegisterCallback */
    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    uint32_t nAdcCtrlReg = 0U;
    EAL_ADC_INST_DATA **apoInstData;
    EAL_ADC_DATA *poEalAdcData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalAdcInstData->oId, ADC_CONTROL_NW_ADDR, &nAdcCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalAdcInstData->bIsReadTimeTagEn  = (nAdcCtrlReg & BITM_ADC_CONTROL_ENABLE_READ_TIME_TAG_ECCO) != 0U;
    poEalAdcInstData->nAdcCfgQDepth     = poAdcCfg->nAdcCfgQDepth;
    poEalAdcInstData->nAdcQWrIdx        = 0U;
    poEalAdcInstData->nAdcQRdIdx        = 0U;
    poEalAdcInstData->paoCfgs           = paoAdcTxnCfgs;
    poEalAdcInstData->oRxCfg.pPayload   = &poEalAdcInstData->oRxPayload;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalAdcInstData->oId, (void *)pMem,
            poAdcCfg->nAdcCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_ADC_TXN_CONFIG *) * poAdcCfg->nAdcCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_ADC_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_ADC_INST_DATA *);
        *apoInstData = poEalAdcInstData;

        /* Allocate memory for the internal EAL ADC data structure */
        poEalAdcData  = (EAL_ADC_DATA *)((void *)pMem);
        pMem += sizeof(EAL_ADC_DATA);
        poEalAdcData->bIsMulticast  = false;
        poEalAdcData->bInUse        = true;
        poEalAdcData->bMailboxMode  = false;
        poEalAdcData->bResyncPending = false;
        poEalAdcData->nIntfTopic    = 0U;
        poEalAdcData->nFifoTopic    = 0U;
        poEalAdcData->nNumInst      = 1U;
        poEalAdcData->apoInstData   = apoInstData;

        /* Initialise the mutex for this ADC instance */
        poEalAdcInstData->oAdcMutexCfg.pMem = &poEalAdcInstData->anAdcMutexBuffer[0U];
        poEalAdcInstData->oAdcMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalAdcInstData->oAdcMutexCfg, &poEalAdcInstData->hAdcMutex);

        /* Set the callback param in the e2bcore config */
        poIfConfig->pCbParam = poEalAdcData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_ADC;

        /* Set the handle to the EAL_ADC_DATA structure */
        *phAdcHandle = (ADI_EAL_ADC_HANDLE)poEalAdcData;

        /* Provide memory to e2bcore for the RX ADC transaction config (containing an ADC sample) */
        if(adi_e2bcore_addAdcConfigToQueue(&poEalAdcInstData->oId, true, &poEalAdcInstData->oRxCfg)
                != ADI_E2BCORE_ERR_NONE)
        {
            eStatus = ADI_EAL_STATUS_OPEN_FAILED;
        }
    }

    return eStatus;
}

/**
 * @brief       Closes an opened ADC instance
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: ADC device was successfully closed.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcClose(ADI_EAL_ADC_HANDLE hAdcHandle)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalAdcData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalAdcData->apoInstData[i]->hAdcMutex);
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalAdcData, 0, sizeof(EAL_ADC_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Register a callback function for getting notified of ADC events
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @param [in]  pfAdcCallback   Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Callback was registered successfully.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcRegisterCallback(ADI_EAL_ADC_HANDLE hAdcHandle,
        ADI_EAL_ADC_CALLBACK_FUNC pfAdcCallback)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);
    ADI_DBG_REQUIRE(pfAdcCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalAdcData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* Register the callback function */
    poEalAdcInstData->pfunCbk = pfAdcCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Create a multicast group of ADC interfaces
 *
 * @param [in]  poAdcMcastCfg       Configuration for ADC multicast group
 * @param [out] phAdcMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcCreateMcastGroup(ADI_EAL_ADC_MCAST_CFG *poAdcMcastCfg,
        ADI_EAL_ADC_HANDLE *phAdcMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poAdcMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phAdcMcastHandle != NULL, "Pointer to the ADC multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poAdcMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poAdcMcastCfg->pahAdcHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poAdcMcastCfg->pAdcMcastMem != NULL, "Pointer to memory for ADC multicast is NULL");
    ADI_DBG_REQUIRE(poAdcMcastCfg->nAdcMcastMemSize >= ADI_EAL_ADC_MCAST_GROUP_MEM_SIZE(poAdcMcastCfg->nNumInst),
                    "Not enough memory allocated for ADC multicast group");
    ADI_DBG_REQUIRE(poAdcMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poAdcMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poAdcMcastCfg->pAdcMcastMem;
    EAL_ADC_INST_DATA **apoInstData = (EAL_ADC_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_ADC_INST_DATA *) * poAdcMcastCfg->nNumInst);

    /* Save the unicast EAL ADC instances */
    for(uint16_t i = 0U; i < poAdcMcastCfg->nNumInst; i++)
    {
        EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)poAdcMcastCfg->pahAdcHandles[i];
        apoInstData[i] = poEalAdcData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast topics */
        uint32_t nInstNum = poEalAdcData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalAdcData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalAdcData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poAdcMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poAdcMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poAdcMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poAdcMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_ADC_DATA *poEalAdcMcastData        = (EAL_ADC_DATA *)((void *)pMem);
    poEalAdcMcastData->bIsMulticast        = true;
    poEalAdcMcastData->bInUse              = true;
    poEalAdcMcastData->bMailboxMode        = false;
    poEalAdcMcastData->bResyncPending      = false;
    poEalAdcMcastData->nIntfTopic          = poAdcMcastCfg->nIntfTopic;
    poEalAdcMcastData->nFifoTopic          = poAdcMcastCfg->nFifoTopic;
    poEalAdcMcastData->nIfRegVlanTag       = (poAdcMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poAdcMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalAdcMcastData->nFifoRegVlanTag     = (poAdcMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poAdcMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalAdcMcastData->nFifoWriteVlanTag   = (poAdcMcastCfg->poFifoWriteVlanTag != NULL) ? eal_utils_convertVlanTags(
                poAdcMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalAdcMcastData->nNumInst            = poAdcMcastCfg->nNumInst;
    poEalAdcMcastData->apoInstData         = apoInstData;

    /* Set the handle to the EAL_ADC_DATA structure */
    *phAdcMcastHandle = (ADI_EAL_ADC_HANDLE)poEalAdcMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Configure ADC with round robin sampling mode
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @param [in]  poCfg           Configurations for ADC
 * @param [in]  nId             Identifier for configuration (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : ADC configuration request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : ADC command queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL : Network Buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the configure operation is received by the host interface
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcConfigureRoundRobinSampling(ADI_EAL_ADC_HANDLE hAdcHandle,
        ADI_EAL_ADC_ROUND_ROBIN_CFG *poCfg, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);
    ADI_DBG_REQUIRE(poCfg != NULL, "Configuration pointer is NULL");

    /* More ADC configuration checks */
    ADI_DBG_REQUIRE(poCfg->nCyclePeriod > poCfg->nClkPeriod, "Invalid clk / cycle period");

    /* Set the ADC transaction configuration */
    ADI_E2BCORE_ADC_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_ADC_TXN_TX_PAYLOAD aoTxPayload[7U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_ADC_TXN_CONFIG));

    /* Set the ADC transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 7U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the ADC transaction configuration payload */
    aoTxPayload[0U].eAdcCmd = ADI_E2BCORE_ADC_CMD_CLK_PERIOD;
    aoTxPayload[0U].nAdcClkPeriod = poCfg->nClkPeriod;
    aoTxPayload[1U].eAdcCmd = ADI_E2BCORE_ADC_CMD_CYCLE_PERIOD;
    aoTxPayload[1U].nAdcCyclePeriod = poCfg->nCyclePeriod;
    aoTxPayload[2U].eAdcCmd = ADI_E2BCORE_ADC_CMD_UPDATE;
    aoTxPayload[3U].eAdcCmd = ADI_E2BCORE_ADC_CMD_NUM_SAMPLES;
    aoTxPayload[3U].nNumSamples = poCfg->nNumSamples - 1U; /* Command is 0 based */
    aoTxPayload[4U].eAdcCmd = ADI_E2BCORE_ADC_CMD_CH_EN;
    aoTxPayload[4U].nChannelEn = poCfg->nChannelEn;
    aoTxPayload[5U].eAdcCmd = ADI_E2BCORE_ADC_CMD_MODE;
    aoTxPayload[5U].bLutEn = false;
    aoTxPayload[6U].eAdcCmd = ADI_E2BCORE_ADC_CMD_MASK;
    aoTxPayload[6U].nChannelMask = poCfg->nChannelMask;

    /* Loop through the mutex of all instances and lock it */
    LockAdcMutex(poEalAdcData);

    /* Send the ADC commands */
    ADI_EAL_STATUS eEalStatus = SendAdcTransaction(poEalAdcData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockAdcMutex(poEalAdcData);

    return eEalStatus;
}

/**
 * @brief       Configure ADC based on Look Up Table sampling mode
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @param [in]  poCfg           Configurations for ADC
 * @param [in]  nId             Identifier for configuration (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : ADC configuration request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : ADC command queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL : Network buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the configure operation is received by the host interface
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcConfigureLUTSampling(ADI_EAL_ADC_HANDLE hAdcHandle,
        ADI_EAL_ADC_LUT_CFG *poCfg, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);
    ADI_DBG_REQUIRE(poCfg != NULL, "Configuration pointer is NULL");

    /* More ADC configuration checks */
    ADI_DBG_REQUIRE(poCfg->nCyclePeriod > poCfg->nClkPeriod, "Invalid clk / cycle period");

    /* Set the ADC transaction configuration */
    ADI_E2BCORE_ADC_TXN_CONFIG  oTxnCfg;
    ADI_E2BCORE_ADC_TXN_TX_PAYLOAD aoTxPayload[6U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_ADC_TXN_CONFIG));

    /* Set the ADC transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = 6U;
    oTxnCfg.pPayload                  = &aoTxPayload[0U];

    /* Set the ADC transaction configuration payload */
    aoTxPayload[0U].eAdcCmd = ADI_E2BCORE_ADC_CMD_CLK_PERIOD;
    aoTxPayload[0U].nAdcClkPeriod = poCfg->nClkPeriod;
    aoTxPayload[1U].eAdcCmd = ADI_E2BCORE_ADC_CMD_CYCLE_PERIOD;
    aoTxPayload[1U].nAdcCyclePeriod = poCfg->nCyclePeriod;
    aoTxPayload[2U].eAdcCmd = ADI_E2BCORE_ADC_CMD_UPDATE;
    aoTxPayload[3U].eAdcCmd = ADI_E2BCORE_ADC_CMD_MODE;
    aoTxPayload[3U].bLutEn = true;
    aoTxPayload[4U].eAdcCmd = ADI_E2BCORE_ADC_CMD_LUT;
    aoTxPayload[4U].nLutSize = poCfg->nLutSize;
    aoTxPayload[4U].panChannelIdx = poCfg->panChannelIdx;
    aoTxPayload[5U].eAdcCmd = ADI_E2BCORE_ADC_CMD_MASK;
    aoTxPayload[5U].nChannelMask = poCfg->nChannelMask;

    /* Loop through the mutex of all instances and lock it */
    LockAdcMutex(poEalAdcData);

    /* Send the ADC commands */
    ADI_EAL_STATUS eEalStatus = SendAdcTransaction(poEalAdcData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockAdcMutex(poEalAdcData);

    return eEalStatus;
}

/**
 * @brief       Start sampling the enabled channels
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @param [in]  nId             Identifier for starting sampling (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : ADC start sampling request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : ADC command queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the configure operation is received by the host interface
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcStartSampling(ADI_EAL_ADC_HANDLE hAdcHandle, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Set the ADC transaction configuration */
    ADI_E2BCORE_ADC_TXN_CONFIG  oTxnCfg;
    ADI_E2BCORE_ADC_TXN_TX_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_ADC_TXN_CONFIG));

    /* Set the ADC transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the ADC transaction configuration payload */
    aoTxPayload[0U].eAdcCmd = ADI_E2BCORE_ADC_CMD_START;

    /* Loop through the mutex of all instances and lock it */
    LockAdcMutex(poEalAdcData);

    /* Send the ADC commands */
    ADI_EAL_STATUS eEalStatus = SendAdcTransaction(poEalAdcData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockAdcMutex(poEalAdcData);

    return eEalStatus;
}

/**
 * @brief       Start sampling the enabled channels after a delay
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @param [in]  nDelayTime      Time in nanoseconds to delay the start of sampling
 * @param [in]  nId             Identifier for starting sampling (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : ADC start sampling request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : ADC command queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the configure operation is received by the host interface
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcStartSamplingDelayed(ADI_EAL_ADC_HANDLE hAdcHandle, uint32_t nDelayTime, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Set the ADC transaction configuration */
    ADI_E2BCORE_ADC_TXN_CONFIG  oTxnCfg;
    ADI_E2BCORE_ADC_TXN_TX_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_ADC_TXN_CONFIG));

    /* Set the ADC transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = true;
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns      = 1U;
    oTxnCfg.oTxnHdr.nTimeDelay          = nDelayTime;
    oTxnCfg.pPayload                    = &aoTxPayload[0U];

    /* Set the ADC transaction configuration payload */
    aoTxPayload[0U].eAdcCmd = ADI_E2BCORE_ADC_CMD_START;

    /* Loop through the mutex of all instances and lock it */
    LockAdcMutex(poEalAdcData);

    /* Send the ADC commands */
    ADI_EAL_STATUS eEalStatus = SendAdcTransaction(poEalAdcData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockAdcMutex(poEalAdcData);

    return eEalStatus;
}

/**
 * @brief       Start sampling the enabled channels at the given presentation time
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @param [in]  nStartTime      The gPTP time at which to start sampling
 * @param [in]  nId             Identifier for starting sampling (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : ADC start sampling request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : ADC command queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the configure operation is received by the host interface
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcStartSamplingAtTime(ADI_EAL_ADC_HANDLE hAdcHandle, uint64_t nStartTime, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Set the ADC transaction configuration */
    ADI_E2BCORE_ADC_TXN_CONFIG  oTxnCfg;
    ADI_E2BCORE_ADC_TXN_TX_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_ADC_TXN_CONFIG));

    /* Set the ADC transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bIsGptpTimePresent = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = 1U;
    oTxnCfg.oTxnHdr.nGptpTime         = nStartTime;
    oTxnCfg.pPayload                  = &aoTxPayload[0U];

    /* Set the ADC transaction configuration payload */
    aoTxPayload[0U].eAdcCmd = ADI_E2BCORE_ADC_CMD_START;

    /* Loop through the mutex of all instances and lock it */
    LockAdcMutex(poEalAdcData);

    /* Send the ADC commands */
    ADI_EAL_STATUS eEalStatus = SendAdcTransaction(poEalAdcData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockAdcMutex(poEalAdcData);

    return eEalStatus;
}

/**
 * @brief       Stop sampling the channels
 *
 * @param [in]  hAdcHandle      ADC instance handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : ADC start sampling request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : ADC command queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer not available for this request
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcStopSampling(ADI_EAL_ADC_HANDLE hAdcHandle)
{
    /* NOTE: Because of a silicon anomaly in ADC_STOP command,
             we do ADC soft reset for stopping ADC sampling.
             Because soft reset is applied, all ADC configurations
             are lost and needs a reconfiguration. */

    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Extract the data of the first instance even if it is multicast,
       since, we need the ADC register configuration values. */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* Get the instance and remote numbers */
    uint32_t nInstNum = poEalAdcInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalAdcInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalAdcInstData->oId.nIntfNum;

    /* Get the interface configuration */
    ADI_E2BCORE_IF_CONFIG *poE2bIntfCfg =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG));
    uint16_t anAddr[6U];
    uint32_t anData[6U];

    ADI_DBG_REQUIRE(poE2bIntfCfg->nNumIfReg <= 5U, "Invalid assumption regarding number of ADC registers");

    for(uint32_t i = 0U; i < poE2bIntfCfg->nNumIfReg; i++)
    {
        anAddr[i + 1U] = poE2bIntfCfg->aoIfRegData[i].nAddr;
        anData[i + 1U] = poE2bIntfCfg->aoIfRegData[i].nVal;
    }

    anAddr[0U] = ADC_CONTROL_NW_ADDR;
    anData[0U] = (uint32_t)0xAU << BITP_ADC_CONTROL_SOFT_RESET;

    oRegCfg.eType = ADI_E2BCORE_RND_WRITE;
    oRegCfg.nLength = (uint8_t)poE2bIntfCfg->nNumIfReg + 1U;
    oRegCfg.anStartAddr = &anAddr[0U];
    oRegCfg.pBuffer = &anData[0U];

    /* Send the ADC register write message */
    return adi_eal_adcSendRegTransaction(hAdcHandle, false, &oRegCfg);
}

/**
 * @brief       Gets the ADC interface status
 *
 * @param [in]  hAdcHandle     The ADC interface Handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Set / clear successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcReadStatus(ADI_EAL_ADC_HANDLE hAdcHandle)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    bool bIsMcast = poEalAdcData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalAdcData->nIfRegVlanTag : *poEalAdcInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalAdcData->nIntfTopic;
        anTopics[1U] = poEalAdcData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalAdcInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalAdcInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalAdcInstData->oId, ADC_STATS_NW_ADDR, 1U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief       Resets the ADC transaction queue and goes back to a clean state
 *
 * @param  [in] hAdcHandle      The ADC handle
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: ADC reset successful.
 *
 * @note        This API only works with ADC unicast handle.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcResetQueue(ADI_EAL_ADC_HANDLE hAdcHandle)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalAdcData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* Lock the mutex */
    LockAdcMutex(poEalAdcData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearAdcConfigQueue(&poEalAdcInstData->oId);
        poEalAdcInstData->nAdcQWrIdx = 0U;
        poEalAdcInstData->nAdcQRdIdx = 0U;
        poEalAdcInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;

        /* Ignore errors */
        poEalAdcInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  ADC_CONTROL_NW_ADDR;
        uint32_t nMask      =  BITM_ADC_CONTROL_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      =  BITM_ADC_CONTROL_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_adcSendRegTransaction((ADI_EAL_ADC_HANDLE)poEalAdcData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an ADC NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalAdcInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);

        /* Issue FIFO resets */
        nStartAddr = ADC_CONTROL_NW_ADDR;
        nMask      = BITM_ADC_CONTROL_TX_FIFO_RESET | BITM_ADC_CONTROL_RX_FIFO_RESET;
        nData      = BITM_ADC_CONTROL_TX_FIFO_RESET | BITM_ADC_CONTROL_RX_FIFO_RESET;
        eEalStatus = adi_eal_adcSendRegTransaction((ADI_EAL_ADC_HANDLE)poEalAdcData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalAdcInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockAdcMutex(poEalAdcData);

    return eEalStatus;
}

/**
 * @brief Send a register map transaction to the ADC interface or the corresponding FIFO registers
 *
 * @param [in]    hAdcHandle             The ADC interface Handle
 * @param [in]    bIsFifoRegTransaction  Is this reg transaction mapped destined to FIFO registers?
 * @param [inout] poRegCfg               Register read details
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcSendRegTransaction(ADI_EAL_ADC_HANDLE hAdcHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalAdcInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalAdcInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalAdcInstData->oId.nIntfNum;

    bool bIsMcast = poEalAdcData->bIsMulticast;
    uint32_t nVlanTag;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));

    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.bMailbox = poEalAdcData->bMailboxMode;
    oMsgOpts.bSync = poEalAdcData->bResyncPending;

    poEalAdcData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalAdcData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalAdcData->nFifoRegVlanTag : *poEalAdcInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalAdcData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalAdcData->nIfRegVlanTag : *poEalAdcInstData->pnIfRegVlanTag;
    }

    /* Set the Network frame configurations */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    eal_utils_fillNetworkCfg(nInstNum,
                             bIsMcast ? ADI_EAL_ALL_REMOTES : nRemoteNum,
                             &oNwFrameCfg, &nVlanTag);

    /* Create the register map transaction and transmit */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, poRegCfg);
}

/**
 * @brief Sets the downstream VLAN tag for ADC instance
 *
 * @param [in] hAdcHandle         ADC instance handle
 * @param [in] poIfRegVlanTag     Poinetr to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Poinetr to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Poinetr to VLAN tag for FIFO write access
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ADC_CODE
void adi_eal_adcSetDownstreamVlan(ADI_EAL_ADC_HANDLE hAdcHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                  ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Get the unicast instance data from the handle */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* If it is a multicast handle, update the handle structure */
    if(poEalAdcData->bIsMulticast)
    {
        poEalAdcData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalAdcData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalAdcData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalAdcInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalAdcInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalAdcInstData->oId.nIntfNum;

        /* For interface register access */
        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].
        aoIntfConfigs[nIntfNum].nIfRegVlanTag = eal_utils_convertVlanTags(poIfRegVlanTag);

        /* For FIFO register access */
        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].
        aoIntfConfigs[nIntfNum].nFifoRegVlanTag = eal_utils_convertVlanTags(poFifoRegVlanTag);

        /* For FIFO write access */
        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].
        aoIntfConfigs[nIntfNum].nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
}

/**
 * @brief Sets the upstream VLAN ID to be added for ADC FIFO messages
 *
 * @param [in] hAdcHandle  ADC interface Handle
 * @param [in] poVlantag   Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn   If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcSetUpstreamVlan(ADI_EAL_ADC_HANDLE hAdcHandle, ADI_VLAN_CFG *poVlantag, bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    ADI_DBG_REQUIRE(poVlantag != NULL, "Pointer to VLAN tag is NULL");

    /* Add VLAN tag info */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    uint16_t nStartAddr = FIFO_CTAG_CONFIGURATION_NW_ADDR;
    uint32_t nRegData = (bIsVlanEn ? ((uint32_t)poVlantag->nVlanId & BITM_FIFO_CTAG_CONFIGURATION_CTAG_ID) : 0U) |
                        (poVlantag->bDEI ?
                         (uint32_t)BITM_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE : 0U) | (((uint32_t)poVlantag->nPCP & 0x07U) <<
                                 BITP_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY) | (bIsVlanEn ? BITM_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG : 0U);
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.eType = ADI_E2BCORE_RND_WRITE;

    return adi_eal_adcSendRegTransaction(hAdcHandle, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for ADC transactions
 *
 * @param [in]  hAdcHandle      The ADC handle
 * @param [in]  bEnable         Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcSetMailboxMode(ADI_EAL_ADC_HANDLE hAdcHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Set the mailbox mode flag */
    poEalAdcData->bMailboxMode = bEnable;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for ADC transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hAdcHandle      The ADC handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ADC_CODE
ADI_EAL_STATUS adi_eal_adcResyncSeqNum(ADI_EAL_ADC_HANDLE hAdcHandle)
{
    /* Get the internal data from the handle */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)hAdcHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalAdcData);

    /* Set the sync mode flag */
    poEalAdcData->bResyncPending = true;

    return ADI_EAL_STATUS_SUCCESS;
}

/*! \private */
/*================================ STATIC CODE ==============================*/
/*! Sends the ADC transaction */
ADI_EAL_ADC_CODE
static ADI_EAL_STATUS SendAdcTransaction(EAL_ADC_DATA *poEalAdcData, ADI_E2BCORE_ADC_TXN_CONFIG *poTxnCfg)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalAdcData->bIsMulticast;

    /* Use the locally created ADC transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poAdcTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalAdcInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalAdcData, poEalAdcInstData, bIsMcast, &poAdcTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalAdcData->nFifoWriteVlanTag : *poEalAdcInstData->pnFifoWriteVlanTag;

        eal_utils_fillNetworkCfg(poEalAdcInstData->oId.nInstNum,
                                 poEalAdcData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalAdcInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag);

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalAdcData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalAdcData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalAdcData->bIsMulticast ?
                                      poEalAdcData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalAdcInstData->oId.nIntfNum;

        poEalAdcData->bResyncPending = false;

        /* Send the unicast / multicast e2b packet with the FIFO message containing the ADC transaction */
        eEalStatus = eal_utils_sendFifoFrame(&poEalAdcInstData->oId, &oNwFrameCfg, poAdcTxnCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalAdcData, bIsMcast, NULL) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/*! Extract ADC status from register data array */
ADI_EAL_ADC_CODE
static void extractAdcStatus(uint32_t *anData, ADI_EAL_ADC_STATUS_DATA *poStatusData)
{
    /* Extract the number of samples processed */
    poStatusData->nNumSamples = (uint16_t)((anData[0U] & 0xFFFFU));
}

/*! Lock the mutex of all ADC instances */
ADI_EAL_ADC_CODE
static void LockAdcMutex(EAL_ADC_DATA *poEalAdcData)
{
    /* Loop through all the unicast ADC instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalAdcData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalAdcData->apoInstData[i]->hAdcMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all ADC instances */
ADI_EAL_ADC_CODE
static void UnlockAdcMutex(EAL_ADC_DATA *poEalAdcData)
{
    /* Loop through all the unicast ADC instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalAdcData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalAdcData->apoInstData[i]->hAdcMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Returns if response is expected based on transaction and instance configuration */
ADI_EAL_ADC_CODE
static bool IsResponseExpected(EAL_ADC_INST_DATA *poEalAdcData, ADI_E2BCORE_ADC_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            poEalAdcData->bIsReadTimeTagEn);
}

/** Handle error callback */
ADI_EAL_ADC_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalAdcInstData->bIgnoreErr)
    {
        InvokeCbk(poEalAdcData, poEalAdcInstData, ADI_EAL_ADC_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_ADC_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal data of this instance */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* If the read is of the ADC STATUS registers */
    if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == ADC_STATS_NW_ADDR) &&
            (poRegCfg->nLength == 1U))
    {
        /* Process ADC status data */
        ADI_EAL_ADC_STATUS_DATA oAdcStatusData;
        extractAdcStatus(poRegCfg->pBuffer, &oAdcStatusData);
        InvokeCbk(poEalAdcData, poEalAdcInstData, ADI_EAL_ADC_CBK_EVT_INTF_STATUS_EVT, &oAdcStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalAdcData, poEalAdcInstData, ADI_EAL_ADC_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalAdcData, poEalAdcInstData, ADI_EAL_ADC_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_ADC_CODE
static void HandleFifoReadCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the ADC transaction configurations */
    const ADI_E2BCORE_ADC_TXN_CONFIG *poAdc = (const ADI_E2BCORE_ADC_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal data of this instance */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    /* ecco */
    if(poAdc->oTxnHdr.nNumPackedTxns == 0U)
    {
        /* Increment the read index */
        poEalAdcInstData->nAdcQRdIdx = EAL_INC_Q_IDX(poEalAdcInstData->nAdcQRdIdx, poEalAdcInstData->nAdcCfgQDepth);

        /* Give callback to application */
        InvokeCbk(poEalAdcData, poEalAdcInstData, ADI_EAL_ADC_CBK_EVT_ADC_WRITE_RESP, poAdc);
    }
    /* ADC sample */
    else
    {
        /* Give callback to application */
        InvokeCbk(poEalAdcData, poEalAdcInstData, ADI_EAL_ADC_CBK_EVT_ADC_SAMPLE, poAdc);
    }
}

/** Handle mailbox acknowledgment callback */
ADI_EAL_ADC_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_ADC_DATA *poEalAdcData = (EAL_ADC_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[0U];

    ADI_EAL_ADC_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd = poAckDat->bFd;
    oAckData.nTopic = poAckDat->nTopic;

    InvokeCbk(poEalAdcData, poEalAdcInstData, ADI_EAL_ADC_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/** Invoke the callback */
ADI_EAL_ADC_CODE
static void InvokeCbk(EAL_ADC_DATA *poEalAdcData, EAL_ADC_INST_DATA *poEalAdcInstData, ADI_EAL_ADC_CBK_EVT eEvt,
                      const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalAdcInstData->pfunCbk != NULL)
    {
        (poEalAdcInstData->pfunCbk)(poEalAdcInstData->oId.nInstNum, poEalAdcData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_ADC_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_ADC_DATA *poEalAdcData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalAdcData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalAdcInstData->nAdcQWrIdx, poEalAdcInstData->nAdcQRdIdx,
                               poEalAdcInstData->nAdcCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_ADC_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_ADC_DATA *poEalAdcData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalAdcData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInAdcQueue(&poEalAdcInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_ADC_CODE
static void CopyTransactionConfigToQueue(EAL_ADC_DATA *poEalAdcData, ADI_E2BCORE_ADC_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalAdcData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_ADC_TXN_CONFIG *poInstTxnCfg = &poEalAdcInstData->paoCfgs[poEalAdcInstData->nAdcQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_ADC_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_ADC_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_ADC_DATA *poEalAdcData, bool bIsMcast, uint8_t **apRxBuf)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalAdcData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ADC_INST_DATA *poEalAdcInstData = poEalAdcData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_ADC_TXN_CONFIG *poInstTxnCfg = &poEalAdcInstData->paoCfgs[poEalAdcInstData->nAdcQWrIdx];
        poInstTxnCfg->pPayload = apRxBuf != NULL ? apRxBuf[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addAdcConfigToQueue(&poEalAdcInstData->oId, false, poInstTxnCfg);
            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalAdcInstData->nAdcQWrIdx = EAL_INC_Q_IDX(poEalAdcInstData->nAdcQWrIdx, poEalAdcInstData->nAdcCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_ADC_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_ADC_DATA *poEalAdcData, EAL_ADC_INST_DATA *poEalAdcInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoAdcTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalAdcData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalAdcData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalAdcData, *ppoAdcTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoAdcTxnCfg = &poEalAdcInstData->paoCfgs[poEalAdcInstData->nAdcQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/*================================ CALLBACKS ================================*/
/*! e2bcore ADC callback */
ADI_EAL_ADC_CODE
void AdcInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                     void *pCbParam)
{
    /* If pCbParam is NULL, the EAL ADC interface instance is not opened,
       and hence, we shouldn't be processing any callback event */
    if(pCbParam != NULL)
    {
        /* Handle the different callback events from e2bcore */
        switch(eType)
        {
            case ADI_E2BCORE_CBK_ERR:
            {
                HandleErrorCallback(poParam, pCbParam);
                break;
            }

            case ADI_E2BCORE_CBK_REGREAD:
            {
                HandleRegReadCallback(poID, poParam, pCbParam);
                break;
            }

            case ADI_E2BCORE_CBK_FIFOREAD:
            {
                HandleFifoReadCallback(poParam, pCbParam);
                break;
            }

            case ADI_E2BCORE_CBK_ACK:
            {
                HandleMbAckCallback(poID, poParam, pCbParam);
                break;
            }

            default:
            {
                /* Shouldn't come here! */
                ADI_DBG_ERROR();
                break;
            }
        }
    }
}


#endif /* ADI_E2B_IFACE_ADC_ENABLED */
/*! \endcond */

/** @} */

/**
 * EOF: www.analog.com
 */
