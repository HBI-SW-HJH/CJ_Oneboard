/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_app_utils.h
 * \brief: The application utilities
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */
#ifndef ADI_APP_UTILS_H
#define ADI_APP_UTILS_H

/*================================= INCLUDES ================================*/
#include "adi_common.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif

#include "adi_dbg.h"
#include "adi_memmap.h"
#include "adi_network.h"

#ifdef USE_EAL
#include "adi_eal.h"
#include "adi_eal_diag.h"
#endif

#ifdef USE_CLI
#include "app_cli.h"
#endif /* USE_CLI */
/*================================= DEFINES =================================*/
// App modes
#define APP_MODE_RUN                (0U)
#define APP_MODE_CMD                (1U)

// App commands
#define APP_CMD_NONE                (0U)
#define APP_CMD_SWITCHING_MODE      (1U)
#define APP_CMD_HELP                (2U)
#define APP_CMD_BLANK               (3U)
#define APP_CMD_INVALID             (4U)
#define APP_CMD_PING_REMOTE         (5U)
#define APP_CMD_CONTROLLER_STAT     (6U)
#define APP_CMD_REMOTE_STAT         (7U)
#define APP_CMD_PTP_STAT            (8U)
#define APP_CMD_INTF_STAT           (9U)
#define APP_CMD_WRITE_REG           (10U)
#define APP_CMD_READ_REG            (11U)
#define APP_CMD_SSC_STAT            (12U)
#define APP_CMD_SERVER              (13U)
#define APP_CMD_CLIENT              (14U)
#define APP_CMD_PLCA_SET            (15U)
#define APP_CMD_PLCA_DIS            (16U)
#define APP_CMD_MAC_SET             (17U)
#define APP_CMD_IP_SET              (18U)
#define APP_CMD_DHCP                (19U)

#ifdef USE_EAL
/*! Default timeout (in nanoseconds) for GPIO register read operations */
#define GPIO_REG_TIMEOUT_NS         (100U * MS_TO_NS)
/*! Max number of OTP blocks (avoids dependency on adi_otp.h) */
#define APP_OTP_MAX_NUM_BLOCKS      (64U)

/*! Return status for OTP corruption check */
typedef enum APP_OTP_CHECK_STATUS
{
    APP_OTP_CHECK_SUCCESS       = 0,  /*!< No corruption all blocks healthy */
    APP_OTP_CHECK_READ_FAILURE  = 1,  /*!< Could not read bootloader status registers */
    APP_OTP_CHECK_CORRUPTED     = 2,  /*!< One or more OTP blocks are corrupted */
    APP_OTP_CHECK_INVALID_PARAM = 3,  /*!< Invalid parameter (e.g. APP_ALL_REMOTES) */
    APP_OTP_CHECK_TIMEOUT       = 4,  /*!< Bootloader done polling timed out */
    APP_OTP_CHECK_NULL_PTR      = 5,  /*!< NULL pointer passed as parameter */
} APP_OTP_CHECK_STATUS;
#endif /* USE_EAL */

/** Console colors */
#define CONSOLE_BG_COLOR_RESET    (0u)
#define CONSOLE_BG_COLOR_BLACK    (40u)
#define CONSOLE_BG_COLOR_RED      (41u)
#define CONSOLE_BG_COLOR_GREEN    (42u)
#define CONSOLE_BG_COLOR_YELLOW   (43u)
#define CONSOLE_BG_COLOR_BLUE     (44u)
#define CONSOLE_BG_COLOR_MAGENTA  (45u)
#define CONSOLE_BG_COLOR_CYAN     (46u)
#define CONSOLE_BG_COLOR_WHITE    (47u)

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif

#define CTRL_R_KEY                  (18u)

/*================================ DATA TYPES ===============================*/
typedef enum APP_REMOTES
{
    APP_REMOTE_1 = 0,
    APP_REMOTE_2 = 1,
    APP_REMOTE_3 = 2,
    APP_REMOTE_4 = 3,
    APP_REMOTE_5 = 4,
    APP_REMOTE_6 = 5,
    APP_REMOTE_7 = 6,
    APP_REMOTE_8 = 7,
    APP_TOTAL_REMOTES, /* Total number of remote nodes */
    APP_ALL_REMOTES = 0xFFu,
} APP_REMOTES;

/** The MAC header */
typedef struct MAC_HDR
{
    uint64_t nDestMac;          /*!< The destination MAC address */
    uint64_t nSrcMac;           /*!< The source MAC address */
    uint16_t nVlanTag;          /*!< The VLAN tag (if present) */
    uint16_t nEtherType;        /*!< The EtherType of the packet */
    const uint8_t *pPayload;    /*!< Pointer to the payload */
    uint32_t nPayloadLen;       /*!< Length of the payload */
} MAC_HDR;

/* Prototypes */
extern void app_utils_init(void);
extern void app_utils_mac64To8(uint8_t *anMacAddr, uint64_t n64Mac);
extern void app_utils_parseEthHeader(const uint8_t *pData, const uint32_t nSize, MAC_HDR *pHdr);
extern void app_utils_addLineSeparator(bool bNextLinePrompt);
extern void app_utils_setConsoleBgColor(uint8_t color_code);
extern void app_utils_resetConsoleBgColor(void);
extern void app_utils_log(uint8_t *panTxData, uint32_t nSize);
extern void app_utils_setupConsole(void);
extern void app_utils_printHelpContents(void);
extern bool app_utils_consoleRun(uint32_t nDevNum, uint32_t *pnAppCmd, uint32_t *pnAppMode);
extern bool app_utils_parseRxCommand(uint8_t *anRxData, uint32_t nLength, uint32_t *pnAppCmd, uint32_t *pnAppMode);
extern void app_utils_processRxByte(uint8_t nRxByte);
extern void app_utils_logCtrlMacStatus(ADI_NETWORK_EVT_MAC_STATUS_DATA *poStatusDat);
extern void app_utils_logCtrlPhyStatus(ADI_NETWORK_EVT_PHY_STATUS_DATA *poStatusDat);
extern void app_utils_logCtrlPlcaStatus(ADI_NETWORK_EVT_PLCA_STATUS_DATA *poStatusDat);
extern void app_utils_logCtrlPtpStatus(ADI_NETWORK_EVT_PTP_STATUS_DATA *poStatusDat);
extern void app_utils_logCtrlPcsStatus(ADI_NETWORK_EVT_PCS_STATUS_DATA *poStatusDat);
extern void app_utils_logCtrlRegRead(ADI_NETWORK_EVT_READ_REG_DATA *poRegDat);
extern void app_utils_convertVlanTags(uint32_t *panVlanTags, ADI_VLAN_CFG *aoVlanCfgs, uint32_t nNumTags);

#ifdef USE_EAL
extern void app_utils_logPingRcvd(uint32_t nRemoteNum);
extern void app_utils_logRemotePlcaStatus(ADI_PLCA_STATUS_DATA *poStatusDat, uint32_t nRemoteNum);
extern void app_utils_logRemoteMacStatus(ADI_MAC_STATUS_DATA *poStatusDat, uint32_t nRemoteNum);
extern void app_utils_logRemotePhyStatus(ADI_PHY_STATUS_DATA *poStatusDat, uint32_t nRemoteNum);
extern void app_utils_logRemotePtpStatus(ADI_PTP_STATUS_DATA *poStatsDat, uint32_t nRemoteNum);
extern void app_utils_logRemotePcsStatus(ADI_PCS_STATUS_DATA *poStatsDat, uint32_t nRemoteNum);
extern APP_OTP_CHECK_STATUS app_utils_readOtpBootStatus(uint32_t nInstNum, APP_REMOTES eRemote,
                                                        uint8_t anBlockStatus[APP_OTP_MAX_NUM_BLOCKS]);
extern APP_OTP_CHECK_STATUS app_utils_checkOtpBootBlockStatus(uint32_t nInstNum, APP_REMOTES eRemote);

extern ADI_EAL_STATUS app_utils_gpioConfigurePins(uint32_t nInstNum, APP_REMOTES eRemote, uint32_t nPinMask, bool bEnable);
extern ADI_EAL_STATUS app_utils_gpioReadPin(uint32_t nInstNum, APP_REMOTES eRemote, uint32_t *pnPinValue);
extern ADI_EAL_STATUS app_utils_gpioSetPin(uint32_t nInstNum, APP_REMOTES eRemote, uint32_t nPinMask, uint32_t nPinValue);

#endif /* USE_EAL */

/* Callback prototype */
extern void app_utils_rxCmdCbk(uint8_t *anRxData, uint32_t nLength);

#endif /* ADI_APP_UTILS_H */

/** @} */

/*
 * EOF: https://www.analog.com/
*/
