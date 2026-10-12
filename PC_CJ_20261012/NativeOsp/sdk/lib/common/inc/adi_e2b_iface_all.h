/*******************************************************************************
Copyright (c) 2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_e2b_iface_all.h
 * \brief: Master header that enables all E2B interface feature macros.
 *
 * Include this header to enable all interfaces at compile time (default
 * behavior). When EdgeStudio generates a configuration with a specific
 * subset of interfaces, the generated config header defines only the
 * required macros, and this file should NOT be included.
 *
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/

#ifndef ADI_E2B_IFACE_ALL_H
#define ADI_E2B_IFACE_ALL_H

/* Core interfaces */
#ifndef ADI_E2B_IFACE_UART_ENABLED
#define ADI_E2B_IFACE_UART_ENABLED
#endif

#ifndef ADI_E2B_IFACE_SPI_ENABLED
#define ADI_E2B_IFACE_SPI_ENABLED
#endif

#ifndef ADI_E2B_IFACE_I2C_ENABLED
#define ADI_E2B_IFACE_I2C_ENABLED
#endif

#ifndef ADI_E2B_IFACE_ADC_ENABLED
#define ADI_E2B_IFACE_ADC_ENABLED
#endif

#ifndef ADI_E2B_IFACE_ISELED_ENABLED
#define ADI_E2B_IFACE_ISELED_ENABLED
#endif

#ifndef ADI_E2B_IFACE_LIN_ENABLED
#define ADI_E2B_IFACE_LIN_ENABLED
#endif

#ifndef ADI_E2B_IFACE_PORT_ENABLED
#define ADI_E2B_IFACE_PORT_ENABLED
#endif

#ifndef ADI_E2B_IFACE_MELIBU_ENABLED
#define ADI_E2B_IFACE_MELIBU_ENABLED
#endif

/* Flexible IO (parent + sub-types) */
#ifndef ADI_E2B_IFACE_FIO_ENABLED
#define ADI_E2B_IFACE_FIO_ENABLED
#endif

#ifndef ADI_E2B_IFACE_FIO_UART_ENABLED
#define ADI_E2B_IFACE_FIO_UART_ENABLED
#endif

#ifndef ADI_E2B_IFACE_FIO_PDM_ENABLED
#define ADI_E2B_IFACE_FIO_PDM_ENABLED
#endif

#ifndef ADI_E2B_IFACE_FIO_PWM_ENABLED
#define ADI_E2B_IFACE_FIO_PWM_ENABLED
#endif

#ifndef ADI_E2B_IFACE_FIO_OSP_ENABLED
#define ADI_E2B_IFACE_FIO_OSP_ENABLED
#endif

/* Default-always-on interfaces */
#ifndef ADI_E2B_IFACE_SMC_ENABLED
#define ADI_E2B_IFACE_SMC_ENABLED
#endif

#ifndef ADI_E2B_IFACE_SQI_ENABLED
#define ADI_E2B_IFACE_SQI_ENABLED
#endif

#ifndef ADI_E2B_IFACE_SSC_ENABLED
#define ADI_E2B_IFACE_SSC_ENABLED
#endif

#ifndef ADI_E2B_IFACE_TOPDISC_ENABLED
#define ADI_E2B_IFACE_TOPDISC_ENABLED
#endif

#ifndef ADI_E2B_IFACE_WKSLP_ENABLED
#define ADI_E2B_IFACE_WKSLP_ENABLED
#endif

#ifndef ADI_E2B_IFACE_DIAG_ENABLED
#define ADI_E2B_IFACE_DIAG_ENABLED
#endif

/* OTP interface */
#ifndef ADI_E2B_IFACE_OTP_ENABLED
#define ADI_E2B_IFACE_OTP_ENABLED
#endif

#endif /* ADI_E2B_IFACE_ALL_H */
