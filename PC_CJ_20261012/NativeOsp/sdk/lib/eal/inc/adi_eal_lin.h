/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_lin.h
 * \brief:The header file for LIN functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_LIN_H
#define ADI_EAL_LIN_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "e2bcore_lin.h"    /* e2bcore LIN header */

#ifdef ADI_E2B_IFACE_LIN_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each LIN unicast instance */
#define ADI_EAL_LIN_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_LIN_INST_DATA) + sizeof(EAL_LIN_INST_DATA*) + sizeof(EAL_LIN_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_LIN_TXN_CONFIG) + sizeof(ADI_E2BCORE_LIN_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an LIN multicast group */
#define ADI_EAL_LIN_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_LIN_DATA) + (nNumInst)*sizeof(EAL_LIN_INST_DATA*))

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL LIN callback event type
 */
typedef enum ADI_EAL_LIN_CBK_EVT
{
    ADI_EAL_LIN_CBK_EVT_REG_READ_RESP,       /*!< Reg Read complete event */
    ADI_EAL_LIN_CBK_EVT_LIN_READ_RESP,       /*!< Read complete event */
    ADI_EAL_LIN_CBK_EVT_LIN_WRITE_RESP,      /*!< Write complete event */
    ADI_EAL_LIN_CBK_EVT_LIN_FIFO_READ,       /*!< FIFO read event when EAL is configured in ADI_EAL_USAGE_MODE_CUSTOM */
    ADI_EAL_LIN_CBK_EVT_PERIODIC_RX,         /*!< Periodic Rx received event */
    ADI_EAL_LIN_CBK_EVT_INTF_STATUS_EVT,     /*!< LIN interface status event */
    ADI_EAL_LIN_CBK_EVT_FIFO_STATUS_EVT,     /*!< FIFO status event */
    ADI_EAL_LIN_CBK_EVT_ERR,                 /*!< Error event */
    ADI_EAL_LIN_CBK_EVT_MAILBOX_ACK,        /*!< Mailbox ACK event */
} ADI_EAL_LIN_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_LIN_HANDLE;                /*!< The LIN EAL handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void (*ADI_EAL_LIN_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_LIN_HANDLE hLinHandle,
        ADI_EAL_LIN_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL LIN callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */
/*! @brief Structure for LIN status data */
typedef ADI_E2BCORE_LIN_STATUS_DATA ADI_EAL_LIN_STATUS_DATA;

/*! @brief Structure for LIN payload */
typedef ADI_E2BCORE_LIN_TXN_PAYLOAD ADI_EAL_LIN_IO_DATA;

/*! @brief Data structure for each EAL LIN instance */
typedef struct EAL_LIN_INST_DATA
{
    /* ~~~~~~~~~~~ LIN instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;            /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;          /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;        /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;      /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ LIN state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;     /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;          /*!< Indicates in which mode eal is used */
    ADI_EAL_LIN_CALLBACK_FUNC pfunCbk;  /*!< Pointer to store the address of callback function */
    volatile bool       bWaitingForRx;  /*!< Flag indicating the API is waiting for a response */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn; /*!< Indicates if Read time tag enabled */
    uint16_t            nLinCfgQDepth;  /*!< Indicates the size of config queue*/
    volatile uint16_t   nLinQWrIdx;     /*!< Indicates the Write index of transaction config queue */
    volatile uint16_t   nLinQRdIdx;     /*!< Indicates the Read index of transaction config queue */
    ADI_E2BCORE_LIN_TXN_CONFIG *paoCfgs;  /*!< LIN transaction config queue */

    /* ~~~~~~~~~~~ Mutex for the LIN instance ~~~~~~~~~~~ */
    uint8_t anLinMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];   /*!< Buffer for Lin mutex */
    PRJ_OSAL_MUTEX_CFG oLinMutexCfg;            /*!< Lin mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hLinMutex;            /*!< Handle for the Lin mutex */

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    uint8_t anLinFifoRespSemaphoreBuffer[PRJ_OSAL_SEMAPHORE_MEM_SIZE];   /*!< Buffer for LIN semaphore */
    PRJ_OSAL_SEMAPHORE_CFG oFifoRespSemaphoreCfg; /*!< Semaphore configuration for FIFO response */
    PRJ_OSAL_SEMAPHORE_HANDLE hFifoRespSemaphore; /*!< Semaphore handle for FIFO response */
#endif
} EAL_LIN_INST_DATA;

/*! Internal data structure behind the EAL LIN handle */
typedef struct EAL_LIN_DATA
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
    EAL_LIN_INST_DATA   **apoInstData;      /*!< Array of unicast instance data pointers */
} EAL_LIN_DATA;

/**
 * @brief EAL LIN configuration data
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL LIN APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_LIN_CFG
{
    uint32_t nInstNum;              /*!< Indicates the instance number */
    uint32_t nRemoteNum;            /*!< Indicates the remote number */
    uint32_t nIntfNum;              /*!< Indicates the interface number */
    uint32_t nEalLinMemorySize;     /*!< Indicates the allocated memory size*/
    uint16_t nLinCfgQDepth;         /*!< Indicates the size of config queue*/
    uint8_t *pEalLinMemory;         /*!< Pointer to allocated memory (must be 4-byte or 8-byte aligned) */
} ADI_EAL_LIN_CFG;

/*! @brief Configuration for EAL LIN multicast group */
typedef struct ADI_EAL_LIN_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_LIN_HANDLE *pahLinHandles;  /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;      /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;    /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;  /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nLinMcastMemSize; /*!< Size of the LIN multicast group data */
    void               *pLinMcastMem;   /*!< Pointer to the LIN multicast group memory */
} ADI_EAL_LIN_MCAST_CFG;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_LIN_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_LIN_READ_RESP event */
typedef ADI_E2BCORE_LIN_TXN_CONFIG ADI_EAL_LIN_CBK_EVT_LIN_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_LIN_WRITE_RESP event */
typedef ADI_E2BCORE_LIN_TXN_CONFIG ADI_EAL_LIN_CBK_EVT_LIN_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_LIN_FIFO_READ event */
typedef ADI_E2BCORE_CBK_FIFOREADDAT ADI_EAL_LIN_CBK_EVT_LIN_FIFO_READ_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_PERIODIC_RX event */
typedef ADI_E2BCORE_LIN_TXN_CONFIG ADI_EAL_LIN_CBK_EVT_PERIODIC_RX_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_LIN_STATUS_DATA ADI_EAL_LIN_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_LIN_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_LIN_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_LIN_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_LIN_CBK_EVT_MAILBOX_ACK_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/

/**
 * @note LIN Upstream Response Behavior:
 *       The LIN interface generates an upstream message for each downstream message processed.
 *       Regardless of whether it is a write or read operation, the upstream response contains
 *       the PID, data, and checksum values sampled from the LIN physical bus (including the
 *       checksum). The encoding of the LEN field is the same as in the downstream path.
 *       If any error flag is asserted, the contents for PID, data, and checksum are likely invalid.
 *
 *       Therefore, for all LIN write/read APIs:
 *       - The RX buffer (apRxData) must never be NULL
 *       - Each RX buffer must be at least nLength bytes (same as TX data length)
 *       - For write operations: RX buffer receives the echoed data from the bus
 *       - For read operations: RX buffer receives the response data from the slave
 */

extern ADI_EAL_STATUS adi_eal_linOpen(ADI_EAL_LIN_CFG *poLinCfg, ADI_EAL_LIN_HANDLE *phLinHandle);
extern ADI_EAL_STATUS adi_eal_linClose(ADI_EAL_LIN_HANDLE hLinHandle);
extern ADI_EAL_STATUS adi_eal_linRegisterCallback(ADI_EAL_LIN_HANDLE hLinHandle,
        ADI_EAL_LIN_CALLBACK_FUNC pfLinCallback);
extern ADI_EAL_STATUS adi_eal_linCreateMcastGroup(ADI_EAL_LIN_MCAST_CFG *poLinMcastCfg,
        ADI_EAL_LIN_HANDLE *phLinMcastHandle);
extern ADI_EAL_STATUS adi_eal_linWriteRead(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_linWriteReadSync(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_linWriteReadWithDelay(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_linWriteReadWithPresTs(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_linSetupPeriodicWrite(ADI_EAL_LIN_HANDLE hLinHandle, ADI_EAL_LIN_IO_DATA *poLinData,
        uint8_t **apRxData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_linReadStatus(ADI_EAL_LIN_HANDLE hLinHandle);
extern ADI_EAL_STATUS adi_eal_linCustomWriteRead(ADI_EAL_LIN_HANDLE hLinHandle,
        ADI_E2BCORE_LIN_TXN_CONFIG *paoLinTxnCfg, uint16_t nNumLinTxns);
extern ADI_EAL_STATUS adi_eal_linSendRegTransaction(ADI_EAL_LIN_HANDLE hLinHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_linResetQueue(ADI_EAL_LIN_HANDLE hLinHandle);
extern void adi_eal_linSetDownstreamVlan(ADI_EAL_LIN_HANDLE hLinHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_linSetUpstreamVlan(ADI_EAL_LIN_HANDLE hLinHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_linSetMailboxMode(ADI_EAL_LIN_HANDLE hLinHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_linResyncSeqNum(ADI_EAL_LIN_HANDLE hLinHandle);
/*============================ CALLBACKS ============================*/

extern void LinInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                            void *pCbParam);


#endif /* ADI_E2B_IFACE_LIN_ENABLED */
#endif /* ADI_EAL_LIN_H */

/** @} */

/**
 * EOF: www.analog.com
 */
