/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_regspi.h
 * @brief        : This is an auto generated header file with register
                   descriptions for SPI interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGSPI_H
#define ADI_E2BCORE_REGSPI_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SPI_NUM_REGS    (11u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganSPIRegAddr[SPI_NUM_REGS];
extern uint32_t ganSPINwRegAddr[SPI_NUM_REGS];
extern uint32_t ganSPIRegMMS[SPI_NUM_REGS];
extern uint32_t ganSPIRegResetVal[SPI_NUM_REGS];
extern const char *gasSPIRegName[SPI_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Control
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x000201FEu
// Register MMS         : 0u

#define SPI_CONTROL_ADDR                                                        (0x0000u)
#define SPI_CONTROL_NW_ADDR                                                     (0x0000u)
#define SPI_CONTROL_RESET_VAL                                               (0x000201FEu)
#define SPI_CONTROL_MMS                                                              (0u)
#define SPI_CONTROL_VOL_MASK                                                (0xFFFBF1FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | CSb                      | Chip Select                       | 0x000000fe  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | transaction_header_enable| Enables the transaction header    | 0x00000001  | 
// |       |                          | at the transmit FIFO. If          |             |
// |       |                          | enabled, the length and the SS    |             |
// |       |                          | of the transactions shall be      |             |
// |       |                          | selectable without accessing the  |             |
// |       |                          | register map.                     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | resynchronization_require| Starts the resinchronization      | 0x00000000  | 
// |       |                          | feature                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | enable_flow_control      | Sets if the flow control.         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | enable_nrz_communication | Sets the NRZ communication mode.  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | enable_read_time_tag     | If enabled, the interface will    | 0x00000000  |
// |       |                          | tag the SPI transaction           |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | transaction_trigger      | Triggers one transaction. If      | 0x00000000  |
// |       |                          | there is no data in the transmit  |             |
// |       |                          | FIFO, the next transaction will   |             |
// |       |                          | be executed as soon as it         |             |
// |       |                          | arrives. This field is only       |             |
// |       |                          | valid if trigger_mode is set to   |             |
// |       |                          | zero (0).                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | trigger_mode             | Select the transaction            | 0x00000001  |
// |       |                          | processing behavior.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | manual_chip_select       | This bit forces the data in the   | 0x00000000  |
// |       |                          | chip select register to be        |             |
// |       |                          | asserted on the chip select       |             |
// |       |                          | output anytime the device is      |             |
// |       |                          | configured as a master and the    |             |
// |       |                          | device is enabled.                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | loop                     | Loopback mode                     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | spi_enable               | Setting this bit to one (1)       | 0x00000000  |
// |       |                          | enables the SPI interface.        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22    | controller               | This bit sets the SPI interface   | 0x00000000  |
// |       |                          | as a master or a slave.           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | lsb_first                | LSB First.                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24    | cpol                     | Clock Polarity.                   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | cpha                     | Clock Phase.                      | 0x00000000  |
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

// Bit field: CSb
// Bit position, length and mask
#define BITP_SPI_CONTROL_CSB                                                         (0u)
#define BITL_SPI_CONTROL_CSB                                                         (8u)
#define BITM_SPI_CONTROL_CSB                                                (0x000000FFu)

// Bit field: transaction_header_enable
// Bit position, length and mask
#define BITP_SPI_CONTROL_TRANSACTION_HEADER_ENABLE                                   (8u)
#define BITL_SPI_CONTROL_TRANSACTION_HEADER_ENABLE                                   (1u)
#define BITM_SPI_CONTROL_TRANSACTION_HEADER_ENABLE                          (0x00000100u)
// Enums
#define ENUM_SPI_CONTROL_TRANSACTION_HEADER_ENABLE_DISABLED                 (0x00000000u)
#define ENUM_SPI_CONTROL_TRANSACTION_HEADER_ENABLE_ENABLED                  (0x00000001u)

// Bit field: resynchronization_required
// Bit position, length and mask
#define BITP_SPI_CONTROL_RESYNCHRONIZATION_REQUIRED                                 (12u)
#define BITL_SPI_CONTROL_RESYNCHRONIZATION_REQUIRED                                  (1u)
#define BITM_SPI_CONTROL_RESYNCHRONIZATION_REQUIRED                         (0x00001000u)

// Bit field: enable_flow_control
// Bit position, length and mask
#define BITP_SPI_CONTROL_ENABLE_FLOW_CONTROL                                        (13u)
#define BITL_SPI_CONTROL_ENABLE_FLOW_CONTROL                                         (1u)
#define BITM_SPI_CONTROL_ENABLE_FLOW_CONTROL                                (0x00002000u)
// Enums
#define ENUM_SPI_CONTROL_ENABLE_FLOW_CONTROL_DISABLE                        (0x00000000u)
#define ENUM_SPI_CONTROL_ENABLE_FLOW_CONTROL_ENABLE                         (0x00000001u)

// Bit field: enable_nrz_communication
// Bit position, length and mask
#define BITP_SPI_CONTROL_ENABLE_NRZ_COMMUNICATION                                   (14u)
#define BITL_SPI_CONTROL_ENABLE_NRZ_COMMUNICATION                                    (1u)
#define BITM_SPI_CONTROL_ENABLE_NRZ_COMMUNICATION                           (0x00004000u)
// Enums
#define ENUM_SPI_CONTROL_ENABLE_NRZ_COMMUNICATION_DISABLE                   (0x00000000u)
#define ENUM_SPI_CONTROL_ENABLE_NRZ_COMMUNICATION_ENABLE                    (0x00000001u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_SPI_CONTROL_ENABLE_READ_TIME_TAG                                       (15u)
#define BITL_SPI_CONTROL_ENABLE_READ_TIME_TAG                                        (1u)
#define BITM_SPI_CONTROL_ENABLE_READ_TIME_TAG                               (0x00008000u)

// Bit field: transaction_trigger
// Bit position, length and mask
#define BITP_SPI_CONTROL_TRANSACTION_TRIGGER                                        (16u)
#define BITL_SPI_CONTROL_TRANSACTION_TRIGGER                                         (1u)
#define BITM_SPI_CONTROL_TRANSACTION_TRIGGER                                (0x00010000u)

// Bit field: trigger_mode
// Bit position, length and mask
#define BITP_SPI_CONTROL_TRIGGER_MODE                                               (17u)
#define BITL_SPI_CONTROL_TRIGGER_MODE                                                (1u)
#define BITM_SPI_CONTROL_TRIGGER_MODE                                       (0x00020000u)
// Enums
#define ENUM_SPI_CONTROL_TRIGGER_MODE_MANUAL                                (0x00000000u)
#define ENUM_SPI_CONTROL_TRIGGER_MODE_AUTOMATIC                             (0x00000001u)

// Bit field: manual_chip_select
// Bit position, length and mask
#define BITP_SPI_CONTROL_MANUAL_CHIP_SELECT                                         (19u)
#define BITL_SPI_CONTROL_MANUAL_CHIP_SELECT                                          (1u)
#define BITM_SPI_CONTROL_MANUAL_CHIP_SELECT                                 (0x00080000u)
// Enums
#define ENUM_SPI_CONTROL_MANUAL_CHIP_SELECT_AUTOMATIC                       (0x00000001u)
#define ENUM_SPI_CONTROL_MANUAL_CHIP_SELECT_MANUAL                          (0x00000000u)

// Bit field: loop
// Bit position, length and mask
#define BITP_SPI_CONTROL_LOOP                                                       (20u)
#define BITL_SPI_CONTROL_LOOP                                                        (1u)
#define BITM_SPI_CONTROL_LOOP                                               (0x00100000u)
// Enums
#define ENUM_SPI_CONTROL_LOOP_NORMAL                                        (0x00000000u)
#define ENUM_SPI_CONTROL_LOOP_LOOPBACK                                      (0x00000001u)

// Bit field: spi_enable
// Bit position, length and mask
#define BITP_SPI_CONTROL_SPI_ENABLE                                                 (21u)
#define BITL_SPI_CONTROL_SPI_ENABLE                                                  (1u)
#define BITM_SPI_CONTROL_SPI_ENABLE                                         (0x00200000u)
// Enums
#define ENUM_SPI_CONTROL_SPI_ENABLE_DISABLE                                 (0x00000000u)
#define ENUM_SPI_CONTROL_SPI_ENABLE_ENABLE                                  (0x00000001u)

// Bit field: controller
// Bit position, length and mask
#define BITP_SPI_CONTROL_CONTROLLER                                                 (22u)
#define BITL_SPI_CONTROL_CONTROLLER                                                  (1u)
#define BITM_SPI_CONTROL_CONTROLLER                                         (0x00400000u)
// Enums
#define ENUM_SPI_CONTROL_CONTROLLER_IDLE                                    (0x00000000u)
#define ENUM_SPI_CONTROL_CONTROLLER_MASTER                                  (0x00000001u)

// Bit field: lsb_first
// Bit position, length and mask
#define BITP_SPI_CONTROL_LSB_FIRST                                                  (23u)
#define BITL_SPI_CONTROL_LSB_FIRST                                                   (1u)
#define BITM_SPI_CONTROL_LSB_FIRST                                          (0x00800000u)
// Enums
#define ENUM_SPI_CONTROL_LSB_FIRST_MSB_FIRST                                (0x00000000u)
#define ENUM_SPI_CONTROL_LSB_FIRST_LSB_FIRST                                (0x00000001u)

// Bit field: cpol
// Bit position, length and mask
#define BITP_SPI_CONTROL_CPOL                                                       (24u)
#define BITL_SPI_CONTROL_CPOL                                                        (1u)
#define BITM_SPI_CONTROL_CPOL                                               (0x01000000u)
// Enums
#define ENUM_SPI_CONTROL_CPOL_ACTIVE_HIGH                                   (0x00000000u)
#define ENUM_SPI_CONTROL_CPOL_ACTIVE_LOW                                    (0x00000001u)

// Bit field: cpha
// Bit position, length and mask
#define BITP_SPI_CONTROL_CPHA                                                       (25u)
#define BITL_SPI_CONTROL_CPHA                                                        (1u)
#define BITM_SPI_CONTROL_CPHA                                               (0x02000000u)
// Enums
#define ENUM_SPI_CONTROL_CPHA_ACTIVE_HIGH                                   (0x00000000u)
#define ENUM_SPI_CONTROL_CPHA_ACTIVE_LOW                                    (0x00000001u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_SPI_CONTROL_TX_FIFO_RESET                                              (26u)
#define BITL_SPI_CONTROL_TX_FIFO_RESET                                               (1u)
#define BITM_SPI_CONTROL_TX_FIFO_RESET                                      (0x04000000u)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_SPI_CONTROL_RX_FIFO_RESET                                              (27u)
#define BITL_SPI_CONTROL_RX_FIFO_RESET                                               (1u)
#define BITM_SPI_CONTROL_RX_FIFO_RESET                                      (0x08000000u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_SPI_CONTROL_SOFT_RESET                                                 (28u)
#define BITL_SPI_CONTROL_SOFT_RESET                                                  (4u)
#define BITM_SPI_CONTROL_SOFT_RESET                                         (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Length
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define SPI_LENGTH_ADDR                                                         (0x0004u)
#define SPI_LENGTH_NW_ADDR                                                      (0x0004u)
#define SPI_LENGTH_RESET_VAL                                                (0x00000000u)
#define SPI_LENGTH_MMS                                                               (0u)
#define SPI_LENGTH_VOL_MASK                                                 (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | manual_ignore_rd_bytes   | When manual length is selected    | 0x00000000  |
// |       |                          | in a SPI command, the number of   |             |
// |       |                          | ignorable octets of the           |             |
// |       |                          | transaction that shall not be     |             |
// |       |                          | written to the receive FIFO.      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-10 | manual_rd_bytes          | When manual length is selected    | 0x00000000  |
// |       |                          | in a SPI command, the amount of   |             |
// |       |                          | read octets shall use this        |             |
// |       |                          | register.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-21 | manual_wr_bytes          | When manual length is selected    | 0x00000000  |
// |       |                          | in a SPI command, the amount of   |             |
// |       |                          | write octets shall use from this  |             |
// |       |                          | register                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: manual_ignore_rd_bytes
// Bit position, length and mask
#define BITP_SPI_LENGTH_MANUAL_IGNORE_RD_BYTES                                       (0u)
#define BITL_SPI_LENGTH_MANUAL_IGNORE_RD_BYTES                                      (10u)
#define BITM_SPI_LENGTH_MANUAL_IGNORE_RD_BYTES                              (0x000003FFu)

// Bit field: manual_rd_bytes
// Bit position, length and mask
#define BITP_SPI_LENGTH_MANUAL_RD_BYTES                                             (10u)
#define BITL_SPI_LENGTH_MANUAL_RD_BYTES                                             (11u)
#define BITM_SPI_LENGTH_MANUAL_RD_BYTES                                     (0x001FFC00u)

// Bit field: manual_wr_bytes
// Bit position, length and mask
#define BITP_SPI_LENGTH_MANUAL_WR_BYTES                                             (21u)
#define BITL_SPI_LENGTH_MANUAL_WR_BYTES                                             (11u)
#define BITM_SPI_LENGTH_MANUAL_WR_BYTES                                     (0xFFE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Spacing
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x01600802u
// Register MMS         : 0u

#define SPI_SPACING_ADDR                                                        (0x0008u)
#define SPI_SPACING_NW_ADDR                                                     (0x0008u)
#define SPI_SPACING_RESET_VAL                                               (0x01600802u)
#define SPI_SPACING_MMS                                                              (0u)
#define SPI_SPACING_VOL_MASK                                                (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | csb_to_sclk              | Amount of clock cycles (SCLK)     | 0x00000002  |
// |       |                          | that the bus shall be in IDLE     |             |
// |       |                          | state after asserting CSB before  |             |
// |       |                          | starting an SPI transaction.      |             |
// |       |                          | Minimum valid value of 1.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-10 | sclk_to_csb              | Amount of clock cycles (SCLK)     | 0x00000002  |
// |       |                          | that the bus shall be in IDLE     |             |
// |       |                          | state before de-asserting CSB     |             |
// |       |                          | after an SPI transaction.         |             |
// |       |                          | Minimum valid value of 1.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-20 | csb_spacing              | Amount of clock cycles (SCLK)     | 0x00000016  |
// |       |                          | that the bus shall be in IDLE     |             |
// |       |                          | state after de-asserting CSb and  |             |
// |       |                          | before asserting CSb again.       |             |
// |       |                          | Minimum valid value of 1. NOTE:   |             |
// |       |                          | The real spacing is the           |             |
// |       |                          | configured time minus half an     |             |
// |       |                          | SCLK cycle.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: csb_to_sclk
// Bit position, length and mask
#define BITP_SPI_SPACING_CSB_TO_SCLK                                                 (0u)
#define BITL_SPI_SPACING_CSB_TO_SCLK                                                (10u)
#define BITM_SPI_SPACING_CSB_TO_SCLK                                        (0x000003FFu)

// Bit field: sclk_to_csb
// Bit position, length and mask
#define BITP_SPI_SPACING_SCLK_TO_CSB                                                (10u)
#define BITL_SPI_SPACING_SCLK_TO_CSB                                                (10u)
#define BITM_SPI_SPACING_SCLK_TO_CSB                                        (0x000FFC00u)

// Bit field: csb_spacing
// Bit position, length and mask
#define BITP_SPI_SPACING_CSB_SPACING                                                (20u)
#define BITL_SPI_SPACING_CSB_SPACING                                                (12u)
#define BITM_SPI_SPACING_CSB_SPACING                                        (0xFFF00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : stats
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define SPI_STATS_ADDR                                                          (0x000Cu)
#define SPI_STATS_NW_ADDR                                                       (0x000Cu)
#define SPI_STATS_RESET_VAL                                                 (0x00000000u)
#define SPI_STATS_MMS                                                                (0u)
#define SPI_STATS_VOL_MASK                                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | num_rd_octets            | Count of non-ignored octets from  | 0x00000000  |
// |       |                          | the interface MOSI pin.           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | num_transactions         | Number of SPI transactions.       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_rd_octets
// Bit position, length and mask
#define BITP_SPI_STATS_NUM_RD_OCTETS                                                 (0u)
#define BITL_SPI_STATS_NUM_RD_OCTETS                                                (16u)
#define BITM_SPI_STATS_NUM_RD_OCTETS                                        (0x0000FFFFu)

// Bit field: num_transactions
// Bit position, length and mask
#define BITP_SPI_STATS_NUM_TRANSACTIONS                                             (16u)
#define BITL_SPI_STATS_NUM_TRANSACTIONS                                             (16u)
#define BITM_SPI_STATS_NUM_TRANSACTIONS                                     (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : stats2
// Register Addr        : 0x0010u
// Register NW Addr     : 0x0010u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define SPI_STATS2_ADDR                                                         (0x0010u)
#define SPI_STATS2_NW_ADDR                                                      (0x0010u)
#define SPI_STATS2_RESET_VAL                                                (0x00000000u)
#define SPI_STATS2_MMS                                                               (0u)
#define SPI_STATS2_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | num_wr_octets            | Count of output octets to the     | 0x00000000  |
// |       |                          | interface pin MISO pin.           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_wr_octets
// Bit position, length and mask
#define BITP_SPI_STATS2_NUM_WR_OCTETS                                                (0u)
#define BITL_SPI_STATS2_NUM_WR_OCTETS                                               (32u)
#define BITM_SPI_STATS2_NUM_WR_OCTETS                                       (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Status
// Register Addr        : 0x0014u
// Register NW Addr     : 0x0014u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define SPI_STATUS_ADDR                                                         (0x0014u)
#define SPI_STATUS_NW_ADDR                                                      (0x0014u)
#define SPI_STATUS_RESET_VAL                                                (0x00000000u)
#define SPI_STATUS_MMS                                                               (0u)
#define SPI_STATUS_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | bb                       | Indicates if the interface is     | 0x00000000  |
// |       |                          | currently processing a            |             |
// |       |                          | transaction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: bb
// Bit position, length and mask
#define BITP_SPI_STATUS_BB                                                           (0u)
#define BITL_SPI_STATUS_BB                                                           (1u)
#define BITM_SPI_STATUS_BB                                                  (0x00000001u)
// Enums
#define ENUM_SPI_STATUS_BB_FREE                                             (0x00000000u)
#define ENUM_SPI_STATUS_BB_BUSY                                             (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Baud_rate
// Register Addr        : 0x0018u
// Register NW Addr     : 0x0018u
// Register Reset Value : 0x000000C8u
// Register MMS         : 0u

#define SPI_BAUD_RATE_ADDR                                                      (0x0018u)
#define SPI_BAUD_RATE_NW_ADDR                                                   (0x0018u)
#define SPI_BAUD_RATE_RESET_VAL                                             (0x000000C8u)
#define SPI_BAUD_RATE_MMS                                                            (0u)
#define SPI_BAUD_RATE_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 20-00 | baud_rate_period         | SCLK period in nanoseconds. From  | 0x000000c8  |
// |       |                          | 100 ns to max value.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-21 | sampling_delay           | Specified a sampling delay after  | 0x00000000  |
// |       |                          | the sampling edge in              |             |
// |       |                          | nanoseconds.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: baud_rate_period
// Bit position, length and mask
#define BITP_SPI_BAUD_RATE_BAUD_RATE_PERIOD                                          (0u)
#define BITL_SPI_BAUD_RATE_BAUD_RATE_PERIOD                                         (21u)
#define BITM_SPI_BAUD_RATE_BAUD_RATE_PERIOD                                 (0x001FFFFFu)

// Bit field: sampling_delay
// Bit position, length and mask
#define BITP_SPI_BAUD_RATE_SAMPLING_DELAY                                           (21u)
#define BITL_SPI_BAUD_RATE_SAMPLING_DELAY                                           (11u)
#define BITM_SPI_BAUD_RATE_SAMPLING_DELAY                                   (0xFFE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Nrz_high_active_time
// Register Addr        : 0x001Cu
// Register NW Addr     : 0x001Cu
// Register Reset Value : 0x000000C8u
// Register MMS         : 0u

#define SPI_NRZ_HIGH_ACTIVE_TIME_ADDR                                           (0x001Cu)
#define SPI_NRZ_HIGH_ACTIVE_TIME_NW_ADDR                                        (0x001Cu)
#define SPI_NRZ_HIGH_ACTIVE_TIME_RESET_VAL                                  (0x000000C8u)
#define SPI_NRZ_HIGH_ACTIVE_TIME_MMS                                                 (0u)
#define SPI_NRZ_HIGH_ACTIVE_TIME_VOL_MASK                                   (0x001FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 20-00 | nrz_high_active_time     | SCLK period in steps of 10        | 0x000000c8  |
// |       |                          | nanoseconds. From 100 ns to max   |             |
// |       |                          | value.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: nrz_high_active_time
// Bit position, length and mask
#define BITP_SPI_NRZ_HIGH_ACTIVE_TIME_NRZ_HIGH_ACTIVE_TIME                           (0u)
#define BITL_SPI_NRZ_HIGH_ACTIVE_TIME_NRZ_HIGH_ACTIVE_TIME                          (21u)
#define BITM_SPI_NRZ_HIGH_ACTIVE_TIME_NRZ_HIGH_ACTIVE_TIME                  (0x001FFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Nrz_low_active_time
// Register Addr        : 0x0020u
// Register NW Addr     : 0x0020u
// Register Reset Value : 0x000000C8u
// Register MMS         : 0u

#define SPI_NRZ_LOW_ACTIVE_TIME_ADDR                                            (0x0020u)
#define SPI_NRZ_LOW_ACTIVE_TIME_NW_ADDR                                         (0x0020u)
#define SPI_NRZ_LOW_ACTIVE_TIME_RESET_VAL                                   (0x000000C8u)
#define SPI_NRZ_LOW_ACTIVE_TIME_MMS                                                  (0u)
#define SPI_NRZ_LOW_ACTIVE_TIME_VOL_MASK                                    (0x001FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 20-00 | nrz_low_active_time      | SCLK period in steps of 10        | 0x000000c8  |
// |       |                          | nanoseconds. From 100 ns to max   |             |
// |       |                          | value.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: nrz_low_active_time
// Bit position, length and mask
#define BITP_SPI_NRZ_LOW_ACTIVE_TIME_NRZ_LOW_ACTIVE_TIME                             (0u)
#define BITL_SPI_NRZ_LOW_ACTIVE_TIME_NRZ_LOW_ACTIVE_TIME                            (21u)
#define BITM_SPI_NRZ_LOW_ACTIVE_TIME_NRZ_LOW_ACTIVE_TIME                    (0x001FFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Spacing_2
// Register Addr        : 0x0024u
// Register NW Addr     : 0x0024u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define SPI_SPACING_2_ADDR                                                      (0x0024u)
#define SPI_SPACING_2_NW_ADDR                                                   (0x0024u)
#define SPI_SPACING_2_RESET_VAL                                             (0x00000000u)
#define SPI_SPACING_2_MMS                                                            (0u)
#define SPI_SPACING_2_VOL_MASK                                              (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | interbyte_spacing        | Time the bus will be IDLE state   | 0x00000000  |
// |       |                          | after the transmission of one     |             |
// |       |                          | byte, the unit of this control    |             |
// |       |                          | is 10 ns                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: interbyte_spacing
// Bit position, length and mask
#define BITP_SPI_SPACING_2_INTERBYTE_SPACING                                         (0u)
#define BITL_SPI_SPACING_2_INTERBYTE_SPACING                                        (24u)
#define BITM_SPI_SPACING_2_INTERBYTE_SPACING                                (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Control2
// Register Addr        : 0x0028u
// Register NW Addr     : 0x0028u
// Register Reset Value : 0x00000100u
// Register MMS         : 0u

#define SPI_CONTROL2_ADDR                                                       (0x0028u)
#define SPI_CONTROL2_NW_ADDR                                                    (0x0028u)
#define SPI_CONTROL2_RESET_VAL                                              (0x00000100u)
#define SPI_CONTROL2_MMS                                                             (0u)
#define SPI_CONTROL2_VOL_MASK                                               (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | CSb_polarity             | Chip Select polarity. The Cs      | 0x00000000  |
// |       |                          | polarity of each bit position     |             |
// |       |                          | with a one will be inverted       |             |
// |       |                          | (active high)                     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-08 | num_packed_transactions  | Defines the number of SPI         | 0x00000001  |
// |       |                          | transactions that are packed      |             |
// |       |                          | between headers when              |             |
// |       |                          | transaction_header_enable is set  |             |
// |       |                          | to 1'b1                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CSb_polarity
// Bit position, length and mask
#define BITP_SPI_CONTROL2_CSB_POLARITY                                               (0u)
#define BITL_SPI_CONTROL2_CSB_POLARITY                                               (8u)
#define BITM_SPI_CONTROL2_CSB_POLARITY                                      (0x000000FFu)

// Bit field: num_packed_transactions
// Bit position, length and mask
#define BITP_SPI_CONTROL2_NUM_PACKED_TRANSACTIONS                                    (8u)
#define BITL_SPI_CONTROL2_NUM_PACKED_TRANSACTIONS                                   (16u)
#define BITM_SPI_CONTROL2_NUM_PACKED_TRANSACTIONS                           (0x00FFFF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillSPIRegsData();
#endif


#endif /* ADI_E2BCORE_REGSPI_H */

/** @} */

/*
 * EOF: www.analog.com
 */


