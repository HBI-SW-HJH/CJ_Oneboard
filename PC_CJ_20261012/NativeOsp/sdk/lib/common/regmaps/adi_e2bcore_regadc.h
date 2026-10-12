/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_regadc.h
 * @brief        : This is an auto generated header file with register
                   descriptions for ADC interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGADC_H
#define ADI_E2BCORE_REGADC_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define ADC_NUM_REGS    (17u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganADCRegAddr[ADC_NUM_REGS];
extern uint32_t ganADCNwRegAddr[ADC_NUM_REGS];
extern uint32_t ganADCRegMMS[ADC_NUM_REGS];
extern uint32_t ganADCRegResetVal[ADC_NUM_REGS];
extern const char *gasADCRegName[ADC_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00000001u
// Register MMS         : 0u

#define ADC_CONTROL_ADDR                                                        (0x0000u)
#define ADC_CONTROL_NW_ADDR                                                     (0x0000u)
#define ADC_CONTROL_RESET_VAL                                               (0x00000001u)
#define ADC_CONTROL_MMS                                                              (0u)
#define ADC_CONTROL_VOL_MASK                                                (0xFC10FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | enable_upstream_ecco     | If disabled, the interface will   | 0x00000001  |
// |       |                          | not be capable of generating an   |             |
// |       |                          | ecco for each of the processed    |             |
// |       |                          | commands. Regardless of any       |             |
// |       |                          | other condition.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | enable_read_time_tag_ecco| If Enabled, the Interface will    | 0x00000000  | 
// |       |                          | tag the ecco commands with the    |             |
// |       |                          | current gPTP time-stamp.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-02 | enable_read_time_tag     | If Enabled, the Interface will    | 0x00000000  |
// |       |                          | tag the I2C transaction           |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | resynchronization_require| Executes the resynchronization    | 0x00000000  | 
// |       |                          | procedure. Please, check the      |             |
// |       |                          | user guide of the interface for   |             |
// |       |                          | more information about this       |             |
// |       |                          | procedure.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 26    | tx_fifo_reset            | Transmit FIFO Reset.              | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27    | rx_fifo_reset            | Receive FIFO Reset.               | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | soft_reset               | Reset key. Must write a value of  | 0x00000000  |
// |       |                          | 0xA to this field to cause a      |             |
// |       |                          | soft reset of the interface.      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: enable_upstream_ecco
// Bit position, length and mask
#define BITP_ADC_CONTROL_ENABLE_UPSTREAM_ECCO                                        (0u)
#define BITL_ADC_CONTROL_ENABLE_UPSTREAM_ECCO                                        (1u)
#define BITM_ADC_CONTROL_ENABLE_UPSTREAM_ECCO                               (0x00000001u)
// Enums
#define ENUM_ADC_CONTROL_ENABLE_UPSTREAM_ECCO_DISABLE                       (0x00000000u)
#define ENUM_ADC_CONTROL_ENABLE_UPSTREAM_ECCO_ENABLE                        (0x00000001u)

// Bit field: enable_read_time_tag_ecco
// Bit position, length and mask
#define BITP_ADC_CONTROL_ENABLE_READ_TIME_TAG_ECCO                                   (1u)
#define BITL_ADC_CONTROL_ENABLE_READ_TIME_TAG_ECCO                                   (1u)
#define BITM_ADC_CONTROL_ENABLE_READ_TIME_TAG_ECCO                          (0x00000002u)
// Enums
#define ENUM_ADC_CONTROL_ENABLE_READ_TIME_TAG_ECCO_DISABLE                  (0x00000000u)
#define ENUM_ADC_CONTROL_ENABLE_READ_TIME_TAG_ECCO_ENABLE                   (0x00000001u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_ADC_CONTROL_ENABLE_READ_TIME_TAG                                        (2u)
#define BITL_ADC_CONTROL_ENABLE_READ_TIME_TAG                                       (14u)
#define BITM_ADC_CONTROL_ENABLE_READ_TIME_TAG                               (0x0000FFFCu)

// Bit field: resynchronization_required
// Bit position, length and mask
#define BITP_ADC_CONTROL_RESYNCHRONIZATION_REQUIRED                                 (20u)
#define BITL_ADC_CONTROL_RESYNCHRONIZATION_REQUIRED                                  (1u)
#define BITM_ADC_CONTROL_RESYNCHRONIZATION_REQUIRED                         (0x00100000u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_ADC_CONTROL_TX_FIFO_RESET                                              (26u)
#define BITL_ADC_CONTROL_TX_FIFO_RESET                                               (1u)
#define BITM_ADC_CONTROL_TX_FIFO_RESET                                      (0x04000000u)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_ADC_CONTROL_RX_FIFO_RESET                                              (27u)
#define BITL_ADC_CONTROL_RX_FIFO_RESET                                               (1u)
#define BITM_ADC_CONTROL_RX_FIFO_RESET                                      (0x08000000u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_ADC_CONTROL_SOFT_RESET                                                 (28u)
#define BITL_ADC_CONTROL_SOFT_RESET                                                  (4u)
#define BITM_ADC_CONTROL_SOFT_RESET                                         (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control_1
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x00851A0Du
// Register MMS         : 0u

#define ADC_CONTROL_1_ADDR                                                      (0x0004u)
#define ADC_CONTROL_1_NW_ADDR                                                   (0x0004u)
#define ADC_CONTROL_1_RESET_VAL                                             (0x00851A0Du)
#define ADC_CONTROL_1_MMS                                                            (0u)
#define ADC_CONTROL_1_VOL_MASK                                              (0x00BF3F3Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 05-00 | vmtr_ctl_tamp            | ADC Amplification time in 40ns    | 0x0000000d  |
// |       |                          | Increments                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-08 | vmtr_ctl_tsampl          | ADC Sampling Time in 40ns         | 0x0000001a  |
// |       |                          | increments                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21-16 | vmtr_ctl_tconv           | ADC Conversion time in 40ns       | 0x00000005  |
// |       |                          | increments                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | vmtr_en                  | ADC Enable                        | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_ctl_tamp
// Bit position, length and mask
#define BITP_ADC_CONTROL_1_VMTR_CTL_TAMP                                             (0u)
#define BITL_ADC_CONTROL_1_VMTR_CTL_TAMP                                             (6u)
#define BITM_ADC_CONTROL_1_VMTR_CTL_TAMP                                    (0x0000003Fu)

// Bit field: vmtr_ctl_tsampl
// Bit position, length and mask
#define BITP_ADC_CONTROL_1_VMTR_CTL_TSAMPL                                           (8u)
#define BITL_ADC_CONTROL_1_VMTR_CTL_TSAMPL                                           (6u)
#define BITM_ADC_CONTROL_1_VMTR_CTL_TSAMPL                                  (0x00003F00u)

// Bit field: vmtr_ctl_tconv
// Bit position, length and mask
#define BITP_ADC_CONTROL_1_VMTR_CTL_TCONV                                           (16u)
#define BITL_ADC_CONTROL_1_VMTR_CTL_TCONV                                            (6u)
#define BITM_ADC_CONTROL_1_VMTR_CTL_TCONV                                   (0x003F0000u)

// Bit field: vmtr_en
// Bit position, length and mask
#define BITP_ADC_CONTROL_1_VMTR_EN                                                  (23u)
#define BITL_ADC_CONTROL_1_VMTR_EN                                                   (1u)
#define BITM_ADC_CONTROL_1_VMTR_EN                                          (0x00800000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : stats
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define ADC_STATS_ADDR                                                          (0x0008u)
#define ADC_STATS_NW_ADDR                                                       (0x0008u)
#define ADC_STATS_RESET_VAL                                                 (0x00000000u)
#define ADC_STATS_MMS                                                                (0u)
#define ADC_STATS_VOL_MASK                                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_rd_samples           | Count of read octets from the     | 0x00000000  |
// |       |                          | interface SDATA pin.              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_rd_samples
// Bit position, length and mask
#define BITP_ADC_STATS_NUM_RD_SAMPLES                                                (0u)
#define BITL_ADC_STATS_NUM_RD_SAMPLES                                               (16u)
#define BITM_ADC_STATS_NUM_RD_SAMPLES                                       (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control_2
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x00001070u
// Register MMS         : 0u

#define ADC_CONTROL_2_ADDR                                                      (0x000Cu)
#define ADC_CONTROL_2_NW_ADDR                                                   (0x000Cu)
#define ADC_CONTROL_2_RESET_VAL                                             (0x00001070u)
#define ADC_CONTROL_2_MMS                                                            (0u)
#define ADC_CONTROL_2_VOL_MASK                                              (0xF01F1CFBu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | adc_start                | ADC Start                         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | cal_en                   | Calibration Enable                | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | periodic_enable          | Periodic Sampling Mode Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | round_robin_enable       | Round Robin Mode Enable           | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | command_mode             | Command Mode                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | adc_stop                 | ADC Stop                          | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | transaction_header_enable| Transaction Header Enable         | 0x00000001  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-16 | external_trigger_sel     | External Trigger Select -         | 0x00000000  |
// |       |                          | Selects Which SA_IF Pin is Used   |             |
// |       |                          | to Trigger The ADC Start          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | external_trigger_en      | External Trigger Enable           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | adc_num_samples          | Number Of Samples Per ADC         | 0x00000000  |
// |       |                          | Channel                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: adc_start
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_ADC_START                                                 (0u)
#define BITL_ADC_CONTROL_2_ADC_START                                                 (1u)
#define BITM_ADC_CONTROL_2_ADC_START                                        (0x00000001u)

// Bit field: cal_en
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_CAL_EN                                                    (1u)
#define BITL_ADC_CONTROL_2_CAL_EN                                                    (1u)
#define BITM_ADC_CONTROL_2_CAL_EN                                           (0x00000002u)

// Bit field: periodic_enable
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_PERIODIC_ENABLE                                           (3u)
#define BITL_ADC_CONTROL_2_PERIODIC_ENABLE                                           (1u)
#define BITM_ADC_CONTROL_2_PERIODIC_ENABLE                                  (0x00000008u)

// Bit field: round_robin_enable
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_ROUND_ROBIN_ENABLE                                        (4u)
#define BITL_ADC_CONTROL_2_ROUND_ROBIN_ENABLE                                        (1u)
#define BITM_ADC_CONTROL_2_ROUND_ROBIN_ENABLE                               (0x00000010u)

// Bit field: command_mode
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_COMMAND_MODE                                             (10u)
#define BITL_ADC_CONTROL_2_COMMAND_MODE                                              (1u)
#define BITM_ADC_CONTROL_2_COMMAND_MODE                                     (0x00000400u)
// Enums
#define ENUM_ADC_CONTROL_2_COMMAND_MODE_ENUM000                             (0x00000000u)
#define ENUM_ADC_CONTROL_2_COMMAND_MODE_ENUM001                             (0x00000001u)

// Bit field: adc_stop
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_ADC_STOP                                                 (11u)
#define BITL_ADC_CONTROL_2_ADC_STOP                                                  (1u)
#define BITM_ADC_CONTROL_2_ADC_STOP                                         (0x00000800u)

// Bit field: transaction_header_enable
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_TRANSACTION_HEADER_ENABLE                                (12u)
#define BITL_ADC_CONTROL_2_TRANSACTION_HEADER_ENABLE                                 (1u)
#define BITM_ADC_CONTROL_2_TRANSACTION_HEADER_ENABLE                        (0x00001000u)

// Bit field: external_trigger_sel
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_EXTERNAL_TRIGGER_SEL                                     (16u)
#define BITL_ADC_CONTROL_2_EXTERNAL_TRIGGER_SEL                                      (4u)
#define BITM_ADC_CONTROL_2_EXTERNAL_TRIGGER_SEL                             (0x000F0000u)

// Bit field: external_trigger_en
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_EXTERNAL_TRIGGER_EN                                      (20u)
#define BITL_ADC_CONTROL_2_EXTERNAL_TRIGGER_EN                                       (1u)
#define BITM_ADC_CONTROL_2_EXTERNAL_TRIGGER_EN                              (0x00100000u)

// Bit field: adc_num_samples
// Bit position, length and mask
#define BITP_ADC_CONTROL_2_ADC_NUM_SAMPLES                                          (28u)
#define BITL_ADC_CONTROL_2_ADC_NUM_SAMPLES                                           (4u)
#define BITM_ADC_CONTROL_2_ADC_NUM_SAMPLES                                  (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control_3
// Register Addr        : 0x0010u
// Register NW Addr     : 0x0010u
// Register Reset Value : 0xC000FFFFu
// Register MMS         : 0u

#define ADC_CONTROL_3_ADDR                                                      (0x0010u)
#define ADC_CONTROL_3_NW_ADDR                                                   (0x0010u)
#define ADC_CONTROL_3_RESET_VAL                                             (0xC000FFFFu)
#define ADC_CONTROL_3_MMS                                                            (0u)
#define ADC_CONTROL_3_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | channel_enable           | Channel Enable - Controls Which   | 0x0000ffff  |
// |       |                          | Channels Are Enabled For          |             |
// |       |                          | Sampling in Round Robin Sampling  |             |
// |       |                          | Mode                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | channel_mask             | Channel Mask - Controls Which     | 0x0000c000  |
// |       |                          | Channels Are Masked From The      |             |
// |       |                          | Upstream Data Packets             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: channel_enable
// Bit position, length and mask
#define BITP_ADC_CONTROL_3_CHANNEL_ENABLE                                            (0u)
#define BITL_ADC_CONTROL_3_CHANNEL_ENABLE                                           (16u)
#define BITM_ADC_CONTROL_3_CHANNEL_ENABLE                                   (0x0000FFFFu)

// Bit field: channel_mask
// Bit position, length and mask
#define BITP_ADC_CONTROL_3_CHANNEL_MASK                                             (16u)
#define BITL_ADC_CONTROL_3_CHANNEL_MASK                                             (16u)
#define BITM_ADC_CONTROL_3_CHANNEL_MASK                                     (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_0
// Register Addr        : 0x0018u
// Register NW Addr     : 0x0018u
// Register Reset Value : 0xC0C0C0C0u
// Register MMS         : 0u

#define ADC_THRESHOLDS_0_ADDR                                                   (0x0018u)
#define ADC_THRESHOLDS_0_NW_ADDR                                                (0x0018u)
#define ADC_THRESHOLDS_0_RESET_VAL                                          (0xC0C0C0C0u)
#define ADC_THRESHOLDS_0_MMS                                                         (0u)
#define ADC_THRESHOLDS_0_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | vmtr_gt_thresh_ch0       | ADC Greater Than Threshold Ch0    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | vmtr_gt_thresh_ch1       | ADC Greater Than Threshold Ch1    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | vmtr_gt_thresh_ch2       | ADC Greater Than Threshold Ch2    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-24 | vmtr_gt_thresh_ch3       | ADC Greater Than Threshold Ch3    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_gt_thresh_ch0
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH0                                     (0u)
#define BITL_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH0                                     (8u)
#define BITM_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH0                            (0x000000FFu)

// Bit field: vmtr_gt_thresh_ch1
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH1                                     (8u)
#define BITL_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH1                                     (8u)
#define BITM_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH1                            (0x0000FF00u)

// Bit field: vmtr_gt_thresh_ch2
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH2                                    (16u)
#define BITL_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH2                                     (8u)
#define BITM_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH2                            (0x00FF0000u)

// Bit field: vmtr_gt_thresh_ch3
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH3                                    (24u)
#define BITL_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH3                                     (8u)
#define BITM_ADC_THRESHOLDS_0_VMTR_GT_THRESH_CH3                            (0xFF000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_1
// Register Addr        : 0x001Cu
// Register NW Addr     : 0x001Cu
// Register Reset Value : 0xC0C0C0C0u
// Register MMS         : 0u

#define ADC_THRESHOLDS_1_ADDR                                                   (0x001Cu)
#define ADC_THRESHOLDS_1_NW_ADDR                                                (0x001Cu)
#define ADC_THRESHOLDS_1_RESET_VAL                                          (0xC0C0C0C0u)
#define ADC_THRESHOLDS_1_MMS                                                         (0u)
#define ADC_THRESHOLDS_1_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | vmtr_gt_thresh_ch4       | ADC Greater Than Threshold Ch4    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | vmtr_gt_thresh_ch5       | ADC Greater Than Threshold Ch5    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | vmtr_gt_thresh_ch6       | ADC Greater Than Threshold Ch6    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-24 | vmtr_gt_thresh_ch7       | ADC Greater Than Threshold Ch7    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_gt_thresh_ch4
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH4                                     (0u)
#define BITL_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH4                                     (8u)
#define BITM_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH4                            (0x000000FFu)

// Bit field: vmtr_gt_thresh_ch5
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH5                                     (8u)
#define BITL_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH5                                     (8u)
#define BITM_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH5                            (0x0000FF00u)

// Bit field: vmtr_gt_thresh_ch6
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH6                                    (16u)
#define BITL_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH6                                     (8u)
#define BITM_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH6                            (0x00FF0000u)

// Bit field: vmtr_gt_thresh_ch7
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH7                                    (24u)
#define BITL_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH7                                     (8u)
#define BITM_ADC_THRESHOLDS_1_VMTR_GT_THRESH_CH7                            (0xFF000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_2
// Register Addr        : 0x0020u
// Register NW Addr     : 0x0020u
// Register Reset Value : 0xC0C0C0C0u
// Register MMS         : 0u

#define ADC_THRESHOLDS_2_ADDR                                                   (0x0020u)
#define ADC_THRESHOLDS_2_NW_ADDR                                                (0x0020u)
#define ADC_THRESHOLDS_2_RESET_VAL                                          (0xC0C0C0C0u)
#define ADC_THRESHOLDS_2_MMS                                                         (0u)
#define ADC_THRESHOLDS_2_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | vmtr_gt_thresh_ch8       | ADC Greater Than Threshold Ch8    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | vmtr_gt_thresh_ch9       | ADC Greater Than Threshold Ch9    | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | vmtr_gt_thresh_ch10      | ADC Greater Than Threshold Ch10   | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-24 | vmtr_gt_thresh_ch11      | ADC Greater Than Threshold Ch11   | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_gt_thresh_ch8
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH8                                     (0u)
#define BITL_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH8                                     (8u)
#define BITM_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH8                            (0x000000FFu)

// Bit field: vmtr_gt_thresh_ch9
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH9                                     (8u)
#define BITL_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH9                                     (8u)
#define BITM_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH9                            (0x0000FF00u)

// Bit field: vmtr_gt_thresh_ch10
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH10                                   (16u)
#define BITL_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH10                                    (8u)
#define BITM_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH10                           (0x00FF0000u)

// Bit field: vmtr_gt_thresh_ch11
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH11                                   (24u)
#define BITL_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH11                                    (8u)
#define BITM_ADC_THRESHOLDS_2_VMTR_GT_THRESH_CH11                           (0xFF000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_3
// Register Addr        : 0x0024u
// Register NW Addr     : 0x0024u
// Register Reset Value : 0x4040C0C0u
// Register MMS         : 0u

#define ADC_THRESHOLDS_3_ADDR                                                   (0x0024u)
#define ADC_THRESHOLDS_3_NW_ADDR                                                (0x0024u)
#define ADC_THRESHOLDS_3_RESET_VAL                                          (0x4040C0C0u)
#define ADC_THRESHOLDS_3_MMS                                                         (0u)
#define ADC_THRESHOLDS_3_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | vmtr_gt_thresh_ch12      | ADC Greater Than Threshold Ch12   | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | vmtr_gt_thresh_ch13      | ADC Greater Than Threshold Ch13   | 0x000000c0  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | vmtr_lt_thresh_ch0       | ADC Less Than Threshold Ch0       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-24 | vmtr_lt_thresh_ch1       | ADC Less Than Threshold Ch1       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_gt_thresh_ch12
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_3_VMTR_GT_THRESH_CH12                                    (0u)
#define BITL_ADC_THRESHOLDS_3_VMTR_GT_THRESH_CH12                                    (8u)
#define BITM_ADC_THRESHOLDS_3_VMTR_GT_THRESH_CH12                           (0x000000FFu)

// Bit field: vmtr_gt_thresh_ch13
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_3_VMTR_GT_THRESH_CH13                                    (8u)
#define BITL_ADC_THRESHOLDS_3_VMTR_GT_THRESH_CH13                                    (8u)
#define BITM_ADC_THRESHOLDS_3_VMTR_GT_THRESH_CH13                           (0x0000FF00u)

// Bit field: vmtr_lt_thresh_ch0
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_3_VMTR_LT_THRESH_CH0                                    (16u)
#define BITL_ADC_THRESHOLDS_3_VMTR_LT_THRESH_CH0                                     (8u)
#define BITM_ADC_THRESHOLDS_3_VMTR_LT_THRESH_CH0                            (0x00FF0000u)

// Bit field: vmtr_lt_thresh_ch1
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_3_VMTR_LT_THRESH_CH1                                    (24u)
#define BITL_ADC_THRESHOLDS_3_VMTR_LT_THRESH_CH1                                     (8u)
#define BITM_ADC_THRESHOLDS_3_VMTR_LT_THRESH_CH1                            (0xFF000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_4
// Register Addr        : 0x0028u
// Register NW Addr     : 0x0028u
// Register Reset Value : 0x40404040u
// Register MMS         : 0u

#define ADC_THRESHOLDS_4_ADDR                                                   (0x0028u)
#define ADC_THRESHOLDS_4_NW_ADDR                                                (0x0028u)
#define ADC_THRESHOLDS_4_RESET_VAL                                          (0x40404040u)
#define ADC_THRESHOLDS_4_MMS                                                         (0u)
#define ADC_THRESHOLDS_4_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | vmtr_lt_thresh_ch2       | ADC Less Than Threshold Ch2       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | vmtr_lt_thresh_ch3       | ADC Less Than Threshold Ch3       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | vmtr_lt_thresh_ch4       | ADC Less Than Threshold Ch4       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-24 | vmtr_lt_thresh_ch5       | ADC Less Than Threshold Ch5       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_lt_thresh_ch2
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH2                                     (0u)
#define BITL_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH2                                     (8u)
#define BITM_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH2                            (0x000000FFu)

// Bit field: vmtr_lt_thresh_ch3
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH3                                     (8u)
#define BITL_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH3                                     (8u)
#define BITM_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH3                            (0x0000FF00u)

// Bit field: vmtr_lt_thresh_ch4
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH4                                    (16u)
#define BITL_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH4                                     (8u)
#define BITM_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH4                            (0x00FF0000u)

// Bit field: vmtr_lt_thresh_ch5
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH5                                    (24u)
#define BITL_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH5                                     (8u)
#define BITM_ADC_THRESHOLDS_4_VMTR_LT_THRESH_CH5                            (0xFF000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_5
// Register Addr        : 0x002Cu
// Register NW Addr     : 0x002Cu
// Register Reset Value : 0x40404040u
// Register MMS         : 0u

#define ADC_THRESHOLDS_5_ADDR                                                   (0x002Cu)
#define ADC_THRESHOLDS_5_NW_ADDR                                                (0x002Cu)
#define ADC_THRESHOLDS_5_RESET_VAL                                          (0x40404040u)
#define ADC_THRESHOLDS_5_MMS                                                         (0u)
#define ADC_THRESHOLDS_5_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | vmtr_lt_thresh_ch6       | ADC Less Than Threshold Ch6       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | vmtr_lt_thresh_ch7       | ADC Less Than Threshold Ch7       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | vmtr_lt_thresh_ch8       | ADC Less Than Threshold Ch8       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-24 | vmtr_lt_thresh_ch9       | ADC Less Than Threshold Ch9       | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_lt_thresh_ch6
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH6                                     (0u)
#define BITL_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH6                                     (8u)
#define BITM_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH6                            (0x000000FFu)

// Bit field: vmtr_lt_thresh_ch7
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH7                                     (8u)
#define BITL_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH7                                     (8u)
#define BITM_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH7                            (0x0000FF00u)

// Bit field: vmtr_lt_thresh_ch8
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH8                                    (16u)
#define BITL_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH8                                     (8u)
#define BITM_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH8                            (0x00FF0000u)

// Bit field: vmtr_lt_thresh_ch9
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH9                                    (24u)
#define BITL_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH9                                     (8u)
#define BITM_ADC_THRESHOLDS_5_VMTR_LT_THRESH_CH9                            (0xFF000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_6
// Register Addr        : 0x0030u
// Register NW Addr     : 0x0030u
// Register Reset Value : 0x40404040u
// Register MMS         : 0u

#define ADC_THRESHOLDS_6_ADDR                                                   (0x0030u)
#define ADC_THRESHOLDS_6_NW_ADDR                                                (0x0030u)
#define ADC_THRESHOLDS_6_RESET_VAL                                          (0x40404040u)
#define ADC_THRESHOLDS_6_MMS                                                         (0u)
#define ADC_THRESHOLDS_6_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | vmtr_lt_thresh_ch10      | ADC Less Than Threshold Ch10      | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | vmtr_lt_thresh_ch11      | ADC Less Than Threshold Ch11      | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | vmtr_lt_thresh_ch12      | ADC Less Than Threshold Ch12      | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-24 | vmtr_lt_thresh_ch13      | ADC Less Than Threshold Ch13      | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_lt_thresh_ch10
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH10                                    (0u)
#define BITL_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH10                                    (8u)
#define BITM_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH10                           (0x000000FFu)

// Bit field: vmtr_lt_thresh_ch11
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH11                                    (8u)
#define BITL_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH11                                    (8u)
#define BITM_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH11                           (0x0000FF00u)

// Bit field: vmtr_lt_thresh_ch12
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH12                                   (16u)
#define BITL_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH12                                    (8u)
#define BITM_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH12                           (0x00FF0000u)

// Bit field: vmtr_lt_thresh_ch13
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH13                                   (24u)
#define BITL_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH13                                    (8u)
#define BITM_ADC_THRESHOLDS_6_VMTR_LT_THRESH_CH13                           (0xFF000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_7
// Register Addr        : 0x0034u
// Register NW Addr     : 0x0034u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define ADC_THRESHOLDS_7_ADDR                                                   (0x0034u)
#define ADC_THRESHOLDS_7_NW_ADDR                                                (0x0034u)
#define ADC_THRESHOLDS_7_RESET_VAL                                          (0x00000000u)
#define ADC_THRESHOLDS_7_MMS                                                         (0u)
#define ADC_THRESHOLDS_7_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | vmtr_gt_thresh_result    | ADC Greater Than Result           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-16 | vmtr_lt_thresh_result    | ADC Less Than Result              | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_gt_thresh_result
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_7_VMTR_GT_THRESH_RESULT                                  (0u)
#define BITL_ADC_THRESHOLDS_7_VMTR_GT_THRESH_RESULT                                 (14u)
#define BITM_ADC_THRESHOLDS_7_VMTR_GT_THRESH_RESULT                         (0x00003FFFu)

// Bit field: vmtr_lt_thresh_result
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_7_VMTR_LT_THRESH_RESULT                                 (16u)
#define BITL_ADC_THRESHOLDS_7_VMTR_LT_THRESH_RESULT                                 (14u)
#define BITM_ADC_THRESHOLDS_7_VMTR_LT_THRESH_RESULT                         (0x3FFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds_8
// Register Addr        : 0x0038u
// Register NW Addr     : 0x0038u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define ADC_THRESHOLDS_8_ADDR                                                   (0x0038u)
#define ADC_THRESHOLDS_8_NW_ADDR                                                (0x0038u)
#define ADC_THRESHOLDS_8_RESET_VAL                                          (0x00000000u)
#define ADC_THRESHOLDS_8_MMS                                                         (0u)
#define ADC_THRESHOLDS_8_VOL_MASK                                           (0x3FFF3FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | vmtr_gt_thresh_clr       | ADC Greater Than Result Clear     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-16 | vmtr_lt_thresh_clr       | ADC Less Than Result Clear        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_gt_thresh_clr
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_8_VMTR_GT_THRESH_CLR                                     (0u)
#define BITL_ADC_THRESHOLDS_8_VMTR_GT_THRESH_CLR                                    (14u)
#define BITM_ADC_THRESHOLDS_8_VMTR_GT_THRESH_CLR                            (0x00003FFFu)

// Bit field: vmtr_lt_thresh_clr
// Bit position, length and mask
#define BITP_ADC_THRESHOLDS_8_VMTR_LT_THRESH_CLR                                    (16u)
#define BITL_ADC_THRESHOLDS_8_VMTR_LT_THRESH_CLR                                    (14u)
#define BITM_ADC_THRESHOLDS_8_VMTR_LT_THRESH_CLR                            (0x3FFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : clk_period
// Register Addr        : 0x003Cu
// Register NW Addr     : 0x003Cu
// Register Reset Value : 0x0000076Cu
// Register MMS         : 0u

#define ADC_CLK_PERIOD_ADDR                                                     (0x003Cu)
#define ADC_CLK_PERIOD_NW_ADDR                                                  (0x003Cu)
#define ADC_CLK_PERIOD_RESET_VAL                                            (0x0000076Cu)
#define ADC_CLK_PERIOD_MMS                                                           (0u)
#define ADC_CLK_PERIOD_VOL_MASK                                             (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | adc_clk_period           | ADC Clock Period in ns            | 0x0000076c  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: adc_clk_period
// Bit position, length and mask
#define BITP_ADC_CLK_PERIOD_ADC_CLK_PERIOD                                           (0u)
#define BITL_ADC_CLK_PERIOD_ADC_CLK_PERIOD                                          (32u)
#define BITM_ADC_CLK_PERIOD_ADC_CLK_PERIOD                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : cycle_period
// Register Addr        : 0x0040u
// Register NW Addr     : 0x0040u
// Register Reset Value : 0x0000C350u
// Register MMS         : 0u

#define ADC_CYCLE_PERIOD_ADDR                                                   (0x0040u)
#define ADC_CYCLE_PERIOD_NW_ADDR                                                (0x0040u)
#define ADC_CYCLE_PERIOD_RESET_VAL                                          (0x0000C350u)
#define ADC_CYCLE_PERIOD_MMS                                                         (0u)
#define ADC_CYCLE_PERIOD_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | adc_cycle_period         | ADC Cycle Period in ns            | 0x0000c350  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: adc_cycle_period
// Bit position, length and mask
#define BITP_ADC_CYCLE_PERIOD_ADC_CYCLE_PERIOD                                       (0u)
#define BITL_ADC_CYCLE_PERIOD_ADC_CYCLE_PERIOD                                      (32u)
#define BITM_ADC_CYCLE_PERIOD_ADC_CYCLE_PERIOD                              (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : adc_raw_data
// Register Addr        : 0x0044u
// Register NW Addr     : 0x0044u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define ADC_ADC_RAW_DATA_ADDR                                                   (0x0044u)
#define ADC_ADC_RAW_DATA_NW_ADDR                                                (0x0044u)
#define ADC_ADC_RAW_DATA_RESET_VAL                                          (0x00000000u)
#define ADC_ADC_RAW_DATA_MMS                                                         (0u)
#define ADC_ADC_RAW_DATA_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 12-00 | vmtr_raw_data            | ADC Raw Data                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | calib_msr0eq1            | The Measured VCAL0 Is Equal To    | 0x00000000  |
// |       |                          | VCAL1                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | calib_msr0gt1            | The Measured VCAL0 Is Greater     | 0x00000000  |
// |       |                          | Than VCAL1                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vmtr_raw_data
// Bit position, length and mask
#define BITP_ADC_ADC_RAW_DATA_VMTR_RAW_DATA                                          (0u)
#define BITL_ADC_ADC_RAW_DATA_VMTR_RAW_DATA                                         (13u)
#define BITM_ADC_ADC_RAW_DATA_VMTR_RAW_DATA                                 (0x00001FFFu)

// Bit field: calib_msr0eq1
// Bit position, length and mask
#define BITP_ADC_ADC_RAW_DATA_CALIB_MSR0EQ1                                         (16u)
#define BITL_ADC_ADC_RAW_DATA_CALIB_MSR0EQ1                                          (1u)
#define BITM_ADC_ADC_RAW_DATA_CALIB_MSR0EQ1                                 (0x00010000u)

// Bit field: calib_msr0gt1
// Bit position, length and mask
#define BITP_ADC_ADC_RAW_DATA_CALIB_MSR0GT1                                         (17u)
#define BITL_ADC_ADC_RAW_DATA_CALIB_MSR0GT1                                          (1u)
#define BITM_ADC_ADC_RAW_DATA_CALIB_MSR0GT1                                 (0x00020000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillADCRegsData();
#endif


#endif /* ADI_E2BCORE_REGADC_H */

/** @} */

/*
 * EOF: www.analog.com
 */


