/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_phy_std_reg.h
 * @brief        : This is an auto generated header file with register
                   descriptions for PHY
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup macphy
 *  @{
 */

#ifndef ADI_PHY_STD_REG_H
#define ADI_PHY_STD_REG_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define PHY_STD_NUM_REGS    (17u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganPHY_STDRegAddr[PHY_STD_NUM_REGS];
extern uint32_t ganPHY_STDNwRegAddr[PHY_STD_NUM_REGS];
extern uint32_t ganPHY_STDRegMMS[PHY_STD_NUM_REGS];
extern uint32_t ganPHY_STDRegResetVal[PHY_STD_NUM_REGS];
extern const char *gasPHY_STDRegName[PHY_STD_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONTROL_REGISTER
// Register Addr        : 0xFF00u
// Register NW Addr     : 0xC400u
// Register Reset Value : 0x00001000u
// Register MMS         : 0u

#define PHY_STD_CONTROL_REGISTER_ADDR                                           (0xFF00u)
#define PHY_STD_CONTROL_REGISTER_NW_ADDR                                        (0xC400u)
#define PHY_STD_CONTROL_REGISTER_RESET_VAL                                  (0x00001000u)
#define PHY_STD_CONTROL_REGISTER_MMS                                                 (0u)
#define PHY_STD_CONTROL_REGISTER_VOL_MASK                                   (0x0000DE80u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 6     | speedSel1                | Speed indication MSB.             | 0x00000000  |
// |       |                          | speedSel[1:0] = 00b indicates     |             |
// |       |                          | 10Mbps operation                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | collisionTest            | Collision test mode enable. When  | 0x00000000  |
// |       |                          | Set The PHY Reports a Collision   |             |
// |       |                          | Within 512 BT For Any Frame       |             |
// |       |                          | Transmitted                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | duplexMode               | If High The PHY is Operating in   | 0x00000000  |
// |       |                          | Full-Duplex Mode, If Low The PHY  |             |
// |       |                          | is Operating in Half-Duplex       |             |
// |       |                          | Mode. T1S Operation is Always     |             |
// |       |                          | Half-Duplex                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | linkRst                  | Setting this bit causes the       | 0x00000000  |
// |       |                          | transceiver to reset its link     |             |
// |       |                          | status, then resume normal        |             |
// |       |                          | operation.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | lowPwrMode               | Low power mode enable             | 0x00000000  |
// |       |                          | (Redundant, Field Has No Effect)  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | linkCtl                  | Link control.  When zero, the     | 0x00000001  |
// |       |                          | PHY cannot transmit or receive.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | speedSel0                | Speed indication LSB.             | 0x00000000  |
// |       |                          | speedSel[1:0] = 00b indicates     |             |
// |       |                          | 10Mbps operation                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | loopBack                 | PMA loop back mode enable.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | sReset                   | Soft reset. When set a soft       | 0x00000000  |
// |       |                          | reset process is initiated to     |             |
// |       |                          | reset the PHY                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: speedSel1
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_SPEEDSEL1                                      (6u)
#define BITL_PHY_STD_CONTROL_REGISTER_SPEEDSEL1                                      (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_SPEEDSEL1                             (0x00000040u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_SPEEDSEL1_TENMBPS                     (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_SPEEDSEL1_HUNDREDMBPS                 (0x00000001u)
#define ENUM_PHY_STD_CONTROL_REGISTER_SPEEDSEL1_THOUSANDMBPS                (0x00000002u)
#define ENUM_PHY_STD_CONTROL_REGISTER_SPEEDSEL1_ENUM003                     (0x00000003u)

// Bit field: collisionTest
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_COLLISIONTEST                                  (7u)
#define BITL_PHY_STD_CONTROL_REGISTER_COLLISIONTEST                                  (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_COLLISIONTEST                         (0x00000080u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_COLLISIONTEST_DISABLE                 (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_COLLISIONTEST_ENABLE                  (0x00000001u)

// Bit field: duplexMode
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_DUPLEXMODE                                     (8u)
#define BITL_PHY_STD_CONTROL_REGISTER_DUPLEXMODE                                     (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_DUPLEXMODE                            (0x00000100u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_DUPLEXMODE_ENUM000                    (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_DUPLEXMODE_ENUM001                    (0x00000001u)

// Bit field: linkRst
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_LINKRST                                        (9u)
#define BITL_PHY_STD_CONTROL_REGISTER_LINKRST                                        (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_LINKRST                               (0x00000200u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_LINKRST_DISABLE                       (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_LINKRST_ENABLE                        (0x00000001u)

// Bit field: lowPwrMode
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_LOWPWRMODE                                    (11u)
#define BITL_PHY_STD_CONTROL_REGISTER_LOWPWRMODE                                     (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_LOWPWRMODE                            (0x00000800u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_LOWPWRMODE_DISABLE                    (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_LOWPWRMODE_ENABLE                     (0x00000001u)

// Bit field: linkCtl
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_LINKCTL                                       (12u)
#define BITL_PHY_STD_CONTROL_REGISTER_LINKCTL                                        (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_LINKCTL                               (0x00001000u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_LINKCTL_ENUM000                       (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_LINKCTL_ENUM001                       (0x00000001u)

// Bit field: speedSel0
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_SPEEDSEL0                                     (13u)
#define BITL_PHY_STD_CONTROL_REGISTER_SPEEDSEL0                                      (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_SPEEDSEL0                             (0x00002000u)

// Bit field: loopBack
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_LOOPBACK                                      (14u)
#define BITL_PHY_STD_CONTROL_REGISTER_LOOPBACK                                       (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_LOOPBACK                              (0x00004000u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_LOOPBACK_DISABLE                      (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_LOOPBACK_ENABLE                       (0x00000001u)

// Bit field: sReset
// Bit position, length and mask
#define BITP_PHY_STD_CONTROL_REGISTER_SRESET                                        (15u)
#define BITL_PHY_STD_CONTROL_REGISTER_SRESET                                         (1u)
#define BITM_PHY_STD_CONTROL_REGISTER_SRESET                                (0x00008000u)
// Enums
#define ENUM_PHY_STD_CONTROL_REGISTER_SRESET_INACTIVE                       (0x00000000u)
#define ENUM_PHY_STD_CONTROL_REGISTER_SRESET_TRIGGER                        (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : STATUS_REGISTER
// Register Addr        : 0xFF01u
// Register NW Addr     : 0xC401u
// Register Reset Value : 0x0000082Du
// Register MMS         : 0u

#define PHY_STD_STATUS_REGISTER_ADDR                                            (0xFF01u)
#define PHY_STD_STATUS_REGISTER_NW_ADDR                                         (0xC401u)
#define PHY_STD_STATUS_REGISTER_RESET_VAL                                   (0x0000082Du)
#define PHY_STD_STATUS_REGISTER_MMS                                                  (0u)
#define PHY_STD_STATUS_REGISTER_VOL_MASK                                    (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | extendedCapab            | This bit indicates that the PHY   | 0x00000001  |
// |       |                          | supports Clause 22 to Clause 45   |             |
// |       |                          | bridge access method.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | localJabber              | Local jabber event detected       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | linkStatus               | This bit is set by the PHY to     | 0x00000001  |
// |       |                          | report whether a valid link has   |             |
// |       |                          | been established.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | autoNegAbility           | This bit advertises that the PHY  | 0x00000001  |
// |       |                          | supports the Auto-Negotiation     |             |
// |       |                          | interface                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | remoteJabber             | Remote jabber event detected      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | linkNegComplete          | Link negotiation complete         | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | preambSuppr              | This bit indicates the PHY does   | 0x00000000  |
// |       |                          | not accept MDIO frames with a     |             |
// |       |                          | suppressed preamble.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | unidirAbility            | This bit indicates that the PHY   | 0x00000000  |
// |       |                          | can transmit from media           |             |
// |       |                          | independent interface (MII) only  |             |
// |       |                          | when the PHY has determined that  |             |
// |       |                          | a valid link has been             |             |
// |       |                          | established.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | speed10M                 | This bit indicates that the PHY   | 0x00000001  |
// |       |                          | operates at 10 Mb/s in            |             |
// |       |                          | half-duplex mode.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: extendedCapab
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_EXTENDEDCAPAB                                   (0u)
#define BITL_PHY_STD_STATUS_REGISTER_EXTENDEDCAPAB                                   (1u)
#define BITM_PHY_STD_STATUS_REGISTER_EXTENDEDCAPAB                          (0x00000001u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_EXTENDEDCAPAB_NOT_SUPPORTED            (0x00000000u)
#define ENUM_PHY_STD_STATUS_REGISTER_EXTENDEDCAPAB_SUPPORTED                (0x00000001u)

// Bit field: localJabber
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_LOCALJABBER                                     (1u)
#define BITL_PHY_STD_STATUS_REGISTER_LOCALJABBER                                     (1u)
#define BITM_PHY_STD_STATUS_REGISTER_LOCALJABBER                            (0x00000002u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_LOCALJABBER_DETECTED                   (0x00000001u)
#define ENUM_PHY_STD_STATUS_REGISTER_LOCALJABBER_NOT_DETECTED               (0x00000000u)

// Bit field: linkStatus
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_LINKSTATUS                                      (2u)
#define BITL_PHY_STD_STATUS_REGISTER_LINKSTATUS                                      (1u)
#define BITM_PHY_STD_STATUS_REGISTER_LINKSTATUS                             (0x00000004u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_LINKSTATUS_ENUM000                     (0x00000000u)
#define ENUM_PHY_STD_STATUS_REGISTER_LINKSTATUS_ENUM001                     (0x00000001u)

// Bit field: autoNegAbility
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_AUTONEGABILITY                                  (3u)
#define BITL_PHY_STD_STATUS_REGISTER_AUTONEGABILITY                                  (1u)
#define BITM_PHY_STD_STATUS_REGISTER_AUTONEGABILITY                         (0x00000008u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_AUTONEGABILITY_NOT_SUPPORTED           (0x00000000u)
#define ENUM_PHY_STD_STATUS_REGISTER_AUTONEGABILITY_SUPPORTED               (0x00000001u)

// Bit field: remoteJabber
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_REMOTEJABBER                                    (4u)
#define BITL_PHY_STD_STATUS_REGISTER_REMOTEJABBER                                    (1u)
#define BITM_PHY_STD_STATUS_REGISTER_REMOTEJABBER                           (0x00000010u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_REMOTEJABBER_NOT_DETECTED              (0x00000000u)
#define ENUM_PHY_STD_STATUS_REGISTER_REMOTEJABBER_DETECTED                  (0x00000001u)

// Bit field: linkNegComplete
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_LINKNEGCOMPLETE                                 (5u)
#define BITL_PHY_STD_STATUS_REGISTER_LINKNEGCOMPLETE                                 (1u)
#define BITM_PHY_STD_STATUS_REGISTER_LINKNEGCOMPLETE                        (0x00000020u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_LINKNEGCOMPLETE_NOT_TRIGGERED          (0x00000000u)
#define ENUM_PHY_STD_STATUS_REGISTER_LINKNEGCOMPLETE_TRIGGERED              (0x00000001u)

// Bit field: preambSuppr
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_PREAMBSUPPR                                     (6u)
#define BITL_PHY_STD_STATUS_REGISTER_PREAMBSUPPR                                     (1u)
#define BITM_PHY_STD_STATUS_REGISTER_PREAMBSUPPR                            (0x00000040u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_PREAMBSUPPR_NOT_ACTIVE                 (0x00000000u)
#define ENUM_PHY_STD_STATUS_REGISTER_PREAMBSUPPR_ACTIVE                     (0x00000001u)

// Bit field: unidirAbility
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_UNIDIRABILITY                                   (7u)
#define BITL_PHY_STD_STATUS_REGISTER_UNIDIRABILITY                                   (1u)
#define BITM_PHY_STD_STATUS_REGISTER_UNIDIRABILITY                          (0x00000080u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_UNIDIRABILITY_SUPPORTED                (0x00000001u)
#define ENUM_PHY_STD_STATUS_REGISTER_UNIDIRABILITY_NOT_SUPPORTED            (0x00000000u)

// Bit field: speed10M
// Bit position, length and mask
#define BITP_PHY_STD_STATUS_REGISTER_SPEED10M                                       (11u)
#define BITL_PHY_STD_STATUS_REGISTER_SPEED10M                                        (1u)
#define BITM_PHY_STD_STATUS_REGISTER_SPEED10M                               (0x00000800u)
// Enums
#define ENUM_PHY_STD_STATUS_REGISTER_SPEED10M_NOT_SUPPORTED                 (0x00000000u)
#define ENUM_PHY_STD_STATUS_REGISTER_SPEED10M_SUPPORTED                     (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PHY_IDENTIFIER_1
// Register Addr        : 0xFF02u
// Register NW Addr     : 0xC402u
// Register Reset Value : 0x00000283u
// Register MMS         : 0u

#define PHY_STD_PHY_IDENTIFIER_1_ADDR                                           (0xFF02u)
#define PHY_STD_PHY_IDENTIFIER_1_NW_ADDR                                        (0xC402u)
#define PHY_STD_PHY_IDENTIFIER_1_RESET_VAL                                  (0x00000283u)
#define PHY_STD_PHY_IDENTIFIER_1_MMS                                                 (0u)
#define PHY_STD_PHY_IDENTIFIER_1_VOL_MASK                                   (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | phyIdOut_3_18            | IEEE Organizationally Unique      | 0x00000283  |
// |       |                          | Identifier LSBs                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: phyIdOut_3_18
// Bit position, length and mask
#define BITP_PHY_STD_PHY_IDENTIFIER_1_PHYIDOUT_3_18                                  (0u)
#define BITL_PHY_STD_PHY_IDENTIFIER_1_PHYIDOUT_3_18                                 (16u)
#define BITM_PHY_STD_PHY_IDENTIFIER_1_PHYIDOUT_3_18                         (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PHY_IDENTIFIER_2
// Register Addr        : 0xFF03u
// Register NW Addr     : 0xC403u
// Register Reset Value : 0x0000BE00u
// Register MMS         : 0u

#define PHY_STD_PHY_IDENTIFIER_2_ADDR                                           (0xFF03u)
#define PHY_STD_PHY_IDENTIFIER_2_NW_ADDR                                        (0xC403u)
#define PHY_STD_PHY_IDENTIFIER_2_RESET_VAL                                  (0x0000BE00u)
#define PHY_STD_PHY_IDENTIFIER_2_MMS                                                 (0u)
#define PHY_STD_PHY_IDENTIFIER_2_VOL_MASK                                   (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | phyIdChipRev             | Manufacturers Revision Number     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 09-04 | phyIdIcModel             | Manufacturers Model Number        | 0x00000020  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-10 | phyIdOut_19_24           | IEEE Organizationally Unique      | 0x0000002f  |
// |       |                          | Ddentifier MSBs                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: phyIdChipRev
// Bit position, length and mask
#define BITP_PHY_STD_PHY_IDENTIFIER_2_PHYIDCHIPREV                                   (0u)
#define BITL_PHY_STD_PHY_IDENTIFIER_2_PHYIDCHIPREV                                   (4u)
#define BITM_PHY_STD_PHY_IDENTIFIER_2_PHYIDCHIPREV                          (0x0000000Fu)

// Bit field: phyIdIcModel
// Bit position, length and mask
#define BITP_PHY_STD_PHY_IDENTIFIER_2_PHYIDICMODEL                                   (4u)
#define BITL_PHY_STD_PHY_IDENTIFIER_2_PHYIDICMODEL                                   (6u)
#define BITM_PHY_STD_PHY_IDENTIFIER_2_PHYIDICMODEL                          (0x000003F0u)

// Bit field: phyIdOut_19_24
// Bit position, length and mask
#define BITP_PHY_STD_PHY_IDENTIFIER_2_PHYIDOUT_19_24                                (10u)
#define BITL_PHY_STD_PHY_IDENTIFIER_2_PHYIDOUT_19_24                                 (6u)
#define BITM_PHY_STD_PHY_IDENTIFIER_2_PHYIDOUT_19_24                        (0x0000FC00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DEV_IN_PACKAGE_1_1
// Register Addr        : 0x0005u
// Register NW Addr     : 0xC425u
// Register Reset Value : 0x0000000Bu
// Register MMS         : 3u

#define PHY_STD_DEV_IN_PACKAGE_1_1_ADDR                                         (0x0005u)
#define PHY_STD_DEV_IN_PACKAGE_1_1_NW_ADDR                                      (0xC425u)
#define PHY_STD_DEV_IN_PACKAGE_1_1_RESET_VAL                                (0x0000000Bu)
#define PHY_STD_DEV_IN_PACKAGE_1_1_MMS                                               (3u)
#define PHY_STD_DEV_IN_PACKAGE_1_1_VOL_MASK                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | c22DevInPkg              | Clause 22 registers present in    | 0x00000001  |
// |       |                          | package                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | pmaDevInPkg              | PMA present in package            | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | WIS_Present              | WIS Present in Package            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | pcsDevInPkg              | PCS present in package            | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | PHYXS_Present            | PHY XS Present in Package         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | DTEXS_Present            | DTE XS Present in Package         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | TC_Present               | TC Present in Package             | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | AN_Present               | Auto Negotiation Present in       | 0x00000000  |
// |       |                          | Package                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | SPMA_Present             | Separated PMA Present in Package  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | OFDM_Present             | OFDM Present in Package           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | Power_Unit_Present       | Power Unit Present in Package     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: c22DevInPkg
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_C22DEVINPKG                                  (0u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_C22DEVINPKG                                  (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_C22DEVINPKG                         (0x00000001u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_C22DEVINPKG_NOT_PRESENT             (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_C22DEVINPKG_PRESENT                 (0x00000001u)

// Bit field: pmaDevInPkg
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_PMADEVINPKG                                  (1u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_PMADEVINPKG                                  (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_PMADEVINPKG                         (0x00000002u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_PMADEVINPKG_NOT_PRESENT             (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_PMADEVINPKG_PRESENT                 (0x00000001u)

// Bit field: WIS_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_WIS_PRESENT                                  (2u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_WIS_PRESENT                                  (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_WIS_PRESENT                         (0x00000004u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_WIS_PRESENT_NOT_PRESENT             (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_WIS_PRESENT_PRESENT                 (0x00000001u)

// Bit field: pcsDevInPkg
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_PCSDEVINPKG                                  (3u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_PCSDEVINPKG                                  (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_PCSDEVINPKG                         (0x00000008u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_PCSDEVINPKG_NOT_PRESENT             (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_PCSDEVINPKG_PRESENT                 (0x00000001u)

// Bit field: PHYXS_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_PHYXS_PRESENT                                (4u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_PHYXS_PRESENT                                (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_PHYXS_PRESENT                       (0x00000010u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_PHYXS_PRESENT_PHYXS_NOT_PRESENT     (0x00000001u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_PHYXS_PRESENT_PHYXS_PRESENT         (0x00000000u)

// Bit field: DTEXS_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_DTEXS_PRESENT                                (5u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_DTEXS_PRESENT                                (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_DTEXS_PRESENT                       (0x00000020u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_DTEXS_PRESENT_NOT_PRESENT           (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_DTEXS_PRESENT_PRESENT               (0x00000001u)

// Bit field: TC_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_TC_PRESENT                                   (6u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_TC_PRESENT                                   (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_TC_PRESENT                          (0x00000040u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_TC_PRESENT_NOT_PRESENT              (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_TC_PRESENT_PRESENT                  (0x00000001u)

// Bit field: AN_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_AN_PRESENT                                   (7u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_AN_PRESENT                                   (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_AN_PRESENT                          (0x00000080u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_AN_PRESENT_NOT_PRESENT              (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_AN_PRESENT_PRESENT                  (0x00000001u)

// Bit field: SPMA_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_SPMA_PRESENT                                 (8u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_SPMA_PRESENT                                 (4u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_SPMA_PRESENT                        (0x00000F00u)

// Bit field: OFDM_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_OFDM_PRESENT                                (12u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_OFDM_PRESENT                                 (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_OFDM_PRESENT                        (0x00001000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_OFDM_PRESENT_NOT_PRESENT            (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_OFDM_PRESENT_PRESENT                (0x00000001u)

// Bit field: Power_Unit_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_1_POWER_UNIT_PRESENT                          (13u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_1_POWER_UNIT_PRESENT                           (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_1_POWER_UNIT_PRESENT                  (0x00002000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_POWER_UNIT_PRESENT_NOT_PRESENT      (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_1_POWER_UNIT_PRESENT_PRESENT          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DEV_IN_PACKAGE_1_2
// Register Addr        : 0x0006u
// Register NW Addr     : 0xC426u
// Register Reset Value : 0x00001000u
// Register MMS         : 3u

#define PHY_STD_DEV_IN_PACKAGE_1_2_ADDR                                         (0x0006u)
#define PHY_STD_DEV_IN_PACKAGE_1_2_NW_ADDR                                      (0xC426u)
#define PHY_STD_DEV_IN_PACKAGE_1_2_RESET_VAL                                (0x00001000u)
#define PHY_STD_DEV_IN_PACKAGE_1_2_MMS                                               (3u)
#define PHY_STD_DEV_IN_PACKAGE_1_2_VOL_MASK                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 12    | plcaDevInPkg             | PLCA present in package           | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | CL22E_Not_Present        | Clause 22 Extension Not Present   | 0x00000000  |
// |       |                          | in Package                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | VSD1_Present             | Vendor Specific Device 1 Present  | 0x00000000  |
// |       |                          | in Package                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | VSD2_Present             | Vendor Specific Device 2 Present  | 0x00000000  |
// |       |                          | in Package                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaDevInPkg
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_2_PLCADEVINPKG                                (12u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_2_PLCADEVINPKG                                 (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_2_PLCADEVINPKG                        (0x00001000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_PLCADEVINPKG_NOT_PRESENT            (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_PLCADEVINPKG_PRESENT                (0x00000001u)

// Bit field: CL22E_Not_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_2_CL22E_NOT_PRESENT                           (13u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_2_CL22E_NOT_PRESENT                            (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_2_CL22E_NOT_PRESENT                   (0x00002000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_CL22E_NOT_PRESENT_NOT_PRESENT       (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_CL22E_NOT_PRESENT_PRESENT           (0x00000001u)

// Bit field: VSD1_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_2_VSD1_PRESENT                                (14u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_2_VSD1_PRESENT                                 (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_2_VSD1_PRESENT                        (0x00004000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_VSD1_PRESENT_NOT_PRESENT            (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_VSD1_PRESENT_PRESENT                (0x00000001u)

// Bit field: VSD2_Present
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_1_2_VSD2_PRESENT                                (15u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_1_2_VSD2_PRESENT                                 (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_1_2_VSD2_PRESENT                        (0x00008000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_VSD2_PRESENT_NOT_PRESENT            (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_1_2_VSD2_PRESENT_PRESENT                (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PMA_PMD_EXTENDED_ABILITY2
// Register Addr        : 0x000Bu
// Register NW Addr     : 0xC42Bu
// Register Reset Value : 0x00000800u
// Register MMS         : 3u

#define PHY_STD_PMA_PMD_EXTENDED_ABILITY2_ADDR                                  (0x000Bu)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY2_NW_ADDR                               (0xC42Bu)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY2_RESET_VAL                         (0x00000800u)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY2_MMS                                        (3u)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY2_VOL_MASK                          (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11    | pmaExtAb10T1             | PHY Supports Base-T1 Extended     | 0x00000001  |
// |       |                          | Abilities                         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pmaExtAb10T1
// Bit position, length and mask
#define BITP_PHY_STD_PMA_PMD_EXTENDED_ABILITY2_PMAEXTAB10T1                         (11u)
#define BITL_PHY_STD_PMA_PMD_EXTENDED_ABILITY2_PMAEXTAB10T1                          (1u)
#define BITM_PHY_STD_PMA_PMD_EXTENDED_ABILITY2_PMAEXTAB10T1                 (0x00000800u)
// Enums
#define ENUM_PHY_STD_PMA_PMD_EXTENDED_ABILITY2_PMAEXTAB10T1_NOT_SUPPORTED   (0x00000000u)
#define ENUM_PHY_STD_PMA_PMD_EXTENDED_ABILITY2_PMAEXTAB10T1_SUPPORTED       (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PMA_PMD_EXTENDED_ABILITY
// Register Addr        : 0x0032u
// Register NW Addr     : 0xC432u
// Register Reset Value : 0x00000008u
// Register MMS         : 3u

#define PHY_STD_PMA_PMD_EXTENDED_ABILITY_ADDR                                   (0x0032u)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY_NW_ADDR                                (0xC432u)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY_RESET_VAL                          (0x00000008u)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY_MMS                                         (3u)
#define PHY_STD_PMA_PMD_EXTENDED_ABILITY_VOL_MASK                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 2     | pmaExtAb10T1L            | PHY Supports 10BASE-T1L           | 0x00000000  |
// |       |                          | Capability                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | pmaExtAb10T1S            | PHY Supports 10Base-T1S           | 0x00000001  |
// |       |                          | Capability                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pmaExtAb10T1L
// Bit position, length and mask
#define BITP_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1L                          (2u)
#define BITL_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1L                          (1u)
#define BITM_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1L                 (0x00000004u)
// Enums
#define ENUM_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1L_NOT_SUPPORTED   (0x00000000u)
#define ENUM_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1L_SUPPORTED       (0x00000001u)

// Bit field: pmaExtAb10T1S
// Bit position, length and mask
#define BITP_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1S                          (3u)
#define BITL_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1S                          (1u)
#define BITM_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1S                 (0x00000008u)
// Enums
#define ENUM_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1S_NOT_SUPPORTED   (0x00000000u)
#define ENUM_PHY_STD_PMA_PMD_EXTENDED_ABILITY_PMAEXTAB10T1S_SUPPORTED       (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : T1S_PMA_CONTROL
// Register Addr        : 0x08F9u
// Register NW Addr     : 0xC449u
// Register Reset Value : 0x00000400u
// Register MMS         : 3u

#define PHY_STD_T1S_PMA_CONTROL_ADDR                                            (0x08F9u)
#define PHY_STD_T1S_PMA_CONTROL_NW_ADDR                                         (0xC449u)
#define PHY_STD_T1S_PMA_CONTROL_RESET_VAL                                   (0x00000400u)
#define PHY_STD_T1S_PMA_CONTROL_MMS                                                  (3u)
#define PHY_STD_T1S_PMA_CONTROL_VOL_MASK                                    (0x0000C001u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | pmaLoopBack_1            | PMA loop back mode enable         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | pmaMultiDrop             | Multi-drop mode enabled           | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | lowPwrMode_1             | Low power mode enable             | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | pmaTxDisable             | PMA transmission disable          | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | sReset_1                 | PCS and PMA PHY Layer Reset       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pmaLoopBack_1
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_CONTROL_PMALOOPBACK_1                                   (0u)
#define BITL_PHY_STD_T1S_PMA_CONTROL_PMALOOPBACK_1                                   (1u)
#define BITM_PHY_STD_T1S_PMA_CONTROL_PMALOOPBACK_1                          (0x00000001u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_CONTROL_PMALOOPBACK_1_DISABLE                  (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_CONTROL_PMALOOPBACK_1_ENABLE                   (0x00000001u)

// Bit field: pmaMultiDrop
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_CONTROL_PMAMULTIDROP                                   (10u)
#define BITL_PHY_STD_T1S_PMA_CONTROL_PMAMULTIDROP                                    (1u)
#define BITM_PHY_STD_T1S_PMA_CONTROL_PMAMULTIDROP                           (0x00000400u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_CONTROL_PMAMULTIDROP_DISABLE                   (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_CONTROL_PMAMULTIDROP_ENABLE                    (0x00000001u)

// Bit field: lowPwrMode_1
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_CONTROL_LOWPWRMODE_1                                   (11u)
#define BITL_PHY_STD_T1S_PMA_CONTROL_LOWPWRMODE_1                                    (1u)
#define BITM_PHY_STD_T1S_PMA_CONTROL_LOWPWRMODE_1                           (0x00000800u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_CONTROL_LOWPWRMODE_1_DISABLE                   (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_CONTROL_LOWPWRMODE_1_ENABLE                    (0x00000001u)

// Bit field: pmaTxDisable
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_CONTROL_PMATXDISABLE                                   (14u)
#define BITL_PHY_STD_T1S_PMA_CONTROL_PMATXDISABLE                                    (1u)
#define BITM_PHY_STD_T1S_PMA_CONTROL_PMATXDISABLE                           (0x00004000u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_CONTROL_PMATXDISABLE_DISABLE                   (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_CONTROL_PMATXDISABLE_ENABLE                    (0x00000001u)

// Bit field: sReset_1
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_CONTROL_SRESET_1                                       (15u)
#define BITL_PHY_STD_T1S_PMA_CONTROL_SRESET_1                                        (1u)
#define BITM_PHY_STD_T1S_PMA_CONTROL_SRESET_1                               (0x00008000u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_CONTROL_SRESET_1_INACTIVE                      (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_CONTROL_SRESET_1_TRIGGER                       (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : T1S_PMA_STATUS
// Register Addr        : 0x08FAu
// Register NW Addr     : 0xC44Au
// Register Reset Value : 0x00002E00u
// Register MMS         : 3u

#define PHY_STD_T1S_PMA_STATUS_ADDR                                             (0x08FAu)
#define PHY_STD_T1S_PMA_STATUS_NW_ADDR                                          (0xC44Au)
#define PHY_STD_T1S_PMA_STATUS_RESET_VAL                                    (0x00002E00u)
#define PHY_STD_T1S_PMA_STATUS_MMS                                                   (3u)
#define PHY_STD_T1S_PMA_STATUS_VOL_MASK                                     (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 1     | remoteJabber_1           | Remote jabber event detected      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | pmaRxFaultAbili          | Detection of receive faults       | 0x00000001  |
// |       |                          | supported                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | pmaMultiDropAbi          | Half-duplex multi-drop mode       | 0x00000001  |
// |       |                          | supported                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | lowPwrAbility            | Low power mode interface          | 0x00000001  |
// |       |                          | supported                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | pmaLoopBackAbil          | PMA loop back mode supported      | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: remoteJabber_1
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_STATUS_REMOTEJABBER_1                                   (1u)
#define BITL_PHY_STD_T1S_PMA_STATUS_REMOTEJABBER_1                                   (1u)
#define BITM_PHY_STD_T1S_PMA_STATUS_REMOTEJABBER_1                          (0x00000002u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_STATUS_REMOTEJABBER_1_NOT_DETECTED             (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_STATUS_REMOTEJABBER_1_DETECTED                 (0x00000001u)

// Bit field: pmaRxFaultAbili
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_STATUS_PMARXFAULTABILI                                  (9u)
#define BITL_PHY_STD_T1S_PMA_STATUS_PMARXFAULTABILI                                  (1u)
#define BITM_PHY_STD_T1S_PMA_STATUS_PMARXFAULTABILI                         (0x00000200u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_STATUS_PMARXFAULTABILI_NOT_SUPPORTED           (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_STATUS_PMARXFAULTABILI_SUPPORTED               (0x00000001u)

// Bit field: pmaMultiDropAbi
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_STATUS_PMAMULTIDROPABI                                 (10u)
#define BITL_PHY_STD_T1S_PMA_STATUS_PMAMULTIDROPABI                                  (1u)
#define BITM_PHY_STD_T1S_PMA_STATUS_PMAMULTIDROPABI                         (0x00000400u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_STATUS_PMAMULTIDROPABI_NOT_SUPPORTED           (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_STATUS_PMAMULTIDROPABI_SUPPORTED               (0x00000001u)

// Bit field: lowPwrAbility
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_STATUS_LOWPWRABILITY                                   (11u)
#define BITL_PHY_STD_T1S_PMA_STATUS_LOWPWRABILITY                                    (1u)
#define BITM_PHY_STD_T1S_PMA_STATUS_LOWPWRABILITY                           (0x00000800u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_STATUS_LOWPWRABILITY_NOT_SUPPORTED             (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_STATUS_LOWPWRABILITY_SUPPORTED                 (0x00000001u)

// Bit field: pmaLoopBackAbil
// Bit position, length and mask
#define BITP_PHY_STD_T1S_PMA_STATUS_PMALOOPBACKABIL                                 (13u)
#define BITL_PHY_STD_T1S_PMA_STATUS_PMALOOPBACKABIL                                  (1u)
#define BITM_PHY_STD_T1S_PMA_STATUS_PMALOOPBACKABIL                         (0x00002000u)
// Enums
#define ENUM_PHY_STD_T1S_PMA_STATUS_PMALOOPBACKABIL_NOT_SUPPORTED           (0x00000000u)
#define ENUM_PHY_STD_T1S_PMA_STATUS_PMALOOPBACKABIL_SUPPORTED               (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : T1S_TEST_MODE_CONTROL
// Register Addr        : 0x08FBu
// Register NW Addr     : 0xC44Bu
// Register Reset Value : 0x00000000u
// Register MMS         : 3u

#define PHY_STD_T1S_TEST_MODE_CONTROL_ADDR                                      (0x08FBu)
#define PHY_STD_T1S_TEST_MODE_CONTROL_NW_ADDR                                   (0xC44Bu)
#define PHY_STD_T1S_TEST_MODE_CONTROL_RESET_VAL                             (0x00000000u)
#define PHY_STD_T1S_TEST_MODE_CONTROL_MMS                                            (3u)
#define PHY_STD_T1S_TEST_MODE_CONTROL_VOL_MASK                              (0x0000E000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-13 | pmaPatternSel            | PMA Tx Pattern Select             | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pmaPatternSel
// Bit position, length and mask
#define BITP_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL                            (13u)
#define BITL_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL                             (3u)
#define BITM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL                    (0x0000E000u)
// Enums
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM000            (0x00000000u)
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM001            (0x00000001u)
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM002            (0x00000002u)
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM003            (0x00000003u)
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM004            (0x00000004u)
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM005            (0x00000005u)
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM006            (0x00000006u)
#define ENUM_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL_ENUM007            (0x00000007u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DEV_IN_PACKAGE_3_1
// Register Addr        : 0x0005u
// Register NW Addr     : 0xC455u
// Register Reset Value : 0x0000000Bu
// Register MMS         : 2u

#define PHY_STD_DEV_IN_PACKAGE_3_1_ADDR                                         (0x0005u)
#define PHY_STD_DEV_IN_PACKAGE_3_1_NW_ADDR                                      (0xC455u)
#define PHY_STD_DEV_IN_PACKAGE_3_1_RESET_VAL                                (0x0000000Bu)
#define PHY_STD_DEV_IN_PACKAGE_3_1_MMS                                               (2u)
#define PHY_STD_DEV_IN_PACKAGE_3_1_VOL_MASK                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | c22DevInPkg_1            | Clause 22 registers present in    | 0x00000001  |
// |       |                          | package                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | pmaDevInPkg_1            | PMA present in package            | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | WIS_Present_1            | WIS Present in Package            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | pcsDevInPkg_1            | PCS present in package            | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | PHYXS_Present_1          | PHY XS Present in Package         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | DTEXS_Present_1          | DTE XS Present in Package         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | TC_Present_1             | TC Present in Package             | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | AN_Present_1             | Auto Negotiation Present in       | 0x00000000  |
// |       |                          | Package                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | SPMA_Present_1           | Separated PMA Present in Package  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | OFDM_Present_1           | OFDM Present in Package           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | Power_Unit_Present_1     | Power Unit Present in Package     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: c22DevInPkg_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_C22DEVINPKG_1                                (0u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_C22DEVINPKG_1                                (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_C22DEVINPKG_1                       (0x00000001u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_C22DEVINPKG_1_NOT_PRESENT           (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_C22DEVINPKG_1_PRESENT               (0x00000001u)

// Bit field: pmaDevInPkg_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_PMADEVINPKG_1                                (1u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_PMADEVINPKG_1                                (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_PMADEVINPKG_1                       (0x00000002u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_PMADEVINPKG_1_NOT_PRESENT           (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_PMADEVINPKG_1_PRESENT               (0x00000001u)

// Bit field: WIS_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_WIS_PRESENT_1                                (2u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_WIS_PRESENT_1                                (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_WIS_PRESENT_1                       (0x00000004u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_WIS_PRESENT_1_NOT_PRESENT           (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_WIS_PRESENT_1_PRESENT               (0x00000001u)

// Bit field: pcsDevInPkg_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_PCSDEVINPKG_1                                (3u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_PCSDEVINPKG_1                                (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_PCSDEVINPKG_1                       (0x00000008u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_PCSDEVINPKG_1_NOT_PRESENT           (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_PCSDEVINPKG_1_PRESENT               (0x00000001u)

// Bit field: PHYXS_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_PHYXS_PRESENT_1                              (4u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_PHYXS_PRESENT_1                              (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_PHYXS_PRESENT_1                     (0x00000010u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_PHYXS_PRESENT_1_NOT_PRESENT         (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_PHYXS_PRESENT_1_PRESENT             (0x00000001u)

// Bit field: DTEXS_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_DTEXS_PRESENT_1                              (5u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_DTEXS_PRESENT_1                              (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_DTEXS_PRESENT_1                     (0x00000020u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_DTEXS_PRESENT_1_NOT_PRESENT         (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_DTEXS_PRESENT_1_PRESENT             (0x00000001u)

// Bit field: TC_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_TC_PRESENT_1                                 (6u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_TC_PRESENT_1                                 (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_TC_PRESENT_1                        (0x00000040u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_TC_PRESENT_1_NOT_PRESENT            (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_TC_PRESENT_1_PRESENT                (0x00000001u)

// Bit field: AN_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_AN_PRESENT_1                                 (7u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_AN_PRESENT_1                                 (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_AN_PRESENT_1                        (0x00000080u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_AN_PRESENT_1_NOT_PRESENT            (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_AN_PRESENT_1_PRESENT                (0x00000001u)

// Bit field: SPMA_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_SPMA_PRESENT_1                               (8u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_SPMA_PRESENT_1                               (4u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_SPMA_PRESENT_1                      (0x00000F00u)

// Bit field: OFDM_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_OFDM_PRESENT_1                              (12u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_OFDM_PRESENT_1                               (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_OFDM_PRESENT_1                      (0x00001000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_OFDM_PRESENT_1_NOT_PRESENT          (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_OFDM_PRESENT_1_PRESENT              (0x00000001u)

// Bit field: Power_Unit_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_1_POWER_UNIT_PRESENT_1                        (13u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_1_POWER_UNIT_PRESENT_1                         (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_1_POWER_UNIT_PRESENT_1                (0x00002000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_POWER_UNIT_PRESENT_1_NOT_PRESENT    (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_1_POWER_UNIT_PRESENT_1_PRESENT        (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DEV_IN_PACKAGE_3_2
// Register Addr        : 0x0006u
// Register NW Addr     : 0xC456u
// Register Reset Value : 0x00001000u
// Register MMS         : 2u

#define PHY_STD_DEV_IN_PACKAGE_3_2_ADDR                                         (0x0006u)
#define PHY_STD_DEV_IN_PACKAGE_3_2_NW_ADDR                                      (0xC456u)
#define PHY_STD_DEV_IN_PACKAGE_3_2_RESET_VAL                                (0x00001000u)
#define PHY_STD_DEV_IN_PACKAGE_3_2_MMS                                               (2u)
#define PHY_STD_DEV_IN_PACKAGE_3_2_VOL_MASK                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 12    | plcaDevInPkg_1           | PLCA present in package           | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | CL22E_Not_Present_1      | Clause 22 Extension Not Present   | 0x00000000  |
// |       |                          | in Package                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | VSD1_Present_1           | Vendor Specific Device 1 Present  | 0x00000000  |
// |       |                          | in Package                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | VSD2_Present_1           | Vendor Specific Device 2 Present  | 0x00000000  |
// |       |                          | in Package                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: plcaDevInPkg_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_2_PLCADEVINPKG_1                              (12u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_2_PLCADEVINPKG_1                               (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_2_PLCADEVINPKG_1                      (0x00001000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_PLCADEVINPKG_1_NOT_PRESENT          (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_PLCADEVINPKG_1_PRESENT              (0x00000001u)

// Bit field: CL22E_Not_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_2_CL22E_NOT_PRESENT_1                         (13u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_2_CL22E_NOT_PRESENT_1                          (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_2_CL22E_NOT_PRESENT_1                 (0x00002000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_CL22E_NOT_PRESENT_1_NOT_PRESENT     (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_CL22E_NOT_PRESENT_1_PRESENT         (0x00000001u)

// Bit field: VSD1_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_2_VSD1_PRESENT_1                              (14u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_2_VSD1_PRESENT_1                               (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_2_VSD1_PRESENT_1                      (0x00004000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_VSD1_PRESENT_1_NOT_PRESENT          (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_VSD1_PRESENT_1_PRESENT              (0x00000001u)

// Bit field: VSD2_Present_1
// Bit position, length and mask
#define BITP_PHY_STD_DEV_IN_PACKAGE_3_2_VSD2_PRESENT_1                              (15u)
#define BITL_PHY_STD_DEV_IN_PACKAGE_3_2_VSD2_PRESENT_1                               (1u)
#define BITM_PHY_STD_DEV_IN_PACKAGE_3_2_VSD2_PRESENT_1                      (0x00008000u)
// Enums
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_VSD2_PRESENT_1_NOT_PRESENT          (0x00000000u)
#define ENUM_PHY_STD_DEV_IN_PACKAGE_3_2_VSD2_PRESENT_1_PRESENT              (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PCS_CONTROL
// Register Addr        : 0x08F3u
// Register NW Addr     : 0xC463u
// Register Reset Value : 0x00000000u
// Register MMS         : 2u

#define PHY_STD_PCS_CONTROL_ADDR                                                (0x08F3u)
#define PHY_STD_PCS_CONTROL_NW_ADDR                                             (0xC463u)
#define PHY_STD_PCS_CONTROL_RESET_VAL                                       (0x00000000u)
#define PHY_STD_PCS_CONTROL_MMS                                                      (2u)
#define PHY_STD_PCS_CONTROL_VOL_MASK                                        (0x0000C000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 14    | pcsLoopBack              | PCS loop back mode enable         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | pcsRst                   | PCS and PMA layer reset           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: pcsLoopBack
// Bit position, length and mask
#define BITP_PHY_STD_PCS_CONTROL_PCSLOOPBACK                                        (14u)
#define BITL_PHY_STD_PCS_CONTROL_PCSLOOPBACK                                         (1u)
#define BITM_PHY_STD_PCS_CONTROL_PCSLOOPBACK                                (0x00004000u)
// Enums
#define ENUM_PHY_STD_PCS_CONTROL_PCSLOOPBACK_DISABLE                        (0x00000000u)
#define ENUM_PHY_STD_PCS_CONTROL_PCSLOOPBACK_ENABLE                         (0x00000001u)

// Bit field: pcsRst
// Bit position, length and mask
#define BITP_PHY_STD_PCS_CONTROL_PCSRST                                             (15u)
#define BITL_PHY_STD_PCS_CONTROL_PCSRST                                              (1u)
#define BITM_PHY_STD_PCS_CONTROL_PCSRST                                     (0x00008000u)
// Enums
#define ENUM_PHY_STD_PCS_CONTROL_PCSRST_INACTIVE                            (0x00000000u)
#define ENUM_PHY_STD_PCS_CONTROL_PCSRST_TRIGGER                             (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PCS_STATUS
// Register Addr        : 0x08F4u
// Register NW Addr     : 0xC464u
// Register Reset Value : 0x00000000u
// Register MMS         : 2u

#define PHY_STD_PCS_STATUS_ADDR                                                 (0x08F4u)
#define PHY_STD_PCS_STATUS_NW_ADDR                                              (0xC464u)
#define PHY_STD_PCS_STATUS_RESET_VAL                                        (0x00000000u)
#define PHY_STD_PCS_STATUS_MMS                                                       (2u)
#define PHY_STD_PCS_STATUS_VOL_MASK                                         (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 7     | jabDetected              | Remote or local jabber detected   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: jabDetected
// Bit position, length and mask
#define BITP_PHY_STD_PCS_STATUS_JABDETECTED                                          (7u)
#define BITL_PHY_STD_PCS_STATUS_JABDETECTED                                          (1u)
#define BITM_PHY_STD_PCS_STATUS_JABDETECTED                                 (0x00000080u)
// Enums
#define ENUM_PHY_STD_PCS_STATUS_JABDETECTED_DETECTED                        (0x00000001u)
#define ENUM_PHY_STD_PCS_STATUS_JABDETECTED_NOT_DETECTED                    (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PCS_DIAGNOSTIC
// Register Addr        : 0x08F5u
// Register NW Addr     : 0xC465u
// Register Reset Value : 0x00000000u
// Register MMS         : 2u

#define PHY_STD_PCS_DIAGNOSTIC_ADDR                                             (0x08F5u)
#define PHY_STD_PCS_DIAGNOSTIC_NW_ADDR                                          (0xC465u)
#define PHY_STD_PCS_DIAGNOSTIC_RESET_VAL                                    (0x00000000u)
#define PHY_STD_PCS_DIAGNOSTIC_MMS                                                   (2u)
#define PHY_STD_PCS_DIAGNOSTIC_VOL_MASK                                     (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | remoteJabCnt             | Remote jabber error counter       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: remoteJabCnt
// Bit position, length and mask
#define BITP_PHY_STD_PCS_DIAGNOSTIC_REMOTEJABCNT                                     (0u)
#define BITL_PHY_STD_PCS_DIAGNOSTIC_REMOTEJABCNT                                    (16u)
#define BITM_PHY_STD_PCS_DIAGNOSTIC_REMOTEJABCNT                            (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PCS_DIAGNOSTIC_2
// Register Addr        : 0x08F6u
// Register NW Addr     : 0xC466u
// Register Reset Value : 0x00000000u
// Register MMS         : 2u

#define PHY_STD_PCS_DIAGNOSTIC_2_ADDR                                           (0x08F6u)
#define PHY_STD_PCS_DIAGNOSTIC_2_NW_ADDR                                        (0xC466u)
#define PHY_STD_PCS_DIAGNOSTIC_2_RESET_VAL                                  (0x00000000u)
#define PHY_STD_PCS_DIAGNOSTIC_2_MMS                                                 (2u)
#define PHY_STD_PCS_DIAGNOSTIC_2_VOL_MASK                                   (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | corruptedTxCnt           | Physical collision detected       | 0x00000000  |
// |       |                          | counter                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: corruptedTxCnt
// Bit position, length and mask
#define BITP_PHY_STD_PCS_DIAGNOSTIC_2_CORRUPTEDTXCNT                                 (0u)
#define BITL_PHY_STD_PCS_DIAGNOSTIC_2_CORRUPTEDTXCNT                                (16u)
#define BITM_PHY_STD_PCS_DIAGNOSTIC_2_CORRUPTEDTXCNT                        (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillPHY_STDRegsData();
#endif


#endif /* ADI_PHY_STD_REG_H */

/** @} */

/*
 * EOF: www.analog.com
 */


