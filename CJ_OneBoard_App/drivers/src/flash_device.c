// @file    flash_device.c
// @brief   PIC32CM JH01 NVMCTRL flash driver
//
// @copyright 2026 CJ 4DPLEX

#include "appConfig.h"

static uint8_t Flash_IsDataFlashAddr(uint32_t addr)
{
    return (uint8_t)((addr >= NVMCTRL_DATAFLASH_ADDR) && (addr < (NVMCTRL_DATAFLASH_ADDR + NVMCTRL_DATAFLASH_SIZE)));
}

static void Flash_WaitReady(void)
{
    uint32_t retry = 0U;
    while ((NVMCTRL_REGS->NVMCTRL_INTFLAG & NVMCTRL_INTFLAG_READY_Msk) == 0U)
    {
        __NOP();
        if (++retry >= 1000U)
        {
            break;
        }
    }
}

static void Flash_ExecCmd(uint8_t cmd)
{
    Flash_WaitReady();
    NVMCTRL_REGS->NVMCTRL_CTRLA = (uint16_t)(NVMCTRL_CMDEX_KEY | (uint16_t)cmd);
    Flash_WaitReady();
}

static void Flash_EraseRow(uint32_t addr)
{
    NVMCTRL_REGS->NVMCTRL_ADDR = (uint32_t)(addr >> 1U);
    Flash_ExecCmd((Flash_IsDataFlashAddr(addr) != 0U) ? NVM_CMD_DFER : NVM_CMD_ER);
}

void Flash_Read(uint32_t addr, void *data, uint32_t size)
{
    (void)memcpy(data, (const void *)addr, size);
}

static void Flash_WriteWord(uint32_t addr, uint32_t data)
{
    Flash_WaitReady();
    *((volatile uint32_t *)addr) = data;
}

static void Flash_ClearPageBuffer(void)
{
    Flash_ExecCmd(NVM_CMD_PBC);
}

static void Flash_WritePage(uint32_t addr)
{
    NVMCTRL_REGS->NVMCTRL_ADDR = (uint32_t)(addr >> 1U);
    Flash_ExecCmd((Flash_IsDataFlashAddr(addr) != 0U) ? NVM_CMD_DFWP : NVM_CMD_WP);
}

uint8_t Flash_WriteRow(uint32_t addr, const void *data, uint32_t size)
{
    const uint32_t *writeData = (const uint32_t *)data;
    const volatile uint32_t *readData = (const volatile uint32_t *)addr;
    uint32_t page;
    uint32_t word;

    if ((data == NULL) || (size != DATAFLASH_ROW_SIZE) || ((addr % DATAFLASH_ROW_SIZE) != 0U))
    {
        return 0U;
    }

    Flash_EraseRow(addr);
    for (page = 0U; page < 4U; page++)
    {
        Flash_ClearPageBuffer();
        for (word = 0U; word < (NVMCTRL_PAGE_SIZE / 4U); word++)
        {
            Flash_WriteWord(addr + (page * NVMCTRL_PAGE_SIZE) + (word * 4U), writeData[(page * (NVMCTRL_PAGE_SIZE / 4U)) + word]);
        }
        Flash_WritePage(addr + (page * NVMCTRL_PAGE_SIZE));
    }

    for (word = 0U; word < (DATAFLASH_ROW_SIZE / 4U); word++)
    {
        if (readData[word] != writeData[word])
        {
            return 0U;
        }
    }
    return 1U;
}
