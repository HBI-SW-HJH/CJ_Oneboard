// @file    flash_device.h
// @brief   PIC32CM JH01 NVMCTRL flash driver
//
//  PIC32CM5164 Data Flash is defined by the selected DFP:
//    0x00400000..0x00401FFF, page 64B, erase row 256B.
//
// @copyright 2026 CJ 4DPLEX

#ifndef FLASH_DEVICE_H
#define FLASH_DEVICE_H

#include "device.h"
#include <stdint.h>

#define DATAFLASH_BASE_ADDR     NVMCTRL_DATAFLASH_ADDR
#define DATAFLASH_ROW_SIZE      (NVMCTRL_PAGE_SIZE * 4U)
#define NVMCTRL_CMDEX_KEY       0xA500U
#define NVM_CMD_ER              NVMCTRL_CTRLA_CMD_ER_Val
#define NVM_CMD_WP              NVMCTRL_CTRLA_CMD_WP_Val
#define NVM_CMD_DFER            NVMCTRL_CTRLA_CMD_DFER_Val
#define NVM_CMD_DFWP            NVMCTRL_CTRLA_CMD_DFWP_Val
#define NVM_CMD_PBC             NVMCTRL_CTRLA_CMD_PBC_Val

void Flash_Read(uint32_t addr, void *data, uint32_t size);
uint8_t Flash_WriteRow(uint32_t addr, const void *data, uint32_t size);

#endif // FLASH_DEVICE_H
