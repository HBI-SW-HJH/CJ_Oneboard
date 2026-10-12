/*******************************************************************************
 Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
 This software is proprietary & confidential to Analog Devices, Inc.
 and its licensors.
 ******************************************************************************
 * \file: demo_adpd.h
 * \brief: The header file for proximity Sensor demo implementation
 * Developed by: Automotive Software and Systems team, Bangalore, India
 *****************************************************************************/
/** \addtogroup app
 * @{
 */
#ifndef DEMO_ADPD_H
#define DEMO_ADPD_H

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
/*! Number of pixels for displaying logo on the neopixel strip */
#define ADI_LOGO_NUM_OF_PIXELS      (30u)
#define ADPD_AVG_WIND               (3u)
#define ADPD_FIFO_THRESH            (4u)

/*================================ DATA TYPES ===============================*/
typedef void (*DEMO_ADPD_CALLBACK_FUNC)(uint32_t nLength, float fAverage);

/** Data for the ADPD demo */
typedef struct DEMO_ADPD_DATA
{
    bool bPeriodicConfigured;                /*!< Tracks if the periodic read has been setup */
    bool bPeriodicStarted;                   /*!< Tracks if the periodic read has been started */
    uint32_t nProxValIdx;                    /*!< The proximity value index */
    uint32_t nProxValCnt;                    /*!< The proximity value count */
    uint8_t anAdpdCtrlTx[100U][3U];          /*!< A buffer to store the initialization sequence */
    uint32_t nAdpdCtrlIdx;                   /*!< Count for the initialization sequence buffer */
    uint64_t nInitTime;                      /*!< The time when initializations were sent */
    uint64_t nPeriodicCfgTime;               /*!< The time when the periodic read was configured */
    double afAdpdMvnAvg[ADPD_AVG_WIND];      /*!< The moving average buffer */
    DEMO_ADPD_CALLBACK_FUNC pfAdpdCbk;       /*!< Pointer to store the address of callback function */
    ADI_EAL_SPI_HANDLE hProxSpiIntfHandle;   /*!< The SPI EAL handle */
} DEMO_ADPD_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
/* ADPD functions */
extern void demo_adpd_init(ADI_EAL_SPI_HANDLE hSpiHandle, DEMO_ADPD_CALLBACK_FUNC pfAdpdCbk);
extern void demo_adpd_configurePeriodic(void);
extern void demo_adpd_run(void);

#endif /* ADI_E2B_IFACE_SPI_ENABLED */
#endif /* DEMO_ADPD_H */
