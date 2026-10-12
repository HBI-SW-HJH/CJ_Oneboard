/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_regiseled.h
 * @brief        : This is an auto generated header file with register
                   descriptions for ISELED interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGISELED_H
#define ADI_E2BCORE_REGISELED_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define ISELED_NUM_REGS    (7u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganISELEDRegAddr[ISELED_NUM_REGS];
extern uint32_t ganISELEDNwRegAddr[ISELED_NUM_REGS];
extern uint32_t ganISELEDRegMMS[ISELED_NUM_REGS];
extern uint32_t ganISELEDRegResetVal[ISELED_NUM_REGS];
extern const char *gasISELEDRegName[ISELED_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Control
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00000925u
// Register MMS         : 0u

#define ISELED_CONTROL_ADDR                                                     (0x0000u)
#define ISELED_CONTROL_NW_ADDR                                                  (0x0000u)
#define ISELED_CONTROL_RESET_VAL                                            (0x00000925u)
#define ISELED_CONTROL_MMS                                                           (0u)
#define ISELED_CONTROL_VOL_MASK                                             (0xFCE39FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 8     | transaction_header_enable| Reserved. It shall be always      | 0x00000001  | 
// |       |                          | set.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | ilas_mode                | Enables or disables ILAS output   | 0x00000000  |
// |       |                          | drivers                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | resynchronization_require| Once set, the next executed       | 0x00000000  | 
// |       |                          | transaction shall be a            |             |
// |       |                          | resynchronization transaction     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | enable_upstream_ecco     | If disabled, the interface will   | 0x00000001  |
// |       |                          | not be capable of generating an   |             |
// |       |                          | ecco for each of the processed    |             |
// |       |                          | commands. Regardless of any       |             |
// |       |                          | other condition.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | enable_read_time_tag_ecco| If set, the interface will tag    | 0x00000000  | 
// |       |                          | the ecco commands with the        |             |
// |       |                          | current gPTP time-stamp           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | enable_read_time_tag     | If set, the interface will tag    | 0x00000000  |
// |       |                          | the interface transaction         |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | transaction_trigger      | Triggers one transaction. If      | 0x00000000  |
// |       |                          | there is no data in the transmit  |             |
// |       |                          | FIFO, the next transaction will   |             |
// |       |                          | be executed as soon as it         |             |
// |       |                          | arrives. This field is only       |             |
// |       |                          | valid if trigger_mode is set to   |             |
// |       |                          | MANUAL (0).                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | trigger_mode             | Select the transaction            | 0x00000000  |
// |       |                          | processing behavior.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | iseled_enable            | Enables or disables the ISELED    | 0x00000000  |
// |       |                          | interface.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-22 | channel_mode             | Selects the channel mode          | 0x00000000  |
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

// Bit field: transaction_header_enable
// Bit position, length and mask
#define BITP_ISELED_CONTROL_TRANSACTION_HEADER_ENABLE                                (8u)
#define BITL_ISELED_CONTROL_TRANSACTION_HEADER_ENABLE                                (1u)
#define BITM_ISELED_CONTROL_TRANSACTION_HEADER_ENABLE                       (0x00000100u)
// Enums
#define ENUM_ISELED_CONTROL_TRANSACTION_HEADER_ENABLE_RESERVED              (0x00000000u)
#define ENUM_ISELED_CONTROL_TRANSACTION_HEADER_ENABLE_ENABLED               (0x00000001u)

// Bit field: ilas_mode
// Bit position, length and mask
#define BITP_ISELED_CONTROL_ILAS_MODE                                                (9u)
#define BITL_ISELED_CONTROL_ILAS_MODE                                                (1u)
#define BITM_ISELED_CONTROL_ILAS_MODE                                       (0x00000200u)
// Enums
#define ENUM_ISELED_CONTROL_ILAS_MODE_ISELED_MODE                           (0x00000000u)
#define ENUM_ISELED_CONTROL_ILAS_MODE_ILAS_MODE                             (0x00000001u)

// Bit field: resynchronization_required
// Bit position, length and mask
#define BITP_ISELED_CONTROL_RESYNCHRONIZATION_REQUIRED                              (10u)
#define BITL_ISELED_CONTROL_RESYNCHRONIZATION_REQUIRED                               (1u)
#define BITM_ISELED_CONTROL_RESYNCHRONIZATION_REQUIRED                      (0x00000400u)

// Bit field: enable_upstream_ecco
// Bit position, length and mask
#define BITP_ISELED_CONTROL_ENABLE_UPSTREAM_ECCO                                    (11u)
#define BITL_ISELED_CONTROL_ENABLE_UPSTREAM_ECCO                                     (1u)
#define BITM_ISELED_CONTROL_ENABLE_UPSTREAM_ECCO                            (0x00000800u)
// Enums
#define ENUM_ISELED_CONTROL_ENABLE_UPSTREAM_ECCO_DISABLE                    (0x00000000u)
#define ENUM_ISELED_CONTROL_ENABLE_UPSTREAM_ECCO_ENABLE                     (0x00000001u)

// Bit field: enable_read_time_tag_ecco
// Bit position, length and mask
#define BITP_ISELED_CONTROL_ENABLE_READ_TIME_TAG_ECCO                               (12u)
#define BITL_ISELED_CONTROL_ENABLE_READ_TIME_TAG_ECCO                                (1u)
#define BITM_ISELED_CONTROL_ENABLE_READ_TIME_TAG_ECCO                       (0x00001000u)
// Enums
#define ENUM_ISELED_CONTROL_ENABLE_READ_TIME_TAG_ECCO_DISABLE               (0x00000000u)
#define ENUM_ISELED_CONTROL_ENABLE_READ_TIME_TAG_ECCO_ENABLE                (0x00000001u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_ISELED_CONTROL_ENABLE_READ_TIME_TAG                                    (15u)
#define BITL_ISELED_CONTROL_ENABLE_READ_TIME_TAG                                     (1u)
#define BITM_ISELED_CONTROL_ENABLE_READ_TIME_TAG                            (0x00008000u)
// Enums
#define ENUM_ISELED_CONTROL_ENABLE_READ_TIME_TAG_DISABLE                    (0x00000000u)
#define ENUM_ISELED_CONTROL_ENABLE_READ_TIME_TAG_ENABLE                     (0x00000001u)

// Bit field: transaction_trigger
// Bit position, length and mask
#define BITP_ISELED_CONTROL_TRANSACTION_TRIGGER                                     (16u)
#define BITL_ISELED_CONTROL_TRANSACTION_TRIGGER                                      (1u)
#define BITM_ISELED_CONTROL_TRANSACTION_TRIGGER                             (0x00010000u)

// Bit field: trigger_mode
// Bit position, length and mask
#define BITP_ISELED_CONTROL_TRIGGER_MODE                                            (17u)
#define BITL_ISELED_CONTROL_TRIGGER_MODE                                             (1u)
#define BITM_ISELED_CONTROL_TRIGGER_MODE                                    (0x00020000u)
// Enums
#define ENUM_ISELED_CONTROL_TRIGGER_MODE_MANUAL                             (0x00000000u)
#define ENUM_ISELED_CONTROL_TRIGGER_MODE_AUTOMATIC                          (0x00000001u)

// Bit field: iseled_enable
// Bit position, length and mask
#define BITP_ISELED_CONTROL_ISELED_ENABLE                                           (21u)
#define BITL_ISELED_CONTROL_ISELED_ENABLE                                            (1u)
#define BITM_ISELED_CONTROL_ISELED_ENABLE                                   (0x00200000u)
// Enums
#define ENUM_ISELED_CONTROL_ISELED_ENABLE_DISABLE                           (0x00000000u)
#define ENUM_ISELED_CONTROL_ISELED_ENABLE_ENABLE                            (0x00000001u)

// Bit field: channel_mode
// Bit position, length and mask
#define BITP_ISELED_CONTROL_CHANNEL_MODE                                            (22u)
#define BITL_ISELED_CONTROL_CHANNEL_MODE                                             (2u)
#define BITM_ISELED_CONTROL_CHANNEL_MODE                                    (0x00C00000u)
// Enums
#define ENUM_ISELED_CONTROL_CHANNEL_MODE_SAFE_MODE                          (0x00000000u)
#define ENUM_ISELED_CONTROL_CHANNEL_MODE_OPTIMAL_MODE                       (0x00000001u)
#define ENUM_ISELED_CONTROL_CHANNEL_MODE_FAST_MODE                          (0x00000002u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_ISELED_CONTROL_TX_FIFO_RESET                                           (26u)
#define BITL_ISELED_CONTROL_TX_FIFO_RESET                                            (1u)
#define BITM_ISELED_CONTROL_TX_FIFO_RESET                                   (0x04000000u)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_ISELED_CONTROL_RX_FIFO_RESET                                           (27u)
#define BITL_ISELED_CONTROL_RX_FIFO_RESET                                            (1u)
#define BITM_ISELED_CONTROL_RX_FIFO_RESET                                   (0x08000000u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_ISELED_CONTROL_SOFT_RESET                                              (28u)
#define BITL_ISELED_CONTROL_SOFT_RESET                                               (4u)
#define BITM_ISELED_CONTROL_SOFT_RESET                                      (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Instruction
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x00C11F40u
// Register MMS         : 0u

#define ISELED_INSTRUCTION_ADDR                                                 (0x0004u)
#define ISELED_INSTRUCTION_NW_ADDR                                              (0x0004u)
#define ISELED_INSTRUCTION_RESET_VAL                                        (0x00C11F40u)
#define ISELED_INSTRUCTION_MMS                                                       (0u)
#define ISELED_INSTRUCTION_VOL_MASK                                         (0x07FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | downstream_packet_length | In safe mode, indicates the       | 0x00000040  |
// |       |                          | number of bits to transmit per    |             |
// |       |                          | instruction. In optimal mode,     |             |
// |       |                          | indicates the number of bits      |             |
// |       |                          | used for the instruction,         |             |
// |       |                          | address, data and CRC fields. In  |             |
// |       |                          | fast mode, indicates the number   |             |
// |       |                          | of bits used for the              |             |
// |       |                          | instruction, address and data.    |             |
// |       |                          | In this case, the length shall    |             |
// |       |                          | be a multiple of four.            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | add_crc                  | When the interface is configured  | 0x00000001  |
// |       |                          | in fast mode, it will also        |             |
// |       |                          | compute and add the CRC field     |             |
// |       |                          | within an ISELED transaction.     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: downstream_packet_length
// Bit position, length and mask
#define BITP_ISELED_INSTRUCTION_DOWNSTREAM_PACKET_LENGTH                             (0u)
#define BITL_ISELED_INSTRUCTION_DOWNSTREAM_PACKET_LENGTH                             (8u)
#define BITM_ISELED_INSTRUCTION_DOWNSTREAM_PACKET_LENGTH                    (0x000000FFu)

// Bit field: add_crc
// Bit position, length and mask
#define BITP_ISELED_INSTRUCTION_ADD_CRC                                              (8u)
#define BITL_ISELED_INSTRUCTION_ADD_CRC                                              (1u)
#define BITM_ISELED_INSTRUCTION_ADD_CRC                                     (0x00000100u)
// Enums
#define ENUM_ISELED_INSTRUCTION_ADD_CRC_DISABLE                             (0x00000000u)
#define ENUM_ISELED_INSTRUCTION_ADD_CRC_ENABLE                              (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Stats
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define ISELED_STATS_ADDR                                                       (0x0008u)
#define ISELED_STATS_NW_ADDR                                                    (0x0008u)
#define ISELED_STATS_RESET_VAL                                              (0x00000000u)
#define ISELED_STATS_MMS                                                             (0u)
#define ISELED_STATS_VOL_MASK                                               (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_rd_packets           | Counts the number of upstream     | 0x00000000  |
// |       |                          | transactions that have been       |             |
// |       |                          | detected.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | num_wr_packets           | Counts the number of downstream   | 0x00000000  |
// |       |                          | transactions that have been       |             |
// |       |                          | detected.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_rd_packets
// Bit position, length and mask
#define BITP_ISELED_STATS_NUM_RD_PACKETS                                             (0u)
#define BITL_ISELED_STATS_NUM_RD_PACKETS                                            (16u)
#define BITM_ISELED_STATS_NUM_RD_PACKETS                                    (0x0000FFFFu)

// Bit field: num_wr_packets
// Bit position, length and mask
#define BITP_ISELED_STATS_NUM_WR_PACKETS                                            (16u)
#define BITL_ISELED_STATS_NUM_WR_PACKETS                                            (16u)
#define BITM_ISELED_STATS_NUM_WR_PACKETS                                    (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Status
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define ISELED_STATUS_ADDR                                                      (0x000Cu)
#define ISELED_STATUS_NW_ADDR                                                   (0x000Cu)
#define ISELED_STATUS_RESET_VAL                                             (0x00000000u)
#define ISELED_STATUS_MMS                                                            (0u)
#define ISELED_STATUS_VOL_MASK                                              (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | bb                       | Indicates if the interface is     | 0x00000000  |
// |       |                          | currently processing a            |             |
// |       |                          | transaction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | contention               | Indicates that during the last    | 0x00000000  |
// |       |                          | transaction, there was a          |             |
// |       |                          | contention issue in the           |             |
// |       |                          | electrical wire.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | rcv_frame_eoc_error      | Indicates that during the last    | 0x00000000  |
// |       |                          | read transaction, it was not      |             |
// |       |                          | possible to detect the EOC field  |             |
// |       |                          | after the CRC field.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | rcv_frame_crc_error      | Indicates that during the last    | 0x00000000  |
// |       |                          | read transaction, the received    |             |
// |       |                          | CRC field does not match the      |             |
// |       |                          | checksum of the transaction.      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | rcv_frame_freq_sync_error| Indicates that during the last    | 0x00000000  | 
// |       |                          | read transaction, after the       |             |
// |       |                          | reception of the frame_sync       |             |
// |       |                          | field, it was not possible to     |             |
// |       |                          | correctly recover the frame_freq  |             |
// |       |                          | pattern of the transaction.       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | rcv_frame_frame_sync_erro| Indicates that during the last    | 0x00000000  | 
// |       |                          | read transaction, the frame sync  |             |
// |       |                          | field frequency is outside the    |             |
// |       |                          | valid frequency range.            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | rcv_frame_5b4b_error     | Indicates that during the last    | 0x00000000  |
// |       |                          | read transaction, there was an    |             |
// |       |                          | invalid 4b5b code.                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: bb
// Bit position, length and mask
#define BITP_ISELED_STATUS_BB                                                        (0u)
#define BITL_ISELED_STATUS_BB                                                        (1u)
#define BITM_ISELED_STATUS_BB                                               (0x00000001u)
// Enums
#define ENUM_ISELED_STATUS_BB_FREE                                          (0x00000000u)
#define ENUM_ISELED_STATUS_BB_BUSY                                          (0x00000001u)

// Bit field: contention
// Bit position, length and mask
#define BITP_ISELED_STATUS_CONTENTION                                                (1u)
#define BITL_ISELED_STATUS_CONTENTION                                                (1u)
#define BITM_ISELED_STATUS_CONTENTION                                       (0x00000002u)
// Enums
#define ENUM_ISELED_STATUS_CONTENTION_CONTETION                             (0x00000001u)
#define ENUM_ISELED_STATUS_CONTENTION_NOT_CONTENTION                        (0x00000000u)

// Bit field: rcv_frame_eoc_error
// Bit position, length and mask
#define BITP_ISELED_STATUS_RCV_FRAME_EOC_ERROR                                       (2u)
#define BITL_ISELED_STATUS_RCV_FRAME_EOC_ERROR                                       (1u)
#define BITM_ISELED_STATUS_RCV_FRAME_EOC_ERROR                              (0x00000004u)

// Bit field: rcv_frame_crc_error
// Bit position, length and mask
#define BITP_ISELED_STATUS_RCV_FRAME_CRC_ERROR                                       (3u)
#define BITL_ISELED_STATUS_RCV_FRAME_CRC_ERROR                                       (1u)
#define BITM_ISELED_STATUS_RCV_FRAME_CRC_ERROR                              (0x00000008u)

// Bit field: rcv_frame_freq_sync_error
// Bit position, length and mask
#define BITP_ISELED_STATUS_RCV_FRAME_FREQ_SYNC_ERROR                                 (4u)
#define BITL_ISELED_STATUS_RCV_FRAME_FREQ_SYNC_ERROR                                 (1u)
#define BITM_ISELED_STATUS_RCV_FRAME_FREQ_SYNC_ERROR                        (0x00000010u)

// Bit field: rcv_frame_frame_sync_error
// Bit position, length and mask
#define BITP_ISELED_STATUS_RCV_FRAME_FRAME_SYNC_ERROR                                (5u)
#define BITL_ISELED_STATUS_RCV_FRAME_FRAME_SYNC_ERROR                                (1u)
#define BITM_ISELED_STATUS_RCV_FRAME_FRAME_SYNC_ERROR                       (0x00000020u)

// Bit field: rcv_frame_5b4b_error
// Bit position, length and mask
#define BITP_ISELED_STATUS_RCV_FRAME_5B4B_ERROR                                      (6u)
#define BITL_ISELED_STATUS_RCV_FRAME_5B4B_ERROR                                      (1u)
#define BITM_ISELED_STATUS_RCV_FRAME_5B4B_ERROR                             (0x00000040u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TRANSACTION_HEADER
// Register Addr        : 0x0010u
// Register NW Addr     : 0x0010u
// Register Reset Value : 0x00000001u
// Register MMS         : 0u

#define ISELED_TRANSACTION_HEADER_ADDR                                          (0x0010u)
#define ISELED_TRANSACTION_HEADER_NW_ADDR                                       (0x0010u)
#define ISELED_TRANSACTION_HEADER_RESET_VAL                                 (0x00000001u)
#define ISELED_TRANSACTION_HEADER_MMS                                                (0u)
#define ISELED_TRANSACTION_HEADER_VOL_MASK                                  (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_packed_transactions  | Defines the number of ISELED      | 0x00000001  |
// |       |                          | transactions that are packed      |             |
// |       |                          | between headers when              |             |
// |       |                          | transaction_header_enable is set  |             |
// |       |                          | to 1'b1                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_packed_transactions
// Bit position, length and mask
#define BITP_ISELED_TRANSACTION_HEADER_NUM_PACKED_TRANSACTIONS                       (0u)
#define BITL_ISELED_TRANSACTION_HEADER_NUM_PACKED_TRANSACTIONS                      (16u)
#define BITM_ISELED_TRANSACTION_HEADER_NUM_PACKED_TRANSACTIONS              (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SETDIM_CONFIGURATION
// Register Addr        : 0x0014u
// Register NW Addr     : 0x0014u
// Register Reset Value : 0x00003945u
// Register MMS         : 0u

#define ISELED_SETDIM_CONFIGURATION_ADDR                                        (0x0014u)
#define ISELED_SETDIM_CONFIGURATION_NW_ADDR                                     (0x0014u)
#define ISELED_SETDIM_CONFIGURATION_RESET_VAL                               (0x00003945u)
#define ISELED_SETDIM_CONFIGURATION_MMS                                              (0u)
#define ISELED_SETDIM_CONFIGURATION_VOL_MASK                                (0x00003FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | cmd_op_set_op1           | Defines the setdim op code        | 0x00000005  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | cmd_op_set_op2           | Defines the setrgb op code        | 0x00000004  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10-08 | set_op1_packing_mode     | Defines the bytes that will be    | 0x00000001  |
// |       |                          | used for op1 when in setdimrgb    |             |
// |       |                          | mode                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-11 | set_op2_packing_mode     | Defines the bytes that will be    | 0x00000007  |
// |       |                          | used for op2 when in setdimrgb    |             |
// |       |                          | mode                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cmd_op_set_op1
// Bit position, length and mask
#define BITP_ISELED_SETDIM_CONFIGURATION_CMD_OP_SET_OP1                              (0u)
#define BITL_ISELED_SETDIM_CONFIGURATION_CMD_OP_SET_OP1                              (4u)
#define BITM_ISELED_SETDIM_CONFIGURATION_CMD_OP_SET_OP1                     (0x0000000Fu)

// Bit field: cmd_op_set_op2
// Bit position, length and mask
#define BITP_ISELED_SETDIM_CONFIGURATION_CMD_OP_SET_OP2                              (4u)
#define BITL_ISELED_SETDIM_CONFIGURATION_CMD_OP_SET_OP2                              (4u)
#define BITM_ISELED_SETDIM_CONFIGURATION_CMD_OP_SET_OP2                     (0x000000F0u)

// Bit field: set_op1_packing_mode
// Bit position, length and mask
#define BITP_ISELED_SETDIM_CONFIGURATION_SET_OP1_PACKING_MODE                        (8u)
#define BITL_ISELED_SETDIM_CONFIGURATION_SET_OP1_PACKING_MODE                        (3u)
#define BITM_ISELED_SETDIM_CONFIGURATION_SET_OP1_PACKING_MODE               (0x00000700u)

// Bit field: set_op2_packing_mode
// Bit position, length and mask
#define BITP_ISELED_SETDIM_CONFIGURATION_SET_OP2_PACKING_MODE                       (11u)
#define BITL_ISELED_SETDIM_CONFIGURATION_SET_OP2_PACKING_MODE                        (3u)
#define BITM_ISELED_SETDIM_CONFIGURATION_SET_OP2_PACKING_MODE               (0x00003800u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Dummy_config
// Register Addr        : 0x0018u
// Register NW Addr     : 0x0018u
// Register Reset Value : 0x00000007u
// Register MMS         : 0u

#define ISELED_DUMMY_CONFIG_ADDR                                                (0x0018u)
#define ISELED_DUMMY_CONFIG_NW_ADDR                                             (0x0018u)
#define ISELED_DUMMY_CONFIG_RESET_VAL                                       (0x00000007u)
#define ISELED_DUMMY_CONFIG_MMS                                                      (0u)
#define ISELED_DUMMY_CONFIG_VOL_MASK                                        (0x0000000Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | dummy_response_length    | Number of bytes of a dummy        | 0x00000007  |
// |       |                          | response when in optimal and      |             |
// |       |                          | safe modes                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: dummy_response_length
// Bit position, length and mask
#define BITP_ISELED_DUMMY_CONFIG_DUMMY_RESPONSE_LENGTH                               (0u)
#define BITL_ISELED_DUMMY_CONFIG_DUMMY_RESPONSE_LENGTH                               (4u)
#define BITM_ISELED_DUMMY_CONFIG_DUMMY_RESPONSE_LENGTH                      (0x0000000Fu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillISELEDRegsData();
#endif


#endif /* ADI_E2BCORE_REGISELED_H */

/** @} */

/*
 * EOF: www.analog.com
 */


