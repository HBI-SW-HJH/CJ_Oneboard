/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_spi.c
 * @brief: The EAL APIs for SPI interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_SPI_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalSpiData) \
ADI_DBG_REQUIRE((poEalSpiData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalSpiData)->bInUse == true, "Instance is not opened");

/*=================================== DATA ==================================*/

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendSpiTransaction(
    EAL_SPI_DATA *poEalSpiData,
    ADI_E2BCORE_SPI_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod);
static void extractSpiStatus(uint32_t *anData, ADI_EAL_SPI_STATUS_DATA *poStatusData);
static void LockSpiMutex(EAL_SPI_DATA *poEalSpiData);
static void UnlockSpiMutex(EAL_SPI_DATA *poEalSpiData);
static void SetWaitingForRx(EAL_SPI_DATA *poEalSpiData, bool bWaitingForRx);
static bool WaitForResponses(EAL_SPI_DATA *poEalSpiData);
static bool IsResponseExpected(EAL_SPI_INST_DATA *poEalSpiData, ADI_E2BCORE_SPI_TXN_CONFIG *poTxnCfg);
static uint16_t GetNumTxnsWithResp(EAL_SPI_INST_DATA *poEalSpiInstData, ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg,
                                   uint16_t nNumSpiTxns);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void InvokeCbk(EAL_SPI_DATA *poEalSpiData, EAL_SPI_INST_DATA *poEalSpiInstData, ADI_EAL_SPI_CBK_EVT eEvt,
                      const void *pArg);
static bool IsSpaceAvblInEalTxnQueue(EAL_SPI_DATA *poEalSpiData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_SPI_DATA *poEalSpiData);
static void CopyTransactionConfigToQueue(EAL_SPI_DATA *poEalSpiData, ADI_E2BCORE_SPI_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_SPI_DATA *poEalSpiData, bool bIsMcast, uint8_t **apRxBuf);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_SPI_DATA *poEalSpiData, EAL_SPI_INST_DATA *poEalSpiInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoSpiTxnCfg);
static ADI_EAL_STATUS SendCustomSpiTransactions(EAL_SPI_DATA *poEalSpiData, EAL_SPI_INST_DATA *poEalSpiInstData,
        ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg, uint16_t nNumSpiTxns, uint16_t nNumTxnsWithResp);
static ADI_EAL_STATUS AddCustomSpiTxnsToMsg(uint32_t nInstNum, EAL_SPI_DATA *poEalSpiData,
        EAL_SPI_INST_DATA *poEalSpiInstData, ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg, uint16_t nNumSpiTxns);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an SPI instance
 *
 * @param [in]  poSpiCfg          Configuration pointer for EAL SPI instance
 * @param [out] phSpiHandle       Returns the SPI handle for use in other EAL SPI APIs
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: SPI instance was successfully opened.
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiOpen(ADI_EAL_SPI_CFG *poSpiCfg, ADI_EAL_SPI_HANDLE *phSpiHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poSpiCfg != NULL, "Pointer to the SPI module configuration is NULL");
    ADI_DBG_REQUIRE(phSpiHandle != NULL, "Pointer to the SPI handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poSpiCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poSpiCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poSpiCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poSpiCfg->nInstNum;
    uint32_t nRemoteNum = poSpiCfg->nRemoteNum;
    uint32_t nIntfNum = poSpiCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_SPI,
                    "The given pair of instance, remote and interface numbers is not configured as SPI interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poSpiCfg->pEalSpiMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poSpiCfg->nEalSpiMemorySize >= ADI_EAL_SPI_UNICAST_INST_MEM_SIZE(poSpiCfg->nSpiCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poSpiCfg->pEalSpiMemory;

    /* Allocate memory for the SPI transaction config queue. */
    ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfgs = (ADI_E2BCORE_SPI_TXN_CONFIG *)((void *)pMem);
    pMem += (poSpiCfg->nSpiCfgQDepth * sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));

    /* Allocate memory for the EAL SPI instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = (EAL_SPI_INST_DATA *)((void *)pMem);
    pMem += sizeof(EAL_SPI_INST_DATA);
    /* ~~~~~~~~~~~ SPI instance info ~~~~~~~~~~~ */
    poEalSpiInstData->oId.nInstNum      = nInstNum;
    poEalSpiInstData->oId.nRemoteNum    = nRemoteNum;
    poEalSpiInstData->oId.nIntfNum      = nIntfNum;
    poEalSpiInstData->pnIfRegVlanTag    = &poIfConfig->nIfRegVlanTag;
    poEalSpiInstData->pnFifoRegVlanTag  = &poIfConfig->nFifoRegVlanTag;
    poEalSpiInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;
    /* ~~~~~~~~~~~ SPI state info ~~~~~~~~~~~ */
    poEalSpiInstData->bIgnoreErr        = false;
    poEalSpiInstData->eMode             = ADI_EAL_USAGE_MODE_NORMAL;
    poEalSpiInstData->pfunCbk           = NULL; /* Will be filled in adi_eal_spiRegisterCallback */
    poEalSpiInstData->bWaitingForRx     = false;
    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    uint32_t nSpiCtrlReg = 0U;
    EAL_SPI_INST_DATA **apoInstData;
    EAL_SPI_DATA *poEalSpiData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalSpiInstData->oId, SPI_CONTROL_NW_ADDR, &nSpiCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalSpiInstData->bIsReadTimeTagEn  = (nSpiCtrlReg & BITM_SPI_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalSpiInstData->nSpiCfgQDepth     = poSpiCfg->nSpiCfgQDepth;
    poEalSpiInstData->nSpiQWrIdx        = 0U;
    poEalSpiInstData->nSpiQRdIdx        = 0U;
    poEalSpiInstData->paoCfgs           = paoSpiTxnCfgs;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalSpiInstData->oId, (void *)pMem,
            poSpiCfg->nSpiCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_SPI_TXN_CONFIG *) * poSpiCfg->nSpiCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_SPI_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_SPI_INST_DATA *);
        apoInstData[0U] = poEalSpiInstData;

        /* Allocate memory for the internal EAL SPI data structure */
        poEalSpiData  = (EAL_SPI_DATA *)((void *)pMem);
        pMem += sizeof(EAL_SPI_DATA);
        poEalSpiData->bIsMulticast  = false;
        poEalSpiData->bInUse        = true;
        poEalSpiData->nIntfTopic    = 0U;
        poEalSpiData->nFifoTopic    = 0U;
        poEalSpiData->nNumInst      = 1U;
        poEalSpiData->apoInstData   = apoInstData;
        poEalSpiData->bMailboxMode  = false;
        poEalSpiData->bResyncPending     = false;

        /* Initialise the mutex for this SPI instance */
        poEalSpiInstData->oSpiMutexCfg.pMem = &poEalSpiInstData->anSpiMutexBuffer[0U];
        poEalSpiInstData->oSpiMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalSpiInstData->oSpiMutexCfg, &poEalSpiInstData->hSpiMutex);

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        /* Initialise the semaphore for this SPI instance */
        poEalSpiInstData->oFifoRespSemaphoreCfg.pMem = &poEalSpiInstData->anSpiFifoRespSemaphoreBuffer[0U];
        poEalSpiInstData->oFifoRespSemaphoreCfg.nMemSize = PRJ_OSAL_SEMAPHORE_MEM_SIZE;
        poEalSpiInstData->oFifoRespSemaphoreCfg.bCountingSemaphore = false;
        poEalSpiInstData->oFifoRespSemaphoreCfg.nMaxCount = 1U;
        poEalSpiInstData->oFifoRespSemaphoreCfg.nInitialCount = 0U;
        (void)prj_osal_createSemaphore(&poEalSpiInstData->oFifoRespSemaphoreCfg, &poEalSpiInstData->hFifoRespSemaphore);
#endif

        /* Set the callback param in the e2bcore interface config */
        poIfConfig->pCbParam = poEalSpiData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_SPI;

        /* Set the handle to the EAL_SPI_DATA structure */
        *phSpiHandle = (ADI_EAL_SPI_HANDLE)poEalSpiData;
    }

    return eStatus;
}

/**
 * @brief       Closes an opened SPI instance
 * @param [in]  hSpiHandle       The SPI handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: When the SPI device was successfully closed.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiClose(ADI_EAL_SPI_HANDLE hSpiHandle)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalSpiData->apoInstData[i]->hSpiMutex);
#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
        (void)prj_osal_destroySemaphore(poEalSpiData->apoInstData[i]->hFifoRespSemaphore);
#endif
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalSpiData, 0, sizeof(EAL_SPI_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief Allows the application to register callback
 * @param hSpiHandle        The SPI handle
 * @param pfSpiCallback     Pointer to the callback function
 * @return Status
 *         - #ADI_EAL_STATUS:   Returns ADI_EAL_STATUS_SUCCESS if callback was registered.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiRegisterCallback(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_CALLBACK_FUNC pfSpiCallback)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(pfSpiCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalSpiData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* Register the callback function */
    poEalSpiInstData->pfunCbk = pfSpiCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Creates a multicast group of SPI interfaces
 *
 * @param [in]  poSpiMcastCfg       Configuration for SPI multicast group
 * @param [out] phSpiMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiCreateMcastGroup(ADI_EAL_SPI_MCAST_CFG *poSpiMcastCfg,
        ADI_EAL_SPI_HANDLE *phSpiMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poSpiMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phSpiMcastHandle != NULL, "Pointer to the SPI multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poSpiMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poSpiMcastCfg->pahSpiHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poSpiMcastCfg->pSpiMcastMem != NULL, "Pointer to memory for SPI multicast is NULL");
    ADI_DBG_REQUIRE(poSpiMcastCfg->nSpiMcastMemSize >= ADI_EAL_SPI_MCAST_GROUP_MEM_SIZE(poSpiMcastCfg->nNumInst),
                    "Not enough memory allocated for SPI multicast group");
    ADI_DBG_REQUIRE(poSpiMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poSpiMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poSpiMcastCfg->pSpiMcastMem;
    EAL_SPI_INST_DATA **apoInstData = (EAL_SPI_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_SPI_INST_DATA *) * poSpiMcastCfg->nNumInst);

    /* Save the unicast EAL SPI instances */
    for(uint16_t i = 0U; i < poSpiMcastCfg->nNumInst; i++)
    {
        EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)poSpiMcastCfg->pahSpiHandles[i];
        apoInstData[i] = poEalSpiData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast FIFO topic */
        uint32_t nInstNum = poEalSpiData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalSpiData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalSpiData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poSpiMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poSpiMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poSpiMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poSpiMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_SPI_DATA *poEalSpiMcastData = (EAL_SPI_DATA *)((void *)pMem);
    poEalSpiMcastData->bIsMulticast = true;
    poEalSpiMcastData->bInUse       = true;
    poEalSpiMcastData->nIntfTopic   = poSpiMcastCfg->nIntfTopic;
    poEalSpiMcastData->nFifoTopic   = poSpiMcastCfg->nFifoTopic;
    poEalSpiMcastData->nIfRegVlanTag       = (poSpiMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poSpiMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalSpiMcastData->nFifoRegVlanTag     = (poSpiMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poSpiMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalSpiMcastData->nFifoWriteVlanTag   = (poSpiMcastCfg->poFifoWriteVlanTag != NULL) ? eal_utils_convertVlanTags(
                poSpiMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalSpiMcastData->nNumInst     = poSpiMcastCfg->nNumInst;
    poEalSpiMcastData->apoInstData  = apoInstData;
    poEalSpiMcastData->bMailboxMode = false;
    poEalSpiMcastData->bResyncPending    = false;

    /* Set the handle to the EAL_SPI_DATA structure */
    *phSpiMcastHandle = (ADI_EAL_SPI_HANDLE)poEalSpiMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Allows the application to perform an SPI write operation asynchronously
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : SPI write request was sent successfully
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *
 * @details     * The application will be notified (If ID != 0xFF) along with the ID via the
 *                registered callback once the write operation is completed
 *              * This API can be called even when there is another write in progress
 * @note        <b>E2B_ID Usage Pattern (Async API)</b>:
 *              - E2B_ID is <b>optional</b> for asynchronous write operations
 *              - Set nId = 0xFF to disable E2B_ID
 *              - Set nId to any value (0x00-0xFE) to enable E2B_ID tracking
 *              - When enabled, E2B_ID is copied to the upstream acknowledgment message,
 *                allowing correlation between this downstream command and device response
 *              - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWrite(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = 0U;
    oTxnCfg.oTxnHdr.nChipSel          = poSpiData->nChipSel;
    oTxnCfg.pPayload                  = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Send the SPI transaction */
    ADI_EAL_STATUS eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an SPI write operation synchronously
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nId               Identifier for the write request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       : SPI write completed successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL : Buffer not available for this request
 *
 * @details     * This API blocks till the SPI write transaction completes
 *              * Make sure all pending SPI transactions are completed before invoking this API
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
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWriteSync(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = 0U;
    oTxnCfg.oTxnHdr.nChipSel          = poSpiData->nChipSel;
    oTxnCfg.pPayload                  = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalSpiData, true);

    /* Send the SPI transaction */
    eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, NULL, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bool bTimedOut = WaitForResponses(poEalSpiData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalSpiData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an SPI write read operation asynchronously
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nId               Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : SPI write read request was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *
 * @details     * The application will be notified via the registered callback once the
 *                write read operation is completed
 *              * pRxData pointer will be updated with the RX Data and can be read
 *                once the callback is invoked
 * @note        <b>E2B_ID Usage Pattern (Async Write-Read API)</b>:
 *              - E2B_ID is <b>always enabled</b> for write-read operations
 *              - This is <b>required</b> to correlate the combined write-read request with response
 *              - The E2B_ID is included in the downstream message and copied to the upstream
 *                read response, enabling proper command-response correlation
 *              - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *              - Essential for matching responses when multiple write-read operations are pending
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWriteRead(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");
    ADI_DBG_REQUIRE(poSpiData->nReadLength > 0U, "SPI Read length is invalid");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poSpiData->nReadLength;
    oTxnCfg.oTxnHdr.nChipSel          = poSpiData->nChipSel;
    oTxnCfg.pPayload                  = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Send the SPI transaction */
    ADI_EAL_STATUS eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, poSpiData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an SPI write read operation synchronously
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nId               Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       : SPI write read request was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL : Buffer not available for this request
 *
 * @details     * This API blocks till the SPI write read transaction completes
 *              * pRxData pointer will be updated with the RX Data
 *              * Make sure all pending SPI transactions are completed before invoking this API
 * @note        <b>E2B_ID Usage Pattern (Sync Write-Read API)</b>:
 *              - E2B_ID is <b>always enabled</b> for synchronous write-read
 *              - This is <b>intentional and required</b> for proper blocking operation
 *              - The API blocks waiting for the upstream read response with matching E2B_ID
 *              - E2B_ID correlation ensures correct data is received for this specific transaction
 *              - The device uses E2B_ID to acknowledge the execution of the downstream transaction
 *              - The device copies E2B_ID to the response message for tracking
 * @note        This is a synchronous API that internally flushes the transmit buffer and waits for the response before returning.
 *              The response data is available upon successful return (no timeout or error).
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWriteReadSync(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
                                        uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");
    ADI_DBG_REQUIRE(poSpiData->nReadLength > 0U, "SPI Read length is invalid");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poSpiData->nReadLength;
    oTxnCfg.oTxnHdr.nChipSel          = poSpiData->nChipSel;
    oTxnCfg.pPayload                  = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalSpiData, true);

    /* Send the SPI transaction */
    eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, poSpiData->apRxData, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for SPI read response */
        bool bTimedOut = WaitForResponses(poEalSpiData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalSpiData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an SPI write after delay
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nDelay            The delay time in ns
 * @param [in]  nId               Identifier for the write read request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : SPI write read request was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *
 * @details     * This API blocks till the SPI write transaction completes
 *              * Make sure all pending SPI transactions are completed before invoking this API
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWriteWithDelay(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nTimeDelay          = nDelay;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel    = true;
    oTxnCfg.oTxnHdr.nWriteLen           = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = 0U;
    oTxnCfg.oTxnHdr.nChipSel            = poSpiData->nChipSel;
    oTxnCfg.pPayload                    = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Send the SPI transaction */
    ADI_EAL_STATUS eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an SPI write after presentation time
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nTimestamp        The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in]  nId               Identifier for the write read request (Set to 0xFF if no ID is reqd)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : SPI write read request was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @details     * This API blocks till the SPI write transaction completes
 *              * Make sure all pending SPI transactions are completed before invoking this API
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWriteWithPresTs(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent  = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nGptpTime           = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel    = true;
    oTxnCfg.oTxnHdr.nWriteLen           = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = 0U;
    oTxnCfg.oTxnHdr.nChipSel            = poSpiData->nChipSel;
    oTxnCfg.pPayload                    = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Send the SPI transaction */
    ADI_EAL_STATUS eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an SPI write read after delay
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nDelay            The delay time in ns
 * @param [in]  nId               Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : SPI write read request was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @details     * This API blocks till the SPI write transaction completes
 *              * Make sure all pending SPI transactions are completed before invoking this API
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWriteReadWithDelay(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");
    ADI_DBG_REQUIRE(poSpiData->nReadLength > 0U, "SPI Read length is invalid");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = true;
    oTxnCfg.oTxnHdr.nTimeDelay          = nDelay;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel    = true;
    oTxnCfg.oTxnHdr.nWriteLen           = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = poSpiData->nReadLength;
    oTxnCfg.oTxnHdr.nChipSel            = poSpiData->nChipSel;
    oTxnCfg.pPayload                    = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Send the SPI transaction */
    ADI_EAL_STATUS eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, poSpiData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Allows the application to perform an SPI wwrite read after presentation time
 * @param [in]  hSpiHandle        The SPI handle
 * @param [in]  poSpiData         Pointer to SPI I/O DATA
 * @param [in]  nTimestamp        The gPTP time ({uint32_t}sec, {uint32}ns)
 * @param [in]  nId               Identifier for the write read request
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : SPI write read request was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 *
 * @details     * This API blocks till the SPI write transaction completes
 *              * Make sure all pending SPI transactions are completed before invoking this API
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiWriteReadWithPresTs(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");
    ADI_DBG_REQUIRE(poSpiData->nReadLength > 0U, "SPI Read length is invalid");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent  = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked      = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr       = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent     = true;
    oTxnCfg.oTxnHdr.nGptpTime           = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId              = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen   = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen    = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel    = true;
    oTxnCfg.oTxnHdr.nWriteLen           = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen            = poSpiData->nReadLength;
    oTxnCfg.oTxnHdr.nChipSel            = poSpiData->nChipSel;
    oTxnCfg.pPayload                    = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Send the SPI transaction */
    ADI_EAL_STATUS eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, poSpiData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief Allows the application to setup spi periodic write read
 * @param [in] hSpiHandle        The SPI handle
 * @param [in] poSpiData         Pointer to SPI I/O DATA
 * @param [in] nId               Identifier for the write read request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS:   Returns ADI_EAL_STATUS_SUCCESS if SPI setup periodic write read frame was sent successful.
 *             - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *             - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL  : Buffer not available for this request
 * @note       adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *             to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiSetupPeriodicWriteRead(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poSpiData->nReadLength;
    oTxnCfg.oTxnHdr.nChipSel          = poSpiData->nChipSel;
    oTxnCfg.pPayload                  = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalSpiData->bIsMulticast;

    /* Use the locally created SPI transaction by default.
       NOTE: It will be overwritten if the configuration
            should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poSpiTxnCfg = &oTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalSpiInstData, poSpiTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalSpiData, poEalSpiInstData, bIsMcast, &poSpiTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalSpiData, bIsMcast, poSpiData->apRxData) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }

    } while(false);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Switch the mode of all the instances to periodic */
        for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
        {
            EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];
            if(adi_e2bcore_addSpiConfigToQueue(&(poEalSpiInstData->oId), poSpiTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
            poEalSpiInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
        }
    }

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief Allows the application to send spi write frame in multiple transactions
 * @param [in] hSpiHandle        The SPI handle
 * @param [in] poSpiData         Pointer to SPI I/O DATA
 * @param [in] nNumPackTxn       The number of SPI transaction length
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS                       : Returns ADI_EAL_STATUS_SUCCESS if SPI write frame was sent successful.
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Buffer not available for this request
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiMultiWrite(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
                                     uint16_t nNumPackTxn, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poSpiData != NULL, "SPI IO data pointer is NULL");

    /* More SPI IO data checks */
    ADI_DBG_REQUIRE(poSpiData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poSpiData->nWriteLength > 0U, "SPI Write length is invalid");

    /* Set the SPI transaction configuration */
    ADI_E2BCORE_SPI_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nNumPackTxn;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.bOverrideChipSel  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poSpiData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = 0U;
    oTxnCfg.oTxnHdr.nChipSel          = poSpiData->nChipSel;
    oTxnCfg.pPayload                  = poSpiData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockSpiMutex(poEalSpiData);

    /* Send the SPI transaction */
    ADI_EAL_STATUS eEalStatus = SendSpiTransaction(poEalSpiData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief       Gets the SPI interface status
 *
 * @param [in]  hSpiHandle     The SPI interface Handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiReadStatus(ADI_EAL_SPI_HANDLE hSpiHandle)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);

    bool bIsMcast = poEalSpiData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalSpiData->nIfRegVlanTag : *poEalSpiInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalSpiData->nIntfTopic;
        anTopics[1U] = poEalSpiData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalSpiInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalSpiInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalSpiInstData->oId, SPI_STATS_NW_ADDR, 3U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief To write/read to/from interface fifo based on the transaction config
 *
 * @param [in]  hSpiHandle     The SPI interface Handle
 * @param [in]  paoSpiTxnCfg   SPI transaction config
 * @param [in]  nNumSpiTxns    Number of SPI transactions
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : Set / clear successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_FIFO_FAILED           : Failed to add a fifo transaction
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiCustomWriteRead(ADI_EAL_SPI_HANDLE hSpiHandle,
        ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg, uint16_t nNumSpiTxns)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(paoSpiTxnCfg != NULL, "SPI transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumSpiTxns > 0U, "Number of SPI transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalSpiData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* Lock the mutex */
    LockSpiMutex(poEalSpiData);

    /* Get the number of transactions that expects a response */
    uint16_t nNumTxnsWithResp = GetNumTxnsWithResp(poEalSpiInstData, paoSpiTxnCfg, nNumSpiTxns);

    /* Send the custom SPI transactions */
    eEalStatus = SendCustomSpiTransactions(poEalSpiData, poEalSpiInstData, paoSpiTxnCfg, nNumSpiTxns, nNumTxnsWithResp);

    /* Unlock the mutex */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief  Write to interface/fifo register(s)
 *
 * @param [in]    hSpiHandle             The SPI interface Handle
 * @param [in]    bIsFifoRegTransaction  Is this for reg transaction for FIFO
 * @param [inout] poRegCfg               Register read details
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiSendRegTransaction(ADI_EAL_SPI_HANDLE hSpiHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalSpiInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalSpiInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalSpiInstData->oId.nIntfNum;

    bool bIsMcast = poEalSpiData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bMailbox       = poEalSpiData->bMailboxMode;
    oMsgOpts.bSync          = poEalSpiData->bResyncPending;
    oMsgOpts.bTopicOverRide = true;

    poEalSpiData->bResyncPending = false;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalSpiData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalSpiData->nFifoRegVlanTag : *poEalSpiInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalSpiData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalSpiData->nIfRegVlanTag : *poEalSpiInstData->pnIfRegVlanTag;
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
 * @brief       Resets the SPI transaction queue and goes back to a clean state
 *
 * @param  [in] hSpiHandle      The SPI handle
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: SPI reset successful.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiResetQueue(ADI_EAL_SPI_HANDLE hSpiHandle)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalSpiData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* Lock the mutex */
    LockSpiMutex(poEalSpiData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearSpiConfigQueue(&poEalSpiInstData->oId);
        poEalSpiInstData->nSpiQWrIdx = 0U;
        poEalSpiInstData->nSpiQRdIdx = 0U;
        poEalSpiInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
        poEalSpiInstData->bWaitingForRx = false;

        /* Ignore errors */
        poEalSpiInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  SPI_CONTROL_NW_ADDR;
        uint32_t nMask      =  BITM_SPI_CONTROL_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      =  BITM_SPI_CONTROL_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_spiSendRegTransaction((ADI_EAL_SPI_HANDLE)poEalSpiData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an SPI NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalSpiInstData->oId);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for a big timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);

        /* Issue FIFO resets */
        nStartAddr = SPI_CONTROL_NW_ADDR;
        nMask      = BITM_SPI_CONTROL_TX_FIFO_RESET | BITM_SPI_CONTROL_RX_FIFO_RESET;
        nData      = BITM_SPI_CONTROL_TX_FIFO_RESET | BITM_SPI_CONTROL_RX_FIFO_RESET;
        eEalStatus = adi_eal_spiSendRegTransaction((ADI_EAL_SPI_HANDLE)poEalSpiData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalSpiInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockSpiMutex(poEalSpiData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for SPI instance
 *
 * @param [in] hSpiHandle         SPI instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SPI_CODE
void adi_eal_spiSetDownstreamVlan(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                  ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);

    /* Extract the unicast instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* If it is a multicast handle, update the handle structure */
    if(poEalSpiData->bIsMulticast)
    {
        poEalSpiData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalSpiData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalSpiData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalSpiInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalSpiInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalSpiInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for SPI FIFO messages
 *
 * @param [in] hSpiHandle  SPI interface Handle
 * @param [in] poVlanTag   Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn   If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               : API successful
 *              - #ADI_EAL_STATUS_QUEUE_FULL            : EAL queue is full
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL     : E2BCORE buffer not available for this request
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         : Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED          : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       : Failed to add the register map message
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiSetUpstreamVlan(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);

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

    return adi_eal_spiSendRegTransaction(poEalSpiData, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for SPI transactions
 *
 * @param [in]  hSpiHandle      The SPI handle
 * @param [in]  bEnable         Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiSetMailboxMode(ADI_EAL_SPI_HANDLE hSpiHandle, bool bEnable)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);

    /* Set the mailbox mode flag */
    poEalSpiData->bMailboxMode = bEnable;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for SPI transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hSpiHandle      The SPI handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_SPI_CODE
ADI_EAL_STATUS adi_eal_spiResyncSeqNum(ADI_EAL_SPI_HANDLE hSpiHandle)
{
    /* Get the internal data from the handle */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)hSpiHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalSpiData);

    /* Set the sync mode flag */
    poEalSpiData->bResyncPending = true;

    return ADI_EAL_STATUS_SUCCESS;
}

/*============================ CALLBACKS ============================*/
/**
 * @brief The callback for SPI interface
 * @param poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param eType         The type of event this callback is for
 * @param poParam       The data corresponding to this event
 * @param bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param pCbParam      Parameter to be passed back while invoking callback
 */
ADI_EAL_SPI_CODE
void SpiInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                     void *pCbParam)
{
    /* If pCbParam is NULL, the EAL SPI interface instance is not opened,
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
/******************************* STATIC FUNCTIONS *************************************/

/*! Sends the SPI transaction */
ADI_EAL_SPI_CODE
static ADI_EAL_STATUS SendSpiTransaction(
    EAL_SPI_DATA *poEalSpiData,
    ADI_E2BCORE_SPI_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalSpiData->bIsMulticast;

    /* Use the locally created SPI transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poSpiTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalSpiInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalSpiData, poEalSpiInstData, bIsMcast, &poSpiTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalSpiData->nFifoWriteVlanTag : *poEalSpiInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(poEalSpiInstData->oId.nInstNum,
                                 poEalSpiData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalSpiInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag);

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = poEalSpiData->bResyncPending;
        oFifoMsgOpts.bMailbox       = poEalSpiData->bMailboxMode;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalSpiData->bIsMulticast ?
                                      poEalSpiData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalSpiInstData->oId.nIntfNum;

        poEalSpiData->bResyncPending = false;

        if(!bIsPeriodic)
        {
            /* Send the unicast / multicast e2b packet with the FIFO message containing the SPI transaction */
            eEalStatus = eal_utils_sendFifoFrame(&poEalSpiInstData->oId, &oNwFrameCfg, poSpiTxnCfg, &oFifoMsgOpts);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalSpiData, bIsMcast, apRxBuf) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}

/**
 * @brief       Extract SPI status from register data array
 *
 * @param [in]  anData      Register data array
 * @param [out] poStatusData SPI status data
 */
ADI_EAL_SPI_CODE
static void extractSpiStatus(uint32_t *anData, ADI_EAL_SPI_STATUS_DATA *poStatusData)
{
    /* Extract the number of write, read bytes and transactions */
    poStatusData->nNumRdBytes = (uint16_t)((anData[0U] & 0xFFFF0000U) >> 16U);
    poStatusData->nNumSpiTxns = (uint16_t)(anData[0U] & 0xFFFFU);
    poStatusData->nNumWrBytes = anData[1U];
    /* Extract the status flag */
    poStatusData->bBusBusy = ((anData[2U] & 0x1U) == 0U) ? false : true;
}

/*! Lock the mutex of all SPI instances */
ADI_EAL_SPI_CODE
static void LockSpiMutex(EAL_SPI_DATA *poEalSpiData)
{
    /* Loop through all the unicast SPI instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalSpiData->apoInstData[i]->hSpiMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all SPI instances */
ADI_EAL_SPI_CODE
static void UnlockSpiMutex(EAL_SPI_DATA *poEalSpiData)
{
    /* Loop through all the unicast SPI instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalSpiData->apoInstData[i]->hSpiMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Set the waiting for RX flag for all the instances of the handle */
ADI_EAL_SPI_CODE
static void SetWaitingForRx(EAL_SPI_DATA *poEalSpiData, bool bWaitingForRx)
{
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalSpiInstData->bWaitingForRx = bWaitingForRx;
    }
}

/*! Wait for response */
ADI_EAL_SPI_CODE
static bool WaitForResponses(EAL_SPI_DATA *poEalSpiData)
{

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    bool bTimedOut = false;

    /* Loop through all instances and check if response is pending */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];

        /* Check if we are still waiting for response for this instance */
        if(prj_osal_waitSemaphore(poEalSpiInstData->hFifoRespSemaphore) != PRJ_OSAL_STATUS_SUCCESS)
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

    /* Wait for SPI write to be received by the host interface and
       send back a No-op transaction with the ID sent downstream */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);
        bAllResponsesReceived = true;

        for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalSpiInstData->bWaitingForRx)
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
ADI_EAL_SPI_CODE
static bool IsResponseExpected(EAL_SPI_INST_DATA *poEalSpiData, ADI_E2BCORE_SPI_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The transaction has read length greater than 0, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            (poTxnCfg->oTxnHdr.nReadLen > 0U) ||
            poEalSpiData->bIsReadTimeTagEn);
}

/*! Returns the number of transactions with response expected */
ADI_EAL_SPI_CODE
static uint16_t GetNumTxnsWithResp(EAL_SPI_INST_DATA *poEalSpiInstData, ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg,
                                   uint16_t nNumSpiTxns)
{
    uint16_t nNumTxnsWithResp = 0U;

    /* Loop through the transaction configs and get the number of transactions for which response is expected */
    for(uint16_t i = 0U; i < nNumSpiTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalSpiInstData, &paoSpiTxnCfg[i]))
        {
            nNumTxnsWithResp++;
        }
    }

    return nNumTxnsWithResp;
}

/** Handle error callback */
ADI_EAL_SPI_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal SPI data */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)pCbParam;

    /* Get the SPI unicast instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalSpiInstData->bIgnoreErr)
    {
        InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_SPI_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal SPI data */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)pCbParam;

    /* Get the SPI unicast instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* If the read is of the SPI STATUS registers */
    if((poMsgOpts->nTopic == ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum) &&
            (poRegCfg->anStartAddr[0U] == SPI_STATS_NW_ADDR) &&
            (poRegCfg->nLength == 3U))
    {
        /* Process SPI status data */
        ADI_EAL_SPI_STATUS_DATA oSpiStatusData;
        extractSpiStatus(poRegCfg->pBuffer, &oSpiStatusData);
        InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_INTF_STATUS_EVT, &oSpiStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_SPI_CODE
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_SPI_TXN_CONFIG *poSpi = (ADI_E2BCORE_SPI_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal SPI data */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)pCbParam;

    /* Get the SPI unicast instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    /* If the instance is in normal mode */
    if(poEalSpiInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
    {
        /* Increment the read index */
        poEalSpiInstData->nSpiQRdIdx = EAL_INC_Q_IDX(poEalSpiInstData->nSpiQRdIdx, poEalSpiInstData->nSpiCfgQDepth);

        /*******************************
            Cases:
                * Callback indicating SPI write completed (Give callback)
                * Callback indicating SPI write read completed (Give callback)
                * Callback indicating SPI write sync completed (Clear bWaitingForRx flag)
                * Callback indicating SPI write ready sync completed (Clear bWaitingForRx flag)
        *******************************/

        if(poEalSpiInstData->bWaitingForRx)
        {
            poEalSpiInstData->bWaitingForRx = false;

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
            /* Signal the semaphore to indicate response received */
            prj_osal_signalSemaphore(poEalSpiInstData->hFifoRespSemaphore);
#endif
        }
        else if(poSpi->oTxnHdr.nReadLen > 0U)
        {
            InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_SPI_READ_RESP, poSpi);
        }
        else
        {
            InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_SPI_WRITE_RESP, poSpi);
        }
    }
    /* If the instance is in periodic mode */
    else if(poEalSpiInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
    {
        InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_PERIODIC_RX, poSpi);

        if(adi_e2bcore_addSpiConfigToQueue(poID, poSpi) != ADI_E2BCORE_ERR_NONE)
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to add spi config to queue");
        }
    }
    /* If the instance is in custom mode */
    else if(poEalSpiInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
    {
        /* Increment the read index */
        poEalSpiInstData->nSpiQRdIdx = EAL_INC_Q_IDX(poEalSpiInstData->nSpiQRdIdx, poEalSpiInstData->nSpiCfgQDepth);
        InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_SPI_FIFO_READ, poFifoDat);
    }
    else
    {
        /* Nothing to do */
    }
}

/** Handle mailbox acknowledgment callback */
ADI_EAL_SPI_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal SPI data */
    EAL_SPI_DATA *poEalSpiData = (EAL_SPI_DATA *)pCbParam;

    /* Get the SPI unicast instance data */
    EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[0U];

    ADI_EAL_SPI_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    /* Fill in the data */
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd = poAckDat->bFd;
    oAckData.nTopic = poAckDat->nTopic;

    InvokeCbk(poEalSpiData, poEalSpiInstData, ADI_EAL_SPI_CBK_EVT_MAILBOX_ACK, &oAckData);

}

/** Invoke the callback */
ADI_EAL_SPI_CODE
static void InvokeCbk(EAL_SPI_DATA *poEalSpiData, EAL_SPI_INST_DATA *poEalSpiInstData, ADI_EAL_SPI_CBK_EVT eEvt,
                      const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalSpiInstData->pfunCbk != NULL)
    {
        (poEalSpiInstData->pfunCbk)(poEalSpiInstData->oId.nInstNum, poEalSpiData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_SPI_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_SPI_DATA *poEalSpiData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalSpiInstData->nSpiQWrIdx, poEalSpiInstData->nSpiQRdIdx,
                               poEalSpiInstData->nSpiCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_SPI_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_SPI_DATA *poEalSpiData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInSpiQueue(&poEalSpiInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_SPI_CODE
static void CopyTransactionConfigToQueue(EAL_SPI_DATA *poEalSpiData, ADI_E2BCORE_SPI_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_SPI_TXN_CONFIG *poInstTxnCfg = &poEalSpiInstData->paoCfgs[poEalSpiInstData->nSpiQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_SPI_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_SPI_DATA *poEalSpiData, bool bIsMcast, uint8_t **apRxBuf)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalSpiData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_SPI_INST_DATA *poEalSpiInstData = poEalSpiData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_SPI_TXN_CONFIG *poInstTxnCfg = &poEalSpiInstData->paoCfgs[poEalSpiInstData->nSpiQWrIdx];
        poInstTxnCfg->pPayload = apRxBuf != NULL ? apRxBuf[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addSpiConfigToQueue(&poEalSpiInstData->oId, poInstTxnCfg);
            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalSpiInstData->nSpiQWrIdx = EAL_INC_Q_IDX(poEalSpiInstData->nSpiQWrIdx, poEalSpiInstData->nSpiCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_SPI_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_SPI_DATA *poEalSpiData, EAL_SPI_INST_DATA *poEalSpiInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoSpiTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalSpiData))
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalSpiData))
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalSpiData, *ppoSpiTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoSpiTxnCfg = &poEalSpiInstData->paoCfgs[poEalSpiInstData->nSpiQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Sends custom SPI transactions */
ADI_EAL_SPI_CODE
static ADI_EAL_STATUS SendCustomSpiTransactions(EAL_SPI_DATA *poEalSpiData, EAL_SPI_INST_DATA *poEalSpiInstData,
        ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg, uint16_t nNumSpiTxns, uint16_t nNumTxnsWithResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalSpiInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalSpiInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalSpiInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalSpiInstData->nSpiQWrIdx,
                                         poEalSpiInstData->nSpiQRdIdx,
                                         poEalSpiInstData->nSpiCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInSpiQueue(&poEalSpiInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalSpiInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bMailbox       = poEalSpiData->bMailboxMode;
        oFifoMsgOpts.bSync          = poEalSpiData->bResyncPending;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        poEalSpiData->bResyncPending = false;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add SPI transaction */
        eEalStatus = AddCustomSpiTxnsToMsg(nInstNum, poEalSpiData, poEalSpiInstData, paoSpiTxnCfg, nNumSpiTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the SPI EAL mode to operate in custom mode */
        poEalSpiInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/** Add multiple custom SPI transactions */
ADI_EAL_SPI_CODE
static ADI_EAL_STATUS AddCustomSpiTxnsToMsg(uint32_t nInstNum, EAL_SPI_DATA *poEalSpiData,
        EAL_SPI_INST_DATA *poEalSpiInstData, ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg, uint16_t nNumSpiTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add SPI transaction */
    for(uint16_t i = 0U; i < nNumSpiTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalSpiInstData, &paoSpiTxnCfg[i]))
        {
            /* Since the check has been done above, space has to be available.
               Assert this assumption */
            ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                poEalSpiInstData->nSpiQWrIdx,
                                poEalSpiInstData->nSpiQRdIdx,
                                poEalSpiInstData->nSpiCfgQDepth),
                            "Invalid assumption!");

            /* Copy the given SPI configuration into the config queue pool */
            (void)ADI_MEMCPY(&poEalSpiInstData->paoCfgs[poEalSpiInstData->nSpiQWrIdx], &paoSpiTxnCfg[i],
                             sizeof(ADI_E2BCORE_SPI_TXN_CONFIG));

            /* Add the SPI transaction */
            if(adi_e2bcore_addSpiTxn(nInstNum, &poEalSpiInstData->paoCfgs[poEalSpiInstData->nSpiQWrIdx]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }

            /* Increment queue index */
            poEalSpiInstData->nSpiQWrIdx = EAL_INC_Q_IDX(poEalSpiInstData->nSpiQWrIdx, poEalSpiInstData->nSpiCfgQDepth);
        }
        else
        {
            /* Just add an SPI transaction with the config provided */
            if(adi_e2bcore_addSpiTxn(nInstNum, &paoSpiTxnCfg[i]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}


#endif /* ADI_E2B_IFACE_SPI_ENABLED */
/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
