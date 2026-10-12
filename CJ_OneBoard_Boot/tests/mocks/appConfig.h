// Host-only MCU replacement; intentionally has no PDS definitions or APIs.
#ifndef BOOT_HOST_CONFIG_H
#define BOOT_HOST_CONFIG_H
#include <stdint.h>
#include <string.h>
#define __stackless
#define __noreturn
#define __disable_irq() ((void)0)
#define __DMB() ((void)0)
#define __get_PRIMASK() 0U
#define __set_PRIMASK(x) ((void)(x))
#define DRAM_ADDR 0x20000000UL
#define DRAM_SIZE 0x10000UL
#define NVMCTRL_CTRLA_CMD_LR_Val 1U
#define NVMCTRL_CTRLA_CMD_UR_Val 2U
#define LIN_STATE_IDLE 0U
#define SERCOM_USART_INT_INTFLAG_TXC_Msk 1U
typedef struct { uint32_t NVMCTRL_ADDR; uint16_t NVMCTRL_LOCK; } HostNvm;
typedef struct { struct { uint8_t SERCOM_INTFLAG; } USART_INT; } HostSercom;
typedef struct { uint32_t timeMs; } HostTimer;
// Used by the App reprogramming test only, not by Boot.
typedef struct { uint32_t Init_Number; } HostConfig;
extern HostNvm hostNvm;
extern HostSercom hostSercom;
extern HostTimer oAppTimer;
extern HostConfig gConFig;
extern uint32_t gCRC32Table[256];
extern uint32_t hostUid[4];
extern uint8_t s_state;
#define NVMCTRL_REGS (&hostNvm)
#define SERCOM0_REGS (&hostSercom)
#define NVIC_SystemReset() HostReset()
#define CONFIG_UID_WORD0 hostUid[0]
#define CONFIG_UID_WORD1 hostUid[1]
#define CONFIG_UID_WORD2 hostUid[2]
#define CONFIG_UID_WORD3 hostUid[3]
void HostReset(void);
void HostJump(uint32_t address);
uint8_t HostCommand(uint8_t command);
uint8_t HostWriteRow(uint32_t address, const uint32_t *data);
void CRC32_Init(void);
uint32_t CRC32_GetRunTimeCRC32(const uint8_t *data, uint16_t length);
void Flash_Read(uint32_t address, void *data, uint32_t length);
#define LIN_DEVICE_H
#define LIN_FRAME_DATA_LEN 8U
#define CJ_DIAG_REQUEST_ID 0x3CU
#define CJ_DIAG_RESPONSE_ID 0x3DU
#ifndef TRUE
#define TRUE 1U
#endif
#ifndef FALSE
#define FALSE 0U
#endif
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
#include "UDS.h"
#include "lin_task.h"
extern volatile uint32_t g_dbg_lin_drop_count;
void LIN_RegisterFrames(LIN_Device_Frame_t *frames, uint8_t count);
void LIN_Init(void (*rx)(const LIN_Device_Frame_t *), void (*tx)(LIN_Device_Frame_t *));
#endif
