/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_fioUart.h
 * \brief:The header file for Flexible IO UART functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_FIOUART_H
#define ADI_EAL_FIOUART_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "e2bcore_flexibleIO.h"         /* Flexible IO e2bcore header */

#ifdef ADI_E2B_IFACE_FIO_UART_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each FIO_UART unicast instance */
#define ADI_EAL_FIO_UART_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_FIO_UART_INST_DATA) + sizeof(EAL_FIO_UART_INST_DATA*) + sizeof(EAL_FIO_UART_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG) + sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG*) + sizeof(ADI_EAL_FIO_UART_BREAKFIELD_INFO)))

/*! Helper macro to compute the memory size required for an FIO_UART multicast group */
#define ADI_EAL_FIO_UART_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_FIO_UART_DATA) + (nNumInst)*sizeof(EAL_FIO_UART_INST_DATA*))

#define ADI_EAL_FIO_UART_PACKING_BUFF_SIZE        (1188U)   /*!< Size for FIO UART packing buffer */

/*! Helper macro to compute the buffer size needed for formatted TX data
 *  @param nTxBytes - Number of TX data bytes to transmit
 *  @param nDataBits - Number of data bits per byte (7-12)
 *  @param nStopBits - Number of stop bits (1-2)
 *  @param nBreakFieldBits - Number of break field bits
 *  @param bParity - Whether parity is enabled (true/false)
 */
#define ADI_EAL_FIO_UART_FORMATTED_TX_SIZE(nTxBytes, nDataBits, nStopBits, nBreakFieldBits, bParity) \
    ( \
        /* Break field bytes */ \
        ((nBreakFieldBits) == 0U ? 0U : \
            ((nBreakFieldBits) <= (1 + (nDataBits) + (bParity)) ? 2U : \
                (2U * (1U + (((nBreakFieldBits) - (1 + (nDataBits) + (bParity)) + \
                              (1 + (nDataBits) + (nStopBits) + (bParity)) - 1U) / \
                             (1 + (nDataBits) + (nStopBits) + (bParity))))))) + \
        /* Encoded TX data bytes */ \
        ((nTxBytes) * 2U) + \
        /* Initial header */ \
        1U + \
        /* Separator headers (1 per 4 bytes of breakfield + encoded data) */ \
        (((((nBreakFieldBits) == 0U ? 0U : \
            ((nBreakFieldBits) <= (1 + (nDataBits) + (bParity)) ? 2U : \
                (2U * (1U + (((nBreakFieldBits) - (1 + (nDataBits) + (bParity)) + \
                              (1 + (nDataBits) + (nStopBits) + (bParity)) - 1U) / \
                             (1 + (nDataBits) + (nStopBits) + (bParity))))))) + \
           ((nTxBytes) * 2U)) + 3U) / 4U) + \
        /* Final RX header */ \
        2U + \
        /* Padding */ \
        4U + \
        /* Guard bytes */ \
        2U \
    )

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL FIO UART callback event type
 */
typedef enum ADI_EAL_FIO_UART_CBK_EVT
{
    ADI_EAL_FIO_UART_CBK_EVT_REG_READ_RESP,                     /*!< Reg Read complete event */
    ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP,                    /*!< Read complete event */
    ADI_EAL_FIO_UART_CBK_EVT_UART_WRITE_RESP,                   /*!< Write complete event */
    ADI_EAL_FIO_UART_CBK_EVT_UART_FIFO_READ,                    /*!< FIFO read event when EAL is configured in ADI_EAL_USAGE_MODE_CUSTOM */
    ADI_EAL_FIO_UART_CBK_EVT_PERIODIC_RX,                       /*!< Periodic Rx received event */
    ADI_EAL_FIO_UART_CBK_EVT_INTF_STATUS_EVT,                   /*!< FIO UART interface status event */
    ADI_EAL_FIO_UART_CBK_EVT_FIFO_STATUS_EVT,                   /*!< FIFO status event */
    ADI_EAL_FIO_UART_CBK_EVT_ERR,                               /*!< Error event */
    ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT,                   /*!< Rx data is corrupt event */
    ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK,                       /*!< Mailbox Acknowledgement event */
    ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_BREAKFIELD          /*!< Read complete with breakfield detected and removed from reception event */
} ADI_EAL_FIO_UART_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_FIO_UART_HANDLE;                /*!< The EAL FIO UART handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void (*ADI_EAL_FIO_UART_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_FIO_UART_HANDLE hUartHandle,
        ADI_EAL_FIO_UART_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL FIO UART callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */
/*! @brief Internal breakfield information - maintained as queue, per EAL instance of FIO UART for parsing breakfields per transaction */
typedef struct ADI_EAL_FIO_UART_BREAKFIELD_INFO
{
    uint16_t nOffsetForAfterPayloadBreakFields; /*!< Offset for the "after payload" breakfields - gives the position of echoed breakfield in RX stream */
    uint8_t nBreakFieldBitsBeforePayload;     /*!< Number of break field bits to be appended before the payload */
    uint8_t nBreakFieldBitsAfterPayload;      /*!< Number of break field bits to be appended after the payload */
    bool bIsEchoEnabled;                     /*!< Indicates if TX bytes are echoed for this transaction */
} ADI_EAL_FIO_UART_BREAKFIELD_INFO;

/*! @brief Data structure for each EAL FIO UART instance */
typedef struct EAL_FIO_UART_INST_DATA
{
    /* ~~~~~~~~~~~ FIO UART instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID oId;                       /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;                /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;              /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;            /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ FIO UART state info ~~~~~~~~~~~ */
    bool bIgnoreErr;                          /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE eMode;                 /*!< Indicates in which mode eal is used */
    ADI_EAL_FIO_UART_CALLBACK_FUNC pfunCbk;   /*!< Pointer to store the address of callback function */
    bool bWaitingForRx;                       /*!< Flag to check if application is waiting for Rx data */
    bool bIsParityEn;                         /*!< Flag to check if parity is enabled */
    ADI_EAL_PARITY_TYPE eParityType;          /*!< Indicates the parity type */
    uint32_t nBaudRate;                       /*!< UART baud rate specified in bps */
    uint8_t nBreakFieldBits;                  /*!< Number of bits for break field (Range (in bits): minimum = full frame size (start + data + stop + parity bits), maximum = 100) */
    uint8_t nNumOfStopBits;                   /*!< Number of stop bits (Range: 1-2) */
    uint8_t nNumOfDataBits;                   /*!< Number of data bits (Range: 7-12) */
    bool bIsCanEchoCancel;                    /*!< Flag to check if CAN echo cancellation is enabled */
    uint8_t anPackedTmp[ADI_EAL_FIO_UART_PACKING_BUFF_SIZE];               /*!< Workspace for packing raw UART bytes */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool bIsReadTimeTagEn;                       /*!< Indicates if read time tag enabled */
    volatile uint16_t nFioUartQWrIdx;            /*!< Indicates the write index of transaction config queue */
    volatile uint16_t nFioUartQRdIdx;            /*!< Indicates the read index of transaction config queue */
    uint16_t nFioUartCfgQDepth;                  /*!< Indicates the size of config queue */
    uint8_t  nFormattedTxData;                   /*!< Indicates the formatted TX data length */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoCfgs; /*!< FIO UART transaction config queue */
    ADI_EAL_FIO_UART_BREAKFIELD_INFO *paoBreakfieldInfo; /*!< FIO UART breakfield info queue */

    /* ~~~~~~~~~~~ Mutex for the FIO UART instance ~~~~~~~~~~~ */
    uint8_t anFioUartMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /*!< Buffer for FIO UART mutex */
    PRJ_OSAL_MUTEX_CFG oFioUartMutexCfg;          /*!< FIO UART mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hFioUartMutex;          /*!< Handle for the FIO UART mutex */

    /* ~~~~~~~~~~~ Melibu info ~~~~~~~~~~~ */
    void    *hMelibuHandle;              /*!< Stores the Melibu interface handler that uses this UART interface */

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    uint8_t anFioUartFifoRespSemaphoreBuffer[PRJ_OSAL_SEMAPHORE_MEM_SIZE];   /*!< Buffer for FIO UART semaphore */
    PRJ_OSAL_SEMAPHORE_CFG oFifoRespSemaphoreCfg; /*!< Semaphore configuration for FIFO response */
    PRJ_OSAL_SEMAPHORE_HANDLE hFifoRespSemaphore; /*!< Semaphore handle for FIFO response */
#endif
} EAL_FIO_UART_INST_DATA;

/*! Internal data structure behind the EAL FIO UART handle */
typedef struct EAL_FIO_UART_DATA
{
    bool                bIsMulticast;       /*!< Is this a multicast group or a unicast instance? */
    bool                bInUse;             /*!< Is this handle opened? */
    bool                bMailboxMode;       /*!< Flag to indicate if mailbox mode is enabled */
    bool                bResyncPending;     /*!< Flag to indicate if sync mode is enabled for resync */
    uint32_t            nIntfTopic;         /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;         /*!< Topic for accessing FIFO of this mcast group */
    uint32_t            nIfRegVlanTag;      /*!< VLAN tag for interface register access of this mcast group */
    uint32_t            nFifoRegVlanTag;    /*!< VLAN tag for FIFO register access of this mcast group */
    uint32_t            nFifoWriteVlanTag;  /*!< VLAN tag for FIFO write transactions of this mcast group */
    uint16_t            nNumInst;           /*!< Number of unicast instances in this group */
    EAL_FIO_UART_INST_DATA   **apoInstData; /*!< Array of unicast instance data pointers */
    ADI_E2BCORE_MSGOPTS oFifoMsgOpts;       /*!< FIFO message options (persistent storage to avoid local address escape) */
} EAL_FIO_UART_DATA;

/**
 * @brief EAL FIO UART configuration data
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL FIO UART APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper memory access operations. Use ADI_MEM_ALIGN4 or ADI_MEM_ALIGN8
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_FIO_UART_CFG
{
    uint32_t nInstNum;              /*!< Indicates the instance number */
    uint32_t nRemoteNum;            /*!< Indicates the remote number */
    uint32_t nIntfNum;              /*!< Indicates the interface number */
    uint32_t nFioUartMemorySize;    /*!< Indicates the allocated memory size */
    uint8_t *pFioUartMemory;        /*!< Pointer to allocated memory (must be 4-byte or 8-byte aligned) */
    uint16_t nFioUartCfgQDepth;     /*!< Indicates the size of config queue */
} ADI_EAL_FIO_UART_CFG;

/*! @brief Configuration for EAL FIO UART multicast group */
typedef struct ADI_EAL_FIO_UART_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_FIO_UART_HANDLE *pahFioUartHandles; /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;      /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;    /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;  /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nFioUartMcastMemSize;   /*!< Size of the FIO_UART multicast group data */
    void               *pFioUartMcastMem;       /*!< Pointer to the FIO_UART multicast group memory */
} ADI_EAL_FIO_UART_MCAST_CFG;

/*! @brief EAL FIO UART I/O DATA */
typedef struct ADI_EAL_FIO_UART_IO_DATA
{
    uint16_t *pTxData;           /*!< Pointer to store Tx data */
    uint16_t **apRxData;         /*!< Array of pointer to RX data */
    uint16_t nWriteLength;       /*!< The write length */
    uint16_t nReadLength;        /*!< The read length */
    uint16_t *pFormattedTxData;  /*!< Pointer to store formatted Tx data (Helper macro ADI_EAL_FIO_UART_FORMATTED_TX_SIZE can be used to calculate the size of allocated memory) */
} ADI_EAL_FIO_UART_IO_DATA;

/*! @brief FIO UART I/O struct with break field and echo cancellation as additional parameters */
typedef struct ADI_EAL_FIO_UART_TXN_DESC
{
    uint64_t nPresTime;          /*!< GPTP presentation time value, if present */
    uint16_t *pTxData;           /*!< Pointer to Tx data - shouldn't contain the breakfield bits */
    uint16_t **apRxData;         /*!< Array of pointer to RX data for all unicast instances within the handle */
    uint32_t nDelay;             /*!< Delay value in nanoseconds, if present */
    uint16_t  nWriteLength;      /*!< The write length - should account only for the data bytes pointed by pTxData, shouldn't include breakbits */
    uint16_t nReadLength;        /*!< The read length - should account for the expected number of data bytes only, shouldn't include echo, this is internally updated */
    bool bIsGptpPresTime;        /*!< Indicates if GPTP presentation time is enabled */
    bool bIsDelayPresent;        /*!< Indicates if delay is present */
    bool bIsE2bIdPresent;        /*!< Indicates if E2B ID is present */
    bool bCancelEcho;            /*!< Indicates if CAN echo cancellation is to be done for this transaction */
    uint8_t nE2bId;              /*!< E2B ID value, if present */
    uint8_t nBreakFieldBitsBeforePayload;     /*!< Number of break field bits to be appended before the payload - this overrides the break field bits set for the handle in all cases */
    uint8_t nBreakFieldBitsAfterPayload;     /*!< Number of break field bits to be appended after the payload - this overrides the break field bits set for the handle in all cases */
} ADI_EAL_FIO_UART_TXN_DESC;

/*! @brief EAL FIO UART runtime configuration Parameters */
typedef struct ADI_EAL_FIO_UART_PARAMS
{
    uint8_t nNumOfStopBits;             /*!< Number of stop bits */
    uint8_t nNumOfDataBits;             /*!< Number of data bits (data width) */
    bool bIsParityEnabled;              /*!< Indicates if parity is enabled */
    ADI_EAL_PARITY_TYPE eParityType;    /*!< The parity type to be used */
    uint32_t nBaudRate;                 /*!< Baud rate for UART communication */
    uint8_t nBreakFieldBits;            /*!< Number of break field bits (Range (in bits): minimum = full frame size (start + data + stop + parity bits), maximum = 100) */
    bool bIsCanEchoCancel;              /*!< Indicates if CAN echo cancellation is enabled */
    uint32_t *pInstrRAM;                /*!< Pointer to instruction RAM array */
    uint16_t nInstrRamSize;             /*!< Size of instruction RAM array */
    bool bIsOtpConfigured;              /*!< Indicates if configuration is OTP-programmed. When true, instruction RAM programming is skipped */
} ADI_EAL_FIO_UART_PARAMS;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_FIO_UART_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_UART_CBK_EVT_UART_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_UART_WRITE_RESP event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_UART_CBK_EVT_UART_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_UART_FIFO_READ event */
typedef ADI_E2BCORE_CBK_FIFOREADDAT ADI_EAL_FIO_UART_CBK_EVT_UART_FIFO_READ_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_PERIODIC_RX event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_UART_CBK_EVT_PERIODIC_RX_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_FIO_STATUS_DATA ADI_EAL_FIO_UART_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_FIO_UART_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_FIO_UART_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT event */
typedef void *ADI_EAL_FIO_UART_CBK_EVT_RX_DATA_CORRUPT_DATA;

/*! @brief Data for #ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_FIO_UART_CBK_EVT_MAILBOX_ACK_DATA;

/** Data to be used for updating instruction RAM */
typedef ADI_EAL_FIO_INST_RAM_DATA ADI_EAL_FIO_UART_INST_RAM_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_fioUartOpen(ADI_EAL_FIO_UART_CFG *poFioUartCfg, ADI_EAL_FIO_UART_HANDLE *phFioUartHandle);
extern ADI_EAL_STATUS adi_eal_fioUartClose(ADI_EAL_FIO_UART_HANDLE hFioUartHandle);
extern ADI_EAL_STATUS adi_eal_fioUartRegisterCallback(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_CALLBACK_FUNC pfFioUartCallback);
extern ADI_EAL_STATUS adi_eal_fioUartCreateMcastGroup(ADI_EAL_FIO_UART_MCAST_CFG *poFioUartMcastCfg,
        ADI_EAL_FIO_UART_HANDLE *phFioUartMcastHandle);
extern ADI_EAL_STATUS adi_eal_fioUartConfigure(
    ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
    const ADI_EAL_FIO_UART_PARAMS *poParams,
    const ADI_EAL_FIO_UART_INST_RAM_DATA *poFioInstructionRamData);
extern ADI_EAL_STATUS adi_eal_fioUartWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioUartWriteReadSync(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioUartWriteReadWithDelay(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioUartWriteReadWithPresTs(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData, uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioUartSetupPeriodicWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_IO_DATA *poFioUartData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioUartReadStatus(ADI_EAL_FIO_UART_HANDLE hFioUartHandle);
extern ADI_EAL_STATUS adi_eal_fioUartCustomWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t nNumFioUartTxns);
extern ADI_EAL_STATUS adi_eal_fioUartMultiTxnWriteRead(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_EAL_FIO_UART_TXN_DESC *paoFioUartIoData, uint16_t nNumFioUartTxns, uint16_t *pFormattedTxData);
extern ADI_EAL_STATUS adi_eal_fioUartSendRegTransaction(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        bool bIsFifoRegTransaction, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_fioUartResetQueue(ADI_EAL_FIO_UART_HANDLE hFioUartHandle);
extern void adi_eal_fioUartSetDownstreamVlan(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_fioUartSetUpstreamVlan(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_fioUartSetMailboxMode(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_fioUartResyncSeqNum(ADI_EAL_FIO_UART_HANDLE hFioUartHandle);
extern ADI_EAL_STATUS adi_eal_fioUartSetEchoCancel(ADI_EAL_FIO_UART_HANDLE hFioUartHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_fioUartPackedDataTransmit(ADI_EAL_FIO_UART_HANDLE hFioUartHandle,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioUartTxnCfg, uint16_t **anRxPayload,
        uint16_t nNumFioUartTxns, bool bIsTransmitPkt);
/*============================ CALLBACKS ============================*/

extern void FioUartInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                                void *pCbParam);


#endif /* ADI_E2B_IFACE_FIO_UART_ENABLED */
#endif /* ADI_EAL_FIOUART_H */

/** @} */

/**
 * EOF: www.analog.com
 */
