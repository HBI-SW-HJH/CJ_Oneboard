/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_regport.h
 * @brief        : This is an auto generated header file with register
                   descriptions for GPIO PORT interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGPORT_H
#define ADI_E2BCORE_REGPORT_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define PORT_NUM_REGS    (26u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganPORTRegAddr[PORT_NUM_REGS];
extern uint32_t ganPORTNwRegAddr[PORT_NUM_REGS];
extern uint32_t ganPORTRegMMS[PORT_NUM_REGS];
extern uint32_t ganPORTRegResetVal[PORT_NUM_REGS];
extern const char *gasPORTRegName[PORT_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_0_control
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_0_CONTROL_ADDR                                                (0x0000u)
#define PORT_UNIT_0_CONTROL_NW_ADDR                                             (0x0000u)
#define PORT_UNIT_0_CONTROL_RESET_VAL                                       (0x00000000u)
#define PORT_UNIT_0_CONTROL_MMS                                                      (0u)
#define PORT_UNIT_0_CONTROL_VOL_MASK                                        (0x3FFFFFF9u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | active_unit_0            | Enables the Pwm_unit 0            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10-03 | complementary_dead_time_u| Dead Time for the Complementary   | 0x00000000  | 
// |       |                          | Mode on Unit 0                    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | steady_state_unit_0_out_0| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | steady_state_unit_0_out_1| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | steady_state_unit_0_out_2| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | steady_state_unit_0_out_3| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16-15 | combined_output_unit_0_ou| Combined Output Mode for Unit 0   | 0x00000000  | 
// |       |                          | Output 0                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18-17 | combined_output_unit_0_ou| Combined Output Mode for Unit 0   | 0x00000000  | 
// |       |                          | Output 1                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-19 | combined_output_unit_0_ou| Combined Output Mode for Unit 0   | 0x00000000  | 
// |       |                          | Output 2                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22-21 | combined_output_unit_0_ou| Combined Output Mode for Unit 0   | 0x00000000  | 
// |       |                          | Output 3                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | polarity_unit_0          | Output Polarity for Unit 0        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24    | num_pulses_enable_unit_0_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | num_pulses_enable_unit_0_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 26    | num_pulses_enable_unit_0_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27    | num_pulses_enable_unit_0_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28    | enable_mode_unit_0       | Selects Between Instantaneous     | 0x00000000  |
// |       |                          | Enable or No Spurs Enable.        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29    | disable_mode_unit_0      | Selects Between Instantaneous     | 0x00000000  |
// |       |                          | Disable or No Spurs Disable.      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: active_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_ACTIVE_UNIT_0                                       (0u)
#define BITL_PORT_UNIT_0_CONTROL_ACTIVE_UNIT_0                                       (1u)
#define BITM_PORT_UNIT_0_CONTROL_ACTIVE_UNIT_0                              (0x00000001u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_ACTIVE_UNIT_0_UNIT_DISABLED                (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_ACTIVE_UNIT_0_UNIT_ENABLED                 (0x00000001u)

// Bit field: complementary_dead_time_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_0                      (3u)
#define BITL_PORT_UNIT_0_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_0                      (8u)
#define BITM_PORT_UNIT_0_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_0             (0x000007F8u)

// Bit field: steady_state_unit_0_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_0                          (11u)
#define BITL_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_0                           (1u)
#define BITM_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_0                  (0x00000800u)

// Bit field: steady_state_unit_0_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_1                          (12u)
#define BITL_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_1                           (1u)
#define BITM_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_1                  (0x00001000u)

// Bit field: steady_state_unit_0_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_2                          (13u)
#define BITL_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_2                           (1u)
#define BITM_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_2                  (0x00002000u)

// Bit field: steady_state_unit_0_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_3                          (14u)
#define BITL_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_3                           (1u)
#define BITM_PORT_UNIT_0_CONTROL_STEADY_STATE_UNIT_0_OUT_3                  (0x00004000u)

// Bit field: combined_output_unit_0_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_0                       (15u)
#define BITL_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_0                        (2u)
#define BITM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_0               (0x00018000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_0_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_0_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_0_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_0_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_0_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_1                       (17u)
#define BITL_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_1                        (2u)
#define BITM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_1               (0x00060000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_1_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_1_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_1_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_1_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_0_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_2                       (19u)
#define BITL_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_2                        (2u)
#define BITM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_2               (0x00180000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_2_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_2_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_2_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_2_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_0_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_3                       (21u)
#define BITL_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_3                        (2u)
#define BITM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_3               (0x00600000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_3_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_3_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_3_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROL_COMBINED_OUTPUT_UNIT_0_OUT_3_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: polarity_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_POLARITY_UNIT_0                                    (23u)
#define BITL_PORT_UNIT_0_CONTROL_POLARITY_UNIT_0                                     (1u)
#define BITM_PORT_UNIT_0_CONTROL_POLARITY_UNIT_0                            (0x00800000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_POLARITY_UNIT_0_PCU_POLARITY_HIGH_LOW      (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_POLARITY_UNIT_0_PCU_POLARITY_LOW_HIGH      (0x00000001u)

// Bit field: num_pulses_enable_unit_0_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_0                     (24u)
#define BITL_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_0                      (1u)
#define BITM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_0             (0x01000000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_0_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_0_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_0_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_1                     (25u)
#define BITL_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_1                      (1u)
#define BITM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_1             (0x02000000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_1_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_1_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_0_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_2                     (26u)
#define BITL_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_2                      (1u)
#define BITM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_2             (0x04000000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_2_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_2_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_0_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_3                     (27u)
#define BITL_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_3                      (1u)
#define BITM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_3             (0x08000000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_3_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_NUM_PULSES_ENABLE_UNIT_0_OUT_3_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: enable_mode_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_ENABLE_MODE_UNIT_0                                 (28u)
#define BITL_PORT_UNIT_0_CONTROL_ENABLE_MODE_UNIT_0                                  (1u)
#define BITM_PORT_UNIT_0_CONTROL_ENABLE_MODE_UNIT_0                         (0x10000000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_ENABLE_MODE_UNIT_0_PCU_ENABLE_MODE_INSTANTANEOUS (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_ENABLE_MODE_UNIT_0_PCU_ENABLE_MODE_NO_SPURS (0x00000001u)

// Bit field: disable_mode_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROL_DISABLE_MODE_UNIT_0                                (29u)
#define BITL_PORT_UNIT_0_CONTROL_DISABLE_MODE_UNIT_0                                 (1u)
#define BITM_PORT_UNIT_0_CONTROL_DISABLE_MODE_UNIT_0                        (0x20000000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROL_DISABLE_MODE_UNIT_0_PCU_DISABLE_MODE_INSTANTANEOUS (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROL_DISABLE_MODE_UNIT_0_PCU_DISABLE_MODE_NO_SPURS (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_0_num_pulses
// Register Addr        : 0x0004u
// Register NW Addr     : 0x0004u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_0_NUM_PULSES_ADDR                                             (0x0004u)
#define PORT_UNIT_0_NUM_PULSES_NW_ADDR                                          (0x0004u)
#define PORT_UNIT_0_NUM_PULSES_RESET_VAL                                    (0x00000000u)
#define PORT_UNIT_0_NUM_PULSES_MMS                                                   (0u)
#define PORT_UNIT_0_NUM_PULSES_VOL_MASK                                     (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | num_pulses_unit_0_out_0  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | num_pulses_unit_0_out_1  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_pulses_unit_0_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_NUM_PULSES_NUM_PULSES_UNIT_0_OUT_0                          (0u)
#define BITL_PORT_UNIT_0_NUM_PULSES_NUM_PULSES_UNIT_0_OUT_0                         (12u)
#define BITM_PORT_UNIT_0_NUM_PULSES_NUM_PULSES_UNIT_0_OUT_0                 (0x00000FFFu)

// Bit field: num_pulses_unit_0_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_0_NUM_PULSES_NUM_PULSES_UNIT_0_OUT_1                         (12u)
#define BITL_PORT_UNIT_0_NUM_PULSES_NUM_PULSES_UNIT_0_OUT_1                         (12u)
#define BITM_PORT_UNIT_0_NUM_PULSES_NUM_PULSES_UNIT_0_OUT_1                 (0x00FFF000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_0_num_pulses_2
// Register Addr        : 0x0008u
// Register NW Addr     : 0x0008u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_0_NUM_PULSES_2_ADDR                                           (0x0008u)
#define PORT_UNIT_0_NUM_PULSES_2_NW_ADDR                                        (0x0008u)
#define PORT_UNIT_0_NUM_PULSES_2_RESET_VAL                                  (0x00000000u)
#define PORT_UNIT_0_NUM_PULSES_2_MMS                                                 (0u)
#define PORT_UNIT_0_NUM_PULSES_2_VOL_MASK                                   (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | num_pulses_unit_0_out_2  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | num_pulses_unit_0_out_3  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_pulses_unit_0_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_0_NUM_PULSES_2_NUM_PULSES_UNIT_0_OUT_2                        (0u)
#define BITL_PORT_UNIT_0_NUM_PULSES_2_NUM_PULSES_UNIT_0_OUT_2                       (12u)
#define BITM_PORT_UNIT_0_NUM_PULSES_2_NUM_PULSES_UNIT_0_OUT_2               (0x00000FFFu)

// Bit field: num_pulses_unit_0_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_0_NUM_PULSES_2_NUM_PULSES_UNIT_0_OUT_3                       (12u)
#define BITL_PORT_UNIT_0_NUM_PULSES_2_NUM_PULSES_UNIT_0_OUT_3                       (12u)
#define BITM_PORT_UNIT_0_NUM_PULSES_2_NUM_PULSES_UNIT_0_OUT_3               (0x00FFF000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_1_control
// Register Addr        : 0x000Cu
// Register NW Addr     : 0x000Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_1_CONTROL_ADDR                                                (0x000Cu)
#define PORT_UNIT_1_CONTROL_NW_ADDR                                             (0x000Cu)
#define PORT_UNIT_1_CONTROL_RESET_VAL                                       (0x00000000u)
#define PORT_UNIT_1_CONTROL_MMS                                                      (0u)
#define PORT_UNIT_1_CONTROL_VOL_MASK                                        (0x3FFFFFF9u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | active_unit_1            | Enables the Pwm_unit 1            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10-03 | complementary_dead_time_u| Dead Time for the Complementary   | 0x00000000  | 
// |       |                          | Mode on Unit 1                    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | steady_state_unit_1_out_0| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | steady_state_unit_1_out_1| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | steady_state_unit_1_out_2| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | steady_state_unit_1_out_3| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16-15 | combined_output_unit_1_ou| Combined Output Mode for Unit 1   | 0x00000000  | 
// |       |                          | Output 0                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18-17 | combined_output_unit_1_ou| Combined Output Mode for Unit 1   | 0x00000000  | 
// |       |                          | Output 1                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-19 | combined_output_unit_1_ou| Combined Output Mode for Unit 1   | 0x00000000  | 
// |       |                          | Output 2                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22-21 | combined_output_unit_1_ou| Combined Output Mode for Unit 1   | 0x00000000  | 
// |       |                          | Output 3                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | polarity_unit_1          | Output Polarity for Unit 1        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24    | num_pulses_enable_unit_1_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | num_pulses_enable_unit_1_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 26    | num_pulses_enable_unit_1_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27    | num_pulses_enable_unit_1_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28    | enable_mode_unit_1       | Selects Between Instantaneous     | 0x00000000  |
// |       |                          | Enable or No Spurs Enable.        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29    | disable_mode_unit_1      | Selects Between Instantaneous     | 0x00000000  |
// |       |                          | Disable or No Spurs Disable.      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: active_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_ACTIVE_UNIT_1                                       (0u)
#define BITL_PORT_UNIT_1_CONTROL_ACTIVE_UNIT_1                                       (1u)
#define BITM_PORT_UNIT_1_CONTROL_ACTIVE_UNIT_1                              (0x00000001u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_ACTIVE_UNIT_1_UNIT_DISABLED                (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_ACTIVE_UNIT_1_UNIT_ENABLED                 (0x00000001u)

// Bit field: complementary_dead_time_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_1                      (3u)
#define BITL_PORT_UNIT_1_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_1                      (8u)
#define BITM_PORT_UNIT_1_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_1             (0x000007F8u)

// Bit field: steady_state_unit_1_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_0                          (11u)
#define BITL_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_0                           (1u)
#define BITM_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_0                  (0x00000800u)

// Bit field: steady_state_unit_1_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_1                          (12u)
#define BITL_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_1                           (1u)
#define BITM_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_1                  (0x00001000u)

// Bit field: steady_state_unit_1_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_2                          (13u)
#define BITL_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_2                           (1u)
#define BITM_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_2                  (0x00002000u)

// Bit field: steady_state_unit_1_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_3                          (14u)
#define BITL_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_3                           (1u)
#define BITM_PORT_UNIT_1_CONTROL_STEADY_STATE_UNIT_1_OUT_3                  (0x00004000u)

// Bit field: combined_output_unit_1_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_0                       (15u)
#define BITL_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_0                        (2u)
#define BITM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_0               (0x00018000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_0_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_0_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_0_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_0_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_1_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_1                       (17u)
#define BITL_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_1                        (2u)
#define BITM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_1               (0x00060000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_1_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_1_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_1_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_1_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_1_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_2                       (19u)
#define BITL_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_2                        (2u)
#define BITM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_2               (0x00180000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_2_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_2_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_2_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_2_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_1_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_3                       (21u)
#define BITL_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_3                        (2u)
#define BITM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_3               (0x00600000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_3_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_3_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_3_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROL_COMBINED_OUTPUT_UNIT_1_OUT_3_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: polarity_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_POLARITY_UNIT_1                                    (23u)
#define BITL_PORT_UNIT_1_CONTROL_POLARITY_UNIT_1                                     (1u)
#define BITM_PORT_UNIT_1_CONTROL_POLARITY_UNIT_1                            (0x00800000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_POLARITY_UNIT_1_PCU_POLARITY_HIGH_LOW      (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_POLARITY_UNIT_1_PCU_POLARITY_LOW_HIGH      (0x00000001u)

// Bit field: num_pulses_enable_unit_1_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_0                     (24u)
#define BITL_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_0                      (1u)
#define BITM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_0             (0x01000000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_0_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_0_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_1_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_1                     (25u)
#define BITL_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_1                      (1u)
#define BITM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_1             (0x02000000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_1_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_1_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_1_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_2                     (26u)
#define BITL_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_2                      (1u)
#define BITM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_2             (0x04000000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_2_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_2_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_1_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_3                     (27u)
#define BITL_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_3                      (1u)
#define BITM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_3             (0x08000000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_3_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_NUM_PULSES_ENABLE_UNIT_1_OUT_3_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: enable_mode_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_ENABLE_MODE_UNIT_1                                 (28u)
#define BITL_PORT_UNIT_1_CONTROL_ENABLE_MODE_UNIT_1                                  (1u)
#define BITM_PORT_UNIT_1_CONTROL_ENABLE_MODE_UNIT_1                         (0x10000000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_ENABLE_MODE_UNIT_1_PCU_ENABLE_MODE_INSTANTANEOUS (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_ENABLE_MODE_UNIT_1_PCU_ENABLE_MODE_NO_SPURS (0x00000001u)

// Bit field: disable_mode_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROL_DISABLE_MODE_UNIT_1                                (29u)
#define BITL_PORT_UNIT_1_CONTROL_DISABLE_MODE_UNIT_1                                 (1u)
#define BITM_PORT_UNIT_1_CONTROL_DISABLE_MODE_UNIT_1                        (0x20000000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROL_DISABLE_MODE_UNIT_1_PCU_DISABLE_MODE_INSTANTANEOUS (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROL_DISABLE_MODE_UNIT_1_PCU_DISABLE_MODE_NO_SPURS (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_1_num_pulses
// Register Addr        : 0x0010u
// Register NW Addr     : 0x0010u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_1_NUM_PULSES_ADDR                                             (0x0010u)
#define PORT_UNIT_1_NUM_PULSES_NW_ADDR                                          (0x0010u)
#define PORT_UNIT_1_NUM_PULSES_RESET_VAL                                    (0x00000000u)
#define PORT_UNIT_1_NUM_PULSES_MMS                                                   (0u)
#define PORT_UNIT_1_NUM_PULSES_VOL_MASK                                     (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | num_pulses_unit_1_out_0  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | num_pulses_unit_1_out_1  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_pulses_unit_1_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_1_NUM_PULSES_NUM_PULSES_UNIT_1_OUT_0                          (0u)
#define BITL_PORT_UNIT_1_NUM_PULSES_NUM_PULSES_UNIT_1_OUT_0                         (12u)
#define BITM_PORT_UNIT_1_NUM_PULSES_NUM_PULSES_UNIT_1_OUT_0                 (0x00000FFFu)

// Bit field: num_pulses_unit_1_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_NUM_PULSES_NUM_PULSES_UNIT_1_OUT_1                         (12u)
#define BITL_PORT_UNIT_1_NUM_PULSES_NUM_PULSES_UNIT_1_OUT_1                         (12u)
#define BITM_PORT_UNIT_1_NUM_PULSES_NUM_PULSES_UNIT_1_OUT_1                 (0x00FFF000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_1_num_pulses_2
// Register Addr        : 0x0014u
// Register NW Addr     : 0x0014u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_1_NUM_PULSES_2_ADDR                                           (0x0014u)
#define PORT_UNIT_1_NUM_PULSES_2_NW_ADDR                                        (0x0014u)
#define PORT_UNIT_1_NUM_PULSES_2_RESET_VAL                                  (0x00000000u)
#define PORT_UNIT_1_NUM_PULSES_2_MMS                                                 (0u)
#define PORT_UNIT_1_NUM_PULSES_2_VOL_MASK                                   (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | num_pulses_unit_1_out_2  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | num_pulses_unit_1_out_3  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_pulses_unit_1_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_1_NUM_PULSES_2_NUM_PULSES_UNIT_1_OUT_2                        (0u)
#define BITL_PORT_UNIT_1_NUM_PULSES_2_NUM_PULSES_UNIT_1_OUT_2                       (12u)
#define BITM_PORT_UNIT_1_NUM_PULSES_2_NUM_PULSES_UNIT_1_OUT_2               (0x00000FFFu)

// Bit field: num_pulses_unit_1_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_1_NUM_PULSES_2_NUM_PULSES_UNIT_1_OUT_3                       (12u)
#define BITL_PORT_UNIT_1_NUM_PULSES_2_NUM_PULSES_UNIT_1_OUT_3                       (12u)
#define BITM_PORT_UNIT_1_NUM_PULSES_2_NUM_PULSES_UNIT_1_OUT_3               (0x00FFF000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_2_control
// Register Addr        : 0x0018u
// Register NW Addr     : 0x0018u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_2_CONTROL_ADDR                                                (0x0018u)
#define PORT_UNIT_2_CONTROL_NW_ADDR                                             (0x0018u)
#define PORT_UNIT_2_CONTROL_RESET_VAL                                       (0x00000000u)
#define PORT_UNIT_2_CONTROL_MMS                                                      (0u)
#define PORT_UNIT_2_CONTROL_VOL_MASK                                        (0x3FFFFFF9u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | active_unit_2            | Enables the Pwm_unit 2            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10-03 | complementary_dead_time_u| Dead Time for the Complementary   | 0x00000000  | 
// |       |                          | Mode on Unit 2                    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | steady_state_unit_2_out_0| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | steady_state_unit_2_out_1| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | steady_state_unit_2_out_2| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | steady_state_unit_2_out_3| Output Value for PWM_OUT not      | 0x00000000  | 
// |       |                          | Enabled                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16-15 | combined_output_unit_2_ou| Combined Output Mode for Unit 2   | 0x00000000  | 
// |       |                          | Output 0                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18-17 | combined_output_unit_2_ou| Combined Output Mode for Unit 2   | 0x00000000  | 
// |       |                          | Output 1                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-19 | combined_output_unit_2_ou| Combined Output Mode for Unit 2   | 0x00000000  | 
// |       |                          | Output 2                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22-21 | combined_output_unit_2_ou| Combined Output Mode for Unit 2   | 0x00000000  | 
// |       |                          | Output 3                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | polarity_unit_2          | Output Polarity for Unit 2        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24    | num_pulses_enable_unit_2_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | num_pulses_enable_unit_2_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 26    | num_pulses_enable_unit_2_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27    | num_pulses_enable_unit_2_| Selects for Each Output if Runs   | 0x00000000  | 
// |       |                          | in INFINITE MODE or NUM_PULSES    |             |
// |       |                          | MODE.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28    | enable_mode_unit_2       | Selects Between Instantaneous     | 0x00000000  |
// |       |                          | Enable or No Spurs Enable.        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29    | disable_mode_unit_2      | Selects Between Instantaneous     | 0x00000000  |
// |       |                          | Disable or No Spurs Disable.      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: active_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_ACTIVE_UNIT_2                                       (0u)
#define BITL_PORT_UNIT_2_CONTROL_ACTIVE_UNIT_2                                       (1u)
#define BITM_PORT_UNIT_2_CONTROL_ACTIVE_UNIT_2                              (0x00000001u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_ACTIVE_UNIT_2_UNIT_DISABLED                (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_ACTIVE_UNIT_2_UNIT_ENABLED                 (0x00000001u)

// Bit field: complementary_dead_time_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_2                      (3u)
#define BITL_PORT_UNIT_2_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_2                      (8u)
#define BITM_PORT_UNIT_2_CONTROL_COMPLEMENTARY_DEAD_TIME_UNIT_2             (0x000007F8u)

// Bit field: steady_state_unit_2_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_0                          (11u)
#define BITL_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_0                           (1u)
#define BITM_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_0                  (0x00000800u)

// Bit field: steady_state_unit_2_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_1                          (12u)
#define BITL_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_1                           (1u)
#define BITM_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_1                  (0x00001000u)

// Bit field: steady_state_unit_2_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_2                          (13u)
#define BITL_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_2                           (1u)
#define BITM_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_2                  (0x00002000u)

// Bit field: steady_state_unit_2_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_3                          (14u)
#define BITL_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_3                           (1u)
#define BITM_PORT_UNIT_2_CONTROL_STEADY_STATE_UNIT_2_OUT_3                  (0x00004000u)

// Bit field: combined_output_unit_2_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_0                       (15u)
#define BITL_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_0                        (2u)
#define BITM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_0               (0x00018000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_0_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_0_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_0_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_0_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_2_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_1                       (17u)
#define BITL_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_1                        (2u)
#define BITM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_1               (0x00060000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_1_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_1_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_1_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_1_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_2_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_2                       (19u)
#define BITL_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_2                        (2u)
#define BITM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_2               (0x00180000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_2_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_2_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_2_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_2_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: combined_output_unit_2_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_3                       (21u)
#define BITL_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_3                        (2u)
#define BITM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_3               (0x00600000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_3_PCU_COMBINED_OUTPUT_NONE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_3_PCU_COMBINED_OUTPUT_AND (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_3_PCU_COMBINED_OUTPUT_OR (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROL_COMBINED_OUTPUT_UNIT_2_OUT_3_PCU_COMBINED_OUTPUT_XOR (0x00000003u)

// Bit field: polarity_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_POLARITY_UNIT_2                                    (23u)
#define BITL_PORT_UNIT_2_CONTROL_POLARITY_UNIT_2                                     (1u)
#define BITM_PORT_UNIT_2_CONTROL_POLARITY_UNIT_2                            (0x00800000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_POLARITY_UNIT_2_PCU_POLARITY_HIGH_LOW      (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_POLARITY_UNIT_2_PCU_POLARITY_LOW_HIGH      (0x00000001u)

// Bit field: num_pulses_enable_unit_2_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_0                     (24u)
#define BITL_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_0                      (1u)
#define BITM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_0             (0x01000000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_0_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_0_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_2_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_1                     (25u)
#define BITL_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_1                      (1u)
#define BITM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_1             (0x02000000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_1_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_1_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_2_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_2                     (26u)
#define BITL_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_2                      (1u)
#define BITM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_2             (0x04000000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_2_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_2_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: num_pulses_enable_unit_2_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_3                     (27u)
#define BITL_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_3                      (1u)
#define BITM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_3             (0x08000000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_3_PCU_MODE_INFINITE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_NUM_PULSES_ENABLE_UNIT_2_OUT_3_PCU_MODE_NUM_PULSES (0x00000001u)

// Bit field: enable_mode_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_ENABLE_MODE_UNIT_2                                 (28u)
#define BITL_PORT_UNIT_2_CONTROL_ENABLE_MODE_UNIT_2                                  (1u)
#define BITM_PORT_UNIT_2_CONTROL_ENABLE_MODE_UNIT_2                         (0x10000000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_ENABLE_MODE_UNIT_2_PCU_ENABLE_MODE_INSTANTANEOUS (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_ENABLE_MODE_UNIT_2_PCU_ENABLE_MODE_NO_SPURS (0x00000001u)

// Bit field: disable_mode_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROL_DISABLE_MODE_UNIT_2                                (29u)
#define BITL_PORT_UNIT_2_CONTROL_DISABLE_MODE_UNIT_2                                 (1u)
#define BITM_PORT_UNIT_2_CONTROL_DISABLE_MODE_UNIT_2                        (0x20000000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROL_DISABLE_MODE_UNIT_2_PCU_DISABLE_MODE_INSTANTANEOUS (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROL_DISABLE_MODE_UNIT_2_PCU_DISABLE_MODE_NO_SPURS (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_2_num_pulses
// Register Addr        : 0x001Cu
// Register NW Addr     : 0x001Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_2_NUM_PULSES_ADDR                                             (0x001Cu)
#define PORT_UNIT_2_NUM_PULSES_NW_ADDR                                          (0x001Cu)
#define PORT_UNIT_2_NUM_PULSES_RESET_VAL                                    (0x00000000u)
#define PORT_UNIT_2_NUM_PULSES_MMS                                                   (0u)
#define PORT_UNIT_2_NUM_PULSES_VOL_MASK                                     (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | num_pulses_unit_2_out_0  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | num_pulses_unit_2_out_1  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_pulses_unit_2_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_2_NUM_PULSES_NUM_PULSES_UNIT_2_OUT_0                          (0u)
#define BITL_PORT_UNIT_2_NUM_PULSES_NUM_PULSES_UNIT_2_OUT_0                         (12u)
#define BITM_PORT_UNIT_2_NUM_PULSES_NUM_PULSES_UNIT_2_OUT_0                 (0x00000FFFu)

// Bit field: num_pulses_unit_2_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_2_NUM_PULSES_NUM_PULSES_UNIT_2_OUT_1                         (12u)
#define BITL_PORT_UNIT_2_NUM_PULSES_NUM_PULSES_UNIT_2_OUT_1                         (12u)
#define BITM_PORT_UNIT_2_NUM_PULSES_NUM_PULSES_UNIT_2_OUT_1                 (0x00FFF000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_2_num_pulses_2
// Register Addr        : 0x0020u
// Register NW Addr     : 0x0020u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_2_NUM_PULSES_2_ADDR                                           (0x0020u)
#define PORT_UNIT_2_NUM_PULSES_2_NW_ADDR                                        (0x0020u)
#define PORT_UNIT_2_NUM_PULSES_2_RESET_VAL                                  (0x00000000u)
#define PORT_UNIT_2_NUM_PULSES_2_MMS                                                 (0u)
#define PORT_UNIT_2_NUM_PULSES_2_VOL_MASK                                   (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | num_pulses_unit_2_out_2  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | num_pulses_unit_2_out_3  | Number of PWM Pulses to Generate  | 0x00000000  |
// |       |                          | in NUM_PULSES Mode                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: num_pulses_unit_2_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_NUM_PULSES_2_NUM_PULSES_UNIT_2_OUT_2                        (0u)
#define BITL_PORT_UNIT_2_NUM_PULSES_2_NUM_PULSES_UNIT_2_OUT_2                       (12u)
#define BITM_PORT_UNIT_2_NUM_PULSES_2_NUM_PULSES_UNIT_2_OUT_2               (0x00000FFFu)

// Bit field: num_pulses_unit_2_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_2_NUM_PULSES_2_NUM_PULSES_UNIT_2_OUT_3                       (12u)
#define BITL_PORT_UNIT_2_NUM_PULSES_2_NUM_PULSES_UNIT_2_OUT_3                       (12u)
#define BITM_PORT_UNIT_2_NUM_PULSES_2_NUM_PULSES_UNIT_2_OUT_3               (0x00FFF000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : output_mux
// Register Addr        : 0x0024u
// Register NW Addr     : 0x0024u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_OUTPUT_MUX_ADDR                                                    (0x0024u)
#define PORT_OUTPUT_MUX_NW_ADDR                                                 (0x0024u)
#define PORT_OUTPUT_MUX_RESET_VAL                                           (0x00000000u)
#define PORT_OUTPUT_MUX_MMS                                                          (0u)
#define PORT_OUTPUT_MUX_VOL_MASK                                            (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | port0_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | port1_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | port2_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | port3_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | port4_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | port5_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | port6_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | port7_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | port8_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | port9_port_output_enable | Port Controller Output Enable     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | port10_port_output_enable| Port Controller Output Enable     | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | port11_port_output_enable| Port Controller Output Enable     | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | port0_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | port1_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | port2_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | port3_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | port4_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | port5_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | port6_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | port7_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | port8_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | port9_port_override      | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22    | port10_port_override     | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | port11_port_override     | Enable the Override of the        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: port0_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT0_PORT_OUTPUT_ENABLE                                (0u)
#define BITL_PORT_OUTPUT_MUX_PORT0_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT0_PORT_OUTPUT_ENABLE                       (0x00000001u)

// Bit field: port1_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT1_PORT_OUTPUT_ENABLE                                (1u)
#define BITL_PORT_OUTPUT_MUX_PORT1_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT1_PORT_OUTPUT_ENABLE                       (0x00000002u)

// Bit field: port2_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT2_PORT_OUTPUT_ENABLE                                (2u)
#define BITL_PORT_OUTPUT_MUX_PORT2_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT2_PORT_OUTPUT_ENABLE                       (0x00000004u)

// Bit field: port3_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT3_PORT_OUTPUT_ENABLE                                (3u)
#define BITL_PORT_OUTPUT_MUX_PORT3_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT3_PORT_OUTPUT_ENABLE                       (0x00000008u)

// Bit field: port4_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT4_PORT_OUTPUT_ENABLE                                (4u)
#define BITL_PORT_OUTPUT_MUX_PORT4_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT4_PORT_OUTPUT_ENABLE                       (0x00000010u)

// Bit field: port5_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT5_PORT_OUTPUT_ENABLE                                (5u)
#define BITL_PORT_OUTPUT_MUX_PORT5_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT5_PORT_OUTPUT_ENABLE                       (0x00000020u)

// Bit field: port6_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT6_PORT_OUTPUT_ENABLE                                (6u)
#define BITL_PORT_OUTPUT_MUX_PORT6_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT6_PORT_OUTPUT_ENABLE                       (0x00000040u)

// Bit field: port7_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT7_PORT_OUTPUT_ENABLE                                (7u)
#define BITL_PORT_OUTPUT_MUX_PORT7_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT7_PORT_OUTPUT_ENABLE                       (0x00000080u)

// Bit field: port8_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT8_PORT_OUTPUT_ENABLE                                (8u)
#define BITL_PORT_OUTPUT_MUX_PORT8_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT8_PORT_OUTPUT_ENABLE                       (0x00000100u)

// Bit field: port9_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT9_PORT_OUTPUT_ENABLE                                (9u)
#define BITL_PORT_OUTPUT_MUX_PORT9_PORT_OUTPUT_ENABLE                                (1u)
#define BITM_PORT_OUTPUT_MUX_PORT9_PORT_OUTPUT_ENABLE                       (0x00000200u)

// Bit field: port10_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT10_PORT_OUTPUT_ENABLE                              (10u)
#define BITL_PORT_OUTPUT_MUX_PORT10_PORT_OUTPUT_ENABLE                               (1u)
#define BITM_PORT_OUTPUT_MUX_PORT10_PORT_OUTPUT_ENABLE                      (0x00000400u)

// Bit field: port11_port_output_enable
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT11_PORT_OUTPUT_ENABLE                              (11u)
#define BITL_PORT_OUTPUT_MUX_PORT11_PORT_OUTPUT_ENABLE                               (1u)
#define BITM_PORT_OUTPUT_MUX_PORT11_PORT_OUTPUT_ENABLE                      (0x00000800u)

// Bit field: port0_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT0_PORT_OVERRIDE                                    (12u)
#define BITL_PORT_OUTPUT_MUX_PORT0_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT0_PORT_OVERRIDE                            (0x00001000u)

// Bit field: port1_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT1_PORT_OVERRIDE                                    (13u)
#define BITL_PORT_OUTPUT_MUX_PORT1_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT1_PORT_OVERRIDE                            (0x00002000u)

// Bit field: port2_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT2_PORT_OVERRIDE                                    (14u)
#define BITL_PORT_OUTPUT_MUX_PORT2_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT2_PORT_OVERRIDE                            (0x00004000u)

// Bit field: port3_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT3_PORT_OVERRIDE                                    (15u)
#define BITL_PORT_OUTPUT_MUX_PORT3_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT3_PORT_OVERRIDE                            (0x00008000u)

// Bit field: port4_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT4_PORT_OVERRIDE                                    (16u)
#define BITL_PORT_OUTPUT_MUX_PORT4_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT4_PORT_OVERRIDE                            (0x00010000u)

// Bit field: port5_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT5_PORT_OVERRIDE                                    (17u)
#define BITL_PORT_OUTPUT_MUX_PORT5_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT5_PORT_OVERRIDE                            (0x00020000u)

// Bit field: port6_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT6_PORT_OVERRIDE                                    (18u)
#define BITL_PORT_OUTPUT_MUX_PORT6_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT6_PORT_OVERRIDE                            (0x00040000u)

// Bit field: port7_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT7_PORT_OVERRIDE                                    (19u)
#define BITL_PORT_OUTPUT_MUX_PORT7_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT7_PORT_OVERRIDE                            (0x00080000u)

// Bit field: port8_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT8_PORT_OVERRIDE                                    (20u)
#define BITL_PORT_OUTPUT_MUX_PORT8_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT8_PORT_OVERRIDE                            (0x00100000u)

// Bit field: port9_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT9_PORT_OVERRIDE                                    (21u)
#define BITL_PORT_OUTPUT_MUX_PORT9_PORT_OVERRIDE                                     (1u)
#define BITM_PORT_OUTPUT_MUX_PORT9_PORT_OVERRIDE                            (0x00200000u)

// Bit field: port10_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT10_PORT_OVERRIDE                                   (22u)
#define BITL_PORT_OUTPUT_MUX_PORT10_PORT_OVERRIDE                                    (1u)
#define BITM_PORT_OUTPUT_MUX_PORT10_PORT_OVERRIDE                           (0x00400000u)

// Bit field: port11_port_override
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX_PORT11_PORT_OVERRIDE                                   (23u)
#define BITL_PORT_OUTPUT_MUX_PORT11_PORT_OVERRIDE                                    (1u)
#define BITM_PORT_OUTPUT_MUX_PORT11_PORT_OVERRIDE                           (0x00800000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : output_mux2
// Register Addr        : 0x0028u
// Register NW Addr     : 0x0028u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_OUTPUT_MUX2_ADDR                                                   (0x0028u)
#define PORT_OUTPUT_MUX2_NW_ADDR                                                (0x0028u)
#define PORT_OUTPUT_MUX2_RESET_VAL                                          (0x00000000u)
#define PORT_OUTPUT_MUX2_MMS                                                         (0u)
#define PORT_OUTPUT_MUX2_VOL_MASK                                           (0x00000FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | port0_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | port1_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | port2_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | port3_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | port4_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | port5_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | port6_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | port7_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | port8_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | port9_port_override_value| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | port10_port_override_valu| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | port11_port_override_valu| Output Override Value.            | 0x00000000  | 
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | port0_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | port1_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | port2_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | port3_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | port4_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | port5_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | port6_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | port7_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20    | port8_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | port9_port_readback      | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22    | port10_port_readback     | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | port11_port_readback     | Readback value of each Output     | 0x00000000  |
// |       |                          | Port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: port0_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT0_PORT_OVERRIDE_VALUE                              (0u)
#define BITL_PORT_OUTPUT_MUX2_PORT0_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT0_PORT_OVERRIDE_VALUE                     (0x00000001u)

// Bit field: port1_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT1_PORT_OVERRIDE_VALUE                              (1u)
#define BITL_PORT_OUTPUT_MUX2_PORT1_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT1_PORT_OVERRIDE_VALUE                     (0x00000002u)

// Bit field: port2_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT2_PORT_OVERRIDE_VALUE                              (2u)
#define BITL_PORT_OUTPUT_MUX2_PORT2_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT2_PORT_OVERRIDE_VALUE                     (0x00000004u)

// Bit field: port3_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT3_PORT_OVERRIDE_VALUE                              (3u)
#define BITL_PORT_OUTPUT_MUX2_PORT3_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT3_PORT_OVERRIDE_VALUE                     (0x00000008u)

// Bit field: port4_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT4_PORT_OVERRIDE_VALUE                              (4u)
#define BITL_PORT_OUTPUT_MUX2_PORT4_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT4_PORT_OVERRIDE_VALUE                     (0x00000010u)

// Bit field: port5_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT5_PORT_OVERRIDE_VALUE                              (5u)
#define BITL_PORT_OUTPUT_MUX2_PORT5_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT5_PORT_OVERRIDE_VALUE                     (0x00000020u)

// Bit field: port6_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT6_PORT_OVERRIDE_VALUE                              (6u)
#define BITL_PORT_OUTPUT_MUX2_PORT6_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT6_PORT_OVERRIDE_VALUE                     (0x00000040u)

// Bit field: port7_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT7_PORT_OVERRIDE_VALUE                              (7u)
#define BITL_PORT_OUTPUT_MUX2_PORT7_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT7_PORT_OVERRIDE_VALUE                     (0x00000080u)

// Bit field: port8_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT8_PORT_OVERRIDE_VALUE                              (8u)
#define BITL_PORT_OUTPUT_MUX2_PORT8_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT8_PORT_OVERRIDE_VALUE                     (0x00000100u)

// Bit field: port9_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT9_PORT_OVERRIDE_VALUE                              (9u)
#define BITL_PORT_OUTPUT_MUX2_PORT9_PORT_OVERRIDE_VALUE                              (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT9_PORT_OVERRIDE_VALUE                     (0x00000200u)

// Bit field: port10_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT10_PORT_OVERRIDE_VALUE                            (10u)
#define BITL_PORT_OUTPUT_MUX2_PORT10_PORT_OVERRIDE_VALUE                             (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT10_PORT_OVERRIDE_VALUE                    (0x00000400u)

// Bit field: port11_port_override_value
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT11_PORT_OVERRIDE_VALUE                            (11u)
#define BITL_PORT_OUTPUT_MUX2_PORT11_PORT_OVERRIDE_VALUE                             (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT11_PORT_OVERRIDE_VALUE                    (0x00000800u)

// Bit field: port0_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT0_PORT_READBACK                                   (12u)
#define BITL_PORT_OUTPUT_MUX2_PORT0_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT0_PORT_READBACK                           (0x00001000u)

// Bit field: port1_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT1_PORT_READBACK                                   (13u)
#define BITL_PORT_OUTPUT_MUX2_PORT1_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT1_PORT_READBACK                           (0x00002000u)

// Bit field: port2_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT2_PORT_READBACK                                   (14u)
#define BITL_PORT_OUTPUT_MUX2_PORT2_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT2_PORT_READBACK                           (0x00004000u)

// Bit field: port3_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT3_PORT_READBACK                                   (15u)
#define BITL_PORT_OUTPUT_MUX2_PORT3_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT3_PORT_READBACK                           (0x00008000u)

// Bit field: port4_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT4_PORT_READBACK                                   (16u)
#define BITL_PORT_OUTPUT_MUX2_PORT4_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT4_PORT_READBACK                           (0x00010000u)

// Bit field: port5_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT5_PORT_READBACK                                   (17u)
#define BITL_PORT_OUTPUT_MUX2_PORT5_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT5_PORT_READBACK                           (0x00020000u)

// Bit field: port6_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT6_PORT_READBACK                                   (18u)
#define BITL_PORT_OUTPUT_MUX2_PORT6_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT6_PORT_READBACK                           (0x00040000u)

// Bit field: port7_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT7_PORT_READBACK                                   (19u)
#define BITL_PORT_OUTPUT_MUX2_PORT7_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT7_PORT_READBACK                           (0x00080000u)

// Bit field: port8_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT8_PORT_READBACK                                   (20u)
#define BITL_PORT_OUTPUT_MUX2_PORT8_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT8_PORT_READBACK                           (0x00100000u)

// Bit field: port9_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT9_PORT_READBACK                                   (21u)
#define BITL_PORT_OUTPUT_MUX2_PORT9_PORT_READBACK                                    (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT9_PORT_READBACK                           (0x00200000u)

// Bit field: port10_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT10_PORT_READBACK                                  (22u)
#define BITL_PORT_OUTPUT_MUX2_PORT10_PORT_READBACK                                   (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT10_PORT_READBACK                          (0x00400000u)

// Bit field: port11_port_readback
// Bit position, length and mask
#define BITP_PORT_OUTPUT_MUX2_PORT11_PORT_READBACK                                  (23u)
#define BITL_PORT_OUTPUT_MUX2_PORT11_PORT_READBACK                                   (1u)
#define BITM_PORT_OUTPUT_MUX2_PORT11_PORT_READBACK                          (0x00800000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : control
// Register Addr        : 0x002Cu
// Register NW Addr     : 0x002Cu
// Register Reset Value : 0x000100C0u
// Register MMS         : 0u

#define PORT_CONTROL_ADDR                                                       (0x002Cu)
#define PORT_CONTROL_NW_ADDR                                                    (0x002Cu)
#define PORT_CONTROL_RESET_VAL                                              (0x000100C0u)
#define PORT_CONTROL_MMS                                                             (0u)
#define PORT_CONTROL_VOL_MASK                                               (0xFFFF03FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | soft_reset               | Reset key. Must write a value of  | 0x00000000  |
// |       |                          | 0xA to this field to cause a      |             |
// |       |                          | soft reset of the interface.      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | rx_fifo_reset            | Resets the hardware interface     | 0x00000000  |
// |       |                          | reception channel storage that    |             |
// |       |                          | is connected to this interface.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | tx_fifo_reset            | Resets the hardware interface     | 0x00000000  |
// |       |                          | transmission channel storage      |             |
// |       |                          | that is connected to this         |             |
// |       |                          | interface.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | transaction_header_enable| Reserved. It shall be always      | 0x00000001  | 
// |       |                          | set.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | enable_upstream_ecco     | If disabled, the interface will   | 0x00000001  |
// |       |                          | not be capable of generating an   |             |
// |       |                          | ecco for each of the processed    |             |
// |       |                          | commands. Regardless of any       |             |
// |       |                          | other condition.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | enable_read_time_tag_ecco| If set, the interface will tag    | 0x00000000  | 
// |       |                          | the interface transaction         |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp on eccos.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | resynchronization_require| Once set, the next executed       | 0x00000000  | 
// |       |                          | transaction shall be a            |             |
// |       |                          | resynchronization transaction     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | num_packed_transactions  | Default configuration used for    | 0x00000001  |
// |       |                          | the optional field                |             |
// |       |                          | "num_packed_transactions" in the  |             |
// |       |                          | interface payload.                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: soft_reset
// Bit position, length and mask
#define BITP_PORT_CONTROL_SOFT_RESET                                                 (0u)
#define BITL_PORT_CONTROL_SOFT_RESET                                                 (4u)
#define BITM_PORT_CONTROL_SOFT_RESET                                        (0x0000000Fu)

// Bit field: rx_fifo_reset
// Bit position, length and mask
#define BITP_PORT_CONTROL_RX_FIFO_RESET                                              (4u)
#define BITL_PORT_CONTROL_RX_FIFO_RESET                                              (1u)
#define BITM_PORT_CONTROL_RX_FIFO_RESET                                     (0x00000010u)

// Bit field: tx_fifo_reset
// Bit position, length and mask
#define BITP_PORT_CONTROL_TX_FIFO_RESET                                              (5u)
#define BITL_PORT_CONTROL_TX_FIFO_RESET                                              (1u)
#define BITM_PORT_CONTROL_TX_FIFO_RESET                                     (0x00000020u)

// Bit field: transaction_header_enable
// Bit position, length and mask
#define BITP_PORT_CONTROL_TRANSACTION_HEADER_ENABLE                                  (6u)
#define BITL_PORT_CONTROL_TRANSACTION_HEADER_ENABLE                                  (1u)
#define BITM_PORT_CONTROL_TRANSACTION_HEADER_ENABLE                         (0x00000040u)
// Enums
#define ENUM_PORT_CONTROL_TRANSACTION_HEADER_ENABLE_RESERVED                (0x00000000u)
#define ENUM_PORT_CONTROL_TRANSACTION_HEADER_ENABLE_ENABLED                 (0x00000001u)

// Bit field: enable_upstream_ecco
// Bit position, length and mask
#define BITP_PORT_CONTROL_ENABLE_UPSTREAM_ECCO                                       (7u)
#define BITL_PORT_CONTROL_ENABLE_UPSTREAM_ECCO                                       (1u)
#define BITM_PORT_CONTROL_ENABLE_UPSTREAM_ECCO                              (0x00000080u)
// Enums
#define ENUM_PORT_CONTROL_ENABLE_UPSTREAM_ECCO_DISABLE                      (0x00000000u)
#define ENUM_PORT_CONTROL_ENABLE_UPSTREAM_ECCO_ENABLE                       (0x00000001u)

// Bit field: enable_read_time_tag_ecco
// Bit position, length and mask
#define BITP_PORT_CONTROL_ENABLE_READ_TIME_TAG_ECCO                                  (8u)
#define BITL_PORT_CONTROL_ENABLE_READ_TIME_TAG_ECCO                                  (1u)
#define BITM_PORT_CONTROL_ENABLE_READ_TIME_TAG_ECCO                         (0x00000100u)
// Enums
#define ENUM_PORT_CONTROL_ENABLE_READ_TIME_TAG_ECCO_DISABLE                 (0x00000000u)
#define ENUM_PORT_CONTROL_ENABLE_READ_TIME_TAG_ECCO_ENABLE                  (0x00000001u)

// Bit field: resynchronization_required
// Bit position, length and mask
#define BITP_PORT_CONTROL_RESYNCHRONIZATION_REQUIRED                                 (9u)
#define BITL_PORT_CONTROL_RESYNCHRONIZATION_REQUIRED                                 (1u)
#define BITM_PORT_CONTROL_RESYNCHRONIZATION_REQUIRED                        (0x00000200u)

// Bit field: num_packed_transactions
// Bit position, length and mask
#define BITP_PORT_CONTROL_NUM_PACKED_TRANSACTIONS                                   (16u)
#define BITL_PORT_CONTROL_NUM_PACKED_TRANSACTIONS                                   (16u)
#define BITM_PORT_CONTROL_NUM_PACKED_TRANSACTIONS                           (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : general_controls_and_readbacks
// Register Addr        : 0x0030u
// Register NW Addr     : 0x0030u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_GENERAL_CONTROLS_AND_READBACKS_ADDR                                (0x0030u)
#define PORT_GENERAL_CONTROLS_AND_READBACKS_NW_ADDR                             (0x0030u)
#define PORT_GENERAL_CONTROLS_AND_READBACKS_RESET_VAL                       (0x00000000u)
#define PORT_GENERAL_CONTROLS_AND_READBACKS_MMS                                      (0u)
#define PORT_GENERAL_CONTROLS_AND_READBACKS_VOL_MASK                        (0xF0000FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | port0_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | port1_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | port2_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | port3_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | port4_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | port5_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | port6_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | port7_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | port8_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | port9_input_monitor_enabl| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | port10_input_monitor_enab| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | port11_input_monitor_enab| Enables the Input Monitor         | 0x00000000  | 
// |       |                          | Functionality for Each Port       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28    | synchronization_mode     | Selects the Synchronization       | 0x00000000  |
// |       |                          | Configuration Between LOCAL_XTAL  |             |
// |       |                          | and GPTP.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | enable_read_time_tag     | If set, the interface will tag    | 0x00000000  |
// |       |                          | the interface transaction         |             |
// |       |                          | generated data with the current   |             |
// |       |                          | gPTP time-stamp                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: port0_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT0_INPUT_MONITOR_ENABLE          (0u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT0_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT0_INPUT_MONITOR_ENABLE (0x00000001u)

// Bit field: port1_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT1_INPUT_MONITOR_ENABLE          (1u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT1_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT1_INPUT_MONITOR_ENABLE (0x00000002u)

// Bit field: port2_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT2_INPUT_MONITOR_ENABLE          (2u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT2_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT2_INPUT_MONITOR_ENABLE (0x00000004u)

// Bit field: port3_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT3_INPUT_MONITOR_ENABLE          (3u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT3_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT3_INPUT_MONITOR_ENABLE (0x00000008u)

// Bit field: port4_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT4_INPUT_MONITOR_ENABLE          (4u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT4_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT4_INPUT_MONITOR_ENABLE (0x00000010u)

// Bit field: port5_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT5_INPUT_MONITOR_ENABLE          (5u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT5_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT5_INPUT_MONITOR_ENABLE (0x00000020u)

// Bit field: port6_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT6_INPUT_MONITOR_ENABLE          (6u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT6_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT6_INPUT_MONITOR_ENABLE (0x00000040u)

// Bit field: port7_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT7_INPUT_MONITOR_ENABLE          (7u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT7_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT7_INPUT_MONITOR_ENABLE (0x00000080u)

// Bit field: port8_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT8_INPUT_MONITOR_ENABLE          (8u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT8_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT8_INPUT_MONITOR_ENABLE (0x00000100u)

// Bit field: port9_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT9_INPUT_MONITOR_ENABLE          (9u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT9_INPUT_MONITOR_ENABLE          (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT9_INPUT_MONITOR_ENABLE (0x00000200u)

// Bit field: port10_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT10_INPUT_MONITOR_ENABLE        (10u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT10_INPUT_MONITOR_ENABLE         (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT10_INPUT_MONITOR_ENABLE (0x00000400u)

// Bit field: port11_input_monitor_enable
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT11_INPUT_MONITOR_ENABLE        (11u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT11_INPUT_MONITOR_ENABLE         (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_PORT11_INPUT_MONITOR_ENABLE (0x00000800u)

// Bit field: synchronization_mode
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_SYNCHRONIZATION_MODE               (28u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_SYNCHRONIZATION_MODE                (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_SYNCHRONIZATION_MODE       (0x10000000u)
// Enums
#define ENUM_PORT_GENERAL_CONTROLS_AND_READBACKS_SYNCHRONIZATION_MODE_GPTP  (0x00000000u)
#define ENUM_PORT_GENERAL_CONTROLS_AND_READBACKS_SYNCHRONIZATION_MODE_LOCAL_XTAL (0x00000001u)

// Bit field: enable_read_time_tag
// Bit position, length and mask
#define BITP_PORT_GENERAL_CONTROLS_AND_READBACKS_ENABLE_READ_TIME_TAG               (31u)
#define BITL_PORT_GENERAL_CONTROLS_AND_READBACKS_ENABLE_READ_TIME_TAG                (1u)
#define BITM_PORT_GENERAL_CONTROLS_AND_READBACKS_ENABLE_READ_TIME_TAG       (0x80000000u)
// Enums
#define ENUM_PORT_GENERAL_CONTROLS_AND_READBACKS_ENABLE_READ_TIME_TAG_DISABLE (0x00000000u)
#define ENUM_PORT_GENERAL_CONTROLS_AND_READBACKS_ENABLE_READ_TIME_TAG_ENABLE (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : active_output_readback
// Register Addr        : 0x005Cu
// Register NW Addr     : 0x005Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_ACTIVE_OUTPUT_READBACK_ADDR                                        (0x005Cu)
#define PORT_ACTIVE_OUTPUT_READBACK_NW_ADDR                                     (0x005Cu)
#define PORT_ACTIVE_OUTPUT_READBACK_RESET_VAL                               (0x00000000u)
#define PORT_ACTIVE_OUTPUT_READBACK_MMS                                              (0u)
#define PORT_ACTIVE_OUTPUT_READBACK_VOL_MASK                                (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | active_output            | Indicates if Each of the PWM      | 0x00000000  |
// |       |                          | Signals are Toggling or Not       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: active_output
// Bit position, length and mask
#define BITP_PORT_ACTIVE_OUTPUT_READBACK_ACTIVE_OUTPUT                               (0u)
#define BITL_PORT_ACTIVE_OUTPUT_READBACK_ACTIVE_OUTPUT                              (12u)
#define BITM_PORT_ACTIVE_OUTPUT_READBACK_ACTIVE_OUTPUT                      (0x00000FFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : base_time_slope_readback
// Register Addr        : 0x0060u
// Register NW Addr     : 0x0060u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_BASE_TIME_SLOPE_READBACK_ADDR                                      (0x0060u)
#define PORT_BASE_TIME_SLOPE_READBACK_NW_ADDR                                   (0x0060u)
#define PORT_BASE_TIME_SLOPE_READBACK_RESET_VAL                             (0x00000000u)
#define PORT_BASE_TIME_SLOPE_READBACK_MMS                                            (0u)
#define PORT_BASE_TIME_SLOPE_READBACK_VOL_MASK                              (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 08-00 | base_time_slope_unit_0   | Configured Slope Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17-09 | base_time_slope_unit_1   | Configured Slope Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 26-18 | base_time_slope_unit_2   | Configured Slope Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: base_time_slope_unit_0
// Bit position, length and mask
#define BITP_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_0                    (0u)
#define BITL_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_0                    (9u)
#define BITM_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_0           (0x000001FFu)

// Bit field: base_time_slope_unit_1
// Bit position, length and mask
#define BITP_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_1                    (9u)
#define BITL_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_1                    (9u)
#define BITM_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_1           (0x0003FE00u)

// Bit field: base_time_slope_unit_2
// Bit position, length and mask
#define BITP_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_2                   (18u)
#define BITL_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_2                    (9u)
#define BITM_PORT_BASE_TIME_SLOPE_READBACK_BASE_TIME_SLOPE_UNIT_2           (0x07FC0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : duty_readback_0
// Register Addr        : 0x0064u
// Register NW Addr     : 0x0064u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_DUTY_READBACK_0_ADDR                                               (0x0064u)
#define PORT_DUTY_READBACK_0_NW_ADDR                                            (0x0064u)
#define PORT_DUTY_READBACK_0_RESET_VAL                                      (0x00000000u)
#define PORT_DUTY_READBACK_0_MMS                                                     (0u)
#define PORT_DUTY_READBACK_0_VOL_MASK                                       (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 06-00 | duty_0                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-07 | duty_1                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-14 | duty_2                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-21 | duty_3                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: duty_0
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_0_DUTY_0                                             (0u)
#define BITL_PORT_DUTY_READBACK_0_DUTY_0                                             (7u)
#define BITM_PORT_DUTY_READBACK_0_DUTY_0                                    (0x0000007Fu)

// Bit field: duty_1
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_0_DUTY_1                                             (7u)
#define BITL_PORT_DUTY_READBACK_0_DUTY_1                                             (7u)
#define BITM_PORT_DUTY_READBACK_0_DUTY_1                                    (0x00003F80u)

// Bit field: duty_2
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_0_DUTY_2                                            (14u)
#define BITL_PORT_DUTY_READBACK_0_DUTY_2                                             (7u)
#define BITM_PORT_DUTY_READBACK_0_DUTY_2                                    (0x001FC000u)

// Bit field: duty_3
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_0_DUTY_3                                            (21u)
#define BITL_PORT_DUTY_READBACK_0_DUTY_3                                             (7u)
#define BITM_PORT_DUTY_READBACK_0_DUTY_3                                    (0x0FE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : duty_readback_1
// Register Addr        : 0x0068u
// Register NW Addr     : 0x0068u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_DUTY_READBACK_1_ADDR                                               (0x0068u)
#define PORT_DUTY_READBACK_1_NW_ADDR                                            (0x0068u)
#define PORT_DUTY_READBACK_1_RESET_VAL                                      (0x00000000u)
#define PORT_DUTY_READBACK_1_MMS                                                     (0u)
#define PORT_DUTY_READBACK_1_VOL_MASK                                       (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 06-00 | duty_4                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-07 | duty_5                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-14 | duty_6                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-21 | duty_7                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: duty_4
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_1_DUTY_4                                             (0u)
#define BITL_PORT_DUTY_READBACK_1_DUTY_4                                             (7u)
#define BITM_PORT_DUTY_READBACK_1_DUTY_4                                    (0x0000007Fu)

// Bit field: duty_5
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_1_DUTY_5                                             (7u)
#define BITL_PORT_DUTY_READBACK_1_DUTY_5                                             (7u)
#define BITM_PORT_DUTY_READBACK_1_DUTY_5                                    (0x00003F80u)

// Bit field: duty_6
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_1_DUTY_6                                            (14u)
#define BITL_PORT_DUTY_READBACK_1_DUTY_6                                             (7u)
#define BITM_PORT_DUTY_READBACK_1_DUTY_6                                    (0x001FC000u)

// Bit field: duty_7
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_1_DUTY_7                                            (21u)
#define BITL_PORT_DUTY_READBACK_1_DUTY_7                                             (7u)
#define BITM_PORT_DUTY_READBACK_1_DUTY_7                                    (0x0FE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : duty_readback_2
// Register Addr        : 0x006Cu
// Register NW Addr     : 0x006Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_DUTY_READBACK_2_ADDR                                               (0x006Cu)
#define PORT_DUTY_READBACK_2_NW_ADDR                                            (0x006Cu)
#define PORT_DUTY_READBACK_2_RESET_VAL                                      (0x00000000u)
#define PORT_DUTY_READBACK_2_MMS                                                     (0u)
#define PORT_DUTY_READBACK_2_VOL_MASK                                       (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 06-00 | duty_8                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-07 | duty_9                   | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-14 | duty_10                  | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-21 | duty_11                  | Configured Duty Value Readback    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: duty_8
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_2_DUTY_8                                             (0u)
#define BITL_PORT_DUTY_READBACK_2_DUTY_8                                             (7u)
#define BITM_PORT_DUTY_READBACK_2_DUTY_8                                    (0x0000007Fu)

// Bit field: duty_9
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_2_DUTY_9                                             (7u)
#define BITL_PORT_DUTY_READBACK_2_DUTY_9                                             (7u)
#define BITM_PORT_DUTY_READBACK_2_DUTY_9                                    (0x00003F80u)

// Bit field: duty_10
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_2_DUTY_10                                           (14u)
#define BITL_PORT_DUTY_READBACK_2_DUTY_10                                            (7u)
#define BITM_PORT_DUTY_READBACK_2_DUTY_10                                   (0x001FC000u)

// Bit field: duty_11
// Bit position, length and mask
#define BITP_PORT_DUTY_READBACK_2_DUTY_11                                           (21u)
#define BITL_PORT_DUTY_READBACK_2_DUTY_11                                            (7u)
#define BITM_PORT_DUTY_READBACK_2_DUTY_11                                   (0x0FE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : delay_readback_0
// Register Addr        : 0x0070u
// Register NW Addr     : 0x0070u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_DELAY_READBACK_0_ADDR                                              (0x0070u)
#define PORT_DELAY_READBACK_0_NW_ADDR                                           (0x0070u)
#define PORT_DELAY_READBACK_0_RESET_VAL                                     (0x00000000u)
#define PORT_DELAY_READBACK_0_MMS                                                    (0u)
#define PORT_DELAY_READBACK_0_VOL_MASK                                      (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 06-00 | delay_0                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-07 | delay_1                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-14 | delay_2                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-21 | delay_3                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: delay_0
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_0_DELAY_0                                           (0u)
#define BITL_PORT_DELAY_READBACK_0_DELAY_0                                           (7u)
#define BITM_PORT_DELAY_READBACK_0_DELAY_0                                  (0x0000007Fu)

// Bit field: delay_1
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_0_DELAY_1                                           (7u)
#define BITL_PORT_DELAY_READBACK_0_DELAY_1                                           (7u)
#define BITM_PORT_DELAY_READBACK_0_DELAY_1                                  (0x00003F80u)

// Bit field: delay_2
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_0_DELAY_2                                          (14u)
#define BITL_PORT_DELAY_READBACK_0_DELAY_2                                           (7u)
#define BITM_PORT_DELAY_READBACK_0_DELAY_2                                  (0x001FC000u)

// Bit field: delay_3
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_0_DELAY_3                                          (21u)
#define BITL_PORT_DELAY_READBACK_0_DELAY_3                                           (7u)
#define BITM_PORT_DELAY_READBACK_0_DELAY_3                                  (0x0FE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : delay_readback_1
// Register Addr        : 0x0074u
// Register NW Addr     : 0x0074u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_DELAY_READBACK_1_ADDR                                              (0x0074u)
#define PORT_DELAY_READBACK_1_NW_ADDR                                           (0x0074u)
#define PORT_DELAY_READBACK_1_RESET_VAL                                     (0x00000000u)
#define PORT_DELAY_READBACK_1_MMS                                                    (0u)
#define PORT_DELAY_READBACK_1_VOL_MASK                                      (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 06-00 | delay_4                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-07 | delay_5                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-14 | delay_6                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-21 | delay_7                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: delay_4
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_1_DELAY_4                                           (0u)
#define BITL_PORT_DELAY_READBACK_1_DELAY_4                                           (7u)
#define BITM_PORT_DELAY_READBACK_1_DELAY_4                                  (0x0000007Fu)

// Bit field: delay_5
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_1_DELAY_5                                           (7u)
#define BITL_PORT_DELAY_READBACK_1_DELAY_5                                           (7u)
#define BITM_PORT_DELAY_READBACK_1_DELAY_5                                  (0x00003F80u)

// Bit field: delay_6
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_1_DELAY_6                                          (14u)
#define BITL_PORT_DELAY_READBACK_1_DELAY_6                                           (7u)
#define BITM_PORT_DELAY_READBACK_1_DELAY_6                                  (0x001FC000u)

// Bit field: delay_7
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_1_DELAY_7                                          (21u)
#define BITL_PORT_DELAY_READBACK_1_DELAY_7                                           (7u)
#define BITM_PORT_DELAY_READBACK_1_DELAY_7                                  (0x0FE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : delay_readback_2
// Register Addr        : 0x0078u
// Register NW Addr     : 0x0078u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_DELAY_READBACK_2_ADDR                                              (0x0078u)
#define PORT_DELAY_READBACK_2_NW_ADDR                                           (0x0078u)
#define PORT_DELAY_READBACK_2_RESET_VAL                                     (0x00000000u)
#define PORT_DELAY_READBACK_2_MMS                                                    (0u)
#define PORT_DELAY_READBACK_2_VOL_MASK                                      (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 06-00 | delay_8                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-07 | delay_9                  | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 20-14 | delay_10                 | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-21 | delay_11                 | Configured Delay Value Readback   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: delay_8
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_2_DELAY_8                                           (0u)
#define BITL_PORT_DELAY_READBACK_2_DELAY_8                                           (7u)
#define BITM_PORT_DELAY_READBACK_2_DELAY_8                                  (0x0000007Fu)

// Bit field: delay_9
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_2_DELAY_9                                           (7u)
#define BITL_PORT_DELAY_READBACK_2_DELAY_9                                           (7u)
#define BITM_PORT_DELAY_READBACK_2_DELAY_9                                  (0x00003F80u)

// Bit field: delay_10
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_2_DELAY_10                                         (14u)
#define BITL_PORT_DELAY_READBACK_2_DELAY_10                                          (7u)
#define BITM_PORT_DELAY_READBACK_2_DELAY_10                                 (0x001FC000u)

// Bit field: delay_11
// Bit position, length and mask
#define BITP_PORT_DELAY_READBACK_2_DELAY_11                                         (21u)
#define BITL_PORT_DELAY_READBACK_2_DELAY_11                                          (7u)
#define BITM_PORT_DELAY_READBACK_2_DELAY_11                                 (0x0FE00000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_0_controls_2
// Register Addr        : 0x007Cu
// Register NW Addr     : 0x007Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_0_CONTROLS_2_ADDR                                             (0x007Cu)
#define PORT_UNIT_0_CONTROLS_2_NW_ADDR                                          (0x007Cu)
#define PORT_UNIT_0_CONTROLS_2_RESET_VAL                                    (0x00000000u)
#define PORT_UNIT_0_CONTROLS_2_MMS                                                   (0u)
#define PORT_UNIT_0_CONTROLS_2_VOL_MASK                                     (0x80FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | duty_limit_unit_0_out_0  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | duty_limit_unit_0_out_1  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | duty_limit_unit_0_out_2  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | duty_limit_unit_0_out_3  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-04 | divider_settings_unit_0  | Selects the strength of the       | 0x00000000  |
// |       |                          | clock_divider Feature. Maximum    |             |
// |       |                          | value 180.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-09 | emc_strength_unit_0      | Selects the EMC Reduction Mode    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-12 | pair_0_output_configurati| Selects the output configuration  | 0x00000000  | 
// |       |                          | of the first pair of outputs for  |             |
// |       |                          | Unit 0                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-14 | pair_1_output_configurati| Selects the output configuration  | 0x00000000  | 
// |       |                          | of the second pair of outputs     |             |
// |       |                          | for Unit 0                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | full_hbridge_direction_un| Selects the direction of the      | 0x00000000  | 
// |       |                          | full h-bridge for Unit 0          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | auto_shutdown_enable_unit| Enables the auto-shutdown         | 0x00000000  | 
// |       |                          | feature. The pwm unit will stop   |             |
// |       |                          | when a fault condition is         |             |
// |       |                          | detected in an external gpio      |             |
// |       |                          | pin.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | auto_shutdown_polarity_un| Selects the gpio level that is    | 0x00000000  | 
// |       |                          | going to trigger the              |             |
// |       |                          | auto-shutdown                     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22-19 | auto_shutdown_input_monit| Select the gpio that is going to  | 0x00000000  | 
// |       |                          | be monitored for auto-shutdown.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | auto_shutdown_autorestart| The auto-shutdown will be         | 0x00000000  | 
// |       |                          | released when the trigger stops   |             |
// |       |                          | appearing.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | auto_shutdown_restart_uni| Makes the unit active again. If   | 0x00000000  | 
// |       |                          | the trigger has not been          |             |
// |       |                          | cleaned, the pwm unit will        |             |
// |       |                          | remain disabled.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: duty_limit_unit_0_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_0                          (0u)
#define BITL_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_0                          (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_0                 (0x00000001u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_0_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_0_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_0_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_1                          (1u)
#define BITL_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_1                          (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_1                 (0x00000002u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_1_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_1_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_0_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_2                          (2u)
#define BITL_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_2                          (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_2                 (0x00000004u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_2_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_2_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_0_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_3                          (3u)
#define BITL_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_3                          (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_3                 (0x00000008u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_3_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_DUTY_LIMIT_UNIT_0_OUT_3_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: divider_settings_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_DIVIDER_SETTINGS_UNIT_0                          (4u)
#define BITL_PORT_UNIT_0_CONTROLS_2_DIVIDER_SETTINGS_UNIT_0                          (5u)
#define BITM_PORT_UNIT_0_CONTROLS_2_DIVIDER_SETTINGS_UNIT_0                 (0x000001F0u)

// Bit field: emc_strength_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0                              (9u)
#define BITL_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0                              (3u)
#define BITM_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0                     (0x00000E00u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0_PCU_EMC_MODE_NONE   (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0_PCU_EMC_MODE_MINIMUM (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0_PCU_EMC_MODE_NOMINAL (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0_PCU_EMC_MODE_HIGH   (0x00000003u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_EMC_STRENGTH_UNIT_0_PCU_EMC_MODE_MAXIMUM (0x00000004u)

// Bit field: pair_0_output_configuration_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_0              (12u)
#define BITL_PORT_UNIT_0_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_0               (2u)
#define BITM_PORT_UNIT_0_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_0      (0x00003000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_0_STANDARD (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_0_PUSH_PULL (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_0_HALF_BRIDGE (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_0_FULL_BRIDGE (0x00000003u)

// Bit field: pair_1_output_configuration_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_0              (14u)
#define BITL_PORT_UNIT_0_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_0               (2u)
#define BITM_PORT_UNIT_0_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_0      (0x0000C000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_0_STANDARD (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_0_PUSH_PULL (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_0_HALF_BRIDGE (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_0_FULL_BRIDGE (0x00000003u)

// Bit field: full_hbridge_direction_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_0                   (16u)
#define BITL_PORT_UNIT_0_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_0                    (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_0           (0x00010000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_0_REVERSE   (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_0_FORWARD   (0x00000001u)

// Bit field: auto_shutdown_enable_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_0                     (17u)
#define BITL_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_0                      (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_0             (0x00020000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_0_DISABLE     (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_0_ENABLE      (0x00000001u)

// Bit field: auto_shutdown_polarity_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_0                   (18u)
#define BITL_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_0                    (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_0           (0x00040000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_0_LOW       (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_0_HIGH      (0x00000001u)

// Bit field: auto_shutdown_input_monitor_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0              (19u)
#define BITL_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0               (4u)
#define BITM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0      (0x00780000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_0 (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_1 (0x00000001u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_2 (0x00000002u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_3 (0x00000003u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_4 (0x00000004u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_5 (0x00000005u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_6 (0x00000006u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_7 (0x00000007u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_8 (0x00000008u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_9 (0x00000009u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_10 (0x0000000Au)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_0_SA_IF_11 (0x0000000Bu)

// Bit field: auto_shutdown_autorestart_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_0                (23u)
#define BITL_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_0                 (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_0        (0x00800000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_0_DISABLE (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_0_ENABLE (0x00000001u)

// Bit field: auto_shutdown_restart_unit_0
// Bit position, length and mask
#define BITP_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_0                    (31u)
#define BITL_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_0                     (1u)
#define BITM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_0            (0x80000000u)
// Enums
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_0_INACTIVE   (0x00000000u)
#define ENUM_PORT_UNIT_0_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_0_TRIGGER    (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_1_controls_2
// Register Addr        : 0x0080u
// Register NW Addr     : 0x0080u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_1_CONTROLS_2_ADDR                                             (0x0080u)
#define PORT_UNIT_1_CONTROLS_2_NW_ADDR                                          (0x0080u)
#define PORT_UNIT_1_CONTROLS_2_RESET_VAL                                    (0x00000000u)
#define PORT_UNIT_1_CONTROLS_2_MMS                                                   (0u)
#define PORT_UNIT_1_CONTROLS_2_VOL_MASK                                     (0x80FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | duty_limit_unit_1_out_0  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | duty_limit_unit_1_out_1  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | duty_limit_unit_1_out_2  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | duty_limit_unit_1_out_3  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-04 | divider_settings_unit_1  | Selects the strength of the       | 0x00000000  |
// |       |                          | clock_divider Feature. Maximum    |             |
// |       |                          | value 18.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-09 | emc_strength_unit_1      | Selects the EMC Reduction Mode    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-12 | pair_0_output_configurati| Selects the output configuration  | 0x00000000  | 
// |       |                          | of the first pair of outputs for  |             |
// |       |                          | Unit 1                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-14 | pair_1_output_configurati| Selects the output configuration  | 0x00000000  | 
// |       |                          | of the second pair of outputs     |             |
// |       |                          | for Unit 1                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | full_hbridge_direction_un| Selects the direction of the      | 0x00000000  | 
// |       |                          | full h-bridge for Unit 1          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | auto_shutdown_enable_unit| Enables the auto-shutdown         | 0x00000000  | 
// |       |                          | feature. The pwm unit will stop   |             |
// |       |                          | when a fault condition is         |             |
// |       |                          | detected in an external gpio      |             |
// |       |                          | pin.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | auto_shutdown_polarity_un| Selects the gpio level that is    | 0x00000000  | 
// |       |                          | going to trigger the              |             |
// |       |                          | auto-shutdown                     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22-19 | auto_shutdown_input_monit| Select the gpio that is going to  | 0x00000000  | 
// |       |                          | be monitored for auto-shutdown.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | auto_shutdown_autorestart| The auto_shutdown will be         | 0x00000000  | 
// |       |                          | released when the trigger stops   |             |
// |       |                          | appearing.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | auto_shutdown_restart_uni| Makes the unit active again. If   | 0x00000000  | 
// |       |                          | the trigger has not been          |             |
// |       |                          | cleaned, the pwm unit will        |             |
// |       |                          | remain disabled.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: duty_limit_unit_1_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_0                          (0u)
#define BITL_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_0                          (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_0                 (0x00000001u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_0_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_0_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_1_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_1                          (1u)
#define BITL_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_1                          (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_1                 (0x00000002u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_1_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_1_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_1_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_2                          (2u)
#define BITL_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_2                          (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_2                 (0x00000004u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_2_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_2_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_1_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_3                          (3u)
#define BITL_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_3                          (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_3                 (0x00000008u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_3_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_DUTY_LIMIT_UNIT_1_OUT_3_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: divider_settings_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_DIVIDER_SETTINGS_UNIT_1                          (4u)
#define BITL_PORT_UNIT_1_CONTROLS_2_DIVIDER_SETTINGS_UNIT_1                          (5u)
#define BITM_PORT_UNIT_1_CONTROLS_2_DIVIDER_SETTINGS_UNIT_1                 (0x000001F0u)

// Bit field: emc_strength_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1                              (9u)
#define BITL_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1                              (3u)
#define BITM_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1                     (0x00000E00u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1_PCU_EMC_MODE_NONE   (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1_PCU_EMC_MODE_MINIMUM (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1_PCU_EMC_MODE_NOMINAL (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1_PCU_EMC_MODE_HIGH   (0x00000003u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_EMC_STRENGTH_UNIT_1_PCU_EMC_MODE_MAXIMUM (0x00000004u)

// Bit field: pair_0_output_configuration_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_1              (12u)
#define BITL_PORT_UNIT_1_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_1               (2u)
#define BITM_PORT_UNIT_1_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_1      (0x00003000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_1_STANDARD (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_1_PUSH_PULL (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_1_HALF_BRIDGE (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_1_FULL_BRIDGE (0x00000003u)

// Bit field: pair_1_output_configuration_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_1              (14u)
#define BITL_PORT_UNIT_1_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_1               (2u)
#define BITM_PORT_UNIT_1_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_1      (0x0000C000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_1_STANDARD (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_1_PUSH_PULL (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_1_HALF_BRIDGE (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_1_FULL_BRIDGE (0x00000003u)

// Bit field: full_hbridge_direction_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_1                   (16u)
#define BITL_PORT_UNIT_1_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_1                    (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_1           (0x00010000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_1_REVERSE   (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_1_FORWARD   (0x00000001u)

// Bit field: auto_shutdown_enable_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_1                     (17u)
#define BITL_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_1                      (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_1             (0x00020000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_1_DISABLE     (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_1_ENABLE      (0x00000001u)

// Bit field: auto_shutdown_polarity_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_1                   (18u)
#define BITL_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_1                    (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_1           (0x00040000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_1_LOW       (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_1_HIGH      (0x00000001u)

// Bit field: auto_shutdown_input_monitor_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1              (19u)
#define BITL_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1               (4u)
#define BITM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1      (0x00780000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_0 (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_1 (0x00000001u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_2 (0x00000002u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_3 (0x00000003u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_4 (0x00000004u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_5 (0x00000005u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_6 (0x00000006u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_7 (0x00000007u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_8 (0x00000008u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_9 (0x00000009u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_10 (0x0000000Au)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_1_SA_IF_11 (0x0000000Bu)

// Bit field: auto_shutdown_autorestart_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_1                (23u)
#define BITL_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_1                 (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_1        (0x00800000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_1_DISABLE (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_1_ENABLE (0x00000001u)

// Bit field: auto_shutdown_restart_unit_1
// Bit position, length and mask
#define BITP_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_1                    (31u)
#define BITL_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_1                     (1u)
#define BITM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_1            (0x80000000u)
// Enums
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_1_INACTIVE   (0x00000000u)
#define ENUM_PORT_UNIT_1_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_1_TRIGGER    (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : unit_2_controls_2
// Register Addr        : 0x0084u
// Register NW Addr     : 0x0084u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_UNIT_2_CONTROLS_2_ADDR                                             (0x0084u)
#define PORT_UNIT_2_CONTROLS_2_NW_ADDR                                          (0x0084u)
#define PORT_UNIT_2_CONTROLS_2_RESET_VAL                                    (0x00000000u)
#define PORT_UNIT_2_CONTROLS_2_MMS                                                   (0u)
#define PORT_UNIT_2_CONTROLS_2_VOL_MASK                                     (0x80FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | duty_limit_unit_2_out_0  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | duty_limit_unit_2_out_1  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | duty_limit_unit_2_out_2  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | duty_limit_unit_2_out_3  | Enables The 5 Perc Duty Limit.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-04 | divider_settings_unit_2  | Selects the strength of the       | 0x00000000  |
// |       |                          | clock_divider Feature. Maximum    |             |
// |       |                          | value 18.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-09 | emc_strength_unit_2      | Selects the EMC Reduction Mode    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-12 | pair_0_output_configurati| Selects the output configuration  | 0x00000000  | 
// |       |                          | of the first pair of outputs for  |             |
// |       |                          | Unit 2                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-14 | pair_1_output_configurati| Selects the output configuration  | 0x00000000  | 
// |       |                          | of the second pair of outputs     |             |
// |       |                          | for Unit 2                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | full_hbridge_direction_un| Selects the direction of the      | 0x00000000  | 
// |       |                          | full h-bridge for Unit 2          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | auto_shutdown_enable_unit| Enables the auto-shutdown         | 0x00000000  | 
// |       |                          | feature. The pwm unit will stop   |             |
// |       |                          | when a fault condition is         |             |
// |       |                          | detected in an external gpio      |             |
// |       |                          | pin.                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | auto_shutdown_polarity_un| Selects the gpio level that is    | 0x00000000  | 
// |       |                          | going to trigger the              |             |
// |       |                          | auto-shutdown                     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22-19 | auto_shutdown_input_monit| Select the gpio that is going to  | 0x00000000  | 
// |       |                          | be monitored for auto-shutdown.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | auto_shutdown_autorestart| The auto_shutdown will be         | 0x00000000  | 
// |       |                          | released when the trigger stops   |             |
// |       |                          | appearing.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | auto_shutdown_restart_uni| Makes the unit active again. If   | 0x00000000  | 
// |       |                          | the trigger has not been          |             |
// |       |                          | cleaned, the pwm unit will        |             |
// |       |                          | remain disabled.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: duty_limit_unit_2_out_0
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_0                          (0u)
#define BITL_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_0                          (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_0                 (0x00000001u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_0_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_0_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_2_out_1
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_1                          (1u)
#define BITL_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_1                          (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_1                 (0x00000002u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_1_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_1_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_2_out_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_2                          (2u)
#define BITL_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_2                          (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_2                 (0x00000004u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_2_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_2_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: duty_limit_unit_2_out_3
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_3                          (3u)
#define BITL_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_3                          (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_3                 (0x00000008u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_3_PCU_LIMIT_NONE  (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_DUTY_LIMIT_UNIT_2_OUT_3_PCU_LIMIT_5_PERC (0x00000001u)

// Bit field: divider_settings_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_DIVIDER_SETTINGS_UNIT_2                          (4u)
#define BITL_PORT_UNIT_2_CONTROLS_2_DIVIDER_SETTINGS_UNIT_2                          (5u)
#define BITM_PORT_UNIT_2_CONTROLS_2_DIVIDER_SETTINGS_UNIT_2                 (0x000001F0u)

// Bit field: emc_strength_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2                              (9u)
#define BITL_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2                              (3u)
#define BITM_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2                     (0x00000E00u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2_PCU_EMC_MODE_NONE   (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2_PCU_EMC_MODE_MINIMUM (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2_PCU_EMC_MODE_NOMINAL (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2_PCU_EMC_MODE_HIGH   (0x00000003u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_EMC_STRENGTH_UNIT_2_PCU_EMC_MODE_MAXIMUM (0x00000004u)

// Bit field: pair_0_output_configuration_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_2              (12u)
#define BITL_PORT_UNIT_2_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_2               (2u)
#define BITM_PORT_UNIT_2_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_2      (0x00003000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_2_STANDARD (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_2_PUSH_PULL (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_2_HALF_BRIDGE (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_0_OUTPUT_CONFIGURATION_UNIT_2_FULL_BRIDGE (0x00000003u)

// Bit field: pair_1_output_configuration_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_2              (14u)
#define BITL_PORT_UNIT_2_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_2               (2u)
#define BITM_PORT_UNIT_2_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_2      (0x0000C000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_2_STANDARD (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_2_PUSH_PULL (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_2_HALF_BRIDGE (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_PAIR_1_OUTPUT_CONFIGURATION_UNIT_2_FULL_BRIDGE (0x00000003u)

// Bit field: full_hbridge_direction_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_2                   (16u)
#define BITL_PORT_UNIT_2_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_2                    (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_2           (0x00010000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_2_REVERSE   (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_FULL_HBRIDGE_DIRECTION_UNIT_2_FORWARD   (0x00000001u)

// Bit field: auto_shutdown_enable_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_2                     (17u)
#define BITL_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_2                      (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_2             (0x00020000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_2_DISABLE     (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_ENABLE_UNIT_2_ENABLE      (0x00000001u)

// Bit field: auto_shutdown_polarity_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_2                   (18u)
#define BITL_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_2                    (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_2           (0x00040000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_2_LOW       (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_POLARITY_UNIT_2_HIGH      (0x00000001u)

// Bit field: auto_shutdown_input_monitor_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2              (19u)
#define BITL_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2               (4u)
#define BITM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2      (0x00780000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_0 (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_1 (0x00000001u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_2 (0x00000002u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_3 (0x00000003u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_4 (0x00000004u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_5 (0x00000005u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_6 (0x00000006u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_7 (0x00000007u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_8 (0x00000008u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_9 (0x00000009u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_10 (0x0000000Au)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_INPUT_MONITOR_UNIT_2_SA_IF_11 (0x0000000Bu)

// Bit field: auto_shutdown_autorestart_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_2                (23u)
#define BITL_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_2                 (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_2        (0x00800000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_2_DISABLE (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_AUTORESTART_UNIT_2_ENABLE (0x00000001u)

// Bit field: auto_shutdown_restart_unit_2
// Bit position, length and mask
#define BITP_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_2                    (31u)
#define BITL_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_2                     (1u)
#define BITM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_2            (0x80000000u)
// Enums
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_2_INACTIVE   (0x00000000u)
#define ENUM_PORT_UNIT_2_CONTROLS_2_AUTO_SHUTDOWN_RESTART_UNIT_2_TRIGGER    (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : output_port_configuration
// Register Addr        : 0x0088u
// Register NW Addr     : 0x0088u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_OUTPUT_PORT_CONFIGURATION_ADDR                                     (0x0088u)
#define PORT_OUTPUT_PORT_CONFIGURATION_NW_ADDR                                  (0x0088u)
#define PORT_OUTPUT_PORT_CONFIGURATION_RESET_VAL                            (0x00000000u)
#define PORT_OUTPUT_PORT_CONFIGURATION_MMS                                           (0u)
#define PORT_OUTPUT_PORT_CONFIGURATION_VOL_MASK                             (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | output_port_0_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 0.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | output_port_1_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 1.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | output_port_2_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 2.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | output_port_3_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 3.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-16 | output_port_4_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 4.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-20 | output_port_5_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 5.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-24 | output_port_6_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 6.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | output_port_7_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 7.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: output_port_0_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG                        (0u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG               (0x0000000Fu)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_0_CFG_CLK_250       (0x0000000Bu)

// Bit field: output_port_1_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG                        (4u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG               (0x000000F0u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_1_CFG_CLK_250       (0x0000000Bu)

// Bit field: output_port_2_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG                        (8u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG               (0x00000F00u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_2_CFG_CLK_250       (0x0000000Bu)

// Bit field: output_port_3_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG                       (12u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG               (0x0000F000u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_3_CFG_CLK_250       (0x0000000Bu)

// Bit field: output_port_4_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG                       (16u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG               (0x000F0000u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_4_CFG_CLK_250       (0x0000000Bu)

// Bit field: output_port_5_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG                       (20u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG               (0x00F00000u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_5_CFG_CLK_250       (0x0000000Bu)

// Bit field: output_port_6_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG                       (24u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG               (0x0F000000u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_6_CFG_CLK_250       (0x0000000Bu)

// Bit field: output_port_7_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG                       (28u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG                        (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG               (0xF0000000u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_PWM_MODE      (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_25000     (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_12500     (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_6250      (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_3125      (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_2500      (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_2000      (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_1250      (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_625       (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_500       (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_312_5     (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_OUTPUT_PORT_7_CFG_CLK_250       (0x0000000Bu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : output_port_configuration_2
// Register Addr        : 0x008Cu
// Register NW Addr     : 0x008Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define PORT_OUTPUT_PORT_CONFIGURATION_2_ADDR                                   (0x008Cu)
#define PORT_OUTPUT_PORT_CONFIGURATION_2_NW_ADDR                                (0x008Cu)
#define PORT_OUTPUT_PORT_CONFIGURATION_2_RESET_VAL                          (0x00000000u)
#define PORT_OUTPUT_PORT_CONFIGURATION_2_MMS                                         (0u)
#define PORT_OUTPUT_PORT_CONFIGURATION_2_VOL_MASK                           (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | output_port_8_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 8.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | output_port_9_cfg        | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 9.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | output_port_10_cfg       | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 10.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | output_port_11_cfg       | Selects the output mode for       | 0x00000000  |
// |       |                          | SA_IF 11.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: output_port_8_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG                      (0u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG                      (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG             (0x0000000Fu)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_PWM_MODE    (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_25000   (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_12500   (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_6250    (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_3125    (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_2500    (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_2000    (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_1250    (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_625     (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_500     (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_312_5   (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_8_CFG_CLK_250     (0x0000000Bu)

// Bit field: output_port_9_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG                      (4u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG                      (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG             (0x000000F0u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_PWM_MODE    (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_25000   (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_12500   (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_6250    (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_3125    (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_2500    (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_2000    (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_1250    (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_625     (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_500     (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_312_5   (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_9_CFG_CLK_250     (0x0000000Bu)

// Bit field: output_port_10_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG                     (8u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG                     (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG            (0x00000F00u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_PWM_MODE   (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_25000  (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_12500  (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_6250   (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_3125   (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_2500   (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_2000   (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_1250   (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_625    (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_500    (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_312_5  (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_10_CFG_CLK_250    (0x0000000Bu)

// Bit field: output_port_11_cfg
// Bit position, length and mask
#define BITP_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG                    (12u)
#define BITL_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG                     (4u)
#define BITM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG            (0x0000F000u)
// Enums
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_PWM_MODE   (0x00000000u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_25000  (0x00000001u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_12500  (0x00000002u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_6250   (0x00000003u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_3125   (0x00000004u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_2500   (0x00000005u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_2000   (0x00000006u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_1250   (0x00000007u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_625    (0x00000008u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_500    (0x00000009u)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_312_5  (0x0000000Au)
#define ENUM_PORT_OUTPUT_PORT_CONFIGURATION_2_OUTPUT_PORT_11_CFG_CLK_250    (0x0000000Bu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillPORTRegsData();
#endif


#endif /* ADI_E2BCORE_REGPORT_H */

/** @} */

/*
 * EOF: www.analog.com
 */


