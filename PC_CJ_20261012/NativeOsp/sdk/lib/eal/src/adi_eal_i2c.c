/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_i2c.c
 * @brief: The EAL APIs for I2C interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_I2C_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalI2cData) \
ADI_DBG_REQUIRE((poEalI2cData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalI2cData)->bInUse == true, "Instance is not opened");

/*=================================== DATA ==================================*/

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendI2cTransaction(
    EAL_I2C_DATA *poEalI2cData,
    ADI_E2BCORE_I2C_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod);
static void extractI2cStatus(uint32_t *anData, ADI_EAL_I2C_STATUS_DATA *poStatusData);
static void LockI2cMutex(EAL_I2C_DATA *poEalI2cData);
static void UnlockI2cMutex(EAL_I2C_DATA *poEalI2cData);
static void SetWaitingForRx(EAL_I2C_DATA *poEalI2cData, bool bWaitingForRx);
static bool WaitForResponses(EAL_I2C_DATA *poEalI2cData);
static bool IsResponseExpected(EAL_I2C_INST_DATA *poEalI2cData, ADI_E2BCORE_I2C_TXN_CONFIG *poTxnCfg);
static uint16_t GetNumTxnsWithResp(EAL_I2C_INST_DATA *poEalI2cInstData, ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg,
                                   uint16_t nNumI2cTxns);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void InvokeCbk(EAL_I2C_DATA *poEalI2cData, EAL_I2C_INST_DATA *poEalI2cInstData, ADI_EAL_I2C_CBK_EVT eEvt,
                      const void *pArg);
static bool IsSpaceAvblInEalTxnQueue(EAL_I2C_DATA *poEalI2cData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_I2C_DATA *poEalI2cData);
static void CopyTransactionConfigToQueue(EAL_I2C_DATA *poEalI2cData, ADI_E2BCORE_I2C_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_I2C_DATA *poEalI2cData, bool bIsMcast, uint8_t **apRxBuf);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_I2C_DATA *poEalI2cData, EAL_I2C_INST_DATA *poEalI2cInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoI2cTxnCfg);
static ADI_EAL_STATUS SendCustomI2cTransactions(EAL_I2C_DATA *poEalI2cData, EAL_I2C_INST_DATA *poEalI2cInstData,
        ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg, uint16_t nNumI2cTxns, uint16_t nNumTxnsWithResp);
static ADI_EAL_STATUS AddCustomI2cTxnsToMsg(uint32_t nInstNum, EAL_I2C_DATA *poEalI2cData,
        EAL_I2C_INST_DATA *poEalI2cInstData, ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg, uint16_t nNumI2cTxns);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an I2C instance
 *
 * @param [in]  poI2cCfg          Configuration pointer for EAL I2C instance
 * @param [out] phI2cHandle       Returns the I2C handle for use in other EAL I2C APIs
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: I2C instance was successfully opened.
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cOpen(ADI_EAL_I2C_CFG *poI2cCfg, ADI_EAL_I2C_HANDLE *phI2cHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poI2cCfg != NULL, "Pointer to the I2C module configuration is NULL");
    ADI_DBG_REQUIRE(phI2cHandle != NULL, "Pointer to the I2C handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poI2cCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poI2cCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poI2cCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poI2cCfg->nInstNum;
    uint32_t nRemoteNum = poI2cCfg->nRemoteNum;
    uint32_t nIntfNum = poI2cCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_I2C,
                    "The given pair of instance, remote and interface numbers is not configured as I2C interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poI2cCfg->pEalI2cMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poI2cCfg->nEalI2cMemorySize >= ADI_EAL_I2C_UNICAST_INST_MEM_SIZE(poI2cCfg->nI2cCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poI2cCfg->pEalI2cMemory;

    /* Allocate memory for the I2C transaction config queue. */
    ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfgs = (ADI_E2BCORE_I2C_TXN_CONFIG *)((void *)pMem);
    pMem += (poI2cCfg->nI2cCfgQDepth * sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));

    /* Allocate memory for the EAL I2C instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = (EAL_I2C_INST_DATA *)((void *)pMem);
    pMem += sizeof(EAL_I2C_INST_DATA);
    /* ~~~~~~~~~~~ I2C instance info ~~~~~~~~~~~ */
    poEalI2cInstData->oId.nInstNum      = nInstNum;
    poEalI2cInstData->oId.nRemoteNum    = nRemoteNum;
    poEalI2cInstData->oId.nIntfNum      = nIntfNum;
    poEalI2cInstData->pnIfRegVlanTag    = &poIfConfig->nIfRegVlanTag;
    poEalI2cInstData->pnFifoRegVlanTag  = &poIfConfig->nFifoRegVlanTag;
    poEalI2cInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;
    /* ~~~~~~~~~~~ I2C state info ~~~~~~~~~~~ */
    poEalI2cInstData->bIgnoreErr        = false;
    poEalI2cInstData->eMode             = ADI_EAL_USAGE_MODE_NORMAL;
    poEalI2cInstData->pfunCbk           = NULL; /* Will be filled in adi_eal_i2cRegisterCallback */
    poEalI2cInstData->bWaitingForRx     = false;
    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    uint32_t nI2cCtrlReg = 0U;
    EAL_I2C_INST_DATA **apoInstData;
    EAL_I2C_DATA *poEalI2cData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalI2cInstData->oId, I2C_CONTROL_NW_ADDR, &nI2cCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalI2cInstData->bIsReadTimeTagEn  = (nI2cCtrlReg & BITM_I2C_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalI2cInstData->nI2cCfgQDepth     = poI2cCfg->nI2cCfgQDepth;
    poEalI2cInstData->nI2cQWrIdx        = 0U;
    poEalI2cInstData->nI2cQRdIdx        = 0U;
    poEalI2cInstData->paoCfgs           = paoI2cTxnCfgs;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalI2cInstData->oId, (void *)pMem,
            poI2cCfg->nI2cCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_I2C_TXN_CONFIG *) * poI2cCfg->nI2cCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_I2C_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_I2C_INST_DATA *);
        apoInstData[0U] = poEalI2cInstData;

        /* Allocate memory for the internal EAL I2C data structure */
        poEalI2cData  = (EAL_I2C_DATA *)((void *)pMem);
        pMem += sizeof(EAL_I2C_DATA);
        poEalI2cData->bIsMulticast  = false;
        poEalI2cData->bInUse        = true;
        poEalI2cData->nIntfTopic    = 0U;
        poEalI2cData->nFifoTopic    = 0U;
        poEalI2cData->nNumInst      = 1U;
        poEalI2cData->apoInstData   = apoInstData;
        poEalI2cData->bMailboxMode  = false;
        poEalI2cData->bResyncPending = false;

        /* Initialise the mutex for this I2C instance */
        poEalI2cInstData->oI2cMutexCfg.pMem = &poEalI2cInstData->anI2cMutexBuffer[0U];
        poEalI2cInstData->oI2cMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalI2cInstData->oI2cMutexCfg, &poEalI2cInstData->hI2cMutex);

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        /* Initialise the semaphore for this I2C instance */
        poEalI2cInstData->oFifoRespSemaphoreCfg.pMem = &poEalI2cInstData->anI2cFifoRespSemaphoreBuffer[0U];
        poEalI2cInstData->oFifoRespSemaphoreCfg.nMemSize = PRJ_OSAL_SEMAPHORE_MEM_SIZE;
        poEalI2cInstData->oFifoRespSemaphoreCfg.bCountingSemaphore = false;
        poEalI2cInstData->oFifoRespSemaphoreCfg.nMaxCount = 1U;
        poEalI2cInstData->oFifoRespSemaphoreCfg.nInitialCount = 0U;
        (void)prj_osal_createSemaphore(&poEalI2cInstData->oFifoRespSemaphoreCfg, &poEalI2cInstData->hFifoRespSemaphore);
#endif

        /* Set the callback param in the e2bcore interface config */
        poIfConfig->pCbParam = poEalI2cData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_I2C;

        /* Set the handle to the EAL_I2C_DATA structure */
        *phI2cHandle = (ADI_EAL_I2C_HANDLE)poEalI2cData;
    }

    return eStatus;
}

/**
 * @brief       Closes an opened I2C instance
 * @param [in]  hI2cHandle       The I2C handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: When the I2C device was successfully closed.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cClose(ADI_EAL_I2C_HANDLE hI2cHandle)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalI2cData->apoInstData[i]->hI2cMutex);
#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        (void)prj_osal_destroySemaphore(poEalI2cData->apoInstData[i]->hFifoRespSemaphore);
#endif
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalI2cData, 0, sizeof(EAL_I2C_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to register callback
 * @param [in]  hI2cHandle        The I2C handle
 * @param [in]  pfI2cCallback     Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS:   Returns ADI_EAL_STATUS_SUCCESS if callback was registered.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cRegisterCallback(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_CALLBACK_FUNC pfI2cCallback)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(pfI2cCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalI2cData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* Register the callback function */
    poEalI2cInstData->pfunCbk = pfI2cCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Creates a multicast group of I2C interfaces
 *
 * @param [in]  poI2cMcastCfg       Configuration for I2C multicast group
 * @param [out] phI2cMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cCreateMcastGroup(ADI_EAL_I2C_MCAST_CFG *poI2cMcastCfg,
        ADI_EAL_I2C_HANDLE *phI2cMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poI2cMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phI2cMcastHandle != NULL, "Pointer to the I2C multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poI2cMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poI2cMcastCfg->pahI2cHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poI2cMcastCfg->pI2cMcastMem != NULL, "Pointer to memory for I2C multicast is NULL");
    ADI_DBG_REQUIRE(poI2cMcastCfg->nI2cMcastMemSize >= ADI_EAL_I2C_MCAST_GROUP_MEM_SIZE(poI2cMcastCfg->nNumInst),
                    "Not enough memory allocated for I2C multicast group");
    ADI_DBG_REQUIRE(poI2cMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poI2cMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poI2cMcastCfg->pI2cMcastMem;
    EAL_I2C_INST_DATA **apoInstData = (EAL_I2C_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_I2C_INST_DATA *) * poI2cMcastCfg->nNumInst);

    /* Save the unicast EAL I2C instances */
    for(uint16_t i = 0U; i < poI2cMcastCfg->nNumInst; i++)
    {
        EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)poI2cMcastCfg->pahI2cHandles[i];
        apoInstData[i] = poEalI2cData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast FIFO topic */
        uint32_t nInstNum = poEalI2cData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalI2cData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalI2cData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poI2cMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poI2cMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poI2cMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poI2cMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_I2C_DATA *poEalI2cMcastData = (EAL_I2C_DATA *)((void *)pMem);
    poEalI2cMcastData->bIsMulticast = true;
    poEalI2cMcastData->bInUse       = true;
    poEalI2cMcastData->nIntfTopic   = poI2cMcastCfg->nIntfTopic;
    poEalI2cMcastData->nFifoTopic   = poI2cMcastCfg->nFifoTopic;
    poEalI2cMcastData->nIfRegVlanTag       = (poI2cMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poI2cMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalI2cMcastData->nFifoRegVlanTag     = (poI2cMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poI2cMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalI2cMcastData->nFifoWriteVlanTag   = (poI2cMcastCfg->poFifoWriteVlanTag != NULL) ? eal_utils_convertVlanTags(
                poI2cMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalI2cMcastData->nNumInst     = poI2cMcastCfg->nNumInst;
    poEalI2cMcastData->apoInstData  = apoInstData;
    poEalI2cMcastData->bMailboxMode = false;
    poEalI2cMcastData->bResyncPending = false;

    /* Set the handle to the EAL_I2C_DATA structure */
    *phI2cMcastHandle = (ADI_EAL_I2C_HANDLE)poEalI2cMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief      Sends an I2C write transaction
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_WRITE_RESP event will be invoked on completion if ID != 0xFF.
 * @note       <b>E2B_ID Usage Pattern (Async API)</b>:
 *             - E2B_ID is <b>optional</b> for asynchronous write operations
 *             - Set nId = 0xFF to disable E2B_ID
 *             - Set nId to any value (0x00-0xFE) to enable E2B_ID tracking
 *             - When enabled, E2B_ID is copied to the upstream acknowledgment message,
 *               allowing correlation between this downstream command and device response
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write transaction.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWrite(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr  = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nAddress          = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen         = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = 0U;
    oTxnCfg.pPayload                  = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C write transaction synchronously.
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nId               Identifier for the write request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write transaction.
 * @note       This API blocks till a No-op transaction with the ID is returned back
 *             indicating the reception of the I2C write transaction in the I2C host interface.
 * @note       <b>E2B_ID Usage Pattern (Sync API)</b>:
 *             - E2B_ID is <b>always enabled</b> for synchronous operations
 *             - This is <b>intentional and required</b> to receive device acknowledgment
 *             - The API blocks waiting for the upstream response with matching E2B_ID
 *             - E2B_ID enables correlation between the downstream write command and
 *               the upstream acknowledgment, confirming the device processed the transaction
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *             - The nId parameter value is used for tracking and correlation
 * @note       This is a synchronous API that internally flushes the transmit buffer before returning.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWriteSync(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr  = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nAddress          = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen         = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = 0U;
    oTxnCfg.pPayload                  = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalI2cData, true);

    /* Send the I2C transaction */
    eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, NULL, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bool bTimedOut = WaitForResponses(poEalI2cData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalI2cData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C read transaction
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nId               Identifier for the read request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C read transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP event will be invoked on
 *             reception of the I2C response and poI2cData->apRxData will be updated with
 *             the received data.
 * @note       <b>E2B_ID Usage Pattern (Async Read API)</b>:
 *             - E2B_ID is <b>always enabled</b> for read operations
 *             - This is <b>required</b> to correlate read requests with read responses
 *             - The E2B_ID from the downstream read request is copied to the upstream
 *               read response, enabling the application to match responses to requests
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *             - This is essential for handling multiple concurrent read operations
 *             - The nId value is returned in the callback for request/response correlation
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cRead(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr  = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nAddress          = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen         = 0U;
    oTxnCfg.oTxnHdr.nReadLen          = poI2cData->nReadLength;
    oTxnCfg.pPayload                  = NULL;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C read transaction synchronously
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nId               Identifier for the read request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C read transaction.
 * @note       This API blocks till I2C read response is received.
 * @note       poI2cData->apRxData will be updated with the I2C Read data.
 * @note       <b>E2B_ID Usage Pattern (Sync Read API)</b>:
 *             - E2B_ID is <b>always enabled</b> for synchronous reads
 *             - This is <b>intentional and required</b> for proper operation
 *             - The API blocks waiting for the upstream read response with matching E2B_ID
 *             - E2B_ID correlation ensures the API receives the correct response data
 *               for the specific read request, especially important in multi-request scenarios
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *             - The device copies E2B_ID to the response message for tracking
 * @note       This is a synchronous API that internally flushes the transmit buffer and waits for the response before returning.
 *             The response data is available upon successful return (no timeout or error).
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cReadSync(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr  = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nAddress          = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen         = 0U;
    oTxnCfg.oTxnHdr.nReadLen          = poI2cData->nReadLength;
    oTxnCfg.pPayload                  = NULL;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalI2cData, true);

    /* Send the I2C transaction */
    eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for I2C read response */
        bool bTimedOut = WaitForResponses(poEalI2cData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalI2cData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C write read transaction
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nId               Identifier for the write request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write read transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP event will be invoked on
 *             reception of the I2C response and poI2cData->apRxData will be updated with
 *             the received data.
 * @note       <b>E2B_ID Usage Pattern (Async Write-Read API)</b>:
 *             - E2B_ID is <b>always enabled</b> for write-read operations
 *             - This is <b>required</b> to correlate the combined write-read request with response
 *             - The E2B_ID is included in the downstream message and copied to the upstream
 *               read response, enabling proper command-response correlation
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *             - Essential for matching responses when multiple write-read operations are pending
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWriteRead(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Read length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr  = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nAddress          = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen         = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poI2cData->nReadLength;
    oTxnCfg.pPayload                  = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C write read transaction synchronously
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nId               Identifier for the write request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write read transaction.
 * @note       This API blocks till the I2C read response is received.
 * @note       poI2cData->apRxData will be updated with the I2C Read data.
 * @note       <b>E2B_ID Usage Pattern (Sync Write-Read API)</b>:
 *             - E2B_ID is <b>always enabled</b> for synchronous write-read
 *             - This is <b>intentional and required</b> for proper blocking operation
 *             - The API blocks waiting for the upstream read response with matching E2B_ID
 *             - E2B_ID correlation ensures correct data is received for this specific transaction
 *             - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *             - The device copies E2B_ID to the response message for tracking
 * @note       This is a synchronous API that internally flushes the transmit buffer and waits for the response before returning.
 *             The response data is available upon successful return (no timeout or error).
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWriteReadSync(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Read length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr  = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nAddress          = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen         = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poI2cData->nReadLength;
    oTxnCfg.pPayload                  = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalI2cData, true);

    /* Send the I2C transaction */
    eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for I2C read response */
        bool bTimedOut = WaitForResponses(poEalI2cData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalI2cData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C write transaction with delay information
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nDelay            The delay time in ns
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_WRITE_RESP event will be invoked on completion if ID != 0xFF.
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWriteWithDelay(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint32_t nDelay,
        uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nTimeDelay          = nDelay;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr    = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.nAddress            = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen           = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = 0U;
    oTxnCfg.pPayload                    = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C write transaction with presentation time information
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nTimestamp        The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_WRITE_RESP event will be invoked on completion if ID != 0xFF.
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWriteWithPresTs(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent  = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nGptpTime           = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr    = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.nAddress            = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen           = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = 0U;
    oTxnCfg.pPayload                    = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C read transaction with delay information
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nDelay            The delay time in ns
 * @param [in] nId               Identifier for the write request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C read transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP event will be invoked on
 *             reception of the I2C response and poI2cData->apRxData will be updated with
 *             the received data.
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cReadWithDelay(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
                                        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = true;
    oTxnCfg.oTxnHdr.nTimeDelay          = nDelay;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr    = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.nAddress            = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen           = 0U;
    oTxnCfg.oTxnHdr.nReadLen            = poI2cData->nReadLength;
    oTxnCfg.pPayload                    = NULL;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C read transaction with presentation time information
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nTimestamp        The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in] nId               Identifier for the write request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C read transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP event will be invoked on
 *             reception of the I2C response and poI2cData->apRxData will be updated with
 *             the received data.
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cReadWithPresTs(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent  = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = true;
    oTxnCfg.oTxnHdr.nGptpTime           = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr    = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.nAddress            = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen           = 0U;
    oTxnCfg.oTxnHdr.nReadLen            = poI2cData->nReadLength;
    oTxnCfg.pPayload                    = NULL;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C write read transaction with delay information
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nDelay            The delay time in ns
 * @param [in] nId               Identifier for the write request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write read transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP event will be invoked on
 *             reception of the I2C response and poI2cData->apRxData will be updated with
 *             the received data.
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWriteReadWithDelay(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Read length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = true;
    oTxnCfg.oTxnHdr.nTimeDelay          = nDelay;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr    = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.nAddress            = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen           = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = poI2cData->nReadLength;
    oTxnCfg.pPayload                    = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sends an I2C write read transaction with presentation time information
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nTimestamp        The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in] nId               Identifier for the write request
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write read transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP event will be invoked on
 *             reception of the I2C response and poI2cData->apRxData will be updated with
 *             the received data.
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cWriteReadWithPresTs(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poI2cData->nWriteLength > 0U, "I2C Write length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nReadLength > 0U, "I2C Read length is invalid");
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent  = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = true;
    oTxnCfg.oTxnHdr.nGptpTime           = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr    = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.nAddress            = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen           = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = poI2cData->nReadLength;
    oTxnCfg.pPayload                    = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Send the I2C transaction */
    ADI_EAL_STATUS eEalStatus = SendI2cTransaction(poEalI2cData, &oTxnCfg, poI2cData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief      Sets up the FIFO in periodic mode and sends an I2C write read transaction
 * @param [in] hI2cHandle        The I2C handle
 * @param [in] poI2cData         Pointer to I2C I/O DATA
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS:   Successfully sent the I2C write read transaction.
 * @note       Callback with #ADI_EAL_I2C_CBK_EVT_PERIODIC_RX event will be invoked
 *             when the I2C response is received for each period and poI2cData->apRxData
 *             will be updated with the received data.
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cSetupPeriodicWriteRead(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poI2cData != NULL, "I2C IO data pointer is NULL");

    /* More I2C IO data checks */
    ADI_DBG_REQUIRE(poI2cData->nAddress <= 127U, "I2C Address is invalid");

    /* Set the I2C transaction configuration */
    ADI_E2BCORE_I2C_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideDevAddr  = true;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nAddress          = poI2cData->nAddress;
    oTxnCfg.oTxnHdr.nWriteLen         = poI2cData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poI2cData->nReadLength;
    oTxnCfg.pPayload                  = poI2cData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockI2cMutex(poEalI2cData);

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalI2cData->bIsMulticast;

    /* Use the locally created I2C transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poI2cTxnCfg = &oTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalI2cInstData, poI2cTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalI2cData, poEalI2cInstData, bIsMcast, &poI2cTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalI2cData, bIsMcast, poI2cData->apRxData) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }

    } while(false);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Switch the mode of all the instances to periodic */
        for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
        {
            EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];
            if(adi_e2bcore_addI2cConfigToQueue(&(poEalI2cInstData->oId), poI2cTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
            poEalI2cInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
        }
    }

    /* Loop through the mutex of all instances and unlock it */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief       Gets the I2C interface status
 *
 * @param [in]  hI2cHandle      The I2C interface Handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   : Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cReadStatus(ADI_EAL_I2C_HANDLE hI2cHandle)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);

    bool bIsMcast = poEalI2cData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalI2cData->nIfRegVlanTag : *poEalI2cInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalI2cData->nIntfTopic;
        anTopics[1U] = poEalI2cData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalI2cInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalI2cInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalI2cInstData->oId, I2C_STATS1_NW_ADDR, 3U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief To write/read to/from interface fifo based on the transaction config
 *
 * @param [in]  hI2cHandle     The I2C interface Handle
 * @param [in]  paoI2cTxnCfg   I2C transaction config
 * @param [in]  nNumI2cTxns    Number of I2C transactions
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   : Failed to add the register map message
 * @note        This API only works with unicast handles.
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cCustomWriteRead(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg,
        uint16_t nNumI2cTxns)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(paoI2cTxnCfg != NULL, "I2C transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumI2cTxns > 0U, "Number of I2C transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalI2cData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* Lock the mutex */
    LockI2cMutex(poEalI2cData);

    /* Get the number of transactions that expects a response */
    uint16_t nNumTxnsWithResp = GetNumTxnsWithResp(poEalI2cInstData, paoI2cTxnCfg, nNumI2cTxns);

    /* Send the custom I2C transactions */
    eEalStatus = SendCustomI2cTransactions(poEalI2cData, poEalI2cInstData, paoI2cTxnCfg, nNumI2cTxns, nNumTxnsWithResp);

    /* Unlock the mutex */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief Send a register map transaction to the I2C interface or the corresponding FIFO registers
 *
 * @param [in]    hI2cHandle             The I2C interface Handle
 * @param [in]    bIsFifoRegTransaction  Is this reg transaction mapped destined to FIFO registers?
 * @param [inout] poRegCfg               Register read details
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   : Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cSendRegTransaction(ADI_EAL_I2C_HANDLE hI2cHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalI2cInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalI2cInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalI2cInstData->oId.nIntfNum;

    bool bIsMcast = poEalI2cData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync          = poEalI2cData->bResyncPending;
    oMsgOpts.bMailbox       = poEalI2cData->bMailboxMode;
    oMsgOpts.bTopicOverRide = true;

    poEalI2cData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalI2cData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalI2cData->nFifoRegVlanTag : *poEalI2cInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalI2cData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalI2cData->nIfRegVlanTag : *poEalI2cInstData->pnIfRegVlanTag;
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
 * @brief       Resets the I2C transaction queue and goes back to a clean state
 *
 * @param  [in] hI2cHandle      The I2C handle
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: I2C reset successful.
 * @note        This API only works with unicast handles.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cResetQueue(ADI_EAL_I2C_HANDLE hI2cHandle)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalI2cData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* Lock the mutex */
    LockI2cMutex(poEalI2cData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearI2cConfigQueue(&poEalI2cInstData->oId);
        poEalI2cInstData->nI2cQWrIdx = 0U;
        poEalI2cInstData->nI2cQRdIdx = 0U;
        poEalI2cInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
        poEalI2cInstData->bWaitingForRx = false;

        /* Ignore errors */
        poEalI2cInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  I2C_CONTROL_NW_ADDR;
        uint32_t nMask      =  BITM_I2C_CONTROL_RESYNC_REQUIRED;
        uint32_t nData      =  BITM_I2C_CONTROL_RESYNC_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_i2cSendRegTransaction((ADI_EAL_I2C_HANDLE)poEalI2cData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an I2C NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalI2cInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);

        /* Issue FIFO resets */
        nStartAddr = I2C_CONTROL_NW_ADDR;
        nMask      = BITM_I2C_CONTROL_TX_FIFO_RESET | BITM_I2C_CONTROL_RX_FIFO_RESET;
        nData      = BITM_I2C_CONTROL_TX_FIFO_RESET | BITM_I2C_CONTROL_RX_FIFO_RESET;
        eEalStatus = adi_eal_i2cSendRegTransaction((ADI_EAL_I2C_HANDLE)poEalI2cData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalI2cInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockI2cMutex(poEalI2cData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for I2C instance
 *
 * @param [in] hI2cHandle         I2C instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_I2C_CODE
void adi_eal_i2cSetDownstreamVlan(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                  ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* Extract the unicast instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* If it is a multicast handle, update the handle structure */
    if(poEalI2cData->bIsMulticast)
    {
        poEalI2cData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalI2cData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalI2cData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalI2cInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalI2cInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalI2cInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for I2C FIFO messages
 *
 * @param [in] hI2cHandle  I2C interface Handle
 * @param [in] poVlanTag   Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn   If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   : Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cSetUpstreamVlan(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_VLAN_CFG *poVlanTag, bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalI2cData);

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

    return adi_eal_i2cSendRegTransaction(poEalI2cData, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for I2C transactions
 *
 * @param [in]  hI2cHandle      The I2C handle
 * @param [in]  bEnable         Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cSetMailboxMode(ADI_EAL_I2C_HANDLE hI2cHandle, bool bEnable)
{
    /* Cast the opaque handle to the internal EAL I2C data structure */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Validate the handle (non-NULL and instance opened) */
    BASIC_HANDLE_CHECK(poEalI2cData);

    /* Enable/disable mailbox mode for this I2C handle.
       Mailbox mode affects how outgoing messages are delivered (acknowledged/
       sequence-numbered mailbox transport vs normal FIFO transport). */
    poEalI2cData->bMailboxMode = bEnable;

    /* Return success */
    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for I2C transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hI2cHandle      The I2C handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_I2C_CODE
ADI_EAL_STATUS adi_eal_i2cResyncSeqNum(ADI_EAL_I2C_HANDLE hI2cHandle)
{
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)hI2cHandle;

    /* Basic parameter checks - ensure handle is valid and instance is opened */
    BASIC_HANDLE_CHECK(poEalI2cData);

    /* Request sequence-number resynchronization:
       Setting bResyncPending causes the next outgoing message to have the
       sync bit set so the remote can resynchronize sequence numbers.
       The flag is cleared by the send path once the bit has been applied. */
    poEalI2cData->bResyncPending = true;

    /* Indicate the request was accepted */
    return ADI_EAL_STATUS_SUCCESS;
}
/*============================ CALLBACKS ============================*/
/**
 * @brief The callback for I2C interface
 * @param poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param eType         The type of event this callback is for
 * @param poParam       The data corresponding to this event
 * @param bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param pCbParam      Parameter to be passed back while invoking callback
 */
ADI_EAL_I2C_CODE
void I2cInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                     void *pCbParam)
{
    /* If pCbParam is NULL, the EAL I2C interface instance is not opened,
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

/******************************* STATIC FUNCTIONS *************************************/
/*! \cond PRIVATE */

/*! Sends the I2C transaction */
ADI_EAL_I2C_CODE
static ADI_EAL_STATUS SendI2cTransaction(
    EAL_I2C_DATA *poEalI2cData,
    ADI_E2BCORE_I2C_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalI2cData->bIsMulticast;

    /* Use the locally created I2C transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poI2cTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalI2cInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalI2cData, poEalI2cInstData, bIsMcast, &poI2cTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalI2cData->nFifoWriteVlanTag : *poEalI2cInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(poEalI2cInstData->oId.nInstNum,
                                 poEalI2cData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalI2cInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag
                                );

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalI2cData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalI2cData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalI2cData->bIsMulticast ?
                                      poEalI2cData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalI2cInstData->oId.nIntfNum;

        poEalI2cData->bResyncPending = false;

        if(!bIsPeriodic)
        {
            /* Send the unicast / multicast e2b packet with the FIFO message containing the I2C transaction */
            eEalStatus = eal_utils_sendFifoFrame(&poEalI2cInstData->oId, &oNwFrameCfg, poI2cTxnCfg, &oFifoMsgOpts);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalI2cData, bIsMcast, apRxBuf) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/**
 * @brief       Extract I2C status from register data array
 *
 * @param [in]  anData      Register data array
 * @param [out] poStatusData I2C status data
 */
ADI_EAL_I2C_CODE
static void extractI2cStatus(uint32_t *anData, ADI_EAL_I2C_STATUS_DATA *poStatusData)
{
    /* Extract the number of write bytes, read bytes and transactions */
    poStatusData->nNumRdBytes = (uint16_t)((anData[0U] & 0xFFFF0000U) >> 16U);
    poStatusData->nNumI2cTxns = (uint16_t)(anData[0U] & 0xFFFFU);
    poStatusData->nNumWrBytes = anData[1U];
    /* Extract the I2C status flags */
    poStatusData->bBusBusy = ((anData[2U] & 0x1U) == 0U) ? false : true;
    poStatusData->bDataNack = ((anData[2U] & 0x2U) == 0U) ? false : true;
    poStatusData->bAddrNack = ((anData[2U] & 0x4U) == 0U) ? false : true;
}

/*! Lock the mutex of all I2C instances */
ADI_EAL_I2C_CODE
static void LockI2cMutex(EAL_I2C_DATA *poEalI2cData)
{
    /* Loop through all the unicast I2C instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalI2cData->apoInstData[i]->hI2cMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all I2C instances */
ADI_EAL_I2C_CODE
static void UnlockI2cMutex(EAL_I2C_DATA *poEalI2cData)
{
    /* Loop through all the unicast I2C instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalI2cData->apoInstData[i]->hI2cMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Set the waiting for RX flag for all the instances of the handle */
ADI_EAL_I2C_CODE
static void SetWaitingForRx(EAL_I2C_DATA *poEalI2cData, bool bWaitingForRx)
{
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalI2cInstData->bWaitingForRx = bWaitingForRx;
    }
}

/*! Wait for response */
ADI_EAL_I2C_CODE
static bool WaitForResponses(EAL_I2C_DATA *poEalI2cData)
{

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    bool bTimedOut = false;

    /* Loop through all instances and check if response is pending */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];

        /* Check if we are still waiting for response for this instance */
        if(prj_osal_waitSemaphore(poEalI2cInstData->hFifoRespSemaphore) != PRJ_OSAL_STATUS_SUCCESS)
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

    /* Wait for I2C write to be received by the host interface and
       send back a No-op transaction with the ID sent downstream */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);
        bAllResponsesReceived = true;

        for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalI2cInstData->bWaitingForRx)
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
ADI_EAL_I2C_CODE
static bool IsResponseExpected(EAL_I2C_INST_DATA *poEalI2cData, ADI_E2BCORE_I2C_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The transaction has read length greater than 0, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            (poTxnCfg->oTxnHdr.nReadLen > 0U) ||
            poEalI2cData->bIsReadTimeTagEn);
}

/*! Returns the number of transactions with response expected */
ADI_EAL_I2C_CODE
static uint16_t GetNumTxnsWithResp(EAL_I2C_INST_DATA *poEalI2cInstData, ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg,
                                   uint16_t nNumI2cTxns)
{
    uint16_t nNumTxnsWithResp = 0U;

    /* Loop through the transaction configs and get the number of transactions for which response is expected */
    for(uint16_t i = 0U; i < nNumI2cTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalI2cInstData, &paoI2cTxnCfg[i]))
        {
            nNumTxnsWithResp++;
        }
    }

    return nNumTxnsWithResp;
}

/** Handle error callback */
ADI_EAL_I2C_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal I2C data */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)pCbParam;

    /* Get the I2C unicast instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalI2cInstData->bIgnoreErr)
    {
        InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_I2C_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal I2C data */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)pCbParam;

    /* Get the I2C unicast instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* If the read is of the I2C STATUS registers */
    if((poMsgOpts->nTopic == ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum) &&
            (poRegCfg->anStartAddr[0U] == I2C_STATS1_NW_ADDR) &&
            (poRegCfg->nLength == 3U))
    {
        /* Process I2C status data */
        ADI_EAL_I2C_STATUS_DATA oI2cStatusData;
        extractI2cStatus(poRegCfg->pBuffer, &oI2cStatusData);
        InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_INTF_STATUS_EVT, &oI2cStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_I2C_CODE
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_I2C_TXN_CONFIG *poI2c = (ADI_E2BCORE_I2C_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal I2C data */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)pCbParam;

    /* Get the I2C unicast instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    /* If the instance is in normal mode */
    if(poEalI2cInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
    {
        /* Increment the read index */
        poEalI2cInstData->nI2cQRdIdx = EAL_INC_Q_IDX(poEalI2cInstData->nI2cQRdIdx, poEalI2cInstData->nI2cCfgQDepth);

        /*******************************
            Cases:
                * Callback indicating I2C write completed (Give callback)
                * Callback indicating I2C write read completed (Give callback)
                * Callback indicating I2C write sync completed (Clear bWaitingForRx flag)
                * Callback indicating I2C write ready sync completed (Clear bWaitingForRx flag)
        *******************************/

        if(poEalI2cInstData->bWaitingForRx)
        {
            poEalI2cInstData->bWaitingForRx = false;

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
            /* Signal the semaphore to indicate response received */
            prj_osal_signalSemaphore(poEalI2cInstData->hFifoRespSemaphore);
#endif
        }
        else if(poI2c->oTxnHdr.nReadLen > 0U)
        {
            InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP, poI2c);
        }
        else
        {
            InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_I2C_WRITE_RESP, poI2c);
        }
    }
    /* If the instance is in periodic mode */
    else if(poEalI2cInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
    {
        InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_PERIODIC_RX, poI2c);

        if(adi_e2bcore_addI2cConfigToQueue(poID, poI2c) != ADI_E2BCORE_ERR_NONE)
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to add i2c config to queue");
        }
    }
    /* If the instance is in custom mode */
    else if(poEalI2cInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
    {
        /* Increment the read index */
        poEalI2cInstData->nI2cQRdIdx = EAL_INC_Q_IDX(poEalI2cInstData->nI2cQRdIdx, poEalI2cInstData->nI2cCfgQDepth);
        InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_I2C_FIFO_READ, poFifoDat);
    }
    else
    {
        /* Nothing to do */
    }
}

/** Handle mailbox acknowledgment callback */
ADI_EAL_I2C_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal I2C data */
    EAL_I2C_DATA *poEalI2cData = (EAL_I2C_DATA *)pCbParam;

    /* Get the I2C unicast instance data */
    EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[0U];

    ADI_EAL_I2C_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd = poAckDat->bFd;
    oAckData.nTopic = poAckDat->nTopic;

    InvokeCbk(poEalI2cData, poEalI2cInstData, ADI_EAL_I2C_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/** Invoke the callback */
ADI_EAL_I2C_CODE
static void InvokeCbk(EAL_I2C_DATA *poEalI2cData, EAL_I2C_INST_DATA *poEalI2cInstData, ADI_EAL_I2C_CBK_EVT eEvt,
                      const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalI2cInstData->pfunCbk != NULL)
    {
        (poEalI2cInstData->pfunCbk)(poEalI2cInstData->oId.nInstNum, poEalI2cData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_I2C_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_I2C_DATA *poEalI2cData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalI2cInstData->nI2cQWrIdx, poEalI2cInstData->nI2cQRdIdx,
                               poEalI2cInstData->nI2cCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_I2C_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_I2C_DATA *poEalI2cData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInI2cQueue(&poEalI2cInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_I2C_CODE
static void CopyTransactionConfigToQueue(EAL_I2C_DATA *poEalI2cData, ADI_E2BCORE_I2C_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_I2C_TXN_CONFIG *poInstTxnCfg = &poEalI2cInstData->paoCfgs[poEalI2cInstData->nI2cQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_I2C_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_I2C_DATA *poEalI2cData, bool bIsMcast, uint8_t **apRxBuf)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalI2cData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_I2C_INST_DATA *poEalI2cInstData = poEalI2cData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_I2C_TXN_CONFIG *poInstTxnCfg = &poEalI2cInstData->paoCfgs[poEalI2cInstData->nI2cQWrIdx];
        poInstTxnCfg->pPayload = apRxBuf != NULL ? apRxBuf[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addI2cConfigToQueue(&poEalI2cInstData->oId, poInstTxnCfg);
            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalI2cInstData->nI2cQWrIdx = EAL_INC_Q_IDX(poEalI2cInstData->nI2cQWrIdx, poEalI2cInstData->nI2cCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_I2C_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_I2C_DATA *poEalI2cData, EAL_I2C_INST_DATA *poEalI2cInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoI2cTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalI2cData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalI2cData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalI2cData, *ppoI2cTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoI2cTxnCfg = &poEalI2cInstData->paoCfgs[poEalI2cInstData->nI2cQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Sends custom I2C transactions */
ADI_EAL_I2C_CODE
static ADI_EAL_STATUS SendCustomI2cTransactions(EAL_I2C_DATA *poEalI2cData, EAL_I2C_INST_DATA *poEalI2cInstData,
        ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg, uint16_t nNumI2cTxns, uint16_t nNumTxnsWithResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalI2cInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalI2cInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalI2cInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalI2cInstData->nI2cQWrIdx,
                                         poEalI2cInstData->nI2cQRdIdx,
                                         poEalI2cInstData->nI2cCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInI2cQueue(&poEalI2cInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalI2cInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bMailbox       = poEalI2cData->bMailboxMode;
        oFifoMsgOpts.bSync          = poEalI2cData->bResyncPending;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        poEalI2cData->bResyncPending = false;
        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add I2C transaction */
        eEalStatus = AddCustomI2cTxnsToMsg(nInstNum, poEalI2cData, poEalI2cInstData, paoI2cTxnCfg, nNumI2cTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the I2C EAL mode to operate in custom mode */
        poEalI2cInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/** Add multiple custom I2C transactions */
ADI_EAL_I2C_CODE
static ADI_EAL_STATUS AddCustomI2cTxnsToMsg(uint32_t nInstNum, EAL_I2C_DATA *poEalI2cData,
        EAL_I2C_INST_DATA *poEalI2cInstData, ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg, uint16_t nNumI2cTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add I2C transaction */
    for(uint16_t i = 0U; i < nNumI2cTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalI2cInstData, &paoI2cTxnCfg[i]))
        {
            /* Since the check has been done above, space has to be available.
               Assert this assumption */
            ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                poEalI2cInstData->nI2cQWrIdx,
                                poEalI2cInstData->nI2cQRdIdx,
                                poEalI2cInstData->nI2cCfgQDepth),
                            "Invalid assumption!");

            /* Copy the given I2C configuration into the config queue pool */
            (void)ADI_MEMCPY(&poEalI2cInstData->paoCfgs[poEalI2cInstData->nI2cQWrIdx], &paoI2cTxnCfg[i],
                             sizeof(ADI_E2BCORE_I2C_TXN_CONFIG));

            /* Add the I2C transaction */
            if(adi_e2bcore_addI2cTxn(nInstNum, &poEalI2cInstData->paoCfgs[poEalI2cInstData->nI2cQWrIdx]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }

            /* Increment queue index */
            poEalI2cInstData->nI2cQWrIdx = EAL_INC_Q_IDX(poEalI2cInstData->nI2cQWrIdx, poEalI2cInstData->nI2cCfgQDepth);
        }
        else
        {
            /* Just add an I2C transaction with the config provided */
            if(adi_e2bcore_addI2cTxn(nInstNum, &paoI2cTxnCfg[i]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}


#endif /* ADI_E2B_IFACE_I2C_ENABLED */
/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
