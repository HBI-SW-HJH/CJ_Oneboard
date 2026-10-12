/*******************************************************************************
Copyright (c) 2021-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: e2bcore_adc.h
 * @brief: External e2bcore interface of ADC (VMTR) interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */
#ifndef ADI_E2BCORE_ADC_H
#define ADI_E2BCORE_ADC_H

/*================================= INCLUDES ================================*/

#include "adi_e2bcore.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/**
 * @brief ADC transaction config structure: Header + Array<AdcTxnPayload>
 */
typedef ADI_E2BCORE_TXN_CONFIG ADI_E2BCORE_ADC_TXN_CONFIG;

/**
 * @brief Enumeration of the different commands supported by ADC interface
 */
typedef enum ADI_E2BCORE_ADC_CMD
{
    ADI_E2BCORE_ADC_CMD_CLK_PERIOD      = 0U, /*!< Sets time between ADC samples */
    ADI_E2BCORE_ADC_CMD_CYCLE_PERIOD    = 1U, /*!< Sets time between ADC cycles (1 cycle can have 1 or more ADC samples) */
    ADI_E2BCORE_ADC_CMD_UPDATE          = 2U, /*!< Applies the CLK_PERIOD and/or CYCLE_PERIOD */
    ADI_E2BCORE_ADC_CMD_START           = 3U, /*!< Starts ADC sampling cycle */
    ADI_E2BCORE_ADC_CMD_STOP            = 4U, /*!< Stops ADC sampling */
    ADI_E2BCORE_ADC_CMD_LUT             = 5U, /*!< Configures look-up table for the sampling order (Note: Include calibration cycles) */
    ADI_E2BCORE_ADC_CMD_NUM_SAMPLES     = 6U, /*!< Sets the number of samples to be captured per channel in round-robin sampling mode */
    ADI_E2BCORE_ADC_CMD_CH_EN           = 7U, /*!< Sets whether a given channel is enabled (will be sampled) in round-robin sampling mode */
    ADI_E2BCORE_ADC_CMD_MODE            = 8U, /*!< Sets the ADC sampling mode between LUT mode or round-robin mode */
    ADI_E2BCORE_ADC_CMD_MASK            = 9U, /*!< Sets whether the data samples for a given channel are passed back to the ECU or are masked */
} ADI_E2BCORE_ADC_CMD;

/*! @brief ADC TX (Downstream) transaction payload type */
typedef struct ADI_E2BCORE_ADC_TXN_TX_PAYLOAD
{
    ADI_E2BCORE_ADC_CMD eAdcCmd;    /*!< ADC transaction command */
    bool        bLutEn;             /*!< Sets the ADC sampling mode to LUT or round robin
                                         (Note: This is valid only for #ADI_E2BCORE_ADC_CMD_MODE command) */
    uint8_t     nLutSize;           /*!< Sets the number of channel indices to sample from in LUT mode
                                         (Note: This is valid only for #ADI_E2BCORE_ADC_CMD_LUT command) */
    uint8_t     *panChannelIdx;     /*!< Array of channel indices to set in LUT mode
                                         (Note: This is valid only for #ADI_E2BCORE_ADC_CMD_LUT command) */
    uint8_t     nNumSamples;        /*!< Sets the number of samples per channel to be taken in round-robin sampling mode
                                         (Note: This is valid only for #ADI_E2BCORE_ADC_CMD_NUM_SAMPLES command) */
    uint16_t    nChannelEn;         /*!< Sets whether a given channel is enabled (will be sampled)
                                         in round-robin sampling mode (ChannelEn[15:0])
                                         (Note: This is valid only for #ADI_E2BCORE_ADC_CMD_CH_EN command) */
    uint16_t    nChannelMask;       /*!< Sets whether a given channel is masked (data will not be passed to ECU)
                                         (Note: This is valid only for #ADI_E2BCORE_ADC_CMD_MASK command) */
    uint32_t    nAdcClkPeriod;      /*!< Sets the time (in ns) between 2 ADC samples
                                         (Note: Valid only for #ADI_E2BCORE_ADC_CMD_CLK_PERIOD command) */
    uint32_t    nAdcCyclePeriod;    /*!< Sets the time (in ns) between 2 ADC cycles (1 cycle can have 1 or more ADC samples)
                                         (Note: Valid only for #ADI_E2BCORE_ADC_CMD_CYCLE_PERIOD command) */
} ADI_E2BCORE_ADC_TXN_TX_PAYLOAD;

/*! @brief ADC RX (Upstream) transaction payload type */
typedef struct ADI_E2BCORE_ADC_TXN_RX_PAYLOAD
{
    uint8_t     nChIdx;         /*!< Channel index from which ADC data is obtained */
    uint16_t    nAdcData;       /*!< ADC data */
} ADI_E2BCORE_ADC_TXN_RX_PAYLOAD;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_E2BCORE_ERR adi_e2bcore_addAdcMult(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
        uint32_t nNumConfigs, ADI_E2BCORE_ADC_TXN_CONFIG aoAdcIoCfgs[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addAdcTxn(uint32_t nInstNum, ADI_E2BCORE_ADC_TXN_CONFIG *poAdcIoCfg);
extern void adi_e2bcore_clearAdcConfigQueue(const ADI_E2BCORE_ID *poId);
extern ADI_E2BCORE_ERR adi_e2bcore_addAdcConfigToQueue(const ADI_E2BCORE_ID *poId, bool bIsRxConfig,
        ADI_E2BCORE_ADC_TXN_CONFIG *poAdcIoCfg);
extern uint16_t adi_e2bcore_slotsUsedUpInAdcQueue(const ADI_E2BCORE_ID *poId);
extern uint16_t adi_e2bcore_slotsLeftInAdcQueue(const ADI_E2BCORE_ID *poId);

#endif /* ADI_E2BCORE_ADC_H */

/** @} */

/**
 * EOF: www.analog.com
 */
