/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_ao_reg.h
 * @brief        : This is an auto generated header file with register
                   descriptions for Always on
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_AO_REG_H
#define ADI_AO_REG_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define AO_NUM_REGS    (2u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganAORegAddr[AO_NUM_REGS];
extern uint32_t ganAONwRegAddr[AO_NUM_REGS];
extern uint32_t ganAORegMMS[AO_NUM_REGS];
extern uint32_t ganAORegResetVal[AO_NUM_REGS];
extern const char *gasAORegName[AO_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : configuration_fields_0
// Register Addr        : 0xB702u
// Register NW Addr     : 0xB702u
// Register Reset Value : 0x00000023u
// Register MMS         : 0xAu

#define AO_CONFIGURATION_FIELDS_0_ADDR                                          (0xB702u)
#define AO_CONFIGURATION_FIELDS_0_NW_ADDR                                       (0xB702u)
#define AO_CONFIGURATION_FIELDS_0_RESET_VAL                                 (0x00000023u)
#define AO_CONFIGURATION_FIELDS_0_MMS                                              (0xAu)
#define AO_CONFIGURATION_FIELDS_0_VOL_MASK                                  (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | cfg_duv_threshold        | Deep under voltage threshold      | 0x00000003  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 05-03 | cfg_uv_threshold         | Under voltage threshold           | 0x00000004  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | cfg_wake_pin_pol         | Polarity of the wake pin.         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_duv_threshold
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD                             (0u)
#define BITL_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD                             (3u)
#define BITM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD                    (0x00000007u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM000            (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM001            (0x00000001u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM002            (0x00000002u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM003            (0x00000003u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM004            (0x00000004u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM005            (0x00000005u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM006            (0x00000006u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_DUV_THRESHOLD_ENUM007            (0x00000007u)

// Bit field: cfg_uv_threshold
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD                              (3u)
#define BITL_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD                              (3u)
#define BITM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD                     (0x00000038u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM000             (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM001             (0x00000001u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM002             (0x00000002u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM003             (0x00000003u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM004             (0x00000004u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM005             (0x00000005u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM006             (0x00000006u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_UV_THRESHOLD_ENUM007             (0x00000007u)

// Bit field: cfg_wake_pin_pol
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_0_CFG_WAKE_PIN_POL                              (6u)
#define BITL_AO_CONFIGURATION_FIELDS_0_CFG_WAKE_PIN_POL                              (1u)
#define BITM_AO_CONFIGURATION_FIELDS_0_CFG_WAKE_PIN_POL                     (0x00000040u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_WAKE_PIN_POL_ENUM000             (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_0_CFG_WAKE_PIN_POL_ENUM001             (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : configuration_fields_1
// Register Addr        : 0xB703u
// Register NW Addr     : 0xB703u
// Register Reset Value : 0x00000004u
// Register MMS         : 0xAu

#define AO_CONFIGURATION_FIELDS_1_ADDR                                          (0xB703u)
#define AO_CONFIGURATION_FIELDS_1_NW_ADDR                                       (0xB703u)
#define AO_CONFIGURATION_FIELDS_1_RESET_VAL                                 (0x00000004u)
#define AO_CONFIGURATION_FIELDS_1_MMS                                              (0xAu)
#define AO_CONFIGURATION_FIELDS_1_VOL_MASK                                  (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | cfg_ov_threshold         | Over voltage threshold            | 0x00000004  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | cfg_vbat_deep_uv_flag_en | Enable Vbat monitor for deep      | 0x00000000  |
// |       |                          | undervoltage                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | cfg_vbat_uv_flag_en      | Enable Vbat monitor for long      | 0x00000000  |
// |       |                          | term undervoltage                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | cfg_vbat_ov_flag_en      | Enable Vbat monitor for           | 0x00000000  |
// |       |                          | overvoltage                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | cfg_valid                | Validate all configuration (cfg)  | 0x00000000  |
// |       |                          | controls                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: cfg_ov_threshold
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD                              (0u)
#define BITL_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD                              (3u)
#define BITM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD                     (0x00000007u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM000             (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM001             (0x00000001u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM002             (0x00000002u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM003             (0x00000003u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM004             (0x00000004u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM005             (0x00000005u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM006             (0x00000006u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_OV_THRESHOLD_ENUM007             (0x00000007u)

// Bit field: cfg_vbat_deep_uv_flag_en
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_DEEP_UV_FLAG_EN                      (4u)
#define BITL_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_DEEP_UV_FLAG_EN                      (1u)
#define BITM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_DEEP_UV_FLAG_EN             (0x00000010u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_DEEP_UV_FLAG_EN_DISABLE     (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_DEEP_UV_FLAG_EN_ENABLE      (0x00000001u)

// Bit field: cfg_vbat_uv_flag_en
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_UV_FLAG_EN                           (5u)
#define BITL_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_UV_FLAG_EN                           (1u)
#define BITM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_UV_FLAG_EN                  (0x00000020u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_UV_FLAG_EN_DISABLE          (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_UV_FLAG_EN_ENABLE           (0x00000001u)

// Bit field: cfg_vbat_ov_flag_en
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_OV_FLAG_EN                           (6u)
#define BITL_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_OV_FLAG_EN                           (1u)
#define BITM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_OV_FLAG_EN                  (0x00000040u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_OV_FLAG_EN_DISABLE          (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VBAT_OV_FLAG_EN_ENABLE           (0x00000001u)

// Bit field: cfg_valid
// Bit position, length and mask
#define BITP_AO_CONFIGURATION_FIELDS_1_CFG_VALID                                     (7u)
#define BITL_AO_CONFIGURATION_FIELDS_1_CFG_VALID                                     (1u)
#define BITM_AO_CONFIGURATION_FIELDS_1_CFG_VALID                            (0x00000080u)
// Enums
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VALID_DISABLE                    (0x00000000u)
#define ENUM_AO_CONFIGURATION_FIELDS_1_CFG_VALID_ENABLE                     (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillAORegsData();
#endif


#endif /* ADI_AO_REG_H */

/** @} */

/*
 * EOF: www.analog.com
 */


