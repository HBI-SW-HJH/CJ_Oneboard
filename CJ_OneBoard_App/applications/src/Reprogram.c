#include "appConfig.h"

Reprogram_Record_t gUpdateMain;
Reprogram_Record_t gUpdateBackup;
Reprogram_t gReprogram;

static uint8_t Reprogram_Command(uint8_t command)
{
    uint32_t timeout = 1000000UL;
    while ((NVMCTRL_REGS->NVMCTRL_INTFLAG & NVMCTRL_INTFLAG_READY_Msk) == 0U)
    {
        if (--timeout == 0UL)
        {
            return 0U;
        }
    }
    NVMCTRL_REGS->NVMCTRL_STATUS = 0x001CU;
    NVMCTRL_REGS->NVMCTRL_INTFLAG = NVMCTRL_INTFLAG_ERROR_Msk;
    NVMCTRL_REGS->NVMCTRL_CTRLA = (uint16_t)(NVMCTRL_CMDEX_KEY | command);
    timeout = 1000000UL;
    while ((NVMCTRL_REGS->NVMCTRL_INTFLAG & NVMCTRL_INTFLAG_READY_Msk) == 0U)
    {
        if (--timeout == 0UL)
        {
            return 0U;
        }
    }
    return ((NVMCTRL_REGS->NVMCTRL_STATUS & 0x001CU) == 0U) ? 1U : 0U;
}

static uint8_t Reprogram_WriteRow(uint32_t address, const uint32_t *data)
{
    uint32_t irqState;
    uint32_t control;
    uint16_t offset;
    uint8_t result = 1U;
    if ((data == NULL) || ((address & 0xFFUL) != 0UL))
    {
        return 0U;
    }
    if ((address != REPROGRAM_MAIN_ADDR) && (address != REPROGRAM_BACKUP_ADDR))
    {
        return 0U;
    }
    WDT_Clear();
    irqState = __get_PRIMASK();
    __disable_irq();
    control = NVMCTRL_REGS->NVMCTRL_CTRLB;
    NVMCTRL_REGS->NVMCTRL_CTRLB = control | NVMCTRL_CTRLB_MANW_Msk;
    NVMCTRL_REGS->NVMCTRL_ADDR = address >> 1U;
    result = Reprogram_Command(NVM_CMD_DFER);
    for (offset = 0U; (offset < REPROGRAM_ROW_SIZE) && (result != 0U); offset += NVMCTRL_PAGE_SIZE)
    {
        result = Reprogram_Command(NVM_CMD_PBC);
        if (result == 0U)
        {
            break;
        }
        for (uint16_t word = 0U; word < NVMCTRL_PAGE_SIZE; word += 4U)
        {
            *(volatile uint32_t *)(address + offset + word) = data[(offset + word) / 4U];
        }
        NVMCTRL_REGS->NVMCTRL_ADDR = (address + offset) >> 1U;
        result = Reprogram_Command(NVM_CMD_DFWP);
    }
    if (Reprogram_Command(NVMCTRL_CTRLA_CMD_INVALL_Val) == 0U)
    {
        result = 0U;
    }
    NVMCTRL_REGS->NVMCTRL_CTRLB = control;
    __set_PRIMASK(irqState);
    if (memcmp((const void *)address, data, REPROGRAM_ROW_SIZE) != 0)
    {
        result = 0U;
    }
    WDT_Clear();
    return result;
}

static uint8_t Reprogram_Store(uint32_t flag)
{
    uint8_t retry;
    gUpdateMain.magic = REPROGRAM_MAGIC;
    gUpdateMain.index++;
    gUpdateMain.flag = flag;
    gUpdateMain.crc = CRC32_GetRunTimeCRC32((const uint8_t *)&gUpdateMain.magic, (uint16_t)(sizeof(gUpdateMain) - 4U));
    for (retry = 0U; retry < 3U; retry++)
    {
        if (Reprogram_WriteRow(REPROGRAM_BACKUP_ADDR, (const uint32_t *)&gUpdateMain) != 0U)
        {
            break;
        }
    }
    if (retry == 3U)
    {
        return 0U;
    }
    for (retry = 0U; retry < 3U; retry++)
    {
        if (Reprogram_WriteRow(REPROGRAM_MAIN_ADDR, (const uint32_t *)&gUpdateMain) != 0U)
        {
            break;
        }
    }
    Flash_Read(REPROGRAM_BACKUP_ADDR, &gUpdateBackup, sizeof(gUpdateBackup));
    return ((retry < 3U) && (memcmp(&gUpdateMain, &gUpdateBackup, sizeof(gUpdateMain)) == 0)) ? 1U : 0U;
}

static void Reprogram_Load(void)
{
    uint8_t mainValid;
    uint8_t backupValid;
    CRC32_Init();
    Flash_Read(REPROGRAM_MAIN_ADDR, &gUpdateMain, sizeof(gUpdateMain));
    Flash_Read(REPROGRAM_BACKUP_ADDR, &gUpdateBackup, sizeof(gUpdateBackup));
    mainValid = (uint8_t)((gUpdateMain.magic == REPROGRAM_MAGIC) && (gUpdateMain.crc == CRC32_GetRunTimeCRC32((const uint8_t *)&gUpdateMain.magic, (uint16_t)(sizeof(gUpdateMain) - 4U))));
    backupValid = (uint8_t)((gUpdateBackup.magic == REPROGRAM_MAGIC) && (gUpdateBackup.crc == CRC32_GetRunTimeCRC32((const uint8_t *)&gUpdateBackup.magic, (uint16_t)(sizeof(gUpdateBackup) - 4U))));
    if ((backupValid != 0U) && ((mainValid == 0U) || ((int32_t)(gUpdateBackup.index - gUpdateMain.index) > 0)))
    {
        gUpdateMain = gUpdateBackup;
    }
    else if (mainValid == 0U)
    {
        memset(&gUpdateMain, 0, sizeof(gUpdateMain));
        gUpdateMain.address = REPROGRAM_APP_START;
    }
    if (gUpdateMain.recoveryMagic != REPROGRAM_RECOVERY_MAGIC)
    {
        gUpdateMain.recoveryMagic = REPROGRAM_RECOVERY_MAGIC;
        gUpdateMain.recoveryLength = 0UL;
        gUpdateMain.recoveryCrc = 0UL;
        gUpdateMain.restoreCount = 0UL;
        gUpdateMain.appConfirmed = 0UL;
        gUpdateMain.recoveryUsed = 0UL;
    }
}

void Reprogram_Init(void)
{
    Reprogram_Load();
    gReprogram.bootTime = oAppTimer.timeMs;
    gReprogram.bootChecked = 0U;
}

void Reprogram_HardFault(void)
{
    __disable_irq();
    Reprogram_Load();
    gUpdateMain.appConfirmed = 0UL;
    if (gUpdateMain.faultCount < REPROGRAM_FAULT_LIMIT)
    {
        gUpdateMain.faultCount++;
    }
    if (gUpdateMain.faultCount >= REPROGRAM_FAULT_LIMIT)
    {
        Reprogram_Store(2UL);
    }
    else
    {
        Reprogram_Store(0UL);
    }
    NVIC_SystemReset();
}

void Reprogram_Process(void)
{
    if ((gReprogram.bootChecked == 0U) && (gReprogram.resetPending == 0U) && ((uint32_t)(oAppTimer.timeMs - gReprogram.bootTime) >= REPROGRAM_HEALTHY_TIME_MS))
    {
        gReprogram.bootChecked = 1U;
        Reprogram_Load();
        if ((gUpdateMain.faultCount != 0UL) || (gUpdateMain.magic != REPROGRAM_MAGIC) || (gUpdateMain.appConfirmed != 1UL))
        {
            gUpdateMain.faultCount = 0UL;
            gUpdateMain.appConfirmed = 1UL;
            Reprogram_Store(0UL);
        }
    }
    if ((gReprogram.selected != 0U) && ((uint32_t)(oAppTimer.timeMs - gReprogram.requestTime) >= 5000UL))
    {
        gReprogram.selected = 0U;
    }
    if ((gReprogram.resetPending == 0U) || (gReprogram.resetResponseStarted == 0U))
    {
        return;
    }
    // Allow the complete LIN response, including its checksum, to leave the UART.
    if (((uint32_t)(oAppTimer.timeMs - gReprogram.resetTime) >= 100UL) && (s_state == LIN_STATE_IDLE) && ((SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC_Msk) != 0U))
    {
        NVIC_SystemReset();
    }
}

uint8_t Reprogram_Request(const uint8_t *request, uint8_t length, uint8_t *response)
{
    uint8_t nrc = 0U;
    uint32_t address;
    if ((request == NULL) || (response == NULL) || (length == 0U))
    {
        return 0U;
    }
    if ((length == 8U) && (request[0U] == 0x31U) && (request[1U] == 0x01U) && (request[2U] == 0xF3U) && (request[3U] == 0x02U))
    {
        gReprogram.selected = 0U;
        address = ((uint32_t)request[4U] << 24U) | ((uint32_t)request[5U] << 16U) | ((uint32_t)request[6U] << 8U) | request[7U];
        if ((address == gConFig.Init_Number) && (address != 0UL) && (address != 0xFFFFFFFFUL))
        {
            gReprogram.selected = 1U;
            gReprogram.requestTime = oAppTimer.timeMs;
            memcpy(response, request, 8U);
            response[0U] = 0x71U;
            response[8U] = 0U;
            return 9U;
        }
        return 0U;
    }
    if (gReprogram.selected == 0U)
    {
        return 0U;
    }
    response[0U] = (uint8_t)(request[0U] + 0x40U);
    gReprogram.requestTime = oAppTimer.timeMs;
    switch (request[0U])
    {
        case 0x10U:
            if ((length != 2U) || (request[1U] != 0x02U))
            {
                nrc = 0x13U;
                break;
            }
            Reprogram_Load();
            if (gReprogram.bootChecked == 0U)
            {
                gUpdateMain.appConfirmed = 0UL;
            }
            if (gUpdateMain.appConfirmed == 1UL)
            {
                // Snapshot the running image, including an App installed directly with IAR.
                gUpdateMain.length = 0UL;
                gUpdateMain.imageCrc = 0UL;
            }
            if (Reprogram_Store(1UL) == 0U)
            {
                nrc = 0x72U;
                break;
            }
            gReprogram.resetPending = 1U;
            gReprogram.resetResponseStarted = 0U;
            response[1U] = 0x02U;
            response[2U] = 0U;
            response[3U] = 250U;
            response[4U] = 0x01U;
            response[5U] = 0xF4U;
            return 6U;

        case 0x22U:
            if ((length == 3U) && (request[1U] == 0xF2U) && (request[2U] == 0x10U))
            {
                response[1U] = 0xF2U;
                response[2U] = 0x10U;
                response[3U] = 0U;
                Flash_Read(REPROGRAM_MAIN_ADDR, &gUpdateMain, sizeof(gUpdateMain));
                Flash_Read(REPROGRAM_BACKUP_ADDR, &gUpdateBackup, sizeof(gUpdateBackup));
                for (uint8_t i = 0U; i < 4U; i++)
                {
                    response[4U + i] = (uint8_t)(REPROGRAM_APP_START >> ((3U - i) * 8U));
                    response[8U + i] = (uint8_t)(REPROGRAM_APP_SIZE >> ((3U - i) * 8U));
                    response[12U + i] = (uint8_t)(gUpdateMain.flag >> ((3U - i) * 8U));
                    response[16U + i] = (uint8_t)(gUpdateBackup.flag >> ((3U - i) * 8U));
                    response[20U + i] = (uint8_t)(gUpdateMain.faultCount >> ((3U - i) * 8U));
                    response[24U + i] = (uint8_t)(gUpdateBackup.faultCount >> ((3U - i) * 8U));
                    response[29U + i] = (uint8_t)(gUpdateMain.restoreCount >> ((3U - i) * 8U));
                    response[33U + i] = (uint8_t)(gUpdateMain.recoveryLength >> ((3U - i) * 8U));
                }
                // F210: mode, App address/size, Main/Backup flags, Main/Backup fault counts, CRC-valid mask.
                response[28U] = 0U;
                if ((gUpdateMain.magic == REPROGRAM_MAGIC) && (gUpdateMain.crc == CRC32_GetRunTimeCRC32((const uint8_t *)&gUpdateMain.magic, (uint16_t)(sizeof(gUpdateMain) - 4U))))
                {
                    response[28U] |= 1U;
                }
                if ((gUpdateBackup.magic == REPROGRAM_MAGIC) && (gUpdateBackup.crc == CRC32_GetRunTimeCRC32((const uint8_t *)&gUpdateBackup.magic, (uint16_t)(sizeof(gUpdateBackup) - 4U))))
                {
                    response[28U] |= 2U;
                }
                // Extension: restore count, backup length, format/backup/confirmed/used bits.
                response[37U] = 0U;
                if ((response[28U] == 3U) && (gUpdateMain.recoveryMagic == REPROGRAM_RECOVERY_MAGIC))
                {
                    response[37U] = 1U;
                    if ((gUpdateMain.recoveryLength >= 192UL) && (gUpdateMain.recoveryLength <= REPROGRAM_APP_SIZE))
                    {
                        response[37U] |= 2U;
                    }
                    if (gUpdateMain.appConfirmed == 1UL)
                    {
                        response[37U] |= 4U;
                    }
                    if (gUpdateMain.recoveryUsed != 0UL)
                    {
                        response[37U] |= 8U;
                    }
                }
                return 38U;
            }
            nrc = 0x31U;
            break;

        case 0x3EU:
            if ((length != 2U) || (request[1U] != 0U))
            {
                nrc = 0x13U;
                break;
            }
            response[1U] = 0U;
            return 2U;

        default:
            nrc = 0x11U;
            break;
    }
    response[0U] = 0x7FU;
    response[1U] = request[0U];
    response[2U] = nrc;
    return 3U;
}
