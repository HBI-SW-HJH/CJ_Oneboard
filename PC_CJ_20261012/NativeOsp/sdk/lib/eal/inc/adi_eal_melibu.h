/*******************************************************************************
Copyright (c) 2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_melibu.h
 * \brief:The header file for Melibu functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_MELIBU_H
#define ADI_EAL_MELIBU_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "adi_eal_fioUart.h"

#ifdef ADI_E2B_IFACE_MELIBU_ENABLED


/*================================= DEFINES =================================*/

#define ADI_EAL_MELIBU_FRAME_ID_LENGTH              (2U)    /*!< Id number of bytes */
#define ADI_EAL_MELIBU_MAX_DATA_LENGTH              (128U)  /*!< Maximum length of Melibu 2.0 payload */
#define ADI_EAL_MELIBU_FRAME_CRC_LENGTH             (2U)    /*!< Number of bytes of CRC*/

#define ADI_EAL_MELIBU_FRAME_TYPE_SLAVE_RECEIVE     (0U)    /*!< Slave receive flag value */
#define ADI_EAL_MELIBU_FRAME_TYPE_SLAVE_TRANSMIT    (1U)    /*!< Slave transmit flag value */

#define ADI_EAL_MELIBU_MAXIMUM_INTF_PER_NODE        (2U)    /*!< Maximum numnber of interfaces per node to configure */

/** @brief Maximum transmission frame buffer length sent */
#define ADI_EAL_MELIBU_FRAME_MAX_LENGTH  (ADI_EAL_MELIBU_FRAME_ID_LENGTH + ADI_EAL_MELIBU_MAX_DATA_LENGTH + ADI_EAL_MELIBU_FRAME_CRC_LENGTH)

/** @brief Internal helper macro to compute the memory size required for each MELIBU unicast instance */
#define ADI_EAL_MELIBU_UNICAST_INST_MEM_SIZE(nRxQueueDepth) \
    (sizeof(EAL_MELIBU_INST_DATA) + sizeof(EAL_MELIBU_INST_DATA*) + sizeof(EAL_MELIBU_DATA) + \
    (ADI_EAL_MELIBU_FRAME_MAX_LENGTH * 2U) + sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG) + \
    ((nRxQueueDepth) * sizeof(ADI_EAL_MELIBU_REC_DATA)) + \
    (ADI_EAL_FIO_UART_UNICAST_INST_MEM_SIZE(nRxQueueDepth)))

/*! Helper macro to compute the memory size required for an MELIBU FIO multicast group */
#define ADI_EAL_MELIBU_MCAST_GROUP_MEM_SIZE(nNumInstFio) \
( \
    (sizeof(EAL_MELIBU_DATA) + (nNumInstFio)*sizeof(EAL_MELIBU_INST_DATA*)) + \
    (ADI_EAL_FIO_UART_MCAST_GROUP_MEM_SIZE((nNumInstFio)))\
)

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Enumerator to identify F field in header of Melibu 2.0 frame
 */
typedef enum ADI_EAL_MELIBU_FUNCTION_SELECT
{
    ADI_EAL_MELIBU_FUNC_SEL_NO_LED   = 0U,      /*!< No LED frame flag */
    ADI_EAL_MELIBU_FUNC_SEL_LED      = 1U       /*!< LED frame flag */
} ADI_EAL_MELIBU_FUNCTION_SELECT;

/**
 * @brief Enumerator to identify R/T field in header of Melibu 2.0 frame
 */
typedef enum ADI_EAL_MELIBU_R_T
{
    ADI_EAL_MELIBU_SLV_REC    = 0U,             /*!< Slave receive frame flag */
    ADI_EAL_MELIBU_SLV_TRANS  = 1U              /*!< Slave transmit frame flag */
} ADI_EAL_MELIBU_R_T;

/**
 * @brief Enumerator to identify instruction word selection field in header of Melibu 2.0 frame
 */
typedef enum ADI_EAL_MELIBU_INST_WORD_SEL
{
    ADI_EAL_MELIBU_NO_INST_WORD   = 0U,         /*!< No instruction word in frame flag */
    ADI_EAL_MELIBU_INST_WORD      = 1U          /*!< Instruction word in frame flag */
} ADI_EAL_MELIBU_INST_WORD_SEL;

/**
 * @brief EAL MELIBU callback event type
 */
typedef enum ADI_EAL_MELIBU_CBK_EVT
{
    ADI_EAL_MELIBU_CBK_EVT_REG_READ_RESP,                   /*!< Reg Read complete event */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP,                /*!< Read complete event */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_WRITE_RESP,               /*!< Write complete event */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_FIFO_READ,                /*!< FIFO read event */
    ADI_EAL_MELIBU_CBK_EVT_PERIODIC_RX,                     /*!< Periodic Rx received event */
    ADI_EAL_MELIBU_CBK_EVT_INTF_STATUS_EVT,                 /*!< Melibu interface status event */
    ADI_EAL_MELIBU_CBK_EVT_FIFO_STATUS_EVT,                 /*!< FIFO status event */
    ADI_EAL_MELIBU_CBK_EVT_ERR,                             /*!< Error event */
    ADI_EAL_MELIBU_CBK_EVT_RX_DATA_CORRUPT,                 /*!< Rx data is corrupt event */
    ADI_EAL_MELIBU_CBK_EVT_MAILBOX_ACK,                     /*!< Mailbox Acknowledgement event */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_UNEXPECTED_RX,            /*!< Unexpected frame received */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR,            /*!< Frame received with wrong length */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP_CAN_ECHO,       /*!< Read complete event including CAN echo reception (breakfield detected) */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_WRITE_CAN_ECHO,           /*!< Write frame CAN echo reception event (breakfield detected) */
    ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR_CAN_ECHO    /*!< CAN echo Frame received with wrong length (breakfield detected) */
} ADI_EAL_MELIBU_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_MELIBU_HANDLE;            /*!< The EAL FIO MELIBU handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief The EAL MELIBU callback prototype */
typedef void (*ADI_EAL_MELIBU_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_CBK_EVT eCbkEventType, const void *pArg);

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief Melibu data reception structure to manage receptions */
typedef struct ADI_EAL_MELIBU_REC_DATA
{
    uint8_t nExpectedRxLength;                                  /*!< Expected length of data to receive */
    uint8_t anMelibuFrameID[ADI_EAL_MELIBU_FRAME_ID_LENGTH];    /*!< Melibu Frame ID to use to calculate CRC */
    bool bMelibuResponseExpected;                               /*!< Flag to indicate if response is expected */
} ADI_EAL_MELIBU_REC_DATA;

/** @brief Melibu data reception structure sent to APP callback in the event */
typedef struct ADI_EAL_MELIBU_RESP_DATA
{
    uint16_t nRxLength;        /*!< Expected length of data to receive */
    uint16_t *aPortBufferRx;  /*!< Melibu reception data buffer */
    uint8_t nId;              /*!< E2B ID */
} ADI_EAL_MELIBU_RESP_DATA;


/*! @brief Data structure for each EAL Melibu instance */
typedef struct EAL_MELIBU_INST_DATA
{
    /* ~~~~~~~~~~~ Melibu instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID oId;                         /*!< Stores the instance number, remote number and interface number */
    uint32_t nMelibuIntfNum;                    /*!< Interface id */

    /* ~~~~~~~~~~~ Melibu state info ~~~~~~~~~~~ */
    ADI_EAL_MELIBU_CALLBACK_FUNC pfunCbk;       /*!< Pointer to store the address of callback function */
    bool bWaitingForRx;                         /*!< Flag to check if application is waiting for Rx data */
    ADI_EAL_USAGE_MODE eMode;                   /*!< Current usage mode: NORMAL, PERIODIC, or CUSTOM */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    volatile uint16_t nMelibuQWrIdx;            /*!< Indicates the write index of transaction config queue */
    volatile uint16_t nMelibuQRdIdx;            /*!< Indicates the read index of transaction config queue */
    uint16_t nMelibuCfgRxQDepth;                /*!< Indicates the size of Rx config queue */
    uint16_t *anTxBuffer;                       /*!< Memory buffer for transmitting payloads at internal interface level (uint16_t for UART framing) */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG
    *anTxTransactionBuffer;  /*!< Memory buffer for transmitting transactions at internal interface level */
    ADI_EAL_MELIBU_REC_DATA *aoRxMelibuBuffer;  /*!< Memory buffer for receiving transactions at Melibu level */

    /* ~~~~~~~~~~~ Mutex for the MELIBU instance ~~~~~~~~~~~ */
    uint8_t anMelibuMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];   /*!< Buffer for Melibu mutex */
    PRJ_OSAL_MUTEX_CFG oMelibuMutexCfg;         /*!< Melibu mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hMelibuMutex;         /*!< Handle for the Melibu mutex */
    uint8_t nBreakFieldLength;                  /*!< Number of bit times of breakfield to configure */
} EAL_MELIBU_INST_DATA;

/*! Internal data structure behind the EAL MELIBU handle */
typedef struct EAL_MELIBU_DATA
{
    bool                bIsMulticast;           /*!< Is this a multicast group or a unicast instance? */
    bool                bInUse;                 /*!< Is this handle opened? */
    uint16_t            nNumInst;               /*!< Number of unicast instances in this group */
    void               *pInternalPortHandler;   /*!< Array of handles of each internal interface used in the Melibu port */
    EAL_MELIBU_INST_DATA **apoInstData;         /*!< Array of unicast instance data pointers */
    bool                bPortConfigured;        /*!< Flag to indicate if port is configured */
} EAL_MELIBU_DATA;

/*! Internal data structure behind the EAL FIO MELIBU handle */
typedef struct ADI_EAL_MELIBU_CFG
{
    uint32_t nInstNum;                          /*!< Indicates the instance number */
    uint32_t nRemoteNum;                        /*!< Indicates the remote number */
    uint32_t nIntfNum;                          /*!< Indicates the interface number */
    uint32_t nMelibuIntfNum;                    /*!< Melibu Interface Number */
    uint32_t nEalMelibuMemorySize;              /*!< Indicates the allocated memory size*/
    uint8_t *pEalMelibuMemory;                  /*!< Pointer to allocated memory*/
    uint16_t nMelibuCfgRxQDepth;                /*!< Indicates the size of Rx config queue */
    uint8_t nBreakFieldLength;                  /*!< Number of bit times of breakfield to configure */
} ADI_EAL_MELIBU_CFG;

/**
 * @brief Structure to configure a Melibu 2.0 frame
 */
typedef struct ADI_EAL_MELIBU_FRAME_CFG
{
    uint8_t nSlaveAdd;                          /*!< Slave address to send in header*/
    ADI_EAL_MELIBU_R_T nR_T;                    /*!< Reception/transmission flag */
    ADI_EAL_MELIBU_FUNCTION_SELECT eFuncSel;    /*!< Function Selection */
    uint8_t nPayloadLength;                     /*!< Payload length */
    ADI_EAL_MELIBU_INST_WORD_SEL nInstWordSel;  /*!< Instruction word selection */
    uint16_t nInstWord;                         /*!< Instruction word */
    uint16_t nPci;                              /*!< Protocol control information byte */
    uint8_t nResponseLength;                    /*!< Response length */
    uint8_t *anPayload;                         /*!< Buffer with payload */
    uint16_t **anRxPayload;                     /*!< Pointer to array of buffers for reception */
    uint32_t nDelay;                            /*!< Delay to apply to this frame */
    uint64_t nPressTs;                          /*!< Presentation time stamp to apply to this frame */
    bool bEchoCancelled;                        /*!< Flag indicating if CAN echo must be cancelled */
    uint8_t nE2bId;                             /*!< E2B id */
} ADI_EAL_MELIBU_FRAME_CFG;

/**
 * @brief Structure to configure a Melibu 2.0 data
 */
typedef struct ADI_EAL_MELIBU_IO_DATA
{
    uint8_t nSlaveAdd;                          /*!< Slave address to send in header*/
    ADI_EAL_MELIBU_R_T nR_T;                    /*!< Reception/transmission flag */
    ADI_EAL_MELIBU_FUNCTION_SELECT eFuncSel;    /*!< Function Selection */
    uint8_t nPayloadLength;                     /*!< Payload length */
    ADI_EAL_MELIBU_INST_WORD_SEL nInstWordSel;  /*!< Instruction word selection */
    uint16_t nInstWord;                         /*!< Instruction word */
    uint16_t nPci;                              /*!< Protocol control information byte */
    uint8_t nResponseLength;                    /*!< Response length */
    uint8_t *anPayload;                         /*!< Buffer with payload */
    uint16_t **anRxPayload;                     /*!< Pointer to array of buffers for reception */

} ADI_EAL_MELIBU_IO_DATA;

/*! @brief Configuration for EAL FIO UART multicast group */
typedef struct ADI_EAL_MELIBU_MCAST_CFG
{
    uint16_t              nNumInst;             /*!< Number of unicast instances in the multicast group */
    ADI_EAL_MELIBU_HANDLE *pahMelibuHandles;    /*!< Pointer to the array of unicast instance handles */
    uint32_t              nIntfTopic;           /*!< Topic for accessing interface registers of this mcast group */
    uint32_t              nFifoTopic;           /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG          *poIfRegVlanTag;      /*!< Pointer to VLAN tag for interface register access of mcast group */
    ADI_VLAN_CFG          *poFifoRegVlanTag;    /*!< Pointer to VLAN tag for FIFO register access of mcast group */
    ADI_VLAN_CFG          *poFifoWriteVlanTag;  /*!< Pointer to VLAN tag for FIFO write transactions of mcast group */
    uint32_t              nMelibuMcastMemSize;  /*!< Size of the MELIBU multicast group data */
    void                  *pMelibuMcastMem;     /*!< Pointer to the MELIBU multicast group memory */
} ADI_EAL_MELIBU_MCAST_CFG;

/*! @brief  Melibu transaction config structure: Header + ADI_E2BCORE_FLEXIBLE_IO_TXN_PAYLOAD */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_MELIBU_TXN_CONFIG;

/*! @brief Typedef for MELIBU UART parameters, mapping to ADI_EAL_FIO_UART_PARAMS */
typedef ADI_EAL_FIO_UART_PARAMS   ADI_EAL_MELIBU_UART_PARAMS;

/*! @brief Typedef for MELIBU UART instance RAM data, mapping to ADI_EAL_FIO_UART_INST_RAM_DATA */
typedef ADI_EAL_FIO_INST_RAM_DATA ADI_EAL_MELIBU_UART_INST_RAM_DATA;
/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_MELIBU_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_WRITE_RESP event */
typedef ADI_EAL_MELIBU_TXN_CONFIG ADI_EAL_MELIBU_CBK_EVT_MELIBU_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_FIFO_READ event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_MELIBU_FIFO_READ_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_PERIODIC_RX event */
typedef ADI_E2BCORE_TXN_CONFIG ADI_EAL_MELIBU_CBK_EVT_PERIODIC_RX_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_FIO_STATUS_DATA ADI_EAL_MELIBU_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_MELIBU_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_MELIBU_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_RX_DATA_CORRUPT event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_RX_DATA_CORRUPT_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_MELIBU_CBK_EVT_MAILBOX_ACK_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_UNEXPECTED_RX event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_MELIBU_UNEXPECTED_RX_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP_CAN_ECHO event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_MELIBU_READ_RESP_CAN_ECHO_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_WRITE_CAN_ECHO event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_MELIBU_WRITE_CAN_ECHO_DATA;

/*! @brief Data for #ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR_CAN_ECHO event */
typedef ADI_EAL_MELIBU_RESP_DATA ADI_EAL_MELIBU_CBK_EVT_MELIBU_LENGTH_RX_ERR_CAN_ECHO_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/

extern ADI_EAL_STATUS adi_eal_melibuOpen(ADI_EAL_MELIBU_CFG *poMelibuCfg, ADI_EAL_MELIBU_HANDLE *phMelibuHandle);
extern ADI_EAL_STATUS adi_eal_melibuClose(ADI_EAL_MELIBU_HANDLE hMelibuHandle);
extern ADI_EAL_STATUS adi_eal_melibuRegisterCallback(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_CALLBACK_FUNC pfMelibuCallback);
extern ADI_EAL_STATUS adi_eal_melibuConfigure(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        const ADI_EAL_MELIBU_UART_PARAMS *poUartParams,
        const ADI_EAL_MELIBU_UART_INST_RAM_DATA *poUartInstRamData);
extern ADI_EAL_STATUS adi_eal_melibuCreateMcastGroup(ADI_EAL_MELIBU_MCAST_CFG *poMelibuMcastCfg,
        ADI_EAL_MELIBU_HANDLE *phMelibuMcastHandle);
extern ADI_EAL_STATUS adi_eal_melibuWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, bool bEchoCancelled, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_melibuWriteReadSync(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, bool bEchoCancelled, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_melibuWriteReadWithDelay(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, uint32_t nDelay, bool bEchoCancelled, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_melibuWriteReadWithPresTs(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, uint64_t nTimestamp, bool bEchoCancelled, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_melibuSetupPeriodicWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_IO_DATA *poMelibuData, bool bEchoCancelled, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_melibuFlexWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_FRAME_CFG *paoMelibuTxnCfg, uint16_t nNumFrames);
extern ADI_EAL_STATUS adi_eal_melibuCustomWriteRead(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        ADI_EAL_MELIBU_TXN_CONFIG *paoMelibuTxnCfg, uint16_t nNumMelibuTxns, bool bCanEchoCancel);
extern ADI_EAL_STATUS adi_eal_melibuSendRegTransaction(ADI_EAL_MELIBU_HANDLE hMelibuHandle,
        bool bIsFifoRegTransaction, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_melibuSetDownstreamVlan(ADI_EAL_MELIBU_HANDLE hMelibuHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_melibuSetUpstreamVlan(ADI_EAL_MELIBU_HANDLE hMelibuHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_melibuResetQueue(ADI_EAL_MELIBU_HANDLE hMelibuHandle);
extern ADI_EAL_STATUS adi_eal_melibuSetMailboxMode(ADI_EAL_MELIBU_HANDLE hMelibuHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_melibuResyncSeqNum(ADI_EAL_MELIBU_HANDLE hMelibuHandle);


#endif /* ADI_E2B_IFACE_MELIBU_ENABLED */
#endif /* ADI_EAL_MELIBU_H */

/** @} */

/**
 * EOF: www.analog.com
 */
