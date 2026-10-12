/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_e2bcore_reglce.h
 * @brief        : This is an auto generated header file with register
                   descriptions for LCE
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_REGLCE_H
#define ADI_E2BCORE_REGLCE_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define LCE_NUM_REGS    (27u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganLCERegAddr[LCE_NUM_REGS];
extern uint32_t ganLCENwRegAddr[LCE_NUM_REGS];
extern uint32_t ganLCERegMMS[LCE_NUM_REGS];
extern uint32_t ganLCERegResetVal[LCE_NUM_REGS];
extern const char *gasLCERegName[LCE_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RESET
// Register Addr        : 0x0000u
// Register NW Addr     : 0x0000u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_RESET_ADDR                                                          (0x0000u)
#define LCE_RESET_NW_ADDR                                                       (0x0000u)
#define LCE_RESET_RESET_VAL                                                 (0x00000000u)
#define LCE_RESET_MMS                                                                (0u)
#define LCE_RESET_VOL_MASK                                                  (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | reset                    | Initiate a reset of the low       | 0x00000000  |
// |       |                          | complexity engine. The reset      |             |
// |       |                          | value should be set to the        |             |
// |       |                          | 32-bit hexadecimal value          |             |
// |       |                          | '0xAA55A55A'.                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: reset
// Bit position, length and mask
#define BITP_LCE_RESET_RESET                                                         (0u)
#define BITL_LCE_RESET_RESET                                                        (32u)
#define BITM_LCE_RESET_RESET                                                (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DEVICE_MAC_ADDRESS_LOWER
// Register Addr        : 0x0024u
// Register NW Addr     : 0x0024u
// Register Reset Value : 0x22FE7000u
// Register MMS         : 0u

#define LCE_DEVICE_MAC_ADDRESS_LOWER_ADDR                                       (0x0024u)
#define LCE_DEVICE_MAC_ADDRESS_LOWER_NW_ADDR                                    (0x0024u)
#define LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL                              (0x22FE7000u)
#define LCE_DEVICE_MAC_ADDRESS_LOWER_MMS                                             (0u)
#define LCE_DEVICE_MAC_ADDRESS_LOWER_VOL_MASK                               (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | DEVICE_MAC_ADDRESS_LOWER | Set upper half of custom unicast  | 0x22fe7000  |
// |       |                          | MAC address for filtering and     |             |
// |       |                          | source addressing (bits 31 to     |             |
// |       |                          | 0)..                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: DEVICE_MAC_ADDRESS_LOWER
// Bit position, length and mask
#define BITP_LCE_DEVICE_MAC_ADDRESS_LOWER_DEVICE_MAC_ADDRESS_LOWER                   (0u)
#define BITL_LCE_DEVICE_MAC_ADDRESS_LOWER_DEVICE_MAC_ADDRESS_LOWER                  (32u)
#define BITM_LCE_DEVICE_MAC_ADDRESS_LOWER_DEVICE_MAC_ADDRESS_LOWER          (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DEVICE_MULTICAST_ADDRESS_UPPER
// Register Addr        : 0x0028u
// Register NW Addr     : 0x0028u
// Register Reset Value : 0x01E000E0u
// Register MMS         : 0u

#define LCE_DEVICE_MULTICAST_ADDRESS_UPPER_ADDR                                 (0x0028u)
#define LCE_DEVICE_MULTICAST_ADDRESS_UPPER_NW_ADDR                              (0x0028u)
#define LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL                        (0x01E000E0u)
#define LCE_DEVICE_MULTICAST_ADDRESS_UPPER_MMS                                       (0u)
#define LCE_DEVICE_MULTICAST_ADDRESS_UPPER_VOL_MASK                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | DEVICE_MAC_ADDRESS_UPPER | Set upper half of custom unicast  | 0x000000e0  |
// |       |                          | MAC address for filtering and     |             |
// |       |                          | source addressing (bits 47 to     |             |
// |       |                          | 32).                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | MULTICAST_MAC_ADDRESS_UPP| Configure upper half of primary   | 0x000001e0  | 
// |       |                          | multicast MAC address range for   |             |
// |       |                          | filtering (bits 47 to 32).        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: DEVICE_MAC_ADDRESS_UPPER
// Bit position, length and mask
#define BITP_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER             (0u)
#define BITL_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER            (16u)
#define BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER    (0x0000FFFFu)

// Bit field: MULTICAST_MAC_ADDRESS_UPPER
// Bit position, length and mask
#define BITP_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_MULTICAST_MAC_ADDRESS_UPPER         (16u)
#define BITL_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_MULTICAST_MAC_ADDRESS_UPPER         (16u)
#define BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_MULTICAST_MAC_ADDRESS_UPPER (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MULTICAST_MAC_ADDRESS_LOWER
// Register Addr        : 0x002Cu
// Register NW Addr     : 0x002Cu
// Register Reset Value : 0x22FE7000u
// Register MMS         : 0u

#define LCE_MULTICAST_MAC_ADDRESS_LOWER_ADDR                                    (0x002Cu)
#define LCE_MULTICAST_MAC_ADDRESS_LOWER_NW_ADDR                                 (0x002Cu)
#define LCE_MULTICAST_MAC_ADDRESS_LOWER_RESET_VAL                           (0x22FE7000u)
#define LCE_MULTICAST_MAC_ADDRESS_LOWER_MMS                                          (0u)
#define LCE_MULTICAST_MAC_ADDRESS_LOWER_VOL_MASK                            (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MULTICAST_MAC_ADDRESS_LOW| Configure lower half of primary   | 0x22fe7000  | 
// |       |                          | multicast MAC address range for   |             |
// |       |                          | filtering (bits 31 to 0).         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MULTICAST_MAC_ADDRESS_LOWER
// Bit position, length and mask
#define BITP_LCE_MULTICAST_MAC_ADDRESS_LOWER_MULTICAST_MAC_ADDRESS_LOWER             (0u)
#define BITL_LCE_MULTICAST_MAC_ADDRESS_LOWER_MULTICAST_MAC_ADDRESS_LOWER            (32u)
#define BITM_LCE_MULTICAST_MAC_ADDRESS_LOWER_MULTICAST_MAC_ADDRESS_LOWER    (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MULTICAST_ALT_MAC_ADDRESS_UPPER
// Register Addr        : 0x0030u
// Register NW Addr     : 0x0030u
// Register Reset Value : 0x000001E0u
// Register MMS         : 0u

#define LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_ADDR                                (0x0030u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_NW_ADDR                             (0x0030u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_RESET_VAL                       (0x000001E0u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_MMS                                      (0u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_VOL_MASK                        (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MULTICAST_ALT_MAC_ADDRESS| Configure upper half of           | 0x000001e0  | 
// |       |                          | alternate multicast MAC address   |             |
// |       |                          | range for filtering (bits 47 to   |             |
// |       |                          | 32).                              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MULTICAST_ALT_MAC_ADDRESS_UPPER
// Bit position, length and mask
#define BITP_LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_MULTICAST_ALT_MAC_ADDRESS_UPPER     (0u)
#define BITL_LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_MULTICAST_ALT_MAC_ADDRESS_UPPER    (16u)
#define BITM_LCE_MULTICAST_ALT_MAC_ADDRESS_UPPER_MULTICAST_ALT_MAC_ADDRESS_UPPER (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MULTICAST_ALT_MAC_ADDRESS_LOWER
// Register Addr        : 0x0034u
// Register NW Addr     : 0x0034u
// Register Reset Value : 0x22FE7100u
// Register MMS         : 0u

#define LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_ADDR                                (0x0034u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_NW_ADDR                             (0x0034u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_RESET_VAL                       (0x22FE7100u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_MMS                                      (0u)
#define LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_VOL_MASK                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MULTICAST_ALT_MAC_ADDRESS| Configure lower half of           | 0x22fe7100  | 
// |       |                          | alternate multicast MAC address   |             |
// |       |                          | range for filtering (bits 31 to   |             |
// |       |                          | 0).                               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MULTICAST_ALT_MAC_ADDRESS_LOWER
// Bit position, length and mask
#define BITP_LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_MULTICAST_ALT_MAC_ADDRESS_LOWER     (0u)
#define BITL_LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_MULTICAST_ALT_MAC_ADDRESS_LOWER    (32u)
#define BITM_LCE_MULTICAST_ALT_MAC_ADDRESS_LOWER_MULTICAST_ALT_MAC_ADDRESS_LOWER (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MEM_MACRO_TABLE0
// Register Addr        : 0x0038u
// Register NW Addr     : 0x0038u
// Register Reset Value : 0x08000800u
// Register MMS         : 0u

#define LCE_MEM_MACRO_TABLE0_ADDR                                               (0x0038u)
#define LCE_MEM_MACRO_TABLE0_NW_ADDR                                            (0x0038u)
#define LCE_MEM_MACRO_TABLE0_RESET_VAL                                      (0x08000800u)
#define LCE_MEM_MACRO_TABLE0_MMS                                                     (0u)
#define LCE_MEM_MACRO_TABLE0_VOL_MASK                                       (0x3FFF3FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | tx_fifo_size_0           | Sets the transmit buffer size     | 0x00000800  |
// |       |                          | for channel 0, in bytes.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-16 | rx_fifo_size_0           | Sets the receive buffer size for  | 0x00000800  |
// |       |                          | channel 0, in bytes.              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tx_fifo_size_0
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE0_TX_FIFO_SIZE_0                                     (0u)
#define BITL_LCE_MEM_MACRO_TABLE0_TX_FIFO_SIZE_0                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE0_TX_FIFO_SIZE_0                            (0x00003FFFu)

// Bit field: rx_fifo_size_0
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE0_RX_FIFO_SIZE_0                                    (16u)
#define BITL_LCE_MEM_MACRO_TABLE0_RX_FIFO_SIZE_0                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE0_RX_FIFO_SIZE_0                            (0x3FFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MEM_MACRO_TABLE1
// Register Addr        : 0x003Cu
// Register NW Addr     : 0x003Cu
// Register Reset Value : 0x08000800u
// Register MMS         : 0u

#define LCE_MEM_MACRO_TABLE1_ADDR                                               (0x003Cu)
#define LCE_MEM_MACRO_TABLE1_NW_ADDR                                            (0x003Cu)
#define LCE_MEM_MACRO_TABLE1_RESET_VAL                                      (0x08000800u)
#define LCE_MEM_MACRO_TABLE1_MMS                                                     (0u)
#define LCE_MEM_MACRO_TABLE1_VOL_MASK                                       (0x3FFF3FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | tx_fifo_size_1           | Sets the transmit buffer size     | 0x00000800  |
// |       |                          | for channel 1, in bytes.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-16 | rx_fifo_size_1           | Sets the receive buffer size for  | 0x00000800  |
// |       |                          | channel 1, in byes.               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tx_fifo_size_1
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE1_TX_FIFO_SIZE_1                                     (0u)
#define BITL_LCE_MEM_MACRO_TABLE1_TX_FIFO_SIZE_1                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE1_TX_FIFO_SIZE_1                            (0x00003FFFu)

// Bit field: rx_fifo_size_1
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE1_RX_FIFO_SIZE_1                                    (16u)
#define BITL_LCE_MEM_MACRO_TABLE1_RX_FIFO_SIZE_1                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE1_RX_FIFO_SIZE_1                            (0x3FFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MEM_MACRO_TABLE2
// Register Addr        : 0x0040u
// Register NW Addr     : 0x0040u
// Register Reset Value : 0x08000800u
// Register MMS         : 0u

#define LCE_MEM_MACRO_TABLE2_ADDR                                               (0x0040u)
#define LCE_MEM_MACRO_TABLE2_NW_ADDR                                            (0x0040u)
#define LCE_MEM_MACRO_TABLE2_RESET_VAL                                      (0x08000800u)
#define LCE_MEM_MACRO_TABLE2_MMS                                                     (0u)
#define LCE_MEM_MACRO_TABLE2_VOL_MASK                                       (0x3FFF3FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | tx_fifo_size_2           | Sets the transmit buffer size     | 0x00000800  |
// |       |                          | for channel 2, in bytes.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-16 | rx_fifo_size_2           | Sets the receive buffer size for  | 0x00000800  |
// |       |                          | channel 2, in bytes.              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tx_fifo_size_2
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE2_TX_FIFO_SIZE_2                                     (0u)
#define BITL_LCE_MEM_MACRO_TABLE2_TX_FIFO_SIZE_2                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE2_TX_FIFO_SIZE_2                            (0x00003FFFu)

// Bit field: rx_fifo_size_2
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE2_RX_FIFO_SIZE_2                                    (16u)
#define BITL_LCE_MEM_MACRO_TABLE2_RX_FIFO_SIZE_2                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE2_RX_FIFO_SIZE_2                            (0x3FFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MEM_MACRO_TABLE3
// Register Addr        : 0x0044u
// Register NW Addr     : 0x0044u
// Register Reset Value : 0x05000800u
// Register MMS         : 0u

#define LCE_MEM_MACRO_TABLE3_ADDR                                               (0x0044u)
#define LCE_MEM_MACRO_TABLE3_NW_ADDR                                            (0x0044u)
#define LCE_MEM_MACRO_TABLE3_RESET_VAL                                      (0x05000800u)
#define LCE_MEM_MACRO_TABLE3_MMS                                                     (0u)
#define LCE_MEM_MACRO_TABLE3_VOL_MASK                                       (0x3FFF3FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | tx_fifo_size_3           | Sets the transmit buffer size     | 0x00000800  |
// |       |                          | for channel 3, in bytes.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29-16 | rx_fifo_size_3           | Sets the receive buffer size for  | 0x00000500  |
// |       |                          | channel 3, in bytes.              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: tx_fifo_size_3
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE3_TX_FIFO_SIZE_3                                     (0u)
#define BITL_LCE_MEM_MACRO_TABLE3_TX_FIFO_SIZE_3                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE3_TX_FIFO_SIZE_3                            (0x00003FFFu)

// Bit field: rx_fifo_size_3
// Bit position, length and mask
#define BITP_LCE_MEM_MACRO_TABLE3_RX_FIFO_SIZE_3                                    (16u)
#define BITL_LCE_MEM_MACRO_TABLE3_RX_FIFO_SIZE_3                                    (14u)
#define BITM_LCE_MEM_MACRO_TABLE3_RX_FIFO_SIZE_3                            (0x3FFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CFG
// Register Addr        : 0x004Cu
// Register NW Addr     : 0x004Cu
// Register Reset Value : 0x000032AAu
// Register MMS         : 0u

#define LCE_CFG_ADDR                                                            (0x004Cu)
#define LCE_CFG_NW_ADDR                                                         (0x004Cu)
#define LCE_CFG_RESET_VAL                                                   (0x000032AAu)
#define LCE_CFG_MMS                                                                  (0u)
#define LCE_CFG_VOL_MASK                                                    (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | fifo_priority_scheduling | Establishes the priority mode     | 0x00000000  |
// |       |                          | operation for the message         |             |
// |       |                          | scheduler, allowing for either a  |             |
// |       |                          | fixed priority determined by the  |             |
// |       |                          | FIFO index or a balanced          |             |
// |       |                          | distribution through a            |             |
// |       |                          | round-robin approach.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | external_mac_device_addre| Enable or disable external GPIO   | 0x00000000  | 
// |       |                          | configuration for unicast         |             |
// |       |                          | address least significant bits.   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 04-03 | port_target_acknowledge_c| Enables the selection of either   | 0x00000001  | 
// |       |                          | OASPI port, T1S port, or both,    |             |
// |       |                          | for transmitting acknowledges to  |             |
// |       |                          | the ECU.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 06-05 | port_target_fifo_rejects_| Enables the selection of either   | 0x00000001  | 
// |       |                          | OASPI port, T1S port, or both,    |             |
// |       |                          | for transmitting fifo rejects     |             |
// |       |                          | messages to the ECU.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 08-07 | port_target_interrupt_cfg| Enables the selection of either   | 0x00000001  | 
// |       |                          | OASPI port, T1S port, or both,    |             |
// |       |                          | for transmitting interrupt        |             |
// |       |                          | messages to the ECU.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10-09 | port_target_regmap_cfg   | Enables the selection of either   | 0x00000001  |
// |       |                          | OASPI port, T1S port, or both,    |             |
// |       |                          | for transmitting regmap readback  |             |
// |       |                          | data to the ECU.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-14 | port_target_smc_cfg      | Enables the selection of either   | 0x00000000  |
// |       |                          | OASPI port, T1S port, or both,    |             |
// |       |                          | for transmitting SMC messages to  |             |
// |       |                          | the ECU.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: fifo_priority_scheduling
// Bit position, length and mask
#define BITP_LCE_CFG_FIFO_PRIORITY_SCHEDULING                                        (0u)
#define BITL_LCE_CFG_FIFO_PRIORITY_SCHEDULING                                        (1u)
#define BITM_LCE_CFG_FIFO_PRIORITY_SCHEDULING                               (0x00000001u)
// Enums
#define ENUM_LCE_CFG_FIFO_PRIORITY_SCHEDULING_ROUND_ROBIN                   (0x00000000u)
#define ENUM_LCE_CFG_FIFO_PRIORITY_SCHEDULING_FIXED_PRIORITY                (0x00000001u)

// Bit field: external_mac_device_address_lsb_set
// Bit position, length and mask
#define BITP_LCE_CFG_EXTERNAL_MAC_DEVICE_ADDRESS_LSB_SET                             (2u)
#define BITL_LCE_CFG_EXTERNAL_MAC_DEVICE_ADDRESS_LSB_SET                             (1u)
#define BITM_LCE_CFG_EXTERNAL_MAC_DEVICE_ADDRESS_LSB_SET                    (0x00000004u)

// Bit field: port_target_acknowledge_cfg
// Bit position, length and mask
#define BITP_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG                                     (3u)
#define BITL_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG                                     (2u)
#define BITM_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG                            (0x00000018u)
// Enums
#define ENUM_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG_NONE                       (0x00000000u)
#define ENUM_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG_T1S                        (0x00000001u)
#define ENUM_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG_OASPI                      (0x00000002u)
#define ENUM_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG_T1S_AND_OASPI              (0x00000003u)

// Bit field: port_target_fifo_rejects_cfg
// Bit position, length and mask
#define BITP_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG                                    (5u)
#define BITL_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG                                    (2u)
#define BITM_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG                           (0x00000060u)
// Enums
#define ENUM_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG_NONE                      (0x00000000u)
#define ENUM_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG_T1S                       (0x00000001u)
#define ENUM_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG_OASPI                     (0x00000002u)
#define ENUM_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG_T1S_AND_OASPI             (0x00000003u)

// Bit field: port_target_interrupt_cfg
// Bit position, length and mask
#define BITP_LCE_CFG_PORT_TARGET_INTERRUPT_CFG                                       (7u)
#define BITL_LCE_CFG_PORT_TARGET_INTERRUPT_CFG                                       (2u)
#define BITM_LCE_CFG_PORT_TARGET_INTERRUPT_CFG                              (0x00000180u)
// Enums
#define ENUM_LCE_CFG_PORT_TARGET_INTERRUPT_CFG_NONE                         (0x00000000u)
#define ENUM_LCE_CFG_PORT_TARGET_INTERRUPT_CFG_T1S                          (0x00000001u)
#define ENUM_LCE_CFG_PORT_TARGET_INTERRUPT_CFG_OASPI                        (0x00000002u)
#define ENUM_LCE_CFG_PORT_TARGET_INTERRUPT_CFG_T1S_AND_OASPI                (0x00000003u)

// Bit field: port_target_regmap_cfg
// Bit position, length and mask
#define BITP_LCE_CFG_PORT_TARGET_REGMAP_CFG                                          (9u)
#define BITL_LCE_CFG_PORT_TARGET_REGMAP_CFG                                          (2u)
#define BITM_LCE_CFG_PORT_TARGET_REGMAP_CFG                                 (0x00000600u)
// Enums
#define ENUM_LCE_CFG_PORT_TARGET_REGMAP_CFG_NONE                            (0x00000000u)
#define ENUM_LCE_CFG_PORT_TARGET_REGMAP_CFG_T1S                             (0x00000001u)
#define ENUM_LCE_CFG_PORT_TARGET_REGMAP_CFG_OASPI                           (0x00000002u)
#define ENUM_LCE_CFG_PORT_TARGET_REGMAP_CFG_T1S_AND_OASPI                   (0x00000003u)

// Bit field: port_target_smc_cfg
// Bit position, length and mask
#define BITP_LCE_CFG_PORT_TARGET_SMC_CFG                                            (14u)
#define BITL_LCE_CFG_PORT_TARGET_SMC_CFG                                             (2u)
#define BITM_LCE_CFG_PORT_TARGET_SMC_CFG                                    (0x0000C000u)
// Enums
#define ENUM_LCE_CFG_PORT_TARGET_SMC_CFG_NONE                               (0x00000000u)
#define ENUM_LCE_CFG_PORT_TARGET_SMC_CFG_T1S                                (0x00000001u)
#define ENUM_LCE_CFG_PORT_TARGET_SMC_CFG_OASPI                              (0x00000002u)
#define ENUM_LCE_CFG_PORT_TARGET_SMC_CFG_T1S_AND_OASPI                      (0x00000003u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : UPSTREAM_AVB_STREAM_ID_UPPER
// Register Addr        : 0x0068u
// Register NW Addr     : 0x0068u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_UPSTREAM_AVB_STREAM_ID_UPPER_ADDR                                   (0x0068u)
#define LCE_UPSTREAM_AVB_STREAM_ID_UPPER_NW_ADDR                                (0x0068u)
#define LCE_UPSTREAM_AVB_STREAM_ID_UPPER_RESET_VAL                          (0x00000000u)
#define LCE_UPSTREAM_AVB_STREAM_ID_UPPER_MMS                                         (0u)
#define LCE_UPSTREAM_AVB_STREAM_ID_UPPER_VOL_MASK                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | avb_upstream_stream_id_up| Sets the upper part of the        | 0x00000000  | 
// |       |                          | desired stream ID for custom AVB  |             |
// |       |                          | frames.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: avb_upstream_stream_id_upper
// Bit position, length and mask
#define BITP_LCE_UPSTREAM_AVB_STREAM_ID_UPPER_AVB_UPSTREAM_STREAM_ID_UPPER           (0u)
#define BITL_LCE_UPSTREAM_AVB_STREAM_ID_UPPER_AVB_UPSTREAM_STREAM_ID_UPPER          (32u)
#define BITM_LCE_UPSTREAM_AVB_STREAM_ID_UPPER_AVB_UPSTREAM_STREAM_ID_UPPER  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : UPSTREAM_AVB_STREAM_ID_LOWER
// Register Addr        : 0x006Cu
// Register NW Addr     : 0x006Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_UPSTREAM_AVB_STREAM_ID_LOWER_ADDR                                   (0x006Cu)
#define LCE_UPSTREAM_AVB_STREAM_ID_LOWER_NW_ADDR                                (0x006Cu)
#define LCE_UPSTREAM_AVB_STREAM_ID_LOWER_RESET_VAL                          (0x00000000u)
#define LCE_UPSTREAM_AVB_STREAM_ID_LOWER_MMS                                         (0u)
#define LCE_UPSTREAM_AVB_STREAM_ID_LOWER_VOL_MASK                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | avb_upstream_stream_id_lo| Sets the lower part of the        | 0x00000000  | 
// |       |                          | desired stream ID for custom AVB  |             |
// |       |                          | frames.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: avb_upstream_stream_id_lower
// Bit position, length and mask
#define BITP_LCE_UPSTREAM_AVB_STREAM_ID_LOWER_AVB_UPSTREAM_STREAM_ID_LOWER           (0u)
#define BITL_LCE_UPSTREAM_AVB_STREAM_ID_LOWER_AVB_UPSTREAM_STREAM_ID_LOWER          (32u)
#define BITM_LCE_UPSTREAM_AVB_STREAM_ID_LOWER_AVB_UPSTREAM_STREAM_ID_LOWER  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : GPTP_VLAN_CFG
// Register Addr        : 0x0074u
// Register NW Addr     : 0x0074u
// Register Reset Value : 0x40000000u
// Register MMS         : 0u

#define LCE_GPTP_VLAN_CFG_ADDR                                                  (0x0074u)
#define LCE_GPTP_VLAN_CFG_NW_ADDR                                               (0x0074u)
#define LCE_GPTP_VLAN_CFG_RESET_VAL                                         (0x40000000u)
#define LCE_GPTP_VLAN_CFG_MMS                                                        (0u)
#define LCE_GPTP_VLAN_CFG_VOL_MASK                                          (0xC0000FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | gptp_vlan_id             | Sets a specific VLAN ID for       | 0x00000000  |
// |       |                          | processing tagged 802.1as         |             |
// |       |                          | frames.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-30 | gptp_untagged_vlan_mode  | Configures processing of          | 0x00000001  |
// |       |                          | untagged 802.1as frames.          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: gptp_vlan_id
// Bit position, length and mask
#define BITP_LCE_GPTP_VLAN_CFG_GPTP_VLAN_ID                                          (0u)
#define BITL_LCE_GPTP_VLAN_CFG_GPTP_VLAN_ID                                         (12u)
#define BITM_LCE_GPTP_VLAN_CFG_GPTP_VLAN_ID                                 (0x00000FFFu)

// Bit field: gptp_untagged_vlan_mode
// Bit position, length and mask
#define BITP_LCE_GPTP_VLAN_CFG_GPTP_UNTAGGED_VLAN_MODE                              (30u)
#define BITL_LCE_GPTP_VLAN_CFG_GPTP_UNTAGGED_VLAN_MODE                               (2u)
#define BITM_LCE_GPTP_VLAN_CFG_GPTP_UNTAGGED_VLAN_MODE                      (0xC0000000u)
// Enums
#define ENUM_LCE_GPTP_VLAN_CFG_GPTP_UNTAGGED_VLAN_MODE_DISCARD              (0x00000000u)
#define ENUM_LCE_GPTP_VLAN_CFG_GPTP_UNTAGGED_VLAN_MODE_PROCESS              (0x00000001u)
#define ENUM_LCE_GPTP_VLAN_CFG_GPTP_UNTAGGED_VLAN_MODE_IGNORE               (0x00000002u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DECODER_VLAN_CFG
// Register Addr        : 0x0078u
// Register NW Addr     : 0x0078u
// Register Reset Value : 0x40000000u
// Register MMS         : 0u

#define LCE_DECODER_VLAN_CFG_ADDR                                               (0x0078u)
#define LCE_DECODER_VLAN_CFG_NW_ADDR                                            (0x0078u)
#define LCE_DECODER_VLAN_CFG_RESET_VAL                                      (0x40000000u)
#define LCE_DECODER_VLAN_CFG_MMS                                                     (0u)
#define LCE_DECODER_VLAN_CFG_VOL_MASK                                       (0xC0FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | decoder_vlan_id_0        | Set first VLAN identifier for     | 0x00000000  |
// |       |                          | filtering and processing.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | decoder_vlan_id_1        | Set second VLAN identifier for    | 0x00000000  |
// |       |                          | filtering and processing.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-30 | decoder_untagged_vlan_mod| Configure processing mode for     | 0x00000001  | 
// |       |                          | untagged frames (PROCESS,         |             |
// |       |                          | DISCARD, IGNORE).                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: decoder_vlan_id_0
// Bit position, length and mask
#define BITP_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_0                                  (0u)
#define BITL_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_0                                 (12u)
#define BITM_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_0                         (0x00000FFFu)

// Bit field: decoder_vlan_id_1
// Bit position, length and mask
#define BITP_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_1                                 (12u)
#define BITL_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_1                                 (12u)
#define BITM_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_1                         (0x00FFF000u)

// Bit field: decoder_untagged_vlan_mode
// Bit position, length and mask
#define BITP_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE                        (30u)
#define BITL_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE                         (2u)
#define BITM_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE                (0xC0000000u)
// Enums
#define ENUM_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE_DISCARD        (0x00000000u)
#define ENUM_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE_PROCESS        (0x00000001u)
#define ENUM_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE_IGNORE         (0x00000002u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DECODER_VLAN_CFG_2
// Register Addr        : 0x007Cu
// Register NW Addr     : 0x007Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_DECODER_VLAN_CFG_2_ADDR                                             (0x007Cu)
#define LCE_DECODER_VLAN_CFG_2_NW_ADDR                                          (0x007Cu)
#define LCE_DECODER_VLAN_CFG_2_RESET_VAL                                    (0x00000000u)
#define LCE_DECODER_VLAN_CFG_2_MMS                                                   (0u)
#define LCE_DECODER_VLAN_CFG_2_VOL_MASK                                     (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | decoder_vlan_id_2        | Set third VLAN identifier for     | 0x00000000  |
// |       |                          | filtering and processing.         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-12 | decoder_vlan_id_3        | Set fourth VLAN identifier for    | 0x00000000  |
// |       |                          | filtering and processing.         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: decoder_vlan_id_2
// Bit position, length and mask
#define BITP_LCE_DECODER_VLAN_CFG_2_DECODER_VLAN_ID_2                                (0u)
#define BITL_LCE_DECODER_VLAN_CFG_2_DECODER_VLAN_ID_2                               (12u)
#define BITM_LCE_DECODER_VLAN_CFG_2_DECODER_VLAN_ID_2                       (0x00000FFFu)

// Bit field: decoder_vlan_id_3
// Bit position, length and mask
#define BITP_LCE_DECODER_VLAN_CFG_2_DECODER_VLAN_ID_3                               (12u)
#define BITL_LCE_DECODER_VLAN_CFG_2_DECODER_VLAN_ID_3                               (12u)
#define BITM_LCE_DECODER_VLAN_CFG_2_DECODER_VLAN_ID_3                       (0x00FFF000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : UPSTREAM_MAC_ADDRESS_UPPER
// Register Addr        : 0x0114u
// Register NW Addr     : 0x0114u
// Register Reset Value : 0x00000102u
// Register MMS         : 0u

#define LCE_UPSTREAM_MAC_ADDRESS_UPPER_ADDR                                     (0x0114u)
#define LCE_UPSTREAM_MAC_ADDRESS_UPPER_NW_ADDR                                  (0x0114u)
#define LCE_UPSTREAM_MAC_ADDRESS_UPPER_RESET_VAL                            (0x00000102u)
#define LCE_UPSTREAM_MAC_ADDRESS_UPPER_MMS                                           (0u)
#define LCE_UPSTREAM_MAC_ADDRESS_UPPER_VOL_MASK                             (0x8000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | UPSTREAM_MAC_ADDRESS_OVER| Stores the desired source         | 0x00000102  | 
// |       |                          | address when overriding the       |             |
// |       |                          | default MAC address (bits 47 to   |             |
// |       |                          | 32).                              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | UPSTREAM_MAC_ADDRESS_OVER| Enables overriding the default    | 0x00000000  | 
// |       |                          | destination MAC address learned   |             |
// |       |                          | by the engine.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: UPSTREAM_MAC_ADDRESS_OVERRIDE_UPPER
// Bit position, length and mask
#define BITP_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_UPPER      (0u)
#define BITL_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_UPPER     (16u)
#define BITM_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_UPPER (0x0000FFFFu)

// Bit field: UPSTREAM_MAC_ADDRESS_OVERRIDE_EN
// Bit position, length and mask
#define BITP_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_EN        (31u)
#define BITL_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_EN         (1u)
#define BITM_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_EN (0x80000000u)
// Enums
#define ENUM_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_EN_AUTOMATIC (0x00000000u)
#define ENUM_LCE_UPSTREAM_MAC_ADDRESS_UPPER_UPSTREAM_MAC_ADDRESS_OVERRIDE_EN_MANUAL (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : UPSTREAM_MAC_ADDRESS_LOWER
// Register Addr        : 0x0118u
// Register NW Addr     : 0x0118u
// Register Reset Value : 0x03040506u
// Register MMS         : 0u

#define LCE_UPSTREAM_MAC_ADDRESS_LOWER_ADDR                                     (0x0118u)
#define LCE_UPSTREAM_MAC_ADDRESS_LOWER_NW_ADDR                                  (0x0118u)
#define LCE_UPSTREAM_MAC_ADDRESS_LOWER_RESET_VAL                            (0x03040506u)
#define LCE_UPSTREAM_MAC_ADDRESS_LOWER_MMS                                           (0u)
#define LCE_UPSTREAM_MAC_ADDRESS_LOWER_VOL_MASK                             (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | UPSTREAM_MAC_ADDRESS_OVER| Stores the desired source         | 0x03040506  | 
// |       |                          | address when overriding the       |             |
// |       |                          | default MAC address (bits 31 to   |             |
// |       |                          | 0).                               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: UPSTREAM_MAC_ADDRESS_OVERRIDE_LOWER
// Bit position, length and mask
#define BITP_LCE_UPSTREAM_MAC_ADDRESS_LOWER_UPSTREAM_MAC_ADDRESS_OVERRIDE_LOWER      (0u)
#define BITL_LCE_UPSTREAM_MAC_ADDRESS_LOWER_UPSTREAM_MAC_ADDRESS_OVERRIDE_LOWER     (32u)
#define BITM_LCE_UPSTREAM_MAC_ADDRESS_LOWER_UPSTREAM_MAC_ADDRESS_OVERRIDE_LOWER (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SMC_MISC
// Register Addr        : 0x011Cu
// Register NW Addr     : 0x011Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_SMC_MISC_ADDR                                                       (0x011Cu)
#define LCE_SMC_MISC_NW_ADDR                                                    (0x011Cu)
#define LCE_SMC_MISC_RESET_VAL                                              (0x00000000u)
#define LCE_SMC_MISC_MMS                                                             (0u)
#define LCE_SMC_MISC_VOL_MASK                                               (0x3BFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | smc_enable               | Enables the SMC (Scannable        | 0x00000000  |
// |       |                          | Memory Controller).               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-01 | smc_memory_size          | Configures the Scannable Memory   | 0x00000000  |
// |       |                          | Controller region buffer size,    |             |
// |       |                          | in bytes.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | smc_starting_timestamp_en| Conditions the SMC to activate    | 0x00000000  | 
// |       |                          | only at a time after the value    |             |
// |       |                          | of `smc_starting_timestamp`.      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 24-17 | smc_regmap_manual_trigger| Register map bits to enable the   | 0x00000000  | 
// |       |                          | execution of Manual Mode regions  |             |
// |       |                          | in the SMC.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 25    | smc_disable_mode         | How the SMC stops operation       | 0x00000000  |
// |       |                          | after `smc_enable` is             |             |
// |       |                          | deasserted.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27    | smc_manual_trigger_config| How to interpret SMC Manual Mode  | 0x00000000  | 
// |       |                          | trigger signals to enable a       |             |
// |       |                          | Manual Mode region.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 28    | smc_selected_configuratio| Configuration set to use in       | 0x00000000  | 
// |       |                          | subsequent iterations of the      |             |
// |       |                          | SMC. Does not affect the current  |             |
// |       |                          | iteration.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 29    | smc_manual_trigger_detect| Determines the time window in     | 0x00000000  | 
// |       |                          | which to evaluate manual-mode     |             |
// |       |                          | triggers when                     |             |
// |       |                          | `smc_manual_trigger_config` is    |             |
// |       |                          | set to `LEVEL`.                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: smc_enable
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_ENABLE                                                 (0u)
#define BITL_LCE_SMC_MISC_SMC_ENABLE                                                 (1u)
#define BITM_LCE_SMC_MISC_SMC_ENABLE                                        (0x00000001u)
// Enums
#define ENUM_LCE_SMC_MISC_SMC_ENABLE_ACTIVE                                 (0x00000001u)
#define ENUM_LCE_SMC_MISC_SMC_ENABLE_INACTIVE                               (0x00000000u)

// Bit field: smc_memory_size
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_MEMORY_SIZE                                            (1u)
#define BITL_LCE_SMC_MISC_SMC_MEMORY_SIZE                                           (15u)
#define BITM_LCE_SMC_MISC_SMC_MEMORY_SIZE                                   (0x0000FFFEu)

// Bit field: smc_starting_timestamp_enable
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_STARTING_TIMESTAMP_ENABLE                             (16u)
#define BITL_LCE_SMC_MISC_SMC_STARTING_TIMESTAMP_ENABLE                              (1u)
#define BITM_LCE_SMC_MISC_SMC_STARTING_TIMESTAMP_ENABLE                     (0x00010000u)
// Enums
#define ENUM_LCE_SMC_MISC_SMC_STARTING_TIMESTAMP_ENABLE_DELAYED_START       (0x00000001u)
#define ENUM_LCE_SMC_MISC_SMC_STARTING_TIMESTAMP_ENABLE_INSTANT_STAR        (0x00000000u)

// Bit field: smc_regmap_manual_trigger
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_REGMAP_MANUAL_TRIGGER                                 (17u)
#define BITL_LCE_SMC_MISC_SMC_REGMAP_MANUAL_TRIGGER                                  (8u)
#define BITM_LCE_SMC_MISC_SMC_REGMAP_MANUAL_TRIGGER                         (0x01FE0000u)

// Bit field: smc_disable_mode
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_DISABLE_MODE                                          (25u)
#define BITL_LCE_SMC_MISC_SMC_DISABLE_MODE                                           (1u)
#define BITM_LCE_SMC_MISC_SMC_DISABLE_MODE                                  (0x02000000u)
// Enums
#define ENUM_LCE_SMC_MISC_SMC_DISABLE_MODE_END_REGION                       (0x00000000u)
#define ENUM_LCE_SMC_MISC_SMC_DISABLE_MODE_END_ITERATION                    (0x00000001u)

// Bit field: smc_manual_trigger_config
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_CONFIG                                 (27u)
#define BITL_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_CONFIG                                  (1u)
#define BITM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_CONFIG                         (0x08000000u)
// Enums
#define ENUM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_CONFIG_LEVEL                   (0x00000000u)
#define ENUM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_CONFIG_EDGE                    (0x00000001u)

// Bit field: smc_selected_configuration
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION                                (28u)
#define BITL_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION                                 (1u)
#define BITM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION                        (0x10000000u)
// Enums
#define ENUM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION_CONFIG_A               (0x00000000u)
#define ENUM_LCE_SMC_MISC_SMC_SELECTED_CONFIGURATION_CONFIG_B               (0x00000001u)

// Bit field: smc_manual_trigger_detect
// Bit position, length and mask
#define BITP_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_DETECT                                 (29u)
#define BITL_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_DETECT                                  (1u)
#define BITM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_DETECT                         (0x20000000u)
// Enums
#define ENUM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_DETECT_INSTANT                 (0x00000000u)
#define ENUM_LCE_SMC_MISC_SMC_MANUAL_TRIGGER_DETECT_PREVIOUS_ITERATION      (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SMC_TIMEBASE_PERIOD_NS
// Register Addr        : 0x0120u
// Register NW Addr     : 0x0120u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_SMC_TIMEBASE_PERIOD_NS_ADDR                                         (0x0120u)
#define LCE_SMC_TIMEBASE_PERIOD_NS_NW_ADDR                                      (0x0120u)
#define LCE_SMC_TIMEBASE_PERIOD_NS_RESET_VAL                                (0x00000000u)
#define LCE_SMC_TIMEBASE_PERIOD_NS_MMS                                               (0u)
#define LCE_SMC_TIMEBASE_PERIOD_NS_VOL_MASK                                 (0x3FFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 29-00 | smc_timebase_nanoseconds | Number of nanoseconds to use for  | 0x00000000  |
// |       |                          | the SMC timebase. Maximum value   |             |
// |       |                          | 1e9 nanoseconds.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: smc_timebase_nanoseconds
// Bit position, length and mask
#define BITP_LCE_SMC_TIMEBASE_PERIOD_NS_SMC_TIMEBASE_NANOSECONDS                     (0u)
#define BITL_LCE_SMC_TIMEBASE_PERIOD_NS_SMC_TIMEBASE_NANOSECONDS                    (30u)
#define BITM_LCE_SMC_TIMEBASE_PERIOD_NS_SMC_TIMEBASE_NANOSECONDS            (0x3FFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SMC_TIMEBASE_PERIOD_S
// Register Addr        : 0x0124u
// Register NW Addr     : 0x0124u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_SMC_TIMEBASE_PERIOD_S_ADDR                                          (0x0124u)
#define LCE_SMC_TIMEBASE_PERIOD_S_NW_ADDR                                       (0x0124u)
#define LCE_SMC_TIMEBASE_PERIOD_S_RESET_VAL                                 (0x00000000u)
#define LCE_SMC_TIMEBASE_PERIOD_S_MMS                                                (0u)
#define LCE_SMC_TIMEBASE_PERIOD_S_VOL_MASK                                  (0x000FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 19-00 | smc_timebase_seconds     | Number of seconds to use for the  | 0x00000000  |
// |       |                          | SMC timebase.                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: smc_timebase_seconds
// Bit position, length and mask
#define BITP_LCE_SMC_TIMEBASE_PERIOD_S_SMC_TIMEBASE_SECONDS                          (0u)
#define BITL_LCE_SMC_TIMEBASE_PERIOD_S_SMC_TIMEBASE_SECONDS                         (20u)
#define BITM_LCE_SMC_TIMEBASE_PERIOD_S_SMC_TIMEBASE_SECONDS                 (0x000FFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SMC_STARTING_TIMESTAMP_LOWER
// Register Addr        : 0x0128u
// Register NW Addr     : 0x0128u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_SMC_STARTING_TIMESTAMP_LOWER_ADDR                                   (0x0128u)
#define LCE_SMC_STARTING_TIMESTAMP_LOWER_NW_ADDR                                (0x0128u)
#define LCE_SMC_STARTING_TIMESTAMP_LOWER_RESET_VAL                          (0x00000000u)
#define LCE_SMC_STARTING_TIMESTAMP_LOWER_MMS                                         (0u)
#define LCE_SMC_STARTING_TIMESTAMP_LOWER_VOL_MASK                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | smc_starting_timestamp   | GPTP time value that needs to     | 0x00000000  |
// |       |                          | have elapsed before the SMC is    |             |
// |       |                          | activated if                      |             |
// |       |                          | `smc_starting_timestamp_enable`   |             |
// |       |                          | is asserted.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: smc_starting_timestamp
// Bit position, length and mask
#define BITP_LCE_SMC_STARTING_TIMESTAMP_LOWER_SMC_STARTING_TIMESTAMP                 (0u)
#define BITL_LCE_SMC_STARTING_TIMESTAMP_LOWER_SMC_STARTING_TIMESTAMP                (32u)
#define BITM_LCE_SMC_STARTING_TIMESTAMP_LOWER_SMC_STARTING_TIMESTAMP        (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SMC_STARTING_TIMESTAMP_UPPER
// Register Addr        : 0x012Cu
// Register NW Addr     : 0x012Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_SMC_STARTING_TIMESTAMP_UPPER_ADDR                                   (0x012Cu)
#define LCE_SMC_STARTING_TIMESTAMP_UPPER_NW_ADDR                                (0x012Cu)
#define LCE_SMC_STARTING_TIMESTAMP_UPPER_RESET_VAL                          (0x00000000u)
#define LCE_SMC_STARTING_TIMESTAMP_UPPER_MMS                                         (0u)
#define LCE_SMC_STARTING_TIMESTAMP_UPPER_VOL_MASK                           (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | smc_starting_timestamp   | GPTP time value that needs to     | 0x00000000  |
// |       |                          | have elapsed before the SMC is    |             |
// |       |                          | activated if                      |             |
// |       |                          | `smc_starting_timestamp_enable`   |             |
// |       |                          | is asserted.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: smc_starting_timestamp
// Bit position, length and mask
#define BITP_LCE_SMC_STARTING_TIMESTAMP_UPPER_SMC_STARTING_TIMESTAMP                 (0u)
#define BITL_LCE_SMC_STARTING_TIMESTAMP_UPPER_SMC_STARTING_TIMESTAMP                (32u)
#define BITM_LCE_SMC_STARTING_TIMESTAMP_UPPER_SMC_STARTING_TIMESTAMP        (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ACKNOWLEDGE_CTAG_CONFIG
// Register Addr        : 0x0130u
// Register NW Addr     : 0x0130u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_ACKNOWLEDGE_CTAG_CONFIG_ADDR                                        (0x0130u)
#define LCE_ACKNOWLEDGE_CTAG_CONFIG_NW_ADDR                                     (0x0130u)
#define LCE_ACKNOWLEDGE_CTAG_CONFIG_RESET_VAL                               (0x00000000u)
#define LCE_ACKNOWLEDGE_CTAG_CONFIG_MMS                                              (0u)
#define LCE_ACKNOWLEDGE_CTAG_CONFIG_VOL_MASK                                (0x8000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | ack_ctag_id              | Sets the acknowledge ctag ID.     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | ack_ctag_drop_eligible   | Sets the drop eligibility for     | 0x00000000  |
// |       |                          | acknowledge ctag.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-13 | ack_ctag_priority        | Sets the priority for             | 0x00000000  |
// |       |                          | acknowledge ctag.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | ack_transmit_ctag        | Enables the acknowledge ctag.     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: ack_ctag_id
// Bit position, length and mask
#define BITP_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_ID                                 (0u)
#define BITL_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_ID                                (12u)
#define BITM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_ID                        (0x00000FFFu)

// Bit field: ack_ctag_drop_eligible
// Bit position, length and mask
#define BITP_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_DROP_ELIGIBLE                     (12u)
#define BITL_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_DROP_ELIGIBLE                      (1u)
#define BITM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_DROP_ELIGIBLE             (0x00001000u)
// Enums
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_DROP_ELIGIBLE_DONT_DROP   (0x00000000u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_DROP_ELIGIBLE_DROP_ELIGIBLE (0x00000001u)

// Bit field: ack_ctag_priority
// Bit position, length and mask
#define BITP_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY                          (13u)
#define BITL_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY                           (3u)
#define BITM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY                  (0x0000E000u)
// Enums
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY_NETWORK_CONTROL  (0x00000007u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY_VOICE            (0x00000006u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY_VIDEO            (0x00000005u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY_CONTROLLED_LOAD  (0x00000004u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY_EXCELLENT_EFFORT (0x00000003u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY_BEST_EFFORT      (0x00000002u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY_BACKGROUND       (0x00000000u)

// Bit field: ack_transmit_ctag
// Bit position, length and mask
#define BITP_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_TRANSMIT_CTAG                          (31u)
#define BITL_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_TRANSMIT_CTAG                           (1u)
#define BITM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_TRANSMIT_CTAG                  (0x80000000u)
// Enums
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_TRANSMIT_CTAG_TRANSMIT_CTAG    (0x00000001u)
#define ENUM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_TRANSMIT_CTAG_DONT_TRANSMIT_CTAG (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : FIFO_REJECT_CTAG_CONFIG
// Register Addr        : 0x0134u
// Register NW Addr     : 0x0134u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_FIFO_REJECT_CTAG_CONFIG_ADDR                                        (0x0134u)
#define LCE_FIFO_REJECT_CTAG_CONFIG_NW_ADDR                                     (0x0134u)
#define LCE_FIFO_REJECT_CTAG_CONFIG_RESET_VAL                               (0x00000000u)
#define LCE_FIFO_REJECT_CTAG_CONFIG_MMS                                              (0u)
#define LCE_FIFO_REJECT_CTAG_CONFIG_VOL_MASK                                (0x8000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | fifo_reject_ctag_id      | Sets the fifo reject ctag ID.     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | fifo_reject_ctag_drop_eli| Sets the drop eligibility for     | 0x00000000  | 
// |       |                          | fifo reject ctag.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-13 | fifo_reject_ctag_priority| Sets the priority for fifo        | 0x00000000  | 
// |       |                          | reject ctag.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | fifo_reject_transmit_ctag| Enables the fio reject messages   | 0x00000000  | 
// |       |                          | ctag and assigns its QTAG         |             |
// |       |                          | properties.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: fifo_reject_ctag_id
// Bit position, length and mask
#define BITP_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_ID                         (0u)
#define BITL_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_ID                        (12u)
#define BITM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_ID                (0x00000FFFu)

// Bit field: fifo_reject_ctag_drop_eligible
// Bit position, length and mask
#define BITP_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_DROP_ELIGIBLE             (12u)
#define BITL_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_DROP_ELIGIBLE              (1u)
#define BITM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_DROP_ELIGIBLE     (0x00001000u)
// Enums
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_DROP_ELIGIBLE_DONT_DROP (0x00000000u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_DROP_ELIGIBLE_DROP_ELIGIBLE (0x00000001u)

// Bit field: fifo_reject_ctag_priority
// Bit position, length and mask
#define BITP_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY                  (13u)
#define BITL_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY                   (3u)
#define BITM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY          (0x0000E000u)
// Enums
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY_NETWORK_CONTROL (0x00000007u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY_VOICE    (0x00000006u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY_VIDEO    (0x00000005u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY_CONTROLLED_LOAD (0x00000004u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY_EXCELLENT_EFFORT (0x00000003u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY_BEST_EFFORT (0x00000002u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY_BACKGROUND (0x00000000u)

// Bit field: fifo_reject_transmit_ctag
// Bit position, length and mask
#define BITP_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_TRANSMIT_CTAG                  (31u)
#define BITL_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_TRANSMIT_CTAG                   (1u)
#define BITM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_TRANSMIT_CTAG          (0x80000000u)
// Enums
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_TRANSMIT_CTAG_TRANSMIT_CTAG (0x00000001u)
#define ENUM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_TRANSMIT_CTAG_DONT_TRANSMIT_CTAG (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : REGISTERMAP_READ_CTAG_CONFIG
// Register Addr        : 0x0138u
// Register NW Addr     : 0x0138u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_REGISTERMAP_READ_CTAG_CONFIG_ADDR                                   (0x0138u)
#define LCE_REGISTERMAP_READ_CTAG_CONFIG_NW_ADDR                                (0x0138u)
#define LCE_REGISTERMAP_READ_CTAG_CONFIG_RESET_VAL                          (0x00000000u)
#define LCE_REGISTERMAP_READ_CTAG_CONFIG_MMS                                         (0u)
#define LCE_REGISTERMAP_READ_CTAG_CONFIG_VOL_MASK                           (0x8000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | regmap_rb_ctag_id        | Sets the register map readbacks   | 0x00000000  |
// |       |                          | ctag ID.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | regmap_rb_ctag_drop_eligi| Sets the drop eligibility for     | 0x00000000  | 
// |       |                          | register map readbacks ctag.      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-13 | regmap_rb_ctag_priority  | Sets the priority for register    | 0x00000000  |
// |       |                          | map readbacks ctag.               |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | regmap_rb_transmit_ctag  | Enables the register map          | 0x00000000  |
// |       |                          | readback messages ctag and        |             |
// |       |                          | assigns its QTAG properties.      |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: regmap_rb_ctag_id
// Bit position, length and mask
#define BITP_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_ID                      (0u)
#define BITL_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_ID                     (12u)
#define BITM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_ID             (0x00000FFFu)

// Bit field: regmap_rb_ctag_drop_eligible
// Bit position, length and mask
#define BITP_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_DROP_ELIGIBLE          (12u)
#define BITL_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_DROP_ELIGIBLE           (1u)
#define BITM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_DROP_ELIGIBLE  (0x00001000u)
// Enums
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_DROP_ELIGIBLE_DONT_DROP (0x00000000u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_DROP_ELIGIBLE_DROP_ELIGIBLE (0x00000001u)

// Bit field: regmap_rb_ctag_priority
// Bit position, length and mask
#define BITP_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY               (13u)
#define BITL_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY                (3u)
#define BITM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY       (0x0000E000u)
// Enums
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY_NETWORK_CONTROL (0x00000007u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY_VOICE (0x00000006u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY_VIDEO (0x00000005u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY_CONTROLLED_LOAD (0x00000004u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY_EXCELLENT_EFFORT (0x00000003u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY_BEST_EFFORT (0x00000002u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY_BACKGROUND (0x00000000u)

// Bit field: regmap_rb_transmit_ctag
// Bit position, length and mask
#define BITP_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_TRANSMIT_CTAG               (31u)
#define BITL_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_TRANSMIT_CTAG                (1u)
#define BITM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_TRANSMIT_CTAG       (0x80000000u)
// Enums
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_TRANSMIT_CTAG_TRANSMIT_CTAG (0x00000001u)
#define ENUM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_TRANSMIT_CTAG_DONT_TRANSMIT_CTAG (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : INTERURPT_CTAG_CONFIG
// Register Addr        : 0x013Cu
// Register NW Addr     : 0x013Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define LCE_INTERURPT_CTAG_CONFIG_ADDR                                          (0x013Cu)
#define LCE_INTERURPT_CTAG_CONFIG_NW_ADDR                                       (0x013Cu)
#define LCE_INTERURPT_CTAG_CONFIG_RESET_VAL                                 (0x00000000u)
#define LCE_INTERURPT_CTAG_CONFIG_MMS                                                (0u)
#define LCE_INTERURPT_CTAG_CONFIG_VOL_MASK                                  (0x8000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | interrupt_ctag_id        | Sets the interrupts ctag ID.      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | interrupt_ctag_drop_eligi| Sets the drop eligibility for     | 0x00000000  | 
// |       |                          | interrupts ctag.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-13 | interrupt_ctag_priority  | Sets the priority for interrupts  | 0x00000000  |
// |       |                          | ctag.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | interrupt_transmit_ctag  | Enables the interrupt messags     | 0x00000000  |
// |       |                          | ctag and assigns its QTAG         |             |
// |       |                          | properties.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: interrupt_ctag_id
// Bit position, length and mask
#define BITP_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_ID                             (0u)
#define BITL_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_ID                            (12u)
#define BITM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_ID                    (0x00000FFFu)

// Bit field: interrupt_ctag_drop_eligible
// Bit position, length and mask
#define BITP_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_DROP_ELIGIBLE                 (12u)
#define BITL_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_DROP_ELIGIBLE                  (1u)
#define BITM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_DROP_ELIGIBLE         (0x00001000u)
// Enums
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_DROP_ELIGIBLE_DONT_DROP (0x00000000u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_DROP_ELIGIBLE_DROP_ELIGIBLE (0x00000001u)

// Bit field: interrupt_ctag_priority
// Bit position, length and mask
#define BITP_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY                      (13u)
#define BITL_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY                       (3u)
#define BITM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY              (0x0000E000u)
// Enums
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY_NETWORK_CONTROL (0x00000007u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY_VOICE        (0x00000006u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY_VIDEO        (0x00000005u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY_CONTROLLED_LOAD (0x00000004u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY_EXCELLENT_EFFORT (0x00000003u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY_BEST_EFFORT  (0x00000002u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY_BACKGROUND   (0x00000000u)

// Bit field: interrupt_transmit_ctag
// Bit position, length and mask
#define BITP_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_TRANSMIT_CTAG                      (31u)
#define BITL_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_TRANSMIT_CTAG                       (1u)
#define BITM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_TRANSMIT_CTAG              (0x80000000u)
// Enums
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_TRANSMIT_CTAG_TRANSMIT_CTAG (0x00000001u)
#define ENUM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_TRANSMIT_CTAG_DONT_TRANSMIT_CTAG (0x00000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillLCERegsData();
#endif


#endif /* ADI_E2BCORE_REGLCE_H */

/** @} */

/*
 * EOF: www.analog.com
 */


