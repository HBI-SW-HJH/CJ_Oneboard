/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_pmd_reg.h
 * @brief        : This is an auto generated header file with register
                   descriptions for PMD
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup macphy
 *  @{
 */

#ifndef ADI_PMD_REG_H
#define ADI_PMD_REG_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define PMD_NUM_REGS    (5u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganPMDRegAddr[PMD_NUM_REGS];
extern uint32_t ganPMDNwRegAddr[PMD_NUM_REGS];
extern uint32_t ganPMDRegMMS[PMD_NUM_REGS];
extern uint32_t ganPMDRegResetVal[PMD_NUM_REGS];
extern const char *gasPMDRegName[PMD_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : miimctl
// Register Addr        : 0xD200u
// Register NW Addr     : 0xB7E0u
// Register Reset Value : 0x00000000u
// Register MMS         : 0x4u

#define PMD_MIIMCTL_ADDR                                                        (0xD200u)
#define PMD_MIIMCTL_NW_ADDR                                                     (0xB7E0u)
#define PMD_MIIMCTL_RESET_VAL                                               (0x00000000u)
#define PMD_MIIMCTL_MMS                                                            (0x4u)
#define PMD_MIIMCTL_VOL_MASK                                                (0x0000C000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 14    | loopback                 | Control to enable a digital       | 0x00000000  |
// |       |                          | loopback while not transmitting   |             |
// |       |                          | on the cable                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | reset                    | Control to reset the block and    | 0x00000000  |
// |       |                          | its associated register map       |             |
// |       |                          | controls. The bit is only         |             |
// |       |                          | effective when PMD is not (or no  |             |
// |       |                          | longer) in CONFIGURATION mode.    |             |
// |       |                          | It clears automatically when the  |             |
// |       |                          | reset is applied internally       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: loopback
// Bit position, length and mask
#define BITP_PMD_MIIMCTL_LOOPBACK                                                   (14u)
#define BITL_PMD_MIIMCTL_LOOPBACK                                                    (1u)
#define BITM_PMD_MIIMCTL_LOOPBACK                                           (0x00004000u)
// Enums
#define ENUM_PMD_MIIMCTL_LOOPBACK_DISABLED                                  (0x00000000u)
#define ENUM_PMD_MIIMCTL_LOOPBACK_ENABLED                                   (0x00000001u)

// Bit field: reset
// Bit position, length and mask
#define BITP_PMD_MIIMCTL_RESET                                                      (15u)
#define BITL_PMD_MIIMCTL_RESET                                                       (1u)
#define BITM_PMD_MIIMCTL_RESET                                              (0x00008000u)
// Enums
#define ENUM_PMD_MIIMCTL_RESET_INACTIVE                                     (0x00000000u)
#define ENUM_PMD_MIIMCTL_RESET_TRIGGER                                      (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : phyidh
// Register Addr        : 0xD202u
// Register NW Addr     : 0xB7E2u
// Register Reset Value : 0x00000000u
// Register MMS         : 0x4u

#define PMD_PHYIDH_ADDR                                                         (0xD202u)
#define PMD_PHYIDH_NW_ADDR                                                      (0xB7E2u)
#define PMD_PHYIDH_RESET_VAL                                                (0x00000000u)
#define PMD_PHYIDH_MMS                                                             (0x4u)
#define PMD_PHYIDH_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | phyid                    | Readback of PHY identifier        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: phyid
// Bit position, length and mask
#define BITP_PMD_PHYIDH_PHYID                                                        (0u)
#define BITL_PMD_PHYIDH_PHYID                                                       (16u)
#define BITM_PMD_PHYIDH_PHYID                                               (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : phyidl
// Register Addr        : 0xD203u
// Register NW Addr     : 0xB7E3u
// Register Reset Value : 0x00000000u
// Register MMS         : 0x4u

#define PMD_PHYIDL_ADDR                                                         (0xD203u)
#define PMD_PHYIDL_NW_ADDR                                                      (0xB7E3u)
#define PMD_PHYIDL_RESET_VAL                                                (0x00000000u)
#define PMD_PHYIDL_MMS                                                             (0x4u)
#define PMD_PHYIDL_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | phyid                    | Readback of PHY identifier        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: phyid
// Bit position, length and mask
#define BITP_PMD_PHYIDL_PHYID                                                        (0u)
#define BITL_PMD_PHYIDL_PHYID                                                       (16u)
#define BITM_PMD_PHYIDL_PHYID                                               (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : pmdctl
// Register Addr        : 0xD210u
// Register NW Addr     : 0xB7F0u
// Register Reset Value : 0x00000000u
// Register MMS         : 0x4u

#define PMD_PMDCTL_ADDR                                                         (0xD210u)
#define PMD_PMDCTL_NW_ADDR                                                      (0xB7F0u)
#define PMD_PMDCTL_RESET_VAL                                                (0x00000000u)
#define PMD_PMDCTL_MMS                                                             (0x4u)
#define PMD_PMDCTL_VOL_MASK                                                 (0x00000003u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | tpen                     | Control to enable Topology        | 0x00000000  |
// |       |                          | Discovery mode                    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | tprefn                   | Control to configure Topology     | 0x00000000  |
// |       |                          | Discovery mode as a reference     |             |
// |       |                          | node (when 1) or a measured node  |             |
// |       |                          | (when 0)                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | hdcap                    | Readback of the Half-Duplex       | 0x00000000  |
// |       |                          | capability                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | fdcap                    | Readback of the Full-Duplex       | 0x00000000  |
// |       |                          | capability                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tpen
// Bit position, length and mask
#define BITP_PMD_PMDCTL_TPEN                                                         (0u)
#define BITL_PMD_PMDCTL_TPEN                                                         (1u)
#define BITM_PMD_PMDCTL_TPEN                                                (0x00000001u)
// Enums
#define ENUM_PMD_PMDCTL_TPEN_DISABLED                                       (0x00000000u)
#define ENUM_PMD_PMDCTL_TPEN_ENABLED                                        (0x00000001u)

// Bit field: tprefn
// Bit position, length and mask
#define BITP_PMD_PMDCTL_TPREFN                                                       (1u)
#define BITL_PMD_PMDCTL_TPREFN                                                       (1u)
#define BITM_PMD_PMDCTL_TPREFN                                              (0x00000002u)
// Enums
#define ENUM_PMD_PMDCTL_TPREFN_ENUM000                                      (0x00000000u)
#define ENUM_PMD_PMDCTL_TPREFN_ENUM001                                      (0x00000001u)

// Bit field: hdcap
// Bit position, length and mask
#define BITP_PMD_PMDCTL_HDCAP                                                       (14u)
#define BITL_PMD_PMDCTL_HDCAP                                                        (1u)
#define BITM_PMD_PMDCTL_HDCAP                                               (0x00004000u)

// Bit field: fdcap
// Bit position, length and mask
#define BITP_PMD_PMDCTL_FDCAP                                                       (15u)
#define BITL_PMD_PMDCTL_FDCAP                                                        (1u)
#define BITM_PMD_PMDCTL_FDCAP                                               (0x00008000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : vs_ctrl
// Register Addr        : 0xD211u
// Register NW Addr     : 0xB7F1u
// Register Reset Value : 0x00000000u
// Register MMS         : 0x4u

#define PMD_VS_CTRL_ADDR                                                        (0xD211u)
#define PMD_VS_CTRL_NW_ADDR                                                     (0xB7F1u)
#define PMD_VS_CTRL_RESET_VAL                                               (0x00000000u)
#define PMD_VS_CTRL_MMS                                                            (0x4u)
#define PMD_VS_CTRL_VOL_MASK                                                (0x00008000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15    | regmap_clear             | Control to reset all RW bits in   | 0x00000000  |
// |       |                          | the PMD register map (for         |             |
// |       |                          | initialization purposes) without  |             |
// |       |                          | affecting the PMD state machine.  |             |
// |       |                          | Effect is immediate.              |             |
// |       |                          | Self-clearing bit                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: regmap_clear
// Bit position, length and mask
#define BITP_PMD_VS_CTRL_REGMAP_CLEAR                                               (15u)
#define BITL_PMD_VS_CTRL_REGMAP_CLEAR                                                (1u)
#define BITM_PMD_VS_CTRL_REGMAP_CLEAR                                       (0x00008000u)
// Enums
#define ENUM_PMD_VS_CTRL_REGMAP_CLEAR_INACTIVE                              (0x00000000u)
#define ENUM_PMD_VS_CTRL_REGMAP_CLEAR_TRIGGER                               (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillPMDRegsData();
#endif


#endif /* ADI_PMD_REG_H */

/** @} */

/*
 * EOF: www.analog.com
 */


