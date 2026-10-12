// @file    power_device.c
// @brief   PA00 +5V_LED_FLT digital power-good monitor

#include "appConfig.h"

Power_Data_t gPowerData;

void Power_Init(void)
{
    GPIO_Init(POWER_FAULT_GROUP, POWER_ENABLE_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);
    GPIO_Set(POWER_FAULT_GROUP, POWER_ENABLE_PIN, GPIO_HIGH);
    GPIO_Init(POWER_FAULT_GROUP, POWER_FAULT_PIN, GPIO_DIR_INPUT, GPIO_PULL_UP);
    Power_Read();
}

void Power_Read(void)
{
    gPowerData.faultPinHigh =(uint8_t)GPIO_Read(POWER_FAULT_GROUP, POWER_FAULT_PIN);
    gPowerData.voltageFault =(gPowerData.faultPinHigh != 0U) ? 0U : 1U;
}
