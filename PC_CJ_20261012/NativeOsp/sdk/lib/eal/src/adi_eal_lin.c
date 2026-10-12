/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_lin.c
 * @brief: The EAL APIs for LIN interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_LIN_ENABLED

/*! \cond private */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalLinData) \
ADI_DBG_REQUIRE((poEalLinData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalLinData)->bInUse == true, "Instance is not opened");

#define GET_FIELD_VAL(val, mask, pos) (((val) & (mask)) >> (pos))

/**
 * @brief Validates RX buffer pointers for LIN transactions
 *
 * For unicast handles, only apRxData[0] needs to be valid.
 * For multicast handles, all apRxData[0..nNumInst-1] pointers must be valid.
 *
 * @param [in] poEalLinData  Pointer to EAL LIN data structure
 * @param [in] apRxData      Array of RX buffer pointers to validate
 * @return true if all required RX buffers are valid, false otherwise
 */
static inline bool ValidateRxBuffers(const EAL_LIN_DATA *poEalLinData, uint8_t **apRxData)
{
    bool bIsValid = (apRxData != NULL);

    if(bIsValid)
    {
        if(poEalLinData->bIsMulticast)
        {
            /* For multicast, validate all instance RX buffers */
            for(uint16_t i = 0U; (i < poEalLinData->nNumInst) && bIsValid; i++)
            {
                bIsValid = (apRxData[i] != NULL);
            }
        }
        else
        {
            /* For unicast, only validate the first RX buffer */
            bIsValid = (apRxData[0] != NULL);
        }
    }

    return bIsValid;
}

/*=================================== DATA ==================================*/

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendLinTransaction(
    EAL_LIN_DATA *poEalLinData,
    ADI_E2BCORE_LIN_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod);
static void extractLinStatus(uint32_t *anData, ADI_EAL_LIN_STATUS_DATA *poStatusData);
static void LockLinMutex(EAL_LIN_DATA *poEalLinData);
static void UnlockLinMutex(EAL_LIN_DATA *poEalLinData);
static void SetWaitingForRx(EAL_LIN_DATA *poEalLinData, bool bWaitingForRx);
static bool WaitForResponses(EAL_LIN_DATA *poEalLinData);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void InvokeCbk(EAL_LIN_DATA *poEalLinData, EAL_LIN_INST_DATA *poEalLinInstData, ADI_EAL_LIN_CBK_EVT eEvt,
                      const void *pArg);
static bool IsSpaceAvblInEalTxnQueue(EAL_LIN_DATA *poEalLinData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_LIN_DATA *poEalLinData);
static void CopyTransactionConfigToQueue(EAL_LIN_DATA *poEalLinData, ADI_E2BCORE_LIN_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_LIN_DATA *poEalLinData, bool bIsMcast, uint8_t **apRxBuf);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_LIN_DATA *poEalLinData, EAL_LIN_INST_DATA *poEalLinInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoLinTxnCfg);
static ADI_EAL_STATUS SendCustomLinTransactions(EAL_LIN_DATA *poEalLinData, EAL_LIN_INST_DATA *poEalLinInstData,
        ADI_E2BCORE_LIN_TXN_CONFIG *paoLinTxnCfg, uint16_t nNumLinTxns);
static ADI_EAL_STATUS AddCustomLinTxnsToMsg(uint32_t nInstNum, EAL_LIN_DATA *poEalLinData,
        EAL_LIN_INST_DATA *poEalLinInstData, ADI_E2BCORE_LIN_TXN_CONFIG *paoLinTxnCfg, uint16_t nNumLinTxns);

/*! \endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an LIN instance
 *
 * @param [in]  poLinCfg          Configuration pointer for EAL LIN instance
 * @param [out] phLinHandle       Returns the LIN handle for use in other EAL LIN APIs
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: LIN instance was successfully opened.
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linOpen(ADI_EAL_LIN_CFG *poLinCfg, ADI_EAL_LIN_HANDLE *phLinHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poLinCfg != NULL, "Pointer to the LIN module configuration is NULL");
    ADI_DBG_REQUIRE(phLinHandle != NULL, "Pointer to the LIN handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poLinCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poLinCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poLinCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poLinCfg->nInstNum;
    uint32_t nRemoteNum = poLinCfg->nRemoteNum;
    uint32_t nIntfNum = poLinCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_LIN,
                    "The given pair of instance, remote and interface numbers is not configured as LIN interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poLinCfg->pEalLinMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poLinCfg->nEalLinMemorySize >= ADI_EAL_LIN_UNICAST_INST_MEM_SIZE(poLinCfg->nLinCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poLinCfg->pEalLinMemory;

    /* Allocate memory for the LIN transaction config queue. */
    ADI_E2BCORE_LIN_TXN_CONFIG *paoLinTxnCfgs = (ADI_E2BCORE_LIN_TXN_CONFIG *)((void *)pMem);
    pMem += (poLinCfg->nLinCfgQDepth * sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));

    /* Allocate memory for the EAL LIN instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = (EAL_LIN_INST_DATA *)((void *)pMem);
    pMem += sizeof(EAL_LIN_INST_DATA);
    /* ~~~~~~~~~~~ LIN instance info ~~~~~~~~~~~ */
    poEalLinInstData->oId.nInstNum      = nInstNum;
    poEalLinInstData->oId.nRemoteNum    = nRemoteNum;
    poEalLinInstData->oId.nIntfNum      = nIntfNum;
    poEalLinInstData->pnIfRegVlanTag    = &poIfConfig->nIfRegVlanTag;
    poEalLinInstData->pnFifoRegVlanTag  = &poIfConfig->nFifoRegVlanTag;
    poEalLinInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;
    /* ~~~~~~~~~~~ LIN state info ~~~~~~~~~~~ */
    poEalLinInstData->bIgnoreErr        = false;
    poEalLinInstData->eMode             = ADI_EAL_USAGE_MODE_NORMAL;
    poEalLinInstData->pfunCbk           = NULL; /* Will be filled in adi_eal_linRegisterCallback */
    poEalLinInstData->bWaitingForRx     = false;
    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    uint32_t nLinCtrlReg = 0U;
    EAL_LIN_INST_DATA **apoInstData;
    EAL_LIN_DATA *poEalLinData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;

    if(eal_utils_getConfigInfRegVal(&poEalLinInstData->oId, LIN_CONTROL_NW_ADDR, &nLinCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }

    poEalLinInstData->bIsReadTimeTagEn  = (nLinCtrlReg & BITM_LIN_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalLinInstData->nLinCfgQDepth     = poLinCfg->nLinCfgQDepth;
    poEalLinInstData->nLinQWrIdx        = 0U;
    poEalLinInstData->nLinQRdIdx        = 0U;
    poEalLinInstData->paoCfgs           = paoLinTxnCfgs;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalLinInstData->oId, (void *)pMem,
                    poLinCfg->nLinCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }

    pMem += (sizeof(ADI_E2BCORE_LIN_TXN_CONFIG *) * poLinCfg->nLinCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_LIN_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_LIN_INST_DATA *);
        apoInstData[0U] = poEalLinInstData;

        /* Allocate memory for the internal EAL LIN data structure */
        poEalLinData  = (EAL_LIN_DATA *)((void *)pMem);
        pMem += sizeof(EAL_LIN_DATA);
        poEalLinData->bIsMulticast  = false;
        poEalLinData->bInUse        = true;
        poEalLinData->nIntfTopic    = 0U;
        poEalLinData->nFifoTopic    = 0U;
        poEalLinData->nNumInst      = 1U;
        poEalLinData->apoInstData   = apoInstData;
        poEalLinData->bMailboxMode  = false;
        poEalLinData->bResyncPending = false;

        /* Initialise the mutex for this LIN instance */
        poEalLinInstData->oLinMutexCfg.pMem = &poEalLinInstData->anLinMutexBuffer[0U];
        poEalLinInstData->oLinMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalLinInstData->oLinMutexCfg, &poEalLinInstData->hLinMutex);

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        /* Initialise the semaphore for this LIN instance */
        poEalLinInstData->oFifoRespSemaphoreCfg.pMem = &poEalLinInstData->anLinFifoRespSemaphoreBuffer[0U];
        poEalLinInstData->oFifoRespSemaphoreCfg.nMemSize = PRJ_OSAL_SEMAPHORE_MEM_SIZE;
        poEalLinInstData->oFifoRespSemaphoreCfg.bCountingSemaphore = false;
        poEalLinInstData->oFifoRespSemaphoreCfg.nMaxCount = 1U;
        poEalLinInstData->oFifoRespSemaphoreCfg.nInitialCount = 0U;
        (void)prj_osal_createSemaphore(&poEalLinInstData->oFifoRespSemaphoreCfg, &poEalLinInstData->hFifoRespSemaphore);
#endif
        /* Set the callback param in the e2bcore interface config */
        poIfConfig->pCbParam = poEalLinData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_LIN;

        /* Set the handle to the EAL_LIN_DATA structure */
        *phLinHandle = (ADI_EAL_LIN_HANDLE)poEalLinData;
    }

    return eStatus;
}

/**
 * @brief       Closes an opened LIN instance
 * @param [in]  hLinHandle       The LIN handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: When the LIN device was successfully closed.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linClose(ADI_EAL_LIN_HANDLE hLinHandle)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalLinData->apoInstData[i]->hLinMutex);
#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        (void)prj_osal_destroySemaphore(poEalLinData->apoInstData[i]->hFifoRespSemaphore);
#endif
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalLinData, 0, sizeof(EAL_LIN_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to register callback
 * @param [in]  hLinHandle        The LIN handle
 * @param [in]  pfLinCallback     Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS:   Returns ADI_EAL_STATUS_SUCCESS if callback was registered.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linRegisterCallback(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_CALLBACK_FUNC pfLinCallback)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(pfLinCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalLinData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* Register the callback function */
    poEalLinInstData->pfunCbk = pfLinCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Creates a multicast group of LIN interfaces
 *
 * @param [in]  poLinMcastCfg       Configuration for LIN multicast group
 * @param [out] phLinMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linCreateMcastGroup(ADI_EAL_LIN_MCAST_CFG *poLinMcastCfg,
        ADI_EAL_LIN_HANDLE *phLinMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poLinMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phLinMcastHandle != NULL, "Pointer to the LIN multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poLinMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poLinMcastCfg->pahLinHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poLinMcastCfg->pLinMcastMem != NULL, "Pointer to memory for LIN multicast is NULL");
    ADI_DBG_REQUIRE(poLinMcastCfg->nLinMcastMemSize >= ADI_EAL_LIN_MCAST_GROUP_MEM_SIZE(poLinMcastCfg->nNumInst),
                    "Not enough memory allocated for LIN multicast group");
    ADI_DBG_REQUIRE(poLinMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poLinMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poLinMcastCfg->pLinMcastMem;
    EAL_LIN_INST_DATA **apoInstData = (EAL_LIN_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_LIN_INST_DATA *) * poLinMcastCfg->nNumInst);

    /* Save the unicast EAL LIN instances */
    for(uint16_t i = 0U; i < poLinMcastCfg->nNumInst; i++)
    {
        EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)poLinMcastCfg->pahLinHandles[i];
        apoInstData[i] = poEalLinData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast FIFO topic */
        uint32_t nInstNum = poEalLinData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalLinData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalLinData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poLinMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poLinMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poLinMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poLinMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_LIN_DATA *poEalLinMcastData = (EAL_LIN_DATA *)((void *)pMem);
    poEalLinMcastData->bIsMulticast = true;
    poEalLinMcastData->bInUse       = true;
    poEalLinMcastData->nIntfTopic   = poLinMcastCfg->nIntfTopic;
    poEalLinMcastData->nFifoTopic   = poLinMcastCfg->nFifoTopic;
    poEalLinMcastData->nIfRegVlanTag       = (poLinMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
            poLinMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalLinMcastData->nFifoRegVlanTag     = (poLinMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
            poLinMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalLinMcastData->nFifoWriteVlanTag   = (poLinMcastCfg->poFifoWriteVlanTag != NULL) ? eal_utils_convertVlanTags(
            poLinMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalLinMcastData->nNumInst     = poLinMcastCfg->nNumInst;
    poEalLinMcastData->apoInstData  = apoInstData;
    poEalLinMcastData->bMailboxMode  = false;
    poEalLinMcastData->bResyncPending = false;

    /* Set the handle to the EAL_LIN_DATA structure */
    *phLinMcastHandle = (ADI_EAL_LIN_HANDLE)poEalLinMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Allows the application to send LIN write/read frame asynchronously
 *
 * @note  The LIN interface generates an upstream message for each downstream message processed.
 *        Regardless of whether it is a write or read operation, the upstream response contains
 *        the PID, data, and checksum values sampled from the LIN physical bus.
 *        Therefore, the RX buffer (apRxData) must always be provided and must be large enough
 *        to hold the response data.
 *
 * @param [in] hLinHandle        The LIN handle
 * @param [in] poLinData         Pointer to LIN I/O DATA containing:
 *                               - bIsWriteTxn: true for write, false for read
 *                               - nLength: Number of data bytes (1-8)
 *                               - pData: Pointer to TX data buffer
 * @param [in] apRxData          Array of pointers to LIN RX data buffers. Must not be NULL.
 *                               Each buffer must be at least poLinData->nLength bytes.
 *                               The upstream response (PID, data, checksum) will be stored here.
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 *
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           : LIN write frame was sent successfully.
 *              - #ADI_EAL_STATUS_NULL_PTR           : NULL pointer passed for RX buffer
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   : Failed to add the register map message
 * @note       <b>E2B_ID Usage Pattern (Async API)</b>:
 *             - E2B_ID is <b>optional</b> for asynchronous LIN write-read operations
 *             - Set nId = 0xFF to disable E2B_ID
 *             - Set nId to any value (0x00-0xFE) to enable E2B_ID tracking
 *             - When enabled, E2B_ID is copied to the upstream response message,
 *               allowing correlation between this downstream command and device response
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linWriteRead(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData, uint8_t **apRxData,
                                    uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(poLinData != NULL, "LIN IO data pointer is NULL");

    /* More LIN IO data checks */
    ADI_DBG_REQUIRE(poLinData->pData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poLinData->nLength <= 8U && poLinData->nLength != 0U,
                    "Write/read length is greater than 8 or equals to zero");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* RX buffer validation - LIN always generates upstream response
     * For unicast: apRxData[0] must be valid
     * For multicast: apRxData[0..nNumInst-1] must all be valid */
    if(!ValidateRxBuffers(poEalLinData, apRxData))
    {
        eEalStatus = ADI_EAL_STATUS_NULL_PTR;
    }
    else
    {
        /* Set the LIN transaction configuration */
        ADI_E2BCORE_LIN_TXN_CONFIG oTxnCfg;
        (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));
        oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
        oTxnCfg.oTxnHdr.nE2bId            = nId;
        oTxnCfg.pPayload                  = poLinData;

        /* Loop through the mutex of all instances and lock it */
        LockLinMutex(poEalLinData);

        /* Send the LIN transaction */
        eEalStatus = SendLinTransaction(poEalLinData, &oTxnCfg, apRxData, false, 0ULL);

        /* Loop through the mutex of all instances and unlock it */
        UnlockLinMutex(poEalLinData);
    }

    return eEalStatus;
}

/**
 * @brief Allows the application to send LIN write/read frame synchronously
 *
 * This call blocks until the LIN write/read frame has been sent and the response is received.
 *
 * @note  The LIN interface generates an upstream message for each downstream message processed.
 *        Regardless of whether it is a write or read operation, the upstream response contains
 *        the PID, data, and checksum values sampled from the LIN physical bus.
 *        Therefore, the RX buffer (apRxData) must always be provided and must be large enough
 *        to hold the response data.
 *
 * @param [in] hLinHandle        The LIN handle
 * @param [in] poLinData         Pointer to LIN I/O DATA containing:
 *                               - bIsWriteTxn: true for write, false for read
 *                               - nLength: Number of data bytes (1-8)
 *                               - pData: Pointer to TX data buffer
 * @param [in] apRxData          Array of pointers to LIN RX data buffers. Must not be NULL.
 *                               Each buffer must be at least poLinData->nLength bytes.
 *                               The upstream response (PID, data, checksum) will be stored here.
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           : LIN write frame was sent successfully.
 *              - #ADI_EAL_STATUS_NULL_PTR           : NULL pointer passed for RX buffer
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   : Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT        : Timed out waiting for response
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
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linWriteReadSync(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
                                        uint8_t **apRxData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(poLinData != NULL, "LIN IO data pointer is NULL");

    /* More LIN IO data checks */
    ADI_DBG_REQUIRE(poLinData->pData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poLinData->nLength <= 8U && poLinData->nLength != 0U,
                    "Write/read length is greater than 8 or equals to zero");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* RX buffer validation - LIN always generates upstream response
     * For unicast: apRxData[0] must be valid
     * For multicast: apRxData[0..nNumInst-1] must all be valid */
    if(!ValidateRxBuffers(poEalLinData, apRxData))
    {
        eEalStatus = ADI_EAL_STATUS_NULL_PTR;
    }
    else
    {
        /* Set the LIN transaction configuration */
        ADI_E2BCORE_LIN_TXN_CONFIG oTxnCfg;
        (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));
        oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
        oTxnCfg.oTxnHdr.nE2bId            = nId;
        oTxnCfg.pPayload                  = poLinData;

        /* Loop through the mutex of all instances and lock it */
        LockLinMutex(poEalLinData);

        /* Set the waiting for RX flag to true */
        SetWaitingForRx(poEalLinData, true);

        /* Send the LIN transaction */
        eEalStatus = SendLinTransaction(poEalLinData, &oTxnCfg, apRxData, false, 0ULL);

        if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            /* Wait for LIN read response */
            bool bTimedOut = WaitForResponses(poEalLinData);
            eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
        }

        /* Set the waiting for RX flag to false */
        SetWaitingForRx(poEalLinData, false);

        /* Loop through the mutex of all instances and unlock it */
        UnlockLinMutex(poEalLinData);
    }

    return eEalStatus;
}

/**
 * @brief Performs LIN write/read after a specified delay
 *
 * @note  The LIN interface generates an upstream message for each downstream message processed.
 *        Regardless of whether it is a write or read operation, the upstream response contains
 *        the PID, data, and checksum values sampled from the LIN physical bus.
 *        Therefore, the RX buffer (apRxData) must always be provided and must be large enough
 *        to hold the response data.
 *
 * @param [in] hLinHandle        The LIN handle
 * @param [in] poLinData         Pointer to LIN I/O DATA containing:
 *                               - bIsWriteTxn: true for write, false for read
 *                               - nLength: Number of data bytes (1-8)
 *                               - pData: Pointer to TX data buffer
 * @param [in] apRxData          Array of pointers to LIN RX data buffers. Must not be NULL.
 *                               Each buffer must be at least poLinData->nLength bytes.
 *                               The upstream response (PID, data, checksum) will be stored here.
 * @param [in] nDelay            The delay time in nanoseconds before executing the transaction
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           : LIN write frame was sent successfully.
 *              - #ADI_EAL_STATUS_NULL_PTR           : NULL pointer passed for RX buffer
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linWriteReadWithDelay(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(poLinData != NULL, "LIN IO data pointer is NULL");

    /* More LIN IO data checks */
    ADI_DBG_REQUIRE(poLinData->pData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poLinData->nLength <= 8U && poLinData->nLength != 0U,
                    "Write/read length is greater than 8 or equals to zero");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* RX buffer validation - LIN always generates upstream response
     * For unicast: apRxData[0] must be valid
     * For multicast: apRxData[0..nNumInst-1] must all be valid */
    if(!ValidateRxBuffers(poEalLinData, apRxData))
    {
        eEalStatus = ADI_EAL_STATUS_NULL_PTR;
    }
    else
    {
        /* Set the LIN transaction configuration */
        ADI_E2BCORE_LIN_TXN_CONFIG oTxnCfg;
        (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));
        oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
        oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
        oTxnCfg.oTxnHdr.nTimeDelay          = nDelay;
        oTxnCfg.oTxnHdr.nE2bId              = nId;
        oTxnCfg.pPayload                    = poLinData;

        /* Loop through the mutex of all instances and lock it */
        LockLinMutex(poEalLinData);

        /* Send the LIN transaction */
        eEalStatus = SendLinTransaction(poEalLinData, &oTxnCfg, apRxData, false, 0ULL);

        /* Loop through the mutex of all instances and unlock it */
        UnlockLinMutex(poEalLinData);
    }

    return eEalStatus;
}

/**
 * @brief Performs LIN write/read at a specified presentation timestamp
 *
 * @note  The LIN interface generates an upstream message for each downstream message processed.
 *        Regardless of whether it is a write or read operation, the upstream response contains
 *        the PID, data, and checksum values sampled from the LIN physical bus.
 *        Therefore, the RX buffer (apRxData) must always be provided and must be large enough
 *        to hold the response data.
 *
 * @param [in] hLinHandle        The LIN handle
 * @param [in] poLinData         Pointer to LIN I/O DATA containing:
 *                               - bIsWriteTxn: true for write, false for read
 *                               - nLength: Number of data bytes (1-8)
 *                               - pData: Pointer to TX data buffer
 * @param [in] apRxData          Array of pointers to LIN RX data buffers. Must not be NULL.
 *                               Each buffer must be at least poLinData->nLength bytes.
 *                               The upstream response (PID, data, checksum) will be stored here.
 * @param [in] nTimestamp        The gPTP presentation time ({uint32_t}sec, {uint32_t}ns)
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS               : LIN write frame was sent successfully.
 *              - #ADI_EAL_STATUS_NULL_PTR               : NULL pointer passed for RX buffer
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linWriteReadWithPresTs(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(poLinData != NULL, "LIN IO data pointer is NULL");

    /* More LIN IO data checks */
    ADI_DBG_REQUIRE(poLinData->pData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poLinData->nLength <= 8U && poLinData->nLength != 0U,
                    "Write/read length is greater than 8 or equals to zero");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* RX buffer validation - LIN always generates upstream response
     * For unicast: apRxData[0] must be valid
     * For multicast: apRxData[0..nNumInst-1] must all be valid */
    if(!ValidateRxBuffers(poEalLinData, apRxData))
    {
        eEalStatus = ADI_EAL_STATUS_NULL_PTR;
    }
    else
    {
        /* Set the LIN transaction configuration */
        ADI_E2BCORE_LIN_TXN_CONFIG oTxnCfg;
        (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));
        oTxnCfg.oTxnHdr.bIsGptpTimePresent  = true;
        oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
        oTxnCfg.oTxnHdr.nGptpTime           = nTimestamp;
        oTxnCfg.oTxnHdr.nE2bId              = nId;
        oTxnCfg.pPayload                    = poLinData;

        /* Loop through the mutex of all instances and lock it */
        LockLinMutex(poEalLinData);

        /* Send the LIN transaction */
        eEalStatus = SendLinTransaction(poEalLinData, &oTxnCfg, apRxData, false, 0ULL);

        /* Loop through the mutex of all instances and unlock it */
        UnlockLinMutex(poEalLinData);
    }

    return eEalStatus;
}

/**
 * @brief Allows the application to setup LIN periodic write
 *
 * @note  The LIN interface generates an upstream message for each downstream message processed.
 *        The upstream response contains the PID, data, and checksum values sampled from the
 *        LIN physical bus. Therefore, the RX buffer (apRxData) must always be provided and
 *        must be large enough to hold the response data.
 *
 * @param [in] hLinHandle        The LIN handle
 * @param [in] poLinData         Pointer to LIN I/O DATA containing:
 *                               - nLength: Number of data bytes (1-8)
 *                               - pData: Pointer to TX data buffer
 * @param [in] apRxData          Array of pointers to LIN RX data buffers. Must not be NULL.
 *                               Each buffer must be at least poLinData->nLength bytes.
 *                               The upstream response (PID, data, checksum) will be stored here.
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS               : LIN periodic write was setup successfully.
 *              - #ADI_EAL_STATUS_NULL_PTR               : NULL pointer passed for RX buffer
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linSetupPeriodicWrite(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(poLinData != NULL, "LIN IO data pointer is NULL");

    /* More LIN IO data checks */
    ADI_DBG_REQUIRE(poLinData->pData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poLinData->nLength <= 8U && poLinData->nLength != 0U,
                    "Write/read length is greater than 8 or equals to zero");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* RX buffer validation - LIN always generates upstream response
     * For unicast: apRxData[0] must be valid
     * For multicast: apRxData[0..nNumInst-1] must all be valid */
    if(!ValidateRxBuffers(poEalLinData, apRxData))
    {
        eEalStatus = ADI_EAL_STATUS_NULL_PTR;
    }
    else
    {
        /* Set the LIN transaction configuration */
        ADI_E2BCORE_LIN_TXN_CONFIG oTxnCfg;
        (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));
        oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
        oTxnCfg.oTxnHdr.nE2bId            = nId;
        oTxnCfg.pPayload                  = poLinData;

        /* Loop through the mutex of all instances and lock it */
        LockLinMutex(poEalLinData);

        /* Is it a multicast request? */
        bool bIsMcast = poEalLinData->bIsMulticast;

        /* Use the locally created LIN transaction by default.
        NOTE: It will be overwritten if the configuration
                should come from the EAL transaction queue */
        ADI_E2BCORE_TXN_CONFIG *poLinTxnCfg = &oTxnCfg;

        do
        {
            /* Get the unicast instance data */
            EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

            /* Add the transaction to the EAL and e2bcore transaction queues */
            eEalStatus = CheckAndAddTransactionToQueues(poEalLinData, poEalLinInstData, bIsMcast, &poLinTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }

            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalLinData, bIsMcast, apRxData) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }

        } while(false);

        if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            /* Switch the mode of all the instances to periodic */
            for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
            {
                EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

                if(adi_e2bcore_addLinConfigToQueue(&(poEalLinInstData->oId), poLinTxnCfg) != ADI_E2BCORE_ERR_NONE)
                {
                    eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                    break;
                }

                poEalLinInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
            }
        }

        /* Loop through the mutex of all instances and unlock it */
        UnlockLinMutex(poEalLinData);
    }

    return eEalStatus;
}

/**
 * @brief       Gets the LIN interface status
 *
 * @param [in]  hLinHandle     The LIN interface Handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linReadStatus(ADI_EAL_LIN_HANDLE hLinHandle)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);

    bool bIsMcast = poEalLinData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalLinData->nIfRegVlanTag : *poEalLinInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalLinData->nIntfTopic;
        anTopics[1U] = poEalLinData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalLinInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalLinInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalLinInstData->oId, LIN_STATUS_NW_ADDR, 1U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief To write/read to/from interface fifo based on the transaction config
 *
 * @param [in]  hLinHandle     The LIN interface Handle
 * @param [in]  paoLinTxnCfg   LIN transaction config
 * @param [in]  nNumLinTxns    Number of LIN transactions
 * @return      Status
 *              - #ADI_EAL_STATUS                       : LIN write frame was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linCustomWriteRead(ADI_EAL_LIN_HANDLE hLinHandle,
        ADI_E2BCORE_LIN_TXN_CONFIG *paoLinTxnCfg, uint16_t nNumLinTxns)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(paoLinTxnCfg != NULL, "LIN transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumLinTxns > 0U, "Number of LIN transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalLinData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* Lock the mutex */
    LockLinMutex(poEalLinData);

    /* Send the custom LIN transactions */
    eEalStatus = SendCustomLinTransactions(poEalLinData, poEalLinInstData, paoLinTxnCfg, nNumLinTxns);

    /* Unlock the mutex */
    UnlockLinMutex(poEalLinData);

    return eEalStatus;
}

/**
 * @brief Send a register map transaction to the LIN interface or the corresponding FIFO registers
 *
 * @param [in]    hLinHandle             The LIN interface Handle
 * @param [in]    bIsFifoRegTransaction  Is this for reg transaction for FIFO
 * @param [inout] poRegCfg               Register read details
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linSendRegTransaction(ADI_EAL_LIN_HANDLE hLinHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalLinInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalLinInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalLinInstData->oId.nIntfNum;

    bool bIsMcast = poEalLinData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bMailbox       = poEalLinData->bMailboxMode;
    oMsgOpts.bSync          = poEalLinData->bResyncPending;
    oMsgOpts.bTopicOverRide = true;

    poEalLinData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalLinData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalLinData->nFifoRegVlanTag : *poEalLinInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalLinData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalLinData->nIfRegVlanTag : *poEalLinInstData->pnIfRegVlanTag;
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
 * @brief       Resets the Lin transaction queue and goes back to a clean state
 *
 * @param  [in] hLinHandle      The Lin handle
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : Lin reset successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linResetQueue(ADI_EAL_LIN_HANDLE hLinHandle)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalLinData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* Lock the mutex */
    LockLinMutex(poEalLinData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearLinConfigQueue(&poEalLinInstData->oId);
        poEalLinInstData->nLinQWrIdx = 0U;
        poEalLinInstData->nLinQRdIdx = 0U;
        poEalLinInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
        poEalLinInstData->bWaitingForRx = false;

        /* Ignore errors */
        poEalLinInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  LIN_CONTROL_NW_ADDR;
        uint32_t nMask      =  BITM_LIN_CONTROL_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      =  BITM_LIN_CONTROL_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_linSendRegTransaction((ADI_EAL_LIN_HANDLE)poEalLinData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an LIN NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalLinInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);

        /* Issue FIFO resets */
        nStartAddr = LIN_CONTROL_NW_ADDR;
        nMask      = BITM_LIN_CONTROL_TX_FIFO_RESET | BITM_LIN_CONTROL_RX_FIFO_RESET;
        nData      = BITM_LIN_CONTROL_TX_FIFO_RESET | BITM_LIN_CONTROL_RX_FIFO_RESET;
        eEalStatus = adi_eal_linSendRegTransaction((ADI_EAL_LIN_HANDLE)poEalLinData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalLinInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockLinMutex(poEalLinData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for LIN instance
 *
 * @param [in] hLinHandle         LIN instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_LIN_CODE
void adi_eal_linSetDownstreamVlan(ADI_EAL_LIN_HANDLE hLinHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                  ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);

    /* Extract the unicast instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* If it is a multicast handle, update the handle structure */
    if(poEalLinData->bIsMulticast)
    {
        poEalLinData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalLinData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalLinData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalLinInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalLinInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalLinInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for LIN FIFO messages
 *
 * @param [in] hLinHandle  LIN interface Handle
 * @param [in] poVlanTag   Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn   If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linSetUpstreamVlan(ADI_EAL_LIN_HANDLE hLinHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);

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

    return adi_eal_linSendRegTransaction(poEalLinData, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for LIN transactions
 *
 * @param [in]  hLinHandle      The LIN handle
 * @param [in]  bEnable         Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linSetMailboxMode(ADI_EAL_LIN_HANDLE hLinHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);

    /* Set the mailbox mode flag */
    poEalLinData->bMailboxMode = bEnable;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for LIN transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hLinHandle      The LIN handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_LIN_CODE
ADI_EAL_STATUS adi_eal_linResyncSeqNum(ADI_EAL_LIN_HANDLE hLinHandle)
{
    /* Get the internal data from the handle */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)hLinHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalLinData);

    /* Set the sync mode flag */
    poEalLinData->bResyncPending = true;

    return ADI_EAL_STATUS_SUCCESS;
}

/*============================ CALLBACKS ============================*/
/**
 * @brief The callback for LIN interface
 * @param [in] poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param [in] eType         The type of event this callback is for
 * @param [in] poParam       The data corresponding to this event
 * @param [in] bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param [in] pCbParam      Parameter to be passed back while invoking callback
 */
ADI_EAL_LIN_CODE
void LinInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                     void *pCbParam)
{
    /* If pCbParam is NULL, the EAL LIN interface instance is not opened,
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

/*! \cond PRIVATE */
/*================================ STATIC CODE ==============================*/

/*! Sends the LIN transaction */
ADI_EAL_LIN_CODE
static ADI_EAL_STATUS SendLinTransaction(
    EAL_LIN_DATA *poEalLinData,
    ADI_E2BCORE_LIN_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalLinData->bIsMulticast;

    /* Use the locally created LIN transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poLinTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

        /* Add the transaction to the EAL and e2bcore transaction queues */
        eEalStatus = CheckAndAddTransactionToQueues(poEalLinData, poEalLinInstData, bIsMcast, &poLinTxnCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalLinData->nFifoWriteVlanTag : *poEalLinInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(poEalLinInstData->oId.nInstNum,
                                 poEalLinData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalLinInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag
                                );

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalLinData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalLinData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalLinData->bIsMulticast ?
                                      poEalLinData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalLinInstData->oId.nIntfNum;

        poEalLinData->bResyncPending = false;

        if(!bIsPeriodic)
        {
            /* Send the unicast / multicast e2b packet with the FIFO message containing the LIN transaction */
            eEalStatus = eal_utils_sendFifoFrame(&poEalLinInstData->oId, &oNwFrameCfg, poLinTxnCfg, &oFifoMsgOpts);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Add the transaction to queue for all instances */
        if(AddTransactionToQueue(poEalLinData, bIsMcast, apRxBuf) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }
    } while(false);

    return eEalStatus;
}

/* Extract LIN status data */
ADI_EAL_LIN_CODE
static void extractLinStatus(uint32_t *anData, ADI_EAL_LIN_STATUS_DATA *poStatusData)
{
    /* Extract wake-up signal detected flag */
    poStatusData->bIsWakeupSigDetect = (bool)GET_FIELD_VAL(anData[0u],
                                       BITM_LIN_STATUS_DETECTED_WAKEUP_SIGNAL,
                                       BITP_LIN_STATUS_DETECTED_WAKEUP_SIGNAL);

    /* Extract frame response too short flag */
    poStatusData->bIsFrameRespShort = (bool)GET_FIELD_VAL(anData[0u],
                                      BITM_LIN_STATUS_FRAME_RESPONSE_TOO_SHORT,
                                      BITP_LIN_STATUS_FRAME_RESPONSE_TOO_SHORT);

    /* Extract framing error status flag */
    poStatusData->bIsFrameErrDetect = (bool)GET_FIELD_VAL(anData[0u],
                                      BITM_LIN_STATUS_FRAMING_ERROR,
                                      BITP_LIN_STATUS_FRAMING_ERROR);

    /* Extract TX data contention status flag */
    poStatusData->bIsTransCollision = (bool)GET_FIELD_VAL(anData[0u],
                                      BITM_LIN_STATUS_TX_DATA_CONTENTION,
                                      BITP_LIN_STATUS_TX_DATA_CONTENTION);

    /* Extract PID field contention status flag */
    poStatusData->bIsPIDFldCollision = (bool)GET_FIELD_VAL(anData[0u],
                                       BITM_LIN_STATUS_PID_FIELD_CONTENTION,
                                       BITP_LIN_STATUS_PID_FIELD_CONTENTION);

    /* Extract sync field contention status flag */
    poStatusData->bIsSyncFldCollision = (bool)GET_FIELD_VAL(anData[0u],
                                        BITM_LIN_STATUS_SYNC_FIELD_CONTENTION,
                                        BITP_LIN_STATUS_SYNC_FIELD_CONTENTION);

    /* Extract break field contention status flag */
    poStatusData->bIsBrkFldCollision = (bool)GET_FIELD_VAL(anData[0u],
                                       BITM_LIN_STATUS_BREAK_FIELD_CONTENTION,
                                       BITP_LIN_STATUS_BREAK_FIELD_CONTENTION);
}

/*! Lock the mutex of all LIN instances */
ADI_EAL_LIN_CODE
static void LockLinMutex(EAL_LIN_DATA *poEalLinData)
{
    /* Loop through all the unicast LIN instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalLinData->apoInstData[i]->hLinMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all LIN instances */
ADI_EAL_LIN_CODE
static void UnlockLinMutex(EAL_LIN_DATA *poEalLinData)
{
    /* Loop through all the unicast LIN instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalLinData->apoInstData[i]->hLinMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Set the waiting for RX flag for all the instances of the handle */
ADI_EAL_LIN_CODE
static void SetWaitingForRx(EAL_LIN_DATA *poEalLinData, bool bWaitingForRx)
{
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalLinInstData->bWaitingForRx = bWaitingForRx;
    }
}

/*! Wait for response */
ADI_EAL_LIN_CODE
static bool WaitForResponses(EAL_LIN_DATA *poEalLinData)
{

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    bool bTimedOut = false;

    /* Loop through all instances and check if response is pending */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

        /* Check if we are still waiting for response for this instance */
        if(prj_osal_waitSemaphore(poEalLinInstData->hFifoRespSemaphore) != PRJ_OSAL_STATUS_SUCCESS)
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

    /* Wait for LIN write to be received by the host interface and
       send back a No-op transaction with the ID sent downstream */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);
        bAllResponsesReceived = true;

        for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalLinInstData->bWaitingForRx)
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

/** Handle error callback */
ADI_EAL_LIN_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal LIN data */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)pCbParam;

    /* Get the LIN unicast instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalLinInstData->bIgnoreErr)
    {
        InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_LIN_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal LIN data */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)pCbParam;

    /* Get the LIN unicast instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* If the read is of the LIN STATUS registers */
    if((poMsgOpts->nTopic == ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum) &&
            (poRegCfg->anStartAddr[0U] == LIN_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 1U))
    {
        /* Process LIN status data */
        ADI_EAL_LIN_STATUS_DATA oLinStatusData;
        extractLinStatus(poRegCfg->pBuffer, &oLinStatusData);
        InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_INTF_STATUS_EVT, &oLinStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_LIN_CODE
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_LIN_TXN_CONFIG *poLin = (ADI_E2BCORE_LIN_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the LIN payload */
    ADI_E2BCORE_LIN_TXN_PAYLOAD *poLinPayload = (ADI_E2BCORE_LIN_TXN_PAYLOAD *)poLin->pPayload;

    /* Get the internal LIN data */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)pCbParam;

    /* Get the LIN unicast instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    /* If the instance is in normal mode */
    if(poEalLinInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
    {
        /* Increment the read index */
        poEalLinInstData->nLinQRdIdx = EAL_INC_Q_IDX(poEalLinInstData->nLinQRdIdx, poEalLinInstData->nLinCfgQDepth);

        /*******************************
            Cases:
                * Callback indicating LIN write completed (Give callback)
                * Callback indicating LIN write read completed (Give callback)
                * Callback indicating LIN write sync completed (Clear bWaitingForRx flag)
                * Callback indicating LIN write ready sync completed (Clear bWaitingForRx flag)
        *******************************/

        if(poEalLinInstData->bWaitingForRx)
        {
            poEalLinInstData->bWaitingForRx = false;

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
            /* Signal the semaphore to indicate response received */
            prj_osal_signalSemaphore(poEalLinInstData->hFifoRespSemaphore);
#endif
        }
        else if(!poLinPayload->bIsWriteTxn)
        {
            InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_LIN_READ_RESP, poLin);
        }
        else
        {
            InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_LIN_WRITE_RESP, poLin);
        }
    }
    /* If the instance is in periodic mode */
    else if(poEalLinInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
    {
        InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_PERIODIC_RX, poLin);

        if(adi_e2bcore_addLinConfigToQueue(poID, poLin) != ADI_E2BCORE_ERR_NONE)
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to add lin config to queue");
        }
    }
    /* If the instance is in custom mode */
    else if(poEalLinInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
    {
        /* Increment the read index */
        poEalLinInstData->nLinQRdIdx = EAL_INC_Q_IDX(poEalLinInstData->nLinQRdIdx, poEalLinInstData->nLinCfgQDepth);
        InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_LIN_FIFO_READ, poFifoDat);
    }
    else
    {
        /* Nothing to do */
    }
}

static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal LIN data */
    EAL_LIN_DATA *poEalLinData = (EAL_LIN_DATA *)pCbParam;

    /* Get the LIN unicast instance data */
    EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[0U];

    ADI_EAL_LIN_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd = poAckDat->bFd;
    oAckData.nTopic = poAckDat->nTopic;

    InvokeCbk(poEalLinData, poEalLinInstData, ADI_EAL_LIN_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/** Invoke the callback */
ADI_EAL_LIN_CODE
static void InvokeCbk(EAL_LIN_DATA *poEalLinData, EAL_LIN_INST_DATA *poEalLinInstData, ADI_EAL_LIN_CBK_EVT eEvt,
                      const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalLinInstData->pfunCbk != NULL)
    {
        (poEalLinInstData->pfunCbk)(poEalLinInstData->oId.nInstNum, poEalLinData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_LIN_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_LIN_DATA *poEalLinData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalLinInstData->nLinQWrIdx, poEalLinInstData->nLinQRdIdx,
                               poEalLinInstData->nLinCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_LIN_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_LIN_DATA *poEalLinData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInLinQueue(&poEalLinInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_LIN_CODE
static void CopyTransactionConfigToQueue(EAL_LIN_DATA *poEalLinData, ADI_E2BCORE_LIN_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_LIN_TXN_CONFIG *poInstTxnCfg = &poEalLinInstData->paoCfgs[poEalLinInstData->nLinQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_LIN_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_LIN_DATA *poEalLinData, bool bIsMcast, uint8_t **apRxBuf)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalLinData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_LIN_INST_DATA *poEalLinInstData = poEalLinData->apoInstData[i];

        /* Update the RX pointer for read transactions (E2BSW-3419) */
        ADI_E2BCORE_LIN_TXN_CONFIG *poInstTxnCfg = &poEalLinInstData->paoCfgs[poEalLinInstData->nLinQWrIdx];
        ADI_E2BCORE_LIN_TXN_PAYLOAD *poLinPayload = (ADI_E2BCORE_LIN_TXN_PAYLOAD *)poInstTxnCfg->pPayload;
        poLinPayload->pData = apRxBuf != NULL ? apRxBuf[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addLinConfigToQueue(&poEalLinInstData->oId, poInstTxnCfg);

            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalLinInstData->nLinQWrIdx = EAL_INC_Q_IDX(poEalLinInstData->nLinQWrIdx, poEalLinInstData->nLinCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_LIN_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_LIN_DATA *poEalLinData, EAL_LIN_INST_DATA *poEalLinInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoLinTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalLinData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalLinData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalLinData, *ppoLinTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoLinTxnCfg = &poEalLinInstData->paoCfgs[poEalLinInstData->nLinQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Sends custom LIN transactions */
ADI_EAL_LIN_CODE
static ADI_EAL_STATUS SendCustomLinTransactions(EAL_LIN_DATA *poEalLinData, EAL_LIN_INST_DATA *poEalLinInstData,
        ADI_E2BCORE_LIN_TXN_CONFIG *paoLinTxnCfg, uint16_t nNumLinTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalLinInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalLinInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalLinInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalLinInstData->nLinQWrIdx,
                                         poEalLinInstData->nLinQRdIdx,
                                         poEalLinInstData->nLinCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumLinTxns > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInLinQueue(&poEalLinInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumLinTxns > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalLinInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bMailbox       = poEalLinData->bMailboxMode;
        oFifoMsgOpts.bSync          = poEalLinData->bResyncPending;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        poEalLinData->bResyncPending = false;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add LIN transaction */
        eEalStatus = AddCustomLinTxnsToMsg(nInstNum, poEalLinData, poEalLinInstData, paoLinTxnCfg, nNumLinTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the LIN EAL mode to operate in custom mode */
        poEalLinInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/** Add multiple custom LIN transactions */
ADI_EAL_LIN_CODE
static ADI_EAL_STATUS AddCustomLinTxnsToMsg(uint32_t nInstNum, EAL_LIN_DATA *poEalLinData,
        EAL_LIN_INST_DATA *poEalLinInstData, ADI_E2BCORE_LIN_TXN_CONFIG *paoLinTxnCfg, uint16_t nNumLinTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add LIN transaction */
    for(uint16_t i = 0U; i < nNumLinTxns; i++)
    {
        /* Since the check has been done above, space has to be available.
           Assert this assumption */
        ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                            poEalLinInstData->nLinQWrIdx,
                            poEalLinInstData->nLinQRdIdx,
                            poEalLinInstData->nLinCfgQDepth),
                        "Invalid assumption!");

        /* Copy the given LIN configuration into the config queue pool */
        (void)ADI_MEMCPY(&poEalLinInstData->paoCfgs[poEalLinInstData->nLinQWrIdx], &paoLinTxnCfg[i],
                         sizeof(ADI_E2BCORE_LIN_TXN_CONFIG));

        /* Add the LIN transaction */
        if(adi_e2bcore_addLinTxn(nInstNum, &poEalLinInstData->paoCfgs[poEalLinInstData->nLinQWrIdx]) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            break;
        }

        /* Increment queue index */
        poEalLinInstData->nLinQWrIdx = EAL_INC_Q_IDX(poEalLinInstData->nLinQWrIdx, poEalLinInstData->nLinCfgQDepth);
    }

    return eEalStatus;
}


#endif /* ADI_E2B_IFACE_LIN_ENABLED */
/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
