#include "D:/jhhan/program/CJ_OneBoard/Program/CJ_Boot_20260930/CJ_OneBoard_Boot_20260930/applications/inc/Reprogram.h"
#include "appConfig.h"

Reprogram_Record_t gUpdateMain;
Reprogram_Record_t gUpdateBackup;
Reprogram_t gReprogram;

uint32_t Reprogram_Crc(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFFUL;
    uint32_t index;
    if (data == NULL)
    {
        return 0UL;
    }
    for (index = 0UL; index < length; index++)
    {
        crc = gCRC32Table[(crc ^ data[index]) & 0xFFU] ^ (crc >> 8U);
    }
    return crc ^ 0xFFFFFFFFUL;
}

static uint8_t Reprogram_Command(uint8_t command) { return HostCommand(command); }

uint8_t Reprogram_WriteProtect_Lock(void)
{
    uint32_t address;
    if (NVMCTRL_REGS->NVMCTRL_LOCK == 0U)
    {
        return 1U;
    }
    for (address = REPROGRAM_PROTECT_START; address <= REPROGRAM_PROTECT_END; address += REPROGRAM_REGION_SIZE)
    {
        NVMCTRL_REGS->NVMCTRL_ADDR = address >> 1U;
        if (Reprogram_Command(NVMCTRL_CTRLA_CMD_LR_Val) == 0U)
        {
            return 0U;
        }
    }
    return (NVMCTRL_REGS->NVMCTRL_LOCK == 0U) ? 1U : 0U;
}

uint8_t Reprogram_WriteProtect_Unlock(void)
{
    uint32_t address;
    for (address = REPROGRAM_APP_START; address <= REPROGRAM_APP_END; address += REPROGRAM_REGION_SIZE)
    {
        NVMCTRL_REGS->NVMCTRL_ADDR = address >> 1U;
        if (Reprogram_Command(NVMCTRL_CTRLA_CMD_UR_Val) == 0U)
        {
            return 0U;
        }
    }
    return (NVMCTRL_REGS->NVMCTRL_LOCK == REPROGRAM_APP_UNLOCK_MASK) ? 1U : 0U;
}

static uint8_t Reprogram_WriteRow(uint32_t address, const uint32_t *data) { return HostWriteRow(address, data); }

static uint8_t Reprogram_Store(uint32_t flag)
{
    uint8_t retry;
    gUpdateMain.magic = REPROGRAM_MAGIC;
    gUpdateMain.index++;
    gUpdateMain.flag = flag;
    gUpdateMain.crc = Reprogram_Crc((const uint8_t *)&gUpdateMain.magic, sizeof(gUpdateMain) - 4U);
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

static void Reprogram_StartApp(uint32_t address) { HostJump(address); }

static uint8_t Reprogram_Load(void)
{
    uint8_t mainValid;
    uint8_t backupValid;
    Flash_Read(REPROGRAM_MAIN_ADDR, &gUpdateMain, sizeof(gUpdateMain));
    Flash_Read(REPROGRAM_BACKUP_ADDR, &gUpdateBackup, sizeof(gUpdateBackup));
    mainValid = (uint8_t)((gUpdateMain.magic == REPROGRAM_MAGIC) && (gUpdateMain.crc == Reprogram_Crc((const uint8_t *)&gUpdateMain.magic, sizeof(gUpdateMain) - 4U)));
    backupValid = (uint8_t)((gUpdateBackup.magic == REPROGRAM_MAGIC) && (gUpdateBackup.crc == Reprogram_Crc((const uint8_t *)&gUpdateBackup.magic, sizeof(gUpdateBackup) - 4U)));
    if ((mainValid == 0U) && (backupValid == 0U))
    {
        return 0U;
    }
    if ((backupValid != 0U) && ((mainValid == 0U) || ((int32_t)(gUpdateBackup.index - gUpdateMain.index) > 0)))
    {
        gUpdateMain = gUpdateBackup;
        mainValid = 0U;
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
    // Complete an interrupted two-row commit before changing either App image.
    if ((mainValid == 0U) || (backupValid == 0U) || (memcmp(&gUpdateMain, &gUpdateBackup, sizeof(gUpdateMain)) != 0))
    {
        return Reprogram_Store(gUpdateMain.flag);
    }
    return 1U;
}

static uint8_t Reprogram_Copy(uint32_t source, uint32_t destination, uint32_t length)
{
    uint32_t region;
    uint32_t offset;
    if ((length < 192UL) || (length > REPROGRAM_APP_SIZE))
    {
        return 0U;
    }
    if (!(((source == REPROGRAM_APP_START) && (destination == REPROGRAM_RECOVERY_START)) || ((source == REPROGRAM_RECOVERY_START) && (destination == REPROGRAM_APP_START))))
    {
        return 0U;
    }
    for (region = 0UL; region < length; region += REPROGRAM_REGION_SIZE)
    {
        NVMCTRL_REGS->NVMCTRL_ADDR = (destination + region) >> 1U;
        if (Reprogram_Command(NVMCTRL_CTRLA_CMD_UR_Val) == 0U)
        {
            Reprogram_WriteProtect_Lock();
            return 0U;
        }
        for (offset = region; (offset < (region + REPROGRAM_REGION_SIZE)) && (offset < length); offset += REPROGRAM_ROW_SIZE)
        {
            Flash_Read(source + offset, gReprogram.row, sizeof(gReprogram.row));
            if (Reprogram_WriteRow(destination + offset, gReprogram.row) == 0U)
            {
                Reprogram_WriteProtect_Lock();
                return 0U;
            }
        }
        NVMCTRL_REGS->NVMCTRL_ADDR = (destination + region) >> 1U;
        if (Reprogram_Command(NVMCTRL_CTRLA_CMD_LR_Val) == 0U)
        {
            Reprogram_WriteProtect_Lock();
            return 0U;
        }
    }
    return 1U;
}

static uint8_t Reprogram_ImageValid(void)
{
    uint32_t vectors[2U];
    if ((gUpdateMain.address != REPROGRAM_APP_START) || (gUpdateMain.length < 192UL) || (gUpdateMain.length > REPROGRAM_APP_SIZE))
    {
        return 0U;
    }
    Flash_Read(gUpdateMain.address, vectors, sizeof(vectors));
    if ((vectors[0U] <= DRAM_ADDR) || (vectors[0U] > (DRAM_ADDR + DRAM_SIZE)) || ((vectors[0U] & 7UL) != 0UL) || ((vectors[1U] & 1UL) == 0UL) || (vectors[1U] < gUpdateMain.address) || (vectors[1U] >= (gUpdateMain.address + gUpdateMain.length)))
    {
        return 0U;
    }
    return (Reprogram_Crc((const uint8_t *)gUpdateMain.address, gUpdateMain.length) == gUpdateMain.imageCrc) ? 1U : 0U;
}

static uint8_t Reprogram_BackupApp(void)
{
    uint32_t vectors[2U];
    if (gUpdateMain.appConfirmed != 1UL)
    {
        return 1U;
    }
    Flash_Read(REPROGRAM_APP_START, vectors, sizeof(vectors));
    if ((vectors[0U] <= DRAM_ADDR) || (vectors[0U] > (DRAM_ADDR + DRAM_SIZE)) || ((vectors[0U] & 7UL) != 0UL) || ((vectors[1U] & 1UL) == 0UL) || (vectors[1U] < REPROGRAM_APP_START) || (vectors[1U] > REPROGRAM_APP_END))
    {
        return 0U;
    }
    // A running-App snapshot has no BIN length; exclude trailing erased bytes.
    if (gUpdateMain.length == 0UL)
    {
        gUpdateMain.address = REPROGRAM_APP_START;
        gUpdateMain.length = REPROGRAM_APP_SIZE;
        while ((gUpdateMain.length > 192UL) && (*(const uint8_t *)(REPROGRAM_APP_START + gUpdateMain.length - 1UL) == 0xFFU))
        {
            gUpdateMain.length--;
        }
        gUpdateMain.imageCrc = Reprogram_Crc((const uint8_t *)REPROGRAM_APP_START, gUpdateMain.length);
    }
    if (Reprogram_ImageValid() == 0U)
    {
        return 0U;
    }
    gUpdateMain.recoveryLength = 0UL;
    if (Reprogram_Store(1UL) == 0U)
    {
        return 0U;
    }
    if (Reprogram_Copy(REPROGRAM_APP_START, REPROGRAM_RECOVERY_START, gUpdateMain.length) == 0U)
    {
        return 0U;
    }
    if (Reprogram_Crc((const uint8_t *)REPROGRAM_RECOVERY_START, gUpdateMain.length) != gUpdateMain.imageCrc)
    {
        return 0U;
    }
    gUpdateMain.recoveryLength = gUpdateMain.length;
    gUpdateMain.recoveryCrc = gUpdateMain.imageCrc;
    gUpdateMain.recoveryUsed = 0UL;
    return Reprogram_Store(1UL);
}

void Reprogram_Init(void)
{
    uint32_t vectors[2U];
    uint8_t stored;
    if (Reprogram_WriteProtect_Lock() == 0U)
    {
        return;
    }
    CRC32_Init();
    memset(&gReprogram, 0, sizeof(gReprogram));
    stored = Reprogram_Load();
    if (stored == 0U)
    {
        Flash_Read(REPROGRAM_MAIN_ADDR, &gUpdateMain, sizeof(gUpdateMain));
        Flash_Read(REPROGRAM_BACKUP_ADDR, &gUpdateBackup, sizeof(gUpdateBackup));
        if ((gUpdateMain.flag != 0xFFFFFFFFUL) || (gUpdateMain.magic != 0xFFFFFFFFUL) || (gUpdateBackup.flag != 0xFFFFFFFFUL) || (gUpdateBackup.magic != 0xFFFFFFFFUL))
        {
            return;
        }
    }
    if ((stored != 0U) && (gUpdateMain.flag == 2UL))
    {
        if ((gUpdateMain.recoveryUsed != 0UL) || (gUpdateMain.recoveryLength < 192UL) || (gUpdateMain.recoveryLength > REPROGRAM_APP_SIZE))
        {
            return;
        }
        Flash_Read(REPROGRAM_RECOVERY_START, vectors, sizeof(vectors));
        if ((vectors[0U] <= DRAM_ADDR) || (vectors[0U] > (DRAM_ADDR + DRAM_SIZE)) || ((vectors[0U] & 7UL) != 0UL) || ((vectors[1U] & 1UL) == 0UL) || (vectors[1U] < REPROGRAM_APP_START) || (vectors[1U] >= (REPROGRAM_APP_START + gUpdateMain.recoveryLength)))
        {
            return;
        }
        if (Reprogram_Crc((const uint8_t *)REPROGRAM_RECOVERY_START, gUpdateMain.recoveryLength) != gUpdateMain.recoveryCrc)
        {
            return;
        }
        if (Reprogram_Copy(REPROGRAM_RECOVERY_START, REPROGRAM_APP_START, gUpdateMain.recoveryLength) == 0U)
        {
            return;
        }
        if (Reprogram_Crc((const uint8_t *)REPROGRAM_APP_START, gUpdateMain.recoveryLength) != gUpdateMain.recoveryCrc)
        {
            return;
        }
        gUpdateMain.address = REPROGRAM_APP_START;
        gUpdateMain.length = gUpdateMain.recoveryLength;
        gUpdateMain.imageCrc = gUpdateMain.recoveryCrc;
        gUpdateMain.faultCount = 0UL;
        gUpdateMain.appConfirmed = 0UL;
        gUpdateMain.recoveryUsed = 1UL;
        if (gUpdateMain.restoreCount < 0xFFFFFFFFUL)
        {
            gUpdateMain.restoreCount++;
        }
        if (Reprogram_Store(0UL) != 0U)
        {
            NVIC_SystemReset();
        }
        return;
    }
    if ((stored != 0U) && ((gUpdateMain.flag != 0UL) || (gUpdateMain.faultCount >= 3UL)))
    {
        return;
    }
    Flash_Read(REPROGRAM_APP_START, vectors, sizeof(vectors));
    if ((vectors[0U] <= DRAM_ADDR) || (vectors[0U] > (DRAM_ADDR + DRAM_SIZE)) || ((vectors[0U] & 7UL) != 0UL))
    {
        return;
    }
    if (((vectors[1U] & 1UL) == 0UL) || (vectors[1U] < REPROGRAM_APP_START) || (vectors[1U] > REPROGRAM_APP_END))
    {
        return;
    }
    Reprogram_StartApp(REPROGRAM_APP_START);
}

void Reprogram_Process(void)
{
    if ((gReprogram.selected != 0U) && ((uint32_t)(oAppTimer.timeMs - gReprogram.requestTime) >= 5000UL))
    {
        gReprogram.selected = 0U;
        gReprogram.session = 0U;
        gReprogram.downloading = 0U;
    }
    if ((gReprogram.downloading == 0U) && (NVMCTRL_REGS->NVMCTRL_LOCK != 0U))
    {
        if (Reprogram_WriteProtect_Lock() == 0U)
        {
            return;
        }
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
    uint32_t size;
    if ((request == NULL) || (response == NULL) || (length == 0U))
    {
        return 0U;
    }
    if ((length == 8U) && (request[0U] == 0x31U) && (request[1U] == 0x01U) && (request[2U] == 0xF3U) && (request[3U] == 0x02U))
    {
        gReprogram.selected = 0U;
        gReprogram.session = 0U;
        gReprogram.downloading = 0U;
        if (Reprogram_WriteProtect_Lock() == 0U)
        {
            return 0U;
        }
        address = ((uint32_t)request[4U] << 24U) | ((uint32_t)request[5U] << 16U) | ((uint32_t)request[6U] << 8U) | request[7U];
        if ((address == gUDSInitNumber) && (address != 0UL) && (address != 0xFFFFFFFFUL))
        {
            gReprogram.selected = 1U;
            gReprogram.requestTime = oAppTimer.timeMs;
            memcpy(response, request, 8U);
            response[0U] = 0x71U;
            response[8U] = 1U;
            response[9U] = UDS_BOOT_NAD;
            return 10U;
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
            gReprogram.downloading = 0U;
            if (Reprogram_WriteProtect_Lock() == 0U)
            {
                nrc = 0x72U;
                break;
            }
            gReprogram.session = 1U;
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
                response[3U] = 1U;
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
                // F210 extends the existing 12-byte memory response; product data remains unchanged.
                response[28U] = 0U;
                if ((gUpdateMain.magic == REPROGRAM_MAGIC) && (gUpdateMain.crc == Reprogram_Crc((const uint8_t *)&gUpdateMain.magic, sizeof(gUpdateMain) - 4U)))
                {
                    response[28U] |= 1U;
                }
                if ((gUpdateBackup.magic == REPROGRAM_MAGIC) && (gUpdateBackup.crc == Reprogram_Crc((const uint8_t *)&gUpdateBackup.magic, sizeof(gUpdateBackup) - 4U)))
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

        case 0x34U:
            if (gReprogram.session == 0U)
            {
                nrc = 0x7FU;
                break;
            }
            if ((length != 11U) || (request[1U] != 0U) || (request[2U] != 0x44U))
            {
                nrc = 0x13U;
                break;
            }
            address = ((uint32_t)request[3U] << 24U) | ((uint32_t)request[4U] << 16U) | ((uint32_t)request[5U] << 8U) | request[6U];
            size = ((uint32_t)request[7U] << 24U) | ((uint32_t)request[8U] << 16U) | ((uint32_t)request[9U] << 8U) | request[10U];
            if ((address != REPROGRAM_APP_START) || (size < 192UL) || (size > REPROGRAM_APP_SIZE))
            {
                nrc = 0x31U;
                break;
            }
            gReprogram.downloading = 0U;
            gReprogram.verified = 0U;
            if (Reprogram_WriteProtect_Lock() == 0U)
            {
                nrc = 0x72U;
                break;
            }
            if (Reprogram_Load() == 0U)
            {
                Flash_Read(REPROGRAM_MAIN_ADDR, &gUpdateMain, sizeof(gUpdateMain));
                Flash_Read(REPROGRAM_BACKUP_ADDR, &gUpdateBackup, sizeof(gUpdateBackup));
                if (((gUpdateMain.magic == REPROGRAM_MAGIC) && (gUpdateMain.crc == Reprogram_Crc((const uint8_t *)&gUpdateMain.magic, sizeof(gUpdateMain) - 4U))) || ((gUpdateBackup.magic == REPROGRAM_MAGIC) && (gUpdateBackup.crc == Reprogram_Crc((const uint8_t *)&gUpdateBackup.magic, sizeof(gUpdateBackup) - 4U))))
                {
                    nrc = 0x72U;
                    break;
                }
                memset(&gUpdateMain, 0, sizeof(gUpdateMain));
                gUpdateMain.recoveryMagic = REPROGRAM_RECOVERY_MAGIC;
            }
            if (Reprogram_BackupApp() == 0U)
            {
                nrc = 0x72U;
                break;
            }
            gUpdateMain.address = address;
            gUpdateMain.length = size;
            gUpdateMain.imageCrc = 0UL;
            gUpdateMain.appConfirmed = 0UL;
            gUpdateMain.faultCount = 0UL;
            if (Reprogram_Store(1UL) == 0U)
            {
                nrc = 0x72U;
                break;
            }
            if (Reprogram_WriteProtect_Unlock() == 0U)
            {
                nrc = 0x72U;
                break;
            }
            gReprogram.received = 0UL;
            gReprogram.sequence = 1U;
            gReprogram.lastLength = 0U;
            gReprogram.downloading = 1U;
            gReprogram.requestTime = oAppTimer.timeMs;
            memset(gReprogram.row, 0xFF, sizeof(gReprogram.row));
            response[1U] = 0x20U;
            response[2U] = 0U;
            response[3U] = REPROGRAM_BLOCK_SIZE + 2U;
            return 4U;

        case 0x36U:
            if (gReprogram.downloading == 0U)
            {
                nrc = 0x24U;
                break;
            }
            if ((length < 3U) || (length > (REPROGRAM_BLOCK_SIZE + 2U)))
            {
                nrc = 0x13U;
                break;
            }
            if ((gReprogram.lastLength == length) && (request[1U] == gReprogram.lastSequence) && (Reprogram_Crc(request, length) == gReprogram.lastCrc))
            {
                response[1U] = request[1U];
                return 2U;
            }
            if (request[1U] != gReprogram.sequence)
            {
                nrc = 0x73U;
                break;
            }
            size = gUpdateMain.length - gReprogram.received;
            if (size > REPROGRAM_BLOCK_SIZE)
            {
                size = REPROGRAM_BLOCK_SIZE;
            }
            if ((size == 0UL) || (length != (size + 2UL)))
            {
                nrc = 0x13U;
                break;
            }
            memcpy((uint8_t *)gReprogram.row + (gReprogram.received % REPROGRAM_ROW_SIZE), &request[2U], size);
            address = gUpdateMain.address + (gReprogram.received / REPROGRAM_ROW_SIZE) * REPROGRAM_ROW_SIZE;
            if ((((gReprogram.received + size) % REPROGRAM_ROW_SIZE) == 0UL) || ((gReprogram.received + size) == gUpdateMain.length))
            {
                if (Reprogram_WriteRow(address, gReprogram.row) == 0U)
                {
                    gReprogram.downloading = 0U;
                    nrc = 0x72U;
                    break;
                }
                memset(gReprogram.row, 0xFF, sizeof(gReprogram.row));
            }
            gReprogram.received += size;
            gReprogram.lastSequence = request[1U];
            gReprogram.lastLength = length;
            gReprogram.lastCrc = Reprogram_Crc(request, length);
            gReprogram.sequence++;
            response[1U] = request[1U];
            return 2U;

        case 0x37U:
            // CJ transfer-exit parameter: 4-byte big-endian CRC32 of the complete BIN.
            if (length != 5U)
            {
                nrc = 0x13U;
                break;
            }
            if ((gReprogram.verified != 0U) && (gUpdateMain.imageCrc == (((uint32_t)request[1U] << 24U) | ((uint32_t)request[2U] << 16U) | ((uint32_t)request[3U] << 8U) | request[4U])))
            {
                return 1U;
            }
            if ((gReprogram.downloading == 0U) || (gReprogram.received != gUpdateMain.length))
            {
                nrc = 0x24U;
                break;
            }
            gUpdateMain.imageCrc = ((uint32_t)request[1U] << 24U) | ((uint32_t)request[2U] << 16U) | ((uint32_t)request[3U] << 8U) | request[4U];
            if (Reprogram_ImageValid() == 0U)
            {
                nrc = 0x72U;
                break;
            }
            if (Reprogram_WriteProtect_Lock() == 0U)
            {
                nrc = 0x72U;
                break;
            }
            if (Reprogram_Store(0UL) == 0U)
            {
                nrc = 0x72U;
                break;
            }
            gReprogram.downloading = 0U;
            gReprogram.verified = 1U;
            return 1U;

        case 0x11U:
            if ((length != 2U) || (request[1U] != 1U))
            {
                nrc = 0x13U;
                break;
            }
            if (gReprogram.verified == 0U)
            {
                nrc = 0x22U;
                break;
            }
            response[1U] = 1U;
            gReprogram.resetPending = 1U;
            gReprogram.resetResponseStarted = 0U;
            return 2U;
        default:
            nrc = 0x11U;
            break;
    }
    gReprogram.downloading = 0U;
    if (Reprogram_WriteProtect_Lock() == 0U)
    {
        nrc = 0x72U;
    }
    response[0U] = 0x7FU;
    response[1U] = request[0U];
    response[2U] = nrc;
    return 3U;
}
