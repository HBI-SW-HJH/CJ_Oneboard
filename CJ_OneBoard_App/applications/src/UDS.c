// @file    UDS.c
// @brief   UDS identity services and LIN transport timeout
#include "appConfig.h"
#include <stddef.h>
#include <string.h>

uint8_t s_discoverySlot;

void UDS_Init(void)
{
    uint32_t uniqueId[4U];
    uint8_t key[4U];
    // Use the same CPU-derived key in App and Boot, regardless of stored PDS.
    uniqueId[0U] = CONFIG_UID_WORD0;
    uniqueId[1U] = CONFIG_UID_WORD1;
    uniqueId[2U] = CONFIG_UID_WORD2;
    uniqueId[3U] = CONFIG_UID_WORD3;
    CRC32_Init();
    gConFig.Init_Number = CRC32_GetRunTimeCRC32((const uint8_t *)uniqueId, (uint16_t)sizeof(uniqueId));
    if ((gConFig.Init_Number == 0UL) || (gConFig.Init_Number == 0xFFFFFFFFUL))
    {
        gConFig.Init_Number = 0x434A0001UL;
    }
    key[0U] = (uint8_t)(gConFig.Init_Number >> 24U);
    key[1U] = (uint8_t)(gConFig.Init_Number >> 16U);
    key[2U] = (uint8_t)(gConFig.Init_Number >> 8U);
    key[3U] = (uint8_t)gConFig.Init_Number;
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

static void UDS_ReadProduct(const ConFig_t *config, uint8_t *data)
{
    (void)memcpy(data, config->Serial_Number, CONFIG_SERIAL_LENGTH);
    data[10U] = '*';
    (void)memcpy(&data[11U], config->Part_Number, CONFIG_PART_LENGTH);
    data[19U] = '*';
    data[20U] = (uint8_t)(config->version >> 8U);
    data[21U] = (uint8_t)config->version;
    data[22U] = '*';
    data[23U] = config->Chair_Number;
    data[24U] = '*';
    data[25U] = (uint8_t)(config->Init_Number >> 24U);
    data[26U] = (uint8_t)(config->Init_Number >> 16U);
    data[27U] = (uint8_t)(config->Init_Number >> 8U);
    data[28U] = (uint8_t)config->Init_Number;
}

static void UDS_WriteProduct(ConFig_t *config, const uint8_t *data)
{
    (void)memcpy(config->Serial_Number, data, CONFIG_SERIAL_LENGTH);
    (void)memcpy(config->Part_Number, &data[11U], CONFIG_PART_LENGTH);
    config->Serial_Number[CONFIG_SERIAL_LENGTH] = '\0';
    config->Part_Number[CONFIG_PART_LENGTH] = '\0';
    if ((data[23U] >= CONFIG_CHAIR_MIN) && (data[23U] <= CONFIG_CHAIR_MAX))
    {
        config->Chair_Number = data[23U];
    }
}

uint8_t UDS_HandleRequest(const uint8_t *request, uint8_t requestLength, uint8_t *response)
{
    ConFig_t config = gConFig;
    uint16_t did;
    uint8_t length = 0U;
    uint8_t nrc = 0U;
    uint8_t pdsResult;
    uint8_t sid;
    if ((request == NULL) || (response == NULL) || (requestLength == 0U))
    {
        return 0U;
    }
    if ((request[0U] == 0x10U) || (request[0U] == 0x11U) || (request[0U] == 0x34U) || (request[0U] == 0x36U) || (request[0U] == 0x37U) || (request[0U] == 0x3EU) || ((request[0U] == 0x31U) && (requestLength >= 4U) && (request[2U] == 0xF3U) && (request[3U] == 0x02U)) || ((request[0U] == 0x22U) && (requestLength == 3U) && (request[1U] == 0xF2U) && (request[2U] == 0x10U)))
    {
        return Reprogram_Request(request, requestLength, response);
    }
    sid = request[0U];
    (void)memset(response, 0, UDS_MAX_MESSAGE_LENGTH);
    switch (sid)
    {
        case UDS_SID_READ_DATA_BY_ID:
        case UDS_SID_WRITE_DATA_BY_ID:
            if (requestLength < 3U)
            {
                nrc = UDS_NRC_INCORRECT_LENGTH;
                break;
            }
            did = (uint16_t)(((uint16_t)request[1U] << 8U) | request[2U]);
            if (did != UDS_DID_PRODUCT_INFO)
            {
                nrc = UDS_NRC_REQUEST_OUT_OF_RANGE;
                break;
            }
            length = UDS_PRODUCT_DATA_LENGTH;
            if (((sid == UDS_SID_READ_DATA_BY_ID) && (requestLength != 3U)) || ((sid == UDS_SID_WRITE_DATA_BY_ID) && (requestLength != (uint8_t)(length + 3U))))
            {
                nrc = UDS_NRC_INCORRECT_LENGTH;
                break;
            }
            response[0U] = (uint8_t)(sid + UDS_POSITIVE_RESPONSE_OFFSET);
            response[1U] = request[1U];
            response[2U] = request[2U];
            if (sid == UDS_SID_READ_DATA_BY_ID)
            {
                UDS_ReadProduct(&config, &response[3U]);
            }
            else
            {
                if ((((uint16_t)request[23U] << 8U) | request[24U]) != gConFig.version)
                {
                    nrc = UDS_NRC_REQUEST_OUT_OF_RANGE;
                    length = 0U;
                    break;
                }
                if ((request[13U] != '*') || (request[22U] != '*') || (request[25U] != '*') || (request[27U] != '*') || ((((uint32_t)request[28U] << 24U) | ((uint32_t)request[29U] << 16U) | ((uint32_t)request[30U] << 8U) | request[31U]) != gConFig.Init_Number))
                {
                    nrc = UDS_NRC_REQUEST_OUT_OF_RANGE;
                    length = 0U;
                    break;
                }
                UDS_WriteProduct(&config, &request[3U]);
                pdsResult = PDS_Write(&config);
                if (pdsResult == PDS_WRITE_INVALID)
                {
                    nrc = UDS_NRC_REQUEST_OUT_OF_RANGE;
                }
                else if (pdsResult == PDS_WRITE_ERROR)
                {
                    nrc = UDS_NRC_PROGRAMMING_FAILURE;
                }
                length = 0U;
            }
            length = (uint8_t)(length + 3U);
            break;

        case UDS_SID_DIAGNOSTIC_SESSION_CONTROL:
        case UDS_SID_ECU_RESET:
        case UDS_SID_CLEAR_DIAGNOSTIC_INFO:
        case UDS_SID_READ_DTC_INFO:
        case UDS_SID_READ_MEMORY_BY_ADDRESS:
        case UDS_SID_READ_SCALING_DATA_BY_ID:
        case UDS_SID_SECURITY_ACCESS:
        case UDS_SID_COMMUNICATION_CONTROL:
        case UDS_SID_AUTHENTICATION:
        case UDS_SID_READ_DATA_BY_PERIODIC_ID:
        case UDS_SID_DYNAMICALLY_DEFINE_DATA_ID:
        case UDS_SID_INPUT_OUTPUT_CONTROL:
        case UDS_SID_ROUTINE_CONTROL:
        case UDS_SID_REQUEST_DOWNLOAD:
        case UDS_SID_REQUEST_UPLOAD:
        case UDS_SID_TRANSFER_DATA:
        case UDS_SID_REQUEST_TRANSFER_EXIT:
        case UDS_SID_REQUEST_FILE_TRANSFER:
        case UDS_SID_WRITE_MEMORY_BY_ADDRESS:
        case UDS_SID_TESTER_PRESENT:
        case UDS_SID_ACCESS_TIMING_PARAMETER:
        case UDS_SID_SECURED_DATA_TRANSMISSION:
        case UDS_SID_CONTROL_DTC_SETTING:
        case UDS_SID_RESPONSE_ON_EVENT:
        case UDS_SID_LINK_CONTROL:
        default:
            nrc = UDS_NRC_SERVICE_NOT_SUPPORTED;
            break;
    }
    if (nrc != 0U)
    {
        response[0U] = UDS_NEGATIVE_RESPONSE_SID;
        response[1U] = sid;
        response[2U] = nrc;
        length = 3U;
    }
    return length;
}

uint8_t UDS_HandleBroadcast(const uint8_t *request, uint8_t requestLength, uint8_t *response)
{
    ConFig_t config = gConFig;
    uint16_t did;
    uint16_t routine;
    uint32_t initNumber;
    uint8_t nrc = 0U;
    uint8_t pdsResult;

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
            if (initNumber != gConFig.Init_Number)
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
        UDS_ReadProduct(&config, &response[4U]);
        return (uint8_t)(UDS_PRODUCT_DATA_LENGTH + 4U);
    }

    if ((request[0U] != UDS_SID_WRITE_DATA_BY_ID) || (requestLength < 8U))
    {
        return 0U;
    }
    did = (uint16_t)(((uint16_t)request[1U] << 8U) | request[2U]);
    if ((did == UDS_DID_TARGET_PRODUCT) && (requestLength == (uint8_t)(UDS_PRODUCT_DATA_LENGTH + 3U)))
    {
        initNumber = ((uint32_t)request[28U] << 24U) | ((uint32_t)request[29U] << 16U) | ((uint32_t)request[30U] << 8U) | request[31U];
        if (initNumber != gConFig.Init_Number)
        {
            return 0U;
        }
        if ((request[13U] != '*') || (request[22U] != '*') || (request[25U] != '*') || (request[27U] != '*') || ((((uint16_t)request[23U] << 8U) | request[24U]) != gConFig.version))
        {
            nrc = UDS_NRC_REQUEST_OUT_OF_RANGE;
        }
        else
        {
            UDS_WriteProduct(&config, &request[3U]);
        }
    }
    else
    {
        nrc = UDS_NRC_INCORRECT_LENGTH;
    }

    if (nrc == 0U)
    {
        pdsResult = PDS_Write(&config);
        if (pdsResult == PDS_WRITE_INVALID)
        {
            nrc = UDS_NRC_REQUEST_OUT_OF_RANGE;
        }
        else if (pdsResult == PDS_WRITE_ERROR)
        {
            nrc = UDS_NRC_PROGRAMMING_FAILURE;
        }
    }

    if (nrc != 0U)
    {
        response[0U] = UDS_NEGATIVE_RESPONSE_SID;
        response[1U] = UDS_SID_WRITE_DATA_BY_ID;
        response[2U] = nrc;
        return 3U;
    }
    response[0U] = (uint8_t)(UDS_SID_WRITE_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET);
    response[1U] = request[1U];
    response[2U] = request[2U];
    return 3U;
}
