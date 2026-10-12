// @file    appConfig.h
// @brief   CJ 4DPLEX Seat Logo Lamp - project-wide configuration
//          Target: PIC32CM5164JH01032-E/PT
//
// @copyright 2026 CJ 4DPLEX

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define BEGIN_PACK  _Pragma("pack(push, 1)")
#define END_PACK    _Pragma("pack(pop)")

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "device.h"

// ===========================================================
//  Product identity
//  Append new fields; do not reorder existing fields.
// ===========================================================
#define CONFIG_SERIAL_NUMBER       "26J07-0001"
#define CONFIG_PART_NUMBER         "ZX1-0025"
#define CONFIG_VERSION             0x0001U
#define CONFIG_CHAIR_NUMBER        0x0AU
#define CONFIG_CHAIR_MIN           0x0AU
#define CONFIG_CHAIR_MAX           0x0DU
#define CONFIG_SERIAL_LENGTH       10U
#define CONFIG_PART_LENGTH         8U
#define CONFIG_DEFAULTS            { CONFIG_VERSION, CONFIG_SERIAL_NUMBER, CONFIG_PART_NUMBER, CONFIG_CHAIR_NUMBER, 0UL }

#define CONFIG_UID_WORD0           (*(const volatile uint32_t *)0x0080A00CUL)
#define CONFIG_UID_WORD1           (*(const volatile uint32_t *)0x0080A040UL)
#define CONFIG_UID_WORD2           (*(const volatile uint32_t *)0x0080A044UL)
#define CONFIG_UID_WORD3           (*(const volatile uint32_t *)0x0080A048UL)

// Fixed byte layout prevents old tail padding from overwriting appended defaults.
#pragma pack(push, 1)
typedef struct ConFig
{
    uint16_t version;                 // Four nibbles: major.minor.patch.build (0..15).
    char Serial_Number[CONFIG_SERIAL_LENGTH + 1U];
    char Part_Number[CONFIG_PART_LENGTH + 1U];
    uint8_t Chair_Number;             // 0x0A..0x0D; NAD = Chair_Number - 9.
    uint32_t Init_Number;             // Permanent board key; never used as Chair ID/NAD.
    // Append fields here. New fields default to zero unless added to CONFIG_DEFAULTS.
    // PDS reserves the rest of the existing 256-byte row automatically.
} ConFig_t;
#pragma pack(pop)

// ===========================================================
//  CJ LIN protocol
// ===========================================================
#define LIN_BAUD                19200UL
#define LIN_FRAME_DATA_LEN      8U
#define CJ_CONTROL_FRAME_ID     0x01U
#define CJ_ID_CHANGE_OFFSET     0x10U
#define CJ_DIAG_REQUEST_ID      0x3CU
#define CJ_DIAG_RESPONSE_ID     0x3DU
#define CJ_DEFAULT_CHAIR_ID     ((uint8_t)(CONFIG_CHAIR_NUMBER - 0x09U))
#define TRUE                    1U

#define FALSE                   0U

// ===========================================================
//  PIC32CM5164JH01032 Data Flash PDS layout
//  Data Flash: 0x00400000..0x00401FFF (8KB), row: 256B
//  Main Flash (0x00000000..0x0007FFFF) is reserved for application code.
// ===========================================================
#define FLASH_PDS_ROW_SIZE          (NVMCTRL_PAGE_SIZE * 4UL)
#define FLASH_PDS_SYSTEM_BACK_ADDR  (NVMCTRL_DATAFLASH_ADDR + (1UL * FLASH_PDS_ROW_SIZE))
#define FLASH_PDS_SYSTEM_ADDR       (NVMCTRL_DATAFLASH_ADDR + (16UL * FLASH_PDS_ROW_SIZE))

#if ((FLASH_PDS_SYSTEM_ADDR + FLASH_PDS_ROW_SIZE) > \
     (NVMCTRL_DATAFLASH_ADDR + NVMCTRL_DATAFLASH_SIZE))
    #error "PDS layout exceeds PIC32CM5164 Data Flash"
#endif

// ===========================================================
//  Watchdog
// ===========================================================
#define WDT_ENABLE              1U

// WDT clock is 1.024 kHz. 1024 cycles gives a 1 second timeout.
#define WDT_PERIOD              WDT_CONFIG_PER_CYC1024
#define WDT_CLEAR_MS            100U

// ===========================================================
//  Debug
// ===========================================================
#define CODE_DEBUG_EN           0U
#if CODE_DEBUG_EN == 1U
    #include <stdio.h>
    #define DEBUG_OUT(...)  printf(__VA_ARGS__)
#else
    #define DEBUG_OUT(...)
#endif

// ===========================================================
//  Driver Headers
// ===========================================================
#include "clock_device.h"
#include "gpio_device.h"
#include "lin_device.h"
#include "power_device.h"
#include "systick_device.h"
#include "flash_device.h"

// ===========================================================
//  Application Headers
// ===========================================================
#include "SRGBControl.h"
#include "OSPControl.h"
#include "crc32.h"
#include "Reprogram.h"
#include "UDS.h"
#include "Verification.h"
#include "XCP.h"
#include "pds.h"
#include "lin_task.h"
#include "sys_init.h"

#endif // APP_CONFIG_H

//V0.0.0.1 - 20261012 1차 납품 15대/ 신뢰성 시험 보드 3대(White 최대 출력) 
