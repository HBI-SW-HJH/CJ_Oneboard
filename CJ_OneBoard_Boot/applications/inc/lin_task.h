// @file    lin_task.h
// @brief   CJ LIN control/status protocol

#ifndef LIN_TASK_H
#define LIN_TASK_H

#include "lin_device.h"
#include "UDS.h"

#define LIN_RING_BUF_SIZE         32U
#define FRAME_TABLE_SIZE          2U
#define FID_DIAG_RX_IDX           0U
#define FID_DIAG_TX_IDX           1U

typedef struct
{
    LIN_Device_Frame_t frame;
    uint32_t timeMs;
    uint32_t generation;
} LIN_QueueEntry_t;

typedef struct
{
    LIN_QueueEntry_t entries[LIN_RING_BUF_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} LIN_RingBuf_t;

typedef struct
{
    uint8_t data[UDS_MAX_MESSAGE_LENGTH];
    uint8_t length;
    uint8_t count;
    uint8_t sequence;
    uint8_t nad;
    uint32_t timeMs;
    volatile uint32_t generation;
    volatile uint32_t txTimeMs;
} LIN_Diagnostic_t;

extern LIN_RingBuf_t s_rxRing;
extern LIN_RingBuf_t s_txRing;
extern LIN_Device_Frame_t s_frames[FRAME_TABLE_SIZE];
extern LIN_Diagnostic_t s_diag;
extern volatile uint32_t g_dbg_lin_tx_drop_count;
extern volatile uint32_t g_dbg_lin_tp_error_count;

void LINT_Init(void);
void LINT_Process(void);

#endif // LIN_TASK_H
