/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_adc.h
 * \brief:The header file for ADC functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_ADC_H
#define ADI_EAL_ADC_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"        /* The EAL header */
#include "e2bcore_adc.h"    /* e2bcore ADC header */

#ifdef ADI_E2B_IFACE_ADC_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each ADC unicast instance */
#define ADI_EAL_ADC_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_ADC_INST_DATA) + sizeof(EAL_ADC_INST_DATA*) + sizeof(EAL_ADC_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_ADC_TXN_CONFIG) + sizeof(ADI_E2BCORE_ADC_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an ADC multicast group */
#define ADI_EAL_ADC_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_ADC_DATA) + (nNumInst)*sizeof(EAL_ADC_INST_DATA*))

/* Channel indices */
#define SA_IF0_CH_IDX            (0U)        /*!< SA_IF0 ADC channel index */
#define SA_IF1_CH_IDX            (1U)        /*!< SA_IF1 ADC channel index */
#define SA_IF2_CH_IDX            (2U)        /*!< SA_IF2 ADC channel index */
#define SA_IF3_CH_IDX            (3U)        /*!< SA_IF3 ADC channel index */
#define SA_IF4_CH_IDX            (4U)        /*!< SA_IF4 ADC channel index */
#define SA_IF5_CH_IDX            (5U)        /*!< SA_IF5 ADC channel index */
#define VMON_CH_IDX              (6U)        /*!< VMON ADC channel index */
#define DVDDIO_CH_IDX            (7U)        /*!< DVDDIO ADC channel index */
#define LVDD_CH_IDX              (8U)        /*!< LVDD ADC channel index */
#define DVDD_CH_IDX              (9U)        /*!< DVDD ADC channel index */
#define AVDD_CH_IDX              (10U)       /*!< AVDD ADC channel index */
#define TEMPERATURE_CH_IDX       (13U)       /*!< Temperature sensor channel index */
#define CAL0_CH_IDX              (14U)       /*!< Calibration voltage 0 ADC channel index */
#define CAL1_CH_IDX              (15U)       /*!< Calibration voltage 1 ADC channel index */

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL ADC callback event type
 */
typedef enum ADI_EAL_ADC_CBK_EVT
{
    ADI_EAL_ADC_CBK_EVT_ADC_WRITE_RESP,         /*!< Write complete event */
    ADI_EAL_ADC_CBK_EVT_ADC_SAMPLE,             /*!< ADC sample received event */
    ADI_EAL_ADC_CBK_EVT_ERR,                    /*!< Error event */
    ADI_EAL_ADC_CBK_EVT_REG_READ_RESP,          /*!< ADC register read response event */
    ADI_EAL_ADC_CBK_EVT_INTF_STATUS_EVT,        /*!< ADC interface status event */
    ADI_EAL_ADC_CBK_EVT_FIFO_STATUS_EVT,        /*!< FIFO status event */
    ADI_EAL_ADC_CBK_EVT_MAILBOX_ACK,            /*!< Mailbox ACK event */
} ADI_EAL_ADC_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_ADC_HANDLE;               /*!< The ADC EAL handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void (*ADI_EAL_ADC_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_ADC_HANDLE hAdcHandle,
        ADI_EAL_ADC_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL ADC callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Configuration for EAL ADC instance
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL ADC APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_ADC_CFG
{
    uint32_t            nInstNum;       /*!< Indicates the instance number */
    uint32_t            nRemoteNum;     /*!< Indicates the remote number */
    uint32_t            nIntfNum;       /*!< Indicates the interface number */
    uint32_t            nEalAdcMemorySize; /*!< ADC instance memory size */
    uint16_t            nAdcCfgQDepth;  /*!< Size of the transaction config queue */
    uint8_t            *pEalAdcMemory; /*!< ADC instance memory to be supplied by the application (must be 4-byte or 8-byte aligned) */
} ADI_EAL_ADC_CFG;

/*! @brief Configuration for EAL ADC multicast group */
typedef struct ADI_EAL_ADC_MCAST_CFG
{
    uint16_t            nNumInst;           /*!< Number of unicast instances in the multicast group */
    ADI_EAL_ADC_HANDLE *pahAdcHandles;      /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;         /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;         /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;    /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;  /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;/*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nAdcMcastMemSize;   /*!< Size of the ADC multicast group data */
    void                *pAdcMcastMem;      /*!< Pointer to the ADC multicast group memory */
} ADI_EAL_ADC_MCAST_CFG;

/*! @brief ADC interface status data */
typedef struct ADI_EAL_ADC_STATUS_DATA
{
    uint16_t            nNumSamples;    /*!< Number of ADC samples captured */
} ADI_EAL_ADC_STATUS_DATA;

/*! @brief ADC Look-Up-Table based sampling configuration */
typedef struct ADI_EAL_ADC_LUT_CFG
{
    uint32_t            nClkPeriod;     /*!< Time between 2 consecutive samples (in nanoseconds) */
    uint32_t            nCyclePeriod;   /*!< Time between 2 cycles of sampling (in nanoseconds).
                                             Each cycle could have 1 or more samples */
    uint8_t             nLutSize;       /*!< Sets the number of channel indices to sample from */
    uint8_t             *panChannelIdx; /*!< Array of channel indices to sample from */
    uint16_t            nChannelMask;   /*!< Sets whether a given channel is masked
                                             (upstream samples won't have data from these channels) */
} ADI_EAL_ADC_LUT_CFG;

/*! @brief ADC round robin based sampling configuration */
typedef struct ADI_EAL_ADC_ROUND_ROBIN_CFG
{
    uint32_t            nClkPeriod;     /*!< Time between 2 consecutive samples (in nanoseconds) */
    uint32_t            nCyclePeriod;   /*!< Time between 2 cycles of sampling (in nanoseconds).
                                             Each cycle could have 1 or more samples */
    uint16_t            nChannelEn;     /*!< Set bits of channels to be sampled */
    uint16_t            nChannelMask;   /*!< Sets whether a given channel is masked
                                             (upstream samples won't have data from these channels) */
    uint8_t             nNumSamples;    /*!< Set the number of times a channel is to be sampled every cycle */
} ADI_EAL_ADC_ROUND_ROBIN_CFG;

/*! @brief Data structure for each EAL ADC instance */
typedef struct EAL_ADC_INST_DATA
{
    /* ~~~~~~~~~~~ ADC instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;            /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;          /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;        /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;      /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ ADC state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;     /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;          /*!< Indicates in which mode eal is used */
    ADI_EAL_ADC_CALLBACK_FUNC pfunCbk;  /*!< Pointer to store the address of callback function */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn; /*!< Indicates if Read time tag enabled */
    uint16_t            nAdcCfgQDepth;  /*!< Size of the transaction config queue */
    volatile uint16_t   nAdcQWrIdx;     /*!< Transaction config queue write index */
    volatile uint16_t   nAdcQRdIdx;     /*!< Transaction config queue read index */
    ADI_E2BCORE_ADC_TXN_CONFIG *paoCfgs; /*!< ADC transaction config queue */
    ADI_E2BCORE_ADC_TXN_CONFIG oRxCfg;  /*!< ADC RX sample config */
    ADI_E2BCORE_ADC_TXN_RX_PAYLOAD oRxPayload; /*!< ADC RX sample payload */

    /* ~~~~~~~~~~~ Mutex for the ADC instance ~~~~~~~~~~~ */
    uint8_t anAdcMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];      /*!< Buffer for ADC mutex */
    PRJ_OSAL_MUTEX_CFG oAdcMutexCfg;               /*!< ADC mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hAdcMutex;               /*!< Handle for the ADC mutex */
} EAL_ADC_INST_DATA;

/*! Internal data structure behind the EAL ADC handle */
typedef struct EAL_ADC_DATA
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
    EAL_ADC_INST_DATA   **apoInstData;      /*!< Array of unicast instance data pointers */
} EAL_ADC_DATA;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_ADC_CBK_EVT_ADC_WRITE_RESP event */
typedef ADI_E2BCORE_ADC_TXN_CONFIG ADI_EAL_ADC_CBK_EVT_ADC_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_ADC_CBK_EVT_ADC_SAMPLE event */
typedef ADI_E2BCORE_ADC_TXN_CONFIG ADI_EAL_ADC_CBK_EVT_ADC_SAMPLE_DATA;

/*! @brief Data for #ADI_EAL_ADC_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_ADC_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_ADC_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_ADC_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_ADC_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_ADC_STATUS_DATA ADI_EAL_ADC_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_ADC_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_ADC_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_ADC_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_ADC_CBK_EVT_MAILBOX_ACK_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_adcOpen(ADI_EAL_ADC_CFG *poAdcCfg, ADI_EAL_ADC_HANDLE *phAdcHandle);
extern ADI_EAL_STATUS adi_eal_adcClose(ADI_EAL_ADC_HANDLE hAdcHandle);
extern ADI_EAL_STATUS adi_eal_adcRegisterCallback(ADI_EAL_ADC_HANDLE hAdcHandle,
        ADI_EAL_ADC_CALLBACK_FUNC pfAdcCallback);
extern ADI_EAL_STATUS adi_eal_adcCreateMcastGroup(ADI_EAL_ADC_MCAST_CFG *poAdcMcastCfg,
        ADI_EAL_ADC_HANDLE *phAdcMcastHandle);
extern ADI_EAL_STATUS adi_eal_adcConfigureRoundRobinSampling(ADI_EAL_ADC_HANDLE hAdcHandle,
        ADI_EAL_ADC_ROUND_ROBIN_CFG *poCfg, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_adcConfigureLUTSampling(ADI_EAL_ADC_HANDLE hAdcHandle,
        ADI_EAL_ADC_LUT_CFG *poCfg, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_adcStartSampling(ADI_EAL_ADC_HANDLE hAdcHandle, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_adcStartSamplingDelayed(ADI_EAL_ADC_HANDLE hAdcHandle, uint32_t nDelayTime, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_adcStartSamplingAtTime(ADI_EAL_ADC_HANDLE hAdcHandle, uint64_t nStartTime, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_adcStopSampling(ADI_EAL_ADC_HANDLE hAdcHandle);
extern ADI_EAL_STATUS adi_eal_adcResetQueue(ADI_EAL_ADC_HANDLE hAdcHandle);
extern ADI_EAL_STATUS adi_eal_adcReadStatus(ADI_EAL_ADC_HANDLE hAdcHandle);
extern ADI_EAL_STATUS adi_eal_adcSendRegTransaction(ADI_EAL_ADC_HANDLE hAdcHandle, bool bIsFifoRegTransaction,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern void adi_eal_adcSetDownstreamVlan(ADI_EAL_ADC_HANDLE hAdcHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_adcSetUpstreamVlan(ADI_EAL_ADC_HANDLE hAdcHandle, ADI_VLAN_CFG *poVlantag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_adcSetMailboxMode(ADI_EAL_ADC_HANDLE hAdcHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_adcResyncSeqNum(ADI_EAL_ADC_HANDLE hAdcHandle);
/*============================ CALLBACKS ============================*/

extern void AdcInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                            void *pCbParam);


#endif /* ADI_E2B_IFACE_ADC_ENABLED */
#endif /* ADI_EAL_ADC_H */

/** @} */

/**
 * EOF: www.analog.com
 */
