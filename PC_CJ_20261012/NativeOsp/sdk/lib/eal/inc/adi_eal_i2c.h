/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_i2c.h
 * \brief:The header file for I2C functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_I2C_H
#define ADI_EAL_I2C_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "e2bcore_i2c.h"    /* e2bcore I2C header */

#ifdef ADI_E2B_IFACE_I2C_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each I2C unicast instance */
#define ADI_EAL_I2C_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_I2C_INST_DATA) + sizeof(EAL_I2C_INST_DATA*) + sizeof(EAL_I2C_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_I2C_TXN_CONFIG) + sizeof(ADI_E2BCORE_I2C_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an I2C multicast group */
#define ADI_EAL_I2C_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_I2C_DATA) + (nNumInst)*sizeof(EAL_I2C_INST_DATA*))

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL I2C callback event type
 */
typedef enum ADI_EAL_I2C_CBK_EVT
{
    ADI_EAL_I2C_CBK_EVT_REG_READ_RESP,            /*!< Reg Read complete event */
    ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP,            /*!< Read complete event */
    ADI_EAL_I2C_CBK_EVT_I2C_WRITE_RESP,           /*!< Write complete event */
    ADI_EAL_I2C_CBK_EVT_I2C_FIFO_READ,            /*!< FIFO read event when EAL is configured in ADI_EAL_USAGE_MODE_CUSTOM */
    ADI_EAL_I2C_CBK_EVT_PERIODIC_RX,              /*!< Periodic Rx received event */
    ADI_EAL_I2C_CBK_EVT_INTF_STATUS_EVT,          /*!< I2C interface status event */
    ADI_EAL_I2C_CBK_EVT_FIFO_STATUS_EVT,          /*!< FIFO status event */
    ADI_EAL_I2C_CBK_EVT_ERR,                      /*!< Error event */
    ADI_EAL_I2C_CBK_EVT_MAILBOX_ACK,              /*!< Mailbox ACK event */
} ADI_EAL_I2C_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_I2C_HANDLE;                         /*!< The I2C EAL handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void (*ADI_EAL_I2C_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_I2C_HANDLE hI2cHandle,
        ADI_EAL_I2C_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL I2C callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief I2C interface status data */
typedef struct ADI_EAL_I2C_STATUS_DATA
{
    bool bBusBusy;                  /*!< I2C bus busy status */
    bool bAddrNack;                 /*!< Device address not acknowledged */
    bool bDataNack;                 /*!< Data not acknowledged */
    uint16_t nNumRdBytes;           /*!< Number of bytes read from the I2C bus */
    uint16_t nNumI2cTxns;           /*!< Number of I2C transactions processed */
    uint32_t nNumWrBytes;           /*!< Number of bytes written on the I2C bus */
} ADI_EAL_I2C_STATUS_DATA;

/*! @brief Data structure for each EAL I2C instance */
typedef struct EAL_I2C_INST_DATA
{
    /* ~~~~~~~~~~~ I2C instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;            /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;          /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;        /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;      /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ I2C state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;     /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;          /*!< Indicates in which mode eal is used */
    ADI_EAL_I2C_CALLBACK_FUNC pfunCbk;  /*!< Pointer to store the address of callback function */
    volatile bool       bWaitingForRx;  /*!< Flag indicating the API is waiting for a response */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn; /*!< Indicates if Read time tag enabled */
    uint16_t            nI2cCfgQDepth;  /*!< Indicates the size of config queue*/
    volatile uint16_t   nI2cQWrIdx;     /*!< Indicates the Write index of transaction config queue */
    volatile uint16_t   nI2cQRdIdx;     /*!< Indicates the Read index of transaction config queue */
    ADI_E2BCORE_I2C_TXN_CONFIG *paoCfgs;  /*!< I2C transaction config queue */

    /* ~~~~~~~~~~~ Mutex for the I2C instance ~~~~~~~~~~~ */
    uint8_t anI2cMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];   /*!< Buffer for I2c mutex */
    PRJ_OSAL_MUTEX_CFG oI2cMutexCfg;            /*!< I2c mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hI2cMutex;            /*!< Handle for the I2c mutex */

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    uint8_t anI2cFifoRespSemaphoreBuffer[PRJ_OSAL_SEMAPHORE_MEM_SIZE];   /*!< Buffer for I2C semaphore */
    PRJ_OSAL_SEMAPHORE_CFG oFifoRespSemaphoreCfg; /*!< Semaphore configuration for FIFO response */
    PRJ_OSAL_SEMAPHORE_HANDLE hFifoRespSemaphore; /*!< Semaphore handle for FIFO response */
#endif
} EAL_I2C_INST_DATA;

/*! Internal data structure behind the EAL I2C handle */
typedef struct EAL_I2C_DATA
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
    EAL_I2C_INST_DATA   **apoInstData;      /*!< Array of unicast instance data pointers */
} EAL_I2C_DATA;

/*! @brief EAL I2C I/O DATA */
typedef struct ADI_EAL_I2C_IO_DATA
{
    uint8_t nAddress;           /*!< 7-bit read and write address */
    uint8_t *pTxData;           /*!< Pointer to store Tx data */
    uint8_t **apRxData;         /*!< Array of pointer to RX data */
    uint16_t nWriteLength;      /*!< The write length */
    uint16_t nReadLength;       /*!< The read length */
} ADI_EAL_I2C_IO_DATA;

/**
 * @brief EAL I2C configuration data
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL I2C APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_I2C_CFG
{
    uint32_t nInstNum;            /*!< Indicates the instance number */
    uint32_t nRemoteNum;          /*!< Indicates the remote number */
    uint32_t nIntfNum;            /*!< Indicates the interface number */
    uint32_t nEalI2cMemorySize;   /*!< Indicates the allocated memory size*/
    uint16_t nI2cCfgQDepth;       /*!< Indicates the size of config queue*/
    uint8_t *pEalI2cMemory;       /*!< Pointer to allocated memory (must be 4-byte or 8-byte aligned) */
} ADI_EAL_I2C_CFG;

/*! @brief Configuration for EAL I2C multicast group */
typedef struct ADI_EAL_I2C_MCAST_CFG
{
    uint16_t            nNumInst;           /*!< Number of unicast instances in the multicast group */
    ADI_EAL_I2C_HANDLE *pahI2cHandles;      /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;         /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;         /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG       *poIfRegVlanTag;     /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG       *poFifoRegVlanTag;   /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG       *poFifoWriteVlanTag; /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nI2cMcastMemSize;   /*!< Size of the I2C multicast group data */
    void               *pI2cMcastMem;       /*!< Pointer to the I2C multicast group memory */
} ADI_EAL_I2C_MCAST_CFG;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */
/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_I2C_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP event */
typedef ADI_E2BCORE_I2C_TXN_CONFIG ADI_EAL_I2C_CBK_EVT_I2C_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_I2C_WRITE_RESP event */
typedef ADI_E2BCORE_I2C_TXN_CONFIG ADI_EAL_I2C_CBK_EVT_I2C_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_I2C_FIFO_READ event */
typedef ADI_E2BCORE_CBK_FIFOREADDAT ADI_EAL_I2C_CBK_EVT_I2C_FIFO_READ_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_PERIODIC_RX event */
typedef ADI_E2BCORE_I2C_TXN_CONFIG ADI_EAL_I2C_CBK_EVT_PERIODIC_RX_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_I2C_STATUS_DATA ADI_EAL_I2C_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_I2C_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_I2C_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_I2C_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_I2C_CBK_EVT_MAILBOX_ACK_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_i2cOpen(ADI_EAL_I2C_CFG *poI2cCfg, ADI_EAL_I2C_HANDLE *phI2cHandle);
extern ADI_EAL_STATUS adi_eal_i2cClose(ADI_EAL_I2C_HANDLE hI2cHandle);
extern ADI_EAL_STATUS adi_eal_i2cRegisterCallback(ADI_EAL_I2C_HANDLE hI2cHandle,
        ADI_EAL_I2C_CALLBACK_FUNC pfI2cCallback);
extern ADI_EAL_STATUS adi_eal_i2cCreateMcastGroup(ADI_EAL_I2C_MCAST_CFG *poI2cMcastCfg,
        ADI_EAL_I2C_HANDLE *phI2cMcastHandle);
extern ADI_EAL_STATUS adi_eal_i2cWrite(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cWriteSync(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cRead(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cReadSync(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cWriteRead(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cWriteReadSync(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cWriteWithDelay(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cWriteWithPresTs(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cReadWithDelay(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cReadWithPresTs(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cWriteReadWithDelay(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cWriteReadWithPresTs(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cSetupPeriodicWriteRead(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_EAL_I2C_IO_DATA *poI2cData,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_i2cReadStatus(ADI_EAL_I2C_HANDLE hI2cHandle);
extern ADI_EAL_STATUS adi_eal_i2cCustomWriteRead(ADI_EAL_I2C_HANDLE hI2cHandle,
        ADI_E2BCORE_I2C_TXN_CONFIG *paoI2cTxnCfg, uint16_t nNumI2cTxns);
extern ADI_EAL_STATUS adi_eal_i2cSendRegTransaction(ADI_EAL_I2C_HANDLE hI2cHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_i2cResetQueue(ADI_EAL_I2C_HANDLE hI2cHandle);
extern void adi_eal_i2cSetDownstreamVlan(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_i2cSetUpstreamVlan(ADI_EAL_I2C_HANDLE hI2cHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_i2cSetMailboxMode(ADI_EAL_I2C_HANDLE hI2cHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_i2cResyncSeqNum(ADI_EAL_I2C_HANDLE hI2cHandle);
/*============================ CALLBACKS ============================*/
extern void I2cInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                            void *pCbParam);


#endif /* ADI_E2B_IFACE_I2C_ENABLED */
#endif /* ADI_EAL_I2C_H */

/** @} */

/**
 * EOF: www.analog.com
 */
