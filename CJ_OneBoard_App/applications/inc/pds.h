// @file    pds.h
// @brief   Persistent Data Storage for PIC32CM5164 Data Flash

#ifndef PDS_H
#define PDS_H

#include <stdint.h>

#ifndef APP_CONFIG_H
typedef struct ConFig ConFig_t;
#endif

#define PDS_CONFIG_MAGIC        0x434A4346UL
#define PDS_CONFIG_FORMAT       2U
#define PDS_CONFIG_CAPACITY     ((NVMCTRL_PAGE_SIZE * 4UL) - 16U)
#define PDS_CONFIG_MIN_LENGTH   23U
#define PDS_WRITE_ERROR      0U
#define PDS_WRITE_OK         1U
#define PDS_WRITE_INVALID    2U

typedef struct
{
    uint32_t crc32;
    uint32_t index;
    uint32_t magic;
    uint16_t length;
    uint16_t format;
    uint8_t data[PDS_CONFIG_CAPACITY];
} PDS_Record_t;

extern ConFig_t gConFig;
extern PDS_Record_t gPdsMain;
extern PDS_Record_t gPdsBackup;
extern uint8_t gPdsMainValid;
extern uint8_t gPdsBackupValid;
extern volatile uint32_t g_dbg_pds_write_error_count;

void PDS_Init(void);
void PDS_Read(void);
uint8_t PDS_Write(const ConFig_t *config);

#endif // PDS_H
