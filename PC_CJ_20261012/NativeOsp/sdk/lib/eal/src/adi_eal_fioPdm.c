/*******************************************************************************
Copyright (c) 2024-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: adi_eal_fioPdm.c
 * @brief: The EAL APIs for FIO-PDM interface
 * Developed by: E2B Applications team
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"

#ifdef ADI_E2B_IFACE_FIO_PDM_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalFioPdmData) \
ADI_DBG_REQUIRE((poEalFioPdmData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalFioPdmData)->bInUse == true, "Instance is not opened");


/*================================ DATA TYPES ===============================*/

/*=================================== DATA ==================================*/
/* Instruction memory contents for configuring FIO as PDM */
ADI_MEM_DATA_CRIT_CACHE
static uint32_t ganInstructionMemDataFioClk[] =
{
    0xF001D821U,
    0xF001F000U,
    0xF000F000U,
};
static uint32_t ganInstructionMemDataExtClk[] =
{
    0xD120D820U,
    0xD0200000U
};

static uint16_t gnClockDiv = 8U;

/*================================ PROTOTYPES ===============================*/

static void CalcClkDividerInstructionNum(uint32_t nClockPeriod, uint16_t *nClockDivider, uint32_t *nInstructions);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void ExtractFioPdmStatus(uint32_t *panData, ADI_EAL_FIO_STATUS_DATA *poStatusData);
static void InvokeCbk(EAL_FIO_PDM_DATA *poEalFioPdmData, EAL_FIO_PDM_INST_DATA *poEalFioInstPdmData,
                      ADI_EAL_FIO_PDM_CBK_EVT eEvt, const void *pArg);
static void LockFioPdmMutex(EAL_FIO_PDM_DATA *poEalFioPdmData);
static void UnlockFioPdmMutex(EAL_FIO_PDM_DATA *poEalFioPdmData);
static void CopyTransactionConfigToQueue(EAL_FIO_PDM_DATA *poEalFioPdmData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg);
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_FIO_PDM_DATA *poEalFioPdmData);
static bool IsSpaceAvblInEalTxnQueue(EAL_FIO_PDM_DATA *poEalFioPdmData);
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_FIO_PDM_DATA *poEalFioPdmData,
        EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioPdmTxnCfg);
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_FIO_PDM_DATA *poEalFioPdmData, bool bIsMcast);
static bool IsResponseExpected(EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData,
                               ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg);
static ADI_EAL_STATUS ConfigureFifoWriteThresholds(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle);
static ADI_EAL_STATUS ConfigureFifoRxFull(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an instance of FIO-PDM interface
 *
 * @param [in]  poFioPdmCfg     Configurations for this FIO-PDM instance
 * @param [out] phFioPdmHandle  Handle corresponding to this FIO-PDM instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-PDM instance successfully opened
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmOpen(ADI_EAL_FIO_PDM_CFG *poFioPdmCfg, ADI_EAL_FIO_PDM_HANDLE *phFioPdmHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poFioPdmCfg != NULL, "Pointer to the FIO-PDM module configuration is NULL");
    ADI_DBG_REQUIRE(phFioPdmHandle != NULL, "Pointer to the FIO-PDM handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poFioPdmCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poFioPdmCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poFioPdmCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poFioPdmCfg->nInstNum;
    uint32_t nRemoteNum = poFioPdmCfg->nRemoteNum;
    uint32_t nIntfNum = poFioPdmCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_FIO,
                    "The given pair of instance, remote and interface numbers is not configured as FIO_PDM interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poFioPdmCfg->pEalFioPdmMemory != NULL, "Pointer to allocated memory is NULL");
    ADI_DBG_REQUIRE(poFioPdmCfg->nEalFioPdmMemorySize >=
                    ADI_EAL_FIO_PDM_UNICAST_INST_MEM_SIZE(poFioPdmCfg->nFioPdmCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poFioPdmCfg->pEalFioPdmMemory;

    /* Allocate memory for the FIO PDM transaction config queue. */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioPdmTxnCfgs = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)((void *)pMem);
    ADI_DBG_REQUIRE(((uintptr_t)pMem % 4U) == 0U, "Memory is not 4-byte aligned");
    pMem += (poFioPdmCfg->nFioPdmCfgQDepth * sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

    /* Allocate memory for the EAL FIO_PDM instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = (EAL_FIO_PDM_INST_DATA *)((void *)pMem);
    (void)ADI_MEMSET(poEalFioPdmInstData, 0, sizeof(EAL_FIO_PDM_INST_DATA));
    ADI_DBG_REQUIRE(((uintptr_t)pMem % 4U) == 0U, "Memory is not 4-byte aligned");
    pMem += sizeof(EAL_FIO_PDM_INST_DATA);

    /* Instance info */
    poEalFioPdmInstData->oId.nInstNum = poFioPdmCfg->nInstNum;
    poEalFioPdmInstData->oId.nRemoteNum = poFioPdmCfg->nRemoteNum;
    poEalFioPdmInstData->oId.nIntfNum = poFioPdmCfg->nIntfNum;
    poEalFioPdmInstData->pnIfRegVlanTag = &poIfConfig->nIfRegVlanTag;
    poEalFioPdmInstData->pnFifoRegVlanTag = &poIfConfig->nFifoRegVlanTag;
    poEalFioPdmInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;

    /* State info */
    poEalFioPdmInstData->bIgnoreErr         = false;
    poEalFioPdmInstData->eMode              = ADI_EAL_USAGE_MODE_NORMAL;
    poEalFioPdmInstData->pfunCbk            = NULL; /* Will be filled in adi_eal_FioPdmRegisterCallback */
    poEalFioPdmInstData->ePdmClkMode        = poFioPdmCfg->bUseExternalClock ? ENUM_PDM_CLK_MODE_EXTCLK :
            ENUM_PDM_CLK_MODE_FIOCLK;

    /* Transaction config queue info */
    uint32_t nFioCtrlReg = 0U;
    EAL_FIO_PDM_INST_DATA **apoInstData;
    EAL_FIO_PDM_DATA *poEalFioPdmData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalFioPdmInstData->oId, FLEXIO_CONTROL_NW_ADDR, &nFioCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalFioPdmInstData->bIsReadTimeTagEn   = (nFioCtrlReg & BITM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalFioPdmInstData->nFioPdmCfgQDepth   = poFioPdmCfg->nFioPdmCfgQDepth;
    poEalFioPdmInstData->nFioPdmQWrIdx      = 0U;
    poEalFioPdmInstData->nFioPdmQRdIdx      = 0U;
    poEalFioPdmInstData->paoCfgs            = paoFioPdmTxnCfgs;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalFioPdmInstData->oId, (void *)pMem,
            poFioPdmCfg->nFioPdmCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *) * poFioPdmCfg->nFioPdmCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_FIO_PDM_INST_DATA **)((void *)pMem);
        ADI_DBG_REQUIRE(((uintptr_t)pMem % 4U) == 0U, "Memory is not 4-byte aligned");
        pMem += sizeof(EAL_FIO_PDM_INST_DATA *);
        apoInstData[0U] = poEalFioPdmInstData;

        /* Allocate memory for the internal EAL FIO_PDM data structure */
        poEalFioPdmData  = (EAL_FIO_PDM_DATA *)((void *)pMem);
        pMem += sizeof(EAL_FIO_PDM_DATA);
        poEalFioPdmData->bIsMulticast  = false;
        poEalFioPdmData->bInUse        = true;
        poEalFioPdmData->nIntfTopic    = 0U;
        poEalFioPdmData->nFifoTopic    = 0U;
        poEalFioPdmData->nNumInst      = 1U;
        poEalFioPdmData->apoInstData   = apoInstData;
        /* Initialise the mutex for this FIO PDM instance */
        poEalFioPdmInstData->oFioPdmMutexCfg.pMem = &poEalFioPdmInstData->anFioPdmMutexBuffer[0U];
        poEalFioPdmInstData->oFioPdmMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalFioPdmInstData->oFioPdmMutexCfg, &poEalFioPdmInstData->hFioPdmMutex);

        /* Set the callback param in the e2bcore interface config */
        poIfConfig->pCbParam = poEalFioPdmData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_FIO_PDM;

        /* Return the internal FIO-PDM instance data as
           the handle for use in other functions */
        *phFioPdmHandle = (ADI_EAL_FIO_PDM_HANDLE)poEalFioPdmData;
    }

    return eStatus;
}

/**
 * @brief       Closes the opened FIO-PDM interface instance
 *
 * @param [in]  hFioPdmHandle   Handle conrresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-PDM instance successfully closed
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmClose(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle)
{
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalFioPdmData->apoInstData[i]->hFioPdmMutex);
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalFioPdmData, 0, sizeof(EAL_FIO_PDM_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Register a callback function for getting notified of events
 *
 * @param [in]  hFioPdmHandle   Handle corresponding to this instance
 * @param [in]  pfFioPdmCallback    Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Callback registered successfully
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmRegisterCallback(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle,
        ADI_EAL_FIO_PDM_CALLBACK_FUNC pfFioPdmCallback)
{
    /* Extract the FIO-PDM data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);
    ADI_DBG_REQUIRE(pfFioPdmCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalFioPdmData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* Register the callback function */
    poEalFioPdmInstData->pfunCbk = pfFioPdmCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Setup periodic update to the PDM interface
 *
 * @param [in]  hFioPdmHandle   Handle corresponding to this instance
 * @param [in]  poFioPdmData    Pointer to the PDM IO data
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmSetupPeriodicWriteRead(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle,
        ADI_EAL_FIO_PDM_IO_DATA *poFioPdmData)
{

    /* Get the internal data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);
    ADI_DBG_REQUIRE(poEalFioPdmData != NULL, "FIO_PDM IO data pointer is NULL");

    /* Configure the FIO transaction */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oTxnCfg;
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked          = false;
    oTxnCfg.oTxnHdr.bEnIntfTxnHdr           = true;
    oTxnCfg.oTxnHdr.bIsE2bIdPresent         = false;
    oTxnCfg.oTxnHdr.nE2bId                  = 0xFFu;
    oTxnCfg.oTxnHdr.bIsGptpTimePresent      = false;
    oTxnCfg.oTxnHdr.bOverrideWriteLen       = true;
    oTxnCfg.oTxnHdr.bOverrideReadLen        = true;
    oTxnCfg.oTxnHdr.bIsRouterHeaderInserted = false;
    oTxnCfg.oTxnHdr.nWriteLen               = 0U; /*Since there is no write operation it is always maintained to be 0u*/
    oTxnCfg.oTxnHdr.nReadLen                = poFioPdmData->nReadLength;
    oTxnCfg.pPayload                        = poFioPdmData->pRxData;

    /* Loop through the mutex of all instances and lock it */
    LockFioPdmMutex(poEalFioPdmData);

    /* Send the FIO_PDM transaction */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Is it a multicast request? */
    bool bIsMcast = poEalFioPdmData->bIsMulticast;

    /* Use the locally created FIO_PDM transaction by default.
       NOTE: It will be overwritten if the configuration
             should come from the EAL transaction queue */
    ADI_E2BCORE_TXN_CONFIG *poFioPdmTxnCfg = &oTxnCfg;

    do
    {
        /* Get the unicast instance data */
        EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

        /* Is response expected for this transaction */
        bool bIsResponseExpected = IsResponseExpected(poEalFioPdmInstData, poFioPdmTxnCfg);

        /* Add the transaction to the EAL and e2bcore transaction queues */
        if(bIsResponseExpected)
        {
            /* Add the transaction to the EAL and e2bcore transaction queues */
            eEalStatus = CheckAndAddTransactionToQueues(poEalFioPdmData, poEalFioPdmInstData, bIsMcast, &poFioPdmTxnCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        if(bIsResponseExpected)
        {
            /* Add the transaction to queue for all instances */
            if(AddTransactionToQueue(poEalFioPdmData, bIsMcast) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
        }

        /*Configure the FIFO write thresholds to control the flow of SMC*/
        eEalStatus = ConfigureFifoWriteThresholds(hFioPdmHandle);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /*Configure the Rx FIFO full to indicate the Rx FIFO is full*/
        eEalStatus = ConfigureFifoRxFull(hFioPdmHandle);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }
    } while(false);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Switch the mode of all the instances to periodic */
        for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; i++)
        {
            EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[i];
            if(adi_e2bcore_addFlexibleIOConfigToQueue(&(poEalFioPdmInstData->oId), poFioPdmTxnCfg) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
                break;
            }
            poEalFioPdmInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
        }
    }

    /* Loop through the mutex of all instances and unlock it */
    UnlockFioPdmMutex(poEalFioPdmData);

    return eEalStatus;
}

/**
 * @brief       Configure the Flexible IO instruction RAM
 *
 * @param [in] hFioPdmHandle  Handle corresponding to this FIO-PDM instance
 * @param [in] poFioInstructionRamData pointer to instruction RAM data to be programmed
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Updated instruction ram successfully
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmConfigureInstructionRAM(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle,
        ADI_EAL_FIO_PDM_INST_RAM_DATA *poFioInstructionRamData)
{
    /* Extract the FIO-PDM data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);

    /* Extract the FIO-PDM instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* Instruction RAM data */
    ADI_EAL_FIO_INST_RAM_DATA oFioInstRamData;

    /* If the application is overriding the instruction data,
       copy the values given by the application */
    if(poFioInstructionRamData->bOverrideDefault)
    {
        (void)ADI_MEMCPY(&oFioInstRamData, poFioInstructionRamData, sizeof(ADI_EAL_FIO_INST_RAM_DATA));
    }
    /* Else, set the default values */
    else
    {
        /* Set the instruction ram entries based on clock mode selection */
        if(poEalFioPdmInstData->ePdmClkMode == ENUM_PDM_CLK_MODE_FIOCLK)
        {
            oFioInstRamData.panInstructionMemData = &ganInstructionMemDataFioClk[0U];
            oFioInstRamData.nNumInstMemEntries = (uint16_t)(sizeof(ganInstructionMemDataFioClk) / sizeof(
                    ganInstructionMemDataFioClk[0U]));
        }
        else
        {
            oFioInstRamData.panInstructionMemData = &ganInstructionMemDataExtClk[0U];
            oFioInstRamData.nNumInstMemEntries = (uint16_t)(sizeof(ganInstructionMemDataExtClk) / sizeof(
                    ganInstructionMemDataExtClk[0U]));
        }

        /* TX FIFO to SM routing is not fixed */
        oFioInstRamData.bFifoTxRouterFixedSm = true;

        /* Number of active SMs */
        oFioInstRamData.nNumActiveStateMachines = 1U;

        /* Configure SM0 for PDM */
        oFioInstRamData.aoSmCfgs[0U].bDelaySidesetSingleMode = true;
        oFioInstRamData.aoSmCfgs[0U].bEnableAutopush = true;
        oFioInstRamData.aoSmCfgs[0U].bEnableAutopull = false;
        oFioInstRamData.aoSmCfgs[0U].bOsrShiftRight = true;
        oFioInstRamData.aoSmCfgs[0U].bIsrShiftRight = true;
        oFioInstRamData.aoSmCfgs[0U].nPinOutBasepin = 0U;
        oFioInstRamData.aoSmCfgs[0U].nPinInBasepin = 0U;
        oFioInstRamData.aoSmCfgs[0U].nJmpBasepin = 0U;
        oFioInstRamData.aoSmCfgs[0U].nProgramStart = 0U;

        if(poEalFioPdmInstData->ePdmClkMode == ENUM_PDM_CLK_MODE_FIOCLK)
        {
            oFioInstRamData.aoSmCfgs[0U].nProgramSize = 6U;
            oFioInstRamData.aoSmCfgs[0U].nDelaySidesetBits = 1U;
            oFioInstRamData.aoSmCfgs[0U].nSidesetBasepin = 1U;
        }
        else
        {
            oFioInstRamData.aoSmCfgs[0U].nProgramSize = 3U;
            oFioInstRamData.aoSmCfgs[0U].nDelaySidesetBits = 0U;
            oFioInstRamData.aoSmCfgs[0U].nSidesetBasepin = 0U;
        }

        oFioInstRamData.aoSmCfgs[0U].nAutopullThreshold = 0U;
        oFioInstRamData.aoSmCfgs[0U].nAutopushThreshold = 8U;
        oFioInstRamData.aoSmCfgs[0U].nPullThreshold = 0U;
        oFioInstRamData.aoSmCfgs[0U].nPushThreshold = 0U;
        oFioInstRamData.aoSmCfgs[0U].nClkDiv = gnClockDiv;
    }

    return eal_utils_configureFlexibleIO(&poEalFioPdmInstData->oId, &oFioInstRamData);
}

/**
 * @brief       Create a multicast group of FIO PDM interfaces
 *
 * @param [in]  poFioPdmMcastCfg       Configuration for FIO PDM multicast group
 * @param [out] phFioPdmMcastHandle    Handle to the multicast group
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully created the multicast group
 *
 * @note        In multicast configuration, if poIfRegVlanTag, poFifoRegVlanTag and poFifoWriteVlanTag are not present, pass it as NULL.
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmCreateMcastGroup(ADI_EAL_FIO_PDM_MCAST_CFG *poFioPdmMcastCfg,
        ADI_EAL_FIO_PDM_HANDLE *phFioPdmMcastHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poFioPdmMcastCfg != NULL, "Multicast configuration is NULL");
    ADI_DBG_REQUIRE(phFioPdmMcastHandle != NULL, "Pointer to the FIO PDM multicast handle is NULL");

    /* Multicast configuration checks */
    ADI_DBG_REQUIRE(poFioPdmMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
    ADI_DBG_REQUIRE(poFioPdmMcastCfg->pahFioPdmHandles != NULL, "Handle to the unicast instances is NULL");
    ADI_DBG_REQUIRE(poFioPdmMcastCfg->pFioPdmMcastMem != NULL, "Pointer to memory for FIO PDM multicast is NULL");
    ADI_DBG_REQUIRE(poFioPdmMcastCfg->nFioPdmMcastMemSize >= ADI_EAL_FIO_PDM_MCAST_GROUP_MEM_SIZE(
                        poFioPdmMcastCfg->nNumInst),
                    "Not enough memory allocated for FIO PDM multicast group");
    ADI_DBG_REQUIRE(poFioPdmMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");
    ADI_DBG_REQUIRE(poFioPdmMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                    "Assigned multicast interface topic is invalid");

    /* Allocate memory for the array of pointers to unicast instance data */
    uint8_t *pMem = (uint8_t *)poFioPdmMcastCfg->pFioPdmMcastMem;
    EAL_FIO_PDM_INST_DATA **apoInstData = (EAL_FIO_PDM_INST_DATA **)((void *)pMem);
    pMem += (sizeof(EAL_FIO_PDM_INST_DATA *) * poFioPdmMcastCfg->nNumInst);

    /* Save the unicast EAL FIO PDM instances */
    for(uint16_t i = 0U; i < poFioPdmMcastCfg->nNumInst; i++)
    {
        EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)poFioPdmMcastCfg->pahFioPdmHandles[i];
        apoInstData[i] = poEalFioPdmData->apoInstData[0U];

        /* Update the E2BCore interface config with the multicast topics */
        uint32_t nInstNum = poEalFioPdmData->apoInstData[0U]->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioPdmData->apoInstData[0U]->oId.nRemoteNum;
        uint32_t nIntfNum = poEalFioPdmData->apoInstData[0U]->oId.nIntfNum;
        ADI_E2BCORE_IF_CONFIG *poIfCfg =
            &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

        if(poFioPdmMcastCfg->nIntfTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this interface topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poFioPdmMcastCfg->nIntfTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }

        if(poFioPdmMcastCfg->nFifoTopic >= ADI_E2BCORE_MCAST_TOPIC_OFFSET)
        {
            /* Mark this FIFO topic as subscribed in the array (using topic - offset as index) */
            uint8_t nMcastIdx = (uint8_t)(poFioPdmMcastCfg->nFifoTopic - ADI_E2BCORE_MCAST_TOPIC_OFFSET);

            if(nMcastIdx < ADI_E2BCORE_MAX_MCAST_TOPICS)
            {
                poIfCfg->anMcastTopics[nMcastIdx] = 1U;
            }
        }
    }

    /* Set the multicast data structure */
    EAL_FIO_PDM_DATA *poEalFioPdmMcastData = (EAL_FIO_PDM_DATA *)((void *)pMem);
    poEalFioPdmMcastData->bIsMulticast = true;
    poEalFioPdmMcastData->bInUse       = true;
    poEalFioPdmMcastData->nIntfTopic   = poFioPdmMcastCfg->nIntfTopic;
    poEalFioPdmMcastData->nFifoTopic   = poFioPdmMcastCfg->nFifoTopic;
    poEalFioPdmMcastData->nIfRegVlanTag       = (poFioPdmMcastCfg->poIfRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poFioPdmMcastCfg->poIfRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioPdmMcastData->nFifoRegVlanTag     = (poFioPdmMcastCfg->poFifoRegVlanTag != NULL) ? eal_utils_convertVlanTags(
                poFioPdmMcastCfg->poFifoRegVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioPdmMcastData->nFifoWriteVlanTag   = (poFioPdmMcastCfg->poFifoWriteVlanTag != NULL) ? eal_utils_convertVlanTags(
                poFioPdmMcastCfg->poFifoWriteVlanTag) : ADI_VLAN_TAG_NA;
    poEalFioPdmMcastData->nNumInst     = poFioPdmMcastCfg->nNumInst;
    poEalFioPdmMcastData->apoInstData  = apoInstData;

    /* Set the handle to the EAL_FIO_PDM_DATA structure */
    *phFioPdmMcastHandle = (ADI_EAL_FIO_PDM_HANDLE)poEalFioPdmMcastData;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Set the clock period for the FIO PDM interface
 * @param [in]  hFioPdmHandle   Handle corresponding to this instance
 * @param [in]  nClockPeriod    Clock period to be set for the FIO PDM interface (in ns)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Clock period set successfully
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_STATUS adi_eal_fioPdmSetClockPeriod(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, uint32_t nClockPeriod)
{
    /* Get the internal data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;
    uint32_t nInstructions = 0U; // Number of instructions needed for the clock period
    uint16_t nClockDivider = 0U; // Clock divider value needed for the clock period

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);

    /* Check for valid clock period range and set to min and max permitted values */
    if(nClockPeriod < ADI_EAL_FIO_PDM_MIN_CLOCK_PERIOD)
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Selected FIO PDM Clock period is too low. Setting to minimum value.\r\n");
        nClockPeriod = ADI_EAL_FIO_PDM_MIN_CLOCK_PERIOD;
    }
    else if(nClockPeriod > ADI_EAL_FIO_PDM_MAX_CLOCK_PERIOD)
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Selected FIO PDM Clock period is too high. Setting to maximum value.\r\n");
        nClockPeriod = ADI_EAL_FIO_PDM_MAX_CLOCK_PERIOD;
    }
    else
    {
        // Nothing to do
    }

    /*Calculate Clock Divider and Instruction Count*/
    CalcClkDividerInstructionNum(nClockPeriod, &nClockDivider, &nInstructions);

    // Subtract the fixed instructions in assembly code
    uint32_t nInstField = (nInstructions > 3U) ? (nInstructions - 3U) : 0U;

    /* Update delay field for positive and negative clock to account for PDM frequency update*/
    ganInstructionMemDataFioClk[1U] &= ~(((uint32_t)0xFU << 1U) | ((uint32_t)0xFU << 17U));
    ganInstructionMemDataFioClk[1U] |= ((uint32_t)(nInstField & 0xFU) << 1U);
    ganInstructionMemDataFioClk[1U] |= ((uint32_t)(nInstField & 0xFU) << 17U);
    gnClockDiv = nClockDivider; // Set the clock divider globally

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                "FIO PDM Clock Period set to %u ns, Clock Divider: %u, Instructions: %u, 0x%x, 0x%x\r\n",
                nClockPeriod, nClockDivider, nInstructions, ganInstructionMemDataFioClk[0U], ganInstructionMemDataFioClk[1U]);

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Reads the status of the FIO PDM interface
 *
 * @param [in]  hFioPdmHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Read successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmReadStatus(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);

    bool bIsMcast = poEalFioPdmData->bIsMulticast;
    uint32_t anTopics[2U];
    uint32_t nVlanTag = bIsMcast ? poEalFioPdmData->nIfRegVlanTag : *poEalFioPdmInstData->pnIfRegVlanTag;

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalFioPdmData->nIntfTopic;
        anTopics[1U] = poEalFioPdmData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalFioPdmInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalFioPdmInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalFioPdmInstData->oId, FLEXIO_NUM_BYTES_NW_ADDR, 2U, bIsMcast, anTopics, nVlanTag);
}

/**
 * @brief Sets the downstream VLAN tag for FIO PDM instance
 *
 * @param [in] hFioPdmHandle     FIO PDM instance handle
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPDM_CODE
void adi_eal_fioPdmSetDownstreamVlan(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                     ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Extract the FIO-PDM data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);

    /* Get the unicast instance data from the handle */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

    /* If it is a multicast handle, update the handle structure */
    if(poEalFioPdmData->bIsMulticast)
    {
        poEalFioPdmData->nIfRegVlanTag     = eal_utils_convertVlanTags(poIfRegVlanTag);
        poEalFioPdmData->nFifoRegVlanTag   = eal_utils_convertVlanTags(poFifoRegVlanTag);
        poEalFioPdmData->nFifoWriteVlanTag = eal_utils_convertVlanTags(poFifoWriteVlanTag);
    }
    /* If it is a unicast handle, update the interface configs in e2bcore */
    else
    {
        uint32_t nInstNum = poEalFioPdmInstData->oId.nInstNum;
        uint32_t nRemoteNum = poEalFioPdmInstData->oId.nRemoteNum;
        uint32_t nIntfNum = poEalFioPdmInstData->oId.nIntfNum;

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
 * @brief Sets the upstream VLAN ID to be added for FIO PDM FIFO messages
 *
 * @param [in] hFioPdmHandle  FIO PDM interface Handle
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
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmSetUpstreamVlan(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Extract the FIO-PDM data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);

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

    return adi_eal_fioPdmSendRegTransaction((ADI_EAL_FIO_PDM_HANDLE)poEalFioPdmData, true, &oRegCfg);
}

/**
 * @brief Write to Flexible-IO PDM interface register
 *
 * @param [in]  hFioPdmHandle   Handle corresponding to this instance
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
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmSendRegTransaction(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalFioPdmInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalFioPdmInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalFioPdmInstData->oId.nIntfNum;

    bool bIsMcast = poEalFioPdmData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync          = false;
    oMsgOpts.bTopicOverRide = true;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioPdmData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalFioPdmData->nFifoRegVlanTag : *poEalFioPdmInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioPdmData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = bIsMcast ? poEalFioPdmData->nIfRegVlanTag : *poEalFioPdmInstData->pnIfRegVlanTag;
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
 * @param [in]  hFioPdmHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-PDM reset successful
 */
ADI_EAL_FIOPDM_CODE
ADI_EAL_STATUS adi_eal_fioPdmResetQueue(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPdmData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPdmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* Lock the mutex */
    LockFioPdmMutex(poEalFioPdmData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearFlexibleIOConfigQueue(&poEalFioPdmInstData->oId);
        poEalFioPdmInstData->nFioPdmQWrIdx = 0U;
        poEalFioPdmInstData->nFioPdmQRdIdx = 0U;
        poEalFioPdmInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;

        /* Ignore errors */
        poEalFioPdmInstData->bIgnoreErr = true;

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
        eEalStatus = adi_eal_fioPdmSendRegTransaction((ADI_EAL_FIO_PDM_HANDLE)poEalFioPdmData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an FIO_PDM NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalFioPdmInstData->oId);

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
        eEalStatus = adi_eal_fioPdmSendRegTransaction((ADI_EAL_FIO_PDM_HANDLE)poEalFioPdmData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalFioPdmInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockFioPdmMutex(poEalFioPdmData);

    return eEalStatus;
}

/** Notify reception of High or Low IRQs for PDM*/
ADI_EAL_FIOPDM_CODE
bool eal_notify_FioPdmInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
                                EAL_FIO_PDM_INTERRUPT_EVT eFioPdmEvt)
{
    /* Extract the interface internal data structure from the callback parameter */
    void *pInterface = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum].pCbParam;
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)pInterface;

    /* Get the unicast instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    bool bInterruptProcessed = false;

    /* Handle the different callback events from e2bcore */
    switch(eFioPdmEvt)
    {
        case EAL_FIO_PDM_WRITE_FIFO_LOW_IRQ_EVT:
        {
            /* The low threshold of the FIFO has been hit while waiting for the FIFO to be emptied */
            bInterruptProcessed = true;
            /*Change SMC Configurations to function in Config A for SMC to start filling the Tx FIFO*/
            ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
            (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
            uint16_t nRegAddr;
            uint32_t nRegVal, nRegMask;

            /*Register write is performed to change SMC configurations for particular remote node*/
            nRegAddr = LCE_SMC_MISC_NW_ADDR;
            nRegVal = ((uint32_t)ENUM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION_CONFIG_A <<
                       BITP_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION);
            nRegMask = BITM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION;
            oRegCfg.anStartAddr = &nRegAddr;
            oRegCfg.anMask = &nRegMask;
            oRegCfg.pBuffer = &nRegVal;
            oRegCfg.nLength = 1u;
            oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
            if(adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg) != ADI_EAL_STATUS_SUCCESS)
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to send reg transaction in fio pdm low irq handler");
            }
            break;
        }

        case EAL_FIO_PDM_WRITE_FIFO_HIGH_IRQ_EVT:
        {
            /* The high threshold of the FIFO has been hit while filling up the FIFO */
            bInterruptProcessed = true;
            /*Change SMC Configurations to function in Config B for SMC to start filling the Tx FIFO*/
            ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
            (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
            uint16_t nRegAddr;
            uint32_t nRegVal, nRegMask;

            /*Register write is performed to change SMC configurations for particular remote node*/
            nRegAddr = LCE_SMC_MISC_NW_ADDR;
            nRegVal = ((uint32_t)ENUM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION_CONFIG_B <<
                       BITP_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION);
            nRegMask = BITM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION;
            oRegCfg.anStartAddr = &nRegAddr;
            oRegCfg.anMask = &nRegMask;
            oRegCfg.pBuffer = &nRegVal;
            oRegCfg.nLength = 1u;
            oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
            if(adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg) != ADI_EAL_STATUS_SUCCESS)
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to send reg transaction in fio pdm high irq handler");
            }
            break;
        }

        case EAL_FIO_PDM_READ_FIFO_FULL_IRQ_EVT:
        {
            /* The RX FIFO has been hit and it is full */
            bInterruptProcessed = true;
            InvokeCbk(poEalFioPdmData, poEalFioPdmInstData, ADI_EAL_FIO_PDM_CBK_EVT_FIFO_RX_FULL, NULL);
            break;
        }

        default:
        {
            /* Shouldn't come here! */
            ADI_DBG_ERROR();
            break;
        }
    }

    return bInterruptProcessed;
}

/*================================ CALLBACKS ================================*/
/** FIO interface callback */
ADI_EAL_FIOPDM_CODE
void FioPdmInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                        void *pCbParam)
{
    /* If pCbParam is NULL, the EAL FIO-PDM interface instance is not opened,
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

/** Calculate the number of instructions and clock divider needed for the clock period */
/**
 * @brief       Calculate the most optimum number of instructions and clock divider for the clock period
 * @param [in]  nClockPeriod      Target clock period in ns
 * @param [out] nClockDivider     Pointer to store the optimum clock divider
 * @param [out] nInstructions     Pointer to store the optimum instruction count
 */
ADI_EAL_FIOPDM_CODE
static void CalcClkDividerInstructionNum(uint32_t nClockPeriod, uint16_t *nClockDivider, uint32_t *nInstructions)
{
    /*Default values for subsequent calculations*/
    uint16_t bestDivider = 4U;
    uint32_t bestInstructions = 3U;
    uint32_t minError = 0xFFFFFFFFU;

    /* Iterate over possible clock dividers */
    for(uint16_t divider = 4U; divider <= 100U; divider++)
    {
        /* Iterate over possible instruction counts */
        for(uint32_t instructions = 0U; instructions <= 0xFU; instructions++)
        {
            uint32_t totalInstructions = instructions + 3U;
            uint32_t period = totalInstructions * 2U * 10U * divider;
            /*Measures how close the combination gets to the required clock period*/
            uint32_t error = (period > nClockPeriod) ? (period - nClockPeriod) : (nClockPeriod - period);

            /*Update the minimum error, best divider and best instruction count if this combination is better*/
            if(error < minError)
            {
                minError = error;
                bestDivider = divider;
                bestInstructions = totalInstructions;

                /*If a perfect match is found, break out of the loop*/
                if(error == 0U)
                {
                    break;
                }
            }
        }

        /*Make the minimum error found so far available for the next iteration*/
        if(minError == 0U)
        {
            break;
        }
    }

    /*Updating the best clock divider and instruction count*/
    *nClockDivider = bestDivider;
    *nInstructions = bestInstructions;
}

/**
 * @brief       Configure the FIFO thresholds
 * @param [in]  hFioPdmHandle   Handle to the FIO-PDM instance
 * @return      ADI_EAL_STATUS   Status of the operation
 */
ADI_EAL_FIOPDM_CODE
static ADI_EAL_STATUS ConfigureFifoWriteThresholds(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle)
{
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;
    /* Get the unicast instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];
    ADI_EAL_STATUS eEalStatus;

    /* ~~~~~~ Write the thresholds register ~~~~~~ */
    /*Low threshold is set to 31.25% and high threshold is set to 71.875%*/
    /* Fill the address, mask and data fields */
    do
    {
        ADI_E2BCORE_REGMAP_IO_CONFIG oFifoRegCfg;
        (void)ADI_MEMSET(&oFifoRegCfg, 0, sizeof(oFifoRegCfg));
        uint16_t nStartAddr = FIFO_THRESHOLDS_NW_ADDR;
        uint32_t nMask = ((uint32_t)BITM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO |
                          BITM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO);
        uint32_t nData = ((uint32_t)ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_10 <<
                          BITP_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO) |
                         ((uint32_t)ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_23 <<
                          BITP_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO);

        /* Update the register map message structure */
        oFifoRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oFifoRegCfg.nLength = 1U;
        oFifoRegCfg.anStartAddr = &nStartAddr;
        oFifoRegCfg.anMask = &nMask;
        oFifoRegCfg.pBuffer = &nData;

        /* Write to the FIFO register map */
        eEalStatus = adi_eal_fioPdmSendRegTransaction((ADI_EAL_FIO_PDM_HANDLE)poEalFioPdmData, true, &oFifoRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(gaoEalData[poEalFioPdmInstData->oId.nInstNum].nDevNum, 0ULL);

        /* ~~~~~~ Also, configure the appropriate interrupts ~~~~~~ */

        /* Fill the address, mask and data fields */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRemoteRegCfg;
        (void)ADI_MEMSET(&oRemoteRegCfg, 0, sizeof(oRemoteRegCfg));
        nStartAddr = (uint16_t)(IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_NW_ADDR + (4U * poEalFioPdmInstData->oId.nIntfNum));
        nMask = BITM_IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_TIC_ENABLE_1_RISING |
                BITM_IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_TIC_ENABLE_1_FALLING;
        nData = (uint32_t)1U << (BITP_IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_TIC_ENABLE_1_FALLING +
                                 ENUM_IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_TIC_ENABLE_1_FALLING_FIFO0_WRITE_THRESHOLD_1_FALLING) |
                (uint32_t)1U << (BITP_IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_TIC_ENABLE_1_RISING +
                                 ENUM_IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_TIC_ENABLE_1_RISING_FIFO0_WRITE_THRESHOLD_2_RISING);

        /* Update the register map message structure */
        oRemoteRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRemoteRegCfg.nLength = 1U;
        oRemoteRegCfg.anStartAddr = &nStartAddr;
        oRemoteRegCfg.anMask = &nMask;
        oRemoteRegCfg.pBuffer = &nData;

        /* Write to the LV IO register map */
        eEalStatus = adi_eal_sendRegTransaction(poEalFioPdmInstData->oId.nInstNum, poEalFioPdmInstData->oId.nRemoteNum,
                                                &oRemoteRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(gaoEalData[poEalFioPdmInstData->oId.nInstNum].nDevNum, 0ULL);
    } while(false);

    return eEalStatus;
}

/**
 * @brief       Configure the RX FIFO Full event routed to application
 * @param [in]  hFioPdmHandle   Handle to the FIO-PDM instance
 * @return      ADI_EAL_STATUS   Status of the operation
 */
ADI_EAL_FIOPDM_CODE
static ADI_EAL_STATUS ConfigureFifoRxFull(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle)
{
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)hFioPdmHandle;
    /* Get the unicast instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];
    ADI_EAL_STATUS eEalStatus;
    /* Initialize the register configuration*/
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));

    /* Fill the address, mask and data fields */
    do
    {
        uint16_t nRegAddr[3U] = {IO_TIC_CONTROL0_NW_ADDR, IO_TIC_CONTROL1_NW_ADDR, IO_TIC_ENABLE_0_FIFO0_READ_REG0_NW_ADDR};
        uint32_t nRegMask[3U] = {BITM_IO_TIC_CONTROL0_TIC_RESEND_PERIOD, BITM_IO_TIC_CONTROL1_TIC_RESEND_PERIOD, BITM_IO_TIC_ENABLE_0_FIFO0_READ_REG0_TIC_ENABLE_0_RISING};
        uint32_t nRegVal[3U] = {BITM_IO_TIC_CONTROL0_TIC_RESEND_PERIOD, BITM_IO_TIC_CONTROL1_TIC_RESEND_PERIOD, (1U << ENUM_IO_TIC_ENABLE_0_FIFO0_READ_REG0_TIC_ENABLE_0_RISING_FIFO0_READ_FULL_RISING)};
        oRegCfg.anStartAddr = &nRegAddr[0u];
        oRegCfg.anMask = &nRegMask[0u];
        oRegCfg.pBuffer = &nRegVal[0u];
        oRegCfg.nLength = 3u;
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        /* Write to the LV IO register map */
        eEalStatus = adi_eal_sendRegTransaction(poEalFioPdmInstData->oId.nInstNum, poEalFioPdmInstData->oId.nRemoteNum,
                                                &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(gaoEalData[poEalFioPdmInstData->oId.nInstNum].nDevNum, 0ULL);
    } while(false);

    return eEalStatus;
}

/** Adds the transaction to queue for all instances */
ADI_EAL_FIOPDM_CODE
static ADI_E2BCORE_ERR AddTransactionToQueue(EAL_FIO_PDM_DATA *poEalFioPdmData, bool bIsMcast)
{
    ADI_E2BCORE_ERR eError = ADI_E2BCORE_ERR_NONE;

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[i];

        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg = &poEalFioPdmInstData->paoCfgs[poEalFioPdmInstData->nFioPdmQWrIdx];

        /* Add the configuration to e2bcore queue */
        if(bIsMcast)
        {
            eError = adi_e2bcore_addFlexibleIOConfigToQueue(&poEalFioPdmInstData->oId, poInstTxnCfg);
            if(eError != ADI_E2BCORE_ERR_NONE)
            {
                break;
            }
        }

        /* Increment queue index */
        poEalFioPdmInstData->nFioPdmQWrIdx = EAL_INC_Q_IDX(poEalFioPdmInstData->nFioPdmQWrIdx,
                                             poEalFioPdmInstData->nFioPdmCfgQDepth);
    }

    return eError;
}


/** Check and add transaction to queues */
ADI_EAL_FIOPDM_CODE
static ADI_EAL_STATUS CheckAndAddTransactionToQueues(EAL_FIO_PDM_DATA *poEalFioPdmData,
        EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData,
        bool bIsMcast, ADI_E2BCORE_TXN_CONFIG **ppoFioPdmTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Check if space is available in EAL transaction queue */
        if(!IsSpaceAvblInEalTxnQueue(poEalFioPdmData))
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Check if space is available in e2bcore transaction queue */
        if(bIsMcast && !IsSpaceAvblInE2bcoreTxnQueue(poEalFioPdmData))
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Copy the transaction configuration to queue of all instances */
        CopyTransactionConfigToQueue(poEalFioPdmData, *ppoFioPdmTxnCfg);

        /* If it is unicast, use the transaction configuration of the queue */
        if(!bIsMcast)
        {
            *ppoFioPdmTxnCfg = &poEalFioPdmInstData->paoCfgs[poEalFioPdmInstData->nFioPdmQWrIdx];
        }
    } while(false);

    return eEalStatus;
}

/** Handle error callback */
ADI_EAL_FIOPDM_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)pCbParam;

    /* Get the unicast instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalFioPdmInstData->bIgnoreErr)
    {
        InvokeCbk(poEalFioPdmData, poEalFioPdmInstData, ADI_EAL_FIO_PDM_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_FIOPDM_CODE
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
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)pCbParam;

    /* Get the unicast instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* If the read is of the FIO STATUS registers */
    if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FLEXIO_NUM_BYTES_NW_ADDR) &&
            (poRegCfg->nLength == 2U))
    {
        /* Process FIO status data */
        ADI_EAL_FIO_STATUS_DATA oFioPdmStatusData;
        ExtractFioPdmStatus(poRegCfg->pBuffer, &oFioPdmStatusData);
        InvokeCbk(poEalFioPdmData, poEalFioPdmInstData, ADI_EAL_FIO_PDM_CBK_EVT_INTF_STATUS_EVT, &oFioPdmStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalFioPdmData, poEalFioPdmInstData, ADI_EAL_FIO_PDM_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalFioPdmData, poEalFioPdmInstData, ADI_EAL_FIO_PDM_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_FIOPDM_CODE
static void HandleFifoReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFioPdm = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal data of this instance */
    EAL_FIO_PDM_DATA *poEalFioPdmData = (EAL_FIO_PDM_DATA *)pCbParam;

    /* Get the unicast instance data */
    EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[0U];

    /* Make sure that there is some response expected */
    ADI_DBG_REQUIRE(poEalFioPdmInstData->nFioPdmQRdIdx != poEalFioPdmInstData->nFioPdmQWrIdx, "Invalid index detected");

    if(poEalFioPdmInstData->eMode == ADI_EAL_USAGE_MODE_NORMAL)
    {
        /*If not periodic just give as read response event*/
        InvokeCbk(poEalFioPdmData, poEalFioPdmInstData, ADI_EAL_FIO_PDM_CBK_EVT_READ_RESP, poFioPdm);

    }
    else if(poEalFioPdmInstData->eMode == ADI_EAL_USAGE_MODE_PERIODIC)
    {
        /* Give a callback with periodic RX response event */
        InvokeCbk(poEalFioPdmData, poEalFioPdmInstData, ADI_EAL_FIO_PDM_CBK_EVT_PERIODIC_RX, poFioPdm);
        if(adi_e2bcore_addFlexibleIOConfigToQueue(poID, poFioPdm) != ADI_E2BCORE_ERR_NONE)
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to add fio pdm config to queue");
        }
    }
    else
    {
        // Nothing to do
    }

}

/** Extract FIO PDM status data */
ADI_EAL_FIOPDM_CODE
static void ExtractFioPdmStatus(uint32_t *panData, ADI_EAL_FIO_STATUS_DATA *poStatusData)
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

/** Invoke the callback */
ADI_EAL_FIOPDM_CODE
static void InvokeCbk(EAL_FIO_PDM_DATA *poEalFioPdmData, EAL_FIO_PDM_INST_DATA *poEalFioInstPdmData,
                      ADI_EAL_FIO_PDM_CBK_EVT eEvt, const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalFioInstPdmData->pfunCbk != NULL)
    {
        (poEalFioInstPdmData->pfunCbk)(poEalFioInstPdmData->oId.nInstNum, poEalFioPdmData, eEvt, pArg);
    }
}

/** Checks if space is available in transaction queue of all EAL instances */
ADI_EAL_FIOPDM_CODE
static bool IsSpaceAvblInEalTxnQueue(EAL_FIO_PDM_DATA *poEalFioPdmData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[i];

        /* Check if there is space in the EAL queue */
        if(EAL_SLOTS_FREE_IN_Q(poEalFioPdmInstData->nFioPdmQWrIdx, poEalFioPdmInstData->nFioPdmQRdIdx,
                               poEalFioPdmInstData->nFioPdmCfgQDepth) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/** Checks if space is available in transaction queue of all e2bcore instances */
ADI_EAL_FIOPDM_CODE
static bool IsSpaceAvblInE2bcoreTxnQueue(EAL_FIO_PDM_DATA *poEalFioPdmData)
{
    bool bIsSpaceAvbl = true;

    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[i];

        /* Check if there is space in the e2bcore queue */
        if(adi_e2bcore_slotsLeftInFlexibleIOQueue(&poEalFioPdmInstData->oId) == 0U)
        {
            bIsSpaceAvbl = false;
            break;
        }
    }

    return bIsSpaceAvbl;
}

/*! Returns if response is expected based on transaction and instance configuration */
ADI_EAL_FIOPDM_CODE
static bool IsResponseExpected(EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData,
                               ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg)
{
    /* Response is expected for a downstream transaction if
       - The transaction has e2b_id set, or
       - The transaction has read length greater than 0, or
       - The upstream acquisition timestamp is enabled */
    return (poTxnCfg->oTxnHdr.bIsE2bIdPresent ||
            (poTxnCfg->oTxnHdr.nReadLen > 0U) ||
            poEalFioPdmInstData->bIsReadTimeTagEn);
}

/** Copies transaction configuration to the queue of all EAL instances */
ADI_EAL_FIOPDM_CODE
static void CopyTransactionConfigToQueue(EAL_FIO_PDM_DATA *poEalFioPdmData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg)
{
    /* Loop through all the unicast instances */
    for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_FIO_PDM_INST_DATA *poEalFioPdmInstData = poEalFioPdmData->apoInstData[i];

        /* Copy the transaction config to the queue */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg = &poEalFioPdmInstData->paoCfgs[poEalFioPdmInstData->nFioPdmQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poTxnCfg, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
    }
}

/*! Lock the mutex of all FIO_PDM instances */
ADI_EAL_FIOPDM_CODE
static void LockFioPdmMutex(EAL_FIO_PDM_DATA *poEalFioPdmData)
{
    /* Loop through all the unicast FIO_PDM instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalFioPdmData->apoInstData[i]->hFioPdmMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all FIO_PDM instances */
ADI_EAL_FIOPDM_CODE
static void UnlockFioPdmMutex(EAL_FIO_PDM_DATA *poEalFioPdmData)
{
    /* Loop through all the unicast FIO_PDM instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalFioPdmData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalFioPdmData->apoInstData[i]->hFioPdmMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}


#endif /* ADI_E2B_IFACE_FIO_PDM_ENABLED */
/** @endcond */

/** @} */

/**
 * EOF: www.analog.com
 */
