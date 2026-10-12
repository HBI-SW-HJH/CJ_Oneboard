#ifndef REPROGRAM_H
#define REPROGRAM_H

#include <stdint.h>

#define REPROGRAM_APP_START        0x00010000UL
#define REPROGRAM_APP_END          0x0003FFFFUL
#define REPROGRAM_APP_SIZE         (REPROGRAM_APP_END - REPROGRAM_APP_START + 1UL)
#define REPROGRAM_RECOVERY_START   0x00050000UL
#define REPROGRAM_RECOVERY_END     0x0007FFFFUL
#define REPROGRAM_RECOVERY_MAGIC   0x434A5231UL
#define REPROGRAM_PROTECT_START    0x00000000UL
#define REPROGRAM_PROTECT_END      0x0007FFFFUL
#define REPROGRAM_REGION_SIZE      0x00008000UL
#define REPROGRAM_APP_UNLOCK_MASK  0x00FCU
#define REPROGRAM_BACKUP_ADDR      0x00400F00UL
#define REPROGRAM_MAIN_ADDR        0x00401E00UL
#define REPROGRAM_MAGIC            0x434A4254UL
#define REPROGRAM_ROW_SIZE         256U
#define REPROGRAM_BLOCK_SIZE       128U

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
    uint8_t session;
    uint8_t downloading;
    uint8_t sequence;
    uint8_t lastSequence;
    uint8_t lastLength;
    uint8_t resetPending;
    uint8_t verified;
    volatile uint8_t resetResponseStarted;
    uint32_t resetTime;
    uint32_t requestTime;
    uint32_t received;
    uint32_t lastCrc;
    uint32_t row[REPROGRAM_ROW_SIZE / 4U];
} Reprogram_t;

extern Reprogram_Record_t gUpdateMain;
extern Reprogram_Record_t gUpdateBackup;
extern Reprogram_t gReprogram;

void Reprogram_Init(void);
void Reprogram_Process(void);
uint8_t Reprogram_WriteProtect_Lock(void);
uint8_t Reprogram_WriteProtect_Unlock(void);
uint8_t Reprogram_Request(const uint8_t *request, uint8_t length, uint8_t *response);
uint32_t Reprogram_Crc(const uint8_t *data, uint32_t length);
#endif
