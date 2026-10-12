// @file    appConfig.h
// @brief   CJ 4DPLEX Seat Logo Lamp - project-wide configuration
//          Target: PIC32CM5164JH01032-E/PT
//
// @copyright 2026 CJ 4DPLEX

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "device.h"

// ===========================================================
//  CPU identity (independent of App product PDS)
// ===========================================================
#define CONFIG_UID_WORD0           (*(const volatile uint32_t *)0x0080A00CUL)
#define CONFIG_UID_WORD1           (*(const volatile uint32_t *)0x0080A040UL)
#define CONFIG_UID_WORD2           (*(const volatile uint32_t *)0x0080A044UL)
#define CONFIG_UID_WORD3           (*(const volatile uint32_t *)0x0080A048UL)

// ===========================================================
//  CJ LIN protocol
// ===========================================================
#define LIN_BAUD                19200UL
#define LIN_FRAME_DATA_LEN      8U
#define CJ_DIAG_REQUEST_ID      0x3CU
#define CJ_DIAG_RESPONSE_ID     0x3DU
#define TRUE                    1U

#define FALSE                   0U

// ===========================================================
//  Watchdog
// ===========================================================
#define WDT_ENABLE              1U

// WDT clock is 1.024 kHz. 1024 cycles gives a 1 second timeout.
#define WDT_PERIOD              WDT_CONFIG_PER_CYC1024
#define WDT_CLEAR_MS            100U

// ===========================================================
//  Driver Headers
// ===========================================================
#include "clock_device.h"
#include "gpio_device.h"
#include "lin_device.h"
#include "systick_device.h"
#include "flash_device.h"

// ===========================================================
//  Application Headers
// ===========================================================
#include "crc32.h"
#include "Reprogram.h"
#include "UDS.h"
#include "lin_task.h"
#include "sys_init.h"

#endif // APP_CONFIG_H


