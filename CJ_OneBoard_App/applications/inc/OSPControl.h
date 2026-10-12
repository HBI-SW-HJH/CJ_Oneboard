// @file    OSPControl.h
// @brief   Independent Logo and Strobe OSP Manchester control

#ifndef OSP_CONTROL_H
#define OSP_CONTROL_H

#include <stdint.h>
#if defined(__ICCARM__)
#include "device.h"
#endif

#ifndef LED_CONTROL_TYPES_DEFINED
#define LED_CONTROL_TYPES_DEFINED
typedef struct
{
    uint8_t chairMask;
    uint8_t ledMask;
    uint8_t colorMode;
    uint8_t fadeEnabled;
    uint8_t active;
    uint8_t sync;
    uint8_t mode;
    uint8_t color1;
    uint8_t color2;
    uint8_t color3;
    uint8_t effect1;
    uint8_t effect2;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint32_t timeMs;
} LedControlCommand_t;

#define LED_FLICKER_FADE_TIME_MS 500U
#define LED_CONTROL_TICK_US      1000U
#endif

#define OSP_PORT_COUNT          2U
#define OSP_LOGO_NODE_COUNT     24U
#define OSP_STROBE_NODE_COUNT   8U
#define OSP_MAX_NODE_COUNT      OSP_LOGO_NODE_COUNT
#define OSP_OTP_BYTE_COUNT      22U

#define OSP_LOGO_TX_PIN             24U
#define OSP_LOGO_RX_PIN             22U
#define OSP_LOGO_STBY_PIN           23U
#define OSP_STROBE_TX_PIN           18U
#define OSP_STROBE_RX_PIN           16U
#define OSP_STROBE_STBY_PIN         17U
#define OSP_PIN_GROUP               0U
#define OSP_RX_PMUX                 0U
#define OSP_ADDR_BROADCAST          0x000U
#define OSP_ADDR_FIRST              0x001U
#define OSP_CMD_RESET               0x00U
#define OSP_CMD_INIT_BIDIR          0x02U
#define OSP_CMD_GO_ACTIVE           0x05U
#define OSP_CMD_READ_TEMP_STATUS    0x42U
#define OSP_CMD_READ_LED_STATUS     0x46U
#define OSP_CMD_READ_SETUP          0x4CU
#define OSP_CMD_SET_SETUP           0x4DU
#define OSP_CMD_READ_PWM            0x4EU
#define OSP_CMD_SET_PWM             0x4FU
#define OSP_CMD_READ_OTP            0x58U
#define OSP_SETUP_DEFAULT           0x24U
//#define OSP_SETUP_DEFAULT           0x26U
#define OSP_PWM_MAX                 32767U
#define OSP_CURRENT_BIT             0x80U
// Per Logo RGB LED: total average R+G+B current, using nominal 50 mA daytime current.
#define OSP_LOGO_CURRENT_LIMIT_MA   41U
#define OSP_MAX_RAW_BYTES           12U
#define OSP_MAX_TX_BYTES            ((OSP_MAX_RAW_BYTES * 2U) + 4U)
#define OSP_CAPTURE_COUNT           256U
#define OSP_MAX_RX_LEVELS           512U
#define OSP_HALF_BIT_TICKS          10U
#define OSP_TX_DMA_CHANNEL          0U
#define OSP_RX_DMA_CHANNEL          1U
#define OSP_DMA_CHANNEL_COUNT       2U
#define OSP_WAIT_LIMIT              80000UL
#define OSP_PRESENCE_ATTEMPTS       3U
#define OSP_LOGO_FIRST_NODE         0U
#define OSP_LOGO_SECOND_NODE        12U
#define OSP_RESPONSE_ONE_BYTE       5U
#define OSP_RESPONSE_TEMP_STATUS    6U
#define OSP_RESPONSE_PWM            10U
#define OSP_RESPONSE_OTP            12U
#define OSP_STATUS_COMM_ERROR       (1U << 3U)
#define OSP_STATUS_LED_FAULT        (1U << 2U)
#define OSP_STATUS_OVER_TEMP        (1U << 1U)
#define OSP_STATUS_UNDER_VOLT       (1U << 0U)
#define OSP_LED_OPEN_MASK           0x70U
#define OSP_LED_SHORT_MASK          0x07U
#define OSP_TEMPERATURE_LIMIT_C     133

typedef enum
{
    OSP_PORT_LOGO = 0U,
    OSP_PORT_STROBE
} OSP_Port_t;

typedef enum
{
    OSP_FAULT_PART_LOGO = 0U,
    OSP_FAULT_PART_STROBE
} OSP_FaultPart_t;

typedef struct
{
    uint32_t startTimeMs[OSP_PORT_COUNT];
    uint32_t phaseTimeMs[OSP_PORT_COUNT];
    uint32_t communicationErrorCount[OSP_PORT_COUNT];
    uint32_t crcErrorCount[OSP_PORT_COUNT];
    uint32_t manchesterErrorCount[OSP_PORT_COUNT];
    uint16_t startRed[OSP_PORT_COUNT];
    uint16_t startGreen[OSP_PORT_COUNT];
    uint16_t startBlue[OSP_PORT_COUNT];
    uint16_t targetRed[OSP_PORT_COUNT];
    uint16_t targetGreen[OSP_PORT_COUNT];
    uint16_t targetBlue[OSP_PORT_COUNT];
    uint16_t fadedRed[OSP_PORT_COUNT];
    uint16_t fadedGreen[OSP_PORT_COUNT];
    uint16_t fadedBlue[OSP_PORT_COUNT];
    uint16_t outputRed[OSP_PORT_COUNT];
    uint16_t outputGreen[OSP_PORT_COUNT];
    uint16_t outputBlue[OSP_PORT_COUNT];
    uint16_t durationMs[OSP_PORT_COUNT];
    uint16_t fadeInMs[OSP_PORT_COUNT];
    uint16_t fadeOutMs[OSP_PORT_COUNT];
    uint16_t onThreshold[OSP_PORT_COUNT];
    uint16_t flickerPhase[OSP_PORT_COUNT];
    uint16_t pwmRed[OSP_PORT_COUNT];
    uint16_t pwmGreen[OSP_PORT_COUNT];
    uint16_t pwmBlue[OSP_PORT_COUNT];
    int16_t temperatureC[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint16_t rxPwmRed[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint16_t rxPwmGreen[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint16_t rxPwmBlue[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint8_t otp[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT][OSP_OTP_BYTE_COUNT];
    uint8_t status[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint8_t setup[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint8_t ledStatus[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint8_t dynamicValid[OSP_PORT_COUNT][OSP_MAX_NODE_COUNT];
    uint8_t nodeCount[OSP_PORT_COUNT];
    uint8_t readNode[OSP_PORT_COUNT];
    uint8_t mode[OSP_PORT_COUNT];
    uint8_t effect1[OSP_PORT_COUNT];
    uint8_t effect2[OSP_PORT_COUNT];
    uint8_t active[OSP_PORT_COUNT];
    uint8_t fadeEnabled[OSP_PORT_COUNT];
    uint8_t dirty[OSP_PORT_COUNT];
} OSP_Control_t;

extern OSP_Control_t gOSPControl;
#if defined(__ICCARM__)
extern dmac_descriptor_registers_t s_dmaDescriptor[OSP_DMA_CHANNEL_COUNT];
extern dmac_descriptor_registers_t s_dmaWriteback[OSP_DMA_CHANNEL_COUNT];
#endif
extern uint8_t s_txBytes[OSP_MAX_TX_BYTES];
extern uint16_t s_capturePeriods[OSP_CAPTURE_COUNT];

void OSPControl_Init(void);
void OSPControl_Set(const LedControlCommand_t *command);
void OSPControl_Process(void);
// Reads one LED node per call and advances to the next node.
void OSPControl_Read(OSP_Port_t port);
void OSPControl_CheckLogoOpen(void);
void OSPControl_CheckStrobeOpen(void);
uint8_t OSPControl_GetFaultMask(OSP_FaultPart_t part);

#endif // OSP_CONTROL_H
