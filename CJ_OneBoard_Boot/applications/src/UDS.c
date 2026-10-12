// @file    UDS.c
// @brief   UDS identity services and LIN transport timeout
#include "appConfig.h"
#include <stddef.h>
#include <string.h>

uint8_t s_discoverySlot;
uint32_t gUDSInitNumber;

void UDS_Init(void)
{
    uint32_t uniqueId[4U];
    uint8_t key[4U];
    uniqueId[0U] = CONFIG_UID_WORD0;
    uniqueId[1U] = CONFIG_UID_WORD1;
    uniqueId[2U] = CONFIG_UID_WORD2;
    uniqueId[3U] = CONFIG_UID_WORD3;
    CRC32_Init();
    gUDSInitNumber = CRC32_GetRunTimeCRC32((const uint8_t *)uniqueId, (uint16_t)sizeof(uniqueId));
    if ((gUDSInitNumber == 0UL) || (gUDSInitNumber == 0xFFFFFFFFUL))
    {
        gUDSInitNumber = 0x434A0001UL;
    }
    key[0U] = (uint8_t)(gUDSInitNumber >> 24U);
    key[1U] = (uint8_t)(gUDSInitNumber >> 16U);
    key[2U] = (uint8_t)(gUDSInitNumber >> 8U);
    key[3U] = (uint8_t)gUDSInitNumber;
    s_discoverySlot = (uint8_t)(CRC32_GetRunTimeCRC32(key, (uint16_t)sizeof(key)) % UDS_DISCOVERY_SLOT_COUNT);
}

void UDS_Timeout(void)
{
    uint32_t irqState;
    uint32_t txTime;
    if ((s_diag.length != 0U) && ((uint32_t)(oAppTimer.timeMs - s_diag.timeMs) > UDS_TRANSPORT_TIMEOUT_MS))
    {
        s_diag.length = 0U;
        g_dbg_lin_tp_error_count++;
    }
    irqState = __get_PRIMASK();
    __disable_irq();
    txTime = s_diag.txTimeMs;
    if ((s_frames[FID_DIAG_TX_IDX].frameIsValid != FALSE) && ((uint32_t)(oAppTimer.timeMs - txTime) > UDS_TRANSPORT_TIMEOUT_MS))
    {
        s_frames[FID_DIAG_TX_IDX].frameIsValid = FALSE;
        s_txRing.tail = s_txRing.head;
        g_dbg_lin_tp_error_count++;
    }
    __set_PRIMASK(irqState);
}

uint8_t UDS_HandleRequest(const uint8_t *request, uint8_t requestLength, uint8_t *response)
{
    if ((request == NULL) || (response == NULL) || (requestLength == 0U))
    {
        return 0U;
    }
    return Reprogram_Request(request, requestLength, response);
}

uint8_t UDS_HandleBroadcast(const uint8_t *request, uint8_t requestLength, uint8_t *response)
{
    uint16_t routine;
    uint32_t initNumber;

    if ((request == NULL) || (response == NULL) || (requestLength < 4U))
    {
        return 0U;
    }
    (void)memset(response, 0, UDS_MAX_MESSAGE_LENGTH);

    if ((requestLength == 8U) && (request[0U] == 0x31U) && (request[1U] == 0x01U) && (request[2U] == 0xF3U) && (request[3U] == 0x02U))
    {
        return Reprogram_Request(request, requestLength, response);
    }

    if (request[0U] == UDS_SID_ROUTINE_CONTROL)
    {
        routine = (uint16_t)(((uint16_t)request[2U] << 8U) | request[3U]);
        if ((requestLength == 5U) && (request[1U] == UDS_ROUTINE_RESULT) && (routine == UDS_ROUTINE_DISCOVERY))
        {
            if (request[4U] != s_discoverySlot)
            {
                return 0U;
            }
        }
        else if ((requestLength == 8U) && (request[1U] == UDS_ROUTINE_RESULT) && (routine == UDS_ROUTINE_TARGET_READ))
        {
            initNumber = ((uint32_t)request[4U] << 24U) | ((uint32_t)request[5U] << 16U) | ((uint32_t)request[6U] << 8U) | request[7U];
            if (initNumber != gUDSInitNumber)
            {
                return 0U;
            }
        }
        else
        {
            return 0U;
        }

        response[0U] = (uint8_t)(UDS_SID_ROUTINE_CONTROL + UDS_POSITIVE_RESPONSE_OFFSET);
        response[1U] = UDS_ROUTINE_RESULT;
        response[2U] = request[2U];
        response[3U] = request[3U];
        // Boot discovery exposes only the CPU key and execution mode, never product PDS.
        response[4U] = (uint8_t)(gUDSInitNumber >> 24U);
        response[5U] = (uint8_t)(gUDSInitNumber >> 16U);
        response[6U] = (uint8_t)(gUDSInitNumber >> 8U);
        response[7U] = (uint8_t)gUDSInitNumber;
        response[8U] = 1U;
        return 9U;
    }

    return 0U;
}
