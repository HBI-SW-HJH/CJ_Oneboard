/*******************************************************************************
Copyright (c) 2021-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_uart.h
 * @brief: External e2bcore interface of UART interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */
#ifndef ADI_E2BCORE_UART_H
#define ADI_E2BCORE_UART_H

/*================================= INCLUDES ================================*/
#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/**
 * @brief UART transaction config structure: Header + Array<UartTxnPayload>
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_UART_TXN_CONFIG;

/**
 * @brief Enumeration of the different commands supported by UART interface
 */
typedef enum ADI_E2BCORE_UART_CMD
{
    ADI_E2BCORE_UART_CMD_DATA,        /*!< Bit Array data cmd */
    ADI_E2BCORE_UART_CMD_GOOD_CODE,   /*!< Good Code cmd */
    ADI_E2BCORE_UART_CMD_DELAY        /*!< Delay (in bits) cmd */
} ADI_E2BCORE_UART_CMD;

/*! @brief UART transaction payload type */
typedef struct ADI_E2BCORE_UART_TXN_PAYLOAD
{
    ADI_E2BCORE_UART_CMD eUartCmd;    /*!< Uart command type in payload */
    uint8_t              nUartLen;    /*!< Specify uart data length
                                           NOTE: Valid only for #ADI_E2BCORE_UART_CMD_DATA command */
    uint16_t             nUartData;   /*!< Uart transaction data in payload */
} ADI_E2BCORE_UART_TXN_PAYLOAD;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_adduartMult(
    uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
    uint32_t nNumConfigs, ADI_E2BCORE_UART_TXN_CONFIG aoUartIoCfgs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addUartTxn(
    uint32_t nInstNum, ADI_E2BCORE_UART_TXN_CONFIG *poUartIoCfg);
extern ADI_E2BCORE_ERR adi_e2bcore_addUartConfigToQueue(
    const ADI_E2BCORE_ID *poId, bool bIsRxConfig,
    ADI_E2BCORE_UART_TXN_CONFIG *poUartCfg);
extern void adi_e2bcore_clearUartConfigQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsUsedUpInUartQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInUartQueue(const ADI_E2BCORE_ID *poId);

#endif /* ADI_E2BCORE_UART_H */

/** @} */

/**
 * EOF: www.analog.com
 */
