// @file    gpio_device.h
// @brief   GPIO driver using Microchip DFP PORT registers
//
// @copyright 2026 CJ 4DPLEX

#ifndef GPIO_DEVICE_H
#define GPIO_DEVICE_H

#include "device.h"

#define GPIO_GROUP_A    0U
#define GPIO_GROUP_B    1U

typedef enum { GPIO_DIR_INPUT = 0U, GPIO_DIR_OUTPUT = 1U } GPIO_Direction_t;
typedef enum { GPIO_PULL_NONE = 0U, GPIO_PULL_UP = 1U, GPIO_PULL_DOWN = 2U } GPIO_Pull_t;
typedef enum { GPIO_LOW = 0U, GPIO_HIGH = 1U } GPIO_Level_t;

//  Configure a pin as GPIO input or output
void GPIO_Init(uint8_t group, uint8_t pin, GPIO_Direction_t dir, GPIO_Pull_t pull);

//  Set output pin level
void GPIO_Set(uint8_t group, uint8_t pin, GPIO_Level_t level);

//  Enable peripheral MUX on a pin (PMUXEN + set PMUX nibble)
void GPIO_SetPmux(uint8_t group, uint8_t pin, uint8_t pmux);

//  Enable input buffer on a peripheral-muxed pin (needed for SERCOM RX/MISO)
void GPIO_EnableInput(uint8_t group, uint8_t pin);

#endif //  GPIO_DEVICE_H
