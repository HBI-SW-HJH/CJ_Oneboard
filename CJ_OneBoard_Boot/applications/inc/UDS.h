// @file    UDS.h
// @brief   Minimal ISO 14229 service handler over an 8-byte LIN diagnostic frame

#ifndef UDS_H
#define UDS_H

#include <stdint.h>

#define UDS_MAX_MESSAGE_LENGTH              130U
#define UDS_PRODUCT_DATA_LENGTH             29U
#define UDS_TRANSPORT_TIMEOUT_MS            1000U
#define UDS_DID_PRODUCT_INFO                0xF202U
#define UDS_BROADCAST_NAD                   0x7FU
#define UDS_BOOT_NAD                        0x7DU
#define UDS_DISCOVERY_SLOT_COUNT             16U
#define UDS_ROUTINE_DISCOVERY               0xF300U
#define UDS_ROUTINE_TARGET_READ             0xF301U
#define UDS_ROUTINE_RESULT                  0x03U

#define UDS_SID_DIAGNOSTIC_SESSION_CONTROL  0x10U
#define UDS_SID_ECU_RESET                   0x11U
#define UDS_SID_CLEAR_DIAGNOSTIC_INFO       0x14U
#define UDS_SID_READ_DTC_INFO               0x19U
#define UDS_SID_READ_DATA_BY_ID             0x22U
#define UDS_SID_READ_MEMORY_BY_ADDRESS      0x23U
#define UDS_SID_READ_SCALING_DATA_BY_ID     0x24U
#define UDS_SID_SECURITY_ACCESS             0x27U
#define UDS_SID_COMMUNICATION_CONTROL       0x28U
#define UDS_SID_AUTHENTICATION              0x29U
#define UDS_SID_READ_DATA_BY_PERIODIC_ID    0x2AU
#define UDS_SID_DYNAMICALLY_DEFINE_DATA_ID  0x2CU
#define UDS_SID_WRITE_DATA_BY_ID            0x2EU
#define UDS_SID_INPUT_OUTPUT_CONTROL        0x2FU
#define UDS_SID_ROUTINE_CONTROL             0x31U
#define UDS_SID_REQUEST_DOWNLOAD            0x34U
#define UDS_SID_REQUEST_UPLOAD              0x35U
#define UDS_SID_TRANSFER_DATA               0x36U
#define UDS_SID_REQUEST_TRANSFER_EXIT       0x37U
#define UDS_SID_REQUEST_FILE_TRANSFER       0x38U
#define UDS_SID_WRITE_MEMORY_BY_ADDRESS     0x3DU
#define UDS_SID_TESTER_PRESENT              0x3EU
#define UDS_SID_ACCESS_TIMING_PARAMETER     0x83U
#define UDS_SID_SECURED_DATA_TRANSMISSION   0x84U
#define UDS_SID_CONTROL_DTC_SETTING         0x85U
#define UDS_SID_RESPONSE_ON_EVENT           0x86U
#define UDS_SID_LINK_CONTROL                0x87U

#define UDS_POSITIVE_RESPONSE_OFFSET        0x40U
#define UDS_NEGATIVE_RESPONSE_SID           0x7FU
#define UDS_NRC_SERVICE_NOT_SUPPORTED       0x11U
#define UDS_NRC_INCORRECT_LENGTH            0x13U
#define UDS_NRC_REQUEST_OUT_OF_RANGE        0x31U
#define UDS_NRC_PROGRAMMING_FAILURE         0x72U

extern uint8_t s_discoverySlot;
extern uint32_t gUDSInitNumber;

// Payload includes SID, excludes NAD/PCI. Returns complete response length.
void UDS_Init(void);
void UDS_Timeout(void);
uint8_t UDS_HandleRequest(const uint8_t *request, uint8_t requestLength, uint8_t *response);
uint8_t UDS_HandleBroadcast(const uint8_t *request, uint8_t requestLength, uint8_t *response);

#endif // UDS_H
