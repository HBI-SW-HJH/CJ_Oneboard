/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_common.h
 * \brief: Provides common data types for all components in this application
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup common
 *  @{
 */

#ifndef ADI_COMMON_TYPES_H
#define ADI_COMMON_TYPES_H

/*================================= INCLUDES ================================*/
#include "adi_stdlib.h"  /*!< Standard configuration file */
#ifndef ADI_E2B_IFACE_SELECTIVE
#include "adi_e2b_iface_all.h"  /* Default: enable all interfaces */
#endif
/*================================== DEFINES ================================*/
/** Time Conversion MACROS */
#define US_TO_NS        (1000u)
#define MS_TO_NS        (1000000u)
#define MS_TO_US        (1000u)
#define SEC_TO_NS       (1000000000u)
#define SEC_TO_NS_FLOAT (1000000000.0f)
#define SEC_TO_US       (1000000u)
#define SEC_TO_MS       (1000u)

/** Size Conversion MACROS */
#define BYTE_TO_BIT     (8u)
#define SHORT_TO_BYTE   (2u)
#define SHORT_TO_BIT    (16u)
#define WORD_TO_SHORT   (2u)
#define WORD_TO_BYTE    (4u)
#define WORD_TO_BIT     (32u)
#define LONG_TO_WORD    (2u)
#define LONG_TO_SHORT   (4u)
#define LONG_TO_BYTE    (8u)
#define LONG_TO_BIT     (64u)

/* Bit Manipulation MACROS */
/* Shift right by 24 bits */
#define ADI_SHIFT_RIGHT_24(x)  ((x) >> 24)

/* Extract VLAN TPID */
#define EXTRACT_VLAN_TPID(N)        (((N) & 0xFFFF0000U) >> 16U)

/* VLAN S-TAG ethertype */
#define VLAN_S_TAG_TPID             (0x88A8U)

/* VLAN C-TAG ethertype */
#define VLAN_C_TAG_TPID             (0x8100U)

/* E2B protocol ethertype */
#define ADI_E2BCORE_ETYPE           (0xAAC5U)

/* 1722 protocol ethertype */
#define IEEE_1722_ETYPE             (0x22F0U)

#ifdef ADI_DEBUG
/* Buffer Alignment Check MACROS */
/**
 * @brief Check if a pointer is aligned to the specified byte boundary
 * @param ptr Pointer to check
 * @param alignment Alignment boundary (must be power of 2, e.g., 4, 8)
 * @return true if aligned, false otherwise
 */
#define ADI_IS_ALIGNED(ptr, alignment) (((uintptr_t)(const void *)(ptr)) % (alignment) == 0U)

/**
 * @brief Assert that a buffer pointer is aligned to 4 bytes
 * @param ptr Pointer to check
 * @note This macro should be used before passing buffers to EAL APIs
 */
#define ADI_ASSERT_BUFFER_ALIGNED_4(ptr) \
    do { \
        if (!ADI_IS_ALIGNED(ptr, 4U)) { \
            ADI_DBG_LOG(LOG_ERROR, stderr, "Buffer alignment error: pointer is not 4-byte aligned"); \
        } \
    } while(0)

/**
 * @brief Assert that a buffer pointer is aligned to 8 bytes
 * @param ptr Pointer to check
 * @note This macro should be used before passing buffers to EAL APIs on 64-bit platforms
 */
#define ADI_ASSERT_BUFFER_ALIGNED_8(ptr) \
    do { \
        if (!ADI_IS_ALIGNED(ptr, 8U)) { \
            ADI_DBG_LOG(LOG_ERROR, stderr, "Buffer alignment error: pointer is not 8-byte aligned"); \
        } \
    } while(0)
#endif /* ADI_DEBUG */

/* SSC Macros */
#define ADI_SSC_HW_ADC_CNT                            (4U)                   /*!< Number of HW_ADC pins available to be monitored by Safe state controller */
#define ADI_SSC_SA_IF_CNT                             (12U)                  /*!< Number of SA_IF pins available to be monitored by Safe state controller */
#define ADI_SSC_SA_IF_(N)                             (0x00000001U << (N))   /*!< SA_IF pin that has to be monitored / set polarity / enable auto return , N indicates the SA_IF pin number */
#define ADI_SSC_SA_IF_ALL                             (0x00000FFFU)          /*!< can be used to enable the montioring / can be used to set polarity / can be used to auto return of all SA_IF pins in SSC */
#define ADI_SSC_HW_ADC_(N)                            (0x00001000U << (N))   /*!< HW_ADC pin that has to be monitored /  can be used to set polarity /  can be used to enable auto return in SSC , N indicates the HW_ADC pin number */
#define ADI_SSC_SW_ADC_(N)                            (0x00010000U << (N))   /*!< SW_ADC pin that has to be monitored /  can be used to set polarity /  can be used to enable auto return in SSC , N indicates the SW_ADC pin number */
#define ADI_SSC_GPTP_UNLOCKED                         (0x00100000U)          /*!< Monitor / sets polarity / enable auto return for GPTP Unlock event in SSC */
#define ADI_SSC_OTP_CONFIG                            (0x00200000U)          /*!< Monitor / sets polarity for OTP config failed event in SSC*/
#define ADI_SSC_SQI_LVL                               (0x00400000U)          /*!< Monitor / sets polarity for SQI warning event in SSC*/
#define ADI_SSC_MANUAL                                (0x00800000U)          /*!< Monitor / sets polarity for Manual trigger in SSC*/
#define ADI_SSC_ETH_FRAME_CHECK                       (0x40000000U)          /*!< Monitoring the ethernet frame receive check option / can be used to set auto return for this option in SSC */
#define ADI_SSC_DEVICE_UNATTENDED_CHECK               (0x80000000U)          /*!< Montioring of Device unattended bit in SSC */
#define ADI_SSC_MONITOR_ALL                           (0xC0FFFFFFU)          /*!< Monitoring of all Safe state Controller triggers including tiemeouts in SSC */
#define ADI_SSC_HW_ADC_MONITOR_FLAG_LOWER_THAN_LOW_THRESHOLD     (0u)        /*!< To set HW_ADC pin to trigger the safe state if the value has gone lower than the low threshold that is programmed */
#define ADI_SSC_HW_ADC_MONITOR_FLAG_HIGH_THAN_LOW_THRESHOLD      (1u)        /*!< To set HW_ADC to trigger the safe state if the value has gone higher than the high threshold that is programmed */
#define ADI_SSC_SQI_LVL_RED_TRIGGER                              (0u)        /*!< To set SQI RED level (indicates the signal quality) to trigger safe state */
#define ADI_SSC_SQI_LVL_YELLOW_AND_RED_TRIGGER                   (1u)        /*!< To set SQI RED and YELLOW level (indicates the signal quality) to trigger safe state */

/* Number of status register */
#define NUM_MAC_STATUS_REG            (26U)   /*!< Number of MAC status register reads */
#define NUM_PHY_STATUS_REG            (5U)    /*!< Number of PHY status register reads */

/* Bootloader done rising edge interrupt */
#define BOOTLOADER_DONE_RISING_IRQ_ID                   (29u)
#define BOOTLOADER_DONE_RISING_IRQ_MASK                 (1U << ENUM_IO_TIC_ENABLE_29_MISC1_REG0_TIC_ENABLE_29_RISING_BOOTLOADER_DONE_RISING)
#define BOOTLOADER_DONE_RISING_IRQ_TIC_CLR_REG_ADDR     (IO_TIC_CLEAR_29_MISC1_REG0_ADDR)
#define BOOTLOADER_DONE_RISING_IRQ_TIC_CLR_NW_REG_ADDR  (IO_TIC_CLEAR_29_MISC1_REG0_NW_ADDR)
#define BOOTLOADER_DONE_RISING_IRQ_TIC_FIELD_MASK       (1U << ENUM_IO_TIC_CLEAR_29_MISC1_REG0_TIC_CLEAR_29_RISING_BOOTLOADER_DONE_LOW)

/* Wake-sleep interrupts info */
#define WAKE_SLEEP_IRQ_ID                               (28u)
#define WAKE_SLEEP_IRQ_TIC_CLR_REG_NW_ADDR              (IO_TIC_CLEAR_28_MISC0_REG0_NW_ADDR)

/* Confirm awake rising edge interrupt */
#define CONFIRM_AWAKE_RISING_IRQ_ID                     (28u)
#define CONFIRM_AWAKE_RISING_IRQ_MASK                   (1U << ENUM_IO_TIC_ENABLE_28_MISC0_REG0_TIC_ENABLE_28_SLPWK_FLAG_CONFIRM_AWAKE_RISING)
#define CONFIRM_AWAKE_RISING_IRQ_TIC_CLR_REG_ADDR       (IO_TIC_CLEAR_28_MISC0_REG0_ADDR)
#define CONFIRM_AWAKE_RISING_IRQ_TIC_CLR_NW_REG_ADDR    (IO_TIC_CLEAR_28_MISC0_REG0_NW_ADDR)
#define CONFIRM_AWAKE_RISING_IRQ_TIC_FIELD_MASK         (1U << ENUM_IO_TIC_CLEAR_28_MISC0_REG0_TIC_CLEAR_28_SLPWK_FLAG_CONFIRM_AWAKE_RISING)

/* Line busy rising edge interrupt */
#define LINE_BUSY_RISING_IRQ_ID                         (28u)
#define LINE_BUSY_RISING_IRQ_MASK                       (1U << ENUM_IO_TIC_ENABLE_28_MISC0_REG0_TIC_ENABLE_28_SLPWK_FLAG_LINE_BUSY_RISING)
#define LINE_BUSY_RISING_IRQ_TIC_CLR_REG_ADDR           (IO_TIC_CLEAR_28_MISC0_REG0_ADDR)
#define LINE_BUSY_RISING_IRQ_TIC_CLR_NW_REG_ADDR        (IO_TIC_CLEAR_28_MISC0_REG0_NW_ADDR)
#define LINE_BUSY_RISING_IRQ_TIC_FIELD_MASK             (1U << ENUM_IO_TIC_CLEAR_28_MISC0_REG0_TIC_CLEAR_28_SLPWK_FLAG_LINE_BUSY_RISING)

/* Sleep error rising edge interrupt */
#define SLEEP_ERR_RISING_IRQ_ID                         (28u)
#define SLEEP_ERR_RISING_IRQ_MASK                       (1U << ENUM_IO_TIC_ENABLE_28_MISC0_REG0_TIC_ENABLE_28_SLPWK_FLAG_SLEEP_ERR_RISING)
#define SLEEP_ERR_RISING_IRQ_TIC_CLR_REG_ADDR           (IO_TIC_CLEAR_28_MISC0_REG0_ADDR)
#define SLEEP_ERR_RISING_IRQ_TIC_CLR_NW_REG_ADDR        (IO_TIC_CLEAR_28_MISC0_REG0_NW_ADDR)
#define SLEEP_ERR_RISING_IRQ_TIC_FIELD_MASK             (1U << ENUM_IO_TIC_CLEAR_28_MISC0_REG0_TIC_CLEAR_28_SLPWK_FLAG_SLEEP_ERR_RISING)

/* Standby error rising edge interrupt */
#define STANDBY_ERR_RISING_IRQ_ID                       (28u)
#define STANDBY_ERR_RISING_IRQ_MASK                     (1U << ENUM_IO_TIC_ENABLE_28_MISC0_REG0_TIC_ENABLE_28_SLPWK_FLAG_STANDBY_ERR_RISING)
#define STANDBY_ERR_RISING_IRQ_TIC_CLR_REG_ADDR         (IO_TIC_CLEAR_28_MISC0_REG0_ADDR)
#define STANDBY_ERR_RISING_IRQ_TIC_CLR_NW_REG_ADDR      (IO_TIC_CLEAR_28_MISC0_REG0_NW_ADDR)
#define STANDBY_ERR_RISING_IRQ_TIC_FIELD_MASK           (1U << ENUM_IO_TIC_CLEAR_28_MISC0_REG0_TIC_CLEAR_28_SLPWK_FLAG_STANDBY_ERR_RISING)

/* Return from standby rising edge interrupt */
#define RET_FROM_STDBY_RISING_IRQ_ID                    (28u)
#define RET_FROM_STDBY_RISING_IRQ_MASK                  (1U << ENUM_IO_TIC_ENABLE_28_MISC0_REG0_TIC_ENABLE_28_SLPWK_FLAG_RETURNED_FROM_STANDBY_RISING)
#define RET_FROM_STDBY_RISING_IRQ_TIC_CLR_REG_ADDR      (IO_TIC_CLEAR_28_MISC0_REG0_ADDR)
#define RET_FROM_STDBY_RISING_IRQ_TIC_CLR_NW_REG_ADDR   (IO_TIC_CLEAR_28_MISC0_REG0_NW_ADDR)
#define RET_FROM_STDBY_RISING_IRQ_TIC_FIELD_MASK        (1U << ENUM_IO_TIC_CLEAR_28_MISC0_REG0_TIC_CLEAR_28_SLPWK_FLAG_RETURNED_FROM_STANDBY_RISING)

/* Request confirmation for system awake rising edge interrupt */
#define REQ_SYS_WK_CONF_RISING_IRQ_ID                   (28u)
#define REQ_SYS_WK_CONF_RISING_IRQ_MASK                 (1U << ENUM_IO_TIC_ENABLE_28_MISC0_REG0_TIC_ENABLE_28_SLPWK_FLAG_REQ_SYSTEM_WAKEUP_CONF_RISING)
#define REQ_SYS_WK_CONF_RISING_IRQ_TIC_CLR_REG_ADDR     (IO_TIC_CLEAR_28_MISC0_REG0_ADDR)
#define REQ_SYS_WK_CONF_RISING_IRQ_TIC_CLR_NW_REG_ADDR  (IO_TIC_CLEAR_28_MISC0_REG0_NW_ADDR)
#define REQ_SYS_WK_CONF_RISING_IRQ_TIC_FIELD_MASK       (1U << ENUM_IO_TIC_CLEAR_28_MISC0_REG0_TIC_CLEAR_28_SLPWK_FLAG_REQ_SYSTEM_WAKEUP_CONF_RISING)

/* SSC and SQI interrupts info */
#define SSC_SQI_IRQ_ID                                              (34u)
#define SSC_SQI_IRQ_TIC_CLR_REG_NW_ADDR                             (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)

/* Functional to safe mode transition */
#define SSC_MODE_CHANGE_FLAG_RISING_IRQ_ID                          (34U)
#define SSC_MODE_CHANGE_FLAG_RISING_IRQ_MASK                        (1U << ENUM_IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_RISING_SSC_MODE_CHANGE_FLAG_RISING)
#define SSC_MODE_CHANGE_FLAG_RISING_IRQ_TIC_CLR_REG_ADDR            (IO_TIC_CLEAR_34_GPIO1_REG0_ADDR)
#define SSC_MODE_CHANGE_FLAG_RISING_IRQ_TIC_CLR_REG_NW_ADDR         (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)
#define SSC_MODE_CHANGE_FLAG_RISING_IRQ_TIC_FIELD_MASK              (1U << ENUM_IO_TIC_CLEAR_34_GPIO1_REG0_TIC_CLEAR_34_RISING_SSC_MODE_CHANGE_FLAG_RISING)

/* Safe mode to Functional transition */
#define SSC_MODE_CHANGE_FLAG_FALLING_IRQ_ID                         (34U)
#define SSC_MODE_CHANGE_FLAG_FALLING_IRQ_MASK                       (1U << ENUM_IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_FALLING_SSC_MODE_CHANGE_FLAG_FALLING)
#define SSC_MODE_CHANGE_FLAG_FALLING_IRQ_TIC_CLR_REG_ADDR           (IO_TIC_CLEAR_34_GPIO1_REG0_ADDR)
#define SSC_MODE_CHANGE_FLAG_FALLING_IRQ_TIC_CLR_REG_NW_ADDR        (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)
#define SSC_MODE_CHANGE_FLAG_FALLING_IRQ_TIC_FIELD_MASK             ((uint32_t)1U << 12U)

/* Return to functional mode failed */
#define SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_ID                  (34u)
#define SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_MASK                (1U << ENUM_IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_RISING_SSC_UNSUCCESSFUL_RETURN_FUNC_RISING)
#define SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_TIC_CLR_REG_ADDR    (IO_TIC_CLEAR_34_GPIO1_REG0_ADDR)
#define SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_TIC_CLR_REG_NW_ADDR (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)
#define SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_TIC_FIELD_MASK      (1U << ENUM_IO_TIC_CLEAR_34_GPIO1_REG0_TIC_CLEAR_34_RISING_SSC_UNSUCCESSFUL_RETURN_FUNC_RISING)

/* RISING Warning for SQI (entered warning) */
#define SQI_RISING_WARNING_IRQ_ID                                   (34u)
#define SQI_RISING_WARNING_IRQ_MASK                                 (1U << ENUM_IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_RISING_SQI_REACHES_WARNING_RISING)
#define SQI_RISING_WARNING_IRQ_TIC_CLR_REG_ADDR                     (IO_TIC_CLEAR_34_GPIO1_REG0_ADDR)
#define SQI_RISING_WARNING_IRQ_TIC_CLR_REG_NW_ADDR                  (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)
#define SQI_RISING_WARNING_IRQ_TIC_FIELD_MASK                       (1U << ENUM_IO_TIC_CLEAR_34_GPIO1_REG0_TIC_CLEAR_34_RISING_SQI_REACHES_WARNING_RISING)

/* RISING Bad for SQI (entered bad)*/
#define SQI_RISING_BAD_IRQ_ID                                       (34u)
#define SQI_RISING_BAD_IRQ_MASK                                     (1U << ENUM_IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_RISING_SQI_REACHES_BAD_RISING)
#define SQI_RISING_BAD_IRQ_TIC_CLR_REG_ADDR                         (IO_TIC_CLEAR_34_GPIO1_REG0_ADDR)
#define SQI_RISING_BAD_IRQ_TIC_CLR_REG_NW_ADDR                      (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)
#define SQI_RISING_BAD_IRQ_TIC_FIELD_MASK                           (1U << ENUM_IO_TIC_CLEAR_34_GPIO1_REG0_TIC_CLEAR_34_RISING_SQI_REACHES_BAD_RISING)

/* FALLING Warning for SQI (exiting warning) */
#define SQI_FALLING_WARNING_IRQ_ID                                  (34u)
#define SQI_FALLING_WARNING_IRQ_MASK                                (1U << ENUM_IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_FALLING_SQI_REACHES_WARNING_FALLING)
#define SQI_FALLING_WARNING_IRQ_TIC_CLR_REG_ADDR                    (IO_TIC_CLEAR_34_GPIO1_REG0_ADDR)
#define SQI_FALLING_WARNING_IRQ_TIC_CLR_REG_NW_ADDR                 (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)
#define SQI_FALLING_WARNING_IRQ_TIC_FIELD_MASK                      ((uint32_t)1U << 14U)

/* FALLING Bad for SQI (exiting bad) */
#define SQI_FALLING_BAD_IRQ_ID                                      (34u)
#define SQI_FALLING_BAD_IRQ_MASK                                    (1U << ENUM_IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_FALLING_SQI_REACHES_BAD_FALLING)
#define SQI_FALLING_BAD_IRQ_TIC_CLR_REG_ADDR                        (IO_TIC_CLEAR_34_GPIO1_REG0_ADDR)
#define SQI_FALLING_BAD_IRQ_TIC_CLR_REG_NW_ADDR                     (IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR)
#define SQI_FALLING_BAD_IRQ_TIC_FIELD_MASK                          ((uint32_t)1U << 15U)

/* SMC interrupts info */
#define SMC_IRQ_ID                                                           (32u)
#define SMC_IRQ_TIC_CLR_REG_NW_ADDR                                          (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_NW_ADDR)

/* RISING Region collision */
#define SMC_RISING_REGION_COLLISION_IRQ_ID                                   (32u)
#define SMC_RISING_REGION_COLLISION_IRQ_MASK                                 (1U << ENUM_IO_TIC_ENABLE_32_LCE_SMC_SQI_REG0_TIC_ENABLE_32_RISING_LCE_SMC_REGION_COLLISION_RISING)
#define SMC_RISING_REGION_COLLISION_IRQ_TIC_CLR_REG_ADDR                     (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_ADDR)
#define SMC_RISING_REGION_COLLISION_IRQ_TIC_CLR_REG_NW_ADDR                  (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_NW_ADDR)
#define SMC_RISING_REGION_COLLISION_IRQ_TIC_FIELD_MASK                       (1U << ENUM_IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_TIC_CLEAR_32_RISING_LCE_SMC_REGION_COLLISION_RISING)

/* RISING Iteration end */
#define SMC_RISING_ITERATION_END_IRQ_ID                                      (32u)
#define SMC_RISING_ITERATION_END_IRQ_MASK                                    (1U << ENUM_IO_TIC_ENABLE_32_LCE_SMC_SQI_REG0_TIC_ENABLE_32_RISING_LCE_SMC_ITERATION_END_RISING)
#define SMC_RISING_ITERATION_END_IRQ_TIC_CLR_REG_ADDR                        (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_ADDR)
#define SMC_RISING_ITERATION_END_IRQ_TIC_CLR_REG_NW_ADDR                     (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_NW_ADDR)
#define SMC_RISING_ITERATION_END_IRQ_TIC_FIELD_MASK                          (1U << ENUM_IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_TIC_CLEAR_32_RISING_LCE_SMC_ITERATION_END_RISING)

/* RISING Iteration skip */
#define SMC_RISING_ITERATION_SKIP_IRQ_ID                                     (32u)
#define SMC_RISING_ITERATION_SKIP_IRQ_MASK                                   (1U << ENUM_IO_TIC_ENABLE_32_LCE_SMC_SQI_REG0_TIC_ENABLE_32_RISING_LCE_SMC_ITERATION_SKIP_RISING)
#define SMC_RISING_ITERATION_SKIP_IRQ_TIC_CLR_REG_ADDR                       (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_ADDR)
#define SMC_RISING_ITERATION_SKIP_IRQ_TIC_CLR_REG_NW_ADDR                    (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_NW_ADDR)
#define SMC_RISING_ITERATION_SKIP_IRQ_TIC_FIELD_MASK                         (1U << ENUM_IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_TIC_CLEAR_32_RISING_LCE_SMC_ITERATION_SKIP_RISING)

/* RISING Configuration bit change */
#define SMC_RISING_CONFIG_CHANGE_IRQ_ID                                      (32u)
#define SMC_RISING_CONFIG_CHANGE_IRQ_MASK                                    (1U << ENUM_IO_TIC_ENABLE_32_LCE_SMC_SQI_REG0_TIC_ENABLE_32_RISING_LCE_SMC_SELECTED_CONFIGURATION_CHANGE_RISING)
#define SMC_RISING_CONFIG_CHANGE_IRQ_TIC_CLR_REG_ADDR                        (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_ADDR)
#define SMC_RISING_CONFIG_CHANGE_IRQ_TIC_CLR_REG_NW_ADDR                     (IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_NW_ADDR)
#define SMC_RISING_CONFIG_CHANGE_IRQ_TIC_FIELD_MASK                          (1U << ENUM_IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_TIC_CLEAR_32_RISING_LCE_SMC_SELECTED_CONFIGURATION_CHANGE_RISING)

/* RISING Flexible IO Interrupt */
#define FLEXIO_RISING_CONFIGURABLE_INTERRUPT_0_IRQ_ID                        (25u)
#define FLEXIO_RISING_CONFIGURABLE_INTERRUPT_0_IRQ_MASK                      (1U << ENUM_IO_TIC_ENABLE_25_FLEXIO0_REG0_TIC_ENABLE_25_FLEX_IO0_CONFIG_INT_0_RISING)
#define FLEXIO_RISING_CONFIGURABLE_INTERRUPT_0_TIC_CLR_REG_ADDR              (IO_TIC_CLEAR_25_FLEXIO0_REG0_ADDR)
#define FLEXIO_RISING_CONFIGURABLE_INTERRUPT_0_TIC_CLR_REG_NW_ADDR           (IO_TIC_CLEAR_25_FLEXIO0_REG0_NW_ADDR)
#define FLEXIO_RISING_CONFIGURABLE_INTERRUPT_0_TIC_FIELD_MASK                (1U)

/* FIFO0 read thresholds */
#define FIFO0_READ_THRESHOLDS_ID                                   (0U)

/* FIFO0 write thresholds */
#define FIFO0_WRITE_THRESHOLDS_ID                                   (1U)

/* FIFO1 write thresholds */
#define FIFO1_WRITE_THRESHOLDS_ID                                   (3U)

/* FIFO2 write thresholds */
#define FIFO2_WRITE_THRESHOLDS_ID                                   (5U)

/* FIFO3 write thresholds */
#define FIFO3_WRITE_THRESHOLDS_ID                                   (7U)

/*================================== DATA TYPES ================================*/

/*! Defines the different egress timestamp registers */
typedef enum ADI_EGRESS_TS_REG
{
    ADI_TS_EGRESS_NONE, /*!< Don't capture egress timestamp */
    ADI_TS_EGRESS_A,    /*!< Egress A register */
    ADI_TS_EGRESS_B,    /*!< Egress B register */
    ADI_TS_EGRESS_C     /*!< Egress C register */
} ADI_EGRESS_TS_REG;

/*! Enum of reason for sleep */
typedef enum ADI_REASON_FOR_SLEEP
{
    ADI_NO_SLEEP_STATUS         = 0U,   /*!< No reason for sleep */
    ADI_REGMAP_REQ              = 1U,   /*!< Register map write to goto_sleep */
    ADI_VBAT_OVER_VOLTAGE       = 2U,   /*!< VBAT over voltage */
    ADI_VBAT_UNDER_VOLTAGE      = 4U,   /*!< VBAT under voltage */
    ADI_VBAT_DEEP_UNDER_VOLTAGE = 8U,   /*!< VBAT deep under voltage */
} ADI_REASON_FOR_SLEEP;

/*! Enum of reason for wake */
typedef enum ADI_REASON_FOR_WAKE
{
    ADI_NO_WAKE_STATUS          = 0U,   /*!< No reason for wake */
    ADI_WAKE_PIN_ASSERTED       = 1U,   /*!< Wake pin asserted */
    ADI_WUP_DETECTED            = 2U,   /*!< Wake up pulse detected */
    ADI_VBAT_RECOVERY           = 4U,   /*!< VBAT recovered */
    ADI_POR_RELEASE             = 8U,   /*!< Power-on reset released */
} ADI_REASON_FOR_WAKE;

/**
 * @brief Enum for HW_ADC pin mux options
 *
 */
typedef enum ADI_SSC_HW_ADC_MUX
{
    ADI_SSC_HW_ADC_MONITOR_MUX_SA_IF_0,           /*!< Mux the HW_ADC to SA_IF_0 */
    ADI_SSC_HW_ADC_MONITOR_MUX_SA_IF_1,           /*!< Mux the HW_ADC to SA_IF_1 */
    ADI_SSC_HW_ADC_MONITOR_MUX_SA_IF_2,           /*!< Mux the HW_ADC to SA_IF_2 */
    ADI_SSC_HW_ADC_MONITOR_MUX_SA_IF_3,           /*!< Mux the HW_ADC to SA_IF_3 */
    ADI_SSC_HW_ADC_MONITOR_MUX_SA_IF_4,           /*!< Mux the HW_ADC to SA_IF_4 */
    ADI_SSC_HW_ADC_MONITOR_MUX_SA_IF_5,           /*!< Mux the HW_ADC to SA_IF_5 */
    ADI_SSC_HW_ADC_MONITOR_MUX_VMON,              /*!< Mux the HW_ADC to VMON */
    ADI_SSC_HW_ADC_MONITOR_MUX_DVDDIO,            /*!< Mux the HW_ADC to DVDDIO */
    ADI_SSC_HW_ADC_MONITOR_MUX_LVDD,              /*!< Mux the HW_ADC to LVDD */
    ADI_SSC_HW_ADC_MONITOR_MUX_DVDD,              /*!< Mux the HW_ADC to DVDD */
    ADI_SSC_HW_ADC_MONITOR_MUX_AVDD,              /*!< Mux the HW_ADC to AVDD */
    ADI_SSC_HW_ADC_MONITOR_MUX_TEMPERATURE_SENSOR /*!< Mux the HW_ADC to Temperature sensor */
} ADI_SSC_HW_ADC_MUX;

/*! Enumeration of different SQI settle ranges */
typedef enum ADI_SQI_SETTLE_RANGE
{
    ADI_SQI_SETTLE_RANGE_16K = 0u,          /*!< 16K packets needs to be received to set settled flag */
    ADI_SQI_SETTLE_RANGE_60K = 1u,          /*!< 60K packets needs to be received to set settled flag */
    ADI_SQI_SETTLE_RANGE_80K = 2u,          /*!< 80K packets needs to be received to set settled flag */
    ADI_SQI_SETTLE_RANGE_100K = 3u,         /*!< 100K packets needs to be received to set settled flag */
} ADI_SQI_SETTLE_RANGE;

/*! Enum for SQI levels */
typedef enum ADI_SQI_LVL
{
    ADI_SQI_LVL_BAD = 0U,         /*!<  SQI level is bad */
    ADI_SQI_LVL_WARNING = 2U,     /*!<  SQI level is warning */
    ADI_SQI_LVL_GOOD = 4U         /*!<  SQI level is good */
} ADI_SQI_LVL;

/*! MAC status data */
typedef struct ADI_MAC_STATUS_DATA
{
    /* TX frame stat counters */
    uint32_t nTxFrmCnt;                 /*!< TX Frame count */
    uint32_t nTxBroadcastFrmCnt;        /*!< TX Broadcast Frame count */
    uint32_t nTxMulticastFrmCnt;        /*!< TX Multicast Frame count */
    uint32_t nTxUnicastFrmCnt;          /*!< TX Unicast Frame count */

    /* TX frame error counters */
    uint32_t nTxSingleColCnt;           /*!< TX Single collision count */
    uint32_t nTxMultipleColCnt;         /*!< TX Multiple collision count */
    uint32_t nTxDeferredXmitCnt;        /*!< TX deferred transmission count */
    uint32_t nTxLateColCnt;             /*!< TX Late collision count */
    uint32_t nTxExcessColCnt;           /*!< TX excess collision count */
    uint32_t nTxUnrCnt;                 /*!< TX Frames That Were Dropped due to Under Run count */

    /* RX frame stat counters */
    uint32_t nRxFrmCnt;                 /*!< RX Frame count */
    uint32_t nRxBroadcastFrmCnt;        /*!< RX Broadcast Frame count */
    uint32_t nRxMulticastFrmCnt;        /*!< RX Multicast Frame count */
    uint32_t nRxUnicastFrmCnt;          /*!< RX Unicast Frame count */

    /* RX frame error counters */
    uint32_t nRxShortErrCnt;            /*!< RX short frame error count */
    uint32_t nRxCRCErrFrmCnt;           /*!< RX CRC Errored Frame count */
    uint32_t nRxAlignErrFrmCnt;         /*!< RX Align Error count */
    uint32_t nRxLongErrFrmCnt;          /*!< RX Long Frame Error count */
    uint32_t nRxPhyErrFrmCnt;           /*!< RX PHY Error count */
    uint32_t nRxDropHostFullCnt;        /*!< RX Frames Dropped Due to Host FIFO full count */
    uint32_t nRxDropPortFullCnt;        /*!< RX Frames Dropped Due to Port FIFO full count */
    uint32_t nRxDropFilterTableCnt;     /*!< RX Frames Dropped Due to filtering count */
    uint32_t nRxInterFrameGapErrCnt;    /*!< RX IFG Error Counter for Port 1 count */
    uint32_t nRxPreambleErrCnt;         /*!< RX Preamble Error Count */
} ADI_MAC_STATUS_DATA;

/*! PHY Status data */
typedef struct ADI_PHY_STATUS_DATA
{
    bool bSpeed10m;                     /*!< Operating speed of 10Mbps in Half Dulpex Mode */
    bool bUniDirAbility;                /*!< Unidirectional Ability */
    bool bPreambleSuppr;                /*!< Preamble Suppresion */
    bool bLinkNegComplete;              /*!< Link Negotiation Complete */
    bool bRemoteJabber;                 /*!< Remote Fault Condition Detected */
    bool bAutoNegAbility;               /*!< Auto Negotiation Ability */
    bool bLinkStatus;                   /*!< Link Status */
    bool bLocalJabber;                  /*!< Local Jabber Event Detected */
    bool bExtendedCapab;                /*!< Extended Capabilities */
} ADI_PHY_STATUS_DATA;

/*! PLCA status data */
typedef struct ADI_PLCA_STATUS_DATA
{
    bool    bPlcaStatus;                /*!< PLCA Status */
    bool    bBeaconRcvdBeforeTO;        /*!< Beacon received before node's transmit opportunity */
    bool    bUnexpBeaconRcvd;           /*!< Unexpected Beacon Received */
    bool    bRxInTO;                    /*!< Reception occurred in local node's transmit opportunity */
    uint8_t nPlcaNodeId;                /*!< PLCA Node ID */
    uint8_t nPlcaNodeCnt;               /*!< PLCA Node Count */
    uint8_t nPlcaTOTimer;               /*!< PLCA Transmit Opportunity Timer */
    uint8_t nPlcaBurstTimer;            /*!< PLCA Burst Timer */
    uint8_t nPlcaMaxBurstCnt;           /*!< PLCA Maximum Burst Count */
} ADI_PLCA_STATUS_DATA;

/*! PCS status data */
typedef struct ADI_PCS_STATUS_DATA
{
    bool     bJabDetected;              /*!< Remote or Local Jabber Detected */
    uint16_t nRemoteJabCnt;             /*!< Remote Jabber Error Count */
    uint16_t nCorruptedTxCnt;           /*!< Physical Collision Detected Count */
} ADI_PCS_STATUS_DATA;

/*! PTP status data */
typedef struct ADI_PTP_STATUS_DATA
{
    bool     bLocked;                   /*!< PTP locked status */
    uint32_t nSyncCnt;                  /*!< Sync count */
    uint32_t nFollowUpCnt;              /*!< Follow-up count */
    uint16_t nNumLocks;                 /*!< Number of locks count */
    uint16_t nNumUnlocks;               /*!< Number of unlocks count */
    uint16_t nStatusChangeSeqId;        /*!< Sequence ID at which the gPTP status changed */
    int32_t  nGptpTimeErrNs;            /*!< gPTP current time error in nanoseconds */
} ADI_PTP_STATUS_DATA;

/*! Reason for wake / sleep data */
typedef struct ADI_REASON_FOR_WKSLP_DATA
{
    ADI_REASON_FOR_SLEEP    eReasonForSleep;    /*!< Reason for sleep */
    ADI_REASON_FOR_WAKE     eReasonForWake;     /*!< Reason for wake */
} ADI_REASON_FOR_WKSLP_DATA;

/*! Timestamp structure data */
typedef struct ADI_TS_DATA
{
    uint32_t nSec;          /*!< Seconds */
    uint32_t nNanoSec;      /*!< Nanoseconds */
} ADI_TS_DATA;

/*! SSC status data */
typedef struct ADI_SSC_STATUS_DATA
{
    bool bTriggered_SAIF[12];
    bool bTriggered_HW_ADC[4];
    bool bTriggered_SW_ADC[4];
    bool bTriggered_GPTP_UNLOCKED;
    bool bTriggered_OTP_CONFIG;
    bool bTriggered_SQI_LVL;
    bool bTriggered_MANUAL;
    bool bTriggered_Eth_frame_Check_Timeout;
    bool bTriggered_Device_Unattended_Timeout;
    bool bSafeMode;
    uint32_t nStatusOfSSC;
} ADI_SSC_STATUS_DATA;

/*! Structure to configure for SQI */
typedef struct ADI_SQI_CFG_DATA
{
    uint16_t nGoodToWarningLvl;       /*!< Threshold for the combined metric (in millivolts), to mark crossing from good state to warning state */
    uint16_t nWarningToGoodLvl;       /*!< Threshold for the combined metric (in millivolts), to mark crossing from warning state to good state */
    uint16_t nWarningToBadLvl;        /*!< Threshold for the combined metric (in millivolts), to mark crossing from warning state to bad state */
    uint16_t nBadToWarningLvl;        /*!< Threshold for the combined metric (in millivolts), to mark crossing from bad state to warning state */
    uint8_t nSqiSelPlcaId;            /*!< Selects the transmitting node for which the sqi has determined */
    /*!< Selects the number of packets that need to be received to set settled flag */
    ADI_SQI_SETTLE_RANGE    eSqiSettleRange;
} ADI_SQI_CFG_DATA;

/*! SQI Status Data */
typedef struct ADI_SQI_STATUS
{
    ADI_SQI_LVL eSqiLvl;      /*!< SQI level */
    bool bSqiSettleFlag;      /*!< SQI value settle flag */
    bool bSqiValidFlag;       /*!< SQI value valid flag */
} ADI_SQI_STATUS;

/*! @brief VLAN configuration structure */
typedef struct ADI_VLAN_CFG
{
    uint16_t    nTPID;                  /*!< Tag protocol identifier */
    uint8_t     nPCP;                   /*!< Priority code point */
    bool        bDEI;                   /*!< Drop eligible indicator */
    uint16_t    nVlanId;                /*!< VLAN identifier */
} ADI_VLAN_CFG;

/*================================== APIs ================================*/
extern void adi_common_logMacStatus(ADI_MAC_STATUS_DATA *poStatusDat);
extern void adi_common_logPhyStatus(ADI_PHY_STATUS_DATA *poStatusDat);
extern void adi_common_logPlcaStatus(ADI_PLCA_STATUS_DATA *poStatusDat);
extern void adi_common_logPcsStatus(ADI_PCS_STATUS_DATA *poStatusDat);
extern void adi_common_logPtpStatus(ADI_PTP_STATUS_DATA *poStatusDat);
extern void adi_common_logSscStatus(ADI_SSC_STATUS_DATA *poStatusData);
extern void adi_common_extractMacStatus(uint32_t *panData, ADI_MAC_STATUS_DATA *poStatusData);
extern void adi_common_extractPlcaStatus(uint32_t *panData, ADI_PLCA_STATUS_DATA *poStatusData);
extern void adi_common_extractPhyStatus(uint32_t *panData, ADI_PHY_STATUS_DATA *poStatusData);
extern void adi_common_extractPcsStatus(uint32_t *panData, ADI_PCS_STATUS_DATA *poStatusData);
extern void adi_common_extractPtpStatus(uint32_t *panData, ADI_PTP_STATUS_DATA *poStatusData);
extern void adi_common_extractSscStatus(uint32_t *panData, ADI_SSC_STATUS_DATA *poStatusData);
#endif /* ADI_COMMON_TYPES_H */

/** @} */

/*
 * EOF: www.analog.com
 */
