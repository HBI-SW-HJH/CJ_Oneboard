/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_otp_reg.h
 * @brief        : This is an auto generated header file with register
                   descriptions for OTP
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_OTP_REG_H
#define ADI_OTP_REG_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define OTP_NUM_REGS    (8u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganOTPRegAddr[OTP_NUM_REGS];
extern uint32_t ganOTPNwRegAddr[OTP_NUM_REGS];
extern uint32_t ganOTPRegMMS[OTP_NUM_REGS];
extern uint32_t ganOTPRegResetVal[OTP_NUM_REGS];
extern const char *gasOTPRegName[OTP_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : OMC
// Register Addr        : 0xB000u
// Register NW Addr     : 0xB000u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define OTP_OMC_ADDR                                                            (0xB000u)
#define OTP_OMC_NW_ADDR                                                         (0xB000u)
#define OTP_OMC_RESET_VAL                                                   (0x00000000u)
#define OTP_OMC_MMS                                                                (0xAu)
#define OTP_OMC_VOL_MASK                                                    (0x0000003Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | prog_mode_reg            | Places the OTP Controller in      | 0x00000000  |
// |       |                          | program mode; must be set before  |             |
// |       |                          | attempting a program operation.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | wr_pending               | Must be asserted to enable the    | 0x00000000  |
// |       |                          | controller to begin a program     |             |
// |       |                          | operation. Prior to asserting     |             |
// |       |                          | this bit, all intended address    |             |
// |       |                          | and data information must be      |             |
// |       |                          | set. *See section on memory       |             |
// |       |                          | programming for a full            |             |
// |       |                          | explanation.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | auto_incr_en_reg         | Allows the automatic increment    | 0x00000000  |
// |       |                          | of address values during program  |             |
// |       |                          | or read operations; value of      |             |
// |       |                          | increment is given by register    |             |
// |       |                          | OTP_OMC[5:4]                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | verify_and_soak_reg      | If asserted, the programming      | 0x00000000  |
// |       |                          | algorithm will skip the initial   |             |
// |       |                          | program phase and skip directly   |             |
// |       |                          | to the verify and soak portion:   |             |
// |       |                          | This is done by comparing the     |             |
// |       |                          | data in register OTP_WDL to what  |             |
// |       |                          | is currently in the memory at     |             |
// |       |                          | the address indicated by          |             |
// |       |                          | OTP_WAH/OTP_WAL.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 05-04 | ai_addr_offset_reg       | Valid only if OTP_OMC[2] = 1.     | 0x00000000  |
// |       |                          | Variable offset control for the   |             |
// |       |                          | automatic increment of the        |             |
// |       |                          | address after a program or read   |             |
// |       |                          | operation.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: prog_mode_reg
// Bit position, length and mask
#define BITP_OTP_OMC_PROG_MODE_REG                                                   (0u)
#define BITL_OTP_OMC_PROG_MODE_REG                                                   (1u)
#define BITM_OTP_OMC_PROG_MODE_REG                                          (0x00000001u)
// Enums
#define ENUM_OTP_OMC_PROG_MODE_REG_DISABLE                                  (0x00000000u)
#define ENUM_OTP_OMC_PROG_MODE_REG_ENABLE                                   (0x00000001u)

// Bit field: wr_pending
// Bit position, length and mask
#define BITP_OTP_OMC_WR_PENDING                                                      (1u)
#define BITL_OTP_OMC_WR_PENDING                                                      (1u)
#define BITM_OTP_OMC_WR_PENDING                                             (0x00000002u)
// Enums
#define ENUM_OTP_OMC_WR_PENDING_DISABLE                                     (0x00000000u)
#define ENUM_OTP_OMC_WR_PENDING_ENABLE                                      (0x00000001u)

// Bit field: auto_incr_en_reg
// Bit position, length and mask
#define BITP_OTP_OMC_AUTO_INCR_EN_REG                                                (2u)
#define BITL_OTP_OMC_AUTO_INCR_EN_REG                                                (1u)
#define BITM_OTP_OMC_AUTO_INCR_EN_REG                                       (0x00000004u)
// Enums
#define ENUM_OTP_OMC_AUTO_INCR_EN_REG_DISABLE                               (0x00000000u)
#define ENUM_OTP_OMC_AUTO_INCR_EN_REG_ENABLE                                (0x00000001u)

// Bit field: verify_and_soak_reg
// Bit position, length and mask
#define BITP_OTP_OMC_VERIFY_AND_SOAK_REG                                             (3u)
#define BITL_OTP_OMC_VERIFY_AND_SOAK_REG                                             (1u)
#define BITM_OTP_OMC_VERIFY_AND_SOAK_REG                                    (0x00000008u)
// Enums
#define ENUM_OTP_OMC_VERIFY_AND_SOAK_REG_DISABLE                            (0x00000000u)
#define ENUM_OTP_OMC_VERIFY_AND_SOAK_REG_ENABLE                             (0x00000001u)

// Bit field: ai_addr_offset_reg
// Bit position, length and mask
#define BITP_OTP_OMC_AI_ADDR_OFFSET_REG                                              (4u)
#define BITL_OTP_OMC_AI_ADDR_OFFSET_REG                                              (2u)
#define BITM_OTP_OMC_AI_ADDR_OFFSET_REG                                     (0x00000030u)
// Enums
#define ENUM_OTP_OMC_AI_ADDR_OFFSET_REG_ENUM000                             (0x00000000u)
#define ENUM_OTP_OMC_AI_ADDR_OFFSET_REG_ENUM001                             (0x00000001u)
#define ENUM_OTP_OMC_AI_ADDR_OFFSET_REG_ENUM002                             (0x00000002u)
#define ENUM_OTP_OMC_AI_ADDR_OFFSET_REG_ENUM003                             (0x00000003u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : WAL
// Register Addr        : 0xB001u
// Register NW Addr     : 0xB001u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define OTP_WAL_ADDR                                                            (0xB001u)
#define OTP_WAL_NW_ADDR                                                         (0xB001u)
#define OTP_WAL_RESET_VAL                                                   (0x00000000u)
#define OTP_WAL_MMS                                                                (0xAu)
#define OTP_WAL_VOL_MASK                                                    (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | wr_addr_regl             | Low read/write address register   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: wr_addr_regl
// Bit position, length and mask
#define BITP_OTP_WAL_WR_ADDR_REGL                                                    (0u)
#define BITL_OTP_WAL_WR_ADDR_REGL                                                    (8u)
#define BITM_OTP_WAL_WR_ADDR_REGL                                           (0x000000FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : WAH
// Register Addr        : 0xB002u
// Register NW Addr     : 0xB002u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define OTP_WAH_ADDR                                                            (0xB002u)
#define OTP_WAH_NW_ADDR                                                         (0xB002u)
#define OTP_WAH_RESET_VAL                                                   (0x00000000u)
#define OTP_WAH_MMS                                                                (0xAu)
#define OTP_WAH_VOL_MASK                                                    (0x00000007u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | wr_addr_regh             | High read/write address register  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: wr_addr_regh
// Bit position, length and mask
#define BITP_OTP_WAH_WR_ADDR_REGH                                                    (0u)
#define BITL_OTP_WAH_WR_ADDR_REGH                                                    (3u)
#define BITM_OTP_WAH_WR_ADDR_REGH                                           (0x00000007u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : WDL
// Register Addr        : 0xB003u
// Register NW Addr     : 0xB003u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define OTP_WDL_ADDR                                                            (0xB003u)
#define OTP_WDL_NW_ADDR                                                         (0xB003u)
#define OTP_WDL_RESET_VAL                                                   (0x00000000u)
#define OTP_WDL_MMS                                                                (0xAu)
#define OTP_WDL_VOL_MASK                                                    (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | wr_data_regl             | Contains the data to be           | 0x00000000  |
// |       |                          | programmed for the LSBs           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: wr_data_regl
// Bit position, length and mask
#define BITP_OTP_WDL_WR_DATA_REGL                                                    (0u)
#define BITL_OTP_WDL_WR_DATA_REGL                                                    (8u)
#define BITM_OTP_WDL_WR_DATA_REGL                                           (0x000000FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : WDH
// Register Addr        : 0xB004u
// Register NW Addr     : 0xB004u
// Register Reset Value : 0x00000000u
// Register MMS         : 0xAu

#define OTP_WDH_ADDR                                                            (0xB004u)
#define OTP_WDH_NW_ADDR                                                         (0xB004u)
#define OTP_WDH_RESET_VAL                                                   (0x00000000u)
#define OTP_WDH_MMS                                                                (0xAu)
#define OTP_WDH_VOL_MASK                                                    (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | wr_data_regh             | Contains the data to be           | 0x00000000  |
// |       |                          | programmed for the MSBs           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: wr_data_regh
// Bit position, length and mask
#define BITP_OTP_WDH_WR_DATA_REGH                                                    (0u)
#define BITL_OTP_WDH_WR_DATA_REGH                                                    (8u)
#define BITM_OTP_WDH_WR_DATA_REGH                                           (0x000000FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : WRC
// Register Addr        : 0xB005u
// Register NW Addr     : 0xB005u
// Register Reset Value : 0x00000040u
// Register MMS         : 0xAu

#define OTP_WRC_ADDR                                                            (0xB005u)
#define OTP_WRC_NW_ADDR                                                         (0xB005u)
#define OTP_WRC_RESET_VAL                                                   (0x00000040u)
#define OTP_WRC_MMS                                                                (0xAu)
#define OTP_WRC_VOL_MASK                                                    (0x000000C1u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 7     | vpp_en_reg               | Enables the IPS program power     | 0x00000000  |
// |       |                          | supply charge pump to program     |             |
// |       |                          | bytes into the OTP memory. This   |             |
// |       |                          | bit should be 0 when using        |             |
// |       |                          | external VPP.                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: vpp_en_reg
// Bit position, length and mask
#define BITP_OTP_WRC_VPP_EN_REG                                                      (7u)
#define BITL_OTP_WRC_VPP_EN_REG                                                      (1u)
#define BITM_OTP_WRC_VPP_EN_REG                                             (0x00000080u)
// Enums
#define ENUM_OTP_WRC_VPP_EN_REG_DISABLE                                     (0x00000000u)
#define ENUM_OTP_WRC_VPP_EN_REG_ENABLE                                      (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CMD
// Register Addr        : 0xB006u
// Register NW Addr     : 0xB006u
// Register Reset Value : 0x00000060u
// Register MMS         : 0xAu

#define OTP_CMD_ADDR                                                            (0xB006u)
#define OTP_CMD_NW_ADDR                                                         (0xB006u)
#define OTP_CMD_RESET_VAL                                                   (0x00000060u)
#define OTP_CMD_MMS                                                                (0xAu)
#define OTP_CMD_VOL_MASK                                                    (0x000000EFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | cmd_reg                  | OTPCMD[7] Must be asserted.       | 0x00000000  |
// |       |                          | Details below.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 06-05 | cmd_access_mode_reg      | Enables the varying access modes  | 0x00000003  |
// |       |                          | of the OTP operation.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | cmd_en_reg               | If set, enables operation of      | 0x00000000  |
// |       |                          | controller through register       |             |
// |       |                          | OTP_CMD[3:0] rather than          |             |
// |       |                          | OTP_TEC[7:0].                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cmd_reg
// Bit position, length and mask
#define BITP_OTP_CMD_CMD_REG                                                         (0u)
#define BITL_OTP_CMD_CMD_REG                                                         (4u)
#define BITM_OTP_CMD_CMD_REG                                                (0x0000000Fu)
// Enums
#define ENUM_OTP_CMD_CMD_REG_ENUM001                                        (0x00000001u)
#define ENUM_OTP_CMD_CMD_REG_ENUM002                                        (0x00000002u)

// Bit field: cmd_access_mode_reg
// Bit position, length and mask
#define BITP_OTP_CMD_CMD_ACCESS_MODE_REG                                             (5u)
#define BITL_OTP_CMD_CMD_ACCESS_MODE_REG                                             (2u)
#define BITM_OTP_CMD_CMD_ACCESS_MODE_REG                                    (0x00000060u)
// Enums
#define ENUM_OTP_CMD_CMD_ACCESS_MODE_REG_ENUM000                            (0x00000000u)
#define ENUM_OTP_CMD_CMD_ACCESS_MODE_REG_ENUM001                            (0x00000001u)
#define ENUM_OTP_CMD_CMD_ACCESS_MODE_REG_ENUM002                            (0x00000002u)
#define ENUM_OTP_CMD_CMD_ACCESS_MODE_REG_ENUM003                            (0x00000003u)

// Bit field: cmd_en_reg
// Bit position, length and mask
#define BITP_OTP_CMD_CMD_EN_REG                                                      (7u)
#define BITL_OTP_CMD_CMD_EN_REG                                                      (1u)
#define BITM_OTP_CMD_CMD_EN_REG                                             (0x00000080u)
// Enums
#define ENUM_OTP_CMD_CMD_EN_REG_DISABLE                                     (0x00000000u)
#define ENUM_OTP_CMD_CMD_EN_REG_ENABLE                                      (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PST
// Register Addr        : 0xB00Fu
// Register NW Addr     : 0xB00Fu
// Register Reset Value : 0x00000004u
// Register MMS         : 0xAu

#define OTP_PST_ADDR                                                            (0xB00Fu)
#define OTP_PST_NW_ADDR                                                         (0xB00Fu)
#define OTP_PST_RESET_VAL                                                   (0x00000004u)
#define OTP_PST_MMS                                                                (0xAu)
#define OTP_PST_VOL_MASK                                                    (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | prog_over                | When asserted, indicates that     | 0x00000000  |
// |       |                          | the program operation on the OTP  |             |
// |       |                          | memory has been completed;        |             |
// |       |                          | applicable when OTP_OMC[1] = 1.   |             |
// |       |                          | *This bit will be automatically   |             |
// |       |                          | cleared when a new program is     |             |
// |       |                          | initiated via OTP_OMC[1].         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | fatal_error              | When asserted, indicates that     | 0x00000000  |
// |       |                          | the maximum number of re-program  |             |
// |       |                          | attempts have failed; applicable  |             |
// |       |                          | when OTP_OMC[1] = 1. *This bit    |             |
// |       |                          | will be automatically cleared     |             |
// |       |                          | when a new program is initiated   |             |
// |       |                          | via OTP_OMC[1].                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | cmd_done                 | When asserted, indicates that     | 0x00000001  |
// |       |                          | the current command to the OTP    |             |
// |       |                          | memory has finished and that it   |             |
// |       |                          | is available to receive new       |             |
// |       |                          | instructions.                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: prog_over
// Bit position, length and mask
#define BITP_OTP_PST_PROG_OVER                                                       (0u)
#define BITL_OTP_PST_PROG_OVER                                                       (1u)
#define BITM_OTP_PST_PROG_OVER                                              (0x00000001u)
// Enums
#define ENUM_OTP_PST_PROG_OVER_NOT_TRIGGERED                                (0x00000000u)
#define ENUM_OTP_PST_PROG_OVER_TRIGGERED                                    (0x00000001u)

// Bit field: fatal_error
// Bit position, length and mask
#define BITP_OTP_PST_FATAL_ERROR                                                     (1u)
#define BITL_OTP_PST_FATAL_ERROR                                                     (1u)
#define BITM_OTP_PST_FATAL_ERROR                                            (0x00000002u)
// Enums
#define ENUM_OTP_PST_FATAL_ERROR_NOT_TRIGGERED                              (0x00000000u)
#define ENUM_OTP_PST_FATAL_ERROR_TRIGGERED                                  (0x00000001u)

// Bit field: cmd_done
// Bit position, length and mask
#define BITP_OTP_PST_CMD_DONE                                                        (2u)
#define BITL_OTP_PST_CMD_DONE                                                        (1u)
#define BITM_OTP_PST_CMD_DONE                                               (0x00000004u)
// Enums
#define ENUM_OTP_PST_CMD_DONE_ENUM000                                       (0x00000000u)
#define ENUM_OTP_PST_CMD_DONE_ENUM001                                       (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillOTPRegsData();
#endif


#endif /* ADI_OTP_REG_H */

/** @} */

/*
 * EOF: www.analog.com
 */


