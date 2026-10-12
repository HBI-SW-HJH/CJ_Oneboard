/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_spi.h
 * \brief:The header file for SPI functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_SPI_H
#define ADI_EAL_SPI_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "e2bcore_spi.h"    /* e2bcore SPI header */

#ifdef ADI_E2B_IFACE_SPI_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each SPI unicast instance */
#define ADI_EAL_SPI_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_SPI_INST_DATA) + sizeof(EAL_SPI_INST_DATA*) + sizeof(EAL_SPI_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_SPI_TXN_CONFIG) + sizeof(ADI_E2BCORE_SPI_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an SPI multicast group */
#define ADI_EAL_SPI_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_SPI_DATA) + (nNumInst)*sizeof(EAL_SPI_INST_DATA*))

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL SPI callback event type
 */
typedef enum ADI_EAL_SPI_CBK_EVT
{
    ADI_EAL_SPI_CBK_EVT_REG_READ_RESP,          /*!< Reg Read complete event */
    ADI_EAL_SPI_CBK_EVT_SPI_READ_RESP,          /*!< Read complete event */
    ADI_EAL_SPI_CBK_EVT_SPI_WRITE_RESP,         /*!< Write complete event */
    ADI_EAL_SPI_CBK_EVT_SPI_FIFO_READ,          /*!< FIFO read event when EAL is configured in ADI_EAL_USAGE_MODE_CUSTOM */
    ADI_EAL_SPI_CBK_EVT_PERIODIC_RX,            /*!< Periodic Rx received event */
    ADI_EAL_SPI_CBK_EVT_INTF_STATUS_EVT,        /*!< SPI interface status event */
    ADI_EAL_SPI_CBK_EVT_FIFO_STATUS_EVT,        /*!< FIFO status event */
    ADI_EAL_SPI_CBK_EVT_ERR,                    /*!< Error event */
    ADI_EAL_SPI_CBK_EVT_MAILBOX_ACK,            /*!< Mailbox ACK event */
} ADI_EAL_SPI_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_SPI_HANDLE;               /*!< The SPI EAL handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void (*ADI_EAL_SPI_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_SPI_HANDLE hSpiHandle,
        ADI_EAL_SPI_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL SPI callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/** @brief SPI interface status data */
typedef struct ADI_EAL_SPI_STATUS_DATA
{
    bool bBusBusy;                  /*!< SPI bus busy status */
    uint16_t nNumRdBytes;           /*!< Number of bytes read from the MISO line */
    uint16_t nNumSpiTxns;           /*!< Number of SPI transactions processed */
    uint32_t nNumWrBytes;           /*!< Number of bytes written on the MOSI line */
} ADI_EAL_SPI_STATUS_DATA;

/*! @brief Data structure for each EAL SPI instance */
typedef struct EAL_SPI_INST_DATA
{
    /* ~~~~~~~~~~~ SPI instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;            /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;          /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;        /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;      /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ SPI state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;     /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;          /*!< Indicates in which mode eal is used */
    ADI_EAL_SPI_CALLBACK_FUNC pfunCbk;  /*!< Pointer to store the address of callback function */
    volatile bool       bWaitingForRx;  /*!< Flag indicating the API is waiting for a response */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn; /*!< Indicates if Read time tag enabled */
    uint16_t            nSpiCfgQDepth;  /*!< Indicates the size of config queue*/
    volatile uint16_t   nSpiQWrIdx;     /*!< Indicates the Write index of transaction config queue */
    volatile uint16_t   nSpiQRdIdx;     /*!< Indicates the Read index of transaction config queue */
    ADI_E2BCORE_SPI_TXN_CONFIG *paoCfgs;  /*!< SPI transaction config queue */

    /* ~~~~~~~~~~~ Mutex for the SPI instance ~~~~~~~~~~~ */
    uint8_t anSpiMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];   /*!< Buffer for Spi mutex */
    PRJ_OSAL_MUTEX_CFG oSpiMutexCfg;            /*!< Spi mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hSpiMutex;            /*!< Handle for the Spi mutex */

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    uint8_t anSpiFifoRespSemaphoreBuffer[PRJ_OSAL_SEMAPHORE_MEM_SIZE];   /*!< Buffer for Spi semaphore */
    PRJ_OSAL_SEMAPHORE_CFG oFifoRespSemaphoreCfg; /*!< Semaphore configuration for FIFO response */
    PRJ_OSAL_SEMAPHORE_HANDLE hFifoRespSemaphore; /*!< Semaphore handle for FIFO response */
#endif
} EAL_SPI_INST_DATA;

/*! Internal data structure behind the EAL SPI handle */
typedef struct EAL_SPI_DATA
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
    EAL_SPI_INST_DATA   **apoInstData;  /*!< Array of unicast instance data pointers */
} EAL_SPI_DATA;

/*! @brief EAL SPI I/O DATA */
typedef struct ADI_EAL_SPI_IO_DATA
{
    uint8_t *pTxData;           /*!< Pointer to store Tx data */
    uint8_t **apRxData;         /*!< Array of pointer to RX data */
    uint16_t nWriteLength;      /*!< The write length */
    uint16_t nReadLength;       /*!< The read length */
    uint8_t nChipSel;           /*!< Chip select */
} ADI_EAL_SPI_IO_DATA;

/**
 * @brief EAL SPI configuration data
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL SPI APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_SPI_CFG
{
    uint32_t nInstNum;              /*!< Indicates the instance number */
    uint32_t nRemoteNum;            /*!< Indicates the remote number */
    uint32_t nIntfNum;              /*!< Indicates the interface number */
    uint32_t nEalSpiMemorySize;     /*!< Indicates the allocated memory size*/
    uint16_t nSpiCfgQDepth;         /*!< Indicates the size of config queue*/
    uint8_t *pEalSpiMemory;         /*!< Pointer to allocated memory (must be 4-byte or 8-byte aligned) */
} ADI_EAL_SPI_CFG;

/*! @brief Configuration for EAL SPI multicast group */
typedef struct ADI_EAL_SPI_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_SPI_HANDLE *pahSpiHandles;  /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;      /*!< Poinetr to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;    /*!< Poinetr to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;  /*!< Poinetr to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nSpiMcastMemSize; /*!< Size of the SPI multicast group data */
    void               *pSpiMcastMem;   /*!< Pointer to the SPI multicast group memory */
} ADI_EAL_SPI_MCAST_CFG;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_SPI_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_SPI_READ_RESP event */
typedef ADI_E2BCORE_SPI_TXN_CONFIG ADI_EAL_SPI_CBK_EVT_SPI_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_SPI_WRITE_RESP event */
typedef ADI_E2BCORE_SPI_TXN_CONFIG ADI_EAL_SPI_CBK_EVT_SPI_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_SPI_FIFO_READ event */
typedef ADI_E2BCORE_CBK_FIFOREADDAT ADI_EAL_SPI_CBK_EVT_SPI_FIFO_READ_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_PERIODIC_RX event */
typedef ADI_E2BCORE_SPI_TXN_CONFIG ADI_EAL_SPI_CBK_EVT_PERIODIC_RX_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_SPI_STATUS_DATA ADI_EAL_SPI_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_SPI_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_SPI_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_SPI_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_SPI_CBK_EVT_MAILBOX_ACK_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_spiOpen(ADI_EAL_SPI_CFG *poSpiCfg, ADI_EAL_SPI_HANDLE *phSpiHandle);
extern ADI_EAL_STATUS adi_eal_spiClose(ADI_EAL_SPI_HANDLE hSpiHandle);
extern ADI_EAL_STATUS adi_eal_spiRegisterCallback(ADI_EAL_SPI_HANDLE hSpiHandle,
        ADI_EAL_SPI_CALLBACK_FUNC pfSpiCallback);
extern ADI_EAL_STATUS adi_eal_spiCreateMcastGroup(ADI_EAL_SPI_MCAST_CFG *poSpiMcastCfg,
        ADI_EAL_SPI_HANDLE *phSpiMcastHandle);
extern ADI_EAL_STATUS adi_eal_spiWrite(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiWriteSync(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiWriteRead(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiWriteReadSync(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiWriteWithDelay(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiWriteWithPresTs(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiSetupPeriodicWriteRead(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiWriteReadWithDelay(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiWriteReadWithPresTs(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiMultiWrite(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_EAL_SPI_IO_DATA *poSpiData,
        uint16_t nNumPackTxn, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_spiReadStatus(ADI_EAL_SPI_HANDLE hSpiHandle);
extern ADI_EAL_STATUS adi_eal_spiCustomWriteRead(ADI_EAL_SPI_HANDLE hSpiHandle,
        ADI_E2BCORE_SPI_TXN_CONFIG *paoSpiTxnCfg, uint16_t nNumSpiTxns);
extern ADI_EAL_STATUS adi_eal_spiSendRegTransaction(ADI_EAL_SPI_HANDLE hSpiHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_spiResetQueue(ADI_EAL_SPI_HANDLE hSpiHandle);
extern void adi_eal_spiSetDownstreamVlan(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_spiSetUpstreamVlan(ADI_EAL_SPI_HANDLE hSpiHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_spiSetMailboxMode(ADI_EAL_SPI_HANDLE hSpiHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_spiResyncSeqNum(ADI_EAL_SPI_HANDLE hSpiHandle);
/*============================ CALLBACKS ============================*/

extern void SpiInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                            void *pCbParam);


#endif /* ADI_E2B_IFACE_SPI_ENABLED */
#endif /* ADI_EAL_SPI_H */

/** @} */

/**
 * EOF: www.analog.com
 */
