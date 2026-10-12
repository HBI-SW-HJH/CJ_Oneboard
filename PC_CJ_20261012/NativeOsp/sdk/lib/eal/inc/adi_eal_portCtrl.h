/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_portCtrl.h
 * \brief:The header file for port controller functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_PORTCTRL_H
#define ADI_EAL_PORTCTRL_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"            /* The EAL header */
#include "e2bcore_portCtrl.h"   /* e2bcore Port controller header */

#ifdef ADI_E2B_IFACE_PORT_ENABLED


/*================================= DEFINES =================================*/
/*! Helper macro to compute the memory size required for each PORTCTRL unicast instance */
#define ADI_EAL_PORTCTRL_UNICAST_INST_MEM_SIZE(nQueueDepth) \
(sizeof(EAL_PORTCTRL_INST_DATA) + sizeof(EAL_PORTCTRL_INST_DATA*) + sizeof(EAL_PORTCTRL_DATA) + (nQueueDepth) * (sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG) + sizeof(ADI_E2BCORE_PORT_CTRL_TXN_CONFIG*)))

/*! Helper macro to compute the memory size required for an PORTCTRL multicast group */
#define ADI_EAL_PORTCTRL_MCAST_GROUP_MEM_SIZE(nNumInst) \
(sizeof(EAL_PORTCTRL_DATA) + (nNumInst)*sizeof(EAL_PORTCTRL_INST_DATA*))

/*================================ DATA TYPES ===============================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Enums ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief EAL port controller component callback event types
 */
typedef enum ADI_EAL_PORTCTRL_CBK_EVT
{
    ADI_EAL_PORTCTRL_CBK_EVT_GPIO_READ_RESP,                  /*!< GPIO read response event */
    ADI_EAL_PORTCTRL_CBK_EVT_PORTCTRL_WRITE_RESP,             /*!< Write complete event */
    ADI_EAL_PORTCTRL_CBK_EVT_INPUT_MONITOR,                   /*!< Input monitor event */
    ADI_EAL_PORTCTRL_CBK_EVT_TACHOMETER,                      /*!< Tachometer data event */
    ADI_EAL_PORTCTRL_CBK_EVT_ERR,                             /*!< Error event */
    ADI_EAL_PORTCTRL_CBK_EVT_REG_READ_RESP,                   /*!< PortCtrl register read response event */
    ADI_EAL_PORTCTRL_CBK_EVT_INTF_STATUS_EVT,                 /*!< PortCtrl interface status event */
    ADI_EAL_PORTCTRL_CBK_EVT_FIFO_STATUS_EVT,                 /*!< FIFO status event */
    ADI_EAL_PORTCTRL_CBK_EVT_MAILBOX_ACK,                     /*!< Mailbox ACK event */
    ADI_EAL_PORTCTRL_CBK_EVT_READ_TIME_TAG,                   /*!< Read Time Tag event */
    ADI_EAL_PORTCTRL_CBK_EVT_READ_TIME_TAG_ECCO,              /*!< Read Time Tag ECCO event */
} ADI_EAL_PORTCTRL_CBK_EVT;

/* ~~~~~~~~~~~~~~~~~~~~~~ Handle ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void *ADI_EAL_PORTCTRL_HANDLE;                /*!< The PORTCTRL EAL handle */

/* ~~~~~~~~~~~~~~~~~~~~~~ Callback type ~~~~~~~~~~~~~~~~~~~~~~ */

typedef void (*ADI_EAL_PORTCTRL_CALLBACK_FUNC)(uint32_t nInstNum, ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_CBK_EVT eCbkEventType,
        const void *pArg);                              /*!< The EAL PORTCTRL callback prototype */

/* ~~~~~~~~~~~~~~~~~~~~~~ Structs ~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief Configuration for EAL Port controller instance
 *
 * @note Buffer Alignment Requirements:
 *       All buffers passed to EAL Port Control APIs must be aligned to 4-byte or 8-byte boundaries
 *       for proper DMA and memory access operations. Use ADI_MEM_ALIGN4 or __attribute__((aligned(8)))
 *       when declaring buffers in application code.
 */
typedef struct ADI_EAL_PORTCTRL_CFG
{
    uint32_t            nInstNum;               /*!< Indicates the instance number */
    uint32_t            nRemoteNum;             /*!< Indicates the remote number */
    uint32_t            nIntfNum;               /*!< Indicates the interface number */
    uint32_t            nEalPortCtrlMemorySize; /*!< Port ctrl instance memory size */
    uint16_t            nPortCtrlCfgQDepth;     /*!< Size of the transaction config queue */
    uint8_t            *pEalPortCtrlMemory;     /*!< Port ctrl instance memory to be supplied by the application (must be 4-byte or 8-byte aligned) */
} ADI_EAL_PORTCTRL_CFG;

/*! @brief Configuration for EAL PORTCTRL multicast group */
typedef struct ADI_EAL_PORTCTRL_MCAST_CFG
{
    uint16_t            nNumInst;       /*!< Number of unicast instances in the multicast group */
    ADI_EAL_PORTCTRL_HANDLE *pahPortCtrlHandles;  /*!< Pointer to the array of unicast instance handles */
    uint32_t            nIntfTopic;     /*!< Topic for accessing interface registers of this mcast group */
    uint32_t            nFifoTopic;     /*!< Topic for accessing FIFO of this mcast group */
    ADI_VLAN_CFG        *poIfRegVlanTag;      /*!< Pointer to VLAN tag for interface register access of this mcast group */
    ADI_VLAN_CFG        *poFifoRegVlanTag;    /*!< Pointer to VLAN tag for FIFO register access of this mcast group */
    ADI_VLAN_CFG        *poFifoWriteVlanTag;  /*!< Pointer to VLAN tag for FIFO write transactions of this mcast group */
    uint32_t            nPortCtrlMcastMemSize; /*!< Size of the PORTCTRL multicast group data */
    void               *pPortCtrlMcastMem;   /*!< Pointer to the PORTCTRL multicast group memory */
} ADI_EAL_PORTCTRL_MCAST_CFG;

/*! @brief Port controller interface status data */
typedef struct ADI_EAL_PORTCTRL_STATUS_DATA
{
    uint16_t            nActiveOutput;          /*!< Indicates if each of the PWM signals are toggling */
} ADI_EAL_PORTCTRL_STATUS_DATA;

/*! @brief Structure to be passed to the EAL APIs */
typedef struct ADI_EAL_PORTCTRL_IO_DATA
{
    float               fPwmDutyCycle;          /*!< The pwm duty cycle value (in %) (Only for PWM_START APIs) */
    float               fPulseDelay;            /*!< The pwm pulse delay value (in %) (Only for PWM_START APIs) */
    uint16_t            nPinMask;               /*!< The pins for which to update the pin value (Only for PWM_START APIs) */
    uint16_t            nPinVals;               /*!< The pin value to be written (Only for GPIO_WRITE APIs) */
    float               fPwmFreq;               /*!< The pwm frequency value (in Hz) (Only for PWM_START APIs) */
} ADI_EAL_PORTCTRL_IO_DATA;

/*! @brief Data structure for each EAL Port ctrl instance */
typedef struct EAL_PORTCTRL_INST_DATA
{
    /* ~~~~~~~~~~~ PortCtrl instance info ~~~~~~~~~~~ */
    ADI_E2BCORE_ID      oId;                    /*!< Stores the instance number, remote number and interface number */
    uint32_t  *pnIfRegVlanTag;                  /*!< Pointer to VLAN tag for interface register access */
    uint32_t  *pnFifoRegVlanTag;                /*!< Pointer to VLAN tag for FIFO register access */
    uint32_t  *pnFifoWriteVlanTag;              /*!< Pointer to VLAN tag for FIFO write transactions */

    /* ~~~~~~~~~~~ PortCtrl state info ~~~~~~~~~~~ */
    bool                bIgnoreErr;             /*!< Indicates if errors have to be ignored */
    ADI_EAL_USAGE_MODE  eMode;                  /*!< Indicates in which mode eal is used */
    ADI_EAL_PORTCTRL_CALLBACK_FUNC pfunCbk;     /*!< Pointer to store the address of callback function */
    volatile bool       bWaitingForRx;          /*!< Flag to check if application is waiting for Rx data */
    uint32_t            *pnPinVals;             /*!< Pointer to store the pin values */
    uint32_t            nOutputEnMask;          /*!< Bitmask of ports configured as output (from PORT_OUTPUT_MUX register) */

    /* ~~~~~~~~~~~ Transaction config queue info ~~~~~~~~~~~ */
    bool                bIsReadTimeTagEn;       /*!< Indicates if Read time tag enabled */
    uint16_t            nInputMonitorEnMask;    /*!< Bitmask of pins with Input Monitor enabled (bit N = port N) */
    bool                bIsReadTimeTagEccoEn;   /*!< Indicates if Read Time Tag ECCO is enabled */
    uint16_t            nPortCtrlCfgQDepth;     /*!< Size of the transaction config queue */
    volatile uint16_t   nPortCtrlQWrIdx;        /*!< Transaction config queue write index */
    volatile uint16_t   nPortCtrlQRdIdx;        /*!< Transaction config queue read index */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoCfgs;  /*!< Port ctrl transcation config queue */
    ADI_E2BCORE_PORT_CTRL_TXN_CONFIG oRxCfg;    /*!< Port ctrl RX config */
    ADI_E2BCORE_PORT_CTRL_TXN_PAYLOAD oRxPayload; /*!< Port ctrl RX payload */

    /* ~~~~~~~~~~~ Mutex for the PortCtrl instance ~~~~~~~~~~~ */
    uint8_t anPortCtrlMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];  /*!< Buffer for PortCtrl mutex */
    PRJ_OSAL_MUTEX_CFG oPortCtrlMutexCfg;           /*!< PortCtrl mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hPortCtrlMutex;           /*!< Handle for the PortCtrl mutex */

#if defined(ADI_EAL_CONFIG_SEMAPHORE_WAIT)
    uint8_t anPortCtrlFifoRespSemaphoreBuffer[PRJ_OSAL_SEMAPHORE_MEM_SIZE];   /*!< Buffer for PortCtrl semaphore */
    PRJ_OSAL_SEMAPHORE_CFG oFifoRespSemaphoreCfg; /*!< Semaphore configuration for FIFO response */
    PRJ_OSAL_SEMAPHORE_HANDLE hFifoRespSemaphore; /*!< Semaphore handle for FIFO response */
#endif
} EAL_PORTCTRL_INST_DATA;

/*! Internal data structure behind the EAL PORTCTRL handle */
typedef struct EAL_PORTCTRL_DATA
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
    EAL_PORTCTRL_INST_DATA   **apoInstData;  /*!< Array of unicast instance data pointers */
} EAL_PORTCTRL_DATA;

/* ~~~~~~~~~~~~~~~~~~~~~~ Data for callback events ~~~~~~~~~~~~~~~~~~~~~~ */

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_GPIO_READ_RESP event */
typedef uint32_t ADI_EAL_PORTCTRL_CBK_EVT_GPIO_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_PORTCTRL_WRITE_RESP event */
typedef ADI_E2BCORE_PORT_CTRL_TXN_CONFIG ADI_EAL_PORTCTRL_CBK_EVT_PORTCTRL_WRITE_RESP_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_INPUT_MONITOR event */
typedef ADI_E2BCORE_PORT_CTRL_TXN_CONFIG ADI_EAL_PORTCTRL_CBK_EVT_INPUT_MONITOR_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_TACHOMETER event */
typedef ADI_E2BCORE_PORT_CTRL_TXN_CONFIG ADI_EAL_PORTCTRL_CBK_EVT_TACHOMETER_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_ERR event */
typedef ADI_E2BCORE_ERR ADI_EAL_PORTCTRL_CBK_EVT_ERR_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_REG_READ_RESP event */
typedef ADI_E2BCORE_CBK_REGREADDAT ADI_EAL_PORTCTRL_CBK_EVT_REG_READ_RESP_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_INTF_STATUS_EVT event */
typedef ADI_EAL_PORTCTRL_STATUS_DATA ADI_EAL_PORTCTRL_CBK_EVT_INTF_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_FIFO_STATUS_EVT event */
typedef ADI_EAL_FIFO_STATUS_DATA ADI_EAL_PORTCTRL_CBK_EVT_FIFO_STATUS_EVT_DATA;

/*! @brief Data for #ADI_EAL_PORTCTRL_CBK_EVT_MAILBOX_ACK event */
typedef ADI_EAL_CBK_EVT_MAILBOX_DATA ADI_EAL_PORTCTRL_CBK_EVT_MAILBOX_ACK_DATA;

/*============================ PUBLIC PROTOTYPES ============================*/

extern ADI_EAL_STATUS adi_eal_portCtrlOpen(ADI_EAL_PORTCTRL_CFG *poPortCtrlCfg,
        ADI_EAL_PORTCTRL_HANDLE *phPortCtrlHandle);
extern ADI_EAL_STATUS adi_eal_portCtrlClose(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle);
extern ADI_EAL_STATUS adi_eal_portCtrlRegisterCallback(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_CALLBACK_FUNC pfPortCtrlCallback);
extern ADI_EAL_STATUS adi_eal_portCtrlCreateMcastGroup(ADI_EAL_PORTCTRL_MCAST_CFG *poPortCtrlMcastCfg,
        ADI_EAL_PORTCTRL_HANDLE *phPortCtrlMcastHandle);
extern ADI_EAL_STATUS adi_eal_portCtrlGpioWrite(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlGpioWriteWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlGpioWriteWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlGpioWriteSync(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlGpioRead(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, uint32_t **apnPinVals);
extern ADI_EAL_STATUS adi_eal_portCtrlGpioReadSync(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, uint32_t **apnPinVals);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmStart(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmStartWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmStartWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmStop(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmStopWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmStopWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmDuty(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmDutyWithDelay(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint32_t nTimeDelay, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlPwmDutyWithPresTs(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_EAL_PORTCTRL_IO_DATA *poPortCtrlData, uint64_t nTimestamp, uint8_t nId);
extern ADI_EAL_STATUS adi_eal_portCtrlReadStatus(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle);
extern ADI_EAL_STATUS adi_eal_portCtrlResetQueue(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle);
extern ADI_EAL_STATUS adi_eal_portCtrlCustomWrite(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        ADI_E2BCORE_PORT_CTRL_TXN_CONFIG *paoPortCtrlTxnCfgs, uint16_t nNumTxns);
extern ADI_EAL_STATUS adi_eal_portCtrlSendRegTransaction(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        bool bIsFifoRegTransaction, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern void adi_eal_portCtrlSetDownstreamVlan(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, ADI_VLAN_CFG *poIfRegVlanTag,
        ADI_VLAN_CFG *poFifoRegVlanTag, ADI_VLAN_CFG *poFifoWriteVlanTag);
extern ADI_EAL_STATUS adi_eal_portCtrlSetUpstreamVlan(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, ADI_VLAN_CFG *poVlanTag,
        bool bIsVlanEn);
extern ADI_EAL_STATUS adi_eal_portCtrlSetMailboxMode(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, bool bEnable);
extern ADI_EAL_STATUS adi_eal_portCtrlResyncSeqNum(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle);
extern ADI_EAL_STATUS adi_eal_portCtrlSetGpioInputMonitorAndReadTimeTag(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle,
        uint16_t nPinMask, bool bGpioEnable, bool bTimeTagEnable);
extern ADI_EAL_STATUS adi_eal_portCtrlSetReadTimeTagEcco(ADI_EAL_PORTCTRL_HANDLE hPortCtrlHandle, bool bEnable);
/*============================ CALLBACKS ============================*/

extern void GpioInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                             void *pCbParam);


#endif /* ADI_E2B_IFACE_PORT_ENABLED */
#endif /* ADI_EAL_PORTCTRL_H */

/** @} */

/**
 * EOF: www.analog.com
 */
