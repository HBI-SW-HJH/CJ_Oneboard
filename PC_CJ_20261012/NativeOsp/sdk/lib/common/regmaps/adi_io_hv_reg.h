/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_io_hv_reg.h
 * @brief        : This is an auto generated header file with register
                   descriptions for IO HV
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_IO_HV_REG_H
#define ADI_IO_HV_REG_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define IO_HV_NUM_REGS    (11u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganIO_HVRegAddr[IO_HV_NUM_REGS];
extern uint32_t ganIO_HVNwRegAddr[IO_HV_NUM_REGS];
extern uint32_t ganIO_HVRegMMS[IO_HV_NUM_REGS];
extern uint32_t ganIO_HVRegResetVal[IO_HV_NUM_REGS];
extern const char *gasIO_HVRegName[IO_HV_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : revision_control
// Register Addr        : 0xB105u
// Register NW Addr     : 0xB105u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_REVISION_CONTROL_ADDR                                             (0xB105u)
#define IO_HV_REVISION_CONTROL_NW_ADDR                                          (0xB105u)
#define IO_HV_REVISION_CONTROL_RESET_VAL                                    (0x00000000u)
#define IO_HV_REVISION_CONTROL_MMS                                                 (0xAu)
#define IO_HV_REVISION_CONTROL_VOL_MASK                                     (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | rev_code                 | Chip revision code                | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-04 | rev_code_dig             | Digital revision code             | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: rev_code
// Bit position, length and mask
#define BITP_IO_HV_REVISION_CONTROL_REV_CODE                                         (0u)
#define BITL_IO_HV_REVISION_CONTROL_REV_CODE                                         (4u)
#define BITM_IO_HV_REVISION_CONTROL_REV_CODE                                (0x0000000Fu)
// Enums
#define ENUM_IO_HV_REVISION_CONTROL_REV_CODE_ENUM000                        (0x00000000u)
#define ENUM_IO_HV_REVISION_CONTROL_REV_CODE_ENUM003                        (0x00000003u)
#define ENUM_IO_HV_REVISION_CONTROL_REV_CODE_ENUM004                        (0x00000004u)

// Bit field: rev_code_dig
// Bit position, length and mask
#define BITP_IO_HV_REVISION_CONTROL_REV_CODE_DIG                                     (4u)
#define BITL_IO_HV_REVISION_CONTROL_REV_CODE_DIG                                    (12u)
#define BITM_IO_HV_REVISION_CONTROL_REV_CODE_DIG                            (0x0000FFF0u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : bootloader_status_0
// Register Addr        : 0xB106u
// Register NW Addr     : 0xB106u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_BOOTLOADER_STATUS_0_ADDR                                          (0xB106u)
#define IO_HV_BOOTLOADER_STATUS_0_NW_ADDR                                       (0xB106u)
#define IO_HV_BOOTLOADER_STATUS_0_RESET_VAL                                 (0x00000000u)
#define IO_HV_BOOTLOADER_STATUS_0_MMS                                              (0xAu)
#define IO_HV_BOOTLOADER_STATUS_0_VOL_MASK                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | block_status_0           | Bootloader Block 0 and 1 Status,  | 0x00000000  |
// |       |                          | two bits starting from the LSBs.  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: block_status_0
// Bit position, length and mask
#define BITP_IO_HV_BOOTLOADER_STATUS_0_BLOCK_STATUS_0                                (0u)
#define BITL_IO_HV_BOOTLOADER_STATUS_0_BLOCK_STATUS_0                                (4u)
#define BITM_IO_HV_BOOTLOADER_STATUS_0_BLOCK_STATUS_0                       (0x0000000Fu)
// Enums
#define ENUM_IO_HV_BOOTLOADER_STATUS_0_BLOCK_STATUS_0_NOT_INITIALIZED       (0x00000000u)
#define ENUM_IO_HV_BOOTLOADER_STATUS_0_BLOCK_STATUS_0_CORRECT               (0x00000001u)
#define ENUM_IO_HV_BOOTLOADER_STATUS_0_BLOCK_STATUS_0_ECC_FIXED             (0x00000002u)
#define ENUM_IO_HV_BOOTLOADER_STATUS_0_BLOCK_STATUS_0_ERROR                 (0x00000003u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : standby_status
// Register Addr        : 0xB108u
// Register NW Addr     : 0xB108u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_STANDBY_STATUS_ADDR                                               (0xB108u)
#define IO_HV_STANDBY_STATUS_NW_ADDR                                            (0xB108u)
#define IO_HV_STANDBY_STATUS_RESET_VAL                                      (0x00000000u)
#define IO_HV_STANDBY_STATUS_MMS                                                   (0xAu)
#define IO_HV_STANDBY_STATUS_VOL_MASK                                       (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | detected_wake_pin        | Readback of a previous detection  | 0x00000000  |
// |       |                          | of the WAKE pin begin active      |             |
// |       |                          | (e.g. received during standby)    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | detected_wud             | Readback of a previous detection  | 0x00000000  |
// |       |                          | of the WUD begin active (e.g.     |             |
// |       |                          | received during standby)          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | wake_pin                 | Readback of the current (live)    | 0x00000000  |
// |       |                          | value of the WAKE pin (polarity   |             |
// |       |                          | corrected to active high)         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | wud                      | Readback of the current live      | 0x00000000  |
// |       |                          | output of the Wake-up Detect      |             |
// |       |                          | block                             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: detected_wake_pin
// Bit position, length and mask
#define BITP_IO_HV_STANDBY_STATUS_DETECTED_WAKE_PIN                                  (0u)
#define BITL_IO_HV_STANDBY_STATUS_DETECTED_WAKE_PIN                                  (1u)
#define BITM_IO_HV_STANDBY_STATUS_DETECTED_WAKE_PIN                         (0x00000001u)
// Enums
#define ENUM_IO_HV_STANDBY_STATUS_DETECTED_WAKE_PIN_ENUM000                 (0x00000000u)
#define ENUM_IO_HV_STANDBY_STATUS_DETECTED_WAKE_PIN_ENUM001                 (0x00000001u)

// Bit field: detected_wud
// Bit position, length and mask
#define BITP_IO_HV_STANDBY_STATUS_DETECTED_WUD                                       (1u)
#define BITL_IO_HV_STANDBY_STATUS_DETECTED_WUD                                       (1u)
#define BITM_IO_HV_STANDBY_STATUS_DETECTED_WUD                              (0x00000002u)
// Enums
#define ENUM_IO_HV_STANDBY_STATUS_DETECTED_WUD_ENUM000                      (0x00000000u)
#define ENUM_IO_HV_STANDBY_STATUS_DETECTED_WUD_ENUM001                      (0x00000001u)

// Bit field: wake_pin
// Bit position, length and mask
#define BITP_IO_HV_STANDBY_STATUS_WAKE_PIN                                           (2u)
#define BITL_IO_HV_STANDBY_STATUS_WAKE_PIN                                           (1u)
#define BITM_IO_HV_STANDBY_STATUS_WAKE_PIN                                  (0x00000004u)

// Bit field: wud
// Bit position, length and mask
#define BITP_IO_HV_STANDBY_STATUS_WUD                                                (3u)
#define BITL_IO_HV_STANDBY_STATUS_WUD                                                (1u)
#define BITM_IO_HV_STANDBY_STATUS_WUD                                       (0x00000008u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : bootloader_status_1
// Register Addr        : 0xB10Au
// Register NW Addr     : 0xB10Au
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_BOOTLOADER_STATUS_1_ADDR                                          (0xB10Au)
#define IO_HV_BOOTLOADER_STATUS_1_NW_ADDR                                       (0xB10Au)
#define IO_HV_BOOTLOADER_STATUS_1_RESET_VAL                                 (0x00000000u)
#define IO_HV_BOOTLOADER_STATUS_1_MMS                                              (0xAu)
#define IO_HV_BOOTLOADER_STATUS_1_VOL_MASK                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | bootload_done            | Bootloader done                   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | bootload_timeout         | Bootloader did not finish in      | 0x00000000  |
// |       |                          | 500ms. Bootloader aborted         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: bootload_done
// Bit position, length and mask
#define BITP_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE                                 (0u)
#define BITL_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE                                 (1u)
#define BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE                        (0x00000001u)
// Enums
#define ENUM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE_BOOTLOAD_NOT_FINISHED  (0x00000000u)
#define ENUM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE_BOOTLOAD_FINISHED      (0x00000001u)

// Bit field: bootload_timeout
// Bit position, length and mask
#define BITP_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT                              (1u)
#define BITL_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT                              (1u)
#define BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT                     (0x00000002u)
// Enums
#define ENUM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT_NOT_TRIGGERED       (0x00000000u)
#define ENUM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT_TRIGGERED           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : sleep_mode_control
// Register Addr        : 0xB14Au
// Register NW Addr     : 0xB14Au
// Register Reset Value : 0x00000004u
// Register MMS         : 0xAu

#define IO_HV_SLEEP_MODE_CONTROL_ADDR                                           (0xB14Au)
#define IO_HV_SLEEP_MODE_CONTROL_NW_ADDR                                        (0xB14Au)
#define IO_HV_SLEEP_MODE_CONTROL_RESET_VAL                                  (0x00000004u)
#define IO_HV_SLEEP_MODE_CONTROL_MMS                                               (0xAu)
#define IO_HV_SLEEP_MODE_CONTROL_VOL_MASK                                   (0x00000007u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | goto_sleep               | Sleep command forwarded to the    | 0x00000000  |
// |       |                          | sleep/wake always-on block        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | goto_standby             | Standby command applied directly  | 0x00000000  |
// |       |                          | from register map                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: goto_sleep
// Bit position, length and mask
#define BITP_IO_HV_SLEEP_MODE_CONTROL_GOTO_SLEEP                                     (0u)
#define BITL_IO_HV_SLEEP_MODE_CONTROL_GOTO_SLEEP                                     (1u)
#define BITM_IO_HV_SLEEP_MODE_CONTROL_GOTO_SLEEP                            (0x00000001u)
// Enums
#define ENUM_IO_HV_SLEEP_MODE_CONTROL_GOTO_SLEEP_INACTIVE                   (0x00000000u)
#define ENUM_IO_HV_SLEEP_MODE_CONTROL_GOTO_SLEEP_TRIGGER                    (0x00000001u)

// Bit field: goto_standby
// Bit position, length and mask
#define BITP_IO_HV_SLEEP_MODE_CONTROL_GOTO_STANDBY                                   (1u)
#define BITL_IO_HV_SLEEP_MODE_CONTROL_GOTO_STANDBY                                   (1u)
#define BITM_IO_HV_SLEEP_MODE_CONTROL_GOTO_STANDBY                          (0x00000002u)
// Enums
#define ENUM_IO_HV_SLEEP_MODE_CONTROL_GOTO_STANDBY_INACTIVE                 (0x00000000u)
#define ENUM_IO_HV_SLEEP_MODE_CONTROL_GOTO_STANDBY_TRIGGER                  (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : block_reset_control
// Register Addr        : 0xB1FEu
// Register NW Addr     : 0xB1FEu
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_BLOCK_RESET_CONTROL_ADDR                                          (0xB1FEu)
#define IO_HV_BLOCK_RESET_CONTROL_NW_ADDR                                       (0xB1FEu)
#define IO_HV_BLOCK_RESET_CONTROL_RESET_VAL                                 (0x00000000u)
#define IO_HV_BLOCK_RESET_CONTROL_MMS                                              (0xAu)
#define IO_HV_BLOCK_RESET_CONTROL_VOL_MASK                                  (0x00000007u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | otp_soft_reset           | OTP controller reset.             | 0x00000000  |
// |       |                          | Self-clearing bit.                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | boot_loader_soft_reset   | Bootloader reset. Self-clearing   | 0x00000000  |
// |       |                          | bit.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | clr_standby_status       | Clear the readback of a previous  | 0x00000000  |
// |       |                          | detection of the WUD or WAKE pin  |             |
// |       |                          | begin active (e.g. received       |             |
// |       |                          | during standby)                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: otp_soft_reset
// Bit position, length and mask
#define BITP_IO_HV_BLOCK_RESET_CONTROL_OTP_SOFT_RESET                                (0u)
#define BITL_IO_HV_BLOCK_RESET_CONTROL_OTP_SOFT_RESET                                (1u)
#define BITM_IO_HV_BLOCK_RESET_CONTROL_OTP_SOFT_RESET                       (0x00000001u)
// Enums
#define ENUM_IO_HV_BLOCK_RESET_CONTROL_OTP_SOFT_RESET_INACTIVE              (0x00000000u)
#define ENUM_IO_HV_BLOCK_RESET_CONTROL_OTP_SOFT_RESET_TRIGGER               (0x00000001u)

// Bit field: boot_loader_soft_reset
// Bit position, length and mask
#define BITP_IO_HV_BLOCK_RESET_CONTROL_BOOT_LOADER_SOFT_RESET                        (1u)
#define BITL_IO_HV_BLOCK_RESET_CONTROL_BOOT_LOADER_SOFT_RESET                        (1u)
#define BITM_IO_HV_BLOCK_RESET_CONTROL_BOOT_LOADER_SOFT_RESET               (0x00000002u)
// Enums
#define ENUM_IO_HV_BLOCK_RESET_CONTROL_BOOT_LOADER_SOFT_RESET_INACTIVE      (0x00000000u)
#define ENUM_IO_HV_BLOCK_RESET_CONTROL_BOOT_LOADER_SOFT_RESET_TRIGGER       (0x00000001u)

// Bit field: clr_standby_status
// Bit position, length and mask
#define BITP_IO_HV_BLOCK_RESET_CONTROL_CLR_STANDBY_STATUS                            (2u)
#define BITL_IO_HV_BLOCK_RESET_CONTROL_CLR_STANDBY_STATUS                            (1u)
#define BITM_IO_HV_BLOCK_RESET_CONTROL_CLR_STANDBY_STATUS                   (0x00000004u)
// Enums
#define ENUM_IO_HV_BLOCK_RESET_CONTROL_CLR_STANDBY_STATUS_INACTIVE          (0x00000000u)
#define ENUM_IO_HV_BLOCK_RESET_CONTROL_CLR_STANDBY_STATUS_TRIGGER           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : reset_control
// Register Addr        : 0xB1FFu
// Register NW Addr     : 0xB1FFu
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_RESET_CONTROL_ADDR                                                (0xB1FFu)
#define IO_HV_RESET_CONTROL_NW_ADDR                                             (0xB1FFu)
#define IO_HV_RESET_CONTROL_RESET_VAL                                       (0x00000000u)
#define IO_HV_RESET_CONTROL_MMS                                                    (0xAu)
#define IO_HV_RESET_CONTROL_VOL_MASK                                        (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | reset                    | Main soft reset signature. Must   | 0x00000000  |
// |       |                          | write a value of 0xA55A to this   |             |
// |       |                          | field to cause a soft reset on    |             |
// |       |                          | the HV die. Resets oscillator     |             |
// |       |                          | clock, communication clock and    |             |
// |       |                          | functional clock domains.         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: reset
// Bit position, length and mask
#define BITP_IO_HV_RESET_CONTROL_RESET                                               (0u)
#define BITL_IO_HV_RESET_CONTROL_RESET                                              (16u)
#define BITM_IO_HV_RESET_CONTROL_RESET                                      (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : high_speed_serial_comm
// Register Addr        : 0xB200u
// Register NW Addr     : 0xB200u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_HIGH_SPEED_SERIAL_COMM_ADDR                                       (0xB200u)
#define IO_HV_HIGH_SPEED_SERIAL_COMM_NW_ADDR                                    (0xB200u)
#define IO_HV_HIGH_SPEED_SERIAL_COMM_RESET_VAL                              (0x00000000u)
#define IO_HV_HIGH_SPEED_SERIAL_COMM_MMS                                           (0xAu)
#define IO_HV_HIGH_SPEED_SERIAL_COMM_VOL_MASK                               (0x00000003u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | hs_comm_parity_err_clean | Parity error clean from the       | 0x00000000  |
// |       |                          | inter-die interface.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | hs_comm_bit_stop_err_clea| Bit stop error clean from the     | 0x00000000  | 
// |       |                          | inter-die interface.              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: hs_comm_parity_err_clean
// Bit position, length and mask
#define BITP_IO_HV_HIGH_SPEED_SERIAL_COMM_HS_COMM_PARITY_ERR_CLEAN                   (0u)
#define BITL_IO_HV_HIGH_SPEED_SERIAL_COMM_HS_COMM_PARITY_ERR_CLEAN                   (1u)
#define BITM_IO_HV_HIGH_SPEED_SERIAL_COMM_HS_COMM_PARITY_ERR_CLEAN          (0x00000001u)

// Bit field: hs_comm_bit_stop_err_clean
// Bit position, length and mask
#define BITP_IO_HV_HIGH_SPEED_SERIAL_COMM_HS_COMM_BIT_STOP_ERR_CLEAN                 (1u)
#define BITL_IO_HV_HIGH_SPEED_SERIAL_COMM_HS_COMM_BIT_STOP_ERR_CLEAN                 (1u)
#define BITM_IO_HV_HIGH_SPEED_SERIAL_COMM_HS_COMM_BIT_STOP_ERR_CLEAN        (0x00000002u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : otp_config_0
// Register Addr        : 0xB20Fu
// Register NW Addr     : 0xB20Fu
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_OTP_CONFIG_0_ADDR                                                 (0xB20Fu)
#define IO_HV_OTP_CONFIG_0_NW_ADDR                                              (0xB20Fu)
#define IO_HV_OTP_CONFIG_0_RESET_VAL                                        (0x00000000u)
#define IO_HV_OTP_CONFIG_0_MMS                                                     (0xAu)
#define IO_HV_OTP_CONFIG_0_VOL_MASK                                         (0x0000000Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | otp_read_type_diff_red   | Read type of the OTP              | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: otp_read_type_diff_red
// Bit position, length and mask
#define BITP_IO_HV_OTP_CONFIG_0_OTP_READ_TYPE_DIFF_RED                               (0u)
#define BITL_IO_HV_OTP_CONFIG_0_OTP_READ_TYPE_DIFF_RED                               (1u)
#define BITM_IO_HV_OTP_CONFIG_0_OTP_READ_TYPE_DIFF_RED                      (0x00000001u)
// Enums
#define ENUM_IO_HV_OTP_CONFIG_0_OTP_READ_TYPE_DIFF_RED_ENUM000              (0x00000000u)
#define ENUM_IO_HV_OTP_CONFIG_0_OTP_READ_TYPE_DIFF_RED_ENUM001              (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : otp_config_1
// Register Addr        : 0xB210u
// Register NW Addr     : 0xB210u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define IO_HV_OTP_CONFIG_1_ADDR                                                 (0xB210u)
#define IO_HV_OTP_CONFIG_1_NW_ADDR                                              (0xB210u)
#define IO_HV_OTP_CONFIG_1_RESET_VAL                                        (0x00000000u)
#define IO_HV_OTP_CONFIG_1_MMS                                                     (0xAu)
#define IO_HV_OTP_CONFIG_1_VOL_MASK                                         (0x00000003u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | otp_access_mux_ctrl      | OTP access mux control            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: otp_access_mux_ctrl
// Bit position, length and mask
#define BITP_IO_HV_OTP_CONFIG_1_OTP_ACCESS_MUX_CTRL                                  (0u)
#define BITL_IO_HV_OTP_CONFIG_1_OTP_ACCESS_MUX_CTRL                                  (1u)
#define BITM_IO_HV_OTP_CONFIG_1_OTP_ACCESS_MUX_CTRL                         (0x00000001u)
// Enums
#define ENUM_IO_HV_OTP_CONFIG_1_OTP_ACCESS_MUX_CTRL_ENUM000                 (0x00000000u)
#define ENUM_IO_HV_OTP_CONFIG_1_OTP_ACCESS_MUX_CTRL_ENUM001                 (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : config_en_pads_config
// Register Addr        : 0xB21Cu
// Register NW Addr     : 0xB21Cu
// Register Reset Value : 0x000001A8u
// Register MMS         : 0xAu

#define IO_HV_CONFIG_EN_PADS_CONFIG_ADDR                                        (0xB21Cu)
#define IO_HV_CONFIG_EN_PADS_CONFIG_NW_ADDR                                     (0xB21Cu)
#define IO_HV_CONFIG_EN_PADS_CONFIG_RESET_VAL                               (0x000001A8u)
#define IO_HV_CONFIG_EN_PADS_CONFIG_MMS                                            (0xAu)
#define IO_HV_CONFIG_EN_PADS_CONFIG_VOL_MASK                                (0x00003FEEu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 1     | tp_ip_en_en              | Enable the input receiver on the  | 0x00000000  |
// |       |                          | EN pad                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | tp_out_enb_en            | Disable the output driver on the  | 0x00000000  |
// |       |                          | EN pad                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tp_ip_en_en
// Bit position, length and mask
#define BITP_IO_HV_CONFIG_EN_PADS_CONFIG_TP_IP_EN_EN                                 (1u)
#define BITL_IO_HV_CONFIG_EN_PADS_CONFIG_TP_IP_EN_EN                                 (1u)
#define BITM_IO_HV_CONFIG_EN_PADS_CONFIG_TP_IP_EN_EN                        (0x00000002u)
// Enums
#define ENUM_IO_HV_CONFIG_EN_PADS_CONFIG_TP_IP_EN_EN_ENUM0DISABLED          (0x00000000u)
#define ENUM_IO_HV_CONFIG_EN_PADS_CONFIG_TP_IP_EN_EN_ENUM1ENABLED           (0x00000001u)

// Bit field: tp_out_enb_en
// Bit position, length and mask
#define BITP_IO_HV_CONFIG_EN_PADS_CONFIG_TP_OUT_ENB_EN                               (2u)
#define BITL_IO_HV_CONFIG_EN_PADS_CONFIG_TP_OUT_ENB_EN                               (1u)
#define BITM_IO_HV_CONFIG_EN_PADS_CONFIG_TP_OUT_ENB_EN                      (0x00000004u)
// Enums
#define ENUM_IO_HV_CONFIG_EN_PADS_CONFIG_TP_OUT_ENB_EN_ENUM0ENABLED         (0x00000000u)
#define ENUM_IO_HV_CONFIG_EN_PADS_CONFIG_TP_OUT_ENB_EN_ENUM1DISABLED        (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillIO_HVRegsData();
#endif


#endif /* ADI_IO_HV_REG_H */

/** @} */

/*
 * EOF: www.analog.com
 */


