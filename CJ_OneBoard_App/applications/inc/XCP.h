// @file    XCP.h
// @brief   Read-only XCP commands on chair-specific LIN frames

#ifndef XCP_H
#define XCP_H

#include <stdint.h>

#define XCP_REQUEST_ID_BASE     0x20U
#define XCP_RESPONSE_ID_BASE    0x24U
#define XCP_FRAME_LENGTH        8U
#define XCP_CMD_CONNECT         0xFFU
#define XCP_CMD_DISCONNECT      0xFEU
#define XCP_CMD_SHORT_UPLOAD    0xF4U
#define XCP_PID_RESPONSE        0xFFU
#define XCP_PID_ERROR           0xFEU
#define XCP_ERR_UNKNOWN         0x20U
#define XCP_ERR_RANGE           0x22U
#define XCP_ERR_ACCESS          0x24U
#define XCP_ERR_SEQUENCE        0x2BU

typedef struct
{
    uint8_t response[XCP_FRAME_LENGTH];
    uint8_t connected;
} XCP_Control_t;

extern XCP_Control_t gXCPControl;

void XCP_Init(void);
void XCP_HandleRequest(const uint8_t *request);

#endif // XCP_H
