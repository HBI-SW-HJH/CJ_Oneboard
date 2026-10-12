/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_fioOSP.h
 * \brief:The header file for Flexible IO OSP functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_FIOOSP_H
#define ADI_EAL_FIOOSP_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "e2bcore_flexibleIO.h"         /* Flexible IO e2bcore header */

#ifdef ADI_E2B_IFACE_FIO_OSP_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each FIO_OSP unicast instance */
#define ADI_EAL_FIO_OSP_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_FIO_OSP_INST_DATA) + sizeof(EAL_FIO_OSP_INST_DATA*) + sizeof(EAL_FIO_OSP_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG) + sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an FIO_OSP multicast group */
#define ADI_EAL_FIO_OSP_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_FIO_OSP_DATA) + (nNumInst)*sizeof(EAL_FIO_OSP_INST_DATA*))

#define ADI_FIO_OSP_EAL_PAYLOAD_MAX_SIZE (34U) /*!< Maximum payload size for FIO OSP */
#define ADI_FIO_OSP_ADD_TXRX_SIZE        (2U) /*!< Additional frame size with tx and rx size added */

/*================================ DATA TYPES ===============================*/
/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief EAL FIO OSP callback event type
 */
typedef enum ADI_EAL_FIO_OSP_CBK_EVT
{
    ADI_EAL_FIO_OSP_CBK_EVT_REG_READ_RESP,       /*!< Reg Read complete event */
    ADI_EAL_FIO_OSP_CBK_EVT_OSP_READ_RESP,       /*!< Read complete event */
    ADI_EAL_FIO_OSP_CBK_EVT_OSP_WRITE_RESP,      /*!< Write complete event */
    ADI_EAL_FIO_OSP_CBK_EVT_OSP_FIFO_READ,       /*!< FIFO read event when EAL is configured in ADI_EAL_USAGE_MODE_CUSTOM */
    ADI_EAL_FIO_OSP_CBK_EVT_PERIODIC_RX,         /*!< Periodic Rx received event */
    ADI_EAL_FIO_OSP_CBK_EVT_INTF_STATUS_EVT,     /*!< FIO OSP interface status event */
    ADI_EAL_FIO_OSP_CBK_EVT_FIFO_STATUS_EVT,     /*!< FIFO status event */
    ADI_EAL_FIO_OSP_CBK_EVT_READ_TIMEOUT,        /*!< FIO OSP Read Timeout interrupt triggered */
    ADI_EAL_FIO_OSP_CBK_EVT_ERR,                 /*!< Error event */
} ADI_EAL_FIO_OSP_CBK_EVT;

/** EAL FIO PDM Clock modes */
typedef enum ADI_EAL_FIO_OSP_CLK_FREQ
{
    ADI_EAL_FIO_OSP_CLK_2_4_MHZ,
    ADI_EAL_FIO_OSP_CLK_2_0_MHZ
} ADI_EAL_FIO_OSP_CLK_FREQ;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_FIO_OSP_HANDLE;                /*!< The EAL FIO OSP handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */
typedef void (*ADI_EAL_FIO_OSP_CALLBACK_FUNC)(
    uint32_t nInstNum,
    ADI_EAL_FIO_OSP_HANDLE hOspHandle,
    ADI_EAL_FIO_OSP_CBK_EVT eCbkEventType,
    const void *pArg);                              /*!< The EAL FIO OSP callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */
/*! @brief Data structure for each EAL FIO OSP instance */
typedef struct EAL_FIO_OSP_INST_DATA
{
    /* ~~~~~~~~~~~ FIO OSP instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID oId;                       /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;                /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;              /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;            /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ FIO OSP state info ~~~~~~~~~~~ */
    bool bIgnoreErr;                          /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE eMode;                 /*!< Indicates in which mode eal is used */
    ADI_EAL_FIO_OSP_CALLBACK_FUNC pfunCbk;    /*!< Pointer to store the address of callback function */
    volatile bool bWaitingForRx;                       /*!< Flag to check if application is waiting for Rx data */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool bIsReadTimeTagEn;                       /*!< Indicates if read time tag enabled */
    volatile uint16_t nFioOspQWrIdx;            /*!< Indicates the write index of transaction config queue */
    volatile uint16_t nFioOspQRdIdx;            /*!< Indicates the read index of transaction config queue */
    uint16_t nFioOspCfgQDepth;                  /*!< Indicates the size of config queue */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoCfgs; /*!< FIO OSP transaction config queue */

    /* ~~~~~~~~~~~ Mutex for the FIO OSP instance ~~~~~~~~~~~ */
    uint8_t anFioOspMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /*!< Buffer for FIO OSP mutex */
    PRJ_OSAL_MUTEX_CFG oFioOspMutexCfg;          /*!< FIO OSP mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hFioOspMutex;          /*!< Handle for the FIO OSP mutex */
} EAL_FIO_OSP_INST_DATA;

/*! Internal data structure behind the EAL FIO OSP handle */
typedef struct EAL_FIO_OSP_DATA
{
    bool                bIsMulticast;       /*!< Is this a multicast group or a unicast instance? */
    bool                bInUse;             /*!< Is this handle opened? */
    uint32_t            nIntfTopic;         /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;         /*!< Topic for accessing FIFO of this mcast group */
    uint32_t            nIfRegVlanTag;      /*!< VLAN tag for interface register access of this mcast group */
    uint32_t            nFifoRegVlanTag;    /*!< VLAN tag for FIFO register access of this mcast group */
    uint32_t            nFifoWriteVlanTag;  /*!< VLAN tag for FIFO write transactions of this mcast group */
    uint16_t            nNumInst;           /*!< Number of unicast instances in this group */
    EAL_FIO_OSP_INST_DATA   **apoInstData; /*!< Array of unicast instance data pointers */
} EAL_FIO_OSP_DATA;

/**
 * @brief EAL FIO OSP configuration data
 */
typedef struct ADI_EAL_FIO_OSP_CFG
{
    uint32_t nInstNum;              /*!< Indicates the instance number */
    uint32_t nRemoteNum;            /*!< Indicates the remote number */
    uint32_t nIntfNum;              /*!< Indicates the interface number */
    uint32_t nFioOspMemorySize;     /*!< Indicates the allocated memory size */
    uint8_t *pFioOspMemory;         /*!< Pointer to allocated memory */
    uint16_t nFioOspCfgQDepth;      /*!< Indicates the size of config queue */
} ADI_EAL_FIO_OSP_CFG;

/*! @brief Configuration for EAL FIO OSP multicast group */
typedef struct ADI_EAL_FIO_OSP_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_FIO_OSP_HANDLE *pahFioOspHandles; /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;      /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;    /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;  /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nFioOspMcastMemSize;   /*!< Size of the FIO_OSP multicast group data */
    void               *pFioOspMcastMem;       /*!< Pointer to the FIO_OSP multicast group memory */
} ADI_EAL_FIO_OSP_MCAST_CFG;

/*! @brief EAL FIO OSP I/O DATA */
typedef struct ADI_EAL_FIO_OSP_IO_DATA
{
    uint8_t *pTxData;            /*!< Pointer to store Tx data */
    uint8_t **apRxData;          /*!< Array of pointer to RX data */
    uint16_t nWriteLength;       /*!< The write length */
    uint16_t nReadLength;        /*!< The read length */
} ADI_EAL_FIO_OSP_IO_DATA;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_FIO_OSP_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_OSP_READ_RESP event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_OSP_CBK_EVT_OSP_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_OSP_WRITE_RESP event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_OSP_CBK_EVT_OSP_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_OSP_FIFO_READ event */
typedef ADI_E2BCORE_CBK_FIFOREADDAT ADI_EAL_FIO_OSP_CBK_EVT_OSP_FIFO_READ_DATA;

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_PERIODIC_RX event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_OSP_CBK_EVT_PERIODIC_RX_DATA;

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_FIO_STATUS_DATA ADI_EAL_FIO_OSP_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_FIO_OSP_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_FIO_OSP_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_FIO_OSP_CBK_EVT_ERR_DATA;

/*! @brief Data to be used for updating instruction RAM */
typedef ADI_EAL_FIO_INST_RAM_DATA ADI_EAL_FIO_OSP_INST_RAM_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_fioOspOpen(ADI_EAL_FIO_OSP_CFG *poFioOspCfg, ADI_EAL_FIO_OSP_HANDLE *phFioOspHandle);
extern ADI_EAL_STATUS adi_eal_fioOspClose(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle);
extern ADI_EAL_STATUS adi_eal_fioOspRegisterCallback(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_CALLBACK_FUNC pfFioOspCallback);
extern ADI_EAL_STATUS adi_eal_fioOspCreateMcastGroup(ADI_EAL_FIO_OSP_MCAST_CFG *poFioOspMcastCfg,
        ADI_EAL_FIO_OSP_HANDLE *phFioOspMcastHandle);
extern ADI_EAL_STATUS adi_eal_fioOspConfigureInstructionRAM(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_CLK_FREQ eFioOspClkFreq);

extern ADI_EAL_STATUS adi_eal_fioOspWrite(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspWriteSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspWriteReadSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId);

extern ADI_EAL_STATUS adi_eal_fioOspWriteWithDelay(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspWriteReadWithDelay(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint32_t nDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspWriteWithPresTs(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspWriteReadWithPresTs(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint64_t nTimestamp, uint8_t nId);

extern ADI_EAL_STATUS adi_eal_fioOspMultiWrite(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
        uint16_t nNumPackedTxns, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspMultiWriteSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
       uint16_t nNumPackedTxns, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspMultiWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
       uint16_t nNumPackedTxns, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspMultiWriteReadSync(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData,
       uint16_t nNumPackedTxns, uint8_t nId);

extern ADI_EAL_STATUS adi_eal_fioOspSetupPeriodicWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_EAL_FIO_OSP_IO_DATA *poFioOspIoData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_fioOspCustomWriteRead(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoFioOspTxnCfg, uint16_t nNumFioOspTxns);

extern ADI_EAL_STATUS adi_eal_fioOspReadStatus(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle);
extern ADI_EAL_STATUS adi_eal_fioOspSendRegTransaction(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle,
        bool bIsFifoRegTransaction, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_fioOspResetQueue(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle);

extern void adi_eal_fioOspSetDownstreamVlan(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_fioOspSetUpstreamVlan(ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
/*============================ CALLBACKS ============================*/

extern void FioOspInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                               void *pCbParam);


#endif /* ADI_E2B_IFACE_FIO_OSP_ENABLED */
#endif /* ADI_EAL_FIOOSP_H */

/** @} */

/**
 * EOF: www.analog.com
 */
