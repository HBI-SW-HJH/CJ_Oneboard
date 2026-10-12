// @file    clock_device.c
// @brief   System clock driver - external 16 MHz crystal to 48 MHz FDPLL
//
//  Startup sequence
//  1. Start internal 48 MHz as a safe temporary clock
//  2. Enable XOSC: PA14=XIN, PA15=XOUT, crystal mode, 16 MHz gain
//  3. XOSC 16 MHz / 8 = 2 MHz DPLL reference
//  4. FDPLL 2 MHz x 24 = 48 MHz
//  5. Switch GCLK0 to FDPLL96M
//
// @copyright 2026 CJ 4DPLEX

#include "clock_device.h"

//  ---- GCLK synchronisation helpers ----

//  Wait for Generator 0 write sync (SYNCBUSY.GENCTRL0 = bit 2)
static void Clock_GclkGen0Sync(void)
{
    while ((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL0_Msk) != 0U)
    {
    }
}

//  Wait for PCHCTRL write sync
static void Clock_GclkSync(void)
{
    while (GCLK_REGS->GCLK_SYNCBUSY != 0U)
    {
    }
}

void Clock_Init(void)
{
    uint32_t nvmControl;
    uint32_t waitCount;
    uint8_t retry;

    //  1. Configure two Flash wait states BEFORE switching to 48 MHz.
    nvmControl = NVMCTRL_REGS->NVMCTRL_CTRLB;
    nvmControl &= (uint32_t)(~(NVMCTRL_CTRLB_RWS_Msk | NVMCTRL_CTRLB_READMODE_Msk | NVMCTRL_CTRLB_CACHEDIS_Msk));
    nvmControl |= NVMCTRL_CTRLB_RWS(2U) | NVMCTRL_CTRLB_READMODE_NO_MISS_PENALTY | NVMCTRL_CTRLB_CACHEDIS_CACHE_DF_DIS_MAIN_EN;
    NVMCTRL_REGS->NVMCTRL_CTRLB = nvmControl;

    //  2. Enable GCLK APB-A clock
    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_GCLK_Msk;

    //  3. Use OSC48M temporarily so a failed crystal does not stop booting.
    OSCCTRL_REGS->OSCCTRL_OSC48MCTRL = OSCCTRL_OSC48MCTRL_ENABLE_Msk;
    while ((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_OSC48MRDY_Msk) == 0U)
    {
    }
    OSCCTRL_REGS->OSCCTRL_OSC48MDIV = OSCCTRL_OSC48MDIV_DIV_DIV1;
    while ((OSCCTRL_REGS->OSCCTRL_OSC48MSYNCBUSY & OSCCTRL_OSC48MSYNCBUSY_OSC48MDIV_Msk) != 0U)
    {
    }

    Clock_GclkGen0Sync();
    GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_SRC_OSC48M | GCLK_GENCTRL_DIV(1U) | GCLK_GENCTRL_IDC_Msk | GCLK_GENCTRL_GENEN_Msk;
    Clock_GclkGen0Sync();

    //  4. Start the 16 MHz crystal. Retry only three times.
    for (retry = 0U; retry < CLOCK_XOSC_RETRY_COUNT; retry++)
    {
        OSCCTRL_REGS->OSCCTRL_XOSCCTRL = OSCCTRL_XOSCCTRL_ENABLE_Msk | OSCCTRL_XOSCCTRL_XTALEN_Msk | OSCCTRL_XOSCCTRL_GAIN_GAIN16 | OSCCTRL_XOSCCTRL_AMPGC_Msk | OSCCTRL_XOSCCTRL_STARTUP(7U);
        waitCount = 0U;
        while (((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY_Msk) == 0U) && (waitCount < CLOCK_READY_WAIT_COUNT))
        {
            waitCount++;
        }
        if ((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY_Msk) != 0U)
        {
            break;
        }
        OSCCTRL_REGS->OSCCTRL_XOSCCTRL = 0U;
    }
    if ((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY_Msk) == 0U)
    {
        return;
    }

    //  5. XOSC 16 MHz / (2 x (3 + 1)) = 2 MHz reference.
    //     2 MHz x (23 + 1) = 48 MHz FDPLL output.
    OSCCTRL_REGS->OSCCTRL_DPLLCTRLA = 0U;
    while ((OSCCTRL_REGS->OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk) != 0U)
    {
    }
    OSCCTRL_REGS->OSCCTRL_DPLLCTRLB = OSCCTRL_DPLLCTRLB_REFCLK_XOSC | OSCCTRL_DPLLCTRLB_DIV(CLOCK_DPLL_XOSC_DIVIDER) | OSCCTRL_DPLLCTRLB_FILTER_Default;
    OSCCTRL_REGS->OSCCTRL_DPLLRATIO = OSCCTRL_DPLLRATIO_LDR(CLOCK_DPLL_RATIO) | OSCCTRL_DPLLRATIO_LDRFRAC(0U);
    while ((OSCCTRL_REGS->OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk) != 0U)
    {
    }
    OSCCTRL_REGS->OSCCTRL_DPLLPRESC = OSCCTRL_DPLLPRESC_PRESC_DIV1;
    while ((OSCCTRL_REGS->OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_DPLLPRESC_Msk) != 0U)
    {
    }
    OSCCTRL_REGS->OSCCTRL_DPLLCTRLA = OSCCTRL_DPLLCTRLA_ENABLE_Msk;
    while ((OSCCTRL_REGS->OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk) != 0U)
    {
    }

    waitCount = 0U;
    while (((OSCCTRL_REGS->OSCCTRL_DPLLSTATUS & (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk)) != (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk)) && (waitCount < CLOCK_READY_WAIT_COUNT))
    {
        waitCount++;
    }
    if ((OSCCTRL_REGS->OSCCTRL_DPLLSTATUS & (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk)) != (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk))
    {
        return;
    }

    //  6. CPU and all GCLK0 peripherals now use crystal-derived 48 MHz.
    Clock_GclkGen0Sync();
    GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_SRC_FDPLL96M | GCLK_GENCTRL_DIV(1U) | GCLK_GENCTRL_IDC_Msk | GCLK_GENCTRL_GENEN_Msk;
    Clock_GclkGen0Sync();
}

void Clock_EnablePeripheral(uint8_t gclkId)
{
    Clock_GclkSync();
    GCLK_REGS->GCLK_PCHCTRL[gclkId] = GCLK_PCHCTRL_GEN(0U) | GCLK_PCHCTRL_CHEN_Msk;
    Clock_GclkSync();
}

void Clock_EnableSercomApbc(uint8_t sercomIdx)
{
    //  MCLK_APBCMASK: SERCOM0=bit1, SERCOM1=bit2, SERCOM2=bit3 ...
    static const uint32_t masks[6] =
    {
        MCLK_APBCMASK_SERCOM0_Msk,
        MCLK_APBCMASK_SERCOM1_Msk,
        MCLK_APBCMASK_SERCOM2_Msk,
        MCLK_APBCMASK_SERCOM3_Msk,
        MCLK_APBCMASK_SERCOM4_Msk,
        MCLK_APBCMASK_SERCOM5_Msk,
    };

    if (sercomIdx < 6U)
    {
        MCLK_REGS->MCLK_APBCMASK |= masks[sercomIdx];
    }
}
