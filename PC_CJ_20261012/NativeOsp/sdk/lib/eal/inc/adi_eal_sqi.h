/*******************************************************************************
Copyright (c) 2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_sqi.h
 * \brief:The header file for SQI functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_SQI_H
#define ADI_EAL_SQI_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"            /* The EAL header */

#ifdef ADI_E2B_IFACE_SQI_ENABLED


/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Enum for different callback events
 */
typedef enum ADI_EAL_SQI_CBK_EVT
{
    ADI_EAL_SQI_CBK_EVT_SQI_STATUS,                  /*!< Readback of SQI index */
    ADI_EAL_SQI_CBK_EVT_SQI_EXITED_WARNING,         /*!< SQI exited warning level event */
    ADI_EAL_SQI_CBK_EVT_SQI_ENTERED_WARNING,        /*!< SQI entered warning level event */
    ADI_EAL_SQI_CBK_EVT_SQI_ENTERED_BAD,            /*!< SQI entered bad level event */
    ADI_EAL_SQI_CBK_EVT_SQI_EXITED_BAD              /*!< SQI exited bad level event */
} ADI_EAL_SQI_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief Prototype of EAL SQI component callback */
typedef void (*ADI_EAL_SQI_CALLBACK_FUNC)(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SQI_CBK_EVT eCbkEventType,
        const void *pArg);

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */
/** @brief SQI status data */
typedef ADI_SQI_STATUS ADI_EAL_SQI_CBK_EVT_SQI_STATUS_DATA;

/* ~~~~~~~~~~~~~~~~~~~ SQI module configuration structure ~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Structure for configuring SQI module
 */
typedef struct ADI_EAL_SQI_CFG
{
    ADI_EAL_SQI_CALLBACK_FUNC pfSqiCallback;  /*!< Function that is called by the SQI component
                                                       to notify any event to the application */
} ADI_EAL_SQI_CFG;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_sqiInit(uint32_t nInstNum, ADI_EAL_SQI_CFG *poSqiCfg);
extern ADI_EAL_STATUS adi_eal_sqiConfig(uint32_t nInstNum, uint32_t nRemoteNum, ADI_SQI_CFG_DATA *poSqiCfg);
extern ADI_EAL_STATUS adi_eal_sqiGetIndex(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_sqiEnable(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnableSqi);


#endif /* ADI_E2B_IFACE_SQI_ENABLED */
#endif /* ADI_EAL_SQI_H */

/** @} */

/**
 * EOF: www.analog.com
 */

