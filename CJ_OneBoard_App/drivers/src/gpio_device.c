// @file    gpio_device.c
// @brief   GPIO driver — Microchip DFP PORT_REGS / port_registers_t
//
// @copyright 2026 CJ 4DPLEX

#include "gpio_device.h"

void GPIO_Init(uint8_t group, uint8_t pin, GPIO_Direction_t dir, GPIO_Pull_t pull)
{
    uint32_t mask = (1UL << pin);

    //  Disable PMUX → pure GPIO
    PORT_REGS->GROUP[group].PORT_PINCFG[pin] &= (uint8_t)(~PORT_PINCFG_PMUXEN_Msk);

    if (dir == GPIO_DIR_OUTPUT)
    {
        PORT_REGS->GROUP[group].PORT_DIRSET = mask;
        PORT_REGS->GROUP[group].PORT_PINCFG[pin] &= (uint8_t)(~PORT_PINCFG_INEN_Msk);
    }
    else
    {
        PORT_REGS->GROUP[group].PORT_DIRCLR = mask;
        PORT_REGS->GROUP[group].PORT_PINCFG[pin] |= (uint8_t)PORT_PINCFG_INEN_Msk;

        if (pull == GPIO_PULL_UP)
        {
            PORT_REGS->GROUP[group].PORT_PINCFG[pin] |= (uint8_t)PORT_PINCFG_PULLEN_Msk;
            PORT_REGS->GROUP[group].PORT_OUTSET = mask;  //  OUT=1 → pull-up
        }
        else if (pull == GPIO_PULL_DOWN)
        {
            PORT_REGS->GROUP[group].PORT_PINCFG[pin] |= (uint8_t)PORT_PINCFG_PULLEN_Msk;
            PORT_REGS->GROUP[group].PORT_OUTCLR = mask;  //  OUT=0 → pull-down
        }
        else
        {
            PORT_REGS->GROUP[group].PORT_PINCFG[pin] &= (uint8_t)(~PORT_PINCFG_PULLEN_Msk);
        }
    }
}

void GPIO_Set(uint8_t group, uint8_t pin, GPIO_Level_t level)
{
    if (level == GPIO_HIGH)
    {
        PORT_REGS->GROUP[group].PORT_OUTSET = (1UL << pin);
    }
    else
    {
        PORT_REGS->GROUP[group].PORT_OUTCLR = (1UL << pin);
    }
}

GPIO_Level_t GPIO_Read(uint8_t group, uint8_t pin)
{
    return ((PORT_REGS->GROUP[group].PORT_IN & (1UL << pin)) != 0U) ? GPIO_HIGH : GPIO_LOW;
}

void GPIO_SetPmux(uint8_t group, uint8_t pin, uint8_t pmux)
{
    //  Each PMUX byte covers two pins: even → lower nibble, odd → upper nibble
    uint8_t idx = pin >> 1U;

    if ((pin & 0x01U) == 0U)   //  even
    {
        PORT_REGS->GROUP[group].PORT_PMUX[idx] = (uint8_t)((PORT_REGS->GROUP[group].PORT_PMUX[idx] & 0xF0U) | (pmux & 0x0FU));
    }
    else                       //  odd
    {
        PORT_REGS->GROUP[group].PORT_PMUX[idx] = (uint8_t)((PORT_REGS->GROUP[group].PORT_PMUX[idx] & 0x0FU) | ((pmux & 0x0FU) << 4U));
    }

    //  Enable peripheral mux
    PORT_REGS->GROUP[group].PORT_PINCFG[pin] |= (uint8_t)PORT_PINCFG_PMUXEN_Msk;
}

void GPIO_EnableInput(uint8_t group, uint8_t pin)
{
    PORT_REGS->GROUP[group].PORT_PINCFG[pin] |= (uint8_t)PORT_PINCFG_INEN_Msk;
}
