/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: adi_eal_fioPwm.c
 * @brief: The EAL APIs for FIO-PWM interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_FIO_PWM_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalFioPwmData) \
ADI_DBG_REQUIRE((poEalFioPwmData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalFioPwmData)->bInUse == true, "Instance is not opened");

/* States */
#define EAL_FIO_PWM_STATE_PWM_STOP              (0U)
#define EAL_FIO_PWM_STATE_SEND_PWM_DATA         (1U)
#define EAL_FIO_PWM_STATE_WAIT_FOR_INTERVAL     (2U)
#define EAL_FIO_PWM_STATE_WAIT_FOR_FIFO_SPACE   (3U)

/*================================ DATA TYPES ===============================*/
/* PWM update data structure */
typedef struct EAL_FIO_PWM_UPDATE_DATA
{
    uint32_t     nTimePeriod;   /* Time period */
    uint32_t     nHighPeriod1;  /* High period of PWM output 1 */
    uint32_t     nHighPeriod2;  /* High period of PWM output 2 */
    uint32_t     nHighPeriod3;  /* High period of PWM output 3 */
} EAL_FIO_PWM_UPDATE_DATA;

/*=================================== DATA ==================================*/
/* Instruction memory contents for configuring FIO as PWM */
ADI_MEM_DATA_CRIT_CACHE
static uint32_t ganInstructionMemData[] =
{
    0xc420f840U,
    0x8340f840U,
    0x8540d520U,
    0x30c138c1U,
    0xf8408340U,
    0xf8408540U,
    0xf020f420U,
    0x30c038c0U
};

/*================================ PROTOTYPES ===============================*/
static EAL_FIO_PWM_UPDATE_DATA GetRawPwmUpdateData(ADI_EAL_FIO_PWM_OUTPUT_CFG *poPwmConfig);
static void FillPwmUpdateInTransaction(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, uint16_t nTxnIdx,
                                       EAL_FIO_PWM_UPDATE_DATA *poPwmUpdate);
static bool SendPwmDataUpdate(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, bool bMailboxMode, bool bResyncPending);
static void TransitionState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, uint8_t nCurrState, uint8_t nNextState);
static void HandlePwmStopState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData);
static void HandleSendPwmDataState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, bool bMailboxMode, bool bResyncPending);
static void HandleWaitForIntervalState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData);
static void HandleWaitForFifoSpaceState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData);
static void HandleErrorCallback(void *poParam, void *pCbParam);
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void HandleFifoReadCallback(void *poParam, void *pCbParam);
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam);
static void ExtractFioPwmStatus(uint32_t *panData, ADI_EAL_FIO_STATUS_DATA *poStatusData);
static void InvokeCbk(EAL_FIO_PWM_DATA *poEalFioPwmData, EAL_FIO_PWM_INST_DATA *poEalFioInstPwmData,
                      ADI_EAL_FIO_PWM_CBK_EVT eEvt, const void *pArg);
static void LockFioPwmMutex(EAL_FIO_PWM_DATA *poEalFioPwmData);
static void UnlockFioPwmMutex(EAL_FIO_PWM_DATA *poEalFioPwmData);

/** @endcond */
/*=================================== CODE ==================================*/
/**
 * @brief       Opens an instance of FIO-PWM interface
 *
 * @param [in]  poFioPwmCfg     Configurations for this FIO-PWM instance
 * @param [out] phFioPwmHandle  Handle corresponding to this FIO-PWM instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-PWM instance successfully opened
 *              - #ADI_EAL_STATUS_OPEN_FAILED: Failed to configure the transaction config queue
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmOpen(ADI_EAL_FIO_PWM_CFG *poFioPwmCfg, ADI_EAL_FIO_PWM_HANDLE *phFioPwmHandle)
{
    /* Basic parameter checks */
    ADI_DBG_REQUIRE(poFioPwmCfg != NULL, "Pointer to the FIO_PWM module configuration is NULL");
    ADI_DBG_REQUIRE(phFioPwmHandle != NULL, "Pointer to the FIO_PWM handle is NULL");

    /* Validation of instance, remote and interface numbers */
    ADI_DBG_REQUIRE(poFioPwmCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                    "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poFioPwmCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                    "Remote number is greater than max number of nodes supported");
    ADI_DBG_REQUIRE(poFioPwmCfg->nIntfNum < 4U,
                    "Interface number is greater than max number of interfaces supported");
    uint32_t nInstNum = poFioPwmCfg->nInstNum;
    uint32_t nRemoteNum = poFioPwmCfg->nRemoteNum;
    uint32_t nIntfNum = poFioPwmCfg->nIntfNum;
    ADI_E2BCORE_IF_CONFIG *poIfConfig =
        &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum];

    /* More validations based on state and network configuration */
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
    ADI_DBG_REQUIRE(poIfConfig->eIfType == ADI_E2BCORE_IF_TYPE_FIO,
                    "The given pair of instance, remote and interface numbers is not configured as FIO_PWM interface");

    /* Validate the memory provided */
    ADI_DBG_REQUIRE(poFioPwmCfg->pEalFioPwmMemory != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(poFioPwmCfg->nEalFioPwmMemorySize >=
                    ADI_EAL_FIO_PWM_UNICAST_INST_MEM_SIZE(poFioPwmCfg->nFioPwmCfgQDepth),
                    "Allocated memory less than required value");

    /* Allocate memory for the internal data structures from the provided memory */
    uint8_t *pMem = poFioPwmCfg->pEalFioPwmMemory;

    /* Allocate memory for the FIO PWM transaction config queue. */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioPwmTxnCfgs = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)((void *)pMem);
    pMem += (poFioPwmCfg->nFioPwmCfgQDepth * sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

    /* Allocate memory for the EAL FIO_PWM instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = (EAL_FIO_PWM_INST_DATA *)((void *)pMem);
    (void)ADI_MEMSET(poEalFioPwmInstData, 0, sizeof(EAL_FIO_PWM_INST_DATA));
    pMem += sizeof(EAL_FIO_PWM_INST_DATA);

    /* Instance info */
    poEalFioPwmInstData->oId.nInstNum = poFioPwmCfg->nInstNum;
    poEalFioPwmInstData->oId.nRemoteNum = poFioPwmCfg->nRemoteNum;
    poEalFioPwmInstData->oId.nIntfNum = poFioPwmCfg->nIntfNum;
    poEalFioPwmInstData->pnIfRegVlanTag = &poIfConfig->nIfRegVlanTag;
    poEalFioPwmInstData->pnFifoRegVlanTag = &poIfConfig->nFifoRegVlanTag;
    poEalFioPwmInstData->pnFifoWriteVlanTag = &poIfConfig->nFifoWriteVlanTag;

    /* State info */
    poEalFioPwmInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;
    poEalFioPwmInstData->eFioPwmOpMode = poFioPwmCfg->eFioPwmOpMode;
    poEalFioPwmInstData->bIsConfigured = false;
    poEalFioPwmInstData->bRunPwm = false;
    poEalFioPwmInstData->bLowIRQ = false;
    poEalFioPwmInstData->bHighIRQ = false;
    poEalFioPwmInstData->nSendTime = 0ULL;
    poEalFioPwmInstData->nState = EAL_FIO_PWM_STATE_PWM_STOP;

    /* Transaction payload info */
    poEalFioPwmInstData->nPwmUpdateInterval = 0U;
    poEalFioPwmInstData->pFioTxnPayloadMem = poFioPwmCfg->pFioTxnPayloadMem;
    poEalFioPwmInstData->nFioTxnPayloadMemSize = poFioPwmCfg->nFioTxnPayloadMemSize;
    poEalFioPwmInstData->nNumPackedTxns = 0U;
    poEalFioPwmInstData->nE2bId = 0U;

    /* Transaction config queue info */
    uint32_t nFioCtrlReg = 0U;
    EAL_FIO_PWM_INST_DATA **apoInstData;
    EAL_FIO_PWM_DATA *poEalFioPwmData;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    if(eal_utils_getConfigInfRegVal(&poEalFioPwmInstData->oId, FLEXIO_CONTROL_NW_ADDR, &nFioCtrlReg, false) == true)
    {
        eStatus = ADI_EAL_STATUS_SUCCESS;
    }
    poEalFioPwmInstData->bIsReadTimeTagEn = (nFioCtrlReg & BITM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG) != 0U;
    poEalFioPwmInstData->nFioPwmCfgQDepth = poFioPwmCfg->nFioPwmCfgQDepth;
    poEalFioPwmInstData->nFioPwmQWrIdx = 0U;
    poEalFioPwmInstData->nFioPwmQRdIdx = 0U;
    poEalFioPwmInstData->paoCfgs = paoFioPwmTxnCfgs;

    /* Allocate memory for the array of transaction config pointers for e2bcore interface */
    if((eStatus == ADI_EAL_STATUS_SUCCESS)
            && (adi_e2bcore_setInterfaceTransactionConfigQueue(&poEalFioPwmInstData->oId, (void *)pMem,
            poFioPwmCfg->nFioPwmCfgQDepth) != ADI_E2BCORE_ERR_NONE))
    {
        eStatus = ADI_EAL_STATUS_OPEN_FAILED;
    }
    pMem += (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *) * poFioPwmCfg->nFioPwmCfgQDepth);

    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /* Allocate memory for the array of unicast instance data pointers */
        apoInstData = (EAL_FIO_PWM_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_FIO_PWM_INST_DATA *);
        apoInstData[0U] = poEalFioPwmInstData;

        /* Allocate memory for the internal EAL FIO_PWM data structure */
        poEalFioPwmData  = (EAL_FIO_PWM_DATA *)((void *)pMem);
        pMem += sizeof(EAL_FIO_PWM_DATA);
        poEalFioPwmData->bIsMulticast  = false;
        poEalFioPwmData->bInUse        = true;
        poEalFioPwmData->nIntfTopic    = 0U;
        poEalFioPwmData->nFifoTopic    = 0U;
        poEalFioPwmData->nNumInst      = 1U;
        poEalFioPwmData->apoInstData   = apoInstData;
        poEalFioPwmData->bMailboxMode  = false;
        poEalFioPwmData->bResyncPending = false;
        /* Initialise the mutex for this FIO PWM instance */
        poEalFioPwmInstData->oFioPwmMutexCfg.pMem = &poEalFioPwmInstData->anFioPwmMutexBuffer[0U];
        poEalFioPwmInstData->oFioPwmMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalFioPwmInstData->oFioPwmMutexCfg, &poEalFioPwmInstData->hFioPwmMutex);

        /* Initialize SMC structure */
        /* Init FIO-PWM SMC region-1 in running state after SMC enable*/
        poEalFioPwmInstData->nPwmSmcRegionInUse = poFioPwmCfg->nPwmSmcRegion1;
        /* Init FIO-PWM SMC region number 1 & 2 */
        poEalFioPwmInstData->nPwmSmcRegion1 = poFioPwmCfg->nPwmSmcRegion1;
        poEalFioPwmInstData->nPwmSmcRegion2 = poFioPwmCfg->nPwmSmcRegion2;

        /* Set the callback param in the e2bcore interface config */
        poIfConfig->pCbParam = poEalFioPwmData;

        /* Set the interface type in EAL node level data */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfType[nIntfNum] = EAL_INTERFACE_TYPE_FIO_PWM;

        /* Return the internal FIO-PWM instance data as
           the handle for use in other functions */
        *phFioPwmHandle = (ADI_EAL_FIO_PWM_HANDLE)poEalFioPwmData;
    }

    return eStatus;
}

/**
 * @brief       Closes the opened FIO-PWM interface instance
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-PWM instance successfully closed
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmClose(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    /* Mutex cleanup */
    for(uint16_t i = 0U; i < poEalFioPwmData->nNumInst; ++i)
    {
        (void)prj_osal_destroyMutex(poEalFioPwmData->apoInstData[i]->hFioPwmMutex);
    }

    /* Clear data of this instance */
    (void)ADI_MEMSET(poEalFioPwmData, 0, sizeof(EAL_FIO_PWM_DATA));

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Register a callback function for getting notified of events
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @param [in]  pfFioPwmCallback    Pointer to the callback function
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Callback registered successfully
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmRegisterCallback(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_CALLBACK_FUNC pfFioPwmCallback)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);
    ADI_DBG_REQUIRE(pfFioPwmCallback != NULL, "Function pointer is NULL");
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "Cannot register callback for multicast handles");

    /* Get the unicast instance data from the handle */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Register the callback function */
    poEalFioPwmInstData->pfunCbk = pfFioPwmCallback;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Configure the Flexible IO instruction RAM
 *
 * @param [in] hFioPwmHandle  Handle corresponding to this FIO-PWM instance
 * @param [in] poFioInstructionRamData pointer to instruction RAM data to be programmed
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Updated instruction ram successfully
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmConfigureInstructionRAM(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_INST_RAM_DATA *poFioInstructionRamData)
{
    /* Extract the FIO-PWM data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Extract the FIO-PWM instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

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
        /* Set the instruction ram entries */
        oFioInstRamData.panInstructionMemData = &ganInstructionMemData[0U];
        oFioInstRamData.nNumInstMemEntries = (uint16_t)(sizeof(ganInstructionMemData) / sizeof(ganInstructionMemData[0U]));

        /* TX FIFO to SM routing is not fixed */
        oFioInstRamData.bFifoTxRouterFixedSm = false;

        /* Number of active SMs */
        oFioInstRamData.nNumActiveStateMachines = 4U;

        /* Configure SM0-SM2 - Controls the 3 PWM outputs */
        for(uint8_t i = 0U; i < 3U; i++)
        {
            oFioInstRamData.aoSmCfgs[i].bDelaySidesetSingleMode = true;
            oFioInstRamData.aoSmCfgs[i].bEnableAutopush = false;
            oFioInstRamData.aoSmCfgs[i].bEnableAutopull = false;
            oFioInstRamData.aoSmCfgs[i].bOsrShiftRight = true;
            oFioInstRamData.aoSmCfgs[i].bIsrShiftRight = true;
            oFioInstRamData.aoSmCfgs[i].nPinOutBasepin = 0U;
            oFioInstRamData.aoSmCfgs[i].nPinInBasepin = 0U;
            oFioInstRamData.aoSmCfgs[i].nSidesetBasepin = i;
            oFioInstRamData.aoSmCfgs[i].nJmpBasepin = 0U;
            oFioInstRamData.aoSmCfgs[i].nProgramStart = 0U;
            oFioInstRamData.aoSmCfgs[i].nProgramSize = 8U;
            oFioInstRamData.aoSmCfgs[i].nDelaySidesetBits = 1U;
            oFioInstRamData.aoSmCfgs[i].nAutopullThreshold = 0U;
            oFioInstRamData.aoSmCfgs[i].nAutopushThreshold = 0U;
            oFioInstRamData.aoSmCfgs[i].nPullThreshold = 0U;
            oFioInstRamData.aoSmCfgs[i].nPushThreshold = 0U;
            oFioInstRamData.aoSmCfgs[i].nClkDiv = 7U;
        }

        /* Configure SM3 - Controls the time period of the PWM pulses */
        oFioInstRamData.aoSmCfgs[3U].bDelaySidesetSingleMode = true;
        oFioInstRamData.aoSmCfgs[3U].bEnableAutopush = false;
        oFioInstRamData.aoSmCfgs[3U].bEnableAutopull = false;
        oFioInstRamData.aoSmCfgs[3U].bOsrShiftRight = true;
        oFioInstRamData.aoSmCfgs[3U].bIsrShiftRight = true;
        oFioInstRamData.aoSmCfgs[3U].nPinOutBasepin = 0U;
        oFioInstRamData.aoSmCfgs[3U].nPinInBasepin = 0U;
        oFioInstRamData.aoSmCfgs[3U].nSidesetBasepin = 0U;
        oFioInstRamData.aoSmCfgs[3U].nJmpBasepin = 0U;
        oFioInstRamData.aoSmCfgs[3U].nProgramStart = 8U;
        oFioInstRamData.aoSmCfgs[3U].nProgramSize = 8U;
        oFioInstRamData.aoSmCfgs[3U].nDelaySidesetBits = 0U;
        oFioInstRamData.aoSmCfgs[3U].nAutopullThreshold = 0U;
        oFioInstRamData.aoSmCfgs[3U].nAutopushThreshold = 0U;
        oFioInstRamData.aoSmCfgs[3U].nPullThreshold = 0U;
        oFioInstRamData.aoSmCfgs[3U].nPushThreshold = 0U;
        oFioInstRamData.aoSmCfgs[3U].nClkDiv = 7U;
    }

    /* Configure the Flexible IO registers */
    return eal_utils_configureFlexibleIO(&poEalFioPwmInstData->oId, &oFioInstRamData);
}

/**
 * @brief       Configure the FIO-PWM update rate
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @param [in]  poUpdateCfg     Update rate configurations
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmConfigureUpdateRate(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_UPDATE_RATE_CFG *poUpdateCfg)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);
    ADI_DBG_REQUIRE(poUpdateCfg != NULL, "Update configuration is NULL");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Lock the mutex to make sure the configurations
       are not updated in the middle of sending the PWM update */
    LockFioPwmMutex(poEalFioPwmData);

    /* Set the internal flags */
    poEalFioPwmInstData->nE2bId = poUpdateCfg->nE2bId;
    poEalFioPwmInstData->nNumPackedTxns = poUpdateCfg->nNumPackedTxns;
    poEalFioPwmInstData->nPwmUpdateInterval = poUpdateCfg->nPwmUpdateInterval;
    double fFifoLowThreshold = poUpdateCfg->fFifoLowThreshold * 32.0f / 100.0f;
    double fFifoHighThreshold = poUpdateCfg->fFifoHighThreshold * 32.0f / 100.0f;

    /* Round-off the thresholds to the nearest integer by adding 0.5 and flooring */
    fFifoLowThreshold += 0.5f;
    fFifoHighThreshold += 0.5f;

    do
    {
        /* ~~~~~~ Write the thresholds register ~~~~~~ */

        /* Fill the address, mask and data fields */
        ADI_E2BCORE_REGMAP_IO_CONFIG oFifoRegCfg;
        (void)ADI_MEMSET(&oFifoRegCfg, 0, sizeof(oFifoRegCfg));
        uint16_t nStartAddr = FIFO_THRESHOLDS_NW_ADDR;
        uint32_t nMask = BITM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO | BITM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO;
        uint32_t nData = (uint32_t)fFifoLowThreshold << BITP_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO |
                         (uint32_t)fFifoHighThreshold << BITP_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO;

        /* Update the register map message structure */
        oFifoRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oFifoRegCfg.nLength = 1U;
        oFifoRegCfg.anStartAddr = &nStartAddr;
        oFifoRegCfg.anMask = &nMask;
        oFifoRegCfg.pBuffer = &nData;

        /* Write to the FIFO register map */
        eEalStatus = adi_eal_fioPwmSendRegTransaction(poEalFioPwmData, true, &oFifoRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(gaoEalData[poEalFioPwmInstData->oId.nInstNum].nDevNum, 0ULL);

        /* ~~~~~~ Also, configure the appropriate interrupts ~~~~~~ */

        /* Fill the address, mask and data fields */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRemoteRegCfg;
        (void)ADI_MEMSET(&oRemoteRegCfg, 0, sizeof(oRemoteRegCfg));
        nStartAddr = (uint16_t)(IO_TIC_ENABLE_1_FIFO0_WRITE_REG0_NW_ADDR + (4U * poEalFioPwmInstData->oId.nIntfNum));
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
        eEalStatus = adi_eal_sendRegTransaction(poEalFioPwmInstData->oId.nInstNum, poEalFioPwmInstData->oId.nRemoteNum,
                                                &oRemoteRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(gaoEalData[poEalFioPwmInstData->oId.nInstNum].nDevNum, 0ULL);

    } while(false);

    /* Unlock the mutex */
    UnlockFioPwmMutex(poEalFioPwmData);

    return eEalStatus;
}

/**
 * @brief       Update the frequency and duty cycle in unbuffered use-case
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @param [in]  poPwmConfig     Frequency and duty cycle configurations
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully updated frequency and duty cycle
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmUpdateFreqDuty(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_OUTPUT_CFG *poPwmConfig)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);
    ADI_DBG_REQUIRE(poPwmConfig != NULL, "PWM configuration is NULL");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Convert freq and duty cycle to raw time period and
       high period values in terms of instruction cycles */
    EAL_FIO_PWM_UPDATE_DATA oPwmUpdate = GetRawPwmUpdateData(poPwmConfig);

    /* Lock the mutex to make sure the configurations
       are not updated in the middle of sending the PWM update */
    LockFioPwmMutex(poEalFioPwmData);

    /* Update the FIO-PWM transaction with the values provided */
    for(uint16_t i = 0U; i < poEalFioPwmInstData->nNumPackedTxns; i++)
    {
        FillPwmUpdateInTransaction(poEalFioPwmInstData, i, &oPwmUpdate);
    }

    /* Mark the payload as configured */
    poEalFioPwmInstData->bIsConfigured = true;

    /* Unlock the mutex */
    UnlockFioPwmMutex(poEalFioPwmData);

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Updates FIO-PWM  frequency and duty cycle data in idle SMC region
 * @param [in] anRegions       Pointer to the array of SMC regions config structure
 * @param [in] hFioPwmHandle   Handle corresponding to this instance
 * @param [in] poPwmConfig     Frequency and duty cycle configurations
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully updated frequency and duty cycle in idle SMC region
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmUpdateFreqDutySmcRegion(ADI_SMC_REGION_CONFIG anRegions[],
        ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_OUTPUT_CFG *poPwmConfig)
{
    uint8_t nRegionHeaderLen = ADI_VAL_ZERO;
    uint8_t nIndex = ADI_VAL_ZERO;
    uint16_t nRegionAddress = ADI_VAL_ZERO;
    uint16_t nRegAddr = ADI_VAL_ZERO;
    uint8_t nSmcPwmDataStartIndex = ADI_VAL_ZERO;
    uint8_t nRegionIndex = ADI_VAL_ZERO;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    /* Init SMC Base address */
    uint16_t nSmcBaseAddr = SMC_BLOCK_BASE_ADDR;
    /* Init PWM masked write data */
    uint16_t nStartAddr[5u];
    uint32_t nMask[5u] = {0XFFFFFFFFu, 0XFFFFFFFFu, 0XFFFF0000u, SMC_REGION_MASK_EN_CONFIG_A_B, SMC_REGION_MASK_EN_CONFIG_A_B};
    uint32_t nRegData[5u];
    uint8_t anPwmFreqDuty[12u];
    uint8_t *poPwmFreqDuty = anPwmFreqDuty;

    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    ADI_DBG_REQUIRE(poPwmConfig != NULL, "PWM configuration is NULL");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];


    /* Lock the mutex to make sure the configurations
       are not updated in the middle of sending the PWM update */
    LockFioPwmMutex(poEalFioPwmData);

    /* Check for current SMC region in use and get current idle SMC region index */
    if(poEalFioPwmInstData->nPwmSmcRegionInUse == poEalFioPwmInstData->nPwmSmcRegion1)
    {
        /* Set FIO-PWM SMC region-2 as idle region */
        nRegionIndex = poEalFioPwmInstData->nPwmSmcRegion2;
    }
    else
    {
        /* Set FIO-PWM SMC region-1 as idle region */
        nRegionIndex = poEalFioPwmInstData->nPwmSmcRegion1;
    }

    /* Get actual SMC idle region index in array by decrement nRegionIndex value by 1 */
    nRegionIndex--;

    /* Check for SMC region trigger mode is set to automatic */
    if(!(anRegions[nRegionIndex].bIsManuallyTriggered))
    {
        /* Calculate idle region SMC header length */
        CalcRegionHeaderLen(&anRegions[nRegionIndex], &nRegionHeaderLen);

        /* Get SMC idle region address */
        nRegionAddress = anRegions[nRegionIndex].nRegionAddress + nSmcBaseAddr;

        /* Calculate idle SMC region FIO-PWM Frequency duty cycle starting data index */
        nSmcPwmDataStartIndex = (uint8_t)(anRegions[nRegionIndex].nRegionDataLength - (uint16_t)
                                          ADI_EAL_FIO_PWM_FRE_DUTY_CYCLE_DATA_LEN);

        /* Calculate idle SMC region FIO-PWM Frequency duty cycle starting address */
        nRegAddr = nRegionAddress + (uint16_t)nRegionHeaderLen + (uint16_t)nSmcPwmDataStartIndex;

        /* Enable current SMC idle region by writing config A/B bits value to (1) in region header */
        nStartAddr[3] = nRegionAddress;
        nRegData[3] = SMC_REGION_EN_CONFIG_A;

        /* Disable current SMC region use by writing config A/B bits value to (0) in region header */
        nStartAddr[4] = (uint16_t)anRegions[poEalFioPwmInstData->nPwmSmcRegionInUse - (uint8_t)1u].nRegionAddress +
                        nSmcBaseAddr;
        nRegData[4] = SMC_REGION_DISABLE_CONFIG_A_B;

        /* Convert freq and duty cycle to raw time period and
           high period values in terms of instruction cycles */
        EAL_FIO_PWM_UPDATE_DATA oPwmUpdate = GetRawPwmUpdateData(poPwmConfig);

        /* Update the FIO-PWM transaction with the values provided */
        FillPwmUpdateInTransaction(poEalFioPwmInstData, ADI_VAL_ZERO, &oPwmUpdate);

        /* Copy the FIO-PWM Frequency and duty cycle transaction payload */
        (void)ADI_MEMCPY(poPwmFreqDuty, poEalFioPwmInstData->pFioTxnPayloadMem, 10U);

        /* Correcting the endianess, the data stored byte-wise in pFioTxnPayloadMem provided by the application is
        shifted according to the endianess required by the system and stored at the same location by typecasting
        the consecutive 4 bytes to a 32 byte location, since the regmap io config accepts the pointer for regdata as uint32_t type */
        for(nIndex = ADI_VAL_ZERO; nIndex < 3u; nIndex++)
        {
            nRegData[nIndex] = ((uint32_t)poPwmFreqDuty[0] << 24u) | ((uint32_t)poPwmFreqDuty[1] << 16u) |
                               ((uint32_t)poPwmFreqDuty[2] << 8u) | poPwmFreqDuty[3];
            nStartAddr[nIndex] = nRegAddr;
            nRegAddr += 4u;
            poPwmFreqDuty += 4u;
        }

        /* Do a masked write of the above configurations */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        oRegCfg.anStartAddr = &nStartAddr[0u];
        oRegCfg.nLength = 5U;
        oRegCfg.pBuffer = &nRegData[0u];
        oRegCfg.anMask = &nMask[0u];
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

        /* Send the regmap transaction */
        eEalStatus = adi_eal_sendRegTransaction(poEalFioPwmInstData->oId.nInstNum, poEalFioPwmInstData->oId.nRemoteNum,
                                                &oRegCfg);

        if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            /* Update FIO-PWM SMC region use with current idle region index */
            poEalFioPwmInstData->nPwmSmcRegionInUse = nRegionIndex + 1u;
        }
    }
    else
    {
        /* Set SMC configuration status as failed if SMC region trigger mode is not automatic */
        eEalStatus = ADI_EAL_STATUS_SMC_REGION_CONFIG_FAILED;
    }

    /* Unlock the mutex */
    UnlockFioPwmMutex(poEalFioPwmData);

    return eEalStatus;
}

/**
 * @brief       Update the frequency and duty cycle in buffered use-case
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @param [in]  paoPwmConfig    Frequency and duty cycle configurations (nNumPackTxn instances)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully updated frequency and duty cycle
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmUpdateFreqDutyBuffered(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_OUTPUT_CFG *paoPwmConfig)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);
    ADI_DBG_REQUIRE(paoPwmConfig != NULL, "PWM configuration is NULL");

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Lock the mutex to make sure the configurations
       are not updated in the middle of sending the PWM update */
    LockFioPwmMutex(poEalFioPwmData);

    for(uint16_t i = 0U; i < poEalFioPwmInstData->nNumPackedTxns; i++)
    {
        /* Convert freq and duty cycle to raw time period and
           high period values in terms of instruction cycles */
        EAL_FIO_PWM_UPDATE_DATA oPwmUpdate = GetRawPwmUpdateData(&paoPwmConfig[i]);

        /* Update the FIO-PWM transaction with the values provided */
        FillPwmUpdateInTransaction(poEalFioPwmInstData, i, &oPwmUpdate);
    }

    /* Mark the payload as configured */
    poEalFioPwmInstData->bIsConfigured = true;

    /* Unlock the mutex */
    UnlockFioPwmMutex(poEalFioPwmData);

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Starts the PWM output
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully started PWM
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmStart(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    poEalFioPwmInstData->bRunPwm = true;

    return eEalStatus;
}

/**
 * @brief       Stops the PWM output
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Successfully stopped PWM
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmStop(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    poEalFioPwmInstData->bRunPwm = false;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Ticks the FIO-PWM state machine
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 */
ADI_EAL_FIOPWM_CODE
void adi_eal_fioPwmRun(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    bool bMailboxMode = poEalFioPwmData->bMailboxMode;
    bool bResyncPending = poEalFioPwmData->bResyncPending;

    /* Lock the mutex to make sure the configurations
       are not updated in the middle of sending the PWM update */
    LockFioPwmMutex(poEalFioPwmData);

    /* Run the state machine based on the current state */
    switch(poEalFioPwmInstData->nState)
    {
        case EAL_FIO_PWM_STATE_PWM_STOP:
        {
            HandlePwmStopState(poEalFioPwmInstData);
            break;
        }

        case EAL_FIO_PWM_STATE_SEND_PWM_DATA:
        {
            HandleSendPwmDataState(poEalFioPwmInstData, bMailboxMode, bResyncPending);
            poEalFioPwmData->bResyncPending = false; // Cleared after use
            break;
        }

        case EAL_FIO_PWM_STATE_WAIT_FOR_INTERVAL:
        {
            HandleWaitForIntervalState(poEalFioPwmInstData);
            break;
        }

        case EAL_FIO_PWM_STATE_WAIT_FOR_FIFO_SPACE:
        {
            HandleWaitForFifoSpaceState(poEalFioPwmInstData);
            break;
        }

        default:
        {
            /* Shouldn't come here! */
            ADI_DBG_ERROR();
            break;
        }
    }

    /* Unlock the mutex */
    UnlockFioPwmMutex(poEalFioPwmData);
}

/**
 * @brief       Reads the status of the FIO PWM interface
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Read successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmReadStatus(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Extract the data of the first instance even if it is multicast */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    bool bIsMcast = poEalFioPwmData->bIsMulticast;
    uint32_t anTopics[2U];

    /* Set the topics based on whether it is a unicast or multicast handle */
    if(bIsMcast)
    {
        anTopics[0U] = poEalFioPwmData->nIntfTopic;
        anTopics[1U] = poEalFioPwmData->nFifoTopic;
    }
    else
    {
        anTopics[0U] = ADI_E2BCORE_TOPIC_INTF0 + poEalFioPwmInstData->oId.nIntfNum;
        anTopics[1U] = ADI_E2BCORE_TOPIC_FIFO0 + poEalFioPwmInstData->oId.nIntfNum;
    }

    /* Read the statuses */
    return eal_utils_readInfStatus(&poEalFioPwmInstData->oId, FLEXIO_NUM_BYTES_NW_ADDR, 2U, bIsMcast, anTopics,
                                   *poEalFioPwmInstData->pnIfRegVlanTag);
}

/**
 * @brief       Write to Flexible-IO PWM interface register
 *
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
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
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmSendRegTransaction(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);
    ADI_DBG_REQUIRE(poRegCfg != NULL, "Register transaction configuration pointer is NULL");

    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Get the instance number, remote number and interface number */
    uint32_t nInstNum = poEalFioPwmInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalFioPwmInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalFioPwmInstData->oId.nIntfNum;

    bool bIsMcast = poEalFioPwmData->bIsMulticast;

    /* Set the parameters for the message header */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    uint32_t nVlanTag;
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bMailbox       = poEalFioPwmData->bMailboxMode;
    oMsgOpts.bSync          = poEalFioPwmData->bResyncPending;
    oMsgOpts.bTopicOverRide = true;

    if(bIsFifoRegTransaction == true)
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioPwmData->nFifoTopic :
                          (ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum);
        nVlanTag = *poEalFioPwmInstData->pnFifoRegVlanTag;
    }
    else
    {
        oMsgOpts.nTopic = bIsMcast ?
                          poEalFioPwmData->nIntfTopic :
                          (ADI_E2BCORE_TOPIC_INTF0 + nIntfNum);
        nVlanTag = *poEalFioPwmInstData->pnIfRegVlanTag;
    }

    poEalFioPwmData->bResyncPending = false; // Cleared after use

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
 * @param [in]  hFioPwmHandle   Handle corresponding to this instance
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: FIO-PWM reset successful
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmResetQueue(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    /* Validate that we are not using this API with multicast handle */
    ADI_DBG_REQUIRE(poEalFioPwmData->bIsMulticast == false, "This API doesn't work with multicast handles");

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Lock the mutex */
    LockFioPwmMutex(poEalFioPwmData);

    do
    {
        /* Clear the config queue */
        adi_e2bcore_clearFlexibleIOConfigQueue(&poEalFioPwmInstData->oId);
        poEalFioPwmInstData->nFioPwmQWrIdx = 0U;
        poEalFioPwmInstData->nFioPwmQRdIdx = 0U;
        poEalFioPwmInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;

        /* Ignore errors */
        poEalFioPwmInstData->bIgnoreErr = true;

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
        eEalStatus = adi_eal_fioPwmSendRegTransaction((ADI_EAL_FIO_PWM_HANDLE)poEalFioPwmData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Send an FIO_PWM NOP resync transaction */
        eEalStatus = eal_utils_sendNopResyncTxn(&poEalFioPwmInstData->oId);

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
        eEalStatus = adi_eal_fioPwmSendRegTransaction((ADI_EAL_FIO_PWM_HANDLE)poEalFioPwmData, false, &oRegCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for another timeout */
        adi_network_flushTxTimed(200ULL * MS_TO_NS);
    } while(false);

    /* Stop ignoring errors */
    poEalFioPwmInstData->bIgnoreErr = false;

    /* Unlock the mutex */
    UnlockFioPwmMutex(poEalFioPwmData);

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tag for FIO PWM instance
 *
 * @param [in] hFioPwmHandle        FIO PWM instance handle
 * @param [in] poIfRegVlanTag       Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag     Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag   Pointer to VLAN tag for FIFO write access
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPWM_CODE
void adi_eal_fioPwmSetDownstreamVlan(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, ADI_VLAN_CFG *poIfRegVlanTag,
                                     ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

    ADI_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                    && poFifoWriteVlanTag != NULL, "Pointer to VLAN is NULL");

    /* Extract the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    uint32_t nInstNum = poEalFioPwmInstData->oId.nInstNum;
    uint32_t nRemoteNum = poEalFioPwmInstData->oId.nRemoteNum;
    uint32_t nIntfNum = poEalFioPwmInstData->oId.nIntfNum;

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

/**
 * @brief Sets the upstream VLAN ID to be added for FIO PWM FIFO messages
 *
 * @param [in] hFioPwmHandle  FIO PWM interface Handle
 * @param [in] poVlantag      Pointer to VLAN tag information - contains VLAN ID, drop eligible indicator and priority
 * @param [in] bIsVlanEn      If set true, enables the tagging of VLAN ID in upstream messages
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmSetUpstreamVlan(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, ADI_VLAN_CFG *poVlantag,
        bool bIsVlanEn)
{
    /* Get the internal data from the handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic parameter checks */
    BASIC_HANDLE_CHECK(poEalFioPwmData);

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

    return adi_eal_fioPwmSendRegTransaction(hFioPwmHandle, true, &oRegCfg);
}

/**
 * @brief       Enables or disables mailbox mode for FIO PWM transactions
 *
 * @param [in]  hFioPwmHandle   The FIO PWM handle
 * @param [in]  bEnable         Flag to enable/disable mailbox mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmSetMailboxMode(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, bool bEnable)
{
    /* Extract internal FIO-PWM data structure from the opaque handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic sanity checks on the handle */
    ADI_DBG_REQUIRE(poEalFioPwmData != NULL, "Handle is NULL");
    ADI_DBG_REQUIRE(poEalFioPwmData->bInUse == true, "Instance is not opened");

    /* Set or clear mailbox mode.
       When mailbox mode is enabled, outgoing FIFO/register messages are sent
       in mailbox (reliable/acked) mode. When disabled, messages are sent
       in normal non-mailbox mode. */
    poEalFioPwmData->bMailboxMode = bEnable;

    /* Indicate successful completion of the operation */
    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Enables sequence number resynchronization for FIO PWM transactions
 * @details     When enabled, the sync bit is set in the next outgoing message to request
 *              sequence number resynchronization. This is typically used after message drop
 *              failures in mailbox mode.
 *
 * @param [in]  hFioPwmHandle   The FIO PWM handle
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Sync mode setting was successful.
 *
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_FIOPWM_CODE
ADI_EAL_STATUS adi_eal_fioPwmResyncSeqNum(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle)
{
    /* Extract internal FIO-PWM data structure from opaque handle */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)hFioPwmHandle;

    /* Basic sanity checks on the handle */
    ADI_DBG_REQUIRE(poEalFioPwmData != NULL, "Handle is NULL");
    ADI_DBG_REQUIRE(poEalFioPwmData->bInUse == true, "Instance is not opened");

    /* Mark that a resynchronization is requested. The next outgoing
       message will be sent with the sync bit set to request sequence
       number resynchronization from the receiver. */
    poEalFioPwmData->bResyncPending = true;

    /* API completed successfully */
    return ADI_EAL_STATUS_SUCCESS;
}
/*================================ CALLBACKS ================================*/
/** FIO interface callback */
ADI_EAL_FIOPWM_CODE
void FioPwmInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                        void *pCbParam)
{
    /* If pCbParam is NULL, the EAL FIO-PWM interface instance is not opened,
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

/** Notify reception of High or Low IRQs */
ADI_EAL_FIOPWM_CODE
bool eal_notify_FioPwmInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
                                EAL_FIO_PWM_INTERRUPT_EVT eFioPwmEvt)
{
    /* Extract the interface internal data structure from the callback parameter */
    void *pInterface = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum].pCbParam;
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)pInterface;

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    bool bInterruptProcessed = false;

    /* The low threshold of the FIFO has been hit while waiting for the FIFO to be emptied */
    if(eFioPwmEvt == EAL_FIO_PWM_LOW_IRQ_EVT)
    {
        bInterruptProcessed = true;

        if(poEalFioPwmInstData->nState == EAL_FIO_PWM_STATE_WAIT_FOR_FIFO_SPACE)
        {
            poEalFioPwmInstData->bLowIRQ = true;
        }
    }
    /* The high threshold of the FIFO has been hit while filling up the FIFO */
    else if(eFioPwmEvt == EAL_FIO_PWM_HIGH_IRQ_EVT)
    {
        bInterruptProcessed = true;

        if(poEalFioPwmInstData->nState == EAL_FIO_PWM_STATE_WAIT_FOR_INTERVAL)
        {
            poEalFioPwmInstData->bHighIRQ = true;
        }
    }
    else
    {
        /* Nothing to do! */
    }

    return bInterruptProcessed;
}

/** @cond PRIVATE */
/*============================= STATIC FUNCTIONS ============================*/
/** Get the raw time period value from frequency and duty cycle */
ADI_EAL_FIOPWM_CODE
static EAL_FIO_PWM_UPDATE_DATA GetRawPwmUpdateData(ADI_EAL_FIO_PWM_OUTPUT_CFG *poPwmConfig)
{
    EAL_FIO_PWM_UPDATE_DATA oPwmUpdateData;

    /* Convert frequency to time period (in ns) */
    double fTimePeriod = 1.0e9f / poPwmConfig->fFrequency;

    /* Get the high period value for PWM output 1 */
    double fHighPeriod1 = (poPwmConfig->fDutyCycle1 / 100.0f) * fTimePeriod;

    /* Get the high period value for PWM output 2 */
    double fHighPeriod2 = (poPwmConfig->fDutyCycle2 / 100.0f) * fTimePeriod;

    /* Get the high period value for PWM output 3 */
    double fHighPeriod3 = (poPwmConfig->fDutyCycle3 / 100.0f) * fTimePeriod;

    /* Convert time period (in ns) to instruction cycles.
       NOTE: 1 instruction takes 70 ns */
    fTimePeriod = fTimePeriod / 70.0f;
    fHighPeriod1 = fHighPeriod1 / 70.0f;
    fHighPeriod2 = fHighPeriod2 / 70.0f;
    fHighPeriod3 = fHighPeriod3 / 70.0f;

    /* Round-off to the nearest integer by adding 0.5 and flooring */
    fTimePeriod += 0.5f;
    fHighPeriod1 += 0.5f;
    fHighPeriod2 += 0.5f;
    fHighPeriod3 += 0.5f;
    uint32_t nTimePeriodInInstrCycles = (uint32_t)fTimePeriod;
    uint32_t nHighPeriod1InInstrCycles = (uint32_t)fHighPeriod1;
    uint32_t nHighPeriod2InInstrCycles = (uint32_t)fHighPeriod2;
    uint32_t nHighPeriod3InInstrCycles = (uint32_t)fHighPeriod3;

    /* Fill the PWM update structure */
    oPwmUpdateData.nTimePeriod = nTimePeriodInInstrCycles;
    oPwmUpdateData.nHighPeriod1 = nHighPeriod1InInstrCycles;
    oPwmUpdateData.nHighPeriod2 = nHighPeriod2InInstrCycles;
    oPwmUpdateData.nHighPeriod3 = nHighPeriod3InInstrCycles;

    return oPwmUpdateData;
}

/** Fill the PWM update in the transaction payload */
ADI_EAL_FIOPWM_CODE
static void FillPwmUpdateInTransaction(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, uint16_t nTxnIdx,
                                       EAL_FIO_PWM_UPDATE_DATA *poPwmUpdate)
{
    /* Each PWM cycle takes 10 bytes. Based on the transaction index,
       compute the base index in the FIO-PWM transaction payload */
    uint16_t nBaseIdx = nTxnIdx * 10U;

    /* Compute the values for RegA and RegB for SM3 based on the time period */
    uint8_t nSm3RegA = (uint8_t)((poPwmUpdate->nTimePeriod - 7U) % 257U);
    uint8_t nSm3RegB = (uint8_t)(((poPwmUpdate->nTimePeriod - 7U) / 257U) + (nSm3RegA > 0U ? 1U : 0U));
    /* Compute the values for RegA and RegB for SM0-2 based on the high period */
    uint8_t nSm0RegA = (uint8_t)((poPwmUpdate->nHighPeriod1 - 1U) % 257U);
    uint8_t nSm0RegB = (uint8_t)(((poPwmUpdate->nHighPeriod1 - 1U) / 257U) + (nSm0RegA > 0U ? 1U : 0U));
    uint8_t nSm1RegA = (uint8_t)((poPwmUpdate->nHighPeriod2 - 1U) % 257U);
    uint8_t nSm1RegB = (uint8_t)(((poPwmUpdate->nHighPeriod2 - 1U) / 257U) + (nSm1RegA > 0U ? 1U : 0U));
    uint8_t nSm2RegA = (uint8_t)((poPwmUpdate->nHighPeriod3 - 1U) % 257U);
    uint8_t nSm2RegB = (uint8_t)(((poPwmUpdate->nHighPeriod3 - 1U) / 257U) + (nSm2RegA > 0U ? 1U : 0U));

    /* Routing header:
        - Route the first 2 bytes to SM0.
        - Route the next 2 bytes to SM1 */
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx] = 0x05U;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 1U] = nSm0RegA;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 2U] = nSm0RegB;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 3U] = nSm1RegA;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 4U] = nSm1RegB;
    /* Routing header:
        - Route the first 2 bytes to SM2.
        - Route the next 2 bytes to SM3 */
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 5U] = 0xAFU;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 6U] = nSm2RegA;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 7U] = nSm2RegB;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 8U] = nSm3RegA;
    poEalFioPwmInstData->pFioTxnPayloadMem[nBaseIdx + 9U] = nSm3RegB;
}

/** Sends PWM data update frame */
ADI_EAL_FIOPWM_CODE
static bool SendPwmDataUpdate(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, bool bMailboxMode, bool bResyncPending)
{
    /*~~~~~~~~~~ Local variables ~~~~~~~~~~*/
    uint32_t        nInstNum;
    uint32_t        nRemoteNum;
    uint32_t        nIntfNum;
    uint32_t        nVlanTag;
    ADI_EAL_STATUS  eEalStatus = ADI_EAL_STATUS_SUCCESS;
    ADI_NETWORK_FRAME_CFG   oNwFrameCfg;
    ADI_E2BCORE_MSGOPTS     oFifoMsgOpts;
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG  oTxnCfg;
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG  *poTxnCfg = &oTxnCfg;

    /* Get the instance number and remote number */
    nInstNum = poEalFioPwmInstData->oId.nInstNum;
    nRemoteNum = poEalFioPwmInstData->oId.nRemoteNum;
    nIntfNum = poEalFioPwmInstData->oId.nIntfNum;

    /* Set the parameters for getting a buffer from network layer
       and transmitting the e2b packet */
    nVlanTag = *poEalFioPwmInstData->pnFifoWriteVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the parameters for the fifo message header */
    (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
    oFifoMsgOpts.bMailbox       = bMailboxMode;
    oFifoMsgOpts.bSync          = bResyncPending;
    oFifoMsgOpts.bTopicOverRide = true;
    oFifoMsgOpts.nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + nIntfNum;

    do
    {
        /* If ID is used, the config should be persistent till
           the callback is reached, so it needs to be taken from
           the config queue pool */
        bool bIsPersistent = (poEalFioPwmInstData->nE2bId != 0xFFu) ||
                             (poEalFioPwmInstData->bIsReadTimeTagEn);

        if(bIsPersistent)
        {
            /* See if there is space available in the queue */
            if(EAL_IS_Q_FULL(poEalFioPwmInstData->nFioPwmQWrIdx, poEalFioPwmInstData->nFioPwmQRdIdx,
                             poEalFioPwmInstData->nFioPwmCfgQDepth))
            {
                eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            }

            poTxnCfg = &poEalFioPwmInstData->paoCfgs[poEalFioPwmInstData->nFioPwmQWrIdx];
        }

        if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            /* Set the Flexible IO transaction configuration */
            (void)ADI_MEMSET(poTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

            poTxnCfg->oTxnHdr.bAreTxnsPacked    = true;
            poTxnCfg->oTxnHdr.bEnIntfTxnHdr     = true;
            poTxnCfg->oTxnHdr.bIsE2bIdPresent   = poEalFioPwmInstData->nE2bId != 0xFFU;
            poTxnCfg->oTxnHdr.nNumPackedTxns    = poEalFioPwmInstData->nNumPackedTxns;
            poTxnCfg->oTxnHdr.nE2bId            = poEalFioPwmInstData->nE2bId;
            poTxnCfg->oTxnHdr.bOverrideWriteLen = true;
            poTxnCfg->oTxnHdr.bOverrideReadLen  = true;
            poTxnCfg->oTxnHdr.bIsRouterHeaderInserted = true;
            poTxnCfg->oTxnHdr.nWriteLen         = 10U;
            poTxnCfg->oTxnHdr.nReadLen          = 0U;
            poTxnCfg->pPayload                  = poEalFioPwmInstData->pFioTxnPayloadMem;

            /* Send the FIFO frame */
            eEalStatus = eal_utils_sendFifoFrame(&poEalFioPwmInstData->oId, &oNwFrameCfg, poTxnCfg, &oFifoMsgOpts);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }

            if(bIsPersistent)
            {
                /* Increment queue index */
                poEalFioPwmInstData->nFioPwmQWrIdx = EAL_INC_Q_IDX(poEalFioPwmInstData->nFioPwmQWrIdx,
                                                     poEalFioPwmInstData->nFioPwmCfgQDepth);
            }
        }
    } while(false);

    return eEalStatus == ADI_EAL_STATUS_SUCCESS;
}

/** Transition state */
ADI_EAL_FIOPWM_CODE
static void TransitionState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, uint8_t nCurrState, uint8_t nNextState)
{
    /* ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Transitioning from %d to %d\r\n", nCurrState, nNextState); */
    poEalFioPwmInstData->nState = nNextState;
}

/** Handle the PWM_STOP state */
ADI_EAL_FIOPWM_CODE
static void HandlePwmStopState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData)
{
    /* If PWM start has been invoked, start sending the updates */
    if(poEalFioPwmInstData->bRunPwm)
    {
        TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_PWM_STOP, EAL_FIO_PWM_STATE_SEND_PWM_DATA);
    }
}

/** Handle the SEND_PWM_DATA state */
ADI_EAL_FIOPWM_CODE
static void HandleSendPwmDataState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData, bool bMailboxMode, bool bResyncPending)
{
    /* If PWM stop has been invoked, stop sending the updates */
    if(!poEalFioPwmInstData->bRunPwm)
    {
        TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_SEND_PWM_DATA, EAL_FIO_PWM_STATE_PWM_STOP);
    }
    /* If PWM is configured, send the PWM updates */
    else if(poEalFioPwmInstData->bIsConfigured)
    {
        bool bSuccess = SendPwmDataUpdate(poEalFioPwmInstData, bMailboxMode, bResyncPending);

        if(bSuccess)
        {
            /* If PWM update is successful, wait for the interval before sending update again */
            poEalFioPwmInstData->bIsConfigured = (poEalFioPwmInstData->eFioPwmOpMode == ADI_EAL_FIO_PWM_OP_MODE_CONTINUOUS);
            adi_pal_getCurrTime(&poEalFioPwmInstData->nSendTime);
            TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_SEND_PWM_DATA, EAL_FIO_PWM_STATE_WAIT_FOR_INTERVAL);
        }
        else
        {
            /* Retry in the next run! */
        }
    }
    else
    {
        /* Nothing to do! */
    }
}

/** Handle the WAIT_FOR_INTERVAL state */
ADI_EAL_FIOPWM_CODE
static void HandleWaitForIntervalState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData)
{
    uint64_t nCurrTime;
    adi_pal_getCurrTime(&nCurrTime);

    /* If PWM stop has been invoked, stop sending the updates */
    if(!poEalFioPwmInstData->bRunPwm)
    {
        TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_WAIT_FOR_INTERVAL, EAL_FIO_PWM_STATE_PWM_STOP);
    }
    /* If high threshold of the FIFO has been hit, wait for the FIFO to be emptied till the low threshold mark */
    else if(poEalFioPwmInstData->bHighIRQ)
    {
        poEalFioPwmInstData->bHighIRQ = false;
        TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_WAIT_FOR_INTERVAL, EAL_FIO_PWM_STATE_WAIT_FOR_FIFO_SPACE);
    }
    /* If time interval has elapsed, send the PWM updates again */
    else if(nCurrTime - poEalFioPwmInstData->nSendTime > poEalFioPwmInstData->nPwmUpdateInterval)
    {
        TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_WAIT_FOR_INTERVAL, EAL_FIO_PWM_STATE_SEND_PWM_DATA);
    }
    else
    {
        /* Nothing to do! */
    }
}

/** Handle the WAIT_FOR_FIFO_SPACE state */
ADI_EAL_FIOPWM_CODE
static void HandleWaitForFifoSpaceState(EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData)
{
    /* If PWM stop has been invoked, stop sending the updates */
    if(!poEalFioPwmInstData->bRunPwm)
    {
        TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_WAIT_FOR_FIFO_SPACE, EAL_FIO_PWM_STATE_PWM_STOP);
    }
    /* If low threshold of the FIFO has been hit, start sending
       the PWM updates again, so that the FIFO is not underflowed */
    else if(poEalFioPwmInstData->bLowIRQ)
    {
        poEalFioPwmInstData->bLowIRQ = false;
        TransitionState(poEalFioPwmInstData, EAL_FIO_PWM_STATE_WAIT_FOR_FIFO_SPACE, EAL_FIO_PWM_STATE_SEND_PWM_DATA);
    }
    else
    {
        /* Nothing to do! */
    }
}

/** Handle error callback */
ADI_EAL_FIOPWM_CODE
static void HandleErrorCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the error */
    ADI_E2BCORE_CBK_ERRDAT *poErrDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the internal data of this instance */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)pCbParam;

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* If we are not in the reset sequence, forward errors to the application */
    if(!poEalFioPwmInstData->bIgnoreErr)
    {
        InvokeCbk(poEalFioPwmData, poEalFioPwmInstData, ADI_EAL_FIO_PWM_CBK_EVT_ERR, &poErrDat->eError);
    }
}

/** Handle register read callback */
ADI_EAL_FIOPWM_CODE
static void HandleRegReadCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the register read */
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Get the REGMAP_IO message header details */
    const ADI_E2BCORE_MSGOPTS *poMsgOpts = poRegDat->poMsgOpts;

    /* Get the REGMAP_IO message payload details  */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = poRegDat->poInfo;

    /* Get the internal data of this instance */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)pCbParam;

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* If the read is of the FIO STATUS registers */
    if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_INTF0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FLEXIO_NUM_BYTES_NW_ADDR) &&
            (poRegCfg->nLength == 2U))
    {
        /* Process FIO status data */
        ADI_EAL_FIO_STATUS_DATA oFioPwmStatusData;
        ExtractFioPwmStatus(poRegCfg->pBuffer, &oFioPwmStatusData);
        InvokeCbk(poEalFioPwmData, poEalFioPwmInstData, ADI_EAL_FIO_PWM_CBK_EVT_INTF_STATUS_EVT, &oFioPwmStatusData);
    }
    /* If the read is of the FIFO status registers */
    else if((poMsgOpts->nTopic == (ADI_E2BCORE_TOPIC_FIFO0 + poID->nIntfNum)) &&
            (poRegCfg->anStartAddr[0U] == FIFO_STATUS_NW_ADDR) &&
            (poRegCfg->nLength == 5U))
    {
        /* Process FIFO status data */
        ADI_EAL_FIFO_STATUS_DATA oFifoStatusData;
        adi_eal_diagExtractFifoStatus(poRegCfg->pBuffer, &oFifoStatusData);
        InvokeCbk(poEalFioPwmData, poEalFioPwmInstData, ADI_EAL_FIO_PWM_CBK_EVT_FIFO_STATUS_EVT, &oFifoStatusData);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeCbk(poEalFioPwmData, poEalFioPwmInstData, ADI_EAL_FIO_PWM_CBK_EVT_REG_READ_RESP, poParam);
}

/** Handle FIFO read callback */
ADI_EAL_FIOPWM_CODE
static void HandleFifoReadCallback(void *poParam, void *pCbParam)
{
    /* Get the details of the FIFO read */
    ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (ADI_E2BCORE_CBK_FIFOREADDAT *)poParam;

    /* Get the FIO transaction configurations */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)poFifoDat->poInfo;

    /* Get the internal data of this instance */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)pCbParam;

    /* Get the unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    /* Increment the read index */
    poEalFioPwmInstData->nFioPwmQRdIdx = EAL_INC_Q_IDX(poEalFioPwmInstData->nFioPwmQRdIdx,
                                         poEalFioPwmInstData->nFioPwmCfgQDepth);

    /* Give a callback with write response event */
    InvokeCbk(poEalFioPwmData, poEalFioPwmInstData, ADI_EAL_FIO_PWM_CBK_EVT_WRITE_RESP, poTxnCfg);
}

/** Handle mailbox ACK callback */
ADI_EAL_FIOPWM_CODE
static void HandleMbAckCallback(const ADI_E2BCORE_ID *poID, void *poParam, void *pCbParam)
{
    /* Get the details of the ACK */
    ADI_E2BCORE_CBK_ACKDAT *poAckDat = (ADI_E2BCORE_CBK_ACKDAT *)poParam;

    /* Get the internal FIO PWM data */
    EAL_FIO_PWM_DATA *poEalFioPwmData = (EAL_FIO_PWM_DATA *)pCbParam;

    /* Get the FIO PWM unicast instance data */
    EAL_FIO_PWM_INST_DATA *poEalFioPwmInstData = poEalFioPwmData->apoInstData[0U];

    ADI_EAL_FIO_PWM_CBK_EVT_MAILBOX_ACK_DATA oAckData;
    oAckData.nSeqNum = poAckDat->nSeqNum;
    oAckData.bFd     = poAckDat->bFd;
    oAckData.nTopic  = poAckDat->nTopic;

    InvokeCbk(poEalFioPwmData, poEalFioPwmInstData, ADI_EAL_FIO_PWM_CBK_EVT_MAILBOX_ACK, &oAckData);
}

/** Extract FIO PWM status data */
ADI_EAL_FIOPWM_CODE
static void ExtractFioPwmStatus(uint32_t *panData, ADI_EAL_FIO_STATUS_DATA *poStatusData)
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
ADI_EAL_FIOPWM_CODE
static void InvokeCbk(EAL_FIO_PWM_DATA *poEalFioPwmData, EAL_FIO_PWM_INST_DATA *poEalFioInstPwmData,
                      ADI_EAL_FIO_PWM_CBK_EVT eEvt, const void *pArg)
{
    /* Give a callback with the event and data if registered */
    if(poEalFioInstPwmData->pfunCbk != NULL)
    {
        (poEalFioInstPwmData->pfunCbk)(poEalFioInstPwmData->oId.nInstNum, poEalFioPwmData, eEvt, pArg);
    }
}

/*! Lock the mutex of all FIO_PWM instances */
ADI_EAL_FIOPWM_CODE
static void LockFioPwmMutex(EAL_FIO_PWM_DATA *poEalFioPwmData)
{
    /* Loop through all the unicast FIO_PWM instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalFioPwmData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalFioPwmData->apoInstData[i]->hFioPwmMutex);
        /* Ensure the mutex lock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/*! Unlock the mutex of all FIO_PWM instances */
ADI_EAL_FIOPWM_CODE
static void UnlockFioPwmMutex(EAL_FIO_PWM_DATA *poEalFioPwmData)
{
    /* Loop through all the unicast FIO_PWM instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalFioPwmData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalFioPwmData->apoInstData[i]->hFioPwmMutex);
        /* Ensure the mutex unlock operation is successful for each instance */
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}


#endif /* ADI_E2B_IFACE_FIO_PWM_ENABLED */
/** @endcond */

/** @} */

/**
 * EOF: www.analog.com
 */
