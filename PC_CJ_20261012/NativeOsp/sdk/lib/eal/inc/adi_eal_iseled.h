/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_iseled.h
 * @brief: The header file for the eal ISELED interface
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 * @{
 */
#ifndef ADI_EAL_ISELED_H
#define ADI_EAL_ISELED_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"          /* EAL header */
#include "e2bcore_iseled.h"   /* e2bcore ISELED header */

#ifdef ADI_E2B_IFACE_ISELED_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each ISELED unicast instance */
#define ADI_EAL_ISELED_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_ISELED_INST_DATA) + sizeof(EAL_ISELED_INST_DATA*) + sizeof(EAL_ISELED_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_ISELED_TXN_CONFIG) + sizeof(ADI_E2BCORE_ISELED_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an ISELED multicast group */
#define ADI_EAL_ISELED_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_ISELED_DATA) + (nNumInst)*sizeof(EAL_ISELED_INST_DATA*))

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL ISELED callback event type
 */
typedef enum ADI_EAL_ISELED_CBK_EVT
{
    ADI_EAL_ISELED_CBK_EVT_REG_READ_RESP,         /*!< Reg Read complete event */
    ADI_EAL_ISELED_CBK_EVT_ISELED_READ_RESP,      /*!< Read complete event */
    ADI_EAL_ISELED_CBK_EVT_ISELED_WRITE_RESP,     /*!< Write complete event */
    ADI_EAL_ISELED_CBK_EVT_ISELED_FIFO_READ,      /*!< FIFO read event when EAL is configured in ADI_EAL_USAGE_MODE_CUSTOM */
    ADI_EAL_ISELED_CBK_EVT_INTF_STATUS_EVT,       /*!< ISELED interface status event */
    ADI_EAL_ISELED_CBK_EVT_FIFO_STATUS_EVT,       /*!< FIFO status event */
    ADI_EAL_ISELED_CBK_EVT_ERR,                   /*!< Error event */
    ADI_EAL_ISELED_CBK_EVT_MAILBOX_ACK,           /*!< Mailbox Acknowledgment event */
} ADI_EAL_ISELED_CBK_EVT;

/*! ISELED transaction mode */
typedef enum ADI_EAL_ISELED_MODE
{
    ADI_EAL_ISELED_NORMAL_MODE,         /*!< Normal mode (3 bytes of data) */
    ADI_EAL_ISELED_SEQ_MODE,            /*!< Sequential mode (Address auto-increments) */
    ADI_EAL_ISELED_DIM_RGB_MODE,        /*!< Combination mode (Address applies for 4 bytes of data (2 commands)) */
    ADI_EAL_ISELED_SEQ_PLUS_DIM_MODE,   /*!< Sequential and combination mode (Address auto-increments and send data for 2 commands) */
} ADI_EAL_ISELED_MODE;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */
typedef void *ADI_EAL_ISELED_HANDLE;                /*!< The ISELED EAL handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */
typedef void (*ADI_EAL_ISELED_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_ISELED_HANDLE hIseledHandle,
        ADI_EAL_ISELED_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL ISELED callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief ISELED interface status data */
typedef struct ADI_EAL_ISELED_STATUS_DATA
{
    uint16_t nNumRdPackets;         /*!< Number of bytes read */
    uint16_t nNumWrPackets;         /*!< Number of bytes written */
    bool bBusBusy;                  /*!< ISELED bus busy status */
    bool bContention;               /*!< Bus contention status */
    bool bRcvFrameEOCError;         /*!< Frame has EOC error */
    bool bRcvFrameCRCError;         /*!< Frame has CRC error */
    bool bRcvFrameFreqSyncError;    /*!< Frame has freq sync error */
    bool bRcvFrameFrameSyncError;   /*!< Frame has frame sync error */
} ADI_EAL_ISELED_STATUS_DATA;

/*! @brief Data structure for each EAL ISELED instance */
typedef struct EAL_ISELED_INST_DATA
{
    /* ~~~~~~~~~~~ ISELED instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;                /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;              /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;            /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;          /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ ISELED state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;         /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;              /*!< Indicates in which mode eal is used */
    ADI_EAL_ISELED_CALLBACK_FUNC pfunCbk;   /*!< Pointer to store the address of callback function */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn;   /*!< Indicates if Read time tag enabled */
    uint16_t            nIseledCfgQDepth;   /*!< Indicates the size of config queue */
    volatile uint16_t   nIseledQWrIdx;      /*!< Indicates the Write index of transaction config queue */
    volatile uint16_t   nIseledQRdIdx;      /*!< Indicates the Read index of transaction config queue */
    ADI_E2BCORE_ISELED_TXN_CONFIG *paoCfgs;  /*!< ISELED transaction config queue */

    /* ~~~~~~~~~~~ Mutex for the ISELED instance ~~~~~~~~~~~ */
    uint8_t anIseledMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];   /*!< Buffer for Iseled mutex */
    PRJ_OSAL_MUTEX_CFG oIseledMutexCfg;     /*!< Iseled mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hIseledMutex;     /*!< Handle for the Iseled mutex */
} EAL_ISELED_INST_DATA;

/*! Internal data structure behind the EAL ISELED handle */
typedef struct EAL_ISELED_DATA
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
    EAL_ISELED_INST_DATA **apoInstData;     /*!< Array of unicast instance data pointers */
} EAL_ISELED_DATA;

/*! @brief EAL ISELED I/O DATA */
typedef struct ADI_EAL_ISELED_IO_DATA
{
    ADI_EAL_ISELED_MODE eMode;          /*!< ISELED transaction mode */
    uint16_t    nNumIseledCmds;         /*!< Number of ISELED commands */
    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD *pIseledCmds; /*!< Array of ISELED commands */
    uint16_t    nExpectedNumResp;       /*!< Number of ISELED responses expected for this transaction */
    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD **apIseledResps; /*!< Pointer to array of ISELED responses */
} ADI_EAL_ISELED_IO_DATA;

/**
 * @brief EAL ISELED configuration data
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL ISELED APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_ISELED_CFG
{
    bool bIsReadTimeTagEn;          /*!< Indicates if Read time tag enabled */
    uint32_t nInstNum;              /*!< Indicates the instance number */
    uint32_t nRemoteNum;            /*!< Indicates the remote number */
    uint32_t nIntfNum;              /*!< Indicates the interface number */
    uint32_t nEalIseledMemorySize;  /*!< Indicates the allocated memory size*/
    uint16_t nIseledCfgQDepth;      /*!< Indicates the size of config queue*/
    uint8_t *pEalIseledMemory;      /*!< Pointer to allocated memory (must be 4-byte or 8-byte aligned) */
} ADI_EAL_ISELED_CFG;

/*! @brief Configuration for EAL ISELED multicast group */
typedef struct ADI_EAL_ISELED_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_ISELED_HANDLE *pahIseledHandles;  /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;     /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;   /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag; /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nIseledMcastMemSize; /*!< Size of the ISELED multicast group data */
    void               *pIseledMcastMem;   /*!< Pointer to the ISELED multicast group memory */
} ADI_EAL_ISELED_MCAST_CFG;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! Data for #ADI_EAL_ISELED_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_ISELED_CBK_EVT_REG_READ_RESP_DATA;

/** Data for #ADI_EAL_ISELED_CBK_EVT_ISELED_READ_RESP event */
typedef ADI_E2BCORE_ISELED_TXN_CONFIG ADI_EAL_ISELED_CBK_EVT_ISELED_READ_RESP_DATA;

/** Data for #ADI_EAL_ISELED_CBK_EVT_ISELED_WRITE_RESP event */
typedef ADI_E2BCORE_ISELED_TXN_CONFIG ADI_EAL_ISELED_CBK_EVT_ISELED_WRITE_RESP_DATA;

/** Data for #ADI_EAL_ISELED_CBK_EVT_ISELED_FIFO_READ event */
typedef void *ADI_EAL_ISELED_CBK_EVT_ISELED_FIFO_READ_DATA;

/** Data for #ADI_EAL_ISELED_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_ISELED_STATUS_DATA ADI_EAL_ISELED_CBK_EVT_INTF_STATUS_EVT_DATA;

/** Data for #ADI_EAL_ISELED_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_ISELED_CBK_EVT_FIFO_STATUS_EVT_DATA;

/** Data for #ADI_EAL_ISELED_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_ISELED_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_ISELED_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_ISELED_CBK_EVT_MAILBOX_ACK_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/

extern ADI_EAL_STATUS adi_eal_iseledOpen(ADI_EAL_ISELED_CFG *poIseledCfg, ADI_EAL_ISELED_HANDLE *phIseledHandle);
extern ADI_EAL_STATUS adi_eal_iseledClose(ADI_EAL_ISELED_HANDLE hIseledHandle);
extern ADI_EAL_STATUS adi_eal_iseledRegisterCallback(ADI_EAL_ISELED_HANDLE hIseledHandle,
        ADI_EAL_ISELED_CALLBACK_FUNC pfIseledCallback);
extern ADI_EAL_STATUS adi_eal_iseledCreateMcastGroup(ADI_EAL_ISELED_MCAST_CFG *poIseledMcastCfg,
        ADI_EAL_ISELED_HANDLE *phIseledMcastHandle);
extern ADI_EAL_STATUS adi_eal_iseledWrite(ADI_EAL_ISELED_HANDLE hIseledHandle, ADI_EAL_ISELED_IO_DATA *poIseledData,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_iseledCustomWrite(ADI_EAL_ISELED_HANDLE hIseledHandle,
        ADI_E2BCORE_ISELED_TXN_CONFIG *paoIseledCfgs, uint16_t nNumIseledTxns);
extern ADI_EAL_STATUS adi_eal_iseledReadStatus(ADI_EAL_ISELED_HANDLE hIseledHandle);
extern ADI_EAL_STATUS adi_eal_iseledSendRegTransaction(ADI_EAL_ISELED_HANDLE hIseledHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_iseledResetQueue(ADI_EAL_ISELED_HANDLE hIseledHandle);
extern void adi_eal_iseledSetDownstreamVlan(ADI_EAL_ISELED_HANDLE hIseledHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_iseledSetUpstreamVlan(ADI_EAL_ISELED_HANDLE hIseledHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_iseledSetMailboxMode(ADI_EAL_ISELED_HANDLE hIseledHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_iseledResyncSeqNum(ADI_EAL_ISELED_HANDLE hIseledHandle);
/*============================ CALLBACKS ============================*/

extern void IseledInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                               void *pCbParam);


#endif /* ADI_E2B_IFACE_ISELED_ENABLED */
#endif /* ADI_EAL_ISELED_H */
/** @} */

/*
 * EOF: www.analog.com
 */
