#ifndef REPROGRAM_H
#define REPROGRAM_H

#include <stdint.h>

#define REPROGRAM_APP_START        0x00010000UL
#define REPROGRAM_APP_END          0x0003FFFFUL
#define REPROGRAM_APP_SIZE         (REPROGRAM_APP_END - REPROGRAM_APP_START + 1UL)
#define REPROGRAM_RECOVERY_MAGIC   0x434A5231UL
#define REPROGRAM_BACKUP_ADDR      0x00400F00UL
#define REPROGRAM_MAIN_ADDR        0x00401E00UL
#define REPROGRAM_MAGIC            0x434A4254UL
#define REPROGRAM_ROW_SIZE         256U
#define REPROGRAM_FAULT_LIMIT      3UL
#define REPROGRAM_HEALTHY_TIME_MS  10000UL

typedef struct
{
    uint32_t crc;
    uint32_t magic;
    uint32_t index;
    uint32_t flag;
    uint32_t address;
    uint32_t length;
    uint32_t imageCrc;
    uint32_t faultCount;
    uint32_t recoveryMagic;
    uint32_t recoveryLength;
    uint32_t recoveryCrc;
    uint32_t restoreCount;
    uint32_t appConfirmed;
    uint32_t recoveryUsed;
    uint8_t reserved[200U];
} Reprogram_Record_t;

typedef struct
{
    uint8_t selected;
    uint8_t resetPending;
    volatile uint8_t resetResponseStarted;
    uint32_t resetTime;
    uint32_t requestTime;
    uint32_t bootTime;
    uint8_t bootChecked;
} Reprogram_t;

extern Reprogram_Record_t gUpdateMain;
extern Reprogram_Record_t gUpdateBackup;
extern Reprogram_t gReprogram;

void Reprogram_Init(void);
void Reprogram_HardFault(void);
void Reprogram_Process(void);
uint8_t Reprogram_Request(const uint8_t *request, uint8_t length, uint8_t *response);
#endif
