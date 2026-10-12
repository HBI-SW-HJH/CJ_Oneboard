/*******************************************************************************
 Copyright (c) 2024 - Analog Devices Inc. All Rights Reserved.
 This software is proprietary & confidential to Analog Devices, Inc.
 and its licensors.
 ******************************************************************************
 * \file: ex_uartFlexio.h
 * \brief: The header file for the application
 * Developed by: Automotive Software and Systems team, Bangalore, India
 *****************************************************************************/
/** \addtogroup app
 * @{
 */
#ifndef EX_OSPFLEXIO_H
#define EX_OSPFLEXIO_H
/*================================= INCLUDES ================================*/
#include "adi_Eth10BaseT1s_cfg.h"
#include "adi_pal.h"                    /*!< Platform abstraction layer */
#include "adi_dbg.h"                    /*!< Debug interface */
#include "adi_network.h"                /*!< Network layer */
#include "adi_app_utils.h"              /*!< Include common utilities */
#include "adi_eal.h"                    /*!< E2B abstraction layer */
#include "adi_eal_wkslp.h"              /*!< Wake-Sleep */
#include "fioOsp.h"                     /*!< OSP driver Header */
#include "prj_osal.h"                   /*!< Osal layer */

/*================================= DEFINES =================================*/
#if defined(__EV_SC594_SOM__)
#define ADI_NETWORK_DEV_IDX     (1u)    /*!< The index of the SPI device used */
#define ADI_NETWORK_ETH_IDX     (2u)    /*!< The index of the ethernet device used */
#elif defined(RPI3_RASPBIAN)
#define ADI_NETWORK_DEV_IDX     (0u)    /*!< The index of the SPI device used */
#define ADI_NETWORK_ETH_IDX     (2u)    /*!< The index of the ethernet device used */
#elif defined(WIN_X64)
#define ADI_NETWORK_DEV_IDX     (0u)    /*!< The index of the SPI device used */
#define ADI_NETWORK_ETH_IDX     (4u)    /*!< The index of the ethernet device used */
#elif defined(__NUCLEO_STM32__)
#define ADI_NETWORK_DEV_IDX     (0u)    /*!< The index of the SPI device used */
#define ADI_NETWORK_ETH_IDX     (1u)    /*!< The index of the ethernet device used */
#elif defined(LINUX)
#define ADI_NETWORK_DEV_IDX     (0u)    /*!< The index of the SPI device used */
#define ADI_NETWORK_ETH_IDX     (2u)    /*!< The index of the ethernet device used */
#else
#define ADI_NETWORK_DEV_IDX     (0u)    /*!< The index of the SPI device used */
#define ADI_NETWORK_ETH_IDX     (4u)    /*!< The index of the ethernet device used */
#endif

/* ★ AD3304 최대 8개 지원 — discovery에서 실제 연결된 수만큼 MAC이 수신되므로
 *   ganRemoteMacAddrMap 슬롯을 8개로 확보한다.
 *   연결되지 않은 FID는 gbAd3304Connected[FID]=false 로 처리된다. */
#define NUM_REMOTE_NODES        (8u)
#define CTRL_PLCA_ID            (0u)
/* PLCA NodeCnt = 연결된 AD3304 수 + 1(Controller)
 * CreateE2bApp* 에서 실제 연결 수를 알기 전까지는 최대(9)로 설정하고,
 * discovery 완료 후 동적으로 갱신한다. */
#define MAX_PLCA_NODE_CNT       (9u)  /*!< Controller(1) + AD3304 최대 8개 */
/* ★ 큐 깊이 확장: 20 → 60
 *   LED 12개 × 2채널(FIO0+FIO1) unicast + off 노드 처리 = Tick당 최대 ~28 enqueue.
 *   기존 20 슬롯으로는 연속 TX 시 큐 오버플로우(error 12) 발생.
 *   nFioOspCfgQDepth 와 반드시 같은 숫자를 유지할 것. */
#define FIO_OSP_QUEUE_DEPTH     (60U)
#define FIO_OSP_MEM             (ADI_EAL_FIO_OSP_UNICAST_INST_MEM_SIZE(FIO_OSP_QUEUE_DEPTH))
#define FIO_OSP_MEM_WORDS       ((FIO_OSP_MEM + 3U) / 4U)
#define NUM_LEDS                (1u)
#define BROADCAST_DEV_ADDR      (0u)
#define INIT_DEV_ADDR           (1u)

#define LED_INTENSITY       (0x010u)

/* App states */
typedef enum APP_STATE
{
    APP_UINIT,            /*!< Application uninitialized */
    SEND_RESET_CMD,       /*!< Send resetcd.. command */
    SEND_INIT_BIDIR_CMD,  /*!< Send bidirectional init command */
    SEND_SET_STATE_CMD,   /*!< Send set state command */
    SEND_SET_SETUP_CMD,   /*!< Send set setup command */
    SEND_READ_SETUP_CMD,  /*!< Send read setup command */
    SEND_SET_PWM_CMD,     /*!< Send set PWM command */
    SEND_READ_PWM_CMD,    /*!< Send read PWM command */
    SEND_READ_TEMP_CMD,   /*!< Send read temperature command */
    SEND_READ_STATUS_CMD,  /*!< Send read setup command */
    SLEEP_REMOTE_NODE,    /*!< Put remote node to sleep */
    SLEEP_DEVICE,         /*!< Put device to sleep */
    WAKE_DEVICE,          /*!< Wake device */
    WAIT_FOR_SLEEP,       /*!< Wait for sleep confirmation */
    WAKE_REMOTE_NODE,     /*!< Wake remote node */
    READ_FIFO_LVL,        /*!< Read FIFO level */
} APP_STATE;
/*================================ DATA TYPES ===============================*/
/** Application data */
typedef struct APP_DATA
{
    uint32_t eAppMode;               /*!< Application mode */
    uint32_t eAppCmd;                /*!< Application command */
    uint32_t nMacCnt;                /*!< Number of MAC addresses received as part of discovery */
} APP_DATA;

/*================================ COLOR DEFINITIONS ===============================*/

    // color for one LED consists of 3 values (red,green and blue)
    // the following definitions result in the colors:
    // color[0] = RED
    // color[1] = GREEN
    // color[2] = BLUE
    // color[3] = PURPLE
    // color[4] = CYAN
    // color[5] = ORANGE
    // color[6] = WHITE
    
    /* Current Consumption on 5V VDD of up to 550mA when using 0x0555 for "active" colours */
    /* Current Consumption on 5V VDD of up to 340mA when using 0x0333 for "active" colours */
    
    static uint16_t colorRed[COLOR_COUNT_MAX] = { 0x1, 0x0000, 0x0000, 0x1, 0x0000, 0x1, 0x1 };
    static uint16_t colorGreen[COLOR_COUNT_MAX] = { 0x0000, 0x1, 0x0000, 0x0000, 0x1, 0x1, 0x1 };
    static uint16_t colorBlue[COLOR_COUNT_MAX] = { 0x0000, 0x0000, 0x1, 0x1, 0x1, 0x0000, 0x1 };

    static uint8_t colorNbr = 0;

#endif /* EX_OSPFLEXIO_H */
/** @} */

/*
 * EOF: https://www.analog.com/
 */
