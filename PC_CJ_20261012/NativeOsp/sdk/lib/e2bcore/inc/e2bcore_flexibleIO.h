/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_flexibleIO.h
 * \brief: The Flexible IO interface header file for the ADI E2B core
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */

#ifndef E2BCORE_FLEXIBLEIO_H
#define E2BCORE_FLEXIBLEIO_H

/*================================= INCLUDES ================================*/

#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/**
 * @brief Flexible IO transaction config structure: Header + ADI_E2BCORE_FLEXIBLE_IO_TXN_PAYLOAD
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG;

/*! @brief Flexible IO transaction payload type */
typedef uint8_t *ADI_E2BCORE_FLEXIBLE_IO_TXN_PAYLOAD;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_addFlexibleIOMult(
    uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
    uint32_t nNumConfigs, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG aoFlexibleIOConfigs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addFlexibleIOTxn(
    uint32_t nInstNum, ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFlexibleIOConfig);
extern ADI_E2BCORE_ERR adi_e2bcore_addFlexibleIOConfigToQueue(const ADI_E2BCORE_ID *poId,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *poFlexibleIOCfg);
extern void adi_e2bcore_clearFlexibleIOConfigQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsUsedUpInFlexibleIOQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInFlexibleIOQueue(const ADI_E2BCORE_ID *poId);

#endif /* E2BCORE_FLEXIBLEIO_H */

/** @} */
/**
 * EOF: www.analog.com
 */
