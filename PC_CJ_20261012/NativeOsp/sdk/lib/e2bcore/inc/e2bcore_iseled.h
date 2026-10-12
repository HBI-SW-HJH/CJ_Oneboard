/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_iseled.h
 * \brief: The ISELED interface header file for the ADI E2B core
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */

#ifndef E2BCORE_ISELED_H
#define E2BCORE_ISELED_H

/*================================= INCLUDES ================================*/

#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/
/*! Number of bytes used for data in ISELED cmd */
#define ISELED_CMD_DATA_LEN               (3U)

/*================================ DATA TYPES ===============================*/
/*! @brief ISELED fast mode transaction payload type */
typedef struct ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD
{
    uint8_t     nInstruction; /*!< ISELED instruction */
    uint16_t    nAddress;     /*!< ISELED address */
    uint8_t    *pData;        /*!< Pointer to ISELED data */

    // Note:
    // CRC need not be filled by the application.
    // This will be used when the application
    // is notified of RX data via ADI_E2BCORE_CBK
    uint8_t     nCrc;        /*!< Crc */
} ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD;

/*! @brief ISELED safe mode transaction payload type */
typedef struct ADI_E2BCORE_ISELED_SAFE_TXN_PAYLOAD
{
    uint32_t nLen;      /*!< length of the payload */
    uint8_t *pBuffer;   /*!< pointer to the buffer having payload */
} ADI_E2BCORE_ISELED_SAFE_TXN_PAYLOAD;

/*! @brief ISELED optimal mode transaction payload type */
typedef struct ADI_E2BCORE_ISELED_SAFE_TXN_PAYLOAD ADI_E2BCORE_ISELED_OPTIMAL_TXN_PAYLOAD;

/**
 * @brief ISELED transaction config structure: Header + Array<ADI_E2BCORE_ISELED_<mode>_TXN_PAYLOAD>
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_ISELED_TXN_CONFIG;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_addIseledMult(
    uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
    uint32_t nNumConfigs, ADI_E2BCORE_ISELED_TXN_CONFIG aoIseledConfigs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addIseledTxn(
    uint32_t nInstNum, ADI_E2BCORE_ISELED_TXN_CONFIG *poIseledConfig);
extern ADI_E2BCORE_ERR adi_e2bcore_addIseledConfigToQueue(const ADI_E2BCORE_ID *poId,
        ADI_E2BCORE_ISELED_TXN_CONFIG *poIseledCfg);
extern void adi_e2bcore_clearIseledConfigQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsUsedUpInIseledQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInIseledQueue(const ADI_E2BCORE_ID *poId);

#endif /* E2BCORE_ISELED_H */

/** @} */

/*
 * EOF: www.analog.com
 */
