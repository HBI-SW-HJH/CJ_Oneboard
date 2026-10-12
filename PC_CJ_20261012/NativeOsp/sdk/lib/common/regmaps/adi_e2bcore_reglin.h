/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_reglin.h
 * @brief        : This is an auto generated header file with register
                   descriptions for LIN interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGLIN_H
#define ADI_E2BCORE_REGLIN_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define LIN_NUM_REGS    (6u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganLINRegAddr[LIN_NUM_REGS];
extern uint32_t ganLINNwRegAddr[LIN_NUM_REGS];
extern uint32_t ganLINRegMMS[LIN_NUM_REGS];
extern uint32_t ganLINRegResetVal[LIN_NUM_REGS];
extern const char *gasLINRegName[LIN_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Control
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00068800u
// Register MMS         : 0u

#define LIN_CONTROL_ADDR                                                        (0x0000u)
#define LIN_CONTROL_NW_ADDR                                                     (0x0000u)
#define LIN_CONTROL_RESET_VAL                                               (0x00068800u)
#define LIN_CONTROL_MMS                                                              (0u)
#define LIN_CONTROL_VOL_MASK                                                (0xFC2FF801u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | wakeup_slaves            | Generates a wake-up signal. One   | 0x00000000  |
// |       |                          | wake-up pulse per trigger.        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | resynchronization_require| Executes the resynchronization    | 0x00000000  | 
// |       |                          | procedure. Please, check the      |             |
// |       |                          | user guide of the interface for   |             |
// |       |                          | more information about this       |             |
// |       |                          | procedure.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | calculate_checksum       | The LIN interface can compute     | 0x00000000  |
// |       |                          | and add the checksum.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | enable_timebase          | Sets the time base mode.          | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | checksum_mode            | Configures how the LIN interface  | 0x00000001  |
// |       |                          | computes the checksum, if         |             |
// |       |                          | required.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | enable_read_time_tag     | If enabled, the interface will    | 0x00000000  |
// |       |                          | tag the LIN transaction           |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | lin_enable               | Enables or disables the LIN       | 0x00000000  |
// |       |                          | interface.                        |             |
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

// Bit field: wakeup_slaves
// Bit position, length and mask
#define BITP_LIN_CONTROL_WAKEUP_SLAVES                                               (0u)
#define BITL_LIN_CONTROL_WAKEUP_SLAVES                                               (1u)
#define BITM_LIN_CONTROL_WAKEUP_SLAVES                                      (0x00000001u)

// Bit field: resynchronization_required
// Bit position, length and mask
#define BITP_LIN_CONTROL_RESYNCHRONIZATION_REQUIRED                                 (12u)
#define BITL_LIN_CONTROL_RESYNCHRONIZATION_REQUIRED                                  (1u)
#define BITM_LIN_CONTROL_RESYNCHRONIZATION_REQUIRED                         (0x00001000u)

// Bit field: calculate_checksum
// Bit position, length and mask
#define BITP_LIN_CONTROL_CALCULATE_CHECKSUM                                         (16u)
#define BITL_LIN_CONTROL_CALCULATE_CHECKSUM                                          (1u)
#define BITM_LIN_CONTROL_CALCULATE_CHECKSUM                                 (0x00010000u)
// Enums
#define ENUM_LIN_CONTROL_CALCULATE_CHECKSUM_BY_THE_ECU                      (0x00000000u)
#define ENUM_LIN_CONTROL_CALCULATE_CHECKSUM_BY_THE_INTERFACE                (0x00000001u)

// Bit field: enable_timebase
// Bit position, length and mask
#define BITP_LIN_CONTROL_ENABLE_TIMEBASE                                            (17u)
#define BITL_LIN_CONTROL_ENABLE_TIMEBASE                                             (1u)
#define BITM_LIN_CONTROL_ENABLE_TIMEBASE                                    (0x00020000u)
// Enums
#define ENUM_LIN_CONTROL_ENABLE_TIMEBASE_INSTANTANEOUS                      (0x00000000u)
#define ENUM_LIN_CONTROL_ENABLE_TIMEBASE_TIME_BASE                          (0x00000001u)

// Bit field: checksum_mode
// Bit position, length and mask
#define BITP_LIN_CONTROL_CHECKSUM_MODE                                              (18u)
#define BITL_LIN_CONTROL_CHECKSUM_MODE                                               (1u)
#define BITM_LIN_CONTROL_CHECKSUM_MODE                                      (0x00040000u)
// Enums
#define ENUM_LIN_CONTROL_CHECKSUM_MODE_CLASSIC                              (0x00000000u)
#define ENUM_LIN_CONTROL_CHECKSUM_MODE_ENHANCED                             (0x00000001u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_LIN_CONTROL_ENABLE_READ_TIME_TAG                                       (19u)
#define BITL_LIN_CONTROL_ENABLE_READ_TIME_TAG                                        (1u)
#define BITM_LIN_CONTROL_ENABLE_READ_TIME_TAG                               (0x00080000u)
// Enums
#define ENUM_LIN_CONTROL_ENABLE_READ_TIME_TAG_DISABLE                       (0x00000000u)
#define ENUM_LIN_CONTROL_ENABLE_READ_TIME_TAG_ENABE                         (0x00000001u)

// Bit field: lin_enable
// Bit position, length and mask
#define BITP_LIN_CONTROL_LIN_ENABLE                                                 (21u)
#define BITL_LIN_CONTROL_LIN_ENABLE                                                  (1u)
#define BITM_LIN_CONTROL_LIN_ENABLE                                         (0x00200000u)
// Enums
#define ENUM_LIN_CONTROL_LIN_ENABLE_DISABLE                                 (0x00000000u)
#define ENUM_LIN_CONTROL_LIN_ENABLE_ENABLE                                  (0x00000001u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_LIN_CONTROL_TX_FIFO_RESET                                              (26u)
#define BITL_LIN_CONTROL_TX_FIFO_RESET                                               (1u)
#define BITM_LIN_CONTROL_TX_FIFO_RESET                                      (0x04000000u)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_LIN_CONTROL_RX_FIFO_RESET                                              (27u)
#define BITL_LIN_CONTROL_RX_FIFO_RESET                                               (1u)
#define BITM_LIN_CONTROL_RX_FIFO_RESET                                      (0x08000000u)

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_LIN_CONTROL_SOFT_RESET                                                 (28u)
#define BITL_LIN_CONTROL_SOFT_RESET                                                  (4u)
#define BITM_LIN_CONTROL_SOFT_RESET                                         (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Timing
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x27100032u
// Register MMS         : 0u

#define LIN_TIMING_ADDR                                                         (0x0004u)
#define LIN_TIMING_NW_ADDR                                                      (0x0004u)
#define LIN_TIMING_RESET_VAL                                                (0x27100032u)
#define LIN_TIMING_MMS                                                               (0u)
#define LIN_TIMING_VOL_MASK                                                 (0xFFFE03FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | tbit                     | Bitrate of the lin transaction.   | 0x00000032  |
// |       |                          | Specified in us with valid        |             |
// |       |                          | values are from 50 us to 1000     |             |
// |       |                          | us.                               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-17 | tbase                    | The time base is implemented in   | 0x00001388  |
// |       |                          | the controller node and is used   |             |
// |       |                          | to control the timing of the      |             |
// |       |                          | schedule table. This means that   |             |
// |       |                          | the timing for the frames in a    |             |
// |       |                          | schedule table is based upon the  |             |
// |       |                          | time base. Specified in us with   |             |
// |       |                          | typical values between 5 to 50    |             |
// |       |                          | ms.                               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tbit
// Bit position, length and mask
#define BITP_LIN_TIMING_TBIT                                                         (0u)
#define BITL_LIN_TIMING_TBIT                                                        (10u)
#define BITM_LIN_TIMING_TBIT                                                (0x000003FFu)

// Bit field: tbase
// Bit position, length and mask
#define BITP_LIN_TIMING_TBASE                                                       (17u)
#define BITL_LIN_TIMING_TBASE                                                       (15u)
#define BITM_LIN_TIMING_TBASE                                               (0xFFFE0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Timing2
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x68800000u
// Register MMS         : 0u

#define LIN_TIMING2_ADDR                                                        (0x0008u)
#define LIN_TIMING2_NW_ADDR                                                     (0x0008u)
#define LIN_TIMING2_RESET_VAL                                               (0x68800000u)
#define LIN_TIMING2_MMS                                                              (0u)
#define LIN_TIMING2_VOL_MASK                                                (0xFF80FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | tjitter                  | Specifies the time difference     | 0x00000000  |
// |       |                          | from time base tick to the        |             |
// |       |                          | header sending start point        |             |
// |       |                          | (falling edge of break field).    |             |
// |       |                          | Must be a multiple of 10ns.       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tjitter
// Bit position, length and mask
#define BITP_LIN_TIMING2_TJITTER                                                     (0u)
#define BITL_LIN_TIMING2_TJITTER                                                    (16u)
#define BITM_LIN_TIMING2_TJITTER                                            (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Timing3
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x09C40019u
// Register MMS         : 0u

#define LIN_TIMING3_ADDR                                                        (0x000Cu)
#define LIN_TIMING3_NW_ADDR                                                     (0x000Cu)
#define LIN_TIMING3_RESET_VAL                                               (0x09C40019u)
#define LIN_TIMING3_MMS                                                              (0u)
#define LIN_TIMING3_VOL_MASK                                                (0xFFFE03FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | sampling_bit_time        | Relative time from the falling    | 0x00000019  |
// |       |                          | edge of the start bit to the      |             |
// |       |                          | sampling point in us. Must be     |             |
// |       |                          | between 7/16 and 10/16 of Tbit.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-17 | wakeup_pulse_time        | Time that the bus will be held    | 0x000004e2  |
// |       |                          | low to generate a wakeup pulse    |             |
// |       |                          | in 10 ns units.                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: sampling_bit_time
// Bit position, length and mask
#define BITP_LIN_TIMING3_SAMPLING_BIT_TIME                                           (0u)
#define BITL_LIN_TIMING3_SAMPLING_BIT_TIME                                          (10u)
#define BITM_LIN_TIMING3_SAMPLING_BIT_TIME                                  (0x000003FFu)

// Bit field: wakeup_pulse_time
// Bit position, length and mask
#define BITP_LIN_TIMING3_WAKEUP_PULSE_TIME                                          (17u)
#define BITL_LIN_TIMING3_WAKEUP_PULSE_TIME                                          (15u)
#define BITM_LIN_TIMING3_WAKEUP_PULSE_TIME                                  (0xFFFE0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Status
// Register Addr        : 0x0014u
// Register NW Addr     : 0x0014u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LIN_STATUS_ADDR                                                         (0x0014u)
#define LIN_STATUS_NW_ADDR                                                      (0x0014u)
#define LIN_STATUS_RESET_VAL                                                (0x00000000u)
#define LIN_STATUS_MMS                                                               (0u)
#define LIN_STATUS_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | bb                       | Indicates if the interface is     | 0x00000000  |
// |       |                          | currently processing a            |             |
// |       |                          | transaction.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | detected_wakeup_signal   | Toggling signal. On toggle it     | 0x00000000  |
// |       |                          | has detected a wake-up signal.    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | frame_response_too_short | Indicates that during the last    | 0x00000000  |
// |       |                          | transaction, the frame was too    |             |
// |       |                          | short.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | framing_error            | Indicates that during the last    | 0x00000000  |
// |       |                          | transaction, there was a framing  |             |
// |       |                          | error.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | tx_data_contention       | Indicates that during the last    | 0x00000000  |
// |       |                          | transaction, there was a          |             |
// |       |                          | contention issue during the data  |             |
// |       |                          | bytes.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | pid_field_contention     | Indicates that during the last    | 0x00000000  |
// |       |                          | transaction, there was a          |             |
// |       |                          | contention issue during the PID   |             |
// |       |                          | field.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | sync_field_contention    | Indicates that during the last    | 0x00000000  |
// |       |                          | transaction, there was a          |             |
// |       |                          | contention issue during the SYNC  |             |
// |       |                          | field.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | break_field_contention   | Indicates that during the last    | 0x00000000  |
// |       |                          | transaction, there was a          |             |
// |       |                          | contention issue during the       |             |
// |       |                          | break field.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: bb
// Bit position, length and mask
#define BITP_LIN_STATUS_BB                                                           (0u)
#define BITL_LIN_STATUS_BB                                                           (1u)
#define BITM_LIN_STATUS_BB                                                  (0x00000001u)
// Enums
#define ENUM_LIN_STATUS_BB_FREE                                             (0x00000000u)
#define ENUM_LIN_STATUS_BB_BUSY                                             (0x00000001u)

// Bit field: detected_wakeup_signal
// Bit position, length and mask
#define BITP_LIN_STATUS_DETECTED_WAKEUP_SIGNAL                                       (1u)
#define BITL_LIN_STATUS_DETECTED_WAKEUP_SIGNAL                                       (1u)
#define BITM_LIN_STATUS_DETECTED_WAKEUP_SIGNAL                              (0x00000002u)

// Bit field: frame_response_too_short
// Bit position, length and mask
#define BITP_LIN_STATUS_FRAME_RESPONSE_TOO_SHORT                                     (2u)
#define BITL_LIN_STATUS_FRAME_RESPONSE_TOO_SHORT                                     (1u)
#define BITM_LIN_STATUS_FRAME_RESPONSE_TOO_SHORT                            (0x00000004u)

// Bit field: framing_error
// Bit position, length and mask
#define BITP_LIN_STATUS_FRAMING_ERROR                                                (3u)
#define BITL_LIN_STATUS_FRAMING_ERROR                                                (1u)
#define BITM_LIN_STATUS_FRAMING_ERROR                                       (0x00000008u)

// Bit field: tx_data_contention
// Bit position, length and mask
#define BITP_LIN_STATUS_TX_DATA_CONTENTION                                           (4u)
#define BITL_LIN_STATUS_TX_DATA_CONTENTION                                           (1u)
#define BITM_LIN_STATUS_TX_DATA_CONTENTION                                  (0x00000010u)

// Bit field: pid_field_contention
// Bit position, length and mask
#define BITP_LIN_STATUS_PID_FIELD_CONTENTION                                         (5u)
#define BITL_LIN_STATUS_PID_FIELD_CONTENTION                                         (1u)
#define BITM_LIN_STATUS_PID_FIELD_CONTENTION                                (0x00000020u)

// Bit field: sync_field_contention
// Bit position, length and mask
#define BITP_LIN_STATUS_SYNC_FIELD_CONTENTION                                        (6u)
#define BITL_LIN_STATUS_SYNC_FIELD_CONTENTION                                        (1u)
#define BITM_LIN_STATUS_SYNC_FIELD_CONTENTION                               (0x00000040u)

// Bit field: break_field_contention
// Bit position, length and mask
#define BITP_LIN_STATUS_BREAK_FIELD_CONTENTION                                       (7u)
#define BITL_LIN_STATUS_BREAK_FIELD_CONTENTION                                       (1u)
#define BITM_LIN_STATUS_BREAK_FIELD_CONTENTION                              (0x00000080u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : Control2
// Register Addr        : 0x0018u
// Register NW Addr     : 0x0018u
// Register Reset Value : 0x00000001u
// Register MMS         : 0u

#define LIN_CONTROL2_ADDR                                                       (0x0018u)
#define LIN_CONTROL2_NW_ADDR                                                    (0x0018u)
#define LIN_CONTROL2_RESET_VAL                                              (0x00000001u)
#define LIN_CONTROL2_MMS                                                             (0u)
#define LIN_CONTROL2_VOL_MASK                                               (0x0000FFFFu)

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
#define BITP_LIN_CONTROL2_NUM_PACKED_TRANSACTIONS                                    (0u)
#define BITL_LIN_CONTROL2_NUM_PACKED_TRANSACTIONS                                   (16u)
#define BITM_LIN_CONTROL2_NUM_PACKED_TRANSACTIONS                           (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillLINRegsData();
#endif


#endif /* ADI_E2BCORE_REGLIN_H */

/** @} */

/*
 * EOF: www.analog.com
 */


