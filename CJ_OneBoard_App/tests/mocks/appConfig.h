// Host-only replacements. No MCU peripherals are accessed.
#ifndef APP_CONFIG_H
#define APP_CONFIG_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#ifdef CONFIG_EXTENSION_TEST
#define CJ_CONFIG_H
// Same public prefix plus future fields APPENDED; PDS/UDS sources stay unchanged.
#define CONFIG_SERIAL_NUMBER "26I16-0001"
#define CONFIG_PART_NUMBER "ZX1-0025"
#define CONFIG_VERSION 0x0001U
#define CONFIG_CHAIR_NUMBER 0x0AU
#define CONFIG_CHAIR_MIN 0x0AU
#define CONFIG_CHAIR_MAX 0x0DU
#define CONFIG_SERIAL_LENGTH 10U
#define CONFIG_PART_LENGTH 8U
#define CONFIG_DEFAULTS { CONFIG_VERSION, CONFIG_SERIAL_NUMBER, CONFIG_PART_NUMBER, CONFIG_CHAIR_NUMBER, 0UL, { 0x5AU } }
#pragma pack(push, 1)
typedef struct ConFig
{
    uint16_t version;
    char Serial_Number[11];
    char Part_Number[CONFIG_PART_LENGTH + 1U];
    uint8_t Chair_Number;
    uint32_t Init_Number;
    uint8_t future[120];
} ConFig_t;
#pragma pack(pop)
#else
#define CONFIG_SERIAL_NUMBER "26I16-0001"
#define CONFIG_PART_NUMBER "ZX1-0025"
#define CONFIG_VERSION 0x0001U
#define CONFIG_CHAIR_NUMBER 0x0AU
#define CONFIG_CHAIR_MIN 0x0AU
#define CONFIG_CHAIR_MAX 0x0DU
#define CONFIG_SERIAL_LENGTH 10U
#define CONFIG_PART_LENGTH 8U
#define CONFIG_DEFAULTS { CONFIG_VERSION, CONFIG_SERIAL_NUMBER, CONFIG_PART_NUMBER, CONFIG_CHAIR_NUMBER, 0UL }
#pragma pack(push, 1)
typedef struct ConFig
{
    uint16_t version;
    char Serial_Number[CONFIG_SERIAL_LENGTH + 1U];
    char Part_Number[CONFIG_PART_LENGTH + 1U];
    uint8_t Chair_Number;
    uint32_t Init_Number;
} ConFig_t;
#pragma pack(pop)
#endif
#include "UDS.h"
#include "Reprogram.h"
#include "Verification.h"
#include "XCP.h"
#include "SRGBControl.h"
#include "OSPControl.h"
#define LIN_DEVICE_H
#define LIN_BUFF_SIZE 8U
typedef enum { LIN_CHECKSUM_CLASSIC, LIN_CHECKSUM_ENHANCED } LIN_ChecksumType_t;
typedef enum { LIN_MSG_TYPE_RX, LIN_MSG_TYPE_TX } LIN_MsgType_t;
typedef struct
{
    uint8_t frame_id;
    LIN_MsgType_t msg_type;
    LIN_ChecksumType_t checksum;
    uint8_t length;
    uint8_t data[8];
    uint8_t frameIsValid;
} LIN_Device_Frame_t;
#define LIN_FRAME_DATA_LEN 8U
#define CJ_CONTROL_FRAME_ID 0x01U
#define CJ_ID_CHANGE_OFFSET 0x10U
#define CJ_DIAG_REQUEST_ID 0x3CU
#define CJ_DIAG_RESPONSE_ID 0x3DU
#define CONFIG_UID_WORD0 0x13572468UL
#define CONFIG_UID_WORD1 0x24681357UL
#define CONFIG_UID_WORD2 0x89ABCDEFUL
#define CONFIG_UID_WORD3 0x10203040UL
#ifndef TRUE
#define TRUE 1U
#endif
#ifndef FALSE
#define FALSE 0U
#endif
#define NVMCTRL_PAGE_SIZE 64U
#define FLASH_PDS_ROW_SIZE 256U
#define FLASH_PDS_SYSTEM_BACK_ADDR 0x10000100UL
#define FLASH_PDS_SYSTEM_ADDR 0x10001000UL
#include "pds.h"
#include "lin_task.h"
typedef struct { volatile uint32_t timeMs; } MockTimer_t;
extern MockTimer_t oAppTimer;
extern volatile uint32_t g_dbg_lin_drop_count;
void CRC32_Init(void);
uint32_t CRC32_GetRunTimeCRC32(const uint8_t *data, uint16_t length);
void Flash_Read(uint32_t address, void *data, uint32_t size);
uint8_t Flash_WriteRow(uint32_t address, const void *data, uint32_t size);
void LIN_RegisterFrames(LIN_Device_Frame_t *frames, uint8_t count);
void LIN_Init(void (*rx)(const LIN_Device_Frame_t *), void (*tx)(LIN_Device_Frame_t *));
#define __DMB() ((void)0)
#define __get_PRIMASK() 0U
#define __disable_irq() ((void)0)
#define __set_PRIMASK(X) ((void)(X))
#endif
