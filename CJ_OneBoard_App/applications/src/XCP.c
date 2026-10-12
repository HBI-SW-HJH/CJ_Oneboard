// @file    XCP.c
// @brief   Read-only XCP access to LED status variables

#include "appConfig.h"

XCP_Control_t gXCPControl;

void XCP_Init(void)
{
    (void)memset(&gXCPControl, 0, sizeof(gXCPControl));
}

void XCP_HandleRequest(const uint8_t *request)
{
    uint32_t address;
    uint32_t first;
    uint32_t last;
    uint8_t length;
    uint8_t index;
    uint8_t allowed = 0U;
    volatile const uint8_t *source;

    if (request == NULL)
    {
        return;
    }
    (void)memset(gXCPControl.response, 0xFF, XCP_FRAME_LENGTH);
    gXCPControl.response[0U] = XCP_PID_ERROR;

    if (request[0U] == XCP_CMD_CONNECT)
    {
        gXCPControl.connected = 1U;
        gXCPControl.response[0U] = XCP_PID_RESPONSE;
        gXCPControl.response[1U] = 0x01U;
        gXCPControl.response[2U] = 0x00U;
        gXCPControl.response[3U] = XCP_FRAME_LENGTH;
        gXCPControl.response[4U] = XCP_FRAME_LENGTH;
        gXCPControl.response[5U] = 0x01U;
        gXCPControl.response[6U] = 0x01U;
        gXCPControl.response[7U] = 0x00U;
        return;
    }
    if (request[0U] == XCP_CMD_DISCONNECT)
    {
        gXCPControl.connected = 0U;
        gXCPControl.response[0U] = XCP_PID_RESPONSE;
        return;
    }
    if (request[0U] != XCP_CMD_SHORT_UPLOAD)
    {
        gXCPControl.response[1U] = XCP_ERR_UNKNOWN;
        return;
    }
    if (gXCPControl.connected == 0U)
    {
        gXCPControl.response[1U] = XCP_ERR_SEQUENCE;
        return;
    }

    length = request[1U];
    address = (uint32_t)request[4U] | ((uint32_t)request[5U] << 8U) | ((uint32_t)request[6U] << 16U) | ((uint32_t)request[7U] << 24U);
    if ((length == 0U) || (length > 4U) || (request[3U] != 0U))
    {
        gXCPControl.response[1U] = XCP_ERR_RANGE;
        return;
    }

    first = (uint32_t)&gOSPControl;
    last = first + (uint32_t)sizeof(gOSPControl);
    if ((address >= first) && (address < last) && ((uint32_t)length <= (last - address)))
    {
        allowed = 1U;
    }
    first = (uint32_t)&gSRGBControl;
    last = first + (uint32_t)sizeof(gSRGBControl);
    if ((address >= first) && (address < last) && ((uint32_t)length <= (last - address)))
    {
        allowed = 1U;
    }
    first = (uint32_t)&gPowerData;
    last = first + (uint32_t)sizeof(gPowerData);
    if ((address >= first) && (address < last) && ((uint32_t)length <= (last - address)))
    {
        allowed = 1U;
    }
    if (allowed == 0U)
    {
        gXCPControl.response[1U] = XCP_ERR_ACCESS;
        return;
    }

    source = (volatile const uint8_t *)address;
    gXCPControl.response[0U] = XCP_PID_RESPONSE;
    for (index = 0U; index < length; index++)
    {
        gXCPControl.response[index + 1U] = source[index];
    }
}
