/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_regflexio.h
 * @brief        : This is an auto generated header file with register
                   descriptions for Flexible IO interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGFLEXIO_H
#define ADI_E2BCORE_REGFLEXIO_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define FLEXIO_NUM_REGS    (17u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganFLEXIORegAddr[FLEXIO_NUM_REGS];
extern uint32_t ganFLEXIONwRegAddr[FLEXIO_NUM_REGS];
extern uint32_t ganFLEXIORegMMS[FLEXIO_NUM_REGS];
extern uint32_t ganFLEXIORegResetVal[FLEXIO_NUM_REGS];
extern const char *gasFLEXIORegName[FLEXIO_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x80008002u
// Register MMS         : 0u

#define FLEXIO_CONTROL_ADDR                                                     (0x0000u)
#define FLEXIO_CONTROL_NW_ADDR                                                  (0x0000u)
#define FLEXIO_CONTROL_RESET_VAL                                            (0x80008002u)
#define FLEXIO_CONTROL_MMS                                                           (0u)
#define FLEXIO_CONTROL_VOL_MASK                                             (0xFFFFFF7Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | enable                   | Enables or disables the           | 0x00000000  |
// |       |                          | FlexibleIO interface.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | automatic_trigger        | Select the transaction            | 0x00000000  |
// |       |                          | processing behavior.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | manual_trigger           | Triggers one transaction. If      | 0x00000000  |
// |       |                          | there is no data in the transmit  |             |
// |       |                          | FIFO, the next transaction will   |             |
// |       |                          | be executed as soon as it         |             |
// |       |                          | arrives. This field is only       |             |
// |       |                          | valid if automatic_trigger is     |             |
// |       |                          | set to zero (0).                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | external_trigger_enable_r| Executes a transaction when       | 0x00000000  | 
// |       |                          | there is a rising edge in the     |             |
// |       |                          | external trigger port             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | external_trigger_enable_f| Executes a transaction when       | 0x00000000  | 
// |       |                          | there is a falling edge in the    |             |
// |       |                          | external trigger port             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | enable_read_time_tag     | If set, the interface will tag    | 0x00000000  |
// |       |                          | the interface transaction         |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | tx_fifo_reset            | Resets the hardware interface     | 0x00000000  |
// |       |                          | transmission channel storage      |             |
// |       |                          | that is connected to this         |             |
// |       |                          | interface.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | rx_fifo_reset            | Resets the hardware interface     | 0x00000000  |
// |       |                          | reception channel storage that    |             |
// |       |                          | is connected to this interface.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-10 | soft_reset               | Reset key. Must write a value of  | 0x00000000  |
// |       |                          | 0xA to this field to cause a      |             |
// |       |                          | soft reset of the interface.      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | complete_transactions    | If set, the interface will wait   | 0x00000000  |
// |       |                          | until all the data for the        |             |
// |       |                          | current transaction is present    |             |
// |       |                          | at the fifo.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30-15 | num_packed_transactions  | Default configuration used for    | 0x00000001  |
// |       |                          | the optional field                |             |
// |       |                          | "num_packed_transactions" in the  |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | ram_parity_halt_enable   | Enable or clear the state         | 0x00000001  |
// |       |                          | machine halt, caused by parity    |             |
// |       |                          | errors in the instruction RAM     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: enable
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_ENABLE                                                   (0u)
#define BITL_FLEXIO_CONTROL_ENABLE                                                   (1u)
#define BITM_FLEXIO_CONTROL_ENABLE                                          (0x00000001u)
// Enums
#define ENUM_FLEXIO_CONTROL_ENABLE_DISABLE                                  (0x00000000u)
#define ENUM_FLEXIO_CONTROL_ENABLE_ENABLE                                   (0x00000001u)

// Bit field: automatic_trigger
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_AUTOMATIC_TRIGGER                                        (2u)
#define BITL_FLEXIO_CONTROL_AUTOMATIC_TRIGGER                                        (1u)
#define BITM_FLEXIO_CONTROL_AUTOMATIC_TRIGGER                               (0x00000004u)
// Enums
#define ENUM_FLEXIO_CONTROL_AUTOMATIC_TRIGGER_MANUAL                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_AUTOMATIC_TRIGGER_AUTOMATIC                     (0x00000001u)

// Bit field: manual_trigger
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_MANUAL_TRIGGER                                           (3u)
#define BITL_FLEXIO_CONTROL_MANUAL_TRIGGER                                           (1u)
#define BITM_FLEXIO_CONTROL_MANUAL_TRIGGER                                  (0x00000008u)

// Bit field: external_trigger_enable_rising_edge
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_EXTERNAL_TRIGGER_ENABLE_RISING_EDGE                      (4u)
#define BITL_FLEXIO_CONTROL_EXTERNAL_TRIGGER_ENABLE_RISING_EDGE                      (1u)
#define BITM_FLEXIO_CONTROL_EXTERNAL_TRIGGER_ENABLE_RISING_EDGE             (0x00000010u)

// Bit field: external_trigger_enable_falling_edge
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_EXTERNAL_TRIGGER_ENABLE_FALLING_EDGE                     (5u)
#define BITL_FLEXIO_CONTROL_EXTERNAL_TRIGGER_ENABLE_FALLING_EDGE                     (1u)
#define BITM_FLEXIO_CONTROL_EXTERNAL_TRIGGER_ENABLE_FALLING_EDGE            (0x00000020u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG                                     (6u)
#define BITL_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG                                     (1u)
#define BITM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG                            (0x00000040u)
// Enums
#define ENUM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG_DISABLE                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG_ENABLE                     (0x00000001u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_TX_FIFO_RESET                                            (8u)
#define BITL_FLEXIO_CONTROL_TX_FIFO_RESET                                            (1u)
#define BITM_FLEXIO_CONTROL_TX_FIFO_RESET                                   (0x00000100u)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_RX_FIFO_RESET                                            (9u)
#define BITL_FLEXIO_CONTROL_RX_FIFO_RESET                                            (1u)
#define BITM_FLEXIO_CONTROL_RX_FIFO_RESET                                   (0x00000200u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SOFT_RESET                                              (10u)
#define BITL_FLEXIO_CONTROL_SOFT_RESET                                               (4u)
#define BITM_FLEXIO_CONTROL_SOFT_RESET                                      (0x00003C00u)

// Bit field: complete_transactions
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_COMPLETE_TRANSACTIONS                                   (14u)
#define BITL_FLEXIO_CONTROL_COMPLETE_TRANSACTIONS                                    (1u)
#define BITM_FLEXIO_CONTROL_COMPLETE_TRANSACTIONS                           (0x00004000u)
// Enums
#define ENUM_FLEXIO_CONTROL_COMPLETE_TRANSACTIONS_COMPLETE                  (0x00000001u)
#define ENUM_FLEXIO_CONTROL_COMPLETE_TRANSACTIONS_INCOMPLETE                (0x00000000u)

// Bit field: num_packed_transactions
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_NUM_PACKED_TRANSACTIONS                                 (15u)
#define BITL_FLEXIO_CONTROL_NUM_PACKED_TRANSACTIONS                                 (16u)
#define BITM_FLEXIO_CONTROL_NUM_PACKED_TRANSACTIONS                         (0x7FFF8000u)

// Bit field: ram_parity_halt_enable
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_RAM_PARITY_HALT_ENABLE                                  (31u)
#define BITL_FLEXIO_CONTROL_RAM_PARITY_HALT_ENABLE                                   (1u)
#define BITM_FLEXIO_CONTROL_RAM_PARITY_HALT_ENABLE                          (0x80000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_RAM_PARITY_HALT_ENABLE_HALT_ON_ERROR            (0x00000001u)
#define ENUM_FLEXIO_CONTROL_RAM_PARITY_HALT_ENABLE_IGNORE                   (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : NUM_BYTES
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_NUM_BYTES_ADDR                                                   (0x0004u)
#define FLEXIO_NUM_BYTES_NW_ADDR                                                (0x0004u)
#define FLEXIO_NUM_BYTES_RESET_VAL                                          (0x00000000u)
#define FLEXIO_NUM_BYTES_MMS                                                         (0u)
#define FLEXIO_NUM_BYTES_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_rd_bytes             | Counts the number of octets that  | 0x00000000  |
// |       |                          | have been read from the           |             |
// |       |                          | interface to the fifo.            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | num_wr_bytes             | Counts the number of octets that  | 0x00000000  |
// |       |                          | have been read from the fifo to   |             |
// |       |                          | the interface.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_rd_bytes
// Bit position, length and mask
#define BITP_FLEXIO_NUM_BYTES_NUM_RD_BYTES                                           (0u)
#define BITL_FLEXIO_NUM_BYTES_NUM_RD_BYTES                                          (16u)
#define BITM_FLEXIO_NUM_BYTES_NUM_RD_BYTES                                  (0x0000FFFFu)

// Bit field: num_wr_bytes
// Bit position, length and mask
#define BITP_FLEXIO_NUM_BYTES_NUM_WR_BYTES                                          (16u)
#define BITL_FLEXIO_NUM_BYTES_NUM_WR_BYTES                                          (16u)
#define BITM_FLEXIO_NUM_BYTES_NUM_WR_BYTES                                  (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : NUM_TRANSACTIONS
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_NUM_TRANSACTIONS_ADDR                                            (0x0008u)
#define FLEXIO_NUM_TRANSACTIONS_NW_ADDR                                         (0x0008u)
#define FLEXIO_NUM_TRANSACTIONS_RESET_VAL                                   (0x00000000u)
#define FLEXIO_NUM_TRANSACTIONS_MMS                                                  (0u)
#define FLEXIO_NUM_TRANSACTIONS_VOL_MASK                                    (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_transactions         | Counts the number of              | 0x00000000  |
// |       |                          | transactions that have been       |             |
// |       |                          | executed.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_transactions
// Bit position, length and mask
#define BITP_FLEXIO_NUM_TRANSACTIONS_NUM_TRANSACTIONS                                (0u)
#define BITL_FLEXIO_NUM_TRANSACTIONS_NUM_TRANSACTIONS                               (16u)
#define BITM_FLEXIO_NUM_TRANSACTIONS_NUM_TRANSACTIONS                       (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : LENGTHS
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_LENGTHS_ADDR                                                     (0x000Cu)
#define FLEXIO_LENGTHS_NW_ADDR                                                  (0x000Cu)
#define FLEXIO_LENGTHS_RESET_VAL                                            (0x00000000u)
#define FLEXIO_LENGTHS_MMS                                                           (0u)
#define FLEXIO_LENGTHS_VOL_MASK                                             (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | manual_wr_bytes          | Default configuration used for    | 0x00000000  |
// |       |                          | the optional field                |             |
// |       |                          | "manual_wr_bytes" in the          |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | manual_rd_bytes          | Default configuration used for    | 0x00000000  |
// |       |                          | the optional field                |             |
// |       |                          | "manual_rd_bytes" in the          |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: manual_wr_bytes
// Bit position, length and mask
#define BITP_FLEXIO_LENGTHS_MANUAL_WR_BYTES                                          (0u)
#define BITL_FLEXIO_LENGTHS_MANUAL_WR_BYTES                                         (16u)
#define BITM_FLEXIO_LENGTHS_MANUAL_WR_BYTES                                 (0x0000FFFFu)

// Bit field: manual_rd_bytes
// Bit position, length and mask
#define BITP_FLEXIO_LENGTHS_MANUAL_RD_BYTES                                         (16u)
#define BITL_FLEXIO_LENGTHS_MANUAL_RD_BYTES                                         (16u)
#define BITM_FLEXIO_LENGTHS_MANUAL_RD_BYTES                                 (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_2
// Register Addr        : 0x0100u
// Register NW Addr     : 0x0100u
// Register Reset Value : 0x00000104u
// Register MMS         : 0u

#define FLEXIO_CONTROL_2_ADDR                                                   (0x0100u)
#define FLEXIO_CONTROL_2_NW_ADDR                                                (0x0100u)
#define FLEXIO_CONTROL_2_RESET_VAL                                          (0x00000104u)
#define FLEXIO_CONTROL_2_MMS                                                         (0u)
#define FLEXIO_CONTROL_2_VOL_MASK                                           (0x0000010Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | ram_access_enable        | Enables or disables the access    | 0x00000000  |
// |       |                          | to the state machine RAM          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | program_valid            | Indicates if the program was      | 0x00000000  |
// |       |                          | successfully written              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | ram_refresh_enable       | Enables or disables the refresh   | 0x00000001  |
// |       |                          | of the instruction RAM. NOTE:     |             |
// |       |                          | Programs are dependent on this    |             |
// |       |                          | setting. It is not possible to    |             |
// |       |                          | change this bit without updating  |             |
// |       |                          | the program as the clock divider  |             |
// |       |                          | configuration will also change.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | resynchronization_require| Executes the resynchronization    | 0x00000000  | 
// |       |                          | procedure. Please, check the      |             |
// |       |                          | user guide of the interface for   |             |
// |       |                          | more information about this       |             |
// |       |                          | procedure.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | fifo_tx_router_fixed_sm  | Sets the fifo TX router           | 0x00000001  |
// |       |                          | configuration                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: ram_access_enable
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE                                      (0u)
#define BITL_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE                                      (1u)
#define BITM_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE                             (0x00000001u)
// Enums
#define ENUM_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE_RAM_ACCESS_ACTIVE           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE_RAM_ACCESS_INACTIVE         (0x00000000u)

// Bit field: program_valid
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_2_PROGRAM_VALID                                          (1u)
#define BITL_FLEXIO_CONTROL_2_PROGRAM_VALID                                          (1u)
#define BITM_FLEXIO_CONTROL_2_PROGRAM_VALID                                 (0x00000002u)

// Bit field: ram_refresh_enable
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_2_RAM_REFRESH_ENABLE                                     (2u)
#define BITL_FLEXIO_CONTROL_2_RAM_REFRESH_ENABLE                                     (1u)
#define BITM_FLEXIO_CONTROL_2_RAM_REFRESH_ENABLE                            (0x00000004u)
// Enums
#define ENUM_FLEXIO_CONTROL_2_RAM_REFRESH_ENABLE_WITH_REFRESH               (0x00000001u)
#define ENUM_FLEXIO_CONTROL_2_RAM_REFRESH_ENABLE_WITHOUT_REFRESH            (0x00000000u)

// Bit field: resynchronization_required
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_2_RESYNCHRONIZATION_REQUIRED                             (3u)
#define BITL_FLEXIO_CONTROL_2_RESYNCHRONIZATION_REQUIRED                             (1u)
#define BITM_FLEXIO_CONTROL_2_RESYNCHRONIZATION_REQUIRED                    (0x00000008u)

// Bit field: fifo_tx_router_fixed_sm
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_2_FIFO_TX_ROUTER_FIXED_SM                                (8u)
#define BITL_FLEXIO_CONTROL_2_FIFO_TX_ROUTER_FIXED_SM                                (1u)
#define BITM_FLEXIO_CONTROL_2_FIFO_TX_ROUTER_FIXED_SM                       (0x00000100u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM0
// Register Addr        : 0x0104u
// Register NW Addr     : 0x0104u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM0_ADDR                                                 (0x0104u)
#define FLEXIO_CONTROL_SM0_NW_ADDR                                              (0x0104u)
#define FLEXIO_CONTROL_SM0_RESET_VAL                                        (0x00000000u)
#define FLEXIO_CONTROL_SM0_MMS                                                       (0u)
#define FLEXIO_CONTROL_SM0_VOL_MASK                                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | pinout_basepin0          | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinout bus for state      |             |
// |       |                          | machine 0. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 05-03 | pinin_basepin0           | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinin bus for state       |             |
// |       |                          | machine 0. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-06 | sideset_basepin0         | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the side set bus for state    |             |
// |       |                          | machine 0. Each increment of      |             |
// |       |                          | this value will rotate the pin    |             |
// |       |                          | indexes by one pin.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16-09 | program_start0           | Address in which the program      | 0x00000000  |
// |       |                          | starts for state machine 0.       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24-17 | program_size0            | Size of the program in octets     | 0x00000000  |
// |       |                          | for state machine 0.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | delay_sideset_mode0      | Configures the usage of the       | 0x00000000  |
// |       |                          | instruction delay_sidesetting     |             |
// |       |                          | mode for state machine 0.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28-26 | delay_sideset_bits0      | Depending on configuration, can   | 0x00000000  |
// |       |                          | set the number of bits that will  |             |
// |       |                          | be used for each mode for state   |             |
// |       |                          | machine 0                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-29 | jmp_basepin0             | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the BPEO instruction for      |             |
// |       |                          | state machine 0. Value of this    |             |
// |       |                          | field offsets the pin_index       |             |
// |       |                          | parameter of the BPEO             |             |
// |       |                          | instruction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pinout_basepin0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0                                      (0u)
#define BITL_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0                                      (3u)
#define BITM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0                             (0x00000007u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF0                      (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF1                      (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF2                      (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF3                      (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF4                      (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF5                      (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF6                      (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0_SA_IF7                      (0x00000007u)

// Bit field: pinin_basepin0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_PININ_BASEPIN0                                       (3u)
#define BITL_FLEXIO_CONTROL_SM0_PININ_BASEPIN0                                       (3u)
#define BITM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0                              (0x00000038u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF0                       (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF1                       (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF2                       (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF3                       (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF4                       (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF5                       (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF6                       (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_PININ_BASEPIN0_SA_IF7                       (0x00000007u)

// Bit field: sideset_basepin0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0                                     (6u)
#define BITL_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0                                     (3u)
#define BITM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0                            (0x000001C0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF0                     (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF1                     (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF2                     (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF3                     (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF4                     (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF5                     (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF6                     (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0_SA_IF7                     (0x00000007u)

// Bit field: program_start0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_PROGRAM_START0                                       (9u)
#define BITL_FLEXIO_CONTROL_SM0_PROGRAM_START0                                       (8u)
#define BITM_FLEXIO_CONTROL_SM0_PROGRAM_START0                              (0x0001FE00u)

// Bit field: program_size0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_PROGRAM_SIZE0                                       (17u)
#define BITL_FLEXIO_CONTROL_SM0_PROGRAM_SIZE0                                        (8u)
#define BITM_FLEXIO_CONTROL_SM0_PROGRAM_SIZE0                               (0x01FE0000u)

// Bit field: delay_sideset_mode0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_DELAY_SIDESET_MODE0                                 (25u)
#define BITL_FLEXIO_CONTROL_SM0_DELAY_SIDESET_MODE0                                  (1u)
#define BITM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_MODE0                         (0x02000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_MODE0_SINGLEMODE              (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_MODE0_DUALMODE                (0x00000001u)

// Bit field: delay_sideset_bits0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0                                 (26u)
#define BITL_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0                                  (3u)
#define BITM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0                         (0x1C000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0_BITS_0                  (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0_BITS_1                  (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0_BITS_2                  (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0_BITS_3                  (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0_BITS_4                  (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0_BITS_5                  (0x00000005u)

// Bit field: jmp_basepin0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_JMP_BASEPIN0                                        (29u)
#define BITL_FLEXIO_CONTROL_SM0_JMP_BASEPIN0                                         (3u)
#define BITM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0                                (0xE0000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF0                         (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF1                         (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF2                         (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF3                         (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF4                         (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF5                         (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF6                         (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_JMP_BASEPIN0_SA_IF7                         (0x00000007u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM1
// Register Addr        : 0x0108u
// Register NW Addr     : 0x0108u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM1_ADDR                                                 (0x0108u)
#define FLEXIO_CONTROL_SM1_NW_ADDR                                              (0x0108u)
#define FLEXIO_CONTROL_SM1_RESET_VAL                                        (0x00000000u)
#define FLEXIO_CONTROL_SM1_MMS                                                       (0u)
#define FLEXIO_CONTROL_SM1_VOL_MASK                                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | pinout_basepin1          | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinout bus for state      |             |
// |       |                          | machine 1. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 05-03 | pinin_basepin1           | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinin bus for state       |             |
// |       |                          | machine 1. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-06 | sideset_basepin1         | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the side set bus for state    |             |
// |       |                          | machine 1. Each increment of      |             |
// |       |                          | this value will rotate the pin    |             |
// |       |                          | indexes by one pin.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16-09 | program_start1           | Address in which the program      | 0x00000000  |
// |       |                          | starts for state machine 1.       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24-17 | program_size1            | Size of the program in octets     | 0x00000000  |
// |       |                          | for state machine 1.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | delay_sideset_mode1      | Configures the usage of the       | 0x00000000  |
// |       |                          | instruction delay_sidesetting     |             |
// |       |                          | mode for state machine 1.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28-26 | delay_sideset_bits1      | Depending on configuration, can   | 0x00000000  |
// |       |                          | set the number of bits that will  |             |
// |       |                          | be used for each mode for state   |             |
// |       |                          | machine 1                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-29 | jmp_basepin1             | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the BPEO instruction for      |             |
// |       |                          | state machine 1. Value of this    |             |
// |       |                          | field offsets the pin_index       |             |
// |       |                          | parameter of the BPEO             |             |
// |       |                          | instruction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pinout_basepin1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1                                      (0u)
#define BITL_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1                                      (3u)
#define BITM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1                             (0x00000007u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF0                      (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF1                      (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF2                      (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF3                      (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF4                      (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF5                      (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF6                      (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_PINOUT_BASEPIN1_SA_IF7                      (0x00000007u)

// Bit field: pinin_basepin1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_PININ_BASEPIN1                                       (3u)
#define BITL_FLEXIO_CONTROL_SM1_PININ_BASEPIN1                                       (3u)
#define BITM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1                              (0x00000038u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF0                       (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF1                       (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF2                       (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF3                       (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF4                       (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF5                       (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF6                       (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_PININ_BASEPIN1_SA_IF7                       (0x00000007u)

// Bit field: sideset_basepin1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1                                     (6u)
#define BITL_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1                                     (3u)
#define BITM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1                            (0x000001C0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF0                     (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF1                     (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF2                     (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF3                     (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF4                     (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF5                     (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF6                     (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_SIDESET_BASEPIN1_SA_IF7                     (0x00000007u)

// Bit field: program_start1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_PROGRAM_START1                                       (9u)
#define BITL_FLEXIO_CONTROL_SM1_PROGRAM_START1                                       (8u)
#define BITM_FLEXIO_CONTROL_SM1_PROGRAM_START1                              (0x0001FE00u)

// Bit field: program_size1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_PROGRAM_SIZE1                                       (17u)
#define BITL_FLEXIO_CONTROL_SM1_PROGRAM_SIZE1                                        (8u)
#define BITM_FLEXIO_CONTROL_SM1_PROGRAM_SIZE1                               (0x01FE0000u)

// Bit field: delay_sideset_mode1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_DELAY_SIDESET_MODE1                                 (25u)
#define BITL_FLEXIO_CONTROL_SM1_DELAY_SIDESET_MODE1                                  (1u)
#define BITM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_MODE1                         (0x02000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_MODE1_SINGLEMODE              (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_MODE1_DUALMODE                (0x00000001u)

// Bit field: delay_sideset_bits1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1                                 (26u)
#define BITL_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1                                  (3u)
#define BITM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1                         (0x1C000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1_BITS_0                  (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1_BITS_1                  (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1_BITS_2                  (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1_BITS_3                  (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1_BITS_4                  (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_DELAY_SIDESET_BITS1_BITS_5                  (0x00000005u)

// Bit field: jmp_basepin1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_JMP_BASEPIN1                                        (29u)
#define BITL_FLEXIO_CONTROL_SM1_JMP_BASEPIN1                                         (3u)
#define BITM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1                                (0xE0000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF0                         (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF1                         (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF2                         (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF3                         (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF4                         (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF5                         (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF6                         (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_JMP_BASEPIN1_SA_IF7                         (0x00000007u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM2
// Register Addr        : 0x010Cu
// Register NW Addr     : 0x010Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM2_ADDR                                                 (0x010Cu)
#define FLEXIO_CONTROL_SM2_NW_ADDR                                              (0x010Cu)
#define FLEXIO_CONTROL_SM2_RESET_VAL                                        (0x00000000u)
#define FLEXIO_CONTROL_SM2_MMS                                                       (0u)
#define FLEXIO_CONTROL_SM2_VOL_MASK                                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | pinout_basepin2          | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinout bus for state      |             |
// |       |                          | machine 2. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 05-03 | pinin_basepin2           | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinin bus for state       |             |
// |       |                          | machine 2. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-06 | sideset_basepin2         | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the side set bus for state    |             |
// |       |                          | machine 2. Each increment of      |             |
// |       |                          | this value will rotate the pin    |             |
// |       |                          | indexes by one pin.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16-09 | program_start2           | Address in which the program      | 0x00000000  |
// |       |                          | starts for state machine 2.       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24-17 | program_size2            | Size of the program in octets     | 0x00000000  |
// |       |                          | for state machine 2.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | delay_sideset_mode2      | Configures the usage of the       | 0x00000000  |
// |       |                          | instruction delay_sidesetting     |             |
// |       |                          | mode for state machine 2.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28-26 | delay_sideset_bits2      | Depending on configuration, can   | 0x00000000  |
// |       |                          | set the number of bits that will  |             |
// |       |                          | be used for each mode for state   |             |
// |       |                          | machine 2                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-29 | jmp_basepin2             | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the BPEO instruction for      |             |
// |       |                          | state machine 2. Value of this    |             |
// |       |                          | field offsets the pin_index       |             |
// |       |                          | parameter of the BPEO             |             |
// |       |                          | instruction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pinout_basepin2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2                                      (0u)
#define BITL_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2                                      (3u)
#define BITM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2                             (0x00000007u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF0                      (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF1                      (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF2                      (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF3                      (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF4                      (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF5                      (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF6                      (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_PINOUT_BASEPIN2_SA_IF7                      (0x00000007u)

// Bit field: pinin_basepin2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_PININ_BASEPIN2                                       (3u)
#define BITL_FLEXIO_CONTROL_SM2_PININ_BASEPIN2                                       (3u)
#define BITM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2                              (0x00000038u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF0                       (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF1                       (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF2                       (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF3                       (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF4                       (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF5                       (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF6                       (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_PININ_BASEPIN2_SA_IF7                       (0x00000007u)

// Bit field: sideset_basepin2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2                                     (6u)
#define BITL_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2                                     (3u)
#define BITM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2                            (0x000001C0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF0                     (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF1                     (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF2                     (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF3                     (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF4                     (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF5                     (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF6                     (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_SIDESET_BASEPIN2_SA_IF7                     (0x00000007u)

// Bit field: program_start2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_PROGRAM_START2                                       (9u)
#define BITL_FLEXIO_CONTROL_SM2_PROGRAM_START2                                       (8u)
#define BITM_FLEXIO_CONTROL_SM2_PROGRAM_START2                              (0x0001FE00u)

// Bit field: program_size2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_PROGRAM_SIZE2                                       (17u)
#define BITL_FLEXIO_CONTROL_SM2_PROGRAM_SIZE2                                        (8u)
#define BITM_FLEXIO_CONTROL_SM2_PROGRAM_SIZE2                               (0x01FE0000u)

// Bit field: delay_sideset_mode2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_DELAY_SIDESET_MODE2                                 (25u)
#define BITL_FLEXIO_CONTROL_SM2_DELAY_SIDESET_MODE2                                  (1u)
#define BITM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_MODE2                         (0x02000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_MODE2_SINGLEMODE              (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_MODE2_DUALMODE                (0x00000001u)

// Bit field: delay_sideset_bits2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2                                 (26u)
#define BITL_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2                                  (3u)
#define BITM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2                         (0x1C000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2_BITS_0                  (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2_BITS_1                  (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2_BITS_2                  (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2_BITS_3                  (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2_BITS_4                  (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_DELAY_SIDESET_BITS2_BITS_5                  (0x00000005u)

// Bit field: jmp_basepin2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_JMP_BASEPIN2                                        (29u)
#define BITL_FLEXIO_CONTROL_SM2_JMP_BASEPIN2                                         (3u)
#define BITM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2                                (0xE0000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF0                         (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF1                         (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF2                         (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF3                         (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF4                         (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF5                         (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF6                         (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_JMP_BASEPIN2_SA_IF7                         (0x00000007u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM3
// Register Addr        : 0x0110u
// Register NW Addr     : 0x0110u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM3_ADDR                                                 (0x0110u)
#define FLEXIO_CONTROL_SM3_NW_ADDR                                              (0x0110u)
#define FLEXIO_CONTROL_SM3_RESET_VAL                                        (0x00000000u)
#define FLEXIO_CONTROL_SM3_MMS                                                       (0u)
#define FLEXIO_CONTROL_SM3_VOL_MASK                                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | pinout_basepin3          | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinout bus for state      |             |
// |       |                          | machine 3. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 05-03 | pinin_basepin3           | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the pinin bus for state       |             |
// |       |                          | machine 3. Increments of this     |             |
// |       |                          | value rotates the pin indexes     |             |
// |       |                          | one pin.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-06 | sideset_basepin3         | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the side set bus for state    |             |
// |       |                          | machine 3. Each increment of      |             |
// |       |                          | this value will rotate the pin    |             |
// |       |                          | indexes by one pin.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16-09 | program_start3           | Address in which the program      | 0x00000000  |
// |       |                          | starts for state machine 3.       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24-17 | program_size3            | Size of the program in octets     | 0x00000000  |
// |       |                          | for state machine 3.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | delay_sideset_mode3      | Configures the usage of the       | 0x00000000  |
// |       |                          | instruction delay_sidesetting     |             |
// |       |                          | mode for state machine 3.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28-26 | delay_sideset_bits3      | Depending on configuration, can   | 0x00000000  |
// |       |                          | set the number of bits that will  |             |
// |       |                          | be used for each mode for state   |             |
// |       |                          | machine 3                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-29 | jmp_basepin3             | Sets the base pin configuration   | 0x00000000  |
// |       |                          | for the BPEO instruction for      |             |
// |       |                          | state machine 3. Value of this    |             |
// |       |                          | field offsets the pin_index       |             |
// |       |                          | parameter of the BPEO             |             |
// |       |                          | instruction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pinout_basepin3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3                                      (0u)
#define BITL_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3                                      (3u)
#define BITM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3                             (0x00000007u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF0                      (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF1                      (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF2                      (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF3                      (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF4                      (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF5                      (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF6                      (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_PINOUT_BASEPIN3_SA_IF7                      (0x00000007u)

// Bit field: pinin_basepin3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_PININ_BASEPIN3                                       (3u)
#define BITL_FLEXIO_CONTROL_SM3_PININ_BASEPIN3                                       (3u)
#define BITM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3                              (0x00000038u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF0                       (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF1                       (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF2                       (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF3                       (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF4                       (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF5                       (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF6                       (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_PININ_BASEPIN3_SA_IF7                       (0x00000007u)

// Bit field: sideset_basepin3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3                                     (6u)
#define BITL_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3                                     (3u)
#define BITM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3                            (0x000001C0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF0                     (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF1                     (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF2                     (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF3                     (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF4                     (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF5                     (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF6                     (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_SIDESET_BASEPIN3_SA_IF7                     (0x00000007u)

// Bit field: program_start3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_PROGRAM_START3                                       (9u)
#define BITL_FLEXIO_CONTROL_SM3_PROGRAM_START3                                       (8u)
#define BITM_FLEXIO_CONTROL_SM3_PROGRAM_START3                              (0x0001FE00u)

// Bit field: program_size3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_PROGRAM_SIZE3                                       (17u)
#define BITL_FLEXIO_CONTROL_SM3_PROGRAM_SIZE3                                        (8u)
#define BITM_FLEXIO_CONTROL_SM3_PROGRAM_SIZE3                               (0x01FE0000u)

// Bit field: delay_sideset_mode3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_DELAY_SIDESET_MODE3                                 (25u)
#define BITL_FLEXIO_CONTROL_SM3_DELAY_SIDESET_MODE3                                  (1u)
#define BITM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_MODE3                         (0x02000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_MODE3_SINGLEMODE              (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_MODE3_DUALMODE                (0x00000001u)

// Bit field: delay_sideset_bits3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3                                 (26u)
#define BITL_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3                                  (3u)
#define BITM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3                         (0x1C000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3_BITS_0                  (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3_BITS_1                  (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3_BITS_2                  (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3_BITS_3                  (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3_BITS_4                  (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_DELAY_SIDESET_BITS3_BITS_5                  (0x00000005u)

// Bit field: jmp_basepin3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_JMP_BASEPIN3                                        (29u)
#define BITL_FLEXIO_CONTROL_SM3_JMP_BASEPIN3                                         (3u)
#define BITM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3                                (0xE0000000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF0                         (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF1                         (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF2                         (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF3                         (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF4                         (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF5                         (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF6                         (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_JMP_BASEPIN3_SA_IF7                         (0x00000007u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM0_1
// Register Addr        : 0x0114u
// Register NW Addr     : 0x0114u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM0_1_ADDR                                               (0x0114u)
#define FLEXIO_CONTROL_SM0_1_NW_ADDR                                            (0x0114u)
#define FLEXIO_CONTROL_SM0_1_RESET_VAL                                      (0x00000000u)
#define FLEXIO_CONTROL_SM0_1_MMS                                                     (0u)
#define FLEXIO_CONTROL_SM0_1_VOL_MASK                                       (0x0033FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | autopull_threshold0      | If the number of bits shifted     | 0x00000000  |
// |       |                          | from the OSR reaches a threshold  |             |
// |       |                          | (greater than or equal) and       |             |
// |       |                          | autopull is enabled, a pull       |             |
// |       |                          | operation will be performed for   |             |
// |       |                          | state machine 0.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | autopush_threshold0      | If the total number of shifted    | 0x00000000  |
// |       |                          | bits into the ISR reaches a       |             |
// |       |                          | threshold (greater than or        |             |
// |       |                          | equal) and autopush is enabled,   |             |
// |       |                          | a push operation will be          |             |
// |       |                          | performed for state machine 0.    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | pull_threshold0          | Sets the pull threshold for       | 0x00000000  |
// |       |                          | state machine 0                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | push_threshold0          | Sets the push threshold for       | 0x00000000  |
// |       |                          | state machine 0                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | osr_shift_direction0     | Sets the output shift direction   | 0x00000000  |
// |       |                          | for state machine 0.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | isr_shift_direction0     | Configures the input shift        | 0x00000000  |
// |       |                          | direction for state machine 0     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | autopush0                | Sets the autopush level for       | 0x00000000  |
// |       |                          | state machine 0                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | autopull0                | Indicates if the state machine 0  | 0x00000000  |
// |       |                          | will perform autopull.            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: autopull_threshold0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0                                (0u)
#define BITL_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0                                (4u)
#define BITM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0                       (0x0000000Fu)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0_THRESHOLD_7           (0x00000007u)

// Bit field: autopush_threshold0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0                                (4u)
#define BITL_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0                                (4u)
#define BITM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0                       (0x000000F0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0_THRESHOLD_7           (0x00000007u)

// Bit field: pull_threshold0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0                                    (8u)
#define BITL_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0                                    (4u)
#define BITM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0                           (0x00000F00u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0_BITS_7                    (0x00000007u)

// Bit field: push_threshold0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0                                   (12u)
#define BITL_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0                                    (4u)
#define BITM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0                           (0x0000F000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0_BITS_7                    (0x00000007u)

// Bit field: osr_shift_direction0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_OSR_SHIFT_DIRECTION0                              (16u)
#define BITL_FLEXIO_CONTROL_SM0_1_OSR_SHIFT_DIRECTION0                               (1u)
#define BITM_FLEXIO_CONTROL_SM0_1_OSR_SHIFT_DIRECTION0                      (0x00010000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_OSR_SHIFT_DIRECTION0_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_1_OSR_SHIFT_DIRECTION0_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: isr_shift_direction0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_ISR_SHIFT_DIRECTION0                              (17u)
#define BITL_FLEXIO_CONTROL_SM0_1_ISR_SHIFT_DIRECTION0                               (1u)
#define BITM_FLEXIO_CONTROL_SM0_1_ISR_SHIFT_DIRECTION0                      (0x00020000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_ISR_SHIFT_DIRECTION0_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM0_1_ISR_SHIFT_DIRECTION0_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: autopush0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_AUTOPUSH0                                         (20u)
#define BITL_FLEXIO_CONTROL_SM0_1_AUTOPUSH0                                          (1u)
#define BITM_FLEXIO_CONTROL_SM0_1_AUTOPUSH0                                 (0x00100000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH0_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPUSH0_ENABLED                         (0x00000001u)

// Bit field: autopull0
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM0_1_AUTOPULL0                                         (21u)
#define BITL_FLEXIO_CONTROL_SM0_1_AUTOPULL0                                          (1u)
#define BITM_FLEXIO_CONTROL_SM0_1_AUTOPULL0                                 (0x00200000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL0_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM0_1_AUTOPULL0_ENABLED                         (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM1_1
// Register Addr        : 0x0118u
// Register NW Addr     : 0x0118u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM1_1_ADDR                                               (0x0118u)
#define FLEXIO_CONTROL_SM1_1_NW_ADDR                                            (0x0118u)
#define FLEXIO_CONTROL_SM1_1_RESET_VAL                                      (0x00000000u)
#define FLEXIO_CONTROL_SM1_1_MMS                                                     (0u)
#define FLEXIO_CONTROL_SM1_1_VOL_MASK                                       (0x0033FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | autopull_threshold1      | If the number of bits shifted     | 0x00000000  |
// |       |                          | from the OSR reaches a threshold  |             |
// |       |                          | (greater than or equal)  and      |             |
// |       |                          | autopull is enabled, a pull       |             |
// |       |                          | operation will be performed for   |             |
// |       |                          | state machine 1.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | autopush_threshold1      | If the total number of shifted    | 0x00000000  |
// |       |                          | bits into the ISR reaches a       |             |
// |       |                          | threshold (greater than or        |             |
// |       |                          | equal) and autopush is enabled,   |             |
// |       |                          | a push operation will be          |             |
// |       |                          | performed for state machine 1.    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | pull_threshold1          | Sets the pull threshold for       | 0x00000000  |
// |       |                          | state machine 1                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | push_threshold1          | Sets the push threshold for       | 0x00000000  |
// |       |                          | state machine 1                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | osr_shift_direction1     | Sets the output shift direction   | 0x00000000  |
// |       |                          | for state machine 1.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | isr_shift_direction1     | Configures the input shift        | 0x00000000  |
// |       |                          | direction for state machine 1     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | autopush1                | Sets the autopush level for       | 0x00000000  |
// |       |                          | state machine 1                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | autopull1                | Indicates if the state machine 1  | 0x00000000  |
// |       |                          | will perform autopull.            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: autopull_threshold1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1                                (0u)
#define BITL_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1                                (4u)
#define BITM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1                       (0x0000000Fu)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL_THRESHOLD1_THRESHOLD_7           (0x00000007u)

// Bit field: autopush_threshold1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1                                (4u)
#define BITL_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1                                (4u)
#define BITM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1                       (0x000000F0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH_THRESHOLD1_THRESHOLD_7           (0x00000007u)

// Bit field: pull_threshold1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1                                    (8u)
#define BITL_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1                                    (4u)
#define BITM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1                           (0x00000F00u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PULL_THRESHOLD1_BITS_7                    (0x00000007u)

// Bit field: push_threshold1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1                                   (12u)
#define BITL_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1                                    (4u)
#define BITM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1                           (0x0000F000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM1_1_PUSH_THRESHOLD1_BITS_7                    (0x00000007u)

// Bit field: osr_shift_direction1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_OSR_SHIFT_DIRECTION1                              (16u)
#define BITL_FLEXIO_CONTROL_SM1_1_OSR_SHIFT_DIRECTION1                               (1u)
#define BITM_FLEXIO_CONTROL_SM1_1_OSR_SHIFT_DIRECTION1                      (0x00010000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_OSR_SHIFT_DIRECTION1_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_1_OSR_SHIFT_DIRECTION1_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: isr_shift_direction1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_ISR_SHIFT_DIRECTION1                              (17u)
#define BITL_FLEXIO_CONTROL_SM1_1_ISR_SHIFT_DIRECTION1                               (1u)
#define BITM_FLEXIO_CONTROL_SM1_1_ISR_SHIFT_DIRECTION1                      (0x00020000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_ISR_SHIFT_DIRECTION1_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM1_1_ISR_SHIFT_DIRECTION1_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: autopush1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_AUTOPUSH1                                         (20u)
#define BITL_FLEXIO_CONTROL_SM1_1_AUTOPUSH1                                          (1u)
#define BITM_FLEXIO_CONTROL_SM1_1_AUTOPUSH1                                 (0x00100000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH1_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPUSH1_ENABLED                         (0x00000001u)

// Bit field: autopull1
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM1_1_AUTOPULL1                                         (21u)
#define BITL_FLEXIO_CONTROL_SM1_1_AUTOPULL1                                          (1u)
#define BITM_FLEXIO_CONTROL_SM1_1_AUTOPULL1                                 (0x00200000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL1_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM1_1_AUTOPULL1_ENABLED                         (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM2_1
// Register Addr        : 0x011Cu
// Register NW Addr     : 0x011Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM2_1_ADDR                                               (0x011Cu)
#define FLEXIO_CONTROL_SM2_1_NW_ADDR                                            (0x011Cu)
#define FLEXIO_CONTROL_SM2_1_RESET_VAL                                      (0x00000000u)
#define FLEXIO_CONTROL_SM2_1_MMS                                                     (0u)
#define FLEXIO_CONTROL_SM2_1_VOL_MASK                                       (0x0033FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | autopull_threshold2      | If the number of bits shifted     | 0x00000000  |
// |       |                          | from the OSR reaches a threshold  |             |
// |       |                          | (greater than or equal)  and      |             |
// |       |                          | autopull is enabled, a pull       |             |
// |       |                          | operation will be performed for   |             |
// |       |                          | state machine 2.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | autopush_threshold2      | If the total number of shifted    | 0x00000000  |
// |       |                          | bits into the ISR reaches a       |             |
// |       |                          | threshold (greater than or        |             |
// |       |                          | equal) and autopush is enabled,   |             |
// |       |                          | a push operation will be          |             |
// |       |                          | performed for state machine 2.    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | pull_threshold2          | Sets the pull threshold for       | 0x00000000  |
// |       |                          | state machine 2                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | push_threshold2          | Sets the push threshold for       | 0x00000000  |
// |       |                          | state machine 2                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | osr_shift_direction2     | Sets the output shift direction   | 0x00000000  |
// |       |                          | for state machine 2.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | isr_shift_direction2     | Configures the input shift        | 0x00000000  |
// |       |                          | direction for state machine 2     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | autopush2                | Sets the autopush level for       | 0x00000000  |
// |       |                          | state machine 2                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | autopull2                | Indicates if the state machine 2  | 0x00000000  |
// |       |                          | will perform autopull.            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: autopull_threshold2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2                                (0u)
#define BITL_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2                                (4u)
#define BITM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2                       (0x0000000Fu)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL_THRESHOLD2_THRESHOLD_7           (0x00000007u)

// Bit field: autopush_threshold2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2                                (4u)
#define BITL_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2                                (4u)
#define BITM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2                       (0x000000F0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH_THRESHOLD2_THRESHOLD_7           (0x00000007u)

// Bit field: pull_threshold2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2                                    (8u)
#define BITL_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2                                    (4u)
#define BITM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2                           (0x00000F00u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PULL_THRESHOLD2_BITS_7                    (0x00000007u)

// Bit field: push_threshold2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2                                   (12u)
#define BITL_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2                                    (4u)
#define BITM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2                           (0x0000F000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM2_1_PUSH_THRESHOLD2_BITS_7                    (0x00000007u)

// Bit field: osr_shift_direction2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_OSR_SHIFT_DIRECTION2                              (16u)
#define BITL_FLEXIO_CONTROL_SM2_1_OSR_SHIFT_DIRECTION2                               (1u)
#define BITM_FLEXIO_CONTROL_SM2_1_OSR_SHIFT_DIRECTION2                      (0x00010000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_OSR_SHIFT_DIRECTION2_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_1_OSR_SHIFT_DIRECTION2_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: isr_shift_direction2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_ISR_SHIFT_DIRECTION2                              (17u)
#define BITL_FLEXIO_CONTROL_SM2_1_ISR_SHIFT_DIRECTION2                               (1u)
#define BITM_FLEXIO_CONTROL_SM2_1_ISR_SHIFT_DIRECTION2                      (0x00020000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_ISR_SHIFT_DIRECTION2_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM2_1_ISR_SHIFT_DIRECTION2_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: autopush2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_AUTOPUSH2                                         (20u)
#define BITL_FLEXIO_CONTROL_SM2_1_AUTOPUSH2                                          (1u)
#define BITM_FLEXIO_CONTROL_SM2_1_AUTOPUSH2                                 (0x00100000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH2_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPUSH2_ENABLED                         (0x00000001u)

// Bit field: autopull2
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM2_1_AUTOPULL2                                         (21u)
#define BITL_FLEXIO_CONTROL_SM2_1_AUTOPULL2                                          (1u)
#define BITM_FLEXIO_CONTROL_SM2_1_AUTOPULL2                                 (0x00200000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL2_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM2_1_AUTOPULL2_ENABLED                         (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_SM3_1
// Register Addr        : 0x0120u
// Register NW Addr     : 0x0120u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_CONTROL_SM3_1_ADDR                                               (0x0120u)
#define FLEXIO_CONTROL_SM3_1_NW_ADDR                                            (0x0120u)
#define FLEXIO_CONTROL_SM3_1_RESET_VAL                                      (0x00000000u)
#define FLEXIO_CONTROL_SM3_1_MMS                                                     (0u)
#define FLEXIO_CONTROL_SM3_1_VOL_MASK                                       (0x0033FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | autopull_threshold3      | If the number of bits shifted     | 0x00000000  |
// |       |                          | from the OSR reaches a threshold  |             |
// |       |                          | (greater than or equal) and       |             |
// |       |                          | autopull is enabled, a pull       |             |
// |       |                          | operation will be performed for   |             |
// |       |                          | state machine 3.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | autopush_threshold3      | If the total number of shifted    | 0x00000000  |
// |       |                          | bits into the ISR reaches a       |             |
// |       |                          | threshold (greater than or        |             |
// |       |                          | equal) and autopush is enabled,   |             |
// |       |                          | a push operation will be          |             |
// |       |                          | performed for state machine 3.    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | pull_threshold3          | Sets the pull threshold for       | 0x00000000  |
// |       |                          | state machine 3                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | push_threshold3          | Sets the push threshold for       | 0x00000000  |
// |       |                          | state machine 3                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | osr_shift_direction3     | Sets the output shift direction   | 0x00000000  |
// |       |                          | for state machine 3.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | isr_shift_direction3     | Configures the input shift        | 0x00000000  |
// |       |                          | direction for state machine 3     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | autopush3                | Sets the autopush level for       | 0x00000000  |
// |       |                          | state machine 3                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | autopull3                | Indicates if the state machine 3  | 0x00000000  |
// |       |                          | will perform autopull.            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: autopull_threshold3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3                                (0u)
#define BITL_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3                                (4u)
#define BITM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3                       (0x0000000Fu)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL_THRESHOLD3_THRESHOLD_7           (0x00000007u)

// Bit field: autopush_threshold3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3                                (4u)
#define BITL_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3                                (4u)
#define BITM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3                       (0x000000F0u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3_THRESHOLD_1           (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3_THRESHOLD_2           (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3_THRESHOLD_3           (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3_THRESHOLD_4           (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3_THRESHOLD_5           (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3_THRESHOLD_6           (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH_THRESHOLD3_THRESHOLD_7           (0x00000007u)

// Bit field: pull_threshold3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3                                    (8u)
#define BITL_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3                                    (4u)
#define BITM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3                           (0x00000F00u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PULL_THRESHOLD3_BITS_7                    (0x00000007u)

// Bit field: push_threshold3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3                                   (12u)
#define BITL_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3                                    (4u)
#define BITM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3                           (0x0000F000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_0                    (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_1                    (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_2                    (0x00000002u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_3                    (0x00000003u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_4                    (0x00000004u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_5                    (0x00000005u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_6                    (0x00000006u)
#define ENUM_FLEXIO_CONTROL_SM3_1_PUSH_THRESHOLD3_BITS_7                    (0x00000007u)

// Bit field: osr_shift_direction3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_OSR_SHIFT_DIRECTION3                              (16u)
#define BITL_FLEXIO_CONTROL_SM3_1_OSR_SHIFT_DIRECTION3                               (1u)
#define BITM_FLEXIO_CONTROL_SM3_1_OSR_SHIFT_DIRECTION3                      (0x00010000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_OSR_SHIFT_DIRECTION3_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_1_OSR_SHIFT_DIRECTION3_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: isr_shift_direction3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_ISR_SHIFT_DIRECTION3                              (17u)
#define BITL_FLEXIO_CONTROL_SM3_1_ISR_SHIFT_DIRECTION3                               (1u)
#define BITM_FLEXIO_CONTROL_SM3_1_ISR_SHIFT_DIRECTION3                      (0x00020000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_ISR_SHIFT_DIRECTION3_RIGHT_TO_LEFT        (0x00000001u)
#define ENUM_FLEXIO_CONTROL_SM3_1_ISR_SHIFT_DIRECTION3_LEFT_TO_RIGHT        (0x00000000u)

// Bit field: autopush3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_AUTOPUSH3                                         (20u)
#define BITL_FLEXIO_CONTROL_SM3_1_AUTOPUSH3                                          (1u)
#define BITM_FLEXIO_CONTROL_SM3_1_AUTOPUSH3                                 (0x00100000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH3_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPUSH3_ENABLED                         (0x00000001u)

// Bit field: autopull3
// Bit position, length and mask
#define BITP_FLEXIO_CONTROL_SM3_1_AUTOPULL3                                         (21u)
#define BITL_FLEXIO_CONTROL_SM3_1_AUTOPULL3                                          (1u)
#define BITM_FLEXIO_CONTROL_SM3_1_AUTOPULL3                                 (0x00200000u)
// Enums
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL3_DISABLED                        (0x00000000u)
#define ENUM_FLEXIO_CONTROL_SM3_1_AUTOPULL3_ENABLED                         (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CLKDIV0_1
// Register Addr        : 0x0124u
// Register NW Addr     : 0x0124u
// Register Reset Value : 0x00040004u
// Register MMS         : 0u

#define FLEXIO_CLKDIV0_1_ADDR                                                   (0x0124u)
#define FLEXIO_CLKDIV0_1_NW_ADDR                                                (0x0124u)
#define FLEXIO_CLKDIV0_1_RESET_VAL                                          (0x00040004u)
#define FLEXIO_CLKDIV0_1_MMS                                                         (0u)
#define FLEXIO_CLKDIV0_1_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | clkdiv0                  | Set the working frequency for     | 0x00000004  |
// |       |                          | state machine 0. The minimum is   |             |
// |       |                          | 4 when RAM refresh is enabled     |             |
// |       |                          | (enabled by default). Minimum 3   |             |
// |       |                          | when disabled. freq = 100 MHz /   |             |
// |       |                          | value                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | clkdiv1                  | Set the working frequency for     | 0x00000004  |
// |       |                          | state machine 1. The minimum is   |             |
// |       |                          | 4 when RAM refresh is enabled (3  |             |
// |       |                          | when disabled). freq = 100 MHz /  |             |
// |       |                          | value                             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: clkdiv0
// Bit position, length and mask
#define BITP_FLEXIO_CLKDIV0_1_CLKDIV0                                                (0u)
#define BITL_FLEXIO_CLKDIV0_1_CLKDIV0                                               (16u)
#define BITM_FLEXIO_CLKDIV0_1_CLKDIV0                                       (0x0000FFFFu)

// Bit field: clkdiv1
// Bit position, length and mask
#define BITP_FLEXIO_CLKDIV0_1_CLKDIV1                                               (16u)
#define BITL_FLEXIO_CLKDIV0_1_CLKDIV1                                               (16u)
#define BITM_FLEXIO_CLKDIV0_1_CLKDIV1                                       (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CLKDIV2_3
// Register Addr        : 0x0128u
// Register NW Addr     : 0x0128u
// Register Reset Value : 0x00040004u
// Register MMS         : 0u

#define FLEXIO_CLKDIV2_3_ADDR                                                   (0x0128u)
#define FLEXIO_CLKDIV2_3_NW_ADDR                                                (0x0128u)
#define FLEXIO_CLKDIV2_3_RESET_VAL                                          (0x00040004u)
#define FLEXIO_CLKDIV2_3_MMS                                                         (0u)
#define FLEXIO_CLKDIV2_3_VOL_MASK                                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | clkdiv2                  | Set the working frequency for     | 0x00000004  |
// |       |                          | state machine 2. The minimum is   |             |
// |       |                          | 4 when RAM refresh is enabled (3  |             |
// |       |                          | when disabled). freq = 100 MHz /  |             |
// |       |                          | value                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | clkdiv3                  | Set the working frequency for     | 0x00000004  |
// |       |                          | state machine 3. The minimum is   |             |
// |       |                          | 4 when RAM refresh is enabled (3  |             |
// |       |                          | when disabled). freq = 100 MHz /  |             |
// |       |                          | value                             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: clkdiv2
// Bit position, length and mask
#define BITP_FLEXIO_CLKDIV2_3_CLKDIV2                                                (0u)
#define BITL_FLEXIO_CLKDIV2_3_CLKDIV2                                               (16u)
#define BITM_FLEXIO_CLKDIV2_3_CLKDIV2                                       (0x0000FFFFu)

// Bit field: clkdiv3
// Bit position, length and mask
#define BITP_FLEXIO_CLKDIV2_3_CLKDIV3                                               (16u)
#define BITL_FLEXIO_CLKDIV2_3_CLKDIV3                                               (16u)
#define BITM_FLEXIO_CLKDIV2_3_CLKDIV3                                       (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : INTERRUPTS
// Register Addr        : 0x012Cu
// Register NW Addr     : 0x012Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_INTERRUPTS_ADDR                                                  (0x012Cu)
#define FLEXIO_INTERRUPTS_NW_ADDR                                               (0x012Cu)
#define FLEXIO_INTERRUPTS_RESET_VAL                                         (0x00000000u)
#define FLEXIO_INTERRUPTS_MMS                                                        (0u)
#define FLEXIO_INTERRUPTS_VOL_MASK                                          (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | irq_clear_regmap         | Manually clears the interrupt     | 0x00000000  |
// |       |                          | bus of the state machines. This   |             |
// |       |                          | is a bit-wise operation.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | irq_set_regmap           | Manually sets the interrupt bus   | 0x00000000  |
// |       |                          | of the state machines. This is a  |             |
// |       |                          | bit-wise operation.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | irq_status               | Reads the current interrupt bus   | 0x00000000  |
// |       |                          | of the state machines.            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: irq_clear_regmap
// Bit position, length and mask
#define BITP_FLEXIO_INTERRUPTS_IRQ_CLEAR_REGMAP                                      (0u)
#define BITL_FLEXIO_INTERRUPTS_IRQ_CLEAR_REGMAP                                      (8u)
#define BITM_FLEXIO_INTERRUPTS_IRQ_CLEAR_REGMAP                             (0x000000FFu)

// Bit field: irq_set_regmap
// Bit position, length and mask
#define BITP_FLEXIO_INTERRUPTS_IRQ_SET_REGMAP                                        (8u)
#define BITL_FLEXIO_INTERRUPTS_IRQ_SET_REGMAP                                        (8u)
#define BITM_FLEXIO_INTERRUPTS_IRQ_SET_REGMAP                               (0x0000FF00u)

// Bit field: irq_status
// Bit position, length and mask
#define BITP_FLEXIO_INTERRUPTS_IRQ_STATUS                                           (16u)
#define BITL_FLEXIO_INTERRUPTS_IRQ_STATUS                                            (8u)
#define BITM_FLEXIO_INTERRUPTS_IRQ_STATUS                                   (0x00FF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : STATUS
// Register Addr        : 0x0130u
// Register NW Addr     : 0x0130u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define FLEXIO_STATUS_ADDR                                                      (0x0130u)
#define FLEXIO_STATUS_NW_ADDR                                                   (0x0130u)
#define FLEXIO_STATUS_RESET_VAL                                             (0x00000000u)
#define FLEXIO_STATUS_MMS                                                            (0u)
#define FLEXIO_STATUS_VOL_MASK                                              (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | ram_parity_error         | Reports the detection of parity   | 0x00000000  |
// |       |                          | errors in the instruction RAM     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | ram_halted_by_parity_erro| Reports that the state machine    | 0x00000000  | 
// |       |                          | is halted due to the presence of  |             |
// |       |                          | parity errors in the instruction  |             |
// |       |                          | RAM                               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | read_fifo_contention_coun| Increments each time there is a   | 0x00000000  | 
// |       |                          | write contention to the read      |             |
// |       |                          | fifo from a state machine         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: ram_parity_error
// Bit position, length and mask
#define BITP_FLEXIO_STATUS_RAM_PARITY_ERROR                                          (0u)
#define BITL_FLEXIO_STATUS_RAM_PARITY_ERROR                                          (1u)
#define BITM_FLEXIO_STATUS_RAM_PARITY_ERROR                                 (0x00000001u)
// Enums
#define ENUM_FLEXIO_STATUS_RAM_PARITY_ERROR_ERROR_DETECTED                  (0x00000001u)
#define ENUM_FLEXIO_STATUS_RAM_PARITY_ERROR_ERROR_FREE                      (0x00000000u)

// Bit field: ram_halted_by_parity_error
// Bit position, length and mask
#define BITP_FLEXIO_STATUS_RAM_HALTED_BY_PARITY_ERROR                                (1u)
#define BITL_FLEXIO_STATUS_RAM_HALTED_BY_PARITY_ERROR                                (1u)
#define BITM_FLEXIO_STATUS_RAM_HALTED_BY_PARITY_ERROR                       (0x00000002u)
// Enums
#define ENUM_FLEXIO_STATUS_RAM_HALTED_BY_PARITY_ERROR_HALTED                (0x00000001u)
#define ENUM_FLEXIO_STATUS_RAM_HALTED_BY_PARITY_ERROR_RUNNING               (0x00000000u)

// Bit field: read_fifo_contention_counter
// Bit position, length and mask
#define BITP_FLEXIO_STATUS_READ_FIFO_CONTENTION_COUNTER                              (8u)
#define BITL_FLEXIO_STATUS_READ_FIFO_CONTENTION_COUNTER                              (4u)
#define BITM_FLEXIO_STATUS_READ_FIFO_CONTENTION_COUNTER                     (0x00000F00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillFLEXIORegsData();
#endif


#endif /* ADI_E2BCORE_REGFLEXIO_H */

/** @} */

/*
 * EOF: www.analog.com
 */


