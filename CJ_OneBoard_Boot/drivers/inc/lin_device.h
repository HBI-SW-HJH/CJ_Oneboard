// @file    lin_device.h
// @brief   LIN slave driver for PIC32CM5164JH01032-E/PT
//          SERCOM0 USART, 19200 bps
//
//  Pin assignments (schematic CJ_4DPLEX_SEAT_LOGO_STROBE_LAMP_CONTROLLER_A00):
//    PA04 -> SERCOM0/PAD[0] PMUX_D -> LIN_TX
//    PA07 -> SERCOM0/PAD[3] PMUX_D -> LIN_RX
//    PA06 -> GPIO output           -> LIN_NSLP
//
//  Interrupt: SERCOM0_IRQn, handler SERCOM0_6_Handler().
//
// @copyright 2026 CJ 4DPLEX

#ifndef LIN_DEVICE_H
#define LIN_DEVICE_H

#include "device.h"

//  ---- Frame parameters ----
#define LIN_BUFF_SIZE       8U          //  data bytes per LIN frame
#define LIN_BAUD_RATE       19200UL
#define LIN_TX_PIN          4U
#define LIN_RX_PIN          7U
#define LIN_EN_PIN          6U
#define LIN_PIN_GROUP       GPIO_GROUP_A
#define LIN_PIN_PMUX        0x3U
#define LIN_GCLK_ID_CORE    SERCOM0_GCLK_ID_CORE
#define LIN_BAUD_DIVIDER    ((SYSTEM_CLOCK_HZ + LIN_BAUD_RATE) / (2UL * LIN_BAUD_RATE))
#define LIN_BAUD_REG        ((uint16_t)((LIN_BAUD_DIVIDER / 8UL) | ((LIN_BAUD_DIVIDER % 8UL) << 13U)))
#define LIN_VECTOR_COUNT    (16U + (uint32_t)PERIPH_MAX_IRQn + 1U)

//  ---- Checksum type ----
typedef enum {
    LIN_CHECKSUM_CLASSIC  = 0U,
    LIN_CHECKSUM_ENHANCED = 1U,
} LIN_ChecksumType_t;

//  ---- Message direction ----
typedef enum {
    LIN_MSG_TYPE_RX = 0U,
    LIN_MSG_TYPE_TX = 1U,
} LIN_MsgType_t;

//  ---- Frame descriptor ----
typedef struct {
    uint8_t             frame_id;
    LIN_MsgType_t       msg_type;
    LIN_ChecksumType_t  checksum;
    uint8_t             length;
    uint8_t             data[LIN_BUFF_SIZE];
    uint8_t             frameIsValid;
} LIN_Device_Frame_t;

//  ---- Callback types ----
typedef void (*LIN_RxCallback_t)(const LIN_Device_Frame_t *frame);
typedef void (*LIN_TxCallback_t)(LIN_Device_Frame_t *frame);

typedef enum
{
    LIN_STATE_IDLE = 0U,
    LIN_STATE_PID = 1U,
    LIN_STATE_DATA_RX = 2U,
    LIN_STATE_CHECKSUM = 3U,
    LIN_STATE_DATA_TX = 4U
} LIN_State_t;

// LIN driver diagnostics. Kept here so the driver owns its debug state.
extern volatile uint32_t g_dbg_lin_irq_count;
extern volatile uint32_t g_dbg_lin_rxbrk_count;
extern volatile uint32_t g_dbg_lin_rxc_count;
extern volatile uint32_t g_dbg_lin_rxs_count;
extern volatile uint32_t g_dbg_lin_error_count;
extern volatile uint32_t g_dbg_lin_rx_cb_count;
extern volatile uint32_t g_dbg_lin_drop_count;
extern volatile uint8_t g_dbg_lin_last_pid;
extern volatile uint8_t g_dbg_lin_last_raw_id;
extern volatile uint8_t g_dbg_lin_last_state;
extern volatile uint8_t g_dbg_lin_last_intflag;
extern volatile uint16_t g_dbg_lin_last_status;
extern volatile uint8_t g_dbg_lin_error_state;
extern LIN_State_t s_state;
extern LIN_Device_Frame_t *s_frameTable;
extern uint8_t s_frameCount;
extern LIN_Device_Frame_t *s_active;
extern uint8_t s_dataIdx;
extern uint8_t s_pid;
extern LIN_Device_Frame_t s_anyFrame;
extern LIN_RxCallback_t s_rxCb;
extern LIN_TxCallback_t s_txCb;
extern uint32_t s_linVectorTable[LIN_VECTOR_COUNT];
extern uint8_t s_linVectorInstalled;

void SERCOM0_6_Handler(void);

// @brief  Initialise LIN slave (SERCOM0 USART + LIN_NSLP GPIO).
void LIN_Init(LIN_RxCallback_t rxCb, LIN_TxCallback_t txCb);

//  Register frame descriptor table
void LIN_RegisterFrames(LIN_Device_Frame_t *frames, uint8_t frameCount);

// @brief  SERCOM0 combined ISR handler.
void LIN_IrqHandler(void);

//  ---- Utilities ----
uint8_t LIN_CalcPID(uint8_t frameId);
uint8_t LIN_CalcChecksum(const uint8_t *data, uint8_t len, uint8_t pid, LIN_ChecksumType_t type);

#endif //  LIN_DEVICE_H
