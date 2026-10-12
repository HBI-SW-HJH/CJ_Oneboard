/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_ssc.h
 * \brief:The header file for safe state controller API's - to configure safe state controller, force safe state , return to functional mode manually , checks the safe state controller status and alive counter readback
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_SSC_H
#define ADI_EAL_SSC_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"            /* The EAL header */

#ifdef ADI_E2B_IFACE_SSC_ENABLED


/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Enum for different callback events
 */
typedef enum ADI_EAL_SSC_CBK_EVT
{
    ADI_EAL_SSC_CBK_EVT_SAFE_MODE,                   /*!< Change to Safe mode event */
    ADI_EAL_SSC_CBK_EVT_RETURN_TO_FUNC_MODE,         /*!< Return to functional mode event */
    ADI_EAL_SSC_CBK_EVT_UNSUCCESSFUL_RETURN_TO_FUNC, /*!< Return to functional mode failed event */
    ADI_EAL_SSC_CBK_EVT_STATUS,                      /*!< Readback of safe mode triggers event */
    ADI_EAL_SSC_CBK_EVT_ALIVE_COUNTER_RB             /*!< Readback of alive counter event */
} ADI_EAL_SSC_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief Prototype of EAL SSC component callback */
typedef void (*ADI_EAL_SSC_CALLBACK_FUNC)(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_SSC_CBK_EVT eCbkEventType,
        const void *pArg);

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */
/** @brief SSC Status Data */
typedef ADI_SSC_STATUS_DATA ADI_EAL_SSC_CBK_EVT_SSC_STATUS_DATA;

/* ~~~~~~~~~~~~~~~~~~~ SSC module configuration structure ~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Structure for configuring SSC module
 */
typedef struct ADI_EAL_SSC_CFG
{
    ADI_EAL_SSC_CALLBACK_FUNC pfSscCallback;  /*!< Function that is called by the SSC component
                                                       to notify any event to the application */
} ADI_EAL_SSC_CFG;

/**
 * @brief Structure for SSC module settings
 *
 */
typedef struct ADI_EAL_SSC_MONITOR_CONFIG
{
    bool     bDetectBeaconAsEthTraffic;                                 /*!< If set true, enables the monitoring of the beacon will be added to the Ethernet traffic check */
    bool     bEnableHostInt;                                            /*!< If set true, enables the host interface on safe mode */
    uint8_t  nHwAdcThresholdFlag[ADI_SSC_HW_ADC_CNT];                   /*!< Setting of Threshold whether to trigger when the ADC value is higher/lower than high/low threshold */
    uint8_t  nSQISafeModeLvlTrigger;                                    /*!< The SQI level for which the safe mode has to triggered */
    uint32_t nMonitorEnable;                                            /*!< Sets which pins has to be monitored for safe mode */
    uint32_t nMonitorPolarity;                                          /*!< Sets the polarity of monitored pins for which the safe mode has to be triggered */
    uint32_t nSAIFSafeEn;                                               /*!< Sets whether the SA_IF pins have functional / safe mode configurations in safe state */
    uint32_t nSAIFConfig[ADI_SSC_SA_IF_CNT];                            /*!< Configurations of SA_IF pins in safe state */
    ADI_SSC_HW_ADC_MUX eHwAdcConfig[ADI_SSC_HW_ADC_CNT];                /*!< HW_ADC MUX Configuration */
    uint32_t nAutoReturnToFunc;                                         /*!< Enables auto return from safe mode, only available for SA_IF pins, HW_ADC pins, GPTP_UNLOCK and Timeouts */
    uint32_t nEthFrameCheckTimeout;                                     /*!< Set the timeout for receiving valid ethernet frame */
    uint32_t nDeviceUnattendedTimeout;                                  /*!< Set the timeout for device unattended feature */
} ADI_EAL_SSC_MONITOR_CONFIG;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_sscInit(uint32_t nInstNum, ADI_EAL_SSC_CFG *poSscCfg);
extern ADI_EAL_STATUS adi_eal_sscRun(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_sscSetConfig(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_EAL_SSC_MONITOR_CONFIG *poSscSetupCfg);
extern ADI_EAL_STATUS adi_eal_sscReturnToFunctional(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_sscForceSafeState(uint32_t nInstNum, uint32_t nRemoteNum, bool bForceSafeState);
extern ADI_EAL_STATUS adi_eal_sscCheckSafeModeStatus(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_sscAliveCounterRead(uint32_t nInstNum, uint32_t nRemoteNum);
extern void eal_notify_sscRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat);


#endif /* ADI_E2B_IFACE_SSC_ENABLED */
#endif /* ADI_EAL_SSC_H */

/** @} */

/**
 * EOF: www.analog.com
 */

