// @file    systick_device.h
// @brief   SysTick (1ms tick) driver for PIC32CM5164JH01032-E/PT
//
// @copyright 2026 CJ 4DPLEX

#ifndef SYSTICK_DEVICE_H
#define SYSTICK_DEVICE_H

#include <stdint.h>
#include "device.h"

typedef void (*SysTick_Callback_t)(void);

extern SysTick_Callback_t s_tickCb;

// @brief  Initialize SysTick for 1ms interrupt.
// @param  cpuHz   CPU clock frequency in Hz (e.g., 16000000)
// @param  cb      Callback function invoked every 1ms from ISR.
void SysTick_Init(uint32_t cpuHz, SysTick_Callback_t cb);

#endif //  SYSTICK_DEVICE_H
