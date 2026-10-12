// @file    pds.c
// @brief   CJ product information storage

#include "appConfig.h"

ConFig_t gConFig = CONFIG_DEFAULTS;
PDS_Record_t gPdsMain;
PDS_Record_t gPdsBackup;
uint8_t gPdsMainValid;
uint8_t gPdsBackupValid;
volatile uint32_t g_dbg_pds_write_error_count;

void PDS_Init(void)
{
    CRC32_Init();
    memset(&gConFig, 0, sizeof(gConFig));
    gConFig.version = CONFIG_VERSION;
    memcpy(gConFig.Serial_Number, CONFIG_SERIAL_NUMBER, sizeof(CONFIG_SERIAL_NUMBER));
    memcpy(gConFig.Part_Number, CONFIG_PART_NUMBER, sizeof(CONFIG_PART_NUMBER));
    gConFig.Chair_Number = CONFIG_CHAIR_NUMBER;
    gConFig.Init_Number = 0UL;
    g_dbg_pds_write_error_count = 0UL;
    PDS_Read();
    //ver만 업데이트
    if (gConFig.version != CONFIG_VERSION)
    {
        gConFig.version = CONFIG_VERSION;
        PDS_Write(&gConFig);
    }
    
}

void PDS_Read(void)
{
    uint32_t uniqueId[4U];
    uint8_t updateRequired = 0U;
    uint8_t retry;

    Flash_Read(FLASH_PDS_SYSTEM_ADDR, &gPdsMain, sizeof(gPdsMain));
    Flash_Read(FLASH_PDS_SYSTEM_BACK_ADDR, &gPdsBackup, sizeof(gPdsBackup));

    gPdsMainValid = 0U;
    if ((gPdsMain.magic == PDS_CONFIG_MAGIC) && (((gPdsMain.format == PDS_CONFIG_FORMAT) && (gPdsMain.length >= PDS_CONFIG_MIN_LENGTH)) || ((gPdsMain.format == 1U) && (gPdsMain.length >= 24U))) && (gPdsMain.length <= PDS_CONFIG_CAPACITY) && (CRC32_GetRunTimeCRC32((const uint8_t *)&gPdsMain.index, (uint16_t)(sizeof(gPdsMain) - sizeof(gPdsMain.crc32))) == gPdsMain.crc32))
    {
        gPdsMainValid = 1U;
    }

    gPdsBackupValid = 0U;
    if ((gPdsBackup.magic == PDS_CONFIG_MAGIC) && (((gPdsBackup.format == PDS_CONFIG_FORMAT) && (gPdsBackup.length >= PDS_CONFIG_MIN_LENGTH)) || ((gPdsBackup.format == 1U) && (gPdsBackup.length >= 24U))) && (gPdsBackup.length <= PDS_CONFIG_CAPACITY) && (CRC32_GetRunTimeCRC32((const uint8_t *)&gPdsBackup.index, (uint16_t)(sizeof(gPdsBackup) - sizeof(gPdsBackup.crc32))) == gPdsBackup.crc32))
    {
        gPdsBackupValid = 1U;
    }

    if ((gPdsMainValid == 0U) && (gPdsBackupValid == 0U))
    {
        uniqueId[0U] = CONFIG_UID_WORD0;
        uniqueId[1U] = CONFIG_UID_WORD1;
        uniqueId[2U] = CONFIG_UID_WORD2;
        uniqueId[3U] = CONFIG_UID_WORD3;
        gConFig.Init_Number = CRC32_GetRunTimeCRC32((const uint8_t *)uniqueId, (uint16_t)sizeof(uniqueId));
        if ((gConFig.Init_Number == 0UL) || (gConFig.Init_Number == 0xFFFFFFFFUL))
        {
            gConFig.Init_Number = 0x434A0001UL;
        }
        PDS_Write(&gConFig);
        return;
    }

    if ((gPdsBackupValid != 0U) && ((gPdsMainValid == 0U) || ((int32_t)(gPdsBackup.index - gPdsMain.index) > 0)))
    {
        if (gPdsBackup.format == 1U)
        {
            // Format 1: version[0..1], serial[2..12], part[13..22], chair[23], init[24..27].
            memcpy(&gConFig, gPdsBackup.data, 13U);
            memcpy(gConFig.Part_Number, CONFIG_PART_NUMBER, sizeof(CONFIG_PART_NUMBER));
            gConFig.Chair_Number = gPdsBackup.data[23U];
            gConFig.Init_Number = 0UL;
            if (gPdsBackup.length >= 28U)
            {
                gConFig.Init_Number = (uint32_t)gPdsBackup.data[24U] | ((uint32_t)gPdsBackup.data[25U] << 8U) | ((uint32_t)gPdsBackup.data[26U] << 16U) | ((uint32_t)gPdsBackup.data[27U] << 24U);
            }
            updateRequired = 1U;
        }
        else if (gPdsBackup.length < sizeof(gConFig))
        {
            memcpy(&gConFig, gPdsBackup.data, gPdsBackup.length);
            updateRequired = 1U;
        }
        else
        {
            memcpy(&gConFig, gPdsBackup.data, sizeof(gConFig));
        }
        if ((gPdsMainValid == 0U) || (memcmp(&gPdsMain, &gPdsBackup, sizeof(gPdsMain)) != 0))
        {
            gPdsMain = gPdsBackup;
            for (retry = 0U; retry < 3U; retry++)
            {
                if (Flash_WriteRow(FLASH_PDS_SYSTEM_ADDR, &gPdsMain, sizeof(gPdsMain)) != 0U)
                {
                    gPdsMainValid = 1U;
                    break;
                }
                g_dbg_pds_write_error_count++;
            }
            if (retry == 3U)
            {
                Flash_Read(FLASH_PDS_SYSTEM_ADDR, &gPdsMain, sizeof(gPdsMain));
                gPdsMainValid = 0U;
            }
        }
    }
    else
    {
        if (gPdsMain.format == 1U)
        {
            // Format 1: version[0..1], serial[2..12], part[13..22], chair[23], init[24..27].
            memcpy(&gConFig, gPdsMain.data, 13U);
            memcpy(gConFig.Part_Number, CONFIG_PART_NUMBER, sizeof(CONFIG_PART_NUMBER));
            gConFig.Chair_Number = gPdsMain.data[23U];
            gConFig.Init_Number = 0UL;
            if (gPdsMain.length >= 28U)
            {
                gConFig.Init_Number = (uint32_t)gPdsMain.data[24U] | ((uint32_t)gPdsMain.data[25U] << 8U) | ((uint32_t)gPdsMain.data[26U] << 16U) | ((uint32_t)gPdsMain.data[27U] << 24U);
            }
            updateRequired = 1U;
        }
        else if (gPdsMain.length < sizeof(gConFig))
        {
            memcpy(&gConFig, gPdsMain.data, gPdsMain.length);
            updateRequired = 1U;
        }
        else
        {
            memcpy(&gConFig, gPdsMain.data, sizeof(gConFig));
        }
        if ((gPdsBackupValid == 0U) || (memcmp(&gPdsBackup, &gPdsMain, sizeof(gPdsBackup)) != 0))
        {
            gPdsBackup = gPdsMain;
            for (retry = 0U; retry < 3U; retry++)
            {
                if (Flash_WriteRow(FLASH_PDS_SYSTEM_BACK_ADDR, &gPdsBackup, sizeof(gPdsBackup)) != 0U)
                {
                    gPdsBackupValid = 1U;
                    break;
                }
                g_dbg_pds_write_error_count++;
            }
            if (retry == 3U)
            {
                Flash_Read(FLASH_PDS_SYSTEM_BACK_ADDR, &gPdsBackup, sizeof(gPdsBackup));
                gPdsBackupValid = 0U;
            }
        }
    }

    if ((gConFig.Init_Number == 0UL) || (gConFig.Init_Number == 0xFFFFFFFFUL))
    {
        uniqueId[0U] = CONFIG_UID_WORD0;
        uniqueId[1U] = CONFIG_UID_WORD1;
        uniqueId[2U] = CONFIG_UID_WORD2;
        uniqueId[3U] = CONFIG_UID_WORD3;
        gConFig.Init_Number = CRC32_GetRunTimeCRC32((const uint8_t *)uniqueId, (uint16_t)sizeof(uniqueId));
        if ((gConFig.Init_Number == 0UL) || (gConFig.Init_Number == 0xFFFFFFFFUL))
        {
            gConFig.Init_Number = 0x434A0001UL;
        }
        updateRequired = 1U;
    }
    if (updateRequired != 0U)
    {
        PDS_Write(&gConFig);
    }
}

uint8_t PDS_Write(const ConFig_t *config)
{
    uint32_t index = 0UL;
    uint8_t retry;

    if ((config == NULL) || (sizeof(ConFig_t) > PDS_CONFIG_CAPACITY))
    {
        return PDS_WRITE_INVALID;
    }

    if ((gPdsMainValid != 0U) && (gPdsBackupValid != 0U) && (gPdsMain.format == PDS_CONFIG_FORMAT) && (gPdsMain.length == sizeof(ConFig_t)) && (memcmp(&gPdsMain, &gPdsBackup, sizeof(gPdsMain)) == 0) && (memcmp(gPdsMain.data, config, sizeof(ConFig_t)) == 0))
    {
        gConFig = *config;
        return PDS_WRITE_OK;
    }

    if (gPdsMainValid != 0U)
    {
        index = gPdsMain.index;
    }
    if ((gPdsBackupValid != 0U) && ((gPdsMainValid == 0U) || ((int32_t)(gPdsBackup.index - index) > 0)))
    {
        index = gPdsBackup.index;
    }

    memset(&gPdsBackup, 0xFF, sizeof(gPdsBackup));
    gPdsBackup.index = index + 1UL;
    gPdsBackup.magic = PDS_CONFIG_MAGIC;
    gPdsBackup.length = (uint16_t)sizeof(ConFig_t);
    gPdsBackup.format = PDS_CONFIG_FORMAT;
    memcpy(gPdsBackup.data, config, sizeof(ConFig_t));
    gPdsBackup.crc32 = CRC32_GetRunTimeCRC32((const uint8_t *)&gPdsBackup.index, (uint16_t)(sizeof(gPdsBackup) - sizeof(gPdsBackup.crc32)));

    for (retry = 0U; retry < 3U; retry++)
    {
        if (Flash_WriteRow(FLASH_PDS_SYSTEM_BACK_ADDR, &gPdsBackup, sizeof(gPdsBackup)) != 0U)
        {
            gPdsBackupValid = 1U;
            break;
        }
        g_dbg_pds_write_error_count++;
    }
    if (retry == 3U)
    {
        Flash_Read(FLASH_PDS_SYSTEM_BACK_ADDR, &gPdsBackup, sizeof(gPdsBackup));
        gPdsBackupValid = 0U;
        return PDS_WRITE_ERROR;
    }

    gPdsMain = gPdsBackup;
    for (retry = 0U; retry < 3U; retry++)
    {
        if (Flash_WriteRow(FLASH_PDS_SYSTEM_ADDR, &gPdsMain, sizeof(gPdsMain)) != 0U)
        {
            gPdsMainValid = 1U;
            break;
        }
        g_dbg_pds_write_error_count++;
    }
    if (retry == 3U)
    {
        Flash_Read(FLASH_PDS_SYSTEM_ADDR, &gPdsMain, sizeof(gPdsMain));
        gPdsMainValid = 0U;
    }

    gConFig = *config;
    return PDS_WRITE_OK;
}
