/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: adi_eal_fioPdm.h
 * @brief:The header file for FIO PDM functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_FIOPDM_H
#define ADI_EAL_FIOPDM_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"                    /* The EAL header */
#include "e2bcore_flexibleIO.h"         /* Flexible IO e2bcore header */

#ifdef ADI_E2B_IFACE_FIO_PDM_ENABLED


/*================================= DEFINES =================================*/
/** Helper macro to compute the memory size required for each FIO_PDM unicast instance */
#define ADI_EAL_FIO_PDM_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_FIO_PDM_INST_DATA) + sizeof(EAL_FIO_PDM_INST_DATA*) + sizeof(EAL_FIO_PDM_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG) + sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an FIO_PDM multicast group */
#define ADI_EAL_FIO_PDM_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_FIO_PDM_DATA) + (nNumInst)*sizeof(EAL_FIO_PDM_INST_DATA*))

/*! Minimum clock period for FIO PDM interface - in ns*/
#define ADI_EAL_FIO_PDM_MIN_CLOCK_PERIOD 240U

/*! Maximum clock period for FIO PDM interface - in ns*/
#define ADI_EAL_FIO_PDM_MAX_CLOCK_PERIOD 1000U
/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/** EAL FIO PDM callback event type */
typedef enum ADI_EAL_FIO_PDM_CBK_EVT
{
    ADI_EAL_FIO_PDM_CBK_EVT_REG_READ_RESP,      /**< Reg Read complete event */
    ADI_EAL_FIO_PDM_CBK_EVT_READ_RESP,          /*!< Read complete event */
    ADI_EAL_FIO_PDM_CBK_EVT_PERIODIC_RX,            /*!< Periodic Rx received event */
    ADI_EAL_FIO_PDM_CBK_EVT_WRITE_RESP,         /**< Write complete event */
    ADI_EAL_FIO_PDM_CBK_EVT_INTF_STATUS_EVT,    /**< FIO PDM interface status event */
    ADI_EAL_FIO_PDM_CBK_EVT_FIFO_STATUS_EVT,    /**< FIFO status event */
    ADI_EAL_FIO_PDM_CBK_EVT_FIFO_RX_FULL,       /**< FIFO RX full event */
    ADI_EAL_FIO_PDM_CBK_EVT_ERR,                /**< Error event */
} ADI_EAL_FIO_PDM_CBK_EVT;

/** EAL FIO PDM Clock modes */
typedef enum ENUM_PDM_CLK_MODE
{
    ENUM_PDM_CLK_MODE_FIOCLK,
    ENUM_PDM_CLK_MODE_EXTCLK
} ENUM_PDM_CLK_MODE;
/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

/** The EAL FIO PDM handle */
typedef void *ADI_EAL_FIO_PDM_HANDLE;

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** The EAL FIO PDM callback prototype */
typedef void (*ADI_EAL_FIO_PDM_CALLBACK_FUNC)(
    uint32_t nInstNum,                          /* Instance number */
    ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle,       /* FIO PDM handle */
    ADI_EAL_FIO_PDM_CBK_EVT eCbkEventType,      /* Callback event type */
    const void *pArg);                          /* Data corresponding to the event (This should be
                                                   typecasted to ADI_EAL_FIO_PDM_CBK_EVT_<EVT>_DATA*) */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/** EAL FIO PDM module configuration structure */
typedef struct ADI_EAL_FIO_PDM_CFG
{
    uint32_t            nInstNum;               /**< Instance number */
    uint32_t            nRemoteNum;             /**< Remote number */
    uint32_t            nIntfNum;               /**< Interface number */
    uint16_t            nFioPdmCfgQDepth;       /**< Configure the FIO PDM transaction queue size */
    uint8_t             *pEalFioPdmMemory;      /**< Pointer to allocated memory.
                                                     NOTE: Size of the allocation should be:
                                                     ADI_EAL_FIO_PDM_MEM_SIZE + nFioPdmCfgQDepth * ADI_EAL_FIO_PDM_CFG_SIZE */
    uint32_t            nEalFioPdmMemorySize;   /**< Size of memory (in bytes) allocated in pEalFioPdmMemory */
    bool                bUseExternalClock;      /**< Use external clock for PDM interface */
} ADI_EAL_FIO_PDM_CFG;

/*! @brief Configuration for EAL FIO PDM multicast group */
typedef struct ADI_EAL_FIO_PDM_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_FIO_PDM_HANDLE *pahFioPdmHandles;  /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;      /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;    /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;  /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nFioPdmMcastMemSize; /*!< Size of the FIO PDM multicast group data */
    void               *pFioPdmMcastMem;   /*!< Pointer to the FIO PDM multicast group memory */
} ADI_EAL_FIO_PDM_MCAST_CFG;

/*! @brief EAL FIO PDM I/O DATA */
typedef struct ADI_EAL_FIO_PDM_IO_DATA
{
    uint8_t *pRxData;           /*!< Pointer to store Rx data */
    uint16_t nReadLength;        /*!< The read length */
} ADI_EAL_FIO_PDM_IO_DATA;

/** Instruction RAM structure  */
typedef ADI_EAL_FIO_INST_RAM_DATA ADI_EAL_FIO_PDM_INST_RAM_DATA;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/** Data for #ADI_EAL_FIO_PDM_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_FIO_PDM_CBK_EVT_REG_READ_RESP_DATA;

/** Data for #ADI_EAL_FIO_PDM_CBK_EVT_WRITE_RESP event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_PDM_CBK_EVT_WRITE_RESP_DATA;

/** Data for #ADI_EAL_FIO_PDM_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_FIO_STATUS_DATA ADI_EAL_FIO_PDM_CBK_EVT_INTF_STATUS_EVT_DATA;

/** Data for #ADI_EAL_FIO_PDM_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_FIO_PDM_CBK_EVT_FIFO_STATUS_EVT_DATA;

/** Data for #ADI_EAL_FIO_PDM_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_FIO_PDM_CBK_EVT_ERR_DATA;

/* ~~~~~~~~~~~~~~~~~~~~~~ Internal ~~~~~~~~~~~~~~~~~~~~~~ */

/** Internal Data structure for each EAL FIO PDM instance */
typedef struct EAL_FIO_PDM_INST_DATA
{
    /* ~~~~~~~~~~~ FIO PDM instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;                    /**< Stores the instance number, remote number and interface number */
    uint32_t           *pnIfRegVlanTag;         /**< Pointer to VLAN tag for interface register access */
    uint32_t           *pnFifoRegVlanTag;       /**< Pointer to VLAN tag for FIFO register access */
    uint32_t           *pnFifoWriteVlanTag;     /**< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ FIO PDM state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;             /**< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;                  /**< Indicates in which mode this instance is being used in.
                                                     Valid options are:
                                                     - #ADI_EAL_USAGE_MODE_NORMAL
                                                     - #ADI_EAL_USAGE_MODE_PERIODIC */
    ADI_EAL_FIO_PDM_CALLBACK_FUNC pfunCbk;      /**< Pointer to store the address of callback function */
    ENUM_PDM_CLK_MODE   ePdmClkMode;            /*!< Clock mode for PDM */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn;       /**< Is read time tag field enabled in the CONTROL register? */
    uint16_t            nFioPdmCfgQDepth;       /**< Indicates the size of config queue */
    volatile uint16_t   nFioPdmQWrIdx;          /**< Indicates the Write index of transaction config queue */
    volatile uint16_t   nFioPdmQRdIdx;          /**< Indicates the Read index of transaction config queue */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoCfgs; /**< FIO PDM transaction config queue */

    /* ~~~~~~~~~~~ Mutex for the FIO PDM instance ~~~~~~~~~~~ */
    uint8_t             anFioPdmMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /**< Buffer for FIO PDM mutex */
    PRJ_OSAL_MUTEX_CFG  oFioPdmMutexCfg;        /**< FIO PDM mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hFioPdmMutex;         /**< Handle for the FIO PDM mutex */
} EAL_FIO_PDM_INST_DATA;

/*! Internal data structure behind the EAL FIO PDM handle */
typedef struct EAL_FIO_PDM_DATA
{
    bool                bIsMulticast;   /*!< Is this a multicast group or a unicast instance? */
    bool                bInUse;         /*!< Is this handle opened? */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    uint32_t            nIfRegVlanTag;      /*!< VLAN tag for interface register access of this mcast group */
    uint32_t            nFifoRegVlanTag;    /*!< VLAN tag for FIFO register access of this mcast group */
    uint32_t            nFifoWriteVlanTag;  /*!< VLAN tag for FIFO write transactions of this mcast group */
    uint16_t            nNumInst;       /*!< Number of unicast instances in this group */
    EAL_FIO_PDM_INST_DATA   **apoInstData;  /*!< Array of unicast instance data pointers */
} EAL_FIO_PDM_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_fioPdmOpen(ADI_EAL_FIO_PDM_CFG *poFioPdmCfg, ADI_EAL_FIO_PDM_HANDLE *phFioPdmHandle);
extern ADI_EAL_STATUS adi_eal_fioPdmClose(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle);
extern ADI_EAL_STATUS adi_eal_fioPdmRegisterCallback(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle,
        ADI_EAL_FIO_PDM_CALLBACK_FUNC pfFioPdmCallback);
extern ADI_EAL_STATUS adi_eal_fioPdmSetupPeriodicWriteRead(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle,
        ADI_EAL_FIO_PDM_IO_DATA *poFioPdmData);
extern ADI_EAL_STATUS adi_eal_fioPdmReadStatus(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle);
extern ADI_EAL_STATUS adi_eal_fioPdmConfigureInstructionRAM(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle,
        ADI_EAL_FIO_PDM_INST_RAM_DATA *poFioInstructionRamData);
extern ADI_EAL_STATUS adi_eal_fioPdmCreateMcastGroup(ADI_EAL_FIO_PDM_MCAST_CFG *poFioPdmMcastCfg,
        ADI_EAL_FIO_PDM_HANDLE *phFioPdmMcastHandle);
extern ADI_EAL_STATUS adi_eal_fioPdmSetClockPeriod(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, uint32_t nClockPeriod);
extern void adi_eal_fioPdmSetDownstreamVlan(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_fioPdmSetUpstreamVlan(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_fioPdmSendRegTransaction(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_fioPdmResetQueue(ADI_EAL_FIO_PDM_HANDLE hFioPdmHandle);
/*============================ CALLBACKS ============================*/
extern void FioPdmInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                               void *pCbParam);


#endif /* ADI_E2B_IFACE_FIO_PDM_ENABLED */
#endif /* ADI_EAL_FIOPDM_H */

/** @} */

/**
 * EOF: www.analog.com
 */
