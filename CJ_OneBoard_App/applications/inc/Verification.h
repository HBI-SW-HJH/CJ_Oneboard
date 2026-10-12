#ifndef VERIFICATION_H
#define VERIFICATION_H

#include <stdint.h>

#define VERIFICATION_REQUEST_ID_BASE   0x28U
#define VERIFICATION_RESPONSE_ID_BASE  0x2CU
#define VERIFICATION_READ              1U
#define VERIFICATION_FLICKER           2U
#define VERIFICATION_FADE              3U
#define VERIFICATION_STOP              4U
#define VERIFICATION_READ_RGB          5U
#define VERIFICATION_RESULT            6U
#define VERIFICATION_START             7U
#define VERIFICATION_FADE_POINT        8U
#define VERIFICATION_FADE_STATUS       9U
#define VERIFICATION_READ_EXPECTED     10U
#define VERIFICATION_FADE_RGB          11U
#define VERIFICATION_PROTOCOL_VERSION  2U
#define VERIFICATION_CAPTURE_MS        1000U
#define VERIFICATION_FADE_POINT_COUNT  10U
#define VERIFICATION_ZONE_COUNT        6U
#define VERIFICATION_ALL_ZONES         VERIFICATION_ZONE_COUNT

typedef struct
{
    uint32_t startTimeMs;
    uint32_t lastTimeMs;
    uint16_t samples;
    uint16_t edges[VERIFICATION_ZONE_COUNT];
    uint16_t changes[VERIFICATION_ZONE_COUNT];
    uint32_t lastRiseTimeMs[VERIFICATION_ZONE_COUNT];
    uint32_t lastEdgeTimeMs[VERIFICATION_ZONE_COUNT];
    uint8_t riseCount[VERIFICATION_ZONE_COUNT];
    uint8_t minPeriodMs[VERIFICATION_ZONE_COUNT];
    uint8_t maxPeriodMs[VERIFICATION_ZONE_COUNT];
    uint8_t minOnMs[VERIFICATION_ZONE_COUNT];
    uint8_t maxOnMs[VERIFICATION_ZONE_COUNT];
    uint8_t minOffMs[VERIFICATION_ZONE_COUNT];
    uint8_t maxOffMs[VERIFICATION_ZONE_COUNT];
    uint16_t lastRed[VERIFICATION_ZONE_COUNT];
    uint16_t lastGreen[VERIFICATION_ZONE_COUNT];
    uint16_t lastBlue[VERIFICATION_ZONE_COUNT];
    uint16_t fadeExpected[VERIFICATION_ZONE_COUNT][VERIFICATION_FADE_POINT_COUNT][3U];
    uint16_t fadeActual[VERIFICATION_ZONE_COUNT][VERIFICATION_FADE_POINT_COUNT][3U];
    uint8_t fadeStatus[VERIFICATION_ZONE_COUNT][VERIFICATION_FADE_POINT_COUNT];
    uint16_t fadeTimeMs[VERIFICATION_ZONE_COUNT][VERIFICATION_FADE_POINT_COUNT];
    uint16_t fadeReadMs[VERIFICATION_ZONE_COUNT][VERIFICATION_FADE_POINT_COUNT];
    uint32_t readTimeMs;
    uint16_t readDurationMs;
    uint8_t fadePointCount;
    uint16_t captureMs;
    uint16_t expected[3U];
    uint16_t actual[3U];
    uint8_t response[8U];
    uint8_t zone;
    uint8_t node;
    uint8_t color;
    uint8_t fresh;
    uint8_t sequence;
    uint8_t lastOn[VERIFICATION_ZONE_COUNT];
    uint8_t captureReady;
    uint8_t captureMode;
    uint8_t fault;
    uint8_t valid;
    uint8_t readStatus;
    uint8_t EnableFlag;
    volatile uint8_t ActiveFlag;
} Verification_t;

extern Verification_t gVerification;

void Verification_Init(void);
void Verification_Request(const uint8_t *data);
void Verification_Process(void);

#endif
