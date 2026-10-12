// @file    lin_task.h
// @brief   CJ LIN control/status protocol

#ifndef LIN_TASK_H
#define LIN_TASK_H

#include "lin_device.h"
#include "UDS.h"

#define LIN_RING_BUF_SIZE         32U
#define FRAME_TABLE_SIZE          9U
#define FID_CONTROL_RX_IDX        0U
#define FID_STATUS_TX_IDX         1U
#define FID_DIAG_RX_IDX           2U
#define FID_DIAG_TX_IDX           3U
#define FID_ID_CHANGE_RX_IDX      4U
#define FID_XCP_RX_IDX            5U
#define FID_XCP_TX_IDX            6U
#define FID_VERIFICATION_RX_IDX   7U
#define FID_VERIFICATION_TX_IDX   8U

#define LIN_STATUS_PART_FRT_LH     0x00U
#define LIN_STATUS_PART_FRT_RH     0x20U
#define LIN_STATUS_PART_HEADREST   0x40U
#define LIN_STATUS_PART_BACK       0x60U
#define LIN_STATUS_PART_LOGO       0x80U
#define LIN_STATUS_PART_STROBE     0xA0U

#define LIN_STATUS_TEMP_FAULT      0x01U
#define LIN_STATUS_VOLT_FAULT      0x02U
#define LIN_STATUS_SHORT_FAULT     0x04U
#define LIN_STATUS_OPEN_FAULT      0x08U
#define LIN_STATUS_COMM_FAULT      0x10U

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
