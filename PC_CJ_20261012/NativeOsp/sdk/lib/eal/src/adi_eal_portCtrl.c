/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_portCtrl.c
 * @brief: The EAL APIs for port controller interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "eal_internal.h"       /* The EAL internal header */

#ifdef ADI_E2B_IFACE_PORT_ENABLED

/*! \cond private */
/*================================= DEFINES =================================*/
#define PWM_REFERENCE_FREQ      (393700.78f)   /*!< Reference frequency for PWM */
#define ADI_EAL_PORTCTRL_PORT_MASK  ((uint32_t)0x00000FFFUL)  /*!< 12-bit mask for port 0-11 */

/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalPortCtrlData) \
ADI_DBG_REQUIRE((poEalPortCtrlData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalPortCtrlData)->bInUse == true, "Instance is not opened");

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendPortCtrlTransaction(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg);
static ADI_EAL_STATUS SendRegAndPortCtrlTransaction(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg);
static uint16_t GetSlopeMask(ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData);
static void CalculateSlopeFromFreq(ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint16_t *pnSlope, uint16_t *pnSlopeMask,
                                   uint8_t *pnClkDiv);
static void UpdateClkDivFromSlopeMask(uint16_t nSlopeMask, uint8_t nClkDiv, uint16_t *pnStartAddr, uint32_t *pnMask,
                                      uint32_t *pnData);
static void extractPortCtrlStatus(uint32_t *anData, ADI_EAL_PORTCTRL_STATUS_DATA *poStatusData);
static void LockPortCtrlMutex(EAL_PORTCTRL_DATA *poEalPortCtrlData);
static void UnlockPortCtrlMutex(EAL_PORTCTRL_DATA *poEalPortCtrlData);
static void SetWaitingForRx(EAL_PORTCTRL_DATA *poEalPortCtrlData, bool bWaitingForRx);
static bool WaitForResponses(EAL_PORTCTRL_DATA *poEalPortCtrlData);
static void SaveRxPointer(EAL_PORTCTRL_DATA *poEalPortCtrlData, uint32_t **apnPinVals);
static bool IsResponseExpected(EAL_PORTCTRL_INST_DATA *poEalPortCtrlData, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg);
static uint16_t GetNumTxnsWithResp(EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
                                   ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfg,
                                   uint16_t nNumPortCtrlTxns);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleInterfaceRegReadCallback(const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg,
        EAL_PORTCTRL_DATA *poEalPortCtrlData, EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
        ADI_E2BCORE_CBK_REGREADDAT *poRegDat);
static void HandleFifoReadCallback(void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void InvokeCbk(EAL_PORTCTRL_DATA *poEalPortCtrlData, EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
                      ADI_EAL_PORTCTRL_CBK_EVT eEvt, const void *pArg);
static bool IsSpaceAvblInEalTxnQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData);
static void CopyTransactionConfigToQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData, bool bIsMcast, uint8_t **apRxBuf);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoPortCtrlTxnCfg);
static ADI_EAL_STATUS SendCustomPortCtrlTransactions(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfg, uint16_t nNumPortCtrlTxns, uint16_t nNumTxnsWithResp);
static ADI_EAL_STATUS AddCustomPortCtrlTxnsToMsg(uint32_t nInstNum, EAL_PORTCTRL_DATA *poEalPortCtrlData,
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfg,
        uint16_t nNumPortCtrlTxns);

/*! \endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens a PORT_CTRL instance
 *
 * @param [in]  poPortCtrlCfg       Configuration pointer for EAL PORTCTRL instance
 * @param [out] phPortCtrlHandle    Returns the PORTCTRL handle for use in other EAL PORTCTRL APIs
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: PORTCTRL instance was successfully opened.
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlOpen(ADI_EAL_PORTCTRL_CFG *poPortCtrlCfg, ADI_EAL_PORTCTRL_HANDLE *phPortCtrlHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poPortCtrlCfg != NULL, "Pointer to the PORTCTRL module configuration is NULL");
    ADI_DBG_REQUIRE(phPortCtrlHandle != NULL, "Pointer to the PORTCTRL handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poPortCtrlCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poPortCtrlCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poPortCtrlCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poPortCtrlCfg->nInstNum;
    uint32_t nRemoteNum = poPortCtrlCfg->nRemoteNum;
    uint32_t nIntfNum = poPortCtrlCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_PORT,
                    "The given pair of instance, remote and interface numbers is not configured as PORTCTRL interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poPortCtrlCfg->pEalPortCtrlMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poPortCtrlCfg->nEalPortCtrlMemorySize >= ADI_EAL_PORTCTRL_UNICAST_INST_MEM_SIZE(
                        poPortCtrlCfg->nPortCtrlCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poPortCtrlCfg->pEalPortCtrlMemory;

    /* Allocate memory for the PORTCTRL transaction config queue */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfgs = (ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *)((void *)pMem);
    pMem += (poPortCtrlCfg->nPortCtrlCfgQDepth * sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Allocate memory for the EAL PORTCTRL instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = (EAL_PORTCTRL_INST_DATA *)((void *)pMem);
    pMem += sizeof(EAL_PORTCTRL_INST_DATA);
    /* ~~~~~~~~~~~ PORTCTRL instance info ~~~~~~~~~~~ */
    poEalPortCtrlInstData->oId.nInstNum      = nInstNum;
    poEalPortCtrlInstData->oId.nRemoteNum    = nRemoteNum;
    poEalPortCtrlInstData->oId.nIntfNum      = nIntfNum;
    poEalPortCtrlInstData->pnIfRegVlanTag    = &poIfConfig->nIfRegVlanTag;
    poEalPortCtrlInstData->pnFifoRegVlanTag  = &poIfConfig->nFifoRegVlanTag;
    poEalPortCtrlInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;
    /* ~~~~~~~~~~~ PORTCTRL state info ~~~~~~~~~~~ */
    poEalPortCtrlInstData->bIgnoreErr           = false;
    poEalPortCtrlInstData->eMode                = ADI_EAL_USAGE_MODE_NORMAL;
    poEalPortCtrlInstData->pfunCbk              = NULL; /* Will be filled in adi_eal_portCtrlRegisterCallback */
    poEalPortCtrlInstData->bWaitingForRx        = false;
    poEalPortCtrlInstData->pnPinVals            = NULL;
    poEalPortCtrlInstData->bIsReadTimeTagEn     = false;
    poEalPortCtrlInstData->nInputMonitorEnMask  = 0U;
    poEalPortCtrlInstData->nOutputEnMask        = 0U;
    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    uint32_t nPortCtrlCtrlReg = 0U;
    EAL_PORTCTRL_INST_DATA **apoInstData;
    EAL_PORTCTRL_DATA *poEalPortCtrlData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalPortCtrlInstData->oId, PORT_CONTROL_NW_ADDR, &nPortCtrlCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalPortCtrlInstData->bIsReadTimeTagEn  = (nPortCtrlCtrlReg & BITM_PORT_CONTROL_ENABLE_READ_TIME_TAG_ECCO) != 0U;

    /* Read output mux register from config to track which pins are configured as output */
    uint32_t nOutputMuxReg = 0U;
    if(eal_utils_getConfigInfRegVal(&poEalPortCtrlInstData->oId, PORT_OUTPUT_MUX_NW_ADDR, &nOutputMuxReg, false) == true)
    {
        /* Store lower 12 bits which indicate output enable for each port (0-11) */
        poEalPortCtrlInstData->nOutputEnMask = nOutputMuxReg & ADI_EAL_PORTCTRL_PORT_MASK;
    }
    else
    {
        /* Nothing to do - nOutputEnMask remains 0 (all pins treated as input) */
    }
    poEalPortCtrlInstData->nPortCtrlCfgQDepth = poPortCtrlCfg->nPortCtrlCfgQDepth;
    poEalPortCtrlInstData->nPortCtrlQWrIdx   = 0U;
    poEalPortCtrlInstData->nPortCtrlQRdIdx   = 0U;
    poEalPortCtrlInstData->paoCfgs           = paoPortCtrlTxnCfgs;
    poEalPortCtrlInstData->oRxCfg.pPayload   = &poEalPortCtrlInstData->oRxPayload;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalPortCtrlInstData->oId, (void *)pMem,
            poPortCtrlCfg->nPortCtrlCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *) * poPortCtrlCfg->nPortCtrlCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_PORTCTRL_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_PORTCTRL_INST_DATA *);
        *apoInstData = poEalPortCtrlInstData;

        /* Allocate memory for the internal EAL PORTCTRL data structure */
        poEalPortCtrlData = (EAL_PORTCTRL_DATA *)((void *)pMem);
        pMem += sizeof(EAL_PORTCTRL_DATA);
        poEalPortCtrlData->bIsMulticast  = false;
        poEalPortCtrlData->bInUse        = true;
        poEalPortCtrlData->nIntfTopic    = 0U;
        poEalPortCtrlData->nFifoTopic    = 0U;
        poEalPortCtrlData->nNumInst      = 1U;
        poEalPortCtrlData->apoInstData   = apoInstData;
        poEalPortCtrlData->bMailboxMode  = false;
        poEalPortCtrlData->bResyncPending = false;

        /* Initialise the mutex for this PORTCTRL instance */
        poEalPortCtrlInstData->oPortCtrlMutexCfg.pMem = &poEalPortCtrlInstData->anPortCtrlMutexBuffer[0U];
        poEalPortCtrlInstData->oPortCtrlMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalPortCtrlInstData->oPortCtrlMutexCfg, &poEalPortCtrlInstData->hPortCtrlMutex);

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        /* Initialise the semaphore for this PORTCTRL instance */
        poEalPortCtrlInstData->oFifoRespSemaphoreCfg.pMem = &poEalPortCtrlInstData->anPortCtrlFifoRespSemaphoreBuffer[0U];
        poEalPortCtrlInstData->oFifoRespSemaphoreCfg.nMemSize = PRJ_OSAL_SEMAPHORE_MEM_SIZE;
        poEalPortCtrlInstData->oFifoRespSemaphoreCfg.bCountingSemaphore = false;
        poEalPortCtrlInstData->oFifoRespSemaphoreCfg.nMaxCount = 1U;
        poEalPortCtrlInstData->oFifoRespSemaphoreCfg.nInitialCount = 0U;
        (void)prj_osal_createSemaphore(&poEalPortCtrlInstData->oFifoRespSemaphoreCfg,
                                       &poEalPortCtrlInstData->hFifoRespSemaphore);
#endif

        /* Set the callback param in the e2bcore config */
        poIfConfig->pCbParam = poEalPortCtrlData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_PWM;

        /* Set the handle to the EAL_PORTCTRL_DATA structure */
        *phPortCtrlHandle = (ADI_EAL_PORTCTRL_HANDLE)poEalPortCtrlData;

        /* Provide memory to e2bcore for the RX PORTCTRL transaction config (containing an PORTCTRL sample) */
        if(adi_e2bcore_addPortCtrlConfigToQueue(&poEalPortCtrlInstData->oId, true, &poEalPortCtrlInstData->oRxCfg)
                != ADI_E2BCORE_ERR_NONE)
        {
            eStatus = ADI_EAL_STATUS_OPEN_FAILED;
        }
    }

    return eStatus;
}

/**
 * @brief       Closes an open PORT_CTRL instance
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: PORT_CTRL instance was successfully closed.
 *              - #ADI_EAL_STATUS_CLOSE_FAILED: PORT_CTRL instance was not open.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlClose(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalPortCtrlData->apoInstData[i]->hPortCtrlMutex);
#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        (void)prj_osal_destroySemaphore(poEalPortCtrlData->apoInstData[i]->hFifoRespSemaphore);
#endif
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalPortCtrlData, 0, sizeof(EAL_PORTCTRL_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to register callback
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  pfPortCtrlCallback  Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Callback was successfully registered.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlRegisterCallback(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_CALLBACK_FUNC pfPortCtrlCallback)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(pfPortCtrlCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalPortCtrlData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* Register the callback function */
    poEalPortCtrlInstData->pfunCbk = pfPortCtrlCallback;

    return ADI_EAL_STATUS_SUCCESS;
};

/**
 * @brief       Create a multicast group of PORT_CTRL interfaces
 *
 * @param [in]  poPortCtrlMcastCfg       Configuration for PORT_CTRL multicast group
 * @param [out] phPortCtrlMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlCreateMcastGroup(ADI_EAL_PORTCTRL_MCAST_CFG *poPortCtrlMcastCfg,
        ADI_EAL_PORTCTRL_HANDLE *phPortCtrlMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poPortCtrlMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phPortCtrlMcastHandle != NULL, "Pointer to the PORTCTRL multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poPortCtrlMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poPortCtrlMcastCfg->pahPortCtrlHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poPortCtrlMcastCfg->pPortCtrlMcastMem != NULL, "Pointer to memory for PORTCTRL multicast is NULL");
    ADI_DBG_REQUIRE(poPortCtrlMcastCfg->nPortCtrlMcastMemSize >= ADI_EAL_PORTCTRL_MCAST_GROUP_MEM_SIZE(
                        poPortCtrlMcastCfg->nNumInst),
                    "Not enough memory allocated for PORTCTRL multicast group");
    ADI_DBG_REQUIRE(poPortCtrlMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poPortCtrlMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast FIFO topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poPortCtrlMcastCfg->pPortCtrlMcastMem;
    EAL_PORTCTRL_INST_DATA **apoInstData = (EAL_PORTCTRL_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_PORTCTRL_INST_DATA *) * poPortCtrlMcastCfg->nNumInst);

    /* Save the unicast EAL PORTCTRL instances */
    for(uint16_t i = 0U; i < poPortCtrlMcastCfg->nNumInst; i++)
    {
        EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)poPortCtrlMcastCfg->pahPortCtrlHandles[i];
        apoInstData[i] = poEalPortCtrlData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast FIFO topic */
        uint32_t nInstNum = poEalPortCtrlData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalPortCtrlData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalPortCtrlData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poPortCtrlMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poPortCtrlMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poPortCtrlMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poPortCtrlMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_PORTCTRL_DATA *poEalPortCtrlMcastData = (EAL_PORTCTRL_DATA *)((void *)pMem);
    poEalPortCtrlMcastData->bIsMulticast = true;
    poEalPortCtrlMcastData->bInUse       = true;
    poEalPortCtrlMcastData->nIntfTopic   = poPortCtrlMcastCfg->nIntfTopic;
    poEalPortCtrlMcastData->nFifoTopic   = poPortCtrlMcastCfg->nFifoTopic;
    poEalPortCtrlMcastData->nNumInst     = poPortCtrlMcastCfg->nNumInst;
    poEalPortCtrlMcastData->nIfRegVlanTag       = (poPortCtrlMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poPortCtrlMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalPortCtrlMcastData->nFifoRegVlanTag     = (poPortCtrlMcastCfg->poFifoRegVlanTag != NULL) ?
            eal_utils_convertVlanTags(poPortCtrlMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalPortCtrlMcastData->nFifoWriteVlanTag   = (poPortCtrlMcastCfg->poFifoWriteVlanTag != NULL) ?
            eal_utils_convertVlanTags(poPortCtrlMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalPortCtrlMcastData->apoInstData  = apoInstData;
    poEalPortCtrlMcastData->bMailboxMode  = false;
    poEalPortCtrlMcastData->bResyncPending = false;

    /* Set the handle to the EAL_PORTCTRL_DATA structure */
    *phPortCtrlMcastHandle = (ADI_EAL_PORTCTRL_HANDLE)poEalPortCtrlMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to send gpio write frames asynchronously
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to GPIO write DATA
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : GPIO write request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the write operation is completed
 *              * This API can be called even when there is another write in progress
 * @note        <b>E2B_ID Usage Pattern (Async API)</b>:
 *              - E2B_ID is <b>optional</b> for asynchronous GPIO write operations
 *              - Set nId = 0xFF to disable E2B_ID
 *              - Set nId to any value (0x00-0xFE) to enable E2B_ID tracking
 *              - When enabled, E2B_ID is copied to the upstream acknowledgment message,
 *                allowing correlation between this downstream command and device response
 *              - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlGpioWrite(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0U].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_GPIO_STEADY_VAL;
    aoTxPayload[0U].nGpioSteadyVal = poPortCtrlData->nPinVals;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Enable or disable GPIO Input Monitor and Read Time Tag for one or more ports
 *
 * @param [in]  hPortCtrlHandle   The PORTCTRL handle
 * @param [in]  nPinMask          Bitmask of ports to configure (bit N = port N, valid bits 0-11)
 * @param [in]  bGpioEnable       true to enable GPIO Input Monitor, false to disable
 * @param [in]  bTimeTagEnable    true to enable Read Time Tag, false to disable
 * @return      Status (ADI_EAL_STATUS_SUCCESS or error)
 *
 * @details     This API sets the GPIO Input Monitor and Read Time Tag enable bits for the specified ports.
 *              Multiple ports can be configured in a single call using a bitmask (e.g., 0x0FFF for all 12 ports).
 *              It updates the relevant register, sets the enable variables in instance data, and sends
 *              the corresponding transactions if enabled. Event callbacks are handled in lower-level handlers.
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to
 *              transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlSetGpioInputMonitorAndReadTimeTag(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        uint16_t nPinMask, bool bGpioEnable, bool bTimeTagEnable)
{
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nRegVal = 0U;
    uint32_t nBitMask;
    uint32_t nBitMaskWithTimeTag;
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    uint16_t nAddr = PORT_GENERAL_CONTROLS_AND_READBACKS_NW_ADDR;

    /* Parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    if ((nPinMask == 0U) || ((nPinMask & (uint16_t)0xF000U) != 0U))
    {
        eStatus = ADI_EAL_STATUS_INVALID_PARAM;
    }
    else if ((bGpioEnable == true) &&
             ((poEalPortCtrlData->apoInstData[0U]->nOutputEnMask & (uint32_t)nPinMask) != 0U))
    {
        /* Pin is configured as output - input monitor cannot be enabled */
        eStatus = ADI_EAL_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Use the pin mask directly as the bit mask for the register */
        nBitMask = (uint32_t)nPinMask;

        /* Lock mutex */
        LockPortCtrlMutex(poEalPortCtrlData);

        /* Calculate bit mask for GPIO Input Monitor based on pin mask */
        if (bGpioEnable)
        {
            nRegVal = nRegVal | nBitMask;
        }
        else
        {
            /* Disable: write 0 to specified pin bits (mask-write limits affected bits) */
        }

        /* Modify value for Read Time Tag */
        if (bTimeTagEnable)
        {
            nRegVal = nRegVal | BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_ENABLE_READ_TIME_TAG;
        }
        else
        {
            /* Disable: write 0 to time tag bit (mask-write limits affected bits) */
        }

        nBitMaskWithTimeTag = nBitMask | BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_ENABLE_READ_TIME_TAG;

        /* Write updated value to register. Caller/app must flush as needed. */
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nBitMaskWithTimeTag;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.anStartAddr = &nAddr;
        oRegCfg.pBuffer = &nRegVal;

        eStatus = adi_eal_portCtrlSendRegTransaction(hPortCtrlHandle, false, &oRegCfg);
        if (eStatus == ADI_EAL_STATUS_SUCCESS)
        {
            /* Update the per-pin bitmask in instance data on successful write */
            if (bGpioEnable)
            {
                poEalPortCtrlData->apoInstData[0U]->nInputMonitorEnMask |= nPinMask;
            }
            else
            {
                poEalPortCtrlData->apoInstData[0U]->nInputMonitorEnMask &= (uint16_t)(~nPinMask);
            }
            poEalPortCtrlData->apoInstData[0U]->bIsReadTimeTagEn = bTimeTagEnable;
        }

        UnlockPortCtrlMutex(poEalPortCtrlData);
    }

    return eStatus;
}

/**
 * @brief       Enable or disable Read Time Tag ECCO for a port
 *
 * @param [in]  hPortCtrlHandle   The PORTCTRL handle
 * @param [in]  bEnable           true to enable, false to disable
 * @return      Status (ADI_EAL_STATUS_SUCCESS or error)
 *
 * @details     This API sets the Read Time Tag ECCO enable bit for the port. It updates the
 *              relevant register, sets the enable variable in instance data, and sends the
 *              corresponding transaction if enabled. Event callbacks are handled in lower-level handlers.
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to
 *              transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlSetReadTimeTagEcco(
    ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
    bool bEnable)
{
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t regVal = 0U;
    uint32_t bitMask = BITM_PORT_CONTROL_ENABLE_READ_TIME_TAG_ECCO;
    uint16_t nAddr = PORT_CONTROL_NW_ADDR;
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;

    BASIC_HANDLE_CHECK(poEalPortCtrlData); 

    LockPortCtrlMutex(poEalPortCtrlData);

    /* Modify value */
    if (bEnable)
    {
        regVal = regVal | bitMask;
    }
    else
    {
        regVal = regVal & (~bitMask);
    }

    /* Write updated value using mask write. Caller/app must flush as needed. */
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.anMask = &bitMask;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.pBuffer = &regVal;
    eStatus = adi_eal_portCtrlSendRegTransaction(hPortCtrlHandle, false, &oRegCfg);

    if (eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Update the cached value in instance data on successful write */
        poEalPortCtrlData->apoInstData[0U]->bIsReadTimeTagEccoEn = bEnable;
        /* Note: READ_TIME_TAG_ECCO events will arrive automatically from remote when enabled */
    }

    UnlockPortCtrlMutex(poEalPortCtrlData);
    return eStatus;
}
/**
 * @brief       Allows the application to send gpio write frames asynchronously and executed after time Delay.
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to GPIO write DATA
 * @param [in]  nTimeDelay          The delay time in ns
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : GPIO write request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the write operation is completed
 *              * This API can be called even when there is another write in progress
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlGpioWriteWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId)

{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));
    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nTimeDelay       = nTimeDelay;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    aoTxPayload[0U].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_GPIO_STEADY_VAL;
    aoTxPayload[0U].nGpioSteadyVal = poPortCtrlData->nPinVals;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;

}

/**
 * @brief       Allows the application to send gpio write frames asynchronously and executed at presentation time.
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to GPIO write DATA
 * @param [in]  nTimestamp          The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : GPIO write request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the write operation is completed
 *              * This API can be called even when there is another write in progress
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlGpioWriteWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId)

{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));
    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsGptpTimePresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nGptpTime       = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    aoTxPayload[0U].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_GPIO_STEADY_VAL;
    aoTxPayload[0U].nGpioSteadyVal = poPortCtrlData->nPinVals;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;

}

/**
 * @brief       Allows the application to send gpio write frames synchronously
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to GPIO write DATA
 * @param [in]  nId                 Identifier for the write request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : GPIO write request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request

 *
 * @details     * This API blocks till the GPIO write transaction completes
 *              * Make sure all pending GPIO transactions are completed before invoking this API
 * @note        <b>E2B_ID Usage Pattern (Sync API)</b>:
 *              - E2B_ID is <b>always enabled</b> for synchronous operations
 *              - This is <b>intentional and required</b> to receive device acknowledgment
 *              - The API blocks waiting for the upstream response with matching E2B_ID
 *              - E2B_ID enables correlation between the downstream write command and
 *                the upstream acknowledgment, confirming the device processed the transaction
 *              - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *              - The nId parameter value is used for tracking and correlation
 * @note        This is a synchronous API that internally flushes the transmit buffer before returning.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlGpioWriteSync(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;
    bool bTimedOut = false;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = true;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0U].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_GPIO_STEADY_VAL;
    aoTxPayload[0U].nGpioSteadyVal = poPortCtrlData->nPinVals;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalPortCtrlData, true);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bTimedOut = WaitForResponses(poEalPortCtrlData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalPortCtrlData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to send gpio read frames asynchronously
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [out] apnPinVals          Array of pointers to the store the pin values
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: GPIO read frame was sent successfully.
 *
 * @details   pnPinVals pointer will be updated with the pin values when the #ADI_EAL_PORTCTRL_CBK_EVT_GPIO_READ_RESP event callback is issued
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlGpioRead(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, uint32_t **apnPinVals)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(apnPinVals != NULL, "Pointer to read values is NULL");

    /* Set the parameters for the GPIO register map config */
    uint16_t nRegAddr = (uint16_t)PORT_OUTPUT_MUX2_NW_ADDR;

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG));
    oRegCfg.eType       = ADI_E2BCORE_BURST_READ;
    oRegCfg.nLength     = 1u;
    oRegCfg.anStartAddr = &nRegAddr;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Save the RX pointers for all instances */
    SaveRxPointer(poEalPortCtrlData, apnPinVals);

    /* Send the register transaction */
    ADI_EAL_STATUS eEalStatus = adi_eal_portCtrlSendRegTransaction(hPortCtrlHandle, false, &oRegCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to send gpio read frames synchronously
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [out] apnPinVals          Array of pointers to the store the pin values
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: GPIO read frame was sent successfully.
 *
 * @note        This is a synchronous API that internally flushes the transmit buffer and waits for the response before returning.
 *              The response data is available upon successful return (no timeout or error).
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlGpioReadSync(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, uint32_t **apnPinVals)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;
    bool bTimedOut = false;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(apnPinVals != NULL, "Pointer to read values is NULL");

    /* Set the parameters for the GPIO register map config */
    uint16_t nRegAddr = (uint16_t)PORT_OUTPUT_MUX2_NW_ADDR;

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG));
    oRegCfg.eType       = ADI_E2BCORE_BURST_READ;
    oRegCfg.nLength     = 1u;
    oRegCfg.anStartAddr = &nRegAddr;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Save the RX pointers for all instances */
    SaveRxPointer(poEalPortCtrlData, apnPinVals);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalPortCtrlData, true);

    /* Send the register transaction */
    ADI_EAL_STATUS eEalStatus = adi_eal_portCtrlSendRegTransaction(hPortCtrlHandle, false, &oRegCfg);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for register read response */
        bTimedOut = WaitForResponses(poEalPortCtrlData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalPortCtrlData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Starts PWM in the pins specified
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: PWM start frame was sent successfully.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmStart(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
                                        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData,
                                        uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[4U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 4U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* DutyCycle calculation */
    float fDutyCycle = ((poPortCtrlData->fPwmDutyCycle * 128.0f) / 100.0f);
    fDutyCycle += 0.5f;

    /* PulseDelay calculation */
    float fDelay = ((poPortCtrlData->fPulseDelay * 128.0f) / 100.0f);
    fDelay += 0.5f;

    /* Get the slope values */
    uint16_t nSlope = 0u;
    uint16_t nSlopeMask = 0u;
    uint8_t nClkDiv = 0U;
    CalculateSlopeFromFreq(poPortCtrlData, &nSlope, &nSlopeMask, &nClkDiv);

    /* Update the clk divider register address, mask and value based on slope mask */
    uint16_t nStartAddr;
    uint32_t nMask;
    uint32_t nData;
    UpdateClkDivFromSlopeMask(nSlopeMask, nClkDiv, &nStartAddr, &nMask, &nData);

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_SLOPE;
    aoTxPayload[0u].nMask        = nSlopeMask;
    aoTxPayload[0u].nSlope       = nSlope;

    aoTxPayload[1u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_PULSE_DELAY ;
    aoTxPayload[1u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[1u].nPulseDelay  = (uint8_t)fDelay;

    aoTxPayload[2u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY;
    aoTxPayload[2u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[2u].nDuty        = (uint8_t)fDutyCycle;

    aoTxPayload[3u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_START;
    aoTxPayload[3u].nMask        = poPortCtrlData->nPinMask;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendRegAndPortCtrlTransaction(poEalPortCtrlData, &oRegCfg, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Starts PWM after a delay in the pins specified
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nTimeDelay          The delay time in ns
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : GPIO write request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmStartWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[4U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nTimeDelay      = nTimeDelay;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 4U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* DutyCycle calculation */
    float fDutyCycle = ((poPortCtrlData->fPwmDutyCycle * 128.0f) / 100.0f);
    fDutyCycle += 0.5f;

    /* PulseDelay calculation */
    float fDelay = ((poPortCtrlData->fPulseDelay * 128.0f) / 100.0f);
    fDelay += 0.5f;

    /* Get the slope values */
    uint16_t nSlope = 0u;
    uint16_t nSlopeMask = 0u;
    uint8_t nClkDiv = 0U;
    CalculateSlopeFromFreq(poPortCtrlData, &nSlope, &nSlopeMask, &nClkDiv);

    /* Update the clk divider register address, mask and value based on slope mask */
    uint16_t nStartAddr;
    uint32_t nMask;
    uint32_t nData;
    UpdateClkDivFromSlopeMask(nSlopeMask, nClkDiv, &nStartAddr, &nMask, &nData);

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_SLOPE;
    aoTxPayload[0u].nMask        = nSlopeMask;
    aoTxPayload[0u].nSlope       = nSlope;

    aoTxPayload[1u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_PULSE_DELAY ;
    aoTxPayload[1u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[1u].nPulseDelay  = (uint8_t)fDelay;

    aoTxPayload[2u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY;
    aoTxPayload[2u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[2u].nDuty        = (uint8_t)fDutyCycle;

    aoTxPayload[3u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_START;
    aoTxPayload[3u].nMask        = poPortCtrlData->nPinMask;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendRegAndPortCtrlTransaction(poEalPortCtrlData, &oRegCfg, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Starts PWM at the presentation time in the pins specified
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nTimestamp          The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : PWM start frame was sent successfully.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmStartWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[4U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsGptpTimePresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nGptpTime       = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 4U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* DutyCycle calculation */
    float fDutyCycle = ((poPortCtrlData->fPwmDutyCycle * 128.0f) / 100.0f);
    fDutyCycle += 0.5f;

    /* PulseDelay calculation */
    float fDelay = ((poPortCtrlData->fPulseDelay * 128.0f) / 100.0f);
    fDelay += 0.5f;

    /* Get the slope values */
    uint16_t nSlope = 0u;
    uint16_t nSlopeMask = 0u;
    uint8_t nClkDiv = 0U;
    CalculateSlopeFromFreq(poPortCtrlData, &nSlope, &nSlopeMask, &nClkDiv);

    /* Update the clk divider register address, mask and value based on slope mask */
    uint16_t nStartAddr;
    uint32_t nMask;
    uint32_t nData;
    UpdateClkDivFromSlopeMask(nSlopeMask, nClkDiv, &nStartAddr, &nMask, &nData);

    /* Set the register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_SLOPE;
    aoTxPayload[0u].nMask        = nSlopeMask;
    aoTxPayload[0u].nSlope       = nSlope;

    aoTxPayload[1u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_PULSE_DELAY;
    aoTxPayload[1u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[1u].nPulseDelay  = (uint8_t)fDelay;

    aoTxPayload[2u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY;
    aoTxPayload[2u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[2u].nDuty        = (uint8_t)fDutyCycle;

    aoTxPayload[3u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_START;
    aoTxPayload[3u].nMask        = poPortCtrlData->nPinMask;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendRegAndPortCtrlTransaction(poEalPortCtrlData, &oRegCfg, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Stops PWM in the pins specified
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: PWM stop frame was sent successfully.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmStop(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
                                       ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_STOP;
    aoTxPayload[0u].nMask        = poPortCtrlData->nPinMask;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Stops PWM after a delay in the pins specified
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nTimeDelay          The delay time in ns
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: PWM stop frame was sent successfully.
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmStopWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nTimeDelay      = nTimeDelay;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_STOP;
    aoTxPayload[0u].nMask        = poPortCtrlData->nPinMask;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Stops PWM at the presentation time in the pins specified
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nTimestamp          The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: PWM stop frame was sent successfully.
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmStopWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsGptpTimePresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nGptpTime       = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_STOP;
    aoTxPayload[0u].nMask        = poPortCtrlData->nPinMask;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Changes the duty cycle in the pins specified
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nId                 Identifier for this request (Set to 0xFF is no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Frame to update duty cycle was sent successfully.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmDuty(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
                                       ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* DutyCycle calculation */
    float fDutyCycle = ((poPortCtrlData->fPwmDutyCycle * 128.0f) / 100.0f);
    fDutyCycle += 0.5f;

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY;
    aoTxPayload[0u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[0u].nDuty        = (uint8_t)fDutyCycle;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Changes the duty cycle in the pins specified after a delay
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nTimeDelay          The delay time in ns
 * @param [in]  nId                 Identifier for this request (Set to 0xFF is no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Frame to update duty cycle was sent successfully.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmDutyWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* DutyCycle calculation */
    float fDutyCycle = ((poPortCtrlData->fPwmDutyCycle * 128.0f) / 100.0f);
    fDutyCycle += 0.5f;

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nTimeDelay      = nTimeDelay;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY;
    aoTxPayload[0u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[0u].nDuty        = (uint8_t)fDutyCycle;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Changes the duty cycle in the pins specified at the presentation time
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @param [in]  poPortCtrlData      Pointer to PWM data
 * @param [in]  nTimestamp          The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in]  nId                 Identifier for this request (Set to 0xFF is no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Frame to update duty cycle was sent successfully.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlPwmDutyWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poPortCtrlData != NULL, "GPIO IO pointer is NULL");

    /* DutyCycle calculation */
    float fDutyCycle = ((poPortCtrlData->fPwmDutyCycle * 128.0f) / 100.0f);
    fDutyCycle += 0.5f;

    /* Set the PORTCTRL transaction configuration */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oTxnCfg;
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD aoTxPayload[1U];
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

    /* Set the PORTCTRL transaction configuration header */
    oTxnCfg.oTxnHdr.bIsGptpTimePresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked  = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nGptpTime       = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId          = nId;
    oTxnCfg.oTxnHdr.nNumPackedTxns  = 1U;
    oTxnCfg.pPayload                = &aoTxPayload[0U];

    /* Set the PORTCTRL transaction configuration payload */
    (void)ADI_MEMSET(aoTxPayload, 0, sizeof(aoTxPayload));
    aoTxPayload[0u].ePortCtrlCmd = ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY;
    aoTxPayload[0u].nMask        = poPortCtrlData->nPinMask;
    aoTxPayload[0u].nDuty        = (uint8_t)fDutyCycle;

    /* Loop through the mutex of all instances and lock it */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Send the PORTCTRL commands */
    ADI_EAL_STATUS eEalStatus = SendPortCtrlTransaction(poEalPortCtrlData, &oTxnCfg);

    /* Loop through the mutex of all instances and unlock it */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Gets the PORT_CTRL interface status
 *
 * @param [in]  hPortCtrlHandle     The PORT_CTRL handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlReadStatus(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    bool bIsMcast = poEalPortCtrlData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalPortCtrlData->nIfRegVlanTag : *poEalPortCtrlInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalPortCtrlData->nIntfTopic;
        anTopics[1U] = poEalPortCtrlData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalPortCtrlInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalPortCtrlInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalPortCtrlInstData->oId, PORT_ACTIVE_OUTPUT_READBACK_NW_ADDR, 1U, bIsMcast,
                                   anTopics, nVlanTag);
}

/**
 * @brief       Resets the PORT_CTRL transaction queue and goes back to a clean state
 *
 * @param  [in] hPortCtrlHandle      The PortCtrl handle
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: PortCtrl reset successful.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlResetQueue(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalPortCtrlData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* Lock the mutex */
    LockPortCtrlMutex(poEalPortCtrlData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearPortCtrlConfigQueue(&poEalPortCtrlInstData->oId);
        poEalPortCtrlInstData->nPortCtrlQWrIdx = 0U;
        poEalPortCtrlInstData->nPortCtrlQRdIdx = 0U;
        poEalPortCtrlInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;

        /* Ignore errors */
        poEalPortCtrlInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  PORT_CONTROL_NW_ADDR;
        uint32_t nMask      =  BITM_PORT_CONTROL_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      =  BITM_PORT_CONTROL_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_portCtrlSendRegTransaction((ADI_EAL_PORTCTRL_HANDLE)poEalPortCtrlData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an PORTCTRL NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalPortCtrlInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);

        /* Issue FIFO resets */
        nStartAddr = PORT_CONTROL_NW_ADDR;
        nMask      = BITM_PORT_CONTROL_TX_FIFO_RESET | BITM_PORT_CONTROL_RX_FIFO_RESET;
        nData      = BITM_PORT_CONTROL_TX_FIFO_RESET | BITM_PORT_CONTROL_RX_FIFO_RESET;
        eEalStatus = adi_eal_portCtrlSendRegTransaction((ADI_EAL_PORTCTRL_HANDLE)poEalPortCtrlData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalPortCtrlInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief       Send custom PORT_CTRL transactions
 *
 * @param [in]  hPortCtrlHandle     The PortCtrl handle
 * @param [in]  paoPortCtrlTxnCfgs  Array of PORT_CTRL transactions
 * @param [in]  nNumTxns            Number of PORT_CTRL transactions
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully added the request to queue
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlCustomWrite(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfgs, uint16_t nNumTxns)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(paoPortCtrlTxnCfgs != NULL, "PORTCTRL transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumTxns > 0U, "Number of PORTCTRL transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalPortCtrlData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* Lock the mutex */
    LockPortCtrlMutex(poEalPortCtrlData);

    /* Get the number of transactions that expects a response */
    uint16_t nNumTxnsWithResp = GetNumTxnsWithResp(poEalPortCtrlInstData, paoPortCtrlTxnCfgs, nNumTxns);

    /* Send the custom PORTCTRL transactions */
    eEalStatus = SendCustomPortCtrlTransactions(poEalPortCtrlData, poEalPortCtrlInstData, paoPortCtrlTxnCfgs,
                 nNumTxns, nNumTxnsWithResp);

    /* Unlock the mutex */
    UnlockPortCtrlMutex(poEalPortCtrlData);

    return eEalStatus;
}

/**
 * @brief Send a register map transaction to the PORTCTRL interface or the corresponding FIFO registers
 *
 * @param [in]    hPortCtrlHandle        The PortCtrl handle
 * @param [in]    bIsFifoRegTransaction  Is this for reg transaction for FIFO
 * @param [inout] poRegCfg               Register read details
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlSendRegTransaction(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalPortCtrlInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalPortCtrlInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalPortCtrlInstData->oId.nIntfNum;

    bool bIsMcast = poEalPortCtrlData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bMailbox       = poEalPortCtrlData->bMailboxMode;
    oMsgOpts.bSync          = poEalPortCtrlData->bResyncPending;
    oMsgOpts.bTopicOverRide = true;

    poEalPortCtrlData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalPortCtrlData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalPortCtrlData->nFifoRegVlanTag : *poEalPortCtrlInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalPortCtrlData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalPortCtrlData->nIfRegVlanTag : *poEalPortCtrlInstData->pnIfRegVlanTag;
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
 * @brief Sets the downstream VLAN tag for PORTCTRL instance
 *
 * @param [in] hPortCtrlHandle    PORTCTRL instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_PORTCTRL_CODE
void adi_eal_portCtrlSetDownstreamVlan(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                       ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    /* Extract the unicast instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* If it is a multicast handle, update the handle structure */
    if(poEalPortCtrlData->bIsMulticast)
    {
        poEalPortCtrlData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalPortCtrlData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalPortCtrlData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalPortCtrlInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalPortCtrlInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalPortCtrlInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for Port controller FIFO messages
 *
 * @param [in] hPortCtrlHandle  Port controller interface Handle
 * @param [in] poVlanTag        Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn        If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlSetUpstreamVlan(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    ADI_DBG_REQUIRE(poVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* Add VLAN tag info */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    uint16_t nStartAddr = FIFO_CTAG_CONFIGURATION_NW_ADDR;
    uint32_t nRegData = (bIsVlanEn ? ((uint32_t)poVlanTag->nVlanId & BITM_FIFO_CTAG_CONFIGURATION_CTAG_ID) : 0U) |
                        (poVlanTag->bDEI ?
                         (uint32_t)BITM_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE : 0U) | (((uint32_t)poVlanTag->nPCP & 0x07U) <<
                                 BITP_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY) | (bIsVlanEn ? BITM_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG : 0U);
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.eType = ADI_E2BCORE_RND_WRITE;

    return adi_eal_portCtrlSendRegTransaction(poEalPortCtrlData, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for PORTCTRL transactions
 *
 * @param [in]  hPortCtrlHandle      The PORTCTRL handle
 * @param [in]  bEnable              Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlSetMailboxMode(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    /* Set the mailbox mode flag */
    poEalPortCtrlData->bMailboxMode = bEnable;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for PORTCTRL transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hPortCtrlHandle      The PORTCTRL handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_PORTCTRL_CODE
ADI_EAL_STATUS adi_eal_portCtrlResyncSeqNum(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle)
{
    /* Get the internal data from the handle */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)hPortCtrlHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalPortCtrlData);

    /* Set the sync mode flag */
    poEalPortCtrlData->bResyncPending = true;

    return ADI_EAL_STATUS_SUCCESS;
}

/*================================ CALLBACKS ================================*/
/**
 * @brief The callback for PortCtrl interface
 * @param poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param eType         The type of event this callback is for
 * @param poParam       The data corresponding to this event
 * @param bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param pCbParam      Parameter to be passed back while invoking callback
 */
ADI_EAL_PORTCTRL_CODE
void GpioInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                      void *pCbParam)
{
    /* If pCbParam is NULL, the EAL PORTCTRL interface instance is not opened,
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

/*! \cond private */
/*================================ STATIC CODE ==============================*/
/*! Sends the PORTCTRL transaction */
ADI_EAL_PORTCTRL_CODE
static ADI_EAL_STATUS SendPortCtrlTransaction(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalPortCtrlData->bIsMulticast;

    /* Use the locally created PORTCTRL transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poPortCtrlTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalPortCtrlInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalPortCtrlData, poEalPortCtrlInstData, bIsMcast, &poPortCtrlTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalPortCtrlData->nFifoWriteVlanTag : *poEalPortCtrlInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(poEalPortCtrlInstData->oId.nInstNum,
                                 poEalPortCtrlData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalPortCtrlInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag);

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalPortCtrlData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalPortCtrlData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalPortCtrlData->bIsMulticast ?
                                      poEalPortCtrlData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalPortCtrlInstData->oId.nIntfNum;

        poEalPortCtrlData->bResyncPending = false;

        /* Send the unicast / multicast e2b packet with the FIFO message containing the PORTCTRL transaction */
        eEalStatus = eal_utils_sendFifoFrame(&poEalPortCtrlInstData->oId, &oNwFrameCfg, poPortCtrlTxnCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalPortCtrlData, bIsMcast, NULL) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/*! Sends the register map and port controller transaction */
ADI_EAL_PORTCTRL_CODE
static ADI_EAL_STATUS SendRegAndPortCtrlTransaction(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalPortCtrlData->bIsMulticast;

    /* Use the locally created PORTCTRL transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poPortCtrlTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalPortCtrlInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalPortCtrlData, poEalPortCtrlInstData, bIsMcast, &poPortCtrlTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalPortCtrlData->nFifoWriteVlanTag : *poEalPortCtrlInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(poEalPortCtrlInstData->oId.nInstNum,
                                 poEalPortCtrlData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalPortCtrlInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag
                                );

        /* Set the topics */
        uint32_t anTopics[] =
        {
            /* Interface topic */
            poEalPortCtrlData->bIsMulticast ? poEalPortCtrlData->nIntfTopic : (ADI_E2BCORE_TOPIC_INTF0 + poEalPortCtrlInstData->oId.nIntfNum),
            /* FIFO topic */
            poEalPortCtrlData->bIsMulticast ? poEalPortCtrlData->nFifoTopic : (ADI_E2BCORE_TOPIC_FIFO0 + poEalPortCtrlInstData->oId.nIntfNum)
        };

        /* Send the unicast / multicast e2b packet with the FIFO message containing the PORTCTRL transaction */
        eEalStatus = eal_utils_sendRegmapFifoFrame(&poEalPortCtrlInstData->oId, &oNwFrameCfg, poRegCfg, poPortCtrlTxnCfg,
                     &anTopics[0U]);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalPortCtrlData, bIsMcast, NULL) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/*! Get the slope mask */
ADI_EAL_PORTCTRL_CODE
static uint16_t GetSlopeMask(ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData)
{
    uint16_t nSlopeMask = 0U;

    /* Pins are set to map to unit 0 */
    if((((poPortCtrlData->nPinMask) & 0x0FU) != 0U) && ((((poPortCtrlData->nPinMask) & 0xF0U) >> 4U) == 0U)
            && (((poPortCtrlData->nPinMask & 0xF00U) >> 8U) == 0U))
    {
        nSlopeMask = 0x01u;
    }
    /* Pins are set to map to unit 1 */
    else if(((poPortCtrlData->nPinMask & 0x0FU) == 0U) && (((poPortCtrlData->nPinMask & 0xF0U) >> 4U) != 0U)
            && (((poPortCtrlData->nPinMask & 0xF00U) >> 8U) == 0U))
    {
        nSlopeMask = 0x02u;
    }
    /* Pins are set to map to unit 2 */
    else if(((poPortCtrlData->nPinMask & 0x0FU) == 0U) && (((poPortCtrlData->nPinMask & 0xF0U) >> 4U) == 0U)
            && (((poPortCtrlData->nPinMask & 0xF00U) >> 8U) != 0U))
    {
        nSlopeMask = 0x04u;
    }
    else
    {
        /* Incorrect slope mask */
        ADI_DBG_REQUIRE(nSlopeMask != 0x00u, "Failed to configure slope unit mask: use one PWM unit at a time");
        ADI_DBG_ERROR();
    }

    return nSlopeMask;
}

/*! Calculate the slope and other things from frequency */
ADI_EAL_PORTCTRL_CODE
static void CalculateSlopeFromFreq(ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint16_t *pnSlope, uint16_t *pnSlopeMask,
                                   uint8_t *pnClkDiv)
{
    /* Clock divider setting */
    uint8_t  nClkDiv = 0u;
    /* Reference frequency */
    float nReferFreq = PWM_REFERENCE_FREQ;
    /* PWM time period based on clock divider value */
    uint32_t nPwmTimePeriod = 0u;
    /* float value of Slope */
    float fSlope = 0.0f;
    /* Slope */
    uint16_t nSlope = 0u;
    /* Slope mask */
    uint16_t nSlopeMask = 0u;

    /* If the desired frequency is greater than reference frequency, clock divider setting is 0 */
    if(poPortCtrlData->fPwmFreq >= PWM_REFERENCE_FREQ)
    {
        nClkDiv = 0U;
    }
    /* Else, Keep incrementing the clock divider value and dividing the reference frequency by 2,
       till the desired frequency is in range [reference_freq, reference_freq/2] */
    else if((poPortCtrlData->fPwmFreq > 0.0f))
    {
        uint8_t i = 0u;

        while(nReferFreq != 0.0f)
        {
            i++;

            if((nReferFreq >= (poPortCtrlData->fPwmFreq)) && ((nReferFreq / 2.0f) < (poPortCtrlData->fPwmFreq)))
            {
                nClkDiv = i;
                break;
            }
            else
            {
                nReferFreq /= 2.0f;
            }
        }
    }
    else
    {
        ADI_DBG_ERROR();
    }

    /* Time period based on clock divider = 2^clk_div */
    nPwmTimePeriod = ((uint32_t)1U << (uint32_t)nClkDiv);

    /* Calculate slope.
       Slope = (2^clk_div) * 10ns * 32512 * freq */
    fSlope = ((float)nPwmTimePeriod * 10.0f * 32512.0f * poPortCtrlData->fPwmFreq) / (float)SEC_TO_NS;
    fSlope += 0.5f; /* Rounding off by adding 0.5 and flooring */
    nSlope = (uint16_t)fSlope;

    /* Calculation of slope mask */
    nSlopeMask = GetSlopeMask(poPortCtrlData);

    *pnSlope = nSlope;
    *pnSlopeMask = nSlopeMask;
    *pnClkDiv = nClkDiv;
}

/* Update clk div register from slop mask */
ADI_EAL_PORTCTRL_CODE
static void UpdateClkDivFromSlopeMask(uint16_t nSlopeMask, uint8_t nClkDiv, uint16_t *pnStartAddr, uint32_t *pnMask,
                                      uint32_t *pnData)
{
    /* Set the registers based on slope mask */
    switch(nSlopeMask)
    {
        /* Port controller unit 0 */
        case 0x01u:
        {
            *pnStartAddr = (uint16_t)PORT_UNIT_0_CONTROLS_2_NW_ADDR;
            *pnMask = (uint32_t)0x1FU << BITP_PORT_UNIT_0_CONTROLS_2_DIVIDER_SETTINGS_UNIT_0;
            *pnData = (uint32_t)nClkDiv << BITP_PORT_UNIT_0_CONTROLS_2_DIVIDER_SETTINGS_UNIT_0;
            break;
        }

        /* Port controller unit 1 */
        case 0x02u:
        {
            *pnStartAddr = (uint16_t)PORT_UNIT_1_CONTROLS_2_NW_ADDR;
            *pnMask = (uint32_t)0x1FU << BITP_PORT_UNIT_1_CONTROLS_2_DIVIDER_SETTINGS_UNIT_1;
            *pnData = (uint32_t)nClkDiv << BITP_PORT_UNIT_1_CONTROLS_2_DIVIDER_SETTINGS_UNIT_1;
            break;
        }

        /* Port controller unit 2 */
        case 0x04u:
        {
            *pnStartAddr = (uint16_t)PORT_UNIT_2_CONTROLS_2_NW_ADDR;
            *pnMask = (uint32_t)0x1FU << BITP_PORT_UNIT_2_CONTROLS_2_DIVIDER_SETTINGS_UNIT_2;
            *pnData = (uint32_t)nClkDiv << BITP_PORT_UNIT_2_CONTROLS_2_DIVIDER_SETTINGS_UNIT_2;
            break;
        }

        default:
        {
            /* Invalid event */
            ADI_DBG_ERROR();
            break;
        }
    }
}

/*! Extract PortCtrl status from register data array */
ADI_EAL_PORTCTRL_CODE
static void extractPortCtrlStatus(uint32_t *anData, ADI_EAL_PORTCTRL_STATUS_DATA *poStatusData)
{
    /* Extract the current active output */
    poStatusData->nActiveOutput = (uint16_t)((anData[0U] & BITM_PORT_ACTIVE_OUTPUT_READBACK_ACTIVE_OUTPUT));
}

/*! Lock the mutex of all PORTCTRL instances */
ADI_EAL_PORTCTRL_CODE
static void LockPortCtrlMutex(EAL_PORTCTRL_DATA *poEalPortCtrlData)
{
    /* Loop through all the unicast PORTCTRL instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalPortCtrlData->apoInstData[i]->hPortCtrlMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all PORTCTRL instances */
ADI_EAL_PORTCTRL_CODE
static void UnlockPortCtrlMutex(EAL_PORTCTRL_DATA *poEalPortCtrlData)
{
    /* Loop through all the unicast PORTCTRL instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalPortCtrlData->apoInstData[i]->hPortCtrlMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Set the waiting for RX flag for all the instances of the handle */
ADI_EAL_PORTCTRL_CODE
static void SetWaitingForRx(EAL_PORTCTRL_DATA *poEalPortCtrlData, bool bWaitingForRx)
{
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalPortCtrlInstData->bWaitingForRx = bWaitingForRx;
    }
}

/*! Wait for response */
ADI_EAL_PORTCTRL_CODE
static bool WaitForResponses(EAL_PORTCTRL_DATA *poEalPortCtrlData)
{

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    bool bTimedOut = false;

    /* Loop through all instances and check if response is pending */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];

        /* Check if we are still waiting for response for this instance */
        if(prj_osal_waitSemaphore(poEalPortCtrlInstData->hFifoRespSemaphore) != PRJ_OSAL_STATUS_SUCCESS)
        {
            bTimedOut = true;
            break;
        }
    }

#else

    bool bTimedOut = false;
    bool bAllResponsesReceived = true;
    uint64_t nCurrTime, nStartTime;

    /* Get the start time */
    adi_pal_getCurrTime(&nCurrTime);
    nStartTime = nCurrTime;

    /* Wait for PORTCTRL write to be received by the host interface and
       send back a No-op transaction with the ID sent downstream */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);
        bAllResponsesReceived = true;

        for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalPortCtrlInstData->bWaitingForRx)
            {
                bAllResponsesReceived = false;
                break;
            }
        }

        /* Check if the responses have timed-out */
        if((nCurrTime - nStartTime) > (ADI_EAL_RESP_TIMEOUT_MS * (uint64_t)MS_TO_NS))
        {
            bTimedOut = true;
        }

        /* If responses have timed-out or if we have received all responses,
           break out of this wait */
        if(bTimedOut || bAllResponsesReceived)
        {
            break;
        }
    }

#endif
    return bTimedOut;
}

/*! Save the RX pointer */
ADI_EAL_PORTCTRL_CODE
static void SaveRxPointer(EAL_PORTCTRL_DATA *poEalPortCtrlData, uint32_t **apnPinVals)
{
    /******************  Loop through all the unicast PORTCTRL instances and save the RX pointer ******************/
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];
        poEalPortCtrlInstData->pnPinVals = apnPinVals[i]; /* Pointer to store the pin values - used for GPIO reads */
    }
}

/*! Returns if response is expected based on transaction and instance configuration */
ADI_EAL_PORTCTRL_CODE
static bool IsResponseExpected(EAL_PORTCTRL_INST_DATA *poEalPortCtrlData, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            poEalPortCtrlData->bIsReadTimeTagEccoEn);
}

/*! Returns the number of transactions with response expected */
ADI_EAL_PORTCTRL_CODE
static uint16_t GetNumTxnsWithResp(EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
                                   ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfg,
                                   uint16_t nNumPortCtrlTxns)
{
    uint16_t nNumTxnsWithResp = 0U;

    /* Loop through the transaction configs and get the number of transactions for which response is expected */
    for(uint16_t i = 0U; i < nNumPortCtrlTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalPortCtrlInstData, &paoPortCtrlTxnCfg[i]))
        {
            nNumTxnsWithResp++;
        }
    }

    return nNumTxnsWithResp;
}

/** Handle error callback */
ADI_EAL_PORTCTRL_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalPortCtrlInstData->bIgnoreErr)
    {
        InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_PORTCTRL_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal data of this instance */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* If the read is from interface register map */
    if(poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum))
    {
        HandleInterfaceRegReadCallback(poRegCfg, poEalPortCtrlData, poEalPortCtrlInstData, poRegDat);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle interface register read callback */
ADI_EAL_PORTCTRL_CODE
static void HandleInterfaceRegReadCallback(const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg,
        EAL_PORTCTRL_DATA *poEalPortCtrlData, EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
        ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{

    /* If the read is of the PORTCTRL STATUS registers */
    if((poRegCfg->anStartAddr[0U] == PORT_ACTIVE_OUTPUT_READBACK_NW_ADDR) &&
            (poRegCfg->nLength == 1U))
    {
        /* Process PORTCTRL status data */
        ADI_EAL_PORTCTRL_STATUS_DATA oPortCtrlStatusData;
        extractPortCtrlStatus(poRegCfg->pBuffer, &oPortCtrlStatusData);
        InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_INTF_STATUS_EVT, &oPortCtrlStatusData);
    }
    /* GPIO Read response */
    else if((poRegCfg->anStartAddr[0U] == PORT_OUTPUT_MUX2_NW_ADDR) &&
            (poRegCfg->nLength == 1U))
    {
        /* Copy the readback value */
        if(poEalPortCtrlInstData->pnPinVals != NULL)
        {
            *(poEalPortCtrlInstData->pnPinVals) = ((poRegDat->poInfo->pBuffer[0u] >> BITP_PORT_OUTPUT_MUX2_PORT0_PORT_READBACK) &
                                                   0xFFFu);
        }

        if(
            poEalPortCtrlInstData->bWaitingForRx == false && /* async gpioRead API called */
            poEalPortCtrlInstData->pnPinVals != NULL         /* A non-null pinVals is passed along with gpioRead API */
        )
        {
            InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_GPIO_READ_RESP,
                      poEalPortCtrlInstData->pnPinVals);
        }

        /* Clear the waiting for Rx flag */
        poEalPortCtrlInstData->bWaitingForRx = false;

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        /* Signal the semaphore to indicate response received */
        prj_osal_signalSemaphore(poEalPortCtrlInstData->hFifoRespSemaphore);
#endif
    }
    else
    {
        /* Nothing to do */
    }
}

/** Handle FIFO read callback */
ADI_EAL_PORTCTRL_CODE
/* Handle ECCO response for PortCtrl, similar to UART ECCO handler */
static void HandlePortCtrlEccoResponse(
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat,
    const ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poPortCtrl,
    EAL_PORTCTRL_DATA *poEalPortCtrlData,
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData)
{
    (void)poFifoDat;

    if (poEalPortCtrlInstData->bIsReadTimeTagEccoEn && poPortCtrl->oTxnHdr.bIsGptpTimePresent)
    {
        InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_READ_TIME_TAG_ECCO, poPortCtrl);
    }
   
}
static void HandleFifoReadCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the PORTCTRL transaction configurations */
    const ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poPortCtrl = (const ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the transaction payload */
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD *poPayload = (ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD *)poPortCtrl->pPayload;

    /* Get the internal data of this instance */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    /* ecco */
    if(poPortCtrl->oTxnHdr.nNumPackedTxns == 0U)
    {
        /* Increment the read index */
        poEalPortCtrlInstData->nPortCtrlQRdIdx = EAL_INC_Q_IDX(poEalPortCtrlInstData->nPortCtrlQRdIdx,
                poEalPortCtrlInstData->nPortCtrlCfgQDepth);

        /* Always call ECCO handler before waiting flag logic, as requested */
        HandlePortCtrlEccoResponse(poFifoDat, poPortCtrl, poEalPortCtrlData, poEalPortCtrlInstData);

        /* Return from GpioWriteSync API by clearing the waiting flag */
        if(poEalPortCtrlInstData->bWaitingForRx)
        {
            poEalPortCtrlInstData->bWaitingForRx = false;
#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
            /* Signal the semaphore to indicate response received */
            prj_osal_signalSemaphore(poEalPortCtrlInstData->hFifoRespSemaphore);
#endif
        }
        else
        {
            /* Give callback to application */
            InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_PORTCTRL_WRITE_RESP, poPortCtrl);
        }
    }
    /* Input monitor data */
    else if(poPayload->ePortCtrlCmd == ADI_E2BCORE_PORT_CTRL_CMD_PWM_MONITOR)
    {
        /* Give callback to application */
        InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_INPUT_MONITOR, poPortCtrl);

        /* READ_TIME_TAG event - Time tag data arrives (command type 0x9U) */
        if (poEalPortCtrlInstData->bIsReadTimeTagEn&& poPortCtrl->oTxnHdr.bIsGptpTimePresent)
        {
            /* Invoke callback when time tag event arrives from remote */
            InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_READ_TIME_TAG, poPortCtrl);
        }
    }
    /* Tachometer data */
    else if(poPayload->ePortCtrlCmd == ADI_E2BCORE_PORT_CTRL_CMD_TCH_SPEED)
    {
        /* Give callback to application */
        InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_TACHOMETER, poPortCtrl);
    }
    else
    {
        /* Nothing to do */
    }
}
/** Handle mailbox acknowledgment callback */
ADI_EAL_PORTCTRL_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal PORTCTRL data */
    EAL_PORTCTRL_DATA *poEalPortCtrlData = (EAL_PORTCTRL_DATA *)pCbParam;

    /* Get the PORTCTRL unicast instance data */
    EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[0U];

    ADI_EAL_PORTCTRL_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd = poAckDat->bFd;
    oAckData.nTopic = poAckDat->nTopic;

    InvokeCbk(poEalPortCtrlData, poEalPortCtrlInstData, ADI_EAL_PORTCTRL_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/** Invoke the callback */
ADI_EAL_PORTCTRL_CODE
static void InvokeCbk(EAL_PORTCTRL_DATA *poEalPortCtrlData, EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
                      ADI_EAL_PORTCTRL_CBK_EVT eEvt, const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalPortCtrlInstData->pfunCbk != NULL)
    {
        (poEalPortCtrlInstData->pfunCbk)(poEalPortCtrlInstData->oId.nInstNum, poEalPortCtrlData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_PORTCTRL_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalPortCtrlInstData->nPortCtrlQWrIdx, poEalPortCtrlInstData->nPortCtrlQRdIdx,
                               poEalPortCtrlInstData->nPortCtrlCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_PORTCTRL_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInPortCtrlQueue(&poEalPortCtrlInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_PORTCTRL_CODE
static void CopyTransactionConfigToQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poInstTxnCfg =
            &poEalPortCtrlInstData->paoCfgs[poEalPortCtrlInstData->nPortCtrlQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_PORTCTRL_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_PORTCTRL_DATA *poEalPortCtrlData, bool bIsMcast, uint8_t **apRxBuf)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalPortCtrlData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData = poEalPortCtrlData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poInstTxnCfg =
            &poEalPortCtrlInstData->paoCfgs[poEalPortCtrlInstData->nPortCtrlQWrIdx];
        poInstTxnCfg->pPayload = apRxBuf != NULL ? apRxBuf[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addPortCtrlConfigToQueue(&poEalPortCtrlInstData->oId, false, poInstTxnCfg);
            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalPortCtrlInstData->nPortCtrlQWrIdx = EAL_INC_Q_IDX(poEalPortCtrlInstData->nPortCtrlQWrIdx,
                poEalPortCtrlInstData->nPortCtrlCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_PORTCTRL_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoPortCtrlTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalPortCtrlData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalPortCtrlData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalPortCtrlData, *ppoPortCtrlTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoPortCtrlTxnCfg = &poEalPortCtrlInstData->paoCfgs[poEalPortCtrlInstData->nPortCtrlQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Sends custom PORTCTRL transactions */
ADI_EAL_PORTCTRL_CODE
static ADI_EAL_STATUS SendCustomPortCtrlTransactions(EAL_PORTCTRL_DATA *poEalPortCtrlData,
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfg, uint16_t nNumPortCtrlTxns, uint16_t nNumTxnsWithResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalPortCtrlInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalPortCtrlInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalPortCtrlInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalPortCtrlInstData->nPortCtrlQWrIdx,
                                         poEalPortCtrlInstData->nPortCtrlQRdIdx,
                                         poEalPortCtrlInstData->nPortCtrlCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInPortCtrlQueue(&poEalPortCtrlInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalPortCtrlInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bMailbox       = poEalPortCtrlData->bMailboxMode;
        oFifoMsgOpts.bSync          = poEalPortCtrlData->bResyncPending;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        poEalPortCtrlData->bResyncPending = false;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add PORTCTRL transaction */
        eEalStatus = AddCustomPortCtrlTxnsToMsg(nInstNum, poEalPortCtrlData, poEalPortCtrlInstData, paoPortCtrlTxnCfg,
                                                nNumPortCtrlTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the PORTCTRL EAL mode to operate in custom mode */
        poEalPortCtrlInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/** Add multiple custom PORTCTRL transactions */
ADI_EAL_PORTCTRL_CODE
static ADI_EAL_STATUS AddCustomPortCtrlTxnsToMsg(uint32_t nInstNum, EAL_PORTCTRL_DATA *poEalPortCtrlData,
        EAL_PORTCTRL_INST_DATA *poEalPortCtrlInstData, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfg,
        uint16_t nNumPortCtrlTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add PORTCTRL transaction */
    for(uint16_t i = 0U; i < nNumPortCtrlTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalPortCtrlInstData, &paoPortCtrlTxnCfg[i]))
        {
            /* Since the check has been done above, space has to be available.
               Assert this assumption */
            ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                poEalPortCtrlInstData->nPortCtrlQWrIdx,
                                poEalPortCtrlInstData->nPortCtrlQRdIdx,
                                poEalPortCtrlInstData->nPortCtrlCfgQDepth),
                            "Invalid assumption!");

            /* Copy the given PORTCTRL configuration into the config queue pool */
            (void)ADI_MEMCPY(&poEalPortCtrlInstData->paoCfgs[poEalPortCtrlInstData->nPortCtrlQWrIdx], &paoPortCtrlTxnCfg[i],
                             sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG));

            /* Add the PORTCTRL transaction */
            if(adi_e2bcore_addSinglePortCtrlTxn(nInstNum,
                                                &poEalPortCtrlInstData->paoCfgs[poEalPortCtrlInstData->nPortCtrlQWrIdx]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }

            /* Increment queue index */
            poEalPortCtrlInstData->nPortCtrlQWrIdx = EAL_INC_Q_IDX(poEalPortCtrlInstData->nPortCtrlQWrIdx,
                    poEalPortCtrlInstData->nPortCtrlCfgQDepth);
        }
        else
        {
            /* Just add an PORTCTRL transaction with the config provided */
            if(adi_e2bcore_addSinglePortCtrlTxn(nInstNum, &paoPortCtrlTxnCfg[i]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}


#endif /* ADI_E2B_IFACE_PORT_ENABLED */
/*! \endcond */

/** @} */

/**
 * EOF: www.analog.com
 */
