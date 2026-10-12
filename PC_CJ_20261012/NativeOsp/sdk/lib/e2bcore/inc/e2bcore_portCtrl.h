/*******************************************************************************
Copyright (c) 2021-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_portCtrl.h
 * @brief: External e2bcore interface of Port controller interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */
#ifndef E2BCORE_PORTCTRL_H
#define E2BCORE_PORTCTRL_H

/*================================= INCLUDES ================================*/
#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/*
 *                                     ________                     ________
 * ____________________________________|      |_____________________|      |___ ...
 * |<- Time_delay ->||<------ Time_period ------>|
 *                   |<- Pulse_delay ->|
 *                   |<-------- Pulse 1 -------->||<-------- Pulse 2 -------->| ...
 */

/**
 * @brief Port controller transaction config structure: { ADI_E2BCORE_TXN_HDR, ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD[] }
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_PORT_CTRL_TXN_CONFIG;

/**
 * @brief Enumeration of the different commands supported by port controller interface
 */
typedef enum ADI_E2BCORE_PORT_CTRL_CMD
{
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_SLOPE           = 0x0U,  /*!< PWM slope command */
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_PULSE_DELAY     = 0x1U,  /*!< PWM pulse delay command */
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY            = 0x2U,  /*!< PWM duty cycle command */
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_START           = 0x3U,  /*!< PWM start command */
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_STOP            = 0x4U,  /*!< PWM stop command */
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_GPIO_STEADY_VAL = 0x5U,  /*!< GPIO steady state value command */
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_MONITOR         = 0x6U,  /*!< PWM monitor command */
    ADI_E2BCORE_PORT_CTRL_CMD_TCH_SPEED           = 0x7U,  /*!< Tachometer speed command */
    ADI_E2BCORE_PORT_CTRL_CMD_PWM_CLOCK_SYNC      = 0x8U,  /*!< PWM clock sync command */
} ADI_E2BCORE_PORT_CTRL_CMD;

/*! @brief Port controller transaction payload type */
typedef struct ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD
{
    ADI_E2BCORE_PORT_CTRL_CMD ePortCtrlCmd; /*!< Port controller transaction command */
    uint8_t     nDuty;          /*!< Duty cycle of the PWM signal (0 indicates 0%, 0x80 indicates 100%)
                                     (Note: This is valid only for #ADI_E2BCORE_PORT_CTRL_CMD_PWM_DUTY command) */
    uint8_t     nPulseDelay;    /*!< Refer the above diagram. The actual delay in units of time is calculated
                                     by: Pulse_delay = (nPulseDelay / 128) * Time_period
                                     (Note: This is valid only for #ADI_E2BCORE_PORT_CTRL_CMD_PWM_PULSE_DELAY command) */
    uint16_t    nMask;          /*!< Mask for updating specific PWM signals
                                     (Bit 11 indicates PWM_11, Bit 0 indicates PWM_0)
                                     (Note: For PWM_SLOPE, bits indicate the mask of the port controller unit.
                                     E.g., Bit 2 indicates port controller unit 2,
                                     Bit 0 indicates port controller unit 0, etc.)
                                     (Note: This is not valid for PWM_WAIT or PWM_TXN_DELAY commands) */
    uint16_t    nGpioSteadyVal; /*!< Set the GPIO steady state value
                                     (Note: This is valid only for #ADI_E2BCORE_PORT_CTRL_CMD_PWM_GPIO_STEADY_VAL) */
    uint16_t    nMonitorData;   /*!< GPIO pin values when port monitor is enabled
                                     (Note: This is valid only for #ADI_E2BCORE_PORT_CTRL_CMD_PWM_MONITOR command) */
    uint16_t    nSlope;         /*!< Time slope for setting the PWM frequency
                                     (Time period = [(32768 / nSlope) * pwm_tick_period]
                                     (Note: This is valid only for #ADI_E2BCORE_PORT_CTRL_CMD_PWM_SLOPE command) */
    uint16_t    anTchSpeed[12U];    /*!< Tachometer speed of the 12 PWM pins
                                         (Note: This is valid only for #ADI_E2BCORE_PORT_CTRL_CMD_TCH_SPEED command) */

    /// TODO: Add any other configurations as required
} ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_addMultiplePortCtrlTxns(
    uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
    uint32_t nNumConfigs, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG aoPortCtrlIoCfgs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addSinglePortCtrlTxn(
    uint32_t nInstNum, ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poPortCtrlIoCfg);
extern void adi_e2bcore_clearPortCtrlConfigQueue(const ADI_E2BCORE_ID *poId);
extern ADI_E2BCORE_ERR adi_e2bcore_addPortCtrlConfigToQueue(
    const ADI_E2BCORE_ID *poId, bool bIsRxConfig,
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *poPortCtrlCfg);
extern uint16_t adi_e2bcore_slotsUsedUpInPortCtrlQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInPortCtrlQueue(const ADI_E2BCORE_ID *poId);

#endif /* E2BCORE_PORTCTRL_H */

/** @} */

/**
 * EOF: www.analog.com
 */
