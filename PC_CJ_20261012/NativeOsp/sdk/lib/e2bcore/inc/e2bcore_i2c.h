/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_i2c.h
 * \brief: The I2C interface header file for the ADI E2B core
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */

#ifndef E2BCORE_I2C_H
#define E2BCORE_I2C_H

/*================================= INCLUDES ================================*/

#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/**
 * @brief I2C transaction config structure: Header + ADI_E2BCORE_I2C_TXN_PAYLOAD
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_I2C_TXN_CONFIG;

/*! @brief I2C transaction payload type */
typedef uint8_t *ADI_E2BCORE_I2C_TXN_PAYLOAD;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_addI2cMult(
    uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
    uint32_t nNumConfigs, ADI_E2BCORE_I2C_TXN_CONFIG aoI2cConfigs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addI2cTxn(
    uint32_t nInstNum, ADI_E2BCORE_I2C_TXN_CONFIG *poI2cConfig);
extern ADI_E2BCORE_ERR adi_e2bcore_addI2cConfigToQueue(
    const ADI_E2BCORE_ID *poId, ADI_E2BCORE_I2C_TXN_CONFIG *poI2cCfg);
extern void adi_e2bcore_clearI2cConfigQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsUsedUpInI2cQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInI2cQueue(const ADI_E2BCORE_ID *poId);

#endif /* E2BCORE_I2C_H */

/** @} */
/*
 * EOF: www.analog.com
 */
