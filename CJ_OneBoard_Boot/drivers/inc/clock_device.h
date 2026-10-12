// @file    clock_device.h
// @brief   System clock configuration for PIC32CM5164JH01032-E/PT
//          Uses Microchip DFP register definitions.
//
//  Configuration
//  ─────────────
//  External 16 MHz crystal (DSX321G on XIN/XOUT = PA14/PA15)
//  XOSC / 8 -> FDPLL x 24 -> GCLK0 -> CPU/peripherals at 48 MHz
//
// @copyright 2026 CJ 4DPLEX

#ifndef CLOCK_DEVICE_H
#define CLOCK_DEVICE_H

#include "device.h"

//  CPU / peripheral clock frequency in Hz
#define SYSTEM_CLOCK_HZ     48000000UL
#define CLOCK_READY_WAIT_COUNT       100000UL
#define CLOCK_XOSC_RETRY_COUNT       3U
#define CLOCK_DPLL_XOSC_DIVIDER      3U
#define CLOCK_DPLL_RATIO             23U

// @brief  Start GCLK0 at 48 MHz using the external 16 MHz crystal and FDPLL.
void Clock_Init(void);

// @brief  Enable a GCLK peripheral channel sourced from GCLK0.
// @param  gclkId   GCLK PCHCTRL channel ID
//                  e.g. SERCOM1_GCLK_ID_CORE (19), SERCOM2_GCLK_ID_CORE (20)
void Clock_EnablePeripheral(uint8_t gclkId);

// @brief  Enable MCLK APBC clock for a SERCOM.
// @param  sercomIdx  0..5 for SERCOM0..5
void Clock_EnableSercomApbc(uint8_t sercomIdx);

#endif //  CLOCK_DEVICE_H
