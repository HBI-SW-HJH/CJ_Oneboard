/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: adi_eal_fioUart.c
 * @brief: The EAL APIs for FIO-UART interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_FIO_UART_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalFioUartData) \
ADI_DBG_REQUIRE((poEalFioUartData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalFioUartData)->bInUse == true, "Instance is not opened");

/*================================ DATA TYPES ===============================*/
/* Internal structure to hold detailed RX error checking results */
typedef struct EAL_FIO_UART_RX_INFO
{
    uint16_t nRecvLength;               /* Actual received length */
    bool bIsBreakDetectedBeforePayload; /* Indicates if breakfield is detected before payload */
    bool bIsBreakDetectedAfterPayload;  /* Indicates if breakfield is detected after payload */
} EAL_FIO_UART_RX_INFO;

/* Internal data structure for updating the transaction data */
typedef struct ADI_EAL_FIO_UART_UPDATE_TXN_DATA
{
    uint16_t *pTxData;                    /*!< Pointer to TX data */
    uint16_t nWriteLength;                /*!< Length of data to be written */
    uint16_t nReadLength;                 /*!< Length of data to be read */
    uint16_t *pFormattedTxData;           /*!< Pointer to formatted TX data */
    uint16_t *pnWriteLengthOutput;        /*!< Pointer to formatted TX data length */
    uint8_t nBreakFieldBitsBeforePayload; /*!< Number of break field bits before payload */
    uint8_t nBreakFieldBitsAfterPayload;  /*!< Number of break field bits after payload */
    bool bCancelEcho;                     /*!< Indicates if CAN echo cancellation is to be done */
} ADI_EAL_FIO_UART_UPDATE_TXN_DATA;

/*=================================== DATA ==================================*/
/* Instructions for FLEXIBLE IO UART without parity */
static uint32_t ganInstructionMemData[] =
{

    0xc020c420U,
    0xf860f000U,
    0x40c07080U,
    0xc429412aU,
    0xf010e020U,
    0x0232e033U,
    0xe033e033U,
    0xe033e033U,
    0xe033e033U,
    0xe033e033U,
    0xe033e033U,
    0xe033e033U,
    0xe033e0e0U,
    0xf4007100U,
    0x0040f840U,
    0xe100d402U,
    0x40c0d828U,
    0x2292cd00U,
    0x4140d828U,
    0x2292cee0U,
    0x45e0d828U,
    0x2292d833U,
    0xd833d833U,
    0xd833d833U,
    0xd833d833U,
    0xd833d833U,
    0xd833d833U,
    0xd833d833U,
    0xd831cde0U,
    0xcfe04440U,
    0xdbe04460U,
    0x2400dae0U,
    0x44e01000U,
    0xdb282291U,
    0x10004560U,
    0xd8282292U,
    0xcd004140U,
    0xd8282292U,
    0x39603e20U,
    0xda08da80U,
    0xda883620U

};

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendFioUartTransaction(
    EAL_FIO_UART_DATA *poEalFioUartData,
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg,
    uint16_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod);
static void UpdateUartTxData(EAL_FIO_UART_DATA *poEalFioUartData,
                             ADI_EAL_FIO_UART_UPDATE_TXN_DATA *poUpdateUartTxnData);
static void AddBreakfieldBits(uint8_t *anPackedBuffer, uint16_t *pnBufferIndex,
                              EAL_FIO_UART_INST_DATA *poEalFioUartInstData, uint8_t nBreakFieldBits);
static void AddBreakfieldBitsAfterPayload(uint8_t *anPackedBuffer, uint16_t *pnBufferIndex,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData, uint8_t nBreakFieldBits);
static void PackUartDataByte(uint16_t nDataBits, EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                             uint8_t *anPackedBuffer, uint16_t *pnBufferIndex);
static void FormatPackedDataWithHeaders(uint8_t *anPackedBuffer,
                                        uint16_t nBufferIndex,
                                        ADI_EAL_FIO_UART_UPDATE_TXN_DATA *poUpdateUartTxnData);
static void LockFioUartMutex(EAL_FIO_UART_DATA *poEalFioUartData);
static void UnlockFioUartMutex(EAL_FIO_UART_DATA *poEalFioUartData);
static void SetWaitingForRx(EAL_FIO_UART_DATA *poEalFioUartData, bool bWaitingForRx);
static bool WaitForResponses(EAL_FIO_UART_DATA *poEalFioUartData);
static bool IsResponseExpected(EAL_FIO_UART_INST_DATA *poEalFioUartData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg);
static uint16_t GetNumTxnsWithResp(EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                                   ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg,
                                   uint16_t nNumFioUartTxns);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static bool CheckforError(EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                          uint8_t *pRxData, uint16_t nRxLength, EAL_FIO_UART_RX_INFO *poRxInfo);
static void DecodeRxData(EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                         uint8_t *pRxData, uint16_t *panDecodedRxData, uint16_t nRxLength);
static void InvokeCbk(EAL_FIO_UART_DATA *poEalFioUartData, EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                      ADI_EAL_FIO_UART_CBK_EVT eEvt, const void *pArg);
static bool IsSpaceAvblInEalTxnQueue(EAL_FIO_UART_DATA *poEalFioUartData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_FIO_UART_DATA *poEalFioUartData);
static void CopyTransactionConfigToQueue(EAL_FIO_UART_DATA *poEalFioUartData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_FIO_UART_DATA *poEalFioUartData, bool bIsMcast, uint16_t **apRxBuf);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioUartTxnCfg);
static void AddTransactionToQueues(EAL_FIO_UART_DATA *poEalFioUartData,
                                   EAL_FIO_UART_INST_DATA *poEalFioUartInstData, bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioUartTxnCfg);
static ADI_EAL_STATUS SendCustomFioUartTransactions(EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t nNumFioUartTxns, uint16_t nNumTxnsWithResp);
static ADI_EAL_STATUS AddCustomFioUartTxnsToMsg(uint32_t nInstNum, EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg,
        uint16_t nNumFioUartTxns);
static bool CheckQueueSpaceForRspTxn(EAL_FIO_UART_DATA *poEalFioUartData,
                                     ADI_EAL_FIO_UART_TXN_DESC *paoFioUartIoTxnWithBreak,
                                     uint16_t nNumFioUartTxns);
static ADI_EAL_STATUS SendFioUartTxnsWithBreak(EAL_FIO_UART_DATA *poEalFioUartData,
        ADI_EAL_FIO_UART_TXN_DESC *paoFioUartTxnWithBreak, uint16_t nNumFioUartTxns, uint16_t *pFormattedTxData);
static ADI_EAL_STATUS AddFioUartTxnsWithBreakToMsg(uint32_t nInstNum, EAL_FIO_UART_DATA *poEalFioUartData,
        ADI_EAL_FIO_UART_TXN_DESC *paoFioUartTxnWithBreak,
        uint16_t nNumFioUartTxns, uint16_t *pFormattedTxData);
static ADI_EAL_STATUS CheckQueueSpaceForTransaction(EAL_FIO_UART_DATA *poEalFioUartData, bool bIsMcast);
static ADI_EAL_STATUS AddFioUartTxnsToMsg(uint32_t nInstNum, EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t **apRxBuf,
        uint16_t nNumFioUartTxns);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an instance of FIO-UART interface
 *
 * @param [in]  poFioUartCfg     Configurations for this FIO-UART instance
 * @param [out] phFioUartHandle  Handle corresponding to this FIO-UART instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-UART instance successfully opened
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartOpen(ADI_EAL_FIO_UART_CFG *poFioUartCfg, ADI_EAL_FIO_UART_HANDLE *phFioUartHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poFioUartCfg != NULL, "Pointer to the FIO-UART module configuration is NULL");
    ADI_DBG_REQUIRE(phFioUartHandle != NULL, "Pointer to the FIO-UART handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poFioUartCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poFioUartCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poFioUartCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum   = poFioUartCfg->nInstNum;
    uint32_t nRemoteNum = poFioUartCfg->nRemoteNum;
    uint32_t nIntfNum   = poFioUartCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_FIO,
                    "The given pair of instance, remote and interface numbers is not configured as FIO interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poFioUartCfg->pFioUartMemory != NULL, "Pointer to allocated memory is NULL");
    ADI_DBG_REQUIRE(poFioUartCfg->nFioUartMemorySize >=
                    ADI_EAL_FIO_UART_UNICAST_INST_MEM_SIZE(poFioUartCfg->nFioUartCfgQDepth),
                    "Allocated memory is less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poFioUartCfg->pFioUartMemory;

    /* Allocate memory for the FIO UART transaction config queue. */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfgs = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)((void *)pMem);
    pMem += (poFioUartCfg->nFioUartCfgQDepth * sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

    /* Allocate memory for the internal breakfields information queue required for RX parsing
       (each element is co-related to a transaction config queue element) */
    ADI_EAL_FIO_UART_BREAKFIELD_INFO *paoBreakfieldInfo = (ADI_EAL_FIO_UART_BREAKFIELD_INFO *)((void *)pMem);
    pMem += (poFioUartCfg->nFioUartCfgQDepth * sizeof(ADI_EAL_FIO_UART_BREAKFIELD_INFO));

    /* Allocate memory for the EAL FIO_UART instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = (EAL_FIO_UART_INST_DATA *)((void *)pMem);
    (void)ADI_MEMSET(poEalFioUartInstData, 0, sizeof(EAL_FIO_UART_INST_DATA));
    pMem += sizeof(EAL_FIO_UART_INST_DATA);

    /* Instance info */
    poEalFioUartInstData->oId.nInstNum       = poFioUartCfg->nInstNum;
    poEalFioUartInstData->oId.nRemoteNum     = poFioUartCfg->nRemoteNum;
    poEalFioUartInstData->oId.nIntfNum       = poFioUartCfg->nIntfNum;
    poEalFioUartInstData->pnIfRegVlanTag     = &poIfConfig->nIfRegVlanTag;
    poEalFioUartInstData->pnFifoRegVlanTag   = &poIfConfig->nFifoRegVlanTag;
    poEalFioUartInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;

    /* State info */
    poEalFioUartInstData->bIgnoreErr = false;
    poEalFioUartInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
    poEalFioUartInstData->pfunCbk = NULL;
    poEalFioUartInstData->bWaitingForRx = false;

    /* Transaction config queue info */
    uint32_t nFlexioCtrlReg = 0U;
    EAL_FIO_UART_INST_DATA **apoInstData;
    EAL_FIO_UART_DATA *poEalFioUartData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;

    if(eal_utils_getConfigInfRegVal(&poEalFioUartInstData->oId, FLEXIO_CONTROL_NW_ADDR, &nFlexioCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }

    poEalFioUartInstData->bIsReadTimeTagEn = (nFlexioCtrlReg & BITM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalFioUartInstData->nFioUartCfgQDepth = poFioUartCfg->nFioUartCfgQDepth;
    poEalFioUartInstData->nFioUartQWrIdx    = 0U;
    poEalFioUartInstData->nFioUartQRdIdx    = 0U;
    poEalFioUartInstData->paoCfgs           = paoFioUartTxnCfgs;
    poEalFioUartInstData->paoBreakfieldInfo = paoBreakfieldInfo;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalFioUartInstData->oId, (void *)pMem,
                    poFioUartCfg->nFioUartCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }

    pMem += (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *) * poFioUartCfg->nFioUartCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_FIO_UART_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_FIO_UART_INST_DATA *);
        apoInstData[0U] = poEalFioUartInstData;

        /* Allocate memory for the internal EAL FIO_UART data structure */
        poEalFioUartData = (EAL_FIO_UART_DATA *)((void *)pMem);
        pMem += sizeof(EAL_FIO_UART_DATA);
        poEalFioUartData->bIsMulticast  = false;
        poEalFioUartData->bInUse        = true;
        poEalFioUartData->bMailboxMode  = false;
        poEalFioUartData->bResyncPending = false;
        poEalFioUartData->nIntfTopic    = 0U;
        poEalFioUartData->nFifoTopic    = 0U;
        poEalFioUartData->nNumInst      = 1U;
        poEalFioUartData->apoInstData   = apoInstData;

        /* Mutex config */
        poEalFioUartInstData->oFioUartMutexCfg.pMem = &poEalFioUartInstData->anFioUartMutexBuffer[0U];
        poEalFioUartInstData->oFioUartMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalFioUartInstData->oFioUartMutexCfg, &poEalFioUartInstData->hFioUartMutex);

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        /* Initialise the semaphore for this FIO-UART instance */
        poEalFioUartInstData->oFifoRespSemaphoreCfg.pMem = &poEalFioUartInstData->anFioUartFifoRespSemaphoreBuffer[0U];
        poEalFioUartInstData->oFifoRespSemaphoreCfg.nMemSize = PRJ_OSAL_SEMAPHORE_MEM_SIZE;
        poEalFioUartInstData->oFifoRespSemaphoreCfg.bCountingSemaphore = false;
        poEalFioUartInstData->oFifoRespSemaphoreCfg.nMaxCount = 1U;
        poEalFioUartInstData->oFifoRespSemaphoreCfg.nInitialCount = 0U;
        (void)prj_osal_createSemaphore(&poEalFioUartInstData->oFifoRespSemaphoreCfg, &poEalFioUartInstData->hFifoRespSemaphore);
#endif
        /* Set the callback param in the e2bcore config */
        poIfConfig->pCbParam = poEalFioUartData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_FIO_UART;

        /* Return the internal FIO-UART instance data as
           the handle for use in other functions */
        *phFioUartHandle = (ADI_EAL_FIO_UART_HANDLE)poEalFioUartData;
    }

    return eStatus;
}

/**
 * @brief       Closes the opened FIO-UART interface instance
 *
 * @param [in]  hFioUartHandle   Handle conrresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-UART instance successfully closed
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartClose(ADI_EAL_FIO_UART_HANDLE hFioUartHandle)
{
    /* Extract the FIO-UART data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalFioUartData->apoInstData[i]->hFioUartMutex);
#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        (void)prj_osal_destroySemaphore(poEalFioUartData->apoInstData[i]->hFifoRespSemaphore);
#endif
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalFioUartData, 0, sizeof(EAL_FIO_UART_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Register a callback function for getting notified of events
 *
 * @param [in]  hFioUartHandle       Handle corresponding to this instance
 * @param [in]  pfFioUartCallback    Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Callback registered successfully
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartRegisterCallback(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_CALLBACK_FUNC pfFioUartCallback)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(pfFioUartCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalFioUartData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* Register the callback function */
    poEalFioUartInstData->pfunCbk = pfFioUartCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Creates a multicast group of FIO_UART interfaces
 *
 * @param [in]  poFioUartMcastCfg       Configuration for FIO_UART multicast group
 * @param [out] phFioUartMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartCreateMcastGroup(ADI_EAL_FIO_UART_MCAST_CFG *poFioUartMcastCfg,
        ADI_EAL_FIO_UART_HANDLE *phFioUartMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poFioUartMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phFioUartMcastHandle != NULL, "Pointer to the FIO_UART multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poFioUartMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poFioUartMcastCfg->pahFioUartHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poFioUartMcastCfg->pFioUartMcastMem != NULL, "Pointer to memory for FIO_UART multicast is NULL");
    ADI_DBG_REQUIRE(poFioUartMcastCfg->nFioUartMcastMemSize >= ADI_EAL_FIO_UART_MCAST_GROUP_MEM_SIZE(
                        poFioUartMcastCfg->nNumInst),
                    "Not enough memory allocated for FIO_UART multicast group");
    ADI_DBG_REQUIRE(poFioUartMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poFioUartMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poFioUartMcastCfg->pFioUartMcastMem;
    EAL_FIO_UART_INST_DATA **apoInstData = (EAL_FIO_UART_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_FIO_UART_INST_DATA *) * poFioUartMcastCfg->nNumInst);

    /* Save the unicast EAL FIO_UART instances */
    for(uint16_t i = 0U; i < poFioUartMcastCfg->nNumInst; i++)
    {
        EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)poFioUartMcastCfg->pahFioUartHandles[i];
        apoInstData[i] = poEalFioUartData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast FIFO topic */
        uint32_t nInstNum   = poEalFioUartData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioUartData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum   = poEalFioUartData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poFioUartMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poFioUartMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poFioUartMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poFioUartMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_FIO_UART_DATA *poEalFioUartMcastData = (EAL_FIO_UART_DATA *)((void *)pMem);
    poEalFioUartMcastData->bIsMulticast      = true;
    poEalFioUartMcastData->bInUse            = true;
    poEalFioUartMcastData->bMailboxMode      = false;
    poEalFioUartMcastData->bResyncPending    = false;
    poEalFioUartMcastData->nIntfTopic        = poFioUartMcastCfg->nIntfTopic;
    poEalFioUartMcastData->nFifoTopic        = poFioUartMcastCfg->nFifoTopic;
    poEalFioUartMcastData->nIfRegVlanTag     = (poFioUartMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poFioUartMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioUartMcastData->nFifoRegVlanTag   = (poFioUartMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poFioUartMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioUartMcastData->nFifoWriteVlanTag = (poFioUartMcastCfg->poFifoWriteVlanTag != NULL) ?
            eal_utils_convertVlanTags(poFioUartMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioUartMcastData->nNumInst     = poFioUartMcastCfg->nNumInst;
    poEalFioUartMcastData->apoInstData  = apoInstData;

    /* Set the handle to the EAL_FIO_UART_DATA structure */
    *phFioUartMcastHandle = (ADI_EAL_FIO_UART_HANDLE)poEalFioUartMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to configure the Flexible IO UART instruction RAM and configuration parameters
 *
 * @param [in] hFioUartHandle  Handle corresponding to this FIO-UART instance
 * @param [in] poParams        Pointer to FIO-UART parameter structure
 * @param [in] poFioInstructionRamData Pointer to instruction RAM data to be programmed (can be NULL if poParams->bIsOtpConfigured is true)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Configured FioUART handle and updated instruction RAM successfully
 *
 * @note        <b>OTP Configuration Support</b>:
 *              - When poParams->bIsOtpConfigured = true:
 *                  - Instruction RAM programming is skipped
 *                  - The device uses OTP as the source for instruction RAM
 *                  - poFioInstructionRamData can be NULL
 *                  - Runtime parameters (baudrate, parity, etc.) are still updated
 *              - When poParams->bIsOtpConfigured = false:
 *                  - Instruction RAM must be programmed
 *                  - poFioInstructionRamData must not be NULL
 *                  - If poFioInstructionRamData->bOverrideDefault is true, application-provided data is used
 *                  - If poFioInstructionRamData->bOverrideDefault is false, default instruction RAM is used
 *
 * @note        This API must be called after adi_eal_fioUartOpen() and before any read/write operations.
 * @note        This is a synchronous API that internally flushes the transmit buffer before returning (for non-OTP case).
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartConfigure(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, const ADI_EAL_FIO_UART_PARAMS *poParams,
                                        const ADI_EAL_FIO_UART_INST_RAM_DATA *poFioInstructionRamData)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the FIO-UART data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioUartData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Validate instruction RAM data based on OTP configuration */
    if(poParams->bIsOtpConfigured == false)
    {
        /* Non-OTP case: Instruction RAM data is required */
        ADI_DBG_REQUIRE(poFioInstructionRamData != NULL, "Instruction RAM data required for non-OTP configuration");
    }

    /* Extract the FIO-UART instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    poEalFioUartInstData->nNumOfDataBits   = poParams->nNumOfDataBits;
    poEalFioUartInstData->bIsParityEn      = poParams->bIsParityEnabled;
    poEalFioUartInstData->eParityType      = poParams->eParityType;
    poEalFioUartInstData->nBaudRate        = poParams->nBaudRate;
    poEalFioUartInstData->nBreakFieldBits  = poParams->nBreakFieldBits;
    poEalFioUartInstData->nNumOfStopBits   = poParams->nNumOfStopBits;
    poEalFioUartInstData->bIsCanEchoCancel = poParams->bIsCanEchoCancel;

    /* Check if OTP is configured */
    if(poParams->bIsOtpConfigured == false)
    {
        /* Non-OTP Case: Program instruction RAM */
        /* Instruction RAM data */
        ADI_EAL_FIO_INST_RAM_DATA oFioInstRamData;
        (void)ADI_MEMSET(&oFioInstRamData, 0, sizeof(oFioInstRamData));

        /* If the application is overriding the instruction data,
           copy the values given by the application */
        if(poFioInstructionRamData->bOverrideDefault)
        {
            (void)ADI_MEMCPY(&oFioInstRamData, poFioInstructionRamData, sizeof(ADI_EAL_FIO_INST_RAM_DATA));
        }
        /* Else, set the default values */
        else
        {
            /* Set the instruction ram entries */
            oFioInstRamData.panInstructionMemData = &ganInstructionMemData[0U];
            oFioInstRamData.nNumInstMemEntries =
                (uint16_t)(sizeof(ganInstructionMemData) / sizeof(ganInstructionMemData[0U]));

            /* SM routing is not fixed to SM0 */
            oFioInstRamData.bFifoTxRouterFixedSm = false;

            /* Number of active SMs (2 - 1 for TX and 1 for RX) */
            oFioInstRamData.nNumActiveStateMachines = 2U;

            /* Compute the clock divider from baud rate. Here instr_per_bit = 20 */
            double fClkDivVal = ((100.0f * 1000000.0f) / (20.0f * (double)
                                 poEalFioUartInstData->nBaudRate));
            fClkDivVal += 0.5f;

            /* State machine 0 configuration */
            oFioInstRamData.aoSmCfgs[0U].bDelaySidesetSingleMode = true;
            oFioInstRamData.aoSmCfgs[0U].bEnableAutopush         = false;
            oFioInstRamData.aoSmCfgs[0U].bEnableAutopull         = true;
            oFioInstRamData.aoSmCfgs[0U].bOsrShiftRight          = true;
            oFioInstRamData.aoSmCfgs[0U].bIsrShiftRight          = true;
            oFioInstRamData.aoSmCfgs[0U].nPinOutBasepin          = 0U;
            oFioInstRamData.aoSmCfgs[0U].nPinInBasepin           = 7U;
            oFioInstRamData.aoSmCfgs[0U].nSidesetBasepin         = 0U;
            oFioInstRamData.aoSmCfgs[0U].nJmpBasepin             = 0U;
            oFioInstRamData.aoSmCfgs[0U].nProgramStart           = 0U;
            oFioInstRamData.aoSmCfgs[0U].nProgramSize            = 29U;
            oFioInstRamData.aoSmCfgs[0U].nDelaySidesetBits       = 0U;
            oFioInstRamData.aoSmCfgs[0U].nAutopullThreshold      = 7U;
            oFioInstRamData.aoSmCfgs[0U].nAutopushThreshold      = 0U;
            oFioInstRamData.aoSmCfgs[0U].nPullThreshold          = 7U;
            oFioInstRamData.aoSmCfgs[0U].nPushThreshold          = 0U;
            oFioInstRamData.aoSmCfgs[0U].nClkDiv                 = (uint16_t)fClkDivVal;

            /* State Machine 1 configuration */
            oFioInstRamData.aoSmCfgs[1U].bDelaySidesetSingleMode = true;
            oFioInstRamData.aoSmCfgs[1U].bEnableAutopush         = true;
            oFioInstRamData.aoSmCfgs[1U].bEnableAutopull         = false;
            oFioInstRamData.aoSmCfgs[1U].bOsrShiftRight          = true;
            oFioInstRamData.aoSmCfgs[1U].bIsrShiftRight          = true;
            oFioInstRamData.aoSmCfgs[1U].nPinOutBasepin          = 0U;
            oFioInstRamData.aoSmCfgs[1U].nPinInBasepin           = 7U;
            oFioInstRamData.aoSmCfgs[1U].nSidesetBasepin         = 0U;
            oFioInstRamData.aoSmCfgs[1U].nJmpBasepin             = 0U;
            oFioInstRamData.aoSmCfgs[1U].nProgramStart           = 29U;
            oFioInstRamData.aoSmCfgs[1U].nProgramSize            = 53U;
            oFioInstRamData.aoSmCfgs[1U].nDelaySidesetBits       = 0U;
            oFioInstRamData.aoSmCfgs[1U].nAutopullThreshold      = 0U;
            oFioInstRamData.aoSmCfgs[1U].nAutopushThreshold      = 8U;
            oFioInstRamData.aoSmCfgs[1U].nPullThreshold          = 0U;
            oFioInstRamData.aoSmCfgs[1U].nPushThreshold          = 0U;
            oFioInstRamData.aoSmCfgs[1U].nClkDiv                 = (uint16_t)fClkDivVal;

        }

        /* Configure the Flexible IO registers */
        eEalStatus = eal_utils_configureFlexibleIO(&poEalFioUartInstData->oId, &oFioInstRamData);
    }

    /* else: OTP Case - Skip instruction RAM programming, eEalStatus already set to SUCCESS */

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO UART write read operation asynchronously
 *
 * @param [in] hFioUartHandle   Handle corresponding to this instance
 * @param [in] poFioUartData    Pointer to FIO UART data
 * @param [in] nId              Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 * @note       <b>E2B_ID Usage Pattern (Async API)</b>:
 *             - E2B_ID is <b>optional</b> for asynchronous FIO UART write-read operations
 *             - Set nId = 0xFF to disable E2B_ID
 *             - Set nId to any value (0x00-0xFE) to enable E2B_ID tracking
 *             - When enabled, E2B_ID is copied to the upstream response message,
 *               allowing correlation between this downstream command and device response
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, ADI_EAL_FIO_UART_IO_DATA *poFioUartData,
                                        uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(poFioUartData != NULL, "FIO_UART IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioUartData->nReadLength <= 255U, "FIO_UART Read length is invalid");
    ADI_DBG_REQUIRE(poFioUartData->nWriteLength <= 255U, "FIO_UART Write length is invalid");

    /* Get the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    if(poEalFioUartInstData->bIsCanEchoCancel == false)
    {
        ADI_DBG_REQUIRE(poFioUartData->nReadLength + poFioUartData->nWriteLength <= 255U, "When echo is enabled "
                        "the total of read length and write length should be less than or equal to 255");
    }

    /* Set the FIO_UART transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked          = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr           = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent         = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nE2bId                  = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen       = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen        = true;
    oTxnCfg.oTxnHdr.bIsRouterHeaderInserted = true;

    /* Loop through the mutex of all instances and lock it */
    LockFioUartMutex(poEalFioUartData);

    uint16_t nWriteLength;
    /* Encode UART data */
    ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
    oUpdateUartTxnData.bCancelEcho                  = poEalFioUartInstData->bIsCanEchoCancel;
    oUpdateUartTxnData.nReadLength                  = poFioUartData->nReadLength;
    oUpdateUartTxnData.nWriteLength                 = poFioUartData->nWriteLength;
    oUpdateUartTxnData.pTxData                      = poFioUartData->pTxData;
    oUpdateUartTxnData.pFormattedTxData             = poFioUartData->pFormattedTxData;
    oUpdateUartTxnData.nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
    oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = 0U;
    oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;

    /* If echo cancellation is disabled and we have breakfields in the payload, then we have to account for breakfield echo as well */
    if((poEalFioUartInstData->bIsCanEchoCancel == false))
    {
        oUpdateUartTxnData.nReadLength +=
            (poEalFioUartInstData->nBreakFieldBits == 0U ? 0U : 1U);
    }

    UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

    oTxnCfg.pPayload          = poFioUartData->pFormattedTxData;
    oTxnCfg.oTxnHdr.nWriteLen = nWriteLength;

    if(poEalFioUartInstData->bIsCanEchoCancel)
    {
        oTxnCfg.oTxnHdr.nReadLen = oUpdateUartTxnData.nReadLength * 2U;
    }
    else
    {
        oTxnCfg.oTxnHdr.nReadLen = (oUpdateUartTxnData.nReadLength * 2U) + (poFioUartData->nWriteLength * 2U);
    }

    /* Send the FIO_UART transaction */
    ADI_EAL_STATUS eEalStatus = SendFioUartTransaction(poEalFioUartData, &oTxnCfg, poFioUartData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO UART write read operation synchronously
 *
 * @param [in] hFioUartHandle   Handle corresponding to this instance
 * @param [in] poFioUartData    Pointer to FIO UART data
 * @param [in] nId              Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT       : Timeout error
 * @note       <b>E2B_ID Usage Pattern (Sync API)</b>:
 *             - E2B_ID is <b>always enabled</b> for synchronous operations
 *             - This is <b>intentional and required</b> to receive device acknowledgment
 *             - The API blocks waiting for the upstream read response with matching E2B_ID
 *             - E2B_ID enables correlation between the downstream write-read command and
 *               the upstream response, confirming the device processed the transaction
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *             - Essential for proper synchronous operation and response matching
 * @note       This is a synchronous API that internally flushes the transmit buffer and waits for the response before returning.
 *             The response data is available upon successful return (no timeout or error).
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartWriteReadSync(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData,
        uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(poFioUartData != NULL, "FIO_UART IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioUartData->nReadLength <= 255U, "FIO_UART Read length is invalid");
    ADI_DBG_REQUIRE(poFioUartData->nWriteLength <= 255U, "FIO_UART Write length is invalid");

    /* Get the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    if(poEalFioUartInstData->bIsCanEchoCancel == false)
    {
        ADI_DBG_REQUIRE(poFioUartData->nReadLength + poFioUartData->nWriteLength <= 255U, "When echo is enabled "
                        "the total of read length and write length should be less than or equal to 255");
    }

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the FIO_UART transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked          = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr           = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent         = true;
    oTxnCfg.oTxnHdr.nE2bId                  = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen       = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen        = true;
    oTxnCfg.oTxnHdr.bIsRouterHeaderInserted = true;

    /* Loop through the mutex of all instances and lock it */
    LockFioUartMutex(poEalFioUartData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalFioUartData, true);
    uint16_t nWriteLength;

    /* Encode UART data */
    ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
    oUpdateUartTxnData.bCancelEcho                  = poEalFioUartInstData->bIsCanEchoCancel;
    oUpdateUartTxnData.nReadLength                  = poFioUartData->nReadLength;
    oUpdateUartTxnData.nWriteLength                 = poFioUartData->nWriteLength;
    oUpdateUartTxnData.pTxData                      = poFioUartData->pTxData;
    oUpdateUartTxnData.pFormattedTxData             = poFioUartData->pFormattedTxData;
    oUpdateUartTxnData.nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
    oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = 0U;
    oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;

    /* If echo cancellation is disabled and we have breakfields in the payload, then we have to account for breakfield echo as well */
    if((poEalFioUartInstData->bIsCanEchoCancel == false))
    {
        oUpdateUartTxnData.nReadLength +=
            (poEalFioUartInstData->nBreakFieldBits == 0U ? 0U : 1U);
    }

    UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

    oTxnCfg.pPayload          = poFioUartData->pFormattedTxData;
    oTxnCfg.oTxnHdr.nWriteLen = nWriteLength;

    if(poEalFioUartInstData->bIsCanEchoCancel)
    {
        oTxnCfg.oTxnHdr.nReadLen = oUpdateUartTxnData.nReadLength * 2U;
    }
    else
    {
        oTxnCfg.oTxnHdr.nReadLen = (oUpdateUartTxnData.nReadLength * 2U) + (poFioUartData->nWriteLength * 2U);
    }

    /* Send the FIO_UART transaction */
    eEalStatus = SendFioUartTransaction(poEalFioUartData, &oTxnCfg, poFioUartData->apRxData, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for FIO_UART read response */
        bool bTimedOut = WaitForResponses(poEalFioUartData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalFioUartData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO UART write read operation after delay
 *
 * @param [in] hFioUartHandle   Handle corresponding to this instance
 * @param [in] poFioUartData    Pointer to FIO UART data
 * @param [in] nDelay           Delay time in ns
 * @param [in] nId              Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartWriteReadWithDelay(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData,
        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(poFioUartData != NULL, "FIO_UART IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioUartData->nReadLength <= 255U, "FIO_UART Read length is invalid");
    ADI_DBG_REQUIRE(poFioUartData->nWriteLength <= 255U, "FIO_UART Write length is invalid");

    /* Get the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    if(poEalFioUartInstData->bIsCanEchoCancel == false)
    {
        ADI_DBG_REQUIRE(poFioUartData->nReadLength + poFioUartData->nWriteLength <= 255U, "When echo is enabled "
                        "the total of read length and write length should be less than or equal to 255");
    }

    /* Set the FIO_UART transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent     = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked          = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr           = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent         = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nTimeDelay              = nDelay;
    oTxnCfg.oTxnHdr.nE2bId                  = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen       = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen        = true;
    oTxnCfg.oTxnHdr.bIsRouterHeaderInserted = true;

    /* Loop through the mutex of all instances and lock it */
    LockFioUartMutex(poEalFioUartData);
    uint16_t nWriteLength;

    /* Encode UART data */
    ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
    oUpdateUartTxnData.bCancelEcho                  = poEalFioUartInstData->bIsCanEchoCancel;
    oUpdateUartTxnData.nReadLength                  = poFioUartData->nReadLength;
    oUpdateUartTxnData.nWriteLength                 = poFioUartData->nWriteLength;
    oUpdateUartTxnData.pTxData                      = poFioUartData->pTxData;
    oUpdateUartTxnData.pFormattedTxData             = poFioUartData->pFormattedTxData;
    oUpdateUartTxnData.nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
    oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = 0U;
    oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;

    /* If echo cancellation is disabled and we have breakfields in the payload, then we have to account for breakfield echo as well */
    if((poEalFioUartInstData->bIsCanEchoCancel == false))
    {
        oUpdateUartTxnData.nReadLength +=
            (poEalFioUartInstData->nBreakFieldBits == 0U ? 0U : 1U);
    }

    UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

    oTxnCfg.pPayload          = poFioUartData->pFormattedTxData;
    oTxnCfg.oTxnHdr.nWriteLen = nWriteLength;

    if(poEalFioUartInstData->bIsCanEchoCancel)
    {
        oTxnCfg.oTxnHdr.nReadLen = oUpdateUartTxnData.nReadLength * 2U;
    }
    else
    {
        oTxnCfg.oTxnHdr.nReadLen = (oUpdateUartTxnData.nReadLength * 2U) + (poFioUartData->nWriteLength * 2U);
    }

    /* Send the FIO_UART transaction */
    ADI_EAL_STATUS eEalStatus = SendFioUartTransaction(poEalFioUartData, &oTxnCfg, poFioUartData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO UART write read operation after presentation time
 *
 * @param [in] hFioUartHandle   Handle corresponding to this instance
 * @param [in] poFioUartData    Pointer to FIO UART data
 * @param [in] nTimestamp       The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in] nId              Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartWriteReadWithPresTs(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData,
        uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(poFioUartData != NULL, "FIO_UART IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioUartData->nReadLength <= 255U, "FIO_UART Read length is invalid");
    ADI_DBG_REQUIRE(poFioUartData->nWriteLength <= 255U, "FIO_UART Write length is invalid");

    /* Get the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    if(poEalFioUartInstData->bIsCanEchoCancel == false)
    {
        ADI_DBG_REQUIRE(poFioUartData->nReadLength + poFioUartData->nWriteLength <= 255U, "When echo is enabled "
                        "the total of read length and write length should be less than or equal to 255");
    }

    /* Set the FIO_UART transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent      = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked          = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr           = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent         = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nGptpTime               = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId                  = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen       = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen        = true;
    oTxnCfg.oTxnHdr.bIsRouterHeaderInserted = true;

    /* Loop through the mutex of all instances and lock it */
    LockFioUartMutex(poEalFioUartData);
    uint16_t nWriteLength;

    /* Encode UART data */
    ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
    oUpdateUartTxnData.bCancelEcho                  = poEalFioUartInstData->bIsCanEchoCancel;
    oUpdateUartTxnData.nReadLength                  = poFioUartData->nReadLength;
    oUpdateUartTxnData.nWriteLength                 = poFioUartData->nWriteLength;
    oUpdateUartTxnData.pTxData                      = poFioUartData->pTxData;
    oUpdateUartTxnData.pFormattedTxData             = poFioUartData->pFormattedTxData;
    oUpdateUartTxnData.nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
    oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = 0U;
    oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;

    /* If echo cancellation is disabled and we have breakfields in the payload, then we have to account for breakfield echo as well */
    if((poEalFioUartInstData->bIsCanEchoCancel == false))
    {
        oUpdateUartTxnData.nReadLength +=
            (poEalFioUartInstData->nBreakFieldBits == 0U ? 0U : 1U);
    }

    UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

    oTxnCfg.pPayload          = poFioUartData->pFormattedTxData;
    oTxnCfg.oTxnHdr.nWriteLen = nWriteLength;

    if(poEalFioUartInstData->bIsCanEchoCancel)
    {
        oTxnCfg.oTxnHdr.nReadLen = oUpdateUartTxnData.nReadLength * 2U;
    }
    else
    {
        oTxnCfg.oTxnHdr.nReadLen = (oUpdateUartTxnData.nReadLength * 2U) + (poFioUartData->nWriteLength * 2U);
    }

    /* Send the FIO_UART transaction */
    ADI_EAL_STATUS eEalStatus = SendFioUartTransaction(poEalFioUartData, &oTxnCfg, poFioUartData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief Allows the application to setup FIO UART periodic write read
 *
 * @param [in] hFioUartHandle      Handle corresponding to this instance
 * @param [in] poFioUartData       Pointer to FIO UART data
 * @param [in] nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS                   : Returns ADI_EAL_STATUS_SUCCESS if FIO UART setup periodic write read frame was sent successful.
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Buffer not available for this request
 *             - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_LATCH_FAILED : Failed to set FIFO in latching mode
 *             - #ADI_EAL_STATUS_FIFO_FAILED       : Failed to add FIFO transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartSetupPeriodicWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(poFioUartData != NULL, "FIO_UART IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioUartData->nReadLength <= 255U, "FIO_UART Read length is invalid");
    ADI_DBG_REQUIRE(poFioUartData->nWriteLength <= 255U, "FIO_UART Write length is invalid");

    /* Get the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    if(poEalFioUartInstData->bIsCanEchoCancel == false)
    {
        ADI_DBG_REQUIRE(poFioUartData->nReadLength + poFioUartData->nWriteLength <= 255U, "When echo is enabled "
                        "the total of read length and write length should be less than or equal to 255");
    }

    /* Set the FIO_UART transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked          = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr           = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent         = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId                  = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen       = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen        = true;
    oTxnCfg.oTxnHdr.bIsRouterHeaderInserted = true;

    /* Loop through the mutex of all instances and lock it */
    LockFioUartMutex(poEalFioUartData);
    uint16_t nWriteLength;
    /* Encode UART data */
    ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
    oUpdateUartTxnData.bCancelEcho                  = poEalFioUartInstData->bIsCanEchoCancel;
    oUpdateUartTxnData.nReadLength                  = poFioUartData->nReadLength;
    oUpdateUartTxnData.nWriteLength                 = poFioUartData->nWriteLength;
    oUpdateUartTxnData.pTxData                      = poFioUartData->pTxData;
    oUpdateUartTxnData.pFormattedTxData             = poFioUartData->pFormattedTxData;
    oUpdateUartTxnData.nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
    oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = 0U;
    oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;

    /* If echo cancellation is disabled and we have breakfields in the payload, then we have to account for breakfield echo as well */
    if((poEalFioUartInstData->bIsCanEchoCancel == false))
    {
        oUpdateUartTxnData.nReadLength +=
            (poEalFioUartInstData->nBreakFieldBits == 0U ? 0U : 1U);
    }

    UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

    oTxnCfg.pPayload          = poFioUartData->pFormattedTxData;
    oTxnCfg.oTxnHdr.nWriteLen = nWriteLength;

    if(poEalFioUartInstData->bIsCanEchoCancel)
    {
        oTxnCfg.oTxnHdr.nReadLen = oUpdateUartTxnData.nReadLength * 2U;
    }
    else
    {
        oTxnCfg.oTxnHdr.nReadLen = (oUpdateUartTxnData.nReadLength * 2U) + (poFioUartData->nWriteLength * 2U);
    }

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalFioUartData->bIsMulticast;

    /* Use the locally created FIO_UART transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poFioUartTxnCfg = &oTxnCfg;

    do
    {
        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalFioUartInstData, poFioUartTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalFioUartData, poEalFioUartInstData, bIsMcast, &poFioUartTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalFioUartData, bIsMcast, poFioUartData->apRxData) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Switch the mode of all the instances to periodic */
        for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
        {
            EAL_FIO_UART_INST_DATA *poEalFioUartUnicastInstData = poEalFioUartData->apoInstData[i];

            if(adi_e2bcore_addFlexibleIOConfigToQueue(&(poEalFioUartUnicastInstData->oId), poFioUartTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }

            poEalFioUartUnicastInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
        }
    }

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief To write/read to/from interface fifo based on the transaction config
 *
 * @param [in] hFioUartHandle   Handle corresponding to this instance
 * @param [in] paoFioUartTxnCfg FIO UART transaction configs
 * @param [in] nNumFioUartTxns  Number of transaction configs
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO UART custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartCustomWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t nNumFioUartTxns)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(paoFioUartTxnCfg != NULL, "FIO_UART transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumFioUartTxns > 0U, "Number of FIO_UART transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioUartData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* Lock the mutex */
    LockFioUartMutex(poEalFioUartData);

    /* Get the number of transactions that expects a response */
    uint16_t nNumTxnsWithResp = GetNumTxnsWithResp(poEalFioUartInstData, paoFioUartTxnCfg, nNumFioUartTxns);

    /* Send the custom FIO_UART transactions */
    eEalStatus = SendCustomFioUartTransactions(poEalFioUartData, poEalFioUartInstData, paoFioUartTxnCfg, nNumFioUartTxns,
                 nNumTxnsWithResp);

    /* Unlock the mutex */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief Function to send multiple FIO UART write/read transactions with break field
 *
 * @param [in] hFioUartHandle    FIO UART handle (can be unicast/multicast)
 * @param [in] paoFioUartIoData  Pointer to array of FIO UART IO with breakfield data structures
 * @param [in] nNumFioUartTxns   Number of FIO UART transactions, pointed to by paoFioUartIoData
 * @param [in] pFormattedTxData  Pointer to hold formatted TX data, should be pointing to the memory of max size required for any transaction in the array
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO UART custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartMultiTxnWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_TXN_DESC *paoFioUartIoData, uint16_t nNumFioUartTxns, uint16_t *pFormattedTxData)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(paoFioUartIoData != NULL, "FIO_UART transaction pointer is NULL");
    ADI_DBG_REQUIRE(nNumFioUartTxns > 0U, "Number of FIO_UART transactions is invalid");
    ADI_DBG_REQUIRE(pFormattedTxData != NULL, "Formatted TX data pointer is NULL");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Lock the mutex */
    LockFioUartMutex(poEalFioUartData);

    /* Check if there is enough space for all transactions in all unicast instances provided by the handle */
    bool bNoSpace = CheckQueueSpaceForRspTxn(poEalFioUartData, paoFioUartIoData, nNumFioUartTxns);

    /* Proceed if there is enough space in the config queue */
    if(!bNoSpace)
    {
        eEalStatus = SendFioUartTxnsWithBreak(poEalFioUartData, paoFioUartIoData, nNumFioUartTxns, pFormattedTxData);
    }
    else
    {
        eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
    }

    /* Unlock the mutex */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief       Reads the status of the FIO UART interface
 *
 * @param [in]  hFioUartHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Read successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartReadStatus(ADI_EAL_FIO_UART_HANDLE hFioUartHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    bool bIsMcast = poEalFioUartData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalFioUartData->nIfRegVlanTag : *poEalFioUartInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalFioUartData->nIntfTopic;
        anTopics[1U] = poEalFioUartData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalFioUartInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalFioUartInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalFioUartInstData->oId, FLEXIO_NUM_BYTES_NW_ADDR, 2U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief Write to Flexible-IO UART interface register
 *
 * @param [in]  hFioUartHandle   Handle corresponding to this instance
 * @param [in]  bIsFifoRegTransaction  Is this a reg transaction for FIFO
 * @param [in]  poRegCfg        Register write configuration
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartSendRegTransaction(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum   = poEalFioUartInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalFioUartInstData->oId.nRemoteNum;
    uint32_t nIntfNum   = poEalFioUartInstData->oId.nIntfNum;

    bool bIsMcast = poEalFioUartData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bMailbox       = poEalFioUartData->bMailboxMode;
    oMsgOpts.bSync          = poEalFioUartData->bResyncPending;
    oMsgOpts.bTopicOverRide = true;

    poEalFioUartData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioUartData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalFioUartData->nFifoRegVlanTag : *poEalFioUartInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioUartData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalFioUartData->nIfRegVlanTag : *poEalFioUartInstData->pnIfRegVlanTag;
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
 * @brief       Resets the FIO transaction queue and goes back to a clean state
 *
 * @param [in]  hFioUartHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-UART reset successful
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartResetQueue(ADI_EAL_FIO_UART_HANDLE hFioUartHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioUartData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* Lock the mutex */
    LockFioUartMutex(poEalFioUartData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearFlexibleIOConfigQueue(&poEalFioUartInstData->oId);
        poEalFioUartInstData->nFioUartQWrIdx = 0U;
        poEalFioUartInstData->nFioUartQRdIdx = 0U;
        poEalFioUartInstData->eMode          = ADI_EAL_USAGE_MODE_NORMAL;

        /* Ignore errors */
        poEalFioUartInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr = FLEXIO_CONTROL_2_NW_ADDR;
        uint32_t nMask      = BITM_FLEXIO_CONTROL_2_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      = BITM_FLEXIO_CONTROL_2_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask      = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType       = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength     = 1U;
        oRegCfg.pBuffer     = &nData;
        eEalStatus = adi_eal_fioUartSendRegTransaction((ADI_EAL_FIO_UART_HANDLE)poEalFioUartData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an FIO_UART NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalFioUartInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);

        /* Issue FIFO resets */
        nStartAddr = FLEXIO_CONTROL_NW_ADDR;
        nMask      = BITM_FLEXIO_CONTROL_TX_FIFO_RESET | BITM_FLEXIO_CONTROL_RX_FIFO_RESET;
        nData      = BITM_FLEXIO_CONTROL_TX_FIFO_RESET | BITM_FLEXIO_CONTROL_RX_FIFO_RESET;
        eEalStatus = adi_eal_fioUartSendRegTransaction((ADI_EAL_FIO_UART_HANDLE)poEalFioUartData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalFioUartInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for FIO UART instance
 *
 * @param [in] hFioUartHandle     FIO UART instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
void adi_eal_fioUartSetDownstreamVlan(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                      ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Extract the FIO-Uart data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    /* Get the unicast instance data from the handle */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* If it is a multicast handle, update the handle structure */
    if(poEalFioUartData->bIsMulticast)
    {
        poEalFioUartData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalFioUartData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalFioUartData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum   = poEalFioUartInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioUartInstData->oId.nRemoteNum;
        uint32_t nIntfNum   = poEalFioUartInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for FIO UART FIFO messages
 *
 * @param [in] hFioUartHandle  FIO UART interface Handle
 * @param [in] poVlanTag       Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn       If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartSetUpstreamVlan(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Extract the FIO-Uart data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    ADI_DBG_REQUIRE(poVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* Add VLAN tag info */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    uint16_t nStartAddr = FIFO_CTAG_CONFIGURATION_NW_ADDR;
    uint32_t nRegData = (bIsVlanEn ? ((uint32_t)poVlanTag->nVlanId & BITM_FIFO_CTAG_CONFIGURATION_CTAG_ID) : 0U) |
                        (poVlanTag->bDEI ?
                         (uint32_t)BITM_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE : 0U) | (((uint32_t)poVlanTag->nPCP & 0x07U) <<
                                 BITP_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY) | (bIsVlanEn ? BITM_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG : 0U);
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength     = 1U;
    oRegCfg.pBuffer     = &nRegData;
    oRegCfg.eType       = ADI_E2BCORE_RND_WRITE;

    return adi_eal_fioUartSendRegTransaction(poEalFioUartData, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for FIOUART transactions
 *
 * @param [in]  hFioUartHandle   The FIOUART handle
 * @param [in]  bEnable          Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartSetMailboxMode(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    /* Set the mailbox mode flag */
    poEalFioUartData->bMailboxMode = bEnable;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for FIOUART transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hFioUartHandle   The FIOUART handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartResyncSeqNum(ADI_EAL_FIO_UART_HANDLE hFioUartHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    /* Set the sync mode flag */
    poEalFioUartData->bResyncPending = true;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables or disables echo cancellation for FIO UART transactions
 *
 * @param [in]  hFioUartHandle   The FIO UART handle
 * @param [in]  bEnable          Flag to enable/disable echo cancellation
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Echo cancellation mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartSetEchoCancel(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);

    /* Set the echo cancel flag for all unicast instances */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];
        poEalFioUartInstData->bIsCanEchoCancel = bEnable;
    }

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief To write/read to/from interface fifo based on the transaction config
 *
 * @param [in] hFioUartHandle   Handle corresponding to this instance
 * @param [in] paoFioUartTxnCfg FIO UART transaction configs
 * @param [out] anRxPayload     Pointer to array of pointers for received payload data
 * @param [in] nNumFioUartTxns  Number of transaction configs
 * @param [in] bIsTransmitPkt   Frame has to transmit or not
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO UART custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 * Limitation - When this API is used in the application no other FIO UART eal APIs can be used
 *              in the same application.
 */

ADI_EAL_FIOUART_CODE
ADI_EAL_STATUS adi_eal_fioUartPackedDataTransmit(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t **anRxPayload, uint16_t nNumFioUartTxns,
        bool bIsTransmitPkt)
{
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hFioUartHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioUartData);
    ADI_DBG_REQUIRE(paoFioUartTxnCfg != NULL, "FIO_UART transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumFioUartTxns > 0U, "Number of SPI transactions is invalid");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Lock the mutex */
    LockFioUartMutex(poEalFioUartData);

    /* Is it a multicast request? */
    bool bIsMcast = poEalFioUartData->bIsMulticast;
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];
    uint32_t nInstNum = poEalFioUartInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalFioUartInstData->oId.nRemoteNum;
    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;
    uint16_t nNumTxnsWithResp  = 0u;

    if(bIsMcast == true)
    {
        nRemoteNum = ADI_EAL_ALL_REMOTES;
    }

    for(uint8_t nFioUartTxn = 0u; nFioUartTxn < nNumFioUartTxns; nFioUartTxn++)
    {
        /* Use the locally created FIO_UART transaction by default*/
        ADI_E2BCORE_TXN_CONFIG *poFioUartTxnCfg = &paoFioUartTxnCfg[nFioUartTxn];

        bool bIsResponseExpected = poFioUartTxnCfg->oTxnHdr.bIsE2bIdPresent || (poFioUartTxnCfg->oTxnHdr.nReadLen > 0U)
                                   || (poEalFioUartInstData->bIsCanEchoCancel == false);

        /* Check if there is enough space in the transaction queue */
        if(bIsResponseExpected == true)
        {
            nNumTxnsWithResp++;
        }
    }

    do
    {
        eEalStatus = CheckQueueSpaceForTransaction(poEalFioUartData, bIsMcast);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        uint16_t nSpaceLeftInQueue;

        /* Space remaining in the transaction queue */
        for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
        {
            EAL_FIO_UART_INST_DATA *poEalFioUartInstCheckSpace = poEalFioUartData->apoInstData[i];
            nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                    poEalFioUartInstCheckSpace->nFioUartQWrIdx,
                                    poEalFioUartInstCheckSpace->nFioUartQRdIdx,
                                    poEalFioUartInstCheckSpace->nFioUartCfgQDepth);

            /* Check if there is enough space in the transaction queue */
            if(nNumTxnsWithResp > nSpaceLeftInQueue)
            {
                eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalFioUartInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* Check whether network buffer initialized */
        if(!gaoEalData[nInstNum].bBuffInit)
        {
            uint8_t *pBuffer = NULL;
            /* Get the device number mapped to this instance */
            uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
            /* Get a buffer from the network layer for filling the e2b data */
            ADI_NETWORK_ERR eNwErr = adi_network_getTxBuffer(nDevNum, &pBuffer, &oNwFrameCfg);

            if(eNwErr != ADI_NETWORK_OK)
            {
                eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
                bShouldReleaseNwMutex = true;
                break;
            }

            /* Start filling e2b data into the buffer */
            if(adi_e2bcore_startPacket(nInstNum, oNwFrameCfg.panPhysAddr, pBuffer, oNwFrameCfg.nLenByte,
                                       false) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_START_FAILED;
                bShouldReleaseNwMutex = true;
                break;
            }

            gaoEalData[nInstNum].bBuffInit = true;
        }

        /* Set the FIFO message options (using persistent storage in handle) */
        (void)ADI_MEMSET(&poEalFioUartData->oFifoMsgOpts, 0, sizeof(poEalFioUartData->oFifoMsgOpts));
        poEalFioUartData->oFifoMsgOpts.bMailbox       = poEalFioUartData->bMailboxMode;
        poEalFioUartData->oFifoMsgOpts.bSync          = poEalFioUartData->bResyncPending;
        poEalFioUartData->oFifoMsgOpts.bTopicOverRide = true;
        poEalFioUartData->oFifoMsgOpts.nTopic         = poEalFioUartData->bIsMulticast ? poEalFioUartData->nFifoTopic :
                ADI_E2BCORE_TOPIC_FIFO0 + poEalFioUartInstData->oId.nIntfNum;

        /* Start e2b FIFO write message */
        if(adi_e2bcore_startMessage(nInstNum, &poEalFioUartData->oFifoMsgOpts,
                                    E2B_MSGTYPE_FIFO_WRITE_IO) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            bShouldReleaseNwMutex = true;
            break;
        }

        eEalStatus = AddFioUartTxnsToMsg(nInstNum, poEalFioUartData, poEalFioUartInstData, paoFioUartTxnCfg, anRxPayload,
                                         nNumFioUartTxns);

        for(uint8_t nFioUartTxn = 0u; nFioUartTxn < nNumFioUartTxns; nFioUartTxn++)
        {
            /* Use the locally created FIO_UART transaction by default.
            NOTE: It will be overwritten if the configuration
                 should come from the EAL transaction queue */
            /* Use the locally created FIO_UART transaction by default*/
            ADI_E2BCORE_TXN_CONFIG *poFioUartTxnCfg = &paoFioUartTxnCfg[nFioUartTxn];
            bool bIsResponseExpected = poFioUartTxnCfg->oTxnHdr.bIsE2bIdPresent || (poFioUartTxnCfg->oTxnHdr.nReadLen > 0U);

            if(bIsResponseExpected == true)
            {
                AddTransactionToQueues(poEalFioUartData, poEalFioUartInstData, bIsMcast, &poFioUartTxnCfg);
            }
        }

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        if(!bIsTransmitPkt)
        {
            /* Finish adding all FIO_UART transactions into the FIFO message */
            adi_e2bcore_finishMessage(nInstNum, &poEalFioUartData->oFifoMsgOpts);
        }
        else
        {
            /* Finishes E2B message and transmits */
            eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &poEalFioUartData->oFifoMsgOpts);

            /* Clearing the buffer init flag after transmitting the frame on bus */
            gaoEalData[nInstNum].bBuffInit = false;
        }

    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    /* Unlock the mutex */
    UnlockFioUartMutex(poEalFioUartData);

    return eEalStatus;
}

/*============================ CALLBACKS ============================*/
/**
 * @brief The callback for FIO UART interface
 * @param [in] poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param [in] eType         The type of event this callback is for
 * @param [in] poParam       The data corresponding to this event
 * @param [in] bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param [in] pCbParam      Parameter to be passed back while invoking callback
 */
ADI_EAL_FIOUART_CODE
void FioUartInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                         void *pCbParam)
{
    /* If pCbParam is NULL, the EAL FIO-Uart interface instance is not opened,
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
                HandleFifoReadCallback(poID, poParam, pCbParam);
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

/** @cond PRIVATE */
/*============================= STATIC FUNCTIONS ============================*/
/*! Sends the FIO_UART transaction */
ADI_EAL_FIOUART_CODE
static ADI_EAL_STATUS SendFioUartTransaction(
    EAL_FIO_UART_DATA *poEalFioUartData,
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg,
    uint16_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod
)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalFioUartData->bIsMulticast;

    /* Use the locally created FIO_UART transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poFioUartTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalFioUartInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalFioUartData, poEalFioUartInstData, bIsMcast, &poFioUartTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalFioUartData->nFifoWriteVlanTag : *poEalFioUartInstData->pnFifoWriteVlanTag;

        eal_utils_fillNetworkCfg(poEalFioUartInstData->oId.nInstNum,
                                 poEalFioUartData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalFioUartInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag);

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalFioUartData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalFioUartData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalFioUartData->bIsMulticast ?
                                      poEalFioUartData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalFioUartInstData->oId.nIntfNum;

        poEalFioUartData->bResyncPending = false;

        if(!bIsPeriodic)
        {
            /* Send the unicast / multicast e2b packet with the FIFO message containing the FIO_UART transaction */
            eEalStatus = eal_utils_sendFifoFrame(&poEalFioUartInstData->oId, &oNwFrameCfg, poFioUartTxnCfg, &oFifoMsgOpts);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalFioUartData, bIsMcast, apRxBuf) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/* Encodes the UART TX data */
ADI_EAL_FIOUART_CODE
static void UpdateUartTxData(EAL_FIO_UART_DATA *poEalFioUartData,
                             ADI_EAL_FIO_UART_UPDATE_TXN_DATA *poUpdateUartTxnData)
{
    /* Get the unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* nWriteLength has the number of bytes to transmit on UART TX line ,
    One byte of data is actually sent out as 2 bytes as the encoding of TX data includes start bit, stop bit and
    actual data bits (sent by the application) with/without parity */

    /* Working buffer - supports up to 593 data bytes + breakfield */

    uint8_t *anPackedBuffer = poEalFioUartInstData->anPackedTmp;

    /* Initialize buffer */
    (void)ADI_MEMSET(anPackedBuffer, 0, 1188U);

    uint16_t nBufferIndex = 0U;

    /* Add breakfield if specified, before the payload */
    AddBreakfieldBits(anPackedBuffer, &nBufferIndex,
                      poEalFioUartInstData, poUpdateUartTxnData->nBreakFieldBitsBeforePayload);

    /* Pack each data byte with UART framing */
    for(uint16_t i = 0U; i < poUpdateUartTxnData->nWriteLength; i++)
    {
        PackUartDataByte(poUpdateUartTxnData->pTxData[i], poEalFioUartInstData, anPackedBuffer, &nBufferIndex);
    }

    /* Add breakfield if specified, after the payload */
    AddBreakfieldBitsAfterPayload(&anPackedBuffer[nBufferIndex], &nBufferIndex,
                                  poEalFioUartInstData, poUpdateUartTxnData->nBreakFieldBitsAfterPayload);

    /* Format packed data with routing headers */
    FormatPackedDataWithHeaders(anPackedBuffer, nBufferIndex,
                                poUpdateUartTxnData);
}

ADI_EAL_FIOUART_CODE
static void AddBreakfieldBits(uint8_t *anPackedBuffer, uint16_t *pnBufferIndex,
                              EAL_FIO_UART_INST_DATA *poEalFioUartInstData, uint8_t nBreakFieldBits)
{
    do
    {
        if(nBreakFieldBits > 0U)
        {
            /* Calculate total frame bits: start(1) + data(7-12) + parity(0 or 1) + stop bits(1 or 2) */
            uint8_t nNumOfDataBits = poEalFioUartInstData->nNumOfDataBits;
            uint8_t nNumOfStopBits = poEalFioUartInstData->nNumOfStopBits;
            uint8_t nParityCount   = poEalFioUartInstData->bIsParityEn ? 1U : 0U;

            uint8_t nTotalFrameBits = 1U + nNumOfDataBits + nNumOfStopBits + nParityCount;

            if(nBreakFieldBits < nTotalFrameBits)
            {
                break;  /* Breakfield bits logically need to be atleast longer than total frame bits */
            }

            /* Breakfield bits that can fit in middle byte pairs (full capacity) */
            uint8_t nBitsPerMiddlePair = nTotalFrameBits;

            /* Breakfield bits that can fit in last byte pair (reduced by 1) */
            uint8_t nBitsInLastPair = nBitsPerMiddlePair - nNumOfStopBits;

            uint8_t nBytePairCount;

            /* Calculate byte pairs needed: 1 for last + ceil(remaining / middle_capacity) */
            uint8_t nRemainingAfterLast = nBreakFieldBits - nBitsInLastPair;
            nBytePairCount = ((nRemainingAfterLast + nBitsPerMiddlePair - 1U) / nBitsPerMiddlePair) + 1U;


            *pnBufferIndex = (uint16_t)nBytePairCount * 2U;  /* Each byte pair is 2 bytes */
            int16_t nByteIndex         = (int16_t)(*pnBufferIndex) - 1;
            uint8_t nRemainingBits     = nBreakFieldBits;
            int16_t nLastBytePairIndex = (int16_t)(nBytePairCount) - 1;

            for(int16_t i = nLastBytePairIndex; i >= 0; i--)
            {
                if(i == nLastBytePairIndex) /* 2 bytes needed to encode the breakfield */
                {
                    uint8_t byte1 = 0x00U;
                    uint8_t byte2 = 0xFFU;

                    /* Position where first stop bit starts in byte2 */
                    uint8_t nStopPos = ((nNumOfDataBits > 7U) ? (nNumOfDataBits - 7U) : 0U) + nParityCount;
                    byte2 = (uint8_t)(0xFFU << nStopPos);  /* Stop bits need to be high */

                    anPackedBuffer[nByteIndex--] = byte2;
                    anPackedBuffer[nByteIndex--] = byte1;

                    nRemainingBits = nRemainingBits - nBitsInLastPair;
                }

                else if((i < nLastBytePairIndex) && (i > 0))
                {
                    anPackedBuffer[nByteIndex--] = 0x00U;
                    anPackedBuffer[nByteIndex--] = 0x00U;
                    nRemainingBits -= nBitsPerMiddlePair;
                }

                else
                {
                    uint8_t byte1 = 0xFFU;
                    uint8_t byte2 = 0xFFU;

                    /* Position where first padding bit starts in byte2 (this is also the count of max breakfield bits byte 2 can hold) */
                    uint8_t nPadPos = ((nNumOfDataBits > 7U) ? (nNumOfDataBits - 7U) : 0U) + nParityCount + nNumOfStopBits;

                    byte2 = byte2 >> (8U - nPadPos);

                    if(nRemainingBits <= nPadPos)
                    {
                        byte2 = byte2 >> nRemainingBits;
                        nRemainingBits = 0U;
                    }
                    else
                    {
                        byte2 = 0x00U;
                        nRemainingBits = nRemainingBits - nPadPos;
                    }

                    if(nRemainingBits < 8U)
                    {
                        byte1 = byte1 >> nRemainingBits;
                    }
                    else
                    {
                        byte1 = 0x00U;  /* Shifting 8 bits on uint8_t results in 0 */
                    }

                    anPackedBuffer[nByteIndex--] = byte2;
                    anPackedBuffer[nByteIndex--] = byte1;
                }
            }
        }

    } while(false);
}

ADI_EAL_FIOUART_CODE
static void AddBreakfieldBitsAfterPayload(uint8_t *anPackedBuffer, uint16_t *pnBufferIndex,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData, uint8_t nBreakFieldBits)
{
    uint16_t nBufferIndex = 0U;

    do
    {
        if(nBreakFieldBits > 0U)
        {
            /* Calculate total frame bits: start(1) + data(7-12) + parity(0 or 1) + stop bits(1 or 2) */
            uint8_t nNumOfDataBits = poEalFioUartInstData->nNumOfDataBits;
            uint8_t nNumOfStopBits = poEalFioUartInstData->nNumOfStopBits;
            uint8_t nParityCount = poEalFioUartInstData->bIsParityEn ? 1U : 0U;

            uint8_t nTotalFrameBits = 1U + nNumOfDataBits + nNumOfStopBits + nParityCount;

            /* Calculate the number of byte pairs required to encode the breakfield bits */
            uint8_t nBytePairCount = ((nBreakFieldBits - 1U) / nTotalFrameBits) + 1U;

            for(uint8_t i = 0U; i < nBytePairCount; i++)
            {
                /* Data(6:0)[7:1] + start bit[0] */
                anPackedBuffer[nBufferIndex]      = 0x00U;

                /* Padding + Parity if present + Stop bit + Data bit(7) */
                anPackedBuffer[nBufferIndex + 1U] = 0X00U;

                /* Last byte pair needs STOP bits to be added */
                if(i == (nBytePairCount - 1U))
                {
                    /* Since we have byte count required to encode breakbits -
                      the last byte pair must have remaining breakbits less than or equal to totalframe bits */
                    uint8_t nRemainingBreakBits = nBreakFieldBits - (nTotalFrameBits * i);
                    ADI_DBG_ENSURE(nRemainingBreakBits <= nTotalFrameBits,
                                   "Remaining break bits should be less than or equal to total frame bits");

                    if(nRemainingBreakBits < 8U)
                    {
                        anPackedBuffer[nBufferIndex]      = (uint8_t)(0xFFU << nRemainingBreakBits);
                        anPackedBuffer[nBufferIndex + 1U] = 0xFFU;
                    }
                    else
                    {
                        anPackedBuffer[nBufferIndex + 1U] = (uint8_t)((uint16_t)0x00FFU << (nRemainingBreakBits - 8U));
                    }

                    /* Calculate the number of bit space available after adding the remaining break bits */
                    uint8_t nBitSpaceLeft = nTotalFrameBits - nRemainingBreakBits;

                    /* Check if we have enough bitspace to add stop bits at the end -
                       if we did then we the previous transaction already has the stop bits
                       less add an extra transactions with all '1' as STOP bit and IDLE bits are same */
                    if(nBitSpaceLeft < nNumOfStopBits)
                    {
                        anPackedBuffer[nBufferIndex + 2U] = 0xFFU;
                        anPackedBuffer[nBufferIndex + 3U] = 0xFFU;
                        nBufferIndex += 2U;
                    }
                }

                nBufferIndex += 2U;
            }
        }

    } while(false);

    *pnBufferIndex += nBufferIndex;
}

ADI_EAL_FIOUART_CODE
static void PackUartDataByte(uint16_t nDataBits, EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                             uint8_t *anPackedBuffer, uint16_t *pnBufferIndex)
{
    uint8_t nParity = 0U;
    uint8_t nCurrentByte;
    uint8_t nNumDataBits;
    uint8_t nNumStopBits;
    uint8_t nBitsInFirstByte;
    uint8_t nBitsInSecondByte;
    uint8_t nSecondByteStartPos;

    /* Get parameter configuration from instance data */
    bool bIsParityEnabled = poEalFioUartInstData->bIsParityEn;
    ADI_EAL_PARITY_TYPE eParityType = poEalFioUartInstData->eParityType;
    nNumDataBits = poEalFioUartInstData->nNumOfDataBits;
    nNumStopBits = poEalFioUartInstData->nNumOfStopBits;

    do
    {
        /* Mask input data to actual number of data bits being used (7-12 bits) */
        uint16_t nMaskedData = nDataBits & ((uint16_t)(((uint16_t)1U << nNumDataBits) - (uint16_t)1U));

        /* Calculate parity if enabled */
        if(bIsParityEnabled)
        {
            nParity = eal_utils_computeParity16(eParityType, nDataBits);
        }

        /* Calculate bit distribution between two bytes:
        * Byte 1: Start bit (0) at LSB + up to 7 data bits (positions 1-7)
        * Byte 2: Remaining data bits + parity (optional) + stop bits + padding 1s
        */
        nBitsInFirstByte = (nNumDataBits > 7U) ? 7U : nNumDataBits;
        nBitsInSecondByte = (nNumDataBits > 7U) ? (nNumDataBits - 7U) : 0U;

        /* Byte 1: Start bit (0) at LSB + lower data bits shifted left by 1 */
        nCurrentByte = (uint8_t)((nMaskedData & ((uint16_t)(((uint16_t)1U << nBitsInFirstByte) - (uint16_t)1U))) <<
                                 1U);  /* Data bits [6:0] shifted left and start bit at LSB*/
        anPackedBuffer[(*pnBufferIndex)++] = nCurrentByte;

        /* Byte 2: Build from LSB to MSB */
        nSecondByteStartPos = 0U;
        nCurrentByte        = 0U;

        /* Add remaining data bits (if any) starting from LSB */
        if(nBitsInSecondByte > 0U)
        {
            nCurrentByte = (uint8_t)((nMaskedData >> nBitsInFirstByte) & ((1U << nBitsInSecondByte) - 1U));
            nSecondByteStartPos = nBitsInSecondByte;
        }

        /* Add parity bit at next position */
        if(bIsParityEnabled)
        {
            nCurrentByte |= (uint8_t)(nParity << nSecondByteStartPos);
            nSecondByteStartPos++;
        }

        /* Add stop bits - all stop bits are '1' */
        if(nNumStopBits == 1U)
        {
            nCurrentByte |= (uint8_t)(1U << nSecondByteStartPos);
        }
        else if(nNumStopBits == 2U)
        {
            nCurrentByte |= (uint8_t)(3U << nSecondByteStartPos);
        }
        else
        {
            /* Invalid number of stop bits - should not reach here due to configuration validation */
            ADI_DBG_ERROR();
            break;
        }

        /* Padding bits can be either 1 or 0 so no extra logic needed */

        anPackedBuffer[(*pnBufferIndex)++] = nCurrentByte;

    } while(false);
}

ADI_EAL_FIOUART_CODE
static void FormatPackedDataWithHeaders(uint8_t *anPackedBuffer,
                                        uint16_t nBufferIndex,
                                        ADI_EAL_FIO_UART_UPDATE_TXN_DATA *poUpdateUartTxnData)
{
    uint16_t nWordIndex    = 0U;
    uint16_t nCurrentWord  = 0U;
    uint16_t *pnLastHeader = &poUpdateUartTxnData->pFormattedTxData[0U];
    uint8_t nByteInWord    = 0U;  /* Track position within current word (0 or 1) */
    bool bIsHeaderInUpperByte = false;

    /* Add first header byte(s) */
    if(poUpdateUartTxnData->bCancelEcho == false)
    {
        nCurrentWord = 0x40U;  /* Header marker in LSB */
        nCurrentWord |= ((uint16_t)(poUpdateUartTxnData->nWriteLength + poUpdateUartTxnData->nReadLength)) << 8U;
        poUpdateUartTxnData->pFormattedTxData[nWordIndex++] = nCurrentWord;
        nByteInWord  = 0U;
        nCurrentWord = 0U;
    }
    else
    {
        nCurrentWord = 0x00U;  /* Header marker */
        nByteInWord  = 1U;
    }

    /* Copy packed data with header insertions every 4 bytes */
    uint16_t nBytesProcessed = (poUpdateUartTxnData->bCancelEcho == false) ? 2U : 1U;
    uint16_t nDataBytesAfterLastHeader = 0U;  /* Track data bytes since last header */

    for(uint16_t i = 0U; i < nBufferIndex; i++)
    {
        /* Insert separator header every 4 data bytes */
        if((nBytesProcessed % 5U) == 0U)
        {
            pnLastHeader = &poUpdateUartTxnData->pFormattedTxData[nWordIndex];

            if(nByteInWord == 1U)
            {
                /* Complete current word with header. Since header is 0x00 nothing needs to be done */
                bIsHeaderInUpperByte = true;
                poUpdateUartTxnData->pFormattedTxData[nWordIndex++] = nCurrentWord;
                nByteInWord  = 0U;
                nCurrentWord = 0U;
            }
            else
            {
                /* Start new word with header */
                nCurrentWord = 0x00U;  /* Separator header in LSB */
                bIsHeaderInUpperByte = false;
                nByteInWord  = 1U;
            }

            nBytesProcessed++;
            nDataBytesAfterLastHeader = 0U;  /* Reset counter after header insertion */
        }

        /* Add data byte */
        if(nByteInWord == 0U)
        {
            nCurrentWord = anPackedBuffer[i];
            nByteInWord  = 1U;
        }
        else
        {
            nCurrentWord |= ((uint16_t)anPackedBuffer[i]) << 8U;
            poUpdateUartTxnData->pFormattedTxData[nWordIndex++] = nCurrentWord;
            nByteInWord  = 0U;
            nCurrentWord = 0U;
        }

        nBytesProcessed++;
        nDataBytesAfterLastHeader++;
    }

    /* Add final header for Rx length if CanEchoCancel is true */
    if((poUpdateUartTxnData->bCancelEcho == true) && (poUpdateUartTxnData->nReadLength > 0U))
    {
        /* Check whether the last state machine is followed by 4 bytes of payload,
           if not, change the last state machine header to include the payload bytes
           for RX state machine as well */
        if(nDataBytesAfterLastHeader == 1U)
        {
            if(bIsHeaderInUpperByte == true)
            {
                pnLastHeader[0u] = (pnLastHeader[0u] & 0x00FFU) | 0x1000U;
            }
            else
            {
                pnLastHeader[0u] = (pnLastHeader[0u] & 0xFF00U) | 0x0010U;
            }
        }
        else if(nDataBytesAfterLastHeader == 2U)
        {
            if(bIsHeaderInUpperByte == true)
            {
                pnLastHeader[0u] = (pnLastHeader[0u] & 0x00FFU) | 0x0400U;
            }
            else
            {
                pnLastHeader[0u] = (pnLastHeader[0u] & 0xFF00U) | 0x0004U;
            }
        }
        else if(nDataBytesAfterLastHeader == 3U)
        {
            if(bIsHeaderInUpperByte == true)
            {
                pnLastHeader[0u] = (pnLastHeader[0u] & 0x00FFU) | 0x0100U;
            }
            else
            {
                pnLastHeader[0u] = (pnLastHeader[0u] & 0xFF00U) | 0x0001U;
            }
        }
        else
        {
            /* No change needed if 4 payload bytes are present after last header, instead we add a new
               RX SM header */
            if(nByteInWord == 1U)
            {
                nCurrentWord |= ((uint16_t)0x40U) << 8U;
                poUpdateUartTxnData->pFormattedTxData[nWordIndex++] = nCurrentWord;
                nByteInWord = 0U;
            }
            else
            {
                nCurrentWord = 0x40U;
                nByteInWord = 1U;
            }
        }

        /* Now pack read length */
        if(nByteInWord == 1U)
        {
            nCurrentWord |= ((uint16_t)(uint8_t)poUpdateUartTxnData->nReadLength) << 8U;
            poUpdateUartTxnData->pFormattedTxData[nWordIndex++] = nCurrentWord;
            nByteInWord = 0U;
        }
        else
        {
            nCurrentWord |= ((uint8_t)poUpdateUartTxnData->nReadLength);
            nByteInWord = 1U;
        }
    }

    /* Handle any remaining odd byte */
    if(nByteInWord == 1U)
    {
        poUpdateUartTxnData->pFormattedTxData[nWordIndex++] = nCurrentWord;
    }

    /* Calculate total bytes from what was actually written */
    uint16_t nTotalBytes = nWordIndex * 2U;  /* Each word is 2 bytes */

    /* If the last word only had 1 byte (not a full word), subtract 1 */
    if(nByteInWord == 1U)
    {
        nTotalBytes -= 1U;
    }

    /* Return total formatted length */
    *(poUpdateUartTxnData->pnWriteLengthOutput) = nTotalBytes;
}

/*! Lock the mutex of all FIO_UART instances */
ADI_EAL_FIOUART_CODE
static void LockFioUartMutex(EAL_FIO_UART_DATA *poEalFioUartData)
{
    /* Loop through all the unicast FIO_UART instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalFioUartData->apoInstData[i]->hFioUartMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all FIO_UART instances */
ADI_EAL_FIOUART_CODE
static void UnlockFioUartMutex(EAL_FIO_UART_DATA *poEalFioUartData)
{
    /* Loop through all the unicast FIO_UART instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalFioUartData->apoInstData[i]->hFioUartMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Set the waiting for RX flag for all the instances of the handle */
ADI_EAL_FIOUART_CODE
static void SetWaitingForRx(EAL_FIO_UART_DATA *poEalFioUartData, bool bWaitingForRx)
{
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalFioUartInstData->bWaitingForRx = bWaitingForRx;
    }
}

/*! Wait for response */
ADI_EAL_FIOUART_CODE
static bool WaitForResponses(EAL_FIO_UART_DATA *poEalFioUartData)
{

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    bool bTimedOut = false;

    /* Loop through all instances and check if response is pending */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];

        /* Check if we are still waiting for response for this instance */
        if(prj_osal_waitSemaphore(poEalFioUartInstData->hFifoRespSemaphore) != PRJ_OSAL_STATUS_SUCCESS)
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

    /* Wait for FIO_UART write to be received by the host interface and
       send back a No-op transaction with the ID sent downstream */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);
        bAllResponsesReceived = true;

        for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalFioUartInstData->bWaitingForRx)
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
ADI_EAL_FIOUART_CODE
static bool IsResponseExpected(EAL_FIO_UART_INST_DATA *poEalFioUartData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The transaction has read length greater than 0, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            (poTxnCfg->oTxnHdr.nReadLen > 0U) ||
            poEalFioUartData->bIsReadTimeTagEn ||
            (poEalFioUartData->bIsCanEchoCancel == false));
}

/*! Returns the number of transactions with response expected */
ADI_EAL_FIOUART_CODE
static uint16_t GetNumTxnsWithResp(EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                                   ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg,
                                   uint16_t nNumFioUartTxns)
{
    uint16_t nNumTxnsWithResp = 0U;

    /* Loop through the transaction configs and get the number of transactions for which response is expected */
    for(uint16_t i = 0U; i < nNumFioUartTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalFioUartInstData, &paoFioUartTxnCfg[i]))
        {
            nNumTxnsWithResp++;
        }
    }

    return nNumTxnsWithResp;
}

/*! Checks whether there is enough space in the queues for all instances, when a response for a transaction is expected */
ADI_EAL_FIOUART_CODE
static bool CheckQueueSpaceForRspTxn(EAL_FIO_UART_DATA *poEalFioUartData,
                                     ADI_EAL_FIO_UART_TXN_DESC *paoFioUartIoTxnWithBreak,
                                     uint16_t nNumFioUartTxns)
{
    bool bNoSpace = false;

    /* Maintains the number of transactions with response (by checking readlength and e2bid) */
    uint16_t nNumTxnRespReq = 0U;

    /* Loop through the transactions to figure out how many of them require response,
       as they need to be added to EAL and e2bcore queue */
    for(uint16_t nNumTxn = 0U; nNumTxn < nNumFioUartTxns; nNumTxn++)
    {
        if((paoFioUartIoTxnWithBreak[nNumTxn].nReadLength > 0U) || paoFioUartIoTxnWithBreak[nNumTxn].bIsE2bIdPresent
                || (paoFioUartIoTxnWithBreak[nNumTxn].bCancelEcho == false))
        {
            nNumTxnRespReq++;
        }
    }

    /* Loop through each instance in the handle and check whether we have enough space in the queue */
    for(uint16_t nInstanceNum = 0U; nInstanceNum < poEalFioUartData->nNumInst; nInstanceNum++)
    {
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[nInstanceNum];
        uint16_t nNumTxnsWithRspForCurrInst = 0U;

        /* If read time tag is enabled for a particular instance then
           all transactions will give an response
           else we get responses only for the transactions with e2bid or readlength > 0 */
        if(poEalFioUartInstData->bIsReadTimeTagEn)
        {
            nNumTxnsWithRspForCurrInst = nNumFioUartTxns;
        }
        else
        {
            nNumTxnsWithRspForCurrInst = nNumTxnRespReq;
        }

        /* Check if we have space to enqueue the transactions that generate upstream messages */
        if((EAL_SLOTS_FREE_IN_Q(poEalFioUartInstData->nFioUartQWrIdx, poEalFioUartInstData->nFioUartQRdIdx,
                                poEalFioUartInstData->nFioUartCfgQDepth) <= nNumTxnsWithRspForCurrInst)
                || (adi_e2bcore_slotsLeftInFlexibleIOQueue(&poEalFioUartInstData->oId) <= nNumTxnsWithRspForCurrInst))
        {
            /* Not enough space in the queue(s) */
            bNoSpace = true;
            break;
        }
    }

    return bNoSpace;
}

/* Handle error callback */
ADI_EAL_FIOUART_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)pCbParam;

    /* Get the FIO_UART unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalFioUartInstData->bIgnoreErr)
    {
        InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/* Handle register read callback */
ADI_EAL_FIOUART_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal data of this instance */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)pCbParam;

    /* Get the FIO_UART unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* If the read is of the FIO STATUS registers */
    if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FLEXIO_NUM_BYTES_NW_ADDR) &&
            (poRegCfg->nLength == 2U))
    {
        /* Process FIO status data */
        ADI_EAL_FIO_STATUS_DATA oFioUartStatusData;
        oFioUartStatusData.nNumWrBytes = (uint16_t)((poRegCfg->pBuffer[0U] & 0xFFFF0000U) >> 16U);
        oFioUartStatusData.nNumRdBytes = (uint16_t)(poRegCfg->pBuffer[0U] & 0xFFFFU);
        oFioUartStatusData.nNumTransactions = (uint16_t)(poRegCfg->pBuffer[1U]);
        InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_INTF_STATUS_EVT, &oFioUartStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_REG_READ_RESP, poParam);
}

/* Handle FIFO read callback */
ADI_EAL_FIOUART_CODE
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal data of this instance */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)pCbParam;

    /* Get the FIO_UART unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    /* Get the UART payload */
    uint8_t *poEalFioUartPayload = (uint8_t *)poTxnCfg->pPayload;

    /* Check the RX data for errors and decode UART data */
    bool bRxCorrupt = false;
    bool bBreakfieldDetected = false;
    bool bWriteResp = false;

    do
    {
        EAL_FIO_UART_RX_INFO oRxInfo;
        (void)ADI_MEMSET(&oRxInfo, 0, sizeof(EAL_FIO_UART_RX_INFO));

        /* If transaction is coming with data */
        if(poTxnCfg->oTxnHdr.nReadLen > 0U)
        {
            /* Readlength is divided by 2 */
            poTxnCfg->oTxnHdr.nReadLen =  poTxnCfg->oTxnHdr.nReadLen >> 1U;

            /* Check for error */
            bRxCorrupt = CheckforError(poEalFioUartInstData, poEalFioUartPayload, poTxnCfg->oTxnHdr.nReadLen, &oRxInfo);

            /* Procced to decode RX data only if there were no errors found */
            if(bRxCorrupt == false)
            {
                /* Set breakfield detection if breakfield is present either before or after the payload */
                bBreakfieldDetected = oRxInfo.bIsBreakDetectedBeforePayload || oRxInfo.bIsBreakDetectedAfterPayload;

                /* Incase of breakfield found before the payload, advance the payload pointer to point
                  to the actual read bytes without breakfield */
                if(oRxInfo.bIsBreakDetectedBeforePayload)
                {
                    poEalFioUartPayload += 2U;
                }

                /* Update the readlen (there could be a chance of timeout or breakfield before payload) */
                poTxnCfg->oTxnHdr.nReadLen = oRxInfo.nRecvLength;

                /* Decode the validated data in-place */
                DecodeRxData(poEalFioUartInstData, poEalFioUartPayload, (uint16_t *)poTxnCfg->pPayload, poTxnCfg->oTxnHdr.nReadLen);

                /* Decrement the read length if there was a breakfield detected after the TX bytes before giving a callback to application */
                if(oRxInfo.bIsBreakDetectedAfterPayload)
                {
                    poTxnCfg->oTxnHdr.nReadLen--;
                }
            }
        }
        /* if upstream transaction is comming with no data it is a write response */
        else
        {
            bWriteResp = true;
        }

        /* Proceed giving callbacks to application */
        if(poEalFioUartInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
        {
            /* Increment the read index */
            poEalFioUartInstData->nFioUartQRdIdx = EAL_INC_Q_IDX(poEalFioUartInstData->nFioUartQRdIdx,
                                                   poEalFioUartInstData->nFioUartCfgQDepth);

            /******************************
                        Cases:
                            * FIO UART write read sync completed (Clear bWaitingForRx flag)
                            * Callback indicating FIO UART read/write read completed (Give callback)
                            * Callback indicating FIO UART write completed (Give callback)
            *******************************/
            if(poEalFioUartInstData->bWaitingForRx)
            {
                poEalFioUartInstData->bWaitingForRx = false;

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
                /* Signal the semaphore to indicate response received */
                prj_osal_signalSemaphore(poEalFioUartInstData->hFifoRespSemaphore);
#endif
            }

            if(bRxCorrupt)
            {
                InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT, poTxnCfg);
            }
            else if(bWriteResp == true)
            {
                /* Give a callback with write response event */
                InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_UART_WRITE_RESP, poTxnCfg);
            }
            else if(bBreakfieldDetected == true)
            {
                /* Give a callback with read response event */
                InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD, poTxnCfg);
            }
            else
            {
                /* Give a callback with read response event */
                InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP, poTxnCfg);
            }
        }
        else if(poEalFioUartInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
        {
            /* Give a callback with periodic RX response event */
            InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_PERIODIC_RX, poTxnCfg);

            if(adi_e2bcore_addFlexibleIOConfigToQueue(poID, poTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to add fio uart config to queue");
            }
        }
        else if(poEalFioUartInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
        {
            /* Increment the read index */
            poEalFioUartInstData->nFioUartQRdIdx = EAL_INC_Q_IDX(poEalFioUartInstData->nFioUartQRdIdx,
                                                   poEalFioUartInstData->nFioUartCfgQDepth);

            if(bRxCorrupt)
            {
                InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT, poTxnCfg);

            }
            else if(bWriteResp == true)
            {
                /* Give a callback with write response event */
                InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_UART_WRITE_RESP, poTxnCfg);
            }
            else
            {
                /* Give a callback with FIFO read response event */
                InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_UART_FIFO_READ, poFifoDat);
            }
        }
        else
        {
            /* Nothing to do */
        }
    } while(false);
}

/** Handle Mailbox ACK callback */
ADI_EAL_FIOUART_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal UART data */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)pCbParam;

    /* Get the UART unicast instance data */
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

    ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd     = poAckDat->bFd;
    oAckData.nTopic  = poAckDat->nTopic;

    InvokeCbk(poEalFioUartData, poEalFioUartInstData, ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/* Checks if the RX data is corrupted */
ADI_EAL_FIOUART_CODE
static bool CheckforError(EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                          uint8_t *pRxData, uint16_t nRxLength, EAL_FIO_UART_RX_INFO *poRxInfo)
{
    bool bError          = false;
    bool bTimeoutCheck   = false;
    uint16_t nRecvLength = 0U;   /* Initialize to 0 - will count actual received bytes */

    do
    {
        /* Initialize output structure */
        if(poRxInfo != NULL)
        {
            (void)ADI_MEMSET(poRxInfo, 0, sizeof(EAL_FIO_UART_RX_INFO));
        }
        else
        {
            ADI_DBG_ERROR();
            break;
        }

        /* Breakfield info */
        ADI_EAL_FIO_UART_BREAKFIELD_INFO *poBreakfieldInfo =
            &poEalFioUartInstData->paoBreakfieldInfo[poEalFioUartInstData->nFioUartQRdIdx];

        /* Check if we have breakfield present before payload and got echoed back */
        if((poBreakfieldInfo->nBreakFieldBitsBeforePayload > 0U) && (poBreakfieldInfo->bIsEchoEnabled == true))
        {
            /* If the bytes are zeros as expected - set breakfield detected
               Else, it could be either timeout or error which will be caught in the loop */
            if((pRxData[0U] == 0U) && (pRxData[1U] == 0U))
            {
                /* Remove breakfield bytes from processing */
                pRxData += 2U;
                nRxLength -= 1U;
                poRxInfo->bIsBreakDetectedBeforePayload = true;
            }
        }

        /* Configuration */
        const uint8_t nNumDataBits = poEalFioUartInstData->nNumOfDataBits;
        const uint8_t nNumStopBits = poEalFioUartInstData->nNumOfStopBits;
        const bool bIsParityEn  = poEalFioUartInstData->bIsParityEn;
        const ADI_EAL_PARITY_TYPE eParityType = poEalFioUartInstData->eParityType;

        /* Derived positions */
        const uint8_t nBitsInFirstByte  = (nNumDataBits > 7U) ? 7U : nNumDataBits;
        const uint8_t nBitsInSecondByte = (nNumDataBits > 7U) ? (uint8_t)(nNumDataBits - 7U) : 0U;
        const uint8_t nParityPos        = bIsParityEn ? nBitsInSecondByte : 0U;
        const uint8_t nStopPos          = bIsParityEn ? (uint8_t)(nParityPos + 1U) : nBitsInSecondByte;

        /* Masks */
        const uint8_t dataLowMask   = (uint8_t)((1U << nBitsInFirstByte) - 1U);
        const uint8_t dataHighMask  = (nBitsInSecondByte == 0U) ? 0U : (uint8_t)((1U << nBitsInSecondByte) - 1U);
        const uint8_t stopMask      = (nNumStopBits == 1U) ? (uint8_t)(1U << nStopPos)
                                      : (uint8_t)(3U << nStopPos); /* 2 stop bits */

        /* Process each 2-byte UART word */
        for(uint16_t i = 0U; i < nRxLength; i++)
        {
            /* Byte order: pRxData[0] = first byte, pRxData[1]   = second byte */
            const uint8_t byte1 = pRxData[0U];
            const uint8_t byte2 = pRxData[1U];

            /* Timeout detection */
            if(bTimeoutCheck)
            {
                if((byte1 != 0xFFU) || (byte2 != 0xFFU))
                {
                    /* It's not a timeout but a corrupt frame */
                    bError = true;
                    break;
                }
                else
                {
                    pRxData += 2U;
                    continue; /* Skip timeout bytes - don't increment nRecvLength */
                }
            }

            /* Check if breakfields present after the payload and got echoed back as expected */
            if((poBreakfieldInfo->nBreakFieldBitsAfterPayload > 0U) && (poBreakfieldInfo->bIsEchoEnabled == true) &&
                    (poBreakfieldInfo->nOffsetForAfterPayloadBreakFields == i))
            {
                /* If the expected bytes are zeros, then proceed
                  else it could be either timeout or error which must be caught below */
                if((byte1 == 0U) && (byte2 == 0U))
                {
                    /* Ignore this - as it not an error event - we still need to increment the recv len and
                    this would have to removed in the decode RX data*/
                    nRecvLength++;
                    poRxInfo->bIsBreakDetectedAfterPayload = true;
                    /* Advance to next UART word (2 bytes) */
                    pRxData += 2U;
                    continue;
                }
            }

            /* Check for timeout bytes */
            if((byte1 == 0xFFU) && (byte2 == 0xFFU))
            {
                bTimeoutCheck = true;
                /* Advance to next UART word (2 bytes) */
                pRxData += 2U;
                continue;
            }

            /* Start bit must be 0 in LSB of byte1 */
            const uint8_t nStartBit = byte1 & 0x01U;

            /* Extract data bits */
            const uint16_t dataLow  = (uint16_t)((uint8_t)((byte1 >> 1U) & dataLowMask));
            const uint16_t dataHigh = (nBitsInSecondByte > 0U) ? (uint16_t)((uint8_t)(byte2 & dataHighMask)) : 0U;
            const uint16_t nData    = (uint16_t)(dataLow | (dataHigh << nBitsInFirstByte));

            /* Validate parity if enabled */
            if(bIsParityEn)
            {
                const uint8_t nParityBit = (uint8_t)((byte2 >> nParityPos) & 0x01U);
                const uint8_t nComputedParity = eal_utils_computeParity16(eParityType, nData);

                /* Stop bit(s) should be 1. Padding bits can be anything */
                const uint8_t nStopBitsField = (uint8_t)(byte2 & stopMask);
                const bool bStopOk = (nNumStopBits == 1U) ?
                                     (nStopBitsField == (uint8_t)(1U << nStopPos)) :
                                     (nStopBitsField == (uint8_t)(3U << nStopPos));

                if((nComputedParity != nParityBit) || (nStartBit != 0U) || (!bStopOk))
                {
                    bError = true;
                    break;
                }
            }
            else
            {
                /* Without parity:
                - stop bits immediately follow the remaining data bits
                - padding bits (if any) can be anything */
                const uint8_t nStopBitsField = (uint8_t)(byte2 & stopMask);
                const bool bStopOk = (nNumStopBits == 1U) ?
                                     (nStopBitsField == (uint8_t)(1U << nStopPos)) :
                                     (nStopBitsField == (uint8_t)(3U << nStopPos));

                if((nStartBit != 0U) || (!bStopOk))
                {
                    bError = true;
                    break;
                }
            }

            /* Increment received length for valid data bytes */
            nRecvLength++;

            /* Advance to next UART word (2 bytes) */
            pRxData += 2U;
        }

        /* Update the receive info structure */
        poRxInfo->nRecvLength = nRecvLength;

    } while(false);

    return bError;
}

/* Decode UART RX data without validation - for use after CheckforError passes */
static void DecodeRxData(
    EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
    uint8_t *pRxData,
    uint16_t *panDecodedRxData,
    uint16_t nRxLength)
{
    /* Configuration */
    const uint8_t nNumDataBits = poEalFioUartInstData->nNumOfDataBits;
    const uint8_t nBitsInFirstByte = (nNumDataBits > 7U) ? 7U : nNumDataBits;
    const uint8_t nBitsInSecondByte = (nNumDataBits > 7U) ? (uint8_t)(nNumDataBits - 7U) : 0U;

    /* Masks */
    const uint8_t dataLowMask = (uint8_t)((1U << nBitsInFirstByte) - 1U);
    const uint8_t dataHighMask = (nBitsInSecondByte == 0U) ? 0U : (uint8_t)((1U << nBitsInSecondByte) - 1U);

    /* Count for breakfield - assumption is that there can only 2 bytes of it */
    uint8_t nBreakfieldByteCount = 0U;

    /* Process each 2-byte UART word */
    for(uint16_t i = 0U; i < nRxLength; i++)
    {
        const uint8_t byte1 = pRxData[0U];
        const uint8_t byte2 = pRxData[1U];

        /* Assumption : This data is already void of errors and also breakfield before payload must be have been removed, so incase if 0 is
           encountered we skip - as it might be the breakfield (which was appended after TX bytes) */
        if((byte1 == 0U) && (byte2 == 0U))
        {
            nBreakfieldByteCount++;
            /* Skip the two bytes from processing */
            pRxData += 2U;
            continue;
        }

        /* Extract data bits */
        const uint16_t dataLow = (uint16_t)((uint8_t)((byte1 >> 1U) & dataLowMask));
        const uint16_t dataHigh = (nBitsInSecondByte > 0U) ? (uint16_t)((uint8_t)(byte2 & dataHighMask)) : 0U;
        const uint16_t nData = (uint16_t)(dataLow | (dataHigh << nBitsInFirstByte));

        /* Write extracted data back into RX buffer as uint16_t (little-endian) */
        *panDecodedRxData = nData;

        panDecodedRxData += 1U;
        pRxData += 2U;
    }

    /* Debug check to ensure we indeed didn't exceed more than 1 breakfield byte */
    ADI_DBG_ENSURE(nBreakfieldByteCount < 2U,
                   "More than 2 bytes detected as breakfield - unexpected condition!");
}

/* Invoke the callback */
ADI_EAL_FIOUART_CODE
static void InvokeCbk(EAL_FIO_UART_DATA *poEalFioUartData, EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
                      ADI_EAL_FIO_UART_CBK_EVT eEvt, const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalFioUartInstData->pfunCbk != NULL)
    {
        (poEalFioUartInstData->pfunCbk)(poEalFioUartInstData->oId.nInstNum, poEalFioUartData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_FIOUART_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_FIO_UART_DATA *poEalFioUartData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalFioUartInstData->nFioUartQWrIdx, poEalFioUartInstData->nFioUartQRdIdx,
                               poEalFioUartInstData->nFioUartCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_FIOUART_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_FIO_UART_DATA *poEalFioUartData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInFlexibleIOQueue(&poEalFioUartInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_FIOUART_CODE
static void CopyTransactionConfigToQueue(EAL_FIO_UART_DATA *poEalFioUartData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg = &poEalFioUartInstData->paoCfgs[poEalFioUartInstData->nFioUartQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

        /* Add breakfield information as well when we are adding transaction config to the queue
          (required for proper parsing of RX transaction corresponding to the transaction) */
        ADI_EAL_FIO_UART_BREAKFIELD_INFO *poBreakFieldInfo =
            &poEalFioUartInstData->paoBreakfieldInfo[poEalFioUartInstData->nFioUartQWrIdx];

        /* When using general/custom API (not multitransaction API), breakfields configuration reflect the instance level settings */
        poBreakFieldInfo->nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
        poBreakFieldInfo->bIsEchoEnabled = !poEalFioUartInstData->bIsCanEchoCancel;

        /* Since we are not adding the breakfields after the payload here, it is safe to set other values as zero */
        poBreakFieldInfo->nBreakFieldBitsAfterPayload = 0U;
        poBreakFieldInfo->nOffsetForAfterPayloadBreakFields = 0U;
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_FIOUART_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_FIO_UART_DATA *poEalFioUartData, bool bIsMcast, uint16_t **apRxBuf)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalFioUartData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg = &poEalFioUartInstData->paoCfgs[poEalFioUartInstData->nFioUartQWrIdx];
        poInstTxnCfg->pPayload = apRxBuf != NULL ? apRxBuf[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addFlexibleIOConfigToQueue(&poEalFioUartInstData->oId, poInstTxnCfg);

            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalFioUartInstData->nFioUartQWrIdx = EAL_INC_Q_IDX(poEalFioUartInstData->nFioUartQWrIdx,
                                               poEalFioUartInstData->nFioUartCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_FIOUART_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioUartTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalFioUartData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalFioUartData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalFioUartData, *ppoFioUartTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoFioUartTxnCfg = &poEalFioUartInstData->paoCfgs[poEalFioUartInstData->nFioUartQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Sends custom FIO_UART transactions */
ADI_EAL_FIOUART_CODE
static ADI_EAL_STATUS SendCustomFioUartTransactions(EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t nNumFioUartTxns, uint16_t nNumTxnsWithResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalFioUartInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioUartInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalFioUartInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalFioUartInstData->nFioUartQWrIdx,
                                         poEalFioUartInstData->nFioUartQRdIdx,
                                         poEalFioUartInstData->nFioUartCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInFlexibleIOQueue(&poEalFioUartInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalFioUartInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bMailbox       = poEalFioUartData->bMailboxMode;
        oFifoMsgOpts.bSync          = poEalFioUartData->bResyncPending;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        poEalFioUartData->bResyncPending = false;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add FIO_UART transaction */
        eEalStatus = AddCustomFioUartTxnsToMsg(nInstNum, poEalFioUartData, poEalFioUartInstData, paoFioUartTxnCfg,
                                               nNumFioUartTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the FIO_UART EAL mode to operate in custom mode */
        poEalFioUartInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/*! Prepare the network buffers and encode the FIOUART data of num transactions */
static ADI_EAL_STATUS SendFioUartTxnsWithBreak(EAL_FIO_UART_DATA *poEalFioUartData,
        ADI_EAL_FIO_UART_TXN_DESC *paoFioUartTxnWithBreak, uint16_t nNumFioUartTxns, uint16_t *pFormattedTxData)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        bool bIsMcast = poEalFioUartData->bIsMulticast;

        /* Assumption : All unicast handles belong to the same instance */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];
        uint32_t nInstNum = poEalFioUartInstData->oId.nInstNum;

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalFioUartData->nFifoWriteVlanTag : *poEalFioUartInstData->pnFifoWriteVlanTag;

        eal_utils_fillNetworkCfg(poEalFioUartInstData->oId.nInstNum,
                                 poEalFioUartData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalFioUartInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag);

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalFioUartData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalFioUartData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalFioUartData->bIsMulticast ?
                                      poEalFioUartData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalFioUartInstData->oId.nIntfNum;

        poEalFioUartData->bResyncPending = false;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add FIO_UART transaction */
        eEalStatus = AddFioUartTxnsWithBreakToMsg(nInstNum, poEalFioUartData,
                     paoFioUartTxnWithBreak,
                     nNumFioUartTxns,
                     pFormattedTxData);

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

/** Add multiple custom FIO_UART transactions */
ADI_EAL_FIOUART_CODE
static ADI_EAL_STATUS AddCustomFioUartTxnsToMsg(uint32_t nInstNum, EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg,
        uint16_t nNumFioUartTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add FIO_UART transaction */
    for(uint16_t i = 0U; i < nNumFioUartTxns; i++)
    {
        uint16_t nWriteLength;

        /* Encode UART data */
        ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
        oUpdateUartTxnData.bCancelEcho                  = poEalFioUartInstData->bIsCanEchoCancel;
        oUpdateUartTxnData.nReadLength                  = paoFioUartTxnCfg[i].oTxnHdr.nReadLen;
        oUpdateUartTxnData.nWriteLength                 = paoFioUartTxnCfg[i].oTxnHdr.nWriteLen;
        oUpdateUartTxnData.pTxData                      = paoFioUartTxnCfg[i].pPayload;
        oUpdateUartTxnData.pFormattedTxData             = paoFioUartTxnCfg[i].pPayload;
        oUpdateUartTxnData.nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
        oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = 0U;
        oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;

        /* If echo cancellation is disabled and we have breakfields in the payload, then we have to account for breakfield echo as well */
        if((poEalFioUartInstData->bIsCanEchoCancel == false))
        {
            oUpdateUartTxnData.nReadLength +=
                (poEalFioUartInstData->nBreakFieldBits == 0U ? 0U : 1U);
        }

        UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

        /* Update read and write length internally */
        if(poEalFioUartInstData->bIsCanEchoCancel)
        {
            paoFioUartTxnCfg[i].oTxnHdr.nReadLen = oUpdateUartTxnData.nReadLength * 2U;
        }
        else
        {
            paoFioUartTxnCfg[i].oTxnHdr.nReadLen = (oUpdateUartTxnData.nReadLength * 2U) +
                                                   (paoFioUartTxnCfg[i].oTxnHdr.nWriteLen * 2U);
        }

        paoFioUartTxnCfg[i].oTxnHdr.nWriteLen = nWriteLength;

        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalFioUartInstData, &paoFioUartTxnCfg[i]))
        {
            /* Since the check has been done above, space has to be available.
               Assert this assumption */
            ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                poEalFioUartInstData->nFioUartQWrIdx,
                                poEalFioUartInstData->nFioUartQRdIdx,
                                poEalFioUartInstData->nFioUartCfgQDepth),
                            "Invalid assumption!");

            /* Copy the given FIO_UART configuration into the config queue pool */
            (void)ADI_MEMCPY(&poEalFioUartInstData->paoCfgs[poEalFioUartInstData->nFioUartQWrIdx], &paoFioUartTxnCfg[i],
                             sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

            /* Add breakfield information as well when we are adding transaction config to the queue
            (required for proper parsing of RX transaction corresponding to the transaction) */
            ADI_EAL_FIO_UART_BREAKFIELD_INFO *poBreakFieldInfo =
                &poEalFioUartInstData->paoBreakfieldInfo[poEalFioUartInstData->nFioUartQWrIdx];

            /* When using general API (not multitransaction API), the breakfields configuration reflect the instance level settings */
            poBreakFieldInfo->nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
            poBreakFieldInfo->bIsEchoEnabled = !poEalFioUartInstData->bIsCanEchoCancel;

            /* Since we are not adding the breakfields after the payload here, it is safe to set other values as zero */
            poBreakFieldInfo->nBreakFieldBitsAfterPayload = 0U;
            poBreakFieldInfo->nOffsetForAfterPayloadBreakFields = 0U;

            /* Add the FIO_UART transaction */
            if(adi_e2bcore_addFlexibleIOTxn(nInstNum,
                                            &poEalFioUartInstData->paoCfgs[poEalFioUartInstData->nFioUartQWrIdx]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }

            /* Increment queue index */
            poEalFioUartInstData->nFioUartQWrIdx = EAL_INC_Q_IDX(poEalFioUartInstData->nFioUartQWrIdx,
                                                   poEalFioUartInstData->nFioUartCfgQDepth);
        }
        else
        {
            /* Just add an FIO_UART transaction with the config provided */
            if(adi_e2bcore_addFlexibleIOTxn(nInstNum, &paoFioUartTxnCfg[i]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}

/*! Encode the FIO UART transactions and add them to queue if there responses are expected */
static ADI_EAL_STATUS AddFioUartTxnsWithBreakToMsg(uint32_t nInstNum, EAL_FIO_UART_DATA *poEalFioUartData,
        ADI_EAL_FIO_UART_TXN_DESC *paoFioUartTxnWithBreak,
        uint16_t nNumFioUartTxns, uint16_t *pFormattedTxData)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add FIO_UART transaction */
    for(uint16_t i = 0U; i < nNumFioUartTxns; i++)
    {
        uint16_t nWriteLength;

        /* If echo cancellation is disabled and we have breakfields in the payload, then we have to account for breakfield echo as well */
        if((paoFioUartTxnWithBreak[i].bCancelEcho == false))
        {
            paoFioUartTxnWithBreak[i].nReadLength +=
                ((uint16_t)(paoFioUartTxnWithBreak[i].nBreakFieldBitsBeforePayload > 0U ? 1U : 0U)) +
                ((uint16_t)(paoFioUartTxnWithBreak[i].nBreakFieldBitsAfterPayload > 0U ? 1U : 0U));
        }

        /* Copy the write length to a local variable - needed to fill breakfield information for RX parsing */
        uint16_t nUnModifiedWriteLength = paoFioUartTxnWithBreak[i].nWriteLength;

        /* Encode UART data */
        ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
        oUpdateUartTxnData.bCancelEcho                  = paoFioUartTxnWithBreak[i].bCancelEcho;
        oUpdateUartTxnData.nReadLength                  = paoFioUartTxnWithBreak[i].nReadLength;
        oUpdateUartTxnData.nWriteLength                 = paoFioUartTxnWithBreak[i].nWriteLength;
        oUpdateUartTxnData.pTxData                      = paoFioUartTxnWithBreak[i].pTxData;
        oUpdateUartTxnData.pFormattedTxData             = pFormattedTxData;
        oUpdateUartTxnData.nBreakFieldBitsBeforePayload = paoFioUartTxnWithBreak[i].nBreakFieldBitsBeforePayload;
        oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = paoFioUartTxnWithBreak[i].nBreakFieldBitsAfterPayload;
        oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;
        UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

        /* Update read and write length internally */
        if(paoFioUartTxnWithBreak[i].bCancelEcho)
        {
            paoFioUartTxnWithBreak[i].nReadLength = paoFioUartTxnWithBreak[i].nReadLength * 2U;
        }
        else
        {
            paoFioUartTxnWithBreak[i].nReadLength = (paoFioUartTxnWithBreak[i].nReadLength * 2U) +
                                                    (paoFioUartTxnWithBreak[i].nWriteLength * 2U);
        }

        paoFioUartTxnWithBreak[i].nWriteLength = nWriteLength;

        /* Fill the transaction config */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oFioUartTxnCfg;
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFioUartTxnCfg = &oFioUartTxnCfg;

        /* If its not a multicast handle, pick a config from the queue, when response is expected
           For multicast cases, we can send the local transaction config directly to e2bcore for TX encoding
           and add the transaction config from the queue to e2bcore queue after the encoding */
        if(!poEalFioUartData->bIsMulticast)
        {
            EAL_FIO_UART_INST_DATA *poUnicastInstance = poEalFioUartData->apoInstData[0U];

            if(paoFioUartTxnWithBreak[i].bIsE2bIdPresent || (paoFioUartTxnWithBreak[i].nReadLength > 0U)
                    || poUnicastInstance->bIsReadTimeTagEn)
            {
                uint16_t nWrIdx = poUnicastInstance->nFioUartQWrIdx;
                poFioUartTxnCfg = &poUnicastInstance->paoCfgs[nWrIdx];
            }
        }

        (void)ADI_MEMSET(poFioUartTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
        poFioUartTxnCfg->oTxnHdr.bAreTxnsPacked          = false;
        poFioUartTxnCfg->oTxnHdr.bEnIntfTxnHdr           = true;
        poFioUartTxnCfg->oTxnHdr.bIsE2bIdPresent         = paoFioUartTxnWithBreak[i].bIsE2bIdPresent;
        poFioUartTxnCfg->oTxnHdr.nE2bId                  = paoFioUartTxnWithBreak[i].nE2bId;
        poFioUartTxnCfg->oTxnHdr.bOverrideWriteLen       = true;
        poFioUartTxnCfg->oTxnHdr.bOverrideReadLen        = true;
        poFioUartTxnCfg->oTxnHdr.bIsRouterHeaderInserted = true;
        poFioUartTxnCfg->oTxnHdr.nGptpTime               = paoFioUartTxnWithBreak[i].nPresTime;
        poFioUartTxnCfg->oTxnHdr.bIsGptpTimePresent      = paoFioUartTxnWithBreak[i].bIsGptpPresTime;
        poFioUartTxnCfg->oTxnHdr.bIsTimeDelayPresent     = paoFioUartTxnWithBreak[i].bIsDelayPresent;
        poFioUartTxnCfg->oTxnHdr.nTimeDelay              = paoFioUartTxnWithBreak[i].nDelay;
        poFioUartTxnCfg->oTxnHdr.nReadLen                = paoFioUartTxnWithBreak[i].nReadLength;
        poFioUartTxnCfg->oTxnHdr.nWriteLen               = paoFioUartTxnWithBreak[i].nWriteLength;
        poFioUartTxnCfg->pPayload = &pFormattedTxData[0U];

        /* Add the FIO_UART transaction to the network buffer */
        if(adi_e2bcore_addFlexibleIOTxn(nInstNum,
                                        poFioUartTxnCfg) != ADI_E2BCORE_ERR_NONE)
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
        for(uint16_t k = 0U; k < poEalFioUartData->nNumInst; k++)
        {
            EAL_FIO_UART_INST_DATA *poFioUartInstance = poEalFioUartData->apoInstData[k];

            if(paoFioUartTxnWithBreak[i].bIsE2bIdPresent || poFioUartInstance->bIsReadTimeTagEn
                    || (paoFioUartTxnWithBreak[i].nReadLength > 0U))
            {
                /* Set the payload pointer to the RX pointer given in the transaction data structure */
                poFioUartInstance->paoCfgs[poFioUartInstance->nFioUartQWrIdx].pPayload = paoFioUartTxnWithBreak[i].apRxData[k];

                /* Add the breakfield information per transaction which is useful in RX parsing */
                ADI_EAL_FIO_UART_BREAKFIELD_INFO *poBreakFieldInfo =
                    &poFioUartInstance->paoBreakfieldInfo[poFioUartInstance->nFioUartQWrIdx];
                poBreakFieldInfo->nBreakFieldBitsBeforePayload = paoFioUartTxnWithBreak[i].nBreakFieldBitsBeforePayload;
                poBreakFieldInfo->nBreakFieldBitsAfterPayload  = paoFioUartTxnWithBreak[i].nBreakFieldBitsAfterPayload;
                poBreakFieldInfo->nOffsetForAfterPayloadBreakFields = nUnModifiedWriteLength;
                poBreakFieldInfo->bIsEchoEnabled               = !paoFioUartTxnWithBreak[i].bCancelEcho;

                if(poEalFioUartData->bIsMulticast)
                {
                    /* Add the queue element pointer to e2bcore */
                    /* Note : There is no copy of the transaction config structure fields used for TX encoding here,
                     as this is only required for accumulating the RX payload*/
                    if(adi_e2bcore_addFlexibleIOConfigToQueue(&poFioUartInstance->oId,
                            &poFioUartInstance->paoCfgs[poFioUartInstance->nFioUartQWrIdx]) != ADI_E2BCORE_ERR_NONE)
                    {
                        eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                        break;
                    }
                }

                /* Increment queue index for the instance */
                poFioUartInstance->nFioUartQWrIdx = EAL_INC_Q_IDX(poFioUartInstance->nFioUartQWrIdx,
                                                    poFioUartInstance->nFioUartCfgQDepth);
            }
        }

    }

    return eEalStatus;
}

/** Check and add transaction to queues */
ADI_EAL_FIOUART_CODE
static void AddTransactionToQueues(EAL_FIO_UART_DATA *poEalFioUartData,
                                   EAL_FIO_UART_INST_DATA *poEalFioUartInstData, bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioUartTxnCfg)
{
    do
    {
        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalFioUartData, *ppoFioUartTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoFioUartTxnCfg = &poEalFioUartInstData->paoCfgs[poEalFioUartInstData->nFioUartQWrIdx];
        }
    } while(false);
}

/**
 * @brief Check queue space for trnasaction
 *
 * @param [in] poEalFioUartData Interface data
 * @param [in] bIsMcast         Flag to inditace if frame is multicast
 * @return ADI_EAL_SPI_CODE
 */
ADI_EAL_FIOUART_CODE
static ADI_EAL_STATUS CheckQueueSpaceForTransaction(EAL_FIO_UART_DATA *poEalFioUartData, bool bIsMcast)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalFioUartData))
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalFioUartData))
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

    } while(false);

    return eEalStatus;
}

/**
 * @brief Add multiple  FIO_UART transactions
 *
 * @param [in] nInstNum             Instance number
 * @param [in] poEalFioUartData     Interface data
 * @param [in] paoFioUartTxnCfg     Pointer o array of transactions
 * @param [in] apRxBuf              Array to pointer of reception buffers
 * @param [in] nNumFioUartTxns      Number of transaction to add
 * @return ADI_EAL_STATUS
 */
ADI_EAL_FIOUART_CODE
static ADI_EAL_STATUS AddFioUartTxnsToMsg(uint32_t nInstNum, EAL_FIO_UART_DATA *poEalFioUartData,
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t **apRxBuf,
        uint16_t nNumFioUartTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    bool bIsMcast = poEalFioUartData->bIsMulticast;

    /* Loop through all configs and add FIO_UART transaction */
    for(uint16_t nTrans = 0U; nTrans < nNumFioUartTxns; nTrans++)
    {
        ADI_E2BCORE_TXN_CONFIG *poFioUartTxnCfg = &paoFioUartTxnCfg[nTrans];

        uint8_t nBreakFieldBits = poEalFioUartInstData->nBreakFieldBits;

        /* Encode UART data */
        if((poEalFioUartInstData->bIsCanEchoCancel == false) && (nBreakFieldBits > 0U))
        {
            poFioUartTxnCfg->oTxnHdr.nReadLen++;
        }

        /* Encode UART data */
        uint16_t nWriteLength;
        ADI_EAL_FIO_UART_UPDATE_TXN_DATA oUpdateUartTxnData;
        oUpdateUartTxnData.bCancelEcho                  = poEalFioUartInstData->bIsCanEchoCancel;
        oUpdateUartTxnData.nReadLength                  = poFioUartTxnCfg->oTxnHdr.nReadLen;
        oUpdateUartTxnData.nWriteLength                 = poFioUartTxnCfg->oTxnHdr.nWriteLen;
        oUpdateUartTxnData.pTxData                      = poFioUartTxnCfg->pPayload;
        oUpdateUartTxnData.pFormattedTxData             = poFioUartTxnCfg->pPayload;
        oUpdateUartTxnData.nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
        oUpdateUartTxnData.nBreakFieldBitsAfterPayload  = 0U;
        oUpdateUartTxnData.pnWriteLengthOutput          = &nWriteLength;

        /* Encode UART data */
        UpdateUartTxData(poEalFioUartData, &oUpdateUartTxnData);

        /* Update read and write length internally */
        if(poEalFioUartInstData->bIsCanEchoCancel)
        {
            poFioUartTxnCfg->oTxnHdr.nReadLen = poFioUartTxnCfg->oTxnHdr.nReadLen * 2U;
        }
        else
        {
            poFioUartTxnCfg->oTxnHdr.nReadLen = (poFioUartTxnCfg->oTxnHdr.nReadLen * 2U) +
                                                (poFioUartTxnCfg->oTxnHdr.nWriteLen * 2U);
        }

        poFioUartTxnCfg->oTxnHdr.nWriteLen = nWriteLength;

        /* Add the FIO_UART transaction */
        if(adi_e2bcore_addFlexibleIOTxn(nInstNum, poFioUartTxnCfg) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            break;
        }

        for(uint8_t nInst = 0u; nInst < poEalFioUartData->nNumInst; nInst++)
        {
            EAL_FIO_UART_INST_DATA *poCurrentInstData = poEalFioUartData->apoInstData[nInst];

            /* If response is expected, the config should be persistent
               till the response is received, so it needs to be taken
               from the config queue pool */
            if(IsResponseExpected(poCurrentInstData, poFioUartTxnCfg))
            {
                /* Since the check has been done above, space has to be available.
                   Assert this assumption */
                ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                    poCurrentInstData->nFioUartQWrIdx,
                                    poCurrentInstData->nFioUartQRdIdx,
                                    poCurrentInstData->nFioUartCfgQDepth),
                                "Invalid assumption!");

                /* Copy the given FIO_UART configuration into the config queue pool */
                (void)ADI_MEMCPY(&poCurrentInstData->paoCfgs[poCurrentInstData->nFioUartQWrIdx], poFioUartTxnCfg,
                                 sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

                /* Save the RX pointer */
                ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg = &poCurrentInstData->paoCfgs[poCurrentInstData->nFioUartQWrIdx];
                poInstTxnCfg->pPayload = apRxBuf != NULL ? apRxBuf[nInst] : NULL;

                /* Add the configuration to e2bcore queue */
                if(bIsMcast)
                {
                    (void)adi_e2bcore_addFlexibleIOConfigToQueue(&poCurrentInstData->oId, poInstTxnCfg);
                }

                /* Increment queue index */
                poCurrentInstData->nFioUartQWrIdx = EAL_INC_Q_IDX(poCurrentInstData->nFioUartQWrIdx,
                                                    poCurrentInstData->nFioUartCfgQDepth);
            }

        }
    }

    return eEalStatus;
}

#endif /* ADI_E2B_IFACE_FIO_UART_ENABLED */
/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
