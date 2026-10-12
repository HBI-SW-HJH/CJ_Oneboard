/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: network_internal.h
 * \brief: Network layer component internal header file
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup network
 *  @{
 */

#ifndef NETWORK_INTERNAL_H
#define NETWORK_INTERNAL_H

/*================================= INCLUDES ================================*/
#include "adi_common.h"      /* Common types header */
#include "adi_network.h"     /* Class header */
#include "adi_pal.h"         /* PAL header */
#include "adi_dbg.h"         /* Diagnostics */
#include "adi_memmap.h"      /* Memory map */
#include "prj_osal.h"        /* OSAL header */

/*================================= DEFINES =================================*/
#define NETWORK_MAX_CTRL_CMD_REG_LEN  (128U)                           /* Maximum number of registers that can be read/written to in one macphy control command (OASPI specification) */
#define NETWORK_MAX_NUM_REGS    (NETWORK_MAX_CTRL_CMD_REG_LEN)         /* Number of register space to be allocated in the network layer control queue (per queue element). Max value 128 according to spec but can be reduced to save memory if needed  */
#define NETWORK_MAX_REG_LEN     (NETWORK_MAX_CTRL_CMD_REG_LEN)         /* Used by Macphy driver to perform checks - simplified to avoid MISRA C-2012 Rule 14.3 violation with invariant comparison */
#define NETWORK_CTRL_Q_SIZE     (19U)
#define NETWORK_DATA_Q_SIZE           (32U)

/* Priority queue feature gate: define ADI_NETWORK_PRIO_Q_EN to enable separate HP/LP TX queues.
 * When not defined (default), only a single queue is used and no HP queue memory is allocated.
 * Define ADI_NETWORK_PRIO_Q_EN in your application header or build configuration to enable. */

#ifdef ADI_NETWORK_PRIO_Q_EN
#define NETWORK_DATA_HP_Q_SIZE        (8U)                                     /* HP queue depth - small, for low-volume safety traffic */
#define NETWORK_DATA_LP_PRIO_Q_SIZE   (NETWORK_DATA_Q_SIZE - NETWORK_DATA_HP_Q_SIZE)   /* LP queue depth when priority FIFOs are enabled (24) */
#endif
#define NETWORK_RX_Q_SIZE       (4U)
#ifdef __NUCLEO_STM32__
#define MAX_MACPHY_INST         (1U)
#else
#define MAX_MACPHY_INST         (2U)
#endif
#define HV_DIE_RESET            (0xA55A)
#define LV_DIE_RESET            (0xA55A)
#define IO_TIC_ENABLE_34_GPIO1_REG0_TIC_ENABLE_34_FALLING_SSC_MODE_CHANGE_FLAG_HIGH (12u)
#define IO_TIC_CLEAR_34_GPIO1_REG0_TIC_CLEAR_34_FALLING_SSC_MODE_CHANGE_FLAG_HIGH (12u)
#define ADI_NETWORK_ETH_FRAME_MIN_SIZE       (64u)
/*================================ DATA TYPES ===============================*/
/*! Topology discovery measurement */
typedef enum TOPO_DISC_MEAS_TYPE
{
    TOPO_DISC_INTERNAL_DELAY, /*!< Internal delay measurement */
    TOPO_DISC_DISTANCE,    /*!< Distance measurement */
    TOPO_DISC_AUTOMATIC    /*!< Automtaic mode of topology discovery */
} TOPO_DISC_MEAS_TYPE;

/*! Internal Network structure */
typedef struct ADI_NETWORK_DATA
{
    uint32_t    nNumDevices;        /*!< Total number of devices */
    uint32_t    nNumDevOpened;      /*!< Number of opened devices */
#ifdef MACPHY_EN
    uint32_t    nSpiDevs;           /*!< Number of SPI devices */
    uint32_t    nSpiInst;           /*!< Number of SPI instances (for MACPHY) */
    const ADI_NETWORK_MACPHY_CFG *paoNwMacPhyCfg[MAX_MACPHY_INST]; /*!< Pointer to MACPHY device configuration structure */
#endif /* MACPHY_EN */
#ifdef ETH_EN
    uint32_t    nEthDevs;           /*!< Number of Ethernet devices */
    uint32_t    nEthInst;           /*!< Number of Ethernet instances */
#endif /* ETH_EN */
    volatile uint16_t anEthWrIdx[MAX_MACPHY_INST];   /*!< Network buffer write index (LP) */
#ifdef ADI_NETWORK_PRIO_Q_EN
    volatile uint16_t anEthWrIdxHp[MAX_MACPHY_INST]; /*!< Network buffer write index (HP) */
#endif
    volatile uint16_t nRxWrIdx[MAX_MACPHY_INST];     /*!< Network buffer RX write index */
    volatile uint16_t nRxRdIdx[MAX_MACPHY_INST];     /*!< Network buffer RX read index */
    volatile uint16_t nCtrlWrIdx;   /*!< Register desc write index */
    volatile uint16_t nCtrlRdIdx;   /*!< Register desc read index */
#ifdef ADI_SERDES_MODE
    volatile uint16_t anDataDescWrIdx[MAX_MACPHY_INST];   /*!< Data descriptor write index */
    volatile uint16_t anDataDescRdIdx[MAX_MACPHY_INST];   /*!< Data descriptor read index */
#endif
    ADI_NETWORK_DEVINFO aoDevInfo[ADI_NETWORK_MAX_DEVS]; /*!< Info of each of the network device */
    uint8_t     anControllerMacAddr[6u]; /*!< Controller MAC address (Source MAC address) */
    ADI_NETWORK_CBK   pfCbk;        /*!< Callback function */
} ADI_NETWORK_DATA;

/*! @brief Structure for masked write data */
typedef struct ADI_NETWORK_MASKED_WR_DATA
{
    uint32_t anRegWrData[NETWORK_MAX_NUM_REGS];   /*!< register data */
    uint32_t anRegMapMask[NETWORK_MAX_NUM_REGS];  /*!< register mask */
} ADI_NETWORK_MASKED_WR_DATA;

/*! @brief Wrapper structure to hold callback function pointer (MISRA C-2012 Rule 11.1 compliance) */
typedef struct ADI_NETWORK_CBK_WRAPPER
{
    ADI_NETWORK_CBK pfCallback;  /*!< Callback function pointer */
} ADI_NETWORK_CBK_WRAPPER;

/*=================================== DATA ==================================*/
extern ADI_NETWORK_DATA goNwData;
#ifndef ADI_SERDES_MODE
extern uint8_t ganEthBuff[MAX_MACPHY_INST][NETWORK_DATA_Q_SIZE][ADI_PAL_MAX_ETH_FRM];
#ifdef ADI_NETWORK_PRIO_Q_EN
extern uint8_t ganEthBuffHp[MAX_MACPHY_INST][NETWORK_DATA_HP_Q_SIZE][ADI_PAL_MAX_ETH_FRM];
#endif
#endif
extern uint8_t ganEthRxBuff[MAX_MACPHY_INST][NETWORK_RX_Q_SIZE][ADI_PAL_MAX_ETH_FRM];
extern volatile bool gabTransmitIdxTaken[NETWORK_DATA_Q_SIZE][ADI_NETWORK_MAX_DEVS];
#ifdef ADI_NETWORK_PRIO_Q_EN
extern volatile bool gabTransmitIdxTakenHp[NETWORK_DATA_HP_Q_SIZE][ADI_NETWORK_MAX_DEVS];
#endif

/*! Handle for the Transmit Index mutex */
extern PRJ_OSAL_MUTEX_HANDLE ghNwTxIdxMutex;
/*============================= EXTERN PROTOTYPES ===========================*/

#ifdef MACPHY_EN
// MACPHY APIs:
extern void network_macphy_Init(void);
extern void network_macphy_Terminate(void);
extern void network_macphy_Open(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg);
extern void network_macphy_Close(uint32_t nDevNum);
extern void network_macphy_Run(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_WriteRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    uint32_t  *panRegData,
    ADI_NETWORK_CBK pfCbk
);
extern ADI_NETWORK_ERR network_macphy_ReadRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    ADI_NETWORK_CBK pfCbk
);
extern ADI_NETWORK_ERR network_macphy_MaskedWriteRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    uint32_t  *panRegData,
    uint32_t  *panRegMask
);

#ifndef ADI_SERDES_MODE
extern void network_macphy_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg);
#else
extern ADI_NETWORK_ERR network_macphy_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
        uint8_t *pBuffer);
extern void network_macphy_ResetOaspiQueue(uint32_t nDevNum);
#endif
extern ADI_NETWORK_ERR network_macphy_GetMacStatus(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_GetPlcaStatus(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_GetPhyStatus(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_GetPcsStatus(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_GetPtpStatus(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_MacReset(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_HVDieReset(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_LVDieReset(uint32_t nDevNum);
extern bool network_macphy_AreTxBuffersEmpty(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_SetPlcaMulti(uint32_t nDevNum, const ADI_NETWORK_MACPHY_PLCA_CFG *poPlcaCfg);
extern ADI_NETWORK_ERR network_macphy_SetFilterTable(uint32_t nDevNum, uint8_t anMacAddr[][6u], uint8_t nNumAddr);
extern ADI_NETWORK_ERR network_macphy_ClearFilterTable(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_SetVlanFilter(uint32_t nDevNum, uint16_t nVlanId1, uint16_t nVlanId2,
        bool bDropFrames, bool bEnable);
extern ADI_NETWORK_ERR network_macphy_SetEtherTypeFilter(uint32_t nDevNum, uint16_t nEtherType1,
        uint16_t nEtherType2, bool bDropFrames, bool bEnable);
extern ADI_NETWORK_ERR network_macphy_SleepNode(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_StandbyNode(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_SendWakeUpPulse(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_TriggerWakeUpPin(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_PingControllerNode(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_SetMacFIFOSize_Macphy(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_MACPHY_CONFIG *poFIFOSize);
extern ADI_NETWORK_ERR network_macphy_SetMacFIFOSize_DualMode(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_DUAL_MODE_CONFIG *poFIFOSize);
extern ADI_NETWORK_ERR network_macphy_GetReasonForWkslp(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_Reconfigure(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg);
extern ADI_NETWORK_ERR network_macphy_SetPlcaToTimer(uint32_t nDevNum, uint32_t nToTime);
extern ADI_NETWORK_ERR network_macphy_SetPlcaBurstTimer(uint32_t nDevNum, uint8_t nMaxBurstCount, uint8_t nBurstTime);
extern ADI_NETWORK_ERR network_macphy_SetPlcaLeaderMode(uint32_t nDevNum, bool bPlcaLeader, bool bPlcaLeaderModeEn);
extern ADI_NETWORK_ERR network_macphy_EnablePlcaPrecedence(uint32_t nDevNum, bool bPlcaPrecedenceEn);
extern ADI_NETWORK_ERR network_macphy_EnableTestMode(uint32_t nDevNum, ADI_NETWORK_TESTMODE eTestMode);
extern ADI_NETWORK_ERR network_macphy_SetPmaControl(uint32_t nDevNum, bool bPmaReset, bool bTransmitDisable);
extern ADI_NETWORK_ERR network_macphy_EnableMacLoopBack(uint32_t nDevNum, bool bMacLoopback);
extern ADI_NETWORK_ERR network_macphy_EnablePmaLoopBack(uint32_t nDevNum, bool bPmaLoopback);
extern ADI_NETWORK_ERR network_macphy_EnablePcsLoopBack(uint32_t nDevNum, bool bPcsLoopback);
extern ADI_NETWORK_ERR network_macphy_CaptureExtTimestamp(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_EnableTsTimer(uint32_t nDevNum, uint32_t nFreq, uint8_t nDutyCycle,
        uint32_t nStartTime);
extern ADI_NETWORK_ERR network_macphy_StopTsTimer(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_EnablePpsOutput(uint32_t nDevNum, uint8_t nSAIF);
extern ADI_NETWORK_ERR network_macphy_sscSetConfig(uint32_t nDevNum,
        const ADI_NETWORK_MACPHY_SSC_MONITOR_CONFIG *poSscSetupCfg);
extern ADI_NETWORK_ERR network_macphy_sscReturnToFunctional(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_sscForceSafeState(uint32_t nDevNum, bool bForceSafeState);
extern ADI_NETWORK_ERR network_macphy_sscAliveCounterRead(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_sscCheckSafeModeStatus(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_sscMarkDeviceAttended(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_ConfigureSqi(uint32_t nDevNum,
        const ADI_NETWORK_MACPHY_SQI_CFG *poSqiCfg);
extern ADI_NETWORK_ERR network_macphy_GetSqiIndex(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_GetRevision(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_disableSqi(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_GetBootLoaderStatus(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_StartTopoDiscMeas(uint32_t nDevNum, TOPO_DISC_MEAS_TYPE eTopoDiscMeasType);
extern ADI_NETWORK_ERR network_macphy_GetTopoDiscMeas(uint32_t nDevNum);
extern ADI_NETWORK_ERR network_macphy_EnableTDBit(uint32_t nDevNum, bool bEnTDBit);
extern ADI_NETWORK_ERR network_macphy_SetGPTPInitialRateCorrection(uint32_t nDevNum, bool bEnable);
#endif /* MACPHY_EN */

#ifdef ETH_EN
// Ethernet APIs:
extern void network_eth_Init(void);
extern void network_eth_Terminate(void);
extern void network_eth_Open(uint32_t nDevNum);
extern void network_eth_Close(uint32_t nDevNum);
extern void network_eth_Run(uint32_t nDevNum);
#ifndef ADI_SERDES_MODE
void network_eth_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg);
#else
void network_eth_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg, uint8_t *pBuffer);
#endif
extern void network_eth_GetMacStatus(uint32_t nDevNum);
extern void network_eth_GetPhyStatus(uint32_t nDevNum);
extern void network_eth_MacReset(uint32_t nDevNum);
extern bool network_eth_AreTxBuffersEmpty(uint32_t nDevNum);
extern void network_eth_SetPlca(uint32_t nDevNum, const ADI_NETWORK_MACPHY_PLCA_CFG *poPlcaCfg);
extern void network_eth_SetFilterTable(uint32_t nDevNum, uint8_t panMacAddr[][6u], uint8_t nNumAddr);
extern void network_eth_ClearFilterTable(uint32_t nDevNum);
#endif /* ETH_EN */

#endif /* NETWORK_INTERNAL_H */

/** @} */

/**
 * EOF: www.analog.com
 */
