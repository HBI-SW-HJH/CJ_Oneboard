/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_wkslp.h
 * \brief:The header file for wake / sleep functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_WKSLP_H
#define ADI_EAL_WKSLP_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"  /* The EAL header */

#ifdef ADI_E2B_IFACE_WKSLP_ENABLED


/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL Wake-sleep component callback event types
 */
typedef enum ADI_EAL_WKSLP_CBK_EVT
{
    ADI_EAL_WKSLP_CBK_EVT_AWAKE,            /*!< Remote node is awake event */
    ADI_EAL_WKSLP_CBK_EVT_STANDBY,          /*!< Remote node transitioned to standby event */
    ADI_EAL_WKSLP_CBK_EVT_SLEEP,            /*!< Remote node transitioned to sleep event */
    ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED,   /*!< Remote node transition to standby failed event */
    ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED,     /*!< Remote node transition to sleep failed event */
    ADI_EAL_WKSLP_CBK_EVT_REASON_FOR_WKSLP, /*!< Remote node reason for wake / sleep readback event */
} ADI_EAL_WKSLP_CBK_EVT;

/**
 * @brief Enum of the different node wake/sleep status
 */
typedef enum ADI_EAL_WKSLP_NODE_STATUS
{
    ADI_EAL_WKSLP_NODE_AWAKE,     /*!< Node is in awake state */
    ADI_EAL_WKSLP_NODE_SLEEP,     /*!< Node is in sleep state */
    ADI_EAL_WKSLP_NODE_STANDBY,   /*!< Node is in standby state */
} ADI_EAL_WKSLP_NODE_STATUS;

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** Prototype of EAL Wake-sleep component callback */
typedef void (*ADI_EAL_WKSLP_CALLBACK_FUNC)(ADI_EAL_WKSLP_CBK_EVT eCbkEventType, const void *pArg);

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Data for #ADI_EAL_WKSLP_CBK_EVT_AWAKE event
 */
typedef struct ADI_EAL_WKSLP_CBK_EVT_AWAKE_DATA
{
    uint32_t nInstNum;                      /*!< Instance number of EAL */
    uint32_t nRemoteNum;                    /*!< The remote number */
    ADI_EAL_WKSLP_NODE_STATUS ePrevState;   /*!< The previous state of the node */
} ADI_EAL_WKSLP_CBK_EVT_AWAKE_DATA;

/**
 * @brief Data for #ADI_EAL_WKSLP_CBK_EVT_STANDBY event
 */
typedef ADI_EAL_WKSLP_CBK_EVT_AWAKE_DATA ADI_EAL_WKSLP_CBK_EVT_STANDBY_DATA;

/**
 * @brief Data for #ADI_EAL_WKSLP_CBK_EVT_SLEEP event
 */
typedef ADI_EAL_WKSLP_CBK_EVT_AWAKE_DATA ADI_EAL_WKSLP_CBK_EVT_SLEEP_DATA;

/**
 * @brief Data for #ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED event
 */
typedef struct ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED_DATA
{
    uint32_t nInstNum;                      /*!< Instance number of EAL */
    uint32_t nRemoteNum;                    /*!< The remote number */
    ADI_EAL_STATUS eFailStatus;             /*!< Indicates the failure status */
} ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED_DATA;

/**
 * @brief Data for #ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED event
 */
typedef ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED_DATA ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED_DATA;

/**
 * @brief Data for #ADI_EAL_WKSLP_CBK_EVT_REASON_FOR_WKSLP event
 */
typedef struct ADI_EAL_WKSLP_CBK_EVT_REASON_FOR_WKSLP_DATA
{
    uint32_t nInstNum;                          /*!< Instance number of EAL */
    uint32_t nRemoteNum;                        /*!< The remote number */
    ADI_REASON_FOR_SLEEP eReasonForSleep;       /*!< Reason for sleep */
    ADI_REASON_FOR_WAKE eReasonForWake;         /*!< Reason for wake */
} ADI_EAL_WKSLP_CBK_EVT_REASON_FOR_WKSLP_DATA;

/* ~~~~~~~~~~~~~~~~~~~ Wake sleep module configuration structure ~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Data structure for configuring Wake-sleep module
 */
typedef struct ADI_EAL_WKSLP_CFG
{
    ADI_EAL_WKSLP_CALLBACK_FUNC pfWkslpCallback;  /*!< Function that is called by the wake-sleep component
                                                       to notify any event to the application */
} ADI_EAL_WKSLP_CFG;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_wkslpInit(uint32_t nInstNum, ADI_EAL_WKSLP_CFG *poWkslpCfg);
extern ADI_EAL_STATUS adi_eal_wkslpRun(uint32_t nInstNum);
extern ADI_EAL_STATUS adi_eal_wkslpStandbyNode(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_wkslpSleepNode(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_wkslpWakeNetwork(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_wkslpGetNodeStatus(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_EAL_WKSLP_NODE_STATUS *peNodeStatus);
extern ADI_EAL_STATUS adi_eal_wkslpGetReasonForWkslp(uint32_t nInstNum, uint32_t nRemoteNum);
extern void adi_eal_wkslpNotifyNodeState(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_WKSLP_NODE_STATUS eState);


#endif /* ADI_E2B_IFACE_WKSLP_ENABLED */
#endif /* ADI_EAL_WKSLP_H */

/** @} */

/**
 * EOF: www.analog.com
 */
