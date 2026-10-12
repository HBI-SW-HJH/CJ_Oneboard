// @file    OSPControl.c
// @brief   Independent Logo and Strobe OSP Manchester control

#include "OSPControl.h"
#include "Verification.h"
#include "power_device.h"
#include "sys_init.h"
#include "device.h"
#include <stddef.h>
#include <string.h>
__ALIGNED(16) dmac_descriptor_registers_t s_dmaDescriptor[OSP_DMA_CHANNEL_COUNT];
__ALIGNED(16) dmac_descriptor_registers_t s_dmaWriteback[OSP_DMA_CHANNEL_COUNT];
uint8_t s_txBytes[OSP_MAX_TX_BYTES];
uint16_t s_capturePeriods[OSP_CAPTURE_COUNT];

OSP_Control_t gOSPControl;

static void OSP_GpioOutput(uint8_t pin, uint8_t high)
{
    uint32_t mask = (1UL << pin);

    PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_PINCFG[pin] &= (uint8_t)(~PORT_PINCFG_PMUXEN_Msk);
    PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_PINCFG[pin] &= (uint8_t)(~PORT_PINCFG_INEN_Msk);
    PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_DIRSET = mask;
    if (high != 0U)
    {
        PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_OUTSET = mask;
    }
    else
    {
        PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_OUTCLR = mask;
    }
}

static void OSP_GpioPmux(uint8_t pin, uint8_t function, uint8_t inputEnable)
{
    uint8_t index = (uint8_t)(pin >> 1U);
    uint8_t value = PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_PMUX[index];

    if ((pin & 1U) == 0U)
    {
        value = (uint8_t)((value & 0xF0U) | (function & 0x0FU));
    }
    else
    {
        value = (uint8_t)((value & 0x0FU) | ((function & 0x0FU) << 4U));
    }
    PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_PMUX[index] = value;
    PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_PINCFG[pin] |= (uint8_t)PORT_PINCFG_PMUXEN_Msk;
    if (inputEnable != 0U)
    {
        PORT_REGS->GROUP[OSP_PIN_GROUP].PORT_PINCFG[pin] |= (uint8_t)PORT_PINCFG_INEN_Msk;
    }
}

static void OSP_EnableGclk(uint8_t peripheralId)
{
    while (GCLK_REGS->GCLK_SYNCBUSY != 0U)
    {
        __NOP();
    }
    GCLK_REGS->GCLK_PCHCTRL[peripheralId] = GCLK_PCHCTRL_GEN(1U) | GCLK_PCHCTRL_CHEN_Msk;
    while (GCLK_REGS->GCLK_SYNCBUSY != 0U)
    {
        __NOP();
    }
}

static void OSP_DelayMs(uint16_t milliseconds)
{
    volatile uint32_t loops;

    while (milliseconds > 0U)
    {
        for (loops = 0U; loops < 4000U; loops++)
        {
            __NOP();
        }
        milliseconds--;
    }
}

static void OSP_DelayUs(uint16_t microseconds)
{
    uint32_t previous = SysTick->VAL;
    uint32_t elapsed = 0U;
    uint32_t current;
    uint32_t reload = SysTick->LOAD + 1U;
    uint32_t required = (reload * microseconds) / LED_CONTROL_TICK_US;

    while (elapsed < required)
    {
        current = SysTick->VAL;
        elapsed += (previous >= current) ? (previous - current) : (previous + reload - current);
        previous = current;
    }
}

static void OSP_StopDmaChannel(uint8_t channel)
{
    DMAC_REGS->DMAC_CHID = channel;
    DMAC_REGS->DMAC_CHCTRLA = 0U;
    DMAC_REGS->DMAC_CHINTFLAG = DMAC_CHINTFLAG_Msk;
}

static void OSP_InitTxSerial(sercom_registers_t *serial)
{
    serial->SPIM.SERCOM_CTRLA = SERCOM_SPIM_CTRLA_SWRST_Msk;
    while (serial->SPIM.SERCOM_SYNCBUSY != 0U)
    {
        __NOP();
    }
    // Only PAD2/MOSI is routed to TX. SCK and SS are not connected.
    // Each SPI bit is one Manchester half-bit: 48 MHz / 10 = 4.8 Mbps.
    serial->SPIM.SERCOM_CTRLA = SERCOM_SPIM_CTRLA_MODE_SPI_MASTER | SERCOM_SPIM_CTRLA_DOPO_0x1;
    serial->SPIM.SERCOM_CTRLB = 0U;
    serial->SPIM.SERCOM_BAUD = 4U;
    while (serial->SPIM.SERCOM_SYNCBUSY != 0U)
    {
        __NOP();
    }
}

static void OSP_InitHardware(void)
{
    uint32_t config;
    uint32_t retry = 0U;

    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_GCLK_Msk;
    GCLK_REGS->GCLK_GENCTRL[1U] = GCLK_GENCTRL_SRC_FDPLL96M | GCLK_GENCTRL_DIV(1U) | GCLK_GENCTRL_IDC_Msk | GCLK_GENCTRL_GENEN_Msk;
    while ((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL1_Msk) != 0U)
    {
        __NOP();
    }
    MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_DMAC_Msk;
    DMAC_REGS->DMAC_CTRL = DMAC_CTRL_SWRST_Msk;
    while ((DMAC_REGS->DMAC_CTRL & DMAC_CTRL_SWRST_Msk) != 0U)
    {
        retry++;
        if (retry >= 1000U)
        {
            break;
        }
    }
    (void)memset(s_dmaDescriptor, 0, sizeof(s_dmaDescriptor));
    (void)memset(s_dmaWriteback, 0, sizeof(s_dmaWriteback));
    DMAC_REGS->DMAC_BASEADDR = (uint32_t)s_dmaDescriptor;
    DMAC_REGS->DMAC_WRBADDR = (uint32_t)s_dmaWriteback;
    DMAC_REGS->DMAC_PRICTRL0 = DMAC_PRICTRL0_LVLPRI0(1U) | DMAC_PRICTRL0_RRLVLEN0_Msk;
    DMAC_REGS->DMAC_QOSCTRL = DMAC_QOSCTRL_WRBQOS_HIGH | DMAC_QOSCTRL_FQOS_HIGH | DMAC_QOSCTRL_DQOS_HIGH;
    DMAC_REGS->DMAC_CTRL = (uint16_t)(DMAC_CTRL_DMAENABLE_Msk | DMAC_CTRL_LVLEN0_Msk | DMAC_CTRL_LVLEN1_Msk);
    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;
    MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM1_Msk | MCLK_APBCMASK_SERCOM3_Msk | MCLK_APBCMASK_EVSYS_Msk | MCLK_APBCMASK_TC2_Msk;
    OSP_EnableGclk(EIC_GCLK_ID);
    OSP_EnableGclk(TC2_GCLK_ID);
    OSP_EnableGclk(SERCOM1_GCLK_ID_CORE);
    OSP_EnableGclk(SERCOM3_GCLK_ID_CORE);
    OSP_EnableGclk(SERCOM1_GCLK_ID_SLOW);
    OSP_InitTxSerial(SERCOM3_REGS);
    OSP_InitTxSerial(SERCOM1_REGS);
    EIC_REGS->EIC_CTRLA = 0U;
    while (EIC_REGS->EIC_SYNCBUSY != 0U)
    {
        __NOP();
    }
    config = EIC_REGS->EIC_CONFIG0;
    config &= (uint32_t)(~(EIC_CONFIG0_SENSE0_Msk | EIC_CONFIG0_SENSE6_Msk));
    config |= EIC_CONFIG0_SENSE0_LOW | EIC_CONFIG0_SENSE6_LOW;
    EIC_REGS->EIC_CONFIG0 = config;
    EIC_REGS->EIC_ASYNCH |= EIC_ASYNCH_ASYNCH0_Msk | EIC_ASYNCH_ASYNCH6_Msk;
    EIC_REGS->EIC_EVCTRL |= EIC_EVCTRL_EXTINTEO0_Msk | EIC_EVCTRL_EXTINTEO6_Msk;
    EIC_REGS->EIC_CTRLA = EIC_CTRLA_ENABLE_Msk;
    while (EIC_REGS->EIC_SYNCBUSY != 0U)
    {
        __NOP();
    }
    TC2_REGS->COUNT16.TC_CTRLA = TC_CTRLA_SWRST_Msk;
    while (TC2_REGS->COUNT16.TC_SYNCBUSY != 0U)
    {
        __NOP();
    }
    // Capture only falling-to-falling periods.
    // Manchester transitions are recovered from 2/3/4 half-bit intervals.
    TC2_REGS->COUNT16.TC_CTRLA = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_CAPTEN1_Msk;
    TC2_REGS->COUNT16.TC_EVCTRL = TC_EVCTRL_EVACT_PWP | TC_EVCTRL_TCEI_Msk;
}

static void OSP_StartCapture(OSP_Port_t port)
{
    // Always start from a new DMA descriptor.
    OSP_StopDmaChannel(OSP_RX_DMA_CHANNEL);
    DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_SWRST_Msk;
    while ((DMAC_REGS->DMAC_CHCTRLA & DMAC_CHCTRLA_SWRST_Msk) != 0U)
    {
        __NOP();
    }
    (void)TC2_REGS->COUNT16.TC_CC[1U];
    TC2_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_Msk;
    (void)memset(s_capturePeriods, 0, sizeof(s_capturePeriods));
    (void)memset(&s_dmaWriteback[OSP_RX_DMA_CHANNEL], 0, sizeof(s_dmaWriteback[OSP_RX_DMA_CHANNEL]));

    EVSYS_REGS->EVSYS_CHANNEL[0U] = EVSYS_CHANNEL_EVGEN((port == OSP_PORT_LOGO) ? EVENT_ID_GEN_EIC_EXTINT_6 : EVENT_ID_GEN_EIC_EXTINT_0) | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
    EVSYS_REGS->EVSYS_USER[EVENT_ID_USER_TC2_EVU] = EVSYS_USER_CHANNEL(1U);
    s_dmaDescriptor[OSP_RX_DMA_CHANNEL].DMAC_BTCTRL = DMAC_BTCTRL_VALID_Msk | DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_DSTINC_Msk;
    s_dmaDescriptor[OSP_RX_DMA_CHANNEL].DMAC_BTCNT = OSP_CAPTURE_COUNT;
    s_dmaDescriptor[OSP_RX_DMA_CHANNEL].DMAC_SRCADDR = (uint32_t)&TC2_REGS->COUNT16.TC_CC[1U];
    s_dmaDescriptor[OSP_RX_DMA_CHANNEL].DMAC_DSTADDR = (uint32_t)&s_capturePeriods[OSP_CAPTURE_COUNT];
    s_dmaDescriptor[OSP_RX_DMA_CHANNEL].DMAC_DESCADDR = 0U;
    s_dmaWriteback[OSP_RX_DMA_CHANNEL].DMAC_BTCNT = OSP_CAPTURE_COUNT;
    __DMB();

    DMAC_REGS->DMAC_CHID = OSP_RX_DMA_CHANNEL;
    DMAC_REGS->DMAC_CHCTRLB = DMAC_CHCTRLB_TRIGSRC_TC2_MC1 | DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_LVL(1U);
    DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE_Msk;

    TC2_REGS->COUNT16.TC_COUNT = 0U;
    TC2_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_Msk;
    TC2_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;
    while (TC2_REGS->COUNT16.TC_SYNCBUSY != 0U)
    {
        __NOP();
    }
    TC2_REGS->COUNT16.TC_CTRLBSET = TC_CTRLBSET_CMD_RETRIGGER;
}

static uint16_t OSP_StopCapture(void)
{
    uint16_t captureCount;
    uint32_t retry;

    TC2_REGS->COUNT16.TC_CTRLA &= (uint32_t)(~TC_CTRLA_ENABLE_Msk);
    while (TC2_REGS->COUNT16.TC_SYNCBUSY != 0U)
    {
        __NOP();
    }
    DMAC_REGS->DMAC_CHID = OSP_RX_DMA_CHANNEL;
    retry = 0U;
    if ((DMAC_REGS->DMAC_CHCTRLA & DMAC_CHCTRLA_ENABLE_Msk) != 0U)
    {
        DMAC_REGS->DMAC_CHCTRLB |= DMAC_CHCTRLB_CMD_SUSPEND;
        for (retry = 0U; retry < 1000U; retry++)
        {
            if ((DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_SUSP_Msk) != 0U)
            {
                break;
            }
        }
    }
    __DMB();
    captureCount = (uint16_t)(OSP_CAPTURE_COUNT - s_dmaWriteback[OSP_RX_DMA_CHANNEL].DMAC_BTCNT);
    OSP_StopDmaChannel(OSP_RX_DMA_CHANNEL);
    if ((retry >= 1000U) || (captureCount > OSP_CAPTURE_COUNT) || ((TC2_REGS->COUNT16.TC_INTFLAG & TC_INTFLAG_ERR_Msk) != 0U))
    {
        captureCount = 0U;
    }
    return captureCount;
}

static void OSP_EncodeManchester(const uint8_t *raw, uint8_t rawLength)
{
    static const uint8_t encodedNibble[16U] =
    {
        0xAAU, 0xA9U, 0xA6U, 0xA5U, 0x9AU, 0x99U, 0x96U, 0x95U,
        0x6AU, 0x69U, 0x66U, 0x65U, 0x5AU, 0x59U, 0x56U, 0x55U
    };
    uint8_t index;

    s_txBytes[0U] = 0xFFU;
    s_txBytes[1U] = 0xFFU;
    for (index = 0U; index < rawLength; index++)
    {
        s_txBytes[2U + (index * 2U)] = encodedNibble[raw[index] >> 4U];
        s_txBytes[3U + (index * 2U)] = encodedNibble[raw[index] & 0x0FU];
    }
    s_txBytes[2U + (rawLength * 2U)] = 0xFFU;
    s_txBytes[3U + (rawLength * 2U)] = 0xFFU;
}

static uint8_t OSP_SendManchester(OSP_Port_t port, uint16_t byteCount)
{
    sercom_registers_t *serial;
    uint8_t trigger;
    uint8_t pin;
    uint8_t valid;
    uint32_t retry = 0U;

    if (port == OSP_PORT_LOGO)
    {
        serial = SERCOM3_REGS;
        trigger = SERCOM3_DMAC_ID_TX;
        pin = OSP_LOGO_TX_PIN;
    }
    else
    {
        serial = SERCOM1_REGS;
        trigger = SERCOM1_DMAC_ID_TX;
        pin = OSP_STROBE_TX_PIN;
    }

    // Hardware shifts eight Manchester half-bits per DMA byte.
    s_dmaDescriptor[OSP_TX_DMA_CHANNEL].DMAC_BTCTRL = DMAC_BTCTRL_VALID_Msk | DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_SRCINC_Msk;
    s_dmaDescriptor[OSP_TX_DMA_CHANNEL].DMAC_BTCNT = byteCount;
    s_dmaDescriptor[OSP_TX_DMA_CHANNEL].DMAC_SRCADDR = (uint32_t)&s_txBytes[byteCount];
    s_dmaDescriptor[OSP_TX_DMA_CHANNEL].DMAC_DSTADDR = (uint32_t)&serial->SPIM.SERCOM_DATA;
    s_dmaDescriptor[OSP_TX_DMA_CHANNEL].DMAC_DESCADDR = 0U;

    DMAC_REGS->DMAC_CHID = OSP_TX_DMA_CHANNEL;
    DMAC_REGS->DMAC_CHCTRLB = DMAC_CHCTRLB_TRIGSRC(trigger) | DMAC_CHCTRLB_TRIGACT_BEAT;
    DMAC_REGS->DMAC_CHINTFLAG = DMAC_CHINTFLAG_Msk;
    serial->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE_Msk;
    while (serial->SPIM.SERCOM_SYNCBUSY != 0U)
    {
        __NOP();
    }
    serial->SPIM.SERCOM_INTFLAG = SERCOM_SPIM_INTFLAG_TXC_Msk;
    OSP_GpioPmux(pin, 2U, 0U);
    DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE_Msk;
    while ((DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_TCMPL_Msk) == 0U)
    {
        // Leave APB bus slots for RX capture DMA.
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        retry++;
        if (retry >= OSP_WAIT_LIMIT)
        {
            break;
        }
    }

    valid = (retry < OSP_WAIT_LIMIT) ? 1U : 0U;
    retry = 0U;
    while ((serial->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_TXC_Msk) == 0U)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        retry++;
        if (retry >= 1000U)
        {
            break;
        }
    }
    if (retry >= 1000U)
    {
        valid = 0U;
    }
    OSP_StopDmaChannel(OSP_TX_DMA_CHANNEL);
    OSP_GpioOutput(pin, 1U);
    serial->SPIM.SERCOM_CTRLA &= (uint32_t)(~SERCOM_SPIM_CTRLA_ENABLE_Msk);
    while (serial->SPIM.SERCOM_SYNCBUSY != 0U)
    {
        __NOP();
    }
    // End-to-start gap for 24 nodes, including opposite address parity.
    OSP_DelayUs(15U);
    return valid;
}

static uint8_t OSP_CalculateCrc(const uint8_t *data, uint8_t length)
{
    static const uint8_t crcNibble[16U] =
    {
        0x00U, 0x2FU, 0x5EU, 0x71U, 0xBCU, 0x93U, 0xE2U, 0xCDU,
        0x57U, 0x78U, 0x09U, 0x26U, 0xEBU, 0xC4U, 0xB5U, 0x9AU
    };
    uint8_t crc = 0U;
    uint8_t index;

    for (index = 0U; index < length; index++)
    {
        crc ^= data[index];
        crc = (uint8_t)((crc << 4U) ^ crcNibble[crc >> 4U]);
        crc = (uint8_t)((crc << 4U) ^ crcNibble[crc >> 4U]);
    }
    return crc;
}

static uint16_t OSP_BuildCapturedLevels(uint16_t captureCount, uint8_t *levels)
{
    uint16_t capture;
    uint16_t start = 0U;
    uint16_t levelCount = 2U;
    uint16_t units;
    uint8_t bit = 1U;
    uint8_t bitCount;

    if (captureCount == 0U)
    {
        return 0U;
    }
    // The last idle interval starts the response. Skip the request echo.
    for (capture = (uint16_t)(captureCount - 1U); capture > 0U; capture--)
    {
        if ((s_capturePeriods[capture] < 15U) || (s_capturePeriods[capture] >= 45U))
        {
            start = capture;
            break;
        }
    }
    levels[0U] = 0U;
    levels[1U] = 1U;
    for (capture = (uint16_t)(start + 1U); capture < captureCount; capture++)
    {
        if ((levelCount + 4U) > OSP_MAX_RX_LEVELS)
        {
            return 0U;
        }
        // Classify half-bit intervals without a software divide on Cortex-M0+.
        units = 0U;
        if ((s_capturePeriods[capture] >= 15U) && (s_capturePeriods[capture] < 25U))
        {
            units = 2U;
        }
        else if ((s_capturePeriods[capture] >= 25U) && (s_capturePeriods[capture] < 35U))
        {
            units = 3U;
        }
        else if ((s_capturePeriods[capture] >= 35U) && (s_capturePeriods[capture] < 45U))
        {
            units = 4U;
        }
        bitCount = 1U;
        if (units == 3U)
        {
            bit ^= 1U;
            bitCount = (bit != 0U) ? 2U : 1U;
        }
        else if ((units == 4U) && (bit == 0U))
        {
            levels[levelCount] = 0U;
            levels[levelCount + 1U] = 1U;
            levelCount = (uint16_t)(levelCount + 2U);
        }
        else if (units != 2U)
        {
            // Idle or invalid interval. The next falling edge starts a new frame.
            levelCount = 0U;
            bit = 1U;
        }
        while (bitCount > 0U)
        {
            levels[levelCount] = (bit != 0U) ? 0U : 1U;
            levels[levelCount + 1U] = (bit != 0U) ? 1U : 0U;
            levelCount = (uint16_t)(levelCount + 2U);
            bitCount--;
        }
    }
    if ((bit == 0U) && ((levelCount + 2U) <= OSP_MAX_RX_LEVELS))
    {
        // A final 0-to-1 transition has no additional falling edge.
        levels[levelCount] = 0U;
        levels[levelCount + 1U] = 1U;
        levelCount = (uint16_t)(levelCount + 2U);
    }
    return levelCount;
}

static uint8_t OSP_DecodeManchester(const uint8_t *levels, uint16_t levelCount, uint8_t *raw, uint8_t rawLength, uint16_t address, uint8_t command)
{
    uint8_t header[3U];
    uint8_t headerLevels[2U][48U];
    uint16_t requiredLevels = (uint16_t)rawLength * 16U;
    uint16_t offset;
    uint16_t bit;
    uint8_t invert;
    uint8_t first;
    uint8_t second;
    uint8_t valid;
    uint8_t byteIndex;
    uint8_t psi = (rawLength == OSP_RESPONSE_OTP) ? 7U : (uint8_t)(rawLength - 4U);

    if (levelCount < requiredLevels)
    {
        return 0U;
    }
    header[0U] = (uint8_t)(0xA0U | ((address >> 6U) & 0x0FU));
    header[1U] = (uint8_t)(((address & 0x3FU) << 2U) | (psi >> 1U));
    header[2U] = (uint8_t)(((psi & 1U) << 7U) | command);
    for (bit = 0U; bit < 24U; bit++)
    {
        first = (uint8_t)(1U ^ ((header[bit / 8U] >> (7U - (bit % 8U))) & 1U));
        headerLevels[0U][bit * 2U] = first;
        headerLevels[0U][(bit * 2U) + 1U] = (uint8_t)(first ^ 1U);
        headerLevels[1U][bit * 2U] = (uint8_t)(first ^ 1U);
        headerLevels[1U][(bit * 2U) + 1U] = first;
    }
    for (offset = 0U; offset <= (uint16_t)(levelCount - requiredLevels); offset++)
    {
        invert = levels[offset];
        if (memcmp(&levels[offset], headerLevels[invert], 48U) != 0)
        {
            continue;
        }
        // Header, address, PSI and command matched. Decode payload and CRC.
        (void)memset(raw, 0, rawLength);
        (void)memcpy(raw, header, sizeof(header));
        valid = 1U;
        for (bit = 24U; bit < ((uint16_t)rawLength * 8U); bit++)
        {
            first = (uint8_t)(levels[offset + (bit * 2U)] ^ invert);
            second = (uint8_t)(levels[offset + (bit * 2U) + 1U] ^ invert);
            if (first == second)
            {
                valid = 0U;
                break;
            }
            byteIndex = (uint8_t)(bit / 8U);
            if (first == 0U)
            {
                raw[byteIndex] |= (uint8_t)(1U << (7U - (bit % 8U)));
            }
        }
        if ((valid != 0U) && (OSP_CalculateCrc(raw, (uint8_t)(rawLength - 1U)) == raw[rawLength - 1U]))
        {
            return 1U;
        }
    }
    return 0U;
}

static uint8_t OSP_Transfer(OSP_Port_t port, uint16_t address, uint8_t command, const uint8_t *payload, uint8_t payloadLength, uint8_t *response, uint8_t responseLength, uint16_t timeoutUs)
{
    uint8_t raw[OSP_MAX_RAW_BYTES];
    uint8_t levels[OSP_MAX_RX_LEVELS];
    uint8_t rawLength;
    uint8_t index;
    uint16_t captureCount;
    uint16_t levelCount;
    uint32_t wait;
    uint32_t previousTick;
    uint32_t currentTick;
    uint32_t elapsedTicks;
    uint32_t timeoutTicks;
    uint32_t reloadTicks;

    if ((payloadLength > 6U) || (responseLength > OSP_MAX_RAW_BYTES))
    {
        return 0U;
    }

    raw[0U] = (uint8_t)(0xA0U | ((address >> 6U) & 0x0FU));
    raw[1U] = (uint8_t)(((address & 0x3FU) << 2U) | ((payloadLength >> 1U) & 0x03U));
    raw[2U] = (uint8_t)(((payloadLength & 1U) << 7U) | (command & 0x7FU));
    for (index = 0U; index < payloadLength; index++)
    {
        raw[3U + index] = payload[index];
    }
    rawLength = (uint8_t)(payloadLength + 4U);
    raw[rawLength - 1U] = OSP_CalculateCrc(raw, (uint8_t)(rawLength - 1U));
    OSP_EncodeManchester(raw, rawLength);

    if (responseLength != 0U)
    {
        OSP_StartCapture(port);
    }
    if (OSP_SendManchester(port, ((uint16_t)rawLength * 2U) + 4U) == 0U)
    {
        if (responseLength != 0U)
        {
            (void)OSP_StopCapture();
        }
        gOSPControl.communicationErrorCount[(uint8_t)port]++;
        return 0U;
    }
    if (responseLength == 0U)
    {
        return 1U;
    }

    reloadTicks = SysTick->LOAD + 1U;
    timeoutTicks = (reloadTicks * timeoutUs) / LED_CONTROL_TICK_US;
    previousTick = SysTick->VAL;
    elapsedTicks = 0U;
    for (wait = 0U; wait < OSP_WAIT_LIMIT; wait++)
    {
        currentTick = SysTick->VAL;
        elapsedTicks += (previousTick >= currentTick) ? (previousTick - currentTick) : (previousTick + reloadTicks - currentTick);
        previousTick = currentTick;
        if (elapsedTicks >= timeoutTicks)
        {
            break;
        }
    }
    captureCount = OSP_StopCapture();
    levelCount = OSP_BuildCapturedLevels(captureCount, levels);
    if (OSP_DecodeManchester(levels, levelCount, response, responseLength, address, command) != 0U)
    {
        return 1U;
    }
    gOSPControl.manchesterErrorCount[(uint8_t)port]++;
    gOSPControl.communicationErrorCount[(uint8_t)port]++;
    return 0U;
}

static uint16_t OSP_Interpolate(uint16_t start, uint16_t target, uint16_t elapsed, uint16_t duration)
{
    int32_t delta;

    if ((duration == 0U) || (elapsed >= duration))
    {
        return target;
    }
    delta = (int32_t)target - (int32_t)start;
    return (uint16_t)((int32_t)start + ((delta * (int32_t)elapsed) / (int32_t)duration));
}

void OSPControl_Set(const LedControlCommand_t *command)
{
    uint8_t port;
    uint16_t onTimeMs;

    if (command == NULL)
    {
        return;
    }

    for (port = 0U; port < OSP_PORT_COUNT; port++)
    {
        if ((command->ledMask & (uint8_t)(1U << (port + 4U))) == 0U)
        {
            continue;
        }

        // A cyclic LIN command must not restart Fade/Flicker unless Sync is set.
        if ((command->sync == 0U) && (gOSPControl.mode[port] == command->mode) && (gOSPControl.active[port] == command->active) && (gOSPControl.fadeEnabled[port] == command->fadeEnabled) && (gOSPControl.effect1[port] == command->effect1) && (gOSPControl.effect2[port] == command->effect2) && (gOSPControl.targetRed[port] == command->red) && (gOSPControl.targetGreen[port] == command->green) && (gOSPControl.targetBlue[port] == command->blue))
        {
            continue;
        }

        gOSPControl.startRed[port] = gOSPControl.fadedRed[port];
        gOSPControl.startGreen[port] = gOSPControl.fadedGreen[port];
        gOSPControl.startBlue[port] = gOSPControl.fadedBlue[port];
        gOSPControl.targetRed[port] = command->red;
        gOSPControl.targetGreen[port] = command->green;
        gOSPControl.targetBlue[port] = command->blue;
        gOSPControl.mode[port] = command->mode;
        gOSPControl.active[port] = command->active;
        gOSPControl.fadeEnabled[port] = command->fadeEnabled;
        gOSPControl.effect1[port] = command->effect1;
        gOSPControl.effect2[port] = command->effect2;
        gOSPControl.startTimeMs[port] = command->timeMs;
        gOSPControl.phaseTimeMs[port] = command->timeMs;
        if ((command->mode != 6U) || (command->effect2 == 0U))
        {
            gOSPControl.onThreshold[port] = 0U;
        }
        else if (command->effect1 >= 250U)
        {
            gOSPControl.onThreshold[port] = 1000U;
        }
        else
        {
            onTimeMs = (uint16_t)((((uint32_t)command->effect1 * 1000U) + ((uint32_t)command->effect2 * 125U)) / ((uint32_t)command->effect2 * 250U));
            gOSPControl.onThreshold[port] = (uint16_t)(onTimeMs * command->effect2);
        }
        gOSPControl.flickerPhase[port] = 0U;
        if ((command->fadeEnabled != 0U) && (command->mode == 7U))
        {
            gOSPControl.fadeInMs[port] = (uint16_t)((uint16_t)command->effect1 * 100U);
            gOSPControl.fadeOutMs[port] = (uint16_t)((uint16_t)command->effect2 * 100U);
        }
        if (command->fadeEnabled != 0U)
        {
            gOSPControl.durationMs[port] = (command->active != 0U) ? gOSPControl.fadeInMs[port] : gOSPControl.fadeOutMs[port];
        }
        else
        {
            gOSPControl.durationMs[port] = 0U;
        }
        gOSPControl.dirty[port] = 1U;
    }
}

static void OSP_WriteColor(OSP_Port_t port, uint16_t red, uint16_t green, uint16_t blue)
{
    uint8_t payload[6U];
    uint32_t pwmSum;
    uint32_t pwmLimit;

    gOSPControl.pwmRed[port] = (uint16_t)(((uint32_t)red * OSP_PWM_MAX) / 65535U);
    gOSPControl.pwmGreen[port] = (uint16_t)(((uint32_t)green * OSP_PWM_MAX) / 65535U);
    gOSPControl.pwmBlue[port] = (uint16_t)(((uint32_t)blue * OSP_PWM_MAX) / 65535U);
    if ((port == OSP_PORT_LOGO) && (gVerification.ActiveFlag == 0U))
    {
        pwmSum = (uint32_t)gOSPControl.pwmRed[port] + gOSPControl.pwmGreen[port] + gOSPControl.pwmBlue[port];
        pwmLimit = ((uint32_t)OSP_PWM_MAX * OSP_LOGO_CURRENT_LIMIT_MA) / 50U;
        if (pwmSum > pwmLimit)
        {
            gOSPControl.pwmRed[port] = (uint16_t)(((uint32_t)gOSPControl.pwmRed[port] * pwmLimit) / pwmSum);
            gOSPControl.pwmGreen[port] = (uint16_t)(((uint32_t)gOSPControl.pwmGreen[port] * pwmLimit) / pwmSum);
            gOSPControl.pwmBlue[port] = (uint16_t)(((uint32_t)gOSPControl.pwmBlue[port] * pwmLimit) / pwmSum);
        }
    }
    payload[0U] = (uint8_t)(OSP_CURRENT_BIT | (gOSPControl.pwmRed[port] >> 8U));
    payload[1U] = (uint8_t)gOSPControl.pwmRed[port];
    payload[2U] = (uint8_t)(OSP_CURRENT_BIT | (gOSPControl.pwmGreen[port] >> 8U));
    payload[3U] = (uint8_t)gOSPControl.pwmGreen[port];
    payload[4U] = (uint8_t)(OSP_CURRENT_BIT | (gOSPControl.pwmBlue[port] >> 8U));
    payload[5U] = (uint8_t)gOSPControl.pwmBlue[port];
    (void)OSP_Transfer(port, OSP_ADDR_BROADCAST, OSP_CMD_SET_PWM, payload, 6U, NULL, 0U, 0U);
    gOSPControl.dirty[port] = 0U;
}

static void OSP_ReadStatic(OSP_Port_t port)
{
    static const uint8_t otpAddress[3U] = { 0x18U, 0x12U, 0x0AU };
    uint8_t response[OSP_MAX_RAW_BYTES];
    uint8_t node;
    uint8_t block;
    uint8_t index;
    uint8_t address;
    uint8_t destination;
    uint8_t readValid;
    uint8_t attempt;
    uint8_t attempts;

    for (node = 0U; node < gOSPControl.nodeCount[(uint8_t)port]; node++)
    {
        attempts = (node == 0U) ? OSP_PRESENCE_ATTEMPTS : 1U;
        readValid = 0U;
        for (attempt = 0U; attempt < attempts; attempt++)
        {
            readValid = OSP_Transfer(port, (uint16_t)(node + 1U), OSP_CMD_READ_SETUP, NULL, 0U, response, OSP_RESPONSE_ONE_BYTE, 1000U);
            if (readValid != 0U)
            {
                break;
            }
        }
        if (readValid == 0U)
        {
            // Do not read OTP or poll LEDs which were not detected.
            gOSPControl.nodeCount[(uint8_t)port] = node;
            return;
        }
        gOSPControl.setup[(uint8_t)port][node] = response[3U];
        for (block = 0U; block < 3U; block++)
        {
            address = otpAddress[block];
            readValid = OSP_Transfer(port, (uint16_t)(node + 1U), OSP_CMD_READ_OTP, &address, 1U, response, OSP_RESPONSE_OTP, 1000U);
            if (readValid == 0U)
            {
                // Keep unread OTP bytes at their initialized value.
            }
            else
            {
                for (index = 0U; index < 8U; index++)
                {
                    destination = (uint8_t)(address + 7U - index);
                    if ((destination >= 0x0AU) && (destination <= 0x1FU))
                    {
                        gOSPControl.otp[(uint8_t)port][node][destination - 0x0AU] = response[3U + index];
                    }
                }
            }
        }
    }
}

static void OSP_InitPort(OSP_Port_t port)
{
    uint8_t setup = OSP_SETUP_DEFAULT;

    (void)OSP_Transfer(port, OSP_ADDR_BROADCAST, OSP_CMD_RESET, NULL, 0U, NULL, 0U, 0U);
    OSP_DelayMs(2U);
    (void)OSP_Transfer(port, OSP_ADDR_BROADCAST, OSP_CMD_SET_SETUP, &setup, 1U, NULL, 0U, 0U);
    (void)OSP_Transfer(port, OSP_ADDR_FIRST, OSP_CMD_INIT_BIDIR, NULL, 0U, NULL, 0U, 0U);
    OSP_DelayMs(20U);
    (void)OSP_Transfer(port, OSP_ADDR_BROADCAST, OSP_CMD_GO_ACTIVE, NULL, 0U, NULL, 0U, 0U);
    OSP_ReadStatic(port);
}

void OSPControl_Init(void)
{
    uint8_t port;
    uint8_t node;

    (void)memset(&gOSPControl, 0, sizeof(gOSPControl));
    for (port = 0U; port < OSP_PORT_COUNT; port++)
    {
        gOSPControl.fadeInMs[port] = LED_FLICKER_FADE_TIME_MS;
        gOSPControl.fadeOutMs[port] = LED_FLICKER_FADE_TIME_MS;
    }
    gOSPControl.nodeCount[OSP_PORT_LOGO] = OSP_LOGO_NODE_COUNT;
    gOSPControl.nodeCount[OSP_PORT_STROBE] = OSP_STROBE_NODE_COUNT;

    OSP_GpioOutput(OSP_LOGO_STBY_PIN, 0U);
    OSP_GpioOutput(OSP_STROBE_STBY_PIN, 0U);
    OSP_GpioOutput(OSP_LOGO_TX_PIN, 1U);
    OSP_GpioOutput(OSP_STROBE_TX_PIN, 1U);
    OSP_GpioPmux(OSP_LOGO_RX_PIN, OSP_RX_PMUX, 1U);
    OSP_GpioPmux(OSP_STROBE_RX_PIN, OSP_RX_PMUX, 1U);

    OSP_InitHardware();
    // Let LED power settle before RESET, as in the original project.
    OSP_DelayMs(5U);
    OSP_InitPort(OSP_PORT_LOGO);
    OSP_InitPort(OSP_PORT_STROBE);

    // Read all dynamic data once before LIN starts. Periodic calls read one node.
    for (port = 0U; port < OSP_PORT_COUNT; port++)
    {
        for (node = 0U; node < gOSPControl.nodeCount[port]; node++)
        {
            OSPControl_Read((OSP_Port_t)port);
        }
    }
}

void OSPControl_Process(void)
{
    uint8_t port;
    uint32_t nowMs;
    uint32_t phaseTicks;
    uint32_t phase;
    uint32_t elapsedMs;
    uint16_t fadeElapsedMs;
    uint16_t red;
    uint16_t green;
    uint16_t blue;

    nowMs = oAppTimer.timeMs;
    for (port = 0U; port < OSP_PORT_COUNT; port++)
    {
        if (gOSPControl.mode[port] != 6U)
        {
            continue;
        }

        // LIN D7 is Hz. D6 sets the ON threshold in 10% steps.
        phaseTicks = nowMs - gOSPControl.phaseTimeMs[port];
        gOSPControl.phaseTimeMs[port] = nowMs;
        if (phaseTicks >= 1000U)
        {
            phaseTicks %= 1000U;
        }
        phase = gOSPControl.flickerPhase[port] + (phaseTicks * gOSPControl.effect2[port]);
        if (phase >= 2000U)
        {
            phase %= 1000U;
        }
        else if (phase >= 1000U)
        {
            phase -= 1000U;
        }
        gOSPControl.flickerPhase[port] = (uint16_t)phase;
        red = gOSPControl.fadedRed[port];
        green = gOSPControl.fadedGreen[port];
        blue = gOSPControl.fadedBlue[port];
        if ((gOSPControl.effect2[port] == 0U) || (gOSPControl.flickerPhase[port] >= gOSPControl.onThreshold[port]))
        {
            red = 0U;
            green = 0U;
            blue = 0U;
        }

        if ((red != gOSPControl.outputRed[port]) || (green != gOSPControl.outputGreen[port]) || (blue != gOSPControl.outputBlue[port]) || (gOSPControl.dirty[port] != 0U))
        {
            gOSPControl.outputRed[port] = red;
            gOSPControl.outputGreen[port] = green;
            gOSPControl.outputBlue[port] = blue;
            gOSPControl.dirty[port] = 1U;
        }
    }

    for (port = 0U; port < OSP_PORT_COUNT; port++)
    {
        if (gOSPControl.dirty[port] != 0U)
        {
            OSP_WriteColor((OSP_Port_t)port, gOSPControl.outputRed[port], gOSPControl.outputGreen[port], gOSPControl.outputBlue[port]);
        }
    }

    if (oAppTimer.tick10ms < 10U)
    {
        return;
    }

    for (port = 0U; port < OSP_PORT_COUNT; port++)
    {
        elapsedMs = oAppTimer.timeMs - gOSPControl.startTimeMs[port];
        fadeElapsedMs = (elapsedMs >= gOSPControl.durationMs[port]) ? gOSPControl.durationMs[port] : (uint16_t)elapsedMs;
        gOSPControl.fadedRed[port] = OSP_Interpolate(gOSPControl.startRed[port], gOSPControl.targetRed[port], fadeElapsedMs, gOSPControl.durationMs[port]);
        gOSPControl.fadedGreen[port] = OSP_Interpolate(gOSPControl.startGreen[port], gOSPControl.targetGreen[port], fadeElapsedMs, gOSPControl.durationMs[port]);
        gOSPControl.fadedBlue[port] = OSP_Interpolate(gOSPControl.startBlue[port], gOSPControl.targetBlue[port], fadeElapsedMs, gOSPControl.durationMs[port]);

        if (gOSPControl.mode[port] != 6U)
        {
            if ((gOSPControl.fadedRed[port] != gOSPControl.outputRed[port]) || (gOSPControl.fadedGreen[port] != gOSPControl.outputGreen[port]) || (gOSPControl.fadedBlue[port] != gOSPControl.outputBlue[port]) || (gOSPControl.dirty[port] != 0U))
            {
                gOSPControl.outputRed[port] = gOSPControl.fadedRed[port];
                gOSPControl.outputGreen[port] = gOSPControl.fadedGreen[port];
                gOSPControl.outputBlue[port] = gOSPControl.fadedBlue[port];
                gOSPControl.dirty[port] = 1U;
            }
        }
    }
}

void OSPControl_Read(OSP_Port_t port)
{
    static const uint8_t commands[3U] =
    {
        OSP_CMD_READ_TEMP_STATUS, OSP_CMD_READ_PWM,
        OSP_CMD_READ_LED_STATUS
    };
    uint8_t response[OSP_MAX_RAW_BYTES];
    uint8_t node;
    uint8_t item;
    uint8_t responseLength;
    uint8_t valid;

    if ((uint8_t)port >= OSP_PORT_COUNT)
    {
        return;
    }
    if (gOSPControl.nodeCount[port] == 0U)
    {
        return;
    }

    node = gOSPControl.readNode[port];
    if (node >= gOSPControl.nodeCount[port])
    {
        node = 0U;
    }
    gOSPControl.dynamicValid[port][node] = 1U;
    for (item = 0U; item < 3U; item++)
    {
        responseLength = (item == 0U) ? OSP_RESPONSE_TEMP_STATUS : ((item == 1U) ? OSP_RESPONSE_PWM : OSP_RESPONSE_ONE_BYTE);
        valid = OSP_Transfer(port, (uint16_t)(node + 1U), commands[item], NULL, 0U, response, responseLength, (uint16_t)((node + 1U) * 16U + 40U));
        if (valid == 0U)
        {
            gOSPControl.dynamicValid[port][node] = 0U;
        }
        else
        {
            switch (item)
            {
                case 0U:
                    // TEMPSTAT is sent as TEMP MSB and STATUS LSB.
                    gOSPControl.temperatureC[port][node] = (int16_t)((((int32_t)response[3U] * 1072) / 1000) - 126);
                    gOSPControl.status[port][node] = response[4U];
                    break;
                case 1U:
                    gOSPControl.rxPwmRed[port][node] = (uint16_t)(((uint16_t)(response[3U] & 0x7FU) << 8U) | response[4U]);
                    gOSPControl.rxPwmGreen[port][node] = (uint16_t)(((uint16_t)(response[5U] & 0x7FU) << 8U) | response[6U]);
                    gOSPControl.rxPwmBlue[port][node] = (uint16_t)(((uint16_t)(response[7U] & 0x7FU) << 8U) | response[8U]);
                    break;
                case 2U:
                    gOSPControl.ledStatus[port][node] = response[3U];
                    break;
                default:
                    break;
            }
        }
    }
    node++;
    gOSPControl.readNode[port] = (node >= gOSPControl.nodeCount[port]) ? 0U : node;
}

void OSPControl_CheckLogoOpen(void)
{
    uint8_t node;

    if ((gOSPControl.nodeCount[OSP_PORT_LOGO] == OSP_LOGO_NODE_COUNT) && ((gOSPControl.ledStatus[OSP_PORT_LOGO][OSP_LOGO_FIRST_NODE] & OSP_LED_OPEN_MASK) == 0U) && (gOSPControl.dynamicValid[OSP_PORT_LOGO][OSP_LOGO_FIRST_NODE] != 0U) && ((gOSPControl.status[OSP_PORT_LOGO][OSP_LOGO_FIRST_NODE] & OSP_STATUS_COMM_ERROR) == 0U) && ((gOSPControl.ledStatus[OSP_PORT_LOGO][OSP_LOGO_SECOND_NODE] & OSP_LED_OPEN_MASK) == 0U) && (gOSPControl.dynamicValid[OSP_PORT_LOGO][OSP_LOGO_SECOND_NODE] != 0U) && ((gOSPControl.status[OSP_PORT_LOGO][OSP_LOGO_SECOND_NODE] & OSP_STATUS_COMM_ERROR) == 0U))
    {
        return;
    }

    gOSPControl.nodeCount[OSP_PORT_LOGO] = OSP_LOGO_NODE_COUNT;
    gOSPControl.readNode[OSP_PORT_LOGO] = 0U;
    for (node = 0U; node < OSP_LOGO_NODE_COUNT; node++)
    {
        gOSPControl.status[OSP_PORT_LOGO][node] = 0U;
        gOSPControl.ledStatus[OSP_PORT_LOGO][node] = 0U;
        gOSPControl.dynamicValid[OSP_PORT_LOGO][node] = 0U;
    }
    OSP_InitPort(OSP_PORT_LOGO);
    for (node = 0U; node < gOSPControl.nodeCount[OSP_PORT_LOGO]; node++)
    {
        gOSPControl.dynamicValid[OSP_PORT_LOGO][node] = 1U;
    }
    OSP_WriteColor(OSP_PORT_LOGO, gOSPControl.outputRed[OSP_PORT_LOGO], gOSPControl.outputGreen[OSP_PORT_LOGO], gOSPControl.outputBlue[OSP_PORT_LOGO]);
}

void OSPControl_CheckStrobeOpen(void)
{
    uint8_t node;

    if ((gOSPControl.nodeCount[OSP_PORT_STROBE] == OSP_STROBE_NODE_COUNT) && ((gOSPControl.ledStatus[OSP_PORT_STROBE][0U] & OSP_LED_OPEN_MASK) == 0U) && (gOSPControl.dynamicValid[OSP_PORT_STROBE][0U] != 0U) && ((gOSPControl.status[OSP_PORT_STROBE][0U] & OSP_STATUS_COMM_ERROR) == 0U))
    {
        return;
    }

    gOSPControl.nodeCount[OSP_PORT_STROBE] = OSP_STROBE_NODE_COUNT;
    gOSPControl.readNode[OSP_PORT_STROBE] = 0U;
    for (node = 0U; node < OSP_STROBE_NODE_COUNT; node++)
    {
        gOSPControl.status[OSP_PORT_STROBE][node] = 0U;
        gOSPControl.ledStatus[OSP_PORT_STROBE][node] = 0U;
        gOSPControl.dynamicValid[OSP_PORT_STROBE][node] = 0U;
    }
    OSP_InitPort(OSP_PORT_STROBE);
    for (node = 0U; node < gOSPControl.nodeCount[OSP_PORT_STROBE]; node++)
    {
        gOSPControl.dynamicValid[OSP_PORT_STROBE][node] = 1U;
    }
    OSP_WriteColor(OSP_PORT_STROBE, gOSPControl.outputRed[OSP_PORT_STROBE], gOSPControl.outputGreen[OSP_PORT_STROBE], gOSPControl.outputBlue[OSP_PORT_STROBE]);
}

uint8_t OSPControl_GetFaultMask(OSP_FaultPart_t part)
{
    uint8_t port;
    uint8_t node;
    uint8_t result = 0U;

    if (part == OSP_FAULT_PART_LOGO)
    {
        port = OSP_PORT_LOGO;
    }
    else if (part == OSP_FAULT_PART_STROBE)
    {
        port = OSP_PORT_STROBE;
    }
    else
    {
        return 0U;
    }

    if (gPowerData.voltageFault != 0U)
    {
        result |= 0x02U;
    }
    if (((port == OSP_PORT_LOGO) && (gOSPControl.nodeCount[port] != OSP_LOGO_NODE_COUNT)) || ((port == OSP_PORT_STROBE) && (gOSPControl.nodeCount[port] != OSP_STROBE_NODE_COUNT)))
    {
        result |= 0x10U;
    }
    for (node = 0U; node < gOSPControl.nodeCount[port]; node++)
    {
        if (((gOSPControl.status[port][node] & OSP_STATUS_OVER_TEMP) != 0U) || (gOSPControl.temperatureC[port][node] > OSP_TEMPERATURE_LIMIT_C))
        {
            result |= 0x01U;
        }
        if ((gOSPControl.status[port][node] & OSP_STATUS_UNDER_VOLT) != 0U)
        {
            result |= 0x02U;
        }
        if ((gOSPControl.ledStatus[port][node] & OSP_LED_SHORT_MASK) != 0U)
        {
            result |= 0x04U;
        }
        if ((gOSPControl.ledStatus[port][node] & OSP_LED_OPEN_MASK) != 0U)
        {
            result |= 0x08U;
        }
        if ((gOSPControl.dynamicValid[port][node] == 0U) || ((gOSPControl.status[port][node] & OSP_STATUS_COMM_ERROR) != 0U))
        {
            result |= 0x10U;
        }
    }
    return result;
}
