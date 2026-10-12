/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: demo_neopixels.h
 * \brief: Neopixels demo implementation
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */
#ifndef DEMO_NEOPIXELS_H
#define DEMO_NEOPIXELS_H

#include "adi_common.h"

#ifdef ADI_E2B_IFACE_SPI_ENABLED
/*================================= INCLUDES ================================*/
#include "adi_pal.h"                    /*!< Platform abstraction layer */
#include "adi_dbg.h"                    /*!< Debug interface */
#include "adi_network.h"                /*!< Network layer */
#include "adi_app_utils.h"              /*!< Include common utilities */
#include "adi_eal.h"                    /*!< E2B abstraction layer */
#include "adi_eal_spi.h"                /*!< E2B SPI abstraction layer */

/*================================= DEFINES =================================*/
#define NEO_MAX_LENGTH                  (30U)  /*!< Number of pixels for displaying logo on the neopixel strip */
#define NEO_CHANNELS                    ( 3U)  /*!< Number of bytes to set RGB */
#define NEO_RED_INTENSITY_VAL           (120U) /*!< Neostrip red color intesity value */
#define NEO_GREEN_INTENSITY_VAL         (120U) /*!< Neostrip green color intesity value */
#define NEO_BLUE_INTENSITY_VAL          (120U) /*!< Neostrip blue color intesity value */

/*================================ DATA TYPES ===============================*/

typedef enum DEMO_NEO_MODE
{
    E_ANIMATION,
    E_LENGTH
} DEMO_NEO_MODE;

/** Data for the Neo Pixels demo */
typedef struct DEMO_NEO_DATA
{
    DEMO_NEO_MODE eMode;            /*!< Neopixel modes */
    uint32_t nLength;
    uint32_t nAnimateLength;        /*!< Neostrip Animation length */
    uint32_t nNeoLength;
    uint32_t nNumOutput;
    uint32_t nAvgAdcSampleVal;      /*!< Average ADC Sensor data used to turn NeoStrip On */
    uint64_t nPrevSendTime;         /*!< Track the previous write time */
} DEMO_NEO_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
/* Neopixel functions */
extern void demo_neopixelsInit(ADI_EAL_SPI_HANDLE hSpiHandle);
extern void demo_neopixelsRun(void);
extern void demo_neopixels_setLength(uint32_t nValue);
extern void demo_neopixels_startAnimation(void);
extern void demo_neopixels_stopAnimation(void);
extern void demo_adpd_ProcessSensorData(uint8_t *pBuff, uint32_t nSize);
extern void demo_neopixelsUpdateOutput(uint32_t nLength, uint32_t nProxOutput);
extern void demo_neopixels_setLength_updateConsole(uint32_t nValue);

#endif /* ADI_E2B_IFACE_SPI_ENABLED */
#endif /* DEMO_NEOPIXELS_H */
