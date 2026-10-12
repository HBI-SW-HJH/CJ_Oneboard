/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_iseled.c
 * @brief: The EAL APIs for ISELED interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_ISELED_ENABLED

/*! \cond private */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalIseledData) \
ADI_DBG_REQUIRE((poEalIseledData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalIseledData)->bInUse == true, "Instance is not opened");

/*=================================== DATA ==================================*/

/*================================ PROTOTYPES ===============================*/
static void LockIseledMutex(EAL_ISELED_DATA *poEalIseledData);
static void UnlockIseledMutex(EAL_ISELED_DATA *poEalIseledData);
static ADI_EAL_STATUS SendIseledTransaction(
    EAL_ISELED_DATA *poEalIseledData,
    ADI_E2BCORE_ISELED_TXN_CONFIG *poTxnCfg,
    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD **apIseledResps);
static bool IsResponseExpected(EAL_ISELED_INST_DATA *poEalIseledData, ADI_E2BCORE_ISELED_TXN_CONFIG *poTxnCfg);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_ISELED_DATA *poEalIseledData,
        EAL_ISELED_INST_DATA *poEalIseledInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoIseledTxnCfg);
static bool IsSpaceAvblInEalTxnQueue(EAL_ISELED_DATA *poEalIseledData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_ISELED_DATA *poEalIseledData);
static void CopyTransactionConfigToQueue(EAL_ISELED_DATA *poEalIseledData, ADI_E2BCORE_ISELED_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_ISELED_DATA *poEalIseledData, bool bIsMcast,
                                  ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD **apIseledResps);
static uint16_t GetNumTxnsWithResp(EAL_ISELED_INST_DATA *poEalIseledInstData,
                                   ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledTxnCfg,
                                   uint16_t nNumIseledTxns);
static ADI_EAL_STATUS SendCustomIseledTransactions(EAL_ISELED_DATA *poEalIseledData,
        EAL_ISELED_INST_DATA *poEalIseledInstData, ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledTxnCfg, uint16_t nNumIseledTxns,
        uint16_t nNumTxnsWithResp);
static ADI_EAL_STATUS AddCustomIseledTxnsToMsg(uint32_t nInstNum, EAL_ISELED_DATA *poEalIseledData,
        EAL_ISELED_INST_DATA *poEalIseledInstData, ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledTxnCfg, uint16_t nNumIseledTxns);
static void extractIseledStatus(uint32_t *anData, ADI_EAL_ISELED_STATUS_DATA *poStatusData);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void InvokeCbk(EAL_ISELED_DATA *poEalIseledData, EAL_ISELED_INST_DATA *poEalIseledInstData,
                      ADI_EAL_ISELED_CBK_EVT eEvt, const void *pArg);

/*! \endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an ISELED instance
 *
 * @param [in]  poIseledCfg         Configuration pointer for EAL ISELED instance
 * @param [out] phIseledHandle      Returns the ISELED handle for use in other EAL ISELED APIs
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: ISELED instance was successfully opened.
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledOpen(ADI_EAL_ISELED_CFG *poIseledCfg, ADI_EAL_ISELED_HANDLE *phIseledHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poIseledCfg != NULL, "Pointer to the ISELED module configuration is NULL");
    ADI_DBG_REQUIRE(phIseledHandle != NULL, "Pointer to the ISELED handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poIseledCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poIseledCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poIseledCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poIseledCfg->nInstNum;
    uint32_t nRemoteNum = poIseledCfg->nRemoteNum;
    uint32_t nIntfNum = poIseledCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_ISELED,
                    "The given pair of instance, remote and interface numbers is not configured as ISELED interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poIseledCfg->pEalIseledMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poIseledCfg->nEalIseledMemorySize >=
                    ADI_EAL_ISELED_UNICAST_INST_MEM_SIZE(poIseledCfg->nIseledCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poIseledCfg->pEalIseledMemory;

    /* Allocate memory for the ISELED transaction config queue. */
    ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledTxnCfgs = (ADI_E2BCORE_ISELED_TXN_CONFIG *)((void *)pMem);
    pMem += (poIseledCfg->nIseledCfgQDepth * sizeof(ADI_E2BCORE_ISELED_TXN_CONFIG));

    /* Allocate memory for the EAL ISELED instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = (EAL_ISELED_INST_DATA *)((void *)pMem);
    pMem += sizeof(EAL_ISELED_INST_DATA);
    /* ~~~~~~~~~~~ ISELED instance info ~~~~~~~~~~~ */
    poEalIseledInstData->oId.nInstNum      = nInstNum;
    poEalIseledInstData->oId.nRemoteNum    = nRemoteNum;
    poEalIseledInstData->oId.nIntfNum      = nIntfNum;
    poEalIseledInstData->pnIfRegVlanTag    = &poIfConfig->nIfRegVlanTag;
    poEalIseledInstData->pnFifoRegVlanTag  = &poIfConfig->nFifoRegVlanTag;
    poEalIseledInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;
    /* ~~~~~~~~~~~ ISELED state info ~~~~~~~~~~~ */
    poEalIseledInstData->bIgnoreErr        = false;
    poEalIseledInstData->eMode             = ADI_EAL_USAGE_MODE_NORMAL;
    poEalIseledInstData->pfunCbk           = NULL; /* Will be filled in adi_eal_iseledRegisterCallback */
    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    uint32_t nIseledCtrlReg = 0U;
    EAL_ISELED_INST_DATA **apoInstData;
    EAL_ISELED_DATA *poEalIseledData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalIseledInstData->oId, ISELED_CONTROL_NW_ADDR, &nIseledCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalIseledInstData->bIsReadTimeTagEn  = (nIseledCtrlReg & BITM_ISELED_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalIseledInstData->nIseledCfgQDepth     = poIseledCfg->nIseledCfgQDepth;
    poEalIseledInstData->nIseledQWrIdx        = 0U;
    poEalIseledInstData->nIseledQRdIdx        = 0U;
    poEalIseledInstData->paoCfgs           = paoIseledTxnCfgs;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalIseledInstData->oId, (void *)pMem,
            poIseledCfg->nIseledCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_ISELED_TXN_CONFIG *) * poIseledCfg->nIseledCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_ISELED_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_ISELED_INST_DATA *);
        apoInstData[0U] = poEalIseledInstData;

        /* Allocate memory for the internal EAL ISELED data structure */
        poEalIseledData  = (EAL_ISELED_DATA *)((void *)pMem);
        pMem += sizeof(EAL_ISELED_DATA);
        poEalIseledData->bIsMulticast  = false;
        poEalIseledData->bInUse        = true;
        poEalIseledData->nIntfTopic    = 0U;
        poEalIseledData->nFifoTopic    = 0U;
        poEalIseledData->nNumInst      = 1U;
        poEalIseledData->apoInstData   = apoInstData;
        poEalIseledData->bMailboxMode  = false;
        poEalIseledData->bResyncPending = false;

        /* Initialise the mutex for this ISELED instance */
        poEalIseledInstData->oIseledMutexCfg.pMem = &poEalIseledInstData->anIseledMutexBuffer[0U];
        poEalIseledInstData->oIseledMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalIseledInstData->oIseledMutexCfg, &poEalIseledInstData->hIseledMutex);

        /* Set the callback param in the e2bcore interface config */
        poIfConfig->pCbParam = poEalIseledData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_ISELED;

        /* Set the handle to the EAL_ISELED_DATA structure */
        *phIseledHandle = (ADI_EAL_ISELED_HANDLE)poEalIseledData;
    }

    return eStatus;
}

/**
 * @brief       Closes an opened ISELED instance
 * @param [in]  hIseledHandle       The ISELED handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: When the ISELED device was successfully closed.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledClose(ADI_EAL_ISELED_HANDLE hIseledHandle)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalIseledData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalIseledData->apoInstData[i]->hIseledMutex);
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalIseledData, 0, sizeof(EAL_ISELED_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Allows the application to register callback
 * @param hIseledHandle        The ISELED handle
 * @param pfIseledCallback     Pointer to the callback function
 * @return ADI_EAL_STATUS   Returns ADI_EAL_STATUS_SUCCESS if callback was registered.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledRegisterCallback(ADI_EAL_ISELED_HANDLE hIseledHandle,
        ADI_EAL_ISELED_CALLBACK_FUNC pfIseledCallback)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);
    ADI_DBG_REQUIRE(pfIseledCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalIseledData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* Register the callback function */
    poEalIseledInstData->pfunCbk = pfIseledCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Creates a multicast group of ISELED interfaces
 *
 * @param [in]  poIseledMcastCfg       Configuration for ISELED multicast group
 * @param [out] phIseledMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        This is a local operation that completes synchronously without network communication.
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledCreateMcastGroup(ADI_EAL_ISELED_MCAST_CFG *poIseledMcastCfg,
        ADI_EAL_ISELED_HANDLE *phIseledMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poIseledMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phIseledMcastHandle != NULL, "Pointer to the ISELED multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poIseledMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poIseledMcastCfg->pahIseledHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poIseledMcastCfg->pIseledMcastMem != NULL, "Pointer to memory for ISELED multicast is NULL");
    ADI_DBG_REQUIRE(poIseledMcastCfg->nIseledMcastMemSize >=
                    ADI_EAL_ISELED_MCAST_GROUP_MEM_SIZE(poIseledMcastCfg->nNumInst),
                    "Not enough memory allocated for ISELED multicast group");
    ADI_DBG_REQUIRE(poIseledMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poIseledMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poIseledMcastCfg->pIseledMcastMem;
    EAL_ISELED_INST_DATA **apoInstData = (EAL_ISELED_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_ISELED_INST_DATA *) * poIseledMcastCfg->nNumInst);

    /* Save the unicast EAL ISELED instances */
    for(uint16_t i = 0U; i < poIseledMcastCfg->nNumInst; i++)
    {
        EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)poIseledMcastCfg->pahIseledHandles[i];
        apoInstData[i] = poEalIseledData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast FIFO topic */
        uint32_t nInstNum = poEalIseledData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalIseledData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalIseledData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poIseledMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poIseledMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poIseledMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poIseledMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_ISELED_DATA *poEalIseledMcastData = (EAL_ISELED_DATA *)((void *)pMem);
    poEalIseledMcastData->bIsMulticast = true;
    poEalIseledMcastData->bInUse       = true;
    poEalIseledMcastData->nIntfTopic   = poIseledMcastCfg->nIntfTopic;
    poEalIseledMcastData->nFifoTopic   = poIseledMcastCfg->nFifoTopic;
    poEalIseledMcastData->nIfRegVlanTag       = (poIseledMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poIseledMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalIseledMcastData->nFifoRegVlanTag     = (poIseledMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poIseledMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalIseledMcastData->nFifoWriteVlanTag   = (poIseledMcastCfg->poFifoWriteVlanTag != NULL) ? eal_utils_convertVlanTags(
                poIseledMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalIseledMcastData->nNumInst     = poIseledMcastCfg->nNumInst;
    poEalIseledMcastData->apoInstData  = apoInstData;
    poEalIseledMcastData->bMailboxMode  = false;
    poEalIseledMcastData->bResyncPending = false;

    /* Set the handle to the EAL_ISELED_DATA structure */
    *phIseledMcastHandle = (ADI_EAL_ISELED_HANDLE)poEalIseledMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to send an ISELED write frame asynchronously
 * @param [in]  hIseledHandle       The ISELED handle
 * @param [in]  poIseledData        ISELED I/O data
 * @param [in]  nId                 Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : ISELED write request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Buffer not available for this request
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the write operation is completed
 *              * This API can be called even when there is another write in progress
 * @note        <b>E2B_ID Usage Pattern (Async API)</b>:
 *              - E2B_ID is <b>optional</b> for asynchronous ISELED write operations
 *              - Set nId = 0xFF to disable E2B_ID
 *              - Set nId to any value (0x00-0xFE) to enable E2B_ID tracking
 *              - When enabled, E2B_ID is copied to the upstream acknowledgment message,
 *                allowing correlation between this downstream command and device response
 *              - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledWrite(ADI_EAL_ISELED_HANDLE hIseledHandle, ADI_EAL_ISELED_IO_DATA *poIseledData,
                                   uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);
    ADI_DBG_REQUIRE(poIseledData != NULL, "ISELED IO data pointer is NULL");

    /* Set the ISELED transaction configuration */
    ADI_E2BCORE_ISELED_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_ISELED_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = true;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nNumPackedTxns      = poIseledData->nNumIseledCmds;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bSetExpectedNumResp = poIseledData->nExpectedNumResp > 0U;
    oTxnCfg.oTxnHdr.bIseledSeqModeEn    = (poIseledData->eMode == ADI_EAL_ISELED_SEQ_MODE) ||
                                          (poIseledData->eMode == ADI_EAL_ISELED_SEQ_PLUS_DIM_MODE);
    oTxnCfg.oTxnHdr.bIseledDimRgbEn     = (poIseledData->eMode == ADI_EAL_ISELED_DIM_RGB_MODE) ||
                                          (poIseledData->eMode == ADI_EAL_ISELED_SEQ_PLUS_DIM_MODE);
    oTxnCfg.oTxnHdr.nExpectedNumResp    = poIseledData->nExpectedNumResp;
    oTxnCfg.pPayload                    = poIseledData->pIseledCmds;

    /* Loop through the mutex of all instances and lock it */
    LockIseledMutex(poEalIseledData);

    /* Send the ISELED transaction */
    ADI_EAL_STATUS eEalStatus = SendIseledTransaction(poEalIseledData, &oTxnCfg, poIseledData->apIseledResps);

    /* Loop through the mutex of all instances and unlock it */
    UnlockIseledMutex(poEalIseledData);

    return eEalStatus;
}

/**
 * @brief       Allows sending custom FIFO commands to ISELED interface
 *
 * @param [in]  hIseledHandle       The ISELED handle
 * @param [in]  paoIseledCfgs       Pointer to array of transaction configs
 * @param [in]  nNumIseledTxns      Size of the above array
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           : Added request to queue successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL        : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_FIFO_FAILED       : Failed to add a fifo transaction
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledCustomWrite(ADI_EAL_ISELED_HANDLE hIseledHandle,
        ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledCfgs, uint16_t nNumIseledTxns)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);
    ADI_DBG_REQUIRE(paoIseledCfgs != NULL, "ISELED transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumIseledTxns > 0U, "Number of ISELED transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalIseledData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* Lock the mutex */
    LockIseledMutex(poEalIseledData);

    /* Get the number of transactions that expects a response */
    uint16_t nNumTxnsWithResp = GetNumTxnsWithResp(poEalIseledInstData, paoIseledCfgs, nNumIseledTxns);

    /* Send the custom ISELED transactions */
    eEalStatus = SendCustomIseledTransactions(poEalIseledData, poEalIseledInstData, paoIseledCfgs, nNumIseledTxns,
                 nNumTxnsWithResp);

    /* Unlock the mutex */
    UnlockIseledMutex(poEalIseledData);

    return eEalStatus;
}

/**
 * @brief       Gets the ISELED interface status
 *
 * @param [in]  hIseledHandle     The ISELED interface Handle
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
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledReadStatus(ADI_EAL_ISELED_HANDLE hIseledHandle)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);

    bool bIsMcast = poEalIseledData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalIseledData->nIfRegVlanTag : *poEalIseledInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalIseledData->nIntfTopic;
        anTopics[1U] = poEalIseledData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalIseledInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalIseledInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalIseledInstData->oId, ISELED_STATS_NW_ADDR, 2U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief  Send a register map transaction to the ISELED interface or the corresponding FIFO registers
 *
 * @param [in]    hIseledHandle          The ISELED interface Handle
 * @param [in]    bIsFifoRegTransaction  Is this for reg transaction for FIFO
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
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledSendRegTransaction(ADI_EAL_ISELED_HANDLE hIseledHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalIseledInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalIseledInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalIseledInstData->oId.nIntfNum;

    bool bIsMcast = poEalIseledData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bMailbox       = poEalIseledData->bMailboxMode;
    oMsgOpts.bSync          = poEalIseledData->bResyncPending;
    oMsgOpts.bTopicOverRide = true;

    poEalIseledData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalIseledData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalIseledData->nFifoRegVlanTag : *poEalIseledInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalIseledData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalIseledData->nIfRegVlanTag : *poEalIseledInstData->pnIfRegVlanTag;
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
 * @brief       Resets the Iseled transaction queue and goes back to a clean state
 *
 * @param  [in] hIseledHandle      The Iseled handle
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Iseled reset successful.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledResetQueue(ADI_EAL_ISELED_HANDLE hIseledHandle)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalIseledData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* Lock the mutex */
    LockIseledMutex(poEalIseledData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearIseledConfigQueue(&poEalIseledInstData->oId);
        poEalIseledInstData->nIseledQWrIdx = 0U;
        poEalIseledInstData->nIseledQRdIdx = 0U;
        poEalIseledInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;

        /* Ignore errors */
        poEalIseledInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  ISELED_CONTROL_NW_ADDR;
        uint32_t nMask      =  BITM_ISELED_CONTROL_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      =  BITM_ISELED_CONTROL_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_iseledSendRegTransaction((ADI_EAL_ISELED_HANDLE)poEalIseledData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an ISELED NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalIseledInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);

        /* Issue FIFO resets */
        nStartAddr = ISELED_CONTROL_NW_ADDR;
        nMask      = BITM_ISELED_CONTROL_TX_FIFO_RESET | BITM_ISELED_CONTROL_RX_FIFO_RESET;
        nData      = BITM_ISELED_CONTROL_TX_FIFO_RESET | BITM_ISELED_CONTROL_RX_FIFO_RESET;
        eEalStatus = adi_eal_iseledSendRegTransaction((ADI_EAL_ISELED_HANDLE)poEalIseledData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalIseledInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockIseledMutex(poEalIseledData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for ISELED instance
 *
 * @param [in] hIseledHandle      ISELED instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ISELED_CODE
void adi_eal_iseledSetDownstreamVlan(ADI_EAL_ISELED_HANDLE hIseledHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                     ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);

    /* Extract the unicast instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* If it is a multicast handle, update the handle structure */
    if(poEalIseledData->bIsMulticast)
    {
        poEalIseledData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalIseledData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalIseledData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalIseledInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalIseledInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalIseledInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for ISELED FIFO messages
 *
 * @param [in] hIseledHandle  ISELED interface Handle
 * @param [in] poVlanTag      Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn      If set true, enables the tagging of VLAN ID in upstream messages
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
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledSetUpstreamVlan(ADI_EAL_ISELED_HANDLE hIseledHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);

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

    return adi_eal_iseledSendRegTransaction(poEalIseledData, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for ISELED transactions
 *
 * @param [in]  hIseledHandle      The ISELED handle
 * @param [in]  bEnable            Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledSetMailboxMode(ADI_EAL_ISELED_HANDLE hIseledHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);

    /* Set the mailbox mode flag */
    poEalIseledData->bMailboxMode = bEnable;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for ISELED transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hIseledHandle      The ISELED handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 */
ADI_EAL_ISELED_CODE
ADI_EAL_STATUS adi_eal_iseledResyncSeqNum(ADI_EAL_ISELED_HANDLE hIseledHandle)
{
    /* Get the internal data from the handle */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)hIseledHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalIseledData);

    /* Set the sync mode flag */
    poEalIseledData->bResyncPending = true;

    return ADI_EAL_STATUS_SUCCESS;
}

/*============================ CALLBACKS ============================*/
/**
 * @brief The callback for ISELED interface
 * @param poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param eType         The type of event this callback is for
 * @param poParam       The data corresponding to this event
 * @param bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param pCbParam      Parameter to be passed back while invoking callback
 */
ADI_EAL_ISELED_CODE
void IseledInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                        void *pCbParam)
{
    /* If pCbParam is NULL, the EAL ISELED interface instance is not opened,
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
/*! Lock the mutex of all ISELED instances */
ADI_EAL_ISELED_CODE
static void LockIseledMutex(EAL_ISELED_DATA *poEalIseledData)
{
    /* Loop through all the unicast ISELED instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalIseledData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalIseledData->apoInstData[i]->hIseledMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all ISELED instances */
ADI_EAL_ISELED_CODE
static void UnlockIseledMutex(EAL_ISELED_DATA *poEalIseledData)
{
    /* Loop through all the unicast ISELED instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalIseledData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalIseledData->apoInstData[i]->hIseledMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Sends the ISELED transaction */
ADI_EAL_ISELED_CODE
static ADI_EAL_STATUS SendIseledTransaction(
    EAL_ISELED_DATA *poEalIseledData,
    ADI_E2BCORE_ISELED_TXN_CONFIG *poTxnCfg,
    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD **apIseledResps)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalIseledData->bIsMulticast;

    /* Use the locally created ISELED transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poIseledTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalIseledInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalIseledData, poEalIseledInstData, bIsMcast, &poIseledTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalIseledData->nFifoWriteVlanTag : *poEalIseledInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(poEalIseledInstData->oId.nInstNum,
                                 poEalIseledData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalIseledInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag
                                );

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalIseledData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalIseledData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalIseledData->bIsMulticast ?
                                      poEalIseledData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalIseledInstData->oId.nIntfNum;

        poEalIseledData->bResyncPending = false;

        /* Send the unicast / multicast e2b packet with the FIFO message containing the ISELED transaction */
        eEalStatus = eal_utils_sendFifoFrame(&poEalIseledInstData->oId, &oNwFrameCfg, poIseledTxnCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalIseledData, bIsMcast, apIseledResps) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/*! Returns if response is expected based on transaction and instance configuration */
ADI_EAL_ISELED_CODE
static bool IsResponseExpected(EAL_ISELED_INST_DATA *poEalIseledData, ADI_E2BCORE_ISELED_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The transaction expects ISELED responses, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent) ||
           (poTxnCfg->oTxnHdr.bSetExpectedNumResp) ||
           (poEalIseledData->bIsReadTimeTagEn);
}

/** Check and add transaction to queues */
ADI_EAL_ISELED_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_ISELED_DATA *poEalIseledData,
        EAL_ISELED_INST_DATA *poEalIseledInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoIseledTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalIseledData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalIseledData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalIseledData, *ppoIseledTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoIseledTxnCfg = &poEalIseledInstData->paoCfgs[poEalIseledInstData->nIseledQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_ISELED_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_ISELED_DATA *poEalIseledData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalIseledData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalIseledInstData->nIseledQWrIdx, poEalIseledInstData->nIseledQRdIdx,
                               poEalIseledInstData->nIseledCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_ISELED_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_ISELED_DATA *poEalIseledData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalIseledData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInIseledQueue(&poEalIseledInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_ISELED_CODE
static void CopyTransactionConfigToQueue(EAL_ISELED_DATA *poEalIseledData, ADI_E2BCORE_ISELED_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalIseledData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_ISELED_TXN_CONFIG *poInstTxnCfg = &poEalIseledInstData->paoCfgs[poEalIseledInstData->nIseledQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_ISELED_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_ISELED_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_ISELED_DATA *poEalIseledData, bool bIsMcast,
                                  ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD **apIseledResps)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalIseledData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_ISELED_TXN_CONFIG *poInstTxnCfg = &poEalIseledInstData->paoCfgs[poEalIseledInstData->nIseledQWrIdx];
        poInstTxnCfg->pPayload = apIseledResps != NULL ? apIseledResps[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addIseledConfigToQueue(&poEalIseledInstData->oId, poInstTxnCfg);
            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalIseledInstData->nIseledQWrIdx = EAL_INC_Q_IDX(poEalIseledInstData->nIseledQWrIdx,
                                             poEalIseledInstData->nIseledCfgQDepth);
    }

    return eError;
}

/*! Returns the number of transactions with response expected */
ADI_EAL_ISELED_CODE
static uint16_t GetNumTxnsWithResp(EAL_ISELED_INST_DATA *poEalIseledInstData,
                                   ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledTxnCfg,
                                   uint16_t nNumIseledTxns)
{
    uint16_t nNumTxnsWithResp = 0U;

    /* Loop through the transaction configs and get the number of transactions for which response is expected */
    for(uint16_t i = 0U; i < nNumIseledTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalIseledInstData, &paoIseledTxnCfg[i]))
        {
            nNumTxnsWithResp++;
        }
    }

    return nNumTxnsWithResp;
}

/** Sends custom ISELED transactions */
ADI_EAL_ISELED_CODE
static ADI_EAL_STATUS SendCustomIseledTransactions(EAL_ISELED_DATA *poEalIseledData,
        EAL_ISELED_INST_DATA *poEalIseledInstData,
        ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledTxnCfg, uint16_t nNumIseledTxns, uint16_t nNumTxnsWithResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalIseledInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalIseledInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalIseledInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalIseledInstData->nIseledQWrIdx,
                                         poEalIseledInstData->nIseledQRdIdx,
                                         poEalIseledInstData->nIseledCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInIseledQueue(&poEalIseledInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalIseledInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalIseledData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalIseledData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        poEalIseledData->bResyncPending = false;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add ISELED transaction */
        eEalStatus = AddCustomIseledTxnsToMsg(nInstNum, poEalIseledData, poEalIseledInstData, paoIseledTxnCfg, nNumIseledTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the ISELED EAL mode to operate in custom mode */
        poEalIseledInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/** Add multiple custom ISELED transactions */
ADI_EAL_ISELED_CODE
static ADI_EAL_STATUS AddCustomIseledTxnsToMsg(uint32_t nInstNum, EAL_ISELED_DATA *poEalIseledData,
        EAL_ISELED_INST_DATA *poEalIseledInstData, ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledTxnCfg, uint16_t nNumIseledTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add ISELED transaction */
    for(uint16_t i = 0U; i < nNumIseledTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalIseledInstData, &paoIseledTxnCfg[i]))
        {
            /* Since the check has been done above, space has to be available.
               Assert this assumption */
            ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                poEalIseledInstData->nIseledQWrIdx,
                                poEalIseledInstData->nIseledQRdIdx,
                                poEalIseledInstData->nIseledCfgQDepth),
                            "Invalid assumption!");

            /* Copy the given ISELED configuration into the config queue pool */
            (void)ADI_MEMCPY(&poEalIseledInstData->paoCfgs[poEalIseledInstData->nIseledQWrIdx], &paoIseledTxnCfg[i],
                             sizeof(ADI_E2BCORE_ISELED_TXN_CONFIG));

            /* Add the ISELED transaction */
            if(adi_e2bcore_addIseledTxn(nInstNum,
                                        &poEalIseledInstData->paoCfgs[poEalIseledInstData->nIseledQWrIdx]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }

            /* Increment queue index */
            poEalIseledInstData->nIseledQWrIdx = EAL_INC_Q_IDX(poEalIseledInstData->nIseledQWrIdx,
                                                 poEalIseledInstData->nIseledCfgQDepth);
        }
        else
        {
            /* Just add an ISELED transaction with the config provided */
            if(adi_e2bcore_addIseledTxn(nInstNum, &paoIseledTxnCfg[i]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}

/**
 * @brief       Extract ISELED status from register data array
 *
 * @param [in]  anData      Register data array
 * @param [out] poStatusData ISELED status data
 */
ADI_EAL_ISELED_CODE
static void extractIseledStatus(uint32_t *anData, ADI_EAL_ISELED_STATUS_DATA *poStatusData)
{
    /* Extract the number of write and read commands */
    poStatusData->nNumWrPackets           = (uint16_t)((anData[0U] & 0xFFFF0000U) >> 16U);
    poStatusData->nNumRdPackets           = (uint16_t)((anData[0U] & 0xFFFFU));
    /* Extract the status flags */
    poStatusData->bBusBusy                = ((anData[1U] & 0x01U) == 0U) ? false : true;
    poStatusData->bContention             = ((anData[1U] & 0x02U) == 0U) ? false : true;
    poStatusData->bRcvFrameEOCError       = ((anData[1U] & 0x04U) == 0U) ? false : true;
    poStatusData->bRcvFrameCRCError       = ((anData[1U] & 0x08U) == 0U) ? false : true;
    poStatusData->bRcvFrameFreqSyncError  = ((anData[1U] & 0x16U) == 0U) ? false : true;
    poStatusData->bRcvFrameFrameSyncError = ((anData[1U] & 0x32U) == 0U) ? false : true;
}

/** Handle error callback */
ADI_EAL_ISELED_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal ISELED data */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)pCbParam;

    /* Get the ISELED unicast instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalIseledInstData->bIgnoreErr)
    {
        InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_ISELED_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal ISELED data */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)pCbParam;

    /* Get the ISELED unicast instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* If the read is of the ISELED STATUS registers */
    if((poMsgOpts->nTopic == ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum) &&
            (poRegCfg->anStartAddr[0U] == ISELED_STATS_NW_ADDR) &&
            (poRegCfg->nLength == 2U))
    {
        /* Process ISELED status data */
        ADI_EAL_ISELED_STATUS_DATA oIseledStatusData;
        extractIseledStatus(poRegCfg->pBuffer, &oIseledStatusData);
        InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_INTF_STATUS_EVT, &oIseledStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_ISELED_CODE
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_ISELED_TXN_CONFIG *poIseled = (ADI_E2BCORE_ISELED_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal ISELED data */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)pCbParam;

    /* Get the ISELED unicast instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    /* If the instance is in normal mode */
    if(poEalIseledInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
    {
        /* Increment the read index */
        poEalIseledInstData->nIseledQRdIdx = EAL_INC_Q_IDX(poEalIseledInstData->nIseledQRdIdx,
                                             poEalIseledInstData->nIseledCfgQDepth);

        /* ISELED responses are received */
        if(poIseled->oTxnHdr.nExpectedNumResp > 0U)
        {
            InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_ISELED_READ_RESP, poIseled);
        }
        /* ISELED write completed */
        else
        {
            InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_ISELED_WRITE_RESP, poIseled);
        }
    }
    /* If the instance is in custom mode */
    else if(poEalIseledInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
    {
        /* Increment the read index */
        poEalIseledInstData->nIseledQRdIdx = EAL_INC_Q_IDX(poEalIseledInstData->nIseledQRdIdx,
                                             poEalIseledInstData->nIseledCfgQDepth);
        InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_ISELED_FIFO_READ, poFifoDat);
    }
    else
    {
        /* Nothing to do */
    }
}

/** Handle mailbox acknowledgment callback */
ADI_EAL_ISELED_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal ISELED data */
    EAL_ISELED_DATA *poEalIseledData = (EAL_ISELED_DATA *)pCbParam;

    /* Get the ISELED unicast instance data */
    EAL_ISELED_INST_DATA *poEalIseledInstData = poEalIseledData->apoInstData[0U];

    ADI_EAL_ISELED_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd = poAckDat->bFd;
    oAckData.nTopic = poAckDat->nTopic;

    InvokeCbk(poEalIseledData, poEalIseledInstData, ADI_EAL_ISELED_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/** Invoke the callback */
ADI_EAL_ISELED_CODE
static void InvokeCbk(EAL_ISELED_DATA *poEalIseledData, EAL_ISELED_INST_DATA *poEalIseledInstData,
                      ADI_EAL_ISELED_CBK_EVT eEvt, const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalIseledInstData->pfunCbk != NULL)
    {
        (poEalIseledInstData->pfunCbk)(poEalIseledInstData->oId.nInstNum, poEalIseledData, eEvt, pArg);
    }
}


#endif /* ADI_E2B_IFACE_ISELED_ENABLED */
/*! \endcond */

/** @} */
/*
 * EOF: www.analog.com
*/
