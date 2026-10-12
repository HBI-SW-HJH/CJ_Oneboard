/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: adi_eal_fioPwm.h
 * @brief:The header file for FIO PWM functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_FIOPWM_H
#define ADI_EAL_FIOPWM_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"                    /* The EAL header */
#include "adi_eal_smc.h"                /* EAL SMC header */
#include "e2bcore_flexibleIO.h"         /* Flexible IO e2bcore header */

#ifdef ADI_E2B_IFACE_FIO_PWM_ENABLED


/*================================= DEFINES =================================*/
/** Helper macro to compute the memory size required for each FIO_PWM unicast instance */
#define ADI_EAL_FIO_PWM_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_FIO_PWM_INST_DATA) + sizeof(EAL_FIO_PWM_INST_DATA*) + sizeof(EAL_FIO_PWM_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG) + sizeof(ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG*)))
#define ADI_EAL_FIO_PWM_FRE_DUTY_CYCLE_DATA_LEN              (10U) /*!< FIO-PWM Frequency duty cycle data length in bytes */

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/** EAL FIO PWM callback event type */
typedef enum ADI_EAL_FIO_PWM_CBK_EVT
{
    ADI_EAL_FIO_PWM_CBK_EVT_REG_READ_RESP,      /**< Reg Read complete event */
    ADI_EAL_FIO_PWM_CBK_EVT_WRITE_RESP,         /**< Write complete event */
    ADI_EAL_FIO_PWM_CBK_EVT_INTF_STATUS_EVT,    /**< FIO PWM interface status event */
    ADI_EAL_FIO_PWM_CBK_EVT_FIFO_STATUS_EVT,    /**< FIFO status event */
    ADI_EAL_FIO_PWM_CBK_EVT_ERR,                /**< Error event */
    ADI_EAL_FIO_PWM_CBK_EVT_MAILBOX_ACK,        /**< Mailbox ACK event */
} ADI_EAL_FIO_PWM_CBK_EVT;

/** EAL FIO PWM operation modes */
typedef enum ADI_EAL_FIO_PWM_OP_MODE
{
    ADI_EAL_FIO_PWM_OP_MODE_ONE_SHOT,           /**< Only one set of PWM updates is done each time.
                                                     To send the next set of PWM updates,
                                                     adi_eal_fioPwmUpdateFreqDuty or
                                                     adi_eal_fioPwmUpdateFreqDutyBuffered needs to be invoked */
    ADI_EAL_FIO_PWM_OP_MODE_CONTINUOUS,         /**< PWM updates will be continuously sent.
                                                     To change the frequency / duty cycle,
                                                     adi_eal_fioPwmUpdateFreqDuty or
                                                     adi_eal_fioPwmUpdateFreqDutyBuffered needs to be invoked */
} ADI_EAL_FIO_PWM_OP_MODE;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

/** The EAL FIO PWM handle */
typedef void *ADI_EAL_FIO_PWM_HANDLE;

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

/** The EAL FIO PWM callback prototype */
typedef void (*ADI_EAL_FIO_PWM_CALLBACK_FUNC)(
    uint32_t nInstNum,                          /* Instance number */
    ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,       /* FIO PWM handle */
    ADI_EAL_FIO_PWM_CBK_EVT eCbkEventType,      /* Callback event type */
    const void *pArg);                          /* Data corresponding to the event (This should be
                                                   typecasted to ADI_EAL_FIO_PWM_CBK_EVT_<EVT>_DATA*) */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL FIO PWM module configuration structure
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL FIO PWM APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_FIO_PWM_CFG
{
    uint32_t            nInstNum;               /**< Instance number */
    uint32_t            nRemoteNum;             /**< Remote number */
    uint32_t            nIntfNum;               /**< Interface number */
    uint16_t            nFioPwmCfgQDepth;       /**< Configure the FIO PWM transaction queue size */
    uint8_t             *pEalFioPwmMemory;      /**< Pointer to allocated memory (must be 4-byte or 8-byte aligned).
                                                     NOTE: Size of the allocation should be:
                                                     ADI_EAL_FIO_PWM_MEM_SIZE + nFioPwmCfgQDepth * ADI_EAL_FIO_PWM_CFG_SIZE */
    uint32_t            nEalFioPwmMemorySize;   /**< Size of memory (in bytes) allocated in pEalFioPwmMemory */
    ADI_EAL_FIO_PWM_OP_MODE eFioPwmOpMode;      /**< Configure the operation mode of FIO PWM */
    uint8_t             *pFioTxnPayloadMem;     /**< Pointer to the transaction payload memory */
    uint32_t            nFioTxnPayloadMemSize;  /**< Size of the transaction payload memory (Should be a multiple of 10) */
    uint8_t             nPwmSmcRegion1;            /**< FIO-PWM SMC region-1 number */
    uint8_t             nPwmSmcRegion2;            /**< FIO-PWM SMC region-2 number */
} ADI_EAL_FIO_PWM_CFG;

/** EAL FIO PWM update rate configuration structure */
typedef struct ADI_EAL_FIO_PWM_UPDATE_RATE_CFG
{
    uint8_t             nE2bId;                 /**< Identifier for PWM update frame.
                                                     Set to 0xFF if nE2bId shouldn't be inserted */
    uint16_t            nNumPackedTxns;         /**< Number of updates to be sent in a single frame */
    double              fFifoLowThreshold;      /**< Lower threshold of FIFO (in %) */
    double              fFifoHighThreshold;     /**< Upper threshold of FIFO (in %) */
    uint32_t            nPwmUpdateInterval;     /**< Update Interval (in ns) */
} ADI_EAL_FIO_PWM_UPDATE_RATE_CFG;

/** EAL FIO PWM output waveform configuration */
typedef struct ADI_EAL_FIO_PWM_OUTPUT_CFG
{
    double              fFrequency;             /**< Frequency of each of the output PWM waveforms (in Hz) */
    double              fDutyCycle1;            /**< Duty cycle of the 1st output PWM waveform (in %) */
    double              fDutyCycle2;            /**< Duty cycle of the 2nd output PWM waveform (in %) */
    double              fDutyCycle3;            /**< Duty cycle of the 3rd output PWM waveform (in %) */
} ADI_EAL_FIO_PWM_OUTPUT_CFG;

/** Instruction RAM structure  */
typedef ADI_EAL_FIO_INST_RAM_DATA ADI_EAL_FIO_PWM_INST_RAM_DATA;
/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/** Data for #ADI_EAL_FIO_PWM_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_FIO_PWM_CBK_EVT_REG_READ_RESP_DATA;

/** Data for #ADI_EAL_FIO_PWM_CBK_EVT_WRITE_RESP event */
typedef ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG ADI_EAL_FIO_PWM_CBK_EVT_WRITE_RESP_DATA;

/** Data for #ADI_EAL_FIO_PWM_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_FIO_STATUS_DATA ADI_EAL_FIO_PWM_CBK_EVT_INTF_STATUS_EVT_DATA;

/** Data for #ADI_EAL_FIO_PWM_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_FIO_PWM_CBK_EVT_FIFO_STATUS_EVT_DATA;

/** Data for #ADI_EAL_FIO_PWM_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_FIO_PWM_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_FIO_PWM_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_FIO_PWM_CBK_EVT_MAILBOX_ACK_DATA;

/* ~~~~~~~~~~~~~~~~~~~~~~ Internal ~~~~~~~~~~~~~~~~~~~~~~ */

/** Internal Data structure for each EAL FIO PWM instance */
typedef struct EAL_FIO_PWM_INST_DATA
{
    /* ~~~~~~~~~~~ FIO PWM instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;                    /**< Stores the instance number, remote number and interface number */
    uint32_t           *pnIfRegVlanTag;         /**< Pointer to VLAN tag for interface register access */
    uint32_t           *pnFifoRegVlanTag;       /**< Pointer to VLAN tag for FIFO register access */
    uint32_t           *pnFifoWriteVlanTag;     /**< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ FIO PWM state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;             /**< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;                  /**< Indicates in which mode this instance is being used in.
                                                     Valid options are:
                                                     - #ADI_EAL_USAGE_MODE_NORMAL
                                                     - #ADI_EAL_USAGE_MODE_PERIODIC */
    ADI_EAL_FIO_PWM_CALLBACK_FUNC pfunCbk;      /**< Pointer to store the address of callback function */
    ADI_EAL_FIO_PWM_OP_MODE eFioPwmOpMode;      /**< Operation mode of FIO PWM */
    volatile bool       bIsConfigured;          /**< Are PWM updates configured? */
    volatile bool       bRunPwm;                /**< Should PWM updates be sent? */
    volatile bool       bLowIRQ;                /**< Low threshold of FIFO reached */
    volatile bool       bHighIRQ;               /**< High threshold of FIFO reached */
    uint8_t             nState;                 /**< State of the interface */
    uint64_t            nSendTime;              /**< PWM update send time */

    /* ~~~~~~~~~~~ FIO transaction payload info ~~~~~~~~~~~ */
    uint32_t            nPwmUpdateInterval;     /**< Interval (in ns) between update frames */
    uint8_t             *pFioTxnPayloadMem;     /**< Pointer to the transaction payload memory */
    uint32_t            nFioTxnPayloadMemSize;  /**< Size of the transaction payload memory */
    uint16_t            nNumPackedTxns;         /**< Number of updates to be sent in a single frame */
    uint8_t             nE2bId;                 /**< Identifier for PWM update frame */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn;       /**< Is read time tag field enabled in the CONTROL register? */
    uint16_t            nFioPwmCfgQDepth;       /**< Indicates the size of config queue */
    volatile uint16_t   nFioPwmQWrIdx;          /**< Indicates the Write index of transaction config queue */
    volatile uint16_t   nFioPwmQRdIdx;          /**< Indicates the Read index of transaction config queue */
    ADI_E2BCORE_FLEXIBLE_IO_TXN_CONFIG *paoCfgs; /**< FIO PWM transaction config queue */

    /* ~~~~~~~~~~~ Mutex for the FIO PWM instance ~~~~~~~~~~~ */
    uint8_t             anFioPwmMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE]; /**< Buffer for FIO PWM mutex */
    PRJ_OSAL_MUTEX_CFG  oFioPwmMutexCfg;        /**< FIO PWM mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hFioPwmMutex;         /**< Handle for the FIO PWM mutex */

    /* ~~~~~~~~~~~ SMC structure for the FIO PWM instance ~~~~~~~~~~~ */
    uint8_t nPwmSmcRegionInUse;                 /**< Indicates which FIO-PWM SMC region is in use */
    uint8_t nPwmSmcRegion1;                     /**< Indicates FIO-PWM SMC region-1 number */
    uint8_t nPwmSmcRegion2;                     /**< Indicates FIO-PWM SMC region-2 number */
} EAL_FIO_PWM_INST_DATA;

/*! Internal data structure behind the EAL FIO PWM handle */
typedef struct EAL_FIO_PWM_DATA
{
    bool                bIsMulticast;   /*!< Is this a multicast group or a unicast instance? */
    bool                bInUse;         /*!< Is this handle opened? */
    bool                bMailboxMode;   /*!< Flag to indicate if mailbox mode is enabled */
    bool                bResyncPending; /*!< Flag to indicate if sync mode is enabled for resync */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    uint16_t            nNumInst;       /*!< Number of unicast instances in this group */
    EAL_FIO_PWM_INST_DATA   **apoInstData;  /*!< Array of unicast instance data pointers */
} EAL_FIO_PWM_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_fioPwmOpen(ADI_EAL_FIO_PWM_CFG *poFioPwmCfg, ADI_EAL_FIO_PWM_HANDLE *phFioPwmHandle);
extern ADI_EAL_STATUS adi_eal_fioPwmClose(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle);
extern ADI_EAL_STATUS adi_eal_fioPwmRegisterCallback(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_CALLBACK_FUNC pfFioPwmCallback);
extern ADI_EAL_STATUS adi_eal_fioPwmConfigureInstructionRAM(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_INST_RAM_DATA *poFioInstructionRamData);
extern ADI_EAL_STATUS adi_eal_fioPwmConfigureUpdateRate(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_UPDATE_RATE_CFG *poUpdateCfg);
extern ADI_EAL_STATUS adi_eal_fioPwmUpdateFreqDuty(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_OUTPUT_CFG *poPwmConfig);
extern ADI_EAL_STATUS adi_eal_fioPwmUpdateFreqDutySmcRegion(ADI_SMC_REGION_CONFIG anRegions[],
        ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_OUTPUT_CFG *poPwmConfig);
extern ADI_EAL_STATUS adi_eal_fioPwmUpdateFreqDutyBuffered(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle,
        ADI_EAL_FIO_PWM_OUTPUT_CFG *paoPwmConfig);
extern ADI_EAL_STATUS adi_eal_fioPwmStart(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle);
extern ADI_EAL_STATUS adi_eal_fioPwmStop(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle);
extern void adi_eal_fioPwmRun(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle);
extern ADI_EAL_STATUS adi_eal_fioPwmReadStatus(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle);
extern ADI_EAL_STATUS adi_eal_fioPwmSendRegTransaction(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS adi_eal_fioPwmResetQueue(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle);
extern void adi_eal_fioPwmSetDownstreamVlan(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_fioPwmSetUpstreamVlan(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, ADI_VLAN_CFG *poVlantag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_fioPwmSetMailboxMode(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_fioPwmResyncSeqNum(ADI_EAL_FIO_PWM_HANDLE hFioPwmHandle);
/*============================ CALLBACKS ============================*/
extern void FioPwmInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                               void *pCbParam);


#endif /* ADI_E2B_IFACE_FIO_PWM_ENABLED */
#endif /* ADI_EAL_FIOPWM_H */

/** @} */

/**
 * EOF: www.analog.com
 */
