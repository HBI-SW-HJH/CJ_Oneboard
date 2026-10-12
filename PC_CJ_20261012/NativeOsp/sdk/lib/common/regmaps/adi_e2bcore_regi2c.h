/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_regi2c.h
 * @brief        : This is an auto generated header file with register
                   descriptions for I2C interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGI2C_H
#define ADI_E2BCORE_REGI2C_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_NUM_REGS    (6u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganI2CRegAddr[I2C_NUM_REGS];
extern uint32_t ganI2CNwRegAddr[I2C_NUM_REGS];
extern uint32_t ganI2CRegMMS[I2C_NUM_REGS];
extern uint32_t ganI2CRegResetVal[I2C_NUM_REGS];
extern const char *gasI2CRegName[I2C_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00000800u
// Register MMS         : 0u

#define I2C_CONTROL_ADDR                                                        (0x0000u)
#define I2C_CONTROL_NW_ADDR                                                     (0x0000u)
#define I2C_CONTROL_RESET_VAL                                               (0x00000800u)
#define I2C_CONTROL_MMS                                                              (0u)
#define I2C_CONTROL_VOL_MASK                                                (0xFFEB1FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | target_device_address    | Default configuartion used for    | 0x00000000  |
// |       |                          | the optional field "slave         |             |
// |       |                          | address" in the interface         |             |
// |       |                          | payload.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | target_device_address_typ| Default configuartion used for    | 0x00000000  | 
// |       |                          | the optional field "slave         |             |
// |       |                          | address type" in the interface    |             |
// |       |                          | payload.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | transaction_header_enable| Reserved. It shall be always      | 0x00000001  | 
// |       |                          | set.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | resync_required          | Executes the resynchronization    | 0x00000000  |
// |       |                          | procedure. Please, check the      |             |
// |       |                          | user guide of the interface for   |             |
// |       |                          | more information about this       |             |
// |       |                          | procedure.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | controller_transaction_tr| Triggers one transaction. If      | 0x00000000  | 
// |       |                          | there is no data in the transmit  |             |
// |       |                          | FIFO, the next transaction will   |             |
// |       |                          | be executed as soon as it         |             |
// |       |                          | arrives. This field is only       |             |
// |       |                          | valid if trigger_mode is set to   |             |
// |       |                          | one (1).                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | controller_trigger_mode  | Select the transaction            | 0x00000000  |
// |       |                          | processing behavior.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | enable_read_time_tag     | If set, the interface will tag    | 0x00000000  |
// |       |                          | the interface transaction         |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | enable                   | Enables or disables the I2C       | 0x00000000  |
// |       |                          | interface.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25-23 | speed                    | Selects the I2C master mode       | 0x00000000  |
// |       |                          | speed.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 26    | tx_fifo_reset            | Resets the hardware interface     | 0x00000000  |
// |       |                          | transmission channel storage      |             |
// |       |                          | that is connected to this         |             |
// |       |                          | interface.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27    | rx_fifo_reset            | Resets the hardware interface     | 0x00000000  |
// |       |                          | reception channel storage that    |             |
// |       |                          | is connected to this interface.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | soft_reset               | Reset key. Must write a value of  | 0x00000000  |
// |       |                          | 0xA to this field to cause a      |             |
// |       |                          | soft reset of the interface.      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: target_device_address
// Bit position, length and mask
#define BITP_I2C_CONTROL_TARGET_DEVICE_ADDRESS                                       (0u)
#define BITL_I2C_CONTROL_TARGET_DEVICE_ADDRESS                                      (10u)
#define BITM_I2C_CONTROL_TARGET_DEVICE_ADDRESS                              (0x000003FFu)

// Bit field: target_device_address_type
// Bit position, length and mask
#define BITP_I2C_CONTROL_TARGET_DEVICE_ADDRESS_TYPE                                 (10u)
#define BITL_I2C_CONTROL_TARGET_DEVICE_ADDRESS_TYPE                                  (1u)
#define BITM_I2C_CONTROL_TARGET_DEVICE_ADDRESS_TYPE                         (0x00000400u)
// Enums
#define ENUM_I2C_CONTROL_TARGET_DEVICE_ADDRESS_TYPE_ADDR_7BIT               (0x00000000u)
#define ENUM_I2C_CONTROL_TARGET_DEVICE_ADDRESS_TYPE_ADDR_10BIT              (0x00000001u)

// Bit field: transaction_header_enable
// Bit position, length and mask
#define BITP_I2C_CONTROL_TRANSACTION_HEADER_ENABLE                                  (11u)
#define BITL_I2C_CONTROL_TRANSACTION_HEADER_ENABLE                                   (1u)
#define BITM_I2C_CONTROL_TRANSACTION_HEADER_ENABLE                          (0x00000800u)
// Enums
#define ENUM_I2C_CONTROL_TRANSACTION_HEADER_ENABLE_RESERVED                 (0x00000000u)
#define ENUM_I2C_CONTROL_TRANSACTION_HEADER_ENABLE_ENABLED                  (0x00000001u)

// Bit field: resync_required
// Bit position, length and mask
#define BITP_I2C_CONTROL_RESYNC_REQUIRED                                            (12u)
#define BITL_I2C_CONTROL_RESYNC_REQUIRED                                             (1u)
#define BITM_I2C_CONTROL_RESYNC_REQUIRED                                    (0x00001000u)

// Bit field: controller_transaction_trigger
// Bit position, length and mask
#define BITP_I2C_CONTROL_CONTROLLER_TRANSACTION_TRIGGER                             (16u)
#define BITL_I2C_CONTROL_CONTROLLER_TRANSACTION_TRIGGER                              (1u)
#define BITM_I2C_CONTROL_CONTROLLER_TRANSACTION_TRIGGER                     (0x00010000u)

// Bit field: controller_trigger_mode
// Bit position, length and mask
#define BITP_I2C_CONTROL_CONTROLLER_TRIGGER_MODE                                    (17u)
#define BITL_I2C_CONTROL_CONTROLLER_TRIGGER_MODE                                     (1u)
#define BITM_I2C_CONTROL_CONTROLLER_TRIGGER_MODE                            (0x00020000u)
// Enums
#define ENUM_I2C_CONTROL_CONTROLLER_TRIGGER_MODE_MANUAL                     (0x00000000u)
#define ENUM_I2C_CONTROL_CONTROLLER_TRIGGER_MODE_AUTOMATIC                  (0x00000001u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_I2C_CONTROL_ENABLE_READ_TIME_TAG                                       (19u)
#define BITL_I2C_CONTROL_ENABLE_READ_TIME_TAG                                        (1u)
#define BITM_I2C_CONTROL_ENABLE_READ_TIME_TAG                               (0x00080000u)
// Enums
#define ENUM_I2C_CONTROL_ENABLE_READ_TIME_TAG_DISABLE                       (0x00000000u)
#define ENUM_I2C_CONTROL_ENABLE_READ_TIME_TAG_ENABLE                        (0x00000001u)

// Bit field: enable
// Bit position, length and mask
#define BITP_I2C_CONTROL_ENABLE                                                     (21u)
#define BITL_I2C_CONTROL_ENABLE                                                      (1u)
#define BITM_I2C_CONTROL_ENABLE                                             (0x00200000u)
// Enums
#define ENUM_I2C_CONTROL_ENABLE_DISABLE                                     (0x00000000u)
#define ENUM_I2C_CONTROL_ENABLE_ENABLE                                      (0x00000001u)

// Bit field: speed
// Bit position, length and mask
#define BITP_I2C_CONTROL_SPEED                                                      (23u)
#define BITL_I2C_CONTROL_SPEED                                                       (3u)
#define BITM_I2C_CONTROL_SPEED                                              (0x03800000u)
// Enums
#define ENUM_I2C_CONTROL_SPEED_STANDARD                                     (0x00000000u)
#define ENUM_I2C_CONTROL_SPEED_FAST_MODE                                    (0x00000001u)
#define ENUM_I2C_CONTROL_SPEED_FAST_PLUS                                    (0x00000002u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_I2C_CONTROL_TX_FIFO_RESET                                              (26u)
#define BITL_I2C_CONTROL_TX_FIFO_RESET                                               (1u)
#define BITM_I2C_CONTROL_TX_FIFO_RESET                                      (0x04000000u)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_I2C_CONTROL_RX_FIFO_RESET                                              (27u)
#define BITL_I2C_CONTROL_RX_FIFO_RESET                                               (1u)
#define BITM_I2C_CONTROL_RX_FIFO_RESET                                      (0x08000000u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_I2C_CONTROL_SOFT_RESET                                                 (28u)
#define BITL_I2C_CONTROL_SOFT_RESET                                                  (4u)
#define BITM_I2C_CONTROL_SOFT_RESET                                         (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : length
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define I2C_LENGTH_ADDR                                                         (0x0004u)
#define I2C_LENGTH_NW_ADDR                                                      (0x0004u)
#define I2C_LENGTH_RESET_VAL                                                (0x00000000u)
#define I2C_LENGTH_MMS                                                               (0u)
#define I2C_LENGTH_VOL_MASK                                                 (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | manual_rd_bytes          | Default configuration used for    | 0x00000000  |
// |       |                          | the optional field                |             |
// |       |                          | "manual_rd_bytes" in the          |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | manual_wr_bytes          | Default configuration used for    | 0x00000000  |
// |       |                          | the optional field                |             |
// |       |                          | "manual_wr_bytes" in the          |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: manual_rd_bytes
// Bit position, length and mask
#define BITP_I2C_LENGTH_MANUAL_RD_BYTES                                              (0u)
#define BITL_I2C_LENGTH_MANUAL_RD_BYTES                                             (16u)
#define BITM_I2C_LENGTH_MANUAL_RD_BYTES                                     (0x0000FFFFu)

// Bit field: manual_wr_bytes
// Bit position, length and mask
#define BITP_I2C_LENGTH_MANUAL_WR_BYTES                                             (16u)
#define BITL_I2C_LENGTH_MANUAL_WR_BYTES                                             (16u)
#define BITM_I2C_LENGTH_MANUAL_WR_BYTES                                     (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : stats1
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define I2C_STATS1_ADDR                                                         (0x0008u)
#define I2C_STATS1_NW_ADDR                                                      (0x0008u)
#define I2C_STATS1_RESET_VAL                                                (0x00000000u)
#define I2C_STATS1_MMS                                                               (0u)
#define I2C_STATS1_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_rd_octets            | Counts the number of octets that  | 0x00000000  |
// |       |                          | have been read from the SDATA     |             |
// |       |                          | pin.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | num_trans                | Counts the number of              | 0x00000000  |
// |       |                          | transactions that have been       |             |
// |       |                          | executed.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_rd_octets
// Bit position, length and mask
#define BITP_I2C_STATS1_NUM_RD_OCTETS                                                (0u)
#define BITL_I2C_STATS1_NUM_RD_OCTETS                                               (16u)
#define BITM_I2C_STATS1_NUM_RD_OCTETS                                       (0x0000FFFFu)

// Bit field: num_trans
// Bit position, length and mask
#define BITP_I2C_STATS1_NUM_TRANS                                                   (16u)
#define BITL_I2C_STATS1_NUM_TRANS                                                   (16u)
#define BITM_I2C_STATS1_NUM_TRANS                                           (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : stats2
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define I2C_STATS2_ADDR                                                         (0x000Cu)
#define I2C_STATS2_NW_ADDR                                                      (0x000Cu)
#define I2C_STATS2_RESET_VAL                                                (0x00000000u)
#define I2C_STATS2_MMS                                                               (0u)
#define I2C_STATS2_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | num_wr_octets            | Counts the number of octets that  | 0x00000000  |
// |       |                          | have been written into the SDATA  |             |
// |       |                          | pin.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_wr_octets
// Bit position, length and mask
#define BITP_I2C_STATS2_NUM_WR_OCTETS                                                (0u)
#define BITL_I2C_STATS2_NUM_WR_OCTETS                                               (32u)
#define BITM_I2C_STATS2_NUM_WR_OCTETS                                       (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : status
// Register Addr        : 0x0010u
// Register NW Addr     : 0x0010u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define I2C_STATUS_ADDR                                                         (0x0010u)
#define I2C_STATUS_NW_ADDR                                                      (0x0010u)
#define I2C_STATUS_RESET_VAL                                                (0x00000000u)
#define I2C_STATUS_MMS                                                               (0u)
#define I2C_STATUS_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | bb                       | Indicates if the interface is     | 0x00000000  |
// |       |                          | currently processing a            |             |
// |       |                          | transaction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | nack                     | Indicates if the las processed    | 0x00000000  |
// |       |                          | transaction ended up in a         |             |
// |       |                          | data-not-acknowledge.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | dnack                    | Indicates if the las processed    | 0x00000000  |
// |       |                          | transaction ended up in a         |             |
// |       |                          | device-not-acknowledge.           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: bb
// Bit position, length and mask
#define BITP_I2C_STATUS_BB                                                           (0u)
#define BITL_I2C_STATUS_BB                                                           (1u)
#define BITM_I2C_STATUS_BB                                                  (0x00000001u)
// Enums
#define ENUM_I2C_STATUS_BB_FREE                                             (0x00000000u)
#define ENUM_I2C_STATUS_BB_BUSY                                             (0x00000001u)

// Bit field: nack
// Bit position, length and mask
#define BITP_I2C_STATUS_NACK                                                         (1u)
#define BITL_I2C_STATUS_NACK                                                         (1u)
#define BITM_I2C_STATUS_NACK                                                (0x00000002u)
// Enums
#define ENUM_I2C_STATUS_NACK_DETECTED                                       (0x00000001u)
#define ENUM_I2C_STATUS_NACK_NOT_DETECTED                                   (0x00000000u)

// Bit field: dnack
// Bit position, length and mask
#define BITP_I2C_STATUS_DNACK                                                        (2u)
#define BITL_I2C_STATUS_DNACK                                                        (1u)
#define BITM_I2C_STATUS_DNACK                                               (0x00000004u)
// Enums
#define ENUM_I2C_STATUS_DNACK_DETECTED                                      (0x00000001u)
#define ENUM_I2C_STATUS_DNACK_NOT_DETECTED                                  (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control2
// Register Addr        : 0x0014u
// Register NW Addr     : 0x0014u
// Register Reset Value : 0x00010001u
// Register MMS         : 0u

#define I2C_CONTROL2_ADDR                                                       (0x0014u)
#define I2C_CONTROL2_NW_ADDR                                                    (0x0014u)
#define I2C_CONTROL2_RESET_VAL                                              (0x00010001u)
#define I2C_CONTROL2_MMS                                                             (0u)
#define I2C_CONTROL2_VOL_MASK                                               (0x0001FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_packed_transactions  | Default configuration used for    | 0x00000001  |
// |       |                          | the optional field                |             |
// |       |                          | "num_packed_tranactions" in the   |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | force_wr_start_condition | Force Sending a Write Start       | 0x00000001  |
// |       |                          | Condition Although Write Bytes    |             |
// |       |                          | is Equal to 0                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_packed_transactions
// Bit position, length and mask
#define BITP_I2C_CONTROL2_NUM_PACKED_TRANSACTIONS                                    (0u)
#define BITL_I2C_CONTROL2_NUM_PACKED_TRANSACTIONS                                   (16u)
#define BITM_I2C_CONTROL2_NUM_PACKED_TRANSACTIONS                           (0x0000FFFFu)

// Bit field: force_wr_start_condition
// Bit position, length and mask
#define BITP_I2C_CONTROL2_FORCE_WR_START_CONDITION                                  (16u)
#define BITL_I2C_CONTROL2_FORCE_WR_START_CONDITION                                   (1u)
#define BITM_I2C_CONTROL2_FORCE_WR_START_CONDITION                          (0x00010000u)
// Enums
#define ENUM_I2C_CONTROL2_FORCE_WR_START_CONDITION_DISABLE_FORCED_WR_START  (0x00000000u)
#define ENUM_I2C_CONTROL2_FORCE_WR_START_CONDITION_ENABLE_FORCED_WR_START   (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillI2CRegsData();
#endif


#endif /* ADI_E2BCORE_REGI2C_H */

/** @} */

/*
 * EOF: www.analog.com
 */


