// @file    power_device.h
// @brief   PA00 +5V_LED_FLT digital power-good monitor

#ifndef POWER_DEVICE_H
#define POWER_DEVICE_H

#include <stdint.h>

#define POWER_FAULT_GROUP    GPIO_GROUP_A
#define POWER_FAULT_PIN      0U
#define POWER_ENABLE_PIN     1U

typedef struct
{
    uint8_t faultPinHigh;
    uint8_t voltageFault;
} Power_Data_t;

extern Power_Data_t gPowerData;

void Power_Init(void);
void Power_Read(void);

#endif // POWER_DEVICE_H
