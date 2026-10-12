// @file    SRGBControl.h
// @brief   AL58263 two-device sRGB LED control

#ifndef SRGB_CONTROL_H
#define SRGB_CONTROL_H

#include <stdint.h>

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

#define SRGB_DEVICE_COUNT       2U
#define SRGB_CHANNEL_COUNT      16U
#define SRGB_ZONE_COUNT         4U

#define SRGB_DI_PIN                 8U
#define SRGB_DCK_PIN                9U
#define SRGB_LAT_PIN                10U
#define SRGB_DO_PIN                 11U
#define SRGB_PIN_GROUP              0U
#define SRGB_COMMAND_DEFAULT        0x26ACU // 25mA x 2, short threshold 4.5V
#define SRGB_INSTR_ERROR_MODE       14U
#define SRGB_INSTR_DATA_LATCH       1U
#define SRGB_INSTR_GLOBAL_LATCH     2U
#define SRGB_INSTR_READ_DATA        6U
#define SRGB_INSTR_RETURN_NORMAL    8U
#define SRGB_INSTR_WRITE_COMMAND    10U
#define SRGB_DIAG_OPEN              2U
#define SRGB_DIAG_SHORT             4U
#define SRGB_DIAG_SHORT_GROUND      6U
#define SRGB_DIAG_THERMAL_WATCHDOG  8U
#define SRGB_DIAG_SMART             10U
#define SRGB_DIAG_REXT_WATCHDOG     14U

typedef enum
{
    SRGB_ZONE_FRT_LH = 0U,
    SRGB_ZONE_FRT_RH,
    SRGB_ZONE_HEADREST,
    SRGB_ZONE_BACK
} SRGB_Zone_t;

typedef enum
{
    SRGB_FAULT_PART_FRT_LH = 0U,
    SRGB_FAULT_PART_FRT_RH,
    SRGB_FAULT_PART_HEADREST,
    SRGB_FAULT_PART_BACK
} SRGB_FaultPart_t;

typedef struct
{
    uint32_t startTimeMs[SRGB_ZONE_COUNT];
    uint32_t phaseTimeMs[SRGB_ZONE_COUNT];
    uint32_t communicationErrorCount;
    uint16_t startRed[SRGB_ZONE_COUNT];
    uint16_t startGreen[SRGB_ZONE_COUNT];
    uint16_t startBlue[SRGB_ZONE_COUNT];
    uint16_t targetRed[SRGB_ZONE_COUNT];
    uint16_t targetGreen[SRGB_ZONE_COUNT];
    uint16_t targetBlue[SRGB_ZONE_COUNT];
    uint16_t fadedRed[SRGB_ZONE_COUNT];
    uint16_t fadedGreen[SRGB_ZONE_COUNT];
    uint16_t fadedBlue[SRGB_ZONE_COUNT];
    uint16_t outputRed[SRGB_ZONE_COUNT];
    uint16_t outputGreen[SRGB_ZONE_COUNT];
    uint16_t outputBlue[SRGB_ZONE_COUNT];
    uint16_t durationMs[SRGB_ZONE_COUNT];
    uint16_t fadeInMs[SRGB_ZONE_COUNT];
    uint16_t fadeOutMs[SRGB_ZONE_COUNT];
    uint16_t onThreshold[SRGB_ZONE_COUNT];
    uint16_t flickerPhase[SRGB_ZONE_COUNT];
    uint16_t txCommandWord[SRGB_DEVICE_COUNT];
    uint16_t txImage[SRGB_DEVICE_COUNT][SRGB_CHANNEL_COUNT];
    uint16_t rxCommandWord[SRGB_DEVICE_COUNT];
    uint16_t rxImage[SRGB_DEVICE_COUNT][SRGB_CHANNEL_COUNT];
    uint16_t openFault[SRGB_DEVICE_COUNT];
    uint16_t shortFault[SRGB_DEVICE_COUNT];
    uint16_t shortGroundFault[SRGB_DEVICE_COUNT];
    uint16_t smartFault[SRGB_DEVICE_COUNT];
    uint16_t thermalWatchdogFault[SRGB_DEVICE_COUNT];
    uint16_t rextWatchdogFault[SRGB_DEVICE_COUNT];
    uint8_t mode[SRGB_ZONE_COUNT];
    uint8_t effect1[SRGB_ZONE_COUNT];
    uint8_t effect2[SRGB_ZONE_COUNT];
    uint8_t active[SRGB_ZONE_COUNT];
    uint8_t fadeEnabled[SRGB_ZONE_COUNT];
    uint8_t imageDirty;
    uint8_t dynamicValid;
} SRGB_Control_t;

extern SRGB_Control_t gSRGBControl;

void SRGBControl_Init(void);
void SRGBControl_Set(const LedControlCommand_t *command);
void SRGBControl_Process(void);
void SRGBControl_Read(void);
uint8_t SRGBControl_GetFaultMask(SRGB_FaultPart_t part);

#endif // SRGB_CONTROL_H
