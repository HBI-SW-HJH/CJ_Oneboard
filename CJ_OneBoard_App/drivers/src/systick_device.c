// @file    systick_device.c
// @brief   SysTick 1ms; application millisecond timers advance every tick
//
// @copyright 2026 CJ 4DPLEX

#include "systick_device.h"
#include <stddef.h>   //  NULL

SysTick_Callback_t s_tickCb = NULL;

void SysTick_Init(uint32_t cpuHz, SysTick_Callback_t cb)
{
    s_tickCb = cb;
    (void)SysTick_Config(cpuHz / 1000U);
}

void SysTick_Handler(void)
{
    if (s_tickCb != NULL)
    {
        s_tickCb();
    }
}
