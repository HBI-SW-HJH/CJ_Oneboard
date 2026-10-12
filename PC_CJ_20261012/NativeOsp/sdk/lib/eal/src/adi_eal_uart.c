/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_uart.c
 * @brief: The EAL APIs for UART interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_UART_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/*! Maximum configurable queue depth for UART transaction config (uint16_t range minus sentinel) */
#define ADI_EAL_UART_MAX_CFG_Q_DEPTH  ((uint16_t)0xFFFFU)

/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalUartData) \
ADI_DBG_REQUIRE((poEalUartData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalUartData)->bInUse == true, "Instance is not opened");

/*=================================== DATA ==================================*/
ADI_EAL_UART_DATA
static ADI_E2BCORE_UART_TXN_PAYLOAD gaoUartCfg[ADI_EAL_UART_CFG_MAX_LENGTH];

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendUartTransaction(
    EAL_UART_DATA *poEalUartData,
    ADI_E2BCORE_UART_TXN_CONFIG *poTxnCfg,
    bool bIsPeriodic,
    uint64_t nPeriod);
static void UpdateUartCfg(uint16_t nLength, ADI_E2BCORE_UART_TXN_PAYLOAD *paoUartCfg, uint8_t *pTxData,
                          EAL_UART_DATA *poEalUartData);
static void extractUartStatus(uint32_t *anData, ADI_EAL_UART_STATUS_DATA *poStatusData);
static void LockUartMutex(EAL_UART_DATA *poEalUartData);
static void UnlockUartMutex(EAL_UART_DATA *poEalUartData);
static void SetWaitingForRx(EAL_UART_DATA *poEalUartData, bool bWaitingForRx);
static bool WaitForResponses(EAL_UART_DATA *poEalUartData);
static bool IsResponseExpected(EAL_UART_INST_DATA *poEalUartData, ADI_E2BCORE_UART_TXN_CONFIG *poTxnCfg);
static uint16_t GetNumTxnsWithResp(EAL_UART_INST_DATA *poEalUartInstData, ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg,
                                   uint16_t nNumUartTxns);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleUartEccoResponse(
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat,
    ADI_E2BCORE_UART_TXN_CONFIG *poUart,
    EAL_UART_DATA *poEalUartData,
    EAL_UART_INST_DATA *poEalUartInstData
);
static void HandleUartRxResponse(
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat,
    ADI_E2BCORE_UART_TXN_CONFIG *poUart,
    EAL_UART_DATA *poEalUartData,
    EAL_UART_INST_DATA *poEalUartInstData
);
static void InvokeCbk(EAL_UART_DATA *poEalUartData, EAL_UART_INST_DATA *poEalUartInstData, ADI_EAL_UART_CBK_EVT eEvt,
                      const void *pArg);
static bool CheckQueueSpaceForRspTxn(EAL_UART_DATA *poEalUartData,
                                     ADI_EAL_UART_TXN_DESC *paoUartIoTxnWithBreak,
                                     uint16_t nNumUartTxns);
static bool IsSpaceAvblInEalTxnQueue(EAL_UART_DATA *poEalUartData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_UART_DATA *poEalUartData);
static void CopyTransactionConfigToQueue(EAL_UART_DATA *poEalUartData, ADI_E2BCORE_UART_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_UART_DATA *poEalUartData, bool bIsMcast);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_UART_DATA *poEalUartData,
        EAL_UART_INST_DATA *poEalUartInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoUartTxnCfg);
static ADI_EAL_STATUS SendCustomUartTransactions(EAL_UART_DATA *poEalUartData, EAL_UART_INST_DATA *poEalUartInstData,
        ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg, uint16_t nNumUartTxns, uint16_t nNumTxnsWithResp);
static ADI_EAL_STATUS SendUartTxnsWithBreakField(EAL_UART_DATA *poEalUartData,
        ADI_EAL_UART_TXN_DESC *paoUartIoData, uint16_t nNumUartTxns);
static ADI_EAL_STATUS AddCustomUartTxnsToMsg(uint32_t nInstNum, EAL_UART_DATA *poEalUartData,
        EAL_UART_INST_DATA *poEalUartInstData, ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg, uint16_t nNumUartTxns);
static ADI_EAL_STATUS AddUartTxnsWithBreakFieldToMsg(uint32_t nInstNum, EAL_UART_DATA *poEalUartData,
        ADI_EAL_UART_TXN_DESC *paoUartIoData, uint16_t nNumUartTxns);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an UART instance
 *
 * @param [in]  poUartCfg          Configuration pointer for EAL UART instance
 * @param [out] phUartHandle       Returns the UART handle for use in other EAL UART APIs
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: UART instance was successfully opened.
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartOpen(ADI_EAL_UART_CFG *poUartCfg, ADI_EAL_UART_HANDLE *phUartHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poUartCfg != NULL, "Pointer to the UART module configuration is NULL");
    ADI_DBG_REQUIRE(phUartHandle != NULL, "Pointer to the UART handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poUartCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poUartCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poUartCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poUartCfg->nInstNum;
    uint32_t nRemoteNum = poUartCfg->nRemoteNum;
    uint32_t nIntfNum = poUartCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_UART,
                    "The given pair of instance, remote and interface numbers is not configured as UART interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poUartCfg->pEalUartMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poUartCfg->nEalUartMemorySize >= ADI_EAL_UART_UNICAST_INST_MEM_SIZE(poUartCfg->nUartCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poUartCfg->pEalUartMemory;

    /* Allocate memory for the UART transaction config queue */
    ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfgs = (ADI_E2BCORE_UART_TXN_CONFIG *)((void *)pMem);
    pMem += (poUartCfg->nUartCfgQDepth * sizeof(ADI_E2BCORE_UART_TXN_CONFIG));

    /* Allocate memory for the EAL UART instance data */
    EAL_UART_INST_DATA *poEalUartInstData = (EAL_UART_INST_DATA *)((void *)pMem);
    pMem += sizeof(EAL_UART_INST_DATA);
    /* ~~~~~~~~~~~ UART instance info ~~~~~~~~~~~ */
    poEalUartInstData->oId.nInstNum      = nInstNum;
    poEalUartInstData->oId.nRemoteNum    = nRemoteNum;
    poEalUartInstData->oId.nIntfNum      = nIntfNum;
    poEalUartInstData->pnIfRegVlanTag    = &poIfConfig->nIfRegVlanTag;
    poEalUartInstData->pnFifoRegVlanTag  = &poIfConfig->nFifoRegVlanTag;
    poEalUartInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;

    /* ~~~~~~~~~~~ UART state info ~~~~~~~~~~~ */
    poEalUartInstData->bIgnoreErr        = false;
    poEalUartInstData->eMode             = ADI_EAL_USAGE_MODE_NORMAL;
    poEalUartInstData->pfunCbk           = NULL; /* Will be filled in adi_eal_uartRegisterCallback */
    poEalUartInstData->bWaitingForRx     = false;
    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    poEalUartInstData->bIsParityEnabled  = false; /* Will be filled in adi_eal_uartConfigure */
    poEalUartInstData->eParityType       = ADI_EAL_EVEN_PARITY; /* Will be filled in adi_eal_uartConfigure */
    poEalUartInstData->bIsUartConfigured = false; /* Will be filled in adi_eal_uartConfigure */
    poEalUartInstData->nNumOfStopBits    = 0U; /* Will be filled in adi_eal_uartConfigure */
    poEalUartInstData->nNumOfDataBits    = 0U; /* Will be filled in adi_eal_uartConfigure */
    uint32_t nUartCtrlReg = 0U;
    EAL_UART_INST_DATA **apoInstData;
    EAL_UART_DATA *poEalUartData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalUartInstData->oId, UART_CONTROL_NW_ADDR, &nUartCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalUartInstData->bIsReadTimeTagEn  = (nUartCtrlReg & BITM_UART_CONTROL_ENABLE_READ_TIME_TAG_ECCO) != 0U;
    poEalUartInstData->nUartCfgQDepth    = poUartCfg->nUartCfgQDepth;
    poEalUartInstData->nUartQWrIdx       = 0U;
    poEalUartInstData->nUartQRdIdx       = 0U;
    poEalUartInstData->paoCfgs           = paoUartTxnCfgs;
    poEalUartInstData->oRxCfg.pPayload   = &poEalUartInstData->oRxPayload;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface.
     * E2BCore uses a circular buffer with a sentinel slot, so depth N needs N+1 entries
     * to provide N usable slots. */
    uint16_t nE2bcoreQDepth = 0U;
    if(poUartCfg->nUartCfgQDepth < ADI_EAL_UART_MAX_CFG_Q_DEPTH)
    {
        nE2bcoreQDepth = (uint16_t)(poUartCfg->nUartCfgQDepth + 1U);
    }
    else
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalUartInstData->oId, (void *)pMem,
            nE2bcoreQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_UART_TXN_CONFIG *) * nE2bcoreQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_UART_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_UART_INST_DATA *);
        *apoInstData = poEalUartInstData;

        /* Allocate memory for the internal EAL UART data structure */
        poEalUartData  = (EAL_UART_DATA *)((void *)pMem);
        pMem += sizeof(EAL_UART_DATA);
        poEalUartData->bIsMulticast  = false;
        poEalUartData->bInUse        = true;
        poEalUartData->bMailboxMode  = false;
        poEalUartData->bResyncPending = false;
        poEalUartData->nIntfTopic    = 0U;
        poEalUartData->nFifoTopic    = 0U;
        poEalUartData->nNumInst      = 1U;
        poEalUartData->apoInstData   = apoInstData;

        /* Initialise the mutex for this UART instance */
        poEalUartInstData->oUartMutexCfg.pMem = &poEalUartInstData->anUartMutexBuffer[0U];
        poEalUartInstData->oUartMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalUartInstData->oUartMutexCfg, &poEalUartInstData->hUartMutex);

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        /* Initialise the semaphore for this UART instance */
        poEalUartInstData->oFifoRespSemaphoreCfg.pMem = &poEalUartInstData->anUartFifoRespSemaphoreBuffer[0U];
        poEalUartInstData->oFifoRespSemaphoreCfg.nMemSize = PRJ_OSAL_SEMAPHORE_MEM_SIZE;
        poEalUartInstData->oFifoRespSemaphoreCfg.bCountingSemaphore = false;
        poEalUartInstData->oFifoRespSemaphoreCfg.nMaxCount = 1U;
        poEalUartInstData->oFifoRespSemaphoreCfg.nInitialCount = 0U;
        (void)prj_osal_createSemaphore(&poEalUartInstData->oFifoRespSemaphoreCfg, &poEalUartInstData->hFifoRespSemaphore);
#endif
        /* Set the callback param in the e2bcore config */
        poIfConfig->pCbParam = poEalUartData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_UART;

        /* Set the handle to the EAL_UART_DATA structure */
        *phUartHandle = (ADI_EAL_UART_HANDLE)poEalUartData;

        /* Provide memory to e2bcore for the RX UART transaction config (containing an UART sample) */
        if(adi_e2bcore_addUartConfigToQueue(&poEalUartInstData->oId, true, &poEalUartInstData->oRxCfg)
                != ADI_E2BCORE_ERR_NONE)
        {
            eStatus = ADI_EAL_STATUS_OPEN_FAILED;
        }
    }

    return eStatus;
}

/**
 * @brief       Closes an opened UART instance
 * @param [in]  hUartHandle       The UART handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: When the UART device was successfully closed.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartClose(ADI_EAL_UART_HANDLE hUartHandle)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalUartData->apoInstData[i]->hUartMutex);
#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        (void)prj_osal_destroySemaphore(poEalUartData->apoInstData[i]->hFifoRespSemaphore);
#endif
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalUartData, 0, sizeof(EAL_UART_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Allows the application to register callback
 * @param [in] hUartHandle        The UART handle
 * @param [in] pfUartCallback     Pointer to the callback function
 * @return Status
 *         - #ADI_EAL_STATUS:   Returns ADI_EAL_STATUS_SUCCESS if callback was registered.
 *
 * @note Call #adi_eal_uartConfigure before UART transactions that depend on correctly
 *       parsed UART RX data in the registered callback.
 *
 * @sa adi_eal_uartConfigure
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartRegisterCallback(ADI_EAL_UART_HANDLE hUartHandle, ADI_EAL_UART_CALLBACK_FUNC pfUartCallback)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(pfUartCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalUartData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* Register the callback function */
    poEalUartInstData->pfunCbk = pfUartCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Allows the application to configure number of data, stop bits and parity for uart
 * @param [in] hUartHandle       The UART handle
 * @param [in] nNumOfStopBits    Number of stop bits
 * @param [in] nNumOfDataBits    Number of data bits (data width)
 * @param [in] bIsParityEnabled  Indicates if parity is enabled
 * @param [in] eParityType       The parity type to be used
 * @return Status
 *         - #ADI_EAL_STATUS:   Returns ADI_EAL_STATUS_SUCCESS if UART stop, data bits and parity was configured successful.
 *
 * @note This API is a prerequisite for correct UART RX data parsing in the registered
 *       callback and for correct TX data formatting for downstream transactions.
 *       The configuration (data bits, stop bits, parity) set by this API is used to
 *       decode incoming UART RX data for async TX response handling and UART read.
 *       If this API is not called before issuing UART transactions, received callback
 *       data may be parsed incorrectly, and UART read may not work as expected.
 *       Call this API after #adi_eal_uartOpen and #adi_eal_uartRegisterCallback,
 *       and before any UART transactions that depend on correctly parsed RX data.
 *
 * @sa adi_eal_uartRegisterCallback
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartConfigure(ADI_EAL_UART_HANDLE hUartHandle, uint8_t nNumOfStopBits, uint8_t nNumOfDataBits,
                                     bool bIsParityEnabled, ADI_EAL_PARITY_TYPE eParityType)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(nNumOfStopBits <= 2u, "Number of stop bits is invalid");
    ADI_DBG_REQUIRE((nNumOfDataBits > 0u) && (nNumOfDataBits <= 8u), "Number of data bits is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalUartData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* Lock the mutex */
    LockUartMutex(poEalUartData);

    /* Configure the UART settings */
    poEalUartInstData->bIsParityEnabled = bIsParityEnabled;
    poEalUartInstData->eParityType = eParityType;
    poEalUartInstData->nNumOfStopBits = nNumOfStopBits;
    poEalUartInstData->nNumOfDataBits = nNumOfDataBits;
    poEalUartInstData->bIsUartConfigured = true;

    /* Unlock the mutex */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief       Create a multicast group of UART interfaces
 *
 * @param [in]  poUartMcastCfg       Configuration for UART multicast group
 * @param [out] phUartMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartCreateMcastGroup(ADI_EAL_UART_MCAST_CFG *poUartMcastCfg,
        ADI_EAL_UART_HANDLE *phUartMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poUartMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phUartMcastHandle != NULL, "Pointer to the UART multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poUartMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poUartMcastCfg->pahUartHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poUartMcastCfg->pUartMcastMem != NULL, "Pointer to memory for UART multicast is NULL");
    ADI_DBG_REQUIRE(poUartMcastCfg->nUartMcastMemSize >= ADI_EAL_UART_MCAST_GROUP_MEM_SIZE(poUartMcastCfg->nNumInst),
                    "Not enough memory allocated for UART multicast group");
    ADI_DBG_REQUIRE(poUartMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poUartMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poUartMcastCfg->pUartMcastMem;
    EAL_UART_INST_DATA **apoInstData = (EAL_UART_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_UART_INST_DATA *) * poUartMcastCfg->nNumInst);

    /* Save the unicast EAL UART instances */
    for(uint16_t i = 0U; i < poUartMcastCfg->nNumInst; i++)
    {
        EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)poUartMcastCfg->pahUartHandles[i];
        apoInstData[i] = poEalUartData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast FIFO topic */
        uint32_t nInstNum = poEalUartData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalUartData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalUartData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poUartMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poUartMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poUartMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poUartMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_UART_DATA *poEalUartMcastData = (EAL_UART_DATA *)((void *)pMem);
    poEalUartMcastData->bIsMulticast = true;
    poEalUartMcastData->bInUse       = true;
    poEalUartMcastData->bMailboxMode = false;
    poEalUartMcastData->bResyncPending = false;
    poEalUartMcastData->nIntfTopic   = poUartMcastCfg->nIntfTopic;
    poEalUartMcastData->nFifoTopic   = poUartMcastCfg->nFifoTopic;
    poEalUartMcastData->nIfRegVlanTag       = (poUartMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
            poUartMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalUartMcastData->nFifoRegVlanTag     = (poUartMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
            poUartMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalUartMcastData->nFifoWriteVlanTag   = (poUartMcastCfg->poFifoWriteVlanTag != NULL) ? eal_utils_convertVlanTags(
            poUartMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalUartMcastData->nNumInst     = poUartMcastCfg->nNumInst;
    poEalUartMcastData->apoInstData  = apoInstData;

    /* Set the handle to the EAL_UART_DATA structure */
    *phUartMcastHandle = (ADI_EAL_UART_HANDLE)poEalUartMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to perform an UART write operation asynchronously
 * @param [in]  hUartHandle       The UART handle
 * @param [in]  pTxData           Pointer to the Tx Data
 * @param [in]  nLength           The write length
 * @param [in]  nId               Identifier for the write request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       : UART write completed successful.
 * @note        <b>E2B_ID Usage Pattern (Async API)</b>:
 *              - E2B_ID is <b>optional</b> for asynchronous write operations
 *              - Set nId = 0xFF to disable E2B_ID
 *              - Set nId to any value (0x00-0xFE) to enable E2B_ID tracking
 *              - When enabled, E2B_ID is copied to the upstream acknowledgment message,
 *                allowing correlation between this downstream command and device response
 *              - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartWrite(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(pTxData != NULL, "Pointer to Tx data is null");
    ADI_DBG_REQUIRE(nLength > 0u, "Write length is invalid");
    ADI_DBG_REQUIRE(nLength <= ADI_EAL_UART_CFG_MAX_LENGTH, "Write length exceeds max length");

    /* Loop through the mutex of all instances and lock it */
    LockUartMutex(poEalUartData);

    /* Update the uart config */
    UpdateUartCfg(nLength, gaoUartCfg, pTxData, poEalUartData);

    /* Set the UART transaction configuration */
    ADI_E2BCORE_UART_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nLength;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.pPayload                  = gaoUartCfg;

    /* Send the UART transaction */
    ADI_EAL_STATUS eEalStatus = SendUartTransaction(poEalUartData, &oTxnCfg, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an UART write operation synchronously
 * @param [in]  hUartHandle       The UART handle
 * @param [in]  pTxData           Pointer to the Tx Data
 * @param [in]  nLength           The write length
 * @param [in]  nId               Identifier for the write request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       : UART write completed successful.
 *
 * @details     * This API blocks till the UART write transaction completes
 *              * Make sure all pending UART transactions are completed before invoking this API
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
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartWriteSync(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(pTxData != NULL, "Pointer to Tx data is null");
    ADI_DBG_REQUIRE(nLength > 0u, "Write length is invalid");
    ADI_DBG_REQUIRE(nLength <= ADI_EAL_UART_CFG_MAX_LENGTH, "Write length exceeds max length");

    /* Loop through the mutex of all instances and lock it */
    LockUartMutex(poEalUartData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalUartData, true);

    /* Update the uart config */
    UpdateUartCfg(nLength, gaoUartCfg, pTxData, poEalUartData);

    /* Set the UART transaction configuration */
    ADI_E2BCORE_UART_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nLength;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.pPayload                  = gaoUartCfg;

    /* Send the UART transaction */
    ADI_EAL_STATUS eEalStatus = SendUartTransaction(poEalUartData, &oTxnCfg, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bool bTimedOut = WaitForResponses(poEalUartData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalUartData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an UART write operation asynchronously after delay
 * @param [in]  hUartHandle       The UART handle
 * @param [in]  pTxData           Pointer to the Tx Data
 * @param [in]  nLength           The write length
 * @param [in]  nDelay            The delay time in ns
 * @param [in]  nId               Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       : UART write read request was sent successful.
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartWriteWithDelay(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength,
        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(pTxData != NULL, "Pointer to Tx data is null");
    ADI_DBG_REQUIRE(nLength > 0u, "Write length is invalid");
    ADI_DBG_REQUIRE(nLength <= ADI_EAL_UART_CFG_MAX_LENGTH, "Write length exceeds max length");

    /* Loop through the mutex of all instances and lock it */
    LockUartMutex(poEalUartData);

    /* Update the uart config */
    UpdateUartCfg(nLength, gaoUartCfg, pTxData, poEalUartData);

    /* Set the UART transaction configuration */
    ADI_E2BCORE_UART_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nTimeDelay          = nDelay;
    oTxnCfg.oTxnHdr.nNumPackedTxns      = nLength;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.pPayload                    = gaoUartCfg;

    /* Send the UART transaction */
    ADI_EAL_STATUS eEalStatus = SendUartTransaction(poEalUartData, &oTxnCfg, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to insert a UART delay command (delay in UART bit-times)
 * @param [in]  hUartHandle       The UART handle
 * @param [in]  nDelayBits        Delay in UART bit-times (12-bit value)
 * @param [in]  nId               Identifier for the delay command request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       : UART delay command request was sent successfully.
 *              - #ADI_EAL_STATUS_INVALID_PARAM : nDelayBits exceeds the 12-bit maximum (0x0FFF).
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartDelayBits(ADI_EAL_UART_HANDLE hUartHandle, uint16_t nDelayBits, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Runtime validation: nDelayBits is a 12-bit field */
        if(nDelayBits > 0x0FFFU)
        {
            eEalStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        /* Loop through the mutex of all instances and lock it */
        LockUartMutex(poEalUartData);

        /* Set the UART delay command payload in a local buffer to avoid races on gaoUartCfg */
        ADI_E2BCORE_UART_TXN_PAYLOAD aoUartCfg[1U];
        (void)ADI_MEMSET(&aoUartCfg[0U], 0, sizeof(ADI_E2BCORE_UART_TXN_PAYLOAD));
        aoUartCfg[0U].eUartCmd = ADI_E2BCORE_UART_CMD_DELAY;
        aoUartCfg[0U].nUartData = nDelayBits;

        /* Set the UART transaction configuration */
        ADI_E2BCORE_UART_TXN_CONFIG oTxnCfg;
        (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
        oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
        oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
        oTxnCfg.oTxnHdr.nNumPackedTxns    = 1U;
        oTxnCfg.oTxnHdr.nE2bId            = nId;
        oTxnCfg.pPayload                  = aoUartCfg;

        /* Send the UART transaction */
        eEalStatus = SendUartTransaction(poEalUartData, &oTxnCfg, false, 0ULL);

        /* Loop through the mutex of all instances and unlock it */
        UnlockUartMutex(poEalUartData);
    } while(false);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an UART write operation asynchronously at a presentation time
 * @param [in]  hUartHandle       The UART handle
 * @param [in]  pTxData           Pointer to the Tx Data
 * @param [in]  nLength           The write length
 * @param [in]  nTimestamp        The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in]  nId               Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       : UART write read request was sent successful.
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartWriteWithPresTs(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength,
        uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(pTxData != NULL, "Pointer to Tx data is null");
    ADI_DBG_REQUIRE(nLength > 0u, "Write length is invalid");
    ADI_DBG_REQUIRE(nLength <= ADI_EAL_UART_CFG_MAX_LENGTH, "Write length exceeds max length");

    /* Loop through the mutex of all instances and lock it */
    LockUartMutex(poEalUartData);

    /* Update the uart config */
    UpdateUartCfg(nLength, gaoUartCfg, pTxData, poEalUartData);

    /* Set the UART transaction configuration */
    ADI_E2BCORE_UART_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent  = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nGptpTime           = nTimestamp;
    oTxnCfg.oTxnHdr.nNumPackedTxns      = nLength;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.pPayload                    = gaoUartCfg;

    /* Send the UART transaction */
    ADI_EAL_STATUS eEalStatus = SendUartTransaction(poEalUartData, &oTxnCfg, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief Allows the application to setup uart periodic write
 * @param hUartHandle       The UART handle
 * @param pTxData           Pointer to the Tx Data that is to be latched
 * @param nLength           The write length
 * @param nId               Identifier for the write read request
 * @return Status
 *         - #ADI_EAL_STATUS                   Returns ADI_EAL_STATUS_SUCCESS if UART setup periodic write frame was sent successful.
 *         - #ADI_EAL_STATUS_QUEUE_FULL        EAL queue is full
 *         - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL E2BCORE buffer not available for this request
 * @note   adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *         to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartSetupPeriodicWrite(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData,
        uint16_t nLength, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(pTxData != NULL, "Pointer to Tx data is null");
    ADI_DBG_REQUIRE(nLength > 0u, "Write length is invalid");
    ADI_DBG_REQUIRE(nLength <= ADI_EAL_UART_CFG_MAX_LENGTH, "Write length exceeds max length");

    /* Loop through the mutex of all instances and lock it */
    LockUartMutex(poEalUartData);

    /* Update the uart config */
    UpdateUartCfg(nLength, gaoUartCfg, pTxData, poEalUartData);

    /* Set the UART transaction configuration */
    ADI_E2BCORE_UART_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nLength;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.pPayload                  = gaoUartCfg;

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalUartData->bIsMulticast;

    /* Use the locally created UART transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poUartTxnCfg = &oTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalUartInstData, poUartTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalUartData, poEalUartInstData, bIsMcast, &poUartTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalUartData, bIsMcast) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }

    } while(false);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Switch the mode of all the instances to periodic */
        for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
        {
            EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];
            if(adi_e2bcore_addUartConfigToQueue(&(poEalUartInstData->oId), false, poUartTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
            poEalUartInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
        }
    }

    /* Loop through the mutex of all instances and unlock it */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief       Gets the UART interface status
 *
 * @param [in]  hUartHandle     The UART interface Handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartReadStatus(ADI_EAL_UART_HANDLE hUartHandle)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    bool bIsMcast = poEalUartData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalUartData->nIfRegVlanTag : *poEalUartInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalUartData->nIntfTopic;
        anTopics[1U] = poEalUartData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalUartInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalUartInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalUartInstData->oId, UART_STATS_NW_ADDR, 1U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief To write/read to/from interface fifo based on the transaction config
 *
 * @param [in]  hUartHandle     The UART interface Handle
 * @param [in]  paoUartTxnCfg   UART transaction config
 * @param [in]  nNumUartTxns    Number of UART transactions
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartCustomWriteRead(ADI_EAL_UART_HANDLE hUartHandle,
        ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg, uint16_t nNumUartTxns)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(paoUartTxnCfg != NULL, "UART transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumUartTxns > 0U, "Number of UART transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalUartData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* Lock the mutex */
    LockUartMutex(poEalUartData);

    /* Get the number of transactions that expects a response */
    uint16_t nNumTxnsWithResp = GetNumTxnsWithResp(poEalUartInstData, paoUartTxnCfg, nNumUartTxns);

    /* Send the custom UART transactions */
    eEalStatus = SendCustomUartTransactions(poEalUartData, poEalUartInstData, paoUartTxnCfg, nNumUartTxns,
                                            nNumTxnsWithResp);

    /* Unlock the mutex */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief Function to send multiple UART write/read transactions with break field support
 *
 * @param [in] hUartHandle   UART interface handle (can be unicast or multicast)
 * @param [in] paoUartIoData Pointer to UART IO transaction descriptors
 * @param [in] nNumUartTxns  Number of UART transactions (descriptors)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartMultiTxnWrite(ADI_EAL_UART_HANDLE hUartHandle,
        ADI_EAL_UART_TXN_DESC *paoUartIoData, uint16_t nNumUartTxns)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(paoUartIoData != NULL, "UART transaction pointer is NULL");
    ADI_DBG_REQUIRE(nNumUartTxns > 0U, "Number of UART transactions is invalid");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Lock the mutex */
    LockUartMutex(poEalUartData);

    /* Check if there is enough space for all transactions in all unicast instances provided by the handle */
    bool bNoSpace = CheckQueueSpaceForRspTxn(poEalUartData, paoUartIoData, nNumUartTxns);

    /* Proceed if there is enough space in the config queue */
    if(!bNoSpace)
    {
        eEalStatus = SendUartTxnsWithBreakField(poEalUartData, paoUartIoData, nNumUartTxns);
    }
    else
    {
        eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
    }

    /* Unlock the mutex */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief Send a register map transaction to the UART interface or the corresponding FIFO registers
 *
 * @param [in]    hUartHandle               The UART interface Handle
 * @param [in]    bIsFifoRegTransaction     Is this for reg transaction for FIFO
 * @param [inout] poRegCfg                  Register read details
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartSendRegTransaction(ADI_EAL_UART_HANDLE hUartHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalUartInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalUartInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalUartInstData->oId.nIntfNum;

    bool bIsMcast = poEalUartData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bMailbox       = poEalUartData->bMailboxMode;
    oMsgOpts.bSync          = poEalUartData->bResyncPending;
    oMsgOpts.bTopicOverRide = true;

    poEalUartData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalUartData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalUartData->nFifoRegVlanTag : *poEalUartInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalUartData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalUartData->nIfRegVlanTag : *poEalUartInstData->pnIfRegVlanTag;
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
 * @brief       Resets the UART transaction queue and goes back to a clean state
 *
 * @param  [in] hUartHandle      The Uart handle
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Uart reset successful.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartResetQueue(ADI_EAL_UART_HANDLE hUartHandle)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalUartData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* Lock the mutex */
    LockUartMutex(poEalUartData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearUartConfigQueue(&poEalUartInstData->oId);
        poEalUartInstData->nUartQWrIdx = 0U;
        poEalUartInstData->nUartQRdIdx = 0U;
        poEalUartInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
        poEalUartInstData->bWaitingForRx = false;

        /* Ignore errors */
        poEalUartInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  UART_CONTROL_NW_ADDR;
        uint32_t nMask      =  BITM_UART_CONTROL_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      =  BITM_UART_CONTROL_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_uartSendRegTransaction((ADI_EAL_UART_HANDLE)poEalUartData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an UART NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalUartInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalUartInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockUartMutex(poEalUartData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for UART instance
 *
 * @param [in] hUartHandle        UART instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 */
ADI_EAL_UART_CODE
void adi_eal_uartSetDownstreamVlan(ADI_EAL_UART_HANDLE hUartHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                   ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    /* Extract the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* If it is a multicast handle, update the handle structure */
    if(poEalUartData->bIsMulticast)
    {
        poEalUartData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalUartData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalUartData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalUartInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalUartInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalUartInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for UART FIFO messages
 *
 * @param [in] hUartHandle  UART interface Handle
 * @param [in] poVlanTag    Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn    If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartSetUpstreamVlan(ADI_EAL_UART_HANDLE hUartHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

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

    return adi_eal_uartSendRegTransaction(poEalUartData, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for UART transactions
 *
 * @param [in]  hUartHandle      The UART handle
 * @param [in]  bEnable          Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartSetMailboxMode(ADI_EAL_UART_HANDLE hUartHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    /* Set the mailbox mode flag */
    poEalUartData->bMailboxMode = bEnable;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for UART transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hUartHandle      The UART handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartResyncSeqNum(ADI_EAL_UART_HANDLE hUartHandle)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    /* Set the sync mode flag */
    poEalUartData->bResyncPending = true;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables or disables READ_TIME_TAG_ECCO feature for UART
 *
 * @param [in]  hUartHandle    UART handle
 * @param [in]  bEnable        true to enable, false to disable
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Feature enabled/disabled
 *              - #ADI_EAL_STATUS_INVALID_PARAM: READ_TIME_TAG_ECCO feature is not supported for multicast handles
 *              - Other error codes propagated from
 *                #adi_eal_uartSendRegTransaction (for example
 *                #ADI_EAL_STATUS_START_FAILED, #ADI_EAL_STATUS_NTW_BUFF_FULL,
 *                #ADI_EAL_STATUS_REGMAPIO_FAILED).
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartSetReadTimeTagEcco(ADI_EAL_UART_HANDLE hUartHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;
    EAL_UART_INST_DATA *poEalUartInstData;
    uint32_t nMask = BITM_UART_CONTROL_ENABLE_READ_TIME_TAG_ECCO;
    uint32_t nData = bEnable ? nMask : 0U;
    uint16_t nAddr = UART_CONTROL_NW_ADDR;
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;

    BASIC_HANDLE_CHECK(poEalUartData);
    poEalUartInstData = poEalUartData->apoInstData[0U];

    do
    {
        /* READ_TIME_TAG_ECCO is maintained per unicast instance, so multicast handles are rejected. */
        if(poEalUartData->bIsMulticast)
        {
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        /* Program only the ECCO enable bit and preserve the remaining UART control register fields. */
        LockUartMutex(poEalUartData);

        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;

        eStatus = adi_eal_uartSendRegTransaction(hUartHandle, false, &oRegCfg);

        if(eStatus == ADI_EAL_STATUS_SUCCESS)
        {
            /* Mirror the hardware setting locally so response handling uses the updated behavior immediately. */
            poEalUartInstData->bIsReadTimeTagEn = bEnable;
        }

        UnlockUartMutex(poEalUartData);
    } while(false);

    return eStatus;
}

/**
 * @brief       Enables or disables the UART RX and TX interfaces
 * @details     This function allows independent control of the UART receive and transmit
 *              interfaces by setting the appropriate bits in the UART CONTROL register.
 *              Both interfaces can be enabled or disabled independently. This API supports
 *              both unicast and multicast handles, allowing simultaneous configuration of
 *              multiple UART interfaces.
 *
 * @param [in]  hUartHandle      The UART handle (unicast or multicast)
 * @param [in]  bEnableRx        Flag to enable (true) or disable (false) UART RX interface
 * @param [in]  bEnableTx        Flag to enable (true) or disable (false) UART TX interface
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: UART interface enable/disable was successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full
 *              - #ADI_EAL_STATUS_QUEUE_FULL: UART transaction queue full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL: E2BCore internal buffer full
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_UART_CODE
ADI_EAL_STATUS adi_eal_uartSetInterfaceEnable(ADI_EAL_UART_HANDLE hUartHandle, bool bEnableRx, bool bEnableTx)
{
    /* Get the internal data from the handle */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)hUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalUartData);

    /* Setup register write configuration for UART CONTROL register */
    uint16_t nStartAddr = UART_CONTROL_NW_ADDR;
    uint32_t nMask = BITM_UART_CONTROL_RX_UART_ENABLE | BITM_UART_CONTROL_TX_UART_ENABLE;
    uint32_t nRegData = ((bEnableRx == true ? (uint32_t)ENUM_UART_CONTROL_RX_UART_ENABLE_ENABLE :
                          (uint32_t)ENUM_UART_CONTROL_RX_UART_ENABLE_DISABLE) << BITP_UART_CONTROL_RX_UART_ENABLE) |
                        ((bEnableTx == true ? (uint32_t)ENUM_UART_CONTROL_TX_UART_ENABLE_ENABLE :
                          (uint32_t)ENUM_UART_CONTROL_TX_UART_ENABLE_DISABLE) << BITP_UART_CONTROL_TX_UART_ENABLE);

    /* Configure the register map IO structure for mask write operation */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.anMask = &nMask;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    /* Send the register write transaction */
    return adi_eal_uartSendRegTransaction(hUartHandle, false, &oRegCfg);
}

/*============================ CALLBACKS ============================*/
/**
 * @brief The callback for UART interface
 * @param poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param eType         The type of event this callback is for
 * @param poParam       The data corresponding to this event
 * @param bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param pCbParam      Parameter to be passed back while invoking callback
 *
 * @note For callback events that depend on decoded UART RX data (e.g.,
 *       #ADI_EAL_UART_CBK_EVT_UART_WRITE_RESP, #ADI_EAL_UART_CBK_EVT_UART_READ_RESP),
 *       #adi_eal_uartConfigure must be called before the corresponding UART transactions.
 *
 * @sa adi_eal_uartConfigure, adi_eal_uartRegisterCallback
 */
ADI_EAL_UART_CODE
void UartInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                      void *pCbParam)
{
    /* If pCbParam is NULL, the EAL UART interface instance is not opened,
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

/******************************* STATIC FUNCTIONS *************************************/
/*! \cond PRIVATE */

/*! Sends the UART transaction */
ADI_EAL_UART_CODE
static ADI_EAL_STATUS SendUartTransaction(
    EAL_UART_DATA *poEalUartData,
    ADI_E2BCORE_UART_TXN_CONFIG *poTxnCfg,
    bool bIsPeriodic,
    uint64_t nPeriod)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalUartData->bIsMulticast;

    /* Use the locally created UART transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poUartTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalUartInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalUartData, poEalUartInstData, bIsMcast, &poUartTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalUartData->nFifoWriteVlanTag : *poEalUartInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(poEalUartInstData->oId.nInstNum,
                                 poEalUartData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalUartInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag
                                );

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalUartData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalUartData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalUartData->bIsMulticast ?
                                      poEalUartData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalUartInstData->oId.nIntfNum;

        poEalUartData->bResyncPending = false;

        if(!bIsPeriodic)
        {
            /* Send the unicast / multicast e2b packet with the FIFO message containing the UART transaction */
            eEalStatus = eal_utils_sendFifoFrame(&poEalUartInstData->oId, &oNwFrameCfg, poUartTxnCfg, &oFifoMsgOpts);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalUartData, bIsMcast) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/**
 * @brief Updates the uart config with populating the uart data with start, stop and parity bits
 * @param nLength       The write length
 * @param paoUartCfg    Pointer to the array of uart configs
 * @param pTxData       Pointer to Tx data
 * @param poEalUartData Pointer to the uart data
 */
ADI_EAL_UART_CODE
static void UpdateUartCfg(uint16_t nLength, ADI_E2BCORE_UART_TXN_PAYLOAD *paoUartCfg, uint8_t *pTxData,
                          EAL_UART_DATA *poEalUartData)
{
    /* Get the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    for(uint32_t nLC = 0; nLC < nLength; nLC++)
    {
        /* Set the parameters for the UART transaction */
        (void) ADI_MEMSET(&paoUartCfg[nLC], 0, sizeof(ADI_E2BCORE_UART_TXN_PAYLOAD));

        /* Length of Start bit */
        uint8_t nIndex = 1u;

        /* Use only data length number of bits */
        pTxData[nLC] &= ((1U << poEalUartInstData->nNumOfDataBits) - 1U);

        /* Set the data in the appropriate location */
        paoUartCfg[nLC].nUartData = (uint16_t)pTxData[nLC] << nIndex;
        nIndex += poEalUartInstData->nNumOfDataBits;

        /* Set the parity bit */
        if(poEalUartInstData->bIsParityEnabled)
        {
            paoUartCfg[nLC].nUartData |= (uint16_t)eal_utils_computeParity(poEalUartInstData->eParityType, pTxData[nLC]) << nIndex;
            nIndex++;
        }

        /* Set the stop bit(s) */
        if(poEalUartInstData->nNumOfStopBits == 1u)
        {
            paoUartCfg[nLC].nUartData |= ((uint16_t)1u << nIndex);
            nIndex++;
        }
        else if(poEalUartInstData->nNumOfStopBits == 2u)
        {
            paoUartCfg[nLC].nUartData |= ((uint16_t)3u << nIndex);
            nIndex += 2u;
        }
        else
        {
            /* Nothing to do */
        }

        /* Set the data length */
        paoUartCfg[nLC].nUartLen = nIndex;
    }
}

/**
 * @brief       Extract UART status from register data array
 *
 * @param [in]  anData      Register data array
 * @param [out] poStatusData UART status data
 */
ADI_EAL_UART_CODE
static void extractUartStatus(uint32_t *anData, ADI_EAL_UART_STATUS_DATA *poStatusData)
{
    /* Extract the number of read and write bytes */
    poStatusData->nNumTxBytes = (uint16_t)((anData[0U] & 0xFFFF0000U) >> 16U);
    poStatusData->nNumRxBytes = (uint16_t)(anData[0U] & 0xFFFFU);
}

/*! Lock the mutex of all UART instances */
ADI_EAL_UART_CODE
static void LockUartMutex(EAL_UART_DATA *poEalUartData)
{
    /* Loop through all the unicast UART instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalUartData->apoInstData[i]->hUartMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all UART instances */
ADI_EAL_UART_CODE
static void UnlockUartMutex(EAL_UART_DATA *poEalUartData)
{
    /* Loop through all the unicast UART instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalUartData->apoInstData[i]->hUartMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Set the waiting for RX flag for all the instances of the handle */
ADI_EAL_UART_CODE
static void SetWaitingForRx(EAL_UART_DATA *poEalUartData, bool bWaitingForRx)
{
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalUartInstData->bWaitingForRx = bWaitingForRx;
    }
}

/*! Wait for response */
ADI_EAL_UART_CODE
static bool WaitForResponses(EAL_UART_DATA *poEalUartData)
{

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    bool bTimedOut = false;

    /* Loop through all instances and check if response is pending */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];

        /* Check if we are still waiting for response for this instance */
        if(prj_osal_waitSemaphore(poEalUartInstData->hFifoRespSemaphore) != PRJ_OSAL_STATUS_SUCCESS)
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

    /* Wait for UART write to be received by the host interface and
       send back a No-op transaction with the ID sent downstream */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);
        bAllResponsesReceived = true;

        for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalUartInstData->bWaitingForRx)
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

/*! Returns if response is expected based on transaction and instance configuration */
ADI_EAL_UART_CODE
static bool IsResponseExpected(EAL_UART_INST_DATA *poEalUartData, ADI_E2BCORE_UART_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            poEalUartData->bIsReadTimeTagEn);
}

/*! Returns the number of transactions with response expected */
ADI_EAL_UART_CODE
static uint16_t GetNumTxnsWithResp(EAL_UART_INST_DATA *poEalUartInstData, ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg,
                                   uint16_t nNumUartTxns)
{
    uint16_t nNumTxnsWithResp = 0U;

    /* Loop through the transaction configs and get the number of transactions for which response is expected */
    for(uint16_t i = 0U; i < nNumUartTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalUartInstData, &paoUartTxnCfg[i]))
        {
            nNumTxnsWithResp++;
        }
    }

    return nNumTxnsWithResp;
}

/*! Checks whether there is enough space in the queues for all instances, when a response for a transaction is expected */
ADI_EAL_UART_CODE
static bool CheckQueueSpaceForRspTxn(EAL_UART_DATA *poEalUartData,
                                     ADI_EAL_UART_TXN_DESC *paoUartIoTxnWithBreak,
                                     uint16_t nNumUartTxns)
{
    bool bNoSpace = false;

    uint16_t nNumTxnsWithResp = 0U;

    /* Loop through all transactions, and count the number of transactions
       that needs a response due to the presence of e2bid */
    for(uint16_t nNumTxn = 0U; nNumTxn < nNumUartTxns; nNumTxn++)
    {
        if(paoUartIoTxnWithBreak[nNumTxn].bIsE2bIdPresent)
        {
            nNumTxnsWithResp++;
        }
    }

    /* Loop through each instance in the handle and check whether we have enough space in the queue */
    for(uint16_t nInstanceNum = 0U; nInstanceNum < poEalUartData->nNumInst; nInstanceNum++)
    {
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[nInstanceNum];
        uint16_t nNumTxnsWithRspForCurrInst = 0U;

        /* If timetagging is not enabled for this instance,
          then we only need responses for the transactions that has an e2bid,
          otherwise, all the transactions will trigger a response */
        if(!poEalUartInstData->bIsReadTimeTagEn)
        {
            nNumTxnsWithRspForCurrInst = nNumTxnsWithResp;
        }
        else
        {
            nNumTxnsWithRspForCurrInst = nNumUartTxns;
        }

        if((EAL_SLOTS_FREE_IN_Q(poEalUartInstData->nUartQWrIdx, poEalUartInstData->nUartQRdIdx,
                                poEalUartInstData->nUartCfgQDepth) < nNumTxnsWithRspForCurrInst)
                || (adi_e2bcore_slotsLeftInUartQueue(&poEalUartInstData->oId) < nNumTxnsWithRspForCurrInst))
        {
            /* Not enough space in the queue(s) */
            bNoSpace = true;
            break;
        }
    }

    return bNoSpace;
}

/**
 * @brief       Invokes timestamped ECCO acquisition callback when timetag feature is enabled
 * @details     Sends READ_TIME_TAG_ECCO callback to application only when both the feature is enabled
 *              and gPTP acquisition timestamp is available in the response. This ensures timestamped
 *              ECCO events are delivered to the application for time-critical operations.
 *
 * @param [in]  poEalUartData          Pointer to UART EAL instance data
 * @param [in]  poEalUartInstData      Pointer to UART unicast instance data
 * @param [in]  poUartRxData           Pointer to received UART RX data containing gPTP timestamp
 *
 * @note        This is a helper function to reduce cyclomatic complexity of HandleUartEccoResponse.
 *              It consolidates the condition check and callback invocation for the timestamped ECCO feature.
 */
ADI_EAL_UART_CODE
static void InvokeEccoTimeTagCallback(EAL_UART_DATA *poEalUartData, EAL_UART_INST_DATA *poEalUartInstData,
                                      const ADI_EAL_UART_RX_DATA *poUartRxData)
{
    /* Send timestamped callback only when feature is enabled and gPTP timestamp is available */
    if(poEalUartInstData->bIsReadTimeTagEn && poUartRxData->bIsGptpAcquisitionTimePresent)
    {
        InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_READ_TIME_TAG_ECCO, poUartRxData);
    }
}

/** Handle error callback */
ADI_EAL_UART_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalUartInstData->bIgnoreErr)
    {
        InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_UART_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal data of this instance */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* If the read is of the UART STATUS registers */
    if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == UART_STATS_NW_ADDR) &&
            (poRegCfg->nLength == 1U))
    {
        /* Process UART status data */
        ADI_EAL_UART_STATUS_DATA oUartStatusData;
        extractUartStatus(poRegCfg->pBuffer, &oUartStatusData);
        InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_INTF_STATUS_EVT, &oUartStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_UART_CODE
static void HandleFifoReadCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the UART transaction configurations */
    ADI_E2BCORE_UART_TXN_CONFIG *poUart = (ADI_E2BCORE_UART_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal data of this instance */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)pCbParam;

    /* Extract the unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    /* ecco */
    if(poUart->oTxnHdr.nNumPackedTxns == 0U)
    {
        HandleUartEccoResponse(poFifoDat, poUart, poEalUartData, poEalUartInstData);
    }
    /* UART RX data */
    else
    {
        HandleUartRxResponse(poFifoDat, poUart, poEalUartData, poEalUartInstData);
    }
}

/** Handle Mailbox ACK callback */
ADI_EAL_UART_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal UART data */
    EAL_UART_DATA *poEalUartData = (EAL_UART_DATA *)pCbParam;

    /* Get the UART unicast instance data */
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];

    ADI_EAL_UART_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd = poAckDat->bFd;
    oAckData.nTopic = poAckDat->nTopic;

    InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/** Handle Uart Ecco response */
ADI_EAL_UART_CODE
static void HandleUartEccoResponse(
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat,
    ADI_E2BCORE_UART_TXN_CONFIG *poUart,
    EAL_UART_DATA *poEalUartData,
    EAL_UART_INST_DATA *poEalUartInstData
)
{
    /* Structure to hold the UART RX data to be sent to application */
    ADI_EAL_UART_RX_DATA oUartRxData;
    (void)ADI_MEMSET(&oUartRxData, 0, sizeof(oUartRxData));
    oUartRxData.bIsE2bIdPresent = true;
    oUartRxData.bIsGptpAcquisitionTimePresent = poUart->oTxnHdr.bIsGptpTimePresent;
    oUartRxData.nGptpAcquisitionTime = poUart->oTxnHdr.nGptpTime;
    oUartRxData.nE2bId = poUart->oTxnHdr.nE2bId;
    oUartRxData.nReadLength = 0; // No data for ECCO
    oUartRxData.pRxData = NULL;

    if(poEalUartInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
    {
        /* Increment the read index */
        poEalUartInstData->nUartQRdIdx = EAL_INC_Q_IDX(poEalUartInstData->nUartQRdIdx,
                                         poEalUartInstData->nUartCfgQDepth);

        /* Return from the synchronous API */
        if(poEalUartInstData->bWaitingForRx)
        {
            poEalUartInstData->bWaitingForRx = false;

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
            /* Signal the semaphore to indicate response received */
            prj_osal_signalSemaphore(poEalUartInstData->hFifoRespSemaphore);
#endif
        }
        /* Give callback */
        else
        {
            /* Invoke timestamped ECCO callback when timetag feature is enabled */
            InvokeEccoTimeTagCallback(poEalUartData, poEalUartInstData, &oUartRxData);
            InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_UART_WRITE_RESP, &oUartRxData);
        }
    }
    else if(poEalUartInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
    {
        /* Invoke timestamped ECCO callback when timetag feature is enabled */
        InvokeEccoTimeTagCallback(poEalUartData, poEalUartInstData, &oUartRxData);
        /* Give callback */
        InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_PERIODIC_RX, &oUartRxData);
        /* Add the config back to the queue */
        if(adi_e2bcore_addUartConfigToQueue(&poEalUartInstData->oId, false, poUart) != ADI_E2BCORE_ERR_NONE)
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to add uart config to queue");
        }
    }
    else if(poEalUartInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
    {
        /* Invoke timestamped ECCO callback when timetag feature is enabled */
        InvokeEccoTimeTagCallback(poEalUartData, poEalUartInstData, &oUartRxData);
        /* Give callback */
        InvokeCbk(poEalUartData, poEalUartInstData,  ADI_EAL_UART_CBK_EVT_UART_FIFO_READ, poFifoDat);
        /* Increment the read index */
        poEalUartInstData->nUartQRdIdx = EAL_INC_Q_IDX(poEalUartInstData->nUartQRdIdx, poEalUartInstData->nUartCfgQDepth);
    }
    else
    {
        /* Nothing to do */
    }
}

/** Handle Uart RX response */
ADI_EAL_UART_CODE
static void HandleUartRxResponse(
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat,
    ADI_E2BCORE_UART_TXN_CONFIG *poUart,
    EAL_UART_DATA *poEalUartData,
    EAL_UART_INST_DATA *poEalUartInstData
)
{
    /* Structure to hold the UART RX data to be sent to application */
    ADI_EAL_UART_RX_DATA oUartRxData;
    (void)ADI_MEMSET(&oUartRxData, 0, sizeof(oUartRxData));

    /* Get the UART payload */
    ADI_E2BCORE_UART_TXN_PAYLOAD *poRxPayload = (ADI_E2BCORE_UART_TXN_PAYLOAD *)poUart->pPayload;

    /* Received read bits length */
    uint8_t nNumReadBits = poRxPayload->nUartLen;

    /* Get the UART data by stripping out the start bit */
    uint8_t nUartData = (uint8_t)(poRxPayload->nUartData >> 1u);

    /* Get the UART data length by removing the length of
       start bit, stop bit(s) and parity bit from the total length */
    poRxPayload->nUartLen = (poRxPayload->nUartLen - 1U - poEalUartInstData->nNumOfStopBits -
                             (poEalUartInstData->bIsParityEnabled ? 1U : 0U));

    /* Extract only the data bits based on the length computed above */
    nUartData &= ((1U << poRxPayload->nUartLen) - 1U);

    /* Extract the parity bit */
    uint8_t nParityBit = (uint8_t)((poRxPayload->nUartData >> (poRxPayload->nUartLen + 1U)) & 0x01u);

    /* Get the position of stop bit */
    uint8_t nStopBitsPos = 1U /* Skip start bit */ +
                           poRxPayload->nUartLen /* Skip the data bits */ +
                           (poEalUartInstData->bIsParityEnabled ? 1U : 0U) /* Skip the parity bit */;

    /* Extract the stop bit */
    uint8_t nStopBits = (uint8_t)(poRxPayload->nUartData >> nStopBitsPos);

    /* Get the expected stop bits value */
    uint8_t nExpStopBits = (1U << poEalUartInstData->nNumOfStopBits) - 1U;

    /* Start bit should always be 0 */
    bool bStartBitCorrupted = (poRxPayload->nUartData & 1u) != 0u;

    /* Check the parity */
    bool bParityCheckFailed = poEalUartInstData->bIsParityEnabled ?
                              (eal_utils_computeParity(poEalUartInstData->eParityType, nUartData) != nParityBit) :
                              false;

    /* Check the stop bit */
    bool bStopBitCorrupted = (nStopBits & nExpStopBits) != nExpStopBits;

    do
    {
        /* If start bit or stop bit(s) are corrupted */
        if(bStartBitCorrupted || bStopBitCorrupted)
        {
            /* Incase of detection of corrupted, send the callback to application with unmodified read byte */
            ADI_EAL_UART_RX_ERROR_DATA oUartRxErrorData;
            (void)ADI_MEMSET(&oUartRxErrorData, 0, sizeof(oUartRxErrorData));
            oUartRxErrorData.bIsE2bIdPresent = false; // No e2b_id for RX data non ecco frame
            oUartRxErrorData.bIsGptpAcquisitionTimePresent = poUart->oTxnHdr.bIsGptpTimePresent;
            oUartRxErrorData.nGptpAcquisitionTime = poUart->oTxnHdr.nGptpTime;
            oUartRxErrorData.nReadLength = 1U; // Always 1 byte of data
            oUartRxErrorData.nNumReadBits = nNumReadBits;
            oUartRxErrorData.pRxData = &poRxPayload->nUartData; // Send unmodified data including start, stop and parity bits
            InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_RX_DATA_CORRUPT, &oUartRxErrorData);
            break;
        }

        /* If parity check failed */
        if(bParityCheckFailed)
        {
            ADI_EAL_UART_RX_ERROR_DATA oUartRxErrorData;
            (void)ADI_MEMSET(&oUartRxErrorData, 0, sizeof(oUartRxErrorData));
            oUartRxErrorData.bIsE2bIdPresent = false; // No e2b_id for RX data non ecco frame
            oUartRxErrorData.bIsGptpAcquisitionTimePresent = poUart->oTxnHdr.bIsGptpTimePresent;
            oUartRxErrorData.nGptpAcquisitionTime = poUart->oTxnHdr.nGptpTime;
            oUartRxErrorData.nReadLength = 1U; // Always 1 byte of data
            oUartRxErrorData.nNumReadBits = nNumReadBits;
            oUartRxErrorData.pRxData = &poRxPayload->nUartData; // Send unmodified data including start, stop and parity bits
            InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_RX_PARITY_ERROR, &oUartRxErrorData);
            break;
        }

        if(poEalUartInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
        {
            /* Give callback to application */
            oUartRxData.bIsE2bIdPresent = false; // No e2b_id for RX data non ecco frame
            oUartRxData.bIsGptpAcquisitionTimePresent = poUart->oTxnHdr.bIsGptpTimePresent;
            oUartRxData.nGptpAcquisitionTime = poUart->oTxnHdr.nGptpTime;
            oUartRxData.nReadLength = 1U; // Always 1 byte of data
            oUartRxData.pRxData = &nUartData;
            InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_UART_READ_RESP, &oUartRxData);
        }
        else if(poEalUartInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
        {
            /* Give callback to application */
            oUartRxData.bIsE2bIdPresent = false; // No e2b_id for RX data non ecco frame
            oUartRxData.bIsGptpAcquisitionTimePresent = poUart->oTxnHdr.bIsGptpTimePresent;
            oUartRxData.nGptpAcquisitionTime = poUart->oTxnHdr.nGptpTime;
            oUartRxData.nReadLength = 1U; // Always 1 byte of data
            oUartRxData.pRxData = &nUartData;
            InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_PERIODIC_RX, &oUartRxData);
        }
        else if(poEalUartInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
        {
            /* Give callback to application */
            poRxPayload->nUartData = nUartData;
            InvokeCbk(poEalUartData, poEalUartInstData, ADI_EAL_UART_CBK_EVT_UART_FIFO_READ, poFifoDat);
        }
        else
        {
            /* Nothing to do */
        }
    } while(false);
}

/** Invoke the callback */
ADI_EAL_UART_CODE
static void InvokeCbk(EAL_UART_DATA *poEalUartData, EAL_UART_INST_DATA *poEalUartInstData, ADI_EAL_UART_CBK_EVT eEvt,
                      const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalUartInstData->pfunCbk != NULL)
    {
        (poEalUartInstData->pfunCbk)(poEalUartInstData->oId.nInstNum, poEalUartData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_UART_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_UART_DATA *poEalUartData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalUartInstData->nUartQWrIdx, poEalUartInstData->nUartQRdIdx,
                               poEalUartInstData->nUartCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_UART_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_UART_DATA *poEalUartData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInUartQueue(&poEalUartInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_UART_CODE
static void CopyTransactionConfigToQueue(EAL_UART_DATA *poEalUartData, ADI_E2BCORE_UART_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_UART_TXN_CONFIG *poInstTxnCfg = &poEalUartInstData->paoCfgs[poEalUartInstData->nUartQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_UART_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_UART_DATA *poEalUartData, bool bIsMcast)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_UART_TXN_CONFIG *poInstTxnCfg = &poEalUartInstData->paoCfgs[poEalUartInstData->nUartQWrIdx];

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addUartConfigToQueue(&poEalUartInstData->oId, false, poInstTxnCfg);
            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalUartInstData->nUartQWrIdx = EAL_INC_Q_IDX(poEalUartInstData->nUartQWrIdx, poEalUartInstData->nUartCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_UART_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_UART_DATA *poEalUartData,
        EAL_UART_INST_DATA *poEalUartInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoUartTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalUartData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalUartData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalUartData, *ppoUartTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoUartTxnCfg = &poEalUartInstData->paoCfgs[poEalUartInstData->nUartQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Sends custom UART transactions */
ADI_EAL_UART_CODE
static ADI_EAL_STATUS SendCustomUartTransactions(EAL_UART_DATA *poEalUartData, EAL_UART_INST_DATA *poEalUartInstData,
        ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg, uint16_t nNumUartTxns, uint16_t nNumTxnsWithResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalUartInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalUartInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalUartInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalUartInstData->nUartQWrIdx,
                                         poEalUartInstData->nUartQRdIdx,
                                         poEalUartInstData->nUartCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInUartQueue(&poEalUartInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalUartInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bMailbox       = poEalUartData->bMailboxMode;
        oFifoMsgOpts.bSync          = poEalUartData->bResyncPending;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        poEalUartData->bResyncPending = false;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add UART transaction */
        eEalStatus = AddCustomUartTxnsToMsg(nInstNum, poEalUartData, poEalUartInstData, paoUartTxnCfg, nNumUartTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the UART EAL mode to operate in custom mode */
        poEalUartInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/** Add UART transactions to the network buffer */
ADI_EAL_UART_CODE
static ADI_EAL_STATUS SendUartTxnsWithBreakField(EAL_UART_DATA *poEalUartData,
        ADI_EAL_UART_TXN_DESC *paoUartIoData, uint16_t nNumUartTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        bool bIsMcast = poEalUartData->bIsMulticast;

        EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];
        /* Assumption : All unicast instances of the handle belong to the same e2bcore lib instance */
        uint32_t nInstNum = poEalUartInstData->oId.nInstNum;

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalUartData->nFifoWriteVlanTag : *poEalUartInstData->pnFifoWriteVlanTag;

        eal_utils_fillNetworkCfg(poEalUartInstData->oId.nInstNum,
                                 poEalUartData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalUartInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag);

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalUartData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalUartData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalUartData->bIsMulticast ?
                                      poEalUartData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalUartInstData->oId.nIntfNum;

        poEalUartData->bResyncPending = false;
        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add UART transaction */
        eEalStatus = AddUartTxnsWithBreakFieldToMsg(nInstNum, poEalUartData, paoUartIoData, nNumUartTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/**
 * @brief Process packed UART transaction payloads and update UART configuration
 *
 * @param[in]  poTxnPayload     Pointer to array of UART transaction payloads to process
 * @param[in]  nNumPackedTxns   Number of packed transactions in the payload array
 * @param[in]  poEalUartData    Pointer to UART instance data for configuration updates
 *
 * @return     ADI_EAL_STATUS   Status of transaction processing
 *             - #ADI_EAL_STATUS_SUCCESS        All transactions processed successfully
 *             - #ADI_EAL_STATUS_INVALID_PARAM  Invalid UART command encountered
 *
 * @details    Iterates through packed UART transactions and processes each based on command type.
 *             Data commands update UART configuration, while delay and good-code commands
 *             are pre-encoded and require no additional processing.
 */
ADI_EAL_UART_CODE
static ADI_EAL_STATUS ProcessUartTxnPayload(ADI_E2BCORE_UART_TXN_PAYLOAD *poTxnPayload,
        uint16_t nNumPackedTxns, EAL_UART_DATA *poEalUartData)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Iterate through all packed transactions in the payload */
    for(uint16_t j = 0U; j < nNumPackedTxns; j++)
    {
        /* Process data command - update UART configuration with data byte */
        if(poTxnPayload[j].eUartCmd == ADI_E2BCORE_UART_CMD_DATA)
        {
            /* Extract UART data byte from transaction payload */
            uint8_t nUartData = (uint8_t)poTxnPayload[j].nUartData;
            
            /* Update UART configuration with the data */
            UpdateUartCfg(1U, &poTxnPayload[j], &nUartData, poEalUartData);
        }
        /* Handle delay and good-code commands (already encoded by caller) */
        else if((poTxnPayload[j].eUartCmd == ADI_E2BCORE_UART_CMD_DELAY) ||
                (poTxnPayload[j].eUartCmd == ADI_E2BCORE_UART_CMD_GOOD_CODE))
        {
            /* Delay and good-code transactions are already command-encoded by the caller. */
            /* No additional processing required */
        }
        /* Invalid command type - set error and abort processing */
        else
        {
            ADI_DBG_REQUIRE(false, "Invalid UART command");
            eEalStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }
    }

    return eEalStatus;
}

/** Add multiple custom UART transactions */
ADI_EAL_UART_CODE
static ADI_EAL_STATUS AddCustomUartTxnsToMsg(uint32_t nInstNum, EAL_UART_DATA *poEalUartData,
        EAL_UART_INST_DATA *poEalUartInstData, ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg, uint16_t nNumUartTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add UART transaction */
    for(uint16_t i = 0U; i < nNumUartTxns; i++)
    {
        ADI_E2BCORE_UART_TXN_PAYLOAD *poTxnPayload = (ADI_E2BCORE_UART_TXN_PAYLOAD *)paoUartTxnCfg[i].pPayload;

        /* Process packed transactions and update the uart data */
        eEalStatus = ProcessUartTxnPayload(poTxnPayload, paoUartTxnCfg[i].oTxnHdr.nNumPackedTxns, poEalUartData);
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalUartInstData, &paoUartTxnCfg[i]))
        {
            /* Since the check has been done above, space has to be available.
               Assert this assumption */
            ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                poEalUartInstData->nUartQWrIdx,
                                poEalUartInstData->nUartQRdIdx,
                                poEalUartInstData->nUartCfgQDepth),
                            "Invalid assumption!");

            /* Copy the given UART configuration into the config queue pool */
            (void)ADI_MEMCPY(&poEalUartInstData->paoCfgs[poEalUartInstData->nUartQWrIdx], &paoUartTxnCfg[i],
                             sizeof(ADI_E2BCORE_UART_TXN_CONFIG));

            /* Add the UART transaction */
            if(adi_e2bcore_addUartTxn(nInstNum, &poEalUartInstData->paoCfgs[poEalUartInstData->nUartQWrIdx]) !=
                    ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }

            /* Increment queue index */
            poEalUartInstData->nUartQWrIdx = EAL_INC_Q_IDX(poEalUartInstData->nUartQWrIdx, poEalUartInstData->nUartCfgQDepth);
        }
        else
        {
            /* Just add an UART transaction with the config provided */
            if(adi_e2bcore_addUartTxn(nInstNum, &paoUartTxnCfg[i]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}

/** Encode multiple transactions and add them to queue if there is a response expected */
ADI_EAL_UART_CODE
static ADI_EAL_STATUS AddUartTxnsWithBreakFieldToMsg(uint32_t nInstNum, EAL_UART_DATA *poEalUartData,
        ADI_EAL_UART_TXN_DESC *paoUartIoData, uint16_t nNumUartTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    EAL_UART_INST_DATA *poEalUartInstData = poEalUartData->apoInstData[0U];
    uint16_t nStopBits = poEalUartInstData->nNumOfStopBits == 1U ? 0x01U : 0x03U;

    /* Loop through all configs and add UART transaction */
    for(uint16_t i = 0U; i < nNumUartTxns; i++)
    {
        uint16_t nNumBreakCmdBeforePayload = 0U;
        bool bIsExtraStopBitTxnAdded = false;
        uint16_t nNumBreakCmdAfterPayload = 0U;
        uint16_t nAfterPayloadBreakFieldOffset = 0U;


        /* Add breakfields before encoding the actual payload */
        if(paoUartIoData[i].nBreakFieldBitsBeforePayload > 0U)
        {
            nNumBreakCmdBeforePayload = ((((uint16_t)paoUartIoData[i].nBreakFieldBitsBeforePayload) + 11U) / 12U);

            for(uint16_t j = 0U; j < nNumBreakCmdBeforePayload; j++)
            {
                gaoUartCfg[j].eUartCmd =  ADI_E2BCORE_UART_CMD_DATA;
                gaoUartCfg[j].nUartLen =  12U;
                gaoUartCfg[j].nUartData = 0x0000;

                if(j == (nNumBreakCmdBeforePayload - 1U))
                {
                    /* Last breakfield byte may not have all 12 bits as '0' */
                    gaoUartCfg[j].nUartLen = (uint8_t)(paoUartIoData[i].nBreakFieldBitsBeforePayload -
                                                       (j * 12U));

                    /* We need to add a stop bit after the break fields, so space of accommodate those bits should be present,
                       otherwise add a new uart transaction for stop bit only */
                    if((12U - gaoUartCfg[j].nUartLen) >= poEalUartInstData->nNumOfStopBits)
                    {
                        gaoUartCfg[j].nUartData |= (nStopBits << gaoUartCfg[j].nUartLen); /* Add stop bits */
                        gaoUartCfg[j].nUartLen += poEalUartInstData->nNumOfStopBits; /* Increment by num stop bits */
                    }
                    else
                    {
                        /* If full 12 bits are used for break field, then
                           add an additional transaction just for stop bit */
                        gaoUartCfg[j + 1U].eUartCmd =  ADI_E2BCORE_UART_CMD_DATA;
                        gaoUartCfg[j + 1U].nUartLen =  poEalUartInstData->nNumOfStopBits;
                        gaoUartCfg[j + 1U].nUartData = nStopBits;

                        /* If stop bit txn was added as an extra transaction, increment the break field count */
                        bIsExtraStopBitTxnAdded = true;
                    }
                }
            }
        }

        /* If an extra stop bit transaction has been added, update the nNumBreakCmdBeforePayload to reflect the same */
        if(bIsExtraStopBitTxnAdded)
        {
            nNumBreakCmdBeforePayload++;
        }

        /* Serialize the UART data (add start, stop and parity bits to the data) */
        UpdateUartCfg(paoUartIoData[i].nLength, &gaoUartCfg[nNumBreakCmdBeforePayload], paoUartIoData[i].pTxData,
                      poEalUartData);

        nAfterPayloadBreakFieldOffset = nNumBreakCmdBeforePayload + paoUartIoData[i].nLength;

        /* Add breakbits after the payload if required for the transaction */
        if(paoUartIoData[i].nBreakFieldBitsAfterPayload > 0U)
        {
            nNumBreakCmdAfterPayload = ((((uint16_t)paoUartIoData[i].nBreakFieldBitsAfterPayload) + 11U) / 12U);
            uint16_t nTotalNumBytes = nAfterPayloadBreakFieldOffset + nNumBreakCmdAfterPayload;

            for(uint16_t j = nAfterPayloadBreakFieldOffset;
                    j < nTotalNumBytes; j++)
            {
                gaoUartCfg[j].eUartCmd =  ADI_E2BCORE_UART_CMD_DATA;
                gaoUartCfg[j].nUartLen =  12U;
                gaoUartCfg[j].nUartData = 0x0000;

                if(j == (nTotalNumBytes - 1U))
                {
                    /* Last breakfield byte may not have all 12 bits as '0' */
                    uint8_t nBitsInLastBreakField =
                        (uint8_t)(((uint16_t)(paoUartIoData[i].nBreakFieldBitsAfterPayload)) -
                                  ((j - nAfterPayloadBreakFieldOffset) * 12U));
                    gaoUartCfg[j].nUartLen = nBitsInLastBreakField;

                    /* We need to add a stop bit after the breakfield; check if we have enough space for stop bits in the current transaction */
                    if((12U - nBitsInLastBreakField) >= poEalUartInstData->nNumOfStopBits)
                    {
                        gaoUartCfg[j].nUartData |= (nStopBits << gaoUartCfg[j].nUartLen); /* Add stop bits */
                        gaoUartCfg[j].nUartLen += poEalUartInstData->nNumOfStopBits; /* Increment by num stop bits */
                    }
                    else
                    {
                        /* If there is no space for stop bit in the current transaction, add a new transaction just for stop bit */
                        gaoUartCfg[j + 1U].eUartCmd =  ADI_E2BCORE_UART_CMD_DATA;
                        gaoUartCfg[j + 1U].nUartLen =  poEalUartInstData->nNumOfStopBits;
                        gaoUartCfg[j + 1U].nUartData = nStopBits;

                        /* If stop bits txn was added as an extra transaction, increment the break field count */
                        nNumBreakCmdAfterPayload++;
                        break;
                    }
                }
            }
        }

        ADI_E2BCORE_UART_TXN_CONFIG oUartTxnCfg;
        ADI_E2BCORE_UART_TXN_CONFIG *poUartTxnCfg = &oUartTxnCfg;

        /* If its not a multicast handle, pick a config from the queue, when response is expected
          For multicast cases, we can send the local transaction config directly to e2bcore for TX encoding
          and add the transaction config from the queue to e2bcore queue after the encoding */
        if(!poEalUartData->bIsMulticast)
        {
            uint16_t nWrIdx = poEalUartInstData->nUartQWrIdx;

            if(paoUartIoData[i].bIsE2bIdPresent || poEalUartInstData->bIsReadTimeTagEn)
            {
                poUartTxnCfg = &poEalUartInstData->paoCfgs[nWrIdx];
            }
        }

        (void)ADI_MEMSET(poUartTxnCfg, 0, sizeof(ADI_E2BCORE_UART_TXN_CONFIG));
        poUartTxnCfg->oTxnHdr.bIsGptpTimePresent = paoUartIoData[i].bIsGptpPresTime;
        poUartTxnCfg->oTxnHdr.nGptpTime = paoUartIoData[i].nGptpPresTime;
        poUartTxnCfg->oTxnHdr.bIsTimeDelayPresent = paoUartIoData[i].bIsDelayPresent;
        poUartTxnCfg->oTxnHdr.nTimeDelay = paoUartIoData[i].nDelay;
        poUartTxnCfg->oTxnHdr.bAreTxnsPacked = true;
        poUartTxnCfg->oTxnHdr.nNumPackedTxns = nAfterPayloadBreakFieldOffset + nNumBreakCmdAfterPayload;
        poUartTxnCfg->oTxnHdr.bIsE2bIdPresent = paoUartIoData[i].bIsE2bIdPresent;
        poUartTxnCfg->oTxnHdr.nE2bId = paoUartIoData[i].nE2bId;
        poUartTxnCfg->pPayload = (void *)gaoUartCfg;

        /* Add the UART transaction */
        if(adi_e2bcore_addUartTxn(nInstNum, poUartTxnCfg) !=
                ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            break;
        }

        /* If response is expected for unicast handle -
           Txn config is already picked from EAL queue and added to e2bcore from the above call,
           we increment the queue if the return is success status
           If response is expected for mutlicast handle -
           Txn config is added to the e2bcore queue individually for all instances and
           increment the queue index for each instance, if the return is success status
           In both cases, the pointer to payload in the transaction config is changed to point to RX pointer given from above layer */
        for(uint16_t k = 0U; k < poEalUartData->nNumInst; k++)
        {
            EAL_UART_INST_DATA *poUartInstance = poEalUartData->apoInstData[k];

            if(paoUartIoData[i].bIsE2bIdPresent || poUartInstance->bIsReadTimeTagEn)
            {
                if(poEalUartData->bIsMulticast)
                {
                    /* Add the queue element pointer to e2bcore */
                    /* Note : There is no copy of the transaction config structure fields used for TX encoding here,
                     as this is only required for getting RX (not payload, but header only for UART interface) */
                    if(adi_e2bcore_addUartConfigToQueue(&poUartInstance->oId, false,
                                                        &poUartInstance->paoCfgs[poUartInstance->nUartQWrIdx]) != ADI_E2BCORE_ERR_NONE)
                    {
                        eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
                    }
                }

                /* Increment queue index */
                poUartInstance->nUartQWrIdx = EAL_INC_Q_IDX(poUartInstance->nUartQWrIdx, poUartInstance->nUartCfgQDepth);
            }
        }
    }

    return eEalStatus;
}


#endif /* ADI_E2B_IFACE_UART_ENABLED */

/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
