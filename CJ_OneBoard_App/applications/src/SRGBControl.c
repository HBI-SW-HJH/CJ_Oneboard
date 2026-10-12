// @file    SRGBControl.c
// @brief   AL58263 two-device sRGB LED control

#include "SRGBControl.h"
#include "power_device.h"
#include "sys_init.h"
#include "device.h"
#include <stddef.h>
#include <string.h>
// Immediate frame latch; disable zero-data sleep (up to 4ms wake-up).

SRGB_Control_t gSRGBControl;

static void SRGB_SetPin(uint8_t pin, uint8_t high)
{
    if (high != 0U)
    {
        PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTSET = (1UL << pin);
    }
    else
    {
        PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTCLR = (1UL << pin);
    }
}

#if defined(__ICCARM__)
#pragma inline=forced
#endif
static __INLINE uint8_t SRGB_ClockBit(uint8_t txBit, uint8_t latHigh)
{
    uint32_t highMask = ((txBit != 0U) ? (1UL << SRGB_DI_PIN) : 0U) | ((latHigh != 0U) ? (1UL << SRGB_LAT_PIN) : 0U);
    uint8_t rxBit;

    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTCLR = ((1UL << SRGB_DI_PIN) | (1UL << SRGB_LAT_PIN)) & ~highMask;
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTSET = highMask;
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTSET = (1UL << SRGB_DCK_PIN);
    // DO propagation <=55ns at 3.3V. Three CPU cycles >=62.5ns at 48MHz.
    __NOP();
    __NOP();
    __NOP();
    rxBit = ((PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_IN & (1UL << SRGB_DO_PIN)) != 0U) ? 1U : 0U;
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTCLR = (1UL << SRGB_DCK_PIN);
    return rxBit;
}

static void SRGB_Instruction(uint8_t clocks)
{
    uint8_t index;

    for (index = 0U; index < clocks; index++)
    {
        (void)SRGB_ClockBit(0U, 1U);
    }
    SRGB_SetPin(SRGB_LAT_PIN, 0U);
}

static void SRGB_ShiftPair(uint16_t downstream, uint16_t upstream, uint8_t latchClocks)
{
    uint8_t bit;
    uint32_t data = ((uint32_t)downstream << 16U) | upstream;

    for (bit = 32U; bit > 0U; bit--)
    {
        (void)SRGB_ClockBit((uint8_t)((data >> (bit - 1U)) & 1U), (uint8_t)(bit <= latchClocks));
    }
    SRGB_SetPin(SRGB_LAT_PIN, 0U);
}

static void SRGB_WriteImage(void)
{
    uint8_t output;
    uint8_t latchClocks;

    for (output = SRGB_CHANNEL_COUNT; output > 0U; output--)
    {
        latchClocks = (output == 1U) ? SRGB_INSTR_GLOBAL_LATCH : SRGB_INSTR_DATA_LATCH;
        SRGB_ShiftPair(gSRGBControl.txImage[1U][output - 1U], gSRGBControl.txImage[0U][output - 1U], latchClocks);
    }
}

static uint16_t SRGB_ReadWord(void)
{
    uint8_t bit;
    uint16_t value = 0U;

    // DI/LAT are already low in read mode; do not rewrite them every bit.
    for (bit = 0U; bit < 16U; bit++)
    {
        PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTSET = (1UL << SRGB_DCK_PIN);
        __NOP(); __NOP(); __NOP();
        value = (uint16_t)((value << 1U) | ((PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_IN >> SRGB_DO_PIN) & 1U));
        PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTCLR = (1UL << SRGB_DCK_PIN);
    }
    return value;
}

static uint8_t SRGB_ReadImageAndCommand(void)
{
    uint8_t device;
    uint8_t output;

    SRGB_Instruction(SRGB_INSTR_READ_DATA);
    for (device = SRGB_DEVICE_COUNT; device > 0U; device--)
    {
        gSRGBControl.rxCommandWord[device - 1U] = SRGB_ReadWord();
        for (output = SRGB_CHANNEL_COUNT; output > 0U; output--)
        {
            gSRGBControl.rxImage[device - 1U][SRGB_CHANNEL_COUNT - output] = SRGB_ReadWord();
        }
    }
    SRGB_Instruction(SRGB_INSTR_RETURN_NORMAL);
    return (uint8_t)((gSRGBControl.rxCommandWord[0U] == gSRGBControl.txCommandWord[0U]) && (gSRGBControl.rxCommandWord[1U] == gSRGBControl.txCommandWord[1U]));
}

static void SRGB_ReadDiagnostic(uint8_t instruction, uint16_t result[SRGB_DEVICE_COUNT])
{
    uint8_t index;
    uint8_t device;
    uint16_t value;

    // Error mode: 14 DCK pulses with LAT high, then N DI pulses with DCK low.
    // Keep LAT high until all DI pulses have propagated through the chain.
    for (index = 0U; index < SRGB_INSTR_ERROR_MODE; index++)
    {
        (void)SRGB_ClockBit(0U, 1U);
    }
    for (index = 0U; index < instruction; index++)
    {
        SRGB_SetPin(SRGB_DI_PIN, 1U);
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        SRGB_SetPin(SRGB_DI_PIN, 0U);
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
    }
    SRGB_SetPin(SRGB_LAT_PIN, 0U);
    // Minimum detection time: 180ns for short-to-GND, 100ns for LED errors.
    __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

    // Thermal and REXT results are valid before the first read clock.
    if ((instruction == SRGB_DIAG_THERMAL_WATCHDOG) || (instruction == SRGB_DIAG_REXT_WATCHDOG))
    {
        for (device = SRGB_DEVICE_COUNT; device > 0U; device--)
        {
            value = 0U;
            for (index = 0U; index < SRGB_CHANNEL_COUNT; index++)
            {
                value = (uint16_t)((value << 1U) | ((PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_IN >> SRGB_DO_PIN) & 1U));
                PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTSET = (1UL << SRGB_DCK_PIN);
                __NOP(); __NOP(); __NOP();
                PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_OUTCLR = (1UL << SRGB_DCK_PIN);
            }
            result[device - 1U] = (uint16_t)(~value);
        }
    }
    else
    {
        result[1U] = (uint16_t)(~SRGB_ReadWord());
        result[0U] = (uint16_t)(~SRGB_ReadWord());
        if ((instruction <= SRGB_DIAG_SHORT_GROUND) || (instruction == SRGB_DIAG_SMART))
        {
            result[1U] = (uint16_t)((result[1U] >> 1U) | (uint16_t)(result[1U] << 15U));
            result[0U] = (uint16_t)((result[0U] >> 1U) | (uint16_t)(result[0U] << 15U));
        }
    }
    SRGB_Instruction(SRGB_INSTR_RETURN_NORMAL);
}

static uint16_t SRGB_Interpolate(uint16_t start, uint16_t target, uint16_t elapsed, uint16_t duration)
{
    int32_t delta;
    int32_t value;

    if ((duration == 0U) || (elapsed >= duration))
    {
        return target;
    }
    delta = (int32_t)target - (int32_t)start;
    value = (int32_t)start + ((delta * (int32_t)elapsed) / (int32_t)duration);
    return (uint16_t)value;
}

void SRGBControl_Set(const LedControlCommand_t *command)
{
    uint8_t zone;
    uint16_t onTimeMs;

    if (command == NULL)
    {
        return;
    }

    for (zone = 0U; zone < SRGB_ZONE_COUNT; zone++)
    {
        if ((command->ledMask & (uint8_t)(1U << zone)) == 0U)
        {
            continue;
        }

        // A cyclic LIN command must not restart Fade/Flicker unless Sync is set.
        if ((command->sync == 0U) && (gSRGBControl.mode[zone] == command->mode) && (gSRGBControl.active[zone] == command->active) && (gSRGBControl.fadeEnabled[zone] == command->fadeEnabled) && (gSRGBControl.effect1[zone] == command->effect1) && (gSRGBControl.effect2[zone] == command->effect2) && (gSRGBControl.targetRed[zone] == command->red) && (gSRGBControl.targetGreen[zone] == command->green) && (gSRGBControl.targetBlue[zone] == command->blue))
        {
            continue;
        }

        gSRGBControl.startRed[zone] = gSRGBControl.fadedRed[zone];
        gSRGBControl.startGreen[zone] = gSRGBControl.fadedGreen[zone];
        gSRGBControl.startBlue[zone] = gSRGBControl.fadedBlue[zone];
        gSRGBControl.targetRed[zone] = command->red;
        gSRGBControl.targetGreen[zone] = command->green;
        gSRGBControl.targetBlue[zone] = command->blue;
        gSRGBControl.mode[zone] = command->mode;
        gSRGBControl.active[zone] = command->active;
        gSRGBControl.fadeEnabled[zone] = command->fadeEnabled;
        gSRGBControl.effect1[zone] = command->effect1;
        gSRGBControl.effect2[zone] = command->effect2;
        gSRGBControl.startTimeMs[zone] = command->timeMs;
        gSRGBControl.phaseTimeMs[zone] = command->timeMs;
        if ((command->mode != 6U) || (command->effect2 == 0U))
        {
            gSRGBControl.onThreshold[zone] = 0U;
        }
        else if (command->effect1 >= 250U)
        {
            gSRGBControl.onThreshold[zone] = 1000U;
        }
        else
        {
            onTimeMs = (uint16_t)((((uint32_t)command->effect1 * 1000U) + ((uint32_t)command->effect2 * 125U)) / ((uint32_t)command->effect2 * 250U));
            gSRGBControl.onThreshold[zone] = (uint16_t)(onTimeMs * command->effect2);
        }
        gSRGBControl.flickerPhase[zone] = 0U;
        if ((command->fadeEnabled != 0U) && (command->mode == 7U))
        {
            gSRGBControl.fadeInMs[zone] = (uint16_t)((uint16_t)command->effect1 * 100U);
            gSRGBControl.fadeOutMs[zone] = (uint16_t)((uint16_t)command->effect2 * 100U);
        }
        if (command->fadeEnabled != 0U)
        {
            gSRGBControl.durationMs[zone] = (command->active != 0U) ? gSRGBControl.fadeInMs[zone] : gSRGBControl.fadeOutMs[zone];
        }
        else
        {
            gSRGBControl.durationMs[zone] = 0U;
        }
    }
}

void SRGBControl_Init(void)
{
    uint8_t device;
    uint8_t zone;

    (void)memset(&gSRGBControl, 0, sizeof(gSRGBControl));
    for (zone = 0U; zone < SRGB_ZONE_COUNT; zone++)
    {
        gSRGBControl.fadeInMs[zone] = LED_FLICKER_FADE_TIME_MS;
        gSRGBControl.fadeOutMs[zone] = LED_FLICKER_FADE_TIME_MS;
    }
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_PINCFG[SRGB_DI_PIN] &= (uint8_t)(~(PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_INEN_Msk));
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_PINCFG[SRGB_DCK_PIN] &= (uint8_t)(~(PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_INEN_Msk));
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_PINCFG[SRGB_LAT_PIN] &= (uint8_t)(~(PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_INEN_Msk));
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_DIRSET = (1UL << SRGB_DI_PIN) | (1UL << SRGB_DCK_PIN) | (1UL << SRGB_LAT_PIN);
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_PINCFG[SRGB_DO_PIN] &= (uint8_t)(~PORT_PINCFG_PMUXEN_Msk);
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_PINCFG[SRGB_DO_PIN] |= (uint8_t)PORT_PINCFG_INEN_Msk;
    PORT_REGS->GROUP[SRGB_PIN_GROUP].PORT_DIRCLR = (1UL << SRGB_DO_PIN);
    SRGB_SetPin(SRGB_DCK_PIN, 0U);
    SRGB_SetPin(SRGB_LAT_PIN, 0U);

    for (device = 0U; device < SRGB_DEVICE_COUNT; device++)
    {
        gSRGBControl.txCommandWord[device] = SRGB_COMMAND_DEFAULT;
    }
    SRGB_ShiftPair(gSRGBControl.txCommandWord[1U], gSRGBControl.txCommandWord[0U], SRGB_INSTR_WRITE_COMMAND);
    SRGB_WriteImage();
    if (SRGB_ReadImageAndCommand() == 0U)
    {
        gSRGBControl.communicationErrorCount++;
    }
}

void SRGBControl_Process(void)
{
    uint8_t zone;
    uint8_t device;
    uint8_t offset;
    uint32_t nowMs;
    uint32_t phaseTicks;
    uint32_t phase;
    uint32_t elapsedMs;
    uint16_t fadeElapsedMs;

    nowMs = oAppTimer.timeMs;
    for (zone = 0U; zone < SRGB_ZONE_COUNT; zone++)
    {
        if (gSRGBControl.mode[zone] == 6U)
        {
            phaseTicks = nowMs - gSRGBControl.phaseTimeMs[zone];
            gSRGBControl.phaseTimeMs[zone] = nowMs;
            if (phaseTicks >= 1000U)
            {
                phaseTicks %= 1000U;
            }
            phase = gSRGBControl.flickerPhase[zone] + (phaseTicks * gSRGBControl.effect2[zone]);
            if (phase >= 1000U)
            {
                phase %= 1000U;
            }
            gSRGBControl.flickerPhase[zone] = (uint16_t)phase;
            gSRGBControl.outputRed[zone] = gSRGBControl.fadedRed[zone];
            gSRGBControl.outputGreen[zone] = gSRGBControl.fadedGreen[zone];
            gSRGBControl.outputBlue[zone] = gSRGBControl.fadedBlue[zone];
            if ((gSRGBControl.effect2[zone] == 0U) || (gSRGBControl.flickerPhase[zone] >= gSRGBControl.onThreshold[zone]))
            {
                gSRGBControl.outputRed[zone] = 0U;
                gSRGBControl.outputGreen[zone] = 0U;
                gSRGBControl.outputBlue[zone] = 0U;
            }
        }

        device = (zone < SRGB_ZONE_HEADREST) ? 1U : 0U;
        offset = ((zone == SRGB_ZONE_FRT_RH) || (zone == SRGB_ZONE_HEADREST)) ? 2U : 0U;
        if ((gSRGBControl.txImage[device][offset] != gSRGBControl.outputRed[zone]) || (gSRGBControl.txImage[device][offset + 1U] != gSRGBControl.outputRed[zone]) || (gSRGBControl.txImage[device][offset + 4U] != gSRGBControl.outputGreen[zone]) || (gSRGBControl.txImage[device][offset + 5U] != gSRGBControl.outputGreen[zone]) || (gSRGBControl.txImage[device][offset + 8U] != gSRGBControl.outputBlue[zone]) || (gSRGBControl.txImage[device][offset + 9U] != gSRGBControl.outputBlue[zone]))
        {
            gSRGBControl.txImage[device][offset] = gSRGBControl.outputRed[zone];
            gSRGBControl.txImage[device][offset + 1U] = gSRGBControl.outputRed[zone];
            gSRGBControl.txImage[device][offset + 4U] = gSRGBControl.outputGreen[zone];
            gSRGBControl.txImage[device][offset + 5U] = gSRGBControl.outputGreen[zone];
            gSRGBControl.txImage[device][offset + 8U] = gSRGBControl.outputBlue[zone];
            gSRGBControl.txImage[device][offset + 9U] = gSRGBControl.outputBlue[zone];
            gSRGBControl.imageDirty = 1U;
        }
    }

    if (gSRGBControl.imageDirty != 0U)
    {
        SRGB_WriteImage();
        gSRGBControl.imageDirty = 0U;
    }

    if (oAppTimer.tick10ms < 10U)
    {
        return;
    }

    for (zone = 0U; zone < SRGB_ZONE_COUNT; zone++)
    {
        elapsedMs = oAppTimer.timeMs - gSRGBControl.startTimeMs[zone];
        fadeElapsedMs = (elapsedMs >= gSRGBControl.durationMs[zone]) ? gSRGBControl.durationMs[zone] : (uint16_t)elapsedMs;
        gSRGBControl.fadedRed[zone] = SRGB_Interpolate(gSRGBControl.startRed[zone], gSRGBControl.targetRed[zone], fadeElapsedMs, gSRGBControl.durationMs[zone]);
        gSRGBControl.fadedGreen[zone] = SRGB_Interpolate(gSRGBControl.startGreen[zone], gSRGBControl.targetGreen[zone], fadeElapsedMs, gSRGBControl.durationMs[zone]);
        gSRGBControl.fadedBlue[zone] = SRGB_Interpolate(gSRGBControl.startBlue[zone], gSRGBControl.targetBlue[zone], fadeElapsedMs, gSRGBControl.durationMs[zone]);

        if (gSRGBControl.mode[zone] != 6U)
        {
            gSRGBControl.outputRed[zone] = gSRGBControl.fadedRed[zone];
            gSRGBControl.outputGreen[zone] = gSRGBControl.fadedGreen[zone];
            gSRGBControl.outputBlue[zone] = gSRGBControl.fadedBlue[zone];
        }
    }
}

void SRGBControl_Read(void)
{
    SRGB_ReadDiagnostic(SRGB_DIAG_OPEN, gSRGBControl.openFault);
    SRGB_ReadDiagnostic(SRGB_DIAG_SHORT, gSRGBControl.shortFault);
    SRGB_ReadDiagnostic(SRGB_DIAG_SHORT_GROUND, gSRGBControl.shortGroundFault);
    SRGB_ReadDiagnostic(SRGB_DIAG_THERMAL_WATCHDOG, gSRGBControl.thermalWatchdogFault);
    SRGB_ReadDiagnostic(SRGB_DIAG_SMART, gSRGBControl.smartFault);
    SRGB_ReadDiagnostic(SRGB_DIAG_REXT_WATCHDOG, gSRGBControl.rextWatchdogFault);
    gSRGBControl.dynamicValid = SRGB_ReadImageAndCommand();
    if (gSRGBControl.dynamicValid == 0U)
    {
        gSRGBControl.communicationErrorCount++;
    }
}

uint8_t SRGBControl_GetFaultMask(SRGB_FaultPart_t part)
{
    uint8_t device;
    uint16_t channelMask;
    uint8_t result = 0U;

    if (part == SRGB_FAULT_PART_FRT_LH)
    {
        device = 1U;
        channelMask = 0x0333U;
    }
    else if (part == SRGB_FAULT_PART_FRT_RH)
    {
        device = 1U;
        channelMask = 0x0CCCU;
    }
    else if (part == SRGB_FAULT_PART_HEADREST)
    {
        device = 0U;
        channelMask = 0x0CCCU;
    }
    else if (part == SRGB_FAULT_PART_BACK)
    {
        device = 0U;
        channelMask = 0x0333U;
    }
    else
    {
        return 0U;
    }

    // Thermal warning is chip-wide (bit15); bit0 is the GCK watchdog result.
    if ((gSRGBControl.thermalWatchdogFault[device] & 0x8000U) != 0U)
    {
        result |= 0x01U;
    }
    if (gPowerData.voltageFault != 0U)
    {
        result |= 0x02U;
    }
    if (((gSRGBControl.shortFault[device] | gSRGBControl.shortGroundFault[device]) & channelMask) != 0U)
    {
        result |= 0x04U;
    }
    if ((gSRGBControl.openFault[device] & channelMask) != 0U)
    {
        result |= 0x08U;
    }
    if (gSRGBControl.dynamicValid == 0U)
    {
        result |= 0x10U;
    }
    return result;
}
