/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: iseled.h
 * \brief: This is the header file for ISELED driver
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */
#ifndef ISELED_H
#define ISELED_H

/*================================= INCLUDES ================================*/
#include "adi_eal_iseled.h"

/*================================= DEFINES =================================*/
#define MAX_NUM_ISELED   (8u)
#define APP_ISELED_TOTAL_COUNT (255u)

/*================================ DATA TYPES ===============================*/

/*============================ PUBLIC PROTOTYPES ============================*/
extern void IseledInitDrv(uint8_t nRemoteNum, ADI_EAL_ISELED_HANDLE hIseledHandle);
extern ADI_EAL_STATUS IseledResetStrip(uint8_t nRemoteNum);
extern ADI_EAL_STATUS IseledInitStrip(uint8_t nRemoteNum);
extern ADI_EAL_STATUS IseledReadTemp(uint8_t nRemoteNum);
extern ADI_EAL_STATUS IseledSetRgb(uint8_t nRemoteNum, ADI_EAL_ISELED_MODE eMode,
                                   ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD *poFastModeData,
                                   uint16_t nNumpackTxn, uint8_t nId, uint16_t nExpectedNumResp);

#endif /* ISELED_H */

/** @} */

/**
 * EOF: https://www.analog.com/
 */
