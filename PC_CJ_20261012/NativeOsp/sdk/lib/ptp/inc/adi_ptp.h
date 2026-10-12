/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_ptp.h
 * \brief: A simple gPTP master
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup ptp
 *  @{
 */

#ifndef ADI_PTP_H
#define ADI_PTP_H

/*================================= INCLUDES ================================*/
#include "adi_common.h"

/*================================= DEFINES =================================*/
#define ADI_PTP_ETHERTYPE                   (0x88F7u) /*!< EtherType of gPTP */

/*================================ DATA TYPES ===============================*/
/*! @brief Different events types from the PTP library */
typedef enum ADI_PTP_EVT
{
    ADI_PTP_EVT_GET_BUFFER, /*!< Initialization completed */
    ADI_PTP_EVT_TRANSMIT_BUFFER, /*!< RX received from 10baset1 event */
} ADI_PTP_EVT;

/*! @brief Different PTP sync message intervals */
typedef enum ADI_PTP_SYNC_MSG_INTVL
{
    ADI_PTP_SYNC_MSG_INTVL_31_25 = -5,  /*!< logMessageInterval = -5 (31.25 ms) */
    ADI_PTP_SYNC_MSG_INTVL_62_5 = -4,   /*!< logMessageInterval = -4 (62.5 ms) */
    ADI_PTP_SYNC_MSG_INTVL_125 = -3,    /*!< logMessageInterval = -3 (125 ms) */
    ADI_PTP_SYNC_MSG_INTVL_250 = -2,    /*!< logMessageInterval = -2 (250 ms) */
} ADI_PTP_SYNC_MSG_INTVL;

/*! @brief Data for the get buffer callback event */
typedef struct ADI_PTP_CBK_GETBUFDAT
{
    uint32_t nLength; /*!< The length of the buffer requested, will be < 1500 */
    uint8_t *pBuffer; /*!< Application should update this pointer with a buffer where the ptp library can fill the data */
} ADI_PTP_CBK_GETBUFDAT;

/*! @brief Data for the transmit event */
typedef struct ADI_PTP_CBK_TRANSMITDAT
{
    bool bEnableTimeStamp; /*!< If the application should get the timestamp */
    uint32_t nActLength; /*!< The actual length of the frame */
    const uint8_t *pBuffer; /*!< The buffer to be transmitted */
    const uint8_t *anDestAddr; /*!< The destination address to which this packet must be sent */
    bool bError; /*!< Allows application to notify if there is an error */
} ADI_PTP_CBK_TRANSMITDAT;

/*! Callback function */
typedef void (*ADI_PTP_CBK)(ADI_PTP_EVT eEvent, void *pData);

/*! @brief Configuration structure of the PTP stack */
typedef struct ADI_PTP_CFG
{
    ADI_PTP_SYNC_MSG_INTVL eMsgIntvl; /**< Sync message interval (logMessageInterval) */
    ADI_PTP_CBK pCbk; /**< The callback function pointer */
} ADI_PTP_CFG;

/*============================ PUBLIC PROTOTYPES ============================*/
extern void adi_ptp_init(const ADI_PTP_CFG *poPtpCfg);
extern void adi_ptp_sendSync(void);
extern void adi_ptp_setSyncEgress(uint32_t nSeconds, uint32_t nNanoseconds);
extern void adi_ptp_sendFollowUp(void);

#endif /* ADI_PTP_H */

/** @} */

/*
 * EOF: www.analog.com
*/
