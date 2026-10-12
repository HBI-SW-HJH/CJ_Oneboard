/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_spi.h
 * \brief: The SPI interface header file for the ADI E2B core
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */

#ifndef E2BCORE_SPI_H
#define E2BCORE_SPI_H

/*================================= INCLUDES ================================*/

#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/**
 * @brief SPI transaction config structure: Header + ADI_E2BCORE_SPI_TXN_PAYLOAD
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_SPI_TXN_CONFIG;

/*! @brief SPI transaction payload type */
typedef uint8_t *ADI_E2BCORE_SPI_TXN_PAYLOAD;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_addSpiMult(
    uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
    uint32_t nNumConfigs, ADI_E2BCORE_SPI_TXN_CONFIG aoSpiConfigs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addSpiTxn(
    uint32_t nInstNum, ADI_E2BCORE_SPI_TXN_CONFIG *poSpiConfig);
extern ADI_E2BCORE_ERR adi_e2bcore_addSpiConfigToQueue(const ADI_E2BCORE_ID *poId,
        ADI_E2BCORE_SPI_TXN_CONFIG *poSpiCfg);
extern void adi_e2bcore_clearSpiConfigQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsUsedUpInSpiQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInSpiQueue(const ADI_E2BCORE_ID *poId);

#endif /* E2BCORE_SPI_H */

/** @} */
/*
 * EOF: www.analog.com
 */
