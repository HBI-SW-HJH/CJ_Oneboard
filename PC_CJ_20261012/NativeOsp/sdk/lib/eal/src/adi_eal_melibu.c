/*******************************************************************************
Copyright (c) 2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: adi_eal_melibu.c
 * @brief: The EAL APIs for melibu interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "adi_eal_melibu.h"
#include "eal_internal.h"

#ifdef ADI_E2B_IFACE_MELIBU_ENABLED

/** @cond PRIVATE */
/*================================= DEFINES =================================*/
/* Basic handle check */
#define BASIC_HANDLE_CHECK(poEalMelibuData) \
ADI_EAL_MELIBU_DBG_REQUIRE((poEalMelibuData) != NULL, "Handle is NULL"); \
ADI_EAL_MELIBU_DBG_REQUIRE((poEalMelibuData)->bInUse == true, "Instance is not opened");\

/* Basic handle check */
#define BASIC_HANDLE_CHECK_DBG(poEalMelibuData) \
ADI_DBG_REQUIRE((poEalMelibuData) != NULL, "Handle is NULL"); \
ADI_DBG_REQUIRE((poEalMelibuData)->bInUse == true, "Instance is not opened");\

#define ADI_EAL_MELIBU_MAX_FUNC_SEL_FLAG_VALUE          (2U)     /*!< Maximum value of function selector flag in frame */

#define ADI_EAL_MELIBU_FRAME_ID_INDEX                   (0U)     /*!< Id byte position in frame buffer */
#define ADI_EAL_MELIBU_FRAME_ID_FIELDS_RT_BITPOS        (0U)     /*!< Bit position into ID of reception/transmission flag */
#define ADI_EAL_MELIBU_FRAME_ID_FIELDS_F_BITPOS         (1U)     /*!< Bit position into ID of function flag */
#define ADI_EAL_MELIBU_FRAME_ID_FIELDS_I_BITPOS         (2U)     /*!< Bit position into ID of intruction word flag */
#define ADI_EAL_MELIBU_FRAME_ID_FIELDS_PL_LENGTH_BITPOS (3U)     /*!< Bit position into ID of payload length */
#define ADI_EAL_MELIBU_FRAME_ID_FIELDS_P1_BITPOS        (6U)     /*!< Bit position into ID of parity 1 bit */
#define ADI_EAL_MELIBU_FRAME_ID_FIELDS_P0_BITPOS        (7U)     /*!< Bit position into ID of parity 0 bit */
#define ADI_EAL_MELIBU_FRAME_ID_LENGTH                  (2U)     /*!< Id number of bytes */
/** @brief Byte position of data starting byte */
#define ADI_EAL_MELIBU_FRAME_DATA_INDEX                 (ADI_EAL_MELIBU_FRAME_ID_INDEX + ADI_EAL_MELIBU_FRAME_ID_LENGTH)

#define ADI_EAL_MELIBU_P0_MASK                          (0x3F00u) /*!< Parity 0 calculation mask */
#define ADI_EAL_MELIBU_P1_MASK                          (0x3FFFu) /*!< Parity 1 calculation mask */

#define ADI_EAL_MELIBU_CRC_POLYNOM                      (0x1021u) /*!< CRC polynomium */

#define ADI_EAL_MELIBU_TX_FRAME_LENGTH_DEFAUT           (23U)   /*!< Transmission Frame length by default that no needs to overwrite length in SPI transactions */
#define ADI_EAL_MELIBU_MCAST_MAX_FIO_UART_INTF          (50U)   /*!< Maximum FIO UART interfaces in a multicast group */

#define ADI_EAL_MELIBU_MAXIMUM_BREAKFIELD_LENGTH        (21U)   /*!< Maximum length of breakfield in bit times */
#define ADI_EAL_MELIBU_MINIMUM_BREAKFIELD_LENGTH        (12U)   /*!< Minimum length of breakfield in bit times */

#define ADI_EAL_MELIBU_NO_E2B_ID                        (0xFFU) /*!< E2B ID not assigned */

#define ADI_EAL_MELIBU_MAX_INST_WORD_SEL                (2U)    /*!< Maximum instruction word selector */

/** @brief Macro to verify input parameters returning error instead of causing crash */
#define ADI_EAL_MELIBU_DBG_REQUIRE(cond, errMsg) {\
    if(!(cond)) {\
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, errMsg);\
        eEalStatus = ADI_EAL_STATUS_START_FAILED;\
        break;\
    }\
}

/*=================================== DATA ==================================*/


/*================================ DATA TYPES ===============================*/
/**
 * @brief Possible parity calculations
 */
typedef enum ADI_EAL_MELIBU_PARITY_TYPES
{
    ADI_EAL_MELIBU_PARITY_TYPE_P0,  /*!< P0 parity calculation */
    ADI_EAL_MELIBU_PARITY_TYPE_P1   /*!< P1 parity calculation */
} ADI_EAL_MELIBU_PARITY_TYPES;

/*================================ PROTOTYPES ===============================*/
static void MelibuFiouUartReadRespNotification(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        const ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg, ADI_EAL_FIO_UART_CBK_EVT eUartCbkEvent);
static void FillFrameConfig(ADI_EAL_MELIBU_FRAME_CFG *poMelframeCfg, ADI_EAL_MELIBU_IO_DATA *poMelFramedata,
                            uint8_t nId, bool bEchoCancelled, uint32_t nDelay, uint64_t nTimestamp);
static void SetWaitingForRx(EAL_MELIBU_DATA *poEalMelibuData, bool bWaitingForRx);
static bool WaitForResponses(EAL_MELIBU_DATA *poEalMelibuData);
static void MelibuPrepareFioUartTransaction(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFioUartTxnCfg,
        ADI_EAL_MELIBU_FRAME_CFG *poMelibuFrame, uint8_t nTxLength, uint8_t *anTxBuffer, uint16_t *pBuffer);
static ADI_EAL_STATUS MelibuPlLengthEncode(ADI_EAL_MELIBU_FUNCTION_SELECT eFuncSel, uint8_t *nPayloadLength,
        uint8_t *nPL_Length);
static uint8_t MelibuComputeParity(ADI_EAL_PARITY_TYPE eParityType, uint16_t nTxData);
static uint8_t MelibuComputeParityBits(uint16_t nId, ADI_EAL_MELIBU_PARITY_TYPES eParityType);
static uint16_t MelibuCrc16(uint16_t nCrc, uint8_t nNewByte);
static uint16_t MelibuCrcCalculation(uint8_t *pData, uint8_t nDataLength);
static bool MelibuCrcVerification(uint8_t *pMelibuFrameID, uint16_t *pPayload, uint8_t nDataLength,
                                  uint16_t nCrcReceived);
static ADI_EAL_STATUS MelibuSlaveReceiveConfigData(ADI_EAL_MELIBU_FRAME_CFG *poFrameCfg, uint8_t *pBuffer,
        uint8_t *pnTxLength);
static ADI_EAL_STATUS MelibuSlaveTransmitConfigData(ADI_EAL_MELIBU_FRAME_CFG *poFrameCfg, uint8_t *pBuffer,
        uint8_t *pnTxLength);
static void AddTransactionToQueue(EAL_MELIBU_DATA *poEalMelibuData, ADI_EAL_MELIBU_FRAME_CFG *poFrameCfg,
                                  uint8_t *anMelibuFrameID, bool bMelibuResponseExpected);
static void AddCustomWriteTransactionToQueue(EAL_MELIBU_DATA *poEalMelibuData, uint8_t nExpectedRxLength,
        uint16_t *anMelibuFrameID);
static void LockMelibuMutex(EAL_MELIBU_DATA *poEalMelibuData);
static void UnlockMelibuMutex(EAL_MELIBU_DATA *poEalMelibuData);
static ADI_EAL_STATUS MelibuSend(ADI_EAL_MELIBU_HANDLE hMelibuHandle, ADI_EAL_MELIBU_FRAME_CFG *aoFrameCfg,
                                 uint16_t nNumFrames);
static ADI_EAL_STATUS MelibuPeriodicBuildFrame(ADI_EAL_MELIBU_FRAME_CFG *poMelibuTxnCfg,
        uint8_t *anTxBuffer,
        uint8_t *pnTxLength);
static ADI_EAL_STATUS MelibuPeriodicQueueToFioUart(EAL_FIO_UART_DATA *poEalFioUartData,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFioUartTxnCfg,
        ADI_EAL_MELIBU_FRAME_CFG *poMelibuTxnCfg);
static ADI_EAL_STATUS MelibuPeriodicSubmitToE2BCore(EAL_FIO_UART_DATA *poEalFioUartData);
static void MelibuPeriodicEnableMode(EAL_MELIBU_DATA *poEalMelibuData);
static bool IsReadTimeTagEnabled(const EAL_MELIBU_DATA *poEalMelibuData);

/*============================ INTERNAL CALLBACKS ===========================*/

static void MelibuFioUartNotification(uint32_t nInstNum, ADI_EAL_FIO_UART_HANDLE hUartHandle,
                                      ADI_EAL_FIO_UART_CBK_EVT eUartCbkEvent, const void *pArg);

/** @endcond */

/*=================================== CODE ==================================*/

/**
 * @brief       Opens a MELIBU interface instance
 *
 * @param [in]  poMelibuCfg       Pointer to MELIBU configuration structure containing:
 *                                - Instance, remote, and interface numbers
 *                                - Memory buffer for internal data structures
 *                                - Queue depth and breakfield length settings
 * @param [out] phMelibuHandle    Returned handle for use in subsequent MELIBU API calls
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: MELIBU instance opened successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to open instance or create internal FIO UART interface
 *
 * @details This function initializes a MELIBU interface by:
 *          1. Validating configuration parameters and memory allocation
 *          2. Allocating internal buffers (TX/RX, transaction config, instance data)
 *          3. Initializing the EAL MELIBU data structure with queue management
 *          4. Creating an internal FIO UART interface for frame transmission/reception
 *          5. Registering an internal callback to handle FIO UART events
 *          6. Initializing a mutex for thread-safe queue access
 *
 * @note    The bPortConfigured flag is initially false. Call adi_eal_melibuConfigure()
 *          after network initialization to complete the setup before using transaction APIs.
 *
 * @see adi_eal_melibuConfigure() - Required to finalize configuration
 * @see adi_eal_melibuClose() - Resource cleanup
 * @see adi_eal_melibuWriteRead() - Send MELIBU transactions
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuOpen(ADI_EAL_MELIBU_CFG *poMelibuCfg, ADI_EAL_MELIBU_HANDLE *phMelibuHandle)
{

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Basic parameter checks */
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg != NULL, "Pointer to the Melibu module configuration is NULL");

        ADI_EAL_MELIBU_DBG_REQUIRE(phMelibuHandle != NULL, "Pointer to the Melibu handle is NULL");

        /* Validation of instance, remote and interface numbers */
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->nInstNum < ADI_EAL_MAX_NUM_INSTANCES,
                                   "Instance number is greater than max instances available");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->nRemoteNum < ADI_EAL_MAX_NUM_NODES,
                                   "Remote number is greater than max number of nodes supported");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->nIntfNum < 4U,
                                   "Interface number is greater than max number of interfaces supported");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->nMelibuIntfNum < ADI_EAL_MELIBU_MAXIMUM_INTF_PER_NODE,
                                   "Interface number is greater than max number of Melibu interfaces per node supported");

        uint32_t nInstNum = poMelibuCfg->nInstNum;
        uint32_t nRemoteNum = poMelibuCfg->nRemoteNum;
        uint32_t nIntfNum = poMelibuCfg->nIntfNum;
        ADI_E2BCORE_IF_TYPE eIfType =
            gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[nIntfNum].eIfType;

        /* More validations based on state and network configuration */
        ADI_EAL_MELIBU_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "EAL instance is not in use");
        ADI_EAL_MELIBU_DBG_REQUIRE(eIfType == ADI_E2BCORE_IF_TYPE_FIO,
                                   "The given pair of instance, remote and interface numbers is not configured as FIO interface");

        /* Validate the memory provided */
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->pEalMelibuMemory != NULL, "Pointer to allocated memory is null");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->nEalMelibuMemorySize >= ADI_EAL_MELIBU_UNICAST_INST_MEM_SIZE(
                                       poMelibuCfg->nMelibuCfgRxQDepth),
                                   "Allocated memory less than required value");

        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->nBreakFieldLength <= ADI_EAL_MELIBU_MAXIMUM_BREAKFIELD_LENGTH,
                                   "Breakfield length is greater than maximum supported");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuCfg->nBreakFieldLength >= ADI_EAL_MELIBU_MINIMUM_BREAKFIELD_LENGTH,
                                   "Breakfield length is smaller than minimum supported");

        /* Allocate memory for the internal data structures from the provided memory */
        uint8_t *pMem = poMelibuCfg->pEalMelibuMemory;

        /* Allocate memory for the internal transmision payloads buffer (uint16_t for UART framing) */
        uint16_t *anTxBuffer = (uint16_t *)((void *)pMem);
        pMem += (ADI_EAL_MELIBU_FRAME_MAX_LENGTH * 2U); /* Double size for uint16_t array */

        /* Allocate memory for the internal transmision transactions buffer */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *anTxTransactionBuffer = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)((void *)pMem);
        pMem += sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG);

        /* Allocate memory for the internal reception buffer */
        ADI_EAL_MELIBU_REC_DATA *aoRxMelibuBuffer = (ADI_EAL_MELIBU_REC_DATA *)((void *)pMem);
        pMem += poMelibuCfg->nMelibuCfgRxQDepth * sizeof(ADI_EAL_MELIBU_REC_DATA);

        /* Allocate memory for the EAL MELIBU instance data */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = (EAL_MELIBU_INST_DATA *)((void *)pMem);
        pMem += sizeof(EAL_MELIBU_INST_DATA);

        /* ~~~~~~~~~~~ MELIBU state info ~~~~~~~~~~~ */
        poEalMelibuInstData->oId.nInstNum   = nInstNum;
        poEalMelibuInstData->oId.nRemoteNum = nRemoteNum;
        poEalMelibuInstData->oId.nIntfNum   = nIntfNum;
        poEalMelibuInstData->nMelibuIntfNum = poMelibuCfg->nMelibuIntfNum;
        poEalMelibuInstData->pfunCbk        = NULL;     /* Will be filled in adi_eal_melibuRegisterCallback */

        /* Transaction config queue info */
        poEalMelibuInstData->nMelibuCfgRxQDepth    = poMelibuCfg->nMelibuCfgRxQDepth;
        poEalMelibuInstData->nMelibuQWrIdx         = 0U;
        poEalMelibuInstData->nMelibuQRdIdx         = 0U;
        poEalMelibuInstData->anTxBuffer            = anTxBuffer;
        poEalMelibuInstData->anTxTransactionBuffer = anTxTransactionBuffer;
        poEalMelibuInstData->aoRxMelibuBuffer      = aoRxMelibuBuffer;
        poEalMelibuInstData->bWaitingForRx         = false;
        poEalMelibuInstData->nBreakFieldLength     = poMelibuCfg->nBreakFieldLength;
        poEalMelibuInstData->eMode                 = ADI_EAL_USAGE_MODE_NORMAL;

        /* Allocate memory for the array of unicast instance data pointers */
        EAL_MELIBU_INST_DATA **apoInstData = (EAL_MELIBU_INST_DATA **)((void *)pMem);
        pMem += sizeof(EAL_MELIBU_INST_DATA *);
        apoInstData[0u] = poEalMelibuInstData;

        /* Allocate memory for the internal EAL MELIBU data structure */
        EAL_MELIBU_DATA *poEalMelibuData  = (EAL_MELIBU_DATA *)((void *)pMem);
        pMem += sizeof(EAL_MELIBU_DATA);
        poEalMelibuData->bIsMulticast  = false;
        poEalMelibuData->bInUse        = true;

        /* Final configuration is done in API "adi_eal_melibuConfigure()" */
        poEalMelibuData->bPortConfigured = false;

        ADI_EAL_FIO_UART_CFG oFioUartCfg;
        /* Configure FIO AURT */
        oFioUartCfg.nInstNum           = poMelibuCfg->nInstNum;
        oFioUartCfg.nRemoteNum         = poMelibuCfg->nRemoteNum;
        oFioUartCfg.nIntfNum           = poMelibuCfg->nIntfNum;
        oFioUartCfg.nFioUartMemorySize = (uint32_t)ADI_EAL_FIO_UART_UNICAST_INST_MEM_SIZE(poMelibuCfg->nMelibuCfgRxQDepth);
        oFioUartCfg.pFioUartMemory     = pMem;
        oFioUartCfg.nFioUartCfgQDepth  = poMelibuCfg->nMelibuCfgRxQDepth;

        /* Open FIO UART interface */
        eEalStatus = adi_eal_fioUartOpen(&oFioUartCfg, &poEalMelibuData->pInternalPortHandler);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Register internal FIO UART callback */
        eEalStatus = adi_eal_fioUartRegisterCallback(poEalMelibuData->pInternalPortHandler, MelibuFioUartNotification);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }


        /* Set number of instance as 1 due to it is a unicast port */
        poEalMelibuData->nNumInst = 1U;
        poEalMelibuData->apoInstData = apoInstData;

        /* Initialise the mutex for this MELIBU instance */
        poEalMelibuInstData->oMelibuMutexCfg.pMem = &poEalMelibuInstData->anMelibuMutexBuffer[0U];
        poEalMelibuInstData->oMelibuMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
        (void)prj_osal_createMutex(&poEalMelibuInstData->oMelibuMutexCfg, &poEalMelibuInstData->hMelibuMutex);

        /* Set the handle to the EAL_MELIBU_DATA structure */
        *phMelibuHandle = (ADI_EAL_MELIBU_HANDLE)poEalMelibuData;

        /* Link Melibu handle with internal interfaces */
        /* Get the internal data from the handle */
        EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)poEalMelibuData->pInternalPortHandler;
        /* Get the unicast instance data */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = (EAL_FIO_UART_INST_DATA *)poEalFioUartData->apoInstData[0U];
        poEalFioUartInstData->hMelibuHandle = *phMelibuHandle;
    } while(false);

    return eEalStatus;
}

/**
 * @brief       Closes an opened MELIBU interface instance
 *
 * @param [in]  hMelibuHandle   Handle corresponding to the MELIBU instance to close
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: MELIBU instance successfully closed
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to close internal FIO UART interface
 *
 * @details This function cleans up and closes a MELIBU instance by:
 *          1. Validating the handle and instance state
 *          2. Closing the internal FIO UART interface created during adi_eal_melibuOpen()
 *          3. Destroying mutexes allocated for both unicast and multicast instances
 *          4. Clearing all internal data structures
 *          5. Marking the instance as no longer in use
 *
 * @note This function must be called before the handle is destroyed to ensure
 *       proper resource cleanup and prevent memory leaks.
 *
 * @see adi_eal_melibuOpen() - Counterpart initialization function
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuClose(ADI_EAL_MELIBU_HANDLE hMelibuHandle)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);

        /* Close UART internal port */
        if(ADI_EAL_STATUS_SUCCESS != adi_eal_fioUartClose(poEalMelibuData->pInternalPortHandler))
        {
            eEalStatus = ADI_EAL_STATUS_CLOSE_FAILED;
        }

        /* Mutex cleanup */
        for(uint16_t i = 0U; i < poEalMelibuData->nNumInst; ++i)
        {
            (void)prj_osal_destroyMutex(poEalMelibuData->apoInstData[i]->hMelibuMutex);
        }

        /* Clear data of this instance */
        (void)ADI_MEMSET(poEalMelibuData, 0, sizeof(EAL_MELIBU_DATA));
    } while(false);

    return eEalStatus;
}

/**
 * @brief Registers a callback function for MELIBU events
 *
 * @param [in] hMelibuHandle      Handle to the MELIBU instance (must be unicast)
 * @param [in] pfMelibuCallback   Pointer to callback function invoked when MELIBU events occur
 *
 * @return Status
 *         - #ADI_EAL_STATUS_SUCCESS: Callback registered successfully
 *         - Error status if handle is invalid, callback is NULL, or handle is multicast
 *
 * @details The registered callback is invoked asynchronously to notify the application of:
 *          - Successful frame reception and response data availability
 *          - Unexpected frame receptions
 *          - Error conditions during frame transmission/reception
 *
 *          The callback function signature must be:
 *          @code
 *          void callback(uint32_t nInstNum, ADI_EAL_MELIBU_HANDLE hHandle,
 *                       ADI_EAL_MELIBU_CBK_EVT eCbkEvent, const void *pArg)
 *          @endcode
 *
 * @note - Only works with unicast handles; returns error for multicast handles
 *       - Callback is invoked from interrupt/callback context; avoid blocking operations
 *       - Must be called after adi_eal_melibuOpen() and before sending transactions
 *       - For synchronous operation, use adi_eal_melibuWriteReadSync() instead
 *
 * @see adi_eal_melibuWriteRead() - Asynchronous transaction with callback notification
 * @see adi_eal_melibuWriteReadSync() - Synchronous transaction without callback
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuRegisterCallback(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_CALLBACK_FUNC pfMelibuCallback)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    /* Get the internal data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(pfMelibuCallback != NULL, "Function pointer is NULL");
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bIsMulticast == false, "Cannot register callback for multicast handles");

        /* Get the unicast instance data from the handle */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];

        /* Register the callback function */
        poEalMelibuInstData->pfunCbk = pfMelibuCallback;
    } while(false);

    return eEalStatus;
}

/**
 * @brief       Configures the MELIBU port after network initialization
 *
 * @param [in]  hMelibuHandle       The MELIBU handle (must be unicast)
 * @param [in]  poUartParams        Pointer to UART parameter structure
 * @param [in]  poUartInstRamData   Pointer to instruction RAM data (can be NULL if OTP is configured)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Port configuration successful
 *              - #ADI_EAL_STATUS_START_FAILED: Configuration failed or handle is invalid
 *
 * @details     This function finalizes the configuration of the internal FIO UART interface
 *              created during adi_eal_melibuOpen(). It must be called after network
 *              initialization and before sending any MELIBU transactions.
 *
 *              The function configures:
 *              - UART parameters: stop bits, data bits, parity
 *              - Baud rate from user-specified value
 *              - Break field length from instance configuration
 *              - CAN echo cancellation support
 *              - Instruction RAM (if not OTP-programmed)
 *
 *              <b>OTP Configuration Support</b>:
 *              - When poUartParams->bIsOtpConfigured = true:
 *                  - Instruction RAM programming is skipped
 *                  - The device uses OTP as the source for instruction RAM
 *                  - poUartInstRamData can be NULL
 *                  - Runtime parameters (baudrate, parity, etc.) are still updated
 *                  - bPortConfigured flag is set to true
 *              - When poUartParams->bIsOtpConfigured = false:
 *                  - Instruction RAM must be programmed
 *                  - poUartInstRamData must not be NULL
 *                  - If poUartInstRamData->bOverrideDefault is true, application-provided data is used
 *                  - If poUartInstRamData->bOverrideDefault is false, default instruction RAM is used
 *                  - bPortConfigured flag is set to true
 *
 *              If already configured, the function resets the queue instead of
 *              reconfiguring UART parameters.
 *
 * @note        - Only works with unicast handles
 *              - Must be called after adi_eal_melibuOpen()
 *              - Must be called after E2B network initialization
 *              - This API must be called for both OTP and non-OTP configurations
 *              - After successful completion, bPortConfigured is true, enabling all read/write APIs
 *
 * @see adi_eal_melibuOpen() - Call before this function
 * @see adi_eal_melibuWriteRead() - Call after this function
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuConfigure(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
                                       const ADI_EAL_MELIBU_UART_PARAMS *poUartParams,
                                       const ADI_EAL_MELIBU_UART_INST_RAM_DATA *poUartInstRamData)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);

        /* Validate that we are not using this API with multicast handle */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bIsMulticast == false, "This API doesn't work with multicast handles");

        /* Get the unicast instance data from the handle */
        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;

        /* If port already configured, reset the queue first */
        if(poEalMelibuData->bPortConfigured == true)
        {
            eEalStatus = adi_eal_fioUartResetQueue(hFioUartHandle);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }

        /* Configure FIO UART interface */
        eEalStatus = adi_eal_fioUartConfigure(hFioUartHandle, poUartParams, poUartInstRamData);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Port is totally configured */
        poEalMelibuData->bPortConfigured = true;

    } while(false);

    /* Remap all errors to generic START_FAILED status */
    if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        eEalStatus = ADI_EAL_STATUS_START_FAILED;
    }

    return eEalStatus;
}

/**
 * @brief       Creates a multicast group of MELIBU interfaces
 *
 * @param [in]  poMelibuMcastCfg       Configuration structure for MELIBU multicast group containing:
 *                                     - nNumInst: Number of unicast MELIBU instances
 *                                     - pahMelibuHandles: Array of unicast MELIBU handles
 *                                     - nIntfTopic: E2B topic for interface register access
 *                                     - nFifoTopic: E2B topic for FIFO operations
 *                                     - VLAN tag pointers (optional)
 *                                     - Memory buffer and size for multicast data
 *
 * @param [out] phMelibuMcastHandle    Returned handle to the created multicast group
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Multicast group created successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to create multicast group
 *
 * @details     This function creates a multicast group that allows sending MELIBU transactions
 *              to multiple instances simultaneously. It:
 *              1. Validates all configuration parameters and memory allocation
 *              2. Allocates memory for multicast data structure and instance pointers
 *              3. Collects internal FIO UART handles from each unicast MELIBU instance
 *              4. Creates an internal FIO UART multicast group with provided topics and VLAN tags
 *              5. Initializes multicast metadata (bIsMulticast=true, nNumInst, apoInstData)
 *              6. Marks port as configured (bPortConfigured=true)
 *
 * @note        - All unicast MELIBU instances must be created and configured before
 *                creating a multicast group
 *              - The multicast handle supports write operations only; read is not supported
 *              - For VLAN tagging, pass NULL pointers if not required
 *
 * @see adi_eal_melibuOpen() - Required to create unicast instances first
 * @see adi_eal_melibuConfigure() - Required to configure unicast instances
 * @see adi_eal_melibuWriteRead() - Send transactions using the multicast handle
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuCreateMcastGroup(ADI_EAL_MELIBU_MCAST_CFG *poMelibuMcastCfg,
        ADI_EAL_MELIBU_HANDLE *phMelibuMcastHandle)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Basic parameter checks */
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg != NULL, "Multicast configuration is NULL");
        ADI_EAL_MELIBU_DBG_REQUIRE(phMelibuMcastHandle != NULL, "Pointer to the MELIBU multicast handle is NULL");

        /* Multicast configuration checks */
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg->nNumInst > 0U, "Multicast group should have at least 1 unicast instance");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg->pahMelibuHandles != NULL,
                                   "Handle to array of the unicast instances is NULL");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg->pMelibuMcastMem != NULL, "Pointer to memory for MELIBU multicast is NULL");

        /* Verify sufficient memory is allocated for multicast group */
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg->nMelibuMcastMemSize >= ADI_EAL_MELIBU_MCAST_GROUP_MEM_SIZE(
                                       poMelibuMcastCfg->nNumInst), "Not enough memory allocated for MELIBU multicast group");

        /* Validate multicast topic assignments are in valid range */
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg->nIntfTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                                   "Assigned multicast interface topic is invalid");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg->nFifoTopic >= ADI_E2BCORE_MAX_UCAST_TOPICS,
                                   "Assigned multicast interface topic is invalid");

        /* Verify all unicast EAL MELIBU instance handles are valid */
        for(uint16_t i = 0U; i < poMelibuMcastCfg->nNumInst; i++)
        {
            ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuMcastCfg->pahMelibuHandles[i] != NULL, "Handle to  unicast instance is NULL");
        }

        /* Check if any validation errors occurred */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Array to store FIO UART handles for multicast group creation */
        ADI_EAL_FIO_UART_HANDLE  ahFioUartHandles[ADI_EAL_MELIBU_MCAST_MAX_FIO_UART_INTF];

        /* === Memory allocation for multicast data structure === */

        /* Calculate total memory available for allocation */
        uint32_t nMemoryToAllocate = poMelibuMcastCfg->nMelibuMcastMemSize;
        uint8_t *pMem = (uint8_t *)poMelibuMcastCfg->pMelibuMcastMem;

        /* Allocate memory for multicast MELIBU data structure */
        EAL_MELIBU_DATA *poEalMelibuMcastData = (EAL_MELIBU_DATA *)((void *)pMem);
        pMem += sizeof(EAL_MELIBU_DATA);
        nMemoryToAllocate -= (uint32_t)sizeof(EAL_MELIBU_DATA);

        /* Allocate memory for the array of pointers to unicast instance data */
        EAL_MELIBU_INST_DATA **apoInstData = (EAL_MELIBU_INST_DATA **)((void *)pMem);
        uint32_t nTempMemeSize = (uint32_t)sizeof(EAL_MELIBU_INST_DATA *);

        /* Advance memory pointer and update remaining memory */
        pMem += (nTempMemeSize * poMelibuMcastCfg->nNumInst);
        nMemoryToAllocate -= (uint32_t)(sizeof(EAL_MELIBU_INST_DATA *) * poMelibuMcastCfg->nNumInst);

        /* Verify sufficient memory remains for FIO UART multicast group */
        if(nMemoryToAllocate == 0U)
        {
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* === Extract unicast instances and build FIO UART handle array === */

        /* Iterate through each unicast MELIBU instance in the group */
        for(uint16_t i = 0U; i < poMelibuMcastCfg->nNumInst; i++)
        {
            /* Extract instance data from unicast MELIBU handle */
            EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)poMelibuMcastCfg->pahMelibuHandles[i];
            apoInstData[i] = poEalMelibuData->apoInstData[0U];

            /* Extract FIO UART handle for multicast group creation */
            ahFioUartHandles[i] = poEalMelibuData->pInternalPortHandler;
        }

        /* === Configure FIO UART multicast group as internal port === */

        /* Populate FIO UART multicast configuration structure */
        ADI_EAL_FIO_UART_MCAST_CFG oFioUartMcastCfg;
        oFioUartMcastCfg.nNumInst               = poMelibuMcastCfg->nNumInst;
        oFioUartMcastCfg.pahFioUartHandles      = ahFioUartHandles;
        oFioUartMcastCfg.nIntfTopic             = poMelibuMcastCfg->nIntfTopic;
        oFioUartMcastCfg.nFifoTopic             = poMelibuMcastCfg->nFifoTopic;

        /* Set VLAN tag configurations (may be NULL if not required) */
        oFioUartMcastCfg.poIfRegVlanTag         = poMelibuMcastCfg->poIfRegVlanTag;
        oFioUartMcastCfg.poFifoRegVlanTag       = poMelibuMcastCfg->poFifoRegVlanTag;
        oFioUartMcastCfg.poFifoWriteVlanTag     = poMelibuMcastCfg->poFifoWriteVlanTag;

        /* Pass remaining memory to FIO UART multicast creation */
        oFioUartMcastCfg.nFioUartMcastMemSize   = nMemoryToAllocate;
        oFioUartMcastCfg.pFioUartMcastMem       = pMem;

        /* Create the underlying FIO UART multicast group */
        eEalStatus = adi_eal_fioUartCreateMcastGroup(&oFioUartMcastCfg, &poEalMelibuMcastData->pInternalPortHandler);

        /* === Initialize multicast data structure === */

        /* Set multicast-specific flags and configuration */
        poEalMelibuMcastData->bIsMulticast = true;
        poEalMelibuMcastData->bInUse       = true;
        poEalMelibuMcastData->nNumInst     = poMelibuMcastCfg->nNumInst;
        poEalMelibuMcastData->apoInstData  = apoInstData;

        /* Return the multicast handle to caller */
        *phMelibuMcastHandle = (ADI_EAL_MELIBU_HANDLE)poEalMelibuMcastData;

        /* Mark port configuration as complete */
        poEalMelibuMcastData->bPortConfigured = true;

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Sends an asynchronous MELIBU write/read transaction
 *
 * @param [in]  hMelibuHandle       The MELIBU interface handle
 * @param [in]  poMelibuData        Pointer to MELIBU frame data structure containing:
 *                                  - nSlaveAddr: Target slave address
 *                                  - eFuncSelect: Function selection (LED/NO_LED)
 *                                  - nPayloadLength: Number of bytes to send
 *                                  - nResponseLength: Expected response length
 *                                  - nInstWordSel: Instruction word selector
 *                                  - nInstWord: Instruction word value
 *                                  - pPayload: Pointer to payload data
 * @param [in]  bEchoCancelled      If true, slave handles echo cancellation internally;
 *                                  if false, response includes echo + slave response
 * @param [in]  nId                 E2B transaction ID for callback identification (0xFF = no ID)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Transaction queued successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer unavailable
 *
 * @details     Queues a MELIBU transaction for asynchronous execution without blocking.
 *              The transaction is processed by the internal FIO UART interface and
 *              the result is delivered via the registered callback function.
 *
 * @note        - This is a non-blocking asynchronous call
 *              - Callback notification occurs when response is received (nId != 0xFF)
 *              - Use #adi_eal_melibuWriteReadSync() for blocking operation
 *              - Port must be configured via #adi_eal_melibuConfigure() before use
 *
 * @see         adi_eal_melibuWriteReadSync() - Blocking variant
 * @see         adi_eal_melibuRegisterCallback() - Register callback for async events
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle, ADI_EAL_MELIBU_IO_DATA *poMelibuData,
                                       bool bEchoCancelled, uint8_t nId)
{

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bPortConfigured == true, "Melibu port is not totally configured");

        ADI_EAL_MELIBU_FRAME_CFG oMelibuFrameCfg;

        /* Fill frame configuration */
        FillFrameConfig(&oMelibuFrameCfg, poMelibuData, nId, bEchoCancelled, 0U, 0U);

        /* Loop through the mutex of all instances and lock it */
        LockMelibuMutex(poEalMelibuData);

        /* Execute transaction */
        eEalStatus = MelibuSend(hMelibuHandle, &oMelibuFrameCfg, 1U);

        /* Loop through the mutex of all instances and unlock it */
        UnlockMelibuMutex(poEalMelibuData);

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Sends a synchronous MELIBU write/read transaction
 *
 * @param [in]  hMelibuHandle       The MELIBU interface handle (must be unicast)
 * @param [in]  poMelibuData        Pointer to MELIBU frame data containing slave address,
 *                                  function selection, payload, and expected response length
 * @param [in]  bEchoCancelled      If true, slave handles echo cancellation; if false,
 *                                  response includes echo + slave data
 * @param [in]  nId                 E2B transaction ID for tracking (0xFF = no ID)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Transaction completed and response received
 *              - #ADI_EAL_STATUS_RX_TIMEOUT: Response not received within timeout period
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or port not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer unavailable
 *
 * @details     This function blocks until the MELIBU transaction completes:
 *              1. Validates handle and configuration state
 *              2. Fills frame configuration from provided MELIBU data
 *              3. Acquires mutex for thread-safe queue access
 *              4. Submits transaction via MelibuSend()
 *              5. Polls for response using WaitForResponses() with timeout mechanism
 *              6. Clears waiting state and releases mutex
 *              7. Returns status upon completion or timeout
 *
 * @note        - This is a blocking call; application waits until response arrives
 *              - Use adi_eal_melibuWriteRead() for non-blocking asynchronous operation
 *              - Must call adi_eal_melibuConfigure() before using this API
 *              - The response data is delivered via the internal callback mechanism
 *              - Timeout duration determined by internal system timer configuration
 *
 * @see adi_eal_melibuWriteRead() - Asynchronous variant with callback notification
 * @see adi_eal_melibuConfigure() - Required configuration step
 * @see adi_eal_melibuRegisterCallback() - Register callback for async events
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuWriteReadSync(ADI_EAL_MELIBU_HANDLE hMelibuHandle, ADI_EAL_MELIBU_IO_DATA *poMelibuData,
        bool bEchoCancelled, uint8_t nId)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;
    bool bUnlockMutex = false;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bPortConfigured == true, "Melibu port is not totally configured");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuData != NULL, "Pointer to data is null");

        ADI_EAL_MELIBU_FRAME_CFG oMelibuFrameCfg;

        /* Fill frame configuration */
        FillFrameConfig(&oMelibuFrameCfg, poMelibuData, nId, bEchoCancelled, 0U, 0U);


        /* Loop through the mutex of all instances and lock it */
        LockMelibuMutex(poEalMelibuData);
        bUnlockMutex = true;

        if((poMelibuData->nResponseLength > 0U) || (bEchoCancelled == false) || (nId != ADI_EAL_MELIBU_NO_E2B_ID)
                || IsReadTimeTagEnabled(poEalMelibuData))
        {
            /* Set the waiting for RX flag to true */
            SetWaitingForRx(poEalMelibuData, true);
        }

        /* Execute transaction */
        eEalStatus = MelibuSend(hMelibuHandle, &oMelibuFrameCfg, 1U);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for FIO_UART read response */
        bool bTimedOut = WaitForResponses(poEalMelibuData);
        eEalStatus = bTimedOut ? ADI_EAL_STATUS_RX_TIMEOUT : ADI_EAL_STATUS_SUCCESS;

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Set the waiting for RX flag to false */
        SetWaitingForRx(poEalMelibuData, false);

    } while(false);

    if(bUnlockMutex)
    {
        /* Loop through the mutex of all instances and unlock it */
        UnlockMelibuMutex(poEalMelibuData);
    }

    return eEalStatus;
}

/**
 * @brief       Sends an asynchronous MELIBU write/read transaction with programmable delay
 *
 * @param [in]  hMelibuHandle       The MELIBU interface handle
 * @param [in]  poMelibuData        Pointer to MELIBU frame data structure
 * @param [in]  nDelay              Delay in nanoseconds before transmission
 * @param [in]  bEchoCancelled      If true, slave handles echo cancellation internally;
 *                                  if false, response includes echo + slave data
 * @param [in]  nId                 E2B transaction ID for callback identification (0xFF = no ID)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Transaction queued successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or port not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer unavailable
 *
 * @details     Queues a MELIBU transaction for asynchronous execution with a programmable delay.
 *              The specified delay is inserted before transmitting the frame over the MELIBU bus.
 *              Transaction completion is notified via the registered callback (if nId != 0xFF).
 *
 * @note        - This is a non-blocking asynchronous call
 *              - Port must be configured via #adi_eal_melibuConfigure() before use
 *              - Use #adi_eal_melibuWriteRead() for immediate transmission without delay
 *              - Use #adi_eal_melibuWriteReadSync() for blocking operation
 *
 * @see         adi_eal_melibuWriteRead() - Variant without delay
 * @see         adi_eal_melibuWriteReadSync() - Blocking variant
 * @see         adi_eal_melibuRegisterCallback() - Register callback for async events
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuWriteReadWithDelay(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, uint32_t nDelay, bool bEchoCancelled, uint8_t nId)
{

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;
    bool bUnlockMutex = false;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bPortConfigured == true, "Melibu port is not totally configured");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuData != NULL, "Pointer to data is null");

        ADI_EAL_MELIBU_FRAME_CFG oMelibuFrameCfg;

        /* Fill frame configuration */
        FillFrameConfig(&oMelibuFrameCfg, poMelibuData, nId, bEchoCancelled, nDelay, 0U);

        /* Loop through the mutex of all instances and lock it */
        LockMelibuMutex(poEalMelibuData);
        bUnlockMutex = true;

        /* Execute transaction */
        eEalStatus = MelibuSend(hMelibuHandle, &oMelibuFrameCfg, 1U);

    } while(false);

    if(bUnlockMutex)
    {
        /* Loop through the mutex of all instances and unlock it */
        UnlockMelibuMutex(poEalMelibuData);
    }

    return eEalStatus;
}

/**
 * @brief       Sends an asynchronous MELIBU write/read transaction with presentation timestamp
 *
 * @param [in]  hMelibuHandle       The MELIBU interface handle
 * @param [in]  poMelibuData        Pointer to MELIBU frame data structure containing:
 *                                  - nSlaveAddr: Target slave address
 *                                  - eFuncSelect: Function selection (LED/NO_LED)
 *                                  - nPayloadLength: Number of bytes to send
 *                                  - nResponseLength: Expected response length
 *                                  - nInstWordSel: Instruction word selector
 *                                  - nInstWord: Instruction word value
 *                                  - pPayload: Pointer to payload data
 * @param [in]  nTimestamp          gPTP presentation timestamp for time-synchronized transmission
 * @param [in]  bEchoCancelled      If true, slave handles echo cancellation internally;
 *                                  if false, response includes echo + slave response
 * @param [in]  nId                 E2B transaction ID for callback identification (0xFF = no ID)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Transaction queued successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or port not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer unavailable
 *
 * @details     Queues a MELIBU transaction for asynchronous execution with a presentation timestamp.
 *              The frame transmission is synchronized to occur at the specified gPTP time,
 *              enabling coordinated multi-node MELIBU operations. Transaction completion is
 *              notified via the registered callback function (if nId != 0xFF).
 *
 * @note        - This is a non-blocking asynchronous call
 *              - Port must be configured via #adi_eal_melibuConfigure() before use
 *              - Timestamp must be valid for the system's timing infrastructure
 *              - Use #adi_eal_melibuWriteRead() for immediate transmission without timestamp
 *
 * @see         adi_eal_melibuWriteRead() - Variant without timestamp
 * @see         adi_eal_melibuWriteReadWithDelay() - Variant with delay instead of timestamp
 * @see         adi_eal_melibuRegisterCallback() - Register callback for async events
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuWriteReadWithPresTs(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, uint64_t nTimestamp, bool bEchoCancelled, uint8_t nId)
{

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;
    bool bUnlockMutex = false;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bPortConfigured == true, "Melibu port is not totally configured");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuData != NULL, "Pointer to data is null");

        ADI_EAL_MELIBU_FRAME_CFG oMelibuFrameCfg;

        /* Fill frame configuration */
        FillFrameConfig(&oMelibuFrameCfg, poMelibuData, nId, bEchoCancelled, 0U, nTimestamp);

        /* Loop through the mutex of all instances and lock it */
        LockMelibuMutex(poEalMelibuData);
        bUnlockMutex = true;

        /* Execute transaction */
        eEalStatus = MelibuSend(hMelibuHandle, &oMelibuFrameCfg, 1U);

    } while(false);

    if(bUnlockMutex)
    {
        /* Loop through the mutex of all instances and unlock it */
        UnlockMelibuMutex(poEalMelibuData);
    }

    return eEalStatus;
}

/**
 * @brief       Sets up periodic MELIBU write-read transactions with automatic re-queuing
 *
 * @param [in]  hMelibuHandle         The MELIBU interface handle
 * @param [in]  poMelibuData          Pointer to MELIBU IO data structure containing:
 *                                    - Slave address
 *                                    - Function selection (MEL1BU_FUNC_*)
 *                                    - Write data buffer and length
 *                                    - Expected response length
 * @param [in]  bEchoCancelled        Echo cancellation configuration:
 *                                    - true: Enable echo cancellation (slave echoes request)
 *                                    - false: Disable echo cancellation
 * @param [in]  nId                   E2B transaction ID (0x00-0xFE) for callback association
 *                                    (0xFF = no callback)
 *
 * @return      Status
 *              - #ADI_EAL_USAGE_MODE_NORMAL: Initial transaction queued, mode switched to PERIODIC
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or port not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Insufficient queue space
 *
 * @details     This function enables periodic sensor polling by:
 *              1. Sending initial MELIBU transaction via FIO UART layer
 *              2. Switching interface to PERIODIC usage mode
 *              3. Callback automatically re-queues the transaction after each response
 *
 *              Once in PERIODIC mode:
 *              - Transactions repeat continuously without application intervention
 *              - Callback delivers responses via #ADI_EAL_MELIBU_CBK_EVT_PERIODIC_RX event
 *              - Queue read index is NOT incremented (transaction stays in queue)
 *              - To stop: Call #adi_eal_melibuResetQueue() to return to NORMAL mode
 *
 * @note        - This is a non-blocking asynchronous call
 *              - Port must be configured via #adi_eal_melibuConfigure() before use
 *              - Only ONE transaction should be in PERIODIC mode at a time per interface
 *              - Switching to PERIODIC mode applies to ALL instances on this remote number
 *              - Cannot use with #adi_eal_melibuCustomWriteRead() simultaneously
 *
 * @see         adi_eal_melibuResetQueue() - Return to NORMAL mode and stop periodic polling
 * @see         adi_eal_melibuRegisterCallback() - Register callback for PERIODIC_RX events
 * @see         adi_eal_fioUartSetupPeriodicWriteRead() - Reference implementation pattern
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuSetupPeriodicWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, bool bEchoCancelled, uint8_t nId)
{
    /* Initialize return status to success */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Frame configuration structure for MELIBU transaction */
    ADI_EAL_MELIBU_FRAME_CFG oMelibuTxnCfg;

    /* Cast handle to internal MELIBU data structure */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    /* Track whether mutex needs to be unlocked on exit */
    bool bUnlockMutex = false;

    do
    {
        /* Validate handle and configuration */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bPortConfigured == true, "Melibu port is not totally configured");
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bIsMulticast == false, "This API doesn't work with multicast handles");
        ADI_EAL_MELIBU_DBG_REQUIRE(poMelibuData != NULL, "Pointer to data is null");

        /* Fill MELIBU frame configuration */
        FillFrameConfig(&oMelibuTxnCfg, poMelibuData, nId, bEchoCancelled, 0U, 0U);

        /* Acquire mutex for thread-safe access */
        LockMelibuMutex(poEalMelibuData);
        bUnlockMutex = true;

        /* Build MELIBU frame */
        uint8_t anTxBuffer[ADI_EAL_MELIBU_FRAME_MAX_LENGTH];
        uint8_t nTxLength = 0U;
        eEalStatus = MelibuPeriodicBuildFrame(&oMelibuTxnCfg, anTxBuffer, &nTxLength);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Prepare FIO UART transaction configuration */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG oFioUartTxnCfg;
        (void)ADI_MEMSET(&oFioUartTxnCfg, 0, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));
        MelibuPrepareFioUartTransaction(&oFioUartTxnCfg, &oMelibuTxnCfg, nTxLength, anTxBuffer,
                                        poEalMelibuInstData->anTxBuffer);

        /* Configure echo cancellation */
        EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)poEalMelibuData->pInternalPortHandler;
        eEalStatus = adi_eal_fioUartSetEchoCancel((ADI_EAL_FIO_UART_HANDLE)poEalFioUartData, oMelibuTxnCfg.bEchoCancelled);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Queue transaction to FIO UART instances */
        eEalStatus = MelibuPeriodicQueueToFioUart(poEalFioUartData, &oFioUartTxnCfg, &oMelibuTxnCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Add to MELIBU RX queue if response expected */
        bool bIsMelibuResponseExpected = ((oMelibuTxnCfg.nResponseLength > 0U) ||
                                          (oMelibuTxnCfg.bEchoCancelled == false) ||
                                          (nId != ADI_EAL_MELIBU_NO_E2B_ID) ||
                                          IsReadTimeTagEnabled(poEalMelibuData));

        if(bIsMelibuResponseExpected)
        {
            AddTransactionToQueue(poEalMelibuData, &oMelibuTxnCfg, anTxBuffer, bIsMelibuResponseExpected);
        }

        /* Submit to E2B core and activate FIO UART periodic mode */
        eEalStatus = MelibuPeriodicSubmitToE2BCore(poEalFioUartData);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Enable periodic mode on MELIBU instances */
        MelibuPeriodicEnableMode(poEalMelibuData);

    } while(false);

    /* Release mutex if acquired */
    if(bUnlockMutex)
    {
        UnlockMelibuMutex(poEalMelibuData);
    }

    return eEalStatus;
}

/**
 * @brief       Sends multiple MELIBU transactions with full configuration control
 *
 * @param [in]  hMelibuHandle         The MELIBU interface handle
 * @param [in]  paoMelibuTxnCfg       Pointer to array of preconfigured MELIBU frame configurations.
 *                                    Each configuration specifies slave address, function selection,
 *                                    payload, response length, E2B ID, and optional delay/timestamp
 * @param [in]  nNumFrames            Number of MELIBU frames to send in this batch
 *                                    (must not exceed configured queue depth)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: All transactions queued successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or port not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Insufficient queue space for all transactions
 *
 * @details     This function provides maximum flexibility for MELIBU transactions by allowing
 *              the application to specify complete frame configurations directly. It:
 *              1. Validates handle and queue space availability
 *              2. Acquires mutex for thread-safe queue access
 *              3. Submits all frames to the internal FIO UART layer for transmission
 *              4. Releases mutex and returns status
 *
 *              Each frame is processed asynchronously, with results delivered via the
 *              registered callback function (if E2B ID != 0xFF).
 *
 * @note        - This is a non-blocking asynchronous call
 *              - All frames are queued for transmission; actual transmission occurs later
 *              - Port must be configured via #adi_eal_melibuConfigure() before use
 *              - Use this API when frame parameters cannot be derived from #ADI_EAL_MELIBU_IO_DATA
 *              - For simpler operations, use #adi_eal_melibuWriteRead() instead
 *
 * @see         adi_eal_melibuWriteRead() - Simplified API for standard operations
 * @see         adi_eal_melibuCustomWriteRead() - Advanced API with custom E2B transaction control
 * @see         adi_eal_melibuRegisterCallback() - Register callback for async completion notification
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuFlexWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_FRAME_CFG *paoMelibuTxnCfg, uint16_t nNumFrames)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;
    bool bUnlockMutex = false;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bPortConfigured == true, "Melibu port is not totally configured");
        ADI_EAL_MELIBU_DBG_REQUIRE(paoMelibuTxnCfg != NULL, "Pointer to data is null");

        /* Get the unicast instance data from the handle */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];
        uint16_t nNumResponses = 0u;
        const bool bIsReadTimeTagEn = IsReadTimeTagEnabled(poEalMelibuData);

        for(uint16_t nFrame = 0u; nFrame < nNumFrames; nFrame++)
        {
            ADI_EAL_MELIBU_FRAME_CFG *poMelibuTxnCfg = &paoMelibuTxnCfg[nFrame];

            if((poMelibuTxnCfg->bEchoCancelled == false) || (poMelibuTxnCfg->nResponseLength > 0u)
                    || (poMelibuTxnCfg->nE2bId != ADI_EAL_MELIBU_NO_E2B_ID) || bIsReadTimeTagEn)
            {
                nNumResponses++;
            }
        }

        ADI_EAL_MELIBU_DBG_REQUIRE(nNumResponses <= poEalMelibuInstData->nMelibuCfgRxQDepth, "Too many transactions");

        /* Loop through the mutex of all instances and lock it */
        LockMelibuMutex(poEalMelibuData);
        bUnlockMutex = true;

        /* Execute transaction */
        eEalStatus = MelibuSend(hMelibuHandle, paoMelibuTxnCfg, nNumFrames);

    } while(false);

    if(bUnlockMutex)
    {
        /* Loop through the mutex of all instances and unlock it */
        UnlockMelibuMutex(poEalMelibuData);
    }

    return eEalStatus;
}

/**
 * @brief       Sends custom MELIBU transactions with advanced E2B core control
 *
 * @param [in]  hMelibuHandle         The MELIBU interface handle (must be unicast)
 * @param [in]  paoMelibuTxnCfg       Pointer to array of custom MELIBU transaction configurations
 * @param [in]  nNumMelibuTxns        Number of MELIBU transactions to send
 *                                    (must not exceed configured queue depth)
 * @param [in]  bCanEchoCancel        Enable/disable CAN echo cancellation for all transactions
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: All transactions submitted successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or port not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Insufficient queue space for transactions
 *
 * @details     This advanced API allows direct submission of custom MELIBU transaction
 *              configurations with full E2B core control. It:
 *              1. Validates handle, configuration parameters, and queue availability
 *              2. Counts expected responses based on read length and echo cancellation
 *              3. Verifies sufficient queue space for all response transactions
 *              4. Acquires mutex for thread-safe queue operations
 *              5. Configures echo cancellation in the internal FIO UART interface
 *              6. Submits all transactions to FIO UART for transmission
 *              7. Adds response entries to the reception queue for each transaction
 *              8. Releases mutex and returns status
 *
 * @note        - This is a non-blocking asynchronous call
 *              - Only works with unicast handles; returns error for multicast
 *              - Bypasses standard MELIBU frame assembly; allows direct E2B control
 *              - Echo cancellation setting applies to all transactions in the batch
 *              - Transaction responses are delivered via registered callback
 *              - Port must be configured via adi_eal_melibuConfigure() before use
 *
 * @see         adi_eal_melibuFlexWriteRead() - Simplified variant using frame structures
 * @see         adi_eal_melibuWriteRead() - Standard API for simple transactions
 * @see         adi_eal_fioUartCustomWriteRead() - Underlying FIO UART function
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuCustomWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_TXN_CONFIG *paoMelibuTxnCfg, uint16_t nNumMelibuTxns, bool bCanEchoCancel)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;
    bool bUnlockMutex = false;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bPortConfigured == true, "Melibu port is not totally configured");

        /* Validate that we are not using this API with multicast handle */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bIsMulticast == false, "This API doesn't work with multicast handles");

        /* Input parameters checks */
        ADI_EAL_MELIBU_DBG_REQUIRE(paoMelibuTxnCfg != NULL, "Melibu transfer data pointer is NULL");
        ADI_EAL_MELIBU_DBG_REQUIRE(nNumMelibuTxns > 0U, "Number of frames to transmit is 0");

        /* Loop through the mutex of all instances and lock it */
        LockMelibuMutex(poEalMelibuData);
        bUnlockMutex = true;

        uint16_t nNumTxnsWithResp = 0u;
        const bool bIsReadTimeTagEn = IsReadTimeTagEnabled(poEalMelibuData);

        ADI_EAL_MELIBU_TXN_CONFIG *poMelibuTxnConfig;

        /* Calculate number of expected responses */
        for(uint16_t nTrans = 0u; nTrans < nNumMelibuTxns; nTrans++)
        {
            poMelibuTxnConfig = &paoMelibuTxnCfg[nTrans];
            uint16_t nRxLength = poMelibuTxnConfig->oTxnHdr.nReadLen;

            /* Is response expected for this transaction */
            bool bIsResponseExpected = ((nRxLength > 0U) || (bCanEchoCancel == false)
                                        || bIsReadTimeTagEn);

            /* If reception is required from application */
            if(bIsResponseExpected)
            {
                nNumTxnsWithResp++;
            }
        }

        /* Check if all the queues have enough space for responses */
        EAL_MELIBU_INST_DATA *poEalMelibuInstDataSpaceCheck = poEalMelibuData->apoInstData[0U];

        /* Space remaining in the transaction queue */
        uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                         poEalMelibuInstDataSpaceCheck->nMelibuQWrIdx,
                                         poEalMelibuInstDataSpaceCheck->nMelibuQRdIdx,
                                         poEalMelibuInstDataSpaceCheck->nMelibuCfgRxQDepth);

        /* Check if there is enough space in the transaction queue */
        if(nNumTxnsWithResp > nSpaceLeftInQueue)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;
        eEalStatus = adi_eal_fioUartSetEchoCancel(hFioUartHandle, bCanEchoCancel);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        eEalStatus = adi_eal_fioUartCustomWriteRead(hFioUartHandle, paoMelibuTxnCfg, nNumMelibuTxns);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* For each transaction to send */
        for(uint16_t nTrans = 0u; nTrans < nNumMelibuTxns; nTrans++)
        {
            poMelibuTxnConfig = &paoMelibuTxnCfg[nTrans];
            uint16_t nRxLength = poMelibuTxnConfig->oTxnHdr.nReadLen;
            uint16_t *pTxData  = (uint16_t *)poMelibuTxnConfig->pPayload;

            /* If reception is required from application */
            if(nRxLength > 0U)
            {
                /* Add the transaction to queue for all instances */
                AddCustomWriteTransactionToQueue(poEalMelibuData, (uint8_t)(nRxLength / 2U), pTxData);
            }
        }

        /* Switch all instances to CUSTOM mode */
        for(uint16_t nInstIdx = 0U; nInstIdx < poEalMelibuData->nNumInst; nInstIdx++)
        {
            poEalMelibuData->apoInstData[nInstIdx]->eMode = ADI_EAL_USAGE_MODE_CUSTOM;
        }

    } while(false);

    if(bUnlockMutex)
    {
        /* Loop through the mutex of all instances and unlock it */
        UnlockMelibuMutex(poEalMelibuData);
    }

    return eEalStatus;
}

/**
 * @brief Sets the downstream VLAN tags for MELIBU interface transactions
 *
 * @param [in] hMelibuHandle      MELIBU instance handle (must be unicast)
 * @param [in] poIfRegVlanTag     Pointer to VLAN tag for interface register access
 * @param [in] poFifoRegVlanTag   Pointer to VLAN tag for FIFO register access
 * @param [in] poFifoWriteVlanTag Pointer to VLAN tag for FIFO write access
 *
 * @return Status
 *         - #ADI_EAL_STATUS_SUCCESS: VLAN tags configured successfully
 *         - #ADI_EAL_STATUS_START_FAILED: Invalid handle or instance not opened
 *
 * @details Configures VLAN tagging for downstream MELIBU transactions by:
 *          1. Validating the MELIBU handle and instance state
 *          2. Delegating to the internal FIO UART interface to apply VLAN tags
 *          3. Each VLAN tag applies to different transaction types:
 *             - Interface register: Direct register read/write operations
 *             - FIFO register: FIFO-based register transactions
 *             - FIFO write: FIFO write-specific operations
 *
 * @note Only works with unicast MELIBU handles
 *       All three VLAN tag pointers must be non-NULL
 *       VLAN tags should be configured before sending transactions
 *
 * @see adi_eal_melibuSetUpstreamVlan() - Configure upstream VLAN tags
 * @see adi_eal_melibuOpen() - Initialize MELIBU instance
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuSetDownstreamVlan(ADI_EAL_MELIBU_HANDLE hMelibuHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag)
{

    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);

        ADI_EAL_MELIBU_DBG_REQUIRE(poIfRegVlanTag != NULL && poFifoRegVlanTag != NULL
                                   && poFifoWriteVlanTag != NULL, "Pointer to VLAN tag is NULL");

        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;
        adi_eal_fioUartSetDownstreamVlan(hFioUartHandle, poIfRegVlanTag, poFifoRegVlanTag, poFifoWriteVlanTag);
    } while(false);

    return eEalStatus;
}

/**
 * @brief Sets the upstream VLAN tag for MELIBU FIFO messages
 *
 * @param [in] hMelibuHandle    MELIBU interface handle (must be unicast)
 * @param [in] poVlanTag        Pointer to VLAN tag configuration containing:
 *                              - nVlanId: VLAN identifier
 *                              - nPCP: Priority code point (0-7)
 *                              - bDEI: Drop eligible indicator
 *                              - nTPID: Tag protocol identifier
 * @param [in] bIsVlanEn        If true, enables VLAN tagging for upstream messages;
 *                              if false, disables VLAN tagging
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: VLAN tag configured successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or multicast handle
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Register map operation failed
 *
 * @details Configures the VLAN tag to be added to upstream MELIBU FIFO messages
 *          (responses, acknowledgments, and FIFO reads from slave to master).
 *          This function delegates to the internal FIO UART interface to apply
 *          the VLAN configuration via E2B register transactions.
 *
 * @note - Only works with unicast MELIBU handles; returns error for multicast
 *       - VLAN tagging is applied to all upstream FIFO transactions
 *       - Must be called after adi_eal_melibuConfigure()
 *       - Use adi_eal_melibuSetDownstreamVlan() for downstream VLAN configuration
 *
 * @see adi_eal_melibuSetDownstreamVlan() - Configure downstream VLAN tags
 * @see adi_eal_melibuOpen() - Initialize MELIBU instance
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuSetUpstreamVlan(ADI_EAL_MELIBU_HANDLE hMelibuHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        /* Validate that we are not using this API with multicast handle */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bIsMulticast == false, "This API doesn't work with multicast handles");
        ADI_EAL_MELIBU_DBG_REQUIRE(poVlanTag != NULL, "VLAN tag pointer is NULL");

        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;
        eEalStatus = adi_eal_fioUartSetUpstreamVlan(hFioUartHandle, poVlanTag, bIsVlanEn);
    } while(false);

    return eEalStatus;
}

/**
 * @brief       Sends a register transaction through the MELIBU interface
 *
 * @param [in]  hMelibuHandle           MELIBU interface handle (must be unicast)
 * @param [in]  bIsFifoRegTransaction   If true, targets FIFO registers; if false, targets interface registers
 * @param [in]  poRegCfg                Register configuration containing address, data, and operation type
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Register transaction submitted successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or instance not opened
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Register map operation failed
 *
 * @details     Submits a low-level register read/write transaction through the internal FIO UART interface.
 *              This function is typically used for direct register access to MELIBU interface or FIFO
 *              configuration registers, bypassing the standard MELIBU frame assembly.
 *
 * @note        - Only works with unicast MELIBU handles
 *              - For standard MELIBU frame operations, use adi_eal_melibuWriteRead() instead
 *              - Port must be configured via adi_eal_melibuConfigure() before use
 *
 * @see         adi_eal_melibuWriteRead() - Higher-level transaction API for MELIBU frames
 * @see         adi_eal_fioUartSendRegTransaction() - Underlying FIO UART function
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuSendRegTransaction(ADI_EAL_MELIBU_HANDLE hMelibuHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);
        ADI_EAL_MELIBU_DBG_REQUIRE(poRegCfg != NULL, "Register configuration pointer is NULL");

        /* Rset internal interface queues */
        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;

        /* Execute register send in FIO UART interface */
        eEalStatus = adi_eal_fioUartSendRegTransaction(hFioUartHandle, bIsFifoRegTransaction, poRegCfg);

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Resets the MELIBU transaction queue to a clean state
 *
 * @param [in]  hMelibuHandle   Handle corresponding to the MELIBU instance (must be unicast)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: MELIBU queue reset successful
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or instance not opened
 *
 * @details     This function clears the MELIBU transaction queue and prepares the instance
 *              for new transactions by:
 *              1. Resetting the internal FIO UART interface queue
 *              2. Clearing queue indices (write and read pointers)
 *              3. Clearing the waiting-for-response flag
 *              4. Zeroing all RX buffers to remove stale data
 *
 *              This is useful for recovering from error states or reinitializing the
 *              interface for a new sequence of transactions.
 *
 * @note        - Only works with unicast MELIBU handles; returns error for multicast handles
 *              - Must be called after adi_eal_melibuOpen() and adi_eal_melibuConfigure()
 *              - Any pending transactions in the queue will be discarded
 *              - Use with caution as it clears all buffered responses
 *
 * @see         adi_eal_melibuOpen() - Initialize MELIBU instance
 * @see         adi_eal_melibuConfigure() - Configure MELIBU port
 * @see         adi_eal_melibuWriteRead() - Send MELIBU transactions
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuResetQueue(ADI_EAL_MELIBU_HANDLE hMelibuHandle)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;
    bool bUnlockMutex = false;

    do
    {
        /* Basic parameter checks */
        BASIC_HANDLE_CHECK(poEalMelibuData);

        /* Validate that we are not using this API with multicast handle */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bIsMulticast == false, "This API doesn't work with multicast handles");

        /* Get the unicast instance data from the handle */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];

        /* Loop through the mutex of all instances and lock it */
        LockMelibuMutex(poEalMelibuData);
        bUnlockMutex = true;

        /* Rset internal interface queues */
        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;

        eEalStatus = adi_eal_fioUartResetQueue(hFioUartHandle);

        /* Reset queues counters */
        poEalMelibuInstData->nMelibuQWrIdx = 0U;
        poEalMelibuInstData->nMelibuQRdIdx = 0U;
        poEalMelibuInstData->bWaitingForRx = false;

        /* Reset mode back to NORMAL */
        poEalMelibuInstData->eMode = ADI_EAL_USAGE_MODE_NORMAL;

        /* Clean all Rx buffer */
        (void)ADI_MEMSET((uint8_t *)poEalMelibuInstData->aoRxMelibuBuffer, 0,
                         (poEalMelibuInstData->nMelibuCfgRxQDepth * sizeof(ADI_EAL_MELIBU_REC_DATA)));
    } while(false);

    if(bUnlockMutex)
    {
        /* Loop through the mutex of all instances and unlock it */
        UnlockMelibuMutex(poEalMelibuData);
    }

    return eEalStatus;
}

/**
 * @brief       Enables or disables mailbox mode for MELIBU transactions
 *
 * @param [in]  hMelibuHandle   The MELIBU interface handle (must be unicast)
 * @param [in]  bEnable         If true, enables mailbox mode; if false, disables it
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Mailbox mode configured successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or instance not opened
 *
 * @details     Configures mailbox mode operation for the MELIBU interface by delegating
 *              to the internal FIO UART interface. Mailbox mode provides:
 *              - Guaranteed in-order delivery of transactions
 *              - Sequence number management via mailbox protocol
 *              - Simplified frame synchronization
 *
 *              When disabled, standard transaction queueing applies without
 *              mailbox-specific sequence number handling.
 *
 * @note        - Only works with unicast MELIBU handles
 *              - Use adi_eal_melibuResyncSeqNum() to recover from message drops
 *              - Mailbox mode must be configured before sending transactions
 *
 * @see adi_eal_melibuResyncSeqNum() - Resynchronize sequence numbers after message loss
 * @see adi_eal_melibuWriteRead() - Send MELIBU transactions
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuSetMailboxMode(ADI_EAL_MELIBU_HANDLE hMelibuHandle, bool bEnable)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract MELIBU data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Validate handle is not NULL */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData != NULL, "Handle is NULL");

        /* Verify instance has been opened */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bInUse == true, "Instance is not opened");

        /* Get the internal FIO UART port handler */
        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;

        /* Configure mailbox mode on underlying FIO UART layer */
        eEalStatus = adi_eal_fioUartSetMailboxMode(hFioUartHandle, bEnable);

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Triggers sequence number resynchronization for MELIBU transactions
 *
 * @param [in]  hMelibuHandle   The MELIBU interface handle (must be unicast)
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Resync request submitted successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or instance not opened
 *
 * @details     This function sets the sync bit in the next outgoing MELIBU message to request
 *              sequence number resynchronization. This is used to recover from message drop
 *              failures in mailbox mode by resetting both master and slave sequence counters
 *              to a known state (typically 0).
 *
 *              The function delegates to the internal FIO UART interface to enable sync mode,
 *              which will automatically insert the sync bit into the next transmitted frame.
 *
 * @note        - Only works with unicast MELIBU handles
 *              - The sync bit takes effect on the next transaction sent via adi_eal_melibuWriteRead()
 *              - Typically called after detecting sequence number mismatches or message losses
 *              - Use #adi_eal_melibuSetMailboxMode() to enable mailbox mode before using this function
 *
 * @see adi_eal_melibuSetMailboxMode() - Enable mailbox mode with sequence number tracking
 * @see adi_eal_melibuWriteRead() - Send transactions with automatic sync bit insertion
 */
ADI_EAL_MELIBU_CODE
ADI_EAL_STATUS adi_eal_melibuResyncSeqNum(ADI_EAL_MELIBU_HANDLE hMelibuHandle)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Extract MELIBU data structure from handle */
        EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

        /* Validate handle is not NULL */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData != NULL, "Handle is NULL");

        /* Verify instance has been opened */
        ADI_EAL_MELIBU_DBG_REQUIRE(poEalMelibuData->bInUse == true, "Instance is not opened");

        /* Get internal FIO UART handler for this MELIBU instance */
        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;

        /* Resynchronize sequence numbers on underlying FIO UART layer */
        eEalStatus = adi_eal_fioUartResyncSeqNum(hFioUartHandle);

    } while(false);

    return eEalStatus;
}

/** @cond PRIVATE */
/*============================= CALLBACKS ============================*/

/**
 * @brief Internal FIO UART callback handler that bridges FIO UART events to MELIBU layer
 *
 * @param [in] nInstNum      Instance number from the E2B event
 * @param [in] hUartHandle   FIO UART handle generating the callback
 * @param [in] eUartCbkEvent FIO UART callback event type
 * @param [in] pArg          Pointer to event-specific data (transaction config, FIFO read data, etc.)
 *
 * @details This internal callback is automatically registered during adi_eal_melibuOpen()
 *          and processes FIO UART events to deliver responses to the MELIBU application layer:
 *
 *          **For Read Response Events:**
 *          - Retrieves the queued RX buffer entry from the MELIBU reception queue
 *          - Copies received frame data into the application's RX buffer
 *          - Extracts and stores the E2B transaction ID if present
 *          - Clears the waiting-for-response flag for synchronous operations
 *          - Invokes the application callback with the response data
 *
 *          **For FIFO Read Events:**
 *          - Similar processing as read response events
 *          - Handles frames received via FIFO-based transactions
 *
 *          **For Unexpected Events:**
 *          - Handles queue empty conditions (unexpected frame reception)
 *          - Notifies application via callback with ADI_EAL_MELIBU_CBK_EVT_MELIBU_UNEXPECTED_RX
 *
 * @note This callback bridges the FIO UART interrupt context to the MELIBU layer.
 *       Do not call this function directly; it is invoked internally by the FIO UART interface.
 *
 * @see adi_eal_melibuOpen() - Registers this callback with FIO UART
 * @see adi_eal_melibuRegisterCallback() - Application callback registration
 * @see adi_eal_melibuWriteRead() - Initiates transactions that trigger this callback
 *
 * @internal
 */
ADI_EAL_MELIBU_CODE
static void MelibuFioUartNotification(
    uint32_t                    nInstNum,
    ADI_EAL_FIO_UART_HANDLE     hUartHandle,
    ADI_EAL_FIO_UART_CBK_EVT    eUartCbkEvent,
    const void                  *pArg
)
{

    EAL_FIO_UART_INST_DATA *poEalFioUartInstData;

    const ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg;

    /* Get the internal data from the handle */
    EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)hUartHandle;

    /* Get the unicast instance data */
    poEalFioUartInstData = poEalFioUartData->apoInstData[0U];
    ADI_EAL_MELIBU_HANDLE hMelibuHandle = poEalFioUartInstData->hMelibuHandle;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    /* Get the unicast instance data from the handle */
    EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];

    switch(eUartCbkEvent)
    {
        case ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD:
        {
            /* Parse input parameter */
            poTxnCfg = (const ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_DATA *)pArg;
            MelibuFiouUartReadRespNotification(hMelibuHandle, poTxnCfg, ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD);
            break;
        }

        case ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP:
        {
            /* Parse input parameter */
            poTxnCfg = (const ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_DATA *)pArg;
            MelibuFiouUartReadRespNotification(hMelibuHandle, poTxnCfg, ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP);
            break;
        }

        case ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT:
        {
            /* Parse input parameter */
            poTxnCfg = (const ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_DATA *)pArg;
            MelibuFiouUartReadRespNotification(hMelibuHandle, poTxnCfg, ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT);
            break;
        }

        case ADI_EAL_FIO_UART_CBK_EVT_UART_FIFO_READ:
        {
            ADI_EAL_MELIBU_CBK_EVT eMelibuCbkEvent;
            void *EvArg = NULL;

            /* Get the details of the FIFO read */
            const ADI_E2BCORE_CBK_FIFOREADDAT *poFifoDat = (const ADI_E2BCORE_CBK_FIFOREADDAT *)pArg;

            /* Parse input parameter */
            poTxnCfg = (const ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_DATA *)poFifoDat->poInfo;
            ADI_EAL_MELIBU_CBK_EVT_MELIBU_FIFO_READ_DATA oMelibuReadRespData;

            if(!EAL_IS_Q_EMPTY(poEalMelibuInstData->nMelibuQWrIdx, poEalMelibuInstData->nMelibuQRdIdx,
                               poEalMelibuInstData->nMelibuCfgRxQDepth))
            {
                /* Increment the read index */
                poEalMelibuInstData->nMelibuQRdIdx = EAL_INC_Q_IDX(poEalMelibuInstData->nMelibuQRdIdx,
                                                     poEalMelibuInstData->nMelibuCfgRxQDepth);

                /* Set reception flag for Sync frames */
                poEalMelibuInstData->bWaitingForRx = false;

                do
                {
                    /* Mount the response data */
                    oMelibuReadRespData.nRxLength = poTxnCfg->oTxnHdr.nReadLen;

                    /* If E2B ID is present take it */
                    if(poTxnCfg->oTxnHdr.bIsE2bIdPresent == true)
                    {
                        oMelibuReadRespData.nId = poTxnCfg->oTxnHdr.nE2bId;
                    }
                    else
                    {
                        oMelibuReadRespData.nId = ADI_EAL_MELIBU_NO_E2B_ID;
                    }

                    /* Take reception payload */
                    uint16_t *pRxData = (uint16_t *)poTxnCfg->pPayload;

                    /* For FIFO read, just point to the received data */
                    oMelibuReadRespData.aPortBufferRx =  &pRxData[0U];

                    EvArg = (void *)&oMelibuReadRespData;

                    /* Execute callback with event of MELIBU read response received */
                    eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_FIFO_READ;

                } while(false);
            }
            else
            {
                /* Execute callback with event of unexpected frame received */
                eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_UNEXPECTED_RX;
                EvArg = NULL;
            }

            /* Execute delegated callback if it is installed */
            if(poEalMelibuInstData->pfunCbk != NULL)
            {
                poEalMelibuInstData->pfunCbk(poEalMelibuInstData->oId.nInstNum, hMelibuHandle, eMelibuCbkEvent,
                                             (const void *)EvArg);
            }

            break;
        }

        case ADI_EAL_FIO_UART_CBK_EVT_PERIODIC_RX:
        {
            /* PERIODIC mode: fioUart gives PERIODIC_RX callback with transaction config.
             * Do NOT increment melibu RdIdx — transaction stays in queue for re-use.
             * fioUart layer handles e2bcore re-queuing internally. */
            poTxnCfg = (const ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)pArg;

            ADI_EAL_MELIBU_RESP_DATA oPeriodicRespData;
            void *pPeriodicEvArg = NULL;

            if(!EAL_IS_Q_EMPTY(poEalMelibuInstData->nMelibuQWrIdx, poEalMelibuInstData->nMelibuQRdIdx,
                               poEalMelibuInstData->nMelibuCfgRxQDepth))
            {
                /* Mount the response data */
                oPeriodicRespData.nRxLength = poTxnCfg->oTxnHdr.nReadLen;

                if(poTxnCfg->oTxnHdr.bIsE2bIdPresent == true)
                {
                    oPeriodicRespData.nId = poTxnCfg->oTxnHdr.nE2bId;
                }
                else
                {
                    oPeriodicRespData.nId = ADI_EAL_MELIBU_NO_E2B_ID;
                }

                oPeriodicRespData.aPortBufferRx = (uint16_t *)poTxnCfg->pPayload;
                pPeriodicEvArg = (void *)&oPeriodicRespData;
            }

            /* Give periodic RX callback to application */
            if(poEalMelibuInstData->pfunCbk != NULL)
            {
                poEalMelibuInstData->pfunCbk(poEalMelibuInstData->oId.nInstNum, hMelibuHandle,
                                             ADI_EAL_MELIBU_CBK_EVT_PERIODIC_RX, (const void *)pPeriodicEvArg);
            }

            break;
        }

        case ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK:
        {
            /* Get the mailbox ACK data from FIO UART */
            const ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK_DATA *poFioUartAckData =
                (const ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK_DATA *)pArg;

            /* Map to Melibu mailbox ACK data (same structure) */
            ADI_EAL_MELIBU_CBK_EVT_MAILBOX_ACK_DATA oMelibuAckData;
            oMelibuAckData.nSeqNum = poFioUartAckData->nSeqNum;
            oMelibuAckData.bFd     = poFioUartAckData->bFd;
            oMelibuAckData.nTopic  = poFioUartAckData->nTopic;

            /* Execute delegated callback if it is installed */
            if(poEalMelibuInstData->pfunCbk != NULL)
            {
                poEalMelibuInstData->pfunCbk(poEalMelibuInstData->oId.nInstNum,
                                             hMelibuHandle,
                                             ADI_EAL_MELIBU_CBK_EVT_MAILBOX_ACK,
                                             (const void *)&oMelibuAckData);
            }

            break;
        }

        default:
        {
            if(poEalMelibuInstData->pfunCbk != NULL)
            {
                poEalMelibuInstData->pfunCbk(poEalMelibuInstData->oId.nInstNum, hMelibuHandle, (ADI_EAL_MELIBU_CBK_EVT)eUartCbkEvent,
                                             pArg);
            }

            break;
        }
    }
}

/*============================= STATIC FUNCTIONS ============================*/
ADI_EAL_MELIBU_CODE
static void MelibuFiouUartReadRespNotification(
    ADI_EAL_MELIBU_HANDLE hMelibuHandle,
    const ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poTxnCfg,
    ADI_EAL_FIO_UART_CBK_EVT    eUartCbkEvent)
{
    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    /* Get the unicast instance data from the handle */
    EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];
    ADI_EAL_MELIBU_CBK_EVT eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_UNEXPECTED_RX;
    ADI_EAL_MELIBU_RESP_DATA oMelibuReadRespData;

    void *EvArg = NULL;

    if(!EAL_IS_Q_EMPTY(poEalMelibuInstData->nMelibuQWrIdx, poEalMelibuInstData->nMelibuQRdIdx,
                       poEalMelibuInstData->nMelibuCfgRxQDepth))
    {
        /* Get the Rx buffer */
        ADI_EAL_MELIBU_REC_DATA *poRxMelibuBuffer = (ADI_EAL_MELIBU_REC_DATA *)
            &poEalMelibuInstData->aoRxMelibuBuffer[poEalMelibuInstData->nMelibuQRdIdx];

        if(eUartCbkEvent == ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD)
        {
            /* Decrement expected length as breakfield is not part of it */
            poRxMelibuBuffer->nExpectedRxLength -= 1U;
        }

        /* Increment the read index. This function is only called for NORMAL mode events
         * from fioUart (READ_RESP, BREAKFIELD, RX_DATA_CORRUPT). PERIODIC mode is handled
         * by the PERIODIC_RX case in MelibuFioUartNotification. */
        poEalMelibuInstData->nMelibuQRdIdx = EAL_INC_Q_IDX(poEalMelibuInstData->nMelibuQRdIdx,
                                             poEalMelibuInstData->nMelibuCfgRxQDepth);

        /* Set reception flag for Sync frames */
        poEalMelibuInstData->bWaitingForRx = false;

        do
        {
            /* Mount the response data */
            oMelibuReadRespData.nRxLength = poTxnCfg->oTxnHdr.nReadLen;

            /* If E2B ID is present take it */
            if(poTxnCfg->oTxnHdr.bIsE2bIdPresent == true)
            {
                oMelibuReadRespData.nId = poTxnCfg->oTxnHdr.nE2bId;
            }
            else
            {
                oMelibuReadRespData.nId = ADI_EAL_MELIBU_NO_E2B_ID;
            }

            oMelibuReadRespData.aPortBufferRx = (uint16_t *)poTxnCfg->pPayload;

            EvArg = (void *)&oMelibuReadRespData;

            if(eUartCbkEvent == ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT)
            {
                /* Execute callback with event of MELIBU read response with data corruption */
                eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_RX_DATA_CORRUPT;
                break;
            }

            /* If number of bytes received are not the expected */
            if(poTxnCfg->oTxnHdr.nReadLen != poRxMelibuBuffer->nExpectedRxLength)
            {
                if(eUartCbkEvent == ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD)
                {
                    /* Execute callback with event of Melibu read response with reception length error with CAN echo */
                    eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR_CAN_ECHO;
                }
                else
                {
                    /* Execute callback with event of Melibu read response with reception length error */
                    eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR;
                }

                break;
            }

            if(poRxMelibuBuffer->bMelibuResponseExpected == true)
            {

                /* Calculate payload length excluding CRC */
                uint8_t nPayloadLength = poRxMelibuBuffer->nExpectedRxLength - 2U;
                uint16_t *pPayload = (uint16_t *)poTxnCfg->pPayload;

                /* Extract received CRC */
                uint16_t nCrcReceived = (pPayload[poRxMelibuBuffer->nExpectedRxLength - 1U] & 0x00FFU);
                nCrcReceived <<= 8U;
                nCrcReceived |= (pPayload[poRxMelibuBuffer->nExpectedRxLength - 2U] & 0x00FFU);

                if(eUartCbkEvent == ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD)
                {
                    nPayloadLength -= 2U;
                    pPayload += 2U;
                }

                /* Calculate CRC */
                bool bCrcValid = MelibuCrcVerification(poRxMelibuBuffer->anMelibuFrameID, pPayload,
                                                       nPayloadLength, nCrcReceived);

                /* If CRC and Melibu ID are valid */
                if(bCrcValid == false)
                {
                    /* Execute callback with event of MELIBU read response with data corruption */
                    eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_RX_DATA_CORRUPT;
                }
                else
                {
                    if(eUartCbkEvent == ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD)
                    {
                        /* Execute callback with event of MELIBU read response with breakfield */
                        eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP_CAN_ECHO;
                        break;
                    }

                    if(eUartCbkEvent == ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP)
                    {
                        /* Execute callback with event of MELIBU read response */
                        eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP;
                        break;
                    }

                }
            }
            else
            {
                /* Execute callback with event of MELIBU write CAN echo */
                eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_WRITE_CAN_ECHO;
            }

        } while(false);
    }
    else
    {
        /* Execute callback with event of unexpected frame received */
        eMelibuCbkEvent = ADI_EAL_MELIBU_CBK_EVT_MELIBU_UNEXPECTED_RX;
        EvArg = NULL;
    }

    /* Execute delegated callback: this function is only called for NORMAL mode events from fioUart
     * (READ_RESP, BREAKFIELD, RX_DATA_CORRUPT). PERIODIC mode is handled by PERIODIC_RX case
     * in MelibuFioUartNotification. CUSTOM mode is handled by UART_FIFO_READ case. */
    if(poEalMelibuInstData->pfunCbk != NULL)
    {
        poEalMelibuInstData->pfunCbk(poEalMelibuInstData->oId.nInstNum, hMelibuHandle, eMelibuCbkEvent,
                                     (const void *)EvArg);
    }

}

/**
 * @brief Fills MELIBU frame configuration from application data
 *
 * @param [out] poMelframeCfg    Pointer to MELIBU frame configuration structure to populate
 * @param [in]  poMelFramedata   Pointer to MELIBU frame data provided by application
 * @param [in]  nId              E2B transaction ID for tracking (0xFF = no ID)
 * @param [in]  bEchoCancelled   Flag indicating if slave handles echo cancellation
 * @param [in]  nDelay           Programmable delay in nanoseconds before transmission
 * @param [in]  nTimestamp       gPTP presentation timestamp for time-synchronized transmission
 *
 * @return ADI_EAL_STATUS
 *         - #ADI_EAL_STATUS_SUCCESS: Frame configuration populated successfully
 *
 * @details This function bridges application-level MELIBU data to internal frame configuration
 *          by copying all relevant parameters including timing and echo cancellation settings.
 *          The populated configuration is then used by lower-layer functions to assemble
 *          and transmit the MELIBU frame.
 *
 * @see adi_eal_melibuWriteRead() - Calls this function internally
 * @see MelibuPrepareFioUartTransaction() - Uses the populated configuration
 */
ADI_EAL_MELIBU_CODE
static void FillFrameConfig(ADI_EAL_MELIBU_FRAME_CFG *poMelframeCfg, ADI_EAL_MELIBU_IO_DATA *poMelFramedata,
                            uint8_t nId, bool bEchoCancelled, uint32_t nDelay, uint64_t nTimestamp)
{
    /* Copy slave addressing information */
    poMelframeCfg->nSlaveAdd       = poMelFramedata->nSlaveAdd;

    /* Set read/transmit direction flag */
    poMelframeCfg->nR_T            = poMelFramedata->nR_T;

    /* Configure function selection (LED/NO_LED) */
    poMelframeCfg->eFuncSel        = poMelFramedata->eFuncSel;

    /* Set payload length in bytes */
    poMelframeCfg->nPayloadLength  = poMelFramedata->nPayloadLength;

    /* Configure instruction word selector and value */
    poMelframeCfg->nInstWordSel    = poMelFramedata->nInstWordSel;
    poMelframeCfg->nInstWord       = poMelFramedata->nInstWord;

    /* Set Protocol Control Information (PCI) */
    poMelframeCfg->nPci            = poMelFramedata->nPci;

    /* Configure expected response length */
    poMelframeCfg->nResponseLength = poMelFramedata->nResponseLength;

    /* Set pointers to transmit and receive payload buffers */
    poMelframeCfg->anPayload       = poMelFramedata->anPayload;
    poMelframeCfg->anRxPayload     = poMelFramedata->anRxPayload;

    /* Set E2B transaction ID for response correlation */
    poMelframeCfg->nE2bId          = nId;

    /* Configure echo cancellation setting */
    poMelframeCfg->bEchoCancelled  = bEchoCancelled;

    /* Set inter-frame delay in microseconds */
    poMelframeCfg->nDelay          = nDelay;

    /* Store timestamp for transaction timing */
    poMelframeCfg->nPressTs        = nTimestamp;
}

/**
 * @brief       Sets the waiting-for-response flag for all MELIBU instances
 *
 * @param [in]  poEalMelibuData  Pointer to MELIBU interface data containing instance array
 * @param [in]  bWaitingForRx    If true, sets flag to wait for response; if false, clears it
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: Flag updated successfully for all instances
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid data pointer
 *
 * @details     Updates the waiting-for-response flag for all instances in a MELIBU interface group.
 *              This flag is used by synchronous operations to determine when a response has been
 *              received. For unicast handles, this updates a single instance; for multicast
 *              handles, it updates all instances in the group.
 *
 * @note        This is an internal helper function typically called before and after
 *              submitting MELIBU transactions during synchronous operations.
 *
 * @see         WaitForResponses() - Polls this flag until response arrives or timeout occurs
 * @see         adi_eal_melibuWriteReadSync() - Uses this function internally
 */
ADI_EAL_MELIBU_CODE
static void SetWaitingForRx(EAL_MELIBU_DATA *poEalMelibuData, bool bWaitingForRx)
{

    ADI_DBG_REQUIRE(poEalMelibuData != NULL, "Melibu data pointer NULL in FillFrameConfig function");

    for(uint16_t i = 0U; i < poEalMelibuData->nNumInst; i++)
    {
        /* Get the unicast instance data */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[i];

        /* Set the waiting for RX flag */
        poEalMelibuInstData->bWaitingForRx = bWaitingForRx;
    }
}

/**
 * @brief       Waits for MELIBU transaction responses with timeout
 *
 * @param [in]  poEalMelibuData  Pointer to MELIBU interface data structure
 *
 * @return      bool
 *              - true: Response timeout occurred (no responses received within timeout period)
 *              - false: All pending responses received successfully
 *
 * @details     This function blocks until all MELIBU instances receive their expected responses
 *              or a timeout occurs. It:
 *              1. Continuously processes network events via adi_network_run()
 *              2. Checks waiting-for-response flags for all instances
 *              3. Compares elapsed time against the configured timeout (ADI_EAL_RESP_TIMEOUT_MS)
 *              4. Returns when all responses arrive or timeout is exceeded
 *
 * @note        - Used internally by synchronous transaction APIs
 *              - Should only be called with valid, non-NULL MELIBU data pointer
 *              - Blocks the calling thread during the wait period
 *
 * @see         SetWaitingForRx() - Sets response waiting flags before transaction
 * @see         adi_eal_melibuWriteReadSync() - Blocking API that uses this function
 */
ADI_EAL_MELIBU_CODE
static bool WaitForResponses(EAL_MELIBU_DATA *poEalMelibuData)
{
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

        for(uint16_t i = 0U; i < poEalMelibuData->nNumInst; i++)
        {
            /* Get the unicast instance data */
            EAL_MELIBU_INST_DATA *poEalMelibuiInstData = poEalMelibuData->apoInstData[i];

            /* Check if we are still waiting for response for this instance */
            if(poEalMelibuiInstData->bWaitingForRx)
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

/**
 * @brief Prepares a FIO UART transaction configuration from a MELIBU frame
 *
 * @param[out] poFioUartTxnCfg  Pointer to FIO UART transaction configuration to populate
 * @param[in]  poMelibuFrame    Pointer to MELIBU frame configuration with timing and response parameters
 * @param[in]  nTxLength        Length of the MELIBU frame data in bytes
 * @param[in]  anTxBuffer       Pointer to buffer containing the assembled MELIBU frame
 * @param[out] pBuffer          Pointer to output buffer for FIO UART payload (must be at least nTxLength bytes)
 *
 * @return ADI_EAL_STATUS
 *         - #ADI_EAL_STATUS_SUCCESS: Transaction configuration prepared successfully
 *
 * @details This function configures a FIO UART transaction by:
 *          1. Setting common transaction header flags (packing, E2B ID presence)
 *          2. Configuring optional timing: delay and/or gPTP presentation timestamp
 *          3. Setting FIO UART-specific parameters: read/write lengths and interface header
 *          4. Copying MELIBU frame data to output buffer
 *          5. Adjusting response length for echo cancellation mode if applicable
 *
 * @note    - Echo cancellation affects response length calculation:
 *            - If disabled: response includes TX echo + slave data + 1 breakfield byte
 *            - If enabled: response is only slave data
 *          - The output buffer must be pre-allocated by the caller
 *
 * @see adi_eal_melibuWriteRead() - Uses this function to prepare transactions
 * @see adi_eal_fioUartCustomWriteRead() - Underlying FIO UART transmission function
 */
ADI_EAL_MELIBU_CODE
static void MelibuPrepareFioUartTransaction(
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFioUartTxnCfg,
    ADI_EAL_MELIBU_FRAME_CFG *poMelibuFrame,
    uint8_t nTxLength,
    uint8_t *anTxBuffer,
    uint16_t *pBuffer)
{
    /* Extract expected response length from MELIBU frame configuration */
    uint8_t nRxLength = poMelibuFrame->nResponseLength;

    /* Validate transmit buffer pointer is not NULL */
    ADI_DBG_REQUIRE(anTxBuffer != NULL,
                    " Pointer to buffer containing the assembled MELIBU frame data NULL in MelibuPrepareFioUartTransaction function");

    /* Validate output buffer pointer is not NULL */
    ADI_DBG_REQUIRE(pBuffer != NULL,
                    " Pointer to output buffer NULL in MelibuPrepareFioUartTransaction function");

    /* === Configure common transaction header flags === */

    /* Transactions are not packed (single MELIBU frame per FIO transaction) */
    poFioUartTxnCfg->oTxnHdr.bAreTxnsPacked     = false;

    /* No resync required for this transaction */
    poFioUartTxnCfg->oTxnHdr.bResyncTillThisTxn = false;

    /* This is a standalone transaction */
    poFioUartTxnCfg->oTxnHdr.bContinueNextTxn   = false;

    /* Check if E2B ID is present for response correlation */
    poFioUartTxnCfg->oTxnHdr.bIsE2bIdPresent    = (poMelibuFrame->nE2bId != ADI_EAL_MELIBU_NO_E2B_ID);

    /* Copy E2B transaction ID */
    poFioUartTxnCfg->oTxnHdr.nE2bId             = poMelibuFrame->nE2bId;

    /* Configure inter-frame delay if specified */
    if(poMelibuFrame->nDelay > 0U)
    {
        /* Enable time delay and set delay value */
        poFioUartTxnCfg->oTxnHdr.bIsTimeDelayPresent = true;
        poFioUartTxnCfg->oTxnHdr.nTimeDelay = poMelibuFrame->nDelay;
    }
    else
    {
        /* No delay required for this transaction */
        poFioUartTxnCfg->oTxnHdr.bIsTimeDelayPresent = false;
        poFioUartTxnCfg->oTxnHdr.nTimeDelay = 0U;
    }

    if(poMelibuFrame->nPressTs > 0U)
    {
        poFioUartTxnCfg->oTxnHdr.bIsGptpTimePresent = true;
        poFioUartTxnCfg->oTxnHdr.nGptpTime = poMelibuFrame->nPressTs;
    }
    else
    {
        poFioUartTxnCfg->oTxnHdr.bIsGptpTimePresent = false;
        poFioUartTxnCfg->oTxnHdr.nGptpTime = 0U;
    }

    /* ~~~~~ Interface specific header flags ~~~~~ */
    poFioUartTxnCfg->oTxnHdr.bEnIntfTxnHdr     = true;
    poFioUartTxnCfg->oTxnHdr.bOverrideReadLen  = true;
    poFioUartTxnCfg->oTxnHdr.nReadLen          = nRxLength;
    poFioUartTxnCfg->oTxnHdr.bOverrideWriteLen = true;
    poFioUartTxnCfg->oTxnHdr.nWriteLen         = nTxLength;
    poFioUartTxnCfg->oTxnHdr.bIsRouterHeaderInserted = true;

    for(uint8_t i = 0u; i < nTxLength; i++)
    {
        pBuffer[i] = anTxBuffer[i];
    }

    poFioUartTxnCfg->pPayload = pBuffer;

    if(poMelibuFrame->bEchoCancelled == false)
    {
        /* If CAN echo is not cancelled FIO UART will receive Rx + Tx + 1 per breakfield */
        poMelibuFrame->nResponseLength += nTxLength + 1U;
    }
}

/**
 * @brief Encodes MELIBU 2.0 payload length into the frame PL_Length field
 *
 * @param [in]  eFuncSel            Function selection (LED or NO_LED)
 * @param [in,out] nPayloadLength   Input: actual payload length in bytes
 *                                  Output: adjusted to valid length for the function selection
 * @param [out] nPL_Length          Encoded PL_Length value (3-bit field, 0-7) for frame assembly
 *
 * @return Status
 *         - #ADI_EAL_STATUS_SUCCESS: Payload length successfully encoded
 *         - #ADI_EAL_STATUS_START_FAILED: Invalid function selection or unsupported payload length
 *
 * @details  Converts an actual payload length to the 3-bit PL_Length field value used in MELIBU 2.0
 *          frame headers. The valid payload lengths depend on the function selection:
 *       PL_Length  FUNC_SEL_LED  FUNC_SEL_NO_LED
 *           0             6              0
 *           1            12              2
 *           2            24              4
 *           3            36              6
 *           4            48              8
 *           5            60             10
 *           6            84             18
 *           7           128             24
 *
 * @details If eFuncSel is out of range, an error is returned and nPL_Length is set to 0
 *          if nPayloadLength is out of valid values, an error is returned and nPL_Length is set to 0
 *
 * @note The PL_Length field uses only 3 bits (values 0-7) to represent 8 possible payload lengths.
 *       This function maps actual byte lengths to these encoded values for frame construction.
 *
 * @see MelibuSlaveTransmitConfigData() - Uses this function for frame assembly
 * @see MelibuSlaveReceiveConfigData() - Uses this function for frame assembly
 */
ADI_EAL_MELIBU_CODE
static ADI_EAL_STATUS MelibuPlLengthEncode(ADI_EAL_MELIBU_FUNCTION_SELECT eFuncSel, uint8_t *nPayloadLength,
        uint8_t *nPL_Length)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* Validate payload length pointer is not NULL */
        ADI_DBG_REQUIRE(nPayloadLength != NULL,
                        "Pointer to Payload length NULL in MelibuPlLengthEncode function");

        /* Validate output PL_Length pointer is not NULL */
        ADI_DBG_REQUIRE(nPL_Length != NULL,
                        "Pointer to output PLLength NULL in MelibuPlLengthEncode function");

        /* Encode payload length based on function selection */
        switch(eFuncSel)
        {
            /* NO_LED mode: payload lengths are multiples of 2 bytes */
            case ADI_EAL_MELIBU_FUNC_SEL_NO_LED:
            {
                /* Handle standard lengths (0-10 bytes) */
                if(*nPayloadLength <= 10U)
                {
                    /* Payload must be even number of bytes */
                    if(*nPayloadLength % 2U != 0U)
                    {
                        /* Invalid odd length - return error */
                        eEalStatus = ADI_EAL_STATUS_START_FAILED;
                        *nPL_Length = 0U;
                    }
                    else
                    {
                        /* Encode as length / 2 */
                        *nPL_Length = *nPayloadLength / 2U;
                        *nPayloadLength = *nPL_Length * 2U;
                    }
                }
                /* Special encoding for 18-byte payload */
                else if(*nPayloadLength == 18U)
                {
                    *nPL_Length = 6U;
                    *nPayloadLength = 18U;
                }
                /* Special encoding for 24-byte payload */
                else if(*nPayloadLength == 24U)
                {
                    *nPL_Length = 7U;
                    *nPayloadLength = 24U;
                }
                else
                {
                    /* Invalid payload length for NO_LED mode */
                    eEalStatus = ADI_EAL_STATUS_START_FAILED;
                    *nPL_Length = 0U;
                }

                break;
            }

            /* LED mode: payload lengths are multiples of 12 bytes (except 6) */
            case ADI_EAL_MELIBU_FUNC_SEL_LED:
            {
                /* Special case: 6 bytes encodes as 0 */
                if(*nPayloadLength == 6U)
                {
                    *nPL_Length = 0U;
                }
                /* Handle range 12-60 bytes */
                else if(*nPayloadLength <= 60U)
                {
                    /* Must be multiple of 12 bytes and non-zero */
                    if((*nPayloadLength % 12U != 0U) || (*nPayloadLength == 0U))
                    {
                        /* Invalid length - not multiple of 12 */
                        eEalStatus = ADI_EAL_STATUS_START_FAILED;
                        *nPL_Length = 0U;
                    }
                    else
                    {
                        *nPL_Length = *nPayloadLength / 12U;
                        *nPayloadLength = *nPL_Length * 12U;
                    }
                }
                else if(*nPayloadLength == 84U)
                {
                    *nPL_Length = 6U;
                    *nPayloadLength = 84U;
                }
                else if(*nPayloadLength == 128U)
                {
                    *nPL_Length = 7U;
                    *nPayloadLength = 128U;
                }
                else
                {
                    /* Shouldn't come here! */
                    eEalStatus = ADI_EAL_STATUS_START_FAILED;
                    *nPL_Length = 0U;
                }

                break;
            }

            default:
            {
                /* Shouldn't reach here!! */
                eEalStatus = ADI_EAL_STATUS_START_FAILED;
                *nPL_Length = 0U;
                break;
            }
        }

    } while(false);

    return eEalStatus;
}

/**
 * @brief Computes parity bits for MELIBU 2.0 frame ID field
 *
 * @param [in] nId         16-bit ID field value from which parity is calculated
 * @param [in] eParityType Type of parity to compute:
 *                         - #ADI_EAL_MELIBU_PARITY_TYPE_P0: Parity over bits 8-13
 *                         - #ADI_EAL_MELIBU_PARITY_TYPE_P1: Parity over bits 0-13
 *
 * @return uint8_t         Computed parity bit (0 or 1) using odd parity
 *
 * @details This function calculates either P0 or P1 parity bit for frame validation:
 *          - Applies the appropriate mask to extract relevant bits from the ID
 *          - Computes odd parity over the masked bits
 *          - Returns the calculated parity bit value
 *
 *          The computed parity bits are inserted into the frame ID at:
 *          - Bit 6 (P0): Parity over bits 8-13
 *          - Bit 7 (P1): Parity over bits 0-13
 *
 * @note Used internally during MELIBU frame assembly to ensure frame integrity.
 *
 * @see MelibuComputeParity() - Core parity calculation
 * @see MelibuSlaveTransmitConfigData() - Frame assembly using this function
 * @see MelibuSlaveReceiveConfigData() - Frame assembly using this function
 */
ADI_EAL_MELIBU_CODE
static uint8_t MelibuComputeParityBits(uint16_t nId, ADI_EAL_MELIBU_PARITY_TYPES eParityType)
{
    uint16_t nData  = 0U;

    /* Computed parity bit value (0 or 1) */
    uint8_t nParity = 0U;

    /* Calculate parity based on requested type */
    switch(eParityType)
    {
        /* P0 parity: odd parity over bits 0,2,4,6,8,10,12 */
        case ADI_EAL_MELIBU_PARITY_TYPE_P0:
        {
            /* Mask ID to extract bits covered by P0 */
            nData = (uint16_t)ADI_EAL_MELIBU_P0_MASK & nId;

            /* Compute odd parity over masked bits */
            nParity = MelibuComputeParity(ADI_EAL_ODD_PARITY, nData);
            break;
        }

        /* P1 parity: odd parity over bits 0-13 */
        case ADI_EAL_MELIBU_PARITY_TYPE_P1:
        {
            /* Mask ID to extract bits covered by P1 */
            nData = (uint16_t)ADI_EAL_MELIBU_P1_MASK & nId;

            /* Compute odd parity over masked bits */
            nParity = MelibuComputeParity(ADI_EAL_ODD_PARITY, nData);
            break;
        }

        default:
        {
            /* Shouldn't reach here!! */
            break;
        }
    }

    return nParity;
}

/**
 * @brief       Computes odd parity for MELIBU 2.0 frame fields
 *
 * @param[in]   eParityType Type of parity to calculate:
 *                          - #ADI_EAL_ODD_PARITY: Returns 1 if odd number of set bits
 *                          - #ADI_EAL_EVEN_PARITY: Returns 1 if even number of set bits
 * @param[in]   nTxData     16-bit data value for which parity is calculated
 *
 * @return      uint8_t     Computed parity bit value (0 or 1)
 *
 * @details     This function calculates parity by:
 *              1. Performing XOR operations across all bits to determine bit population
 *              2. Extracting the final result as a single parity bit
 *              3. Inverting the result if odd parity is requested
 *
 *              The algorithm efficiently computes parity using bit manipulation
 *              rather than counting individual bits.
 *
 * @note        Used internally during MELIBU frame ID assembly for P0 and P1
 *              parity bit calculation. The parity bits ensure frame integrity
 *              during transmission over the MELIBU bus.
 *
 * @see         MelibuComputeParityBits() - Wrapper function for frame ID parity
 * @see         MelibuSlaveTransmitConfigData() - Uses this for frame assembly
 * @see         MelibuSlaveReceiveConfigData() - Uses this for frame assembly
 */
ADI_EAL_MELIBU_CODE
static uint8_t MelibuComputeParity(ADI_EAL_PARITY_TYPE eParityType, uint16_t nTxData)
{
    /* Local variable to compute parity */
    uint8_t nParity;

    /* Computation of the parity */
    nTxData ^= nTxData >> 8;
    nTxData ^= nTxData >> 4;
    nTxData ^= nTxData >> 2;
    nTxData ^= nTxData >> 1;
    nParity = (uint8_t)(nTxData & 1u);

    if(eParityType == ADI_EAL_ODD_PARITY)
    {
        nParity ^= 1u;  /* Toggle parity bit for odd parity */
    }

    return nParity;
}

/**
 * @brief   Verify CRC-16 checksum for MELIBU 2.0 frame data received
 *
 * @param[in]  pMelibuFrameID  Pointer to Melibu frame ID for CRC calculation.
 *                             Must be non-NULL if nDataLength > 0.
 * @param[in]  pPayload        Pointer to data buffer for CRC calculation.
 *                             Must be non-NULL if nDataLength > 0.
 * @param[in]  nDataLength     Length of data in bytes to process.
 *                             Valid range: 0 to ADI_EAL_MELIBU_FRAME_MAX_LENGTH - 2 bytes.
 * @param[in]  nCrcReceived    Received CRC-16 value to verify against calculated CRC.
 *
 * @return     bool        Calculated CRC-16 value using CCITT polynomial (0x1021)
 *                             - Returns true is received CRC is matching with calculated
 *                             - Returns false if received CRC is not matching with calculated
 *
 * @details    Computes CRC-16 checksum over the provided data buffer using the CCITT
 *             polynomial (0x1021). The CRC is initialized to 0xFFFF and finalized with
 *             bitwise NOT operation. Used to ensure MELIBU frame integrity during
 *             transmission over the MELIBU bus.
 *
 * @see        MelibuCrc16() - Internal CRC-16 helper function
 * @see        MelibuSlaveReceiveConfigData() - Frame assembly that uses this function
 */
ADI_EAL_MELIBU_CODE
static bool MelibuCrcVerification(uint8_t *pMelibuFrameID, uint16_t *pPayload, uint8_t nDataLength,
                                  uint16_t nCrcReceived)
{
    bool bCrcValid = false;

    /* Validate input pointers - cannot verify CRC with NULL pointers */
    if((pPayload != NULL) && (pMelibuFrameID != NULL))
    {
        uint16_t nCrc;
        uint8_t nIndex = 0U;
        uint8_t  nByte = 0U;
        nCrc = 0xFFFFu; //Initialization of crc to 0xFFFF for CCITT

        /* Calculate crc for Melibu Frame ID first */
        for(nIndex = 0U; nIndex < ADI_EAL_MELIBU_FRAME_ID_LENGTH; nIndex++)
        {
            nByte = pMelibuFrameID[nIndex];
            nCrc  = MelibuCrc16(nCrc, nByte);
        }

        /* Calculate crc for Melibu Payload next */
        if(nDataLength > 0U)
        {
            for(nIndex = 0U; nIndex < nDataLength; nIndex++)
            {
                nByte = (uint8_t)pPayload[nIndex];
                nCrc  = MelibuCrc16(nCrc, nByte);
            }
        }

        bCrcValid = (nCrc == nCrcReceived);
    }

    return bCrcValid;
}

/**
 * @brief   Calculates CRC-16 checksum for MELIBU 2.0 frame data
 *
 * @param[in]  pData           Pointer to data buffer for CRC calculation.
 *                             Must be non-NULL if nDataLength > 0.
 * @param[in]  nDataLength     Length of data in bytes to process.
 *                             Valid range: 0 to ADI_EAL_MELIBU_FRAME_MAX_LENGTH - 2 bytes.
 *
 * @return     uint16_t        Calculated CRC-16 value using CCITT polynomial (0x1021)
 *                             - Returns 0x0000 if pData is NULL or nDataLength is 0
 *                             - Returns computed 16-bit CRC value for valid input
 *
 * @details    Computes CRC-16 checksum over the provided data buffer using the CCITT
 *             polynomial (0x1021). The CRC is initialized to 0xFFFF and finalized with
 *             bitwise NOT operation. Used to ensure MELIBU frame integrity during
 *             transmission over the MELIBU bus.
 *
 * @note       - The CRC field itself should not be included in the calculation
 *             - Used internally by frame assembly functions during transmission
 *             - Can be used for received frame validation by comparing computed vs. transmitted CRC
 *
 * @see        MelibuCrc16() - Internal CRC-16 helper function
 * @see        MelibuSlaveReceiveConfigData() - Frame assembly that uses this function
 */
ADI_EAL_MELIBU_CODE
static uint16_t MelibuCrcCalculation(uint8_t *pData, uint8_t nDataLength)
{

    uint16_t nCrc = 0xFFFFu; //Initialization of crc to 0xFFFF for CCITT

    for(uint8_t nIndex = 0U; nIndex < nDataLength; nIndex++)
    {
        nCrc  = MelibuCrc16(nCrc, pData[nIndex]);
    }

    return nCrc; //The crc value for CCITT
}

/**
 * @brief       Computes CRC-16 polynomial iteration for MELIBU frame data
 *
 * @param [in]  nCrc        Current CRC-16 value from previous iteration
 * @param [in]  nNewByte    Next byte of data to process
 *
 * @return      uint16_t    Updated CRC-16 value after processing the byte
 *
 * @details     This helper function performs a single iteration of the CRC-16 calculation
 *              using the CCITT polynomial (0x1021). It processes one byte at a time and
 *              updates the running CRC value. This function is called iteratively by
 *              MelibuCrcCalculation() to compute CRC over entire frames.
 *
 * @note        - Should not be called directly; use MelibuCrcCalculation() instead
 *              - Used internally for MELIBU 2.0 frame integrity verification
 *
 * @see         MelibuCrcCalculation() - Wrapper function for complete CRC calculation
 */
ADI_EAL_MELIBU_CODE
static uint16_t MelibuCrc16(uint16_t nCrc, uint8_t nNewByte)
{
    /* Process each bit in the new byte (8 iterations) */
    for(uint8_t i = 0u; i < 8u; i++)
    {
        /* Check if MSB of CRC XOR'd with MSB of byte is 1 */
        /* This determines if polynomial division requires XOR with polynomial */
        if((((nCrc & 0x8000u) >> 8u) ^ (nNewByte & 0x80u)) != 0u)
        {
            /* Shift left and XOR with CRC-16 CCITT polynomial (0x1021) */
            nCrc = (nCrc << 1u)  ^ ADI_EAL_MELIBU_CRC_POLYNOM;
        }
        else
        {
            /* Just shift left without XOR */
            nCrc = (nCrc << 1u);
        }

        /* Shift byte left to process next bit */
        nNewByte <<= 1u;
    }

    /* Return updated CRC value */
    return nCrc;
}

/**
 * @brief Queues MELIBU reception buffers for all instances in a group
 *
 * @param [in] poEalMelibuData Pointer to MELIBU interface data containing instance array
 * @param [in] poFrameCfg      Pointer to MELIBU frame configuration with response parameters
 * @param [in] anMelibuFrameID Pointer to MELIBU frame ID associated with the transaction
 * @param [in] bMelibuResponseExpected Flag indicating if a response is expected for this transaction
 *
 * @details This function prepares the MELIBU reception queue for all instances by:
 *          1. Iterating through each instance in the group (unicast or multicast)
 *          2. Validating that RX payload pointers are not NULL
 *          3. Initializing reception buffer metadata (expected length and pointer)
 *          4. Incrementing the queue write index for each instance
 *
 *          Called after submitting a transaction to ensure responses are captured
 *          when received via the internal callback mechanism.
 *
 * @note    - For multicast: processes all instances in the group
 *          - For unicast: processes a single instance
 *          - Must be called after submitting the corresponding transaction
 *
 * @see AddCustomWriteTransactionToQueue() - Similar function for custom transactions
 * @see MelibuFioUartNotification() - Callback that uses queued reception buffers
 */
ADI_EAL_MELIBU_CODE
static void AddTransactionToQueue(EAL_MELIBU_DATA *poEalMelibuData, ADI_EAL_MELIBU_FRAME_CFG *poFrameCfg,
                                  uint8_t *anMelibuFrameID, bool bMelibuResponseExpected)
{

    /* Loop through all the instances and add the transaction to queue */
    for(uint16_t nInst = 0U; nInst < poEalMelibuData->nNumInst; nInst++)
    {

        /* Get the unicast instance data */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[nInst];

        /* Init reception buffer */
        ADI_EAL_MELIBU_REC_DATA *poRxMelibuBuffer = &poEalMelibuInstData->aoRxMelibuBuffer[poEalMelibuInstData->nMelibuQWrIdx];
        poRxMelibuBuffer->nExpectedRxLength = poFrameCfg->nResponseLength;
        poRxMelibuBuffer->bMelibuResponseExpected = bMelibuResponseExpected;

        for(uint8_t i = 0U; i < ADI_EAL_MELIBU_FRAME_ID_LENGTH; i++)
        {
            poRxMelibuBuffer->anMelibuFrameID[i] = anMelibuFrameID[i];
        }

        /* Increment queue index */
        poEalMelibuInstData->nMelibuQWrIdx = EAL_INC_Q_IDX(poEalMelibuInstData->nMelibuQWrIdx,
                                             poEalMelibuInstData->nMelibuCfgRxQDepth);
    }
}

/**
 * @brief Queues a reception buffer for a custom write transaction
 *
 * @param [in] poEalMelibuData      Pointer to MELIBU interface data
 * @param [in] nExpectedRxLength    Expected response length in bytes
 * @param [in] anMelibuFrameID      Pointer to MELIBU frame ID associated with the transaction
 *
 *
 * @details This function initializes and queues a reception buffer for the unicast
 *          MELIBU instance. It updates the reception buffer metadata with the expected
 *          response length and increments the queue write index.
 *
 *          Called after submitting a custom write transaction to prepare the instance
 *          to capture the response when received via the internal callback.
 *
 * @note    - Only works with unicast MELIBU handles
 *          - Must be called after submitting the corresponding custom transaction
 *          - Used internally by adi_eal_melibuCustomWriteRead()
 *          - Caller must validate handle before calling this function
 *
 * @see AddTransactionToQueue() - Similar function for standard transactions
 * @see adi_eal_melibuCustomWriteRead() - Calls this function internally
 */
ADI_EAL_MELIBU_CODE
static void AddCustomWriteTransactionToQueue(EAL_MELIBU_DATA *poEalMelibuData, uint8_t nExpectedRxLength,
        uint16_t *anMelibuFrameID)
{
    /* Get the unicast instance data */
    EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];

    /* Init reception buffer */
    ADI_EAL_MELIBU_REC_DATA *poRxMelibuBuffer = &poEalMelibuInstData->aoRxMelibuBuffer[poEalMelibuInstData->nMelibuQWrIdx];
    poRxMelibuBuffer->nExpectedRxLength = nExpectedRxLength;

    /* Increment queue index */
    poEalMelibuInstData->nMelibuQWrIdx = EAL_INC_Q_IDX(poEalMelibuInstData->nMelibuQWrIdx,
                                         poEalMelibuInstData->nMelibuCfgRxQDepth);
}

/**
 * @brief       Locks the mutex for all MELIBU instances
 *
 * @param [in]  poEalMelibuData  Pointer to MELIBU interface data structure
 *
 *
 * @details     Acquires the mutex lock for all instances in the MELIBU interface group.
 *              This ensures thread-safe access to shared queue structures during transaction
 *              submission and response handling. For unicast handles, locks a single instance;
 *              for multicast handles, locks all instances in the group.
 *
 * @note        - Must be paired with UnlockMelibuMutex() to avoid deadlock
 *              - Called internally before queue operations (add/remove transactions)
 *              - Do not call directly in application code
 *
 * @see         UnlockMelibuMutex() - Releases the acquired mutex
 * @see         adi_eal_melibuWriteRead() - Uses this function internally
 */
ADI_EAL_MELIBU_CODE
static void LockMelibuMutex(EAL_MELIBU_DATA *poEalMelibuData)
{
    /* Loop through all the unicast MELIBU instances and lock the mutex */
    for(uint16_t i = 0U; i < poEalMelibuData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_lockMutex(poEalMelibuData->apoInstData[i]->hMelibuMutex);
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
    }
}

/**
 * @brief       Unlocks the mutex for all MELIBU instances
 *
 * @param [in]  poEalMelibuData  Pointer to MELIBU interface data structure
 *
 *
 * @details     Releases the mutex lock for all instances in the MELIBU interface group.
 *              This function must be paired with LockMelibuMutex() to maintain proper
 *              synchronization and avoid deadlock conditions during queue operations.
 *
 * @note        - Must only be called after a successful LockMelibuMutex() call
 *              - Do not call directly in application code; used internally by EAL APIs
 *              - For multicast handles, unlocks all instances in the group
 *              - For unicast handles, unlocks the single instance
 *
 * @see         LockMelibuMutex() - Acquires the mutex lock
 * @see         adi_eal_melibuWriteRead() - Public API that uses this function internally
 */
ADI_EAL_MELIBU_CODE
static void UnlockMelibuMutex(EAL_MELIBU_DATA *poEalMelibuData)
{
    /* Loop through all the unicast MELIBU instances and unlock the mutex */
    for(uint16_t i = 0U; i < poEalMelibuData->nNumInst; i++)
    {
        PRJ_OSAL_STATUS eOsalStatus = prj_osal_unlockMutex(poEalMelibuData->apoInstData[i]->hMelibuMutex);
        ADI_DBG_ENSURE(eOsalStatus == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");
    }
}

/**
 * @brief   Assembles a MELIBU 2.0 slave transmit frame
 *
 * @param[in]  poFrameCfg   Pointer to frame configuration containing:
 *                          - nSlaveAdd: Slave address (0-255)
 *                          - eFuncSel: Function selection (LED or NO_LED)
 *                          - nInstWordSel: Instruction word selector (0-1)
 *                          - nPayloadLength: Payload length in bytes
 * @param[out] pBuffer      Output buffer for assembled frame (minimum 2 bytes)
 * @param[out] pnTxLength   Frame length in bytes (always 2 for transmit frames)
 *
 * @return ADI_EAL_STATUS
 *         - #ADI_EAL_STATUS_SUCCESS: Frame successfully assembled
 *         - #ADI_EAL_STATUS_START_FAILED: Invalid parameters
 *
 * @details This function assembles a MELIBU 2.0 slave transmit frame according to specification:
 *          1. Validates input parameters (all pointers non-NULL, valid function/instruction selectors)
 *          2. Encodes payload length to PL_Length field (3 bits) based on function selection
 *          3. Constructs Frame ID (2 bytes):
 *             - Byte 0: Slave address
 *             - Byte 1: [P1|P0|PL_Len|I|F|R/T] where R/T=1 for transmit
 *          4. Calculates P0 and P1 parity bits for frame integrity
 *          5. Sets output frame length to 2 bytes (transmit frames contain only ID)
 *
 * @note    - Transmit frames from slave typically contain only the frame ID without payload
 *          - The frame must be followed by CRC bytes for complete MELIBU messages
 *          - Parity bits ensure frame integrity during MELIBU bus transmission
 *
 * @see MelibuSlaveReceiveConfigData() - For slave receive frame assembly
 * @see MelibuPlLengthEncode() - Payload length encoding logic
 * @see MelibuComputeParityBits() - Parity bit calculation
 */
ADI_EAL_MELIBU_CODE
static ADI_EAL_STATUS MelibuSlaveTransmitConfigData(ADI_EAL_MELIBU_FRAME_CFG *poFrameCfg, uint8_t *pBuffer,
        uint8_t *pnTxLength)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        ADI_DBG_REQUIRE(poFrameCfg != NULL,
                        "Pointer to frame config NULL in MelibuSlaveTransmitConfigData function");
        ADI_DBG_REQUIRE(pBuffer != NULL,
                        "Pointer to buffer NULL in MelibuSlaveTransmitConfigData function");
        ADI_DBG_REQUIRE(pnTxLength != NULL,
                        "Pointer to length output NULL in MelibuSlaveTransmitConfigData function");

        uint8_t nPL_Length = 0U;
        eEalStatus = MelibuPlLengthEncode(poFrameCfg->eFuncSel, &poFrameCfg->nPayloadLength, &nPL_Length);

        /* Set ID */
        pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX] = poFrameCfg->nSlaveAdd;
        pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX + 1U] = (uint8_t)(0U |
            ((uint8_t)ADI_EAL_MELIBU_SLV_TRANS << ADI_EAL_MELIBU_FRAME_ID_FIELDS_RT_BITPOS)  |
            ((uint8_t)poFrameCfg->eFuncSel << ADI_EAL_MELIBU_FRAME_ID_FIELDS_F_BITPOS) |
            ((uint8_t)poFrameCfg->nInstWordSel << ADI_EAL_MELIBU_FRAME_ID_FIELDS_I_BITPOS) |
            (nPL_Length << ADI_EAL_MELIBU_FRAME_ID_FIELDS_PL_LENGTH_BITPOS));
        uint16_t nId = (((uint16_t)pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX + 1U]) << 8U) |
                       pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX];
        uint8_t nP0 = MelibuComputeParityBits(nId, ADI_EAL_MELIBU_PARITY_TYPE_P0);
        uint8_t nP1 = MelibuComputeParityBits(nId, ADI_EAL_MELIBU_PARITY_TYPE_P1);
        pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX + 1U] |= (uint8_t)((nP0 << ADI_EAL_MELIBU_FRAME_ID_FIELDS_P0_BITPOS) |
            (nP1 << ADI_EAL_MELIBU_FRAME_ID_FIELDS_P1_BITPOS));

        *pnTxLength = ADI_EAL_MELIBU_FRAME_ID_LENGTH;
    } while(false);

    return eEalStatus;
}

/**
 * @brief   Assembles a MELIBU 2.0 slave receive frame
 *
 * @param[in]  poFrameCfg   Pointer to frame configuration structure containing:
 *                          - nSlaveAdd: Slave address
 *                          - eFuncSel: Function selection (LED or NO_LED)
 *                          - nInstWordSel: Instruction word selector
 *                          - nInstWord: Instruction word value
 *                          - nPayloadLength: Payload data length
 *                          - anPayload: Payload data pointer
 *                          - nPci: PCI byte (for NO_LED function)
 * @param[out] pBuffer      Output buffer for assembled frame
 *                          (minimum ADI_EAL_MELIBU_FRAME_MAX_LENGTH bytes)
 * @param[out] pnTxLength   Total assembled frame length in bytes
 *
 * @return ADI_EAL_STATUS
 *         - #ADI_EAL_STATUS_SUCCESS: Frame successfully assembled
 *         - #ADI_EAL_STATUS_START_FAILED: Invalid parameters or encoding failed
 *
 * @details Constructs a MELIBU 2.0 slave receive frame by:
 *          1. Validating all input parameters
 *          2. Encoding payload length to PL_Length field (3 bits)
 *          3. Building Frame ID with slave address and control bits
 *          4. Computing P0 and P1 parity bits
 *          5. Inserting instruction word (if applicable)
 *          6. Inserting PCI byte (if NO_LED function)
 *          7. Appending payload data and CRC-16 checksum
 *
 * @note    - Frame structure: [ID (2B)] [InstWord (2B)] [PCI (1B)] [Payload] [CRC (2B)]
 *          - Instruction word only included if nInstWordSel == ADI_EAL_MELIBU_INST_WORD
 *          - PCI byte only included if eFuncSel == ADI_EAL_MELIBU_FUNC_SEL_NO_LED
 *          - CRC-16 calculated over ID + instruction + PCI + payload
 *          - Frame length varies based on payload size and function selection
 *
 * @see MelibuSlaveTransmitConfigData() - For slave transmit frame assembly
 * @see MelibuPlLengthEncode() - Payload length encoding
 * @see MelibuCrcCalculation() - CRC-16 calculation
 */
ADI_EAL_MELIBU_CODE
static ADI_EAL_STATUS MelibuSlaveReceiveConfigData(ADI_EAL_MELIBU_FRAME_CFG *poFrameCfg, uint8_t *pBuffer,
        uint8_t *pnTxLength)
{
    /* Return status of the API */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    do
    {
        /* === Input parameter validation === */

        /* Validate frame configuration pointer */
        ADI_DBG_REQUIRE(poFrameCfg != NULL,
                        "Pointer to frame config NULL in MelibuSlaveReceiveConfigData function");

        /* Validate output buffer pointer */
        ADI_DBG_REQUIRE(pBuffer != NULL,
                        "Pointer to buffer NULL in MelibuSlaveReceiveConfigData function");

        /* Validate transmit length output pointer */
        ADI_DBG_REQUIRE(pnTxLength != NULL,
                        "Pointer to length output NULL in MelibuSlaveReceiveConfigData function");

        /* Validate payload pointer if payload length is non-zero */
        ADI_EAL_MELIBU_DBG_REQUIRE(((poFrameCfg->nPayloadLength == 0U) || (poFrameCfg->anPayload != NULL)),
                                   "Pointer to payload in frame config NULL in MelibuSlaveReceiveConfigData function");

        /* Validate function selection is within valid range */
        ADI_DBG_REQUIRE(((uint8_t)poFrameCfg->eFuncSel < ADI_EAL_MELIBU_MAX_FUNC_SEL_FLAG_VALUE),
                        "Function selection parameter incorrect in MelibuSlaveReceiveConfigData function");

        /* Validate instruction word selector is within valid range */
        ADI_DBG_REQUIRE(((uint8_t)poFrameCfg->nInstWordSel < ADI_EAL_MELIBU_MAX_INST_WORD_SEL),
                        "Intruction word selection parameter incorrect in MelibuSlaveReceiveConfigData function");

        /* Local variables for frame assembly */
        uint8_t nPL_Length = 0u;  /* Encoded payload length */
        uint8_t index      = 0u;  /* Buffer index for payload copying */
        uint8_t nSidLength = 0u;  /* SID field length */

        /* === Encode payload length === */

        /* Convert actual payload length to MELIBU encoded PL_Length field */
        eEalStatus =  MelibuPlLengthEncode(poFrameCfg->eFuncSel, &poFrameCfg->nPayloadLength, &nPL_Length);

        /* Check if payload length encoding was successful */
        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* === Assemble MELIBU Frame ID (2 bytes) === */

        /* Byte 0: Slave address */
        pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX] = poFrameCfg->nSlaveAdd;

        /* Byte 1: Pack R/T, Function, Instruction Selector, and PL_Length fields */
        pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX + 1U] = (uint8_t)(0U |
            ((uint8_t)ADI_EAL_MELIBU_SLV_REC << ADI_EAL_MELIBU_FRAME_ID_FIELDS_RT_BITPOS)  |
            ((uint8_t)poFrameCfg->eFuncSel << ADI_EAL_MELIBU_FRAME_ID_FIELDS_F_BITPOS) |
            ((uint8_t)poFrameCfg->nInstWordSel << ADI_EAL_MELIBU_FRAME_ID_FIELDS_I_BITPOS) |
            (nPL_Length << ADI_EAL_MELIBU_FRAME_ID_FIELDS_PL_LENGTH_BITPOS));

        /* Reconstruct 16-bit ID from assembled bytes for parity calculation */
        uint16_t nId = (((uint16_t)pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX + 1U]) << 8U) |
                       pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX];

        /* Calculate P0 parity bit over designated ID bits */
        uint8_t nP0 = MelibuComputeParityBits(nId, ADI_EAL_MELIBU_PARITY_TYPE_P0);

        /* Calculate P1 parity bit over designated ID bits */
        uint8_t nP1 = MelibuComputeParityBits(nId, ADI_EAL_MELIBU_PARITY_TYPE_P1);

        /* Insert computed parity bits into Frame ID byte 1 */
        pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX + 1U] |= (uint8_t)((nP0 << ADI_EAL_MELIBU_FRAME_ID_FIELDS_P0_BITPOS) |
            (nP1 << ADI_EAL_MELIBU_FRAME_ID_FIELDS_P1_BITPOS));

        /* === Add instruction word if specified === */

        /* Check if instruction word is present in frame */
        if(poFrameCfg->nInstWordSel == ADI_EAL_MELIBU_INST_WORD)
        {
            pBuffer[ADI_EAL_MELIBU_FRAME_DATA_INDEX] = (uint8_t)poFrameCfg->nInstWord;
            pBuffer[ADI_EAL_MELIBU_FRAME_DATA_INDEX + 1U] = (uint8_t)(poFrameCfg->nInstWord >> 8U);
            index += 2u;
            nSidLength = 2u;
        }

        if(poFrameCfg->eFuncSel == ADI_EAL_MELIBU_FUNC_SEL_NO_LED)
        {
            pBuffer[ADI_EAL_MELIBU_FRAME_DATA_INDEX + index] = (uint8_t)poFrameCfg->nPci;
            index += 1u;
        }

        /* If any payload to send */
        if(poFrameCfg->nPayloadLength > 0U)
        {
            /* Set data and CRC */
            (void)ADI_MEMCPY(&pBuffer[ADI_EAL_MELIBU_FRAME_DATA_INDEX + index], poFrameCfg->anPayload, poFrameCfg->nPayloadLength);

        }

        uint16_t nCrc = MelibuCrcCalculation(&pBuffer[ADI_EAL_MELIBU_FRAME_ID_INDEX],
                                             (ADI_EAL_MELIBU_FRAME_ID_LENGTH + poFrameCfg->nPayloadLength + nSidLength));

        pBuffer[ADI_EAL_MELIBU_FRAME_DATA_INDEX + poFrameCfg->nPayloadLength + nSidLength] = (uint8_t)nCrc;
        pBuffer[ADI_EAL_MELIBU_FRAME_DATA_INDEX + poFrameCfg->nPayloadLength + nSidLength + 1U] = (uint8_t)(nCrc >> 8U);
        *pnTxLength = ADI_EAL_MELIBU_FRAME_ID_LENGTH + poFrameCfg->nPayloadLength + ADI_EAL_MELIBU_FRAME_CRC_LENGTH +
                      nSidLength;

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Sends multiple MELIBU frames over the internal FIO UART interface
 *
 * @param [in]  hMelibuHandle   The MELIBU interface handle (must be unicast)
 * @param [in]  aoFrameCfg      Array of MELIBU frame configurations to transmit
 * @param [in]  nNumFrames      Number of frames in the array to send
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS: All frames queued successfully
 *              - #ADI_EAL_STATUS_START_FAILED: Invalid handle or port not configured
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Insufficient queue space for responses
 *
 * @details     This function assembles and transmits multiple MELIBU frames by:
 *              1. Validating handle and queuing space for expected responses
 *              2. Assembling each frame according to its configuration (transmit or receive mode)
 *              3. Preparing FIO UART transactions with timing and echo cancellation settings
 *              4. Submitting all transactions to the internal FIO UART interface
 *              5. Queuing reception buffers for frames expecting responses
 *
 *              Frames are transmitted asynchronously; responses are delivered via the
 *              registered callback function.
 *
 * @note        - Only works with unicast MELIBU handles
 *              - Frame mode (nR_T) determines assembly: slave receive or slave transmit
 *              - Response queueing depends on expected response length and echo cancellation
 *              - This is an internal helper function; use public APIs like
 *                adi_eal_melibuFlexWriteRead() or adi_eal_melibuWriteRead() instead
 *
 * @see         adi_eal_melibuFlexWriteRead() - Public API that uses this function
 * @see         adi_eal_melibuWriteRead() - Standard transaction API
 *
 * @internal
 */
ADI_EAL_MELIBU_CODE
static ADI_EAL_STATUS MelibuSend(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
                                 ADI_EAL_MELIBU_FRAME_CFG *aoFrameCfg, uint16_t nNumFrames)
{

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Extract the Melibu data from the handle */
    EAL_MELIBU_DATA *poEalMelibuData = (EAL_MELIBU_DATA *)hMelibuHandle;

    do
    {
        /* Get the unicast instance data from the handle */
        EAL_MELIBU_INST_DATA *poEalMelibuInstData = poEalMelibuData->apoInstData[0U];

        uint8_t anTxBuffer[ADI_EAL_MELIBU_FRAME_MAX_LENGTH];
        uint8_t nTxLength = 0U;

        uint8_t nNumTxnsWithResp = 0U;
        uint8_t nRxLength = 0U;
        ADI_EAL_MELIBU_FRAME_CFG *poMelibuFrame;

        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *pnTxTransactionBuffer = poEalMelibuInstData->anTxTransactionBuffer;

        ADI_EAL_FIO_UART_HANDLE hFioUartHandle = poEalMelibuData->pInternalPortHandler;
        const bool bIsReadTimeTagEn = IsReadTimeTagEnabled(poEalMelibuData);

        /* Calculate number of expected responses */
        for(uint16_t nTrans = 0U; nTrans < nNumFrames; nTrans++)
        {
            poMelibuFrame = &aoFrameCfg[nTrans];
            nRxLength = poMelibuFrame->nResponseLength;
            /* Is response expected for this transaction */
            bool bIsResponseExpected = ((nRxLength > 0U) || (poMelibuFrame->bEchoCancelled == false)
                                        || bIsReadTimeTagEn);

            /* If reception is required from application */
            if(bIsResponseExpected)
            {
                nNumTxnsWithResp++;
            }
        }

        /* Check if all the queues have enough space for responses */
        for(uint16_t nInts = 0U; nInts < poEalMelibuData->nNumInst; nInts++)
        {
            EAL_MELIBU_INST_DATA *poEalMelibuInstDataSpaceCheck = poEalMelibuData->apoInstData[nInts];
            /* Space remaining in the transaction queue */
            uint16_t nSpaceLeftInQueue = EAL_SLOTS_FREE_IN_Q(
                                             poEalMelibuInstDataSpaceCheck->nMelibuQWrIdx,
                                             poEalMelibuInstDataSpaceCheck->nMelibuQRdIdx,
                                             poEalMelibuInstDataSpaceCheck->nMelibuCfgRxQDepth);

            /* Check if there is enough space in the transaction queue */
            if(nNumTxnsWithResp > nSpaceLeftInQueue)
            {
                eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
                break;
            }
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Mount buffer of data to send over 8 bits type buffer to calculate crc */
        for(uint16_t nTrans = 0U; nTrans < nNumFrames; nTrans++)
        {
            poMelibuFrame = &aoFrameCfg[nTrans];

            /* Time-tag responses are handled separately via bIsReadTimeTagEn;
            * bIsMelibuResponseExpected only tracks actual data payloads. */
            bool bIsMelibuResponseExpected = (poMelibuFrame->nResponseLength > 0U);
            (void)ADI_MEMSET(&anTxBuffer[0U], 0, ADI_EAL_MELIBU_FRAME_MAX_LENGTH);

            /* Mount frames considering Melibu frames structure */
            if(poMelibuFrame->nR_T == ADI_EAL_MELIBU_SLV_REC)
            {
                eEalStatus = MelibuSlaveReceiveConfigData(poMelibuFrame, &anTxBuffer[0U], &nTxLength);
            }
            else if(poMelibuFrame->nR_T == ADI_EAL_MELIBU_SLV_TRANS)
            {
                eEalStatus = MelibuSlaveTransmitConfigData(poMelibuFrame, &anTxBuffer[0U], &nTxLength);
            }
            else
            {
                /* Shouldn't come here! */
                eEalStatus = ADI_EAL_STATUS_START_FAILED;
                break;
            }

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }

            bool bLastFrame = false;
            ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFioUartTxnCfg = (ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *)pnTxTransactionBuffer;

            /* Mount FIO UART transaction */
            MelibuPrepareFioUartTransaction(poFioUartTxnCfg, poMelibuFrame, nTxLength, anTxBuffer,
                                            poEalMelibuInstData->anTxBuffer);

            /* If this is the last transaction to send indicate in the flag to execute transmission */
            if(nTrans >= (nNumFrames - 1U))
            {
                bLastFrame = true;
            }

            eEalStatus =  adi_eal_fioUartSetEchoCancel(hFioUartHandle, poMelibuFrame->bEchoCancelled);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }

            /* Add transaction to frame to send */
            eEalStatus = adi_eal_fioUartPackedDataTransmit(hFioUartHandle, poFioUartTxnCfg, poMelibuFrame->anRxPayload, 1U,
                         bLastFrame);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }

            /* If response is expected */
            if((poMelibuFrame->nResponseLength > 0U) || bIsReadTimeTagEn)
            {
                /* Add the transaction to queue for all instances */
                AddTransactionToQueue(poEalMelibuData, poMelibuFrame, anTxBuffer, bIsMelibuResponseExpected);

            }

        }

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Builds MELIBU frame for periodic operation
 *
 * @param [in]  poMelibuTxnCfg    Pointer to MELIBU transaction configuration
 * @param [out] anTxBuffer        Buffer to hold assembled frame
 * @param [out] pnTxLength        Pointer to store frame length
 *
 * @return      Status of frame assembly operation
 */
ADI_EAL_MELIBU_CODE
static ADI_EAL_STATUS MelibuPeriodicBuildFrame(
    ADI_EAL_MELIBU_FRAME_CFG *poMelibuTxnCfg,
    uint8_t *anTxBuffer,
    uint8_t *pnTxLength)
{
    ADI_EAL_STATUS eEalStatus;

    /* Initialize transmit buffer */
    (void)ADI_MEMSET(anTxBuffer, 0, ADI_EAL_MELIBU_FRAME_MAX_LENGTH);

    /* Build frame based on transaction type */
    if(poMelibuTxnCfg->nR_T == ADI_EAL_MELIBU_SLV_REC)
    {
        /* Slave receive: master writes to slave */
        eEalStatus = MelibuSlaveReceiveConfigData(poMelibuTxnCfg, anTxBuffer, pnTxLength);
    }
    else if(poMelibuTxnCfg->nR_T == ADI_EAL_MELIBU_SLV_TRANS)
    {
        /* Slave transmit: master reads from slave */
        eEalStatus = MelibuSlaveTransmitConfigData(poMelibuTxnCfg, anTxBuffer, pnTxLength);
    }
    else
    {
        /* Invalid transaction type */
        eEalStatus = ADI_EAL_STATUS_START_FAILED;
    }

    return eEalStatus;
}

/**
 * @brief       Queues transaction configuration to FIO UART instances
 *
 * @param [in]  poEalFioUartData  Pointer to FIO UART data structure
 * @param [in]  poFioUartTxnCfg   Pointer to FIO UART transaction configuration
 * @param [in]  poMelibuTxnCfg    Pointer to MELIBU transaction configuration
 *
 * @return      Status of queueing operation
 */
ADI_EAL_MELIBU_CODE
static ADI_EAL_STATUS MelibuPeriodicQueueToFioUart(
    EAL_FIO_UART_DATA *poEalFioUartData,
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFioUartTxnCfg,
    ADI_EAL_MELIBU_FRAME_CFG *poMelibuTxnCfg)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Queue to all FIO UART instances */
    for(uint16_t nIdx = 0U; nIdx < poEalFioUartData->nNumInst; nIdx++)
    {
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[nIdx];

        /* Check queue space availability */
        if(EAL_SLOTS_FREE_IN_Q(poEalFioUartInstData->nFioUartQWrIdx, poEalFioUartInstData->nFioUartQRdIdx,
                               poEalFioUartInstData->nFioUartCfgQDepth) == 0U)
        {
            eEalStatus = ADI_EAL_STATUS_QUEUE_FULL;
            break;
        }

        /* Copy transaction configuration to queue */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poInstTxnCfg =
            &poEalFioUartInstData->paoCfgs[poEalFioUartInstData->nFioUartQWrIdx];
        (void)ADI_MEMCPY(poInstTxnCfg, poFioUartTxnCfg, sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG));

        /* Configure breakfield metadata for RX parsing */
        ADI_EAL_FIO_UART_BREAKFIELD_INFO *poBreakFieldInfo =
            &poEalFioUartInstData->paoBreakfieldInfo[poEalFioUartInstData->nFioUartQWrIdx];
        poBreakFieldInfo->nBreakFieldBitsBeforePayload = poEalFioUartInstData->nBreakFieldBits;
        poBreakFieldInfo->bIsEchoEnabled = !poEalFioUartInstData->bIsCanEchoCancel;
        poBreakFieldInfo->nBreakFieldBitsAfterPayload = 0U;
        poBreakFieldInfo->nOffsetForAfterPayloadBreakFields = 0U;

        /* Set response payload pointer */
        poInstTxnCfg->pPayload = (poMelibuTxnCfg->anRxPayload != NULL) ?
                                 poMelibuTxnCfg->anRxPayload[0U] : NULL;

        /* Increment queue write index */
        poEalFioUartInstData->nFioUartQWrIdx = EAL_INC_Q_IDX(poEalFioUartInstData->nFioUartQWrIdx,
                                               poEalFioUartInstData->nFioUartCfgQDepth);
    }

    return eEalStatus;
}

/**
 * @brief       Submits transaction to E2B core and activates FIO UART periodic mode
 *
 * @param [in]  poEalFioUartData  Pointer to FIO UART data structure
 *
 * @return      Status of E2B core submission and mode activation
 */
ADI_EAL_MELIBU_CODE
static ADI_EAL_STATUS MelibuPeriodicSubmitToE2BCore(EAL_FIO_UART_DATA *poEalFioUartData)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Submit to E2B core and enable periodic mode for all instances */
    for(uint16_t nIdx = 0U; nIdx < poEalFioUartData->nNumInst; nIdx++)
    {
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[nIdx];

        /* Calculate previous write index (circular buffer) */
        uint16_t nPrevWrIdx = (poEalFioUartInstData->nFioUartQWrIdx == 0U)
                              ? (poEalFioUartInstData->nFioUartCfgQDepth - 1U)
                              : (poEalFioUartInstData->nFioUartQWrIdx - 1U);

        /* Get configuration that was just queued */
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poStoredCfg = &poEalFioUartInstData->paoCfgs[nPrevWrIdx];

        /* Submit to E2B core for SMC processing */
        if(adi_e2bcore_addFlexibleIOConfigToQueue(&poEalFioUartInstData->oId, poStoredCfg) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        /* Activate periodic mode on this instance */
        poEalFioUartInstData->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
    }

    return eEalStatus;
}

/**
 * @brief       Enables periodic mode on all MELIBU instances
 *
 * @param [in]  poEalMelibuData   Pointer to MELIBU data structure
 */
ADI_EAL_MELIBU_CODE
static void MelibuPeriodicEnableMode(EAL_MELIBU_DATA *poEalMelibuData)
{
    /* Set periodic mode for all MELIBU instances */
    for(uint16_t nLoopIdx = 0U; nLoopIdx < poEalMelibuData->nNumInst; nLoopIdx++)
    {
        poEalMelibuData->apoInstData[nLoopIdx]->eMode = ADI_EAL_USAGE_MODE_PERIODIC;
    }
}

/*!
 * @brief   Checks if Read Time Tag is enabled on the underlying FIO UART instance
 *
 * @param [in] poEalMelibuData  Pointer to MELIBU data structure (unicast or multicast)
 *
 * @return  true if read time tag is enabled on the underlying FIO UART instance, false otherwise.
 *          Always returns false for multicast handles.
 *
 * @details When Read Time Tag is enabled, every downstream transaction generates an upstream
 *          response containing an acquisition timestamp, regardless of whether a data response
 *          is otherwise expected. This affects response-expected logic in MelibuSend(),
 *          WriteReadSync (SetWaitingForRx guard), SetupPeriodicWriteRead, FlexWriteRead,
 *          and CustomWriteRead.
 *
 *          The flag is stored in the FIO UART instance data (bIsReadTimeTagEn) and is set
 *          during FIO UART configuration based on the FLEXIO_CONTROL register's
 *          ENABLE_READ_TIME_TAG bit.
 *
 * @note    - Multicast handles always return false because multicast is a write-only
 *            broadcast mechanism with no per-instance response queue management at
 *            the MELIBU layer. The underlying FIO UART layer handles per-instance
 *            bIsReadTimeTagEn checks in its own multicast code paths.
 *          - For unicast handles, only apoInstData[0U] is checked since unicast
 *            MELIBU instances always have exactly one underlying FIO UART instance.
 *
 * @see     adi_eal_fioUartConfigure() - Sets bIsReadTimeTagEn from register value
 * @see     MelibuSend() - Uses this to determine response queueing
 * @see     adi_eal_melibuWriteReadSync() - Uses this for SetWaitingForRx guard
 */
ADI_EAL_MELIBU_CODE
static bool IsReadTimeTagEnabled(const EAL_MELIBU_DATA *poEalMelibuData)
{
    /* Default: multicast handles are write-only; response tracking is not applicable */
    bool bIsEnabled = false;

    /* Only check the flag for unicast handles */
    if(poEalMelibuData->bIsMulticast == false)
    {
        /* Access the underlying FIO UART instance data through the internal port handler */
        EAL_FIO_UART_DATA *poEalFioUartData = (EAL_FIO_UART_DATA *)poEalMelibuData->pInternalPortHandler;

        /* Unicast MELIBU always has exactly one FIO UART instance at index 0 */
        EAL_FIO_UART_INST_DATA *poEalFioUartInstData = poEalFioUartData->apoInstData[0U];

        bIsEnabled = poEalFioUartInstData->bIsReadTimeTagEn;
    }
    else
    {
        /* do nothing for multicast handles */
    }

    return bIsEnabled;
}

#endif /* ADI_E2B_IFACE_MELIBU_ENABLED */
/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/
