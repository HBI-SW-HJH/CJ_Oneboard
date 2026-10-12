#include "appConfig.h"
#include <assert.h>
#include <stdio.h>

SRGB_Control_t gSRGBControl;
OSP_Control_t gOSPControl;
VerificationHostTimer_t oAppTimer;
VerificationHostPower_t gPowerData;
static uint8_t faults[4];
static uint8_t ospMissing[2];
static uint16_t rxDifference;
static unsigned srgbReads;
static unsigned ospReads;
static unsigned timerResetCalls;

void AppTimer_ResetReadCount(void)
{
    assert(gVerification.ActiveFlag == 1U);
    timerResetCalls++;
}

void SRGBControl_Read(void)
{
    srgbReads++;
    memcpy(gSRGBControl.rxImage, gSRGBControl.txImage, sizeof(gSRGBControl.rxImage));
    gSRGBControl.rxImage[1][0] -= rxDifference;
    gSRGBControl.dynamicValid = 1;
    oAppTimer.timeMs++;
}
uint8_t SRGBControl_GetFaultMask(SRGB_FaultPart_t zone) { return faults[zone]; }
void OSPControl_Read(OSP_Port_t port)
{
    uint8_t node = gOSPControl.readNode[port];
    ospReads++;
    gOSPControl.dynamicValid[port][node] = !ospMissing[port];
    if (!ospMissing[port])
    {
        gOSPControl.rxPwmRed[port][node] = gOSPControl.pwmRed[port];
        gOSPControl.rxPwmGreen[port][node] = gOSPControl.pwmGreen[port];
        gOSPControl.rxPwmBlue[port][node] = gOSPControl.pwmBlue[port];
    }
    oAppTimer.timeMs += ospMissing[port] ? 7 : 1;
}

static void Request(uint8_t command, uint8_t zone, uint8_t node, uint8_t color, uint8_t fresh, uint16_t duration)
{
    uint8_t data[8] = { command, zone, node, color, fresh, (uint8_t)duration, (uint8_t)(duration >> 8), 0 };
    Verification_Request(data);
}

static void Reset(void)
{
    Verification_Init();
    memset(&gSRGBControl, 0, sizeof(gSRGBControl));
    memset(&gOSPControl, 0, sizeof(gOSPControl));
    memset(faults, 0, sizeof(faults));
    memset(ospMissing, 0, sizeof(ospMissing));
    memset(&gPowerData, 0, sizeof(gPowerData));
    rxDifference = 0;
    srgbReads = ospReads = 0;
    oAppTimer.timeMs = 0;
    oAppTimer.tick10ms = 10;
    gOSPControl.nodeCount[0] = 24;
    gOSPControl.nodeCount[1] = 8;
    gSRGBControl.txImage[1][0] = 10000;
    gSRGBControl.outputRed[0] = 12000;
    gOSPControl.pwmRed[0] = 5000;
    gOSPControl.outputRed[0] = 18000;
}

int main(void)
{
    Reset();
    Request(VERIFICATION_FADE, 6, 0, 0, 0, 0);
    oAppTimer.timeMs = 100;
    Verification_Process();
    assert(gVerification.fadePointCount == 1);
    assert(srgbReads == 1 && ospReads == 2);
    assert(gVerification.fadeExpected[0][0][0] == 10000);
    assert(gVerification.fadeActual[0][0][0] == 10000);
    assert(gVerification.fadeExpected[4][0][0] == 5000);
    assert(gVerification.fadeActual[4][0][0] == 5000);
    assert(gVerification.fadeReadMs[0][0] == 1);
    assert(gVerification.fadeTimeMs[4][0] == 101);
    puts("PASS: actual TX snapshot (not next computed value) vs hardware RX; one shared SRGB read.");

    rxDifference = 300;
    faults[3] = 8;
    ospMissing[0] = 1;
    oAppTimer.timeMs = 205;
    Verification_Process();
    assert(gVerification.fadeActual[0][1][0] == 9700);
    assert((gVerification.fadeStatus[3][1] & 0x10) != 0);
    assert((gVerification.fadeStatus[4][1] & 0x41) == 0x40);
    assert((gVerification.fadeStatus[5][1] & 0xF1) == 1);
    assert(gVerification.fadeTimeMs[0][1] == 205);
    puts("PASS: injected mismatch retained; Back Open and Logo no-response fail independently; actual capture time retained.");

    gOSPControl.nodeCount[1] = 0;
    gOSPControl.dynamicValid[1][0] = 1;
    oAppTimer.timeMs = 300;
    Verification_Process();
    assert((gVerification.fadeStatus[5][2] & 0x41) == 0x40);
    for (unsigned point = 4; point <= 10; point++)
    {
        oAppTimer.timeMs = point * 100 + 1;
        Verification_Process();
    }
    assert(gVerification.captureReady == 1 && gVerification.fadePointCount == 10);
    Request(VERIFICATION_FADE_STATUS, 3, 1, 0, 0, 0);
    assert(gVerification.response[0] == 0x89 && (gVerification.response[3] & 0x10) != 0);
    assert(((gVerification.response[4] << 8) | gVerification.response[5]) == 205);
    puts("PASS: skipped exact 100ms boundary still captures ten points; stale valid/no-node cannot pass; metadata packet matches.");

    Reset();
    Request(VERIFICATION_READ, 0, 0, 0, 1, 0);
    Verification_Process();
    assert((gVerification.response[3] & 0xF1) == 1);
    Request(VERIFICATION_READ, 0, 0, 1, 0, 0);
    Verification_Process();
    assert((gVerification.response[3] & 0xF1) == 1 && srgbReads == 1);
    puts("PASS: RGB cached response status survives new requests without repeating hardware read.");

    Reset();
    oAppTimer.tick10ms = 0;
    rxDifference = 300;
    Request(VERIFICATION_READ_RGB, 0, 0, 0, 1, 17);
    Verification_Process();
    assert(gVerification.EnableFlag == 0 && srgbReads == 1);
    assert(gVerification.response[7] == 17);
    assert(((gVerification.response[1] << 8) | gVerification.response[2]) == 9700);
    Request(VERIFICATION_READ_EXPECTED, 0, 0, 0, 0, 18);
    assert(gVerification.response[7] == 18 && srgbReads == 1);
    assert(((gVerification.response[1] << 8) | gVerification.response[2]) == 10000);
    Request(VERIFICATION_READ_EXPECTED, 4, 0, 0, 0, 19);
    assert(gVerification.response[7] == 18);
    gVerification.captureReady = 1;
    gVerification.captureMode = VERIFICATION_FADE;
    gVerification.fadeStatus[4][3] = 0x41;
    for (unsigned color = 0; color < 3; color++)
    {
        gVerification.fadeExpected[4][3][color] = (uint16_t)(1000 + color * 100);
        gVerification.fadeActual[4][3][color] = (uint16_t)(900 + color * 100);
    }
    Request(VERIFICATION_FADE_RGB, 4, 3, 0, 0, 20);
    assert(gVerification.response[0] == 0x41 && gVerification.response[7] == 20);
    assert(((gVerification.response[5] << 8) | gVerification.response[6]) == 1200);
    Request(VERIFICATION_FADE_RGB, 4, 3, 1, 0, 21);
    assert(gVerification.response[0] == 0x41 && gVerification.response[7] == 21);
    assert(((gVerification.response[5] << 8) | gVerification.response[6]) == 1100);
    Request(VERIFICATION_FADE_RGB, 4, 10, 1, 0, 22);
    assert(gVerification.response[7] == 21 && srgbReads == 1 && ospReads == 0);
    puts("PASS: compact RGB keeps all TX/RX words, status and sequence; invalid targets rejected; no added/skipped LED measurements.");

    for (unsigned hz = 10; hz <= 50; hz += 40)
    {
        for (unsigned duty = 10; duty <= 90; duty += 80)
        {
            Reset();
            unsigned period = 1000 / hz;
            unsigned on = period * duty / 100;
            Request(VERIFICATION_FLICKER, 6, 0, 0, 0, (uint16_t)(4 * period));
            for (unsigned time = 1; time <= 4 * period; time++)
            {
                oAppTimer.timeMs = time;
                gSRGBControl.txImage[1][0] = time % period < on ? 1000 : 0;
                Verification_Process();
            }
            assert(gVerification.captureReady == 1);
            assert(gVerification.minPeriodMs[0] == period && gVerification.maxPeriodMs[0] == period);
            assert(gVerification.minOnMs[0] == on && gVerification.maxOnMs[0] == on);
            assert(gVerification.minOffMs[0] == period - on && gVerification.maxOffMs[0] == period - on);
            assert(srgbReads == 0 && ospReads == 0);
        }
    }
    puts("PASS: 10/50Hz x 10/90% timing captures actual TX state changes, no read overhead during flicker.");
    Request(VERIFICATION_START, 0, 0, 0, 0, 0);
    assert(timerResetCalls == 0U && gVerification.ActiveFlag == 1U);
    Request(VERIFICATION_STOP, 0, 0, 0, 0, 0);
    assert(timerResetCalls == 1U && gVerification.ActiveFlag == 0U && gVerification.EnableFlag == 0U && gVerification.response[0] == 0x84U);
    puts("PASS: Stop resets diagnostic timers before releasing ActiveFlag; response and capture shutdown preserved.");
    return 0;
}
