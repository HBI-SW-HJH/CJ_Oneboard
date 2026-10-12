/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_regfifo.h
 * @brief        : This is an auto generated header file with register
                   descriptions for FIFO controller
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGFIFO_H
#define ADI_E2BCORE_REGFIFO_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define FIFO_NUM_REGS    (10u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganFIFORegAddr[FIFO_NUM_REGS];
extern uint32_t ganFIFONwRegAddr[FIFO_NUM_REGS];
extern uint32_t ganFIFORegMMS[FIFO_NUM_REGS];
extern uint32_t ganFIFORegResetVal[FIFO_NUM_REGS];
extern const char *gasFIFORegName[FIFO_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00008000u
// Register MMS         : 0u

#define FIFO_CONTROL_ADDR                                                       (0x0000u)
#define FIFO_CONTROL_NW_ADDR                                                    (0x0000u)
#define FIFO_CONTROL_RESET_VAL                                              (0x00008000u)
#define FIFO_CONTROL_MMS                                                             (0u)
#define FIFO_CONTROL_VOL_MASK                                               (0x0FFFF000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 16-15 | port_destination         | Enables the selection of either   | 0x00000001  |
// |       |                          | OASPI port, T1S port, or both,    |             |
// |       |                          | for transmitting data to the      |             |
// |       |                          | ECU.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | trigger_full_read        | When asserted, initiates          | 0x00000000  |
// |       |                          | continuous generation of          |             |
// |       |                          | messages until the data stream    |             |
// |       |                          | is depleted, with each message's  |             |
// |       |                          | maximum size limited by the       |             |
// |       |                          | read_block_size value             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22    | trigger_read             | When asserted, initiates data     | 0x00000000  |
// |       |                          | generation in the form of a       |             |
// |       |                          | message containing up to the      |             |
// |       |                          | specified number of bytes in the  |             |
// |       |                          | "read_block_size" field, but      |             |
// |       |                          | only if the data stream is not    |             |
// |       |                          | empty                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | loop                     | Sets the reception path to be     | 0x00000000  |
// |       |                          | internally shorted to the         |             |
// |       |                          | transmission path for debugging   |             |
// |       |                          | purposes.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24    | stats_reset              | Enables users to reset all        | 0x00000000  |
// |       |                          | bidirectional data stream         |             |
// |       |                          | statistics to zero.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-26 | read_mode                | Controls the data stream read     | 0x00000000  |
// |       |                          | configuration, allowing for       |             |
// |       |                          | manual or automatic control over  |             |
// |       |                          | data generation based on          |             |
// |       |                          | user-triggered actions or         |             |
// |       |                          | continuous message generation     |             |
// |       |                          | until data depletion              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | soft_reset               | Removes the stored data from the  | 0x00000000  |
// |       |                          | memory storage and restores the   |             |
// |       |                          | register map to its default       |             |
// |       |                          | state.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: port_destination
// Bit position, length and mask
#define BITP_FIFO_CONTROL_PORT_DESTINATION                                          (15u)
#define BITL_FIFO_CONTROL_PORT_DESTINATION                                           (2u)
#define BITM_FIFO_CONTROL_PORT_DESTINATION                                  (0x00018000u)
// Enums
#define ENUM_FIFO_CONTROL_PORT_DESTINATION_NONE                             (0x00000000u)
#define ENUM_FIFO_CONTROL_PORT_DESTINATION_T1S                              (0x00000001u)
#define ENUM_FIFO_CONTROL_PORT_DESTINATION_OASPI                            (0x00000002u)
#define ENUM_FIFO_CONTROL_PORT_DESTINATION_T1S_AND_OASPI                    (0x00000003u)

// Bit field: trigger_full_read
// Bit position, length and mask
#define BITP_FIFO_CONTROL_TRIGGER_FULL_READ                                         (21u)
#define BITL_FIFO_CONTROL_TRIGGER_FULL_READ                                          (1u)
#define BITM_FIFO_CONTROL_TRIGGER_FULL_READ                                 (0x00200000u)

// Bit field: trigger_read
// Bit position, length and mask
#define BITP_FIFO_CONTROL_TRIGGER_READ                                              (22u)
#define BITL_FIFO_CONTROL_TRIGGER_READ                                               (1u)
#define BITM_FIFO_CONTROL_TRIGGER_READ                                      (0x00400000u)

// Bit field: loop
// Bit position, length and mask
#define BITP_FIFO_CONTROL_LOOP                                                      (23u)
#define BITL_FIFO_CONTROL_LOOP                                                       (1u)
#define BITM_FIFO_CONTROL_LOOP                                              (0x00800000u)
// Enums
#define ENUM_FIFO_CONTROL_LOOP_ACTIVE                                       (0x00000000u)
#define ENUM_FIFO_CONTROL_LOOP_NOT_ACTIVE                                   (0x00000001u)

// Bit field: stats_reset
// Bit position, length and mask
#define BITP_FIFO_CONTROL_STATS_RESET                                               (24u)
#define BITL_FIFO_CONTROL_STATS_RESET                                                (1u)
#define BITM_FIFO_CONTROL_STATS_RESET                                       (0x01000000u)

// Bit field: read_mode
// Bit position, length and mask
#define BITP_FIFO_CONTROL_READ_MODE                                                 (26u)
#define BITL_FIFO_CONTROL_READ_MODE                                                  (2u)
#define BITM_FIFO_CONTROL_READ_MODE                                         (0x0C000000u)
// Enums
#define ENUM_FIFO_CONTROL_READ_MODE_MANUAL_MODE                             (0x00000000u)
#define ENUM_FIFO_CONTROL_READ_MODE_PERIODIC_UNTIL_EMPTY                    (0x00000001u)
#define ENUM_FIFO_CONTROL_READ_MODE_PERIODIC_BLOCK_READS                    (0x00000002u)
#define ENUM_FIFO_CONTROL_READ_MODE_ON_DEMAND                               (0x00000003u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_FIFO_CONTROL_SOFT_RESET                                                (28u)
#define BITL_FIFO_CONTROL_SOFT_RESET                                                 (4u)
#define BITM_FIFO_CONTROL_SOFT_RESET                                        (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : read_block_sizes
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FIFO_READ_BLOCK_SIZES_ADDR                                              (0x0004u)
#define FIFO_READ_BLOCK_SIZES_NW_ADDR                                           (0x0004u)
#define FIFO_READ_BLOCK_SIZES_RESET_VAL                                     (0x00000000u)
#define FIFO_READ_BLOCK_SIZES_MMS                                                    (0u)
#define FIFO_READ_BLOCK_SIZES_VOL_MASK                                      (0x003FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 10-00 | read_block_size_instantan| Minimum transmission message      | 0x00000000  | 
// |       |                          | length required for ON_DEMAND     |             |
// |       |                          | mode when reception path is idle  |             |
// |       |                          | in bytes.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21-11 | read_block_size          | Maximum transmission message      | 0x00000000  |
// |       |                          | length for data retrieval in      |             |
// |       |                          | various operational modes.        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: read_block_size_instantaneous
// Bit position, length and mask
#define BITP_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE_INSTANTANEOUS                     (0u)
#define BITL_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE_INSTANTANEOUS                    (11u)
#define BITM_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE_INSTANTANEOUS            (0x000007FFu)

// Bit field: read_block_size
// Bit position, length and mask
#define BITP_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE                                  (11u)
#define BITL_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE                                  (11u)
#define BITM_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE                          (0x003FF800u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : read_period
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FIFO_READ_PERIOD_ADDR                                                   (0x0008u)
#define FIFO_READ_PERIOD_NW_ADDR                                                (0x0008u)
#define FIFO_READ_PERIOD_RESET_VAL                                          (0x00000000u)
#define FIFO_READ_PERIOD_MMS                                                         (0u)
#define FIFO_READ_PERIOD_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | read_period              | Configurable time interval        | 0x00000000  |
// |       |                          | between consecutive data reads,   |             |
// |       |                          | determining the frequency of      |             |
// |       |                          | periodic data retrieval in        |             |
// |       |                          | specific operational modes.       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: read_period
// Bit position, length and mask
#define BITP_FIFO_READ_PERIOD_READ_PERIOD                                            (0u)
#define BITL_FIFO_READ_PERIOD_READ_PERIOD                                           (32u)
#define BITM_FIFO_READ_PERIOD_READ_PERIOD                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : status
// Register Addr        : 0x0014u
// Register NW Addr     : 0x0014u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FIFO_STATUS_ADDR                                                        (0x0014u)
#define FIFO_STATUS_NW_ADDR                                                     (0x0014u)
#define FIFO_STATUS_RESET_VAL                                               (0x00000000u)
#define FIFO_STATUS_MMS                                                              (0u)
#define FIFO_STATUS_VOL_MASK                                                (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | write_threshold_empty_sta| Status field indicating if the    | 0x00000000  | 
// |       |                          | FIFO buffer is empty in the       |             |
// |       |                          | input (write) direction, with a   |             |
// |       |                          | fixed threshold value of 0        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | write_threshold_1_status | Indicates the current status of   | 0x00000000  |
// |       |                          | the first threshold in the input  |             |
// |       |                          | (write) direction of the FIFO     |             |
// |       |                          | buffer, providing information on  |             |
// |       |                          | whether the buffer level has      |             |
// |       |                          | reached or exceeded the           |             |
// |       |                          | configured percentage             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | write_threshold_2_status | Indicates the current status of   | 0x00000000  |
// |       |                          | the second threshold in the       |             |
// |       |                          | input (write) direction of the    |             |
// |       |                          | FIFO buffer, providing            |             |
// |       |                          | information on whether the        |             |
// |       |                          | buffer level has reached or       |             |
// |       |                          | exceeded the configured           |             |
// |       |                          | percentage                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | write_threshold_3_status | Indicates the current status of   | 0x00000000  |
// |       |                          | the third threshold in the input  |             |
// |       |                          | (write) direction of the FIFO     |             |
// |       |                          | buffer, providing information on  |             |
// |       |                          | whether the buffer level has      |             |
// |       |                          | reached or exceeded the           |             |
// |       |                          | configured percentage             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | write_threshold_full_stat| Status field indicating if the    | 0x00000000  | 
// |       |                          | FIFO buffer is full in the input  |             |
// |       |                          | (write) direction                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | read_threshold_empty_stat| Status field indicating if the    | 0x00000000  | 
// |       |                          | FIFO buffer is empty in the       |             |
// |       |                          | output (read) direction, with a   |             |
// |       |                          | fixed threshold value of 0        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | read_threshold_1_status  | Indicates the current status of   | 0x00000000  |
// |       |                          | the first threshold in the        |             |
// |       |                          | output (read) direction of the    |             |
// |       |                          | FIFO buffer, providing            |             |
// |       |                          | information on whether the        |             |
// |       |                          | buffer level has reached or       |             |
// |       |                          | exceeded the configured           |             |
// |       |                          | percentage                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | read_threshold_2_status  | Indicates the current status of   | 0x00000000  |
// |       |                          | the second threshold in the       |             |
// |       |                          | output (read) direction of the    |             |
// |       |                          | FIFO buffer, providing            |             |
// |       |                          | information on whether the        |             |
// |       |                          | buffer level has reached or       |             |
// |       |                          | exceeded the configured           |             |
// |       |                          | percentage                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | read_threshold_3_status  | Indicates the current status of   | 0x00000000  |
// |       |                          | the third threshold in the        |             |
// |       |                          | output (read) direction of the    |             |
// |       |                          | FIFO buffer, providing            |             |
// |       |                          | information on whether the        |             |
// |       |                          | buffer level has reached or       |             |
// |       |                          | exceeded the configured           |             |
// |       |                          | percentage                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | read_threshold_full_statu| Status field indicating if the    | 0x00000000  | 
// |       |                          | FIFO buffer is full in the        |             |
// |       |                          | output (read) direction           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: write_threshold_empty_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_WRITE_THRESHOLD_EMPTY_STATUS                                (0u)
#define BITL_FIFO_STATUS_WRITE_THRESHOLD_EMPTY_STATUS                                (1u)
#define BITM_FIFO_STATUS_WRITE_THRESHOLD_EMPTY_STATUS                       (0x00000001u)
// Enums
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_EMPTY_STATUS_EMPTY                 (0x00000001u)
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_EMPTY_STATUS_NOT_EMPTY             (0x00000000u)

// Bit field: write_threshold_1_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_WRITE_THRESHOLD_1_STATUS                                    (1u)
#define BITL_FIFO_STATUS_WRITE_THRESHOLD_1_STATUS                                    (1u)
#define BITM_FIFO_STATUS_WRITE_THRESHOLD_1_STATUS                           (0x00000002u)
// Enums
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_1_STATUS_THRESHOLD_MEET            (0x00000001u)
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_1_STATUS_THRESHOLD_NOT_MEET        (0x00000000u)

// Bit field: write_threshold_2_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_WRITE_THRESHOLD_2_STATUS                                    (2u)
#define BITL_FIFO_STATUS_WRITE_THRESHOLD_2_STATUS                                    (1u)
#define BITM_FIFO_STATUS_WRITE_THRESHOLD_2_STATUS                           (0x00000004u)
// Enums
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_2_STATUS_THRESHOLD_MEET            (0x00000001u)
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_2_STATUS_THRESHOLD_NOT_MEET        (0x00000000u)

// Bit field: write_threshold_3_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_WRITE_THRESHOLD_3_STATUS                                    (3u)
#define BITL_FIFO_STATUS_WRITE_THRESHOLD_3_STATUS                                    (1u)
#define BITM_FIFO_STATUS_WRITE_THRESHOLD_3_STATUS                           (0x00000008u)
// Enums
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_3_STATUS_THRESHOLD_MEET            (0x00000001u)
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_3_STATUS_THRESHOLD_NOT_MEET        (0x00000000u)

// Bit field: write_threshold_full_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_WRITE_THRESHOLD_FULL_STATUS                                 (4u)
#define BITL_FIFO_STATUS_WRITE_THRESHOLD_FULL_STATUS                                 (1u)
#define BITM_FIFO_STATUS_WRITE_THRESHOLD_FULL_STATUS                        (0x00000010u)
// Enums
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_FULL_STATUS_FULL                   (0x00000001u)
#define ENUM_FIFO_STATUS_WRITE_THRESHOLD_FULL_STATUS_NOT_FULL               (0x00000000u)

// Bit field: read_threshold_empty_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_READ_THRESHOLD_EMPTY_STATUS                                (16u)
#define BITL_FIFO_STATUS_READ_THRESHOLD_EMPTY_STATUS                                 (1u)
#define BITM_FIFO_STATUS_READ_THRESHOLD_EMPTY_STATUS                        (0x00010000u)
// Enums
#define ENUM_FIFO_STATUS_READ_THRESHOLD_EMPTY_STATUS_EMPTY                  (0x00000001u)
#define ENUM_FIFO_STATUS_READ_THRESHOLD_EMPTY_STATUS_NOT_EMPTY              (0x00000000u)

// Bit field: read_threshold_1_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_READ_THRESHOLD_1_STATUS                                    (17u)
#define BITL_FIFO_STATUS_READ_THRESHOLD_1_STATUS                                     (1u)
#define BITM_FIFO_STATUS_READ_THRESHOLD_1_STATUS                            (0x00020000u)
// Enums
#define ENUM_FIFO_STATUS_READ_THRESHOLD_1_STATUS_THRESHOLD_MEET             (0x00000001u)
#define ENUM_FIFO_STATUS_READ_THRESHOLD_1_STATUS_THRESHOLD_NOT_MEET         (0x00000000u)

// Bit field: read_threshold_2_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_READ_THRESHOLD_2_STATUS                                    (18u)
#define BITL_FIFO_STATUS_READ_THRESHOLD_2_STATUS                                     (1u)
#define BITM_FIFO_STATUS_READ_THRESHOLD_2_STATUS                            (0x00040000u)
// Enums
#define ENUM_FIFO_STATUS_READ_THRESHOLD_2_STATUS_THRESHOLD_MEET             (0x00000001u)
#define ENUM_FIFO_STATUS_READ_THRESHOLD_2_STATUS_THRESHOLD_NOT_MEET         (0x00000000u)

// Bit field: read_threshold_3_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_READ_THRESHOLD_3_STATUS                                    (19u)
#define BITL_FIFO_STATUS_READ_THRESHOLD_3_STATUS                                     (1u)
#define BITM_FIFO_STATUS_READ_THRESHOLD_3_STATUS                            (0x00080000u)
// Enums
#define ENUM_FIFO_STATUS_READ_THRESHOLD_3_STATUS_THRESHOLD_MEET             (0x00000001u)
#define ENUM_FIFO_STATUS_READ_THRESHOLD_3_STATUS_THRESHOLD_NOT_MEET         (0x00000000u)

// Bit field: read_threshold_full_status
// Bit position, length and mask
#define BITP_FIFO_STATUS_READ_THRESHOLD_FULL_STATUS                                 (20u)
#define BITL_FIFO_STATUS_READ_THRESHOLD_FULL_STATUS                                  (1u)
#define BITM_FIFO_STATUS_READ_THRESHOLD_FULL_STATUS                         (0x00100000u)
// Enums
#define ENUM_FIFO_STATUS_READ_THRESHOLD_FULL_STATUS_FULL                    (0x00000001u)
#define ENUM_FIFO_STATUS_READ_THRESHOLD_FULL_STATUS_NOT_FULL                (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : read_stats
// Register Addr        : 0x0018u
// Register NW Addr     : 0x0018u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FIFO_READ_STATS_ADDR                                                    (0x0018u)
#define FIFO_READ_STATS_NW_ADDR                                                 (0x0018u)
#define FIFO_READ_STATS_RESET_VAL                                           (0x00000000u)
#define FIFO_READ_STATS_MMS                                                          (0u)
#define FIFO_READ_STATS_VOL_MASK                                            (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | read_read_stats          | Represents the total amount of    | 0x00000000  |
// |       |                          | data read from the output data    |             |
// |       |                          | stream                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | read_write_stats         | Represents the total amount of    | 0x00000000  |
// |       |                          | data written to the output data   |             |
// |       |                          | stream in bytes.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: read_read_stats
// Bit position, length and mask
#define BITP_FIFO_READ_STATS_READ_READ_STATS                                         (0u)
#define BITL_FIFO_READ_STATS_READ_READ_STATS                                        (16u)
#define BITM_FIFO_READ_STATS_READ_READ_STATS                                (0x0000FFFFu)

// Bit field: read_write_stats
// Bit position, length and mask
#define BITP_FIFO_READ_STATS_READ_WRITE_STATS                                       (16u)
#define BITL_FIFO_READ_STATS_READ_WRITE_STATS                                       (16u)
#define BITM_FIFO_READ_STATS_READ_WRITE_STATS                               (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : thresholds
// Register Addr        : 0x001Cu
// Register NW Addr     : 0x001Cu
// Register Reset Value : 0x22182218u
// Register MMS         : 0u

#define FIFO_THRESHOLDS_ADDR                                                    (0x001Cu)
#define FIFO_THRESHOLDS_NW_ADDR                                                 (0x001Cu)
#define FIFO_THRESHOLDS_RESET_VAL                                           (0x22182218u)
#define FIFO_THRESHOLDS_MMS                                                          (0u)
#define FIFO_THRESHOLDS_VOL_MASK                                            (0x7FFF7FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 04-00 | FIFO_read_threshold_3_rat| Sets the percentage level for     | 0x00000018  | 
// |       |                          | the third threshold in the        |             |
// |       |                          | output (read) direction of the    |             |
// |       |                          | FIFO buffer, with each step       |             |
// |       |                          | representing 1/32 or 0.03125      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 09-05 | FIFO_read_threshold_2_rat| Sets the percentage level for     | 0x00000010  | 
// |       |                          | the second threshold in the       |             |
// |       |                          | output (read) direction of the    |             |
// |       |                          | FIFO buffer, with each step       |             |
// |       |                          | representing 1/32 or 0.03125      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14-10 | FIFO_read_threshold_1_rat| Sets the percentage level for     | 0x00000008  | 
// |       |                          | the first threshold in the        |             |
// |       |                          | output (read) direction of the    |             |
// |       |                          | FIFO buffer, with each step       |             |
// |       |                          | representing 1/32 or 0.03125      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-16 | FIFO_write_threshold_3_ra| Sets the percentage level for     | 0x00000018  | 
// |       |                          | the third threshold in the input  |             |
// |       |                          | (write) direction of the FIFO     |             |
// |       |                          | buffer, with each step            |             |
// |       |                          | representing 1/32 or 0.03125      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25-21 | FIFO_write_threshold_2_ra| Sets the percentage level for     | 0x00000010  | 
// |       |                          | the second threshold in the       |             |
// |       |                          | input (write) direction of the    |             |
// |       |                          | FIFO buffer, with each step       |             |
// |       |                          | representing 1/32 or 0.03125      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30-26 | FIFO_write_threshold_1_ra| Sets the percentage level for     | 0x00000008  | 
// |       |                          | the first threshold in the input  |             |
// |       |                          | (write) direction of the FIFO     |             |
// |       |                          | buffer, with each step            |             |
// |       |                          | representing 1/32 or 0.03125      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: FIFO_read_threshold_3_ratio
// Bit position, length and mask
#define BITP_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO                             (0u)
#define BITL_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO                             (5u)
#define BITM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO                    (0x0000001Fu)
// Enums
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_0        (0x00000000u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_1        (0x00000001u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_2        (0x00000002u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_3        (0x00000003u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_4        (0x00000004u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_5        (0x00000005u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_6        (0x00000006u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_7        (0x00000007u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_8        (0x00000008u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_9        (0x00000009u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_10       (0x0000000Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_11       (0x0000000Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_12       (0x0000000Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_13       (0x0000000Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_14       (0x0000000Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_15       (0x0000000Fu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_16       (0x00000010u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_17       (0x00000011u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_18       (0x00000012u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_19       (0x00000013u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_20       (0x00000014u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_21       (0x00000015u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_22       (0x00000016u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_23       (0x00000017u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_24       (0x00000018u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_25       (0x00000019u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_26       (0x0000001Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_27       (0x0000001Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_28       (0x0000001Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_29       (0x0000001Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_30       (0x0000001Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_3_RATIO_THRESHOLD_31       (0x0000001Fu)

// Bit field: FIFO_read_threshold_2_ratio
// Bit position, length and mask
#define BITP_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO                             (5u)
#define BITL_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO                             (5u)
#define BITM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO                    (0x000003E0u)
// Enums
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_0        (0x00000000u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_1        (0x00000001u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_2        (0x00000002u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_3        (0x00000003u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_4        (0x00000004u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_5        (0x00000005u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_6        (0x00000006u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_7        (0x00000007u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_8        (0x00000008u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_9        (0x00000009u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_10       (0x0000000Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_11       (0x0000000Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_12       (0x0000000Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_13       (0x0000000Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_14       (0x0000000Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_15       (0x0000000Fu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_16       (0x00000010u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_17       (0x00000011u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_18       (0x00000012u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_19       (0x00000013u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_20       (0x00000014u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_21       (0x00000015u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_22       (0x00000016u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_23       (0x00000017u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_24       (0x00000018u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_25       (0x00000019u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_26       (0x0000001Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_27       (0x0000001Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_28       (0x0000001Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_29       (0x0000001Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_30       (0x0000001Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_2_RATIO_THRESHOLD_31       (0x0000001Fu)

// Bit field: FIFO_read_threshold_1_ratio
// Bit position, length and mask
#define BITP_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO                            (10u)
#define BITL_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO                             (5u)
#define BITM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO                    (0x00007C00u)
// Enums
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_0        (0x00000000u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_1        (0x00000001u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_2        (0x00000002u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_3        (0x00000003u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_4        (0x00000004u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_5        (0x00000005u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_6        (0x00000006u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_7        (0x00000007u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_8        (0x00000008u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_9        (0x00000009u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_10       (0x0000000Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_11       (0x0000000Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_12       (0x0000000Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_13       (0x0000000Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_14       (0x0000000Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_15       (0x0000000Fu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_16       (0x00000010u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_17       (0x00000011u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_18       (0x00000012u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_19       (0x00000013u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_20       (0x00000014u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_21       (0x00000015u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_22       (0x00000016u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_23       (0x00000017u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_24       (0x00000018u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_25       (0x00000019u)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_26       (0x0000001Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_27       (0x0000001Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_28       (0x0000001Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_29       (0x0000001Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_30       (0x0000001Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_READ_THRESHOLD_1_RATIO_THRESHOLD_31       (0x0000001Fu)

// Bit field: FIFO_write_threshold_3_ratio
// Bit position, length and mask
#define BITP_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO                           (16u)
#define BITL_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO                            (5u)
#define BITM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO                   (0x001F0000u)
// Enums
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_0       (0x00000000u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_1       (0x00000001u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_2       (0x00000002u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_3       (0x00000003u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_4       (0x00000004u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_5       (0x00000005u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_6       (0x00000006u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_7       (0x00000007u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_8       (0x00000008u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_9       (0x00000009u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_10      (0x0000000Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_11      (0x0000000Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_12      (0x0000000Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_13      (0x0000000Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_14      (0x0000000Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_15      (0x0000000Fu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_16      (0x00000010u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_17      (0x00000011u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_18      (0x00000012u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_19      (0x00000013u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_20      (0x00000014u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_21      (0x00000015u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_22      (0x00000016u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_23      (0x00000017u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_24      (0x00000018u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_25      (0x00000019u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_26      (0x0000001Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_27      (0x0000001Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_28      (0x0000001Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_29      (0x0000001Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_30      (0x0000001Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_3_RATIO_THRESHOLD_31      (0x0000001Fu)

// Bit field: FIFO_write_threshold_2_ratio
// Bit position, length and mask
#define BITP_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO                           (21u)
#define BITL_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO                            (5u)
#define BITM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO                   (0x03E00000u)
// Enums
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_0       (0x00000000u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_1       (0x00000001u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_2       (0x00000002u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_3       (0x00000003u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_4       (0x00000004u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_5       (0x00000005u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_6       (0x00000006u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_7       (0x00000007u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_8       (0x00000008u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_9       (0x00000009u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_10      (0x0000000Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_11      (0x0000000Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_12      (0x0000000Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_13      (0x0000000Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_14      (0x0000000Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_15      (0x0000000Fu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_16      (0x00000010u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_17      (0x00000011u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_18      (0x00000012u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_19      (0x00000013u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_20      (0x00000014u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_21      (0x00000015u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_22      (0x00000016u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_23      (0x00000017u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_24      (0x00000018u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_25      (0x00000019u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_26      (0x0000001Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_27      (0x0000001Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_28      (0x0000001Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_29      (0x0000001Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_30      (0x0000001Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_2_RATIO_THRESHOLD_31      (0x0000001Fu)

// Bit field: FIFO_write_threshold_1_ratio
// Bit position, length and mask
#define BITP_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO                           (26u)
#define BITL_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO                            (5u)
#define BITM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO                   (0x7C000000u)
// Enums
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_0       (0x00000000u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_1       (0x00000001u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_2       (0x00000002u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_3       (0x00000003u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_4       (0x00000004u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_5       (0x00000005u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_6       (0x00000006u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_7       (0x00000007u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_8       (0x00000008u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_9       (0x00000009u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_10      (0x0000000Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_11      (0x0000000Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_12      (0x0000000Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_13      (0x0000000Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_14      (0x0000000Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_15      (0x0000000Fu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_16      (0x00000010u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_17      (0x00000011u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_18      (0x00000012u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_19      (0x00000013u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_20      (0x00000014u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_21      (0x00000015u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_22      (0x00000016u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_23      (0x00000017u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_24      (0x00000018u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_25      (0x00000019u)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_26      (0x0000001Au)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_27      (0x0000001Bu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_28      (0x0000001Cu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_29      (0x0000001Du)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_30      (0x0000001Eu)
#define ENUM_FIFO_THRESHOLDS_FIFO_WRITE_THRESHOLD_1_RATIO_THRESHOLD_31      (0x0000001Fu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : write_stats
// Register Addr        : 0x0020u
// Register NW Addr     : 0x0020u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FIFO_WRITE_STATS_ADDR                                                   (0x0020u)
#define FIFO_WRITE_STATS_NW_ADDR                                                (0x0020u)
#define FIFO_WRITE_STATS_RESET_VAL                                          (0x00000000u)
#define FIFO_WRITE_STATS_MMS                                                         (0u)
#define FIFO_WRITE_STATS_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | write_read_stats         | Represents the total amount of    | 0x00000000  |
// |       |                          | data read from the input data     |             |
// |       |                          | stream in bytes.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | write_write_stats        | Represents the total amount of    | 0x00000000  |
// |       |                          | data written to the input data    |             |
// |       |                          | stream in bytes.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: write_read_stats
// Bit position, length and mask
#define BITP_FIFO_WRITE_STATS_WRITE_READ_STATS                                       (0u)
#define BITL_FIFO_WRITE_STATS_WRITE_READ_STATS                                      (16u)
#define BITM_FIFO_WRITE_STATS_WRITE_READ_STATS                              (0x0000FFFFu)

// Bit field: write_write_stats
// Bit position, length and mask
#define BITP_FIFO_WRITE_STATS_WRITE_WRITE_STATS                                     (16u)
#define BITL_FIFO_WRITE_STATS_WRITE_WRITE_STATS                                     (16u)
#define BITM_FIFO_WRITE_STATS_WRITE_WRITE_STATS                             (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : level
// Register Addr        : 0x0024u
// Register NW Addr     : 0x0024u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FIFO_LEVEL_ADDR                                                         (0x0024u)
#define FIFO_LEVEL_NW_ADDR                                                      (0x0024u)
#define FIFO_LEVEL_RESET_VAL                                                (0x00000000u)
#define FIFO_LEVEL_MMS                                                               (0u)
#define FIFO_LEVEL_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | read_fifo_level          | Indicates the current amount of   | 0x00000000  |
// |       |                          | data available in the input data  |             |
// |       |                          | stream buffer in bytes.           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | write_fifo_level         | Indicates the current amount of   | 0x00000000  |
// |       |                          | data available in the output      |             |
// |       |                          | data stream buffer                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: read_fifo_level
// Bit position, length and mask
#define BITP_FIFO_LEVEL_READ_FIFO_LEVEL                                              (0u)
#define BITL_FIFO_LEVEL_READ_FIFO_LEVEL                                             (16u)
#define BITM_FIFO_LEVEL_READ_FIFO_LEVEL                                     (0x0000FFFFu)

// Bit field: write_fifo_level
// Bit position, length and mask
#define BITP_FIFO_LEVEL_WRITE_FIFO_LEVEL                                            (16u)
#define BITL_FIFO_LEVEL_WRITE_FIFO_LEVEL                                            (16u)
#define BITM_FIFO_LEVEL_WRITE_FIFO_LEVEL                                    (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : read_block_time
// Register Addr        : 0x0028u
// Register NW Addr     : 0x0028u
// Register Reset Value : 0x00000064u
// Register MMS         : 0u

#define FIFO_READ_BLOCK_TIME_ADDR                                               (0x0028u)
#define FIFO_READ_BLOCK_TIME_NW_ADDR                                            (0x0028u)
#define FIFO_READ_BLOCK_TIME_RESET_VAL                                      (0x00000064u)
#define FIFO_READ_BLOCK_TIME_MMS                                                     (0u)
#define FIFO_READ_BLOCK_TIME_VOL_MASK                                       (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | read_block_time_instantan| Configurable time duration        | 0x00000064  | 
// |       |                          | determining FIFO path idle        |             |
// |       |                          | status for data transmission in   |             |
// |       |                          | ON_DEMAND mode.                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: read_block_time_instantaneous
// Bit position, length and mask
#define BITP_FIFO_READ_BLOCK_TIME_READ_BLOCK_TIME_INSTANTANEOUS                      (0u)
#define BITL_FIFO_READ_BLOCK_TIME_READ_BLOCK_TIME_INSTANTANEOUS                     (16u)
#define BITM_FIFO_READ_BLOCK_TIME_READ_BLOCK_TIME_INSTANTANEOUS             (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ctag_configuration
// Register Addr        : 0x002Cu
// Register NW Addr     : 0x002Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FIFO_CTAG_CONFIGURATION_ADDR                                            (0x002Cu)
#define FIFO_CTAG_CONFIGURATION_NW_ADDR                                         (0x002Cu)
#define FIFO_CTAG_CONFIGURATION_RESET_VAL                                   (0x00000000u)
#define FIFO_CTAG_CONFIGURATION_MMS                                                  (0u)
#define FIFO_CTAG_CONFIGURATION_VOL_MASK                                    (0x8000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | ctag_id                  | The 802.1Q CTAG ID for this FIFO  | 0x00000000  |
// |       |                          | messages. If transmit_ctag is     |             |
// |       |                          | set false, this field shall be    |             |
// |       |                          | set to zero.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | ctag_drop_eligible       | The 802.1Q CTAG drop eligible     | 0x00000000  |
// |       |                          | for this FIFO messages            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-13 | ctag_priority            | The 802.1Q CTAG priority for      | 0x00000000  |
// |       |                          | this FIFO messages                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | transmit_ctag            | The 802.1Q CTAG will be attached  | 0x00000000  |
// |       |                          | to frames that contains messages  |             |
// |       |                          | from this FIFO                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: ctag_id
// Bit position, length and mask
#define BITP_FIFO_CTAG_CONFIGURATION_CTAG_ID                                         (0u)
#define BITL_FIFO_CTAG_CONFIGURATION_CTAG_ID                                        (12u)
#define BITM_FIFO_CTAG_CONFIGURATION_CTAG_ID                                (0x00000FFFu)

// Bit field: ctag_drop_eligible
// Bit position, length and mask
#define BITP_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE                             (12u)
#define BITL_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE                              (1u)
#define BITM_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE                     (0x00001000u)
// Enums
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE_DONT_DROP           (0x00000000u)
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_DROP_ELIGIBLE_DROP_ELIGIBLE       (0x00000001u)

// Bit field: ctag_priority
// Bit position, length and mask
#define BITP_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY                                  (13u)
#define BITL_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY                                   (3u)
#define BITM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY                          (0x0000E000u)
// Enums
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY_NETWORK_CONTROL          (0x00000007u)
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY_VOICE                    (0x00000006u)
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY_VIDEO                    (0x00000005u)
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY_CONTROLLED_LOAD          (0x00000004u)
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY_EXCELLENT_EFFORT         (0x00000003u)
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY_BEST_EFFORT              (0x00000002u)
#define ENUM_FIFO_CTAG_CONFIGURATION_CTAG_PRIORITY_BACKGROUND               (0x00000000u)

// Bit field: transmit_ctag
// Bit position, length and mask
#define BITP_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG                                  (31u)
#define BITL_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG                                   (1u)
#define BITM_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG                          (0x80000000u)
// Enums
#define ENUM_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG_TRANSMIT_CTAG            (0x00000001u)
#define ENUM_FIFO_CTAG_CONFIGURATION_TRANSMIT_CTAG_DONT_TRANSMIT_CTAG       (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillFIFORegsData();
#endif


#endif /* ADI_E2BCORE_REGFIFO_H */

/** @} */

/*
 * EOF: www.analog.com
 */


