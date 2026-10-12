/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: eal_internal.h
 * \brief: The internal EAL header
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

/*! \cond PRIVATE */
#ifndef EAL_INTERNAL_H
#define EAL_INTERNAL_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"            /* The EAL header */
/*!
 * @brief Selective interface compilation guard.
 *
 * When @c ADI_E2B_IFACE_SELECTIVE is not defined, all E2B interface macros
 * are enabled by default via adi_e2b_iface_all.h. Define
 * @c ADI_E2B_IFACE_SELECTIVE at the project level and selectively define
 * only the @c ADI_E2B_IFACE_*_ENABLED macros your application requires
 * to reduce binary size.
 *
 * @see adi_e2b_iface_all.h for the full list of available interface macros.
 */
#ifndef ADI_E2B_IFACE_SELECTIVE
#include "adi_e2b_iface_all.h"  /* Default: enable all interfaces */
#endif
#ifdef ADI_E2B_IFACE_ADC_ENABLED
#include "adi_eal_adc.h"        /* EAL ADC header */
#endif
#ifdef ADI_E2B_IFACE_SPI_ENABLED
#include "adi_eal_spi.h"        /* EAL SPI header */
#endif
#ifdef ADI_E2B_IFACE_I2C_ENABLED
#include "adi_eal_i2c.h"        /* EAL I2C header */
#endif
#ifdef ADI_E2B_IFACE_ISELED_ENABLED
#include "adi_eal_iseled.h"     /* EAL ISELED header */
#endif
#ifdef ADI_E2B_IFACE_UART_ENABLED
#include "adi_eal_uart.h"       /* EAL UART header */
#endif
#ifdef ADI_E2B_IFACE_PORT_ENABLED
#include "adi_eal_portCtrl.h"   /* EAL PORT CTRL header */
#endif
#ifdef ADI_E2B_IFACE_DIAG_ENABLED
#include "adi_eal_diag.h"       /* EAL diag header */
#endif
#ifdef ADI_E2B_IFACE_SSC_ENABLED
#include "adi_eal_ssc.h"        /* EAL SSC header */
#endif
#ifdef ADI_E2B_IFACE_SQI_ENABLED
#include "adi_eal_sqi.h"        /* EAL SQI header */
#endif
#ifdef ADI_E2B_IFACE_SMC_ENABLED
#include "adi_eal_smc.h"        /* EAL SMC header */
#endif
#ifdef ADI_E2B_IFACE_TOPDISC_ENABLED
#include "adi_eal_topDisc.h"    /* EAL topology discovery header */
#endif
#ifdef ADI_E2B_IFACE_WKSLP_ENABLED
#include "adi_eal_wkslp.h"      /* EAL wake-sleep header */
#endif
#ifdef ADI_E2B_IFACE_LIN_ENABLED
#include "adi_eal_lin.h"        /* EAL LIN header */
#endif
#ifdef ADI_E2B_IFACE_FIO_PWM_ENABLED
#include "adi_eal_fioPwm.h"     /* EAL FIO PWM header */
#endif
#ifdef ADI_E2B_IFACE_FIO_UART_ENABLED
#include "adi_eal_fioUart.h"    /* EAL FIO UART header */
#endif
#ifdef ADI_E2B_IFACE_FIO_PDM_ENABLED
#include "adi_eal_fioPdm.h"     /* EAL FIO PDM header */
#endif
#ifdef ADI_E2B_IFACE_FIO_OSP_ENABLED
#include "adi_eal_fioOsp.h"     /* EAL FIO OSP header */
#endif

/*================================= DEFINES =================================*/
#define ADI_EAL_MAX_NUM_INSTANCES       (ADI_E2BCORE_MAX_INST)  /*!< Max number of EAL instances */
#define ADI_EAL_MAX_NUM_INF             (4u)                    /*!< Max number of interfaces per node */
#define ADI_EAL_MAX_NUM_SPI_INTFS       (0xFu)                  /*!< Max number of EAL SPI instances */
#define ADI_EAL_MAX_NUM_GPIO_INTFS      (0xFu)                  /*!< Max number of EAL GPIO instances */
#define ADI_EAL_MAX_NUM_I2C_INTFS       (0xFu)                  /*!< Max number of EAL I2C instances */
#define ADI_EAL_MAX_NUM_UART_INTFS      (0xFu)                  /*!< Max number of EAL UART instances */
#define ADI_EAL_MAX_FRAME_LENGTH        (1500u)                 /*!< Max length of the frame */
#define ADI_EAL_RESP_TIMEOUT_MS         (1000u)                 /*!< Response timeout in milliseconds */
/* For a frame length of 1500 bytes, 6 bytes will be consumed by header,
   remaining 1494 bytes can be filled by uart transactions.
   1 byte for transaction header + 8 bytes for gptp time + 2 bytes num packed + 1 byte E2b Id
   so 1494 - 12 = 1482. 1 uart cfg consumes 2 bytes so max number of configs in this case is 741 */
#define ADI_EAL_UART_CFG_MAX_LENGTH         (741u)                  /*!< Max number of UART configs */
#define CONVERT_FREQ_TO_TIME_PERIOD(nFreq)  (SEC_TO_NS / (nFreq))   /* Macro to Convert */
#define CONVERT_DUTY_CYCLE_PERCENT(nDuty)   ((uint16_t)((uint32_t)(nDuty) * 65535U / 100U))

#define EAL_SLOTS_FREE_IN_Q(nWrIdx, nRdIdx, nQSize) ((nQSize) - EAL_SLOTS_USED_IN_Q((nWrIdx), (nRdIdx), (nQSize)))
#define EAL_SLOTS_USED_IN_Q(nWrIdx, nRdIdx, nQSize) ((((nQSize) + (nWrIdx) - (nRdIdx)) < (nQSize)) ? (uint16_t)((nQSize) + (nWrIdx) - (nRdIdx)) : (uint16_t) ((nWrIdx) - (nRdIdx)))
#define EAL_IS_Q_FULL(nWrIdx, nRdIdx, nQSize) (EAL_SLOTS_USED_IN_Q((nWrIdx), (nRdIdx), (nQSize)) == ((nQSize) - 1U))
#define EAL_IS_Q_EMPTY(nWrIdx, nRdIdx, nQSize) (EAL_SLOTS_USED_IN_Q((nWrIdx), (nRdIdx), (nQSize)) == 0U)
#define EAL_INC_Q_IDX(nIdx, nQSize) ((((nIdx) + 1U) >= (nQSize)) ? ((nIdx) + 1U - (nQSize)) : ((nIdx) + 1U))

#define SET_FIFO_MSG_OPTS(poEalIntfData, oFifoMsgOpts) \
(void)ADI_MEMSET(&(oFifoMsgOpts), 0, sizeof((oFifoMsgOpts))); \
(oFifoMsgOpts).bSync          = true; \
(oFifoMsgOpts).bTopicOverRide = true; \
(oFifoMsgOpts).nTopic         = ADI_E2BCORE_TOPIC_FIFO0 + (poEalIntfData)->nIntfNum;

#define SET_REGMAP_MSG_OPTS(poEalIntfData, oMsgOpts) \
(void)ADI_MEMSET(&(oMsgOpts), 0, sizeof((oMsgOpts))); \
(oMsgOpts).bSync          = true; \
(oMsgOpts).bTopicOverRide = true; \
(oMsgOpts).nTopic         = ADI_E2BCORE_TOPIC_INTF0 + (poEalIntfData)->nIntfNum; \

#define WAIT_FOR_TIME_OUT(WaitingFlag, nTimeout) \
if(eEalStatus == ADI_EAL_STATUS_SUCCESS) \
{ \
    bool bTimedOut = false; \
    uint64_t nCurrTime, nStartTime; \
    adi_pal_getCurrTime(&nCurrTime); \
    nStartTime = nCurrTime; \
    while(true) \
    { \
        adi_network_run(); \
        adi_pal_getCurrTime(&nCurrTime); \
        if((nCurrTime - nStartTime) > (nTimeout)) \
        { \
            bTimedOut = true; \
        } \
        if(bTimedOut || !(WaitingFlag)) \
        { \
            break; \
        } \
    } \
    if(bTimedOut) \
    { \
        eEalStatus = ADI_EAL_STATUS_RX_TIMEOUT; \
        (WaitingFlag) = false; \
    } \
}

#define RELEASE_NW_MUTEX() \
if(bShouldReleaseNwMutex) \
{ \
    ADI_NETWORK_ERR eNwErr = adi_network_releaseMutex(); \
    ADI_DBG_ENSURE(eNwErr == ADI_NETWORK_OK, "Failed to release network layer mutex"); \
}
/*================================ DATA TYPES ===============================*/
/**
 * @brief EAL wake sleep interrupt events
 */
typedef enum EAL_WKSLP_INTERRUPT_EVT
{
    EAL_WKSLP_REMOTE_NODE_CONF_AWAKE_EVT,       /*!< Remote Node awake confirmed event */
    EAL_WKSLP_REMOTE_NODE_RET_FROM_STBY_EVT,    /*!< Remote Node returned from standby event */
    EAL_WKSLP_REQ_SYS_WAKEUP_CONF_EVT,          /*!< Request to confirm system wakeup event */
    EAL_WKSLP_LINE_BUSY_EVT,                    /*!< Node is still sending frames and sleep / standby failed */
    EAL_WKSLP_SLEEP_ERR_EVT,                    /*!< Watchdog timer expiry (Going to sleep failed) */
    EAL_WKSLP_STANDBY_ERR_EVT,                  /*!< Watchdog timer expiry (Going to standby failed) */
    EAL_WKSLP_PING_RCVD_EVT,                    /*!< Node has replied even after going to sleep / standby */
} EAL_WKSLP_INTERRUPT_EVT;

/**
 * @brief EAL Safe State Controller interrupt events
 */
typedef enum EAL_SSC_INTERRUPT_EVT
{
    EAL_SSC_MODE_CHANGE_TO_SAFE_EVT,            /*!< Switch to Safe mode event */
    EAL_SSC_MODE_CHANGE_TO_FUNC_EVT,            /*!< Switch to Functional mode event */
    EAL_SSC_UNSUCCESSFUL_RETURN_TO_FUNC_EVT     /*!< Unsuccessful return to functional mode event */
} EAL_SSC_INTERRUPT_EVT;

/**
 * @brief EAL SQI interrupt events
 */
typedef enum EAL_SQI_INTERRUPT_EVT
{
    EAL_SQI_ENTERED_WARNING_EVT,        /*!< SQI level entered warning event */
    EAL_SQI_EXITED_WARNING_EVT,         /*!< SQI level exited warning event */
    EAL_SQI_ENTERED_BAD_EVT,            /*!< SQI level entered bad event */
    EAL_SQI_EXITED_BAD_EVT              /*!< SQI level exited bad event */
} EAL_SQI_INTERRUPT_EVT;

/**
 * @brief EAL SMC interrupt events
 */
typedef enum EAL_SMC_INTERRUPT_EVT
{
    EAL_SMC_ITERATION_END_EVT,        /*!< Iteration complete event in SMC */
    EAL_SMC_ITERATION_SKIP_EVT,       /*!< Iteration skipped event in SMC */
    EAL_SMC_REGION_COLLISION_EVT,     /*!< Region collision event , occurs when a write on address range of a region is detected that is executing in SMC */
    EAL_SMC_CONFIGURATION_CHANGE_EVT  /*!< Configuration change event, occurs when there is a change in the selected configuration that is detected at the beginning of an iteration  */
} EAL_SMC_INTERRUPT_EVT;

typedef enum EAL_FIO_PWM_INTERRUPT_EVT
{
    EAL_FIO_PWM_LOW_IRQ_EVT,
    EAL_FIO_PWM_HIGH_IRQ_EVT,
} EAL_FIO_PWM_INTERRUPT_EVT;

/**
 * @brief EAL FIO OSP interrupt events
 */
typedef enum EAL_FIO_OSP_INTERRUPT_EVT
{
    EAL_FIO_OSP_TIMEOUT_IRQ_EVT
} EAL_FIO_OSP_INTERRUPT_EVT;

/**
 * @brief EAL FIO PDM interrupt events
 */
typedef enum EAL_FIO_PDM_INTERRUPT_EVT
{
    EAL_FIO_PDM_WRITE_FIFO_LOW_IRQ_EVT,    /*!< Low interrupt event for FIO PDM FIFO Write */
    EAL_FIO_PDM_WRITE_FIFO_HIGH_IRQ_EVT,   /*!< High interrupt event for FIO PDM FIFO Write */
    EAL_FIO_PDM_READ_FIFO_FULL_IRQ_EVT,    /*!< Full interrupt event for FIO PDM FIFO Read */
} EAL_FIO_PDM_INTERRUPT_EVT;

/* EAL interface type - currently useful in differentiating the FLEXIO IO interface types */
typedef enum EAL_INTERFACE_TYPE
{
    EAL_INTERFACE_TYPE_UNDEF,     /*!< Undefined interface */
    EAL_INTERFACE_TYPE_SPI,       /*!< SPI interface */
    EAL_INTERFACE_TYPE_FIO_PDM,   /*!< FIO PDM interface */
    EAL_INTERFACE_TYPE_FIO_PWM,   /*!< FIO PWM interface */
    EAL_INTERFACE_TYPE_FIO_UART,  /*!< FIO UART interface */
    EAL_INTERFACE_TYPE_I2C,       /*!< I2C interface */
    EAL_INTERFACE_TYPE_ISELED,    /*!< ISELED interface */
    EAL_INTERFACE_TYPE_LIN,       /*!< LIN interface */
    EAL_INTERFACE_TYPE_UART,      /*!< UART interface */
    EAL_INTERFACE_TYPE_PWM,       /*!< Port Control interface */
    EAL_INTERFACE_TYPE_ADC,        /*!< ADC interface */
    EAL_INTERFACE_TYPE_FIO_OSP      /*!< FIO OSP interface*/
} EAL_INTERFACE_TYPE;

/**
 * @brief  Data structure for each EAL node data instance
 */
typedef struct EAL_NODE_DATA
{
    bool bNotifyWkslp;                          /*!< Flag indicating request to notify events to wake-sleep module */
    bool bNotifyOtp;                            /*!< Flag indicating request to notify events to OTP module */
    bool bNotifySsc;                            /*!< Flag indicating request to notify events to Safe State Controller module */
    bool bNotifySqi;                            /*!< Flag indicating request to notify events to SQI module */
    bool bNotifyTopoDisc;                       /*!< Flag indicating request to notify events to Topology Discovery module */
    bool bWaitingForResp;                       /*!< Waiting for remote node resp */
    ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg;     /*!< Pointer to register config */
    EAL_INTERFACE_TYPE aeIntfType[ADI_EAL_MAX_NUM_INF]; /*!< Interface type */
    ADI_EAL_INTERRUPT_CALLBACK_FUNC aeIntfCbk[ADI_EAL_MAX_NUM_INF];  /* !< Pointer to store address to callback function for each interface*/
} EAL_NODE_DATA;

/*! Data structure for each EAL instance */
typedef struct EAL_DATA
{
    bool bInUse;                                         /*!< Flag for checking if EAL is initialized */
    bool bEnableFrameDup;                                /*!< Flag for frame duplication */
    bool bBuffInit;                                      /*!< Flag indicating if network buffer is initialized */
    bool bLCETopicConfigured;                            /*!< Flag to enable topic override (topic_node=0) for all node register transactions.
                                                              When true, the LCE topics are pre-configured (OTP-programmed or post-configureNetwork)
                                                              and all node-level RegMap transactions use topic override with TOPIC_NODE(0). */
    uint32_t nDevNum;                                    /*!< Device number mapped to this EAL instance */
    uint32_t nMacCnt;                                    /*!< Number of mac address received during network discovery */
    uint32_t nNumDiscNodes;                              /*!< Number of nodes discovered */
    EAL_NODE_DATA aoEalNodeData[ADI_EAL_MAX_NUM_NODES];
    ADI_E2BCORE_CFG *poE2bCoreCfg;                       /*!< Pointer to the network configuration */
    uint8_t anRemotePlcaMap[ADI_EAL_MAX_NUM_NODES];      /*!< Mapping between remote number and PLCA ID */
    ADI_EAL_TOP_CALLBACK_FUNC pfEalTopCbk;               /*!< Pointer to store the address of callback function */
    ADI_EAL_NODE_CALLBACK_FUNC pfEalNodeCbk;             /*!< Pointer to store the address of node callback function */
    uint8_t anEalMutexBuffer[PRJ_OSAL_MUTEX_MEM_SIZE];            /*!< Buffer for EAL mutex */
    PRJ_OSAL_MUTEX_CFG oEalMutexCfg;                     /*!< EAL mutex configuration */
    PRJ_OSAL_MUTEX_HANDLE hEalMutex;                     /*!< Handle for the EAL mutex */
} EAL_DATA;

#ifdef ADI_E2B_IFACE_I2C_ENABLED
/**
 * @brief EAL I2C REGMAP and FIFO transaction
 */
typedef struct ADI_EAL_I2C_REGMAP_FIFO_DATA
{
    uint16_t nNumFifoTxns;                     /*!< Number of fifo transactions */
    uint8_t nNumRegmapTxns;                    /*!< Number of regmap transactions */
    ADI_E2BCORE_MSGOPTS *pMsgOpts;             /*!< Pointer to store message options for regmap transaction */
    ADI_E2BCORE_MSGOPTS *pFifoMsgOpts;         /*!< Pointer to store message options for fifo transaction */
    ADI_E2BCORE_I2C_TXN_CONFIG *pI2cCfg;       /*!< Pointer to store I2C config */
    ADI_E2BCORE_REGMAP_IO_CONFIG *pRegmapCfg;  /*!< Pointer to store Regmap config */
} ADI_EAL_I2C_REGMAP_FIFO_DATA;
#endif

#ifdef ADI_E2B_IFACE_SPI_ENABLED
/**
 * @brief EAL SPI REGMAP and FIFO transaction
 */
typedef struct ADI_EAL_SPI_REGMAP_FIFO_DATA
{
    uint16_t nNumFifoTxns;                     /*!< Number of fifo transactions */
    uint8_t nNumRegmapTxns;                    /*!< Number of regmap transactions */
    ADI_E2BCORE_MSGOPTS *pMsgOpts;             /*!< Pointer to store message options for regmap transaction */
    ADI_E2BCORE_MSGOPTS *pFifoMsgOpts;         /*!< Pointer to store message options for fifo transaction */
    ADI_E2BCORE_SPI_TXN_CONFIG *pSpiCfg;       /*!< Pointer to store SPI config */
    ADI_E2BCORE_REGMAP_IO_CONFIG *pRegmapCfg;  /*!< Pointer to store Regmap config */
} ADI_EAL_SPI_REGMAP_FIFO_DATA;
#endif

/* EAL data object for each  instance */
extern EAL_DATA gaoEalData[ADI_EAL_MAX_NUM_INSTANCES];

/*! Prototype for interface API to add multiple transaction */
typedef ADI_E2BCORE_ERR(*ADI_E2BCORE_ADD_INTF_MSG)(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts,
        uint32_t nNumConfigs, ADI_E2BCORE_TXN_CONFIG aoIoCfgs[]);
/* ~~~~~~~~~~~~~~~~~~~ Event Notifications ~~~~~~~~~~~~~~~~~~~ */
#ifdef ADI_E2B_IFACE_WKSLP_ENABLED
extern bool eal_notify_WkslpInterrupt(uint32_t nInstNum, uint32_t nRemoteNum,
                                      EAL_WKSLP_INTERRUPT_EVT eWakeSleepIntEvt);
extern void eal_notify_WkslpRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat);
#endif
#ifdef ADI_E2B_IFACE_DIAG_ENABLED
extern void eal_notify_DiagRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat);
#endif
#ifdef ADI_E2B_IFACE_TOPDISC_ENABLED
extern void eal_notify_TopoDiscRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_E2BCORE_CBK_REGREADDAT *poRegDat);
#endif
#ifdef ADI_E2B_IFACE_OTP_ENABLED
extern void eal_notify_OtpRegReadResp(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
                                      ADI_E2BCORE_CBK_REGREADDAT *poRegDat);
#endif
#ifdef ADI_E2B_IFACE_SSC_ENABLED
extern bool eal_notify_SscInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, EAL_SSC_INTERRUPT_EVT eSscIntEvt);
extern void eal_notify_sscRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat);
#endif
#ifdef ADI_E2B_IFACE_SQI_ENABLED
extern bool eal_notify_SqiInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, EAL_SQI_INTERRUPT_EVT eSqiIntEvt);
extern void eal_notify_sqiRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat);
#endif
#ifdef ADI_E2B_IFACE_SMC_ENABLED
extern bool eal_notify_SmcInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, EAL_SMC_INTERRUPT_EVT eSmcIntEvt);
#endif
#ifdef ADI_E2B_IFACE_FIO_PWM_ENABLED
extern bool eal_notify_FioPwmInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
                                       EAL_FIO_PWM_INTERRUPT_EVT eFioPwmEvt);
#endif
#ifdef ADI_E2B_IFACE_FIO_PDM_ENABLED
extern bool eal_notify_FioPdmInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
                                       EAL_FIO_PDM_INTERRUPT_EVT eFioPdmEvt);
#endif
/************************* Utilities ******************************************/
extern void adi_eal_configureTopicOverride(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts);
extern void eal_utils_fillNetworkCfg(uint32_t nInstNum, uint32_t nRemoteNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
                                     uint32_t *pnVlanTag);
extern ADI_EAL_STATUS eal_utils_sendRegmapMessage(uint32_t nInstNum,
        ADI_NETWORK_FRAME_CFG *pNwFrameCfg, ADI_E2BCORE_MSGOPTS *poRegMsgOpts, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg);
extern ADI_EAL_STATUS eal_utils_readInfStatus(ADI_E2BCORE_ID *poId, uint16_t nStatusRegAddr, uint8_t nLength,
        bool bIsMcast, uint32_t anTopics[2U], uint32_t nVlanTag);
extern ADI_EAL_STATUS eal_utils_sendNopResyncTxn(ADI_E2BCORE_ID *poId);
extern ADI_EAL_STATUS eal_utils_sendRegmapFifoFrame(ADI_E2BCORE_ID *poId,
        ADI_NETWORK_FRAME_CFG *pNwFrameCfg,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg,
        ADI_E2BCORE_TXN_CONFIG *poTxnCfg,
        uint32_t *panTopics);
extern ADI_EAL_STATUS eal_utils_sendFifoFrame(ADI_E2BCORE_ID *poId,
        ADI_NETWORK_FRAME_CFG *pNwFrameCfg,
        ADI_E2BCORE_TXN_CONFIG *poTxnCfg,
        ADI_E2BCORE_MSGOPTS *pFifoMsgOpts);
extern ADI_EAL_STATUS eal_utils_waitForTimeout(bool *pbWaitingFlag, uint32_t nTimeout);
extern ADI_EAL_STATUS eal_utils_startE2bMessage(uint32_t nInstNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
        ADI_E2BCORE_MSGOPTS *poFifoMsgOpts);
extern void eal_utils_finishMessageAndTransmit(uint32_t nInstNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
        ADI_E2BCORE_MSGOPTS *poFifoMsgOpts);
extern bool eal_utils_getConfigRemoteRegVal(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nRegAddr,
        uint32_t *pRegData);
extern bool eal_utils_setConfigRemoteRegVal(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nRegAddr,
        uint32_t nRegData);
extern bool eal_utils_getConfigInfRegVal(ADI_E2BCORE_ID *poId, uint16_t nRegAddr, uint32_t *pRegData, bool bIsFifoReg);
extern bool eal_utils_setConfigInfRegVal(ADI_E2BCORE_ID *poId, uint16_t nRegAddr, uint32_t nRegData, bool bIsFifoReg);
extern uint8_t eal_utils_computeParity(ADI_EAL_PARITY_TYPE eParityType, uint8_t nTxData);
extern uint8_t eal_utils_computeParity16(ADI_EAL_PARITY_TYPE eParityType, uint16_t nData);
#ifdef ADI_E2B_IFACE_FIO_ENABLED
extern ADI_EAL_STATUS eal_utils_configureFlexibleIO(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData);
#endif /* ADI_E2B_IFACE_FIO_ENABLED */
extern uint32_t eal_utils_convertVlanTags(ADI_VLAN_CFG *poVlanCfg);
#ifdef ADI_E2B_IFACE_SMC_ENABLED
extern void CalcRegionHeaderLen(ADI_SMC_REGION_CONFIG *poRegionCfg, uint8_t *pnRegionHeaderLen);
#endif /* ADI_E2B_IFACE_SMC_ENABLED */
#endif /* EAL_INTERNAL_H */

/*! \endcond */

/** @} */

/*
 * EOF: www.analog.com
 */
