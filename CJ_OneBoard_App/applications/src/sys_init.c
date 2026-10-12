// @file    sys_init.c
// @brief   System initialisation for CJ 4DPLEX Logo Lamp
//
// @copyright 2026 CJ 4DPLEX

#include "appConfig.h"

AppTimer_t oAppTimer = { 0U, 0U, 0U, 30U, 60U, 90U, 0U, 0U };

void AppTimer_TickHandler(void)
{
    oAppTimer.timeMs++;
    oAppTimer.tick1ms++;
    oAppTimer.tick10ms++;
    oAppTimer.tickWdtMs++;
    oAppTimer.tick100ms++;
    oAppTimer.tick100ms_1++;
    oAppTimer.tick100ms_2++;
    oAppTimer.tick2s++;
}

void AppTimer_ResetReadCount(void)
{
    uint32_t irqState = __get_PRIMASK();
    __disable_irq();
    oAppTimer.tick100ms = 30U;
    oAppTimer.tick100ms_1 = 60U;
    oAppTimer.tick100ms_2 = 90U;
    oAppTimer.tick2s = 0U;
    __set_PRIMASK(irqState);
}

// ===========================================================
//  WDT 초기화 (내부 전용)
// ===========================================================
#if WDT_ENABLE == 1U
static void WDT_Init(void)
{
    uint32_t retry = 0U;

    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_WDT_Msk;
    WDT_REGS->WDT_CONFIG = WDT_PERIOD;
    WDT_REGS->WDT_CTRLA  = WDT_CTRLA_ENABLE_Msk;
    while ((WDT_REGS->WDT_SYNCBUSY & WDT_SYNCBUSY_ENABLE_Msk) != 0U)
    {
        __NOP();
        if (++retry >= 1000U)
        {
            break;
        }
    }
    WDT_Clear();
}

void WDT_Clear(void)
{
    uint32_t retry = 0U;
    WDT_REGS->WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
    while ((WDT_REGS->WDT_SYNCBUSY & WDT_SYNCBUSY_CLEAR_Msk) != 0U)
    {
        __NOP();
        if (++retry >= 1000U)
        {
            break;
        }
    }
}
#else
static void WDT_Init(void) { }// WDT disabled 
void WDT_Clear(void) { }
#endif

// ===========================================================
//  WDT 클리어 (main() while(1) 에서 주기적으로 호출)
// ===========================================================

// ===========================================================
//  SYS_Init
// ===========================================================

void SYS_Init(void)
{
    Clock_Init();
    SysTick_Init(SYSTEM_CLOCK_HZ, AppTimer_TickHandler);
    Power_Init();

    // Load the board Chair ID before LIN is initialised.
    PDS_Init();

    OSPControl_Init();
    SRGBControl_Init();
    // Initialise CJ control, status and node-management frames from PDS.
    LINT_Init();

    // Start the watchdog only after all blocking initialisation is complete.
    WDT_Init();
    __disable_irq();
    oAppTimer.tick1ms = 0U;
    oAppTimer.tick10ms = 0U;
    AppTimer_ResetReadCount();
    __enable_irq();
    Reprogram_Init();
}
