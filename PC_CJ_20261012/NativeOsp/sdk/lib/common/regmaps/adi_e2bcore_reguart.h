/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_reguart.h
 * @brief        : This is an auto generated header file with register
                   descriptions for UART interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGUART_H
#define ADI_E2BCORE_REGUART_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define UART_NUM_REGS    (6u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganUARTRegAddr[UART_NUM_REGS];
extern uint32_t ganUARTNwRegAddr[UART_NUM_REGS];
extern uint32_t ganUARTRegMMS[UART_NUM_REGS];
extern uint32_t ganUARTRegResetVal[UART_NUM_REGS];
extern const char *gasUARTRegName[UART_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00028000u
// Register MMS         : 0u

#define UART_CONTROL_ADDR                                                       (0x0000u)
#define UART_CONTROL_NW_ADDR                                                    (0x0000u)
#define UART_CONTROL_RESET_VAL                                              (0x00028000u)
#define UART_CONTROL_MMS                                                             (0u)
#define UART_CONTROL_VOL_MASK                                               (0xFEDBFC00u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 14-10 | read_bits                | Indicates the number of bits to   | 0x00000000  |
// |       |                          | read after the start bit. Must    |             |
// |       |                          | be greater than 1 and smaller     |             |
// |       |                          | than 12.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | transaction_header_enable| Reserved. It shall be always      | 0x00000001  | 
// |       |                          | set.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | resynchronization_require| Executes the resynchronization    | 0x00000000  | 
// |       |                          | procedure. Please, check the      |             |
// |       |                          | user guide of the interface for   |             |
// |       |                          | more information about this       |             |
// |       |                          | procedure.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | enable_upstream_ecco     | If disabled, the interface will   | 0x00000001  |
// |       |                          | not be capable of generating an   |             |
// |       |                          | ecco for each of the processed    |             |
// |       |                          | commands. Regardless of any       |             |
// |       |                          | other condition.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | enable_read_time_tag     | If Enabled, the Interface will    | 0x00000000  |
// |       |                          | tag the UART transaction          |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | enable_read_time_tag_ecco| If Enabled, the Interface will    | 0x00000000  | 
// |       |                          | tag the ecco commands with the    |             |
// |       |                          | current gPTP time-stamp.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22    | rx_uart_enable           | Setting this bit to one (1)       | 0x00000000  |
// |       |                          | enables the Rx UART interface.    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | tx_uart_enable           | Setting this bit to one (1)       | 0x00000000  |
// |       |                          | enables the Tx UART interface.    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | cts_enable               | The interface can only output a   | 0x00000000  |
// |       |                          | new byte is the CTS input port    |             |
// |       |                          | is set to one (1).                |             |
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

// Bit field: read_bits
// Bit position, length and mask
#define BITP_UART_CONTROL_READ_BITS                                                 (10u)
#define BITL_UART_CONTROL_READ_BITS                                                  (5u)
#define BITM_UART_CONTROL_READ_BITS                                         (0x00007C00u)

// Bit field: transaction_header_enable
// Bit position, length and mask
#define BITP_UART_CONTROL_TRANSACTION_HEADER_ENABLE                                 (15u)
#define BITL_UART_CONTROL_TRANSACTION_HEADER_ENABLE                                  (1u)
#define BITM_UART_CONTROL_TRANSACTION_HEADER_ENABLE                         (0x00008000u)
// Enums
#define ENUM_UART_CONTROL_TRANSACTION_HEADER_ENABLE_RESERVED                (0x00000000u)
#define ENUM_UART_CONTROL_TRANSACTION_HEADER_ENABLE_ENABLED                 (0x00000001u)

// Bit field: resynchronization_required
// Bit position, length and mask
#define BITP_UART_CONTROL_RESYNCHRONIZATION_REQUIRED                                (16u)
#define BITL_UART_CONTROL_RESYNCHRONIZATION_REQUIRED                                 (1u)
#define BITM_UART_CONTROL_RESYNCHRONIZATION_REQUIRED                        (0x00010000u)

// Bit field: enable_upstream_ecco
// Bit position, length and mask
#define BITP_UART_CONTROL_ENABLE_UPSTREAM_ECCO                                      (17u)
#define BITL_UART_CONTROL_ENABLE_UPSTREAM_ECCO                                       (1u)
#define BITM_UART_CONTROL_ENABLE_UPSTREAM_ECCO                              (0x00020000u)
// Enums
#define ENUM_UART_CONTROL_ENABLE_UPSTREAM_ECCO_DISABLE                      (0x00000000u)
#define ENUM_UART_CONTROL_ENABLE_UPSTREAM_ECCO_ENABLE                       (0x00000001u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_UART_CONTROL_ENABLE_READ_TIME_TAG                                      (19u)
#define BITL_UART_CONTROL_ENABLE_READ_TIME_TAG                                       (1u)
#define BITM_UART_CONTROL_ENABLE_READ_TIME_TAG                              (0x00080000u)
// Enums
#define ENUM_UART_CONTROL_ENABLE_READ_TIME_TAG_DISABLE                      (0x00000000u)
#define ENUM_UART_CONTROL_ENABLE_READ_TIME_TAG_ENABLE                       (0x00000001u)

// Bit field: enable_read_time_tag_ecco
// Bit position, length and mask
#define BITP_UART_CONTROL_ENABLE_READ_TIME_TAG_ECCO                                 (20u)
#define BITL_UART_CONTROL_ENABLE_READ_TIME_TAG_ECCO                                  (1u)
#define BITM_UART_CONTROL_ENABLE_READ_TIME_TAG_ECCO                         (0x00100000u)
// Enums
#define ENUM_UART_CONTROL_ENABLE_READ_TIME_TAG_ECCO_DISABLE                 (0x00000000u)
#define ENUM_UART_CONTROL_ENABLE_READ_TIME_TAG_ECCO_ENABLE                  (0x00000001u)

// Bit field: rx_uart_enable
// Bit position, length and mask
#define BITP_UART_CONTROL_RX_UART_ENABLE                                            (22u)
#define BITL_UART_CONTROL_RX_UART_ENABLE                                             (1u)
#define BITM_UART_CONTROL_RX_UART_ENABLE                                    (0x00400000u)
// Enums
#define ENUM_UART_CONTROL_RX_UART_ENABLE_DISABLE                            (0x00000000u)
#define ENUM_UART_CONTROL_RX_UART_ENABLE_ENABLE                             (0x00000001u)

// Bit field: tx_uart_enable
// Bit position, length and mask
#define BITP_UART_CONTROL_TX_UART_ENABLE                                            (23u)
#define BITL_UART_CONTROL_TX_UART_ENABLE                                             (1u)
#define BITM_UART_CONTROL_TX_UART_ENABLE                                    (0x00800000u)
// Enums
#define ENUM_UART_CONTROL_TX_UART_ENABLE_DISABLE                            (0x00000000u)
#define ENUM_UART_CONTROL_TX_UART_ENABLE_ENABLE                             (0x00000001u)

// Bit field: cts_enable
// Bit position, length and mask
#define BITP_UART_CONTROL_CTS_ENABLE                                                (25u)
#define BITL_UART_CONTROL_CTS_ENABLE                                                 (1u)
#define BITM_UART_CONTROL_CTS_ENABLE                                        (0x02000000u)
// Enums
#define ENUM_UART_CONTROL_CTS_ENABLE_DO_NOT_CHECK                           (0x00000000u)
#define ENUM_UART_CONTROL_CTS_ENABLE_CHECK                                  (0x00000001u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_UART_CONTROL_TX_FIFO_RESET                                             (26u)
#define BITL_UART_CONTROL_TX_FIFO_RESET                                              (1u)
#define BITM_UART_CONTROL_TX_FIFO_RESET                                     (0x04000000u)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_UART_CONTROL_RX_FIFO_RESET                                             (27u)
#define BITL_UART_CONTROL_RX_FIFO_RESET                                              (1u)
#define BITM_UART_CONTROL_RX_FIFO_RESET                                     (0x08000000u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_UART_CONTROL_SOFT_RESET                                                (28u)
#define BITL_UART_CONTROL_SOFT_RESET                                                 (4u)
#define BITM_UART_CONTROL_SOFT_RESET                                        (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TIMING
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x000196E0u
// Register MMS         : 0u

#define UART_TIMING_ADDR                                                        (0x0004u)
#define UART_TIMING_NW_ADDR                                                     (0x0004u)
#define UART_TIMING_RESET_VAL                                               (0x000196E0u)
#define UART_TIMING_MMS                                                              (0u)
#define UART_TIMING_VOL_MASK                                                (0x001FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 20-00 | tbit                     | Specified in ns. Min valid value  | 0x000196e0  |
// |       |                          | is 160.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tbit
// Bit position, length and mask
#define BITP_UART_TIMING_TBIT                                                        (0u)
#define BITL_UART_TIMING_TBIT                                                       (21u)
#define BITM_UART_TIMING_TBIT                                               (0x001FFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TIMING2
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x0000CB70u
// Register MMS         : 0u

#define UART_TIMING2_ADDR                                                       (0x0008u)
#define UART_TIMING2_NW_ADDR                                                    (0x0008u)
#define UART_TIMING2_RESET_VAL                                              (0x0000CB70u)
#define UART_TIMING2_MMS                                                             (0u)
#define UART_TIMING2_VOL_MASK                                               (0x001FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 20-00 | sampling_bit_time        | Specified in ns. Relative time    | 0x0000cb70  |
// |       |                          | from the falling edge of the      |             |
// |       |                          | start bit to the sampling point.  |             |
// |       |                          | The close to 8/16 of Tbit the     |             |
// |       |                          | better.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: sampling_bit_time
// Bit position, length and mask
#define BITP_UART_TIMING2_SAMPLING_BIT_TIME                                          (0u)
#define BITL_UART_TIMING2_SAMPLING_BIT_TIME                                         (21u)
#define BITM_UART_TIMING2_SAMPLING_BIT_TIME                                 (0x001FFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : STATS
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define UART_STATS_ADDR                                                         (0x000Cu)
#define UART_STATS_NW_ADDR                                                      (0x000Cu)
#define UART_STATS_RESET_VAL                                                (0x00000000u)
#define UART_STATS_MMS                                                               (0u)
#define UART_STATS_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_read_words           | Number of upstream transmitted    | 0x00000000  |
// |       |                          | word.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | num_write_words          | Number of downstream transmitted  | 0x00000000  |
// |       |                          | words.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_read_words
// Bit position, length and mask
#define BITP_UART_STATS_NUM_READ_WORDS                                               (0u)
#define BITL_UART_STATS_NUM_READ_WORDS                                              (16u)
#define BITM_UART_STATS_NUM_READ_WORDS                                      (0x0000FFFFu)

// Bit field: num_write_words
// Bit position, length and mask
#define BITP_UART_STATS_NUM_WRITE_WORDS                                             (16u)
#define BITL_UART_STATS_NUM_WRITE_WORDS                                             (16u)
#define BITM_UART_STATS_NUM_WRITE_WORDS                                     (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : STATUS
// Register Addr        : 0x0010u
// Register NW Addr     : 0x0010u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define UART_STATUS_ADDR                                                        (0x0010u)
#define UART_STATUS_NW_ADDR                                                     (0x0010u)
#define UART_STATUS_RESET_VAL                                               (0x00000000u)
#define UART_STATUS_MMS                                                              (0u)
#define UART_STATUS_VOL_MASK                                                (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | word_transmitted         | Toggling signal every time an     | 0x00000000  |
// |       |                          | octet is transmitted.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | word_received            | Toggling signal every time an     | 0x00000000  |
// |       |                          | octet is received.                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: word_transmitted
// Bit position, length and mask
#define BITP_UART_STATUS_WORD_TRANSMITTED                                            (0u)
#define BITL_UART_STATUS_WORD_TRANSMITTED                                            (1u)
#define BITM_UART_STATUS_WORD_TRANSMITTED                                   (0x00000001u)

// Bit field: word_received
// Bit position, length and mask
#define BITP_UART_STATUS_WORD_RECEIVED                                               (1u)
#define BITL_UART_STATUS_WORD_RECEIVED                                               (1u)
#define BITM_UART_STATUS_WORD_RECEIVED                                      (0x00000002u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL2
// Register Addr        : 0x0014u
// Register NW Addr     : 0x0014u
// Register Reset Value : 0x00000001u
// Register MMS         : 0u

#define UART_CONTROL2_ADDR                                                      (0x0014u)
#define UART_CONTROL2_NW_ADDR                                                   (0x0014u)
#define UART_CONTROL2_RESET_VAL                                             (0x00000001u)
#define UART_CONTROL2_MMS                                                            (0u)
#define UART_CONTROL2_VOL_MASK                                              (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_packed_transactions  | Default configuration used for    | 0x00000001  |
// |       |                          | the optional field                |             |
// |       |                          | "num_packed_tranactions" in the   |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_packed_transactions
// Bit position, length and mask
#define BITP_UART_CONTROL2_NUM_PACKED_TRANSACTIONS                                   (0u)
#define BITL_UART_CONTROL2_NUM_PACKED_TRANSACTIONS                                  (16u)
#define BITM_UART_CONTROL2_NUM_PACKED_TRANSACTIONS                          (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillUARTRegsData();
#endif


#endif /* ADI_E2BCORE_REGUART_H */

/** @} */

/*
 * EOF: www.analog.com
 */


