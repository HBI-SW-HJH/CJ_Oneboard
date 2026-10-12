/*******************************************************************************
Copyright (c) 2024-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: adi_eal_fioOSP.c
 * @brief: The EAL APIs for FIO-OSP interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_FIO_OSP_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalFioOspData) \
ADI_DBG_REQUIRE((poEalFioOspData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalFioOspData)->bInUse == true, "Instance is not opened");

/*================================ DATA TYPES ===============================*/
/*=================================== DATA ==================================*/
/* Instruction memory contents for configuring FIO as OSP physical layer */
static uint32_t ganInstructionMemData[] =
{
    0xcc00ce00u,
    0x08807460u,
    0xc400c020u,
    0xd3002200u,
    0xe100cbe0u,
    0xc9e0cc00u,
    0xce0031a4u,
    0x73c001c0u,
    0xcbe0c9e0u,
    0xcfe0cde0u,
    0x13604280u,
    0xe100a240u,
    0xcde0cfe0u,
    0x26003a80u,
    0xe1000f80u,
    0xc420e100u,
    0xa2407460u,
    0x0420e220u,
    0x0d40c004u,
    0xc0203060u,
    0x83402880u,
    0xc024c000u,
    0x30608340u,
    0x08802e20u,
    0x2f204e80u,
    0x3e200f80u,
    0xd8203600u,
    0xcde0cfe0u,
    0x01a01f80u,
    0x4f202de0u,
    0xda243780u,
    0xf0000080u
};

/*================================ PROTOTYPES ===============================*/
static ADI_EAL_STATUS SendFioOspTransaction(
    EAL_FIO_OSP_DATA *poEalFioOspData,
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod);
static void LockFioOspMutex(EAL_FIO_OSP_DATA *poEalFioOspData);
static void UnlockFioOspMutex(EAL_FIO_OSP_DATA *poEalFioOspData);
static void SetWaitingForRx(EAL_FIO_OSP_DATA *poEalFioOspData, bool bWaitingForRx);
static bool WaitForResponses(EAL_FIO_OSP_DATA *poEalFioOspData);
static bool IsResponseExpected(EAL_FIO_OSP_INST_DATA *poEalFioOspData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg);
static uint16_t GetNumTxnsWithResp(EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
                                   ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg,
                                   uint16_t nNumFioOspTxns);

static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void InvokeCbk(EAL_FIO_OSP_DATA *poEalFioOspData, EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
                      ADI_EAL_FIO_OSP_CBK_EVT eEvt, const void *pArg);
static void ExtractFioOspStatus(uint32_t *panData, ADI_EAL_FIO_STATUS_DATA *poStatusData);

static bool IsSpaceAvblInEalTxnQueue(EAL_FIO_OSP_DATA *poEalFioOspData);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_FIO_OSP_DATA *poEalFioOspData);
static void CopyTransactionConfigToQueue(EAL_FIO_OSP_DATA *poEalFioOspData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_FIO_OSP_DATA *poEalFioOspData, bool bIsMcast, uint8_t **apRxBuf);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_FIO_OSP_DATA *poEalFioOspData,
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioOspTxnCfg);

static ADI_EAL_STATUS SendCustomFioOspTransactions(EAL_FIO_OSP_DATA *poEalFioOspData,
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg, uint16_t nNumFioOspTxns, uint16_t nNumTxnsWithResp);
static ADI_EAL_STATUS AddCustomFioOspTxnsToMsg(uint32_t nInstNum, EAL_FIO_OSP_DATA *poEalFioOspData,
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg,
        uint16_t nNumFioOspTxns);
static bool eal_notify_FioOspInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
                                       ADI_EAL_INTERFACE_INTERRUPT_CBK_EVT eFioOspEvt);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an instance of FIO-OSP interface
 *
 * @param [in]  poFioOspCfg      Configurations for this FIO-OSP instance
 * @param [out] phFioOspHandle   Handle corresponding to this FIO-OSP instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-OSP instance successfully opened
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */

ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspOpen(ADI_EAL_FIO_OSP_CFG *poFioOspCfg, ADI_EAL_FIO_OSP_HANDLE *phFioOspHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poFioOspCfg != NULL, "Pointer to the FIO-Osp module configuration is NULL");
    ADI_DBG_REQUIRE(phFioOspHandle != NULL, "Pointer to the FIO-Osp handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poFioOspCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poFioOspCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poFioOspCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poFioOspCfg->nInstNum;
    uint32_t nRemoteNum = poFioOspCfg->nRemoteNum;
    uint32_t nIntfNum = poFioOspCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_FIO,
                    "The given pair of instance, remote and interface numbers is not configured as FIO interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poFioOspCfg->pFioOspMemory != NULL, "Pointer to allocated memory is NULL");
    ADI_DBG_REQUIRE(poFioOspCfg->nFioOspMemorySize >=
                    ADI_EAL_FIO_OSP_UNICAST_INST_MEM_SIZE(poFioOspCfg->nFioOspCfgQDepth),
                    "Allocated memory is less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poFioOspCfg->pFioOspMemory;

    /* Allocate memory for the FIO OSP transaction config queue. */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfgs = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)((void *)pMem);
    ADI_DBG_REQUIRE(((uintptr_t)pMem % 4U) == 0U, "Memory is not 4-byte aligned");
    pMem += (poFioOspCfg->nFioOspCfgQDepth * sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

    /* Allocate memory for the EAL FIO_OSP instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = (EAL_FIO_OSP_INST_DATA *)((void *)pMem);
    (void)ADI_MEMSET(poEalFioOspInstData, 0, sizeof(EAL_FIO_OSP_INST_DATA));
    pMem += sizeof(EAL_FIO_OSP_INST_DATA);

    /* Instance info */
    poEalFioOspInstData->oId.nInstNum = poFioOspCfg->nInstNum;
    poEalFioOspInstData->oId.nRemoteNum = poFioOspCfg->nRemoteNum;
    poEalFioOspInstData->oId.nIntfNum = poFioOspCfg->nIntfNum;
    poEalFioOspInstData->pnIfRegVlanTag = &poIfConfig->nIfRegVlanTag;
    poEalFioOspInstData->pnFifoRegVlanTag = &poIfConfig->nFifoRegVlanTag;
    poEalFioOspInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;

    /* State info */
    poEalFioOspInstData->bIgnoreErr = false;
    poEalFioOspInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
    poEalFioOspInstData->pfunCbk = NULL;
    poEalFioOspInstData->bWaitingForRx = false;

    /* Transaction config queue info */
    uint32_t nFlexioCtrlReg = 0U;
    EAL_FIO_OSP_INST_DATA **apoInstData;
    EAL_FIO_OSP_DATA *poEalFioOspData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;

    if(eal_utils_getConfigInfRegVal(&poEalFioOspInstData->oId, FLEXIO_CONTROL_NW_ADDR, &nFlexioCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }

    poEalFioOspInstData->bIsReadTimeTagEn = (nFlexioCtrlReg & BITM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalFioOspInstData->nFioOspCfgQDepth = poFioOspCfg->nFioOspCfgQDepth;
    poEalFioOspInstData->nFioOspQWrIdx = 0U;
    poEalFioOspInstData->nFioOspQRdIdx = 0U;
    poEalFioOspInstData->paoCfgs = paoFioOspTxnCfgs;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalFioOspInstData->oId, (void *)pMem,
                    poFioOspCfg->nFioOspCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }

    pMem += (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *) * poFioOspCfg->nFioOspCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_FIO_OSP_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_FIO_OSP_INST_DATA *);
        apoInstData[0U] = poEalFioOspInstData;

        /* Allocate memory for the internal EAL FIO_Osp data structure */
        poEalFioOspData = (EAL_FIO_OSP_DATA *)((void *)pMem);
        pMem += sizeof(EAL_FIO_OSP_DATA);
        poEalFioOspData->bIsMulticast  = false;
        poEalFioOspData->bInUse        = true;
        poEalFioOspData->nIntfTopic    = 0U;
        poEalFioOspData->nFifoTopic    = 0U;
        poEalFioOspData->nNumInst      = 1U;
        poEalFioOspData->apoInstData   = apoInstData;

        /* Mutex config */
        poEalFioOspInstData->oFioOspMutexCfg.pMem = &poEalFioOspInstData->anFioOspMutexBuffer[0U];
        poEalFioOspInstData->oFioOspMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalFioOspInstData->oFioOspMutexCfg, &poEalFioOspInstData->hFioOspMutex);

        /* Set the callback param in the e2bcore config */
        poIfConfig->pCbParam = poEalFioOspData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_FIO_OSP;

        adi_eal_registerInterfaceInterruptCbk(nInstNum, nRemoteNum, nIntfNum,
                                              eal_notify_FioOspInterrupt);

        /* Return the internal FIO-OSP instance data as
           the handle for use in other functions */
        *phFioOspHandle = (ADI_EAL_FIO_OSP_HANDLE)poEalFioOspData;
    }

    return eStatus;
}

/**
 * @brief       Closes the opened FIO-OSP interface instance
 *
 * @param [in]  hFioOspHandle   Handle conrresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-OSP instance successfully closed
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */

ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspClose(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle)
{
    /* Extract the FIO-OSP data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalFioOspData->apoInstData[i]->hFioOspMutex);
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalFioOspData, 0, sizeof(EAL_FIO_OSP_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Register a callback function for getting notified of events
 *
 * @param [in]  hFioOspHandle       Handle corresponding to this instance
 * @param [in]  pfFioOspCallback    Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Callback registered successfully
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */

ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspRegisterCallback(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_CALLBACK_FUNC pfFioOspCallback)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(pfFioOspCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalFioOspData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* Register the callback function */
    poEalFioOspInstData->pfunCbk = pfFioOspCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Creates a multicast group of FIO_OSP interfaces
 *
 * @param [in]  poFioOspMcastCfg       Configuration for FIO_OSP multicast group
 * @param [out] phFioOspMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspCreateMcastGroup(ADI_EAL_FIO_OSP_MCAST_CFG *poFioOspMcastCfg,
        ADI_EAL_FIO_OSP_HANDLE *phFioOspMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poFioOspMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phFioOspMcastHandle != NULL, "Pointer to the FIO_OSP multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poFioOspMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poFioOspMcastCfg->pahFioOspHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poFioOspMcastCfg->pFioOspMcastMem != NULL, "Pointer to memory for FIO_OSP multicast is NULL");
    ADI_DBG_REQUIRE(poFioOspMcastCfg->nFioOspMcastMemSize >= ADI_EAL_FIO_OSP_MCAST_GROUP_MEM_SIZE(
                        poFioOspMcastCfg->nNumInst),
                    "Not enough memory allocated for FIO_OSP multicast group");
    ADI_DBG_REQUIRE(poFioOspMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poFioOspMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poFioOspMcastCfg->pFioOspMcastMem;
    EAL_FIO_OSP_INST_DATA **apoInstData = (EAL_FIO_OSP_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_FIO_OSP_INST_DATA *) * poFioOspMcastCfg->nNumInst);

    /* Save the unicast EAL FIO_OSP instances */
    for(uint16_t i = 0U; i < poFioOspMcastCfg->nNumInst; i++)
    {
        EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)poFioOspMcastCfg->pahFioOspHandles[i];
        apoInstData[i] = poEalFioOspData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast topics */
        uint32_t nInstNum = poEalFioOspData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioOspData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalFioOspData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poFioOspMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poFioOspMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poFioOspMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poFioOspMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_FIO_OSP_DATA *poEalFioOspMcastData = (EAL_FIO_OSP_DATA *)((void *)pMem);
    poEalFioOspMcastData->bIsMulticast = true;
    poEalFioOspMcastData->bInUse       = true;
    poEalFioOspMcastData->nIntfTopic   = poFioOspMcastCfg->nIntfTopic;
    poEalFioOspMcastData->nFifoTopic   = poFioOspMcastCfg->nFifoTopic;
    poEalFioOspMcastData->nIfRegVlanTag       = (poFioOspMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poFioOspMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioOspMcastData->nFifoRegVlanTag     = (poFioOspMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poFioOspMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioOspMcastData->nFifoWriteVlanTag   = (poFioOspMcastCfg->poFifoWriteVlanTag != NULL) ?
            eal_utils_convertVlanTags(poFioOspMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioOspMcastData->nNumInst     = poFioOspMcastCfg->nNumInst;
    poEalFioOspMcastData->apoInstData  = apoInstData;

    /* Set the handle to the EAL_FIO_OSP_DATA structure */
    *phFioOspMcastHandle = (ADI_EAL_FIO_OSP_HANDLE)poEalFioOspMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Update the Flexible IO OSP instruction RAM
 *
 * @param [in] hFioOspHandle  Handle corresponding to this FIO-OSP instance
 * @param [in] eFioOspClkFreq Clock frequency to be programmed
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Updated instruction ram successfully
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */

ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspConfigureInstructionRAM(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_CLK_FREQ eFioOspClkFreq)
{
    /* Extract the FIO-OSP data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioOspData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Extract the FIO-OSP instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* Instruction RAM data */
    ADI_EAL_FIO_INST_RAM_DATA oFioInstRamData;
    (void)ADI_MEMSET(&oFioInstRamData, 0, sizeof(oFioInstRamData));

    oFioInstRamData.panInstructionMemData = &ganInstructionMemData[0U];
    oFioInstRamData.nNumInstMemEntries =
        (uint16_t)(sizeof(ganInstructionMemData) / sizeof(ganInstructionMemData[0U]));

    ADI_DBG_REQUIRE(oFioInstRamData.nNumInstMemEntries <= 127U, "Instruction count exceeds uint8_t range");

    switch(eFioOspClkFreq)
    {
        case ADI_EAL_FIO_OSP_CLK_2_0_MHZ:
        {
            oFioInstRamData.aoSmCfgs[0U].nClkDiv = 5u;
            break;
        }

        case ADI_EAL_FIO_OSP_CLK_2_4_MHZ:
        {
            oFioInstRamData.aoSmCfgs[0U].nClkDiv = 4u;
            break;
        }

        default:
        {
            oFioInstRamData.aoSmCfgs[0U].nClkDiv = 4u;
            break;
        }
    };

    /* SM routing is fixed to SM0 */
    oFioInstRamData.bFifoTxRouterFixedSm = true;

    /* Number of active SMs (Only 1 - used for both TX and RX) */
    oFioInstRamData.nNumActiveStateMachines = 1U;

    /* State machine 0 configuration */
    oFioInstRamData.aoSmCfgs[0U].bDelaySidesetSingleMode = true;

    oFioInstRamData.aoSmCfgs[0U].bEnableAutopush = true;

    oFioInstRamData.aoSmCfgs[0U].bEnableAutopull = true;

    oFioInstRamData.aoSmCfgs[0U].bOsrShiftRight  = false;

    oFioInstRamData.aoSmCfgs[0U].bIsrShiftRight  = false;

    oFioInstRamData.aoSmCfgs[0U].nPinOutBasepin  = 0U;

    oFioInstRamData.aoSmCfgs[0U].nPinInBasepin   = 0U;

    oFioInstRamData.aoSmCfgs[0U].nSidesetBasepin = 0U;

    oFioInstRamData.aoSmCfgs[0U].nJmpBasepin     = 0U;

    oFioInstRamData.aoSmCfgs[0U].nProgramStart   = 0U;

    oFioInstRamData.aoSmCfgs[0U].nProgramSize    = (uint8_t)(oFioInstRamData.nNumInstMemEntries * 2U);

    oFioInstRamData.aoSmCfgs[0U].nDelaySidesetBits  = 0U;

    oFioInstRamData.aoSmCfgs[0U].nAutopullThreshold = 7U;

    oFioInstRamData.aoSmCfgs[0U].nAutopushThreshold = 8U;

    oFioInstRamData.aoSmCfgs[0U].nPullThreshold     = 0U;

    oFioInstRamData.aoSmCfgs[0U].nPushThreshold     = 0U;

    /* Configure the Flexible IO registers */
    return eal_utils_configureFlexibleIO(&poEalFioOspInstData->oId, &oFioInstRamData);
}

/**
 * @brief       Writes to the Flexible IO OSP interface
 *
 * @param [in]  hFioOspHandle   Handle corresponding to this instance
 * @param [in]  poFioOspIoData  Pointer to the data to be written
 * @param [in]  nId             E2B ID to be used for this transaction (set this to 0xFF if not required)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Write successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWrite(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
                                   ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData != NULL, "FIO_OSP IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = 0U;

    /* create payload with tx length and expected rx length added */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = 0u;
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, NULL, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief       Writes to the Flexible IO OSP interface synchronously
 *
 * @param [in]  hFioOspHandle   Handle corresponding to this instance
 * @param [in]  poFioOspIoData  Pointer to the data to be written
 * @param [in]  nId             E2B ID to be used for this transaction
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Write successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT : Timeout error
 *
 * @note        This is a synchronous API that internally flushes the transmit buffer before returning.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWriteSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
                                       ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId)
{
    ADI_EAL_STATUS eEalStatus;

    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData != NULL, "FIO_OSP IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;

    /* create payload with tx length and expected rx length added */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = 0u;
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalFioOspData, true);

    /* Send the FIO_OSP transaction */
    eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, NULL, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bool bTimedOut = WaitForResponses(poEalFioOspData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalFioOspData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief       WriteRead to the Flexible IO OSP interface
 *
 * @param [in]  hFioOspHandle   Handle corresponding to this instance
 * @param [in]  poFioOspIoData  Pointer to the data to be written
 * @param [in]  nId             E2B ID to be used for this transaction (set this to 0xFF if not required)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Write successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
                                       ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");
    ADI_DBG_REQUIRE((poFioOspIoData->nReadLength > 0U)
                    && (poFioOspIoData->nReadLength < 32u), "OSP Read length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = poFioOspIoData->nReadLength;

    /* create payload with tx length and expected rx length added */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = (uint8_t)(poFioOspIoData->nReadLength * 8u);
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief       WriteRead to the Flexible IO OSP interface synchronously
 *
 * @param [in]  hFioOspHandle   Handle corresponding to this instance
 * @param [in]  poFioOspIoData  Pointer to the data to be written
 * @param [in]  nId             E2B ID to be used for this transaction
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Write successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Failed to add the register map message
 *              - #ADI_EAL_STATUS_RX_TIMEOUT : Timeout error
 *
 * @note        This is a synchronous API that internally flushes the transmit buffer and waits for the response before returning.
 *              The response data is available upon successful return (no timeout or error).
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWriteReadSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");
    ADI_DBG_REQUIRE((poFioOspIoData->nReadLength > 0U)
                    && (poFioOspIoData->nReadLength < 32u), "OSP Read length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = poFioOspIoData->nReadLength;

    /* create rx length prepended payload */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = (uint8_t)(poFioOspIoData->nReadLength * 8u);
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalFioOspData, true);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bool bTimedOut = WaitForResponses(poEalFioOspData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalFioOspData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO OSP write operation after delay
 *
 * @param [in] hFioOspHandle   Handle corresponding to this instance
 * @param [in] poFioOspIoData  Pointer to FIO OSP data
 * @param [in] nDelay          Delay time in ns
 * @param [in] nId             Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *              - #ADI_EAL_STATUS                  : Frame was sent successfully.
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL    : Buffer not available for this request
 *              - #ADI_EAL_STATUS_START_FAILED     : Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED  : Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWriteWithDelay(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData != NULL, "FIO_OSP IO data pointer is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nTimeDelay        = nDelay;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = 0u;

    /* create rx length prepended payload */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = 0u;
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO OSP write read operation after delay
 *
 * @param [in] hFioOspHandle   Handle corresponding to this instance
 * @param [in] poFioOspIoData    Pointer to FIO OSP data
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
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWriteReadWithDelay(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint32_t nDelay, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");
    ADI_DBG_REQUIRE((poFioOspIoData->nReadLength > 0U)
                    && (poFioOspIoData->nReadLength < 32u), "OSP Read length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsTimeDelayPresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nTimeDelay        = nDelay;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = poFioOspIoData->nReadLength;

    /* Create RX length prepended payload */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = (uint8_t)(poFioOspIoData->nReadLength * 8u);
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO OSP write operation after presentation time
 *
 * @param [in] hFioOspHandle   Handle corresponding to this instance
 * @param [in] poFioOspIoData    Pointer to FIO OSP data
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
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWriteWithPresTs(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");

    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nGptpTime         = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = 0U;

    /* Create Rx length prepended payload*/
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = 0u;
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Allows the application to do a FIO OSP write read operation after presentation time
 *
 * @param [in] hFioOspHandle   Handle corresponding to this instance
 * @param [in] poFioOspIoData    Pointer to FIO OSP data
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
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspWriteReadWithPresTs(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint64_t nTimestamp, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");
    ADI_DBG_REQUIRE((poFioOspIoData->nReadLength > 0U)
                    && (poFioOspIoData->nReadLength < 32u), "OSP Read length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bIsGptpTimePresent = true;
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = (nId != 0xFFU);
    oTxnCfg.oTxnHdr.nGptpTime         = nTimestamp;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = poFioOspIoData->nReadLength;

    /* Create Rx length prepended payload */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = (uint8_t)(poFioOspIoData->nReadLength * 8u);
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Allows the application to setup FIO OSP periodic write read
 *
 * @param [in] hFioOspHandle      Handle corresponding to this instance
 * @param [in] poFioOspIoData       Pointer to FIO OSP data
 * @param [in] nId                Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS                   : Returns ADI_EAL_STATUS_SUCCESS if FIO OSP setup periodic write read frame was sent successful.
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL     : Buffer not available for this request
 *             - #ADI_EAL_STATUS_START_FAILED      : Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_LATCH_FAILED : Failed to set FIFO in latching mode
 *             - #ADI_EAL_STATUS_FIFO_FAILED       : Failed to add FIFO transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspSetupPeriodicWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");
    ADI_DBG_REQUIRE((poFioOspIoData->nReadLength > 0U)
                    && (poFioOspIoData->nReadLength < 32u), "OSP Read length is invalid");

    /* Buffer to hold payload with prepended rx length */
    uint8_t anOspFrame[ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE];
    anOspFrame[0u] = (uint8_t)(poFioOspIoData->nWriteLength * 8u);

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength + ADI_FIO_OSP_ADD_TXRX_SIZE;
    oTxnCfg.oTxnHdr.nReadLen          = poFioOspIoData->nReadLength;

    /* Create Rx length prepended payload */
    if(poFioOspIoData->nWriteLength > 0u)
    {
        (void)ADI_MEMCPY(&anOspFrame[1u], poFioOspIoData->pTxData, poFioOspIoData->nWriteLength);
    }

    anOspFrame[poFioOspIoData->nWriteLength + 1u] = (uint8_t)(poFioOspIoData->nReadLength * 8u);
    oTxnCfg.pPayload = anOspFrame;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalFioOspData->bIsMulticast;

    /* Use the locally created FIO_OSP transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poFioOspTxnCfg = &oTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalFioOspInstData, poFioOspTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalFioOspData, poEalFioOspInstData, bIsMcast, &poFioOspTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalFioOspData, bIsMcast, poFioOspIoData->apRxData) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }

    } while(false);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Switch the mode of all the instances to periodic */
        for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
        {
            EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[i];

            if(adi_e2bcore_addFlexibleIOConfigToQueue(&(poEalFioOspInstData->oId), poFioOspTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }

            poEalFioOspInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
        }
    }

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Writes multiple FIFO messages to the Flexible IO interface in a single E2B frame. Tx and Rx length data should be included in the poFioOspIoData
 *
 * @param [in] hFioOspHandle          The FIO_OSP handle
 * @param [in] poFioOspIoData         Pointer to FIO_OSP I/O DATA
 * @param [in] nNumPackedTxns       The number of FIO_OSP transaction length
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO OSP custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspMultiWrite(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
                                        uint16_t nNumPackedTxns, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData != NULL, "FIO_OSP IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)memset(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFU;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nNumPackedTxns;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = 0u;
    oTxnCfg.pPayload                  = poFioOspIoData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Write multiple FIFO messages to the Flexible IO interface in a single E2B frame synchronously. Tx and Rx length data
 * should be included in the poFioOspIoData
 *
 * @param [in] hFioOspHandle          The FIO_OSP handle
 * @param [in] poFioOspIoData         Pointer to FIO_OSP I/O DATA
 * @param [in] nNumPackedTxns       The number of FIO_OSP transaction length
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO OSP custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 *             - #ADI_EAL_STATUS_RX_TIMEOUT : Timeout error
 *
 * @note        This is a synchronous API that internally flushes the transmit buffer before returning.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspMultiWriteSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint16_t nNumPackedTxns, uint8_t nId)
{
    ADI_EAL_STATUS eEalStatus;

    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData != NULL, "FIO_OSP IO data pointer is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)memset(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nNumPackedTxns;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = 0u;
    oTxnCfg.pPayload                  = poFioOspIoData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Set the waiting for RX flag to true */
    SetWaitingForRx(poEalFioOspData, true);

    /* Send the FIO_OSP transaction */
    eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bool bTimedOut = WaitForResponses(poEalFioOspData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalFioOspData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Sends multiple write/read messages to the Flexible IO interface in a single E2B frame synchronously.
 * Tx and Rx length data should be included in the poFioOspIoData
 *
 * @param [in] hFioOspHandle          The FIO_OSP handle
 * @param [in] poFioOspIoData         Pointer to FIO_OSP I/O DATA
 * @param [in] nNumPackedTxns       The number of FIO_OSP transaction length
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO OSP custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspMultiWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint16_t nNumPackedTxns, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");
    ADI_DBG_REQUIRE((poFioOspIoData->nReadLength > 0U)
                    && (poFioOspIoData->nReadLength < 32u), "OSP Read length is invalid");

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)memset(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = nId != 0xFFu;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nNumPackedTxns;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poFioOspIoData->nReadLength;
    oTxnCfg.pPayload                  = poFioOspIoData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Sends multiple write/read messages to the Flexible IO interface in a single E2B frame synchronously.
 * Tx and Rx length data should be included in the poFioOspIoData
 *
 * @param [in] hFioOspHandle          The FIO_OSP handle
 * @param [in] poFioOspIoData         Pointer to FIO_OSP I/O DATA
 * @param [in] nNumPackedTxns       The number of FIO_OSP transaction length
 * @param [in] nId               Identifier for the write request (Set to 0xFF if no ID is reqd)
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO OSP custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 *             - #ADI_EAL_STATUS_RX_TIMEOUT : Timeout error
 *
 * @note        This is a synchronous API that internally flushes the transmit buffer before returning.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspMultiWriteReadSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint16_t nNumPackedTxns, uint8_t nId)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poFioOspIoData->pTxData != NULL, "Pointer to the TX data is NULL");
    ADI_DBG_REQUIRE(poFioOspIoData->apRxData != NULL, "Pointer to the RX data is NULL");
    ADI_DBG_REQUIRE((poFioOspIoData->nWriteLength > 0U)
                    && (poFioOspIoData->nWriteLength < 32u), "OSP Write length is invalid");
    ADI_DBG_REQUIRE((poFioOspIoData->nReadLength > 0U)
                    && (poFioOspIoData->nReadLength < 32u), "OSP Read length is invalid");

    /* Set the FIO_OSP transaction configuration */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)memset(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked    = true;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr     = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent   = true;
    oTxnCfg.oTxnHdr.nNumPackedTxns    = nNumPackedTxns;
    oTxnCfg.oTxnHdr.nE2bId            = nId;
    oTxnCfg.oTxnHdr.bOverrideWriteLen = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen  = true;
    oTxnCfg.oTxnHdr.nWriteLen         = poFioOspIoData->nWriteLength;
    oTxnCfg.oTxnHdr.nReadLen          = poFioOspIoData->nReadLength;
    oTxnCfg.pPayload                  = poFioOspIoData->pTxData;

    /* Loop through the mutex of all instances and lock it */
    LockFioOspMutex(poEalFioOspData);

    SetWaitingForRx(poEalFioOspData, true);

    /* Send the FIO_OSP transaction */
    ADI_EAL_STATUS eEalStatus = SendFioOspTransaction(poEalFioOspData, &oTxnCfg, poFioOspIoData->apRxData, false, 0ULL);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Wait for No-op transaction with the ID */
        bool bTimedOut = WaitForResponses(poEalFioOspData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;
    }

    /* Set the waiting for RX flag to false */
    SetWaitingForRx(poEalFioOspData, false);

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief To write/read to/from interface fifo based on the transaction config
 *
 * @param [in] hFioOspHandle   Handle corresponding to this instance
 * @param [in] paoFioOspTxnCfg FIO OSP transaction configs
 * @param [in] nNumFioOspTxns  Number of transaction configs
 * @return     Status
 *             - #ADI_EAL_STATUS_SUCCESS       FIO OSP custom transaction was successful
 *             - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *             - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *             - #ADI_EAL_STATUS_FIFO_FAILED   Failed to add a fifo transaction
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspCustomWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg, uint16_t nNumFioOspTxns)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(paoFioOspTxnCfg != NULL, "FIO_OSP transaction config pointer is NULL");
    ADI_DBG_REQUIRE(nNumFioOspTxns > 0U, "Number of FIO_OSP transactions is invalid");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioOspData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* Lock the mutex */
    LockFioOspMutex(poEalFioOspData);

    /* Get the number of transactions that expects a response */
    uint16_t nNumTxnsWithResp = GetNumTxnsWithResp(poEalFioOspInstData, paoFioOspTxnCfg, nNumFioOspTxns);

    /* Send the custom FIO_OSP transactions */
    eEalStatus = SendCustomFioOspTransactions(poEalFioOspData, poEalFioOspInstData, paoFioOspTxnCfg, nNumFioOspTxns,
                 nNumTxnsWithResp);

    /* Unlock the mutex */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief       Reads the status of the FIO OSP interface
 *
 * @param [in]  hFioOspHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Read successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspReadStatus(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);

    bool bIsMcast = poEalFioOspData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalFioOspData->nIfRegVlanTag : *poEalFioOspInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalFioOspData->nIntfTopic;
        anTopics[1U] = poEalFioOspData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalFioOspInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalFioOspInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalFioOspInstData->oId, FLEXIO_NUM_BYTES_NW_ADDR, 2U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief Write to Flexible-IO OSP interface register
 *
 * @param [in]  hFioOspHandle   Handle corresponding to this instance
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
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspSendRegTransaction(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalFioOspInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalFioOspInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalFioOspInstData->oId.nIntfNum;

    bool bIsMcast = poEalFioOspData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync          = false;
    oMsgOpts.bTopicOverRide = true;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioOspData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalFioOspData->nFifoRegVlanTag : *poEalFioOspInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioOspData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalFioOspData->nIfRegVlanTag : *poEalFioOspInstData->pnIfRegVlanTag;
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
 * @param [in]  hFioOspHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-OSP reset successful
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspResetQueue(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioOspData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* Lock the mutex */
    LockFioOspMutex(poEalFioOspData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearFlexibleIOConfigQueue(&poEalFioOspInstData->oId);
        poEalFioOspInstData->nFioOspQWrIdx = 0U;
        poEalFioOspInstData->nFioOspQRdIdx = 0U;
        poEalFioOspInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
        poEalFioOspInstData->bWaitingForRx = false;

        /* Ignore errors */
        poEalFioOspInstData->bIgnoreErr = true;

        /* Trigger resync */
        uint16_t nStartAddr =  FLEXIO_CONTROL_2_NW_ADDR;
        uint32_t nMask      =  BITM_FLEXIO_CONTROL_2_RESYNCHRONIZATION_REQUIRED;
        uint32_t nData      =  BITM_FLEXIO_CONTROL_2_RESYNCHRONIZATION_REQUIRED;

        /* Set the parameters for register config */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.anMask = &nMask;
        oRegCfg.anStartAddr = &nStartAddr;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.pBuffer = &nData;
        eEalStatus = adi_eal_fioOspSendRegTransaction((ADI_EAL_FIO_OSP_HANDLE)poEalFioOspData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an FIO_OSP NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalFioOspInstData->oId);

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
        eEalStatus = adi_eal_fioOspSendRegTransaction((ADI_EAL_FIO_OSP_HANDLE)poEalFioOspData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalFioOspInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockFioOspMutex(poEalFioOspData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for FIO OSP instance
 *
 * @param [in] hFioOspHandle     FIO OSP instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOOSP_CODE
void adi_eal_fioOspSetDownstreamVlan(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                     ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Extract the FIO-OSP data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);

    /* Get the unicast instance data from the handle */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* If it is a multicast handle, update the handle structure */
    if(poEalFioOspData->bIsMulticast)
    {
        poEalFioOspData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalFioOspData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalFioOspData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalFioOspInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioOspInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalFioOspInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for FIO OSP FIFO messages
 *
 * @param [in] hFioOspHandle  FIO OSP interface Handle
 * @param [in] poVlanTag      Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn      If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOOSP_CODE
ADI_EAL_STATUS adi_eal_fioOspSetUpstreamVlan(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Extract the FIO-OSP data from the handle */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)hFioOspHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioOspData);

    ADI_DBG_REQUIRE(poVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* Add VLAN tag info */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    uint16_t nStartAddr = FIFO_CTAG_CONFIGURATION_NW_ADDR;
    uint32_t nRegData = ((uint32_t)poVlanTag->nVlanId & BITM_FIFO_CTAG_CONFIGURATION_CTAG_ID) |
                        (poVlanTag->bDEI ? (uint32_t)BITM_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE : 0U) |
                        (((uint32_t)poVlanTag->nPCP & 0x07U) << BITP_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY) |
                        (bIsVlanEn ? BITM_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG : 0U);

    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.pBuffer = &nRegData;
    oRegCfg.eType = ADI_E2BCORE_RND_WRITE;

    return adi_eal_fioOspSendRegTransaction(poEalFioOspData, true, &oRegCfg);
}

/*============================ CALLBACKS ============================*/
/**
 * @brief The callback for FIO OSP interface
 * @param [in] poID          The ID (Instance, Remote, Interface numbers) for which the callback is coming
 * @param [in] eType         The type of event this callback is for
 * @param [in] poParam       The data corresponding to this event
 * @param [in] bIsTxFrame    Flag showing if the callback is a TX frame callback
 * @param [in] pCbParam      Parameter to be passed back while invoking callback
 */
ADI_EAL_FIOOSP_CODE
void FioOspInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                        void *pCbParam)
{
    /* If pCbParam is NULL, the EAL FIO-OSP interface instance is not opened,
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
/*! Notify reception of High or Low IRQs for OSP */
ADI_EAL_FIOOSP_CODE
static bool eal_notify_FioOspInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
                                       ADI_EAL_INTERFACE_INTERRUPT_CBK_EVT eFioOspEvt)
{
    /* Extract the interface internal data structure from the callback parameter */
    void *pInterface = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum].pCbParam;
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)pInterface;

    /* Get the unicast instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    bool bInterruptProcessed = false;

    /* Handle the different callback events from e2bcore */
    switch(eFioOspEvt)
    {
        case ADI_EAL_NODE_CBK_EVT_FLEXIO_IRQS:
        {
            bInterruptProcessed = true;
            InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_READ_TIMEOUT, NULL);
            break;
        }

        default:
        {
            /* Shouldn't come here! */
            break;
        }
    }

    return bInterruptProcessed;
}

ADI_EAL_FIOOSP_CODE
/*! Sends the FIO_OSP transaction */
static ADI_EAL_STATUS SendFioOspTransaction(
    EAL_FIO_OSP_DATA *poEalFioOspData,
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg,
    uint8_t **apRxBuf,
    bool bIsPeriodic,
    uint64_t nPeriod)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalFioOspData->bIsMulticast;

    /* Use the locally created FIO_OSP transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poFioOspTxnCfg = poTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalFioOspInstData, poTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            eEalStatus = CheckAndAddTransactionToQueues(poEalFioOspData, poEalFioOspInstData, bIsMcast, &poFioOspTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = bIsMcast ? poEalFioOspData->nFifoWriteVlanTag : *poEalFioOspInstData->pnFifoWriteVlanTag;

        eal_utils_fillNetworkCfg(poEalFioOspInstData->oId.nInstNum,
                                 poEalFioOspData->bIsMulticast ? ADI_EAL_ALL_REMOTES : poEalFioOspInstData->oId.nRemoteNum,
                                 &oNwFrameCfg, &nVlanTag);

        /* Set the FIFO message options */
        ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = false;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = poEalFioOspData->bIsMulticast ?
                                      poEalFioOspData->nFifoTopic :
                                      ADI_E2BCORE_TOPIC_FIFO0 + poEalFioOspInstData->oId.nIntfNum;

        if(!bIsPeriodic)
        {
            /* Send the unicast / multicast e2b packet with the FIFO message containing the FIO_OSP transaction */
            eEalStatus = eal_utils_sendFifoFrame(&poEalFioOspInstData->oId, &oNwFrameCfg, poFioOspTxnCfg, &oFifoMsgOpts);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalFioOspData, bIsMcast, apRxBuf) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }
    } while(false);

    return eEalStatus;
}


/*! Lock the mutex of all FIO_OSP instances */
ADI_EAL_FIOOSP_CODE
static void LockFioOspMutex(EAL_FIO_OSP_DATA *poEalFioOspData)
{
    /* Loop through all the unicast FIO_OSP instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalFioOspData->apoInstData[i]->hFioOspMutex);
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all FIO_OSP instances */
ADI_EAL_FIOOSP_CODE
static void UnlockFioOspMutex(EAL_FIO_OSP_DATA *poEalFioOspData)
{
    /* Loop through all the unicast FIO_OSP instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalFioOspData->apoInstData[i]->hFioOspMutex);
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/*! Set the waiting for RX flag for all the instances of the handle */
ADI_EAL_FIOOSP_CODE
static void SetWaitingForRx(EAL_FIO_OSP_DATA *poEalFioOspData, bool bWaitingForRx)
{
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalFioOspInstData->bWaitingForRx = bWaitingForRx;
    }
}

/*! Wait for response */
ADI_EAL_FIOOSP_CODE
static bool WaitForResponses(EAL_FIO_OSP_DATA *poEalFioOspData)
{
    bool bTimedOut = false;
    bool bAllResponsesReceived = true;
    uint64_t nCurrTime, nStartTime;

    /* Get the start time */
    adi_pal_getCurrTime(&nCurrTime);
    nStartTime = nCurrTime;

    /* Wait for FIO_OSP write to be received by the host interface and
       send back a No-op transaction with the ID sent downstream */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);
        bAllResponsesReceived = true;

        for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalFioOspInstData->bWaitingForRx)
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

    return bTimedOut;
}

/*! Returns if response is expected based on transaction and instance configuration */
ADI_EAL_FIOOSP_CODE
static bool IsResponseExpected(EAL_FIO_OSP_INST_DATA *poEalFioOspData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The transaction has read length greater than 0, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            (poTxnCfg->oTxnHdr.nReadLen > 0U) ||
            poEalFioOspData->bIsReadTimeTagEn);
}

/*! Returns the number of transactions with response expected */
ADI_EAL_FIOOSP_CODE
static uint16_t GetNumTxnsWithResp(EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
                                   ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg,
                                   uint16_t nNumFioOspTxns)
{
    uint16_t nNumTxnsWithResp = 0U;

    for(uint16_t i = 0U; i < nNumFioOspTxns; i++)
    {
        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalFioOspInstData, &paoFioOspTxnCfg[i]))
        {
            nNumTxnsWithResp++;
        }
    }

    return nNumTxnsWithResp;
}

/* Handle error callback */
ADI_EAL_FIOOSP_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)pCbParam;

    /* Get the FIO_OSP unicast instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalFioOspInstData->bIgnoreErr)
    {
        InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/* Handle register read callback */
ADI_EAL_FIOOSP_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    ADI_DBG_ENSURE(poID != NULL, "poID is NULL in HandleRegReadCallback");

    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal data of this instance */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)pCbParam;

    /* Get the FIO_OSP unicast instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    /* If the read is of the FIO STATUS registers */
    if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FLEXIO_NUM_BYTES_NW_ADDR) &&
            (poRegCfg->nLength == 2U))
    {
        /* Process FIO status data */
        ADI_EAL_FIO_STATUS_DATA oFioOspStatusData;
        ExtractFioOspStatus(poRegCfg->pBuffer, &oFioOspStatusData);
        InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_INTF_STATUS_EVT, &oFioOspStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_REG_READ_RESP, poParam);
}

/** Extract FIO OSP status data */
ADI_EAL_FIOOSP_CODE
static void ExtractFioOspStatus(uint32_t *panData, ADI_EAL_FIO_STATUS_DATA *poStatusData)
{
    /* Extract the number of read bytes */
    poStatusData->nNumRdBytes = (uint16_t)((panData[0U] & BITM_FLEXIO_NUM_BYTES_NUM_RD_BYTES) >>
                                           BITP_FLEXIO_NUM_BYTES_NUM_RD_BYTES);
    /* Extract the number of write bytes */
    poStatusData->nNumWrBytes = (uint16_t)((panData[0U] & BITM_FLEXIO_NUM_BYTES_NUM_WR_BYTES) >>
                                           BITP_FLEXIO_NUM_BYTES_NUM_WR_BYTES);
    /* Extract the number of transactions processed */
    poStatusData->nNumTransactions = (uint16_t)panData[1U];
}

/* Handle FIFO read callback */
ADI_EAL_FIOOSP_CODE
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal data of this instance */
    EAL_FIO_OSP_DATA *poEalFioOspData = (EAL_FIO_OSP_DATA *)pCbParam;

    /* Get the FIO_OSP unicast instance data */
    EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[0U];

    do
    {
        /* Proceed giving callbacks to application */
        if(poEalFioOspInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
        {
            /* Increment the read index */
            poEalFioOspInstData->nFioOspQRdIdx = EAL_INC_Q_IDX(poEalFioOspInstData->nFioOspQRdIdx,
                                                 poEalFioOspInstData->nFioOspCfgQDepth);

            /******************************
                        Cases:
                            * FIO OSP write read sync completed (Clear bWaitingForRx flag)
                            * Callback indicating FIO OSP read/write read completed (Give callback)
                            * Callback indicating FIO OSP write completed (Give callback)
            *******************************/
            if(poEalFioOspInstData->bWaitingForRx)
            {
                poEalFioOspInstData->bWaitingForRx = false;
            }
            else if(poTxnCfg->oTxnHdr.nReadLen > 0U)
            {
                /* Give a callback with read response event */
                InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_OSP_READ_RESP, poTxnCfg);
            }
            else
            {
                /* Give a callback with write response event */
                InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_OSP_WRITE_RESP, poTxnCfg);
            }
        }
        else if(poEalFioOspInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
        {
            /* Give a callback with periodic RX response event */
            InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_PERIODIC_RX, poTxnCfg);

            if(adi_e2bcore_addFlexibleIOConfigToQueue(poID, poTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to add fio osp config to queue");
            }
        }
        else if(poEalFioOspInstData->eMode == ADI_EAL_USAGE_MODE_CUSTOM)
        {
            /* Increment the read index */
            poEalFioOspInstData->nFioOspQRdIdx = EAL_INC_Q_IDX(poEalFioOspInstData->nFioOspQRdIdx,
                                                 poEalFioOspInstData->nFioOspCfgQDepth);

            /* Give a callback with FIFO read response event */
            InvokeCbk(poEalFioOspData, poEalFioOspInstData, ADI_EAL_FIO_OSP_CBK_EVT_OSP_FIFO_READ, poFifoDat);
        }
        else
        {
            // Nothing to do
        }
    } while(false);
}

/* Invoke the callback */
ADI_EAL_FIOOSP_CODE
static void InvokeCbk(EAL_FIO_OSP_DATA *poEalFioOspData, EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
                      ADI_EAL_FIO_OSP_CBK_EVT eEvt, const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalFioOspInstData->pfunCbk != NULL)
    {
        (poEalFioOspInstData->pfunCbk)(poEalFioOspInstData->oId.nInstNum, poEalFioOspData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_FIOOSP_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_FIO_OSP_DATA *poEalFioOspData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalFioOspInstData->nFioOspQWrIdx, poEalFioOspInstData->nFioOspQRdIdx,
                               poEalFioOspInstData->nFioOspCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_FIOOSP_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_FIO_OSP_DATA *poEalFioOspData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInFlexibleIOQueue(&poEalFioOspInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_FIOOSP_CODE
static void CopyTransactionConfigToQueue(EAL_FIO_OSP_DATA *poEalFioOspData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg = &poEalFioOspInstData->paoCfgs[poEalFioOspInstData->nFioOspQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    }
}

/** Adds the transaction to queue for all instances */
ADI_EAL_FIOOSP_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_FIO_OSP_DATA *poEalFioOspData, bool bIsMcast, uint8_t **apRxBuf)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalFioOspData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData = poEalFioOspData->apoInstData[i];

        /* Save the RX pointer */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg = &poEalFioOspInstData->paoCfgs[poEalFioOspInstData->nFioOspQWrIdx];
        poInstTxnCfg->pPayload = apRxBuf != NULL ? apRxBuf[i] : NULL;

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addFlexibleIOConfigToQueue(&poEalFioOspInstData->oId, poInstTxnCfg);

            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalFioOspInstData->nFioOspQWrIdx = EAL_INC_Q_IDX(poEalFioOspInstData->nFioOspQWrIdx,
                                             poEalFioOspInstData->nFioOspCfgQDepth);
    }

    return eError;
}

/** Check and add transaction to queues */
ADI_EAL_FIOOSP_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_FIO_OSP_DATA *poEalFioOspData,
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioOspTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalFioOspData))
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalFioOspData))
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalFioOspData, *ppoFioOspTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoFioOspTxnCfg = &poEalFioOspInstData->paoCfgs[poEalFioOspInstData->nFioOspQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Sends custom FIO_OSP transactions */
ADI_EAL_FIOOSP_CODE
static ADI_EAL_STATUS SendCustomFioOspTransactions(EAL_FIO_OSP_DATA *poEalFioOspData,
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg, uint16_t nNumFioOspTxns, uint16_t nNumTxnsWithResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Should network layer mutex be released? */
    bool bShouldReleaseNwMutex = false;

    do
    {
        /* Local variables */
        uint32_t nInstNum = poEalFioOspInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioOspInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalFioOspInstData->oId.nIntfNum;

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalFioOspInstData->nFioOspQWrIdx,
                                         poEalFioOspInstData->nFioOspQRdIdx,
                                         poEalFioOspInstData->nFioOspCfgQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Space remaining in the e2bcore transaction queue */
        nSpaceLeftInQueue = adi_e2bcore_slotsLeftInFlexibleIOQueue(&poEalFioOspInstData->oId);

        /* Check if there is enough space in the e2bcore transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Set the network frame configurations */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = *poEalFioOspInstData->pnFifoWriteVlanTag;
        eal_utils_fillNetworkCfg(
            nInstNum,
            nRemoteNum,
            &oNwFrameCfg, &nVlanTag
        );

        /* Set the FIFO message options */
        static ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
        (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
        oFifoMsgOpts.bSync          = false;
        oFifoMsgOpts.bTopicOverRide = true;
        oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

        eEalStatus = eal_utils_startE2bMessage(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        /* Loop through all configs and add FIO_OSP transaction */
        eEalStatus = AddCustomFioOspTxnsToMsg(nInstNum, poEalFioOspData, poEalFioOspInstData, paoFioOspTxnCfg,
                                              nNumFioOspTxns);

        /* Exit with status if there was any error above */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            bShouldReleaseNwMutex = true;
            break;
        }

        eal_utils_finishMessageAndTransmit(nInstNum, &oNwFrameCfg, &oFifoMsgOpts);

        /* Set the FIO_OSP EAL mode to operate in custom mode */
        poEalFioOspInstData->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/** Add multiple custom FIO_OSP transactions */
ADI_EAL_FIOOSP_CODE
static ADI_EAL_STATUS AddCustomFioOspTxnsToMsg(uint32_t nInstNum, EAL_FIO_OSP_DATA *poEalFioOspData,
        EAL_FIO_OSP_INST_DATA *poEalFioOspInstData, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg,
        uint16_t nNumFioOspTxns)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Loop through all configs and add FIO_OSP transaction */
    for(uint16_t i = 0U; i < nNumFioOspTxns; i++)
    {

        /* If response is expected, the config should be persistent
           till the response is received, so it needs to be taken
           from the config queue pool */
        if(IsResponseExpected(poEalFioOspInstData, &paoFioOspTxnCfg[i]))
        {
            /* Since the check has been done above, space has to be available.
               Assert this assumption */
            ADI_DBG_REQUIRE(!EAL_IS_Q_FULL(
                                poEalFioOspInstData->nFioOspQWrIdx,
                                poEalFioOspInstData->nFioOspQRdIdx,
                                poEalFioOspInstData->nFioOspCfgQDepth),
                            "Invalid assumption!");

            /* Copy the given FIO_OSP configuration into the config queue pool */
            (void)ADI_MEMCPY(&poEalFioOspInstData->paoCfgs[poEalFioOspInstData->nFioOspQWrIdx], &paoFioOspTxnCfg[i],
                             sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

            /* Add the FIO_OSP transaction */
            if(adi_e2bcore_addFlexibleIOTxn(nInstNum,
                                            &poEalFioOspInstData->paoCfgs[poEalFioOspInstData->nFioOspQWrIdx]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }

            /* Increment queue index */
            poEalFioOspInstData->nFioOspQWrIdx = EAL_INC_Q_IDX(poEalFioOspInstData->nFioOspQWrIdx,
                                                 poEalFioOspInstData->nFioOspCfgQDepth);
        }
        else
        {
            /* Just add an FIO_OSP transaction with the config provided */
            if(adi_e2bcore_addFlexibleIOTxn(nInstNum, &paoFioOspTxnCfg[i]) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}


#endif /* ADI_E2B_IFACE_FIO_OSP_ENABLED */
/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
