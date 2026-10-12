/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_diag.h
 * \brief:The header file for diagnostics functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_DIAG_H
#define ADI_EAL_DIAG_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"            /* The EAL header */
#ifdef ADI_E2B_IFACE_SPI_ENABLED
#include "adi_eal_spi.h"        /* EAL SPI header */
#endif
#ifdef ADI_E2B_IFACE_I2C_ENABLED
#include "adi_eal_i2c.h"        /* EAL I2C header */
#endif
#ifdef ADI_E2B_IFACE_UART_ENABLED
#include "adi_eal_uart.h"       /* EAL UART header */
#endif
#ifdef ADI_E2B_IFACE_ADC_ENABLED
#include "adi_eal_adc.h"        /* EAL ADC header */
#endif
#include "adi_eal_portCtrl.h"   /* EAL PORT CTRL header */
#ifdef ADI_E2B_IFACE_ISELED_ENABLED
#include "adi_eal_iseled.h"     /* EAL ISELED header */
#endif
#ifdef ADI_E2B_IFACE_LIN_ENABLED
#include "adi_eal_lin.h"        /* EAL LIN header */
#endif
#ifdef ADI_E2B_IFACE_FIO_PWM_ENABLED
#include "adi_eal_fioPwm.h"     /* EAL FIO PWM header */
#endif
#ifdef ADI_E2B_IFACE_FIO_UART_ENABLED
#include "adi_eal_fioUart.h"    /* EAL FIO UART header */
#endif

#ifdef ADI_E2B_IFACE_DIAG_ENABLED


/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL diag component callback event types
 */
typedef enum ADI_EAL_DIAG_CBK_EVT
{
    ADI_EAL_DIAG_CBK_EVT_MAC_STATUS,     /*!< MAC statistics event */
    ADI_EAL_DIAG_CBK_EVT_PLCA_STATUS,   /*!< PLCA status event */
    ADI_EAL_DIAG_CBK_EVT_PHY_STATUS,    /*!< PHY status event */
    ADI_EAL_DIAG_CBK_EVT_PCS_STATUS,     /*!< PCS statistics event */
    ADI_EAL_DIAG_CBK_EVT_PTP_STATUS,     /*!< PTP statistics event */
} ADI_EAL_DIAG_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** Prototype of EAL diag component callback */
typedef void (*ADI_EAL_DIAG_CALLBACK_FUNC)(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_DIAG_CBK_EVT eCbkEventType,
        const void *pArg);

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief MAC status data */
typedef ADI_MAC_STATUS_DATA ADI_EAL_DIAG_CBK_EVT_MAC_STATUS_DATA;

/** @brief PLCA status data */
typedef ADI_PLCA_STATUS_DATA ADI_EAL_DIAG_CBK_EVT_PLCA_STATUS_DATA;

/** @brief PHY status data */
typedef ADI_PHY_STATUS_DATA ADI_EAL_DIAG_CBK_EVT_PHY_STATUS_DATA;

/** @brief PCS statistics data */
typedef ADI_PCS_STATUS_DATA ADI_EAL_DIAG_CBK_EVT_PCS_STATUS_DATA;

/** @brief PTP statistics data */
typedef ADI_PTP_STATUS_DATA ADI_EAL_DIAG_CBK_EVT_PTP_STATUS_DATA;


/* ~~~~~~~~~~~~~~~~~~~ Diagnostics module configuration structure ~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Data structure for configuring diagnostics module
 */
typedef struct ADI_EAL_DIAG_CFG
{
    ADI_EAL_DIAG_CALLBACK_FUNC pfDiagCallback;  /*!< Function that is called by the diag component
                                                     to notify any event to the application */
} ADI_EAL_DIAG_CFG;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_diagInit(uint32_t nInstNum, ADI_EAL_DIAG_CFG *poDiagCfg);
extern ADI_EAL_STATUS adi_eal_diagGetMacStatus(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_diagGetPlcaStatus(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_diagGetPhyStatus(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_diagGetPcsStatus(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_diagGetPtpStatus(uint32_t nInstNum, uint32_t nRemoteNum);
extern void adi_eal_diagLogFifoStatus(ADI_EAL_FIFO_STATUS_DATA *poStatusData);
#ifdef ADI_E2B_IFACE_SPI_ENABLED
extern void adi_eal_diagLogSpiStatus(ADI_EAL_SPI_STATUS_DATA *poStatusData);
#endif
#ifdef ADI_E2B_IFACE_I2C_ENABLED
extern void adi_eal_diagLogI2cStatus(ADI_EAL_I2C_STATUS_DATA *poStatusData);
#endif
#ifdef ADI_E2B_IFACE_UART_ENABLED
extern void adi_eal_diagLogUartStatus(ADI_EAL_UART_STATUS_DATA *poStatusData);
#endif
#ifdef ADI_E2B_IFACE_PORT_ENABLED
extern void adi_eal_diagLogPortCtrlStatus(ADI_EAL_PORTCTRL_STATUS_DATA *poStatusData);
#endif
#ifdef ADI_E2B_IFACE_ADC_ENABLED
extern void adi_eal_diagLogAdcStatus(ADI_EAL_ADC_STATUS_DATA *poStatusData);
#endif
#ifdef ADI_E2B_IFACE_ISELED_ENABLED
extern void adi_eal_diagLogIseledStatus(ADI_EAL_ISELED_STATUS_DATA *poStatusData);
#endif
#ifdef ADI_E2B_IFACE_LIN_ENABLED
extern void adi_eal_diagLogLinStatus(ADI_EAL_LIN_STATUS_DATA *poStatusData);
#endif
#ifdef ADI_E2B_IFACE_FIO_ENABLED
extern void adi_eal_diagLogFlexibleIOStatus(ADI_EAL_FIO_STATUS_DATA *poStatusData);
#endif
extern void adi_eal_diagExtractFifoStatus(uint32_t *anData, ADI_EAL_FIFO_STATUS_DATA *poStatusData);


#endif /* ADI_E2B_IFACE_DIAG_ENABLED */
#endif /* ADI_EAL_DIAG_H */

/** @} */

/**
 * EOF: www.analog.com
 */
