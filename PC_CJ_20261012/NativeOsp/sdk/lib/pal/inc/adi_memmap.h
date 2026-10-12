/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_memmap.h
 * \brief: The memory map configuration file for the application
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup main
 *  @{
 */

#ifndef ADI_MEMMAP_H
#define ADI_MEMMAP_H

/*================================= DEFINES =================================*/
#if defined(__ADSPSC59x__)
#define ADI_MEM_CODE_CRIT           __attribute__ ((section (".l2_cached_code")))
#define ADI_MEM_CODE_NOCRIT         __attribute__ ((section (".l3_code")))
#define ADI_MEM_DATA_CRIT_CACHE     __attribute__ ((section (".l2_cached_data")))
#define ADI_MEM_DATA_CRIT_NOCACHE   __attribute__ ((section (".l2_uncached_data")))
#define ADI_MEM_DATA_NOCRIT_CACHE   __attribute__ ((section (".l3_data")))
#define ADI_MEM_DATA_NOCRIT_NOCACHE __attribute__ ((section (".l3_uncached_data")))
#define ADI_MEM_DATA_CONST          __attribute__ ((section (".rodata")))
#define ADI_OASPI_DATA_CONST        __attribute__ ((section (".rodata")))

#define ADI_MEM_ALIGN4              __attribute__ ((aligned(4)))
#define ADI_MEM_ALIGN8              __attribute__ ((aligned(8)))
#define ADI_MEM_ALIGN16             __attribute__ ((aligned(16)))
#define ADI_MEM_ALIGN32             __attribute__ ((aligned(32)))
#define ADI_MEM_ALIGN1024           __attribute__ ((aligned(1024)))
#define ADI_MEM_ALIGN4096           __attribute__ ((aligned(4096)))
#define ADI_MEM_ALIGN16384          __attribute__ ((aligned(16384)))

#define ADI_MEM_IRQ                __attribute__ ((interrupt ("IRQ")))

#define ADI_E2BCORE_ADC_CODE       __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_FIFO_CODE      __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_I2C_CODE       __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_ISELED_CODE    __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_LIN_CODE       __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_PORTCTRL_CODE  __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_REGMAP_CODE    __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_SPI_CODE       __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_FIO_CODE       __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_UART_CODE      __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_UTILS_CODE     __attribute__ ((section (".l2_cached_code")))
#define ADI_E2BCORE_CODE           __attribute__ ((section (".l2_cached_code")))

#define ADI_EAL_ADC_CODE           __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_DIAG_CODE          __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_I2C_CODE           __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_ISELED_CODE        __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_PORTCTRL_CODE      __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_LIN_CODE           __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_SPI_CODE           __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_FIOPWM_CODE        __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_FIOPDM_CODE        __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_FIOOSP_CODE        __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_FIOUART_CODE       __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_SQI_CODE           __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_SSC_CODE           __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_SMC_CODE           __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_TOPO_CODE          __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_UART_CODE          __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_WKSLP_CODE         __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_MELIBU_CODE        __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_CODE               __attribute__ ((section (".l2_cached_code")))
#define ADI_EAL_OTP_CODE           __attribute__ ((section (".l2_cached_code")))

#define ADI_NETWORK_CODE          __attribute__ ((section (".l2_cached_code")))
#define ADI_NETWORK_ETH_CODE      __attribute__ ((section (".l2_cached_code")))
#define ADI_NETWORK_MACPHY_CODE   __attribute__ ((section (".l2_cached_code")))

#define ADI_OASPI_CODE            __attribute__ ((section (".l2_cached_code")))
#define ADI_OASPI_CODE_NOCRIT     __attribute__ ((section (".l3_code")))
#define ADI_OASPI_PRIO_CODE       __attribute__ ((section (".l2_cached_code")))

#define ADI_COMMON_CODE           __attribute__ ((section (".l2_cached_code")))

#define ADI_NETWORK_MACPHY_DATA   __attribute__ ((section (".l2_cached_data")))
#define ADI_OASPI_DATA            __attribute__ ((section (".l2_cached_data")))
#define ADI_OASPI_DATA_NOCACHE    __attribute__ ((section (".l2_uncached_data")))
#define ADI_EAL_UART_DATA         __attribute__ ((section (".l2_cached_data")))
#define ADI_EAL_WKSLP_DATA        __attribute__ ((section (".l2_cached_data")))
#define ADI_EAL_SQI_DATA          __attribute__ ((section (".l2_cached_data")))
#define ADI_SSC_DATA              __attribute__ ((section (".l2_cached_data")))
#define ADI_EAL_SMC_DATA          __attribute__ ((section (".l2_cached_data")))
#define ADI_EAL_TOPO_DATA         __attribute__ ((section (".l2_cached_data")))
#define ADI_EAL_DIAG_DATA         __attribute__ ((section (".l2_cached_data")))
#define ADI_EAL_DATA              __attribute__ ((section (".l2_cached_data")))
#define ADI_E2BCORE_DATA          __attribute__ ((section (".l2_cached_data")))

#elif defined(__NUCLEO_STM32__)
#define ADI_E2BCORE_ADC_CODE      __attribute__ ((section(".e2bcore_adc_cached_code")))
#define ADI_E2BCORE_FIFO_CODE     __attribute__ ((section(".e2bcore_fifo_cached_code")))
#define ADI_E2BCORE_I2C_CODE      __attribute__ ((section(".e2bcore_i2c_cached_code")))
#define ADI_E2BCORE_ISELED_CODE   __attribute__ ((section(".e2bcore_iseled_cached_code")))
#define ADI_E2BCORE_LIN_CODE      __attribute__ ((section(".e2bcore_lin_cached_code")))
#define ADI_E2BCORE_PORTCTRL_CODE __attribute__ ((section(".e2bcore_portctrl_cached_code")))
#define ADI_E2BCORE_REGMAP_CODE   __attribute__ ((section(".e2bcore_regmap_cached_code")))
#define ADI_E2BCORE_SPI_CODE      __attribute__ ((section(".e2bcore_spi_cached_code")))
#define ADI_E2BCORE_FIO_CODE      __attribute__ ((section(".e2bcore_fio_cached_code")))
#define ADI_E2BCORE_UART_CODE     __attribute__ ((section(".e2bcore_uart_cached_code")))
#define ADI_E2BCORE_UTILS_CODE    __attribute__ ((section(".e2bcore_utils_cached_code")))
#define ADI_E2BCORE_CODE          __attribute__ ((section(".e2bcore_cached_code")))

#define ADI_EAL_ADC_CODE         __attribute__ ((section(".eal_adc_cached_code")))
#define ADI_EAL_DIAG_CODE        __attribute__ ((section(".eal_diag_cached_code")))
#define ADI_EAL_I2C_CODE         __attribute__ ((section(".eal_i2c_cached_code")))
#define ADI_EAL_ISELED_CODE      __attribute__ ((section(".eal_iseled_cached_code")))
#define ADI_EAL_PORTCTRL_CODE    __attribute__ ((section(".eal_portctrl_cached_code")))
#define ADI_EAL_LIN_CODE         __attribute__ ((section(".eal_lin_cached_code")))
#define ADI_EAL_SPI_CODE         __attribute__ ((section(".eal_spi_cached_code")))
#define ADI_EAL_FIOPWM_CODE      __attribute__ ((section(".eal_fiopwm_cached_code")))
#define ADI_EAL_FIOPDM_CODE      __attribute__ ((section(".eal_fiopdm_cached_code")))
#define ADI_EAL_FIOOSP_CODE      __attribute__ ((section(".eal_fioosp_cached_code")))
#define ADI_EAL_FIOUART_CODE     __attribute__ ((section(".eal_fiouart_cached_code")))
#define ADI_EAL_SQI_CODE         __attribute__ ((section(".eal_sqi_cached_code")))
#define ADI_EAL_SSC_CODE         __attribute__ ((section(".eal_ssc_cached_code")))
#define ADI_EAL_SMC_CODE         __attribute__ ((section(".eal_smc_cached_code")))
#define ADI_EAL_TOPO_CODE        __attribute__ ((section(".eal_topo_cached_code")))
#define ADI_EAL_UART_CODE        __attribute__ ((section(".eal_uart_cached_code")))
#define ADI_EAL_WKSLP_CODE       __attribute__ ((section(".eal_wkslp_cached_code")))
#define ADI_EAL_MELIBU_CODE      __attribute__ ((section(".eal_melibu_cached_code")))
#define ADI_EAL_CODE             __attribute__ ((section(".eal_cached_code")))
#define ADI_EAL_OTP_CODE         __attribute__ ((section(".eal_otp_cached_code")))

#define ADI_NETWORK_CODE         __attribute__ ((section(".network_cached_code")))
#define ADI_NETWORK_ETH_CODE     __attribute__ ((section(".network_eth_cached_code")))
#define ADI_NETWORK_MACPHY_CODE  __attribute__ ((section(".network_macphy_cached_code")))

#define ADI_OASPI_CODE           __attribute__ ((section(".oaspi_cached_code")))
#define ADI_OASPI_CODE_NOCRIT    __attribute__ ((section(".oaspi_cached_code")))
#define ADI_OASPI_PRIO_CODE      __attribute__ ((section(".oaspi_prio_cached_code")))

#define ADI_COMMON_CODE          __attribute__ ((section(".common_cached_code")))

#define ADI_NETWORK_MACPHY_DATA  __attribute__ ((section(".network_macphy_data")))
#define ADI_OASPI_DATA           __attribute__ ((section(".oaspi_data")))
#define ADI_OASPI_DATA_NOCACHE   __attribute__ ((section(".oaspi_data")))
#define ADI_EAL_UART_DATA        __attribute__ ((section(".eal_uart_data")))
#define ADI_EAL_WKSLP_DATA       __attribute__ ((section(".eal_wkslp_data")))
#define ADI_EAL_SQI_DATA         __attribute__ ((section(".eal_sqi_data")))
#define ADI_SSC_DATA             __attribute__ ((section(".eal_ssc_data")))
#define ADI_EAL_SMC_DATA         __attribute__ ((section(".eal_smc_data")))
#define ADI_EAL_TOPO_DATA        __attribute__ ((section(".eal_topo_data")))
#define ADI_EAL_DIAG_DATA        __attribute__ ((section(".eal_diag_data")))
#define ADI_EAL_DATA             __attribute__ ((section(".eal_data")))
#define ADI_E2BCORE_DATA         __attribute__ ((section(".e2bcore_data")))

#define ADI_MEM_CODE_CRIT
#define ADI_MEM_CODE_NOCRIT
#define ADI_MEM_DATA_CRIT_CACHE
#define ADI_MEM_DATA_CRIT_NOCACHE
#define ADI_MEM_DATA_NOCRIT_CACHE
#define ADI_MEM_DATA_NOCRIT_NOCACHE
#define ADI_MEM_DATA_CONST
#define ADI_OASPI_DATA_CONST

#define ADI_MEM_ALIGN4
#define ADI_MEM_ALIGN8
#define ADI_MEM_ALIGN16
#define ADI_MEM_ALIGN32
#define ADI_MEM_ALIGN1024
#define ADI_MEM_ALIGN4096
#define ADI_MEM_ALIGN16384

#define ADI_MEM_IRQ

#else

#define ADI_E2BCORE_ADC_CODE
#define ADI_E2BCORE_FIFO_CODE
#define ADI_E2BCORE_I2C_CODE
#define ADI_E2BCORE_ISELED_CODE
#define ADI_E2BCORE_LIN_CODE
#define ADI_E2BCORE_PORTCTRL_CODE
#define ADI_E2BCORE_REGMAP_CODE
#define ADI_E2BCORE_SPI_CODE
#define ADI_E2BCORE_FIO_CODE
#define ADI_E2BCORE_UART_CODE
#define ADI_E2BCORE_UTILS_CODE
#define ADI_E2BCORE_CODE

#define ADI_EAL_ADC_CODE
#define ADI_EAL_DIAG_CODE
#define ADI_EAL_I2C_CODE
#define ADI_EAL_ISELED_CODE
#define ADI_EAL_PORTCTRL_CODE
#define ADI_EAL_LIN_CODE
#define ADI_EAL_SPI_CODE
#define ADI_EAL_FIOPWM_CODE
#define ADI_EAL_FIOPDM_CODE
#define ADI_EAL_FIOOSP_CODE
#define ADI_EAL_FIOUART_CODE
#define ADI_EAL_SQI_CODE
#define ADI_EAL_SSC_CODE
#define ADI_EAL_SMC_CODE
#define ADI_EAL_TOPO_CODE
#define ADI_EAL_UART_CODE
#define ADI_EAL_WKSLP_CODE
#define ADI_EAL_MELIBU_CODE
#define ADI_EAL_CODE
#define ADI_EAL_OTP_CODE

#define ADI_NETWORK_CODE
#define ADI_NETWORK_ETH_CODE
#define ADI_NETWORK_MACPHY_CODE

#define ADI_OASPI_CODE
#define ADI_OASPI_CODE_NOCRIT
#define ADI_OASPI_PRIO_CODE

#define ADI_COMMON_CODE

#define ADI_NETWORK_MACPHY_DATA
#define ADI_OASPI_DATA
#define ADI_OASPI_DATA_NOCACHE
#define ADI_EAL_UART_DATA
#define ADI_EAL_WKSLP_DATA
#define ADI_EAL_SQI_DATA
#define ADI_SSC_DATA
#define ADI_EAL_SMC_DATA
#define ADI_EAL_TOPO_DATA
#define ADI_EAL_DIAG_DATA
#define ADI_EAL_DATA
#define ADI_E2BCORE_DATA

#define ADI_MEM_CODE_CRIT
#define ADI_MEM_CODE_NOCRIT
#define ADI_MEM_DATA_CRIT_CACHE
#define ADI_MEM_DATA_CRIT_NOCACHE
#define ADI_MEM_DATA_NOCRIT_CACHE
#define ADI_MEM_DATA_NOCRIT_NOCACHE
#define ADI_MEM_DATA_CONST
#define ADI_OASPI_DATA_CONST

#define ADI_MEM_ALIGN4
#define ADI_MEM_ALIGN8
#define ADI_MEM_ALIGN16
#define ADI_MEM_ALIGN32
#define ADI_MEM_ALIGN1024
#define ADI_MEM_ALIGN4096
#define ADI_MEM_ALIGN16384

#define ADI_MEM_IRQ
#endif

#endif /* ADI_MEMMAP_H */

/** @} */

/*
 * EOF: www.analog.com/
 */
