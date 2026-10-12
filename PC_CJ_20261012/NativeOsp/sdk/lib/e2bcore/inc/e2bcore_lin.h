/*******************************************************************************
Copyright (c) 2021-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_lin.h
 * @brief: External e2bcore interface of LIN interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */
#ifndef ADI_E2BCORE_LIN_H
#define ADI_E2BCORE_LIN_H

/*================================= INCLUDES ================================*/

#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/**
 * @brief LIN config structure: { ADI_E2BCORE_TXN_HDR, ADI_E2BCORE_LIN_TXN_PAYLOAD[] }
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_LIN_TXN_CONFIG;

/** @brief LIN interface status data */
typedef struct ADI_E2BCORE_LIN_STATUS_DATA
{
    bool        bIsBrkFldCollision;     /*!< Indicates the Break field collision detect status */
    bool        bIsSyncFldCollision;    /*!< Indicates the Sync field collision detect status */
    bool        bIsPIDFldCollision;     /*!< Indicates the PID field collision detect status */
    bool        bIsTransCollision;      /*!< Indicates the Transmission collision detect status */
    bool        bIsFrameErrDetect;      /*!< Indicates the Frame error detect status */
    bool        bIsFrameRespShort;      /*!< Indicates if the Frame response is too short */
    bool        bIsWakeupSigDetect;     /*!< Indicates the Wakeup signal detect status */
} ADI_E2BCORE_LIN_STATUS_DATA;

/*! @brief LIN transaction payload type */
typedef struct ADI_E2BCORE_LIN_TXN_PAYLOAD
{
    bool        bIsWriteTxn;                   /*!< Indicates if it's write transaction */
    bool        bAddCheckSum;                  /*!< Is checksum inserted in payload? */
    uint8_t     nLength;                       /*!< Length of the payload */
    uint8_t     nPid;                          /*!< Proctected identifier field */
    uint8_t     nCheckSum;                     /*!< Checksum for the payload */
    uint8_t     *pData;                        /*!< Pointer to LIN data */
    ADI_E2BCORE_LIN_STATUS_DATA oStatusData;   /*!< LIN interface status data*/
} ADI_E2BCORE_LIN_TXN_PAYLOAD;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_addLinMult(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
        uint32_t nNumConfigs, ADI_E2BCORE_LIN_TXN_CONFIG aoLinIoCfgs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addLinTxn(uint32_t nInstNum, ADI_E2BCORE_LIN_TXN_CONFIG *poLinIoCfg);
extern ADI_E2BCORE_ERR adi_e2bcore_addLinConfigToQueue(const ADI_E2BCORE_ID *poId,
        ADI_E2BCORE_LIN_TXN_CONFIG *poLinCfg);
extern void adi_e2bcore_clearLinConfigQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsUsedUpInLinQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInLinQueue(const ADI_E2BCORE_ID *poId);

#endif /* ADI_E2BCORE_LIN_H */

/** @} */

/**
 * EOF: www.analog.com
 */
