#include "appConfig.h"

Verification_t gVerification;

static uint8_t Verification_Read(uint8_t zone, uint8_t node, uint8_t fresh)
{
    uint8_t port;
    uint8_t device;
    uint8_t offset;
    uint8_t nextNode;
    uint8_t status;

    if (zone < SRGB_ZONE_COUNT)
    {
        device = (zone < SRGB_ZONE_HEADREST) ? 1U : 0U;
        offset = ((zone == SRGB_ZONE_FRT_RH) || (zone == SRGB_ZONE_HEADREST)) ? 2U : 0U;
        gVerification.expected[0U] = gSRGBControl.txImage[device][offset];
        gVerification.expected[1U] = gSRGBControl.txImage[device][offset + 4U];
        gVerification.expected[2U] = gSRGBControl.txImage[device][offset + 8U];
        if (fresh != 0U)
        {
            gVerification.readTimeMs = oAppTimer.timeMs;
            SRGBControl_Read();
            gVerification.readDurationMs = (uint16_t)(oAppTimer.timeMs - gVerification.readTimeMs);
        }
        gVerification.actual[0U] = gSRGBControl.rxImage[device][offset];
        gVerification.actual[1U] = gSRGBControl.rxImage[device][offset + 4U];
        gVerification.actual[2U] = gSRGBControl.rxImage[device][offset + 8U];
        gVerification.valid = gSRGBControl.dynamicValid;
        gVerification.fault = SRGBControl_GetFaultMask((SRGB_FaultPart_t)zone);
    }
    else
    {
        port = (uint8_t)(zone - SRGB_ZONE_COUNT);
        gVerification.expected[0U] = gOSPControl.pwmRed[port];
        gVerification.expected[1U] = gOSPControl.pwmGreen[port];
        gVerification.expected[2U] = gOSPControl.pwmBlue[port];
        gVerification.readTimeMs = oAppTimer.timeMs;
        gOSPControl.dynamicValid[port][node] = 0U;
        if (node < gOSPControl.nodeCount[port])
        {
            nextNode = gOSPControl.readNode[port];
            gOSPControl.readNode[port] = node;
            OSPControl_Read((OSP_Port_t)port);
            gOSPControl.readNode[port] = nextNode;
        }
        gVerification.readDurationMs = (uint16_t)(oAppTimer.timeMs - gVerification.readTimeMs);
        gVerification.actual[0U] = gOSPControl.rxPwmRed[port][node];
        gVerification.actual[1U] = gOSPControl.rxPwmGreen[port][node];
        gVerification.actual[2U] = gOSPControl.rxPwmBlue[port][node];
        gVerification.valid = gOSPControl.dynamicValid[port][node];
        gVerification.fault = 0U;
        if ((gOSPControl.status[port][node] & OSP_STATUS_COMM_ERROR) != 0U)
        {
            gVerification.fault |= 0x10U;
        }
        if ((gOSPControl.status[port][node] & OSP_STATUS_OVER_TEMP) != 0U)
        {
            gVerification.fault |= 0x01U;
        }
        if (((gOSPControl.status[port][node] & OSP_STATUS_UNDER_VOLT) != 0U) || (gPowerData.voltageFault != 0U))
        {
            gVerification.fault |= 0x02U;
        }
        if ((gOSPControl.ledStatus[port][node] & OSP_LED_OPEN_MASK) != 0U)
        {
            gVerification.fault |= 0x08U;
        }
        if ((gOSPControl.ledStatus[port][node] & OSP_LED_SHORT_MASK) != 0U)
        {
            gVerification.fault |= 0x04U;
        }
    }
    status = (uint8_t)((gVerification.valid != 0U) ? 0x01U : 0x40U);
    status |= (uint8_t)((gVerification.expected[0U] == gVerification.actual[0U]) ? 0x02U : 0U);
    status |= (uint8_t)((gVerification.expected[1U] == gVerification.actual[1U]) ? 0x04U : 0U);
    status |= (uint8_t)((gVerification.expected[2U] == gVerification.actual[2U]) ? 0x08U : 0U);
    status |= (uint8_t)((gVerification.fault & 0x08U) != 0U ? 0x10U : 0U);
    status |= (uint8_t)((gVerification.fault & 0x04U) != 0U ? 0x20U : 0U);
    status |= (uint8_t)((gVerification.fault & 0x10U) != 0U ? 0x40U : 0U);
    status |= (uint8_t)((gVerification.fault & 0x03U) != 0U ? 0x80U : 0U);
    return status;
}

void Verification_Init(void)
{
    memset(&gVerification, 0, sizeof(gVerification));
}

void Verification_Request(const uint8_t *data)
{
    uint32_t irqState;
    uint8_t zone;
    uint8_t color;
    uint16_t value;
    if (data == NULL)
    {
        return;
    }
    if (data[0U] == VERIFICATION_START)
    {
        gVerification.ActiveFlag = 1U;
        gVerification.EnableFlag = 0U;
        gVerification.captureReady = 0U;
        gVerification.response[0U] = 0x87U;
        gVerification.response[1U] = VERIFICATION_PROTOCOL_VERSION;
        gVerification.response[2U] = 0x56U;
        gVerification.response[3U] = 0x32U;
        return;
    }
    if (data[0U] == VERIFICATION_STOP)
    {
        AppTimer_ResetReadCount();
        gVerification.ActiveFlag = 0U;
        gVerification.EnableFlag = 0U;
        gVerification.captureReady = 0U;
        gVerification.response[0U] = 0x84U;
        return;
    }
    if (data[0U] == VERIFICATION_RESULT)
    {
        if ((gVerification.captureReady == 0U) || (data[1U] >= VERIFICATION_ZONE_COUNT))
        {
            return;
        }
        zone = data[1U];
        irqState = __get_PRIMASK();
        __disable_irq();
        gVerification.response[0U] = (gVerification.captureMode == VERIFICATION_FLICKER) ? 0x82U : 0x83U;
        gVerification.response[1U] = zone;
        if (gVerification.captureMode == VERIFICATION_FLICKER)
        {
            gVerification.response[2U] = gVerification.minPeriodMs[zone];
            gVerification.response[3U] = gVerification.maxPeriodMs[zone];
            gVerification.response[4U] = gVerification.minOnMs[zone];
            gVerification.response[5U] = gVerification.maxOnMs[zone];
            gVerification.response[6U] = gVerification.minOffMs[zone];
            gVerification.response[7U] = gVerification.maxOffMs[zone];
        }
        else
        {
            gVerification.response[2U] = (uint8_t)(gVerification.samples >> 8U);
            gVerification.response[3U] = (uint8_t)gVerification.samples;
            gVerification.response[4U] = (uint8_t)(gVerification.changes[zone] >> 8U);
            gVerification.response[5U] = (uint8_t)gVerification.changes[zone];
            gVerification.response[6U] = (uint8_t)(gVerification.edges[zone] >> 8U);
            gVerification.response[7U] = (uint8_t)gVerification.edges[zone];
        }
        __set_PRIMASK(irqState);
        return;
    }
    if ((data[0U] == VERIFICATION_READ_EXPECTED) || (data[0U] == VERIFICATION_FADE_RGB))
    {
        if ((gVerification.EnableFlag != 0U) || (data[1U] >= VERIFICATION_ZONE_COUNT))
        {
            return;
        }
        if ((data[0U] == VERIFICATION_READ_EXPECTED) && ((data[1U] != gVerification.zone) || (data[2U] != gVerification.node)))
        {
            return;
        }
        if ((data[0U] == VERIFICATION_FADE_RGB) && ((gVerification.captureReady == 0U) || (gVerification.captureMode != VERIFICATION_FADE) || (data[2U] >= VERIFICATION_FADE_POINT_COUNT) || (data[3U] > 1U)))
        {
            return;
        }
        irqState = __get_PRIMASK();
        __disable_irq();
        gVerification.response[0U] = gVerification.readStatus;
        for (color = 0U; color < 3U; color++)
        {
            value = gVerification.expected[color];
            if (data[0U] == VERIFICATION_FADE_RGB)
            {
                gVerification.response[0U] = gVerification.fadeStatus[data[1U]][data[2U]];
                value = gVerification.fadeExpected[data[1U]][data[2U]][color];
                if (data[3U] != 0U)
                {
                    value = gVerification.fadeActual[data[1U]][data[2U]][color];
                }
            }
            gVerification.response[1U + (color * 2U)] = (uint8_t)(value >> 8U);
            gVerification.response[2U + (color * 2U)] = (uint8_t)value;
        }
        gVerification.response[7U] = data[5U];
        __set_PRIMASK(irqState);
        return;
    }
    if ((data[0U] == VERIFICATION_FADE_POINT) || (data[0U] == VERIFICATION_FADE_STATUS))
    {
        if ((gVerification.captureReady == 0U) || (gVerification.captureMode != VERIFICATION_FADE) || (data[1U] >= VERIFICATION_ZONE_COUNT) || (data[2U] >= VERIFICATION_FADE_POINT_COUNT) || (data[3U] >= 3U))
        {
            return;
        }
        zone = data[1U];
        irqState = __get_PRIMASK();
        __disable_irq();
        gVerification.response[0U] = 0x88U;
        gVerification.response[1U] = zone;
        gVerification.response[2U] = data[2U];
        gVerification.response[3U] = data[3U];
        gVerification.response[4U] = (uint8_t)(gVerification.fadeExpected[zone][data[2U]][data[3U]] >> 8U);
        gVerification.response[5U] = (uint8_t)gVerification.fadeExpected[zone][data[2U]][data[3U]];
        gVerification.response[6U] = (uint8_t)(gVerification.fadeActual[zone][data[2U]][data[3U]] >> 8U);
        gVerification.response[7U] = (uint8_t)gVerification.fadeActual[zone][data[2U]][data[3U]];
        if (data[0U] == VERIFICATION_FADE_STATUS)
        {
            gVerification.response[0U] = 0x89U;
            gVerification.response[3U] = gVerification.fadeStatus[zone][data[2U]];
            gVerification.response[4U] = (uint8_t)(gVerification.fadeTimeMs[zone][data[2U]] >> 8U);
            gVerification.response[5U] = (uint8_t)gVerification.fadeTimeMs[zone][data[2U]];
            gVerification.response[6U] = (uint8_t)(gVerification.fadeReadMs[zone][data[2U]] >> 8U);
            gVerification.response[7U] = (uint8_t)gVerification.fadeReadMs[zone][data[2U]];
        }
        __set_PRIMASK(irqState);
        return;
    }
    if ((gVerification.EnableFlag != 0U) || (data[1U] > VERIFICATION_ALL_ZONES))
    {
        return;
    }
    if (((data[0U] == VERIFICATION_READ) || (data[0U] == VERIFICATION_READ_RGB)) && (data[1U] < VERIFICATION_ZONE_COUNT) && ((data[0U] == VERIFICATION_READ_RGB) || (data[3U] <= 2U)) && (data[2U] < ((data[1U] == 4U) ? OSP_LOGO_NODE_COUNT : ((data[1U] == 5U) ? OSP_STROBE_NODE_COUNT : 1U))))
    {
        gVerification.zone = data[1U];
        gVerification.node = data[2U];
        gVerification.color = (data[0U] == VERIFICATION_READ_RGB) ? 0U : data[3U];
        gVerification.fresh = data[4U];
        gVerification.sequence = data[5U];
    }
    else if ((data[0U] == VERIFICATION_FLICKER) || (data[0U] == VERIFICATION_FADE))
    {
        gVerification.zone = data[1U];
        gVerification.startTimeMs = oAppTimer.timeMs;
        gVerification.lastTimeMs = oAppTimer.timeMs;
        gVerification.samples = 0U;
        memset(gVerification.edges, 0, sizeof(gVerification.edges));
        memset(gVerification.changes, 0, sizeof(gVerification.changes));
        memset(gVerification.lastRiseTimeMs, 0, sizeof(gVerification.lastRiseTimeMs));
        memset(gVerification.lastEdgeTimeMs, 0, sizeof(gVerification.lastEdgeTimeMs));
        memset(gVerification.riseCount, 0, sizeof(gVerification.riseCount));
        memset(gVerification.maxPeriodMs, 0, sizeof(gVerification.maxPeriodMs));
        memset(gVerification.minPeriodMs, 0xFF, sizeof(gVerification.minPeriodMs));
        memset(gVerification.maxOnMs, 0, sizeof(gVerification.maxOnMs));
        memset(gVerification.minOnMs, 0xFF, sizeof(gVerification.minOnMs));
        memset(gVerification.maxOffMs, 0, sizeof(gVerification.maxOffMs));
        memset(gVerification.minOffMs, 0xFF, sizeof(gVerification.minOffMs));
        memset(gVerification.lastRed, 0, sizeof(gVerification.lastRed));
        memset(gVerification.lastGreen, 0, sizeof(gVerification.lastGreen));
        memset(gVerification.lastBlue, 0, sizeof(gVerification.lastBlue));
        memset(gVerification.lastOn, 0, sizeof(gVerification.lastOn));
        if (data[0U] == VERIFICATION_FADE)
        {
            memset(gVerification.fadeExpected, 0, sizeof(gVerification.fadeExpected));
            memset(gVerification.fadeActual, 0, sizeof(gVerification.fadeActual));
            memset(gVerification.fadeStatus, 0, sizeof(gVerification.fadeStatus));
            memset(gVerification.fadeTimeMs, 0, sizeof(gVerification.fadeTimeMs));
            memset(gVerification.fadeReadMs, 0, sizeof(gVerification.fadeReadMs));
            gVerification.fadePointCount = 0U;
        }
        gVerification.captureReady = 0U;
        gVerification.captureMode = data[0U];
        gVerification.captureMs = (uint16_t)data[5U] | ((uint16_t)data[6U] << 8U);
        if ((gVerification.captureMs < 20U) || (gVerification.captureMs > VERIFICATION_CAPTURE_MS))
        {
            gVerification.captureMs = VERIFICATION_CAPTURE_MS;
        }
    }
    else
    {
        return;
    }
    irqState = __get_PRIMASK();
    __disable_irq();
    memset(gVerification.response, 0, sizeof(gVerification.response));
    gVerification.response[0U] = 0x80U;
    gVerification.response[1U] = data[1U];
    gVerification.EnableFlag = data[0U];
    __set_PRIMASK(irqState);
}

void Verification_Process(void)
{
    uint8_t result[8U];
    uint8_t zone;
    uint8_t device;
    uint8_t offset;
    uint8_t fadePoint;
    uint8_t on;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint32_t periodMs;
    uint32_t durationMs;
    uint32_t irqState;

    if (gVerification.EnableFlag == 0U)
    {
        return;
    }

    if (((gVerification.EnableFlag == VERIFICATION_FLICKER) || (gVerification.EnableFlag == VERIFICATION_FADE)) && (gVerification.lastTimeMs != oAppTimer.timeMs) && ((uint32_t)(oAppTimer.timeMs - gVerification.startTimeMs) <= gVerification.captureMs))
    {
        for (zone = 0U; zone < VERIFICATION_ZONE_COUNT; zone++)
        {
            if ((gVerification.zone != VERIFICATION_ALL_ZONES) && (zone != gVerification.zone))
            {
                continue;
            }
            if (zone < SRGB_ZONE_COUNT)
            {
                device = (zone < SRGB_ZONE_HEADREST) ? 1U : 0U;
                offset = ((zone == SRGB_ZONE_FRT_RH) || (zone == SRGB_ZONE_HEADREST)) ? 2U : 0U;
                red = gSRGBControl.txImage[device][offset];
                green = gSRGBControl.txImage[device][offset + 4U];
                blue = gSRGBControl.txImage[device][offset + 8U];
            }
            else
            {
                red = gOSPControl.pwmRed[zone - SRGB_ZONE_COUNT];
                green = gOSPControl.pwmGreen[zone - SRGB_ZONE_COUNT];
                blue = gOSPControl.pwmBlue[zone - SRGB_ZONE_COUNT];
            }
            on = ((red != 0U) || (green != 0U) || (blue != 0U)) ? 1U : 0U;
            if (gVerification.samples != 0U)
            {
                if (on != gVerification.lastOn[zone])
                {
                    if (gVerification.EnableFlag == VERIFICATION_FLICKER)
                    {
                        if (gVerification.edges[zone] != 0U)
                        {
                            durationMs = oAppTimer.timeMs - gVerification.lastEdgeTimeMs[zone];
                            if (durationMs > 255U)
                            {
                                durationMs = 255U;
                            }
                            if (gVerification.lastOn[zone] != 0U)
                            {
                                if (durationMs < gVerification.minOnMs[zone])
                                {
                                    gVerification.minOnMs[zone] = (uint8_t)durationMs;
                                }
                                if (durationMs > gVerification.maxOnMs[zone])
                                {
                                    gVerification.maxOnMs[zone] = (uint8_t)durationMs;
                                }
                            }
                            else
                            {
                                if (durationMs < gVerification.minOffMs[zone])
                                {
                                    gVerification.minOffMs[zone] = (uint8_t)durationMs;
                                }
                                if (durationMs > gVerification.maxOffMs[zone])
                                {
                                    gVerification.maxOffMs[zone] = (uint8_t)durationMs;
                                }
                            }
                        }
                        gVerification.lastEdgeTimeMs[zone] = oAppTimer.timeMs;
                        if (gVerification.riseCount[zone] != 0U)
                        {
                            if (on != 0U)
                            {
                                periodMs = oAppTimer.timeMs - gVerification.lastRiseTimeMs[zone];
                                if (periodMs > 255U)
                                {
                                    periodMs = 255U;
                                }
                                if (periodMs < gVerification.minPeriodMs[zone])
                                {
                                    gVerification.minPeriodMs[zone] = (uint8_t)periodMs;
                                }
                                if (periodMs > gVerification.maxPeriodMs[zone])
                                {
                                    gVerification.maxPeriodMs[zone] = (uint8_t)periodMs;
                                }
                            }
                        }
                        if (on != 0U)
                        {
                            gVerification.lastRiseTimeMs[zone] = oAppTimer.timeMs;
                            gVerification.riseCount[zone]++;
                        }
                    }
                    gVerification.edges[zone]++;
                }
                if ((red != gVerification.lastRed[zone]) || (green != gVerification.lastGreen[zone]) || (blue != gVerification.lastBlue[zone]))
                {
                    gVerification.changes[zone]++;
                }
            }
            gVerification.lastRed[zone] = red;
            gVerification.lastGreen[zone] = green;
            gVerification.lastBlue[zone] = blue;
            gVerification.lastOn[zone] = on;
        }
        gVerification.samples++;
        gVerification.lastTimeMs = oAppTimer.timeMs;
    }
    if ((gVerification.EnableFlag == VERIFICATION_FADE) && (gVerification.fadePointCount < VERIFICATION_FADE_POINT_COUNT) && ((uint32_t)(oAppTimer.timeMs - gVerification.startTimeMs) >= ((uint32_t)gVerification.fadePointCount + 1U) * 100U))
    {
        fadePoint = gVerification.fadePointCount;
        for (zone = 0U; zone < VERIFICATION_ZONE_COUNT; zone++)
        {
            if ((gVerification.zone != VERIFICATION_ALL_ZONES) && (zone != gVerification.zone))
            {
                continue;
            }
            gVerification.fadeStatus[zone][fadePoint] = Verification_Read(zone, 0U, (uint8_t)((gVerification.zone != VERIFICATION_ALL_ZONES) || (zone == 0U) || (zone >= SRGB_ZONE_COUNT)));
            memcpy(gVerification.fadeExpected[zone][fadePoint], gVerification.expected, sizeof(gVerification.expected));
            memcpy(gVerification.fadeActual[zone][fadePoint], gVerification.actual, sizeof(gVerification.actual));
            gVerification.fadeTimeMs[zone][fadePoint] = (uint16_t)(gVerification.readTimeMs - gVerification.startTimeMs);
            gVerification.fadeReadMs[zone][fadePoint] = gVerification.readDurationMs;
        }
        gVerification.fadePointCount++;
    }
    if ((gVerification.EnableFlag != VERIFICATION_READ) && (gVerification.EnableFlag != VERIFICATION_READ_RGB) && (oAppTimer.tick10ms < 10U))
    {
        return;
    }

    if ((gVerification.EnableFlag == VERIFICATION_READ) || (gVerification.EnableFlag == VERIFICATION_READ_RGB))
    {
        if (gVerification.fresh != 0U)
        {
            gVerification.readStatus = Verification_Read(gVerification.zone, gVerification.node, 1U);
        }
        result[0U] = 0x81U;
        result[1U] = gVerification.zone;
        result[2U] = gVerification.node;
        result[3U] = gVerification.readStatus;
        result[4U] = (uint8_t)(gVerification.expected[gVerification.color] >> 8U);
        result[5U] = (uint8_t)gVerification.expected[gVerification.color];
        result[6U] = (uint8_t)(gVerification.actual[gVerification.color] >> 8U);
        result[7U] = (uint8_t)gVerification.actual[gVerification.color];
        if (gVerification.EnableFlag == VERIFICATION_READ_RGB)
        {
            result[0U] = result[3U];
            result[1U] = (uint8_t)(gVerification.actual[0U] >> 8U);
            result[2U] = (uint8_t)gVerification.actual[0U];
            result[3U] = (uint8_t)(gVerification.actual[1U] >> 8U);
            result[4U] = (uint8_t)gVerification.actual[1U];
            result[5U] = (uint8_t)(gVerification.actual[2U] >> 8U);
            result[6U] = (uint8_t)gVerification.actual[2U];
            result[7U] = gVerification.sequence;
        }
    }
    else if (((gVerification.EnableFlag == VERIFICATION_FLICKER) || ((gVerification.EnableFlag == VERIFICATION_FADE) && (gVerification.fadePointCount == VERIFICATION_FADE_POINT_COUNT))) && ((uint32_t)(oAppTimer.timeMs - gVerification.startTimeMs) >= gVerification.captureMs))
    {
        zone = (gVerification.zone == VERIFICATION_ALL_ZONES) ? 0U : gVerification.zone;
        result[0U] = (gVerification.EnableFlag == VERIFICATION_FLICKER) ? 0x82U : 0x83U;
        result[1U] = zone;
        if (gVerification.EnableFlag == VERIFICATION_FLICKER)
        {
            result[2U] = gVerification.minPeriodMs[zone];
            result[3U] = gVerification.maxPeriodMs[zone];
            result[4U] = gVerification.minOnMs[zone];
            result[5U] = gVerification.maxOnMs[zone];
            result[6U] = gVerification.minOffMs[zone];
            result[7U] = gVerification.maxOffMs[zone];
        }
        else
        {
            result[2U] = (uint8_t)(gVerification.samples >> 8U);
            result[3U] = (uint8_t)gVerification.samples;
            result[4U] = (uint8_t)(gVerification.changes[zone] >> 8U);
            result[5U] = (uint8_t)gVerification.changes[zone];
            result[6U] = (uint8_t)(gVerification.edges[zone] >> 8U);
            result[7U] = (uint8_t)gVerification.edges[zone];
        }
    }
    else
    {
        return;
    }

    irqState = __get_PRIMASK();
    __disable_irq();
    memcpy(gVerification.response, result, sizeof(result));
    if ((gVerification.EnableFlag == VERIFICATION_FLICKER) || (gVerification.EnableFlag == VERIFICATION_FADE))
    {
        gVerification.captureReady = 1U;
    }
    gVerification.EnableFlag = 0U;
    __set_PRIMASK(irqState);
}
