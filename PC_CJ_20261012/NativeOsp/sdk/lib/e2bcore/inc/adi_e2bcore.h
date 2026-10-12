/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_e2bcore.h
 * \brief: The library interface header file for the ADI E2B core
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup e2bcore
 *  @{
 */

#ifndef ADI_E2BCORE_H
#define ADI_E2BCORE_H

/*================================= INCLUDES ================================*/
#include "adi_common.h"
#include "adi_memmap.h"             /* Memory map */
#include "adi_e2bcore_regfifo.h"    /* FIFO Register configuration definitions */
#include "adi_e2bcore_reghost.h"    /* HOST Register configuration definitions */
#include "adi_e2bcore_regi2c.h"     /* I2C Register configuration definitions */
#include "adi_io_lv_reg.h"          /* IO Register configuration definitions */
#include "adi_e2bcore_regiseled.h"  /* ISELED Register configuration definitions */
#include "adi_e2bcore_reglce.h"     /* LCE Register configuration definitions */
#include "adi_e2bcore_reglin.h"     /* LIN Register configuration definitions */
#include "adi_e2bcore_regport.h"    /* PORT Register configuration definitions */
#include "adi_e2bcore_regspi.h"     /* SPI Register configuration definitions */
#include "adi_e2bcore_regflexio.h"  /* Flexible IO Register configuration definitions */
#include "adi_e2bcore_reguart.h"    /* UART Register configuration definitions */
#include "adi_e2bcore_regadc.h"     /* ADC Register configuration definitions */
#include "adi_otp_reg.h"            /* OTP map Register configuration definitions */
#include "adi_io_hv_reg.h"          /* HV IO map Register configuration definitions */
#include "adi_maclce_reg.h"            /* MAC Register configuration definitions */
#include "adi_phy_std_reg.h"        /* PHY STD Register configuration definitions */
#include "adi_phy_oa_reg.h"         /* PHY VS Register configuration definitions */

/*================================= DEFINES =================================*/
#define ADI_E2BCORE_MMAP_CODE_CRIT              ADI_MEM_CODE_CRIT          /*!< L2 code placement macro */
#define ADI_E2BCORE_MMAP_CODE_NOCRIT            ADI_MEM_CODE_NOCRIT        /*!< L3 Code placement macro */
#define ADI_E2BCORE_MMAP_DATA_CONST             ADI_MEM_DATA_CONST         /*!< Const data placement macro */
#define ADI_E2BCORE_MMAP_DATA_CRIT_CACHE        ADI_MEM_DATA_CRIT_CACHE    /*!< L2 Cached data placement macro */
#define ADI_E2BCORE_MMAP_DATA_NOCRIT_CACHE      ADI_MEM_DATA_NOCRIT_CACHE  /*!< L3 Cached data placement macro */

#define ADI_E2BCORE_MAX_INST        (2u)            /*!< Max number of E2B lib instances */
#define ADI_E2BCORE_MAX_REG_IN_TXN  (256U)          /*!< Max number of register ops in a single transaction */
#define ADI_E2BCORE_ID_NA           (0xFFFFFFFFU)   /*!< N/A value for the ID */
#define ADI_E2BCORE_LAST_SEQNUM     (0xFFU)         /*!< The last sequence number before looping back */
#define ADI_E2BCORE_MAX_Q_DEPTH     (20U)           /*!< The maximum number of transactions that can be queued up */

/** Default topic definitions **/
#define ADI_E2BCORE_TOPIC_NODE        (0x00U)   /*!< Topic for remote node reg access */
#define ADI_E2BCORE_TOPIC_INTF0       (0x01u)   /*!< Topic for accessing interface 0 */
#define ADI_E2BCORE_TOPIC_INTF1       (0x02u)   /*!< Topic for accessing interface 1 */
#define ADI_E2BCORE_TOPIC_INTF2       (0x03u)   /*!< Topic for accessing interface 2 */
#define ADI_E2BCORE_TOPIC_INTF3       (0x04u)   /*!< Topic for accessing interface 3 */
#define ADI_E2BCORE_TOPIC_FIFO0       (0x05u)   /*!< Topic for accessing FIFO 0 */
#define ADI_E2BCORE_TOPIC_FIFO1       (0x06u)   /*!< Topic for accessing FIFO 1 */
#define ADI_E2BCORE_TOPIC_FIFO2       (0x07u)   /*!< Topic for accessing FIFO 2 */
#define ADI_E2BCORE_TOPIC_FIFO3       (0x08u)   /*!< Topic for accessing FIFO 3 */
#define ADI_E2BCORE_MAX_UCAST_TOPICS  (0x09U)   /*!< Max number of unicast topics */
#define ADI_E2BCORE_TOPIC_LCE         (0x7FU)   /*!< Fixed topic ID for LCE (127) */
#define ADI_E2BCORE_MAX_TOPICS        (128u)    /*!< The maximum number of topics in the network */
#define ADI_E2BCORE_MCAST_TOPIC_OFFSET (9U)     /*!< Offset of first multicast topic */
#define ADI_E2BCORE_MAX_MCAST_TOPICS  (119u)    /*!< Number of multicast topics (9-127) including LCE */
#define ADI_E2BCORE_TOPIC_MASK        (0x7FU)   /*!< The mask for the topic in the message header */

/** RX HW channels */
#define ADI_E2BCORE_MAX_RX_HW_CH    (6U) /*!< 4 FIFOs + 1 REGMAP + 1 FIFO_REJECT */
#define ADI_E2BCORE_HW_CH_REGMAP    (0U) /*!< REGMAP HW channel */
#define ADI_E2BCORE_HW_CH_FIFO0     (1U) /*!< FIFO 0 HW channel */
#define ADI_E2BCORE_HW_CH_FIFO1     (2U) /*!< FIFO 1 HW channel */
#define ADI_E2BCORE_HW_CH_FIFO2     (3U) /*!< FIFO 2 HW channel */
#define ADI_E2BCORE_HW_CH_FIFO3     (4U) /*!< FIFO 3 HW channel */
#define ADI_E2BCORE_HW_CH_FIFO_REJ  (5U) /*!< FIFO REJECT HW channel */

/* Register start offsets */
#define ADI_E2BCORE_REGOFFSET_SUB   (0xFC00U)          /*!< Offset address for subscription table */

/*! E2B Message Types */
#define E2B_NUM_MSGTYPE                  (0x07U)          /*!< Number of message types */
#define E2B_MSGTYPE_IDLE                 (0x00u)          /*!< Idle message type */
#define E2B_MSGTYPE_REGMAP_IO            (0x01u)          /*!< Regmap IO message type */
#define E2B_MSGTYPE_FIFO_WRITE_IO        (0x02u)          /*!< Fifo write message type */
#define E2B_MSGTYPE_FIFO_WRITE_REJECTED  (0x03u)          /*!< Fifo write rejected message type */
#define E2B_MSGTYPE_INTERRUPT            (0x04u)          /*!< Interrupt message type */
#define E2B_MSGTYPE_NACK_STREAM          (0x05u)          /*!< Nack stream message type */
#define E2B_MSGTYPE_ACK_MB               (0x06u)          /*!< Ack mailbox message type */

/*! ACF Subtypes */
#define ADI_ACF_NTSCF_MSG                (0x82u)           /*!< ACF Non time synchronous control format sub type */
#define ADI_ACF_TSCF_MSG                 (0x05u)           /*!< ACF time synchronous control format sub type */

/*! ACF Message Types */
#define ADI_ACF_REGMAP_IO                 (0x7Bu)          /*!< ACF Regmap IO message type */
#define ADI_ACF_FIFO_WRITE                (0x7Cu)          /*!< ACF Fifo write message type */

#define E2B_REGMAP_MAX   (200u) /*!< Maximum number of registers that can be written in one message */

#define E2B_L2_HEADER_SIZE                (14u)   /*!< L2 header size */
#define E2B_SI_ANOMALY_FRAME_DROP_SIZE    (59u)   /*!< Silicon anomaly - frame drop packet size */

/*================================ DATA TYPES ===============================*/
/*! @brief Callback types */
typedef enum ADI_E2BCORE_CBKTYPE
{
    ADI_E2BCORE_CBK_ERR,           /*!< Triggered when there is an Error */
    ADI_E2BCORE_CBK_INT,           /*!< Triggered when there is an Interrupt */
    ADI_E2BCORE_CBK_ACK,           /*!< Triggered when there is a ack from the remote to the command sent */
    ADI_E2BCORE_CBK_REGREAD,       /*!< Triggered when there register read data is received from the remote */
    ADI_E2BCORE_CBK_FIFOREAD,      /*!< Triggered when fifo rx data is received from the remote */
    ADI_E2BCORE_CBK_INITGETBUF,    /*!< Request a packet from the application to initialize the network */
    ADI_E2BCORE_CBK_INITTRANSMIT,  /*!< Request application to transmit a packet */
    ADI_E2BCORE_CBK_MAC_ADDR_RCVD, /*!< MAC address event for discovery */
} ADI_E2BCORE_CBKTYPE;

/*! @brief Interrupt types */
typedef enum ADI_E2BCORE_INTTYPE
{
    ADI_E2BCORE_INT_LOW,      /*!< Low level triggered interrupt */
    ADI_E2BCORE_INT_HIGH,     /*!< High level triggered interrupt */
    ADI_E2BCORE_INT_RISING,   /*!< Rising edge triggered interrupt */
    ADI_E2BCORE_INT_FALLING   /*!< Falling edge triggered interrupt */
} ADI_E2BCORE_INTTYPE;

/*! @brief E2B error types */
typedef enum ADI_E2BCORE_ERR
{
    ADI_E2BCORE_ERR_NONE        = (int32_t)  0,  /*!< No Error */
    ADI_E2BCORE_ERR_UNK         = (int32_t) -1,  /*!< Unknown Error */
    ADI_E2BCORE_ERR_PARAM       = (int32_t) -2,  /*!< Input parameters are wrong */
    ADI_E2BCORE_ERR_PKTSADR     = (int32_t) -3,  /*!< Received frame's source address does not match MAC address
                                                      of any remote node in the config file */
    ADI_E2BCORE_ERR_PKTDADR     = (int32_t) -4,  /*!< Received frame's destination address does not match
                                                      MAC address of the controller node in the config file */
    ADI_E2BCORE_ERR_PKTETYP     = (int32_t) -5,  /*!< Received packet EtherType is incorrect */
    ADI_E2BCORE_ERR_PKTCMD      = (int32_t) -6,  /*!< The node was not expecting a packet of this command */
    ADI_E2BCORE_ERR_PKTLEN      = (int32_t) -7,  /*!< Received packet length is incorrect */
    ADI_E2BCORE_ERR_PKTNACK     = (int32_t) -8, /*!< Received a NACK */
    ADI_E2BCORE_ERR_PKTREJ      = (int32_t) -9, /*!< Received a FIFO rejected packet */
    ADI_E2BCORE_ERR_PKTFMT      = (int32_t) -10, /*!< The packet format was incorrect */
    ADI_E2BCORE_ERR_BUF_INSUF   = (int32_t) -11, /*!< Buffer length is insufficent for the packet to be sent out */
    ADI_E2BCORE_ERR_REG_INVOP   = (int32_t) -12, /*!< Invalid regmap operation */
    ADI_E2BCORE_ERR_PORT_INVCMD = (int32_t) -13, /*!< Invalid Port controller interface command */
    ADI_E2BCORE_ERR_ADC_INVCMD  = (int32_t) -14, /*!< Invalid ADC interface command */
    ADI_E2BCORE_ERR_I2C_INVCMD  = (int32_t) -15, /*!< Invalid I2C interface transaction */
    ADI_E2BCORE_ERR_SPI_INVCMD  = (int32_t) -16, /*!< Invalid SPI interface transaction */
    ADI_E2BCORE_ERR_FLEXIBLE_IO_INVCMD  = (int32_t) -17, /*!< Invalid Flexible IO interface transaction */
    ADI_E2BCORE_ERR_LIN_INVCMD  = (int32_t) -18, /*!< Invalid LIN interface transaction */
    ADI_E2BCORE_ERR_ISELED_INVCMD = (int32_t) -19, /*!< Invalid ISELED interface transaction */
    ADI_E2BCORE_ERR_UART_INVCMD = (int32_t) -20, /*!< Invalid UART interface transaction */
    ADI_E2BCORE_ERR_INV_SEQNUM  = (int32_t) -21, /*!< Invalid RX sequence number */
    ADI_E2BCORE_ERR_NULL_PTR    = (int32_t) -22, /*!< Null pointer error */
} ADI_E2BCORE_ERR;

/*! @brief E2B sub-error types */
typedef enum ADI_E2BCORE_SUBERR
{
    ADI_E2BCORE_SUBERR_NONE            = (int32_t) 0,  /*!< No error */
    ADI_E2BCORE_SUBERR_FRAMELEN        = (int32_t) -1,  /*!< Invalid packet frame length */
    ADI_E2BCORE_SUBERR_RESPACKLEN      = (int32_t) -2,  /*!< Invalid ACK/NACK message header length  */
    ADI_E2BCORE_SUBERR_COMMSGHDRLEN    = (int32_t) -3,  /*!< Invalid common message header length */
    ADI_E2BCORE_SUBERR_COMHDRLEN       = (int32_t) -4,  /*!< Invalid common header length */
    ADI_E2BCORE_SUBERR_RESPINTMSGLEN   = (int32_t) -5,  /*!< Invalid interrupt response message length */
    ADI_E2BCORE_SUBERR_PORTCTRLINTF_INVCMD = (int32_t) -6,  /*!< Invalid Port controller interface command */
    ADI_E2BCORE_SUBERR_PORTCTRL_MSGQEMPTY  = (int32_t) -7,  /*!< Port controller message queue is empty */
    ADI_E2BCORE_SUBERR_REGMAPIO_INVTXNTYPE = (int32_t) -8,  /*!< Regmap IO transaction type is invalid */
    ADI_E2BCORE_SUBERR_TOPICID_INV         = (int32_t) -9,  /*!< There is no mapping to the received topic ID */
} ADI_E2BCORE_SUBERR;

/*! @brief E2B interface types */
typedef enum ADI_E2BCORE_IF_TYPE
{
    ADI_E2BCORE_IF_TYPE_UNDEF,    /*!< Undefined */
    ADI_E2BCORE_IF_TYPE_SPI,      /*!< SPI interface */
    ADI_E2BCORE_IF_TYPE_FIO,      /*!< Flexible IO interface */
    ADI_E2BCORE_IF_TYPE_I2C,      /*!< I2C interface */
    ADI_E2BCORE_IF_TYPE_ISELED,   /*!< ISELED interface */
    ADI_E2BCORE_IF_TYPE_LIN,      /*!< LIN interface */
    ADI_E2BCORE_IF_TYPE_UART,     /*!< UART interface */
    ADI_E2BCORE_IF_TYPE_PORT,     /*!< Port controller */
    ADI_E2BCORE_IF_TYPE_ADC,      /*!< ADC interface */
    ADI_E2BCORE_IF_TYPE_COUNT     /*!< Total number of interface types */
} ADI_E2BCORE_IF_TYPE;

/*! @brief REGMAP_IO transaction types */
typedef enum ADI_E2BCORE_REGMAP_IO_TYPE
{
    ADI_E2BCORE_RND_WRITE,        /*!< Random writes */
    ADI_E2BCORE_RND_MASK_WRITE,   /*!< Random mask writes */
    ADI_E2BCORE_BURST_WRITE,      /*!< Burst writes */
    ADI_E2BCORE_BURST_MASK_WRITE, /*!< Burst mask writes */
    ADI_E2BCORE_RND_READ,         /*!< Random reads */
    ADI_E2BCORE_BURST_READ,       /*!< Burst reads */
    ADI_E2BCORE_RND_READ_RESP,    /*!< Random reads response*/
    ADI_E2BCORE_BURST_READ_RESP   /*!< Burst reads response */
} ADI_E2BCORE_REGMAP_IO_TYPE;

/*! @brief E2B version information */
typedef struct ADI_E2BCORE_VERSION
{
    uint32_t nMajorVersion;     /*!<Library major version number */
    uint32_t nMinorVersion;     /*!< Library minor version number */
    uint32_t nPatchVersion;     /*!< Library patch number */
} ADI_E2BCORE_VERSION;

/*! @brief E2B build date information */
typedef struct ADI_E2BCORE_BUILD_INFO
{
    char     sBuildTime[21u];  /*!< Library build date and time */
} ADI_E2BCORE_BUILD_INFO;

/*! @brief E2B register address value structure */
typedef struct ADI_E2BCORE_REG_ADDR_VAL
{
    uint16_t nAddr;       /*!< register Address */
    uint32_t nVal;        /*!< register Value */
} ADI_E2BCORE_REG_ADDR_VAL;

/*! @brief E2B Node identification structure */
typedef struct ADI_E2BCORE_ID
{
    uint32_t nInstNum;              /*!< Instance number */
    uint32_t nRemoteNum;            /*!< E2B remote number */
    uint32_t nIntfNum;              /*!< Interface id */
} ADI_E2BCORE_ID;

/*! Callback prototype */
typedef void (*ADI_E2BCORE_CBK)(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                                void *pCbParam);

/*! @brief E2B interface config structure */
typedef struct ADI_E2BCORE_IF_CONFIG
{
    ADI_E2BCORE_ID oId;                      /*!< The ID of this interface */
    bool bEnabled;                           /*!< If this interface is enabled */
    ADI_E2BCORE_IF_TYPE eIfType;             /*!< Type of interface */
    uint32_t nIfTopic;                       /*!< The default topic of the interface */
    uint32_t nNumIfReg;                      /*!< Number of interface registers */
    ADI_E2BCORE_REG_ADDR_VAL *aoIfRegData;   /*!< Pointer to the interface register values */
    uint32_t nFifoTopic;                     /*!< The default topic of the FIFO */
    uint32_t nNumFifoReg;                    /*!< Number of FIFO registers */
    ADI_E2BCORE_REG_ADDR_VAL *aoFifoRegData; /*!< Pointer to the FIFO register values */
    void *pCbParam;                          /*!< Parameter to be passed back while invoking callback */
    ADI_E2BCORE_CBK pfIntfCb;                /*!< Callback for the interface */
    uint32_t nIfRegVlanTag;                  /*!< VLAN tag for interface register access */
    uint32_t nFifoRegVlanTag;                /*!< VLAN tag for FIFO register access */
    uint32_t nFifoWriteVlanTag;              /*!< VLAN tag for FIFO write transactions */

    /**
     * Need not be initialized by application and is used internally
     */
    uint8_t  nCurrState;            /*!< Current RX processing state of the interface */
    uint16_t nTxnQDepth;            /*!< Depth of transaction config queue */
    uint16_t nTxnQWrIdx;            /*!< Write index of transaction config queue */
    uint16_t nTxnQRdIdx;            /*!< Read index of transaction config queue */
    uint32_t nCurrTxnReadLen;       /*!< Bytes queued up in the current transaction config */
    uint32_t nStateCnt;             /*!< Count in the current state */
    void    **apoTxnCfgs;           /*!< Array of pointers to transaction config queue */
    void    *poRxTxnCfg;            /*!< RX transaction config (Used in port_ctrl, ADC, UART) */
    bool     bCurrCfgRx;            /*!< Flag indicating if RX transaction is being processed currently */
    uint8_t  anMcastTopics[ADI_E2BCORE_MAX_MCAST_TOPICS]; /*!< Array of multicast topics (9-127) this interface subscribes to, indexed by (topic - 9), 0 = not subscribed */
} ADI_E2BCORE_IF_CONFIG;

/*! @brief E2B Remote configuration structure */
typedef struct ADI_E2BCORE_REMOTE_CONFIG
{
    ADI_E2BCORE_ID oId;                     /*!< The ID of this remote */
    bool bEnabled;                          /*!< If this node is enabled or disabled */
    uint32_t nNumRegisters;                 /*!< Number of registers in the configuration */
    ADI_E2BCORE_REG_ADDR_VAL *aoRegData;    /*!< Pointer to the register configuration values */
    uint8_t nNumIf;                         /*!< Number of interfaces for the remote */
    ADI_E2BCORE_IF_CONFIG *aoIntfConfigs;   /*!< Interface config array */
    void *pCbParam;                         /*!< Parameter to be passed back while invoking callback */
    ADI_E2BCORE_CBK pfRemoteCb;             /*!< Remote callback */
    uint8_t anMacAddr[6U];                  /*!< The MAC address of the E2B remote */
    bool bIsLocalLCE;                       /*!< If set true, the node is a part of controller, to be filled explicitly */
    uint32_t nNodeRegVlanTag;               /*!< VLAN tag for node level register access */

    /**
     * Need not be initialized by application and is used internally
     */
    uint8_t anSeqNum[ADI_E2BCORE_MAX_UCAST_TOPICS];     /*!< The next sequence number to be transmitted for each of the topics */
    uint8_t anExpSeqNum[ADI_E2BCORE_MAX_RX_HW_CH];      /*!< The next expected RX sequence number for each of the HW channels */
} ADI_E2BCORE_REMOTE_CONFIG;

/*! The stack configuration
 * @note This structure and all it's children must persist beyond the
 * call of adi_e2bcore_init for the entire duration of the library's
 * operation.
 * */
/*! @brief E2B stack configuration structure */
typedef struct ADI_E2BCORE_CFG
{
    uint8_t anMacAddr[6U];                           /*!< The MAC address of the E2B controller */
    uint32_t nNumNodes;                              /*!< Number of nodes in the network */
    ADI_E2BCORE_REMOTE_CONFIG *aoRemoteCfgs;         /*!< Remote configuration */
    void *pCbParam;                                  /*!< Parameter to be passed back while invoking callback */
    ADI_E2BCORE_CBK pfLibCb;                         /*!< Callback */
    bool bInitializeNetwork;                         /*!< Tells if the lib must initialize the network during adi_e2bcore_init */
    bool bNodeConfigured;                            /*!< When true, enables topic=0 (TOPIC_NODE) override from instance creation for OTP-programmed nodes.
                                                      *   Application must set this before adi_eal_createInstance(). When true, EAL applies topic override
                                                      *   for all node register transactions from the start, rather than waiting for adi_eal_configureNetwork(). */
    uint32_t anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS]; /*!< Contains VLAN tags for multicast group node level register access */

    /**
     * Need not be initialized by application and is used internally
     */
    uint8_t anMcastSeqNum[ADI_E2BCORE_MAX_TOPICS];  /*!< The next sequence number to be transmitted for each of the topics */
    bool bIsLocalLCEConfigured;                     /*!< Indicates if the local LCE is configured on controller node as a part the e2b network */
} ADI_E2BCORE_CFG;

/*! @brief Message options for E2B command */
typedef struct ADI_E2BCORE_MSGOPTS
{
    /**
     * To be filled by the application when invoking any of the adds
     * These fields are also used by ADI_E2BCORE_CBK
     */
    /*! Indicates whether the remote should send a response to the command */
    bool bMailbox;
    /*! Indicates whether the response is an acknowledgement */
    bool bSync;
    /*! Indicates that the command has an additional destination topic address and overrides the default topic address */
    bool bTopicOverRide;
    /*! Indicates the topic number */
    uint32_t nTopic;

    /**
     * Not required to be filled by application when invoking adds, but is used when providing callbacks via ADI_E2BCORE_CBK
     */
    /*! Sequence number of the packet received */
    uint32_t nSeqNum;
    /*! Total message length */
    uint32_t nMsgLen;
    /*! The type of E2B message */
    uint32_t nType;
    /*! The frame drop returned */
    bool  bFd;

} ADI_E2BCORE_MSGOPTS;

/*! @brief Error callback data structure */
typedef struct ADI_E2BCORE_CBK_ERRDAT
{
    ADI_E2BCORE_ERR eError;   /*!< The type of error */
    const void *poData;       /*!< Any data associated with the error */
} ADI_E2BCORE_CBK_ERRDAT;

/*! @brief Interrupt callback data structure */
typedef struct ADI_E2BCORE_CBK_INTDAT
{
    const ADI_E2BCORE_MSGOPTS *poMsgOpts;   /*!< The header information */
    ADI_E2BCORE_INTTYPE eType;              /*!< Interrupt type */
    uint8_t nIdentifier;                    /*!< The interrupt identifier */
    uint8_t nMask;                          /*!< The interrupt mask */
} ADI_E2BCORE_CBK_INTDAT;

/*! @brief ACK callback data structure */
typedef struct ADI_E2BCORE_CBK_ACKDAT
{
    const ADI_E2BCORE_MSGOPTS *poMsgOpts;   /*!< The header information */
    uint8_t nSeqNum;                        /*!< The sequence number */
    uint8_t nTopic;                         /*!< The topic returned */
    bool  bFd;                              /*!< The Frame Drop returned */
} ADI_E2BCORE_CBK_ACKDAT;

/*! @brief FIFO Write Reject callback data structure */
typedef struct ADI_E2BCORE_CBK_FIFOREJDAT
{
    const ADI_E2BCORE_MSGOPTS *poMsgOpts;   /*!< The header information */
    uint32_t nSeqNum;                       /*!< The originating sequence number */
} ADI_E2BCORE_CBK_FIFOREJDAT;

/*! @brief Data callback data structure */
typedef struct ADI_E2BCORE_CBK_FIFOREADDAT
{
    ADI_E2BCORE_MSGOPTS *poMsgOpts; /*!< The header information */
    void *poInfo; /*!< Interface specific information */
} ADI_E2BCORE_CBK_FIFOREADDAT;

/*! @brief Data for the initgetbuf callback event */
typedef struct ADI_E2BCORE_CBK_INITGETBUFDAT
{
    uint32_t nLength; /*!< The length of the buffer requested, will be < 1500 */
    uint8_t *pBuffer; /*!< Application should update this pointer with a buffer where the stack can fill the data */
    uint32_t nNumVlanTags; /*!< Number of VLAN tags to be inserted in the ethernet frame */
    const uint8_t *panMacAddr; /*!< Pointer to MAC address*/
} ADI_E2BCORE_CBK_INITGETBUFDAT;

/*! @brief Data for the inittransmit event */
typedef struct ADI_E2BCORE_CBK_INITTRANSMITDAT
{
    uint32_t nActLength;              /*!< The actual length of the frame */
    const uint8_t *pBuffer;           /*!< The buffer to be transmitted */
    const uint8_t *anDestAddr;        /*!< The destination address to which this packet must be sent */
    bool bError;                      /*!< Allows application to notify if there is an error */
    bool bToLocalLCE;                 /*!< If set true, the packet is destined for local LCE */
    uint32_t nVlanTag;                /*!< VLAN tag for the frame */
} ADI_E2BCORE_CBK_INITTRANSMITDAT;

/*! @brief Regmap IO data structure */
typedef struct ADI_E2BCORE_REGMAP_IO_CONFIG
{
    const uint16_t *anStartAddr;      /*!< Start address for register writes */
    uint8_t nLength;                  /*!< Number of registers to write to / read from*/
    uint32_t *pBuffer;                /*!< Pointer to buffer containing register data to be written */
    const uint32_t *anMask;           /*!< Pointer to buffer containing register masks to be written */
    ADI_E2BCORE_REGMAP_IO_TYPE eType; /*!< Write Type */
} ADI_E2BCORE_REGMAP_IO_CONFIG;

/*! @brief Register read callback data stucture */
typedef struct ADI_E2BCORE_CBK_REGREADDAT
{
    const ADI_E2BCORE_MSGOPTS *poMsgOpts;       /*!< Header data */
    const ADI_E2BCORE_REGMAP_IO_CONFIG *poInfo; /*!< Information on the register read */
} ADI_E2BCORE_CBK_REGREADDAT;

/*! @brief Structure for sending out TX dissected data when no config is present */
typedef struct ADI_E2BCORE_TX_CONFIG
{
    const uint8_t *pBuffer; /*!< FIFO data */
    uint32_t nLength;       /*!< Length of the FIFO data */
} ADI_E2BCORE_TX_CONFIG;

/*! @brief ISELED modes */
typedef enum E2B_ISELED_MODE
{
    E2B_ISELED_MODE_SAFE = ENUM_ISELED_CONTROL_CHANNEL_MODE_SAFE_MODE,     /*!< Safe mode */
    E2B_ISELED_MODE_OPTIMAL = ENUM_ISELED_CONTROL_CHANNEL_MODE_OPTIMAL_MODE,  /*!< Optimal mode*/
    E2B_ISELED_MODE_FAST = ENUM_ISELED_CONTROL_CHANNEL_MODE_FAST_MODE,     /*!< Fast mode */
    E2B_ISELED_MODE_INVALID,  /*!< Invalid mode */
} E2B_ISELED_MODE;

/*! @brief Transaction header - transactions of FIFO_WRITE message */
typedef struct ADI_E2BCORE_TXN_HDR
{
    /* ~~~~~ Common header flags (Valid for all the interfaces) ~~~~~ */
    bool        bIsGptpTimePresent;     /*!< Indicates if gPTP timestamp is present */
    bool        bIsTimeDelayPresent;    /*!< Indicates if transaction delay is specified */
    bool        bAreTxnsPacked;         /*!< Indicates if there are num packed transactions in the message */
    bool        bResyncTillThisTxn;     /*!< Indicates if this a resync transaction */
    bool        bContinueNextTxn;       /*!< Indicates if this transaction has to be continued */
    bool        bEnIntfTxnHdr;          /*!< Indicates if interface header is present, valid for SPI, I2C, ISELED, FIO */
    bool        bIsE2bIdPresent;        /*!< Indicates if E2B ID is present */

    /* ~~~~~ Common header values (Valid for all the interfaces) ~~~~~ */
    uint64_t    nGptpTime;          /*!< gPTP Timestamp */
    uint32_t    nTimeDelay;         /*!< Time delay */
    uint16_t    nNumPackedTxns;     /*!< Number of packed transactions */
    uint8_t     nE2bId;             /*!< E2B id */

    /* ~~~~~ Interface specific header flags ~~~~~ */
    /*! Indicates if write length has to be overriden in this transaction, valid for SPI, I2C, FIO interfaces */
    bool        bOverrideWriteLen;
    /*! Indicates if read length has to be overriden in this transaction, valid for SPI, I2C, FIO interfaces */
    bool        bOverrideReadLen;
    /*! Indicates if read ignored length has to be overriden in this transaction, valid for SPI interface */
    bool        bOverrideReadIgnLen;
    /*! Indicates if chip select has to be overriden in this transaction, valid for SPI interface */
    bool        bOverrideChipSel;
    /*! Indicates if I2C address has to be overriden in this transaction */
    bool        bOverrideDevAddr;
    /*! Indicates if I2C address for this transaction has to be 10 bits */
    bool        bIsAddr10Bits;
    /*! Indicates if sequential mode is enabled in this transaction, valid for ISELED interface */
    bool        bIseledSeqModeEn;
    /*! Indicates if expected number of responses is set is enabled in this transaction, valid for ISELED interface */
    bool        bSetExpectedNumResp;
    /*! Indicates if dim RGB is enabled for ISELED interface */
    bool        bIseledDimRgbEn;
    /*! Indicates if the FIO transaction payload includes the FIFO to SM router header */
    bool        bIsRouterHeaderInserted;

    /* ~~~~~ Interface specific header values ~~~~~ */
    uint16_t    nWriteLen;          /*!< Write length for SPI, I2C, FIO interface transactions */
    uint16_t    nReadLen;           /*!< Read length for SPI, I2C, FIO interface transactions */
    uint16_t    nReadIgnLen;        /*!< Read ignored length for SPI interface transactions */
    uint8_t     nChipSel;           /*!< Chip select for SPI interface transactions */
    uint16_t    nAddress;           /*!< Address for I2C interface transactions */
    uint16_t    nExpectedNumResp;   /*!< Expected number of responses for ISELED interface transactions */

    /* NOTE: ISELED mode and CRC control are determined by the register settings. These fields are
             not to be populated by the application */
    E2B_ISELED_MODE eMode;          /*!< ISELED mode - Library determines mode from the register settings */
    uint16_t    nNumValidResp;      /*!< Valid number of upstream ISELED transactions */
    bool        bIseledCrcEn;       /*!< CRC is enabled or not for the ISELED transactions */
} ADI_E2BCORE_TXN_HDR;

/*! @brief Transaction configuration - transactions of FIFO_WRITE message */
typedef struct ADI_E2BCORE_TXN_CONFIG
{
    ADI_E2BCORE_TXN_HDR oTxnHdr;    /*!< Transaction Header */
    void *pPayload;                 /*!< Transaction Payload(s) */
} ADI_E2BCORE_TXN_CONFIG;

/*! @brief Error callback data structure for packet length */
typedef struct ADI_E2BCORE_CBK_ERRDATA_PKTLEN
{
    ADI_E2BCORE_SUBERR eSubError; /*!< The type of sub-error */
    uint32_t   nReqBufLen;        /*!< Maximum required buffer length */
    uint32_t   nActBufLen;        /*!< Actual buffer length */
} ADI_E2BCORE_CBK_ERRDATA_PKTLEN;

/*! @brief Error callback data structure for packet format */
typedef struct ADI_E2BCORE_CBK_ERRDATA_PKTFMT
{
    ADI_E2BCORE_SUBERR eSubError; /*!< The type of sub-error */
    uint32_t nRxTopic;            /*!< Indicates the topic number */
} ADI_E2BCORE_CBK_ERRDATA_PKTFMT;

/*! @brief Error callback data structure for invalid sequence number*/
typedef struct ADI_E2BCORE_CBK_ERRDATA_INV_SEQNUM
{
    ADI_E2BCORE_SUBERR eSubError;   /*!< The type of sub-error */
    ADI_E2BCORE_MSGOPTS *poMsgOpts; /*!< Pointer to Message options of E2B command */
    uint8_t nExpSeqNum ;            /*!< Indicates the expected sequence number */
} ADI_E2BCORE_CBK_ERRDATA_INV_SEQNUM;

/*! @brief Error callback data structure for source/destination MAC address */
typedef struct ADI_E2BCORE_CBK_ERRDATA_PKTDADR_SADR
{
    uint8_t anRemoteNodeMacAddr[6];  /*!< Source/destination MAC address of remote node */
} ADI_E2BCORE_CBK_ERRDATA_PKTDADR_SADR;

/*! @brief Error callback data structure for register map invalid operation */
typedef struct ADI_E2BCORE_CBK_ERRDATA_REG_INVOP
{
    ADI_E2BCORE_SUBERR eSubError;            /*!< The type of sub-error */
    ADI_E2BCORE_CBK_REGREADDAT *poRegRdData; /*!< Pointer to register read callback data */
} ADI_E2BCORE_CBK_ERRDATA_REG_INVOP;

/*! @brief Error callback data structure for port controller invalid command */
typedef struct ADI_E2BCORE_CBK_ERRDATA_PORT_INVCMD
{
    ADI_E2BCORE_SUBERR eSubError; /*!< The type of sub-error */
    ADI_E2BCORE_CBK_FIFOREADDAT *poData; /*!< Pointer to FIFO read callback data */
} ADI_E2BCORE_CBK_ERRDATA_PORT_INVCMD;

/*================================ EXTERNALS ===============================*/
/** The common multicast MAC address */
extern uint8_t ganAdiE2bMulticastAddr[6U];

/*! The secondary upstream MAC address */
extern uint8_t ganE2bSecMcastMacAddr[6U];

/*! Error callback data for packet length */
extern ADI_E2BCORE_CBK_ERRDATA_PKTLEN goErrDataPktLen;

/*============================ PUBLIC PROTOTYPES ============================*/
/* Management APIs */
extern void adi_e2bcore_getVersion(ADI_E2BCORE_VERSION *poVersionInfo);
extern void adi_e2bcore_getBuildInfo(ADI_E2BCORE_BUILD_INFO *poBuildInfo);

/* Constructor API */
extern void adi_e2bcore_initStack(uint32_t nInstNum, ADI_E2BCORE_CFG *poCfg);
/* Destructor API */
extern void adi_e2bcore_terminateStack(uint32_t nInstNum);
/* Configure the e2b network */
extern ADI_E2BCORE_ERR adi_e2bcore_configureNetwork(uint32_t nInstNum);
/* Reconfigure node */
extern ADI_E2BCORE_ERR adi_e2bcore_reconfigureNode(uint32_t nInstNum, uint32_t nRemoteNum);

/* Discovery sequence APIs */
extern void adi_e2bcore_startDiscoverySeq(uint32_t nInstNum);
extern void adi_e2bcore_stopDiscoverySeq(uint32_t nInstNum);

/* Update config APIs */
void adi_e2bcore_startCfgUpdateSeq(uint32_t nInstNum);
void adi_e2bcore_stopCfgUpdateSeq(uint32_t nInstNum);
void adi_e2bcore_updateMacAddr(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t *pMacAddr);
void adi_e2bcore_useMcastAddr(uint32_t nInstNum, uint8_t *pMacAddr);
void adi_e2bcore_useSecondaryMcastAddr(uint32_t nInstNum, uint8_t *pMacAddr);

/* reception*/
extern void adi_e2bcore_processFrame(uint32_t nInstNum, const uint8_t *pBuffer,
                                     uint32_t nBufLength);
extern ADI_E2BCORE_ERR adi_e2bcore_setInterfaceTransactionConfigQueue(ADI_E2BCORE_ID *poId, void *pCfgQueueMem,
        uint16_t nQueueLength);

/* Packet management */
extern ADI_E2BCORE_ERR adi_e2bcore_startPacket(uint32_t nInstNum, const uint8_t anMacAddr[], uint8_t *pBuffer,
        uint32_t nBufLength, bool bIs1722Encoding);
extern void adi_e2bcore_finishPacket(uint32_t nInstNum, uint32_t *pnActLength,
                                     bool bIsMacHdrIncl, uint8_t nNumVlanTags);

/*********************** Commands *******************/
/* Idle message */
extern ADI_E2BCORE_ERR adi_e2bcore_addIdleMessage(uint32_t nInstNum);
/*Register Read/Write */
extern ADI_E2BCORE_ERR adi_e2bcore_addRegmapIO(
    uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMessageOptions,
    const ADI_E2BCORE_REGMAP_IO_CONFIG aoRegmapConfig[], uint8_t nNumConfigs);

/* Alternate flows */
extern ADI_E2BCORE_ERR adi_e2bcore_startMessage(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMessageOptions,
        uint32_t nType);
extern void adi_e2bcore_finishMessage(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMessageOptions);

extern ADI_E2BCORE_ERR adi_e2bcore_addRndWriteTxn(uint32_t nInstNum, uint8_t nLength, const uint16_t anAddr[],
        uint32_t anData[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addRndMaskWriteTxn(uint32_t nInstNum, uint8_t nLength,
        const uint16_t anAddr[], uint32_t anData[], const uint32_t anMask[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addBurstWriteTxn(uint32_t nInstNum, uint8_t nLength,
        uint16_t nStartAddr, uint32_t anData[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addBurstMaskWriteTxn(uint32_t nInstNum, uint8_t nLength,
        uint16_t nStartAddr, uint32_t anData[], const uint32_t anMask[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addRndReadTxn(uint32_t nInstNum, uint8_t nLength, const uint16_t anAddr[]);
extern ADI_E2BCORE_ERR adi_e2bcore_addBurstReadTxn(uint32_t nInstNum, uint8_t nLength, uint16_t nStartAddr);

/*********************** FIFO Control *******************/
extern ADI_E2BCORE_ERR adi_e2bcore_addFifoReset(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts);
extern ADI_E2BCORE_ERR adi_e2bcore_addFifoReadManual(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
        uint32_t nMaxBytes);
extern ADI_E2BCORE_ERR adi_e2bcore_addFifoReadTrigger(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts, bool bFull);
extern ADI_E2BCORE_ERR adi_e2bcore_addFifoReadOnDemand(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
        uint32_t nMinBytes,
        uint32_t nMaxBytes);

#endif /* ADI_E2BCORE_H */

/** @} */

/*
 * EOF: www.analog.com
 */
