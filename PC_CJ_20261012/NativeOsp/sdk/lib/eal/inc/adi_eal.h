/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal.h
 * \brief: The header file for the ADI E2B abstraction layer (EAL)
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_H
#define ADI_EAL_H

/*================================= INCLUDES ================================*/
#include "adi_e2bcore.h"    /* The E2BCore header */
#include "adi_network.h"    /* Network layer */
#include "adi_pal.h"        /* Platform abstraction layer */
#include "adi_dbg.h"        /* Debug interface */
#include "prj_osal.h"       /* OSAL header */

/*================================= DEFINES =================================*/
#define ADI_EAL_REG_READ_MAXLEN         (100U)              /*!< The max length of EAL reg read */
#define ADI_EAL_ALL_REMOTES             (0xFFFFFFFFu)       /*!< Multicast writes / reads */
#define ADI_EAL_MAX_NUM_NODES           (15u)               /*!< Max number of remote nodes */
#define ADI_EAL_MAX_PLCA_IDS            (8U)                /*!< Max number of PLCA IDs for a node */
#define ADI_VLAN_TAG_NA                 (0xFFFFFFFFU)       /*!< N/A value for the VLAN tag */
#define ADI_VAL_ZERO                    (0u)                /*!< Value zero */
#define ADI_EAL_DISC_TIMEOUT_NS         (100ULL * MS_TO_NS) /*!< Indicates the timeout in nanoseconds for the discovery */
#define ADI_EAL_LV_RESET_WAIT_TIME_NS   (10ULL * MS_TO_NS) /*!< Wait time for LV die reset to complete */
/*================================ DATA TYPES ===============================*/

/**
 * @brief EAL APIs ERROR TYPES
 */
typedef enum ADI_EAL_STATUS
{
    ADI_EAL_STATUS_SUCCESS,                       /*!< No error */
    ADI_EAL_STATUS_NULL_PTR,                      /*!< NULL pointer passed as parameter */
    ADI_EAL_STATUS_INVALID_PARAM,                 /*!< Invalid parameter passed */
    ADI_EAL_STATUS_MUTEX_FAILURE,                 /*!< Mutex lock or unlock failure */
    ADI_EAL_STATUS_MAX_INTFS_EXCEEDED,            /*!< Maximum number of interfaces exceeded */
    ADI_EAL_STATUS_NTW_BUFF_FULL,                 /*!< Failed to get buffer from the network layer */
    ADI_EAL_STATUS_QUEUE_FULL,                    /*!< Failed to get buffer from eal */
    ADI_EAL_STATUS_E2BCORE_BUFF_FULL,             /*!< Failed to get buffer from e2bcore */
    ADI_EAL_STATUS_START_FAILED,                  /*!< Errors with respect to frame buffer */
    ADI_EAL_STATUS_REGMAPIO_FAILED,               /*!< Failed to add regmap IO transaction command to the packet */
    ADI_EAL_STATUS_CFG_FAILED,                    /*!< Configuration failed due to an unknown error in e2bcore */
    ADI_EAL_STATUS_FIFO_FAILED,                   /*!< Failed to add fifo transaction command to the packet */
    ADI_EAL_STATUS_FIFO_LATCH_FAILED,             /*!< Failed to set the FIFO in latching mode, to save the next transaction */
    ADI_EAL_STATUS_CLOSE_FAILED,                  /*!< Failed to close eal instance */
    ADI_EAL_STATUS_RX_TIMEOUT,                    /*!< Response timed out */

    /* Wake sleep status events */
    ADI_EAL_STATUS_WKSLP_NODE_NOT_AWAKE,          /*!< Node is not awake to switch to standby / sleep */
    ADI_EAL_STATUS_WKSLP_WAKE_FAILED,             /*!< Failure to awaken node */
    ADI_EAL_STATUS_WKSLP_LINE_BUSY,               /*!< Ethernet line is busy in the node going to sleep (Indicates failure to go to sleep / standby) */
    ADI_EAL_STATUS_WKSLP_STANDBY_ERR,             /*!< Standby error event (Standby Watchdog timer expiry: Indicates failure to go to standby) */
    ADI_EAL_STATUS_WKSLP_SLEEP_ERR,               /*!< Sleep error event (Sleep Watchdog timer expiry: Indicates failure to go to sleep) */
    ADI_EAL_STATUS_WKSLP_PING_RESP_ERR,           /*!< Ping response received after going to sleep (Indicates failure to go to sleep / standby) */

    /* Topology Discovery events */
    ADI_EAL_STATUS_TOPO_DISC_DELAY_MEAS_FAILED,   /*!< Topology Discovery delay measurement failed */
    ADI_EAL_STATUS_TOPO_DISC_DIST_MEAS_FAILED,    /*!< Topology Discovery distance measurement failed */
    ADI_EAL_STATUS_TOPO_DISC_AUTO_MEAS_FAILED,    /*!< Topology Discovery automatic mode measurement failed */
    ADI_EAL_STATUS_SMC_REGION_CONFIG_FAILED,      /*!< SMC region configuration failed due to invalid region configuration */
    ADI_EAL_STATUS_OPEN_FAILED,                   /*!< Failed to open the EAL interface instance */
} ADI_EAL_STATUS;

/**
 * @brief EAL top-level component callback event types
 */
typedef enum ADI_EAL_CBK_EVT
{
    ADI_EAL_CBK_EVT_MAC_ADDR_RCVD,              /*!< MAC address received event */
    ADI_EAL_CBK_EVT_ERR                         /*!< Event indicating error in EAL */
} ADI_EAL_CBK_EVT;

/**
 * @brief EAL node callback event types
 */
typedef enum ADI_EAL_NODE_CBK_EVT
{
    ADI_EAL_NODE_CBK_EVT_PING_RCVD,             /*!< Ping received event */
    ADI_EAL_NODE_CBK_EVT_LUT_PLCA_ID_RCVD,      /*!< PLCA ID based on LUT received event */
    ADI_EAL_NODE_CBK_EVT_REVNUM_RCVD,           /*!< Revision number received event */
    ADI_EAL_NODE_CBK_EVT_ERR,                   /*!< Node error event */
    ADI_EAL_NODE_CBK_EVT_REG_READ_RESP,         /*!< Reg read response received event */
    ADI_EAL_NODE_CBK_EVT_BOOTLOADER_DONE,       /*!< Bootloader done interrupt received event */
    ADI_EAL_NODE_CBK_EVT_UNK_IRQ,               /*!< Unknown interrupt event */
} ADI_EAL_NODE_CBK_EVT;

/**
 * @brief MAC filter forwarding destination
 */
typedef enum ADI_EAL_MAC_FWD_DEST
{
    ADI_EAL_MAC_FWD_NONE = 0,            /*!< Do not forward frames */
    ADI_EAL_MAC_FWD_HOST,                /*!< Forward frames to HOST only */
    ADI_EAL_MAC_FWD_LCE,                 /*!< Forward frames to LCE only */
    ADI_EAL_MAC_FWD_BOTH_HOST_LCE        /*!< Forward frames to both HOST and LCE */
} ADI_EAL_MAC_FWD_DEST;

/**
 * @brief EAL interface interrupt callback event types
 */
typedef enum ADI_EAL_INTERFACE_INTERRUPT_CBK_EVT
{
    ADI_EAL_NODE_CBK_EVT_FLEXIO_IRQS
} ADI_EAL_INTERFACE_INTERRUPT_CBK_EVT;

/**
 * @brief EAL usage mode
 */
typedef enum ADI_EAL_USAGE_MODE
{
    ADI_EAL_USAGE_MODE_NORMAL,          /*!< To be use the normal EAL flow */
    ADI_EAL_USAGE_MODE_PERIODIC,        /*!< To be use the periodic EAL flow*/
    ADI_EAL_USAGE_MODE_CUSTOM,          /*!< To be use the custom EAL flow */
} ADI_EAL_USAGE_MODE;

/**
 * @brief EAL parity type
 */
typedef enum ADI_EAL_PARITY_TYPE
{
    ADI_EAL_EVEN_PARITY,                /*!< Parity type is even parity */
    ADI_EAL_ODD_PARITY,                 /*!< Parity type is odd parity */
} ADI_EAL_PARITY_TYPE;

/** Prototype of EAL top-level component callback */
typedef void (*ADI_EAL_TOP_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_CBK_EVT eCbkEventType, const void *pArg);

/** Prototype of EAL remote node callback */
typedef void (*ADI_EAL_NODE_CALLBACK_FUNC)(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_EAL_NODE_CBK_EVT eCbkEventType, const void *pArg);

/** Prototype of EAL interface interrupt callback */
typedef bool (*ADI_EAL_INTERRUPT_CALLBACK_FUNC)(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
                                ADI_EAL_INTERFACE_INTERRUPT_CBK_EVT eIntfCbkEvt);                              /*!< The EAL FIO OSP callback prototype */

/** @brief FIFO status data */
typedef struct ADI_EAL_FIFO_STATUS_DATA
{
    bool bTxFifoThresholdEmpty;     /*!< TX FIFO empty status */
    bool bTxFifoThreshold1;         /*!< TX FIFO has threshold 1 bytes */
    bool bTxFifoThreshold2;         /*!< TX FIFO has threshold 2 bytes */
    bool bTxFifoThreshold3;         /*!< TX FIFO has threshold 3 bytes */
    bool bTxFifoThresholdFull;      /*!< TX FIFO full status */
    bool bRxFifoThresholdEmpty;     /*!< RX FIFO empty status */
    bool bRxFifoThreshold1;         /*!< RX FIFO has threshold 1 bytes */
    bool bRxFifoThreshold2;         /*!< RX FIFO has threshold 2 bytes */
    bool bRxFifoThreshold3;         /*!< RX FIFO has threshold 3 bytes */
    bool bRxFifoThresholdFull;      /*!< RX FIFO full status */
    uint16_t nRxFifoReadBytes;      /*!< RX FIFO read bytes */
    uint16_t nRxFifoWriteBytes;     /*!< RX FIFO write bytes */
    uint16_t nTxFifoReadBytes;      /*!< TX FIFO read bytes */
    uint16_t nTxFifoWriteBytes;     /*!< TX FIFO write bytes */
    uint16_t nTxFifoLevel;          /*!< TX FIFO level (in bytes) */
    uint16_t nRxFifoLevel;          /*!< RX FIFO level (in bytes) */
} ADI_EAL_FIFO_STATUS_DATA;

/** @brief Revision number structure */
typedef struct ADI_EAL_NODE_REV_NUM
{
    uint32_t nMajorRevNum;          /*!< Major revision number */
    uint32_t nMinorRevNum;          /*!< Minor revision number */
} ADI_EAL_NODE_REV_NUM;

/* Basepin rotation table:
   +==========+===============+===============+
   | SM index | Basepin value | FIO pin index |
   +==========+===============+===============+
   |   7:0    |       0       |      7:0      |
   |   7:0    |       1       |     0,7:1     |
   |   7:0    |       2       |    1:0,7:2    |
   |   7:0    |       3       |    2:0,7:3    |
   |   7:0    |       4       |    3:0,7:4    |
   |   7:0    |       5       |    4:0,7:5    |
   |   7:0    |       6       |    5:0,7:6    |
   |   7:0    |       7       |     6:0,7     |
   |   1:0    |       0       |      1:0      |
   |   1:0    |       1       |      2:1      |
   |   1:0    |       2       |      3:2      |
   |   1:0    |       3       |      4:3      |
   |   1:0    |       4       |      5:4      |
   |   1:0    |       5       |      6:5      |
   |   1:0    |       6       |      7:6      |
   |   1:0    |       7       |      0,7      |
   +==========+===============+===============+ */

/** Flexible IO state machine configurations */
typedef struct ADI_EAL_FIO_SM_CFG
{
    bool bDelaySidesetSingleMode;   /*!< Is the delay/sideset bits configured with nDelaySidesetBits setting? */
    bool bEnableAutopush;           /*!< Enable autopush */
    bool bEnableAutopull;           /*!< Enable autopull */
    bool bOsrShiftRight;            /*!< Shift LSB first out of the OSR (shift right) */
    bool bIsrShiftRight;            /*!< Shift into MSB first into the ISR (shift right) */
    uint8_t nPinOutBasepin;         /*!< Set the output basepin rotation value (See the above table) */
    uint8_t nPinInBasepin;          /*!< Set the input basepin rotation value (See the above table) */
    uint8_t nSidesetBasepin;        /*!< Set the sideset basepin rotation value (See the above table) */
    uint8_t nJmpBasepin;            /*!< Set the basepin rotation value for pin to be compared in BPEO instruction (See the above table) */
    uint8_t nProgramStart;          /*!< Instruction index corresponding to the start of the program */
    uint8_t nProgramSize;           /*!< Number of instructions in the program */
    uint8_t nDelaySidesetBits;      /*!< Set the number of bits to used for sidesetting
                                         (LSBs of delay/sideset will be used for sidesetting) */
    uint8_t nAutopullThreshold;     /*!< When number of bits shifted out of the OSR exceeds this value,
                                         a PULL operation is performed */
    uint8_t nAutopushThreshold;     /*!< When number of bits shifted into the ISR reaches this value,
                                         a PUSH operation is performed */
    uint8_t nPullThreshold;         /*!< PULL instruction will not pull data into the OSR until these many bits are received in the FIFO */
    uint8_t nPushThreshold;         /*!< PUSH instruction will not push data into the FIFO until these many bits are received in the ISR */
    uint16_t nClkDiv;               /*!< 100MHz/nClkDiv will be the SM instruction execution frequency */
} ADI_EAL_FIO_SM_CFG;

/** @brief Instruction RAM info */
typedef struct ADI_EAL_FIO_INST_RAM_DATA
{
    bool bOverrideDefault;            /*!< Override the default instruction ram and use the below data */
    uint32_t *panInstructionMemData;  /*!< Pointer to array of instruction ram entries (Note: 1 entry in this contains 2 instructions) */
    uint16_t nNumInstMemEntries;      /*!< Number of entries in the panInstructionMemData array */
    bool bFifoTxRouterFixedSm;        /*!< Is the TX FIFO to SM router fixed to SM0? */
    uint8_t nNumActiveStateMachines;  /*!< Number of active state machines */
    ADI_EAL_FIO_SM_CFG aoSmCfgs[4U];  /*!< State machine configurations */
} ADI_EAL_FIO_INST_RAM_DATA;

/** EAL FIO interface status data */
typedef struct ADI_EAL_FIO_STATUS_DATA
{
    uint16_t            nNumRdBytes;            /**< Number of bytes read from the interface */
    uint16_t            nNumWrBytes;            /**< Number of bytes written to the interface */
    uint16_t            nNumTransactions;       /**< Number of transactions processed by the interface */
} ADI_EAL_FIO_STATUS_DATA;

/** PLCA NODE ID configurations */
typedef struct ADI_EAL_NODE_PLCA_ID_CFG
{
    uint8_t nMaskOfPlcaIdsUsed;     /*!< Mask of PLCA IDs to enable */
    uint8_t anPlcaID[ADI_EAL_MAX_PLCA_IDS]; /*!< Array of PLCA IDs to be assigned to a remote node */
} ADI_EAL_NODE_PLCA_ID_CFG;

/** VLAN tag configuration structure for upstream messages */
typedef struct ADI_EAL_UPSTREAM_MSG_VLAN_TAG
{
    ADI_VLAN_CFG oRegRbVlanTag;   /*!< VLAN tag for register readback message */
    bool bRegRbVlanEn;            /*!< If set true, enables the VLAN transmission in upstream for register read back messages */
    ADI_VLAN_CFG oIrqVlanTag;     /*!< VLAN tag for interrupt message */
    bool bIrqVlanEn;              /*!< If set true, enables the VLAN transmission in upstream for interrupt messages */
    ADI_VLAN_CFG oAckVlanTag;     /*!< VLAN tag for acknowledgement message */
    bool bAckVlanEn;              /*!< If set true, enables the VLAN transmission in upstream for acknowledgement messages */
    ADI_VLAN_CFG oFifoRejVlanTag; /*!< VLAN tag for fifo reject message */
    bool bFifoRejVlanEn;              /*!< If set true, enables the VLAN transmission in upstream for FIFO reject messages */
} ADI_EAL_UPSTREAM_MSG_VLAN_TAG;

/** Filter rule for untagged (without VLAN) frames */
typedef enum ADI_EAL_VLAN_FILTER_RULE
{
    ADI_EAL_VLAN_FILTER_RULE_DISCARD_UNTAGGED_FRAMES = ENUM_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE_DISCARD, /*!< Discard any untagged frame */
    ADI_EAL_VLAN_FILTER_RULE_PROCESS_UNTAGGED_FRAMES = ENUM_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE_PROCESS, /*!< Process untagged frames */
    ADI_EAL_VLAN_FILTER_RULE_IGNORE_AND_DROP_ALL_TAGS = ENUM_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE_IGNORE  /*!< Ignore and drop all VLAN tags and process the frame */
} ADI_EAL_VLAN_FILTER_RULE;

/** Topic subscriber in a node */
typedef enum ADI_EAL_TOPIC_SUBSCRIBER
{
    ADI_EAL_TOPIC_SUBSCRIBER_NODE_REGMAP, /*!< Node regmap is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_INF0,  /*!< Interface 0 is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_INF1,  /*!< Interface 1 is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_INF2,  /*!< Interface 2 is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_INF3,  /*!< Interface 3 is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_FIFO0, /*!< FIFO 0 is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_FIFO1, /*!< FIFO 1 is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_FIFO2, /*!< FIFO 2 is the topic subscriber */
    ADI_EAL_TOPIC_SUBSCRIBER_FIFO3  /*!< FIFO 3 is the topic subscriber */
} ADI_EAL_TOPIC_SUBSCRIBER;

/** @brief Interface mailbox data */
typedef struct ADI_EAL_CBK_EVT_MAILBOX_DATA
{
    uint8_t nSeqNum;                 /*!< Expected sequence number by the remote */
    uint8_t nTopic;                  /*!< Message topic */
    bool bFd;                        /*!< Indicates if frame was dropped by remote */
} ADI_EAL_CBK_EVT_MAILBOX_DATA;

/* ~~~~~~~~~~~~~~~~~ Top-level callback event data types ~~~~~~~~~~~~~~~~~ */
/** Data for #ADI_EAL_CBK_EVT_MAC_ADDR_RCVD event */
typedef uint8_t *ADI_EAL_CBK_EVT_MAC_ADDR_RCVD_DATA;

/** Data for #ADI_EAL_CBK_EVT_ERR event */
typedef ADI_E2BCORE_CBK_ERRDAT ADI_EAL_CBK_EVT_ERR_DATA;

/* ~~~~~~~~~~~~~~~~~ Node callback event data types ~~~~~~~~~~~~~~~~~ */
/** Data for #ADI_EAL_NODE_CBK_EVT_PING_RCVD event */
typedef void ADI_EAL_NODE_CBK_EVT_PING_RCVD_DATA;

/** Data for #ADI_EAL_NODE_CBK_EVT_LUT_PLCA_ID_RCVD event */
typedef uint8_t ADI_EAL_NODE_CBK_EVT_LUT_PLCA_ID_RCVD_DATA;

/** Data for #ADI_EAL_NODE_CBK_EVT_REVNUM_RCVD event */
typedef ADI_EAL_NODE_REV_NUM ADI_EAL_NODE_CBK_EVT_REVNUM_RCVD_DATA;

/** Data for #ADI_EAL_NODE_CBK_EVT_ERR event */
typedef ADI_E2BCORE_CBK_ERRDAT ADI_EAL_NODE_CBK_EVT_ERR_DATA;

/** Data for #ADI_EAL_NODE_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_NODE_CBK_EVT_REG_READ_RESP_DATA;

/** Data for #ADI_EAL_NODE_CBK_EVT_BOOTLOADER_DONE event */
typedef void ADI_EAL_NODE_CBK_EVT_BOOTLOADER_DONE_DATA;

/** Data for #ADI_EAL_NODE_CBK_EVT_UNK_IRQ event */
typedef ADI_E2BCORE_CBK_INTDAT ADI_EAL_NODE_CBK_EVT_UNK_IRQ_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern void adi_eal_init(void);
extern void adi_eal_createInstance(uint32_t nInstNum, uint32_t nDevNum, ADI_E2BCORE_CFG *poE2bCoreCfg);
extern void adi_eal_terminateInstance(uint32_t nInstNum);
extern void adi_eal_registerCallback(uint32_t nInstNum, ADI_EAL_TOP_CALLBACK_FUNC pfEalTopCbk);
extern void adi_eal_registerNodeCallback(uint32_t nInstNum, ADI_EAL_NODE_CALLBACK_FUNC pfEalNodeCbk);
extern void adi_eal_setFrameDuplication(uint32_t nInstNum, bool bEnableFrameDup);
extern ADI_EAL_STATUS adi_eal_setInputEnable(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nInputEnable);
extern ADI_EAL_STATUS adi_eal_setMacAddrSampling(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnableSampling);
extern ADI_EAL_STATUS adi_eal_getRemotePlcaLutId(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_setRemotePlcaId(uint32_t nInstNum, uint8_t *panMacAddr,
        ADI_EAL_NODE_PLCA_ID_CFG *poPlcaIdCfg, uint32_t nVlanTag);
extern ADI_EAL_STATUS adi_eal_setPlcaEn(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaEn);
extern ADI_EAL_STATUS adi_eal_setPlcaNodeCnt(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t nPlcaNodeCount);
extern ADI_EAL_STATUS adi_eal_setPlcaToTimer(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t nPlcaToTimer);
extern ADI_EAL_STATUS adi_eal_setPlcaBurstMode(uint32_t nInstNum, uint32_t nRemoteNum,
        uint8_t nPlcaBurstTimer, uint8_t nPlcaMaxBurstCnt);
extern ADI_EAL_STATUS adi_eal_setPlcaPrecedenceMode(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaPrecedenceModeEn);
extern ADI_EAL_STATUS adi_eal_setPlcaLeaderModeEn(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaLeaderModeEn);
extern ADI_EAL_STATUS adi_eal_setNodeAsPlcaLeader(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaLeader);
extern ADI_EAL_STATUS adi_eal_setPlcaLutEn(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaLutEn);
extern ADI_EAL_STATUS adi_eal_setGptpInitialRateCorrection(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnable);
extern ADI_EAL_STATUS adi_eal_startDiscoverySeq(uint32_t nInstNum);
extern ADI_EAL_STATUS adi_eal_stopDiscoverySeq(uint32_t nInstNum);
extern ADI_EAL_STATUS adi_eal_readCfgsFromRemote(uint32_t nInstNum);
extern ADI_EAL_STATUS adi_eal_configureNetwork(uint32_t nInstNum);
extern ADI_EAL_STATUS adi_eal_reconfigureNode(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_writeMacAddr(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t *panMacAddr);
extern ADI_EAL_STATUS adi_eal_updateMacAddr(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t *panMacAddr);
extern ADI_EAL_STATUS adi_eal_resetRemoteHvDie(uint32_t nInstNum);
extern ADI_EAL_STATUS adi_eal_resetRemoteLvDie(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_enableGptpTimerRemote(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_checkRemoteOtpValues(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_pingRemote(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_getRevision(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_sendRegTransaction(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_sendRegTransactionWithTopic(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, uint8_t nTopic);
extern ADI_EAL_STATUS adi_eal_readRegWithTimeout(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, uint32_t nTimeout);
extern ADI_EAL_STATUS adi_eal_setupDualMode(uint32_t nInstNum, uint32_t nRemoteNum);
extern void adi_eal_setNodeLvlRegVlan(uint32_t nInstNum, uint32_t nRemoteNum, ADI_VLAN_CFG *poVlanTag);
extern void adi_eal_setMcastNodeLvlRegVlan(uint32_t nInstNum, uint8_t nMcastTopic, ADI_VLAN_CFG *poVlanTag);
extern ADI_EAL_STATUS adi_eal_setUpstreamVlan(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_EAL_UPSTREAM_MSG_VLAN_TAG *poUpstreamVlanTags);
extern ADI_EAL_STATUS adi_eal_setMacFilterTable(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t anMacAddr[][6u],
        uint8_t nNumAddr);
extern ADI_EAL_STATUS adi_eal_setMacFilterFwdDestinationTable(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t anMacAddr[][6u],
    uint8_t nNumAddr, ADI_EAL_MAC_FWD_DEST aeFwdDest[]);
extern ADI_EAL_STATUS adi_eal_clearMacFilterTable(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_clearMacFilterFwdDestinationTable(uint32_t nInstNum, uint32_t nRemoteNum);
extern ADI_EAL_STATUS adi_eal_setVlanIdFilterTable(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t anVlanId[4U],
        ADI_EAL_VLAN_FILTER_RULE eFilterRule, uint32_t nVlanTag);
extern ADI_EAL_STATUS adi_eal_clearVlanIdFilterTable(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nVlanTag);
extern ADI_EAL_STATUS adi_eal_setGptpVlanIdFilter(uint32_t nInstNum, uint16_t nVlanId,
        ADI_EAL_VLAN_FILTER_RULE eFilterRule);
extern ADI_EAL_STATUS adi_eal_subscribeTopic(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t anTopic[],
        ADI_EAL_TOPIC_SUBSCRIBER aeTopicSubscriber[], uint8_t nNumTopicsCfg);
extern void adi_eal_registerInterfaceInterruptCbk(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
        ADI_EAL_INTERRUPT_CALLBACK_FUNC pfInterfaceInterruptCallback);

/*! \cond PRIVATE */
/*********************** Callbacks *******************/
extern void Eth10BaseT1sCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                            void *pCbParam);
extern void RemoteNodeCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                          void *pCbParam);
/*! \endcond */

#endif /* ADI_EAL_H */
/** @} */

/*
 * EOF: www.analog.com
 */
