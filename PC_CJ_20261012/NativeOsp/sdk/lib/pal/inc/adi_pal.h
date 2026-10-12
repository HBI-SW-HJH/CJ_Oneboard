/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_pal.h
 * \brief: Header file for the PAL (Platform Abstraction Layer) component
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup pal
 *  @{
 */

#ifndef ADI_PAL_H
#define ADI_PAL_H

/*================================= INCLUDES ================================*/
#include "adi_common.h"
#include "prj_osal.h"


/*================================= DEFINES =================================*/
/* ~~~~~~~~~~ Device options ~~~~~~~~~~ */
#define MACPHY_EN       /*!< Enables MAC-PHY device functionality */
#define ETH_EN          /*!< Enables Ethernet device functionality */

/* ~~~~~~~~~~ Host options ~~~~~~~~~~ */
#define HOST_SOM_SC594  (0u)    /*!< SC594 as host */
#define HOST_RPI        (1u)    /*!< RPi as host */
#define HOST_WIN_X64    (2u)    /*!< Windows as host */
#define HOST_NUCLEO_STM (3u)    /*!< STM32 as host */
#define HOST_LINUX      (4u)    /*!< Linux as host */
#define HOST_NNG        (5u)    /*!< NNG server as host */

/* ~~~~~~~~~~ Base-board options ~~~~~~~~~~ */
#define CUSTOMER_DEMO_BRD       (0U) /*!< Customer demo board */
#define FUNCTIONAL_EVAL_BRD     (1U) /*!< Functional eval board */

/* ~~~~~~~~~~ Host specific macros ~~~~~~~~~~ */
#if defined(__EV_SC594_SOM__)   /* Compiler flag */

#define SW_HOST                     (HOST_SOM_SC594)    /*!< Software Host */
#define HOST_NAME_STR               ("HOST_SOM_SC594")  /*!< Software Host name */
#ifdef EMC_TEST
#define ADI_PAL_MAX_NUM_SPI_DEV     (3U)
#else
#define ADI_PAL_MAX_NUM_SPI_DEV     (2U)
#endif
#define ADI_PAL_MAX_NUM_ETH_DEV     (1U)                /*!< Maximum number of ETH devices supported */
#define ADI_PAL_MAX_TMR             (5U)                /*!< Maximum number of timers supported */
#define ADI_PAL_MAX_GPIO            (16u)               /*!< Maximum number of GPIOs supported */
#define SOM_SC594_LED2              (0x04u)             /*!< SC594 LED2 */
#define SOM_SC594_LED5              (0x02u)             /*!< SC594 LED5 */
#define SOM_SC594_LED4              (0x01u)             /*!< SC594 LED4 */
#define SOM_SC594_LEDS_ALL          (SOM_SC594_LED2 | SOM_SC594_LED5 | SOM_SC594_LED4)  /*!< All SC594 LEDs */

#elif defined(RPI3_RASPBIAN)    /* Compiler flag */

#define SW_HOST                     (HOST_RPI)          /*!< Software Host */
#define HOST_NAME_STR               ("HOST_RPI")        /*!< Software Host name */
#define ADI_PAL_MAX_NUM_SPI_DEV     (1U)                /*!< Maximum number of SPI devices supported */
#define ADI_PAL_MAX_NUM_ETH_DEV     (5U)                /*!< Maximum number of ETH devices supported */
#define ADI_PAL_MAX_TMR             (5U)                /*!< Maximum number of timers supported */
#define ADI_PAL_MAX_GPIO            (16u)               /*!< Maximum number of GPIOs supported */

#elif defined(WIN_X64)          /* Compiler flag */

#define SW_HOST                     (HOST_WIN_X64)      /*!< Software Host */
#define HOST_NAME_STR               ("HOST_WIN_X64")    /*!< Software Host name */
#define ADI_PAL_MAX_NUM_SPI_DEV     (4U)                /*!< Maximum number of SPI devices supported */
#define ADI_PAL_MAX_NUM_ETH_DEV     (20U)               /*!< Maximum number of ETH devices supported */
#define ADI_PAL_MAX_TMR             (5U)                /*!< Maximum number of timers supported */
#define ADI_PAL_MAX_GPIO            (16u)               /*!< Maximum number of GPIOs supported */

#elif defined(__NUCLEO_STM32__) /* Compiler flag */

#define SW_HOST                     (HOST_NUCLEO_STM)   /*!< Software Host */
#define HOST_NAME_STR               ("HOST_NUCLEO_STM") /*!< Software Host name */
#define ADI_PAL_MAX_NUM_SPI_DEV     (1U)                /*!< Maximum number of SPI devices supported */
#define ADI_PAL_MAX_NUM_ETH_DEV     (1U)                /*!< Maximum number of ETH devices supported */
#define ADI_PAL_MAX_TMR             (5U)                /*!< Maximum number of timers supported */
#define ADI_PAL_MAX_GPIO            (16u)               /*!< Maximum number of GPIOs supported */

#elif defined(LINUX)            /* Compiler flag */

#define SW_HOST                     (HOST_LINUX)        /*!< Software Host */
#define HOST_NAME_STR               ("HOST_LINUX")      /*!< Software Host name */
#define ADI_PAL_MAX_NUM_SPI_DEV     (4U)                /*!< Maximum number of SPI devices supported */
#define ADI_PAL_MAX_NUM_ETH_DEV     (20U)               /*!< Maximum number of ETH devices supported */
#define ADI_PAL_MAX_TMR             (5U)                /*!< Maximum number of timers supported */
#define ADI_PAL_MAX_GPIO            (16u)               /*!< Maximum number of GPIOs supported */

#elif defined(NNG)              /* Compiler flag */

#define SW_HOST                     (HOST_NNG)          /*!< Software Host */
#define HOST_NAME_STR               ("HOST_NNG")        /*!< Software Host name */
#define ADI_PAL_MAX_NUM_SPI_DEV     (2U)                /*!< Maximum number of SPI devices supported */
#define ADI_PAL_MAX_NUM_ETH_DEV     (1U)                /*!< Maximum number of ETH devices supported */
#define ADI_PAL_MAX_TMR             (5U)                /*!< Maximum number of timers supported */
#define ADI_PAL_MAX_GPIO            (16u)               /*!< Maximum number of GPIOs supported */

#else
#error "No host selected"
#endif

/* ~~~~~~~~~~ Base-board specific macros ~~~~~~~~~~ */
#if defined(CUST_BOARD)         /* Compiler flag */

#define BASE_BOARD      (CUSTOMER_DEMO_BOARD)           /*!< Base board */
#define BASE_BOARD_STR  ("CUSTOMER_DEMO_BOARD")         /*!< Base board name */
/* TODO: Add LEDs and PBs of customer demo board */

#elif defined(FXN_BOARD)        /* Compiler flag */

#define BASE_BOARD      (FUNCTIONAL_EVAL_BRD)           /*!< Base board */
#define BASE_BOARD_STR  ("FUNCTIONAL_EVAL_BRD")         /*!< Base board name */
#define KFEB_LED0       (6u)                            /*!< Base board LED0 */
#define KFEB_LED1       (7u)                            /*!< Base board LED1 */
#define KFEB_LED2       (8u)                            /*!< Base board LED2 */
#define KFEB_LED3       (9u)                            /*!< Base board LED3 */
#define KFEB_PB0        (0u)                            /*!< Base board PB0 */
#define KFEB_PB1        (1u)                            /*!< Base board PB1 */

#else
/* Define as Customer board by deault */
#define BASE_BOARD      (CUSTOMER_DEMO_BOARD)           /*!< Base board */
#define BASE_BOARD_STR  ("CUSTOMER_DEMO_BOARD")         /*!< Base board name */
#endif

/*  ~~~~~~~~~~ OS specific macros ~~~~~~~~~~ */
#if defined(FreeRTOS)
#define OS_TYPE ("FreeRTOS")
#elif defined(PThread)
#define OS_TYPE ("PThread")
#elif defined(NoOS)
#define OS_TYPE ("NoOS")
#endif

/*! Max ethernet frame size */
#define ADI_PAL_MAX_ETH_FRM         (1600U)

/*! Max ethernet queue size */
#define ADI_PAL_MAX_ETH_Q           (10U)

/*! Max size of device name and description */
#define ADI_PAL_DEV_NAME_MAX_SIZE   (100U)

/*================================ DATA TYPES ===============================*/
/*! @brief SPI device details */
typedef struct ADI_PAL_SPIDEVINFO
{
    uint64_t nLocId;        /*!< Unified device location ID */
    uint64_t nSpiLocId;     /*!< SPI device location ID */
    uint64_t nGpioLocId;    /*!< GPIO device location ID */
    char sDescription[ADI_PAL_DEV_NAME_MAX_SIZE]; /*!< Description of device */
} ADI_PAL_SPIDEVINFO;

/*! @brief Ethernet adapter details */
typedef struct ADI_PAL_ETHADAPTERSINFO
{
    uint32_t nId;           /*!< Unified device location ID */
    char sName[ADI_PAL_DEV_NAME_MAX_SIZE]; /*!< Name of device */
    char sDesc[ADI_PAL_DEV_NAME_MAX_SIZE]; /*!< Description of device */
} ADI_PAL_ETHADAPTERSINFO;

/*! @brief Return status of the API calls  */
typedef enum ADI_PAL_STATUS
{
    ADI_PAL_OK,                     /*!< API call OK */
    ADI_PAL_ERR_LIB_NOT_LOADED,     /*!< Library not loaded */
    ADI_PAL_ERR_DEV_OPEN_FAILED,    /*!< Failed to open the device */
    ADI_PAL_ERR_DEV_SET_CLK_FAILED, /*!< Failed to set clock of the device */
    ADI_PAL_ERR_DEV_INIT_FAILED,    /*!< Failed to initialize device */
    ADI_PAL_ERR_DEV_WRITE_FAILED,   /*!< Failed to write to the device */
    ADI_PAL_ERR_DEV_UNINIT_FAILED,  /*!< Failed to un-initialize device */
    ADI_PAL_ERR_DEV_CLOSE_FAILED    /*!< Failed to close the device */
} ADI_PAL_STATUS;

/*! @brief Ethernet callback event types */
typedef enum ADI_ETH_CBK_EVT
{
    ADI_ETH_CBK_RX_RCVD,            /*!< RX frame received event */
    ADI_ETH_CBK_TS_CAPTD,           /*!< Timestamp captured event */
} ADI_ETH_CBK_EVT;

/*! @brief PAL callback to network layer event types */
typedef enum ADI_PAL_EVT
{
    ADI_PAL_ETH_CBK,                /*!< Ethernet callback event */
    ADI_PAL_ETH_TS_CBK,             /*!< Ethernet timestamp captured event */
    ADI_PAL_SPI_CBK,                /*!< SPI callback event */
    ADI_PAL_TMR_CBK,                /*!< Timer callback event */
    ADI_PAL_EVT_MAC_STATS,          /*!< MAC statistics event */
    ADI_PAL_EVT_PHY_STATS,          /*!< PHY statistics event */
    ADI_PAL_EVT_IRQ_CBK,            /*!< IRQ callback event */
    ADI_PAL_EVT_ASYNC_CBK,          /*!< Async operation complete event */
} ADI_PAL_EVT;

/*! @brief PAL callback to application event types */
typedef enum ADI_PAL_APP_EVT
{
    ADI_PAL_EVT_FATAL_ERR,          /*!< Fatal error event */
    ADI_PAL_EVT_CLI_TX,             /*!< CLI transmit event */
    ADI_PAL_EVT_CLI_RX,             /*!< CLI receive event */
#ifdef EMC_TEST
    ADI_PAL_EVT_ATS,                /*!< ATS Event for EMC */
#endif
} ADI_PAL_APP_EVT;

/*! @brief Ethernet event data */
typedef struct ADI_PAL_EVT_ETHDATA
{
    uint8_t *anData;                /*!< Pointer to frame data */
    uint32_t nSize;                 /*!< Size of frame */
} ADI_PAL_EVT_ETHDATA;

/*! @brief Ethernet timestamp data */
typedef struct ADI_PAL_EVT_ETH_TSDATA
{
    uint32_t nSec;                  /*!< Seconds part of timestamp */
    uint32_t nNanoSec;              /*!< Nanoseconds part of timestamp */
} ADI_PAL_EVT_ETH_TSDATA;

/*! @brief CLI transfer event data */
typedef struct ADI_PAL_EVT_CLI_TXDATA
{
    uint8_t *pTxData;               /*!< Pointer to UART data */
    uint32_t nLength;               /*!< Length of UART data */
} ADI_PAL_EVT_CLI_TXDATA;

/*============================ CALLBACK PROTOTYPES ============================*/
/**
 * @brief       PAL callback for network device updates (Network layer / application needs to implement this)
 *
 * @param [in]  nId     ID of the network device
 * @param [in]  eEvent  Callback event
 * @param [in]  pData   Pointer to the event data
 */
extern void adi_pal_networkCbk(uint32_t nId, ADI_PAL_EVT eEvent, const void *pData);

/**
 * @brief       PAL callback to application (Application needs to implement this)
 *
 * @param [in]  nId     Reserved (Always set to 0)
 * @param [in]  eEvent  Callback event
 * @param [in]  pData   Pointer to the event data
 *
 * @note MISRA C-2012 Rule 8.6 Deviation: This function is intentionally declared
 *       but not defined in the PAL library. Implementation must be provided by
 *       the application layer.
 */
extern void adi_pal_appCbk(uint32_t nId, ADI_PAL_APP_EVT eEvent, const void *pData);

/*================================ Log macro ================================*/
/* Prompt (Inserts >>>> into the UART log) */
#define ADI_CONSOLE_PROMPT          (true)  /*!< Inserts prompt (>>>>) at the start of UART log */
#define ADI_CONSOLE_NO_PROMPT       (false) /*!< No prompt at the start of UART log */

/** Size of individual UART log message */
#define UART_TX_LOG_MSG_SZ       (1000u)

/* MISRA C-2012 Rule 8.6 Deviation: The following declarations are platform-provided
 * implementations defined in platform-specific source files outside the PAL library */

/*! UART log data */
extern uint8_t ganUartLogData[UART_TX_LOG_MSG_SZ];

extern uint8_t ganPalMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /*!< Buffer for PAL log mutex */
extern PRJ_OSAL_MUTEX_CFG goPalMutexCfg;          /*!< PAL log mutex configuration */
extern PRJ_OSAL_MUTEX_HANDLE ghPalMutex;          /*!< Handle for the PAL log mutex */

extern uint8_t ganTimeMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /*!< Buffer for adi_pal_getCurrTime mutex */
extern PRJ_OSAL_MUTEX_CFG goTimeMutexCfg;          /*!< adi_pal_getCurrTime mutex configuration */
extern PRJ_OSAL_MUTEX_HANDLE ghTimeMutex;          /*!< Handle for the adi_pal_getCurrTime mutex */

extern bool gbSignalAsyncOperation;         /*!< Indicates whether network layer needs to be signalled asynchronously
                                                 on any events (when configured in async mode) */

/*! PAL logger macro */
#ifdef __TASKING__
#define ADI_PAL_LOG(IsPrompt, ...)  adi_pal_log(IsPrompt, __VA_ARGS__)
#else
#define ADI_PAL_LOG(IsPrompt, msg, ...)  adi_pal_log(IsPrompt, msg, ##__VA_ARGS__)
#endif

/*============================ PUBLIC PROTOTYPES ============================*/
/* MISRA C-2012 Rule 8.6 Deviation: The following function declarations are
 * platform-provided implementations defined in platform-specific source files */

/* PAL Constructor API */
extern void adi_pal_init(void);

/* PAL Destructor API */
extern void adi_pal_close(void);

/* PAL timing APIs */
extern void adi_pal_waitMicroSec(uint64_t nMSec);
extern void adi_pal_getCurrTime(uint64_t *pnCurrTimeNs);

/* PAL LED APIs */
extern void adi_pal_baseBoardLedSet(uint8_t nLedIdx, bool bState);
extern void adi_pal_platformLedSet(uint8_t nLed, bool bState);

/* PAL SPI APIs - Platform-provided implementations */
#ifdef MACPHY_EN
extern void adi_pal_getSpiDevs(uint32_t *nNumDevices, ADI_PAL_SPIDEVINFO *aoDevInfo, uint32_t nMaxNumSpiDevs);
extern ADI_PAL_STATUS adi_pal_spiOpen(uint32_t nId, uint32_t nSpiClk, uint8_t nSpiClkPol, uint8_t nSpiClkPha);
extern ADI_PAL_STATUS adi_pal_spiSend(uint32_t nId, uint8_t *pWriteData, uint8_t *pReadData, uint32_t nSize);
extern ADI_PAL_STATUS adi_pal_spiClose(uint32_t nId);
extern void adi_pal_spiRun(uint32_t nId);
#endif

/* PAL Ethernet APIs - Platform-provided implementations */
#ifdef ETH_EN
extern void adi_pal_getEthAdapters(uint32_t *pnNumDevices, ADI_PAL_ETHADAPTERSINFO *aoDevInfo, uint32_t nMaxNumEthDevs);
extern ADI_PAL_STATUS adi_pal_openEthAdapter(uint32_t nId);
extern ADI_PAL_STATUS adi_pal_ethSend(uint32_t nId, const uint8_t *pData, uint32_t nSize, bool bCaptureTs);
extern ADI_PAL_STATUS adi_pal_closeEthAdapter(uint32_t nId);
extern void adi_pal_ethRun(uint32_t nId);
extern bool adi_pal_isEthTxEmpty(uint32_t nId);
extern void adi_pal_getEthMacStats(uint32_t nId);
extern void adi_pal_getEthPhyStats(uint32_t nId);
extern void adi_pal_ethMacReset(uint32_t nId);
extern void adi_pal_ethSetPlca(uint32_t nId, bool bEnabled, uint32_t nNodeCount, uint32_t nNodeId);
extern void adi_pal_ethSetFilterTable(uint32_t nId, uint8_t panMacAddr[][6u], uint8_t nNumAddr);
extern void adi_pal_ethClearFilterTable(uint32_t nId);
#endif

/* PAL Wake-sleep API - Platform-provided implementation */
extern ADI_PAL_STATUS adi_pal_triggerWakePin(uint32_t nId);
/* PAL TS-CAPT API - Platform-provided implementation */
extern ADI_PAL_STATUS adi_pal_triggerTsCaptPin(uint32_t nId);
/* PAL Error APIs - Platform-provided implementations */
extern void adi_pal_fatalError(void);
extern void adi_pal_setFatalError(void);

/* PAL CLI APIs - Platform-provided implementations */
extern void adi_pal_log(bool bIsPrompt, const char *msg, ...);
extern void adi_pal_cliRun(void);
extern void adi_pal_sendToCLI(uint8_t *panTxData, uint32_t nSize);
#ifdef EMC_TEST
extern void adi_pal_MCU_ATS_Strobe(bool bVal);
#endif

#if 0
extern void adi_pal_waitForKeyPress(void);
#endif

#if 0
extern bool adi_pal_isSpiIrqAsserted(uint32_t nId);
#endif

#endif /* ADI_PAL_H */

/** @} */

/**
 * EOF: www.analog.com
 */
