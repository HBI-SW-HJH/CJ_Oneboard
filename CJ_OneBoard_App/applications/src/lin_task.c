// @file    lin_task.c
// @brief   CJ LIN control/status protocol

#include "appConfig.h"

LIN_RingBuf_t s_rxRing;
LIN_RingBuf_t s_txRing;
LIN_Device_Frame_t s_frames[FRAME_TABLE_SIZE];
LIN_Diagnostic_t s_diag;
volatile uint32_t g_dbg_lin_tx_drop_count;
volatile uint32_t g_dbg_lin_tp_error_count;

static void LINT_RxCallback(const LIN_Device_Frame_t *frame);
static void LINT_TxCallback(LIN_Device_Frame_t *frame);
static void LINT_HandleLampControl(const LIN_Device_Frame_t *frame);
static void LINT_HandleDiagnostic(const LIN_QueueEntry_t *entry);

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

static void LINT_HandleLampControl(const LIN_Device_Frame_t *frame)
{
    LedControlCommand_t command;
    int32_t denominator;
    int32_t cieX;
    int32_t cieY;
    int32_t cieZ;
    int64_t xValue;
    int64_t yValue;
    int64_t zValue;
    int64_t red;
    int64_t green;
    int64_t blue;
    int64_t maximum;

    // LIN ID 0x01 Lamp Control data.
    // D0 Chair: Bit0=Chair1, Bit1=Chair2, Bit2=Chair3, Bit3=Chair4. 0x0F selects all chairs.
    // D1 LED: Bit0=FRT_LH, Bit1=FRT_RH, Bit2=Headrest, Bit3=Back, Bit4=Logo, Bit5=Strobe.
    // D2 Control: Bit7:6 Color, Bit5 Fade, Bit4 Active, Bit3 Sync, Bit2:0 Mode.
    // D2 Color: 00=RGB, 01=CIE1931, 10=CIE1976.
    // D2 Mode: 6=Flicker, 7=Normal.
    // D3: RGB=Red, CIE1931=x, CIE1976=u'.
    // D4: RGB=Green, CIE1931=y, CIE1976=v'.
    // D5: RGB=Blue, CIE1931/CIE1976=Brightness.
    // D6: Normal+Fade=Fade In x 100ms, Flicker=ON Duration 0..250.
    // D7: Normal+Fade=Fade Out x 100ms, Flicker=Frequency 0..100Hz.

    command.chairMask = frame->data[0U] & 0x0FU;
    command.ledMask = frame->data[1U] & 0x3FU;
    command.colorMode = (frame->data[2U] >> 6U) & 0x03U;
    command.fadeEnabled = (frame->data[2U] >> 5U) & 0x01U;
    command.active = (frame->data[2U] >> 4U) & 0x01U;
    command.sync = (frame->data[2U] >> 3U) & 0x01U;
    command.mode = frame->data[2U] & 0x07U;
    command.color1 = frame->data[3U];
    command.color2 = frame->data[4U];
    command.color3 = frame->data[5U];
    command.effect1 = (frame->data[6U] > 250U) ? 250U : frame->data[6U];
    command.effect2 = frame->data[7U];
    command.red = 0U;
    command.green = 0U;
    command.blue = 0U;
    command.timeMs = oAppTimer.timeMs;

    if ((gConFig.Chair_Number < 0x0AU) || (gConFig.Chair_Number > 0x0DU))
    {
        return;
    }
    if ((command.chairMask & (uint8_t)(1U << (gConFig.Chair_Number - 0x0AU))) == 0U)
    {
        return;
    }
    if (command.ledMask == 0U)
    {
        return;
    }
    if ((command.mode < 6U) || (command.mode > 7U))
    {
        return;
    }
    if (command.mode == 6U)
    {
        command.effect2 = (command.effect2 > 100U) ? 100U : command.effect2;
    }
    else
    {
        command.effect2 = (command.effect2 > 250U) ? 250U : command.effect2;
    }

    if (command.active == 0U)
    {
        SRGBControl_Set(&command);
        OSPControl_Set(&command);
        return;
    }
    if (command.colorMode > 2U)
    {
        return;
    }

    if (command.colorMode == 0U)
    {
        command.red = (uint16_t)((uint16_t)command.color1 * 257U);
        command.green = (uint16_t)((uint16_t)command.color2 * 257U);
        command.blue = (uint16_t)((uint16_t)command.color3 * 257U);
    }
    else
    {
        if (((command.color1 == 0U) && (command.color2 == 0U)) || (command.color3 == 0U))
        {
            command.active = 0U;
            SRGBControl_Set(&command);
            OSPControl_Set(&command);
            return;
        }
        if ((command.color1 > 250U) || (command.color2 > 250U))
        {
            return;
        }

        if (command.colorMode == 1U)
        {
            cieX = command.color1;
            cieY = command.color2;
        }
        else
        {
            denominator = ((int32_t)command.color1 * 6) - ((int32_t)command.color2 * 16) + 3000;
            if (denominator <= 0)
            {
                return;
            }
            cieX = ((int32_t)command.color1 * 2250) / denominator;
            cieY = ((int32_t)command.color2 * 1000) / denominator;
        }

        if ((cieX < 0) || (cieX > 250) || (cieY <= 0) || (cieY > 250))
        {
            return;
        }
        if (command.color3 != 0U)
        {
            cieZ = 250 - cieX - cieY;
            cieZ = (cieZ < 0) ? 0 : cieZ;
            yValue = command.color3;
            xValue = ((int64_t)cieX * yValue) / cieY;
            zValue = ((int64_t)cieZ * yValue) / cieY;
            red = ((32406LL * xValue) - (15372LL * yValue) - (4986LL * zValue)) / 10000LL;
            green = ((-9689LL * xValue) + (18758LL * yValue) + (415LL * zValue)) / 10000LL;
            blue = ((557LL * xValue) - (2040LL * yValue) + (10570LL * zValue)) / 10000LL;
            red = (red < 0LL) ? 0LL : red;
            green = (green < 0LL) ? 0LL : green;
            blue = (blue < 0LL) ? 0LL : blue;
            maximum = red;
            maximum = (green > maximum) ? green : maximum;
            maximum = (blue > maximum) ? blue : maximum;
            if (maximum != 0LL)
            {
                command.red = (uint16_t)((red * command.color3 * 65535LL) / (maximum * 255LL));
                command.green = (uint16_t)((green * command.color3 * 65535LL) / (maximum * 255LL));
                command.blue = (uint16_t)((blue * command.color3 * 65535LL) / (maximum * 255LL));
            }
        }
    }

    SRGBControl_Set(&command);
    OSPControl_Set(&command);
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
        if ((data[0U] != (uint8_t)(gConFig.Chair_Number - 0x09U)) && (data[0U] != UDS_BROADCAST_NAD))
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
            responseLength = UDS_HandleRequest(s_diag.data, s_diag.length, response);
        }
        s_diag.length = 0U;
        s_frames[FID_STATUS_TX_IDX].frame_id = gConFig.Chair_Number;
        s_frames[FID_ID_CHANGE_RX_IDX].frame_id = (uint8_t)(gConFig.Chair_Number + CJ_ID_CHANGE_OFFSET);
        s_frames[FID_XCP_RX_IDX].frame_id = (uint8_t)(XCP_REQUEST_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
        s_frames[FID_XCP_TX_IDX].frame_id = (uint8_t)(XCP_RESPONSE_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
        s_frames[FID_XCP_TX_IDX].frameIsValid = FALSE;
        s_frames[FID_VERIFICATION_RX_IDX].frame_id = (uint8_t)(VERIFICATION_REQUEST_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
        s_frames[FID_VERIFICATION_TX_IDX].frame_id = (uint8_t)(VERIFICATION_RESPONSE_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
        if (responseLength != 0U)
        {
            LINT_QueueDiagnosticResponse(s_diag.nad, response, responseLength, entry->generation);
        }
    }
}

static void LINT_RxCallback(const LIN_Device_Frame_t *frame)
{
    LIN_QueueEntry_t entry;
    if ((frame->frame_id == CJ_CONTROL_FRAME_ID) || (frame->frame_id == CJ_DIAG_REQUEST_ID) || (frame->frame_id == s_frames[FID_ID_CHANGE_RX_IDX].frame_id) || (frame->frame_id == s_frames[FID_XCP_RX_IDX].frame_id) || (frame->frame_id == s_frames[FID_VERIFICATION_RX_IDX].frame_id))
    {
        if ((frame->frame_id == CJ_DIAG_REQUEST_ID) && ((frame->data[1U] >> 4U) <= 1U))
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
}

static void LINT_TxCallback(LIN_Device_Frame_t *frame)
{
    LIN_QueueEntry_t entry;
    uint8_t head;
    uint8_t tail;
    if (frame->frame_id == s_frames[FID_STATUS_TX_IDX].frame_id)
    {
        frame->data[0U] = (uint8_t)(LIN_STATUS_PART_FRT_LH | SRGBControl_GetFaultMask(SRGB_FAULT_PART_FRT_LH));
        frame->data[1U] = (uint8_t)(LIN_STATUS_PART_FRT_RH | SRGBControl_GetFaultMask(SRGB_FAULT_PART_FRT_RH));
        frame->data[2U] = (uint8_t)(LIN_STATUS_PART_HEADREST | SRGBControl_GetFaultMask(SRGB_FAULT_PART_HEADREST));
        frame->data[3U] = (uint8_t)(LIN_STATUS_PART_BACK | SRGBControl_GetFaultMask(SRGB_FAULT_PART_BACK));
        frame->data[4U] = (uint8_t)(LIN_STATUS_PART_LOGO | OSPControl_GetFaultMask(OSP_FAULT_PART_LOGO));
        frame->data[5U] = (uint8_t)(LIN_STATUS_PART_STROBE | OSPControl_GetFaultMask(OSP_FAULT_PART_STROBE));
        frame->data[6U] = 0U;
        frame->data[7U] = 0U;
    }
    else if (frame->frame_id == CJ_DIAG_RESPONSE_ID)
    {
        // Driver calls this BEFORE data[0]. Keep this data stable until frame completion.
        if (RingBuf_Pop(&s_txRing, &entry) != 0U)
        {
            (void)memcpy(frame->data, entry.frame.data, LIN_FRAME_DATA_LEN);
        }
        if ((gReprogram.resetPending != 0U) && (frame->data[2U] == 0x50U))
        {
            gReprogram.resetTime = oAppTimer.timeMs;
            gReprogram.resetResponseStarted = 1U;
        }
        s_diag.txTimeMs = oAppTimer.timeMs;
        head = s_txRing.head;
        tail = s_txRing.tail;
        frame->frameIsValid = (head != tail) ? TRUE : FALSE;
    }
    else if (frame->frame_id == s_frames[FID_XCP_TX_IDX].frame_id)
    {
        (void)memcpy(frame->data, gXCPControl.response, XCP_FRAME_LENGTH);
        frame->frameIsValid = FALSE;
    }
    else if (frame->frame_id == s_frames[FID_VERIFICATION_TX_IDX].frame_id)
    {
        (void)memcpy(frame->data, gVerification.response, LIN_FRAME_DATA_LEN);
    }
    else
    {
        // Other frames have no publisher action.
    }
}

void LINT_Init(void)
{
    UDS_Init();
    XCP_Init();
    Verification_Init();
    (void)memset(&s_rxRing, 0, sizeof(s_rxRing));
    (void)memset(&s_txRing, 0, sizeof(s_txRing));
    (void)memset(&s_diag, 0, sizeof(s_diag));
    g_dbg_lin_tx_drop_count = 0U;
    g_dbg_lin_tp_error_count = 0U;
    (void)memset(s_frames, 0, sizeof(s_frames));

    s_frames[FID_CONTROL_RX_IDX].frame_id = CJ_CONTROL_FRAME_ID;
    s_frames[FID_CONTROL_RX_IDX].msg_type = LIN_MSG_TYPE_RX;
    s_frames[FID_CONTROL_RX_IDX].checksum = LIN_CHECKSUM_ENHANCED;
    s_frames[FID_CONTROL_RX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_CONTROL_RX_IDX].frameIsValid = TRUE;

    s_frames[FID_STATUS_TX_IDX].frame_id = gConFig.Chair_Number;
    s_frames[FID_STATUS_TX_IDX].msg_type = LIN_MSG_TYPE_TX;
    s_frames[FID_STATUS_TX_IDX].checksum = LIN_CHECKSUM_ENHANCED;
    s_frames[FID_STATUS_TX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_STATUS_TX_IDX].frameIsValid = TRUE;

    s_frames[FID_DIAG_RX_IDX].frame_id = CJ_DIAG_REQUEST_ID;
    s_frames[FID_DIAG_RX_IDX].msg_type = LIN_MSG_TYPE_RX;
    s_frames[FID_DIAG_RX_IDX].checksum = LIN_CHECKSUM_CLASSIC;
    s_frames[FID_DIAG_RX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_DIAG_RX_IDX].frameIsValid = TRUE;

    s_frames[FID_DIAG_TX_IDX].frame_id = CJ_DIAG_RESPONSE_ID;
    s_frames[FID_DIAG_TX_IDX].msg_type = LIN_MSG_TYPE_TX;
    s_frames[FID_DIAG_TX_IDX].checksum = LIN_CHECKSUM_CLASSIC;
    s_frames[FID_DIAG_TX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_DIAG_TX_IDX].frameIsValid = FALSE;

    s_frames[FID_ID_CHANGE_RX_IDX].frame_id = (uint8_t)(gConFig.Chair_Number + CJ_ID_CHANGE_OFFSET);
    s_frames[FID_ID_CHANGE_RX_IDX].msg_type = LIN_MSG_TYPE_RX;
    s_frames[FID_ID_CHANGE_RX_IDX].checksum = LIN_CHECKSUM_ENHANCED;
    s_frames[FID_ID_CHANGE_RX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_ID_CHANGE_RX_IDX].frameIsValid = TRUE;

    s_frames[FID_XCP_RX_IDX].frame_id = (uint8_t)(XCP_REQUEST_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
    s_frames[FID_XCP_RX_IDX].msg_type = LIN_MSG_TYPE_RX;
    s_frames[FID_XCP_RX_IDX].checksum = LIN_CHECKSUM_ENHANCED;
    s_frames[FID_XCP_RX_IDX].length = XCP_FRAME_LENGTH;
    s_frames[FID_XCP_RX_IDX].frameIsValid = TRUE;

    s_frames[FID_XCP_TX_IDX].frame_id = (uint8_t)(XCP_RESPONSE_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
    s_frames[FID_XCP_TX_IDX].msg_type = LIN_MSG_TYPE_TX;
    s_frames[FID_XCP_TX_IDX].checksum = LIN_CHECKSUM_ENHANCED;
    s_frames[FID_XCP_TX_IDX].length = XCP_FRAME_LENGTH;
    s_frames[FID_XCP_TX_IDX].frameIsValid = FALSE;

    s_frames[FID_VERIFICATION_RX_IDX].frame_id = (uint8_t)(VERIFICATION_REQUEST_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
    s_frames[FID_VERIFICATION_RX_IDX].msg_type = LIN_MSG_TYPE_RX;
    s_frames[FID_VERIFICATION_RX_IDX].checksum = LIN_CHECKSUM_ENHANCED;
    s_frames[FID_VERIFICATION_RX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_VERIFICATION_RX_IDX].frameIsValid = TRUE;

    s_frames[FID_VERIFICATION_TX_IDX].frame_id = (uint8_t)(VERIFICATION_RESPONSE_ID_BASE + gConFig.Chair_Number - CONFIG_CHAIR_MIN);
    s_frames[FID_VERIFICATION_TX_IDX].msg_type = LIN_MSG_TYPE_TX;
    s_frames[FID_VERIFICATION_TX_IDX].checksum = LIN_CHECKSUM_ENHANCED;
    s_frames[FID_VERIFICATION_TX_IDX].length = LIN_FRAME_DATA_LEN;
    s_frames[FID_VERIFICATION_TX_IDX].frameIsValid = TRUE;

    LIN_RegisterFrames(s_frames, FRAME_TABLE_SIZE);
    LIN_Init(LINT_RxCallback, LINT_TxCallback);
}

void LINT_Process(void)
{
    LIN_QueueEntry_t entry;
    while (RingBuf_Pop(&s_rxRing, &entry) != 0U)
    {
        if (entry.frame.frame_id == CJ_CONTROL_FRAME_ID)
        {
            LINT_HandleLampControl(&entry.frame);
        }
        else if (entry.frame.frame_id == CJ_DIAG_REQUEST_ID)
        {
            LINT_HandleDiagnostic(&entry);
        }
        else if (entry.frame.frame_id == s_frames[FID_ID_CHANGE_RX_IDX].frame_id)
        {
            uint8_t newChairId = entry.frame.data[1U];
            if ((newChairId < CONFIG_CHAIR_MIN) || (newChairId > CONFIG_CHAIR_MAX) || (newChairId == gConFig.Chair_Number))
            {
                continue;
            }
            if ((entry.frame.data[0U] != gConFig.Chair_Number) || (entry.frame.data[2U] != gConFig.Chair_Number) || (entry.frame.data[4U] != gConFig.Chair_Number) || (entry.frame.data[6U] != gConFig.Chair_Number))
            {
                continue;
            }
            if ((entry.frame.data[3U] != newChairId) || (entry.frame.data[5U] != newChairId) || (entry.frame.data[7U] != newChairId))
            {
                continue;
            }
            gConFig.Chair_Number = newChairId;
            if (PDS_Write(&gConFig) != PDS_WRITE_OK)
            {
                gConFig.Chair_Number = (uint8_t)(entry.frame.frame_id - CJ_ID_CHANGE_OFFSET);
                continue;
            }
            s_frames[FID_STATUS_TX_IDX].frame_id = newChairId;
            s_frames[FID_ID_CHANGE_RX_IDX].frame_id = (uint8_t)(newChairId + CJ_ID_CHANGE_OFFSET);
            s_frames[FID_XCP_RX_IDX].frame_id = (uint8_t)(XCP_REQUEST_ID_BASE + newChairId - CONFIG_CHAIR_MIN);
            s_frames[FID_XCP_TX_IDX].frame_id = (uint8_t)(XCP_RESPONSE_ID_BASE + newChairId - CONFIG_CHAIR_MIN);
            s_frames[FID_XCP_TX_IDX].frameIsValid = FALSE;
            s_frames[FID_VERIFICATION_RX_IDX].frame_id = (uint8_t)(VERIFICATION_REQUEST_ID_BASE + newChairId - CONFIG_CHAIR_MIN);
            s_frames[FID_VERIFICATION_TX_IDX].frame_id = (uint8_t)(VERIFICATION_RESPONSE_ID_BASE + newChairId - CONFIG_CHAIR_MIN);
        }
        else if (entry.frame.frame_id == s_frames[FID_XCP_RX_IDX].frame_id)
        {
            XCP_HandleRequest(entry.frame.data);
            s_frames[FID_XCP_TX_IDX].frameIsValid = TRUE;
        }
        else if (entry.frame.frame_id == s_frames[FID_VERIFICATION_RX_IDX].frame_id)
        {
            Verification_Request(entry.frame.data);
        }
        else
        {
            // Ignore unregistered frames.
        }
    }
    UDS_Timeout();
}
