/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_network.h
 * \brief: Header for the Network layer component
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup network
 *  @{
 */

#ifndef ADI_NETWORK_H
#define ADI_NETWORK_H

/*================================= INCLUDES ================================*/
#include "adi_pal.h"    /* Regular Ethernet interface + platform related functions */
#ifdef MACPHY_EN
#include "adi_oaspi.h"  /* OA-SPI interface functions */
#endif /* MACPHY_EN */
#include "adi_macphy_reg.h"
#include "adi_phy_std_reg.h"
#include "adi_phy_oa_reg.h"
#include "adi_ao_reg.h"
#include "adi_otp_reg.h"
#include "adi_io_lv_reg.h"
#include "adi_io_hv_reg.h"
#include "adi_pmd_reg.h"
/*================================= DEFINES =================================*/
#define ADI_NETWORK_MAX_DEVS          (ADI_PAL_MAX_NUM_SPI_DEV + ADI_PAL_MAX_NUM_ETH_DEV)   /*!< Maximum number of devices */
#define ADI_NETWORK_MIN_DEVS          (2u)                                                  /*!< Minimum number of devices */
#define ADI_NETWORK_DEV_NAME_MAX_SIZE (ADI_PAL_DEV_NAME_MAX_SIZE)                           /*!< The max length of device name */
#define ADI_NETWORK_MACPHY_PLCA_ID_DEFAULT_VALUE                      (0x000000FFu)         /*!< PLCA Default ID, can be set incase the PLCA ID is no needed */
#define ADI_NETWORK_MACPHY_EN_PLCA_ID_(N)                             (0x00000001u << (N))  /*!< Enables the PLCA ID , N indicates the number of PLCA ID */
#define ADI_NETWORK_MACPHY_PLCA_ID_EN_ALL                             (0x000000FFu)         /*!< Enables all the PLCA ID(s) */
#define ADI_NETWORK_MACPHY_MAX_PLCA_ID                                (8U)                  /*!< Max number of PLCA ID for a node */
#define ADI_NETWORK_MACPHY_BOOTLOADER_BLOCKS                          (64U)                 /*!< Total number of bootloader blocks */
/*================================ DATA TYPES ===============================*/
/*! @brief Enumeration of different device operational mode */
typedef enum ADI_NETWORK_DEV_MODE
{
    ADI_NETWORK_DUAL_MODE = 1U,          /*!< Enables MACPHY and LCE (gPTP) */
    ADI_NETWORK_MACPHY_MODE = 3U         /*!< Enables MACPHY Mode */
} ADI_NETWORK_DEV_MODE;

/*! @brief Enumeration of different chunk payload selector */
typedef enum ADI_NETWORK_CPS
{
    ADI_NETWORK_CPS_8BYTES = 3,          /*!< Selects chunksize 8 bytes */
    ADI_NETWORK_CPS_16BYTES,             /*!< Selects chunksize 16 bytes */
    ADI_NETWORK_CPS_32BYTES,             /*!< Selects chunksize 32 bytes */
    ADI_NETWORK_CPS_64BYTES              /*!< Selects chunksize 64 bytes */
} ADI_NETWORK_CPS;

/*! @brief Enumeration of different test modes */
typedef enum ADI_NETWORK_TESTMODE
{
    ADI_NETWORK_TEST_NORMAL,                      /*!< normal operation */
    ADI_NETWORK_TEST_TRANSMIT1,                   /*!< continuously transmit '1' */
    ADI_NETWORK_TEST_SEND_SQUARE_WAVE,            /*!< send square wave with high and low period=1.6us */
    ADI_NETWORK_TEST_RANDOM_SEQUENCE,             /*!< transmit psuedo random sequence */
    ADI_NETWORK_TEST_HIGH_IMPEDENCE_TERMINATION,  /*!< transmitter presents high impedance termination to the line */
    ADI_NETWORK_TEST_WAKE_UP_TONE = 7             /*!< transmit wake-up tone on the bus */
} ADI_NETWORK_TESTMODE;

/*! @brief MAC-PHY device configuration structure */
typedef struct ADI_NETWORK_MACPHY_CFG
{
    uint32_t nSpiFreq;                  /*!< SPI frequency (in Hz) */
    uint8_t  nClkPha;                   /*!< Clock Phase */
    uint8_t  nClkPol;                   /*!< Clock Polarity */
    ADI_NETWORK_CPS eCPS;               /*!< Chunk payload selector */
    bool bFTSE;                         /*!< Receive frame timestamp enable */
    bool bUse32BitTimestamp;            /*!< Receive frame timestamp selection - if set to true 32-bit timestamps are captured, else 64-bit timestamps are captured (default, aligned with TRM) */
    bool bRXCTE;                        /*!< Receive Cut-Through enable */
    bool bTXCTE;                        /*!< Transmit Cut-Through enable */
    bool bGPTPCntrEn;                   /*!< Enable gPTP counter */
    bool bMacTimer;                     /*!< Enable MAC Timer */
    bool bEnableRxBufferOverflowInt;    /*!< Enable RX buffer overflow interrupt */
    bool bEnableSqiInt;                 /*!< Enables SQI interrupts */
    bool bEnableSscInt;                 /*!< Enables SSC interrupts */
    bool bEnableWkSlp;                  /*!< Enables wake-sleep functionality */
    bool bAppendCrcbyHost;              /*!< Indicates whether the CRC for the frame is appended by host but not by MAC */
    bool bValidateCrcbyHost;            /*!< Indicates whether the CRC for the frame is to validated by host (irrespective of whether it is validated by MAC) */
    bool bEnableAppInt;                 /*!< Enables application configured VS interrupts */
    bool bEnablePrioFifos;              /*!< Enables MAC frame priority FIFOs (dual HP/LP FIFOs with VS-based priority scheduling).
                                             When false (default), single FIFO is used and VS bits route frames to T1S/LCE ports. */
} ADI_NETWORK_MACPHY_CFG;

/*! @brief Different types of network devices */
typedef enum ADI_NETWORK_TYPE
{
    ADI_NETWORK_TYPE_MACPHY,            /*!< MAC-PHY device */
    ADI_NETWORK_TYPE_ETHERNET           /*!< ETHERNET device */
} ADI_NETWORK_TYPE;

/*! @brief Error types */
typedef enum ADI_NETWORK_ERR
{
    ADI_NETWORK_OK,                         /*!< No network error */
    ADI_NETWORK_NO_SPACE,                   /*!< No buffer space available */
    ADI_NETWORK_MUTEX_FAILURE,              /*!< Mutex lock or unlock failure */
    ADI_NETWORK_SPI_SEND_FAILED,            /*!< Error in pal_spiSend API */
#ifdef MACPHY_EN
    ADI_NETWORK_GPIO_WRITE_FAILED,          /*!< Error in writing to GPIO of the platform */
    ADI_NETWORK_MACPHY_STATUS_ERR,          /*!< Error bits asserted in STATUS registers */
    ADI_NETWORK_MACPHY_REG_WRITE_FAILED,    /*!< Register write failed */
    ADI_NETWORK_MACPHY_REG_READ_FAILED,     /*!< Register read failed */
    ADI_NETWORK_MACPHY_DATA_HDR_BAD,        /*!< OA-SPI Header bad bit set */
    ADI_NETWORK_MACPHY_PARITY_ERR,          /*!< OA-SPI Parity check failed */
    ADI_NETWORK_MACPHY_DROP_RX_FRAME,       /*!< OA-SPI drop RX frame bit set */
    ADI_NETWORK_NODE_NOT_AWAKE,             /*!< Node not awake */
    ADI_NETWORK_MACPHY_RX_FRAME_CRC_FAILED, /*!< CRC verification failure in the received etherenet frame */
    ADI_NETWORK_MACPHY_CTRL_Q_FULL,        /*!< OA-SPI control queue full - command could not be submitted */
    ADI_NETWORK_MACPHY_UNEXPECTED_EVT,     /*!< Unexpected event or register address received in callback */
    ADI_NETWORK_MACPHY_SYNC_LOST,           /*!< MAC-PHY sync lost (requires reconfiguration) */
#ifdef ADI_SERDES_MODE
    ADI_NETWORK_MACPHY_TX_DESCRIPTOR_UNAVAILABLE, /*!< No TX descriptor for OASPI transmission */
    ADI_NETWORK_MACPHY_OASPI_TX_QUEUE_FULL, /*!< OASPI TX queue that stores pointers to descriptor is full */
#endif /* ADI_SERDES_MODE */
#endif /* MACPHY_EN */
    ADI_NETWORK_INVALID_PARAM,              /*!< Invalid parameter */
    ADI_NETWORK_DEVICE_CLOSED               /*!< Operation attempted on a closed device */
} ADI_NETWORK_ERR;

/*! @brief Events for the network callbacks */
typedef enum ADI_NETWORK_EVT
{
    ADI_NETWORK_EVT_ETH10_RX_RECEIVED,    /*!< RX received from 10baset1 event */
    ADI_NETWORK_EVT_ETH_RX_RECEIVED,      /*!< RX received from regular ethernet event */
    ADI_NETWORK_EVT_TX_SUCCESS,           /*!< TX frame sent event */
    ADI_NETWORK_EVT_ERR,                  /*!< Error event */
    ADI_NETWORK_EVT_MAC_STATUS,           /*!< MAC status event */
    ADI_NETWORK_EVT_PLCA_STATUS,          /*!< PLCA status event */
    ADI_NETWORK_EVT_PHY_STATUS,           /*!< PHY status event */
    ADI_NETWORK_EVT_PCS_STATUS,           /*!< PCS status event */
    ADI_NETWORK_EVT_PTP_STATUS,           /*!< PTP status event */
    ADI_NETWORK_EVT_READ_REG,             /*!< Read reg event */
    ADI_NETWORK_EVT_WRITE_REG,            /*!< Write reg event */
    ADI_NETWORK_EVT_TS_CAPTD,             /*!< Captured timestamp event */
    ADI_NETWORK_EVT_TS_EXT_CAPT,          /*!< External timestamp capture event */
    ADI_NETWORK_EVT_PING_RESP,            /*!< Ping response received */
    ADI_NETWORK_EVT_AWAKE,                /*!< Node transitioned to awake */
    ADI_NETWORK_EVT_STANDBY,              /*!< Node transitioned to standby */
    ADI_NETWORK_EVT_SLEEP,                /*!< Node transitioned to sleep */
    ADI_NETWORK_EVT_STANDBY_FAILED,       /*!< Node failed to transition to standby */
    ADI_NETWORK_EVT_SLEEP_FAILED,         /*!< Node failed to transition to sleep */
    ADI_NETWORK_EVT_REASON_FOR_WKSLP,     /*!< Reason for wake / sleep event */
    ADI_NETWORK_EVT_SSC_STATUS,           /*!< SSC status read event */
    ADI_NETWORK_EVT_SSC_ALIVE_COUNTER_RB, /*!< SSC Alive counter read event */
    ADI_NETWORK_EVT_SSC_SAFE_STATE,       /*!< SSC Safe State change event */
    ADI_NETWORK_EVT_SSC_RETURN_TO_FUNC, /*!< SSC Return to functional state event */
    ADI_NETWORK_EVT_SSC_UNSUCCESSFUL_RETURN_TO_FUNC, /*!< SSC Unsuccesful return to functional state event */
    ADI_NETWORK_EVT_BOOTLOADER_DONE,       /*!< Bootloader done event */
    ADI_NETWORK_EVT_SQI_STATUS,           /*!< SQI status read event */
    ADI_NETWORK_EVT_SQI_LVL_RETURN_GOOD,  /*!< SQI level return back to good */
    ADI_NETWORK_EVT_SQI_LVL_WARNING,      /*!< SQI level warning event */
    ADI_NETWORK_EVT_SQI_LVL_BAD,          /*!< SQI level bad event */
    ADI_NETWORK_EVT_REVISION_NUM,         /*!< Revision number event */
    ADI_NETWORK_EVT_BOOTLOADER_STATUS,    /*!< Bootloader status event */
    ADI_NETWORK_EVT_TOPO_DISC_DELAY_FAILED, /*!< Topology discovery delay measurement failed */
    ADI_NETWORK_EVT_TOPO_DISC_DELAY_MEAS,   /*!< Topology discovery delay measurement read event */
    ADI_NETWORK_EVT_TOPO_DISC_DIST_FAILED,  /*!< Topology discovery distance measurement failed */
    ADI_NETWORK_EVT_TOPO_DISC_DIST_MEAS,    /*!< Topology discovery distance measurement read event */
    ADI_NETWORK_EVT_TOPO_DISC_AUTO_MEAS_FAILED, /*!< Topology discovery automatic mode measurement failed */
    ADI_NETWORK_EVT_UNK_INT,              /*!< Unknown VS interrupt asserted */
    ADI_NETWORK_EVT_APP_VS_INT,           /*!< Application configured VS interrupt asserted */
#ifdef ADI_SERDES_MODE
    ADI_NETWORK_EVT_APP_REQUEST_BUFFER,   /*!< Event to indicate that a request to get a network layer buffer was made */
    ADI_NETWORK_EVT_APP_FINISHED_SERIALIZATION,  /*!< Event to indicate that a request to get transmit buffer was made */
    ADI_NETWORK_EVT_APP_REQUEST_FLUSH_BUFFERS, /*!< Event to indicate that a request to flush transmit buffers was made */
#endif /* ADI_SERDES_MODE */
} ADI_NETWORK_EVT;

/*! @brief Enumeration of routing information - Port 0 / Port 1 */
typedef enum ADI_NETWORK_MACPHY_ROUTING_INFO
{
    ADI_MACPHY_ROUTE_PORT0_LP = 0,        /*!< Routes the frame to only port 0 (T1S) or Low-priority FIFO */
    ADI_MACPHY_ROUTE_PORT1_HP = 1,        /*!< Routes the frame to only port 1 (LCE) or High-priority FIFO */
    ADI_MACPHY_ROUTE_BOTH     = 2,        /*!< Routes the frame to both ports */
} ADI_NETWORK_MACPHY_ROUTING_INFO;

/*! @brief Enum of the controller states */
typedef enum ADI_NETWORK_CTRL_STATES
{
    ADI_NETWORK_CTRL_STATE_AWAKE,                   /*!< Node in awake state */
    ADI_NETWORK_CTRL_STATE_STANDBY,                 /*!< Node in standby state */
    ADI_NETWORK_CTRL_STATE_SLEEP,                   /*!< Node in sleep state */
    ADI_NETWORK_CTRL_STATE_SEND_STANDBY_CMD,        /*!< Sends command to set node to standby */
    ADI_NETWORK_CTRL_STATE_SEND_SLEEP_CMD,          /*!< Sends command to set node to sleep */
    ADI_NETWORK_CTRL_STATE_WAITING_TO_STANDBY,      /*!< Waiting for node to go to standby */
    ADI_NETWORK_CTRL_STATE_WAITING_TO_SLEEP,        /*!< Waiting for node to go to sleep */
    ADI_NETWORK_CTRL_STATE_PING_NODE,               /*!< Sends a ping command to check if node is awake */
    ADI_NETWORK_CTRL_STATE_WAITING_FOR_PING_RESP,   /*!< Waiting for ping response to check if node is awake */
} ADI_NETWORK_CTRL_STATES;

/*! @brief Enum of the controller node context */
typedef enum ADI_NETWORK_CTRL_CTX
{
    ADI_NETWORK_CTRL_CTX_WAITING_TO_STANDBY,        /*!< Waiting to standby context */
    ADI_NETWORK_CTRL_CTX_WAITING_TO_SLEEP,          /*!< Waiting to sleep context */
} ADI_NETWORK_CTRL_CTX;

/*! @brief Enum of wake-sleep fail status */
typedef enum ADI_NETWORK_WKSLP_FAIL_STATUS
{
    ADI_NETWORK_WKSLP_LINE_BUSY,        /*!< Ethernet line is busy (Indicates failure to go to sleep / standby) */
    ADI_NETWORK_WKSLP_STANDBY_ERR,      /*!< Standby error event (Standby Watchdog timer expiry: Indicates failure to go to standby) */
    ADI_NETWORK_WKSLP_SLEEP_ERR,        /*!< Sleep error event (Sleep Watchdog timer expiry: Indicates failure to go to sleep) */
    ADI_NETWORK_WKSLP_PING_RESP_ERR,    /*!< Ping response received (Indicates failure to go to sleep / standby) */
} ADI_NETWORK_WKSLP_FAIL_STATUS;

/*! @brief Enum for configuring FIFO Size(s) */
typedef enum ADI_NETWORK_MACPHY_FIFO_SIZE
{
    ADI_NETWORK_MACPHY_FIFO_SIZE_0K,     /*!< Sets the FIFO size to 0KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_2K,     /*!< Sets the FIFO size to 2KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_4K,     /*!< Sets the FIFO size to 4KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_6K,     /*!< Sets the FIFO size to 6KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_8K,     /*!< Sets the FIFO size to 8KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_10K,    /*!< Sets the FIFO size to 10KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_12K,    /*!< Sets the FIFO size to 12KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_14K,    /*!< Sets the FIFO size to 14KB */
    ADI_NETWORK_MACPHY_FIFO_SIZE_16K     /*!< Sets the FIFO size to 16KB */
} ADI_NETWORK_MACPHY_FIFO_SIZE;

/*! Enum for bootloader status */
typedef enum ADI_NETWORK_BOOTLOADER_STATUS
{
    ADI_NETWORK_BOOTLOADER_VALID,         /*! Block is valid */
    ADI_NETWORK_BOOTLOADER_CORRUPTED = 3, /*! Block is corrupted */
} ADI_NETWORK_BOOTLOADER_STATUS;


/*! @brief Ethernet frame data structure */
typedef struct ADI_NETWORK_ETH_FRM_DATA
{
    ADI_TS_DATA     *pTimeStamp;        /*!< The pointer to the time stamp */
    uint8_t         *pRxData;           /*!< The pointer to the RX data */
    uint16_t        nLength;            /*!< The length of the data */
} ADI_NETWORK_ETH_FRM_DATA;

/*! @brief Network layer measurements */
typedef struct ADI_NETWORK_IF_MEAS_DATA
{
    uint32_t  nTxFrameCnt;              /*!< TX Frame count */
    uint32_t  nRxFrameCnt;              /*!< RX Frame count */
    uint64_t  nTxBytesCnt;              /*!< TX bytes count */
    uint64_t  nRxBytesCnt;              /*!< RX bytes count */
    double    fTxBandwidth;             /*!< TX BW utilization (in Mbps) */
    double    fRxBandwidth;             /*!< RX BW utilization (in Mbps) */
} ADI_NETWORK_IF_MEAS_DATA;

/*! @brief Network wake sleep data */
typedef struct ADI_NETWORK_WKSLP_DATA
{
    ADI_NETWORK_CTRL_STATES eNodeState; /*!< Current state of the controller node */
    ADI_NETWORK_CTRL_CTX eNodeContext;  /*!< Current context of the controller node */
    bool bGotoSleep;                    /*!< Is sleep request pending? */
    bool bGotoStandby;                  /*!< Is standby request pending? */
    bool bConfirmAwakeInt;              /*!< Has confirm awake interrupt been asserted? */
    bool bReqSysWakeupConfInt;          /*!< Has request wakeup confirmation interrupt been asserted? */
    bool bReturnedFromStandbyInt;       /*!< Has returned from standby interrupt been asserted? */
    bool bLineBusyInt;                  /*!< Has line-busy interrupt been asserted? */
    bool bStandbyErrInt;                /*!< Has standby error interrupt been asserted? */
    bool bSleepErrInt;                  /*!< Has sleep error interrupt been asserted? */
    bool bPingCheckEn;                  /*!< Ping check is enabled */
    bool bPingRcvd;                     /*!< Is ping response received? */
    bool bOaspiErr;                     /*!< Is OA-SPI interface in error state? */
    uint64_t nStartTimeNs;              /*!< Start time for checking timeouts */
} ADI_NETWORK_WKSLP_DATA;

/*! @brief Information about the SSC States */
typedef struct ADI_NETWORK_SSC_DATA
{
    bool bSafeMode;                     /*!< Indicates if the node is safe state */
    bool bUnsuccessfulreturnToFunc;     /*!< Sets when there is an unsuccessful return to functional state from safe state occurs, due to some safe state triggers that are still remaining */
    bool bReturnToFunc;                 /*!< Indicates if the node is returning to functional state */
    bool bFuncMode;                     /*!< Indicates if the node is in functional state */
} ADI_NETWORK_SSC_DATA;

/*! @brief Information about the SQI level */
typedef struct ADI_NETWORK_SQI_DATA
{
    uint8_t nSqiSelPlcaId;                /*!< Monitored plca ids for sqi */
    bool bSQIGoodLvl;                     /*!< Indicates if the SQI level has come back to good state */
    bool bSQIWarningLvl;                  /*!< Indiactes if the SQI level is in warning state */
    bool bSQIBadLvl;                      /*!< Indicates if the SQI level is in bad state */
    bool bInGoodLvl;                      /*!< Indicates if the SQI level is in good state */
} ADI_NETWORK_SQI_DATA;

/*! Information regarding topology discovery - used internally */
typedef struct ADI_NETWORK_TOPO_DISC_DATA
{
    uint8_t nTopoMeasDuration; /*!< Indicates how long a topology discovery measurement takes place - takes a values from 0 to 15ms (0->1) */
    bool bIsTopoRefNode;       /*!< Indicates if MAC-PHY is the reference node in topology discovery, if set false - it acts like a measured node */
    bool bGetDelayResult;      /*!< Indicates if application has requested internal delay result */
    bool bGetDistanceResult;   /*!< Indicates if application has requested distance result */
} ADI_NETWORK_TOPO_DISC_DATA;

/*! @brief MAC-PHY Revision number structure */
typedef struct ADI_NETWORK_MACPHY_REV_NUM
{
    uint32_t nMajorRevNum;            /*!< Major revision number */
    uint32_t nMinorRevNum;            /*!< Minor revision number */
} ADI_NETWORK_MACPHY_REV_NUM;

/*! @brief Information about the devices for opening */
typedef struct ADI_NETWORK_DEVINFO
{
    uint32_t nId;                       /*!< Device index */
    char sName[ADI_NETWORK_DEV_NAME_MAX_SIZE];  /*!< Device name */
    char sDesc[ADI_NETWORK_DEV_NAME_MAX_SIZE];  /*!< Device description */
    ADI_NETWORK_TYPE eNwType;           /*!< Device type */
    uint32_t nCtrlIdx;                  /*!< The index of the controller when multiple instances are opened */
    bool bIsOpened;                     /*!< Is the device opened flag */
    bool bWkslpHistoryCheckEn;          /*!< Flag indicating if wake sleep check has been enabled */
    volatile bool bInErrState;          /*!< Flag indicating device in error state */
    ADI_NETWORK_WKSLP_DATA oWkslpData;  /*!< Wake sleep data */
#ifdef MACPHY_EN
    uint8_t       nChunkSize;           /*!< Configured OA-SPI chunk size */
    volatile bool bSpiComplete;         /*!< SPI completion status */
    volatile bool bInitComplete;        /*!< Init completion status */
    volatile bool bIrqAsserted;         /*!< OA-SPI IRQ asserted */

    volatile bool bReadCompleted;       /*!< Flag indicating register read completion */
    volatile bool bIsPlcaEn;            /*!< Flag indicating if PLCA has been enabled */
    volatile bool bInResetSequence;     /*!< Flag indicating if reset sequence is in progress */

    ADI_REASON_FOR_WKSLP_DATA oData;    /*!< Reason for wake / sleep data */
    ADI_NETWORK_SSC_DATA oSscData;      /*!< SSC data*/
    ADI_NETWORK_SQI_DATA oSqiData;      /*!< SQI level data */
    ADI_NETWORK_MACPHY_REV_NUM oRevNumber; /*!< Revision number */
    ADI_NETWORK_TOPO_DISC_DATA oTopoDiscData; /*!< Topology discovery data - for internal use */
    uint8_t nBootloaderStatus[ADI_NETWORK_MACPHY_BOOTLOADER_BLOCKS]; /*!< Bootloader status for HV and LV die blocks */
    bool bIsGPTPConfigRdRecv; /*!< Indicates if the gPTP config register is readback */
#endif /* MACPHY_EN */
    bool     bEnMeas;                   /*!< Enable measurement  */
    uint64_t nMeasStartTime;            /*!< Network layer measurement start time */
    uint64_t nMeasEndTime;              /*!< Network layer measurement end time */
    ADI_NETWORK_IF_MEAS_DATA oMeas;     /*!< Network layer measurements */
} ADI_NETWORK_DEVINFO;

/*! @brief Structure for getting information about the frame */
typedef struct ADI_NETWORK_FRAME_CFG
{
    bool        bLenInclMacHdr;         /*!< Does the frame already have the MAC header along with the payload? */
    bool        bDuplicate;             /*!< Duplicate the frame across all the open network devices */
    uint16_t    nFrameType;             /*!< The ethertype of the ethernet frame */
    uint16_t    nLenByte;               /*!< The length of the frame */
    const uint8_t *panPhysAddr;         /*!< The destination address */
    ADI_EGRESS_TS_REG eTsReg;           /*!< To which egress register should the timestamp be captured? */
    uint8_t     nNumVlanTags;           /*!< Number of VLAN tags to be inserted */
    uint32_t    *panVlanTagArr;         /*!< Pointer to VLAN tag array */
    ADI_NETWORK_MACPHY_ROUTING_INFO eMacRoutingInfo; /*!< MAC-PHY Destination port to which the MAC frame is to be routed */
} ADI_NETWORK_FRAME_CFG;

#ifdef ADI_SERDES_MODE
typedef struct ADI_NETWORK_SERDES_TRANSMIT_CFG
{
    uint8_t *pBuffer;                   /*!< Pointer to the data buffer */
    uint16_t    nLenByte;               /*!< The length of the frame */
    ADI_EGRESS_TS_REG eTsReg;           /*!< To which egress register should the timestamp be captured? */
#ifdef MACPHY_EN
    /* Use these fields only for MACPHY use cases */
    ADI_NETWORK_MACPHY_ROUTING_INFO eMacRoutingInfo; /*!< MAC-PHY Destination port to which the MAC frame is to be routed */
#endif
} ADI_NETWORK_SERDES_TRANSMIT_CFG;
#endif /* ADI_SERDES_MODE */

/*! @brief Structure for configuring PLCA IDs and count */
typedef struct ADI_NETWORK_MACPHY_PLCA_CFG
{
    bool        bEnabled;               /*!< Enable / disable PLCA */
    uint32_t    nNodeCount;             /*!< PLCA node count */
    uint8_t     anPLCAID[ADI_NETWORK_MACPHY_MAX_PLCA_ID];   /*!< Array of PLCA IDs to be assigned for this node */
    uint8_t     nPlcaIdEn;              /*!< Mask of PLCA IDs to be enabled */
} ADI_NETWORK_MACPHY_PLCA_CFG;

/*! @brief Structure for SSC module settings */
typedef struct ADI_NETWORK_MACPHY_SSC_MONITOR_CONFIG
{
    bool    bDetectBeaconAsEthTraffic;                               /*!< If set true, enables the monitoring of the beacon will be added to the Ethernet traffic check */
    uint8_t nSQISafeModeLvlTrigger;                                  /*!< The SQI level for which the safe mode has to triggered */
    uint32_t nMonitorEnable;                                         /*!< Sets which pins has to be monitored for safe mode */
    uint32_t nMonitorPolarity;                                       /*!< Sets the polarity of monitored pins for which the safe mode has to be triggered */
    uint32_t nSAIFSafeEn;                                            /*!< Sets whether the SA_IF pins have functional / safe mode configurations in safe state */
    uint32_t nSAIFConfig[ADI_SSC_SA_IF_CNT];                         /*!< Configurations of SA_IF pins in safe state */
    uint32_t nAutoReturnToFunc;                                      /*!< Enables auto return from safe mode, only available for SA_IF pins, GPTP_UNLOCK and Timeouts */
    uint32_t nEthFrameCheckTimeout;                                  /*!< Set the timeout for receiving valid ethernet frame */
    uint32_t nDeviceUnattendedTimeout;                               /*!< Set the timeout for device unattended feature */
} ADI_NETWORK_MACPHY_SSC_MONITOR_CONFIG;

/*! @brief Structure to configure MACPHY FIFO Size */
typedef struct ADI_NETWORK_MACPHY_FIFO_SIZE_MACPHY_CONFIG
{
    ADI_NETWORK_MACPHY_FIFO_SIZE eRXFIFOSize;                    /*!< FIFO Size of RX queue (RX FIFO[0]) */
    ADI_NETWORK_MACPHY_FIFO_SIZE eHighPriorityTXSize;            /*!< FIFO Size of TX High Priority queue (TX FIFO[0]) */
    ADI_NETWORK_MACPHY_FIFO_SIZE eLowPriorityTXSize;             /*!< FIFO Size of TX Low Priority queue (TX FIFO[1]) */
    ADI_NETWORK_MACPHY_FIFO_SIZE ePtpRxFifoSize;                 /*!< FIFO Size of GPTP RX queue (TX FIFO[2]) */
} ADI_NETWORK_MACPHY_FIFO_SIZE_MACPHY_CONFIG;

/*! @brief Structure to configure MACPHY FIFO Sizes in Dual Mode */
typedef struct ADI_NETWORK_MACPHY_FIFO_SIZE_DUAL_MODE_CONFIG
{
    ADI_NETWORK_MACPHY_FIFO_SIZE eT1sToOaspiSize;               /*!< The Size of T1S to OASPI FIFO (RX FIFO[0]) */
    ADI_NETWORK_MACPHY_FIFO_SIZE eLceToOaspiSize;               /*!< The Size of LCE to OASPI FIFO (RX FIFO[1]) */
    ADI_NETWORK_MACPHY_FIFO_SIZE eLceToT1sSize;                 /*!< The Size of LCE to T1S FIFO (TX FIFO[1]) */
    ADI_NETWORK_MACPHY_FIFO_SIZE eOaspiToT1sOrLceSize;          /*!< The Size of OASPI to T1S/LCE FIFO (TX FIFO[0]) */
    ADI_NETWORK_MACPHY_FIFO_SIZE eT1sToLceSize;                 /*!< The Size of T1S to LCE FIFO (TX FIFO[2]) */
} ADI_NETWORK_MACPHY_FIFO_SIZE_DUAL_MODE_CONFIG;

/*! Structure to configure SQI */
typedef ADI_SQI_CFG_DATA ADI_NETWORK_MACPHY_SQI_CFG;

/*! Standard callback type for all network events (Needs to be implemented by the application) */
typedef void (*ADI_NETWORK_CBK)(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void *pData);

/* ~~~~~~~~~~~~~~~~~ Event data types ~~~~~~~~~~~~~~~~~ */
/*! Data for #ADI_NETWORK_EVT_ETH10_RX_RECEIVED event */
typedef ADI_NETWORK_ETH_FRM_DATA ADI_NETWORK_EVT_ETH10_RX_RECEIVED_DATA;

/*! Data for #ADI_NETWORK_EVT_ETH_RX_RECEIVED event */
typedef ADI_NETWORK_ETH_FRM_DATA ADI_NETWORK_EVT_ETH_RX_RECEIVED_DATA;

/*! Data for #ADI_NETWORK_EVT_TX_SUCCESS event */
typedef ADI_NETWORK_ETH_FRM_DATA ADI_NETWORK_EVT_TX_SUCCESS_DATA;

/*! @brief Data for #ADI_NETWORK_EVT_ERR event */
typedef struct ADI_NETWORK_EVT_ERR_DATA
{
    const void      *pData;             /*!< Generic error data */
    ADI_NETWORK_ERR eErrType;           /*!< The type of error encountered */
} ADI_NETWORK_EVT_ERR_DATA;

/*! Data for #ADI_NETWORK_EVT_MAC_STATUS event */
typedef ADI_MAC_STATUS_DATA ADI_NETWORK_EVT_MAC_STATUS_DATA;

/*! Data for #ADI_NETWORK_EVT_PLCA_STATUS event */
typedef ADI_PLCA_STATUS_DATA ADI_NETWORK_EVT_PLCA_STATUS_DATA;

/*! Data for #ADI_NETWORK_EVT_PHY_STATUS event */
typedef ADI_PHY_STATUS_DATA ADI_NETWORK_EVT_PHY_STATUS_DATA;

/*! Data for #ADI_NETWORK_EVT_PCS_STATUS event */
typedef ADI_PCS_STATUS_DATA ADI_NETWORK_EVT_PCS_STATUS_DATA;

/*! Data for #ADI_NETWORK_EVT_PTP_STATUS event */
typedef ADI_PTP_STATUS_DATA ADI_NETWORK_EVT_PTP_STATUS_DATA;

/*! @brief Data for #ADI_NETWORK_EVT_READ_REG event */
typedef struct ADI_NETWORK_EVT_READ_REG_DATA
{
    uint8_t         nLength;            /*!< Number of registers read from */
    uint8_t         nMMS;               /*!< MMS (Memory map selector) of the registers */
    uint16_t        nStartAddr;         /*!< Read data start address */
    uint32_t        *pRegData;          /*!< Register read data (Application to provide memory) */
} ADI_NETWORK_EVT_READ_REG_DATA;

/*! Data for #ADI_NETWORK_EVT_WRITE_REG event */
typedef ADI_NETWORK_EVT_READ_REG_DATA ADI_NETWORK_EVT_WRITE_REG_DATA;

/*! @brief Data for #ADI_NETWORK_EVT_TS_CAPTD event */
typedef struct ADI_NETWORK_EVT_TS_CAPTD_DATA
{
    ADI_EGRESS_TS_REG   eChannel;       /*!< Egress register in which the Timestamp is captured */
    ADI_TS_DATA         *poTimestamp;   /*!< Egress timestamp data */
} ADI_NETWORK_EVT_TS_CAPTD_DATA;

/*! Data for #ADI_NETWORK_EVT_TS_EXT_CAPT event */
typedef ADI_TS_DATA ADI_NETWORK_EVT_TS_EXT_CAPT_DATA;

/*! Data for #ADI_NETWORK_EVT_PING_RESP event */
typedef void ADI_NETWORK_EVT_PING_RESP_DATA;

/*! Data for #ADI_NETWORK_EVT_AWAKE event */
typedef ADI_NETWORK_CTRL_STATES ADI_NETWORK_EVT_AWAKE_DATA;

/*! Data for #ADI_NETWORK_EVT_STANDBY event */
typedef void ADI_NETWORK_EVT_STANDBY_DATA;

/*! Data for #ADI_NETWORK_EVT_SLEEP event */
typedef void ADI_NETWORK_EVT_SLEEP_DATA;

/*! Data for #ADI_NETWORK_EVT_STANDBY_FAILED event */
typedef ADI_NETWORK_WKSLP_FAIL_STATUS ADI_NETWORK_EVT_STANDBY_FAILED_DATA;

/*! Data for #ADI_NETWORK_EVT_SLEEP_FAILED event */
typedef ADI_NETWORK_WKSLP_FAIL_STATUS ADI_NETWORK_EVT_SLEEP_FAILED_DATA;

/*! Data for #ADI_NETWORK_EVT_REASON_FOR_WKSLP event */
typedef ADI_REASON_FOR_WKSLP_DATA ADI_NETWORK_EVT_REASON_FOR_WKSLP_DATA;

/*! Data for SSC Status */
typedef ADI_SSC_STATUS_DATA ADI_NETWORK_EVT_SSC_STATUS_DATA;

/*! Data for #ADI_NETWORK_EVT_SSC_SAFE_STATE event */
typedef void ADI_NETWORK_EVT_SSC_SAFE_STATE_DATA;

/*! Data for #ADI_NETWORK_EVT_SSC_RETURN_TO_FUNC event */
typedef void ADI_NETWORK_EVT_SSC_RETURN_TO_FUNC_DATA;

/*! Data for #ADI_NETWORK_EVT_SSC_UNSUCCESSFUL_RETURN_TO_FUNC event */
typedef void ADI_NETWORK_EVT_SSC_UNSUCCESSFUL_RETURN_TO_FUNC_DATA;

/*! Data for #ADI_NETWORK_EVT_SSC_ALIVE_COUNTER_RB event */
typedef uint32_t ADI_NETWORK_EVT_SSC_ALIVE_COUNTER_RB_DATA;

/*! Data for #ADI_NETWORK_EVT_BOOTLOADER_DONE event */
typedef void ADI_NETWORK_EVT_BOOTLOADER_DONE_DATA;

/*! Data for #ADI_NETWORK_EVT_SQI_STATUS event */
typedef ADI_SQI_STATUS ADI_NETWORK_EVT_SQI_STATUS_DATA;

/*! Data for #ADI_NETWORK_EVT_SQI_LVL_RETURN_GOOD event */
typedef void ADI_NETWORK_EVT_SQI_LVL_RETURN_GOOD_DATA;

/*! Data for #ADI_NETWORK_EVT_SQI_LVL_WARNING event */
typedef void ADI_NETWORK_EVT_SQI_LVL_WARNING_DATA;

/*! Data for #ADI_NETWORK_EVT_SQI_LVL_BAD event */
typedef void ADI_NETWORK_EVT_SQI_LVL_BAD_DATA;

/*! Data for #ADI_NETWORK_EVT_REVISION_NUM event */
typedef ADI_NETWORK_MACPHY_REV_NUM ADI_NETWORK_EVT_REVISION_NUM_DATA;

/*! Data for #ADI_NETWORK_EVT_TOPO_DISC_DELAY_FAILED event */
typedef void ADI_NETWORK_EVT_TOPO_DISC_DELAY_FAILED_DATA;

/*! Data for #ADI_NETWORK_EVT_TOPO_DISC_DELAY_MEAS event */
typedef double ADI_NETWORK_EVT_TOPO_DISC_DELAY_MEAS_DATA;

/*! Data for #ADI_NETWORK_EVT_TOPO_DISC_DIST_FAILED event */
typedef void ADI_NETWORK_EVT_TOPO_DISC_DIST_FAILED_DATA;

/*! Data for #ADI_NETWORK_EVT_TOPO_DISC_DIST_MEAS event */
typedef double ADI_NETWORK_EVT_TOPO_DISC_DIST_MEAS_DATA;

/*! Data for #ADI_NETWORK_EVT_TOPO_DISC_AUTO_MEAS_FAILED event */
typedef void ADI_NETWORK_EVT_TOPO_DISC_AUTO_MEAS_FAILED_DATA;

/*! Data for #ADI_NETWORK_EVT_UNK_INT event */
typedef void ADI_NETWORK_EVT_UNK_INT_DATA;

/*! Data for #ADI_NETWORK_EVT_APP_VS_INT event */
typedef void ADI_NETWORK_EVT_APP_VS_INT_DATA;

#ifdef ADI_SERDES_MODE
/*! Data for #ADI_NETWORK_EVT_APP_REQUEST_BUFFER event */
typedef struct ADI_NETWORK_EVT_APP_REQUEST_BUFFER_DATA
{
    const uint8_t *panMacAddr;           /*!< The destination MAC address */
    uint8_t     *pBuffer;               /*!< Pointer to the buffer pointer */
    uint16_t    nLength;                 /*!< Length of the buffer requested */
    uint32_t    nVlanTag;                /*!< VLAN tag to be inserted */
    bool        bisVlanTagPresent;       /*!< Is VLAN tag to be inserted */
} ADI_NETWORK_EVT_APP_REQUEST_BUFFER_DATA;

/*! Data for #ADI_NETWORK_EVT_APP_FINISHED_SERIALIZATION event */
typedef struct ADI_NETWORK_EVT_APP_SERIALIZED_BUFFER_DATA
{
    const uint8_t *panMacAddr;          /*!< The destination MAC address */
    uint16_t   nLength;                 /*!< Length of the buffer to be transmitted */
} ADI_NETWORK_EVT_APP_SERIALIZED_BUFFER_DATA;
#endif /* ADI_SERDES_MODE */
/*============================= EXTERN PROTOTYPES ===========================*/

/* ~~~~~~~~~~~~~~~~~~~~~~ Base APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Initialize the network module */
extern void adi_network_init(void);

/* Terminate the network module */
extern void adi_network_terminate(void);

/* Register callback */
extern void adi_network_registerCallback(ADI_NETWORK_CBK pfCbk);

/* Get the total number of devices in the network */
extern void adi_network_getDevices(uint32_t **pnNumDevices, ADI_NETWORK_DEVINFO **paoDevInfo);

/* Open network device */
extern void adi_network_openDevice(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg);

/* Close network device */
extern void adi_network_closeDevice(uint32_t nDevNum);

/* Set the source MAC address (SW controller's MAC address) */
extern void adi_network_setControllerMacAddr(uint8_t *panCtrlMacAddr);

/* Reconfigure the node */
extern ADI_NETWORK_ERR adi_network_reconfigure(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg);

/* Write to the MAC-PHY controller registers */
extern ADI_NETWORK_ERR adi_network_writeControllerRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    uint32_t  *panRegData,
    ADI_NETWORK_CBK pfCbk
);

/* Read the MAC-PHY controller registers */
extern ADI_NETWORK_ERR adi_network_readControllerRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    ADI_NETWORK_CBK pfCbk
);

/* Performs (controller node) MAC-PHY register read and masked write at given bit postions */
extern ADI_NETWORK_ERR adi_network_maskedWriteControllerRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    uint32_t  *panRegData,
    uint32_t  *panRegMask
);

/* Get a buffer to transfer an ethernet frame */
extern ADI_NETWORK_ERR adi_network_getTxBuffer(
    uint32_t  nDevNum,
    uint8_t   **pBuffer,
    ADI_NETWORK_FRAME_CFG *poNwFrameCfg
);

/* Submit the filled ethernet frame for transmission */
extern void adi_network_transmit(
    uint32_t  nDevNum,
    ADI_NETWORK_FRAME_CFG *poNwFrameCfg
);

#ifdef ADI_SERDES_MODE
/* Submit the filled frame for actual transmission in serdes mode */
extern ADI_NETWORK_ERR adi_network_serdesTransmit(
    uint32_t  nDevNum,
    ADI_NETWORK_SERDES_TRANSMIT_CFG *poNwTransmitFrameCfg
);

/* Reset OASPI data descriptor queue */
extern void adi_network_serdesResetOaspiQueue(uint32_t nDevNum);
#endif /* ADI_SERDES_MODE */

/* Tick the network layer for processing events */
extern void adi_network_run(void);

/* Register a semaphore to be posted on async completion of SPI transfers */
extern void adi_network_registerAsyncSemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore);
/* ~~~~~~~~~~~~~~~~~~~~~~ Status APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Get MAC status */
extern ADI_NETWORK_ERR adi_network_getMacStatus(uint32_t nDevNum);

/* Get PLCA status */
extern ADI_NETWORK_ERR adi_network_getPlcaStatus(uint32_t nDevNum);

/* Get PHY Status */
extern ADI_NETWORK_ERR adi_network_getPhyStatus(uint32_t nDevNum);

/* Get PCS Status */
extern ADI_NETWORK_ERR adi_network_getPcsStatus(uint32_t nDevNum);

/* Get PTP status  */
extern ADI_NETWORK_ERR adi_network_getPtpStatus(uint32_t nDevNum);

/* ~~~~~~~~~~~~~~~~~~~~~~ Reset APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Reset the controller node */
extern ADI_NETWORK_ERR adi_network_controllerMacReset(uint32_t nDevNum);

/* Reset the controller's HV die */
extern ADI_NETWORK_ERR adi_network_controllerHVDieReset(uint32_t nDevNum);

/* Reset the controller's LV die */
extern ADI_NETWORK_ERR adi_network_controllerLVDieReset(uint32_t nDevNum);

/* ~~~~~~~~~~~~~~~~~~~~~~ Buffer flush APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Flush transmit buffers (Blocks till MAC-PHY transmit FIFO is empty) */
extern void adi_network_flushTxBuffer(uint32_t nDevNum, uint64_t nTimeout);

/* Flush transmit buffers for the time specified */
extern void adi_network_flushTxTimed(uint64_t nTimeout);

/* ~~~~~~~~~~~~~~~~~~~~~~ Filter table APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Sets the MAC-PHY filter table */
extern ADI_NETWORK_ERR adi_network_setMacAddrFilterTable(uint32_t nDevNum, uint8_t anMacAddr[][6u], uint8_t nNumAddr);

/* Clears the MAC-PHY filter table */
extern ADI_NETWORK_ERR adi_network_clearMacAddrFilterTable(uint32_t nDevNum);

/* Enables VLAN Filtering */
extern ADI_NETWORK_ERR adi_network_setVlanFilter(uint32_t nDevNum, uint16_t nVlanId1, uint16_t nVlanId2,
        bool bDropFrames, bool bEnable);

/* Enables EtherType filtering */
extern ADI_NETWORK_ERR adi_network_setEtherTypeFilter(uint32_t nDevNum, uint16_t nEtherType1,
        uint16_t nEtherType2, bool bDropFrames, bool bEnable);

/* ~~~~~~~~~~~~~~~~~~~~~~ Wake-sleep APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Send controller node to sleep */
extern ADI_NETWORK_ERR adi_network_sleepControllerNode(uint32_t nDevNum);

/* Send controller node to standby */
extern ADI_NETWORK_ERR adi_network_standbyControllerNode(uint32_t nDevNum);

/* Send a wake-up pulse from controller node */
extern ADI_NETWORK_ERR adi_network_sendWakeUpPulse(uint32_t nDevNum);

/* Trigger wake-up pin of controller node */
extern ADI_NETWORK_ERR adi_network_triggerWakeUpPin(uint32_t nDevNum);

/* Get current state of controller node (Awake / standby / sleep) */
extern void adi_network_getControllerState(uint32_t nDevNum, ADI_NETWORK_CTRL_STATES **peCtrlState);

/* Notify controller node is in awake / standby / sleep state in case OTP is not programmed */
extern void adi_network_notifyControllerNodeState(uint32_t nDevNum, ADI_NETWORK_CTRL_STATES eCtrlState);

/* Get reason for sleep / wake */
extern ADI_NETWORK_ERR adi_network_getReasonForWkslp(uint32_t nDevNum);

/* ~~~~~~~~~~~~~~~~~~~~~~ Safe State Controller APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Sets the configuration required for the safe mode,
which are enabling the monitoring of pins, polarity, configuration of SA_IF pins in safe mode,
and auto return settings */
extern ADI_NETWORK_ERR adi_network_sscSetConfig(uint32_t nDevNum,
        const ADI_NETWORK_MACPHY_SSC_MONITOR_CONFIG *poSscSetupCfg);

/* Manual return to functional state from safe state */
extern ADI_NETWORK_ERR adi_network_sscReturnToFunctional(uint32_t nDevNum);

/* Force safe state mode */
extern ADI_NETWORK_ERR adi_network_sscForceSafeState(uint32_t nDevNum, bool bForceSafeState);

/* Alive counter readback */
extern ADI_NETWORK_ERR adi_network_sscAliveCounterRead(uint32_t nDevNum);

/* Check the safe state controller status */
extern ADI_NETWORK_ERR adi_network_sscCheckSafeModeStatus(uint32_t nDevNum);

/* Writes to the device unattended bit, if not written with in timeout - triggers safe state */
extern ADI_NETWORK_ERR adi_network_sscMarkDeviceAttended(uint32_t nDevNum);

/* ~~~~~~~~~~~~~~~~~~~~~~ PLCA APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/* Set plca for the controller node */
extern ADI_NETWORK_ERR adi_network_setPlca(uint32_t nDevNum, bool bEnabled, uint32_t nNodeCount, uint32_t nNodeId);

/* Set Multiple plca for the controller node */
extern ADI_NETWORK_ERR adi_network_setPlcaMulti(uint32_t nDevNum, const ADI_NETWORK_MACPHY_PLCA_CFG *poPlcaCfg);

/*Sets PLCA leader mode by performing MAC-PHY register read and masked write at given bit postions*/
extern ADI_NETWORK_ERR adi_network_setPlcaLeaderMode(uint32_t nDevNum, bool bPlcaLeader, bool bPlcaLeaderModeEn);

/* Enables the PLCA precedence mode by performing MAC-PHY register read and masked write at given bit postions */
extern ADI_NETWORK_ERR adi_network_enablePlcaPrecedence(uint32_t nDevNum, bool bPlcaPrecedenceEn);

/* Sets PLCA transmit opportunity timer */
extern ADI_NETWORK_ERR adi_network_setPlcaToTimer(uint32_t nDevNum, uint32_t nToTime);

/* Sets PLCA burst timer */
extern ADI_NETWORK_ERR adi_network_setPlcaBurstTimer(uint32_t nDevNum, uint8_t nMaxBurstCount, uint8_t nBurstTime);

/* ~~~~~~~~~~~~~~~~~~~~~~ Loopback APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Enables MAC Loopback */
extern ADI_NETWORK_ERR adi_network_enableMacLoopBack(uint32_t nDevNum, bool bMacLoopback);

/* Enables PMA Loopback */
extern ADI_NETWORK_ERR adi_network_enablePmaLoopBack(uint32_t nDevNum, bool bPmaLoopback);

/* Enables PCS Loopback */
extern ADI_NETWORK_ERR adi_network_enablePcsLoopBack(uint32_t nDevNum, bool bPcsLoopback);

/* ~~~~~~~~~~~~~~~~~~~~~~ Timestamp / timer APIs ~~~~~~~~~~~~~~~~~~~~~~ */

// TODO: Implement once code to sync MAC-PHY time and uC time is finalized
#if 0
/*! Get the current network time */
extern void adi_network_getCurrTime(uint32_t nDevNum, ADI_TS_DATA *poTsData);
#endif

/* Capture external timestamp */
extern ADI_NETWORK_ERR adi_network_captureExtTimestamp(uint32_t nDevNum);

/* Enable TS_TIMER output */
extern ADI_NETWORK_ERR adi_network_enableTsTimer(uint32_t nDevNum, uint32_t nFreq,
        uint8_t nDutyCycle, uint32_t nStartTime);

/* Stop toggling the TS_TIMER output */
extern ADI_NETWORK_ERR adi_network_stopTsTimer(uint32_t nDevNum);

/* Enable 1 PPS signal */
extern ADI_NETWORK_ERR adi_network_enablePpsOutput(uint32_t nDevNum, uint8_t nSAIF);

/* ~~~~~~~~~~~~~~~~~~~~~~ SQI APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/* Configure thresholds and select the node id for SQI */
extern ADI_NETWORK_ERR adi_network_configureSqi(uint32_t nDevNum,
        const ADI_NETWORK_MACPHY_SQI_CFG *poSqiCfg);

/* Gets SQI index */
extern ADI_NETWORK_ERR adi_network_getSqiIndex(uint32_t nDevNum);

/* Reset SQI */
extern ADI_NETWORK_ERR adi_network_disableSqi(uint32_t nDevNum);

/* ~~~~~~~~~~~~~~~~~~~~~~ Topology Discovery APIs ~~~~~~~~~~~~~~~~~~~~~~ */
/* Set time duration for topology discovery measurements */
extern void adi_network_setTopoDiscDuration(uint32_t nDevNum, uint8_t nTimeDuration);

/* Set the reference/measured node in the measurement */
extern ADI_NETWORK_ERR adi_network_setTopoDiscRefNode(uint32_t nDevNum, bool bIsRefNode);

/* Performs internal measurement delay */
extern ADI_NETWORK_ERR adi_network_startDelayMeas(uint32_t nDevNum);

/* Get internal delay result */
extern ADI_NETWORK_ERR adi_network_getDelayMeas(uint32_t nDevNum);

/* Performs distance measurement */
extern ADI_NETWORK_ERR adi_network_startDistanceMeas(uint32_t nDevNum);

/* Get distance measurement result */
extern ADI_NETWORK_ERR adi_network_getDistanceMeas(uint32_t nDevNum);

/* Performs topology discovery in automatic mode */
extern ADI_NETWORK_ERR adi_network_startAutoMeas(uint32_t nDevNum);

/* Clears TD enable bit */
extern ADI_NETWORK_ERR adi_network_clearTDBit(uint32_t nDevNum);
/* ~~~~~~~~~~~~~~~~~~~~~~ Misc APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Get Revision number */
extern ADI_NETWORK_ERR adi_network_getRevision(uint32_t nDevNum);

/* Enables the test mode control */
extern ADI_NETWORK_ERR adi_network_enableTestMode(uint32_t nDevNum, ADI_NETWORK_TESTMODE eTestMode);

/* Sets PMA Control */
extern ADI_NETWORK_ERR adi_network_setPmaControl(uint32_t nDevNum, bool bPmaReset, bool bTransmitDisable);

/* Ping controller node */
extern ADI_NETWORK_ERR adi_network_pingControllerNode(uint32_t nDevNum);

/* Set MAC FIFO(s) size */
extern ADI_NETWORK_ERR adi_network_setMacFIFOSize_Macphy(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_MACPHY_CONFIG *poFIFOSize);

/*! Set MAC FIFO(s) size */
extern ADI_NETWORK_ERR adi_network_setMacFIFOSize_DualMode(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_DUAL_MODE_CONFIG *poFIFOSize);

/*! Get Bootloader status */
extern ADI_NETWORK_ERR adi_network_getBootLoaderStatus(uint32_t nDevNum);

/* Releases the mutex in the network layer */
extern ADI_NETWORK_ERR adi_network_releaseMutex(void);

/* Enables/disables gPTP initial rate correction */
extern ADI_NETWORK_ERR adi_network_setGPTPInitialRateCorrection(uint32_t nDevNum, bool bEnable);
/* ~~~~~~~~~~~~~~~~~~~~~~ VLAN APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Fills VLAN tag(s) in the provided buffer location */
extern void adi_network_fillVlanTags(uint8_t *pBuf, ADI_VLAN_CFG *pVlanCfgArr, uint8_t nNumCfgs,
                                     uint16_t *pnSize);
/* Extracts VLAN tag(s) from the buffer */
extern void adi_network_extractVlanTags(uint8_t *pBuf, uint8_t nMaxVlanCfgs, ADI_VLAN_CFG *pVlanCfgArr,
                                        uint8_t *pnNumCfgs, uint16_t *pnSize);

/* ~~~~~~~~~~~~~~~~~~~~~~ Network layer measurement APIs ~~~~~~~~~~~~~~~~~~~~~~ */

/* Start network layer measurement */
extern void adi_network_startMeasurement(uint32_t nDevNum);

/* Stop network layer measurement */
extern void adi_network_stopMeasurement(uint32_t nDevNum);

/* Get the current network layer measurement data */
extern void adi_network_getMeasurement(uint32_t nDevNum, ADI_NETWORK_IF_MEAS_DATA **pNetworkMeas);

/* Log the network layer measurement data */
extern void adi_network_logMeasurement(uint32_t nDevNum);

#endif /* ADI_NETWORK_H */

/** @} */

/**
 * EOF: www.analog.com
 */
