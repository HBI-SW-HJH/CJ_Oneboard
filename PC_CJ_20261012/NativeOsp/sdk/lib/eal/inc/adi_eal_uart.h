/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_uart.h
 * \brief:The header file for UART functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_UART_H
#define ADI_EAL_UART_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "e2bcore_uart.h"   /* e2bcore UART header */

#ifdef ADI_E2B_IFACE_UART_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each UART unicast instance */
#define ADI_EAL_UART_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_UART_INST_DATA) + sizeof(EAL_UART_INST_DATA*) + sizeof(EAL_UART_DATA) + (nQueueDepth) * sizeof(ADI_E2BCORE_UART_TXN_CONFIG) + ((size_t)(nQueueDepth) + 1U) * sizeof(ADI_E2BCORE_UART_TXN_CONFIG*))

/*! Helper macro to compute the memory size required for an UART multicast group */
#define ADI_EAL_UART_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_UART_DATA) + (nNumInst)*sizeof(EAL_UART_INST_DATA*))

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL UART callback event type
 */
typedef enum ADI_EAL_UART_CBK_EVT
{
    ADI_EAL_UART_CBK_EVT_REG_READ_RESP,       /*!< Reg Read complete event */
    ADI_EAL_UART_CBK_EVT_UART_READ_RESP,      /*!< Read complete event */
    ADI_EAL_UART_CBK_EVT_UART_WRITE_RESP,     /*!< Write complete event */
    ADI_EAL_UART_CBK_EVT_UART_FIFO_READ,      /*!< FIFO read event when EAL is configured in ADI_EAL_USAGE_MODE_CUSTOM */
    ADI_EAL_UART_CBK_EVT_PERIODIC_RX,         /*!< Periodic Rx received event */
    ADI_EAL_UART_CBK_EVT_INTF_STATUS_EVT,     /*!< UART interface status event */
    ADI_EAL_UART_CBK_EVT_FIFO_STATUS_EVT,     /*!< FIFO status event */
    ADI_EAL_UART_CBK_EVT_ERR,                 /*!< Error event */
    ADI_EAL_UART_CBK_EVT_RX_DATA_CORRUPT,     /*!< Rx data is corrupt event */
    ADI_EAL_UART_CBK_EVT_RX_PARITY_ERROR,     /*!< Rx parity error event */
    ADI_EAL_UART_CBK_EVT_MAILBOX_ACK,         /*!< Mailbox ACK event */
    ADI_EAL_UART_CBK_EVT_READ_TIME_TAG_ECCO,  /*!< ECCO time tag event */
} ADI_EAL_UART_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_UART_HANDLE;                /*!< The UART EAL handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void (*ADI_EAL_UART_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_UART_HANDLE hUartHandle,
        ADI_EAL_UART_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL UART callback prototype */


/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief Data structure for UART RX data
 */
typedef struct ADI_EAL_UART_RX_DATA
{
    bool          bIsGptpAcquisitionTimePresent;  /*!< Indicates if gPTP acquisition time is present */
    bool          bResyncTillThisTxn;             /*!< Indicates if resync is done till this transaction */
    bool          bIsE2bIdPresent;                /*!< Indicates if E2B ID is present */
    const uint8_t *pRxData;                       /*!< Pointer to store Rx data */
    uint8_t       nE2bId;                         /*!< E2B id */
    uint16_t      nReadLength;                    /*!< The read length */
    uint64_t      nGptpAcquisitionTime;           /*!< gPTP Acquisition Time ({uint32_t}sec, {uint32}ns) */
} ADI_EAL_UART_RX_DATA;

/**
 * @brief Data structure for UART RX Error data
 */
typedef struct ADI_EAL_UART_RX_ERROR_DATA
{
    uint64_t      nGptpAcquisitionTime;           /*!< gPTP Acquisition Time ({uint32_t}sec, {uint32}ns) */
    const uint16_t *pRxData;                      /*!< Pointer to store Rx data - unmodified data */
    uint16_t      nReadLength;                    /*!< The read length */
    bool          bIsGptpAcquisitionTimePresent;  /*!< Indicates if gPTP acquisition time is present */
    bool          bResyncTillThisTxn;             /*!< Indicates if resync is done till this transaction */
    bool          bIsE2bIdPresent;                /*!< Indicates if E2B ID is present */
    uint8_t       nE2bId;                         /*!< E2B id */
    uint8_t       nNumReadBits;                   /*!< Number of read bits received for the transaction */
} ADI_EAL_UART_RX_ERROR_DATA;

/** @brief UART interface status data */
typedef struct ADI_EAL_UART_STATUS_DATA
{
    uint16_t nNumTxBytes;           /*!< Number of bytes written by the UART interface */
    uint16_t nNumRxBytes;           /*!< Number of bytes read by the UART interface */
} ADI_EAL_UART_STATUS_DATA;

/*! @brief Data structure for each EAL UART instance */
typedef struct EAL_UART_INST_DATA
{
    /* ~~~~~~~~~~~ UART instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;                /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;              /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;            /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;          /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ UART state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;         /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;              /*!< Indicates in which mode eal is used */
    ADI_EAL_UART_CALLBACK_FUNC pfunCbk;     /*!< Pointer to store the address of callback function */
    volatile bool       bWaitingForRx;      /*!< Flag indicating the API is waiting for a response */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsParityEnabled;   /*!< Indicates if parity is enabled */
    ADI_EAL_PARITY_TYPE eParityType;        /*!< Indicates the type of parity to be used */
    bool                bIsUartConfigured;  /*!< Indicates if UART is configured */
    uint8_t             nNumOfStopBits;     /*!< Number of stop bits */
    uint8_t             nNumOfDataBits;     /*!< Number of data bits */
    bool                bIsReadTimeTagEn;   /*!< Indicates if Read time tag enabled */
    uint16_t            nUartCfgQDepth;     /*!< Indicates the size of config queue*/
    volatile uint16_t   nUartQWrIdx;        /*!< Indicates the Write index of transaction config queue */
    volatile uint16_t   nUartQRdIdx;        /*!< Indicates the Read index of transaction config queue */
    ADI_E2BCORE_UART_TXN_CONFIG *paoCfgs;   /*!< UART transaction config queue */
    ADI_E2BCORE_UART_TXN_CONFIG oRxCfg;     /*!< UART RX config */
    ADI_E2BCORE_UART_TXN_PAYLOAD oRxPayload; /*!< UART RX payload */

    /* ~~~~~~~~~~~ Mutex for the UART instance ~~~~~~~~~~~ */
    uint8_t anUartMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];   /*!< Buffer for Uart mutex */
    PRJ_OSAL_MUTEX_CFG oUartMutexCfg;       /*!< Uart mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hUartMutex;       /*!< Handle for the Uart mutex */

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    uint8_t anUartFifoRespSemaphoreBuffer[PRJ_OSAL_SEMAPHORE_MEM_SIZE];   /*!< Buffer for UART semaphore */
    PRJ_OSAL_SEMAPHORE_CFG oFifoRespSemaphoreCfg; /*!< Semaphore configuration for FIFO response */
    PRJ_OSAL_SEMAPHORE_HANDLE hFifoRespSemaphore; /*!< Semaphore handle for FIFO response */
#endif
} EAL_UART_INST_DATA;

/*! Internal data structure behind the EAL UART handle */
typedef struct EAL_UART_DATA
{
    bool                bIsMulticast;   /*!< Is this a multicast group or a unicast instance? */
    bool                bInUse;         /*!< Is this handle opened? */
    bool                bMailboxMode;   /*!< Flag to indicate if mailbox mode is enabled */
    bool                bResyncPending; /*!< Flag to indicate if sync mode is enabled for resync */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    uint32_t            nIfRegVlanTag;      /*!< VLAN tag for interface register access of this mcast group */
    uint32_t            nFifoRegVlanTag;    /*!< VLAN tag for FIFO register access of this mcast group */
    uint32_t            nFifoWriteVlanTag;  /*!< VLAN tag for FIFO write transactions of this mcast group */
    uint16_t            nNumInst;       /*!< Number of unicast instances in this group */
    EAL_UART_INST_DATA  **apoInstData;  /*!< Array of unicast instance data pointers */
} EAL_UART_DATA;

/**
 * @brief EAL UART configuration data
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL UART APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper memory access operations. Use ADI_MEM_ALIGN4 or ADI_MEM_ALIGN8
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_UART_CFG
{
    uint32_t nInstNum;              /*!< Indicates the instance number */
    uint32_t nRemoteNum;            /*!< Indicates the remote number */
    uint32_t nIntfNum;              /*!< Indicates the interface number */
    uint32_t nEalUartMemorySize;    /*!< Indicates the allocated memory size*/
    uint16_t nUartCfgQDepth;        /*!< Indicates the size of config queue*/
    uint8_t *pEalUartMemory;        /*!< Pointer to allocated memory (must be 4-byte or 8-byte aligned) */
} ADI_EAL_UART_CFG;

/*! @brief Configuration for EAL UART multicast group */
typedef struct ADI_EAL_UART_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_UART_HANDLE *pahUartHandles;  /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;      /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;    /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;  /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nUartMcastMemSize; /*!< Size of the UART multicast group data */
    void               *pUartMcastMem;   /*!< Pointer to the UART multicast group memory */
} ADI_EAL_UART_MCAST_CFG;

/*! @brief EAL UART I/O DATA with break field */
typedef struct ADI_EAL_UART_TXN_DESC
{
    uint64_t nGptpPresTime;  /*!< gPTP Presentation Time ({uint32_t}sec, {uint32}ns) */
    uint32_t nDelay;         /*!< Delay in nanoseconds */
    uint8_t *pTxData;        /*!< Pointer to TX data buffer */
    uint16_t nLength;        /*!< Length of payload data bytes only, doesn't include break field bits */
    bool bIsGptpPresTime;    /*!< Indicates if gPTP presentation time is to be sent, to execute the transaction at the timestamp added */
    bool bIsDelayPresent;    /*!< Indicates if delay is to be sent, to execute the transaction after the delay */
    bool bIsE2bIdPresent;    /*!< Indicates if E2B ID is present */
    uint8_t nE2bId;          /*!< E2B ID */
    uint8_t nBreakFieldBitsBeforePayload; /*!< Number of break field bits to be sent before payload bytes; note - all the break field bits are 0 */
    uint8_t nBreakFieldBitsAfterPayload; /*!< Number of break field bits to be sent after payload bytes; note - all the break field bits are 0 */
} ADI_EAL_UART_TXN_DESC;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_UART_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_UART_READ_RESP event */
typedef ADI_EAL_UART_RX_DATA ADI_EAL_UART_CBK_EVT_UART_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_UART_WRITE_RESP event */
typedef ADI_EAL_UART_RX_DATA ADI_EAL_UART_CBK_EVT_UART_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_UART_FIFO_READ event */
typedef ADI_E2BCORE_CBK_FIFOREADDAT ADI_EAL_UART_CBK_EVT_UART_FIFO_READ_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_PERIODIC_RX event */
typedef ADI_EAL_UART_RX_DATA ADI_EAL_UART_CBK_EVT_PERIODIC_RX_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_UART_STATUS_DATA ADI_EAL_UART_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_UART_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_UART_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_RX_DATA_CORRUPT event */
typedef ADI_EAL_UART_RX_ERROR_DATA ADI_EAL_UART_CBK_EVT_RX_DATA_CORRUPT_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_RX_PARITY_ERROR event */
typedef ADI_EAL_UART_RX_ERROR_DATA ADI_EAL_UART_CBK_EVT_RX_PARITY_ERROR_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_UART_CBK_EVT_MAILBOX_ACK_DATA;

/*! @brief Data for #ADI_EAL_UART_CBK_EVT_READ_TIME_TAG_ECCO event */
typedef ADI_EAL_UART_RX_DATA ADI_EAL_UART_CBK_EVT_READ_TIME_TAG_ECCO_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_uartOpen(ADI_EAL_UART_CFG *poUartCfg, ADI_EAL_UART_HANDLE *phUartHandle);
extern ADI_EAL_STATUS adi_eal_uartClose(ADI_EAL_UART_HANDLE hUartHandle);
extern ADI_EAL_STATUS adi_eal_uartRegisterCallback(ADI_EAL_UART_HANDLE hUartHandle,
        ADI_EAL_UART_CALLBACK_FUNC pfUartCallback);
extern ADI_EAL_STATUS adi_eal_uartConfigure(ADI_EAL_UART_HANDLE hUartHandle, uint8_t nNumOfStopBits,
        uint8_t nNumOfDataBits, bool bIsParityEnabled, ADI_EAL_PARITY_TYPE eParityType);
extern ADI_EAL_STATUS adi_eal_uartCreateMcastGroup(ADI_EAL_UART_MCAST_CFG *poUartMcastCfg,
        ADI_EAL_UART_HANDLE *phUartMcastHandle);
extern ADI_EAL_STATUS adi_eal_uartWrite(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength,
                                        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_uartWriteSync(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_uartWriteWithDelay(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_uartDelayBits(ADI_EAL_UART_HANDLE hUartHandle, uint16_t nDelayBits,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_uartWriteWithPresTs(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData, uint16_t nLength,
        uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_uartSetupPeriodicWrite(ADI_EAL_UART_HANDLE hUartHandle, uint8_t *pTxData,
        uint16_t nLength, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_uartReadStatus(ADI_EAL_UART_HANDLE hUartHandle);
extern ADI_EAL_STATUS adi_eal_uartCustomWriteRead(ADI_EAL_UART_HANDLE hUartHandle,
        ADI_E2BCORE_UART_TXN_CONFIG *paoUartTxnCfg, uint16_t nNumUartTxns);
extern ADI_EAL_STATUS adi_eal_uartMultiTxnWrite(ADI_EAL_UART_HANDLE hUartHandle,
        ADI_EAL_UART_TXN_DESC *paoUartIoData, uint16_t nNumUartTxns);
extern ADI_EAL_STATUS adi_eal_uartSendRegTransaction(ADI_EAL_UART_HANDLE hUartHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_uartResetQueue(ADI_EAL_UART_HANDLE hUartHandle);
extern void adi_eal_uartSetDownstreamVlan(ADI_EAL_UART_HANDLE hUartHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_uartSetUpstreamVlan(ADI_EAL_UART_HANDLE hUartHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_uartSetMailboxMode(ADI_EAL_UART_HANDLE hUartHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_uartResyncSeqNum(ADI_EAL_UART_HANDLE hUartHandle);
extern ADI_EAL_STATUS adi_eal_uartSetReadTimeTagEcco(ADI_EAL_UART_HANDLE hUartHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_uartSetInterfaceEnable(ADI_EAL_UART_HANDLE hUartHandle, bool bEnableRx,
        bool bEnableTx);
/*============================ CALLBACKS ============================*/

extern void UartInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                             void *pCbParam);


#endif /* ADI_E2B_IFACE_UART_ENABLED */
#endif /* ADI_EAL_UART_H */

/** @} */

/**
 * EOF: www.analog.com
 */
