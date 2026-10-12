// @file    lin_device.c
// @brief   LIN slave driver - SERCOM0 USART_INT, FORM=0x4 (LIN Slave auto-baud)
//
//  Hardware LIN Slave mode (FORM=0x4):
//    - Break field: detected by hardware -> RXBRK interrupt (INTFLAG bit5)
//    - SYNC byte (0x55): auto-baud locked by hardware, no SW handling needed
//    - After Break+Sync: next bytes are PID, DATA[0..n], CHECKSUM via RXC
//
//  Pin mux:
//    PA04 = SERCOM0/PAD[0] PMUX_D -> LIN_TX  (TXPO=0)
//    PA07 = SERCOM0/PAD[3] PMUX_D -> LIN_RX  (RXPO=3)
//    PA06 = GPIO output, HIGH -> LIN_NSLP
//
// @copyright 2026 CJ 4DPLEX

#include "lin_device.h"
#include "clock_device.h"
#include "gpio_device.h"
#include <stddef.h>

volatile uint32_t g_dbg_lin_irq_count = 0U;
volatile uint32_t g_dbg_lin_rxbrk_count = 0U;
volatile uint32_t g_dbg_lin_rxc_count = 0U;
volatile uint32_t g_dbg_lin_rxs_count = 0U;
volatile uint32_t g_dbg_lin_error_count = 0U;
volatile uint32_t g_dbg_lin_rx_cb_count = 0U;
volatile uint32_t g_dbg_lin_drop_count = 0U;
volatile uint8_t g_dbg_lin_last_pid = 0U;
volatile uint8_t g_dbg_lin_last_raw_id = 0U;
volatile uint8_t g_dbg_lin_last_state = 0U;
volatile uint8_t g_dbg_lin_last_intflag = 0U;
volatile uint16_t g_dbg_lin_last_status = 0U;
volatile uint8_t g_dbg_lin_error_state = 0U;

LIN_State_t s_state = LIN_STATE_IDLE;
LIN_Device_Frame_t *s_frameTable = NULL;
uint8_t s_frameCount = 0U;
LIN_Device_Frame_t *s_active = NULL;
uint8_t s_dataIdx = 0U;
uint8_t s_pid = 0U;
LIN_Device_Frame_t s_anyFrame;
LIN_RxCallback_t s_rxCb = NULL;
LIN_TxCallback_t s_txCb = NULL;
__ALIGNED(256) uint32_t s_linVectorTable[LIN_VECTOR_COUNT];
uint8_t s_linVectorInstalled = 0U;

static void LIN_InstallSercom0Vector(void)
{
    if (s_linVectorInstalled != 0U)
    {
        return;
    }

    uint32_t *src = (uint32_t *)SCB->VTOR;
    for (uint32_t i = 0U; i < LIN_VECTOR_COUNT; i++)
    {
        s_linVectorTable[i] = src[i];
    }

    s_linVectorTable[16U + (uint32_t)SERCOM0_IRQn] = (uint32_t)SERCOM0_6_Handler;

    __disable_irq();
    SCB->VTOR = (uint32_t)s_linVectorTable;
    __DSB();
    __ISB();
    __enable_irq();

    s_linVectorInstalled = 1U;
}

static void LIN_WaitSync(void)
{
    uint32_t retry = 0U;
    while ((SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY & (SERCOM_USART_INT_SYNCBUSY_SWRST_Msk | SERCOM_USART_INT_SYNCBUSY_ENABLE_Msk | SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk)) != 0U)
    {
        __NOP();
        if (++retry >= 1000U)
        {
            break;
        }
    }
}

static LIN_Device_Frame_t *LIN_FindFrame(uint8_t rawId)
{
    for (uint8_t i = 0U; i < s_frameCount; i++)
    {
        if ((s_frameTable[i].frameIsValid != 0U) && (s_frameTable[i].frame_id == rawId))
        {
            return &s_frameTable[i];
        }
    }
    s_anyFrame.frame_id     = rawId;
    s_anyFrame.msg_type     = LIN_MSG_TYPE_RX;
    s_anyFrame.checksum     = LIN_CHECKSUM_CLASSIC;
    s_anyFrame.length       = LIN_BUFF_SIZE;
    s_anyFrame.frameIsValid = 1U;
    return &s_anyFrame;
}

static void LIN_SendByte(uint8_t b)
{
    uint32_t retry = 0U;
    while ((SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk) == 0U)
    {
        __NOP();
        if (++retry >= 1000U)
        {
            break;
        }
    }
    SERCOM0_REGS->USART_INT.SERCOM_DATA = b;
}

uint8_t LIN_CalcPID(uint8_t frameId)
{
    uint8_t id = frameId & 0x3FU;
    uint8_t p0 = (uint8_t)(((id>>0)^(id>>1)^(id>>2)^(id>>4)) & 0x01U);
    uint8_t p1 = (uint8_t)((~((id>>1)^(id>>3)^(id>>4)^(id>>5))) & 0x01U);
    return (uint8_t)(id | (p0 << 6U) | (p1 << 7U));
}

uint8_t LIN_CalcChecksum(const uint8_t *data, uint8_t len, uint8_t pid, LIN_ChecksumType_t type)
{
    uint32_t sum = (type == LIN_CHECKSUM_ENHANCED) ? pid : 0U;
    for (uint8_t i = 0U; i < len; i++)
    {
        sum += data[i];
        if (sum > 0xFFU) { sum -= 0xFFU; }
    }
    return (uint8_t)(~(uint8_t)sum);
}

void LIN_Init(LIN_RxCallback_t rxCb, LIN_TxCallback_t txCb)
{
    uint32_t retry = 0U;
    s_rxCb    = rxCb;
    s_txCb    = txCb;
    s_state   = LIN_STATE_IDLE;

    g_dbg_lin_irq_count   = 0U;
    g_dbg_lin_rxbrk_count = 0U;
    g_dbg_lin_rxc_count   = 0U;
    g_dbg_lin_rxs_count   = 0U;
    g_dbg_lin_error_count = 0U;
    g_dbg_lin_rx_cb_count = 0U;
    g_dbg_lin_drop_count  = 0U;
    g_dbg_lin_last_pid    = 0U;
    g_dbg_lin_last_raw_id = 0U;
    g_dbg_lin_last_state  = (uint8_t)LIN_STATE_IDLE;
    g_dbg_lin_last_intflag = 0U;

    //  1. Clocks
    Clock_EnableSercomApbc(0U);
    Clock_EnablePeripheral(LIN_GCLK_ID_CORE);

    //  2. GPIO
    GPIO_SetPmux(LIN_PIN_GROUP, LIN_TX_PIN, LIN_PIN_PMUX);
    GPIO_SetPmux(LIN_PIN_GROUP, LIN_RX_PIN, LIN_PIN_PMUX);
    GPIO_EnableInput(LIN_PIN_GROUP, LIN_RX_PIN);
    GPIO_Init(LIN_PIN_GROUP, LIN_EN_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);
    GPIO_Set(LIN_PIN_GROUP, LIN_EN_PIN, GPIO_HIGH);

    //  3. Software reset
    SERCOM0_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_SWRST_Msk;
    while ((SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_SWRST_Msk) != 0U)
    {
        __NOP();
        if (++retry >= 1000U)
        {
            break;
        }
    }

    //  4. CTRLA - write WITHOUT ENABLE first (DS60001632 requirement:
//    CTRLB and BAUD must be configured while ENABLE=0)
//    FORM=0x4 : Auto-baud LIN Slave
//               HW detects Break, locks baud on SYNC, fires RXBRK irq
    SERCOM0_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_MODE(0x1U) | SERCOM_USART_INT_CTRLA_SAMPR(0x1U) | SERCOM_USART_INT_CTRLA_TXPO(0x0U) | SERCOM_USART_INT_CTRLA_RXPO(0x3U) | SERCOM_USART_INT_CTRLA_FORM(0x4U) | SERCOM_USART_INT_CTRLA_IBON_Msk | SERCOM_USART_INT_CTRLA_DORD_Msk;
    LIN_WaitSync();

    //  5. CTRLB - must be set while ENABLE=0
    SERCOM0_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_CHSIZE(0x0U) | SERCOM_USART_INT_CTRLB_TXEN_Msk | SERCOM_USART_INT_CTRLB_RXEN_Msk;
    LIN_WaitSync();

    //  6. BAUD - must be set while ENABLE=0
    SERCOM0_REGS->USART_INT.SERCOM_BAUD = LIN_BAUD_REG;
    LIN_WaitSync();

    //  6b. Now enable SERCOM0 (ENABLE=1 full write, no RMW)
    SERCOM0_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_MODE(0x1U) | SERCOM_USART_INT_CTRLA_SAMPR(0x1U) | SERCOM_USART_INT_CTRLA_TXPO(0x0U) | SERCOM_USART_INT_CTRLA_RXPO(0x3U) | SERCOM_USART_INT_CTRLA_FORM(0x4U) | SERCOM_USART_INT_CTRLA_IBON_Msk | SERCOM_USART_INT_CTRLA_DORD_Msk | SERCOM_USART_INT_CTRLA_ENABLE_Msk;
    LIN_WaitSync();

    //  7. Clear stale flags
    SERCOM0_REGS->USART_INT.SERCOM_STATUS  = 0xFFFFU;
    SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = 0xFFU;

    //  8. Enable interrupts:
//    RXBRK : Break+Sync detected (start of frame)
//    RXC   : data byte ready
//    ERROR : bus errors
    SERCOM0_REGS->USART_INT.SERCOM_INTENCLR = 0xFFU;
    SERCOM0_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXBRK_Msk | SERCOM_USART_INT_INTENSET_RXC_Msk | SERCOM_USART_INT_INTENSET_RXS_Msk | SERCOM_USART_INT_INTENSET_ERROR_Msk;

    //  9. NVIC
    
    LIN_InstallSercom0Vector();
    NVIC_ClearPendingIRQ(SERCOM0_IRQn);
    NVIC_SetPriority(SERCOM0_IRQn, 1U);
    NVIC_EnableIRQ(SERCOM0_IRQn);
}

void LIN_RegisterFrames(LIN_Device_Frame_t *frames, uint8_t frameCount)
{
    s_frameTable = frames;
    s_frameCount = frameCount;
}

//  ===========================================================
//   ISR
//  ===========================================================
void LIN_IrqHandler(void)
{
    uint8_t intflag = SERCOM0_REGS->USART_INT.SERCOM_INTFLAG;
    g_dbg_lin_irq_count++;
    g_dbg_lin_last_intflag = intflag;

    //  --- RXS: receive start detected (some LIN transceivers raise this first) ---
    if ((intflag & SERCOM_USART_INT_INTFLAG_RXS_Msk) != 0U)
    {
        g_dbg_lin_rxs_count++;
        SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = SERCOM_USART_INT_INTFLAG_RXS_Msk;
        if (s_state == LIN_STATE_IDLE)
        {
            s_dataIdx = 0U;
            s_active  = NULL;
            s_pid     = 0U;
            s_state   = LIN_STATE_PID;
            g_dbg_lin_last_state = (uint8_t)LIN_STATE_PID;
        }
    }

    //  --- RXBRK: hardware break+sync detected -> new frame start ---
    if ((intflag & SERCOM_USART_INT_INTFLAG_RXBRK_Msk) != 0U)
    {
        g_dbg_lin_rxbrk_count++;
        g_dbg_lin_last_state = (uint8_t)LIN_STATE_PID;
        //  Clear RXBRK flag
        SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = SERCOM_USART_INT_INTFLAG_RXBRK_Msk;

        s_dataIdx = 0U;
        s_active  = NULL;
        s_pid     = 0U;
        s_state   = LIN_STATE_PID;
        return;
    }

    //  --- ERROR ---
// Known benign FERR sources in LIN FORM=0x4 mode:
// [1] BREAK field FERR:
//     The LIN Break field is dominant-low for longer than a normal UART
//     character, so the USART may report a framing error before PID/data.
//     Depending on sampling phase, RXBRK may already be pending or may
//     follow one interrupt later. In both cases we should treat this as
//     a new frame start instead of aborting reception.
// [2] TX echo-back FERR (state=DATA_TX, FERR=1, BUFOVF=0, PERR=0):
//     ATA663211 loops TX back to RX on the single-wire bus.
//     SERCOM sees its own byte as an unexpected RXC -> FERR.
//     Safe to ignore; TX state machine continues normally.
    if ((intflag & SERCOM_USART_INT_INTFLAG_ERROR_Msk) != 0U)
    {
        g_dbg_lin_error_count++;
        uint16_t status = SERCOM0_REGS->USART_INT.SERCOM_STATUS;
        g_dbg_lin_last_status = status;
        g_dbg_lin_error_state = (uint8_t)s_state;

        SERCOM0_REGS->USART_INT.SERCOM_STATUS  = 0xFFFFU;
        SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = SERCOM_USART_INT_INTFLAG_ERROR_Msk;

        //  Drain stale RX byte
        if ((SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk) != 0U)
        {
            (void)SERCOM0_REGS->USART_INT.SERCOM_DATA;
        }

        uint8_t isFerOnly = ((status & SERCOM_USART_INT_STATUS_FERR_Msk) != 0U) && ((status & SERCOM_USART_INT_STATUS_BUFOVF_Msk) == 0U) && ((status & SERCOM_USART_INT_STATUS_PERR_Msk) == 0U);

        //  [1a] Break-field FERR: RXBRK is already pending
        if (isFerOnly && ((SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXBRK_Msk) != 0U))
        {
            //  Clear RXBRK, reset state machine to PID
            SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = SERCOM_USART_INT_INTFLAG_RXBRK_Msk;
            s_dataIdx = 0U;
            s_active  = NULL;
            s_pid     = 0U;
            s_state   = LIN_STATE_PID;
            g_dbg_lin_rxbrk_count++;
            return;
        }

        //  [1b] Break-field FERR seen one IRQ earlier than RXBRK.
// The user-observed pattern STATUS=2, state=IDLE, pid=0 matches this.
// Re-arm the state machine so the following PID byte can still be
// accepted when RXBRK/RXC arrives next.
        if (isFerOnly && ((s_state == LIN_STATE_IDLE) || (s_state == LIN_STATE_PID)))
        {
            s_dataIdx = 0U;
            s_active  = NULL;
            s_pid     = 0U;
            s_state   = LIN_STATE_PID;
            g_dbg_lin_last_state = (uint8_t)LIN_STATE_PID;
            return;
        }

        //  [2] TX echo-back FERR
        if (isFerOnly && (s_state == LIN_STATE_DATA_TX))
        {
            return;  //  echo-back FERR: ignore, TX continues
        }

        //  Real bus error: abort
        s_state = LIN_STATE_IDLE;
        return;
    }

    //  --- RXC: data byte ---
    if ((intflag & SERCOM_USART_INT_INTFLAG_RXC_Msk) == 0U)
    {
        return;
    }

    uint8_t byte = (uint8_t)(SERCOM0_REGS->USART_INT.SERCOM_DATA & 0xFFU);
    g_dbg_lin_rxc_count++;

    switch (s_state)
    {
        case LIN_STATE_IDLE:
            break;

        case LIN_STATE_PID:
        {
            //  In LIN slave auto-baud mode some revisions still surface the
// SYNC byte (0x55) via RXC. If we treat it as PID, the following
// real PID/ID byte is shifted into DATA[0]. Ignore SYNC here and
// wait for the actual PID byte.
            if (byte == 0x55U)
            {
                break;
            }

            uint8_t rawId = byte & 0x3FU;
            uint8_t expectedPid = LIN_CalcPID(rawId);

            //  Accept only a PID with valid parity. Any other byte is noise or
// a misplaced SYNC/start byte, so keep waiting for the PID.
            if (byte != expectedPid)
            {
                s_active = NULL;
                s_dataIdx = 0U;
                break;
            }

            s_pid     = byte;
            s_dataIdx = 0U;
            s_active  = LIN_FindFrame(rawId);
            g_dbg_lin_last_pid    = byte;
            g_dbg_lin_last_raw_id = rawId;

            if (s_active->msg_type == LIN_MSG_TYPE_RX)
            {
                s_state = LIN_STATE_DATA_RX;
            }
            else
            {
                if (s_txCb != NULL) { s_txCb(s_active); }
                LIN_SendByte(s_active->data[s_dataIdx]);
                s_dataIdx = 1U;
                s_state   = LIN_STATE_DATA_TX;
            }
            break;
        }

        case LIN_STATE_DATA_RX:
            s_active->data[s_dataIdx++] = byte;
            if (s_dataIdx >= s_active->length)
            {
                s_state = LIN_STATE_CHECKSUM;
            }
            break;

        case LIN_STATE_CHECKSUM:
            if (byte == LIN_CalcChecksum(s_active->data, s_active->length, s_pid, s_active->checksum))
            {
                if (s_rxCb != NULL)
                {
                    s_rxCb(s_active);
                }
            }
            else
            {
                g_dbg_lin_error_count++;
            }
            s_state = LIN_STATE_IDLE;
            break;

        case LIN_STATE_DATA_TX:
            if (s_dataIdx < s_active->length)
            {
                LIN_SendByte(s_active->data[s_dataIdx++]);
            }
            else
            {
                uint8_t cs = LIN_CalcChecksum(s_active->data, s_active->length, s_pid, s_active->checksum);
                LIN_SendByte(cs);
                s_state = LIN_STATE_IDLE;
            }
            break;

        default:
            s_state = LIN_STATE_IDLE;
            break;
    }
}

void SERCOM0_6_Handler(void)
{
    LIN_IrqHandler();
}

