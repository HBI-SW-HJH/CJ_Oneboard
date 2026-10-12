/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_reghost.h
 * @brief        : This is an auto generated header file with register
                   descriptions for HOST interface
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGHOST_H
#define ADI_E2BCORE_REGHOST_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define HOST_IF_NUM_REGS    (15u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganHOST_IFRegAddr[HOST_IF_NUM_REGS];
extern uint32_t ganHOST_IFNwRegAddr[HOST_IF_NUM_REGS];
extern uint32_t ganHOST_IFRegMMS[HOST_IF_NUM_REGS];
extern uint32_t ganHOST_IFRegResetVal[HOST_IF_NUM_REGS];
extern const char *gasHOST_IFRegName[HOST_IF_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : to_intf_mux
// Register Addr        : 0xD000u
// Register NW Addr     : 0xD000u
// Register Reset Value : 0x77777777u
// Register MMS         : 0u

#define HOST_IF_TO_INTF_MUX_ADDR                                                (0xD000u)
#define HOST_IF_TO_INTF_MUX_NW_ADDR                                             (0xD000u)
#define HOST_IF_TO_INTF_MUX_RESET_VAL                                       (0x77777777u)
#define HOST_IF_TO_INTF_MUX_MMS                                                      (0u)
#define HOST_IF_TO_INTF_MUX_VOL_MASK                                        (0x77777777u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | to_intf_mux_0            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to I2C interface. Use   |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 06-04 | to_intf_mux_1            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to SPI0 interface. Use  |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10-08 | to_intf_mux_2            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to ISELED0 interface.   |             |
// |       |                          | Use 0x7 for no TX FIFO.           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14-12 | to_intf_mux_3            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to ISELED1 interface.   |             |
// |       |                          | Use 0x7 for no TX FIFO.           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18-16 | to_intf_mux_4            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to ISELED2 interface.   |             |
// |       |                          | Use 0x7 for no TX FIFO.           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22-20 | to_intf_mux_5            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to ISELED3 interface.   |             |
// |       |                          | Use 0x7 for no TX FIFO.           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 26-24 | to_intf_mux_6            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to PORT_CONTROLLER      |             |
// |       |                          | interface. Use 0x7 for no TX      |             |
// |       |                          | FIFO.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30-28 | to_intf_mux_7            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to LIN0 interface. Use  |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: to_intf_mux_0
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_0                                       (0u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_0                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_0                              (0x00000007u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_0_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_0_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_0_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_0_HARDWARE_INTERFACE_3         (0x00000003u)

// Bit field: to_intf_mux_1
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_1                                       (4u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_1                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_1                              (0x00000070u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_1_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_1_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_1_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_1_HARDWARE_INTERFACE_3         (0x00000003u)

// Bit field: to_intf_mux_2
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_2                                       (8u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_2                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_2                              (0x00000700u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_2_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_2_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_2_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_2_HARDWARE_INTERFACE_3         (0x00000003u)

// Bit field: to_intf_mux_3
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_3                                      (12u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_3                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_3                              (0x00007000u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_3_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_3_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_3_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_3_HARDWARE_INTERFACE_3         (0x00000003u)

// Bit field: to_intf_mux_4
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_4                                      (16u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_4                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_4                              (0x00070000u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_4_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_4_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_4_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_4_HARDWARE_INTERFACE_3         (0x00000003u)

// Bit field: to_intf_mux_5
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_5                                      (20u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_5                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_5                              (0x00700000u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_5_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_5_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_5_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_5_HARDWARE_INTERFACE_3         (0x00000003u)

// Bit field: to_intf_mux_6
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_6                                      (24u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_6                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_6                              (0x07000000u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_6_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_6_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_6_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_6_HARDWARE_INTERFACE_3         (0x00000003u)

// Bit field: to_intf_mux_7
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_7                                      (28u)
#define BITL_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_7                                       (3u)
#define BITM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_7                              (0x70000000u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_7_HARDWARE_INTERFACE_0         (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_7_HARDWARE_INTERFACE_1         (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_7_HARDWARE_INTERFACE_2         (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX_TO_INTF_MUX_7_HARDWARE_INTERFACE_3         (0x00000003u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : to_intf_mux2
// Register Addr        : 0xD004u
// Register NW Addr     : 0xD004u
// Register Reset Value : 0x00077777u
// Register MMS         : 0u

#define HOST_IF_TO_INTF_MUX2_ADDR                                               (0xD004u)
#define HOST_IF_TO_INTF_MUX2_NW_ADDR                                            (0xD004u)
#define HOST_IF_TO_INTF_MUX2_RESET_VAL                                      (0x00077777u)
#define HOST_IF_TO_INTF_MUX2_MMS                                                     (0u)
#define HOST_IF_TO_INTF_MUX2_VOL_MASK                                       (0x00077777u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | to_intf_mux_8            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to LIN1 interface. Use  |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 06-04 | to_intf_mux_9            | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to UART interface. Use  |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10-08 | to_intf_mux_10           | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to FIO0 interface. Use  |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14-12 | to_intf_mux_11           | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to FIO1 interface. Use  |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18-16 | to_intf_mux_12           | Selects which TX FIFO is          | 0x00000007  |
// |       |                          | connected to ADC interface. Use   |             |
// |       |                          | 0x7 for no TX FIFO.               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: to_intf_mux_8
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8                                      (0u)
#define BITL_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8                                      (3u)
#define BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8                             (0x00000007u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8_HARDWARE_INTERFACE_0        (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8_HARDWARE_INTERFACE_1        (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8_HARDWARE_INTERFACE_2        (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8_HARDWARE_INTERFACE_3        (0x00000003u)

// Bit field: to_intf_mux_9
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9                                      (4u)
#define BITL_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9                                      (3u)
#define BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9                             (0x00000070u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9_HARDWARE_INTERFACE_0        (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9_HARDWARE_INTERFACE_1        (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9_HARDWARE_INTERFACE_2        (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9_HARDWARE_INTERFACE_3        (0x00000003u)

// Bit field: to_intf_mux_10
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10                                     (8u)
#define BITL_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10                                     (3u)
#define BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10                            (0x00000700u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10_HARDWARE_INTERFACE_0       (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10_HARDWARE_INTERFACE_1       (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10_HARDWARE_INTERFACE_2       (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10_HARDWARE_INTERFACE_3       (0x00000003u)

// Bit field: to_intf_mux_11
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11                                    (12u)
#define BITL_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11                                     (3u)
#define BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11                            (0x00007000u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11_HARDWARE_INTERFACE_0       (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11_HARDWARE_INTERFACE_1       (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11_HARDWARE_INTERFACE_2       (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11_HARDWARE_INTERFACE_3       (0x00000003u)

// Bit field: to_intf_mux_12
// Bit position, length and mask
#define BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12                                    (16u)
#define BITL_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12                                     (3u)
#define BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12                            (0x00070000u)
// Enums
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12_HARDWARE_INTERFACE_0       (0x00000000u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12_HARDWARE_INTERFACE_1       (0x00000001u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12_HARDWARE_INTERFACE_2       (0x00000002u)
#define ENUM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12_HARDWARE_INTERFACE_3       (0x00000003u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : from_int_mux
// Register Addr        : 0xD008u
// Register NW Addr     : 0xD008u
// Register Reset Value : 0x000FFFFFu
// Register MMS         : 0u

#define HOST_IF_FROM_INT_MUX_ADDR                                               (0xD008u)
#define HOST_IF_FROM_INT_MUX_NW_ADDR                                            (0xD008u)
#define HOST_IF_FROM_INT_MUX_RESET_VAL                                      (0x000FFFFFu)
#define HOST_IF_FROM_INT_MUX_MMS                                                     (0u)
#define HOST_IF_FROM_INT_MUX_VOL_MASK                                       (0x000FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 04-00 | from_intf_mux_0          | Selects which interface is        | 0x0000001f  |
// |       |                          | routed to FIFO 0                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 09-05 | from_intf_mux_1          | Selects which interface is        | 0x0000001f  |
// |       |                          | routed to FIFO 1                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14-10 | from_intf_mux_2          | Selects which interface is        | 0x0000001f  |
// |       |                          | routed to FIFO 2                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-15 | from_intf_mux_3          | Selects which interface is        | 0x0000001f  |
// |       |                          | routed to FIFO 3                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: from_intf_mux_0
// Bit position, length and mask
#define BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0                                    (0u)
#define BITL_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0                                    (5u)
#define BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0                           (0x0000001Fu)
// Enums
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_I2C                       (0x00000000u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_SPI0                      (0x00000001u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_ISELED0                   (0x00000002u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_ISELED1                   (0x00000003u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_ISELED2                   (0x00000004u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_ISELED3                   (0x00000005u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_PORT_CONTROLLER           (0x00000006u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_LIN0                      (0x00000007u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_LIN1                      (0x00000008u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_UART                      (0x00000009u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_FIO0                      (0x0000000Au)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_FIO1                      (0x0000000Bu)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0_ADC                       (0x0000000Cu)

// Bit field: from_intf_mux_1
// Bit position, length and mask
#define BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1                                    (5u)
#define BITL_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1                                    (5u)
#define BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1                           (0x000003E0u)
// Enums
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_I2C                       (0x00000000u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_SPI0                      (0x00000001u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_ISELED0                   (0x00000002u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_ISELED1                   (0x00000003u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_ISELED2                   (0x00000004u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_ISELED3                   (0x00000005u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_PORT_CONTROLLER           (0x00000006u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_LIN0                      (0x00000007u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_LIN1                      (0x00000008u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_UART                      (0x00000009u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_FIO0                      (0x0000000Au)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_FIO1                      (0x0000000Bu)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1_ADC                       (0x0000000Cu)

// Bit field: from_intf_mux_2
// Bit position, length and mask
#define BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2                                   (10u)
#define BITL_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2                                    (5u)
#define BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2                           (0x00007C00u)
// Enums
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_I2C                       (0x00000000u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_SPI0                      (0x00000001u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_ISELED0                   (0x00000002u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_ISELED1                   (0x00000003u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_ISELED2                   (0x00000004u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_ISELED3                   (0x00000005u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_PORT_CONTROLLER           (0x00000006u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_LIN0                      (0x00000007u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_LIN1                      (0x00000008u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_UART                      (0x00000009u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_FIO0                      (0x0000000Au)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_FIO1                      (0x0000000Bu)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2_ADC                       (0x0000000Cu)

// Bit field: from_intf_mux_3
// Bit position, length and mask
#define BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3                                   (15u)
#define BITL_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3                                    (5u)
#define BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3                           (0x000F8000u)
// Enums
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_I2C                       (0x00000000u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_SPI0                      (0x00000001u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_ISELED0                   (0x00000002u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_ISELED1                   (0x00000003u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_ISELED2                   (0x00000004u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_ISELED3                   (0x00000005u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_PORT_CONTROLLER           (0x00000006u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_LIN0                      (0x00000007u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_LIN1                      (0x00000008u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_UART                      (0x00000009u)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_FIO0                      (0x0000000Au)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_FIO1                      (0x0000000Bu)
#define ENUM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3_ADC                       (0x0000000Cu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : input_selector0
// Register Addr        : 0xD00Cu
// Register NW Addr     : 0xD00Cu
// Register Reset Value : 0x00FFF000u
// Register MMS         : 0u

#define HOST_IF_INPUT_SELECTOR0_ADDR                                            (0xD00Cu)
#define HOST_IF_INPUT_SELECTOR0_NW_ADDR                                         (0xD00Cu)
#define HOST_IF_INPUT_SELECTOR0_RESET_VAL                                   (0x00FFF000u)
#define HOST_IF_INPUT_SELECTOR0_MMS                                                  (0u)
#define HOST_IF_INPUT_SELECTOR0_VOL_MASK                                    (0x00FFF000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-12 | spi_ready_input_selector | Selects the input SA_IF pin       | 0x0000000f  |
// |       |                          | connected to the SPI0 READY       |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-16 | spi_cito_input_selector  | Selects the input SA_IF pin       | 0x0000000f  |
// |       |                          | connected to the SPI0 MISO input  |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-20 | spi_sclk_input_selector  | Selects the input SA_IF pin       | 0x0000000f  |
// |       |                          | connected to the SPI0 SCLK input  |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: spi_ready_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR                       (12u)
#define BITL_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR                        (4u)
#define BITM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR               (0x0000F000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF0        (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF1        (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF2        (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF3        (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF4        (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF5        (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF6        (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF7        (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF8        (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF9        (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF10       (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_READY_INPUT_SELECTOR_SA_IF11       (0x0000000Bu)

// Bit field: spi_cito_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR                        (16u)
#define BITL_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR                         (4u)
#define BITM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR                (0x000F0000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF0         (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF1         (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF2         (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF3         (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF4         (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF5         (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF6         (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF7         (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF8         (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF9         (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF10        (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_CITO_INPUT_SELECTOR_SA_IF11        (0x0000000Bu)

// Bit field: spi_sclk_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR                        (20u)
#define BITL_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR                         (4u)
#define BITM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR                (0x00F00000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF0         (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF1         (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF2         (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF3         (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF4         (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF5         (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF6         (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF7         (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF8         (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF9         (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF10        (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR0_SPI_SCLK_INPUT_SELECTOR_SA_IF11        (0x0000000Bu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : input_selector1
// Register Addr        : 0xD010u
// Register NW Addr     : 0xD010u
// Register Reset Value : 0xFFFF00FFu
// Register MMS         : 0u

#define HOST_IF_INPUT_SELECTOR1_ADDR                                            (0xD010u)
#define HOST_IF_INPUT_SELECTOR1_NW_ADDR                                         (0xD010u)
#define HOST_IF_INPUT_SELECTOR1_RESET_VAL                                   (0xFFFF00FFu)
#define HOST_IF_INPUT_SELECTOR1_MMS                                                  (0u)
#define HOST_IF_INPUT_SELECTOR1_VOL_MASK                                    (0xFFFF00FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | lin0_in_input_selector   | Selects the input SA_IF pin       | 0x0000000f  |
// |       |                          | connected to the LIN0 in input    |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | lin1_in_input_selector   | Selects the input SA_IF pin       | 0x0000000f  |
// |       |                          | connected to the LIN1 in input    |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-16 | uart_rx_input_selector   | Selects the input SA_IF pin       | 0x0000000f  |
// |       |                          | connected to the UART Rx input    |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-20 | uart_cts_input_selector  | Selects the input SA_IF pin       | 0x0000000f  |
// |       |                          | connected to the UART CTS input   |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-24 | i2c_sclk_input_selector  | Selects which input pin is        | 0x0000000f  |
// |       |                          | routed to the I2C SCLK input      |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | i2c_sdata_input_selector | Selects which input pin is        | 0x0000000f  |
// |       |                          | routed to the I2C SDATA input     |             |
// |       |                          | port                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: lin0_in_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR                          (0u)
#define BITL_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR                          (4u)
#define BITM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR                 (0x0000000Fu)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF0          (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF1          (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF2          (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF3          (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF4          (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF5          (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF6          (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF7          (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF8          (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF9          (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF10         (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN0_IN_INPUT_SELECTOR_SA_IF11         (0x0000000Bu)

// Bit field: lin1_in_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR                          (4u)
#define BITL_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR                          (4u)
#define BITM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR                 (0x000000F0u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF0          (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF1          (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF2          (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF3          (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF4          (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF5          (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF6          (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF7          (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF8          (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF9          (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF10         (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR1_LIN1_IN_INPUT_SELECTOR_SA_IF11         (0x0000000Bu)

// Bit field: uart_rx_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR1_UART_RX_INPUT_SELECTOR                         (16u)
#define BITL_HOST_IF_INPUT_SELECTOR1_UART_RX_INPUT_SELECTOR                          (4u)
#define BITM_HOST_IF_INPUT_SELECTOR1_UART_RX_INPUT_SELECTOR                 (0x000F0000u)

// Bit field: uart_cts_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR1_UART_CTS_INPUT_SELECTOR                        (20u)
#define BITL_HOST_IF_INPUT_SELECTOR1_UART_CTS_INPUT_SELECTOR                         (4u)
#define BITM_HOST_IF_INPUT_SELECTOR1_UART_CTS_INPUT_SELECTOR                (0x00F00000u)

// Bit field: i2c_sclk_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR1_I2C_SCLK_INPUT_SELECTOR                        (24u)
#define BITL_HOST_IF_INPUT_SELECTOR1_I2C_SCLK_INPUT_SELECTOR                         (4u)
#define BITM_HOST_IF_INPUT_SELECTOR1_I2C_SCLK_INPUT_SELECTOR                (0x0F000000u)

// Bit field: i2c_sdata_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR1_I2C_SDATA_INPUT_SELECTOR                       (28u)
#define BITL_HOST_IF_INPUT_SELECTOR1_I2C_SDATA_INPUT_SELECTOR                        (4u)
#define BITM_HOST_IF_INPUT_SELECTOR1_I2C_SDATA_INPUT_SELECTOR               (0xF0000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : input_selector2
// Register Addr        : 0xD014u
// Register NW Addr     : 0xD014u
// Register Reset Value : 0xFFFFFFFFu
// Register MMS         : 0u

#define HOST_IF_INPUT_SELECTOR2_ADDR                                            (0xD014u)
#define HOST_IF_INPUT_SELECTOR2_NW_ADDR                                         (0xD014u)
#define HOST_IF_INPUT_SELECTOR2_RESET_VAL                                   (0xFFFFFFFFu)
#define HOST_IF_INPUT_SELECTOR2_MMS                                                  (0u)
#define HOST_IF_INPUT_SELECTOR2_VOL_MASK                                    (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | iseled2_sio_n_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED2 SIO_N    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | iseled2_sio_p_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED2 SIO_P    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | iseled3_sio_n_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED3 SIO_N    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | iseled3_sio_p_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED3 SIO_P    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-16 | iseled0_sio_n_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED0 SIO_N    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-20 | iseled0_sio_p_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED0 SIO_P    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-24 | iseled1_sio_n_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED1 SIO_N    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | iseled1_sio_p_input_selec| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the ISELED1 SIO_P    |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: iseled2_sio_n_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR                    (0u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR           (0x0000000Fu)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_N_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

// Bit field: iseled2_sio_p_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR                    (4u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR           (0x000000F0u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED2_SIO_P_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

// Bit field: iseled3_sio_n_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR                    (8u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR           (0x00000F00u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_N_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

// Bit field: iseled3_sio_p_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR                   (12u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR           (0x0000F000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED3_SIO_P_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

// Bit field: iseled0_sio_n_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR                   (16u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR           (0x000F0000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_N_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

// Bit field: iseled0_sio_p_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR                   (20u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR           (0x00F00000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED0_SIO_P_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

// Bit field: iseled1_sio_n_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR                   (24u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR           (0x0F000000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_N_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

// Bit field: iseled1_sio_p_input_selector
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR                   (28u)
#define BITL_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR                    (4u)
#define BITM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR           (0xF0000000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF0    (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF1    (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF2    (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF3    (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF4    (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF5    (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF6    (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF7    (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF8    (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF9    (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF10   (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR2_ISELED1_SIO_P_INPUT_SELECTOR_SA_IF11   (0x0000000Bu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : input_selector3
// Register Addr        : 0xD018u
// Register NW Addr     : 0xD018u
// Register Reset Value : 0xFFFFFFFFu
// Register MMS         : 0u

#define HOST_IF_INPUT_SELECTOR3_ADDR                                            (0xD018u)
#define HOST_IF_INPUT_SELECTOR3_NW_ADDR                                         (0xD018u)
#define HOST_IF_INPUT_SELECTOR3_RESET_VAL                                   (0xFFFFFFFFu)
#define HOST_IF_INPUT_SELECTOR3_MMS                                                  (0u)
#define HOST_IF_INPUT_SELECTOR3_VOL_MASK                                    (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-16 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-20 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-24 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | flex_io_0_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: flex_io_0_input_selector_0
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0                      (0u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0             (0x0000000Fu)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_0_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_0_input_selector_1
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1                      (4u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1             (0x000000F0u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_1_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_0_input_selector_2
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2                      (8u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2             (0x00000F00u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_2_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_0_input_selector_3
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3                     (12u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3             (0x0000F000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_3_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_0_input_selector_4
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4                     (16u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4             (0x000F0000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_4_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_0_input_selector_5
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5                     (20u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5             (0x00F00000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_5_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_0_input_selector_6
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6                     (24u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6             (0x0F000000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_6_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_0_input_selector_7
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7                     (28u)
#define BITL_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7             (0xF0000000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR3_FLEX_IO_0_INPUT_SELECTOR_7_SA_IF11     (0x0000000Bu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : input_selector4
// Register Addr        : 0xD01Cu
// Register NW Addr     : 0xD01Cu
// Register Reset Value : 0xFFFFFFFFu
// Register MMS         : 0u

#define HOST_IF_INPUT_SELECTOR4_ADDR                                            (0xD01Cu)
#define HOST_IF_INPUT_SELECTOR4_NW_ADDR                                         (0xD01Cu)
#define HOST_IF_INPUT_SELECTOR4_RESET_VAL                                   (0xFFFFFFFFu)
#define HOST_IF_INPUT_SELECTOR4_MMS                                                  (0u)
#define HOST_IF_INPUT_SELECTOR4_VOL_MASK                                    (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-08 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19-16 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-20 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-24 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-28 | flex_io_1_input_selector_| Selects the input SA_IF pin       | 0x0000000f  | 
// |       |                          | connected to the flexible IO      |             |
// |       |                          | input port                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: flex_io_1_input_selector_0
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0                      (0u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0             (0x0000000Fu)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_0_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_1_input_selector_1
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1                      (4u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1             (0x000000F0u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_1_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_1_input_selector_2
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2                      (8u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2             (0x00000F00u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_2_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_1_input_selector_3
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3                     (12u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3             (0x0000F000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_3_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_1_input_selector_4
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4                     (16u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4             (0x000F0000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_4_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_1_input_selector_5
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5                     (20u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5             (0x00F00000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_5_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_1_input_selector_6
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6                     (24u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6             (0x0F000000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_6_SA_IF11     (0x0000000Bu)

// Bit field: flex_io_1_input_selector_7
// Bit position, length and mask
#define BITP_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7                     (28u)
#define BITL_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7                      (4u)
#define BITM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7             (0xF0000000u)
// Enums
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_INPUT_SELECTOR4_FLEX_IO_1_INPUT_SELECTOR_7_SA_IF11     (0x0000000Bu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : output_selector0
// Register Addr        : 0xD020u
// Register NW Addr     : 0xD020u
// Register Reset Value : 0x3F3F3F3Fu
// Register MMS         : 0u

#define HOST_IF_OUTPUT_SELECTOR0_ADDR                                           (0xD020u)
#define HOST_IF_OUTPUT_SELECTOR0_NW_ADDR                                        (0xD020u)
#define HOST_IF_OUTPUT_SELECTOR0_RESET_VAL                                  (0x3F3F3F3Fu)
#define HOST_IF_OUTPUT_SELECTOR0_MMS                                                 (0u)
#define HOST_IF_OUTPUT_SELECTOR0_VOL_MASK                                   (0x3F3F3F3Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 05-00 | io0_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO0 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-08 | io1_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO1 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21-16 | io2_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO2 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-24 | io3_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO3 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: io0_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR                            (0u)
#define BITL_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR                   (0x0000003Fu)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO0_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io1_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR                            (8u)
#define BITL_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR                   (0x00003F00u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO1_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io2_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR                           (16u)
#define BITL_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR                   (0x003F0000u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO2_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io3_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR                           (24u)
#define BITL_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR                   (0x3F000000u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR0_IO3_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : output_selector1
// Register Addr        : 0xD024u
// Register NW Addr     : 0xD024u
// Register Reset Value : 0x3F3F3F3Fu
// Register MMS         : 0u

#define HOST_IF_OUTPUT_SELECTOR1_ADDR                                           (0xD024u)
#define HOST_IF_OUTPUT_SELECTOR1_NW_ADDR                                        (0xD024u)
#define HOST_IF_OUTPUT_SELECTOR1_RESET_VAL                                  (0x3F3F3F3Fu)
#define HOST_IF_OUTPUT_SELECTOR1_MMS                                                 (0u)
#define HOST_IF_OUTPUT_SELECTOR1_VOL_MASK                                   (0x3F3F3F3Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 05-00 | io4_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO4 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-08 | io5_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO5 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21-16 | io6_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO6 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-24 | io7_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO7 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: io4_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR                            (0u)
#define BITL_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR                   (0x0000003Fu)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO4_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io5_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR                            (8u)
#define BITL_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR                   (0x00003F00u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO5_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io6_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR                           (16u)
#define BITL_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR                   (0x003F0000u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO6_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io7_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR                           (24u)
#define BITL_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR                   (0x3F000000u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR1_IO7_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : output_selector2
// Register Addr        : 0xD028u
// Register NW Addr     : 0xD028u
// Register Reset Value : 0x3F3F3F3Fu
// Register MMS         : 0u

#define HOST_IF_OUTPUT_SELECTOR2_ADDR                                           (0xD028u)
#define HOST_IF_OUTPUT_SELECTOR2_NW_ADDR                                        (0xD028u)
#define HOST_IF_OUTPUT_SELECTOR2_RESET_VAL                                  (0x3F3F3F3Fu)
#define HOST_IF_OUTPUT_SELECTOR2_MMS                                                 (0u)
#define HOST_IF_OUTPUT_SELECTOR2_VOL_MASK                                   (0x3F3F3F3Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 05-00 | io8_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO8 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-08 | io9_output_selector      | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO9 output port     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21-16 | io10_output_selector     | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO10 output port    |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-24 | io11_output_selector     | Selects which interface pin is    | 0x0000003f  |
// |       |                          | routed to the IO11 output port    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: io8_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR                            (0u)
#define BITL_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR                   (0x0000003Fu)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO8_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io9_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR                            (8u)
#define BITL_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR                            (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR                   (0x00003F00u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_I2C_SCLK          (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_I2C_SDATA         (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB0         (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB1         (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB2         (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB3         (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB4         (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB5         (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB6         (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_CSB7         (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_MOSI         (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_SPI0_SCLK         (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_ISELED0_SIOP      (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_ISELED1_SIOP      (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_ISELED2_SIOP      (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_ISELED3_SIOP      (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_LIN0_OUT          (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_LIN0_EN           (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_LIN1_OUT          (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_LIN1_EN           (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_UART_TX           (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_UART_RTS          (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT0         (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT1         (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT2         (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT3         (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT4         (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT5         (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT6         (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO0_OUT7         (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT0         (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT1         (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT2         (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT3         (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT4         (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT5         (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT6         (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO9_OUTPUT_SELECTOR_FIO1_OUT7         (0x00000031u)

// Bit field: io10_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR                          (16u)
#define BITL_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR                           (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR                  (0x003F0000u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_I2C_SCLK         (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_I2C_SDATA        (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB0        (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB1        (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB2        (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB3        (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB4        (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB5        (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB6        (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_CSB7        (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_MOSI        (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_SPI0_SCLK        (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_ISELED0_SIOP     (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_ISELED1_SIOP     (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_ISELED2_SIOP     (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_ISELED3_SIOP     (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_LIN0_OUT         (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_LIN0_EN          (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_LIN1_OUT         (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_LIN1_EN          (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_UART_TX          (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_UART_RTS         (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT0        (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT1        (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT2        (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT3        (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT4        (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT5        (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT6        (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO0_OUT7        (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT0        (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT1        (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT2        (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT3        (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT4        (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT5        (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT6        (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO10_OUTPUT_SELECTOR_FIO1_OUT7        (0x00000031u)

// Bit field: io11_output_selector
// Bit position, length and mask
#define BITP_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR                          (24u)
#define BITL_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR                           (6u)
#define BITM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR                  (0x3F000000u)
// Enums
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_I2C_SCLK         (0x00000000u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_I2C_SDATA        (0x00000001u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB0        (0x00000002u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB1        (0x00000003u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB2        (0x00000004u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB3        (0x00000005u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB4        (0x00000006u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB5        (0x00000007u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB6        (0x00000008u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_CSB7        (0x00000009u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_MOSI        (0x0000000Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_SPI0_SCLK        (0x0000000Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_ISELED0_SIOP     (0x0000000Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_ISELED1_SIOP     (0x0000000Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_ISELED2_SIOP     (0x0000000Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_ISELED3_SIOP     (0x0000000Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT0 (0x00000010u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT1 (0x00000011u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT2 (0x00000012u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT3 (0x00000013u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT4 (0x00000014u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT5 (0x00000015u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT6 (0x00000016u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT7 (0x00000017u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT8 (0x00000018u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT9 (0x00000019u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT10 (0x0000001Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_PORT_CONTROLLER_OUT11 (0x0000001Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_LIN0_OUT         (0x0000001Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_LIN0_EN          (0x0000001Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_LIN1_OUT         (0x0000001Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_LIN1_EN          (0x0000001Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_UART_TX          (0x00000020u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_UART_RTS         (0x00000021u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT0        (0x00000022u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT1        (0x00000023u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT2        (0x00000024u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT3        (0x00000025u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT4        (0x00000026u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT5        (0x00000027u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT6        (0x00000028u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO0_OUT7        (0x00000029u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT0        (0x0000002Au)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT1        (0x0000002Bu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT2        (0x0000002Cu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT3        (0x0000002Du)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT4        (0x0000002Eu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT5        (0x0000002Fu)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT6        (0x00000030u)
#define ENUM_HOST_IF_OUTPUT_SELECTOR2_IO11_OUTPUT_SELECTOR_FIO1_OUT7        (0x00000031u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : sa_if_bus_configuration
// Register Addr        : 0xD02Cu
// Register NW Addr     : 0xD02Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define HOST_IF_SA_IF_BUS_CONFIGURATION_ADDR                                    (0xD02Cu)
#define HOST_IF_SA_IF_BUS_CONFIGURATION_NW_ADDR                                 (0xD02Cu)
#define HOST_IF_SA_IF_BUS_CONFIGURATION_RESET_VAL                           (0x00000000u)
#define HOST_IF_SA_IF_BUS_CONFIGURATION_MMS                                          (0u)
#define HOST_IF_SA_IF_BUS_CONFIGURATION_VOL_MASK                            (0x00000FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | sa_if_input_enable       | Configures the input enable       | 0x00000000  |
// |       |                          | value of the sa_if input bus      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: sa_if_input_enable
// Bit position, length and mask
#define BITP_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE                      (0u)
#define BITL_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE                     (12u)
#define BITM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE             (0x00000FFFu)
// Enums
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF0      (0x00000000u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF1      (0x00000001u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF2      (0x00000002u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF3      (0x00000003u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF4      (0x00000004u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF5      (0x00000005u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF6      (0x00000006u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF7      (0x00000007u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF8      (0x00000008u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF9      (0x00000009u)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF10     (0x0000000Au)
#define ENUM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE_SA_IF11     (0x0000000Bu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : debouncer
// Register Addr        : 0xD030u
// Register NW Addr     : 0xD030u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define HOST_IF_DEBOUNCER_ADDR                                                  (0xD030u)
#define HOST_IF_DEBOUNCER_NW_ADDR                                               (0xD030u)
#define HOST_IF_DEBOUNCER_RESET_VAL                                         (0x00000000u)
#define HOST_IF_DEBOUNCER_MMS                                                        (0u)
#define HOST_IF_DEBOUNCER_VOL_MASK                                          (0x00FF0FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | debouncer_enable         | Enables the debouncer on the      | 0x00000000  |
// |       |                          | input ports to the host           |             |
// |       |                          | interface                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-16 | debouncer_period         | Changes the time constant of the  | 0x00000000  |
// |       |                          | debouncer used on the input       |             |
// |       |                          | ports. The debouncer stable time  |             |
// |       |                          | to update is 15 * the value       |             |
// |       |                          | programmed in this field plus 1.  |             |
// |       |                          | The unit is 10 ns.                |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: debouncer_enable
// Bit position, length and mask
#define BITP_HOST_IF_DEBOUNCER_DEBOUNCER_ENABLE                                      (0u)
#define BITL_HOST_IF_DEBOUNCER_DEBOUNCER_ENABLE                                     (12u)
#define BITM_HOST_IF_DEBOUNCER_DEBOUNCER_ENABLE                             (0x00000FFFu)
// Enums
#define ENUM_HOST_IF_DEBOUNCER_DEBOUNCER_ENABLE_ENABLE                      (0x00000001u)
#define ENUM_HOST_IF_DEBOUNCER_DEBOUNCER_ENABLE_DISABLE                     (0x00000000u)

// Bit field: debouncer_period
// Bit position, length and mask
#define BITP_HOST_IF_DEBOUNCER_DEBOUNCER_PERIOD                                     (16u)
#define BITL_HOST_IF_DEBOUNCER_DEBOUNCER_PERIOD                                      (8u)
#define BITM_HOST_IF_DEBOUNCER_DEBOUNCER_PERIOD                             (0x00FF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : io_override
// Register Addr        : 0xD034u
// Register NW Addr     : 0xD034u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define HOST_IF_IO_OVERRIDE_ADDR                                                (0xD034u)
#define HOST_IF_IO_OVERRIDE_NW_ADDR                                             (0xD034u)
#define HOST_IF_IO_OVERRIDE_RESET_VAL                                       (0x00000000u)
#define HOST_IF_IO_OVERRIDE_MMS                                                      (0u)
#define HOST_IF_IO_OVERRIDE_VOL_MASK                                        (0x0FFF0FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | io_override_man          | Sets the manual value of the IO   | 0x00000000  |
// |       |                          | bus, has preference over          |             |
// |       |                          | interfaces                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-16 | io_override_man_en       | Enables the manual override of    | 0x00000000  |
// |       |                          | the IO bus, has preference over   |             |
// |       |                          | interfaces                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: io_override_man
// Bit position, length and mask
#define BITP_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN                                     (0u)
#define BITL_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN                                    (12u)
#define BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN                            (0x00000FFFu)

// Bit field: io_override_man_en
// Bit position, length and mask
#define BITP_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN_EN                                 (16u)
#define BITL_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN_EN                                 (12u)
#define BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN_EN                         (0x0FFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : external_trigger_selector1
// Register Addr        : 0xD040u
// Register NW Addr     : 0xD040u
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 0u

#define HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_ADDR                                 (0xD040u)
#define HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_NW_ADDR                              (0xD040u)
#define HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_RESET_VAL                        (0x0000FFFFu)
#define HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_MMS                                       (0u)
#define HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_VOL_MASK                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-08 | external_trigger_input_se| Selects the gpio port connected   | 0x0000000f  | 
// |       |                          | to the FIO0 external trigger.     |             |
// |       |                          | Use 0xF for unused.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | external_trigger_input_se| Selects the gpio port connected   | 0x0000000f  | 
// |       |                          | to the FIO1 external trigger.     |             |
// |       |                          | Use 0xF for unused.               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: external_trigger_input_selector_10
// Bit position, length and mask
#define BITP_HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_EXTERNAL_TRIGGER_INPUT_SELECTOR_10   (8u)
#define BITL_HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_EXTERNAL_TRIGGER_INPUT_SELECTOR_10   (4u)
#define BITM_HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_EXTERNAL_TRIGGER_INPUT_SELECTOR_10 (0x00000F00u)

// Bit field: external_trigger_input_selector_11
// Bit position, length and mask
#define BITP_HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_EXTERNAL_TRIGGER_INPUT_SELECTOR_11  (12u)
#define BITL_HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_EXTERNAL_TRIGGER_INPUT_SELECTOR_11   (4u)
#define BITM_HOST_IF_EXTERNAL_TRIGGER_SELECTOR1_EXTERNAL_TRIGGER_INPUT_SELECTOR_11 (0x0000F000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillHOST_IFRegsData();
#endif


#endif /* ADI_E2BCORE_REGHOST_H */

/** @} */

/*
 * EOF: www.analog.com
 */


