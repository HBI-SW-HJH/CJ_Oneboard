#include "appConfig.h"

LIN_RingBuf_t s_rxRing;
LIN_RingBuf_t s_txRing;
LIN_Device_Frame_t s_frames[FRAME_TABLE_SIZE];
LIN_Diagnostic_t s_diag;
volatile uint32_t g_dbg_lin_tx_drop_count;
volatile uint32_t g_dbg_lin_tp_error_count;

static uint8_t RingBuf_Push(LIN_RingBuf_t *ring, const LIN_QueueEntry_t *entry)
{
    uint8_t head = ring->head;
    if ((uint8_t)(head - ring->tail) >= LIN_RING_BUF_SIZE)
    {
        return 0U;
    }
    ring->entries[head & (LIN_RING_BUF_SIZE - 1U)] = *entry;
    __DMB();
    ring->head = (uint8_t)(head + 1U);
    return 1U;
}

static uint8_t RingBuf_Pop(LIN_RingBuf_t *ring, LIN_QueueEntry_t *entry)
{
    uint8_t tail = ring->tail;
    if (ring->head == tail)
    {
        return 0U;
    }
    __DMB();
    *entry = ring->entries[tail & (LIN_RING_BUF_SIZE - 1U)];
    __DMB();
    ring->tail = (uint8_t)(tail + 1U);
    return 1U;
}


static void LINT_QueueDiagnosticResponse(uint8_t nad, const uint8_t *data, uint8_t length, uint32_t generation)
{
    LIN_QueueEntry_t entry;
    uint8_t offset = 0U;
    uint8_t sequence = 1U;
    uint8_t copyLength;
    uint8_t dataIndex;
    uint32_t irqState;
    (void)memset(&entry, 0, sizeof(entry));
    entry.frame = s_frames[FID_DIAG_TX_IDX];
    while (offset < length)
    {
        (void)memset(entry.frame.data, 0xFF, LIN_FRAME_DATA_LEN);
        entry.frame.data[0U] = nad;
        if (length <= 6U)
        {
            entry.frame.data[1U] = length;
            dataIndex = 2U;
            copyLength = length;
        }
        else if (offset == 0U)
        {
            entry.frame.data[1U] = 0x10U;
            entry.frame.data[2U] = length;
            dataIndex = 3U;
            copyLength = 5U;
        }
        else
        {
            entry.frame.data[1U] = (uint8_t)(0x20U | sequence);
            sequence = (uint8_t)((sequence + 1U) & 0x0FU);
            dataIndex = 2U;
            copyLength = (uint8_t)(length - offset);
            copyLength = (copyLength > 6U) ? 6U : copyLength;
        }
        (void)memcpy(&entry.frame.data[dataIndex], &data[offset], copyLength);
        if (RingBuf_Push(&s_txRing, &entry) == 0U)
        {
            g_dbg_lin_tx_drop_count++;
            s_txRing.tail = s_txRing.head;
            return;
        }
        offset = (uint8_t)(offset + copyLength);
    }

    // Publish only after every response frame is stored in the TX ring.
    irqState = __get_PRIMASK();
    __disable_irq();
    s_diag.txTimeMs = oAppTimer.timeMs;
    s_frames[FID_DIAG_TX_IDX].frameIsValid = (generation == s_diag.generation) ? TRUE : FALSE;
    __set_PRIMASK(irqState);
}

static void LINT_HandleDiagnostic(const LIN_QueueEntry_t *entry)
{
    const uint8_t *data = entry->frame.data;
    uint8_t type = (uint8_t)(data[1U] >> 4U);
    uint8_t count;
    uint16_t length;
    uint8_t response[UDS_MAX_MESSAGE_LENGTH];
    uint8_t responseLength;
    if (entry->generation != s_diag.generation)
    {
        return;
    }
    if ((type == 0U) || (type == 1U))
    {
        s_diag.length = 0U;
        s_diag.count = 0U;
        s_frames[FID_DIAG_TX_IDX].frameIsValid = FALSE;
        s_txRing.tail = s_txRing.head;
        if ((data[0U] != UDS_BOOT_NAD) && (data[0U] != UDS_BROADCAST_NAD))
        {
            return;
        }
        s_diag.nad = data[0U];
        if (type == 0U)
        {
            count = (uint8_t)(data[1U] & 0x0FU);
            if ((count == 0U) || (count > 6U))
            {
                g_dbg_lin_tp_error_count++;
                return;
            }
            s_diag.length = count;
            s_diag.count = count;
            (void)memcpy(s_diag.data, &data[2U], count);
        }
        else
        {
            length = (uint16_t)(((uint16_t)(data[1U] & 0x0FU) << 8U) | data[2U]);
            if ((length < 7U) || (length > UDS_MAX_MESSAGE_LENGTH))
            {
                g_dbg_lin_tp_error_count++;
                return;
            }
            s_diag.length = (uint8_t)length;
            s_diag.count = 5U;
            s_diag.sequence = 1U;
            (void)memcpy(s_diag.data, &data[3U], 5U);
        }
    }
    else if (type == 2U)
    {
        if ((s_diag.length == 0U) || (data[0U] != s_diag.nad))
        {
            return;
        }
        if (((data[1U] & 0x0FU) != s_diag.sequence) || ((uint32_t)(entry->timeMs - s_diag.timeMs) > UDS_TRANSPORT_TIMEOUT_MS))
        {
            s_diag.length = 0U;
            g_dbg_lin_tp_error_count++;
            return;
        }
        count = (uint8_t)(s_diag.length - s_diag.count);
        count = (count > 6U) ? 6U : count;
        (void)memcpy(&s_diag.data[s_diag.count], &data[2U], count);
        s_diag.count = (uint8_t)(s_diag.count + count);
        s_diag.sequence = (uint8_t)((s_diag.sequence + 1U) & 0x0FU);
    }
    else
    {
        return;
    }
    s_diag.timeMs = entry->timeMs;
    if (s_diag.count == s_diag.length)
    {
        if (s_diag.nad == UDS_BROADCAST_NAD)
        {
            responseLength = UDS_HandleBroadcast(s_diag.data, s_diag.length, response);
        }
        else
        {
            if ((s_diag.length == 11U) && (s_diag.data[0U] == 0x34U) && (gReprogram.selected != 0U) && (gReprogram.session != 0U))
            {
                response[0U] = 0x7FU;
                response[1U] = 0x34U;
                response[2U] = 0x78U;
                LINT_QueueDiagnosticResponse(s_diag.nad, response, 3U, entry->generation);
            }
            responseLength = UDS_HandleRequest(s_diag.data, s_diag.length, response);
        }
        s_diag.length = 0U;
        if (responseLength != 0U)
        {
            LINT_QueueDiagnosticResponse(s_diag.nad, response, responseLength, entry->generation);
        }
    }
}


static void LINT_RxCallback(const LIN_Device_Frame_t *frame)
{
    LIN_QueueEntry_t entry;
    if (frame->frame_id != CJ_DIAG_REQUEST_ID)
    {
        return;
    }
    if ((frame->data[1U] >> 4U) <= 1U)
    {
        s_diag.generation++;
        s_frames[FID_DIAG_TX_IDX].frameIsValid = FALSE;
    }
    entry.frame = *frame;
    entry.timeMs = oAppTimer.timeMs;
    entry.generation = s_diag.generation;
    if (RingBuf_Push(&s_rxRing, &entry) == 0U)
    {
        g_dbg_lin_drop_count++;
    }
}

static void LINT_TxCallback(LIN_Device_Frame_t *frame)
{
    LIN_QueueEntry_t entry;
    uint8_t head;
    uint8_t tail;
        // Driver calls this BEFORE data[0]. Keep this data stable until frame completion.
        if (RingBuf_Pop(&s_txRing, &entry) != 0U)
        {
            (void)memcpy(frame->data, entry.frame.data, LIN_FRAME_DATA_LEN);
        }
        if ((gReprogram.resetPending != 0U) && (frame->data[2U] == 0x51U))
        {
            gReprogram.resetTime = oAppTimer.timeMs;
            gReprogram.resetResponseStarted = 1U;
        }
        s_diag.txTimeMs = oAppTimer.timeMs;
        head = s_txRing.head;
        tail = s_txRing.tail;
        frame->frameIsValid = (head != tail) ? TRUE : FALSE;

}

void LINT_Init(void)
{
    UDS_Init();
    memset(&s_rxRing, 0, sizeof(s_rxRing));
    memset(&s_txRing, 0, sizeof(s_txRing));
    memset(&s_diag, 0, sizeof(s_diag));
    memset(s_frames, 0, sizeof(s_frames));
    s_frames[FID_DIAG_RX_IDX].frame_id = CJ_DIAG_REQUEST_ID;
    s_frames[FID_DIAG_RX_IDX].msg_type = LIN_MSG_TYPE_RX;
    s_frames[FID_DIAG_RX_IDX].checksum = LIN_CHECKSUM_CLASSIC;
    s_frames[FID_DIAG_RX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_DIAG_RX_IDX].frameIsValid = TRUE;
    s_frames[FID_DIAG_TX_IDX].frame_id = CJ_DIAG_RESPONSE_ID;
    s_frames[FID_DIAG_TX_IDX].msg_type = LIN_MSG_TYPE_TX;
    s_frames[FID_DIAG_TX_IDX].checksum = LIN_CHECKSUM_CLASSIC;
    s_frames[FID_DIAG_TX_IDX].length = LIN_FRAME_DATA_LEN;
    LIN_RegisterFrames(s_frames, FRAME_TABLE_SIZE);
    LIN_Init(LINT_RxCallback, LINT_TxCallback);
}

void LINT_Process(void)
{
    LIN_QueueEntry_t entry;
    while (RingBuf_Pop(&s_rxRing, &entry) != 0U)
    {
        LINT_HandleDiagnostic(&entry);
    }
    UDS_Timeout();
}
