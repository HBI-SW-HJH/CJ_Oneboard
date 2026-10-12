/*****************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_maclce_reg.h
 * @brief        : This is an auto generated header file with register
                   descriptions for MAC
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Fri Feb 28 2025 12:22:49 GMT+0530 (India Standard Time)
******************************************************************************/
/** addtogroup macphy
 *  @{
 */

#ifndef ADI_MACLCE_REG_H
#define ADI_MACLCE_REG_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define MAC_NUM_REGS    (126u)

#ifdef TEST_REG
// Data for testing registers
#include <stdint.h>
extern uint32_t ganMACRegAddr[MAC_NUM_REGS];
extern uint32_t ganMACNwRegAddr[MAC_NUM_REGS];
extern uint32_t ganMACRegMMS[MAC_NUM_REGS];
extern uint32_t ganMACRegResetVal[MAC_NUM_REGS];
extern const char *gasMACRegName[MAC_NUM_REGS];
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : IDVER
// Register Addr        : 0x0000u
// Register NW Addr     : 0xC000u
// Register Reset Value : 0x00000011u
// Register MMS         : 0u

#define MAC_IDVER_ADDR                                                          (0x0000u)
#define MAC_IDVER_NW_ADDR                                                       (0xC000u)
#define MAC_IDVER_RESET_VAL                                                 (0x00000011u)
#define MAC_IDVER_MMS                                                                (0u)
#define MAC_IDVER_VOL_MASK                                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | MINVER                   | OA Minor Version.                 | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | MAJVER                   | OA Major Version.                 | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MINVER
// Bit position, length and mask
#define BITP_MAC_IDVER_MINVER                                                        (0u)
#define BITL_MAC_IDVER_MINVER                                                        (4u)
#define BITM_MAC_IDVER_MINVER                                               (0x0000000Fu)

// Bit field: MAJVER
// Bit position, length and mask
#define BITP_MAC_IDVER_MAJVER                                                        (4u)
#define BITL_MAC_IDVER_MAJVER                                                        (4u)
#define BITM_MAC_IDVER_MAJVER                                               (0x000000F0u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PHYID
// Register Addr        : 0x0001u
// Register NW Addr     : 0xC001u
// Register Reset Value : 0x001D1000u
// Register MMS         : 0u

#define MAC_PHYID_ADDR                                                          (0x0001u)
#define MAC_PHYID_NW_ADDR                                                       (0xC001u)
#define MAC_PHYID_RESET_VAL                                                 (0x001D1000u)
#define MAC_PHYID_MMS                                                                (0u)
#define MAC_PHYID_VOL_MASK                                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | REVISION                 | Manufacturers Revision Number.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 09-04 | MODEL                    | Manufacturers Model Number.       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-10 | OUI                      | Organizationally Unique           | 0x00000744  |
// |       |                          | Identifier (Bits 2:23).           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: REVISION
// Bit position, length and mask
#define BITP_MAC_PHYID_REVISION                                                      (0u)
#define BITL_MAC_PHYID_REVISION                                                      (4u)
#define BITM_MAC_PHYID_REVISION                                             (0x0000000Fu)

// Bit field: MODEL
// Bit position, length and mask
#define BITP_MAC_PHYID_MODEL                                                         (4u)
#define BITL_MAC_PHYID_MODEL                                                         (6u)
#define BITM_MAC_PHYID_MODEL                                                (0x000003F0u)

// Bit field: OUI
// Bit position, length and mask
#define BITP_MAC_PHYID_OUI                                                          (10u)
#define BITL_MAC_PHYID_OUI                                                          (22u)
#define BITM_MAC_PHYID_OUI                                                  (0xFFFFFC00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CAPABILITY
// Register Addr        : 0x0002u
// Register NW Addr     : 0xC002u
// Register Reset Value : 0x00000583u
// Register MMS         : 0u

#define MAC_CAPABILITY_ADDR                                                     (0x0002u)
#define MAC_CAPABILITY_NW_ADDR                                                  (0xC002u)
#define MAC_CAPABILITY_RESET_VAL                                            (0x00000583u)
#define MAC_CAPABILITY_MMS                                                           (0u)
#define MAC_CAPABILITY_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | MINCPS                   | Minimum Supported Chunk Payload   | 0x00000003  |
// |       |                          | Size.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | SEQC                     | TX Data Chunk Sequence and Retry  | 0x00000000  |
// |       |                          | Capability.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | AIDC                     | Address Increment Disable         | 0x00000000  |
// |       |                          | Capability.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | FTSC                     | Frame Timestamp Capability.       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | CTC                      | Cut-Through Capability.           | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | DPRAC                    | Direct PHY Register Access        | 0x00000001  |
// |       |                          | Capability.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | IPRAC                    | Indirect PHY Register Access      | 0x00000000  |
// |       |                          | Capability.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | TXFCSVC                  | Transmit Frame Check Sequence     | 0x00000001  |
// |       |                          | Validation Capability.            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MINCPS
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_MINCPS                                                   (0u)
#define BITL_MAC_CAPABILITY_MINCPS                                                   (3u)
#define BITM_MAC_CAPABILITY_MINCPS                                          (0x00000007u)
// Enums
#define ENUM_MAC_CAPABILITY_MINCPS_CPS64                                    (0x00000006u)
#define ENUM_MAC_CAPABILITY_MINCPS_CPS32                                    (0x00000005u)
#define ENUM_MAC_CAPABILITY_MINCPS_CPS16                                    (0x00000004u)
#define ENUM_MAC_CAPABILITY_MINCPS_CPS8                                     (0x00000003u)

// Bit field: SEQC
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_SEQC                                                     (4u)
#define BITL_MAC_CAPABILITY_SEQC                                                     (1u)
#define BITM_MAC_CAPABILITY_SEQC                                            (0x00000010u)
// Enums
#define ENUM_MAC_CAPABILITY_SEQC_SEQC_1                                     (0x00000001u)
#define ENUM_MAC_CAPABILITY_SEQC_SEQC_0                                     (0x00000000u)

// Bit field: AIDC
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_AIDC                                                     (5u)
#define BITL_MAC_CAPABILITY_AIDC                                                     (1u)
#define BITM_MAC_CAPABILITY_AIDC                                            (0x00000020u)
// Enums
#define ENUM_MAC_CAPABILITY_AIDC_AIDC_0                                     (0x00000000u)
#define ENUM_MAC_CAPABILITY_AIDC_AIDC_1                                     (0x00000001u)

// Bit field: FTSC
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_FTSC                                                     (6u)
#define BITL_MAC_CAPABILITY_FTSC                                                     (1u)
#define BITM_MAC_CAPABILITY_FTSC                                            (0x00000040u)
// Enums
#define ENUM_MAC_CAPABILITY_FTSC_FTSC_1                                     (0x00000001u)
#define ENUM_MAC_CAPABILITY_FTSC_FTSC_0                                     (0x00000000u)

// Bit field: CTC
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_CTC                                                      (7u)
#define BITL_MAC_CAPABILITY_CTC                                                      (1u)
#define BITM_MAC_CAPABILITY_CTC                                             (0x00000080u)
// Enums
#define ENUM_MAC_CAPABILITY_CTC_CTC_0                                       (0x00000000u)
#define ENUM_MAC_CAPABILITY_CTC_CTC_1                                       (0x00000001u)

// Bit field: DPRAC
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_DPRAC                                                    (8u)
#define BITL_MAC_CAPABILITY_DPRAC                                                    (1u)
#define BITM_MAC_CAPABILITY_DPRAC                                           (0x00000100u)
// Enums
#define ENUM_MAC_CAPABILITY_DPRAC_DPRAC_0                                   (0x00000000u)
#define ENUM_MAC_CAPABILITY_DPRAC_DPRAC_1                                   (0x00000001u)

// Bit field: IPRAC
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_IPRAC                                                    (9u)
#define BITL_MAC_CAPABILITY_IPRAC                                                    (1u)
#define BITM_MAC_CAPABILITY_IPRAC                                           (0x00000200u)
// Enums
#define ENUM_MAC_CAPABILITY_IPRAC_IPRAC_0                                   (0x00000000u)
#define ENUM_MAC_CAPABILITY_IPRAC_IPRAC_1                                   (0x00000001u)

// Bit field: TXFCSVC
// Bit position, length and mask
#define BITP_MAC_CAPABILITY_TXFCSVC                                                 (10u)
#define BITL_MAC_CAPABILITY_TXFCSVC                                                  (1u)
#define BITM_MAC_CAPABILITY_TXFCSVC                                         (0x00000400u)
// Enums
#define ENUM_MAC_CAPABILITY_TXFCSVC_TXFCSVC_0                               (0x00000000u)
#define ENUM_MAC_CAPABILITY_TXFCSVC_TXFCSVC_1                               (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RESET
// Register Addr        : 0x0003u
// Register NW Addr     : 0xC003u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_RESET_ADDR                                                          (0x0003u)
#define MAC_RESET_NW_ADDR                                                       (0xC003u)
#define MAC_RESET_RESET_VAL                                                 (0x00000000u)
#define MAC_RESET_MMS                                                                (0u)
#define MAC_RESET_VOL_MASK                                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | SWRESET                  | Software Reset.                   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: SWRESET
// Bit position, length and mask
#define BITP_MAC_RESET_SWRESET                                                       (0u)
#define BITL_MAC_RESET_SWRESET                                                       (1u)
#define BITM_MAC_RESET_SWRESET                                              (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONFIG0
// Register Addr        : 0x0004u
// Register NW Addr     : 0xC004u
// Register Reset Value : 0x00000026u
// Register MMS         : 0u

#define MAC_CONFIG0_ADDR                                                        (0x0004u)
#define MAC_CONFIG0_NW_ADDR                                                     (0xC004u)
#define MAC_CONFIG0_RESET_VAL                                               (0x00000026u)
#define MAC_CONFIG0_MMS                                                              (0u)
#define MAC_CONFIG0_VOL_MASK                                                (0x0000FFF7u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 02-00 | CPS                      | Chunk Payload Selector (N).       | 0x00000006  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | SEQE                     | Enable TX Data Chunk Sequence     | 0x00000000  |
// |       |                          | and Retry.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | PROTE                    | Enable Control Data Read Write    | 0x00000001  |
// |       |                          | Protection.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | FTSS                     | Receive Frame Timestamp Select.   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | FTSE                     | Frame Timestamp Enable.           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11-10 | TXCTHRESH                | Transmit Credit Threshold.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | ZARFE                    | Zero-Align Receive Frame Enable.  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | CSARFE                   | CSn Align Receive Frame Enable.   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 14    | TXFCSVE                  | Transmit Frame Check Sequence     | 0x00000000  |
// |       |                          | Validation Enable.                |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15    | SYNC                     | Configuration Synchronization.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CPS
// Bit position, length and mask
#define BITP_MAC_CONFIG0_CPS                                                         (0u)
#define BITL_MAC_CONFIG0_CPS                                                         (3u)
#define BITM_MAC_CONFIG0_CPS                                                (0x00000007u)
// Enums
#define ENUM_MAC_CONFIG0_CPS_CHUNK_8BYTE                                    (0x00000003u)
#define ENUM_MAC_CONFIG0_CPS_CHUNK_16BYTE                                   (0x00000004u)
#define ENUM_MAC_CONFIG0_CPS_CHUNK_32BYTE                                   (0x00000005u)
#define ENUM_MAC_CONFIG0_CPS_CHUNK_64BYTE                                   (0x00000006u)

// Bit field: SEQE
// Bit position, length and mask
#define BITP_MAC_CONFIG0_SEQE                                                        (4u)
#define BITL_MAC_CONFIG0_SEQE                                                        (1u)
#define BITM_MAC_CONFIG0_SEQE                                               (0x00000010u)
// Enums
#define ENUM_MAC_CONFIG0_SEQE_DISABLE                                       (0x00000000u)
#define ENUM_MAC_CONFIG0_SEQE_ENABLE                                        (0x00000001u)

// Bit field: PROTE
// Bit position, length and mask
#define BITP_MAC_CONFIG0_PROTE                                                       (5u)
#define BITL_MAC_CONFIG0_PROTE                                                       (1u)
#define BITM_MAC_CONFIG0_PROTE                                              (0x00000020u)
// Enums
#define ENUM_MAC_CONFIG0_PROTE_DISABLE                                      (0x00000000u)
#define ENUM_MAC_CONFIG0_PROTE_ENABLE                                       (0x00000001u)

// Bit field: FTSS
// Bit position, length and mask
#define BITP_MAC_CONFIG0_FTSS                                                        (6u)
#define BITL_MAC_CONFIG0_FTSS                                                        (1u)
#define BITM_MAC_CONFIG0_FTSS                                               (0x00000040u)
// Enums
#define ENUM_MAC_CONFIG0_FTSS_FTSS_0                                        (0x00000000u)
#define ENUM_MAC_CONFIG0_FTSS_FTSS_1                                        (0x00000001u)

// Bit field: FTSE
// Bit position, length and mask
#define BITP_MAC_CONFIG0_FTSE                                                        (7u)
#define BITL_MAC_CONFIG0_FTSE                                                        (1u)
#define BITM_MAC_CONFIG0_FTSE                                               (0x00000080u)
// Enums
#define ENUM_MAC_CONFIG0_FTSE_DISABLE                                       (0x00000000u)
#define ENUM_MAC_CONFIG0_FTSE_ENABLE                                        (0x00000001u)

// Bit field: TXCTHRESH
// Bit position, length and mask
#define BITP_MAC_CONFIG0_TXCTHRESH                                                  (10u)
#define BITL_MAC_CONFIG0_TXCTHRESH                                                   (2u)
#define BITM_MAC_CONFIG0_TXCTHRESH                                          (0x00000C00u)
// Enums
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_1                                 (0x00000000u)
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_4                                 (0x00000001u)
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_8                                 (0x00000002u)
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_16                                (0x00000003u)

// Bit field: ZARFE
// Bit position, length and mask
#define BITP_MAC_CONFIG0_ZARFE                                                      (12u)
#define BITL_MAC_CONFIG0_ZARFE                                                       (1u)
#define BITM_MAC_CONFIG0_ZARFE                                              (0x00001000u)
// Enums
#define ENUM_MAC_CONFIG0_ZARFE_DISABLE                                      (0x00000000u)
#define ENUM_MAC_CONFIG0_ZARFE_ENABLE                                       (0x00000001u)

// Bit field: CSARFE
// Bit position, length and mask
#define BITP_MAC_CONFIG0_CSARFE                                                     (13u)
#define BITL_MAC_CONFIG0_CSARFE                                                      (1u)
#define BITM_MAC_CONFIG0_CSARFE                                             (0x00002000u)
// Enums
#define ENUM_MAC_CONFIG0_CSARFE_DISABLE                                     (0x00000000u)
#define ENUM_MAC_CONFIG0_CSARFE_ENABLE                                      (0x00000001u)

// Bit field: TXFCSVE
// Bit position, length and mask
#define BITP_MAC_CONFIG0_TXFCSVE                                                    (14u)
#define BITL_MAC_CONFIG0_TXFCSVE                                                     (1u)
#define BITM_MAC_CONFIG0_TXFCSVE                                            (0x00004000u)
// Enums
#define ENUM_MAC_CONFIG0_TXFCSVE_DISABLE                                    (0x00000000u)
#define ENUM_MAC_CONFIG0_TXFCSVE_ENABLE                                     (0x00000001u)

// Bit field: SYNC
// Bit position, length and mask
#define BITP_MAC_CONFIG0_SYNC                                                       (15u)
#define BITL_MAC_CONFIG0_SYNC                                                        (1u)
#define BITM_MAC_CONFIG0_SYNC                                               (0x00008000u)
// Enums
#define ENUM_MAC_CONFIG0_SYNC_NOT_CONFIGURED                                (0x00000000u)
#define ENUM_MAC_CONFIG0_SYNC_CONFIGURED                                    (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CONFIG2
// Register Addr        : 0x0006u
// Register NW Addr     : 0xC006u
// Register Reset Value : 0x005C2020u
// Register MMS         : 0u

#define MAC_CONFIG2_ADDR                                                        (0x0006u)
#define MAC_CONFIG2_NW_ADDR                                                     (0xC006u)
#define MAC_CONFIG2_RESET_VAL                                               (0x005C2020u)
#define MAC_CONFIG2_MMS                                                              (0u)
#define MAC_CONFIG2_VOL_MASK                                                (0x00FFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 2     | FWD_UNK2HOST             | Forward Frames Not Matching Any   | 0x00000000  |
// |       |                          | MAC Address to the Host.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | RCV_IFG_ERR_FRM          | Admit Frames with IFG Errors.     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | HOST_CRC_APPEND          | Enable CRC Append for Frames      | 0x00000001  |
// |       |                          | Coming in from the Host.          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | STATS_CLR_ON_RD          | Statistics Clear on Reading.      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | SFD_DETECT_SRC           | Determines If the SFD is          | 0x00000000  |
// |       |                          | Detected in the PHY or MAC.       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | TX_RDY_ON_EMPTY          | Assert TX_RDY When the Tx FIFO    | 0x00000000  |
// |       |                          | is Empty.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13    | FWD_UNK2LCE              | Forward Frames Not Matching a     | 0x00000001  |
// |       |                          | MAC Addr to LCE.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 21    | RX_LENGTH_FIELD_CHK_EN   | Checks the Length Field and       | 0x00000000  |
// |       |                          | Dumps the Frame If Incorrect.     |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 22    | TX_IFG_PART2_EN          | Enables the Part1/Part2 Optional  | 0x00000001  |
// |       |                          | IFG Feature.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23    | TX_BACK_OFF_DIS          | Disables the Transmit Back off.   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: FWD_UNK2HOST
// Bit position, length and mask
#define BITP_MAC_CONFIG2_FWD_UNK2HOST                                                (2u)
#define BITL_MAC_CONFIG2_FWD_UNK2HOST                                                (1u)
#define BITM_MAC_CONFIG2_FWD_UNK2HOST                                       (0x00000004u)
// Enums
#define ENUM_MAC_CONFIG2_FWD_UNK2HOST_DISABLE                               (0x00000000u)
#define ENUM_MAC_CONFIG2_FWD_UNK2HOST_ENABLE                                (0x00000001u)

// Bit field: RCV_IFG_ERR_FRM
// Bit position, length and mask
#define BITP_MAC_CONFIG2_RCV_IFG_ERR_FRM                                             (4u)
#define BITL_MAC_CONFIG2_RCV_IFG_ERR_FRM                                             (1u)
#define BITM_MAC_CONFIG2_RCV_IFG_ERR_FRM                                    (0x00000010u)
// Enums
#define ENUM_MAC_CONFIG2_RCV_IFG_ERR_FRM_DISABLE                            (0x00000000u)
#define ENUM_MAC_CONFIG2_RCV_IFG_ERR_FRM_ENABLE                             (0x00000001u)

// Bit field: HOST_CRC_APPEND
// Bit position, length and mask
#define BITP_MAC_CONFIG2_HOST_CRC_APPEND                                             (5u)
#define BITL_MAC_CONFIG2_HOST_CRC_APPEND                                             (1u)
#define BITM_MAC_CONFIG2_HOST_CRC_APPEND                                    (0x00000020u)
// Enums
#define ENUM_MAC_CONFIG2_HOST_CRC_APPEND_DISABLE                            (0x00000000u)
#define ENUM_MAC_CONFIG2_HOST_CRC_APPEND_ENABLE                             (0x00000001u)

// Bit field: STATS_CLR_ON_RD
// Bit position, length and mask
#define BITP_MAC_CONFIG2_STATS_CLR_ON_RD                                             (6u)
#define BITL_MAC_CONFIG2_STATS_CLR_ON_RD                                             (1u)
#define BITM_MAC_CONFIG2_STATS_CLR_ON_RD                                    (0x00000040u)
// Enums
#define ENUM_MAC_CONFIG2_STATS_CLR_ON_RD_HOLD_STAT                          (0x00000000u)
#define ENUM_MAC_CONFIG2_STATS_CLR_ON_RD_CLR_STAT                           (0x00000001u)

// Bit field: SFD_DETECT_SRC
// Bit position, length and mask
#define BITP_MAC_CONFIG2_SFD_DETECT_SRC                                              (7u)
#define BITL_MAC_CONFIG2_SFD_DETECT_SRC                                              (1u)
#define BITM_MAC_CONFIG2_SFD_DETECT_SRC                                     (0x00000080u)
// Enums
#define ENUM_MAC_CONFIG2_SFD_DETECT_SRC_PHY                                 (0x00000000u)
#define ENUM_MAC_CONFIG2_SFD_DETECT_SRC_MAC                                 (0x00000001u)

// Bit field: TX_RDY_ON_EMPTY
// Bit position, length and mask
#define BITP_MAC_CONFIG2_TX_RDY_ON_EMPTY                                             (8u)
#define BITL_MAC_CONFIG2_TX_RDY_ON_EMPTY                                             (1u)
#define BITM_MAC_CONFIG2_TX_RDY_ON_EMPTY                                    (0x00000100u)
// Enums
#define ENUM_MAC_CONFIG2_TX_RDY_ON_EMPTY_TX_RDY_ON_TRANS                    (0x00000000u)
#define ENUM_MAC_CONFIG2_TX_RDY_ON_EMPTY_TX_RDY_ON_EMPTY                    (0x00000001u)

// Bit field: FWD_UNK2LCE
// Bit position, length and mask
#define BITP_MAC_CONFIG2_FWD_UNK2LCE                                                (13u)
#define BITL_MAC_CONFIG2_FWD_UNK2LCE                                                 (1u)
#define BITM_MAC_CONFIG2_FWD_UNK2LCE                                        (0x00002000u)
// Enums
#define ENUM_MAC_CONFIG2_FWD_UNK2LCE_DISABLE                                (0x00000000u)
#define ENUM_MAC_CONFIG2_FWD_UNK2LCE_ENABLE                                 (0x00000001u)

// Bit field: RX_LENGTH_FIELD_CHK_EN
// Bit position, length and mask
#define BITP_MAC_CONFIG2_RX_LENGTH_FIELD_CHK_EN                                     (21u)
#define BITL_MAC_CONFIG2_RX_LENGTH_FIELD_CHK_EN                                      (1u)
#define BITM_MAC_CONFIG2_RX_LENGTH_FIELD_CHK_EN                             (0x00200000u)
// Enums
#define ENUM_MAC_CONFIG2_RX_LENGTH_FIELD_CHK_EN_DISABLE                     (0x00000000u)
#define ENUM_MAC_CONFIG2_RX_LENGTH_FIELD_CHK_EN_ENABLE                      (0x00000001u)

// Bit field: TX_IFG_PART2_EN
// Bit position, length and mask
#define BITP_MAC_CONFIG2_TX_IFG_PART2_EN                                            (22u)
#define BITL_MAC_CONFIG2_TX_IFG_PART2_EN                                             (1u)
#define BITM_MAC_CONFIG2_TX_IFG_PART2_EN                                    (0x00400000u)
// Enums
#define ENUM_MAC_CONFIG2_TX_IFG_PART2_EN_DISABLE                            (0x00000000u)
#define ENUM_MAC_CONFIG2_TX_IFG_PART2_EN_ENABLE                             (0x00000001u)

// Bit field: TX_BACK_OFF_DIS
// Bit position, length and mask
#define BITP_MAC_CONFIG2_TX_BACK_OFF_DIS                                            (23u)
#define BITL_MAC_CONFIG2_TX_BACK_OFF_DIS                                             (1u)
#define BITM_MAC_CONFIG2_TX_BACK_OFF_DIS                                    (0x00800000u)
// Enums
#define ENUM_MAC_CONFIG2_TX_BACK_OFF_DIS_TX_BACK_OFF_ENABLE                 (0x00000000u)
#define ENUM_MAC_CONFIG2_TX_BACK_OFF_DIS_TX_BACK_OFF_DISABLE                (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : STATUS0
// Register Addr        : 0x0008u
// Register NW Addr     : 0xC008u
// Register Reset Value : 0x00000040u
// Register MMS         : 0u

#define MAC_STATUS0_ADDR                                                        (0x0008u)
#define MAC_STATUS0_NW_ADDR                                                     (0xC008u)
#define MAC_STATUS0_RESET_VAL                                               (0x00000040u)
#define MAC_STATUS0_MMS                                                              (0u)
#define MAC_STATUS0_VOL_MASK                                                (0x00001F7Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | TXPE                     | Transmit Protocol Error.          | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | TXBOE                    | Host Tx FIFO Overflow.            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | TXBUE                    | Host Tx FIFO Underrun Error.      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | RXBOE                    | Receive Buffer Overflow Error.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | LOFE                     | Loss of Frame Error.              | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | HDRE                     | Header Error.                     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | RESETC                   | Reset Complete.                   | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | PHYINT                   | PHY Interrupt for Port1.          | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | TTSCAA                   | Transmit Timestamp Capture        | 0x00000000  |
// |       |                          | Available A.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | TTSCAB                   | Transmit Timestamp Capture        | 0x00000000  |
// |       |                          | Available B.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | TTSCAC                   | Transmit Timestamp Capture        | 0x00000000  |
// |       |                          | Available C.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | TXFCSE                   | Transmit Frame Check Sequence     | 0x00000000  |
// |       |                          | Error.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | CDPE                     | Control Data Protection Error.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TXPE
// Bit position, length and mask
#define BITP_MAC_STATUS0_TXPE                                                        (0u)
#define BITL_MAC_STATUS0_TXPE                                                        (1u)
#define BITM_MAC_STATUS0_TXPE                                               (0x00000001u)

// Bit field: TXBOE
// Bit position, length and mask
#define BITP_MAC_STATUS0_TXBOE                                                       (1u)
#define BITL_MAC_STATUS0_TXBOE                                                       (1u)
#define BITM_MAC_STATUS0_TXBOE                                              (0x00000002u)

// Bit field: TXBUE
// Bit position, length and mask
#define BITP_MAC_STATUS0_TXBUE                                                       (2u)
#define BITL_MAC_STATUS0_TXBUE                                                       (1u)
#define BITM_MAC_STATUS0_TXBUE                                              (0x00000004u)

// Bit field: RXBOE
// Bit position, length and mask
#define BITP_MAC_STATUS0_RXBOE                                                       (3u)
#define BITL_MAC_STATUS0_RXBOE                                                       (1u)
#define BITM_MAC_STATUS0_RXBOE                                              (0x00000008u)

// Bit field: LOFE
// Bit position, length and mask
#define BITP_MAC_STATUS0_LOFE                                                        (4u)
#define BITL_MAC_STATUS0_LOFE                                                        (1u)
#define BITM_MAC_STATUS0_LOFE                                               (0x00000010u)

// Bit field: HDRE
// Bit position, length and mask
#define BITP_MAC_STATUS0_HDRE                                                        (5u)
#define BITL_MAC_STATUS0_HDRE                                                        (1u)
#define BITM_MAC_STATUS0_HDRE                                               (0x00000020u)

// Bit field: RESETC
// Bit position, length and mask
#define BITP_MAC_STATUS0_RESETC                                                      (6u)
#define BITL_MAC_STATUS0_RESETC                                                      (1u)
#define BITM_MAC_STATUS0_RESETC                                             (0x00000040u)

// Bit field: PHYINT
// Bit position, length and mask
#define BITP_MAC_STATUS0_PHYINT                                                      (7u)
#define BITL_MAC_STATUS0_PHYINT                                                      (1u)
#define BITM_MAC_STATUS0_PHYINT                                             (0x00000080u)

// Bit field: TTSCAA
// Bit position, length and mask
#define BITP_MAC_STATUS0_TTSCAA                                                      (8u)
#define BITL_MAC_STATUS0_TTSCAA                                                      (1u)
#define BITM_MAC_STATUS0_TTSCAA                                             (0x00000100u)

// Bit field: TTSCAB
// Bit position, length and mask
#define BITP_MAC_STATUS0_TTSCAB                                                      (9u)
#define BITL_MAC_STATUS0_TTSCAB                                                      (1u)
#define BITM_MAC_STATUS0_TTSCAB                                             (0x00000200u)

// Bit field: TTSCAC
// Bit position, length and mask
#define BITP_MAC_STATUS0_TTSCAC                                                     (10u)
#define BITL_MAC_STATUS0_TTSCAC                                                      (1u)
#define BITM_MAC_STATUS0_TTSCAC                                             (0x00000400u)

// Bit field: TXFCSE
// Bit position, length and mask
#define BITP_MAC_STATUS0_TXFCSE                                                     (11u)
#define BITL_MAC_STATUS0_TXFCSE                                                      (1u)
#define BITM_MAC_STATUS0_TXFCSE                                             (0x00000800u)

// Bit field: CDPE
// Bit position, length and mask
#define BITP_MAC_STATUS0_CDPE                                                       (12u)
#define BITL_MAC_STATUS0_CDPE                                                        (1u)
#define BITM_MAC_STATUS0_CDPE                                               (0x00001000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : STATUS1
// Register Addr        : 0x0009u
// Register NW Addr     : 0xC009u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_STATUS1_ADDR                                                        (0x0009u)
#define MAC_STATUS1_NW_ADDR                                                     (0xC009u)
#define MAC_STATUS1_RESET_VAL                                               (0x00000000u)
#define MAC_STATUS1_MMS                                                              (0u)
#define MAC_STATUS1_VOL_MASK                                                (0xC3F01F0Au)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | LINK_STATUS              | PHY Link Status.                  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | TX_RDY                   | Tx Ready.                         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | RX_RDY                   | Rx FIFO Contains Data.            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | VS_INT                   | Transaction Interrupt Controller  | 0x00000000  |
// |       |                          | Interrupt                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | RX_IFG_ERR               | Rx MAC Inter Frame Gap Error.     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | MAX_RR_ERR               | Transmit Max Collision Retry      | 0x00000000  |
// |       |                          | Reached.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | RX_ECC_ERR               | ECC Error on Reading the Frame    | 0x00000000  |
// |       |                          | Size from an Rx FIFO.             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | TX_ECC_ERR               | ECC Error on Reading the Frame    | 0x00000000  |
// |       |                          | Size from a Tx FIFO.              |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | RD_ACC_ERR               | Read Access Error.                | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: LINK_STATUS
// Bit position, length and mask
#define BITP_MAC_STATUS1_LINK_STATUS                                                 (0u)
#define BITL_MAC_STATUS1_LINK_STATUS                                                 (1u)
#define BITM_MAC_STATUS1_LINK_STATUS                                        (0x00000001u)
// Enums
#define ENUM_MAC_STATUS1_LINK_STATUS_DOWN                                   (0x00000000u)
#define ENUM_MAC_STATUS1_LINK_STATUS_UP                                     (0x00000001u)

// Bit field: TX_RDY
// Bit position, length and mask
#define BITP_MAC_STATUS1_TX_RDY                                                      (3u)
#define BITL_MAC_STATUS1_TX_RDY                                                      (1u)
#define BITM_MAC_STATUS1_TX_RDY                                             (0x00000008u)

// Bit field: RX_RDY
// Bit position, length and mask
#define BITP_MAC_STATUS1_RX_RDY                                                      (4u)
#define BITL_MAC_STATUS1_RX_RDY                                                      (1u)
#define BITM_MAC_STATUS1_RX_RDY                                             (0x00000010u)

// Bit field: VS_INT
// Bit position, length and mask
#define BITP_MAC_STATUS1_VS_INT                                                      (6u)
#define BITL_MAC_STATUS1_VS_INT                                                      (1u)
#define BITM_MAC_STATUS1_VS_INT                                             (0x00000040u)

// Bit field: RX_IFG_ERR
// Bit position, length and mask
#define BITP_MAC_STATUS1_RX_IFG_ERR                                                  (8u)
#define BITL_MAC_STATUS1_RX_IFG_ERR                                                  (1u)
#define BITM_MAC_STATUS1_RX_IFG_ERR                                         (0x00000100u)

// Bit field: MAX_RR_ERR
// Bit position, length and mask
#define BITP_MAC_STATUS1_MAX_RR_ERR                                                  (9u)
#define BITL_MAC_STATUS1_MAX_RR_ERR                                                  (1u)
#define BITM_MAC_STATUS1_MAX_RR_ERR                                         (0x00000200u)

// Bit field: RX_ECC_ERR
// Bit position, length and mask
#define BITP_MAC_STATUS1_RX_ECC_ERR                                                 (11u)
#define BITL_MAC_STATUS1_RX_ECC_ERR                                                  (1u)
#define BITM_MAC_STATUS1_RX_ECC_ERR                                         (0x00000800u)

// Bit field: TX_ECC_ERR
// Bit position, length and mask
#define BITP_MAC_STATUS1_TX_ECC_ERR                                                 (12u)
#define BITL_MAC_STATUS1_TX_ECC_ERR                                                  (1u)
#define BITM_MAC_STATUS1_TX_ECC_ERR                                         (0x00001000u)

// Bit field: RD_ACC_ERR
// Bit position, length and mask
#define BITP_MAC_STATUS1_RD_ACC_ERR                                                 (30u)
#define BITL_MAC_STATUS1_RD_ACC_ERR                                                  (1u)
#define BITM_MAC_STATUS1_RD_ACC_ERR                                         (0x40000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : BUFSTS
// Register Addr        : 0x000Bu
// Register NW Addr     : 0xC00Bu
// Register Reset Value : 0x00003B00u
// Register MMS         : 0u

#define MAC_BUFSTS_ADDR                                                         (0x000Bu)
#define MAC_BUFSTS_NW_ADDR                                                      (0xC00Bu)
#define MAC_BUFSTS_RESET_VAL                                                (0x00003B00u)
#define MAC_BUFSTS_MMS                                                               (0u)
#define MAC_BUFSTS_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | RCA                      | Receive Chunks Available.         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | TXC                      | Transmit Credits Available.       | 0x0000003b  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RCA
// Bit position, length and mask
#define BITP_MAC_BUFSTS_RCA                                                          (0u)
#define BITL_MAC_BUFSTS_RCA                                                          (8u)
#define BITM_MAC_BUFSTS_RCA                                                 (0x000000FFu)

// Bit field: TXC
// Bit position, length and mask
#define BITP_MAC_BUFSTS_TXC                                                          (8u)
#define BITL_MAC_BUFSTS_TXC                                                          (8u)
#define BITM_MAC_BUFSTS_TXC                                                 (0x0000FF00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : IMASK0
// Register Addr        : 0x000Cu
// Register NW Addr     : 0xC00Cu
// Register Reset Value : 0x00001FBFu
// Register MMS         : 0u

#define MAC_IMASK0_ADDR                                                         (0x000Cu)
#define MAC_IMASK0_NW_ADDR                                                      (0xC00Cu)
#define MAC_IMASK0_RESET_VAL                                                (0x00001FBFu)
#define MAC_IMASK0_MMS                                                               (0u)
#define MAC_IMASK0_VOL_MASK                                                 (0x00001FBFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | TXPEM                    | Transmit Protocol Error Mask.     | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | TXBOEM                   | Transmit Buffer Overflow Error    | 0x00000001  |
// |       |                          | Mask.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | TXBUEM                   | Transmit Buffer Underflow Error   | 0x00000001  |
// |       |                          | Mask.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | RXBOEM                   | Receive Buffer Overflow Error     | 0x00000001  |
// |       |                          | Mask.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | LOFEM                    | Loss of Frame Error Mask.         | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 5     | HDREM                    | Header Error Mask.                | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | RESETCM                  | RESET Complete Mask.              | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 7     | PHYINTM                  | Physical Layer Interrupt Mask.    | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | TTSCAAM                  | Transmit Timestamp Capture        | 0x00000001  |
// |       |                          | Available A Mask.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | TTSCABM                  | Transmit Timestamp Capture        | 0x00000001  |
// |       |                          | Available B Mask.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 10    | TTSCACM                  | Transmit Timestamp Capture        | 0x00000001  |
// |       |                          | Available C Mask.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | TXFCSEM                  | Transmit Frame Check Sequence     | 0x00000001  |
// |       |                          | Error Mask.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | CDPEM                    | Control Data Protection Error     | 0x00000001  |
// |       |                          | Mask.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TXPEM
// Bit position, length and mask
#define BITP_MAC_IMASK0_TXPEM                                                        (0u)
#define BITL_MAC_IMASK0_TXPEM                                                        (1u)
#define BITM_MAC_IMASK0_TXPEM                                               (0x00000001u)

// Bit field: TXBOEM
// Bit position, length and mask
#define BITP_MAC_IMASK0_TXBOEM                                                       (1u)
#define BITL_MAC_IMASK0_TXBOEM                                                       (1u)
#define BITM_MAC_IMASK0_TXBOEM                                              (0x00000002u)

// Bit field: TXBUEM
// Bit position, length and mask
#define BITP_MAC_IMASK0_TXBUEM                                                       (2u)
#define BITL_MAC_IMASK0_TXBUEM                                                       (1u)
#define BITM_MAC_IMASK0_TXBUEM                                              (0x00000004u)

// Bit field: RXBOEM
// Bit position, length and mask
#define BITP_MAC_IMASK0_RXBOEM                                                       (3u)
#define BITL_MAC_IMASK0_RXBOEM                                                       (1u)
#define BITM_MAC_IMASK0_RXBOEM                                              (0x00000008u)

// Bit field: LOFEM
// Bit position, length and mask
#define BITP_MAC_IMASK0_LOFEM                                                        (4u)
#define BITL_MAC_IMASK0_LOFEM                                                        (1u)
#define BITM_MAC_IMASK0_LOFEM                                               (0x00000010u)

// Bit field: HDREM
// Bit position, length and mask
#define BITP_MAC_IMASK0_HDREM                                                        (5u)
#define BITL_MAC_IMASK0_HDREM                                                        (1u)
#define BITM_MAC_IMASK0_HDREM                                               (0x00000020u)

// Bit field: RESETCM
// Bit position, length and mask
#define BITP_MAC_IMASK0_RESETCM                                                      (6u)
#define BITL_MAC_IMASK0_RESETCM                                                      (1u)
#define BITM_MAC_IMASK0_RESETCM                                             (0x00000040u)

// Bit field: PHYINTM
// Bit position, length and mask
#define BITP_MAC_IMASK0_PHYINTM                                                      (7u)
#define BITL_MAC_IMASK0_PHYINTM                                                      (1u)
#define BITM_MAC_IMASK0_PHYINTM                                             (0x00000080u)

// Bit field: TTSCAAM
// Bit position, length and mask
#define BITP_MAC_IMASK0_TTSCAAM                                                      (8u)
#define BITL_MAC_IMASK0_TTSCAAM                                                      (1u)
#define BITM_MAC_IMASK0_TTSCAAM                                             (0x00000100u)

// Bit field: TTSCABM
// Bit position, length and mask
#define BITP_MAC_IMASK0_TTSCABM                                                      (9u)
#define BITL_MAC_IMASK0_TTSCABM                                                      (1u)
#define BITM_MAC_IMASK0_TTSCABM                                             (0x00000200u)

// Bit field: TTSCACM
// Bit position, length and mask
#define BITP_MAC_IMASK0_TTSCACM                                                     (10u)
#define BITL_MAC_IMASK0_TTSCACM                                                      (1u)
#define BITM_MAC_IMASK0_TTSCACM                                             (0x00000400u)

// Bit field: TXFCSEM
// Bit position, length and mask
#define BITP_MAC_IMASK0_TXFCSEM                                                     (11u)
#define BITL_MAC_IMASK0_TXFCSEM                                                      (1u)
#define BITM_MAC_IMASK0_TXFCSEM                                             (0x00000800u)

// Bit field: CDPEM
// Bit position, length and mask
#define BITP_MAC_IMASK0_CDPEM                                                       (12u)
#define BITL_MAC_IMASK0_CDPEM                                                        (1u)
#define BITM_MAC_IMASK0_CDPEM                                               (0x00001000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : IMASK1
// Register Addr        : 0x000Du
// Register NW Addr     : 0xC00Du
// Register Reset Value : 0x43FA1F5Au
// Register MMS         : 0u

#define MAC_IMASK1_ADDR                                                         (0x000Du)
#define MAC_IMASK1_NW_ADDR                                                      (0xC00Du)
#define MAC_IMASK1_RESET_VAL                                                (0x43FA1F5Au)
#define MAC_IMASK1_MMS                                                               (0u)
#define MAC_IMASK1_VOL_MASK                                                 (0x43FA1F5Au)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 1     | LINK_CHANGE_MASK         | Mask Bit for LINK_CHANGE.         | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | TX_RDY_MASK              | Mask Bit for TX_FRM_DONE.         | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 6     | VS_INT_MASK              | Mask Bit for VS_INT.              | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 8     | RX_IFG_ERR_MASK          | Mask Bit for RX_IFG_ERR.          | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 9     | MAX_RR_ERR_MASK          | Mask Bit for MAX_RR_ERR.          | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 11    | RX_ECC_ERR_MASK          | Mask Bit for RXF_ECC_ERR.         | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 12    | TX_ECC_ERR_MASK          | Mask Bit for TXF_ECC_ERR.         | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | RD_ACC_ERR_MASK          | Mask Bit for RD_ACC_ERR.          | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: LINK_CHANGE_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_LINK_CHANGE_MASK                                             (1u)
#define BITL_MAC_IMASK1_LINK_CHANGE_MASK                                             (1u)
#define BITM_MAC_IMASK1_LINK_CHANGE_MASK                                    (0x00000002u)

// Bit field: TX_RDY_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_TX_RDY_MASK                                                  (3u)
#define BITL_MAC_IMASK1_TX_RDY_MASK                                                  (1u)
#define BITM_MAC_IMASK1_TX_RDY_MASK                                         (0x00000008u)

// Bit field: VS_INT_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_VS_INT_MASK                                                  (6u)
#define BITL_MAC_IMASK1_VS_INT_MASK                                                  (1u)
#define BITM_MAC_IMASK1_VS_INT_MASK                                         (0x00000040u)

// Bit field: RX_IFG_ERR_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_RX_IFG_ERR_MASK                                              (8u)
#define BITL_MAC_IMASK1_RX_IFG_ERR_MASK                                              (1u)
#define BITM_MAC_IMASK1_RX_IFG_ERR_MASK                                     (0x00000100u)

// Bit field: MAX_RR_ERR_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_MAX_RR_ERR_MASK                                              (9u)
#define BITL_MAC_IMASK1_MAX_RR_ERR_MASK                                              (1u)
#define BITM_MAC_IMASK1_MAX_RR_ERR_MASK                                     (0x00000200u)

// Bit field: RX_ECC_ERR_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_RX_ECC_ERR_MASK                                             (11u)
#define BITL_MAC_IMASK1_RX_ECC_ERR_MASK                                              (1u)
#define BITM_MAC_IMASK1_RX_ECC_ERR_MASK                                     (0x00000800u)

// Bit field: TX_ECC_ERR_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_TX_ECC_ERR_MASK                                             (12u)
#define BITL_MAC_IMASK1_TX_ECC_ERR_MASK                                              (1u)
#define BITM_MAC_IMASK1_TX_ECC_ERR_MASK                                     (0x00001000u)

// Bit field: RD_ACC_ERR_MASK
// Bit position, length and mask
#define BITP_MAC_IMASK1_RD_ACC_ERR_MASK                                             (30u)
#define BITL_MAC_IMASK1_RD_ACC_ERR_MASK                                              (1u)
#define BITM_MAC_IMASK1_RD_ACC_ERR_MASK                                     (0x40000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TTSCAH
// Register Addr        : 0x0010u
// Register NW Addr     : 0xC010u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_TTSCAH_ADDR                                                         (0x0010u)
#define MAC_TTSCAH_NW_ADDR                                                      (0xC010u)
#define MAC_TTSCAH_RESET_VAL                                                (0x00000000u)
#define MAC_TTSCAH_MMS                                                               (0u)
#define MAC_TTSCAH_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TTSCH_A                  | Transmit Timestamp A Bits 63-32   | 0x00000000  |
// |       |                          | (Seconds).                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TTSCH_A
// Bit position, length and mask
#define BITP_MAC_TTSCAH_TTSCH_A                                                      (0u)
#define BITL_MAC_TTSCAH_TTSCH_A                                                     (32u)
#define BITM_MAC_TTSCAH_TTSCH_A                                             (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TTSCAL
// Register Addr        : 0x0011u
// Register NW Addr     : 0xC011u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_TTSCAL_ADDR                                                         (0x0011u)
#define MAC_TTSCAL_NW_ADDR                                                      (0xC011u)
#define MAC_TTSCAL_RESET_VAL                                                (0x00000000u)
#define MAC_TTSCAL_MMS                                                               (0u)
#define MAC_TTSCAL_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TTSCL_A                  | Transmit Timestamp A Bits 31-0    | 0x00000000  |
// |       |                          | (NanoSeconds).                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TTSCL_A
// Bit position, length and mask
#define BITP_MAC_TTSCAL_TTSCL_A                                                      (0u)
#define BITL_MAC_TTSCAL_TTSCL_A                                                     (32u)
#define BITM_MAC_TTSCAL_TTSCL_A                                             (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TTSCBH
// Register Addr        : 0x0012u
// Register NW Addr     : 0xC012u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_TTSCBH_ADDR                                                         (0x0012u)
#define MAC_TTSCBH_NW_ADDR                                                      (0xC012u)
#define MAC_TTSCBH_RESET_VAL                                                (0x00000000u)
#define MAC_TTSCBH_MMS                                                               (0u)
#define MAC_TTSCBH_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TTSCH_B                  | Transmit Timestamp B Bits 63-32   | 0x00000000  |
// |       |                          | (Seconds).                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TTSCH_B
// Bit position, length and mask
#define BITP_MAC_TTSCBH_TTSCH_B                                                      (0u)
#define BITL_MAC_TTSCBH_TTSCH_B                                                     (32u)
#define BITM_MAC_TTSCBH_TTSCH_B                                             (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TTSCBL
// Register Addr        : 0x0013u
// Register NW Addr     : 0xC013u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_TTSCBL_ADDR                                                         (0x0013u)
#define MAC_TTSCBL_NW_ADDR                                                      (0xC013u)
#define MAC_TTSCBL_RESET_VAL                                                (0x00000000u)
#define MAC_TTSCBL_MMS                                                               (0u)
#define MAC_TTSCBL_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TTSCL_B                  | Transmit Timestamp B Bits 31-0    | 0x00000000  |
// |       |                          | (NanoSeconds).                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TTSCL_B
// Bit position, length and mask
#define BITP_MAC_TTSCBL_TTSCL_B                                                      (0u)
#define BITL_MAC_TTSCBL_TTSCL_B                                                     (32u)
#define BITM_MAC_TTSCBL_TTSCL_B                                             (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TTSCCH
// Register Addr        : 0x0014u
// Register NW Addr     : 0xC014u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_TTSCCH_ADDR                                                         (0x0014u)
#define MAC_TTSCCH_NW_ADDR                                                      (0xC014u)
#define MAC_TTSCCH_RESET_VAL                                                (0x00000000u)
#define MAC_TTSCCH_MMS                                                               (0u)
#define MAC_TTSCCH_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TTSCH_C                  | Transmit Timestamp C Bits 63-32   | 0x00000000  |
// |       |                          | (Seconds).                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TTSCH_C
// Bit position, length and mask
#define BITP_MAC_TTSCCH_TTSCH_C                                                      (0u)
#define BITL_MAC_TTSCCH_TTSCH_C                                                     (32u)
#define BITM_MAC_TTSCCH_TTSCH_C                                             (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TTSCCL
// Register Addr        : 0x0015u
// Register NW Addr     : 0xC015u
// Register Reset Value : 0x00000000u
// Register MMS         : 0u

#define MAC_TTSCCL_ADDR                                                         (0x0015u)
#define MAC_TTSCCL_NW_ADDR                                                      (0xC015u)
#define MAC_TTSCCL_RESET_VAL                                                (0x00000000u)
#define MAC_TTSCCL_MMS                                                               (0u)
#define MAC_TTSCCL_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TTSCL_C                  | Transmit Timestamp C Bits 31-0    | 0x00000000  |
// |       |                          | (NanoSeconds).                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TTSCL_C
// Bit position, length and mask
#define BITP_MAC_TTSCCL_TTSCL_C                                                      (0u)
#define BITL_MAC_TTSCCL_TTSCL_C                                                     (32u)
#define BITM_MAC_TTSCCL_TTSCL_C                                             (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_SPACE
// Register Addr        : 0x0032u
// Register NW Addr     : 0xC032u
// Register Reset Value : 0x000007FFu
// Register MMS         : 1u

#define MAC_TX_SPACE_ADDR                                                       (0x0032u)
#define MAC_TX_SPACE_NW_ADDR                                                    (0xC032u)
#define MAC_TX_SPACE_RESET_VAL                                              (0x000007FFu)
#define MAC_TX_SPACE_MMS                                                             (1u)
#define MAC_TX_SPACE_VOL_MASK                                               (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | TX_SPACE                 | Transmit FIFO Space Available in  | 0x000007ff  |
// |       |                          | Half Words (16 Bits).             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_SPACE
// Bit position, length and mask
#define BITP_MAC_TX_SPACE_TX_SPACE                                                   (0u)
#define BITL_MAC_TX_SPACE_TX_SPACE                                                  (14u)
#define BITM_MAC_TX_SPACE_TX_SPACE                                          (0x00003FFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_THRESH
// Register Addr        : 0x0033u
// Register NW Addr     : 0xC033u
// Register Reset Value : 0x00000004u
// Register MMS         : 1u

#define MAC_RX_THRESH_ADDR                                                      (0x0033u)
#define MAC_RX_THRESH_NW_ADDR                                                   (0xC033u)
#define MAC_RX_THRESH_RESET_VAL                                             (0x00000004u)
#define MAC_RX_THRESH_MMS                                                            (1u)
#define MAC_RX_THRESH_VOL_MASK                                              (0x0000003Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 05-00 | RX_THRESH                | Receive Threshold in Cut          | 0x00000004  |
// |       |                          | Through.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_THRESH
// Bit position, length and mask
#define BITP_MAC_RX_THRESH_RX_THRESH                                                 (0u)
#define BITL_MAC_RX_THRESH_RX_THRESH                                                 (6u)
#define BITM_MAC_RX_THRESH_RX_THRESH                                        (0x0000003Fu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : FIFO_CLR
// Register Addr        : 0x0036u
// Register NW Addr     : 0xC036u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_FIFO_CLR_ADDR                                                       (0x0036u)
#define MAC_FIFO_CLR_NW_ADDR                                                    (0xC036u)
#define MAC_FIFO_CLR_RESET_VAL                                              (0x00000000u)
#define MAC_FIFO_CLR_MMS                                                             (1u)
#define MAC_FIFO_CLR_VOL_MASK                                               (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | MAC_RXF_CLR              | Clear the Receive FIFO(s).        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | ALL_FIFOS_CLR            | Clear All FIFOs.                  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_RXF_CLR
// Bit position, length and mask
#define BITP_MAC_FIFO_CLR_MAC_RXF_CLR                                                (0u)
#define BITL_MAC_FIFO_CLR_MAC_RXF_CLR                                                (1u)
#define BITM_MAC_FIFO_CLR_MAC_RXF_CLR                                       (0x00000001u)

// Bit field: ALL_FIFOS_CLR
// Bit position, length and mask
#define BITP_MAC_FIFO_CLR_ALL_FIFOS_CLR                                              (2u)
#define BITL_MAC_FIFO_CLR_ALL_FIFOS_CLR                                              (1u)
#define BITM_MAC_FIFO_CLR_ALL_FIFOS_CLR                                     (0x00000004u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SCRATCH_0
// Register Addr        : 0x0037u
// Register NW Addr     : 0xC037u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_SCRATCH_0_ADDR                                                      (0x0037u)
#define MAC_SCRATCH_0_NW_ADDR                                                   (0xC037u)
#define MAC_SCRATCH_0_RESET_VAL                                             (0x00000000u)
#define MAC_SCRATCH_0_MMS                                                            (1u)
#define MAC_SCRATCH_0_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | SCRATCH_DATA             | Scratch Data.                     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: SCRATCH_DATA
// Bit position, length and mask
#define BITP_MAC_SCRATCH_0_SCRATCH_DATA                                              (0u)
#define BITL_MAC_SCRATCH_0_SCRATCH_DATA                                             (32u)
#define BITM_MAC_SCRATCH_0_SCRATCH_DATA                                     (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SCRATCH_1
// Register Addr        : 0x0038u
// Register NW Addr     : 0xC038u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_SCRATCH_1_ADDR                                                      (0x0038u)
#define MAC_SCRATCH_1_NW_ADDR                                                   (0xC038u)
#define MAC_SCRATCH_1_RESET_VAL                                             (0x00000000u)
#define MAC_SCRATCH_1_MMS                                                            (1u)
#define MAC_SCRATCH_1_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | SCRATCH_DATA             | Scratch Data.                     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: SCRATCH_DATA
// Bit position, length and mask
#define BITP_MAC_SCRATCH_1_SCRATCH_DATA                                              (0u)
#define BITL_MAC_SCRATCH_1_SCRATCH_DATA                                             (32u)
#define BITM_MAC_SCRATCH_1_SCRATCH_DATA                                     (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SCRATCH_2
// Register Addr        : 0x0039u
// Register NW Addr     : 0xC039u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_SCRATCH_2_ADDR                                                      (0x0039u)
#define MAC_SCRATCH_2_NW_ADDR                                                   (0xC039u)
#define MAC_SCRATCH_2_RESET_VAL                                             (0x00000000u)
#define MAC_SCRATCH_2_MMS                                                            (1u)
#define MAC_SCRATCH_2_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | SCRATCH_DATA             | Scratch Data.                     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: SCRATCH_DATA
// Bit position, length and mask
#define BITP_MAC_SCRATCH_2_SCRATCH_DATA                                              (0u)
#define BITL_MAC_SCRATCH_2_SCRATCH_DATA                                             (32u)
#define BITM_MAC_SCRATCH_2_SCRATCH_DATA                                     (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SCRATCH_3
// Register Addr        : 0x003Au
// Register NW Addr     : 0xC03Au
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_SCRATCH_3_ADDR                                                      (0x003Au)
#define MAC_SCRATCH_3_NW_ADDR                                                   (0xC03Au)
#define MAC_SCRATCH_3_RESET_VAL                                             (0x00000000u)
#define MAC_SCRATCH_3_MMS                                                            (1u)
#define MAC_SCRATCH_3_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | SCRATCH_DATA             | Scratch Data.                     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: SCRATCH_DATA
// Bit position, length and mask
#define BITP_MAC_SCRATCH_3_SCRATCH_DATA                                              (0u)
#define BITL_MAC_SCRATCH_3_SCRATCH_DATA                                             (32u)
#define BITM_MAC_SCRATCH_3_SCRATCH_DATA                                     (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : SPI_INJ_ERR
// Register Addr        : 0x003Du
// Register NW Addr     : 0xC03Du
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_SPI_INJ_ERR_ADDR                                                    (0x003Du)
#define MAC_SPI_INJ_ERR_NW_ADDR                                                 (0xC03Du)
#define MAC_SPI_INJ_ERR_RESET_VAL                                           (0x00000000u)
#define MAC_SPI_INJ_ERR_MMS                                                          (1u)
#define MAC_SPI_INJ_ERR_VOL_MASK                                            (0x00000001u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | TEST_SPI_INJ_ERR         | Inject an Error on the SPI MISO   | 0x00000000  |
// |       |                          | Path.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TEST_SPI_INJ_ERR
// Bit position, length and mask
#define BITP_MAC_SPI_INJ_ERR_TEST_SPI_INJ_ERR                                        (0u)
#define BITL_MAC_SPI_INJ_ERR_TEST_SPI_INJ_ERR                                        (1u)
#define BITM_MAC_SPI_INJ_ERR_TEST_SPI_INJ_ERR                               (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : FIFO_SIZE
// Register Addr        : 0x003Eu
// Register NW Addr     : 0xC03Eu
// Register Reset Value : 0x01102022u
// Register MMS         : 1u

#define MAC_FIFO_SIZE_ADDR                                                      (0x003Eu)
#define MAC_FIFO_SIZE_NW_ADDR                                                   (0xC03Eu)
#define MAC_FIFO_SIZE_RESET_VAL                                             (0x01102022u)
#define MAC_FIFO_SIZE_MMS                                                            (1u)
#define MAC_FIFO_SIZE_VOL_MASK                                              (0x0FFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | HOST_2_T1S_FIFO_SIZE     | HOST to T1S FIFO Size             | 0x00000002  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 07-04 | T1S_2_HOST_SIZE          | T1S to HOST FIFO Size             | 0x00000002  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-12 | LCE_2_HOST_FIFO_SIZE     | LCE to HOST FIFO Size             | 0x00000002  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 23-20 | LCE_2_T1S_SIZE           | HOST to T1S FIFO Size             | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-24 | T1S_2_LCE_SIZE           | T1S to LCE FIFO Size              | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: HOST_2_T1S_FIFO_SIZE
// Bit position, length and mask
#define BITP_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE                                      (0u)
#define BITL_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE                                      (4u)
#define BITM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE                             (0x0000000Fu)
// Enums
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_0K                   (0x00000000u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_2K                   (0x00000001u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_4K                   (0x00000002u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_6K                   (0x00000003u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_8K                   (0x00000004u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_10K                  (0x00000005u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_12K                  (0x00000006u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_14K                  (0x00000007u)
#define ENUM_MAC_FIFO_SIZE_HOST_2_T1S_FIFO_SIZE_TXSIZE_16K                  (0x00000008u)

// Bit field: T1S_2_HOST_SIZE
// Bit position, length and mask
#define BITP_MAC_FIFO_SIZE_T1S_2_HOST_SIZE                                           (4u)
#define BITL_MAC_FIFO_SIZE_T1S_2_HOST_SIZE                                           (4u)
#define BITM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE                                  (0x000000F0u)
// Enums
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_0K                        (0x00000000u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_2K                        (0x00000001u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_4K                        (0x00000002u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_6K                        (0x00000003u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_8K                        (0x00000004u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_10K                       (0x00000005u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_12K                       (0x00000006u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_14K                       (0x00000007u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_HOST_SIZE_RXSIZE_16K                       (0x00000008u)

// Bit field: LCE_2_HOST_FIFO_SIZE
// Bit position, length and mask
#define BITP_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE                                     (12u)
#define BITL_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE                                      (4u)
#define BITM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE                             (0x0000F000u)
// Enums
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_0K                   (0x00000000u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_2K                   (0x00000001u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_4K                   (0x00000002u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_6K                   (0x00000003u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_8K                   (0x00000004u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_10K                  (0x00000005u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_12K                  (0x00000006u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_14K                  (0x00000007u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_HOST_FIFO_SIZE_RXSIZE_16K                  (0x00000008u)

// Bit field: LCE_2_T1S_SIZE
// Bit position, length and mask
#define BITP_MAC_FIFO_SIZE_LCE_2_T1S_SIZE                                           (20u)
#define BITL_MAC_FIFO_SIZE_LCE_2_T1S_SIZE                                            (4u)
#define BITM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE                                   (0x00F00000u)
// Enums
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_0K                         (0x00000000u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_2K                         (0x00000001u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_4K                         (0x00000002u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_6K                         (0x00000003u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_8K                         (0x00000004u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_10K                        (0x00000005u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_12K                        (0x00000006u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_14K                        (0x00000007u)
#define ENUM_MAC_FIFO_SIZE_LCE_2_T1S_SIZE_TXSIZE_16K                        (0x00000008u)

// Bit field: T1S_2_LCE_SIZE
// Bit position, length and mask
#define BITP_MAC_FIFO_SIZE_T1S_2_LCE_SIZE                                           (24u)
#define BITL_MAC_FIFO_SIZE_T1S_2_LCE_SIZE                                            (4u)
#define BITM_MAC_FIFO_SIZE_T1S_2_LCE_SIZE                                   (0x0F000000u)
// Enums
#define ENUM_MAC_FIFO_SIZE_T1S_2_LCE_SIZE_TXSIZE_0K                         (0x00000000u)
#define ENUM_MAC_FIFO_SIZE_T1S_2_LCE_SIZE_TXSIZE_2K                         (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TFC
// Register Addr        : 0x003Fu
// Register NW Addr     : 0xC03Fu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TFC_ADDR                                                            (0x003Fu)
#define MAC_TFC_NW_ADDR                                                         (0xC03Fu)
#define MAC_TFC_RESET_VAL                                                   (0x00000000u)
#define MAC_TFC_MMS                                                                  (1u)
#define MAC_TFC_VOL_MASK                                                    (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 08-00 | TFC                      | Number of Frames in the Tx FIFO.  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TFC
// Bit position, length and mask
#define BITP_MAC_TFC_TFC                                                             (0u)
#define BITL_MAC_TFC_TFC                                                             (9u)
#define BITM_MAC_TFC_TFC                                                    (0x000001FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TXSIZE
// Register Addr        : 0x0040u
// Register NW Addr     : 0xC040u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TXSIZE_ADDR                                                         (0x0040u)
#define MAC_TXSIZE_NW_ADDR                                                      (0xC040u)
#define MAC_TXSIZE_RESET_VAL                                                (0x00000000u)
#define MAC_TXSIZE_MMS                                                               (1u)
#define MAC_TXSIZE_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | TX_SIZE                  | Data in the Tx FIFO. Number of    | 0x00000000  |
// |       |                          | Half Words (16 Bit).              |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_SIZE
// Bit position, length and mask
#define BITP_MAC_TXSIZE_TX_SIZE                                                      (0u)
#define BITL_MAC_TXSIZE_TX_SIZE                                                     (14u)
#define BITM_MAC_TXSIZE_TX_SIZE                                             (0x00003FFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : HTX_OVF_FRM_CNT
// Register Addr        : 0x0041u
// Register NW Addr     : 0xC041u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_HTX_OVF_FRM_CNT_ADDR                                                (0x0041u)
#define MAC_HTX_OVF_FRM_CNT_NW_ADDR                                             (0xC041u)
#define MAC_HTX_OVF_FRM_CNT_RESET_VAL                                       (0x00000000u)
#define MAC_HTX_OVF_FRM_CNT_MMS                                                      (1u)
#define MAC_HTX_OVF_FRM_CNT_VOL_MASK                                        (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | HTX_OVF_FRM_CNT          | Counts Host Tx Frames Dropped     | 0x00000000  |
// |       |                          | Due to FIFO Overflow.             |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: HTX_OVF_FRM_CNT
// Bit position, length and mask
#define BITP_MAC_HTX_OVF_FRM_CNT_HTX_OVF_FRM_CNT                                     (0u)
#define BITL_MAC_HTX_OVF_FRM_CNT_HTX_OVF_FRM_CNT                                    (24u)
#define BITM_MAC_HTX_OVF_FRM_CNT_HTX_OVF_FRM_CNT                            (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MECC_ERR_ADDR
// Register Addr        : 0x0042u
// Register NW Addr     : 0xC042u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_MECC_ERR_ADDR_ADDR                                                  (0x0042u)
#define MAC_MECC_ERR_ADDR_NW_ADDR                                               (0xC042u)
#define MAC_MECC_ERR_ADDR_RESET_VAL                                         (0x00000000u)
#define MAC_MECC_ERR_ADDR_MMS                                                        (1u)
#define MAC_MECC_ERR_ADDR_VOL_MASK                                          (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | MECC_ERR_ADDR            | Address of an Uncorrectable ECC   | 0x00000000  |
// |       |                          | Error in Memory.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MECC_ERR_ADDR
// Bit position, length and mask
#define BITP_MAC_MECC_ERR_ADDR_MECC_ERR_ADDR                                         (0u)
#define BITL_MAC_MECC_ERR_ADDR_MECC_ERR_ADDR                                        (14u)
#define BITM_MAC_MECC_ERR_ADDR_MECC_ERR_ADDR                                (0x00003FFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CECC_ERR_0
// Register Addr        : 0x0043u
// Register NW Addr     : 0xC043u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_CECC_ERR_0_ADDR                                                     (0x0043u)
#define MAC_CECC_ERR_0_NW_ADDR                                                  (0xC043u)
#define MAC_CECC_ERR_0_RESET_VAL                                            (0x00000000u)
#define MAC_CECC_ERR_0_MMS                                                           (1u)
#define MAC_CECC_ERR_0_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | CECC_ERR_CNT             | Corrected ECC Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CECC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_CECC_ERR_0_CECC_ERR_CNT                                             (0u)
#define BITL_MAC_CECC_ERR_0_CECC_ERR_CNT                                            (10u)
#define BITM_MAC_CECC_ERR_0_CECC_ERR_CNT                                    (0x000003FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CECC_ERR_1
// Register Addr        : 0x0044u
// Register NW Addr     : 0xC044u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_CECC_ERR_1_ADDR                                                     (0x0044u)
#define MAC_CECC_ERR_1_NW_ADDR                                                  (0xC044u)
#define MAC_CECC_ERR_1_RESET_VAL                                            (0x00000000u)
#define MAC_CECC_ERR_1_MMS                                                           (1u)
#define MAC_CECC_ERR_1_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | CECC_ERR_CNT             | Corrected ECC Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CECC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_CECC_ERR_1_CECC_ERR_CNT                                             (0u)
#define BITL_MAC_CECC_ERR_1_CECC_ERR_CNT                                            (10u)
#define BITM_MAC_CECC_ERR_1_CECC_ERR_CNT                                    (0x000003FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CECC_ERR_2
// Register Addr        : 0x0045u
// Register NW Addr     : 0xC045u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_CECC_ERR_2_ADDR                                                     (0x0045u)
#define MAC_CECC_ERR_2_NW_ADDR                                                  (0xC045u)
#define MAC_CECC_ERR_2_RESET_VAL                                            (0x00000000u)
#define MAC_CECC_ERR_2_MMS                                                           (1u)
#define MAC_CECC_ERR_2_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | CECC_ERR_CNT             | Corrected ECC Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CECC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_CECC_ERR_2_CECC_ERR_CNT                                             (0u)
#define BITL_MAC_CECC_ERR_2_CECC_ERR_CNT                                            (10u)
#define BITM_MAC_CECC_ERR_2_CECC_ERR_CNT                                    (0x000003FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CECC_ERR_3
// Register Addr        : 0x0046u
// Register NW Addr     : 0xC046u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_CECC_ERR_3_ADDR                                                     (0x0046u)
#define MAC_CECC_ERR_3_NW_ADDR                                                  (0xC046u)
#define MAC_CECC_ERR_3_RESET_VAL                                            (0x00000000u)
#define MAC_CECC_ERR_3_MMS                                                           (1u)
#define MAC_CECC_ERR_3_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | CECC_ERR_CNT             | Corrected ECC Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CECC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_CECC_ERR_3_CECC_ERR_CNT                                             (0u)
#define BITL_MAC_CECC_ERR_3_CECC_ERR_CNT                                            (10u)
#define BITM_MAC_CECC_ERR_3_CECC_ERR_CNT                                    (0x000003FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CECC_ERR_4
// Register Addr        : 0x0047u
// Register NW Addr     : 0xC047u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_CECC_ERR_4_ADDR                                                     (0x0047u)
#define MAC_CECC_ERR_4_NW_ADDR                                                  (0xC047u)
#define MAC_CECC_ERR_4_RESET_VAL                                            (0x00000000u)
#define MAC_CECC_ERR_4_MMS                                                           (1u)
#define MAC_CECC_ERR_4_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | CECC_ERR_CNT             | Corrected ECC Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CECC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_CECC_ERR_4_CECC_ERR_CNT                                             (0u)
#define BITL_MAC_CECC_ERR_4_CECC_ERR_CNT                                            (10u)
#define BITM_MAC_CECC_ERR_4_CECC_ERR_CNT                                    (0x000003FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CECC_ERR_5
// Register Addr        : 0x0048u
// Register NW Addr     : 0xC048u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_CECC_ERR_5_ADDR                                                     (0x0048u)
#define MAC_CECC_ERR_5_NW_ADDR                                                  (0xC048u)
#define MAC_CECC_ERR_5_RESET_VAL                                            (0x00000000u)
#define MAC_CECC_ERR_5_MMS                                                           (1u)
#define MAC_CECC_ERR_5_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | CECC_ERR_CNT             | Corrected ECC Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CECC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_CECC_ERR_5_CECC_ERR_CNT                                             (0u)
#define BITL_MAC_CECC_ERR_5_CECC_ERR_CNT                                            (10u)
#define BITM_MAC_CECC_ERR_5_CECC_ERR_CNT                                    (0x000003FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : CECC_ERR_6
// Register Addr        : 0x0049u
// Register NW Addr     : 0xC049u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_CECC_ERR_6_ADDR                                                     (0x0049u)
#define MAC_CECC_ERR_6_NW_ADDR                                                  (0xC049u)
#define MAC_CECC_ERR_6_RESET_VAL                                            (0x00000000u)
#define MAC_CECC_ERR_6_MMS                                                           (1u)
#define MAC_CECC_ERR_6_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 09-00 | CECC_ERR_CNT             | Corrected ECC Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CECC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_CECC_ERR_6_CECC_ERR_CNT                                             (0u)
#define BITL_MAC_CECC_ERR_6_CECC_ERR_CNT                                            (10u)
#define BITM_MAC_CECC_ERR_6_CECC_ERR_CNT                                    (0x000003FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : FIFO_ALMOST_EMPTY
// Register Addr        : 0x004Cu
// Register NW Addr     : 0xC04Cu
// Register Reset Value : 0x00000004u
// Register MMS         : 1u

#define MAC_FIFO_ALMOST_EMPTY_ADDR                                              (0x004Cu)
#define MAC_FIFO_ALMOST_EMPTY_NW_ADDR                                           (0xC04Cu)
#define MAC_FIFO_ALMOST_EMPTY_RESET_VAL                                     (0x00000004u)
#define MAC_FIFO_ALMOST_EMPTY_MMS                                                    (1u)
#define MAC_FIFO_ALMOST_EMPTY_VOL_MASK                                      (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | AE_THRESH                | Almost Empty Threshold Value      | 0x00000004  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: AE_THRESH
// Bit position, length and mask
#define BITP_MAC_FIFO_ALMOST_EMPTY_AE_THRESH                                         (0u)
#define BITL_MAC_FIFO_ALMOST_EMPTY_AE_THRESH                                         (8u)
#define BITM_MAC_FIFO_ALMOST_EMPTY_AE_THRESH                                (0x000000FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_0
// Register Addr        : 0x0050u
// Register NW Addr     : 0xC050u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_0_ADDR                                                (0x0050u)
#define MAC_ADDR_FILT_UPR_0_NW_ADDR                                             (0xC050u)
#define MAC_ADDR_FILT_UPR_0_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_0_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_0_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_0_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_0_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_0_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_0_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_0_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_0_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_0_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_0_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_0_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_0_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_0_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_0_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_0_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_0_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_0_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_0_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_0_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_0_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_0
// Register Addr        : 0x0051u
// Register NW Addr     : 0xC051u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_0_ADDR                                                (0x0051u)
#define MAC_ADDR_FILT_LWR_0_NW_ADDR                                             (0xC051u)
#define MAC_ADDR_FILT_LWR_0_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_0_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_0_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_0_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_0_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_0_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_1
// Register Addr        : 0x0052u
// Register NW Addr     : 0xC052u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_1_ADDR                                                (0x0052u)
#define MAC_ADDR_FILT_UPR_1_NW_ADDR                                             (0xC052u)
#define MAC_ADDR_FILT_UPR_1_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_1_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_1_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_1_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_1_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_1_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_1_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_1_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_1_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_1_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_1_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_1_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_1_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_1_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_1_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_1_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_1_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_1_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_1_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_1_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_1_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_1
// Register Addr        : 0x0053u
// Register NW Addr     : 0xC053u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_1_ADDR                                                (0x0053u)
#define MAC_ADDR_FILT_LWR_1_NW_ADDR                                             (0xC053u)
#define MAC_ADDR_FILT_LWR_1_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_1_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_1_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_1_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_1_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_1_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_2
// Register Addr        : 0x0054u
// Register NW Addr     : 0xC054u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_2_ADDR                                                (0x0054u)
#define MAC_ADDR_FILT_UPR_2_NW_ADDR                                             (0xC054u)
#define MAC_ADDR_FILT_UPR_2_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_2_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_2_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_2_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_2_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_2_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_2_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_2_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_2_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_2_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_2_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_2_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_2_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_2_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_2_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_2_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_2_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_2_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_2_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_2_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_2_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_2
// Register Addr        : 0x0055u
// Register NW Addr     : 0xC055u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_2_ADDR                                                (0x0055u)
#define MAC_ADDR_FILT_LWR_2_NW_ADDR                                             (0xC055u)
#define MAC_ADDR_FILT_LWR_2_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_2_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_2_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_2_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_2_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_2_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_3
// Register Addr        : 0x0056u
// Register NW Addr     : 0xC056u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_3_ADDR                                                (0x0056u)
#define MAC_ADDR_FILT_UPR_3_NW_ADDR                                             (0xC056u)
#define MAC_ADDR_FILT_UPR_3_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_3_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_3_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_3_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_3_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_3_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_3_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_3_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_3_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_3_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_3_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_3_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_3_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_3_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_3_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_3_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_3_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_3_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_3_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_3_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_3_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_3
// Register Addr        : 0x0057u
// Register NW Addr     : 0xC057u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_3_ADDR                                                (0x0057u)
#define MAC_ADDR_FILT_LWR_3_NW_ADDR                                             (0xC057u)
#define MAC_ADDR_FILT_LWR_3_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_3_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_3_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_3_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_3_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_3_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_4
// Register Addr        : 0x0058u
// Register NW Addr     : 0xC058u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_4_ADDR                                                (0x0058u)
#define MAC_ADDR_FILT_UPR_4_NW_ADDR                                             (0xC058u)
#define MAC_ADDR_FILT_UPR_4_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_4_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_4_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_4_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_4_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_4_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_4_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_4_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_4_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_4_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_4_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_4_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_4_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_4_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_4_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_4_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_4_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_4_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_4_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_4_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_4_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_4
// Register Addr        : 0x0059u
// Register NW Addr     : 0xC059u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_4_ADDR                                                (0x0059u)
#define MAC_ADDR_FILT_LWR_4_NW_ADDR                                             (0xC059u)
#define MAC_ADDR_FILT_LWR_4_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_4_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_4_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_4_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_4_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_4_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_5
// Register Addr        : 0x005Au
// Register NW Addr     : 0xC05Au
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_5_ADDR                                                (0x005Au)
#define MAC_ADDR_FILT_UPR_5_NW_ADDR                                             (0xC05Au)
#define MAC_ADDR_FILT_UPR_5_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_5_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_5_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_5_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_5_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_5_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_5_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_5_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_5_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_5_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_5_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_5_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_5_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_5_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_5_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_5_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_5_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_5_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_5_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_5_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_5_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_5
// Register Addr        : 0x005Bu
// Register NW Addr     : 0xC05Bu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_5_ADDR                                                (0x005Bu)
#define MAC_ADDR_FILT_LWR_5_NW_ADDR                                             (0xC05Bu)
#define MAC_ADDR_FILT_LWR_5_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_5_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_5_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_5_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_5_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_5_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_6
// Register Addr        : 0x005Cu
// Register NW Addr     : 0xC05Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_6_ADDR                                                (0x005Cu)
#define MAC_ADDR_FILT_UPR_6_NW_ADDR                                             (0xC05Cu)
#define MAC_ADDR_FILT_UPR_6_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_6_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_6_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_6_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_6_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_6_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_6_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_6_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_6_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_6_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_6_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_6_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_6_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_6_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_6_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_6_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_6_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_6_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_6_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_6_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_6_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_6
// Register Addr        : 0x005Du
// Register NW Addr     : 0xC05Du
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_6_ADDR                                                (0x005Du)
#define MAC_ADDR_FILT_LWR_6_NW_ADDR                                             (0xC05Du)
#define MAC_ADDR_FILT_LWR_6_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_6_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_6_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_6_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_6_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_6_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_7
// Register Addr        : 0x005Eu
// Register NW Addr     : 0xC05Eu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_7_ADDR                                                (0x005Eu)
#define MAC_ADDR_FILT_UPR_7_NW_ADDR                                             (0xC05Eu)
#define MAC_ADDR_FILT_UPR_7_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_7_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_7_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_7_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_7_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_7_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_7_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_7_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_7_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_7_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_7_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_7_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_7_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_7_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_7_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_7_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_7_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_7_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_7_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_7_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_7_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_7
// Register Addr        : 0x005Fu
// Register NW Addr     : 0xC05Fu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_7_ADDR                                                (0x005Fu)
#define MAC_ADDR_FILT_LWR_7_NW_ADDR                                             (0xC05Fu)
#define MAC_ADDR_FILT_LWR_7_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_7_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_7_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_7_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_7_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_7_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_8
// Register Addr        : 0x0060u
// Register NW Addr     : 0xC060u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_8_ADDR                                                (0x0060u)
#define MAC_ADDR_FILT_UPR_8_NW_ADDR                                             (0xC060u)
#define MAC_ADDR_FILT_UPR_8_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_8_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_8_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_8_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_8_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_8_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_8_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_8_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_8_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_8_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_8_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_8_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_8_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_8_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_8_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_8_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_8_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_8_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_8_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_8_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_8_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_8
// Register Addr        : 0x0061u
// Register NW Addr     : 0xC061u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_8_ADDR                                                (0x0061u)
#define MAC_ADDR_FILT_LWR_8_NW_ADDR                                             (0xC061u)
#define MAC_ADDR_FILT_LWR_8_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_8_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_8_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_8_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_8_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_8_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_9
// Register Addr        : 0x0062u
// Register NW Addr     : 0xC062u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_9_ADDR                                                (0x0062u)
#define MAC_ADDR_FILT_UPR_9_NW_ADDR                                             (0xC062u)
#define MAC_ADDR_FILT_UPR_9_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_UPR_9_MMS                                                      (1u)
#define MAC_ADDR_FILT_UPR_9_VOL_MASK                                        (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_9_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_UPR_9_MAC_ADDR                                           (16u)
#define BITM_MAC_ADDR_FILT_UPR_9_MAC_ADDR                                   (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_9_TO_HOST                                            (16u)
#define BITL_MAC_ADDR_FILT_UPR_9_TO_HOST                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_9_TO_HOST                                    (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_9_TO_HOST_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_9_TO_HOST_FORWARD                            (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_9_TO_LCE                                             (17u)
#define BITL_MAC_ADDR_FILT_UPR_9_TO_LCE                                              (1u)
#define BITM_MAC_ADDR_FILT_UPR_9_TO_LCE                                     (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_9_TO_LCE_DROP                                (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_9_TO_LCE_FORWARD                             (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_9_APPLY2PORT                                         (30u)
#define BITL_MAC_ADDR_FILT_UPR_9_APPLY2PORT                                          (1u)
#define BITM_MAC_ADDR_FILT_UPR_9_APPLY2PORT                                 (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_9_APPLY2PORT_NOTAPPLY                        (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_9_APPLY2PORT_APPLY                           (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_9
// Register Addr        : 0x0063u
// Register NW Addr     : 0xC063u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_9_ADDR                                                (0x0063u)
#define MAC_ADDR_FILT_LWR_9_NW_ADDR                                             (0xC063u)
#define MAC_ADDR_FILT_LWR_9_RESET_VAL                                       (0x00000000u)
#define MAC_ADDR_FILT_LWR_9_MMS                                                      (1u)
#define MAC_ADDR_FILT_LWR_9_VOL_MASK                                        (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_9_MAC_ADDR                                            (0u)
#define BITL_MAC_ADDR_FILT_LWR_9_MAC_ADDR                                           (32u)
#define BITM_MAC_ADDR_FILT_LWR_9_MAC_ADDR                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_10
// Register Addr        : 0x0064u
// Register NW Addr     : 0xC064u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_10_ADDR                                               (0x0064u)
#define MAC_ADDR_FILT_UPR_10_NW_ADDR                                            (0xC064u)
#define MAC_ADDR_FILT_UPR_10_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_UPR_10_MMS                                                     (1u)
#define MAC_ADDR_FILT_UPR_10_VOL_MASK                                       (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_10_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_UPR_10_MAC_ADDR                                          (16u)
#define BITM_MAC_ADDR_FILT_UPR_10_MAC_ADDR                                  (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_10_TO_HOST                                           (16u)
#define BITL_MAC_ADDR_FILT_UPR_10_TO_HOST                                            (1u)
#define BITM_MAC_ADDR_FILT_UPR_10_TO_HOST                                   (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_10_TO_HOST_DROP                              (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_10_TO_HOST_FORWARD                           (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_10_TO_LCE                                            (17u)
#define BITL_MAC_ADDR_FILT_UPR_10_TO_LCE                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_10_TO_LCE                                    (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_10_TO_LCE_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_10_TO_LCE_FORWARD                            (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_10_APPLY2PORT                                        (30u)
#define BITL_MAC_ADDR_FILT_UPR_10_APPLY2PORT                                         (1u)
#define BITM_MAC_ADDR_FILT_UPR_10_APPLY2PORT                                (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_10_APPLY2PORT_NOTAPPLY                       (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_10_APPLY2PORT_APPLY                          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_10
// Register Addr        : 0x0065u
// Register NW Addr     : 0xC065u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_10_ADDR                                               (0x0065u)
#define MAC_ADDR_FILT_LWR_10_NW_ADDR                                            (0xC065u)
#define MAC_ADDR_FILT_LWR_10_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_LWR_10_MMS                                                     (1u)
#define MAC_ADDR_FILT_LWR_10_VOL_MASK                                       (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_10_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_LWR_10_MAC_ADDR                                          (32u)
#define BITM_MAC_ADDR_FILT_LWR_10_MAC_ADDR                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_11
// Register Addr        : 0x0066u
// Register NW Addr     : 0xC066u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_11_ADDR                                               (0x0066u)
#define MAC_ADDR_FILT_UPR_11_NW_ADDR                                            (0xC066u)
#define MAC_ADDR_FILT_UPR_11_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_UPR_11_MMS                                                     (1u)
#define MAC_ADDR_FILT_UPR_11_VOL_MASK                                       (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_11_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_UPR_11_MAC_ADDR                                          (16u)
#define BITM_MAC_ADDR_FILT_UPR_11_MAC_ADDR                                  (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_11_TO_HOST                                           (16u)
#define BITL_MAC_ADDR_FILT_UPR_11_TO_HOST                                            (1u)
#define BITM_MAC_ADDR_FILT_UPR_11_TO_HOST                                   (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_11_TO_HOST_DROP                              (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_11_TO_HOST_FORWARD                           (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_11_TO_LCE                                            (17u)
#define BITL_MAC_ADDR_FILT_UPR_11_TO_LCE                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_11_TO_LCE                                    (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_11_TO_LCE_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_11_TO_LCE_FORWARD                            (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_11_APPLY2PORT                                        (30u)
#define BITL_MAC_ADDR_FILT_UPR_11_APPLY2PORT                                         (1u)
#define BITM_MAC_ADDR_FILT_UPR_11_APPLY2PORT                                (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_11_APPLY2PORT_NOTAPPLY                       (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_11_APPLY2PORT_APPLY                          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_11
// Register Addr        : 0x0067u
// Register NW Addr     : 0xC067u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_11_ADDR                                               (0x0067u)
#define MAC_ADDR_FILT_LWR_11_NW_ADDR                                            (0xC067u)
#define MAC_ADDR_FILT_LWR_11_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_LWR_11_MMS                                                     (1u)
#define MAC_ADDR_FILT_LWR_11_VOL_MASK                                       (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_11_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_LWR_11_MAC_ADDR                                          (32u)
#define BITM_MAC_ADDR_FILT_LWR_11_MAC_ADDR                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_12
// Register Addr        : 0x0068u
// Register NW Addr     : 0xC068u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_12_ADDR                                               (0x0068u)
#define MAC_ADDR_FILT_UPR_12_NW_ADDR                                            (0xC068u)
#define MAC_ADDR_FILT_UPR_12_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_UPR_12_MMS                                                     (1u)
#define MAC_ADDR_FILT_UPR_12_VOL_MASK                                       (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_12_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_UPR_12_MAC_ADDR                                          (16u)
#define BITM_MAC_ADDR_FILT_UPR_12_MAC_ADDR                                  (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_12_TO_HOST                                           (16u)
#define BITL_MAC_ADDR_FILT_UPR_12_TO_HOST                                            (1u)
#define BITM_MAC_ADDR_FILT_UPR_12_TO_HOST                                   (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_12_TO_HOST_DROP                              (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_12_TO_HOST_FORWARD                           (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_12_TO_LCE                                            (17u)
#define BITL_MAC_ADDR_FILT_UPR_12_TO_LCE                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_12_TO_LCE                                    (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_12_TO_LCE_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_12_TO_LCE_FORWARD                            (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_12_APPLY2PORT                                        (30u)
#define BITL_MAC_ADDR_FILT_UPR_12_APPLY2PORT                                         (1u)
#define BITM_MAC_ADDR_FILT_UPR_12_APPLY2PORT                                (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_12_APPLY2PORT_NOTAPPLY                       (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_12_APPLY2PORT_APPLY                          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_12
// Register Addr        : 0x0069u
// Register NW Addr     : 0xC069u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_12_ADDR                                               (0x0069u)
#define MAC_ADDR_FILT_LWR_12_NW_ADDR                                            (0xC069u)
#define MAC_ADDR_FILT_LWR_12_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_LWR_12_MMS                                                     (1u)
#define MAC_ADDR_FILT_LWR_12_VOL_MASK                                       (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_12_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_LWR_12_MAC_ADDR                                          (32u)
#define BITM_MAC_ADDR_FILT_LWR_12_MAC_ADDR                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_13
// Register Addr        : 0x006Au
// Register NW Addr     : 0xC06Au
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_13_ADDR                                               (0x006Au)
#define MAC_ADDR_FILT_UPR_13_NW_ADDR                                            (0xC06Au)
#define MAC_ADDR_FILT_UPR_13_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_UPR_13_MMS                                                     (1u)
#define MAC_ADDR_FILT_UPR_13_VOL_MASK                                       (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_13_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_UPR_13_MAC_ADDR                                          (16u)
#define BITM_MAC_ADDR_FILT_UPR_13_MAC_ADDR                                  (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_13_TO_HOST                                           (16u)
#define BITL_MAC_ADDR_FILT_UPR_13_TO_HOST                                            (1u)
#define BITM_MAC_ADDR_FILT_UPR_13_TO_HOST                                   (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_13_TO_HOST_DROP                              (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_13_TO_HOST_FORWARD                           (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_13_TO_LCE                                            (17u)
#define BITL_MAC_ADDR_FILT_UPR_13_TO_LCE                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_13_TO_LCE                                    (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_13_TO_LCE_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_13_TO_LCE_FORWARD                            (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_13_APPLY2PORT                                        (30u)
#define BITL_MAC_ADDR_FILT_UPR_13_APPLY2PORT                                         (1u)
#define BITM_MAC_ADDR_FILT_UPR_13_APPLY2PORT                                (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_13_APPLY2PORT_NOTAPPLY                       (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_13_APPLY2PORT_APPLY                          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_13
// Register Addr        : 0x006Bu
// Register NW Addr     : 0xC06Bu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_13_ADDR                                               (0x006Bu)
#define MAC_ADDR_FILT_LWR_13_NW_ADDR                                            (0xC06Bu)
#define MAC_ADDR_FILT_LWR_13_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_LWR_13_MMS                                                     (1u)
#define MAC_ADDR_FILT_LWR_13_VOL_MASK                                       (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_13_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_LWR_13_MAC_ADDR                                          (32u)
#define BITM_MAC_ADDR_FILT_LWR_13_MAC_ADDR                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_14
// Register Addr        : 0x006Cu
// Register NW Addr     : 0xC06Cu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_14_ADDR                                               (0x006Cu)
#define MAC_ADDR_FILT_UPR_14_NW_ADDR                                            (0xC06Cu)
#define MAC_ADDR_FILT_UPR_14_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_UPR_14_MMS                                                     (1u)
#define MAC_ADDR_FILT_UPR_14_VOL_MASK                                       (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_14_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_UPR_14_MAC_ADDR                                          (16u)
#define BITM_MAC_ADDR_FILT_UPR_14_MAC_ADDR                                  (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_14_TO_HOST                                           (16u)
#define BITL_MAC_ADDR_FILT_UPR_14_TO_HOST                                            (1u)
#define BITM_MAC_ADDR_FILT_UPR_14_TO_HOST                                   (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_14_TO_HOST_DROP                              (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_14_TO_HOST_FORWARD                           (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_14_TO_LCE                                            (17u)
#define BITL_MAC_ADDR_FILT_UPR_14_TO_LCE                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_14_TO_LCE                                    (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_14_TO_LCE_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_14_TO_LCE_FORWARD                            (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_14_APPLY2PORT                                        (30u)
#define BITL_MAC_ADDR_FILT_UPR_14_APPLY2PORT                                         (1u)
#define BITM_MAC_ADDR_FILT_UPR_14_APPLY2PORT                                (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_14_APPLY2PORT_NOTAPPLY                       (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_14_APPLY2PORT_APPLY                          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_14
// Register Addr        : 0x006Du
// Register NW Addr     : 0xC06Du
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_14_ADDR                                               (0x006Du)
#define MAC_ADDR_FILT_LWR_14_NW_ADDR                                            (0xC06Du)
#define MAC_ADDR_FILT_LWR_14_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_LWR_14_MMS                                                     (1u)
#define MAC_ADDR_FILT_LWR_14_VOL_MASK                                       (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_14_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_LWR_14_MAC_ADDR                                          (32u)
#define BITM_MAC_ADDR_FILT_LWR_14_MAC_ADDR                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_UPR_15
// Register Addr        : 0x006Eu
// Register NW Addr     : 0xC06Eu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_UPR_15_ADDR                                               (0x006Eu)
#define MAC_ADDR_FILT_UPR_15_NW_ADDR                                            (0xC06Eu)
#define MAC_ADDR_FILT_UPR_15_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_UPR_15_MMS                                                     (1u)
#define MAC_ADDR_FILT_UPR_15_VOL_MASK                                       (0xC00BFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | TO_HOST                  | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Host.                             |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 17    | TO_LCE                   | Controls Forwarding of Frames     | 0x00000000  |
// |       |                          | Matching This MAC Address to the  |             |
// |       |                          | Other Port.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | APPLY2PORT               | Apply Rule                        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_15_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_UPR_15_MAC_ADDR                                          (16u)
#define BITM_MAC_ADDR_FILT_UPR_15_MAC_ADDR                                  (0x0000FFFFu)

// Bit field: TO_HOST
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_15_TO_HOST                                           (16u)
#define BITL_MAC_ADDR_FILT_UPR_15_TO_HOST                                            (1u)
#define BITM_MAC_ADDR_FILT_UPR_15_TO_HOST                                   (0x00010000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_15_TO_HOST_DROP                              (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_15_TO_HOST_FORWARD                           (0x00000001u)

// Bit field: TO_LCE
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_15_TO_LCE                                            (17u)
#define BITL_MAC_ADDR_FILT_UPR_15_TO_LCE                                             (1u)
#define BITM_MAC_ADDR_FILT_UPR_15_TO_LCE                                    (0x00020000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_15_TO_LCE_DROP                               (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_15_TO_LCE_FORWARD                            (0x00000001u)

// Bit field: APPLY2PORT
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_UPR_15_APPLY2PORT                                        (30u)
#define BITL_MAC_ADDR_FILT_UPR_15_APPLY2PORT                                         (1u)
#define BITM_MAC_ADDR_FILT_UPR_15_APPLY2PORT                                (0x40000000u)
// Enums
#define ENUM_MAC_ADDR_FILT_UPR_15_APPLY2PORT_NOTAPPLY                       (0x00000000u)
#define ENUM_MAC_ADDR_FILT_UPR_15_APPLY2PORT_APPLY                          (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_FILT_LWR_15
// Register Addr        : 0x006Fu
// Register NW Addr     : 0xC06Fu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_ADDR_FILT_LWR_15_ADDR                                               (0x006Fu)
#define MAC_ADDR_FILT_LWR_15_NW_ADDR                                            (0xC06Fu)
#define MAC_ADDR_FILT_LWR_15_RESET_VAL                                      (0x00000000u)
#define MAC_ADDR_FILT_LWR_15_MMS                                                     (1u)
#define MAC_ADDR_FILT_LWR_15_VOL_MASK                                       (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR                 | MAC Address.                      | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR
// Bit position, length and mask
#define BITP_MAC_ADDR_FILT_LWR_15_MAC_ADDR                                           (0u)
#define BITL_MAC_ADDR_FILT_LWR_15_MAC_ADDR                                          (32u)
#define BITM_MAC_ADDR_FILT_LWR_15_MAC_ADDR                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_MSK_UPR_0
// Register Addr        : 0x0070u
// Register NW Addr     : 0xC070u
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 1u

#define MAC_ADDR_MSK_UPR_0_ADDR                                                 (0x0070u)
#define MAC_ADDR_MSK_UPR_0_NW_ADDR                                              (0xC070u)
#define MAC_ADDR_MSK_UPR_0_RESET_VAL                                        (0x0000FFFFu)
#define MAC_ADDR_MSK_UPR_0_MMS                                                       (1u)
#define MAC_ADDR_MSK_UPR_0_VOL_MASK                                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR_MASK            | MAC Address Bit Mask for the      | 0x0000ffff  |
// |       |                          | Address Table.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR_MASK
// Bit position, length and mask
#define BITP_MAC_ADDR_MSK_UPR_0_MAC_ADDR_MASK                                        (0u)
#define BITL_MAC_ADDR_MSK_UPR_0_MAC_ADDR_MASK                                       (16u)
#define BITM_MAC_ADDR_MSK_UPR_0_MAC_ADDR_MASK                               (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_MSK_LWR_0
// Register Addr        : 0x0071u
// Register NW Addr     : 0xC071u
// Register Reset Value : 0xFFFFFFFFu
// Register MMS         : 1u

#define MAC_ADDR_MSK_LWR_0_ADDR                                                 (0x0071u)
#define MAC_ADDR_MSK_LWR_0_NW_ADDR                                              (0xC071u)
#define MAC_ADDR_MSK_LWR_0_RESET_VAL                                        (0xFFFFFFFFu)
#define MAC_ADDR_MSK_LWR_0_MMS                                                       (1u)
#define MAC_ADDR_MSK_LWR_0_VOL_MASK                                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR_MASK            | MAC Address Bit Mask for the      | 0xffffffff  |
// |       |                          | Address Table.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR_MASK
// Bit position, length and mask
#define BITP_MAC_ADDR_MSK_LWR_0_MAC_ADDR_MASK                                        (0u)
#define BITL_MAC_ADDR_MSK_LWR_0_MAC_ADDR_MASK                                       (32u)
#define BITM_MAC_ADDR_MSK_LWR_0_MAC_ADDR_MASK                               (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_MSK_UPR_1
// Register Addr        : 0x0072u
// Register NW Addr     : 0xC072u
// Register Reset Value : 0x0000FFFFu
// Register MMS         : 1u

#define MAC_ADDR_MSK_UPR_1_ADDR                                                 (0x0072u)
#define MAC_ADDR_MSK_UPR_1_NW_ADDR                                              (0xC072u)
#define MAC_ADDR_MSK_UPR_1_RESET_VAL                                        (0x0000FFFFu)
#define MAC_ADDR_MSK_UPR_1_MMS                                                       (1u)
#define MAC_ADDR_MSK_UPR_1_VOL_MASK                                         (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAC_ADDR_MASK            | MAC Address Bit Mask for the      | 0x0000ffff  |
// |       |                          | Address Table.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR_MASK
// Bit position, length and mask
#define BITP_MAC_ADDR_MSK_UPR_1_MAC_ADDR_MASK                                        (0u)
#define BITL_MAC_ADDR_MSK_UPR_1_MAC_ADDR_MASK                                       (16u)
#define BITM_MAC_ADDR_MSK_UPR_1_MAC_ADDR_MASK                               (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : ADDR_MSK_LWR_1
// Register Addr        : 0x0073u
// Register NW Addr     : 0xC073u
// Register Reset Value : 0xFFFFFFFFu
// Register MMS         : 1u

#define MAC_ADDR_MSK_LWR_1_ADDR                                                 (0x0073u)
#define MAC_ADDR_MSK_LWR_1_NW_ADDR                                              (0xC073u)
#define MAC_ADDR_MSK_LWR_1_RESET_VAL                                        (0xFFFFFFFFu)
#define MAC_ADDR_MSK_LWR_1_MMS                                                       (1u)
#define MAC_ADDR_MSK_LWR_1_VOL_MASK                                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | MAC_ADDR_MASK            | MAC Address Bit Mask for the      | 0xffffffff  |
// |       |                          | Address Table.                    |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAC_ADDR_MASK
// Bit position, length and mask
#define BITP_MAC_ADDR_MSK_LWR_1_MAC_ADDR_MASK                                        (0u)
#define BITL_MAC_ADDR_MSK_LWR_1_MAC_ADDR_MASK                                       (32u)
#define BITM_MAC_ADDR_MSK_LWR_1_MAC_ADDR_MASK                               (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_ADDEND
// Register Addr        : 0x0080u
// Register NW Addr     : 0xC080u
// Register Reset Value : 0x85555555u
// Register MMS         : 1u

#define MAC_TS_ADDEND_ADDR                                                      (0x0080u)
#define MAC_TS_ADDEND_NW_ADDR                                                   (0xC080u)
#define MAC_TS_ADDEND_RESET_VAL                                             (0x85555555u)
#define MAC_TS_ADDEND_MMS                                                            (1u)
#define MAC_TS_ADDEND_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_ADDEND                | Timestamp Accumulator Addend.     | 0x85555555  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_ADDEND
// Bit position, length and mask
#define BITP_MAC_TS_ADDEND_TS_ADDEND                                                 (0u)
#define BITL_MAC_TS_ADDEND_TS_ADDEND                                                (32u)
#define BITM_MAC_TS_ADDEND_TS_ADDEND                                        (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_1SEC_CMP
// Register Addr        : 0x0081u
// Register NW Addr     : 0xC081u
// Register Reset Value : 0x3B9ACA00u
// Register MMS         : 1u

#define MAC_TS_1SEC_CMP_ADDR                                                    (0x0081u)
#define MAC_TS_1SEC_CMP_NW_ADDR                                                 (0xC081u)
#define MAC_TS_1SEC_CMP_RESET_VAL                                           (0x3B9ACA00u)
#define MAC_TS_1SEC_CMP_MMS                                                          (1u)
#define MAC_TS_1SEC_CMP_VOL_MASK                                            (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_1SEC_CMP              | Timestamp 1 Second Compare        | 0x3b9aca00  |
// |       |                          | Value.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_1SEC_CMP
// Bit position, length and mask
#define BITP_MAC_TS_1SEC_CMP_TS_1SEC_CMP                                             (0u)
#define BITL_MAC_TS_1SEC_CMP_TS_1SEC_CMP                                            (32u)
#define BITM_MAC_TS_1SEC_CMP_TS_1SEC_CMP                                    (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_SEC_CNT
// Register Addr        : 0x0082u
// Register NW Addr     : 0xC082u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_SEC_CNT_ADDR                                                     (0x0082u)
#define MAC_TS_SEC_CNT_NW_ADDR                                                  (0xC082u)
#define MAC_TS_SEC_CNT_RESET_VAL                                            (0x00000000u)
#define MAC_TS_SEC_CNT_MMS                                                           (1u)
#define MAC_TS_SEC_CNT_VOL_MASK                                             (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_SEC_CNT               | Write to the Seconds Counter.     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_SEC_CNT
// Bit position, length and mask
#define BITP_MAC_TS_SEC_CNT_TS_SEC_CNT                                               (0u)
#define BITL_MAC_TS_SEC_CNT_TS_SEC_CNT                                              (32u)
#define BITM_MAC_TS_SEC_CNT_TS_SEC_CNT                                      (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_NS_CNT
// Register Addr        : 0x0083u
// Register NW Addr     : 0xC083u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_NS_CNT_ADDR                                                      (0x0083u)
#define MAC_TS_NS_CNT_NW_ADDR                                                   (0xC083u)
#define MAC_TS_NS_CNT_RESET_VAL                                             (0x00000000u)
#define MAC_TS_NS_CNT_MMS                                                            (1u)
#define MAC_TS_NS_CNT_VOL_MASK                                              (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_NS_CNT                | Write to the Nanoseconds          | 0x00000000  |
// |       |                          | Counter.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_NS_CNT
// Bit position, length and mask
#define BITP_MAC_TS_NS_CNT_TS_NS_CNT                                                 (0u)
#define BITL_MAC_TS_NS_CNT_TS_NS_CNT                                                (32u)
#define BITM_MAC_TS_NS_CNT_TS_NS_CNT                                        (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_CFG
// Register Addr        : 0x0084u
// Register NW Addr     : 0xC084u
// Register Reset Value : 0x00000001u
// Register MMS         : 1u

#define MAC_TS_CFG_ADDR                                                         (0x0084u)
#define MAC_TS_CFG_NW_ADDR                                                      (0xC084u)
#define MAC_TS_CFG_RESET_VAL                                                (0x00000001u)
#define MAC_TS_CFG_MMS                                                               (1u)
#define MAC_TS_CFG_VOL_MASK                                                 (0x00000015u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | TS_EN                    | Enable the 1588 Timestamp         | 0x00000001  |
// |       |                          | Counter.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 1     | TS_CLR                   | Clear the 1588 Timestamp          | 0x00000000  |
// |       |                          | Counters.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 2     | TS_TIMER_DEF             | The Default Value for the         | 0x00000000  |
// |       |                          | TS_TIMER Output.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 3     | TS_TIMER_STOP            | Stop Toggling the TS_TIMER        | 0x00000000  |
// |       |                          | Output.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 4     | TS_CAPT_FREE_CNT         | Capture the Free Running          | 0x00000000  |
// |       |                          | Counter.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_EN
// Bit position, length and mask
#define BITP_MAC_TS_CFG_TS_EN                                                        (0u)
#define BITL_MAC_TS_CFG_TS_EN                                                        (1u)
#define BITM_MAC_TS_CFG_TS_EN                                               (0x00000001u)

// Bit field: TS_CLR
// Bit position, length and mask
#define BITP_MAC_TS_CFG_TS_CLR                                                       (1u)
#define BITL_MAC_TS_CFG_TS_CLR                                                       (1u)
#define BITM_MAC_TS_CFG_TS_CLR                                              (0x00000002u)

// Bit field: TS_TIMER_DEF
// Bit position, length and mask
#define BITP_MAC_TS_CFG_TS_TIMER_DEF                                                 (2u)
#define BITL_MAC_TS_CFG_TS_TIMER_DEF                                                 (1u)
#define BITM_MAC_TS_CFG_TS_TIMER_DEF                                        (0x00000004u)

// Bit field: TS_TIMER_STOP
// Bit position, length and mask
#define BITP_MAC_TS_CFG_TS_TIMER_STOP                                                (3u)
#define BITL_MAC_TS_CFG_TS_TIMER_STOP                                                (1u)
#define BITM_MAC_TS_CFG_TS_TIMER_STOP                                       (0x00000008u)

// Bit field: TS_CAPT_FREE_CNT
// Bit position, length and mask
#define BITP_MAC_TS_CFG_TS_CAPT_FREE_CNT                                             (4u)
#define BITL_MAC_TS_CFG_TS_CAPT_FREE_CNT                                             (1u)
#define BITM_MAC_TS_CFG_TS_CAPT_FREE_CNT                                    (0x00000010u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_TIMER_HI
// Register Addr        : 0x0085u
// Register NW Addr     : 0xC085u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_TIMER_HI_ADDR                                                    (0x0085u)
#define MAC_TS_TIMER_HI_NW_ADDR                                                 (0xC085u)
#define MAC_TS_TIMER_HI_RESET_VAL                                           (0x00000000u)
#define MAC_TS_TIMER_HI_MMS                                                          (1u)
#define MAC_TS_TIMER_HI_VOL_MASK                                            (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_TIMER_HI              | TS_TIMER High Period (NS).        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_TIMER_HI
// Bit position, length and mask
#define BITP_MAC_TS_TIMER_HI_TS_TIMER_HI                                             (0u)
#define BITL_MAC_TS_TIMER_HI_TS_TIMER_HI                                            (32u)
#define BITM_MAC_TS_TIMER_HI_TS_TIMER_HI                                    (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_TIMER_LO
// Register Addr        : 0x0086u
// Register NW Addr     : 0xC086u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_TIMER_LO_ADDR                                                    (0x0086u)
#define MAC_TS_TIMER_LO_NW_ADDR                                                 (0xC086u)
#define MAC_TS_TIMER_LO_RESET_VAL                                           (0x00000000u)
#define MAC_TS_TIMER_LO_MMS                                                          (1u)
#define MAC_TS_TIMER_LO_VOL_MASK                                            (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_TIMER_LO              | TS_TIMER Low Period (NS).         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_TIMER_LO
// Bit position, length and mask
#define BITP_MAC_TS_TIMER_LO_TS_TIMER_LO                                             (0u)
#define BITL_MAC_TS_TIMER_LO_TS_TIMER_LO                                            (32u)
#define BITM_MAC_TS_TIMER_LO_TS_TIMER_LO                                    (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_TIMER_QE_CORR
// Register Addr        : 0x0087u
// Register NW Addr     : 0xC087u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_TIMER_QE_CORR_ADDR                                               (0x0087u)
#define MAC_TS_TIMER_QE_CORR_NW_ADDR                                            (0xC087u)
#define MAC_TS_TIMER_QE_CORR_RESET_VAL                                      (0x00000000u)
#define MAC_TS_TIMER_QE_CORR_MMS                                                     (1u)
#define MAC_TS_TIMER_QE_CORR_VOL_MASK                                       (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | TS_TIMER_QE_CORR         | TS_TIMER Quantization Error       | 0x00000000  |
// |       |                          | Correction Value.                 |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_TIMER_QE_CORR
// Bit position, length and mask
#define BITP_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR                                   (0u)
#define BITL_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR                                   (8u)
#define BITM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR                          (0x000000FFu)
// Enums
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_NO_QE_CORR               (0x00000000u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_1NS              (0x00000001u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_2NS              (0x00000002u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_3NS              (0x00000003u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_4NS              (0x00000004u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_5NS              (0x00000005u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_6NS              (0x00000006u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_7NS              (0x00000007u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_8NS              (0x00000008u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_9NS              (0x00000009u)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_10NS             (0x0000000Au)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_11NS             (0x0000000Bu)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_12NS             (0x0000000Cu)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_13NS             (0x0000000Du)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_14NS             (0x0000000Eu)
#define ENUM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR_QE_CORR_15NS             (0x0000000Fu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_TIMER_START
// Register Addr        : 0x0088u
// Register NW Addr     : 0xC088u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_TIMER_START_ADDR                                                 (0x0088u)
#define MAC_TS_TIMER_START_NW_ADDR                                              (0xC088u)
#define MAC_TS_TIMER_START_RESET_VAL                                        (0x00000000u)
#define MAC_TS_TIMER_START_MMS                                                       (1u)
#define MAC_TS_TIMER_START_VOL_MASK                                         (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_TSTART                | Point in Time at Which to Start   | 0x00000000  |
// |       |                          | the TS_TIMER Counter (NS).        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_TSTART
// Bit position, length and mask
#define BITP_MAC_TS_TIMER_START_TS_TSTART                                            (0u)
#define BITL_MAC_TS_TIMER_START_TS_TSTART                                           (32u)
#define BITM_MAC_TS_TIMER_START_TS_TSTART                                   (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_EXT_CAPT0
// Register Addr        : 0x0089u
// Register NW Addr     : 0xC089u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_EXT_CAPT0_ADDR                                                   (0x0089u)
#define MAC_TS_EXT_CAPT0_NW_ADDR                                                (0xC089u)
#define MAC_TS_EXT_CAPT0_RESET_VAL                                          (0x00000000u)
#define MAC_TS_EXT_CAPT0_MMS                                                         (1u)
#define MAC_TS_EXT_CAPT0_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_EXT_CAPTD             | Timestamp Captured on the         | 0x00000000  |
// |       |                          | Assertion of TS_CAPT Pin.         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_EXT_CAPTD
// Bit position, length and mask
#define BITP_MAC_TS_EXT_CAPT0_TS_EXT_CAPTD                                           (0u)
#define BITL_MAC_TS_EXT_CAPT0_TS_EXT_CAPTD                                          (32u)
#define BITM_MAC_TS_EXT_CAPT0_TS_EXT_CAPTD                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_EXT_CAPT1
// Register Addr        : 0x008Au
// Register NW Addr     : 0xC08Au
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_EXT_CAPT1_ADDR                                                   (0x008Au)
#define MAC_TS_EXT_CAPT1_NW_ADDR                                                (0xC08Au)
#define MAC_TS_EXT_CAPT1_RESET_VAL                                          (0x00000000u)
#define MAC_TS_EXT_CAPT1_MMS                                                         (1u)
#define MAC_TS_EXT_CAPT1_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_EXT_CAPTD             | Timestamp Captured on the         | 0x00000000  |
// |       |                          | Assertion of TS_CAPT Pin.         |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_EXT_CAPTD
// Bit position, length and mask
#define BITP_MAC_TS_EXT_CAPT1_TS_EXT_CAPTD                                           (0u)
#define BITL_MAC_TS_EXT_CAPT1_TS_EXT_CAPTD                                          (32u)
#define BITM_MAC_TS_EXT_CAPT1_TS_EXT_CAPTD                                  (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TS_FREECNT_CAPT
// Register Addr        : 0x008Bu
// Register NW Addr     : 0xC08Bu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TS_FREECNT_CAPT_ADDR                                                (0x008Bu)
#define MAC_TS_FREECNT_CAPT_NW_ADDR                                             (0xC08Bu)
#define MAC_TS_FREECNT_CAPT_RESET_VAL                                       (0x00000000u)
#define MAC_TS_FREECNT_CAPT_MMS                                                      (1u)
#define MAC_TS_FREECNT_CAPT_VOL_MASK                                        (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 31-00 | TS_CNT_CAPTD             | Captured Free Running Counter.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TS_CNT_CAPTD
// Bit position, length and mask
#define BITP_MAC_TS_FREECNT_CAPT_TS_CNT_CAPTD                                        (0u)
#define BITL_MAC_TS_FREECNT_CAPT_TS_CNT_CAPTD                                       (32u)
#define BITM_MAC_TS_FREECNT_CAPT_TS_CNT_CAPTD                               (0xFFFFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : PLCA_PRIORITY
// Register Addr        : 0x008Du
// Register NW Addr     : 0xC08Du
// Register Reset Value : 0x00080000u
// Register MMS         : 1u

#define MAC_PLCA_PRIORITY_ADDR                                                  (0x008Du)
#define MAC_PLCA_PRIORITY_NW_ADDR                                               (0xC08Du)
#define MAC_PLCA_PRIORITY_RESET_VAL                                         (0x00080000u)
#define MAC_PLCA_PRIORITY_MMS                                                        (1u)
#define MAC_PLCA_PRIORITY_VOL_MASK                                          (0x000FFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | NODE_PRIORITY_ID         | Current Active Priority           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 15-08 | NET_PRIORITY_ID          | Network Priority List             | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 16    | FRAME_PRIORITY_EN        | Enables the Frame Priority        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 18    | NODE_PRIORITY_EN         | Enables the Node PLCA Priority    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 19    | PLCA_PENDING_PKT_FORCE   | Enable Forcing PCLA Pending       | 0x00000001  |
// |       |                          | Packing.                          |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: NODE_PRIORITY_ID
// Bit position, length and mask
#define BITP_MAC_PLCA_PRIORITY_NODE_PRIORITY_ID                                      (0u)
#define BITL_MAC_PLCA_PRIORITY_NODE_PRIORITY_ID                                      (8u)
#define BITM_MAC_PLCA_PRIORITY_NODE_PRIORITY_ID                             (0x000000FFu)

// Bit field: NET_PRIORITY_ID
// Bit position, length and mask
#define BITP_MAC_PLCA_PRIORITY_NET_PRIORITY_ID                                       (8u)
#define BITL_MAC_PLCA_PRIORITY_NET_PRIORITY_ID                                       (8u)
#define BITM_MAC_PLCA_PRIORITY_NET_PRIORITY_ID                              (0x0000FF00u)

// Bit field: FRAME_PRIORITY_EN
// Bit position, length and mask
#define BITP_MAC_PLCA_PRIORITY_FRAME_PRIORITY_EN                                    (16u)
#define BITL_MAC_PLCA_PRIORITY_FRAME_PRIORITY_EN                                     (1u)
#define BITM_MAC_PLCA_PRIORITY_FRAME_PRIORITY_EN                            (0x00010000u)

// Bit field: NODE_PRIORITY_EN
// Bit position, length and mask
#define BITP_MAC_PLCA_PRIORITY_NODE_PRIORITY_EN                                     (18u)
#define BITL_MAC_PLCA_PRIORITY_NODE_PRIORITY_EN                                      (1u)
#define BITM_MAC_PLCA_PRIORITY_NODE_PRIORITY_EN                             (0x00040000u)

// Bit field: PLCA_PENDING_PKT_FORCE
// Bit position, length and mask
#define BITP_MAC_PLCA_PRIORITY_PLCA_PENDING_PKT_FORCE                               (19u)
#define BITL_MAC_PLCA_PRIORITY_PLCA_PENDING_PKT_FORCE                                (1u)
#define BITM_MAC_PLCA_PRIORITY_PLCA_PENDING_PKT_FORCE                       (0x00080000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_FSIZE
// Register Addr        : 0x0090u
// Register NW Addr     : 0xC090u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_FSIZE_ADDR                                                       (0x0090u)
#define MAC_RX_FSIZE_NW_ADDR                                                    (0xC090u)
#define MAC_RX_FSIZE_RESET_VAL                                              (0x00000000u)
#define MAC_RX_FSIZE_MMS                                                             (1u)
#define MAC_RX_FSIZE_VOL_MASK                                               (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 10-00 | RX_FRM_SIZE              | Receive Frame Size.               | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_FRM_SIZE
// Bit position, length and mask
#define BITP_MAC_RX_FSIZE_RX_FRM_SIZE                                                (0u)
#define BITL_MAC_RX_FSIZE_RX_FRM_SIZE                                               (11u)
#define BITM_MAC_RX_FSIZE_RX_FRM_SIZE                                       (0x000007FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_DUMP_FRM
// Register Addr        : 0x0092u
// Register NW Addr     : 0xC092u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_DUMP_FRM_ADDR                                                    (0x0092u)
#define MAC_RX_DUMP_FRM_NW_ADDR                                                 (0xC092u)
#define MAC_RX_DUMP_FRM_RESET_VAL                                           (0x00000000u)
#define MAC_RX_DUMP_FRM_MMS                                                          (1u)
#define MAC_RX_DUMP_FRM_VOL_MASK                                            (0x00000002u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 1     | FRM_LENGTH_ERR           | Received a Frame with Wrong       | 0x00000000  |
// |       |                          | Length Field.                     |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: FRM_LENGTH_ERR
// Bit position, length and mask
#define BITP_MAC_RX_DUMP_FRM_FRM_LENGTH_ERR                                          (1u)
#define BITL_MAC_RX_DUMP_FRM_FRM_LENGTH_ERR                                          (1u)
#define BITM_MAC_RX_DUMP_FRM_FRM_LENGTH_ERR                                 (0x00000002u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_FRM_CNT
// Register Addr        : 0x00A1u
// Register NW Addr     : 0xC0A1u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_FRM_CNT_ADDR                                                     (0x00A1u)
#define MAC_RX_FRM_CNT_NW_ADDR                                                  (0xC0A1u)
#define MAC_RX_FRM_CNT_RESET_VAL                                            (0x00000000u)
#define MAC_RX_FRM_CNT_MMS                                                           (1u)
#define MAC_RX_FRM_CNT_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_FRM_CNT               | Rx Frame Count.                   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_FRM_CNT
// Bit position, length and mask
#define BITP_MAC_RX_FRM_CNT_RX_FRM_CNT                                               (0u)
#define BITL_MAC_RX_FRM_CNT_RX_FRM_CNT                                              (24u)
#define BITM_MAC_RX_FRM_CNT_RX_FRM_CNT                                      (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_BCAST_CNT
// Register Addr        : 0x00A2u
// Register NW Addr     : 0xC0A2u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_BCAST_CNT_ADDR                                                   (0x00A2u)
#define MAC_RX_BCAST_CNT_NW_ADDR                                                (0xC0A2u)
#define MAC_RX_BCAST_CNT_RESET_VAL                                          (0x00000000u)
#define MAC_RX_BCAST_CNT_MMS                                                         (1u)
#define MAC_RX_BCAST_CNT_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_BCAST_CNT             | Rx Broadcast Frame Count.         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_BCAST_CNT
// Bit position, length and mask
#define BITP_MAC_RX_BCAST_CNT_RX_BCAST_CNT                                           (0u)
#define BITL_MAC_RX_BCAST_CNT_RX_BCAST_CNT                                          (24u)
#define BITM_MAC_RX_BCAST_CNT_RX_BCAST_CNT                                  (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_MCAST_CNT
// Register Addr        : 0x00A3u
// Register NW Addr     : 0xC0A3u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_MCAST_CNT_ADDR                                                   (0x00A3u)
#define MAC_RX_MCAST_CNT_NW_ADDR                                                (0xC0A3u)
#define MAC_RX_MCAST_CNT_RESET_VAL                                          (0x00000000u)
#define MAC_RX_MCAST_CNT_MMS                                                         (1u)
#define MAC_RX_MCAST_CNT_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_MCAST_CNT             | Rx Multicast Frame Count.         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_MCAST_CNT
// Bit position, length and mask
#define BITP_MAC_RX_MCAST_CNT_RX_MCAST_CNT                                           (0u)
#define BITL_MAC_RX_MCAST_CNT_RX_MCAST_CNT                                          (24u)
#define BITM_MAC_RX_MCAST_CNT_RX_MCAST_CNT                                  (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_UCAST_CNT
// Register Addr        : 0x00A4u
// Register NW Addr     : 0xC0A4u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_UCAST_CNT_ADDR                                                   (0x00A4u)
#define MAC_RX_UCAST_CNT_NW_ADDR                                                (0xC0A4u)
#define MAC_RX_UCAST_CNT_RESET_VAL                                          (0x00000000u)
#define MAC_RX_UCAST_CNT_MMS                                                         (1u)
#define MAC_RX_UCAST_CNT_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_UCAST_CNT             | Rx Unicast Frame Count.           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_UCAST_CNT
// Bit position, length and mask
#define BITP_MAC_RX_UCAST_CNT_RX_UCAST_CNT                                           (0u)
#define BITL_MAC_RX_UCAST_CNT_RX_UCAST_CNT                                          (24u)
#define BITM_MAC_RX_UCAST_CNT_RX_UCAST_CNT                                  (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_CRC_ERR_CNT
// Register Addr        : 0x00A5u
// Register NW Addr     : 0xC0A5u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_CRC_ERR_CNT_ADDR                                                 (0x00A5u)
#define MAC_RX_CRC_ERR_CNT_NW_ADDR                                              (0xC0A5u)
#define MAC_RX_CRC_ERR_CNT_RESET_VAL                                        (0x00000000u)
#define MAC_RX_CRC_ERR_CNT_MMS                                                       (1u)
#define MAC_RX_CRC_ERR_CNT_VOL_MASK                                         (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_CRC_ERR_CNT           | Rx CRC Errored Frame Count.       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_CRC_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_RX_CRC_ERR_CNT_RX_CRC_ERR_CNT                                       (0u)
#define BITL_MAC_RX_CRC_ERR_CNT_RX_CRC_ERR_CNT                                      (24u)
#define BITM_MAC_RX_CRC_ERR_CNT_RX_CRC_ERR_CNT                              (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_ALGN_ERR_CNT
// Register Addr        : 0x00A6u
// Register NW Addr     : 0xC0A6u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_ALGN_ERR_CNT_ADDR                                                (0x00A6u)
#define MAC_RX_ALGN_ERR_CNT_NW_ADDR                                             (0xC0A6u)
#define MAC_RX_ALGN_ERR_CNT_RESET_VAL                                       (0x00000000u)
#define MAC_RX_ALGN_ERR_CNT_MMS                                                      (1u)
#define MAC_RX_ALGN_ERR_CNT_VOL_MASK                                        (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_ALGN_ERR_CNT          | Rx Align Error Count.             | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_ALGN_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_RX_ALGN_ERR_CNT_RX_ALGN_ERR_CNT                                     (0u)
#define BITL_MAC_RX_ALGN_ERR_CNT_RX_ALGN_ERR_CNT                                    (24u)
#define BITM_MAC_RX_ALGN_ERR_CNT_RX_ALGN_ERR_CNT                            (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_PREAMBLE_ERR_CNT
// Register Addr        : 0x00A7u
// Register NW Addr     : 0xC0A7u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_PREAMBLE_ERR_CNT_ADDR                                            (0x00A7u)
#define MAC_RX_PREAMBLE_ERR_CNT_NW_ADDR                                         (0xC0A7u)
#define MAC_RX_PREAMBLE_ERR_CNT_RESET_VAL                                   (0x00000000u)
#define MAC_RX_PREAMBLE_ERR_CNT_MMS                                                  (1u)
#define MAC_RX_PREAMBLE_ERR_CNT_VOL_MASK                                    (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_PREAMBLE_ERR_CNT      | Rx Preamble Error Count.          | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_PREAMBLE_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_RX_PREAMBLE_ERR_CNT_RX_PREAMBLE_ERR_CNT                             (0u)
#define BITL_MAC_RX_PREAMBLE_ERR_CNT_RX_PREAMBLE_ERR_CNT                            (24u)
#define BITM_MAC_RX_PREAMBLE_ERR_CNT_RX_PREAMBLE_ERR_CNT                    (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_SHORT_ERR_CNT
// Register Addr        : 0x00A8u
// Register NW Addr     : 0xC0A8u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_SHORT_ERR_CNT_ADDR                                               (0x00A8u)
#define MAC_RX_SHORT_ERR_CNT_NW_ADDR                                            (0xC0A8u)
#define MAC_RX_SHORT_ERR_CNT_RESET_VAL                                      (0x00000000u)
#define MAC_RX_SHORT_ERR_CNT_MMS                                                     (1u)
#define MAC_RX_SHORT_ERR_CNT_VOL_MASK                                       (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_SHORT_ERR_CNT         | Rx Short Frame Error Count.       | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_SHORT_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_RX_SHORT_ERR_CNT_RX_SHORT_ERR_CNT                                   (0u)
#define BITL_MAC_RX_SHORT_ERR_CNT_RX_SHORT_ERR_CNT                                  (24u)
#define BITM_MAC_RX_SHORT_ERR_CNT_RX_SHORT_ERR_CNT                          (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_LONG_ERR_CNT
// Register Addr        : 0x00A9u
// Register NW Addr     : 0xC0A9u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_LONG_ERR_CNT_ADDR                                                (0x00A9u)
#define MAC_RX_LONG_ERR_CNT_NW_ADDR                                             (0xC0A9u)
#define MAC_RX_LONG_ERR_CNT_RESET_VAL                                       (0x00000000u)
#define MAC_RX_LONG_ERR_CNT_MMS                                                      (1u)
#define MAC_RX_LONG_ERR_CNT_VOL_MASK                                        (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_LONG_ERR_CNT          | Rx Long Frame Error Count.        | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_LONG_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_RX_LONG_ERR_CNT_RX_LONG_ERR_CNT                                     (0u)
#define BITL_MAC_RX_LONG_ERR_CNT_RX_LONG_ERR_CNT                                    (24u)
#define BITM_MAC_RX_LONG_ERR_CNT_RX_LONG_ERR_CNT                            (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_PHY_ERR_CNT
// Register Addr        : 0x00AAu
// Register NW Addr     : 0xC0AAu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_PHY_ERR_CNT_ADDR                                                 (0x00AAu)
#define MAC_RX_PHY_ERR_CNT_NW_ADDR                                              (0xC0AAu)
#define MAC_RX_PHY_ERR_CNT_RESET_VAL                                        (0x00000000u)
#define MAC_RX_PHY_ERR_CNT_MMS                                                       (1u)
#define MAC_RX_PHY_ERR_CNT_VOL_MASK                                         (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_PHY_ERR_CNT           | Rx PHY Error Count.               | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_PHY_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_RX_PHY_ERR_CNT_RX_PHY_ERR_CNT                                       (0u)
#define BITL_MAC_RX_PHY_ERR_CNT_RX_PHY_ERR_CNT                                      (24u)
#define BITM_MAC_RX_PHY_ERR_CNT_RX_PHY_ERR_CNT                              (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_DROP_HOST_FULL_CNT
// Register Addr        : 0x00ABu
// Register NW Addr     : 0xC0ABu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_DROP_HOST_FULL_CNT_ADDR                                          (0x00ABu)
#define MAC_RX_DROP_HOST_FULL_CNT_NW_ADDR                                       (0xC0ABu)
#define MAC_RX_DROP_HOST_FULL_CNT_RESET_VAL                                 (0x00000000u)
#define MAC_RX_DROP_HOST_FULL_CNT_MMS                                                (1u)
#define MAC_RX_DROP_HOST_FULL_CNT_VOL_MASK                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_DROP_HOST_FULL_CNT    | Rx Frames Dropped Due to Host     | 0x00000000  |
// |       |                          | FIFO Full.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_DROP_HOST_FULL_CNT
// Bit position, length and mask
#define BITP_MAC_RX_DROP_HOST_FULL_CNT_RX_DROP_HOST_FULL_CNT                         (0u)
#define BITL_MAC_RX_DROP_HOST_FULL_CNT_RX_DROP_HOST_FULL_CNT                        (24u)
#define BITM_MAC_RX_DROP_HOST_FULL_CNT_RX_DROP_HOST_FULL_CNT                (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_DROP_PORT_FULL_CNT
// Register Addr        : 0x00ACu
// Register NW Addr     : 0xC0ACu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_DROP_PORT_FULL_CNT_ADDR                                          (0x00ACu)
#define MAC_RX_DROP_PORT_FULL_CNT_NW_ADDR                                       (0xC0ACu)
#define MAC_RX_DROP_PORT_FULL_CNT_RESET_VAL                                 (0x00000000u)
#define MAC_RX_DROP_PORT_FULL_CNT_MMS                                                (1u)
#define MAC_RX_DROP_PORT_FULL_CNT_VOL_MASK                                  (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_DROP_LCE_FULL_CNT     | Rx Frames Dropped Due to the LCE  | 0x00000000  |
// |       |                          | FIFO Being Full.                  |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_DROP_LCE_FULL_CNT
// Bit position, length and mask
#define BITP_MAC_RX_DROP_PORT_FULL_CNT_RX_DROP_LCE_FULL_CNT                          (0u)
#define BITL_MAC_RX_DROP_PORT_FULL_CNT_RX_DROP_LCE_FULL_CNT                         (24u)
#define BITM_MAC_RX_DROP_PORT_FULL_CNT_RX_DROP_LCE_FULL_CNT                 (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_DROP_FILT_CNT
// Register Addr        : 0x00ADu
// Register NW Addr     : 0xC0ADu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_DROP_FILT_CNT_ADDR                                               (0x00ADu)
#define MAC_RX_DROP_FILT_CNT_NW_ADDR                                            (0xC0ADu)
#define MAC_RX_DROP_FILT_CNT_RESET_VAL                                      (0x00000000u)
#define MAC_RX_DROP_FILT_CNT_MMS                                                     (1u)
#define MAC_RX_DROP_FILT_CNT_VOL_MASK                                       (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_DROP_FILT_CNT         | Rx Frames Dropped Due to          | 0x00000000  |
// |       |                          | Filtering.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_DROP_FILT_CNT
// Bit position, length and mask
#define BITP_MAC_RX_DROP_FILT_CNT_RX_DROP_FILT_CNT                                   (0u)
#define BITL_MAC_RX_DROP_FILT_CNT_RX_DROP_FILT_CNT                                  (24u)
#define BITM_MAC_RX_DROP_FILT_CNT_RX_DROP_FILT_CNT                          (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_IFG_ERR_CNT
// Register Addr        : 0x00AEu
// Register NW Addr     : 0xC0AEu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RX_IFG_ERR_CNT_ADDR                                                 (0x00AEu)
#define MAC_RX_IFG_ERR_CNT_NW_ADDR                                              (0xC0AEu)
#define MAC_RX_IFG_ERR_CNT_RESET_VAL                                        (0x00000000u)
#define MAC_RX_IFG_ERR_CNT_MMS                                                       (1u)
#define MAC_RX_IFG_ERR_CNT_VOL_MASK                                         (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | RX_IFG_ERR_CNT           | IFG Error Counter for Received    | 0x00000000  |
// |       |                          | Frames.                           |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_IFG_ERR_CNT
// Bit position, length and mask
#define BITP_MAC_RX_IFG_ERR_CNT_RX_IFG_ERR_CNT                                       (0u)
#define BITL_MAC_RX_IFG_ERR_CNT_RX_IFG_ERR_CNT                                      (24u)
#define BITM_MAC_RX_IFG_ERR_CNT_RX_IFG_ERR_CNT                              (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_FRM_CNT
// Register Addr        : 0x00B1u
// Register NW Addr     : 0xC0B1u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_FRM_CNT_ADDR                                                     (0x00B1u)
#define MAC_TX_FRM_CNT_NW_ADDR                                                  (0xC0B1u)
#define MAC_TX_FRM_CNT_RESET_VAL                                            (0x00000000u)
#define MAC_TX_FRM_CNT_MMS                                                           (1u)
#define MAC_TX_FRM_CNT_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_FRM_CNT               | Tx Frame Count.                   | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_FRM_CNT
// Bit position, length and mask
#define BITP_MAC_TX_FRM_CNT_TX_FRM_CNT                                               (0u)
#define BITL_MAC_TX_FRM_CNT_TX_FRM_CNT                                              (24u)
#define BITM_MAC_TX_FRM_CNT_TX_FRM_CNT                                      (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_BCAST_CNT
// Register Addr        : 0x00B2u
// Register NW Addr     : 0xC0B2u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_BCAST_CNT_ADDR                                                   (0x00B2u)
#define MAC_TX_BCAST_CNT_NW_ADDR                                                (0xC0B2u)
#define MAC_TX_BCAST_CNT_RESET_VAL                                          (0x00000000u)
#define MAC_TX_BCAST_CNT_MMS                                                         (1u)
#define MAC_TX_BCAST_CNT_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_BCAST_CNT             | Tx Broadcast Frame Count.         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_BCAST_CNT
// Bit position, length and mask
#define BITP_MAC_TX_BCAST_CNT_TX_BCAST_CNT                                           (0u)
#define BITL_MAC_TX_BCAST_CNT_TX_BCAST_CNT                                          (24u)
#define BITM_MAC_TX_BCAST_CNT_TX_BCAST_CNT                                  (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_MCAST_CNT
// Register Addr        : 0x00B3u
// Register NW Addr     : 0xC0B3u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_MCAST_CNT_ADDR                                                   (0x00B3u)
#define MAC_TX_MCAST_CNT_NW_ADDR                                                (0xC0B3u)
#define MAC_TX_MCAST_CNT_RESET_VAL                                          (0x00000000u)
#define MAC_TX_MCAST_CNT_MMS                                                         (1u)
#define MAC_TX_MCAST_CNT_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_MCAST_CNT             | Tx Multicast Frame Count.         | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_MCAST_CNT
// Bit position, length and mask
#define BITP_MAC_TX_MCAST_CNT_TX_MCAST_CNT                                           (0u)
#define BITL_MAC_TX_MCAST_CNT_TX_MCAST_CNT                                          (24u)
#define BITM_MAC_TX_MCAST_CNT_TX_MCAST_CNT                                  (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_UCAST_CNT
// Register Addr        : 0x00B4u
// Register NW Addr     : 0xC0B4u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_UCAST_CNT_ADDR                                                   (0x00B4u)
#define MAC_TX_UCAST_CNT_NW_ADDR                                                (0xC0B4u)
#define MAC_TX_UCAST_CNT_RESET_VAL                                          (0x00000000u)
#define MAC_TX_UCAST_CNT_MMS                                                         (1u)
#define MAC_TX_UCAST_CNT_VOL_MASK                                           (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_UCAST_CNT             | Tx Unicast Frame Count.           | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_UCAST_CNT
// Bit position, length and mask
#define BITP_MAC_TX_UCAST_CNT_TX_UCAST_CNT                                           (0u)
#define BITL_MAC_TX_UCAST_CNT_TX_UCAST_CNT                                          (24u)
#define BITM_MAC_TX_UCAST_CNT_TX_UCAST_CNT                                  (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_SINGLE_COL_CNT
// Register Addr        : 0x00B5u
// Register NW Addr     : 0xC0B5u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_SINGLE_COL_CNT_ADDR                                              (0x00B5u)
#define MAC_TX_SINGLE_COL_CNT_NW_ADDR                                           (0xC0B5u)
#define MAC_TX_SINGLE_COL_CNT_RESET_VAL                                     (0x00000000u)
#define MAC_TX_SINGLE_COL_CNT_MMS                                                    (1u)
#define MAC_TX_SINGLE_COL_CNT_VOL_MASK                                      (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_SINGLE_COL_CNT        | Tx Single Collision Frame Count.  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_SINGLE_COL_CNT
// Bit position, length and mask
#define BITP_MAC_TX_SINGLE_COL_CNT_TX_SINGLE_COL_CNT                                 (0u)
#define BITL_MAC_TX_SINGLE_COL_CNT_TX_SINGLE_COL_CNT                                (24u)
#define BITM_MAC_TX_SINGLE_COL_CNT_TX_SINGLE_COL_CNT                        (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_MULTIPLE_COL_CNT
// Register Addr        : 0x00B6u
// Register NW Addr     : 0xC0B6u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_MULTIPLE_COL_CNT_ADDR                                            (0x00B6u)
#define MAC_TX_MULTIPLE_COL_CNT_NW_ADDR                                         (0xC0B6u)
#define MAC_TX_MULTIPLE_COL_CNT_RESET_VAL                                   (0x00000000u)
#define MAC_TX_MULTIPLE_COL_CNT_MMS                                                  (1u)
#define MAC_TX_MULTIPLE_COL_CNT_VOL_MASK                                    (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_MULTIPLE_COL_CNT      | Tx Multiple Collision Frame       | 0x00000000  |
// |       |                          | Count.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_MULTIPLE_COL_CNT
// Bit position, length and mask
#define BITP_MAC_TX_MULTIPLE_COL_CNT_TX_MULTIPLE_COL_CNT                             (0u)
#define BITL_MAC_TX_MULTIPLE_COL_CNT_TX_MULTIPLE_COL_CNT                            (24u)
#define BITM_MAC_TX_MULTIPLE_COL_CNT_TX_MULTIPLE_COL_CNT                    (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_DEFERRED_XMIT_CNT
// Register Addr        : 0x00B7u
// Register NW Addr     : 0xC0B7u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_DEFERRED_XMIT_CNT_ADDR                                           (0x00B7u)
#define MAC_TX_DEFERRED_XMIT_CNT_NW_ADDR                                        (0xC0B7u)
#define MAC_TX_DEFERRED_XMIT_CNT_RESET_VAL                                  (0x00000000u)
#define MAC_TX_DEFERRED_XMIT_CNT_MMS                                                 (1u)
#define MAC_TX_DEFERRED_XMIT_CNT_VOL_MASK                                   (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_DEFERRED_XMIT_CNT     | Tx Deferred Transmission Frame    | 0x00000000  |
// |       |                          | Count.                            |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_DEFERRED_XMIT_CNT
// Bit position, length and mask
#define BITP_MAC_TX_DEFERRED_XMIT_CNT_TX_DEFERRED_XMIT_CNT                           (0u)
#define BITL_MAC_TX_DEFERRED_XMIT_CNT_TX_DEFERRED_XMIT_CNT                          (24u)
#define BITM_MAC_TX_DEFERRED_XMIT_CNT_TX_DEFERRED_XMIT_CNT                  (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_LATE_COL_CNT
// Register Addr        : 0x00B8u
// Register NW Addr     : 0xC0B8u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_LATE_COL_CNT_ADDR                                                (0x00B8u)
#define MAC_TX_LATE_COL_CNT_NW_ADDR                                             (0xC0B8u)
#define MAC_TX_LATE_COL_CNT_RESET_VAL                                       (0x00000000u)
#define MAC_TX_LATE_COL_CNT_MMS                                                      (1u)
#define MAC_TX_LATE_COL_CNT_VOL_MASK                                        (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_LATE_COL_CNT          | Tx Late Collision Frame Count.    | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_LATE_COL_CNT
// Bit position, length and mask
#define BITP_MAC_TX_LATE_COL_CNT_TX_LATE_COL_CNT                                     (0u)
#define BITL_MAC_TX_LATE_COL_CNT_TX_LATE_COL_CNT                                    (24u)
#define BITM_MAC_TX_LATE_COL_CNT_TX_LATE_COL_CNT                            (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_XSCOLS_CNT
// Register Addr        : 0x00B9u
// Register NW Addr     : 0xC0B9u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_XSCOLS_CNT_ADDR                                                  (0x00B9u)
#define MAC_TX_XSCOLS_CNT_NW_ADDR                                               (0xC0B9u)
#define MAC_TX_XSCOLS_CNT_RESET_VAL                                         (0x00000000u)
#define MAC_TX_XSCOLS_CNT_MMS                                                        (1u)
#define MAC_TX_XSCOLS_CNT_VOL_MASK                                          (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_XSCOLS_CNT            | Tx Excess Collision Frame Count.  | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_XSCOLS_CNT
// Bit position, length and mask
#define BITP_MAC_TX_XSCOLS_CNT_TX_XSCOLS_CNT                                         (0u)
#define BITL_MAC_TX_XSCOLS_CNT_TX_XSCOLS_CNT                                        (24u)
#define BITM_MAC_TX_XSCOLS_CNT_TX_XSCOLS_CNT                                (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_UNR_CNT
// Register Addr        : 0x00BAu
// Register NW Addr     : 0xC0BAu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_TX_UNR_CNT_ADDR                                                     (0x00BAu)
#define MAC_TX_UNR_CNT_NW_ADDR                                                  (0xC0BAu)
#define MAC_TX_UNR_CNT_RESET_VAL                                            (0x00000000u)
#define MAC_TX_UNR_CNT_MMS                                                           (1u)
#define MAC_TX_UNR_CNT_VOL_MASK                                             (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 23-00 | TX_UNR_CNT               | Tx Frames That Were Dropped due   | 0x00000000  |
// |       |                          | to An Under Run on Port 1.        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_UNR_CNT
// Bit position, length and mask
#define BITP_MAC_TX_UNR_CNT_TX_UNR_CNT                                               (0u)
#define BITL_MAC_TX_UNR_CNT_TX_UNR_CNT                                              (24u)
#define BITM_MAC_TX_UNR_CNT_TX_UNR_CNT                                      (0x00FFFFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : HI_RFC
// Register Addr        : 0x00BBu
// Register NW Addr     : 0xC0BBu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_HI_RFC_ADDR                                                         (0x00BBu)
#define MAC_HI_RFC_NW_ADDR                                                      (0xC0BBu)
#define MAC_HI_RFC_RESET_VAL                                                (0x00000000u)
#define MAC_HI_RFC_MMS                                                               (1u)
#define MAC_HI_RFC_VOL_MASK                                                 (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 08-00 | RFC                      | Receive Frame Count.              | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RFC
// Bit position, length and mask
#define BITP_MAC_HI_RFC_RFC                                                          (0u)
#define BITL_MAC_HI_RFC_RFC                                                          (9u)
#define BITM_MAC_HI_RFC_RFC                                                 (0x000001FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : HI_RXSIZE
// Register Addr        : 0x00BDu
// Register NW Addr     : 0xC0BDu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_HI_RXSIZE_ADDR                                                      (0x00BDu)
#define MAC_HI_RXSIZE_NW_ADDR                                                   (0xC0BDu)
#define MAC_HI_RXSIZE_RESET_VAL                                             (0x00000000u)
#define MAC_HI_RXSIZE_MMS                                                            (1u)
#define MAC_HI_RXSIZE_VOL_MASK                                              (0x00000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 13-00 | RXSIZE                   | Data in the Rx FIFO. Number of    | 0x00000000  |
// |       |                          | Half Words(16 Bit).               |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RXSIZE
// Bit position, length and mask
#define BITP_MAC_HI_RXSIZE_RXSIZE                                                    (0u)
#define BITL_MAC_HI_RXSIZE_RXSIZE                                                   (14u)
#define BITM_MAC_HI_RXSIZE_RXSIZE                                           (0x00003FFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : TX_IFG
// Register Addr        : 0x00C1u
// Register NW Addr     : 0xC0C1u
// Register Reset Value : 0x0000000Bu
// Register MMS         : 1u

#define MAC_TX_IFG_ADDR                                                         (0x00C1u)
#define MAC_TX_IFG_NW_ADDR                                                      (0xC0C1u)
#define MAC_TX_IFG_RESET_VAL                                                (0x0000000Bu)
#define MAC_TX_IFG_MMS                                                               (1u)
#define MAC_TX_IFG_VOL_MASK                                                 (0x00003FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | TX_IFG                   | Inter Frame Gap.                  | 0x0000000b  |
// +-------+--------------------------+-----------------------------------+-------------+
// | 13-08 | TX_IFG_PART2             | Inter Frame Gap Part2.            | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_IFG
// Bit position, length and mask
#define BITP_MAC_TX_IFG_TX_IFG                                                       (0u)
#define BITL_MAC_TX_IFG_TX_IFG                                                       (8u)
#define BITM_MAC_TX_IFG_TX_IFG                                              (0x000000FFu)

// Bit field: TX_IFG_PART2
// Bit position, length and mask
#define BITP_MAC_TX_IFG_TX_IFG_PART2                                                 (8u)
#define BITL_MAC_TX_IFG_TX_IFG_PART2                                                 (6u)
#define BITM_MAC_TX_IFG_TX_IFG_PART2                                        (0x00003F00u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : DUPLEX
// Register Addr        : 0x00C2u
// Register NW Addr     : 0xC0C2u
// Register Reset Value : 0x00000010u
// Register MMS         : 1u

#define MAC_DUPLEX_ADDR                                                         (0x00C2u)
#define MAC_DUPLEX_NW_ADDR                                                      (0xC0C2u)
#define MAC_DUPLEX_RESET_VAL                                                (0x00000010u)
#define MAC_DUPLEX_MMS                                                               (1u)
#define MAC_DUPLEX_VOL_MASK                                                 (0x0000007Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 06-01 | TX_JAM_SIZE              | Jam Size (Nibbles).               | 0x00000008  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: TX_JAM_SIZE
// Bit position, length and mask
#define BITP_MAC_DUPLEX_TX_JAM_SIZE                                                  (1u)
#define BITL_MAC_DUPLEX_TX_JAM_SIZE                                                  (6u)
#define BITM_MAC_DUPLEX_TX_JAM_SIZE                                         (0x0000007Eu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : MAX_RETRY
// Register Addr        : 0x00C3u
// Register NW Addr     : 0xC0C3u
// Register Reset Value : 0x0000000Fu
// Register MMS         : 1u

#define MAC_MAX_RETRY_ADDR                                                      (0x00C3u)
#define MAC_MAX_RETRY_NW_ADDR                                                   (0xC0C3u)
#define MAC_MAX_RETRY_RESET_VAL                                             (0x0000000Fu)
#define MAC_MAX_RETRY_MMS                                                            (1u)
#define MAC_MAX_RETRY_VOL_MASK                                              (0x0000000Fu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 03-00 | MAX_RETRY                | Max Retry Count After             | 0x0000000f  |
// |       |                          | Collisions.                       |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAX_RETRY
// Bit position, length and mask
#define BITP_MAC_MAX_RETRY_MAX_RETRY                                                 (0u)
#define BITL_MAC_MAX_RETRY_MAX_RETRY                                                 (4u)
#define BITM_MAC_MAX_RETRY_MAX_RETRY                                        (0x0000000Fu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : LOOP
// Register Addr        : 0x00C4u
// Register NW Addr     : 0xC0C4u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_LOOP_ADDR                                                           (0x00C4u)
#define MAC_LOOP_NW_ADDR                                                        (0xC0C4u)
#define MAC_LOOP_RESET_VAL                                                  (0x00000000u)
#define MAC_LOOP_MMS                                                                 (1u)
#define MAC_LOOP_VOL_MASK                                                   (0x00000001u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | LOOPBACK_EN              | MAC Loopback.                     | 0x00000000  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: LOOPBACK_EN
// Bit position, length and mask
#define BITP_MAC_LOOP_LOOPBACK_EN                                                    (0u)
#define BITL_MAC_LOOP_LOOPBACK_EN                                                    (1u)
#define BITM_MAC_LOOP_LOOPBACK_EN                                           (0x00000001u)
// Enums
#define ENUM_MAC_LOOP_LOOPBACK_EN_LOOPB_DIS                                 (0x00000000u)
#define ENUM_MAC_LOOP_LOOPBACK_EN_LOOPB_EN                                  (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_CRC_EN
// Register Addr        : 0x00C5u
// Register NW Addr     : 0xC0C5u
// Register Reset Value : 0x00000001u
// Register MMS         : 1u

#define MAC_RX_CRC_EN_ADDR                                                      (0x00C5u)
#define MAC_RX_CRC_EN_NW_ADDR                                                   (0xC0C5u)
#define MAC_RX_CRC_EN_RESET_VAL                                             (0x00000001u)
#define MAC_RX_CRC_EN_MMS                                                            (1u)
#define MAC_RX_CRC_EN_VOL_MASK                                              (0x00000001u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 0     | CRC_CHK_EN               | CRC Check Enable on Receive.      | 0x00000001  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: CRC_CHK_EN
// Bit position, length and mask
#define BITP_MAC_RX_CRC_EN_CRC_CHK_EN                                                (0u)
#define BITL_MAC_RX_CRC_EN_CRC_CHK_EN                                                (1u)
#define BITM_MAC_RX_CRC_EN_CRC_CHK_EN                                       (0x00000001u)
// Enums
#define ENUM_MAC_RX_CRC_EN_CRC_CHK_EN_DISABLE                               (0x00000000u)
#define ENUM_MAC_RX_CRC_EN_CRC_CHK_EN_ENABLE                                (0x00000001u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_IFG
// Register Addr        : 0x00C6u
// Register NW Addr     : 0xC0C6u
// Register Reset Value : 0x0000000Au
// Register MMS         : 1u

#define MAC_RX_IFG_ADDR                                                         (0x00C6u)
#define MAC_RX_IFG_NW_ADDR                                                      (0xC0C6u)
#define MAC_RX_IFG_RESET_VAL                                                (0x0000000Au)
#define MAC_RX_IFG_MMS                                                               (1u)
#define MAC_RX_IFG_VOL_MASK                                                 (0x000000FFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 07-00 | RX_IFG                   | Inter Frame Gap.                  | 0x0000000a  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RX_IFG
// Bit position, length and mask
#define BITP_MAC_RX_IFG_RX_IFG                                                       (0u)
#define BITL_MAC_RX_IFG_RX_IFG                                                       (8u)
#define BITM_MAC_RX_IFG_RX_IFG                                              (0x000000FFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_MAX_LEN
// Register Addr        : 0x00C7u
// Register NW Addr     : 0xC0C7u
// Register Reset Value : 0x00000618u
// Register MMS         : 1u

#define MAC_RX_MAX_LEN_ADDR                                                     (0x00C7u)
#define MAC_RX_MAX_LEN_NW_ADDR                                                  (0xC0C7u)
#define MAC_RX_MAX_LEN_RESET_VAL                                            (0x00000618u)
#define MAC_RX_MAX_LEN_MMS                                                           (1u)
#define MAC_RX_MAX_LEN_VOL_MASK                                             (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MAX_FRM_LEN              | Max Frame Length on Receive.      | 0x00000618  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MAX_FRM_LEN
// Bit position, length and mask
#define BITP_MAC_RX_MAX_LEN_MAX_FRM_LEN                                              (0u)
#define BITL_MAC_RX_MAX_LEN_MAX_FRM_LEN                                             (16u)
#define BITM_MAC_RX_MAX_LEN_MAX_FRM_LEN                                     (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RX_MIN_LEN
// Register Addr        : 0x00C8u
// Register NW Addr     : 0xC0C8u
// Register Reset Value : 0x00000040u
// Register MMS         : 1u

#define MAC_RX_MIN_LEN_ADDR                                                     (0x00C8u)
#define MAC_RX_MIN_LEN_NW_ADDR                                                  (0xC0C8u)
#define MAC_RX_MIN_LEN_RESET_VAL                                            (0x00000040u)
#define MAC_RX_MIN_LEN_MMS                                                           (1u)
#define MAC_RX_MIN_LEN_VOL_MASK                                             (0x0000FFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | MIN_FRM_LEN              | Min Frame Length on Receive.      | 0x00000040  |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: MIN_FRM_LEN
// Bit position, length and mask
#define BITP_MAC_RX_MIN_LEN_MIN_FRM_LEN                                              (0u)
#define BITL_MAC_RX_MIN_LEN_MIN_FRM_LEN                                             (16u)
#define BITM_MAC_RX_MIN_LEN_MIN_FRM_LEN                                     (0x0000FFFFu)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RXFILT_VID_TABLE
// Register Addr        : 0x00C9u
// Register NW Addr     : 0xC0C9u
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RXFILT_VID_TABLE_ADDR                                               (0x00C9u)
#define MAC_RXFILT_VID_TABLE_NW_ADDR                                            (0xC0C9u)
#define MAC_RXFILT_VID_TABLE_RESET_VAL                                      (0x00000000u)
#define MAC_RXFILT_VID_TABLE_MMS                                                     (1u)
#define MAC_RXFILT_VID_TABLE_VOL_MASK                                       (0xCFFF0FFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 11-00 | RXFILT_VID_ENTRY1        | VLAN Identifier Filter Table      | 0x00000000  |
// |       |                          | Entry #1.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 27-16 | RXFILT_VID_ENTRY2        | VLAN Filter Identifier Table      | 0x00000000  |
// |       |                          | Entry #2.                         |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 30    | RXFILT_VID_RULE          | VLAN Identifier Filter Rule to    | 0x00000000  |
// |       |                          | Entry Table.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | RXFILT_VID_EN            | Enable VLAN Identifier            | 0x00000000  |
// |       |                          | Filtering.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RXFILT_VID_ENTRY1
// Bit position, length and mask
#define BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY1                                  (0u)
#define BITL_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY1                                 (12u)
#define BITM_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY1                         (0x00000FFFu)

// Bit field: RXFILT_VID_ENTRY2
// Bit position, length and mask
#define BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY2                                 (16u)
#define BITL_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY2                                 (12u)
#define BITM_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY2                         (0x0FFF0000u)

// Bit field: RXFILT_VID_RULE
// Bit position, length and mask
#define BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_RULE                                   (30u)
#define BITL_MAC_RXFILT_VID_TABLE_RXFILT_VID_RULE                                    (1u)
#define BITM_MAC_RXFILT_VID_TABLE_RXFILT_VID_RULE                           (0x40000000u)

// Bit field: RXFILT_VID_EN
// Bit position, length and mask
#define BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_EN                                     (31u)
#define BITL_MAC_RXFILT_VID_TABLE_RXFILT_VID_EN                                      (1u)
#define BITM_MAC_RXFILT_VID_TABLE_RXFILT_VID_EN                             (0x80000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RXFILT_ETYPE_TABLE
// Register Addr        : 0x00CAu
// Register NW Addr     : 0xC0CAu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RXFILT_ETYPE_TABLE_ADDR                                             (0x00CAu)
#define MAC_RXFILT_ETYPE_TABLE_NW_ADDR                                          (0xC0CAu)
#define MAC_RXFILT_ETYPE_TABLE_RESET_VAL                                    (0x00000000u)
#define MAC_RXFILT_ETYPE_TABLE_MMS                                                   (1u)
#define MAC_RXFILT_ETYPE_TABLE_VOL_MASK                                     (0xFFFFFFFFu)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 15-00 | RXFILT_ETYPE_ENTRY1      | Ethernet Type Filter Identifier   | 0x00000000  |
// |       |                          | Table Entry #1.                   |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31-16 | RXFILT_ETYPE_ENTRY2      | Ethernet Type Filter Identifier   | 0x00000000  |
// |       |                          | Table Entry #2.                   |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RXFILT_ETYPE_ENTRY1
// Bit position, length and mask
#define BITP_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY1                              (0u)
#define BITL_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY1                             (16u)
#define BITM_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY1                     (0x0000FFFFu)

// Bit field: RXFILT_ETYPE_ENTRY2
// Bit position, length and mask
#define BITP_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY2                             (16u)
#define BITL_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY2                             (16u)
#define BITM_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY2                     (0xFFFF0000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Register Name        : RXFILT_ETYPE_CNRL
// Register Addr        : 0x00CBu
// Register NW Addr     : 0xC0CBu
// Register Reset Value : 0x00000000u
// Register MMS         : 1u

#define MAC_RXFILT_ETYPE_CNRL_ADDR                                              (0x00CBu)
#define MAC_RXFILT_ETYPE_CNRL_NW_ADDR                                           (0xC0CBu)
#define MAC_RXFILT_ETYPE_CNRL_RESET_VAL                                     (0x00000000u)
#define MAC_RXFILT_ETYPE_CNRL_MMS                                                    (1u)
#define MAC_RXFILT_ETYPE_CNRL_VOL_MASK                                      (0xC0000000u)

// Bit field description table:
// +=======+==========================+===================================+=============+
// | Bits  | Bit Field                | Description                       | Value       |
// +=======+==========================+===================================+=============+
// | 30    | RXFILT_ETYPE_RULE        | Ethernet Type Filter Rule to      | 0x00000000  |
// |       |                          | Entry Table.                      |             |
// +-------+--------------------------+-----------------------------------+-------------+
// | 31    | RXFILT_ETYPE_EN          | Enable Ethernet Type Based        | 0x00000000  |
// |       |                          | Filtering.                        |             |
// +-------+--------------------------+-----------------------------------+-------------+

// Bit field defines:

// Bit field: RXFILT_ETYPE_RULE
// Bit position, length and mask
#define BITP_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_RULE                                (30u)
#define BITL_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_RULE                                 (1u)
#define BITM_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_RULE                        (0x40000000u)
// Enums
#define ENUM_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_RULE_MATCHPASS              (0x00000000u)
#define ENUM_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_RULE_MATCHDISCARD           (0x00000001u)

// Bit field: RXFILT_ETYPE_EN
// Bit position, length and mask
#define BITP_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_EN                                  (31u)
#define BITL_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_EN                                   (1u)
#define BITM_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_EN                          (0x80000000u)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TEST_REG
//~~~~~~~~ TEST ~~~~~~~~
extern void FillMACRegsData();
#endif


#endif /* ADI_MACLCE_REG_H */

/** @} */

/*
 * EOF: www.analog.com
 */


