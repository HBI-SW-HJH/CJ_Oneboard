/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_phy_oa_reg.h
 * @brief        : This is an auto generated header file with register
                   descriptions for PHY
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup macphy
 *  @{
 */

#ifndef ADI_PHY_OA_REG_H
#define ADI_PHY_OA_REG_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define PHY_OA_NUM_REGS    (41u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganPHY_OARegAddr[PHY_OA_NUM_REGS];
extern uint32_t ganPHY_OANwRegAddr[PHY_OA_NUM_REGS];
extern uint32_t ganPHY_OARegMMS[PHY_OA_NUM_REGS];
extern uint32_t ganPHY_OARegResetVal[PHY_OA_NUM_REGS];
extern const char *gasPHY_OARegName[PHY_OA_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : tweaks_1
// Register Addr        : 0x0000u
// Register NW Addr     : 0xE000u
// Register Reset Value : 0x000000C0u
// Register MMS         : 4u

#define PHY_OA_TWEAKS_1_ADDR                                                    (0x0000u)
#define PHY_OA_TWEAKS_1_NW_ADDR                                                 (0xE000u)
#define PHY_OA_TWEAKS_1_RESET_VAL                                           (0x000000C0u)
#define PHY_OA_TWEAKS_1_MMS                                                          (4u)
#define PHY_OA_TWEAKS_1_VOL_MASK                                            (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 10    | no_col_count             | Disable collision detection       | 0x00000000  |
// |       |                          | counting in PLCA mode             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | txc_auto_reset_cmd_on_stu| Automatically send reset          | 0x00000000  | 
// |       |                          | commands to the 3pin interface    |             |
// |       |                          | on the other die, if the RX/ED    |             |
// |       |                          | pins are detected static (with    |             |
// |       |                          | ED=1) for 1us.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: no_col_count
// Bit position, length and mask
#define BITP_PHY_OA_TWEAKS_1_NO_COL_COUNT                                           (10u)
#define BITL_PHY_OA_TWEAKS_1_NO_COL_COUNT                                            (1u)
#define BITM_PHY_OA_TWEAKS_1_NO_COL_COUNT                                   (0x00000400u)
// Enums
#define ENUM_PHY_OA_TWEAKS_1_NO_COL_COUNT_ENUM000                           (0x00000000u)
#define ENUM_PHY_OA_TWEAKS_1_NO_COL_COUNT_ENUM001                           (0x00000001u)

// Bit field: txc_auto_reset_cmd_on_stuck_rx_ed_dis
// Bit position, length and mask
#define BITP_PHY_OA_TWEAKS_1_TXC_AUTO_RESET_CMD_ON_STUCK_RX_ED_DIS                  (12u)
#define BITL_PHY_OA_TWEAKS_1_TXC_AUTO_RESET_CMD_ON_STUCK_RX_ED_DIS                   (1u)
#define BITM_PHY_OA_TWEAKS_1_TXC_AUTO_RESET_CMD_ON_STUCK_RX_ED_DIS          (0x00001000u)
// Enums
#define ENUM_PHY_OA_TWEAKS_1_TXC_AUTO_RESET_CMD_ON_STUCK_RX_ED_DIS_ENUM000  (0x00000000u)
#define ENUM_PHY_OA_TWEAKS_1_TXC_AUTO_RESET_CMD_ON_STUCK_RX_ED_DIS_ENUM001  (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : manual_ctrl_reg_1
// Register Addr        : 0x0003u
// Register NW Addr     : 0xE003u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_MANUAL_CTRL_REG_1_ADDR                                           (0x0003u)
#define PHY_OA_MANUAL_CTRL_REG_1_NW_ADDR                                        (0xE003u)
#define PHY_OA_MANUAL_CTRL_REG_1_RESET_VAL                                  (0x00000000u)
#define PHY_OA_MANUAL_CTRL_REG_1_MMS                                                 (4u)
#define PHY_OA_MANUAL_CTRL_REG_1_VOL_MASK                                   (0x00000600u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 9     | pd_dll                   | DLL power down                    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | pd_xtal_buffer           | XTAL buffer power down            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pd_dll
// Bit position, length and mask
#define BITP_PHY_OA_MANUAL_CTRL_REG_1_PD_DLL                                         (9u)
#define BITL_PHY_OA_MANUAL_CTRL_REG_1_PD_DLL                                         (1u)
#define BITM_PHY_OA_MANUAL_CTRL_REG_1_PD_DLL                                (0x00000200u)
// Enums
#define ENUM_PHY_OA_MANUAL_CTRL_REG_1_PD_DLL_ENUM000                        (0x00000000u)
#define ENUM_PHY_OA_MANUAL_CTRL_REG_1_PD_DLL_ENUM001                        (0x00000001u)

// Bit field: pd_xtal_buffer
// Bit position, length and mask
#define BITP_PHY_OA_MANUAL_CTRL_REG_1_PD_XTAL_BUFFER                                (10u)
#define BITL_PHY_OA_MANUAL_CTRL_REG_1_PD_XTAL_BUFFER                                 (1u)
#define BITM_PHY_OA_MANUAL_CTRL_REG_1_PD_XTAL_BUFFER                        (0x00000400u)
// Enums
#define ENUM_PHY_OA_MANUAL_CTRL_REG_1_PD_XTAL_BUFFER_ENUM000                (0x00000000u)
#define ENUM_PHY_OA_MANUAL_CTRL_REG_1_PD_XTAL_BUFFER_ENUM001                (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : plca_ext
// Register Addr        : 0x000Fu
// Register NW Addr     : 0xE00Fu
// Register Reset Value : 0x00000800u
// Register MMS         : 4u

#define PHY_OA_PLCA_EXT_ADDR                                                    (0x000Fu)
#define PHY_OA_PLCA_EXT_NW_ADDR                                                 (0xE00Fu)
#define PHY_OA_PLCA_EXT_RESET_VAL                                           (0x00000800u)
#define PHY_OA_PLCA_EXT_MMS                                                          (4u)
#define PHY_OA_PLCA_EXT_VOL_MASK                                            (0x00001803u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | plcaLeader               | When PLCALEADEREN is asserted,    | 0x00000000  |
// |       |                          | this bit elects the node as PLCA  |             |
// |       |                          | leader despite the value of its   |             |
// |       |                          | assigned node ID                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | plcaLeaderEn             | When asserted high (1), this bit  | 0x00000000  |
// |       |                          | allows electing a node with ID    |             |
// |       |                          | other than 0 as the PLCA leader.  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | plcaPrecEn               | This setting enables (1) or       | 0x00000000  |
// |       |                          | disables (0) the PLCA precedence  |             |
// |       |                          | mode feature.                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaLeader
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_EXT_PLCALEADER                                              (0u)
#define BITL_PHY_OA_PLCA_EXT_PLCALEADER                                              (1u)
#define BITM_PHY_OA_PLCA_EXT_PLCALEADER                                     (0x00000001u)
// Enums
#define ENUM_PHY_OA_PLCA_EXT_PLCALEADER_NO_PLCA_LEADER                      (0x00000000u)
#define ENUM_PHY_OA_PLCA_EXT_PLCALEADER_PLCA_LEADER                         (0x00000001u)

// Bit field: plcaLeaderEn
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_EXT_PLCALEADEREN                                            (1u)
#define BITL_PHY_OA_PLCA_EXT_PLCALEADEREN                                            (1u)
#define BITM_PHY_OA_PLCA_EXT_PLCALEADEREN                                   (0x00000002u)
// Enums
#define ENUM_PHY_OA_PLCA_EXT_PLCALEADEREN_DISABLE                           (0x00000000u)
#define ENUM_PHY_OA_PLCA_EXT_PLCALEADEREN_ENABLE                            (0x00000001u)

// Bit field: plcaPrecEn
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_EXT_PLCAPRECEN                                             (12u)
#define BITL_PHY_OA_PLCA_EXT_PLCAPRECEN                                              (1u)
#define BITM_PHY_OA_PLCA_EXT_PLCAPRECEN                                     (0x00001000u)
// Enums
#define ENUM_PHY_OA_PLCA_EXT_PLCAPRECEN_DISABLE                             (0x00000000u)
#define ENUM_PHY_OA_PLCA_EXT_PLCAPRECEN_ENABLE                              (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_CTRL2
// Register Addr        : 0x0018u
// Register NW Addr     : 0xE018u
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 4u

#define PHY_OA_PLCA_CTRL2_ADDR                                                  (0x0018u)
#define PHY_OA_PLCA_CTRL2_NW_ADDR                                               (0xE018u)
#define PHY_OA_PLCA_CTRL2_RESET_VAL                                         (0x0000FFFFu)
#define PHY_OA_PLCA_CTRL2_MMS                                                        (4u)
#define PHY_OA_PLCA_CTRL2_VOL_MASK                                          (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaLocalID1             | Multiple PLCA IDs - ID1           | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | plcaLocalID2             | Multiple PLCA IDs - ID2           | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaLocalID1
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL2_PLCALOCALID1                                          (0u)
#define BITL_PHY_OA_PLCA_CTRL2_PLCALOCALID1                                          (8u)
#define BITM_PHY_OA_PLCA_CTRL2_PLCALOCALID1                                 (0x000000FFu)

// Bit field: plcaLocalID2
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL2_PLCALOCALID2                                          (8u)
#define BITL_PHY_OA_PLCA_CTRL2_PLCALOCALID2                                          (8u)
#define BITM_PHY_OA_PLCA_CTRL2_PLCALOCALID2                                 (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_CTRL3
// Register Addr        : 0x0019u
// Register NW Addr     : 0xE019u
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 4u

#define PHY_OA_PLCA_CTRL3_ADDR                                                  (0x0019u)
#define PHY_OA_PLCA_CTRL3_NW_ADDR                                               (0xE019u)
#define PHY_OA_PLCA_CTRL3_RESET_VAL                                         (0x0000FFFFu)
#define PHY_OA_PLCA_CTRL3_MMS                                                        (4u)
#define PHY_OA_PLCA_CTRL3_VOL_MASK                                          (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaLocalID3             | Multiple PLCA IDs - ID3           | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | plcaLocalID4             | Multiple PLCA IDs - ID4           | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaLocalID3
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL3_PLCALOCALID3                                          (0u)
#define BITL_PHY_OA_PLCA_CTRL3_PLCALOCALID3                                          (8u)
#define BITM_PHY_OA_PLCA_CTRL3_PLCALOCALID3                                 (0x000000FFu)

// Bit field: plcaLocalID4
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL3_PLCALOCALID4                                          (8u)
#define BITL_PHY_OA_PLCA_CTRL3_PLCALOCALID4                                          (8u)
#define BITM_PHY_OA_PLCA_CTRL3_PLCALOCALID4                                 (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_CTRL4
// Register Addr        : 0x001Au
// Register NW Addr     : 0xE01Au
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 4u

#define PHY_OA_PLCA_CTRL4_ADDR                                                  (0x001Au)
#define PHY_OA_PLCA_CTRL4_NW_ADDR                                               (0xE01Au)
#define PHY_OA_PLCA_CTRL4_RESET_VAL                                         (0x0000FFFFu)
#define PHY_OA_PLCA_CTRL4_MMS                                                        (4u)
#define PHY_OA_PLCA_CTRL4_VOL_MASK                                          (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaLocalID5             | Multiple PLCA IDs - ID5           | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | plcaLocalID6             | Multiple PLCA IDs - ID6           | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaLocalID5
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL4_PLCALOCALID5                                          (0u)
#define BITL_PHY_OA_PLCA_CTRL4_PLCALOCALID5                                          (8u)
#define BITM_PHY_OA_PLCA_CTRL4_PLCALOCALID5                                 (0x000000FFu)

// Bit field: plcaLocalID6
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL4_PLCALOCALID6                                          (8u)
#define BITL_PHY_OA_PLCA_CTRL4_PLCALOCALID6                                          (8u)
#define BITM_PHY_OA_PLCA_CTRL4_PLCALOCALID6                                 (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_CTRL5
// Register Addr        : 0x001Bu
// Register NW Addr     : 0xE01Bu
// Register Reset Value : 0x000001FFu
// Register MMS         : 4u

#define PHY_OA_PLCA_CTRL5_ADDR                                                  (0x001Bu)
#define PHY_OA_PLCA_CTRL5_NW_ADDR                                               (0xE01Bu)
#define PHY_OA_PLCA_CTRL5_RESET_VAL                                         (0x000001FFu)
#define PHY_OA_PLCA_CTRL5_MMS                                                        (4u)
#define PHY_OA_PLCA_CTRL5_VOL_MASK                                          (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaLocalID7             | Multiple PLCA IDs - ID7           | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | plcaLocalID_en           | Bit 0 of this field enable        | 0x00000001  |
// |       |                          | PLCALOCALID, bit 1 enables        |             |
// |       |                          | PLCALOCALID1, bit 2 enable        |             |
// |       |                          | PLCALOCALID2 and so on etc.       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaLocalID7
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL5_PLCALOCALID7                                          (0u)
#define BITL_PHY_OA_PLCA_CTRL5_PLCALOCALID7                                          (8u)
#define BITM_PHY_OA_PLCA_CTRL5_PLCALOCALID7                                 (0x000000FFu)

// Bit field: plcaLocalID_en
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN                                        (8u)
#define BITL_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN                                        (8u)
#define BITM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN                               (0x0000FF00u)
// Enums
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_0_EN              (0x00000008u)
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_1_EN              (0x00000009u)
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_2_EN              (0x0000000Au)
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_3_EN              (0x0000000Bu)
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_4_EN              (0x0000000Cu)
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_5_EN              (0x0000000Du)
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_6_EN              (0x0000000Eu)
#define ENUM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN_PLCALOCALID_7_EN              (0x0000000Fu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_LUT_REG0
// Register Addr        : 0x001Cu
// Register NW Addr     : 0xE01Cu
// Register Reset Value : 0x00003210u
// Register MMS         : 4u

#define PHY_OA_PLCA_LUT_REG0_ADDR                                               (0x001Cu)
#define PHY_OA_PLCA_LUT_REG0_NW_ADDR                                            (0xE01Cu)
#define PHY_OA_PLCA_LUT_REG0_RESET_VAL                                      (0x00003210u)
#define PHY_OA_PLCA_LUT_REG0_MMS                                                     (4u)
#define PHY_OA_PLCA_LUT_REG0_VOL_MASK                                       (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | plca_lut_code0           | Value of the plcaLocalID0[3:0]    | 0x00000000  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 0                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | plca_lut_code1           | Value of the plcaLocalID0[3:0]    | 0x00000001  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 1                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | plca_lut_code2           | Value of the plcaLocalID0[3:0]    | 0x00000002  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 2                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | plca_lut_code3           | Value of the plcaLocalID0[3:0]    | 0x00000003  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 3                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plca_lut_code0
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE0                                     (0u)
#define BITL_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE0                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE0                            (0x0000000Fu)

// Bit field: plca_lut_code1
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE1                                     (4u)
#define BITL_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE1                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE1                            (0x000000F0u)

// Bit field: plca_lut_code2
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE2                                     (8u)
#define BITL_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE2                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE2                            (0x00000F00u)

// Bit field: plca_lut_code3
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE3                                    (12u)
#define BITL_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE3                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG0_PLCA_LUT_CODE3                            (0x0000F000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_LUT_REG1
// Register Addr        : 0x001Du
// Register NW Addr     : 0xE01Du
// Register Reset Value : 0x00007654u
// Register MMS         : 4u

#define PHY_OA_PLCA_LUT_REG1_ADDR                                               (0x001Du)
#define PHY_OA_PLCA_LUT_REG1_NW_ADDR                                            (0xE01Du)
#define PHY_OA_PLCA_LUT_REG1_RESET_VAL                                      (0x00007654u)
#define PHY_OA_PLCA_LUT_REG1_MMS                                                     (4u)
#define PHY_OA_PLCA_LUT_REG1_VOL_MASK                                       (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | plca_lut_code4           | Value of the plcaLocalID0[3:0]    | 0x00000004  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 4                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | plca_lut_code5           | Value of the plcaLocalID0[3:0]    | 0x00000005  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 5                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | plca_lut_code6           | Value of the plcaLocalID0[3:0]    | 0x00000006  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 6                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | plca_lut_code7           | Value of the plcaLocalID0[3:0]    | 0x00000007  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 7                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plca_lut_code4
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE4                                     (0u)
#define BITL_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE4                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE4                            (0x0000000Fu)

// Bit field: plca_lut_code5
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE5                                     (4u)
#define BITL_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE5                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE5                            (0x000000F0u)

// Bit field: plca_lut_code6
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE6                                     (8u)
#define BITL_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE6                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE6                            (0x00000F00u)

// Bit field: plca_lut_code7
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE7                                    (12u)
#define BITL_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE7                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG1_PLCA_LUT_CODE7                            (0x0000F000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_LUT_REG2
// Register Addr        : 0x001Eu
// Register NW Addr     : 0xE01Eu
// Register Reset Value : 0x0000BA98u
// Register MMS         : 4u

#define PHY_OA_PLCA_LUT_REG2_ADDR                                               (0x001Eu)
#define PHY_OA_PLCA_LUT_REG2_NW_ADDR                                            (0xE01Eu)
#define PHY_OA_PLCA_LUT_REG2_RESET_VAL                                      (0x0000BA98u)
#define PHY_OA_PLCA_LUT_REG2_MMS                                                     (4u)
#define PHY_OA_PLCA_LUT_REG2_VOL_MASK                                       (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | plca_lut_code8           | Value of the plcaLocalID0[3:0]    | 0x00000008  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 8                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | plca_lut_code9           | Value of the plcaLocalID0[3:0]    | 0x00000009  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 9                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | plca_lut_code10          | Value of the plcaLocalID0[3:0]    | 0x0000000a  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 10                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | plca_lut_code11          | Value of the plcaLocalID0[3:0]    | 0x0000000b  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 11                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plca_lut_code8
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE8                                     (0u)
#define BITL_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE8                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE8                            (0x0000000Fu)

// Bit field: plca_lut_code9
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE9                                     (4u)
#define BITL_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE9                                     (4u)
#define BITM_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE9                            (0x000000F0u)

// Bit field: plca_lut_code10
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE10                                    (8u)
#define BITL_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE10                                    (4u)
#define BITM_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE10                           (0x00000F00u)

// Bit field: plca_lut_code11
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE11                                   (12u)
#define BITL_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE11                                    (4u)
#define BITM_PHY_OA_PLCA_LUT_REG2_PLCA_LUT_CODE11                           (0x0000F000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_LUT_REG3
// Register Addr        : 0x001Fu
// Register NW Addr     : 0xE01Fu
// Register Reset Value : 0x0000FEDCu
// Register MMS         : 4u

#define PHY_OA_PLCA_LUT_REG3_ADDR                                               (0x001Fu)
#define PHY_OA_PLCA_LUT_REG3_NW_ADDR                                            (0xE01Fu)
#define PHY_OA_PLCA_LUT_REG3_RESET_VAL                                      (0x0000FEDCu)
#define PHY_OA_PLCA_LUT_REG3_MMS                                                     (4u)
#define PHY_OA_PLCA_LUT_REG3_VOL_MASK                                       (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | plca_lut_code12          | Value of the plcaLocalID0[3:0]    | 0x0000000c  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 12                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | plca_lut_code13          | Value of the plcaLocalID0[3:0]    | 0x0000000d  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 13                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | plca_lut_code14          | Value of the plcaLocalID0[3:0]    | 0x0000000e  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 14                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | plca_lut_code15          | Value of the plcaLocalID0[3:0]    | 0x0000000f  |
// |       |                          | when the 4 SAIF device pins       |             |
// |       |                          | represent code 15                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plca_lut_code12
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE12                                    (0u)
#define BITL_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE12                                    (4u)
#define BITM_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE12                           (0x0000000Fu)

// Bit field: plca_lut_code13
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE13                                    (4u)
#define BITL_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE13                                    (4u)
#define BITM_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE13                           (0x000000F0u)

// Bit field: plca_lut_code14
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE14                                    (8u)
#define BITL_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE14                                    (4u)
#define BITM_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE14                           (0x00000F00u)

// Bit field: plca_lut_code15
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE15                                   (12u)
#define BITL_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE15                                    (4u)
#define BITM_PHY_OA_PLCA_LUT_REG3_PLCA_LUT_CODE15                           (0x0000F000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_LUT_CTRL
// Register Addr        : 0x0020u
// Register NW Addr     : 0xE020u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_PLCA_LUT_CTRL_ADDR                                               (0x0020u)
#define PHY_OA_PLCA_LUT_CTRL_NW_ADDR                                            (0xE020u)
#define PHY_OA_PLCA_LUT_CTRL_RESET_VAL                                      (0x00000000u)
#define PHY_OA_PLCA_LUT_CTRL_MMS                                                     (4u)
#define PHY_OA_PLCA_LUT_CTRL_VOL_MASK                                       (0x00000001u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | pin_to_plca_lut_en       | Enable the use of 4 SAIF device   | 0x00000000  |
// |       |                          | pins to set plcaLocalID[3:0]      |             |
// |       |                          | from programmable LUT             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pin_to_plca_lut_en
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_CTRL_PIN_TO_PLCA_LUT_EN                                 (0u)
#define BITL_PHY_OA_PLCA_LUT_CTRL_PIN_TO_PLCA_LUT_EN                                 (1u)
#define BITM_PHY_OA_PLCA_LUT_CTRL_PIN_TO_PLCA_LUT_EN                        (0x00000001u)
// Enums
#define ENUM_PHY_OA_PLCA_LUT_CTRL_PIN_TO_PLCA_LUT_EN_ENUM000                (0x00000000u)
#define ENUM_PHY_OA_PLCA_LUT_CTRL_PIN_TO_PLCA_LUT_EN_ENUM001                (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_LUT_DBG
// Register Addr        : 0x0021u
// Register NW Addr     : 0xE021u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_PLCA_LUT_DBG_ADDR                                                (0x0021u)
#define PHY_OA_PLCA_LUT_DBG_NW_ADDR                                             (0xE021u)
#define PHY_OA_PLCA_LUT_DBG_RESET_VAL                                       (0x00000000u)
#define PHY_OA_PLCA_LUT_DBG_MMS                                                      (4u)
#define PHY_OA_PLCA_LUT_DBG_VOL_MASK                                        (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | rb_effective_plca_id0    | Effective PLCA ID0 value,         | 0x00000000  |
// |       |                          | considering whether the PLCA LUT  |             |
// |       |                          | was enabled and which code was    |             |
// |       |                          | selected based on the interface   |             |
// |       |                          | pins. For debug purposes          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | rb_effective_plca_en     | Effective PLCA En value,          | 0x00000000  |
// |       |                          | considering the double-buffering  |             |
// |       |                          | while receiving. For debug        |             |
// |       |                          | purposes                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: rb_effective_plca_id0
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_DBG_RB_EFFECTIVE_PLCA_ID0                               (0u)
#define BITL_PHY_OA_PLCA_LUT_DBG_RB_EFFECTIVE_PLCA_ID0                               (8u)
#define BITM_PHY_OA_PLCA_LUT_DBG_RB_EFFECTIVE_PLCA_ID0                      (0x000000FFu)

// Bit field: rb_effective_plca_en
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_LUT_DBG_RB_EFFECTIVE_PLCA_EN                               (15u)
#define BITL_PHY_OA_PLCA_LUT_DBG_RB_EFFECTIVE_PLCA_EN                                (1u)
#define BITM_PHY_OA_PLCA_LUT_DBG_RB_EFFECTIVE_PLCA_EN                       (0x00008000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_vs_reg1
// Register Addr        : 0x0061u
// Register NW Addr     : 0xE061u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_VS_REG1_ADDR                                                  (0x0061u)
#define PHY_OA_TD_VS_REG1_NW_ADDR                                               (0xE061u)
#define PHY_OA_TD_VS_REG1_RESET_VAL                                         (0x00000000u)
#define PHY_OA_TD_VS_REG1_MMS                                                        (4u)
#define PHY_OA_TD_VS_REG1_VOL_MASK                                          (0x0000003Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | td_dm_to_dis             | Topology Discovery - Disable the  | 0x00000000  |
// |       |                          | TD_DM_TO hardware timer, and      |             |
// |       |                          | instead rely on SW control of a   |             |
// |       |                          | (longer) timer. In this case the  |             |
// |       |                          | timeout abort is executed by the  |             |
// |       |                          | SW bringing TX_EN low             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_dm_to_dis
// Bit position, length and mask
#define BITP_PHY_OA_TD_VS_REG1_TD_DM_TO_DIS                                          (0u)
#define BITL_PHY_OA_TD_VS_REG1_TD_DM_TO_DIS                                          (1u)
#define BITM_PHY_OA_TD_VS_REG1_TD_DM_TO_DIS                                 (0x00000001u)
// Enums
#define ENUM_PHY_OA_TD_VS_REG1_TD_DM_TO_DIS_ENUM000                         (0x00000000u)
#define ENUM_PHY_OA_TD_VS_REG1_TD_DM_TO_DIS_ENUM001                         (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_cfg0
// Register Addr        : 0x008Au
// Register NW Addr     : 0xE08Au
// Register Reset Value : 0x0000FF01u
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_CFG0_ADDR                                                 (0x008Au)
#define PHY_OA_DCQ_JM_CFG0_NW_ADDR                                              (0xE08Au)
#define PHY_OA_DCQ_JM_CFG0_RESET_VAL                                        (0x0000FF01u)
#define PHY_OA_DCQ_JM_CFG0_MMS                                                       (4u)
#define PHY_OA_DCQ_JM_CFG0_VOL_MASK                                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | cfg_jm_en                | Jitter monitor - Main enable      | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | cfg_jm_pause             | Jitter monitor - Pauses updates   | 0x00000000  |
// |       |                          | of readback data while active     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | cfg_jm_plca_id_sel       | Jitter monitor - Selected PLCA    | 0x000000ff  |
// |       |                          | Transmit Opportunity ID to        |             |
// |       |                          | monitor (use 0xFF to monitor all  |             |
// |       |                          | IDs)                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_jm_en
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN                                            (0u)
#define BITL_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN                                            (1u)
#define BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN                                   (0x00000001u)
// Enums
#define ENUM_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN_DISABLE                           (0x00000000u)
#define ENUM_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN_ENABLE                            (0x00000001u)

// Bit field: cfg_jm_pause
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE                                         (1u)
#define BITL_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE                                         (1u)
#define BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE                                (0x00000002u)
// Enums
#define ENUM_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE_DISABLE                        (0x00000000u)
#define ENUM_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE_ENABLE                         (0x00000001u)

// Bit field: cfg_jm_plca_id_sel
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_PLCA_ID_SEL                                   (8u)
#define BITL_PHY_OA_DCQ_JM_CFG0_CFG_JM_PLCA_ID_SEL                                   (8u)
#define BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_PLCA_ID_SEL                          (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_cfg1
// Register Addr        : 0x008Bu
// Register NW Addr     : 0xE08Bu
// Register Reset Value : 0x00000002u
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_CFG1_ADDR                                                 (0x008Bu)
#define PHY_OA_DCQ_JM_CFG1_NW_ADDR                                              (0xE08Bu)
#define PHY_OA_DCQ_JM_CFG1_RESET_VAL                                        (0x00000002u)
#define PHY_OA_DCQ_JM_CFG1_MMS                                                       (4u)
#define PHY_OA_DCQ_JM_CFG1_VOL_MASK                                         (0x00000003u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 01-00 | cfg_jm_settled_range_sel | Jitter monitor - Selects the      | 0x00000002  |
// |       |                          | number of packets that need to    |             |
// |       |                          | be received for the integrator    |             |
// |       |                          | to settle, includes value needed  |             |
// |       |                          | to reach maximum shift (16K       |             |
// |       |                          | frames).                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_jm_settled_range_sel
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG1_CFG_JM_SETTLED_RANGE_SEL                             (0u)
#define BITL_PHY_OA_DCQ_JM_CFG1_CFG_JM_SETTLED_RANGE_SEL                             (2u)
#define BITM_PHY_OA_DCQ_JM_CFG1_CFG_JM_SETTLED_RANGE_SEL                    (0x00000003u)
// Enums
#define ENUM_PHY_OA_DCQ_JM_CFG1_CFG_JM_SETTLED_RANGE_SEL_SETTLED_16K        (0x00000000u)
#define ENUM_PHY_OA_DCQ_JM_CFG1_CFG_JM_SETTLED_RANGE_SEL_SETTLED_60K        (0x00000001u)
#define ENUM_PHY_OA_DCQ_JM_CFG1_CFG_JM_SETTLED_RANGE_SEL_SETTLED_80K        (0x00000002u)
#define ENUM_PHY_OA_DCQ_JM_CFG1_CFG_JM_SETTLED_RANGE_SEL_SETTLED_100K       (0x00000003u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_cfg2
// Register Addr        : 0x008Cu
// Register NW Addr     : 0xE08Cu
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_CFG2_ADDR                                                 (0x008Cu)
#define PHY_OA_DCQ_JM_CFG2_NW_ADDR                                              (0xE08Cu)
#define PHY_OA_DCQ_JM_CFG2_RESET_VAL                                        (0x0000FFFFu)
#define PHY_OA_DCQ_JM_CFG2_MMS                                                       (4u)
#define PHY_OA_DCQ_JM_CFG2_VOL_MASK                                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | cfg_jm_th_low_mid_err    | Jitter monitor - Threshold for    | 0x0000ffff  |
// |       |                          | low-to-mid level on combined      |             |
// |       |                          | metric                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_jm_th_low_mid_err
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG2_CFG_JM_TH_LOW_MID_ERR                                (0u)
#define BITL_PHY_OA_DCQ_JM_CFG2_CFG_JM_TH_LOW_MID_ERR                               (16u)
#define BITM_PHY_OA_DCQ_JM_CFG2_CFG_JM_TH_LOW_MID_ERR                       (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_cfg3
// Register Addr        : 0x008Du
// Register NW Addr     : 0xE08Du
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_CFG3_ADDR                                                 (0x008Du)
#define PHY_OA_DCQ_JM_CFG3_NW_ADDR                                              (0xE08Du)
#define PHY_OA_DCQ_JM_CFG3_RESET_VAL                                        (0x0000FFFFu)
#define PHY_OA_DCQ_JM_CFG3_MMS                                                       (4u)
#define PHY_OA_DCQ_JM_CFG3_VOL_MASK                                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | cfg_jm_th_mid_high_err   | Jitter monitor - Threshold for    | 0x0000ffff  |
// |       |                          | mid-to-high level on combined     |             |
// |       |                          | metric                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_jm_th_mid_high_err
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG3_CFG_JM_TH_MID_HIGH_ERR                               (0u)
#define BITL_PHY_OA_DCQ_JM_CFG3_CFG_JM_TH_MID_HIGH_ERR                              (16u)
#define BITM_PHY_OA_DCQ_JM_CFG3_CFG_JM_TH_MID_HIGH_ERR                      (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_cfg4
// Register Addr        : 0x008Eu
// Register NW Addr     : 0xE08Eu
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_CFG4_ADDR                                                 (0x008Eu)
#define PHY_OA_DCQ_JM_CFG4_NW_ADDR                                              (0xE08Eu)
#define PHY_OA_DCQ_JM_CFG4_RESET_VAL                                        (0x00000000u)
#define PHY_OA_DCQ_JM_CFG4_MMS                                                       (4u)
#define PHY_OA_DCQ_JM_CFG4_VOL_MASK                                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | cfg_jm_th_high_mid_err   | Jitter monitor - Threshold for    | 0x00000000  |
// |       |                          | high-to-mid level on combined     |             |
// |       |                          | metric                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_jm_th_high_mid_err
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG4_CFG_JM_TH_HIGH_MID_ERR                               (0u)
#define BITL_PHY_OA_DCQ_JM_CFG4_CFG_JM_TH_HIGH_MID_ERR                              (16u)
#define BITM_PHY_OA_DCQ_JM_CFG4_CFG_JM_TH_HIGH_MID_ERR                      (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_cfg5
// Register Addr        : 0x008Fu
// Register NW Addr     : 0xE08Fu
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_CFG5_ADDR                                                 (0x008Fu)
#define PHY_OA_DCQ_JM_CFG5_NW_ADDR                                              (0xE08Fu)
#define PHY_OA_DCQ_JM_CFG5_RESET_VAL                                        (0x00000000u)
#define PHY_OA_DCQ_JM_CFG5_MMS                                                       (4u)
#define PHY_OA_DCQ_JM_CFG5_VOL_MASK                                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | cfg_jm_th_mid_low_err    | Jitter monitor - Threshold for    | 0x00000000  |
// |       |                          | mid-to-low level on combined      |             |
// |       |                          | metric                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_jm_th_mid_low_err
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_CFG5_CFG_JM_TH_MID_LOW_ERR                                (0u)
#define BITL_PHY_OA_DCQ_JM_CFG5_CFG_JM_TH_MID_LOW_ERR                               (16u)
#define BITM_PHY_OA_DCQ_JM_CFG5_CFG_JM_TH_MID_LOW_ERR                       (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_rb0
// Register Addr        : 0x0093u
// Register NW Addr     : 0xE093u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_RB0_ADDR                                                  (0x0093u)
#define PHY_OA_DCQ_JM_RB0_NW_ADDR                                               (0xE093u)
#define PHY_OA_DCQ_JM_RB0_RESET_VAL                                         (0x00000000u)
#define PHY_OA_DCQ_JM_RB0_MMS                                                        (4u)
#define PHY_OA_DCQ_JM_RB0_VOL_MASK                                          (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 14    | rb_jm_out_valid          | Jitter monitor - Flags when the   | 0x00000000  |
// |       |                          | metric read-backs are valid       |             |
// |       |                          | (integrator reached maximum       |             |
// |       |                          | shift) but the additional         |             |
// |       |                          | settling time might have not      |             |
// |       |                          | elapsed.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | rb_jm_out_settled        | Jitter monitor - Flags when the   | 0x00000000  |
// |       |                          | metric read-backs are valid       |             |
// |       |                          | (integrator reached maximum       |             |
// |       |                          | shift) and additional settling    |             |
// |       |                          | time has elapsed.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: rb_jm_out_valid
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID                                      (14u)
#define BITL_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID                                       (1u)
#define BITM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID                              (0x00004000u)
// Enums
#define ENUM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID_JM_NOT_VALID                 (0x00000000u)
#define ENUM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID_JM_VALID                     (0x00000001u)

// Bit field: rb_jm_out_settled
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED                                    (15u)
#define BITL_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED                                     (1u)
#define BITM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED                            (0x00008000u)
// Enums
#define ENUM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED_JM_NOT_SETTLED             (0x00000000u)
#define ENUM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED_JM_SETTLED                 (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_rb4
// Register Addr        : 0x0097u
// Register NW Addr     : 0xE097u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_RB4_ADDR                                                  (0x0097u)
#define PHY_OA_DCQ_JM_RB4_NW_ADDR                                               (0xE097u)
#define PHY_OA_DCQ_JM_RB4_RESET_VAL                                         (0x00000000u)
#define PHY_OA_DCQ_JM_RB4_MMS                                                        (4u)
#define PHY_OA_DCQ_JM_RB4_VOL_MASK                                          (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-13 | rb_jm_quality_index      | Jitter monitor - Quality index    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: rb_jm_quality_index
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX                                  (13u)
#define BITL_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX                                   (3u)
#define BITM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX                          (0x0000E000u)
// Enums
#define ENUM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX_ENUM_SQI2                (0x00000004u)
#define ENUM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX_ENUM_SQI1                (0x00000002u)
#define ENUM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX_ENUM_SQI0                (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : dcq_jm_rb5
// Register Addr        : 0x0098u
// Register NW Addr     : 0xE098u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_DCQ_JM_RB5_ADDR                                                  (0x0098u)
#define PHY_OA_DCQ_JM_RB5_NW_ADDR                                               (0xE098u)
#define PHY_OA_DCQ_JM_RB5_RESET_VAL                                         (0x00000000u)
#define PHY_OA_DCQ_JM_RB5_MMS                                                        (4u)
#define PHY_OA_DCQ_JM_RB5_VOL_MASK                                          (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | rb_jm_comb_metric        | Jitter monitor - Jitter metric    | 0x00000000  |
// |       |                          | on combined metric, normalized    |             |
// |       |                          | per frame length. Notation is     |             |
// |       |                          | fixed point 0.14 (divide this     |             |
// |       |                          | integer by 16384.0)               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: rb_jm_comb_metric
// Bit position, length and mask
#define BITP_PHY_OA_DCQ_JM_RB5_RB_JM_COMB_METRIC                                     (0u)
#define BITL_PHY_OA_DCQ_JM_RB5_RB_JM_COMB_METRIC                                    (16u)
#define BITM_PHY_OA_DCQ_JM_RB5_RB_JM_COMB_METRIC                            (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_IDVER
// Register Addr        : 0xCA00u
// Register NW Addr     : 0xE200u
// Register Reset Value : 0x00000A10u
// Register MMS         : 4u

#define PHY_OA_PLCA_IDVER_ADDR                                                  (0xCA00u)
#define PHY_OA_PLCA_IDVER_NW_ADDR                                               (0xE200u)
#define PHY_OA_PLCA_IDVER_RESET_VAL                                         (0x00000A10u)
#define PHY_OA_PLCA_IDVER_MMS                                                        (4u)
#define PHY_OA_PLCA_IDVER_VOL_MASK                                          (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaVER                  | This Field Indicates The Version  | 0x00000010  |
// |       |                          | of The OA Memory Map Definition   |             |
// |       |                          | This Layout Adheres To            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | plcaIDM                  | This Field is The Memory Map      | 0x0000000a  |
// |       |                          | Identifier Which Indicates        |             |
// |       |                          | Compatibility With The OA PLCA    |             |
// |       |                          | Memory Map Definition             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaVER
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_IDVER_PLCAVER                                               (0u)
#define BITL_PHY_OA_PLCA_IDVER_PLCAVER                                               (8u)
#define BITM_PHY_OA_PLCA_IDVER_PLCAVER                                      (0x000000FFu)

// Bit field: plcaIDM
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_IDVER_PLCAIDM                                               (8u)
#define BITL_PHY_OA_PLCA_IDVER_PLCAIDM                                               (8u)
#define BITM_PHY_OA_PLCA_IDVER_PLCAIDM                                      (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_CTRL0
// Register Addr        : 0xCA01u
// Register NW Addr     : 0xE201u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_PLCA_CTRL0_ADDR                                                  (0xCA01u)
#define PHY_OA_PLCA_CTRL0_NW_ADDR                                               (0xE201u)
#define PHY_OA_PLCA_CTRL0_RESET_VAL                                         (0x00000000u)
#define PHY_OA_PLCA_CTRL0_MMS                                                        (4u)
#define PHY_OA_PLCA_CTRL0_VOL_MASK                                          (0x0000C000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 14    | plcaRst                  | PLCA reset. Self-clearing bit     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | plcaEn                   | PLCA enable                       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaRst
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL0_PLCARST                                              (14u)
#define BITL_PHY_OA_PLCA_CTRL0_PLCARST                                               (1u)
#define BITM_PHY_OA_PLCA_CTRL0_PLCARST                                      (0x00004000u)
// Enums
#define ENUM_PHY_OA_PLCA_CTRL0_PLCARST_INACTIVE                             (0x00000000u)
#define ENUM_PHY_OA_PLCA_CTRL0_PLCARST_TRIGGER                              (0x00000001u)

// Bit field: plcaEn
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL0_PLCAEN                                               (15u)
#define BITL_PHY_OA_PLCA_CTRL0_PLCAEN                                                (1u)
#define BITM_PHY_OA_PLCA_CTRL0_PLCAEN                                       (0x00008000u)
// Enums
#define ENUM_PHY_OA_PLCA_CTRL0_PLCAEN_DISABLED                              (0x00000000u)
#define ENUM_PHY_OA_PLCA_CTRL0_PLCAEN_ENABLED                               (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_CTRL1
// Register Addr        : 0xCA02u
// Register NW Addr     : 0xE202u
// Register Reset Value : 0x000008FFu
// Register MMS         : 4u

#define PHY_OA_PLCA_CTRL1_ADDR                                                  (0xCA02u)
#define PHY_OA_PLCA_CTRL1_NW_ADDR                                               (0xE202u)
#define PHY_OA_PLCA_CTRL1_RESET_VAL                                         (0x000008FFu)
#define PHY_OA_PLCA_CTRL1_MMS                                                        (4u)
#define PHY_OA_PLCA_CTRL1_VOL_MASK                                          (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaLocalID              | PLCA local ID                     | 0x000000ff  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | plcaNodeCount            | PLCA node count                   | 0x00000008  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaLocalID
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL1_PLCALOCALID                                           (0u)
#define BITL_PHY_OA_PLCA_CTRL1_PLCALOCALID                                           (8u)
#define BITM_PHY_OA_PLCA_CTRL1_PLCALOCALID                                  (0x000000FFu)

// Bit field: plcaNodeCount
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_CTRL1_PLCANODECOUNT                                         (8u)
#define BITL_PHY_OA_PLCA_CTRL1_PLCANODECOUNT                                         (8u)
#define BITM_PHY_OA_PLCA_CTRL1_PLCANODECOUNT                                (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_STATUS
// Register Addr        : 0xCA03u
// Register NW Addr     : 0xE203u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_PLCA_STATUS_ADDR                                                 (0xCA03u)
#define PHY_OA_PLCA_STATUS_NW_ADDR                                              (0xE203u)
#define PHY_OA_PLCA_STATUS_RESET_VAL                                        (0x00000000u)
#define PHY_OA_PLCA_STATUS_MMS                                                       (4u)
#define PHY_OA_PLCA_STATUS_VOL_MASK                                         (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15    | plcaStatus               | PLCA status                       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaStatus
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_STATUS_PLCASTATUS                                          (15u)
#define BITL_PHY_OA_PLCA_STATUS_PLCASTATUS                                           (1u)
#define BITM_PHY_OA_PLCA_STATUS_PLCASTATUS                                  (0x00008000u)
// Enums
#define ENUM_PHY_OA_PLCA_STATUS_PLCASTATUS_ENUM000                          (0x00000000u)
#define ENUM_PHY_OA_PLCA_STATUS_PLCASTATUS_ENUM001                          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_TOTMR
// Register Addr        : 0xCA04u
// Register NW Addr     : 0xE204u
// Register Reset Value : 0x00000020u
// Register MMS         : 4u

#define PHY_OA_PLCA_TOTMR_ADDR                                                  (0xCA04u)
#define PHY_OA_PLCA_TOTMR_NW_ADDR                                               (0xE204u)
#define PHY_OA_PLCA_TOTMR_RESET_VAL                                         (0x00000020u)
#define PHY_OA_PLCA_TOTMR_MMS                                                        (4u)
#define PHY_OA_PLCA_TOTMR_VOL_MASK                                          (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaTOTimer              | These bits configure the minimum  | 0x00000020  |
// |       |                          | duration of the PLCA transmit     |             |
// |       |                          | opportunity in units of Tbits     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaTOTimer
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_TOTMR_PLCATOTIMER                                           (0u)
#define BITL_PHY_OA_PLCA_TOTMR_PLCATOTIMER                                           (8u)
#define BITM_PHY_OA_PLCA_TOTMR_PLCATOTIMER                                  (0x000000FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_BURST
// Register Addr        : 0xCA05u
// Register NW Addr     : 0xE205u
// Register Reset Value : 0x00000080u
// Register MMS         : 4u

#define PHY_OA_PLCA_BURST_ADDR                                                  (0xCA05u)
#define PHY_OA_PLCA_BURST_NW_ADDR                                               (0xE205u)
#define PHY_OA_PLCA_BURST_RESET_VAL                                         (0x00000080u)
#define PHY_OA_PLCA_BURST_MMS                                                        (4u)
#define PHY_OA_PLCA_BURST_VOL_MASK                                          (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | plcaBurstTimer           | These bits configure the maximum  | 0x00000080  |
// |       |                          | time to wait for a new packet     |             |
// |       |                          | from the MAC after successful     |             |
// |       |                          | transmission (100ns units of      |             |
// |       |                          | time)                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | plcaMaxBurstCnt          | PLCA maximum burst count          | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaBurstTimer
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_BURST_PLCABURSTTIMER                                        (0u)
#define BITL_PHY_OA_PLCA_BURST_PLCABURSTTIMER                                        (8u)
#define BITM_PHY_OA_PLCA_BURST_PLCABURSTTIMER                               (0x000000FFu)

// Bit field: plcaMaxBurstCnt
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_BURST_PLCAMAXBURSTCNT                                       (8u)
#define BITL_PHY_OA_PLCA_BURST_PLCAMAXBURSTCNT                                       (8u)
#define BITM_PHY_OA_PLCA_BURST_PLCAMAXBURSTCNT                              (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_DIAG
// Register Addr        : 0xCA06u
// Register NW Addr     : 0xE206u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_PLCA_DIAG_ADDR                                                   (0xCA06u)
#define PHY_OA_PLCA_DIAG_NW_ADDR                                                (0xE206u)
#define PHY_OA_PLCA_DIAG_RESET_VAL                                          (0x00000000u)
#define PHY_OA_PLCA_DIAG_MMS                                                         (4u)
#define PHY_OA_PLCA_DIAG_VOL_MASK                                           (0x00000007u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | plcaDiagBcnBfTO          | PLCA diagnostic beacon before     | 0x00000000  |
// |       |                          | transmit opportunity              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | plcaDiagUnexpB           | PLCA diagnostic unexpected        | 0x00000000  |
// |       |                          | beacon                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | plcaDiagRxInTO           | PLCA diagnostic RX In transmit    | 0x00000000  |
// |       |                          | opportunity                       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaDiagBcnBfTO
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_DIAG_PLCADIAGBCNBFTO                                        (0u)
#define BITL_PHY_OA_PLCA_DIAG_PLCADIAGBCNBFTO                                        (1u)
#define BITM_PHY_OA_PLCA_DIAG_PLCADIAGBCNBFTO                               (0x00000001u)
// Enums
#define ENUM_PHY_OA_PLCA_DIAG_PLCADIAGBCNBFTO_ENUM000                       (0x00000000u)
#define ENUM_PHY_OA_PLCA_DIAG_PLCADIAGBCNBFTO_ENUM001                       (0x00000001u)

// Bit field: plcaDiagUnexpB
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_DIAG_PLCADIAGUNEXPB                                         (1u)
#define BITL_PHY_OA_PLCA_DIAG_PLCADIAGUNEXPB                                         (1u)
#define BITM_PHY_OA_PLCA_DIAG_PLCADIAGUNEXPB                                (0x00000002u)
// Enums
#define ENUM_PHY_OA_PLCA_DIAG_PLCADIAGUNEXPB_ENUM000                        (0x00000000u)
#define ENUM_PHY_OA_PLCA_DIAG_PLCADIAGUNEXPB_ENUM001                        (0x00000001u)

// Bit field: plcaDiagRxInTO
// Bit position, length and mask
#define BITP_PHY_OA_PLCA_DIAG_PLCADIAGRXINTO                                         (2u)
#define BITL_PHY_OA_PLCA_DIAG_PLCADIAGRXINTO                                         (1u)
#define BITM_PHY_OA_PLCA_DIAG_PLCADIAGRXINTO                                (0x00000004u)
// Enums
#define ENUM_PHY_OA_PLCA_DIAG_PLCADIAGRXINTO_ENUM000                        (0x00000000u)
#define ENUM_PHY_OA_PLCA_DIAG_PLCADIAGRXINTO_ENUM001                        (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_ctrl
// Register Addr        : 0xCE00u
// Register NW Addr     : 0xE600u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_CTRL_ADDR                                                     (0xCE00u)
#define PHY_OA_TD_CTRL_NW_ADDR                                                  (0xE600u)
#define PHY_OA_TD_CTRL_RESET_VAL                                            (0x00000000u)
#define PHY_OA_TD_CTRL_MMS                                                           (4u)
#define PHY_OA_TD_CTRL_VOL_MASK                                             (0x0000FF80u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 7     | td_auto_start            | Topology Discovery - Run the      | 0x00000000  |
// |       |                          | automatic flow                    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | td_dm_start              | Topology Discovery - Run the      | 0x00000000  |
// |       |                          | Distance Measurement Step         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12-09 | td_dm_dur                | Topology Discovery - Set the      | 0x00000000  |
// |       |                          | duration of Measurement in ms (0  |             |
// |       |                          | = 1ms)                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | td_dlym_start            | Topology Discovery - Run the      | 0x00000000  |
// |       |                          | internal delay measurement Step   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | td_refn                  | Topology Discovery - Define       | 0x00000000  |
// |       |                          | which type of node this is.       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | td_en                    | Topology Discovery - Main         | 0x00000000  |
// |       |                          | enable. The topology discovery    |             |
// |       |                          | takes over transmit path while    |             |
// |       |                          | disabling it for data frames      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_auto_start
// Bit position, length and mask
#define BITP_PHY_OA_TD_CTRL_TD_AUTO_START                                            (7u)
#define BITL_PHY_OA_TD_CTRL_TD_AUTO_START                                            (1u)
#define BITM_PHY_OA_TD_CTRL_TD_AUTO_START                                   (0x00000080u)
// Enums
#define ENUM_PHY_OA_TD_CTRL_TD_AUTO_START_INACTIVE                          (0x00000000u)
#define ENUM_PHY_OA_TD_CTRL_TD_AUTO_START_TRIGGER                           (0x00000001u)

// Bit field: td_dm_start
// Bit position, length and mask
#define BITP_PHY_OA_TD_CTRL_TD_DM_START                                              (8u)
#define BITL_PHY_OA_TD_CTRL_TD_DM_START                                              (1u)
#define BITM_PHY_OA_TD_CTRL_TD_DM_START                                     (0x00000100u)
// Enums
#define ENUM_PHY_OA_TD_CTRL_TD_DM_START_INACTIVE                            (0x00000000u)
#define ENUM_PHY_OA_TD_CTRL_TD_DM_START_TRIGGER                             (0x00000001u)

// Bit field: td_dm_dur
// Bit position, length and mask
#define BITP_PHY_OA_TD_CTRL_TD_DM_DUR                                                (9u)
#define BITL_PHY_OA_TD_CTRL_TD_DM_DUR                                                (4u)
#define BITM_PHY_OA_TD_CTRL_TD_DM_DUR                                       (0x00001E00u)

// Bit field: td_dlym_start
// Bit position, length and mask
#define BITP_PHY_OA_TD_CTRL_TD_DLYM_START                                           (13u)
#define BITL_PHY_OA_TD_CTRL_TD_DLYM_START                                            (1u)
#define BITM_PHY_OA_TD_CTRL_TD_DLYM_START                                   (0x00002000u)
// Enums
#define ENUM_PHY_OA_TD_CTRL_TD_DLYM_START_INACTIVE                          (0x00000000u)
#define ENUM_PHY_OA_TD_CTRL_TD_DLYM_START_TRIGGER                           (0x00000001u)

// Bit field: td_refn
// Bit position, length and mask
#define BITP_PHY_OA_TD_CTRL_TD_REFN                                                 (14u)
#define BITL_PHY_OA_TD_CTRL_TD_REFN                                                  (1u)
#define BITM_PHY_OA_TD_CTRL_TD_REFN                                         (0x00004000u)
// Enums
#define ENUM_PHY_OA_TD_CTRL_TD_REFN_ENUM000                                 (0x00000000u)
#define ENUM_PHY_OA_TD_CTRL_TD_REFN_ENUM001                                 (0x00000001u)

// Bit field: td_en
// Bit position, length and mask
#define BITP_PHY_OA_TD_CTRL_TD_EN                                                   (15u)
#define BITL_PHY_OA_TD_CTRL_TD_EN                                                    (1u)
#define BITM_PHY_OA_TD_CTRL_TD_EN                                           (0x00008000u)
// Enums
#define ENUM_PHY_OA_TD_CTRL_TD_EN_DISABLED                                  (0x00000000u)
#define ENUM_PHY_OA_TD_CTRL_TD_EN_ENABLED                                   (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_stat
// Register Addr        : 0xCE01u
// Register NW Addr     : 0xE601u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_STAT_ADDR                                                     (0xCE01u)
#define PHY_OA_TD_STAT_NW_ADDR                                                  (0xE601u)
#define PHY_OA_TD_STAT_RESET_VAL                                            (0x00000000u)
#define PHY_OA_TD_STAT_MMS                                                           (4u)
#define PHY_OA_TD_STAT_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11    | td_auto_err              | Topology Discovery - Error in     | 0x00000000  |
// |       |                          | the automatic flow                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | td_dm_err                | Topology Discovery - Error in     | 0x00000000  |
// |       |                          | Distance Measurement              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | td_dm_done               | Topology Discovery - Distance     | 0x00000000  |
// |       |                          | Measurement Done                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | td_dlym_err              | Topology Discovery - Error in     | 0x00000000  |
// |       |                          | the internal delay measurement    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | td_dlym_done             | Topology Discovery - Internal     | 0x00000000  |
// |       |                          | delay measurement Done            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_auto_err
// Bit position, length and mask
#define BITP_PHY_OA_TD_STAT_TD_AUTO_ERR                                             (11u)
#define BITL_PHY_OA_TD_STAT_TD_AUTO_ERR                                              (1u)
#define BITM_PHY_OA_TD_STAT_TD_AUTO_ERR                                     (0x00000800u)
// Enums
#define ENUM_PHY_OA_TD_STAT_TD_AUTO_ERR_ENUM000                             (0x00000000u)
#define ENUM_PHY_OA_TD_STAT_TD_AUTO_ERR_ENUM001                             (0x00000001u)

// Bit field: td_dm_err
// Bit position, length and mask
#define BITP_PHY_OA_TD_STAT_TD_DM_ERR                                               (12u)
#define BITL_PHY_OA_TD_STAT_TD_DM_ERR                                                (1u)
#define BITM_PHY_OA_TD_STAT_TD_DM_ERR                                       (0x00001000u)
// Enums
#define ENUM_PHY_OA_TD_STAT_TD_DM_ERR_ENUM000                               (0x00000000u)
#define ENUM_PHY_OA_TD_STAT_TD_DM_ERR_ENUM001                               (0x00000001u)

// Bit field: td_dm_done
// Bit position, length and mask
#define BITP_PHY_OA_TD_STAT_TD_DM_DONE                                              (13u)
#define BITL_PHY_OA_TD_STAT_TD_DM_DONE                                               (1u)
#define BITM_PHY_OA_TD_STAT_TD_DM_DONE                                      (0x00002000u)
// Enums
#define ENUM_PHY_OA_TD_STAT_TD_DM_DONE_ENUM000                              (0x00000000u)
#define ENUM_PHY_OA_TD_STAT_TD_DM_DONE_ENUM001                              (0x00000001u)

// Bit field: td_dlym_err
// Bit position, length and mask
#define BITP_PHY_OA_TD_STAT_TD_DLYM_ERR                                             (14u)
#define BITL_PHY_OA_TD_STAT_TD_DLYM_ERR                                              (1u)
#define BITM_PHY_OA_TD_STAT_TD_DLYM_ERR                                     (0x00004000u)
// Enums
#define ENUM_PHY_OA_TD_STAT_TD_DLYM_ERR_ENUM000                             (0x00000000u)
#define ENUM_PHY_OA_TD_STAT_TD_DLYM_ERR_ENUM001                             (0x00000001u)

// Bit field: td_dlym_done
// Bit position, length and mask
#define BITP_PHY_OA_TD_STAT_TD_DLYM_DONE                                            (15u)
#define BITL_PHY_OA_TD_STAT_TD_DLYM_DONE                                             (1u)
#define BITM_PHY_OA_TD_STAT_TD_DLYM_DONE                                    (0x00008000u)
// Enums
#define ENUM_PHY_OA_TD_STAT_TD_DLYM_DONE_ENUM000                            (0x00000000u)
#define ENUM_PHY_OA_TD_STAT_TD_DLYM_DONE_ENUM001                            (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_dist_res_low
// Register Addr        : 0xCE02u
// Register NW Addr     : 0xE602u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_DIST_RES_LOW_ADDR                                             (0xCE02u)
#define PHY_OA_TD_DIST_RES_LOW_NW_ADDR                                          (0xE602u)
#define PHY_OA_TD_DIST_RES_LOW_RESET_VAL                                    (0x00000000u)
#define PHY_OA_TD_DIST_RES_LOW_MMS                                                   (4u)
#define PHY_OA_TD_DIST_RES_LOW_VOL_MASK                                     (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | td_dist_mr               | Topology Discovery - Result of    | 0x00000000  |
// |       |                          | the Distance Measurement in       |             |
// |       |                          | Number of Pulses Received         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_dist_mr
// Bit position, length and mask
#define BITP_PHY_OA_TD_DIST_RES_LOW_TD_DIST_MR                                       (0u)
#define BITL_PHY_OA_TD_DIST_RES_LOW_TD_DIST_MR                                      (16u)
#define BITM_PHY_OA_TD_DIST_RES_LOW_TD_DIST_MR                              (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_dist_res_up
// Register Addr        : 0xCE03u
// Register NW Addr     : 0xE603u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_DIST_RES_UP_ADDR                                              (0xCE03u)
#define PHY_OA_TD_DIST_RES_UP_NW_ADDR                                           (0xE603u)
#define PHY_OA_TD_DIST_RES_UP_RESET_VAL                                     (0x00000000u)
#define PHY_OA_TD_DIST_RES_UP_MMS                                                    (4u)
#define PHY_OA_TD_DIST_RES_UP_VOL_MASK                                      (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | td_dist_mr               | Topology Discovery - Result of    | 0x00000000  |
// |       |                          | the Distance Measurement in       |             |
// |       |                          | Number of Pulses Received         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_dist_mr
// Bit position, length and mask
#define BITP_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR                                        (0u)
#define BITL_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR                                       (16u)
#define BITM_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR                               (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_dly_res_low
// Register Addr        : 0xCE04u
// Register NW Addr     : 0xE604u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_DLY_RES_LOW_ADDR                                              (0xCE04u)
#define PHY_OA_TD_DLY_RES_LOW_NW_ADDR                                           (0xE604u)
#define PHY_OA_TD_DLY_RES_LOW_RESET_VAL                                     (0x00000000u)
#define PHY_OA_TD_DLY_RES_LOW_MMS                                                    (4u)
#define PHY_OA_TD_DLY_RES_LOW_VOL_MASK                                      (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | td_dlym_mr               | Topology Discovery - Result of    | 0x00000000  |
// |       |                          | the Internal Delay Measurement    |             |
// |       |                          | in Number of Pulses Received      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_dlym_mr
// Bit position, length and mask
#define BITP_PHY_OA_TD_DLY_RES_LOW_TD_DLYM_MR                                        (0u)
#define BITL_PHY_OA_TD_DLY_RES_LOW_TD_DLYM_MR                                       (16u)
#define BITM_PHY_OA_TD_DLY_RES_LOW_TD_DLYM_MR                               (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_dly_res_up
// Register Addr        : 0xCE05u
// Register NW Addr     : 0xE605u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_DLY_RES_UP_ADDR                                               (0xCE05u)
#define PHY_OA_TD_DLY_RES_UP_NW_ADDR                                            (0xE605u)
#define PHY_OA_TD_DLY_RES_UP_RESET_VAL                                      (0x00000000u)
#define PHY_OA_TD_DLY_RES_UP_MMS                                                     (4u)
#define PHY_OA_TD_DLY_RES_UP_VOL_MASK                                       (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | td_dlym_mr               | Topology Discovery - Result of    | 0x00000000  |
// |       |                          | the Internal Delay Measurement    |             |
// |       |                          | in Number of Pulses Received      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_dlym_mr
// Bit position, length and mask
#define BITP_PHY_OA_TD_DLY_RES_UP_TD_DLYM_MR                                         (0u)
#define BITL_PHY_OA_TD_DLY_RES_UP_TD_DLYM_MR                                        (16u)
#define BITM_PHY_OA_TD_DLY_RES_UP_TD_DLYM_MR                                (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_mndly_res_low
// Register Addr        : 0xCE06u
// Register NW Addr     : 0xE606u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_MNDLY_RES_LOW_ADDR                                            (0xCE06u)
#define PHY_OA_TD_MNDLY_RES_LOW_NW_ADDR                                         (0xE606u)
#define PHY_OA_TD_MNDLY_RES_LOW_RESET_VAL                                   (0x00000000u)
#define PHY_OA_TD_MNDLY_RES_LOW_MMS                                                  (4u)
#define PHY_OA_TD_MNDLY_RES_LOW_VOL_MASK                                    (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | td_remote_dlym_mr        | Topology Discovery - Result of    | 0x00000000  |
// |       |                          | the remote node's Internal Delay  |             |
// |       |                          | Measurement in Number of Pulses   |             |
// |       |                          | Received (auto mode)              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_remote_dlym_mr
// Bit position, length and mask
#define BITP_PHY_OA_TD_MNDLY_RES_LOW_TD_REMOTE_DLYM_MR                               (0u)
#define BITL_PHY_OA_TD_MNDLY_RES_LOW_TD_REMOTE_DLYM_MR                              (16u)
#define BITM_PHY_OA_TD_MNDLY_RES_LOW_TD_REMOTE_DLYM_MR                      (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_mndly_res_up
// Register Addr        : 0xCE07u
// Register NW Addr     : 0xE607u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_MNDLY_RES_UP_ADDR                                             (0xCE07u)
#define PHY_OA_TD_MNDLY_RES_UP_NW_ADDR                                          (0xE607u)
#define PHY_OA_TD_MNDLY_RES_UP_RESET_VAL                                    (0x00000000u)
#define PHY_OA_TD_MNDLY_RES_UP_MMS                                                   (4u)
#define PHY_OA_TD_MNDLY_RES_UP_VOL_MASK                                     (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | td_remote_dlym_mr        | Topology Discovery - Result of    | 0x00000000  |
// |       |                          | the remote node's Internal Delay  |             |
// |       |                          | Measurement in Number of Pulses   |             |
// |       |                          | Received (auto mode)              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_remote_dlym_mr
// Bit position, length and mask
#define BITP_PHY_OA_TD_MNDLY_RES_UP_TD_REMOTE_DLYM_MR                                (0u)
#define BITL_PHY_OA_TD_MNDLY_RES_UP_TD_REMOTE_DLYM_MR                               (16u)
#define BITM_PHY_OA_TD_MNDLY_RES_UP_TD_REMOTE_DLYM_MR                       (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : td_mndly_dur
// Register Addr        : 0xCE08u
// Register NW Addr     : 0xE608u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_TD_MNDLY_DUR_ADDR                                                (0xCE08u)
#define PHY_OA_TD_MNDLY_DUR_NW_ADDR                                             (0xE608u)
#define PHY_OA_TD_MNDLY_DUR_RESET_VAL                                       (0x00000000u)
#define PHY_OA_TD_MNDLY_DUR_MMS                                                      (4u)
#define PHY_OA_TD_MNDLY_DUR_VOL_MASK                                        (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-12 | td_remote_dlym_dur       | Topology Discovery - Estimated    | 0x00000000  |
// |       |                          | duration of the remote node's     |             |
// |       |                          | Internal Delay Measurement in ms  |             |
// |       |                          | (0 = 1ms) (auto mode)             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: td_remote_dlym_dur
// Bit position, length and mask
#define BITP_PHY_OA_TD_MNDLY_DUR_TD_REMOTE_DLYM_DUR                                 (12u)
#define BITL_PHY_OA_TD_MNDLY_DUR_TD_REMOTE_DLYM_DUR                                  (4u)
#define BITM_PHY_OA_TD_MNDLY_DUR_TD_REMOTE_DLYM_DUR                         (0x0000F000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ws_status
// Register Addr        : 0xD000u
// Register NW Addr     : 0xE800u
// Register Reset Value : 0x00008000u
// Register MMS         : 4u

#define PHY_OA_WS_STATUS_ADDR                                                   (0xD000u)
#define PHY_OA_WS_STATUS_NW_ADDR                                                (0xE800u)
#define PHY_OA_WS_STATUS_RESET_VAL                                          (0x00008000u)
#define PHY_OA_WS_STATUS_MMS                                                         (4u)
#define PHY_OA_WS_STATUS_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 14    | ws_lpfail                | Sleep/Wake - Low power entry      | 0x00000000  |
// |       |                          | request status. This bit is       |             |
// |       |                          | cleared when a request to         |             |
// |       |                          | transition to LOW POWER is        |             |
// |       |                          | received                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | ws_lpcap                 | Sleep/Wake - PM client            | 0x00000001  |
// |       |                          | capability                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: ws_lpfail
// Bit position, length and mask
#define BITP_PHY_OA_WS_STATUS_WS_LPFAIL                                             (14u)
#define BITL_PHY_OA_WS_STATUS_WS_LPFAIL                                              (1u)
#define BITM_PHY_OA_WS_STATUS_WS_LPFAIL                                     (0x00004000u)
// Enums
#define ENUM_PHY_OA_WS_STATUS_WS_LPFAIL_ENUM000                             (0x00000000u)
#define ENUM_PHY_OA_WS_STATUS_WS_LPFAIL_ENUM001                             (0x00000001u)

// Bit field: ws_lpcap
// Bit position, length and mask
#define BITP_PHY_OA_WS_STATUS_WS_LPCAP                                              (15u)
#define BITL_PHY_OA_WS_STATUS_WS_LPCAP                                               (1u)
#define BITM_PHY_OA_WS_STATUS_WS_LPCAP                                      (0x00008000u)
// Enums
#define ENUM_PHY_OA_WS_STATUS_WS_LPCAP_ENUM000                              (0x00000000u)
#define ENUM_PHY_OA_WS_STATUS_WS_LPCAP_ENUM001                              (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ws_ctrl
// Register Addr        : 0xD001u
// Register NW Addr     : 0xE801u
// Register Reset Value : 0x00000000u
// Register MMS         : 4u

#define PHY_OA_WS_CTRL_ADDR                                                     (0xD001u)
#define PHY_OA_WS_CTRL_NW_ADDR                                                  (0xE801u)
#define PHY_OA_WS_CTRL_RESET_VAL                                            (0x00000000u)
#define PHY_OA_WS_CTRL_MMS                                                           (4u)
#define PHY_OA_WS_CTRL_VOL_MASK                                             (0x0000C000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 14    | ws_lpexit                | Sleep/Wake - Request transition   | 0x00000000  |
// |       |                          | from low power on network         |             |
// |       |                          | segment. Send Wake-Up Signal      |             |
// |       |                          | (WUS). Self-clearing bit          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | ws_lpreq                 | Sleep/Wake - Request transition   | 0x00000000  |
// |       |                          | to low power on local node. Go    |             |
// |       |                          | to sleep. Self-clearing bit       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: ws_lpexit
// Bit position, length and mask
#define BITP_PHY_OA_WS_CTRL_WS_LPEXIT                                               (14u)
#define BITL_PHY_OA_WS_CTRL_WS_LPEXIT                                                (1u)
#define BITM_PHY_OA_WS_CTRL_WS_LPEXIT                                       (0x00004000u)
// Enums
#define ENUM_PHY_OA_WS_CTRL_WS_LPEXIT_INACTIVE                              (0x00000000u)
#define ENUM_PHY_OA_WS_CTRL_WS_LPEXIT_TRIGGER                               (0x00000001u)

// Bit field: ws_lpreq
// Bit position, length and mask
#define BITP_PHY_OA_WS_CTRL_WS_LPREQ                                                (15u)
#define BITL_PHY_OA_WS_CTRL_WS_LPREQ                                                 (1u)
#define BITM_PHY_OA_WS_CTRL_WS_LPREQ                                        (0x00008000u)
// Enums
#define ENUM_PHY_OA_WS_CTRL_WS_LPREQ_INACTIVE                               (0x00000000u)
#define ENUM_PHY_OA_WS_CTRL_WS_LPREQ_TRIGGER                                (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillPHY_OARegsData();
#endif


#endif /* ADI_PHY_OA_REG_H */

/** @} */

/*
 * EOF: www.analog.com
 */


