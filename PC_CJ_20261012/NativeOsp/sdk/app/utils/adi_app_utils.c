/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_app_utils.c
 * \brief: The application utilities
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "adi_app_utils.h"

/*================================= EXTERNS =================================*/
#if defined(USE_EAL) && defined(ADI_E2B_IFACE_DIAG_ENABLED) && !defined(ADI_APP_SKIP_REMOTE_STAT)
extern ADI_E2BCORE_CFG oEth10BaseT1sNtwrkCfg;
#endif

/*================================= DEFINES =================================*/
/**
 * Max length of the UART RX buffer.
 * If the commands are longer than 20 characters, increase this size!
 */
#define ADI_MAX_RX_COMMAND_LEN    (50u)

#ifdef USE_EAL
/*! Number of bootloader status registers to read (2 HV + 8 LV) */
#define APP_OTP_BOOT_STATUS_NUM_REGS     (10U)
/*! Timeout for bootload_done polling */
#define APP_OTP_BOOTDONE_TIMEOUT_MS      (1000U)
/*! Poll interval in microseconds (3200 us aligns with bootloader register update cadence) */
#define APP_OTP_BOOTDONE_POLL_INTERVAL_US (3200U)
/*! Timeout for individual register reads (ms) */
#define APP_OTP_REG_READ_TIMEOUT_MS      (100U)

/*! Block status values (mirrors ADI_OTP_BLOCK_STATUS in adi_otp.h) */
#define APP_OTP_BLK_UNINITIALIZED  (0U)
#define APP_OTP_BLK_VALID          (1U)
#define APP_OTP_BLK_RECOVERABLE    (2U)
#define APP_OTP_BLK_CORRUPTED      (3U)

/*! Block-status decode constants */
#define APP_OTP_BITS_PER_BLOCK        (2U)
#define APP_OTP_BLK_STATUS_MASK       (0x03U)
#define APP_OTP_BLOCKS_PER_REG        (8U)
#define APP_OTP_LAST_REG_NUM_BLOCKS   (5U)

/*! Summary counters for OTP block classification */
typedef struct APP_OTP_BLOCK_SUMMARY
{
    uint32_t nValid;        /*!< Number of valid blocks */
    uint32_t nRecoverable;  /*!< Number of recoverable (ECC-corrected) blocks */
    uint32_t nCorrupted;    /*!< Number of corrupted blocks */
    uint32_t nEmpty;        /*!< Number of uninitialized blocks */
} APP_OTP_BLOCK_SUMMARY;

/*! Bootloader status register addresses: 2 HV + 8 LV */
static const uint16_t ganOtpBootStatusRegAddr[APP_OTP_BOOT_STATUS_NUM_REGS] =
{
    IO_HV_BOOTLOADER_STATUS_0_ADDR,   /* 0xB106 */
    IO_HV_BOOTLOADER_STATUS_1_ADDR,   /* 0xB10A */
    IO_BOOTLOADER_STATUS_0_ADDR,      /* 0xC806 */
    IO_BOOTLOADER_STATUS_1_ADDR,      /* 0xC807 */
    IO_BOOTLOADER_STATUS_2_ADDR,      /* 0xC808 */
    IO_BOOTLOADER_STATUS_3_ADDR,      /* 0xC809 */
    IO_BOOTLOADER_STATUS_4_ADDR,      /* 0xC80A */
    IO_BOOTLOADER_STATUS_5_ADDR,      /* 0xC80B */
    IO_BOOTLOADER_STATUS_6_ADDR,      /* 0xC80C */
    IO_BOOTLOADER_STATUS_7_ADDR       /* 0xC80D */
};
#endif /* USE_EAL */

/*================================ DATA TYPES ===============================*/

/*=================================== DATA ==================================*/
/* Storage for accumulating RX data till '\n' is entered */
ADI_MEM_DATA_CRIT_NOCACHE
static uint8_t ganRxParsedData[ADI_MAX_RX_COMMAND_LEN];

/* Storage for RX accumulated byte count */
ADI_MEM_DATA_CRIT_NOCACHE
static uint32_t gnRxByteCount;

#ifdef _WIN32
static HANDLE stdoutHandle;
static DWORD outModeInit;
#endif
ADI_MEM_DATA_CRIT_CACHE
static uint16_t gnRegAddr;
ADI_MEM_DATA_CRIT_CACHE
static uint32_t gnRegData;
ADI_MEM_DATA_CRIT_CACHE
static uint8_t gnMMS;

/*================================ PROTOTYPES ===============================*/
static void ProcessRxByte(uint8_t nRxByte);
/*=================================== CODE ==================================*/
ADI_MEM_CODE_CRIT
void app_utils_init(void)
{
#ifdef USE_CLI
    app_cli_init();
#endif /* USE_CLI */
}

/**
 * @brief Allows functions to change from 64 to 8 bit MAC address
 *
 */
ADI_MEM_CODE_CRIT
void app_utils_mac64To8(uint8_t *anMacAddr, uint64_t n64Mac)
{
    anMacAddr[0U] = (uint8_t)((n64Mac & 0xFF0000000000ULL) >> 40U);
    anMacAddr[1U] = (uint8_t)((n64Mac & 0x00FF00000000ULL) >> 32U);
    anMacAddr[2U] = (uint8_t)((n64Mac & 0x0000FF000000ULL) >> 24U);
    anMacAddr[3U] = (uint8_t)((n64Mac & 0x000000FF0000ULL) >> 16U);
    anMacAddr[4U] = (uint8_t)((n64Mac & 0x00000000FF00ULL) >> 8U);
    anMacAddr[5U] = (uint8_t)((n64Mac & 0x0000000000FFULL));
}

/*! Parses the Ethernet frame */
/**
 * @brief Performs the initialization sequence for e2b network
 *
 */
ADI_MEM_CODE_CRIT
void app_utils_parseEthHeader(const uint8_t *pData, const uint32_t nSize, MAC_HDR *pHdr)
{
    /* Extract headers */
    pHdr->nDestMac =
        ((uint64_t)pData[0U] << 40U)
        | ((uint64_t)pData[1U] << 32U)
        | ((uint64_t)pData[2U] << 24U)
        | ((uint64_t)pData[3U] << 16U)
        | ((uint64_t)pData[4U] << 8U)
        | ((uint64_t)pData[5U]);
    pHdr->nSrcMac =
        ((uint64_t)pData[6U] << 40U)
        | ((uint64_t)pData[7U] << 32U)
        | ((uint64_t)pData[8U] << 24U)
        | ((uint64_t)pData[9U] << 16U)
        | ((uint64_t)pData[10U] << 8U)
        | ((uint64_t)pData[11U]);

    /* Check if it's VLAN or not */
    if((pData[12U] == 0x81U) && (pData[13U] == 0x00U))
    {
        pHdr->nVlanTag = ((uint16_t)pData[14U] << 8U) | (uint16_t)(pData[15U]);
        pHdr->nEtherType = ((uint16_t)pData[16U] << 8U) | (uint16_t)(pData[17U]);
        pHdr->pPayload = &pData[18U];
        pHdr->nPayloadLen = nSize - 18U;
    }
    else
    {
        pHdr->nEtherType = ((uint16_t)pData[12U] << 8U) | (uint16_t)(pData[13U]);
        pHdr->pPayload = &pData[14U];
        pHdr->nPayloadLen = nSize - 14U;
    }
}

ADI_MEM_CODE_CRIT
void app_utils_addLineSeparator(bool bNextLinePrompt)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "---------------------------------------------------------------------------\r\n");

    if(bNextLinePrompt)
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
    }
}

/**
 * @brief Set background color for text on the console/terminal
 *
 */
ADI_MEM_CODE_CRIT
void app_utils_setConsoleBgColor(uint8_t color_code)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\x1b[%d;1m", color_code);
}

/**
 * @brief Reset background color for text on the console/terminal
 *
 */
ADI_MEM_CODE_CRIT
void app_utils_resetConsoleBgColor(void)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\x1b[%dm", CONSOLE_BG_COLOR_RESET);
}

ADI_MEM_CODE_CRIT
void app_utils_log(uint8_t *panTxData, uint32_t nSize)
{
#ifdef USE_CLI
    app_cli_log(panTxData, nSize);
#else
#if defined(LINUX) || defined(NNG) || defined(RPI3_RASPBIAN) || defined(WIN_X64)
    ADI_PRINTF("%s", (char *)panTxData);
    ADI_FFLUSH(stdout);
#else
    adi_pal_sendToCLI(panTxData, nSize);
#endif /* LINUX || NNG || RPI3_RASPBIAN || WIN_X64 */
#endif /* USE_CLI */
}

ADI_MEM_CODE_CRIT
void app_utils_setupConsole(void)
{
#ifdef _WIN32
    DWORD outMode = 0;
    stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(stdoutHandle == INVALID_HANDLE_VALUE)
    {
        exit(GetLastError());
    }

    if(!GetConsoleMode(stdoutHandle, &outMode))
    {
        exit(GetLastError());
    }

    outModeInit = outMode;

    /* Enable ANSI escape codes */
    outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if(!SetConsoleMode(stdoutHandle, outMode))
    {
        exit(GetLastError());
    }

#endif

    /* ResetConsoleBgColor(); */
}

ADI_MEM_CODE_CRIT
void app_utils_printHelpContents(void)
{
#ifndef USE_CLI
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " List of Supported Commands:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t'Ctrl + R' to toggle between command mode and run mode\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tType any of the following when in command mode: \r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\thelp                Prints list of supported application commands\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\tcontroller_stat     Reads controller node MAC and PHY status\r\n");

#ifdef USE_EAL
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\tping                Ping Remote Nodes\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\tremote_stat         Reads remote nodes\' MAC and PHY status\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\tintf_stat           Reads remote nodes\' interface status\r\n");
#endif

    app_utils_addLineSeparator(true);
#endif
}

ADI_MEM_CODE_CRIT
static void adi_network_cbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void *pData)
{
    ADI_DBG_REQUIRE((eEvent == ADI_NETWORK_EVT_READ_REG ||
                     eEvent == ADI_NETWORK_EVT_WRITE_REG), "Invalid event in adi_network_cbk");

    switch(eEvent)
    {
        case ADI_NETWORK_EVT_WRITE_REG:
        {
            ADI_NETWORK_EVT_WRITE_REG_DATA *pRegData = (ADI_NETWORK_EVT_WRITE_REG_DATA *)pData;
            break;
        }

        case ADI_NETWORK_EVT_READ_REG:
        {
            ADI_NETWORK_EVT_READ_REG_DATA *pRegData = (ADI_NETWORK_EVT_READ_REG_DATA *)pData;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRegister leng                       : %d\r\n", pRegData->nLength);
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRegister addr                       : 0x%04x\r\n", pRegData->nStartAddr);
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRegister  mms                       : %d\r\n", pRegData->nMMS);
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRegister data                       : 0x" UINT32_HEX_FORMATTER "\r\n",
                        pRegData->pRegData[0U]);
            app_utils_addLineSeparator(true);
            break;
        }

        default:
        {
            /* Nothing to do! */
            break;
        }
    }
}

ADI_MEM_CODE_CRIT
bool app_utils_consoleRun(uint32_t nDevNum, uint32_t *pnAppCmd, uint32_t *pnAppMode)
{
    bool bCmdProcessed = false;

#ifdef USE_CLI
    app_cli_run();
#else
    /* Poll for user input / RX data */
    adi_pal_cliRun();
#endif /* USE_CLI */

    switch(*pnAppCmd)
    {
        case APP_CMD_CONTROLLER_STAT:
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\nReading controller status (MAC, PHY, PLCA and PTP)\r\n");
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
            adi_network_getMacStatus(nDevNum);
            adi_network_getPhyStatus(nDevNum);
            adi_network_getPlcaStatus(nDevNum);
            adi_network_getPtpStatus(nDevNum);
            adi_network_getPcsStatus(nDevNum);
            bCmdProcessed = true;
            break;
        }

#ifdef USE_EAL

        case APP_CMD_PING_REMOTE:
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
            adi_eal_pingRemote(0U, 0xFFFFFFFFU);
            bCmdProcessed = true;
            break;
        }

        case APP_CMD_REMOTE_STAT:
        {
#if defined(ADI_E2B_IFACE_DIAG_ENABLED) && !defined(ADI_APP_SKIP_REMOTE_STAT)
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");

            for(uint32_t i = 0U; i < oEth10BaseT1sNtwrkCfg.nNumNodes; i++)
            {
                adi_eal_diagGetMacStatus(0U, i);
                adi_eal_diagGetPhyStatus(0U, i);
                adi_eal_diagGetPlcaStatus(0U, i);
                adi_eal_diagGetPtpStatus(0U, i);
                adi_eal_diagGetPcsStatus(0U, i);
            }

            bCmdProcessed = true;
#else
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT,
                        "\r\nRemote diagnostics interface not available (disabled at compile time)\r\n");
            bCmdProcessed = true;
#endif /* ADI_E2B_IFACE_DIAG_ENABLED && !ADI_APP_SKIP_REMOTE_STAT */
            break;
        }

#endif

        case APP_CMD_HELP:
        {
            /* Display help message */
            app_utils_printHelpContents();
            bCmdProcessed = false;
            break;
        }

        case APP_CMD_SWITCHING_MODE:
        {
            if(*pnAppMode == APP_MODE_CMD)
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n\r\nSwitching to command mode\r\n");
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
            }
            else
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n\r\nSwitching to run mode\r\n");
            }

            bCmdProcessed = true;

            break;
        }

        case APP_CMD_INVALID:
        {
            if(*pnAppMode != APP_MODE_RUN)
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\nInvalid Command\r\n");
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
            }

            bCmdProcessed = true;
            break;
        }

        case APP_CMD_BLANK:
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
            bCmdProcessed = true;
            break;
        }

        case APP_CMD_WRITE_REG:
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\nWriting register: 0x%4x, 0x%x, 0x" UINT32_HEX_FORMATTER "\r\n", gnRegAddr,
                        gnMMS,
                        gnRegData);
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
            (void)adi_network_writeControllerRegister(nDevNum, gnRegAddr, 1U, gnMMS, &gnRegData, &adi_network_cbk);
            bCmdProcessed = true;
            break;
        }

        case APP_CMD_READ_REG:
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\nReading register: 0x%4x, 0x%x\r\n", gnRegAddr, gnMMS);
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
            (void)adi_network_readControllerRegister(nDevNum, gnRegAddr, 1U, gnMMS, &adi_network_cbk);
            bCmdProcessed = true;
            break;
        }

        default:
        {
            break;
        }
    }

    return bCmdProcessed;
}

/* Parse RX command */
ADI_MEM_CODE_CRIT
bool app_utils_parseRxCommand(uint8_t *anRxData, uint32_t nLength, uint32_t *pnAppCmd, uint32_t *pnAppMode)
{
    bool bCmdProcessed = true;

    /**
     * Parse the command and change the state appropriately
     */
    if(anRxData[0u] == CTRL_R_KEY)
    {
        /* Change mode to command mode */
        if(*pnAppMode == APP_MODE_RUN)
        {
            *pnAppMode = APP_MODE_CMD;
            *pnAppCmd = APP_CMD_SWITCHING_MODE;
        }
        else
        {
            *pnAppMode = APP_MODE_RUN;
            *pnAppCmd = APP_CMD_SWITCHING_MODE;
        }
    }
    else if(*pnAppMode == APP_MODE_CMD)
    {
        char sCommand[10];
        char *sStr = (char *)anRxData;

        if(ADI_STRNCMP("help", (char *)anRxData, 4u) == 0 && ((char *)anRxData)[4] == '\0')
        {
            *pnAppCmd = APP_CMD_HELP;
        }
        else if((ADI_STRNCMP("controller_stat", (char *)anRxData, 15u) == 0 && ((char *)anRxData)[15] == '\0')
                || (ADI_STRNCMP("cs", (char *)anRxData, 2u) == 0 && ((char *)anRxData)[2] == '\0'))
        {
            *pnAppCmd = APP_CMD_CONTROLLER_STAT;
        }

#ifdef USE_EAL
        else if(ADI_STRNCMP("ping", (char *)anRxData, 4u) == 0 && ((char *)anRxData)[4] == '\0')
        {
            *pnAppCmd = APP_CMD_PING_REMOTE;
        }
        else if((ADI_STRNCMP("remote_stat", (char *)anRxData, 11u) == 0 && ((char *)anRxData)[11] == '\0')
                || (ADI_STRNCMP("rs", (char *)anRxData, 2u) == 0 && ((char *)anRxData)[2] == '\0'))
        {
            *pnAppCmd = APP_CMD_REMOTE_STAT;
        }
        else if((ADI_STRNCMP("intf_stat", (char *)anRxData, 9u) == 0 && ((char *)anRxData)[9] == '\0')
                || (ADI_STRNCMP("is", (char *)anRxData, 2u) == 0 && ((char *)anRxData)[2] == '\0'))
        {
            *pnAppCmd = APP_CMD_INTF_STAT;
        }

#endif
        else if(nLength == 0)
        {
            *pnAppCmd = APP_CMD_BLANK;
        }
        else if((sStr[0] == 'R' && sStr[1] == 'W') || (sStr[0] == 'r' && sStr[1] == 'w'))
        {
            ADI_SSCANF(sStr, "%s 0x%hx 0x%hx 0x" UINT32_HEX_FORMATTER, &sCommand[0U], &gnRegAddr, (uint16_t *)&gnMMS, &gnRegData);
            *pnAppCmd = APP_CMD_WRITE_REG;
        }
        else if((sStr[0] == 'R' && sStr[1] == 'R')  || (sStr[0] == 'r' && sStr[1] == 'r'))
        {
            ADI_SSCANF(sStr, "%s 0x%hx 0x%hx", &sCommand[0U], &gnRegAddr, (uint16_t *)&gnMMS);
            *pnAppCmd = APP_CMD_READ_REG;
        }
        else
        {
            bCmdProcessed = false;
        }
    }
    else
    {
        bCmdProcessed = false;
    }

    return bCmdProcessed;
}

/* Process RX byte */
ADI_MEM_CODE_CRIT
void app_utils_processRxByte(uint8_t nRxByte)
{
#ifdef USE_CLI
    app_cli_processRxByte(nRxByte);
#else
    ProcessRxByte(nRxByte);
#endif /* USE_CLI */
}

ADI_MEM_CODE_CRIT
void app_utils_logCtrlMacStatus(ADI_NETWORK_EVT_MAC_STATUS_DATA *poStatusDat)
{
#ifdef USE_CLI
    CMD_EXECUTE_FUNC pfExecMacStats = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_MAC_STATS];
    CMD_INST_MAC_STAT_RESP *poMacStatus = (CMD_INST_MAC_STAT_RESP *)apCmdObj[E_CLI_CMD_MAC_STATS];
    poMacStatus->bIsRemoteNode = false;
    poMacStatus->poMacStatsDat = poStatusDat;
    poMacStatus->eCmd = E_CLI_CMD_MAC_STATS;
    pfExecMacStats(poMacStatus);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Controller MAC status\r\n");
    adi_common_logMacStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logCtrlPhyStatus(ADI_NETWORK_EVT_PHY_STATUS_DATA *poStatusDat)
{
#ifdef USE_CLI
    CMD_EXECUTE_FUNC pfExecPhyStats = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_PHY_STATS];
    CMD_INST_PHY_STAT_RESP *poPhyStatus = (CMD_INST_PHY_STAT_RESP *)apCmdObj[E_CLI_CMD_PHY_STATS];
    poPhyStatus->bIsRemoteNode = false;
    poPhyStatus->poPhyStatsDat = poStatusDat;
    poPhyStatus->eCmd = E_CLI_CMD_PHY_STATS;
    pfExecPhyStats(poPhyStatus);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Controller PHY status\r\n");
    adi_common_logPhyStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logCtrlPlcaStatus(ADI_NETWORK_EVT_PLCA_STATUS_DATA *poStatusDat)
{
#ifdef USE_CLI
    /* TODO */
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Controller PLCA status\r\n");
    adi_common_logPlcaStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logCtrlPtpStatus(ADI_NETWORK_EVT_PTP_STATUS_DATA *poStatusDat)
{
#ifdef USE_CLI
    /* TODO */
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Controller PTP status\r\n");
    adi_common_logPtpStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logCtrlPcsStatus(ADI_NETWORK_EVT_PCS_STATUS_DATA *poStatusDat)
{
#ifdef USE_CLI
    /* CMD_EXECUTE_FUNC pfExecPcsStats = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_PCS_STATS];
     * CMD_INST_PCS_STAT_RESP *poPcsStatus = (CMD_INST_PCS_STAT_RESP *)apCmdObj[E_CLI_CMD_PCS_STATS];
     * poPcsStatus->bIsRemoteNode = false;
     * poPcsStatus->poPcsStatsDat = poStatusDat;
     * poPcsStatus->eCmd = E_CLI_CMD_PCS_STATS;
     * pfExecPcsStats(poPcsStatus); */
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Controller PCS status\r\n");
    adi_common_logPcsStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logCtrlRegRead(ADI_NETWORK_EVT_READ_REG_DATA *poRegDat)
{
#ifdef USE_CLI
    CMD_INST_READ_REG *poRead = (CMD_INST_READ_REG *)apCmdObj[E_CLI_CMD_READ_REG];

    if(poRead->bIsReadCtrlReg)
    {
        CMD_EXECUTE_FUNC pfRegReadResp = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_READ_REG_RESP];
        CMD_INST_READ_REG_RESP *poReadResp = (CMD_INST_READ_REG_RESP *)apCmdObj[E_CLI_CMD_READ_REG_RESP];

        poReadResp->eCmd = E_CLI_CMD_READ_REG_RESP;
        poReadResp->bIsReadCtrlReg = true;
        poReadResp->nMMS = poRegDat->nMMS;
        poReadResp->nStartAddr = poRegDat->nStartAddr;
        poReadResp->nLength = poRegDat->nLength;
        poReadResp->panData = poRegDat->pRegData;
        pfRegReadResp(poReadResp);
    }

#endif

}

ADI_MEM_CODE_CRIT
void app_utils_convertVlanTags(uint32_t *panVlanTags, ADI_VLAN_CFG *aoVlanCfgs, uint32_t nNumTags)
{
    for(uint32_t i = 0u; i < nNumTags; i++)
    {
        panVlanTags[i] = ((uint32_t)aoVlanCfgs[i].nTPID << 16U) |
                         ((uint32_t)(aoVlanCfgs[i].nPCP & 0x07U) << 13U) |
                         (aoVlanCfgs[i].bDEI ? 0x1000U : 0U) |
                         (aoVlanCfgs[i].nVlanId & 0xFFFU);
    }
}

#ifdef USE_EAL
/**
 * @brief Configure GPIO output override pins on a remote node via register transaction
 *
 * Uses a mask write on the HOST_IF_IO_OVERRIDE register to set or clear the
 * override-enable bits (bits [27:16]) for the pins selected by nPinMask,
 * without disturbing any other already-configured pins.
 *
 * Register layout (HOST_IF_IO_OVERRIDE, 0xD034):
 *   Bits [11: 0] - io_override_man    : manual output values (written by gpioSetPin)
 *   Bits [27:16] - io_override_man_en : override-enable per pin (written here)
 *
 * @param [in] nInstNum  EAL instance number
 * @param [in] eRemote   Remote node to configure
 * @param [in] nPinMask  Bitmask of pins to enable/disable as outputs
 *                       (non-zero, <= BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN,
 *                        covering SA_IF0..SA_IF11)
 * @param [in] bEnable   true  = enable output override for the selected pins
 *                       false = disable output override for the selected pins
 *                               (returns them to normal interface control)
 * @return ADI_EAL_STATUS_SUCCESS on success,
 *         ADI_EAL_STATUS_INVALID_PARAM if nPinMask is zero or out of range,
 *         or a register-access error code on failure.
 */
ADI_MEM_CODE_CRIT
ADI_EAL_STATUS app_utils_gpioConfigurePins(uint32_t nInstNum, APP_REMOTES eRemote, uint32_t nPinMask, bool bEnable)
{
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;
    uint16_t nOverrideAddr;
    uint32_t nMaskData;
    uint32_t nWriteData;
    ADI_E2BCORE_REGMAP_IO_CONFIG oWriteRegCfg;

    do
    {
        /* Validate remote index */
        if((uint32_t)eRemote >= ADI_EAL_MAX_NUM_NODES)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Invalid remote index %d (ConfigurePins): must be < %d\r\n",
                        (int)eRemote, (int)ADI_EAL_MAX_NUM_NODES);
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        if((nPinMask == 0U) || ((nPinMask & ~BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN) != 0U))
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Invalid pin mask 0x%04x (ConfigurePins): must be non-zero and <= 0x%04x\r\n",
                        (unsigned int)nPinMask, (unsigned int)BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN);
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        nOverrideAddr = HOST_IF_IO_OVERRIDE_NW_ADDR;

        /* Build mask targeting the override-enable bits [27:16] for the selected pins */
        nMaskData = nPinMask << BITP_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN_EN;

        /* Set enable bits to 1 (enable) or 0 (disable) */
        if(bEnable)
        {
            nWriteData = nMaskData;
        }
        else
        {
            nWriteData = 0U;
        }

        oWriteRegCfg.anStartAddr = &nOverrideAddr;
        oWriteRegCfg.nLength     = 1U;
        oWriteRegCfg.pBuffer     = &nWriteData;
        oWriteRegCfg.anMask      = &nMaskData;
        oWriteRegCfg.eType       = ADI_E2BCORE_RND_MASK_WRITE;

        eStatus = adi_eal_sendRegTransaction(nInstNum, eRemote, &oWriteRegCfg);

        if(eStatus == ADI_EAL_STATUS_SUCCESS)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Pin config %s: Remote%d, mask=0x%04x\r\n",
                        bEnable ? "enabled" : "disabled",
                        (int)eRemote + 1, (unsigned int)nPinMask);
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Pin config FAILED: Remote%d, status=%d\r\n",
                        (int)eRemote + 1, (int)eStatus);
        }
    } while(false);

    return eStatus;
}

/**
 * @brief Read all GPIO pin states from a remote node via register transaction
 *
 * Reads the IO_VIH_VIL_READBACKS register and returns the raw per-pin
 * bit values for all pins (bits [11:0]). The caller can inspect any pin
 * of interest from the returned value.
 *
 * Bit N of *pnPinValue is 1 if pin N is high, 0 if low.
 *
 * @param [in]  nInstNum    EAL instance number
 * @param [in]  eRemote     Remote node to read from
 * @param [out] pnPinValue  Receives the per-pin readback values (bits [11:0]).
 *                          Set to 0xFFFFFFFF when a non-NULL pointer is provided
 *                          but the read fails (outside valid 12-bit range).
 * @return ADI_EAL_STATUS_SUCCESS on success,
 *         ADI_EAL_STATUS_NULL_PTR if pnPinValue is NULL,
 *         ADI_EAL_STATUS_INVALID_PARAM if eRemote is out of range,
 *         or a register-access error code on failure.
 */
ADI_MEM_CODE_CRIT
ADI_EAL_STATUS app_utils_gpioReadPin(uint32_t nInstNum, APP_REMOTES eRemote, uint32_t *pnPinValue)
{
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;
    uint16_t nStartAddr;
    uint32_t nVihVilData;
    ADI_E2BCORE_REGMAP_IO_CONFIG oReadRegCfg;

    do
    {
        /* Validate output pointer */
        if(pnPinValue == NULL)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] NULL output pointer (ReadPin)\r\n");
            eStatus = ADI_EAL_STATUS_NULL_PTR;
            break;
        }

        /* Validate remote index: must address a single node (read cannot target all remotes) */
        if((uint32_t)eRemote >= ADI_EAL_MAX_NUM_NODES)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Invalid remote index %d (ReadPin): must be < %d\r\n",
                        (int)eRemote, (int)ADI_EAL_MAX_NUM_NODES);
            *pnPinValue = 0xFFFFFFFFU;
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        nStartAddr = IO_VIH_VIL_READBACKS_NW_ADDR;

        oReadRegCfg.anStartAddr = &nStartAddr;
        oReadRegCfg.nLength     = 1U;
        oReadRegCfg.pBuffer     = &nVihVilData;
        oReadRegCfg.eType       = ADI_E2BCORE_RND_READ;

        eStatus = adi_eal_readRegWithTimeout(nInstNum, eRemote, &oReadRegCfg, GPIO_REG_TIMEOUT_NS);

        if(eStatus != ADI_EAL_STATUS_SUCCESS)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Reg read FAILED: Remote%d, status=%d\r\n",
                        (int)eRemote + 1, (int)eStatus);
            *pnPinValue = 0xFFFFFFFFU;
            break;
        }

        /* Return all pin values within the io_override_man field (bits [11:0]) */
        *pnPinValue = nVihVilData & BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN;
    } while(false);

    return eStatus;
}

/**
 * @brief Set GPIO output pin(s) on a remote node to individual per-pin states
 *
 * Uses a mask write on the HOST_IF_IO_OVERRIDE register.
 * Only the pins selected by nPinMask are updated; every other pin, and the
 * override-enable field (bits [27:16]), are preserved.
 *
 * The HOST_IF_IO_OVERRIDE register layout:
 *   Bits [11: 0] - io_override_man    : manual output values for GPIO pins 0..11
 *   Bits [27:16] - io_override_man_en : override-enable per pin (written by gpioConfigurePins)
 *
 * Per-pin control example (set pin 0 high, pin 1 low, leave others alone):
 *   app_utils_gpioSetPin(inst, remote, 0x0003U, 0x0001U);
 *
 * @param [in] nInstNum  EAL instance number
 * @param [in] eRemote   Remote node to write to
 * @param [in] nPinMask  Bitmask selecting which pins to modify.
 *                       Must be non-zero and within BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN
 *                       (0x00000FFF, SA_IF0..SA_IF11).
 * @param [in] nPinValue Desired output state for each selected pin.
 *                       Bit N = 1 drives pin N high; bit N = 0 drives pin N low.
 *                       Only bits present in nPinMask are applied.
 * @return ADI_EAL_STATUS_SUCCESS on success,
 *         ADI_EAL_STATUS_INVALID_PARAM if nPinMask is zero or out of range,
 *         or a register-access error code on failure.
 */
ADI_MEM_CODE_CRIT
ADI_EAL_STATUS app_utils_gpioSetPin(uint32_t nInstNum, APP_REMOTES eRemote, uint32_t nPinMask, uint32_t nPinValue)
{
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;
    uint16_t nStartAddr;
    uint32_t nMaskData;
    uint32_t nWriteData;
    ADI_E2BCORE_REGMAP_IO_CONFIG oWriteRegCfg;

    do
    {
        /* Validate remote index */
        if((uint32_t)eRemote >= ADI_EAL_MAX_NUM_NODES)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Invalid remote index %d (SetPin): must be < %d\r\n",
                        (int)eRemote, (int)ADI_EAL_MAX_NUM_NODES);
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        /* Validate pin mask: must be non-zero and confined to the 12-pin io_override_man field */
        if((nPinMask == 0U) || ((nPinMask & ~BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN) != 0U))
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Invalid pin mask 0x%04x (SetPin): must be non-zero and <= 0x%04x\r\n",
                        (unsigned int)nPinMask, (unsigned int)BITM_HOST_IF_IO_OVERRIDE_IO_OVERRIDE_MAN);
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        nStartAddr = HOST_IF_IO_OVERRIDE_NW_ADDR;

        /* Mask targets the io_override_man bits [11:0] for the selected pins */
        nMaskData  = nPinMask;
        nWriteData = nPinValue & nPinMask;

        oWriteRegCfg.anStartAddr = &nStartAddr;
        oWriteRegCfg.nLength     = 1U;
        oWriteRegCfg.pBuffer     = &nWriteData;
        oWriteRegCfg.anMask      = &nMaskData;
        oWriteRegCfg.eType       = ADI_E2BCORE_RND_MASK_WRITE;

        eStatus = adi_eal_sendRegTransaction(nInstNum, eRemote, &oWriteRegCfg);

        if(eStatus != ADI_EAL_STATUS_SUCCESS)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[GPIO] Reg write FAILED (SetPin): Remote%d, status=%d\r\n",
                        (int)eRemote + 1, (int)eStatus);
        }
    } while(false);

    return eStatus;
}

/*=============================================================================
 * Lightweight OTP boot-status check no OTP handler required.
 * Uses the existing EAL instance to directly read bootloader status registers.
 *===========================================================================*/

/**
 * @brief  Read LV bootloader_status_7 and check the bootload_done bit.
 *
 * @param[in]     nInstNum    EAL instance number
 * @param[in]     eRemote     Remote node index
 * @param[in,out] poRegCfg    Register read configuration (anStartAddr and pBuffer are updated)
 * @param[in]     anRegAddr   Array of register addresses
 * @param[in,out] anOtpStatus Array of register values (index 9 is written)
 *
 * @return APP_OTP_CHECK_SUCCESS      if LV bootload_done bit is set
 * @return APP_OTP_CHECK_TIMEOUT      if LV is not done yet (continue polling)
 * @return APP_OTP_CHECK_READ_FAILURE if the register read failed
 */
ADI_MEM_CODE_CRIT
static APP_OTP_CHECK_STATUS CheckLvBootDone(uint32_t nInstNum, APP_REMOTES eRemote,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg,
        const uint16_t *anRegAddr, uint32_t *anOtpStatus)
{
    APP_OTP_CHECK_STATUS eResult = APP_OTP_CHECK_TIMEOUT;

    poRegCfg->anStartAddr = &anRegAddr[9U];
    poRegCfg->pBuffer     = &anOtpStatus[9U];

    if(adi_eal_readRegWithTimeout(nInstNum, (uint32_t)eRemote, poRegCfg,
                                  APP_OTP_REG_READ_TIMEOUT_MS * MS_TO_NS) != ADI_EAL_STATUS_SUCCESS)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Polling bootloader done status: Read failed\r\n");
        eResult = APP_OTP_CHECK_READ_FAILURE;
    }
    else if((anOtpStatus[9U] & BITM_IO_BOOTLOADER_STATUS_7_BOOTLOAD_DONE) != 0U)
    {
        eResult = APP_OTP_CHECK_SUCCESS;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Polling bootloader done status: Done\r\n");
    }
    else
    {
        /* LV not done yet continue polling */
    }

    return eResult;
}

/**
 * @brief  Poll HV and LV bootloader done-status registers until bootload completes.
 *
 * @param[in]     nInstNum    EAL instance number
 * @param[in]     eRemote     Remote node index
 * @param[in]     anRegAddr   Array of register addresses (at least 10 entries)
 * @param[in,out] anOtpStatus Array of register values (indices 1 and 9 are written)
 *
 * @return APP_OTP_CHECK_SUCCESS      if both HV and LV bootload_done bits are set
 * @return APP_OTP_CHECK_TIMEOUT      if polling timed out or HV bootload_timeout bit was set
 * @return APP_OTP_CHECK_READ_FAILURE if a register read failed
 */
ADI_MEM_CODE_CRIT
static APP_OTP_CHECK_STATUS PollBootloaderDone(uint32_t nInstNum, APP_REMOTES eRemote,
        const uint16_t *anRegAddr, uint32_t *anOtpStatus)
{
    APP_OTP_CHECK_STATUS eResult = APP_OTP_CHECK_TIMEOUT;
    uint64_t nCurrTimeNs;
    uint64_t nStartTimeNs;
    bool bWait = true;
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;

    adi_pal_getCurrTime(&nStartTimeNs);

    /* Configure for reading HV bootloader_status_1 (index 1) */
    oRegCfg.anStartAddr = &anRegAddr[1U];
    oRegCfg.nLength     = 1U;
    oRegCfg.pBuffer     = &anOtpStatus[1U];
    oRegCfg.eType       = ADI_E2BCORE_RND_READ;
    oRegCfg.anMask      = NULL;

    while(bWait)
    {
        adi_pal_waitMicroSec(APP_OTP_BOOTDONE_POLL_INTERVAL_US);
        adi_pal_getCurrTime(&nCurrTimeNs);

        /* Re-configure for HV bootloader_status_1 read each iteration */
        oRegCfg.anStartAddr = &anRegAddr[1U];
        oRegCfg.pBuffer     = &anOtpStatus[1U];

        /* Timeout check */
        if((nCurrTimeNs - nStartTimeNs) > ((uint64_t)APP_OTP_BOOTDONE_TIMEOUT_MS * MS_TO_NS))
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Polling bootloader done status: Timeout\r\n");
            bWait = false;
        }
        else if(adi_eal_readRegWithTimeout(nInstNum, (uint32_t)eRemote, &oRegCfg,
                                           APP_OTP_REG_READ_TIMEOUT_MS * MS_TO_NS) != ADI_EAL_STATUS_SUCCESS)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Polling bootloader done status: Read failed\r\n");
            eResult = APP_OTP_CHECK_READ_FAILURE;
            bWait = false;
        }
        /* Check HV bootload_timeout */
        else if((anOtpStatus[1U] & BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT) != 0U)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Polling bootloader done status: HV bootload timeout\r\n");
            eResult = APP_OTP_CHECK_TIMEOUT;
            bWait = false;
        }
        /* HV done check LV via helper */
        else if((anOtpStatus[1U] & BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE) != 0U)
        {
            eResult = CheckLvBootDone(nInstNum, eRemote, &oRegCfg, anRegAddr, anOtpStatus);

            if(eResult != APP_OTP_CHECK_TIMEOUT)
            {
                bWait = false;
            }
            else
            {
                /* LV not done yet continue polling */
            }
        }
        else
        {
            /* HV not done yet continue polling */
        }
    }

    return eResult;
}

/**
 * @brief  Read remaining bootloader status registers (skip indices already read in poll).
 *
 * @param[in]     nInstNum    EAL instance number
 * @param[in]     eRemote     Remote node index
 * @param[in]     anRegAddr   Array of register addresses
 * @param[in,out] anOtpStatus Array of register values (indices other than 1 and 9 are written)
 *
 * @return APP_OTP_CHECK_SUCCESS      if all registers read successfully
 * @return APP_OTP_CHECK_READ_FAILURE if a register read failed
 */
ADI_MEM_CODE_CRIT
static APP_OTP_CHECK_STATUS ReadRemainingStatusRegs(uint32_t nInstNum, APP_REMOTES eRemote,
        const uint16_t *anRegAddr, uint32_t *anOtpStatus)
{
    APP_OTP_CHECK_STATUS eResult = APP_OTP_CHECK_SUCCESS;
    uint8_t nIdx;
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;

    oRegCfg.nLength = 1U;
    oRegCfg.eType   = ADI_E2BCORE_RND_READ;
    oRegCfg.anMask  = NULL;

    for(nIdx = 0U; (nIdx < APP_OTP_BOOT_STATUS_NUM_REGS) && (eResult == APP_OTP_CHECK_SUCCESS); nIdx++)
    {
        /* Skip indices 1 and 9 already read during polling */
        if((nIdx != 1U) && (nIdx != 9U))
        {
            oRegCfg.anStartAddr = &anRegAddr[nIdx];
            oRegCfg.pBuffer     = &anOtpStatus[nIdx];

            if(adi_eal_readRegWithTimeout(nInstNum, (uint32_t)eRemote, &oRegCfg,
                                          APP_OTP_REG_READ_TIMEOUT_MS * MS_TO_NS) != ADI_EAL_STATUS_SUCCESS)
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Read failed\r\n");
                eResult = APP_OTP_CHECK_READ_FAILURE;
            }
            else
            {
                /* Nothing to do */
            }
        }
        else
        {
            /* Nothing to do */
        }
    }

    return eResult;
}

/**
 * @brief  Decode per-block status from the 10 bootloader status registers.
 *
 * @param[in]  anOtpStatus   Array of 10 register values
 * @param[out] anBlockStatus Array of APP_OTP_MAX_NUM_BLOCKS filled with per-block status
 */
ADI_MEM_CODE_CRIT
static void DecodeBlockStatus(const uint32_t *anOtpStatus,
                              uint8_t anBlockStatus[APP_OTP_MAX_NUM_BLOCKS])
{
    uint8_t nBlock = 3U;
    uint8_t nRegIdx;
    uint8_t nBitIdx;

    ADI_DBG_REQUIRE(anOtpStatus != NULL, "anOtpStatus is NULL");
    ADI_DBG_REQUIRE(anBlockStatus != NULL, "anBlockStatus is NULL");

    /* HV blocks: Block-0 is not managed by bootloader -> always valid */
    anBlockStatus[0U] = APP_OTP_BLK_VALID;
    /* HV bootloader_status_0 holds Block-1 and Block-2 status (2 bits each) */
    anBlockStatus[1U] = (uint8_t)((anOtpStatus[0U] >> 0U) & APP_OTP_BLK_STATUS_MASK);
    anBlockStatus[2U] = (uint8_t)((anOtpStatus[0U] >> APP_OTP_BITS_PER_BLOCK) & APP_OTP_BLK_STATUS_MASK);

    /* LV blocks: starting from Block-3 (4th block)
     * Registers IO_BOOTLOADER_STATUS_0 to _6 (indices 2..8): each has 8 blocks */
    for(nRegIdx = 2U; nRegIdx <= 8U; nRegIdx++)
    {
        for(nBitIdx = 0U; nBitIdx < APP_OTP_BLOCKS_PER_REG; nBitIdx++)
        {
            anBlockStatus[nBlock] = (uint8_t)((anOtpStatus[nRegIdx] >>
                                               (APP_OTP_BITS_PER_BLOCK * nBitIdx)) & APP_OTP_BLK_STATUS_MASK);
            nBlock++;
        }
    }

    /* IO_BOOTLOADER_STATUS_7 (index 9): last 5 blocks (56..60) */
    for(nBitIdx = 0U; nBitIdx < APP_OTP_LAST_REG_NUM_BLOCKS; nBitIdx++)
    {
        anBlockStatus[nBlock] = (uint8_t)((anOtpStatus[9U] >>
                                           (APP_OTP_BITS_PER_BLOCK * nBitIdx)) & APP_OTP_BLK_STATUS_MASK);
        nBlock++;
    }
}

/**
 * @brief  Read OTP bootloader block status for a remote node without creating an OTP handler.
 *
 * This function directly reads the 10 bootloader status registers (2 HV + 8 LV)
 * via adi_eal_readRegWithTimeout() and decodes per-block boot status into the
 * caller-provided array. It requires only a running EAL instance; no adi_otp_init()
 * is needed.
 *
 * @param[in]  nInstNum      EAL instance number
 * @param[in]  eRemote       Remote node index (APP_REMOTES enum value)
 * @param[out] anBlockStatus Array of size APP_OTP_MAX_NUM_BLOCKS (64), filled with
 *                           per-block status: 0=Uninitialized, 1=Valid, 2=Recoverable, 3=Corrupted
 *
 * @return APP_OTP_CHECK_SUCCESS       if registers were read and decoded successfully
 * @return APP_OTP_CHECK_READ_FAILURE  if a register read failed
 * @return APP_OTP_CHECK_TIMEOUT       if bootloader done polling timed out
 * @return APP_OTP_CHECK_INVALID_PARAM if eRemote is APP_ALL_REMOTES
 * @return APP_OTP_CHECK_NULL_PTR      if anBlockStatus is NULL
 */
ADI_MEM_CODE_CRIT
APP_OTP_CHECK_STATUS app_utils_readOtpBootStatus(uint32_t nInstNum, APP_REMOTES eRemote,
        uint8_t anBlockStatus[APP_OTP_MAX_NUM_BLOCKS])
{
    APP_OTP_CHECK_STATUS eResult = APP_OTP_CHECK_INVALID_PARAM;
    APP_OTP_CHECK_STATUS ePollResult;
    APP_OTP_CHECK_STATUS eReadResult;
    uint32_t anOtpStatus[APP_OTP_BOOT_STATUS_NUM_REGS] = {0U};

    ADI_DBG_REQUIRE(eRemote != APP_ALL_REMOTES, "Cannot read OTP status for APP_ALL_REMOTES");
    ADI_DBG_REQUIRE(anBlockStatus != NULL, "anBlockStatus must not be NULL");

    do
    {
        /* Runtime guards ADI_DBG_REQUIRE compiles out in release */
        if(anBlockStatus == NULL)
        {
            eResult = APP_OTP_CHECK_NULL_PTR;
            break;
        }

        if(eRemote == APP_ALL_REMOTES)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[OTP] Invalid remote: APP_ALL_REMOTES (readOtpBootStatus)\r\n");
            eResult = APP_OTP_CHECK_INVALID_PARAM;
            break;
        }

        /* Zero the output array */
        (void)ADI_MEMSET(anBlockStatus, 0, APP_OTP_MAX_NUM_BLOCKS);

        /* Phase 1: Poll HV and LV done-status registers */
        ePollResult = PollBootloaderDone(nInstNum, eRemote, ganOtpBootStatusRegAddr, anOtpStatus);

        if(ePollResult != APP_OTP_CHECK_SUCCESS)
        {
            eResult = ePollResult;
            break;
        }

        /* Phase 2: Read remaining status registers for per-block data */
        eReadResult = ReadRemainingStatusRegs(nInstNum, eRemote, ganOtpBootStatusRegAddr, anOtpStatus);

        if(eReadResult != APP_OTP_CHECK_SUCCESS)
        {
            eResult = eReadResult;
            break;
        }

        /* Phase 3: Decode per-block status from the 10 status registers */
        DecodeBlockStatus(anOtpStatus, anBlockStatus);

        eResult = APP_OTP_CHECK_SUCCESS;
    } while(false);

    return eResult;
}

/**
 * @brief  Classify OTP block statuses and log warnings for recoverable/corrupted blocks.
 *
 * @param[in]  anBlockStatus  Array of per-block status values
 * @param[out] poSummary      Summary counters filled by this function
 *
 * @return true if any block is corrupted, false otherwise
 */
ADI_MEM_CODE_CRIT
static bool ClassifyBlockStatuses(const uint8_t anBlockStatus[APP_OTP_MAX_NUM_BLOCKS],
                                  APP_OTP_BLOCK_SUMMARY *poSummary)
{
    bool bCorruption = false;
    uint8_t nBlk;

    ADI_DBG_REQUIRE(anBlockStatus != NULL, "anBlockStatus is NULL");
    ADI_DBG_REQUIRE(poSummary != NULL, "poSummary is NULL");

    for(nBlk = 0U; nBlk < APP_OTP_MAX_NUM_BLOCKS; nBlk++)
    {
        switch(anBlockStatus[nBlk])
        {
            case APP_OTP_BLK_UNINITIALIZED:
                poSummary->nEmpty++;
                break;

            case APP_OTP_BLK_VALID:
                poSummary->nValid++;
                break;

            case APP_OTP_BLK_RECOVERABLE:
                poSummary->nRecoverable++;
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                            "  [WARN] Block "UINT32_FORMATTER": ECC single-bit corrected (recoverable)\r\n",
                            (uint32_t)nBlk);
                break;

            case APP_OTP_BLK_CORRUPTED:
                poSummary->nCorrupted++;
                bCorruption = true;
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                            "  [CORRUPT] Block "UINT32_FORMATTER": Non-recoverable ECC error!\r\n",
                            (uint32_t)nBlk);
                break;

            default:
                /* Unknown status ignore */
                break;
        }
    }

    return bCorruption;
}

/**
 * @brief  Check OTP boot block status for a remote node without creating an OTP handler.
 *
 * Reads the bootloader status registers, decodes per-block status, and checks
 * for corrupted blocks. This is a convenience wrapper around
 * app_utils_readOtpBootStatus() that logs results and returns a summary.
 *
 * @param[in]  nInstNum      EAL instance number
 * @param[in]  eRemote       Remote node index
 *
 * @return APP_OTP_CHECK_SUCCESS       if all blocks are healthy
 * @return APP_OTP_CHECK_READ_FAILURE  if bootloader status registers could not be read
 * @return APP_OTP_CHECK_TIMEOUT       if bootloader done polling timed out
 * @return APP_OTP_CHECK_CORRUPTED     if one or more OTP blocks are corrupted
 * @return APP_OTP_CHECK_INVALID_PARAM if eRemote is APP_ALL_REMOTES
 */
ADI_MEM_CODE_CRIT
APP_OTP_CHECK_STATUS app_utils_checkOtpBootBlockStatus(uint32_t nInstNum, APP_REMOTES eRemote)
{
    APP_OTP_CHECK_STATUS eResult = APP_OTP_CHECK_INVALID_PARAM;
    APP_OTP_CHECK_STATUS eReadResult;
    uint8_t anBlockStatus[APP_OTP_MAX_NUM_BLOCKS];
    APP_OTP_BLOCK_SUMMARY oSummary = {0U, 0U, 0U, 0U};
    bool bCorruption = false;

    ADI_DBG_REQUIRE(eRemote != APP_ALL_REMOTES, "Cannot check OTP corruption for APP_ALL_REMOTES");

    do
    {
        /* Runtime guard ADI_DBG_REQUIRE compiles out in release */
        if(eRemote == APP_ALL_REMOTES)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[OTP] Invalid remote: APP_ALL_REMOTES (checkOtpBootBlockStatus)\r\n");
            eResult = APP_OTP_CHECK_INVALID_PARAM;
            break;
        }

        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                    "\r\n--- OTP Corruption Check (Remote "UINT32_FORMATTER", no OTP handler) ---\r\n",
                    (uint32_t)eRemote + 1U);

        eReadResult = app_utils_readOtpBootStatus(nInstNum, eRemote, anBlockStatus);

        if(eReadResult != APP_OTP_CHECK_SUCCESS)
        {
            if(eReadResult == APP_OTP_CHECK_TIMEOUT)
            {
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                            "  [FAIL] Bootloader done polling timed out.\r\n");
            }
            else
            {
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                            "  [FAIL] Could not read OTP boot status registers.\r\n");
            }

            eResult = eReadResult;
            break;
        }

        /* Classify all blocks and log warnings */
        bCorruption = ClassifyBlockStatuses(anBlockStatus, &oSummary);

        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                    "  Summary - Valid: "UINT32_FORMATTER", Recoverable: "UINT32_FORMATTER
                    ", Corrupted: "UINT32_FORMATTER", Empty: "UINT32_FORMATTER"\r\n",
                    oSummary.nValid, oSummary.nRecoverable, oSummary.nCorrupted, oSummary.nEmpty);

        if(bCorruption)
        {
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                        "  [FAIL] OTP corruption detected on Remote "UINT32_FORMATTER"!\r\n",
                        (uint32_t)eRemote + 1U);
            eResult = APP_OTP_CHECK_CORRUPTED;
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                        "  [PASS] No OTP corruption on Remote "UINT32_FORMATTER".\r\n",
                        (uint32_t)eRemote + 1U);
            eResult = APP_OTP_CHECK_SUCCESS;
        }
    } while(false);

    return eResult;
}

ADI_MEM_CODE_CRIT
void app_utils_logPingRcvd(uint32_t nRemoteNum)
{
#ifdef USE_CLI
    CMD_EXECUTE_FUNC pfExecPingResp = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_PING_RESP];
    CMD_INST_PING_RESP *poPingResp = (CMD_INST_PING_RESP *)apCmdObj[E_CLI_CMD_PING_RESP];
    poPingResp->nRemoteNum = nRemoteNum;
    poPingResp->eCmd = E_CLI_CMD_PING_RESP;
    pfExecPingResp(poPingResp);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Ping response received from Remote" UINT32_FORMATTER "\r\n", nRemoteNum + 1U);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logRemotePlcaStatus(ADI_PLCA_STATUS_DATA *poStatusDat, uint32_t nRemoteNum)
{
#ifdef USE_CLI
    CMD_EXECUTE_FUNC pfExecPhyStatus = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_PHY_STATS];
    CMD_INST_PHY_STAT_RESP *poPhyStatus = (CMD_INST_PHY_STAT_RESP *)apCmdObj[E_CLI_CMD_PHY_STATS];
    poPhyStatus->bIsRemoteNode = true;
    poPhyStatus->nRemoteNum = nRemoteNum;
    poPhyStatus->poPhyStatsDat = poStatusDat;
    poPhyStatus->eCmd = E_CLI_CMD_PHY_STATS;
    /* pfExecPhyStats(poPhyStatus); */
    adi_common_logPlcaStatus(poStatusDat);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "PLCA status for Remote" UINT32_FORMATTER "\r\n", nRemoteNum + 1U);
    adi_common_logPlcaStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logRemoteMacStatus(ADI_MAC_STATUS_DATA *poStatusDat, uint32_t nRemoteNum)
{
#ifdef USE_CLI
    CMD_EXECUTE_FUNC pfExecMacStatus = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_MAC_STATS];
    CMD_INST_MAC_STAT_RESP *poMacStatus = (CMD_INST_MAC_STAT_RESP *)apCmdObj[E_CLI_CMD_MAC_STATS];
    poMacStatus->bIsRemoteNode = true;
    poMacStatus->nRemoteNum = nRemoteNum;
    poMacStatus->poMacStatsDat = poStatusDat;
    poMacStatus->eCmd = E_CLI_CMD_MAC_STATS;
    /* pfExecMacStats(poMacStatus); */
    adi_common_logMacStatus(poStatusDat);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "MAC status for Remote" UINT32_FORMATTER "\r\n", nRemoteNum + 1U);
    adi_common_logMacStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logRemotePhyStatus(ADI_PHY_STATUS_DATA *poStatusDat, uint32_t nRemoteNum)
{
#ifdef USE_CLI
    adi_common_logPhyStatus(poStatusDat);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "PHY status for Remote" UINT32_FORMATTER "\r\n", nRemoteNum + 1U);
    adi_common_logPhyStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logRemotePtpStatus(ADI_PTP_STATUS_DATA *poStatusDat, uint32_t nRemoteNum)
{
#ifdef USE_CLI
    CMD_EXECUTE_FUNC pfExecPtpStatus = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_PTP_STATS];
    CMD_INST_PTP_STAT_RESP *poPtpStatus = (CMD_INST_PTP_STAT_RESP *)apCmdObj[E_CLI_CMD_PTP_STATS];
    poPtpStatus->nRemoteNum = nRemoteNum;
    poPtpStatus->poPtpStatsDat = poStatusDat;
    poPtpStatus->eCmd = E_CLI_CMD_PTP_STATS;
    /* pfExecPtpStats(poPtpStatus); */
    adi_common_logPtpStatus(poStatusDat);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "PTP status for Remote" UINT32_FORMATTER "\r\n", nRemoteNum + 1U);
    adi_common_logPtpStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

ADI_MEM_CODE_CRIT
void app_utils_logRemotePcsStatus(ADI_PCS_STATUS_DATA *poStatusDat, uint32_t nRemoteNum)
{
#ifdef USE_CLI
    CMD_EXECUTE_FUNC pfExecPcsStatus = (CMD_EXECUTE_FUNC)apfCmdExecute[E_CLI_CMD_PCS_STATS];
    CMD_INST_PCS_STAT_RESP *poPcsStatus = (CMD_INST_PCS_STAT_RESP *)apCmdObj[E_CLI_CMD_PCS_STATS];
    poPcsStatus->nRemoteNum = nRemoteNum;
    poPcsStatus->poPcsStatsDat = poStatusDat;
    poPcsStatus->eCmd = E_CLI_CMD_PCS_STATS;
    /* pfExecPcsStats(poPcsStatus); */
    adi_common_logPcsStatus(poStatusDat);
#else
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "PCS status for Remote" UINT32_FORMATTER "\r\n", nRemoteNum + 1U);
    adi_common_logPcsStatus(poStatusDat);
#endif /* USE_CLI */
    app_utils_addLineSeparator(true);
}

#endif

/* Process RX byte */
ADI_MEM_CODE_CRIT
static void ProcessRxByte(uint8_t nRxByte)
{
    if(nRxByte == (uint8_t)'\n' || nRxByte == (uint8_t)'\r')
    {
        /* Convert the accumulated string to lowercase */
        uint32_t i;

        for(i = 0u; i < gnRxByteCount; i++)
        {
            ganRxParsedData[i] = tolower(ganRxParsedData[i]);
        }

        /* Give a callback with the accumulated data */
        app_utils_rxCmdCbk(&ganRxParsedData[0u], gnRxByteCount);

        /* Reset the accumulator counter */
        gnRxByteCount = 0u;

        /* Clear the history */
        (void) ADI_MEMSET(ganRxParsedData, 0, ADI_MAX_RX_COMMAND_LEN * sizeof(uint8_t));
    }
    /* If the UART RX data is Ctrl+C, exit the program */
    else if(nRxByte == 0x03u)
    {
        uint64_t nEndTime, nCurrTime;
        adi_pal_getCurrTime(&nEndTime);
        nEndTime += 10ULL * MS_TO_NS;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Exiting!");

        /* Running cliRun for 10ms to flush all the UART messages */
        do
        {
            adi_pal_cliRun();
            adi_pal_getCurrTime((uint64_t *)&nCurrTime);
        } while(nCurrTime < nEndTime);

        exit(0);
    }
    /**
     * If the UART RX data is Ctrl+R, don't wait for enter key.
     * Give a callback immediately
     */
    else if(nRxByte == 18u)
    {
        ganRxParsedData[0u] = 18u;

        /* Give a callback with the accumulated data */
        app_utils_rxCmdCbk(&ganRxParsedData[0u], gnRxByteCount);

        /* Reset the accumulator counter */
        gnRxByteCount = 0u;

        /* Clear the history */
        (void) ADI_MEMSET(ganRxParsedData, 0, ADI_MAX_RX_COMMAND_LEN * sizeof(uint8_t));
    }
    /* If the UART RX data is backspace */
    else if(nRxByte == 8u || nRxByte == 0x7Fu)
    {
        if(gnRxByteCount > 0u)
        {
            /* Decrement the accumulator byte counter */
            gnRxByteCount--;
        }
    }
    /* Accumulate the RX byte */
    else
    {
        /* Make sure the RX buffer doesn't overflow */
        if(gnRxByteCount < ADI_MAX_RX_COMMAND_LEN)
        {
            /* Accumulate the RX byte */
            ganRxParsedData[gnRxByteCount] = nRxByte;

            /* Increment the accumulator counter */
            gnRxByteCount++;
        }
    }

}

/** @} */

/*
 * EOF: https://www.analog.com/
*/
