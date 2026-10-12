/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * @file: eal_wake_sleep.c
 * @brief: The EAL APIs for sleep-wake component
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** @addtogroup EAL
 *  @{
 */

// ----------------------- INCLUDES -----------------------

#include "eal_internal.h"

#ifdef ADI_E2B_IFACE_WKSLP_ENABLED

/*! \cond PRIVATE */
// ----------------------- DEFINES ------------------------
/// TODO: Update the timeout values
#define WAITING_TO_SLEEP_TIMEOUT_NS         (200ULL  * MS_TO_NS)     /*!< Timeout for getting line busy error / sleep error interrupt for a goto_sleep command*/
#define WAITING_TO_STANDBY_TIMEOUT_NS       (100ULL  * MS_TO_NS)     /*!< Timeout for getting line busy error / standby error interrupt for a goto_standby command */
#define STANDBY_PING_RESP_TIMEOUT_NS        (100ULL  * MS_TO_NS)     /*!< Max time to wait for ping response (for confirmation that node has gone to standby) */
#define SLEEP_PING_RESP_TIMEOUT_NS          (100ULL  * MS_TO_NS)     /*!< Max time to wait for ping response (for confirmation that node has gone to sleep) */
#define STANDBY_FEATURE_OASPI_INV_EN        (0xA000U)                /*!< Data to disable device backdoor mode and invert The OA_SPI Interrupt polarity */
#define BITM_STANDBY_FEATURE_OASPI_INV_EN   (0xA000U)                /*!< Mask to disable device backdoor mode and invert The OA_SPI Interrupt polarity */

// ---------------------- DATA TYPES ----------------------
/**
 * @brief Wake-sleep commands
 */
typedef enum EAL_WKSLP_CMDS
{
    EAL_WKSLP_CMD_STANDBY,      /*!< Set node to standby */
    EAL_WKSLP_CMD_SLEEP,        /*!< Set node to sleep */
    EAL_WKSLP_CMD_SEND_WUS,     /*!< Send Wake-up sequence */
} EAL_WKSLP_CMDS;

/**
 * @brief Remote node wake - sleep states
 */
typedef enum EAL_WKSLP_REMOTE_STATES
{
    EAL_WKSLP_REMOTE_STATE_AWAKE,                   /*!< Node in awake state */
    EAL_WKSLP_REMOTE_STATE_STANDBY,                 /*!< Node in standby state */
    EAL_WKSLP_REMOTE_STATE_SLEEP,                   /*!< Node in sleep state */
    EAL_WKSLP_REMOTE_STATE_SEND_STANDBY_CMD,        /*!< Sends command to set node to standby */
    EAL_WKSLP_REMOTE_STATE_SEND_SLEEP_CMD,          /*!< Sends command to set node to sleep */
    EAL_WKSLP_REMOTE_STATE_WAITING_TO_STANDBY,      /*!< Waiting for node to go to standby */
    EAL_WKSLP_REMOTE_STATE_WAITING_TO_SLEEP,        /*!< Waiting for node to go to sleep */
    EAL_WKSLP_REMOTE_STATE_PING_NODE,               /*!< Sends a ping command to check if node is awake */
    EAL_WKSLP_REMOTE_STATE_WAITING_FOR_PING_RESP,   /*!< Waiting for ping response to check if node is awake */
} EAL_WKSLP_REMOTE_STATES;

/**
 * @brief Remote node context
 */
typedef enum EAL_WKSLP_REMOTE_CONTEXT
{
    EAL_WKSLP_REMOTE_CTX_WAITING_TO_STANDBY,
    EAL_WKSLP_REMOTE_CTX_WAITING_TO_SLEEP,
} EAL_WKSLP_REMOTE_CONTEXT;

/**
 * @brief Data structure for the remote node
 */
typedef struct EAL_WKSLP_REMOTE_DATA
{
    EAL_WKSLP_REMOTE_STATES eRemoteNodeState;       /*!< Current state of the remote node */
    EAL_WKSLP_REMOTE_CONTEXT eRemoteNodeContext;    /*!< Remote node context for state transitions */
    ADI_EAL_WKSLP_NODE_STATUS eRemoteNodeStatus;    /*!< Status of the remote node */
    bool bGotoSleep;                                /*!< Is sleep request pending? */
    bool bGotoStandby;                              /*!< Is standby request pending? */
    bool bConfirmAwakeInt;                          /*!< Has confirm awake interrupt been asserted? */
    bool bReqSysWakeupConfInt;                      /*!< Has request wakeup confirmation interrupt been asserted? */
    bool bReturnedFromStandbyInt;                   /*!< Has returned from standby interrupt been asserted? */
    bool bLineBusyInt;                              /*!< Has line-busy interrupt been asserted? */
    bool bStandbyErrInt;                            /*!< Has standby error interrupt been asserted? */
    bool bSleepErrInt;                              /*!< Has sleep error interrupt been asserted? */
    bool bPingRcvd;                                 /*!< Is ping response received? */
    uint64_t nStartTimeNs;                          /*!< Start time of any event for checking timeouts */
} EAL_WKSLP_REMOTE_DATA;

/**
 * @brief Top-level data structure for the EAL wake-sleep component
 */
typedef struct EAL_WKSLP_DATA
{
    ADI_EAL_WKSLP_CFG *poWkslpCfg;                                     /*!< Wake sleep configuration data */
    EAL_WKSLP_REMOTE_DATA aoEalRemoteWkslpData[ADI_EAL_MAX_NUM_NODES]; /*!< Wake sleep data of remote nodes */
} EAL_WKSLP_DATA;

// ------------------------- DATA -------------------------
/// TODO: (Post 2.0.0) Change these to dynamic memory assignment
/** The internal data structure for wake-sleep component */
ADI_EAL_WKSLP_DATA
static EAL_WKSLP_DATA gaoEalWkslpData[ADI_E2BCORE_MAX_INST];

// --------------------- PRIVATE CODE ---------------------
/**
 * @brief Pings the node to check if the node has successfully gone to sleep / standby
 */
ADI_EAL_WKSLP_CODE
static ADI_EAL_STATUS PingNode(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // Read the MAC address register as a ping test
    uint16_t anRegAddr[1u] = { (uint16_t)LCE_DEVICE_MAC_ADDRESS_LOWER_ADDR};

    // Configurations for the register map message
    static ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)(ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG)));
    oRegCfg.eType       = ADI_E2BCORE_RND_READ;
    oRegCfg.nLength     = 1u;
    oRegCfg.anStartAddr = &anRegAddr[0u];

    // Set the flag for top-level EAL to route the response to this module
    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = true;

    // Send the register read transaction
    ADI_EAL_STATUS eEalStatus = adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);

    if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = false;
    }

    return eEalStatus;
}

/**
 * @brief       Sends a wake-sleep command to the remote node (goto_sleep / goto_standby / wake_up)
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  eWkslpCmd           Wake-sleep commands
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           Call completed successfully
 */
ADI_EAL_WKSLP_CODE
static ADI_EAL_STATUS SendCommandRemote(uint32_t nInstNum, uint32_t nRemoteNum, EAL_WKSLP_CMDS eWkslpCmd)
{
    // Write to the SLEEPWAKE_CMD register with the command,
    uint16_t nRegAddr[2U];
    uint32_t nRegData[2U];
    uint32_t nRegMask[2u]; // Used only in stand-by mode as a prior register write is required which uses mask - the latter stand-by command could be still sent without a mask

    nRegAddr[0U] = (uint16_t)IO_SLEEPWAKE_CMD_NW_ADDR;

    // Configurations for Register map IO message
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)(ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG)));
    oRegCfg.eType       = ADI_E2BCORE_RND_WRITE;
    oRegCfg.nLength     = 1u;
    oRegCfg.anStartAddr = &nRegAddr[0U];
    oRegCfg.pBuffer     = &nRegData[0U];

    // goto_standby command
    if(eWkslpCmd == EAL_WKSLP_CMD_STANDBY)
    {
        /* Disable backdoor mode before sending standby command */
        nRegAddr[0U] = IO_MISC_CONTROL_NW_ADDR;
        nRegAddr[1U] = IO_SLEEPWAKE_CMD_NW_ADDR;

        nRegData[0U] = STANDBY_FEATURE_OASPI_INV_EN;
        nRegData[1U] = BITM_IO_SLEEPWAKE_CMD_SLPWK_CMD_GOTO_STANDBY;

        nRegMask[0U] = BITM_STANDBY_FEATURE_OASPI_INV_EN;
        nRegMask[1U] = BITM_IO_SLEEPWAKE_CMD_SLPWK_CMD_GOTO_STANDBY;

        oRegCfg.eType       = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength     = 2u;
        oRegCfg.anStartAddr = &nRegAddr[0U];
        oRegCfg.pBuffer     = &nRegData[0U];
        oRegCfg.anMask      = &nRegMask[0U];
    }
    // goto_sleep command
    else if(eWkslpCmd == EAL_WKSLP_CMD_SLEEP)
    {
        nRegData[0U] = BITM_IO_SLEEPWAKE_CMD_SLPWK_CMD_GOTO_SLEEP;
    }
    // send_wus command
    else if(eWkslpCmd == EAL_WKSLP_CMD_SEND_WUS)
    {
        nRegData[0U] = BITM_IO_SLEEPWAKE_CMD_SLPWK_CMD_SEND_WUS;
    }
    // invalid command
    else
    {
        /* Invalid command */
        ADI_DBG_REQUIRE(false, "Invalid command");
        ADI_DBG_ERROR();
    }

    // Send the register write transaction
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/** Remote waiting for ping response state handler */
ADI_EAL_WKSLP_CODE
static void RemoteWaitingForPingRespStateHandler(EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData, uint32_t nInstNum,
        uint32_t nRemoteNum)
{
    // If context == waiting_to_standby
    //  * If ping response is received, it is an error condition.
    //    Give a callback and switch to awake state
    //  * If wait times-out, switch to standby state
    // If context == waiting_to_sleep
    //  * If ping response is received, it is an error condition.
    //    Give a callback and switch to awake state
    //  * If wait times-out, switch to sleep state
    uint64_t nCurrTimeNs;
    adi_pal_getCurrTime(&nCurrTimeNs);

    if(poWkslpRemoteData->eRemoteNodeContext == EAL_WKSLP_REMOTE_CTX_WAITING_TO_STANDBY)
    {
        if(nCurrTimeNs - poWkslpRemoteData->nStartTimeNs > STANDBY_PING_RESP_TIMEOUT_NS)
        {
            gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = false;
            poWkslpRemoteData->eRemoteNodeStatus = ADI_EAL_WKSLP_NODE_STANDBY;
            ADI_EAL_WKSLP_CBK_EVT_STANDBY_DATA oEvtData =
            {
                .nInstNum = nInstNum,
                .nRemoteNum = nRemoteNum,
                .ePrevState = ADI_EAL_WKSLP_NODE_AWAKE,
            };
            (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_STANDBY, &oEvtData);
            poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_STANDBY;
        }
        else if(poWkslpRemoteData->bPingRcvd == true)
        {
            gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = false;
            poWkslpRemoteData->bPingRcvd = false;
            ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED_DATA oEvtData =
            {
                .nInstNum = nInstNum,
                .nRemoteNum = nRemoteNum,
                .eFailStatus = ADI_EAL_STATUS_WKSLP_PING_RESP_ERR,
            };
            (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED, &oEvtData);
            poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
        }
        else
        {
            /* Nothing to do */
        }
    }
    else if(poWkslpRemoteData->eRemoteNodeContext == EAL_WKSLP_REMOTE_CTX_WAITING_TO_SLEEP)
    {
        if(nCurrTimeNs - poWkslpRemoteData->nStartTimeNs > SLEEP_PING_RESP_TIMEOUT_NS)
        {
            gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = false;
            poWkslpRemoteData->eRemoteNodeStatus = ADI_EAL_WKSLP_NODE_SLEEP;
            ADI_EAL_WKSLP_CBK_EVT_SLEEP_DATA oEvtData =
            {
                .nInstNum = nInstNum,
                .nRemoteNum = nRemoteNum,
                .ePrevState = ADI_EAL_WKSLP_NODE_AWAKE,
            };
            (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_SLEEP, &oEvtData);
            poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_SLEEP;
        }
        else if(poWkslpRemoteData->bPingRcvd == true)
        {
            gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = false;
            poWkslpRemoteData->bPingRcvd = false;
            ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED_DATA oEvtData =
            {
                .nInstNum = nInstNum,
                .nRemoteNum = nRemoteNum,
                .eFailStatus = ADI_EAL_STATUS_WKSLP_PING_RESP_ERR,
            };
            (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED, &oEvtData);
            poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
        }
        else
        {
            /* Nothing to do */
        }
    }
    else
    {
        /* Shouldn't come here! */
        ADI_DBG_ERROR();
    }
}

/** Remote ping state handler */
ADI_EAL_WKSLP_CODE
static void RemotePingNodeStateHandler(uint32_t nInstNum, uint32_t nRemoteNum, EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData)
{
    // Ping node, save the current time
    // Set the next state to waiting for ping response
    ADI_EAL_STATUS eEalStatus = PingNode(nInstNum, nRemoteNum);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        adi_pal_getCurrTime(&poWkslpRemoteData->nStartTimeNs);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_WAITING_FOR_PING_RESP;
    }
    else
    {
        // Retry in the next run cycle
        // TODO: Add max retries
    }
}

/** Remote waiting to sleep state handler */
ADI_EAL_WKSLP_CODE
static void RemoteWaitingToSleepStateHandler(EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData, uint32_t nInstNum,
        uint32_t nRemoteNum)
{
    // 1. If line_busy or sleep_err is set:
    //      * Give a callback with sleep transition failure event
    //      * Next State = AWAKE
    // 2. If timeout:
    //      * Next state = PING_NODE
    //      * Set context = WAITING_TO_SLEEP

    uint64_t nCurrTimeNs;
    adi_pal_getCurrTime(&nCurrTimeNs);

    if(nCurrTimeNs - poWkslpRemoteData->nStartTimeNs > WAITING_TO_SLEEP_TIMEOUT_NS)
    {
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_PING_NODE;
        poWkslpRemoteData->eRemoteNodeContext = EAL_WKSLP_REMOTE_CTX_WAITING_TO_SLEEP;
    }
    else if(poWkslpRemoteData->bLineBusyInt == true)
    {
        poWkslpRemoteData->bLineBusyInt = false;
        ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED_DATA oEvtData =
        {
            .nInstNum = nInstNum,
            .nRemoteNum = nRemoteNum,
            .eFailStatus = ADI_EAL_STATUS_WKSLP_LINE_BUSY,
        };
        (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED, &oEvtData);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
    }
    else if(poWkslpRemoteData->bSleepErrInt == true)
    {
        poWkslpRemoteData->bSleepErrInt = false;
        ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED_DATA oEvtData =
        {
            .nInstNum = nInstNum,
            .nRemoteNum = nRemoteNum,
            .eFailStatus = ADI_EAL_STATUS_WKSLP_SLEEP_ERR,
        };
        (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_SLEEP_FAILED, &oEvtData);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
    }
    else
    {
        // Nothing to do
    }
}

/** Remote waiting to standby state handler */
ADI_EAL_WKSLP_CODE
static void RemoteWaitingToStandbyStateHandler(EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData, uint32_t nInstNum,
        uint32_t nRemoteNum)
{
    // 1. If line_busy or standby_err is set:
    //      * Give a callback with Standby transition failure event
    //      * Next State = AWAKE
    // 2. If timeout:
    //      * Next state = PING_NODE
    //      * Set context = WAITING_TO_STANDBY

    uint64_t nCurrTimeNs;
    adi_pal_getCurrTime(&nCurrTimeNs);

    if(nCurrTimeNs - poWkslpRemoteData->nStartTimeNs > WAITING_TO_STANDBY_TIMEOUT_NS)
    {
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_PING_NODE;
        poWkslpRemoteData->eRemoteNodeContext = EAL_WKSLP_REMOTE_CTX_WAITING_TO_STANDBY;
    }
    else if(poWkslpRemoteData->bLineBusyInt == true)
    {
        poWkslpRemoteData->bLineBusyInt = false;
        ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED_DATA oEvtData =
        {
            .nInstNum = nInstNum,
            .nRemoteNum = nRemoteNum,
            .eFailStatus = ADI_EAL_STATUS_WKSLP_LINE_BUSY,
        };
        (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED, &oEvtData);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
    }
    else if(poWkslpRemoteData->bStandbyErrInt == true)
    {
        poWkslpRemoteData->bStandbyErrInt = false;
        ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED_DATA oEvtData =
        {
            .nInstNum = nInstNum,
            .nRemoteNum = nRemoteNum,
            .eFailStatus = ADI_EAL_STATUS_WKSLP_STANDBY_ERR,
        };
        (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_STANDBY_FAILED, &oEvtData);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
    }
    else
    {
        // Nothing to do
    }
}

/** Remote send sleep command state handler */
ADI_EAL_WKSLP_CODE
static void RemoteSendSleepStateHandler(uint32_t nInstNum, uint32_t nRemoteNum,
                                        EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData)
{
    // Send sleep command, save the current time, next state = Waiting_to_sleep
    ADI_EAL_STATUS eEalStatus = SendCommandRemote(nInstNum, nRemoteNum, EAL_WKSLP_CMD_SLEEP);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        adi_pal_getCurrTime(&poWkslpRemoteData->nStartTimeNs);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_WAITING_TO_SLEEP;
    }
    else
    {
        // Retry in the next run cycle
        // TODO: Add max retries
    }
}

/** Remote send standby command state handler */
ADI_EAL_WKSLP_CODE
static void RemoteSendStandbyStateHandler(uint32_t nInstNum, uint32_t nRemoteNum,
        EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData)
{
    // Send standby command, save the current time, next state = Waiting_to_standby
    ADI_EAL_STATUS eEalStatus = SendCommandRemote(nInstNum, nRemoteNum, EAL_WKSLP_CMD_STANDBY);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        adi_pal_getCurrTime(&poWkslpRemoteData->nStartTimeNs);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_WAITING_TO_STANDBY;
    }
    else
    {
        // Retry in the next run cycle
        // TODO: Add max retries
    }
}

/** Remote sleep state handler */
ADI_EAL_WKSLP_CODE
static void RemoteSleepStateHandler(EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData, uint32_t nInstNum, uint32_t nRemoteNum)
{
    // If confirm_awake / req_system_wakeup_conf is set:
    //  * Next state = PING_NODE
    //  * Set context = CONFIRM_AWAKE
    if(poWkslpRemoteData->bConfirmAwakeInt == true || poWkslpRemoteData->bReqSysWakeupConfInt == true)
    {
        poWkslpRemoteData->bConfirmAwakeInt = false;
        poWkslpRemoteData->bReqSysWakeupConfInt = false;
        poWkslpRemoteData->eRemoteNodeStatus = ADI_EAL_WKSLP_NODE_AWAKE;
        ADI_EAL_WKSLP_CBK_EVT_AWAKE_DATA oEvtData =
        {
            .nInstNum = nInstNum,
            .nRemoteNum = nRemoteNum,
            .ePrevState = ADI_EAL_WKSLP_NODE_SLEEP,
        };
        (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_AWAKE, &oEvtData);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
    }
}

/** Remote standby state handler */
ADI_EAL_WKSLP_CODE
static void RemoteStandbyStateHandler(EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData, uint32_t nInstNum, uint32_t nRemoteNum)
{
    // If returned_from_standby is set:
    //  * Next state = AWAKE
    if(poWkslpRemoteData->bReturnedFromStandbyInt == true)
    {
        poWkslpRemoteData->bReturnedFromStandbyInt = false;
        poWkslpRemoteData->eRemoteNodeStatus = ADI_EAL_WKSLP_NODE_AWAKE;
        ADI_EAL_WKSLP_CBK_EVT_AWAKE_DATA oEvtData =
        {
            .nInstNum = nInstNum,
            .nRemoteNum = nRemoteNum,
            .ePrevState = ADI_EAL_WKSLP_NODE_STANDBY,
        };
        (gaoEalWkslpData[nInstNum].poWkslpCfg->pfWkslpCallback)(ADI_EAL_WKSLP_CBK_EVT_AWAKE, &oEvtData);
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
    }
}

/** Remote awake state handler */
ADI_EAL_WKSLP_CODE
static void RemoteAwakeStateHandler(EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData)
{
    // 1. If goto_sleep is set:
    //      * Next state = SEND_SLEEP_CMD
    // 2. If goto_standby is set:
    //      * Next state = SEND_STANDBY_CMD
    if(poWkslpRemoteData->bGotoSleep == true)
    {
        poWkslpRemoteData->bGotoSleep = false;
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_SEND_SLEEP_CMD;
    }
    else if(poWkslpRemoteData->bGotoStandby == true)
    {
        poWkslpRemoteData->bGotoStandby = false;
        poWkslpRemoteData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_SEND_STANDBY_CMD;
    }
    else
    {
        // Nothing to do
    }
}

/**
 * @brief Ticks the state machine of one remote node
 */
ADI_EAL_WKSLP_CODE
static void RunRemoteNode(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // Get the wake-sleep data of the remote
    EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData = &gaoEalWkslpData[nInstNum].aoEalRemoteWkslpData[nRemoteNum];
    EAL_WKSLP_REMOTE_STATES eCurrState = poWkslpRemoteData->eRemoteNodeState;

    switch(eCurrState)
    {
        // Handle awake state of remote
        case EAL_WKSLP_REMOTE_STATE_AWAKE:
        {
            RemoteAwakeStateHandler(poWkslpRemoteData);
            break;
        }

        // Handle standby state of remote
        case EAL_WKSLP_REMOTE_STATE_STANDBY:
        {
            RemoteStandbyStateHandler(poWkslpRemoteData, nInstNum, nRemoteNum);
            break;
        }

        // Handle sleep state of remote
        case EAL_WKSLP_REMOTE_STATE_SLEEP:
        {
            RemoteSleepStateHandler(poWkslpRemoteData, nInstNum, nRemoteNum);
            break;
        }

        // Handle send standby command state of remote
        case EAL_WKSLP_REMOTE_STATE_SEND_STANDBY_CMD:
        {
            RemoteSendStandbyStateHandler(nInstNum, nRemoteNum, poWkslpRemoteData);
            break;
        }

        // Handle send sleep command state of remote
        case EAL_WKSLP_REMOTE_STATE_SEND_SLEEP_CMD:
        {
            RemoteSendSleepStateHandler(nInstNum, nRemoteNum, poWkslpRemoteData);
            break;
        }

        // Handle waiting to standby state of remote
        case EAL_WKSLP_REMOTE_STATE_WAITING_TO_STANDBY:
        {
            RemoteWaitingToStandbyStateHandler(poWkslpRemoteData, nInstNum, nRemoteNum);
            break;
        }

        // Handle waiting to sleep state of remote
        case EAL_WKSLP_REMOTE_STATE_WAITING_TO_SLEEP:
        {
            RemoteWaitingToSleepStateHandler(poWkslpRemoteData, nInstNum, nRemoteNum);
            break;
        }

        // Handle ping node state of remote
        case EAL_WKSLP_REMOTE_STATE_PING_NODE:
        {
            RemotePingNodeStateHandler(nInstNum, nRemoteNum, poWkslpRemoteData);
            break;
        }

        // Handle waiting for ping response state of remote
        case EAL_WKSLP_REMOTE_STATE_WAITING_FOR_PING_RESP:
        {
            RemoteWaitingForPingRespStateHandler(poWkslpRemoteData, nInstNum, nRemoteNum);
            break;
        }

        default:
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Undefined remote wake-sleep state");
            ADI_DBG_ERROR();
            break;
        }
    }
}

/*! \endcond */

// ---------------------- PUBLIC CODE ---------------------
/**
 * @brief       Initializes the wake-sleep component
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  poWkslpCfg          Configurations for wake-sleep component
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS   Wake-sleep component successfully initialized
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_WKSLP_CODE
ADI_EAL_STATUS adi_eal_wkslpInit(uint32_t nInstNum, ADI_EAL_WKSLP_CFG *poWkslpCfg)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(poWkslpCfg != NULL, "Wake - sleep config param is NULL");
    ADI_DBG_REQUIRE(poWkslpCfg->pfWkslpCallback != NULL, "Wake - sleep callback function pointer is NULL");

    // Clear the instance memory
    (void) ADI_MEMSET(&gaoEalWkslpData[nInstNum], 0, sizeof(EAL_WKSLP_DATA));

    // Save the configurations
    gaoEalWkslpData[nInstNum].poWkslpCfg = poWkslpCfg;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Tick function that needs to be called periodically to change states
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS   Call completed successfully
 */
ADI_EAL_WKSLP_CODE
ADI_EAL_STATUS adi_eal_wkslpRun(uint32_t nInstNum)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");

    if(gaoEalWkslpData[nInstNum].poWkslpCfg != NULL)
    {
        uint32_t nNumRemotes = gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes;

        // Tick all the remote nodes' state machine
        for(uint32_t i = 0U; i < nNumRemotes; i++)
        {
            bool bEnabled = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[i].bEnabled;

            if(bEnabled == true)
            {
                RunRemoteNode(nInstNum, i);
            }
        }
    }

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Sets the remote node to standby mode
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               Call completed successfully
 *              - #ADI_EAL_STATUS_WKSLP_NODE_NOT_AWAKE  Remote node / controller node not awake to switch to standby
 */
ADI_EAL_WKSLP_CODE
ADI_EAL_STATUS adi_eal_wkslpStandbyNode(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    // Get the current state of the node
    EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData = &gaoEalWkslpData[nInstNum].aoEalRemoteWkslpData[nRemoteNum];
    EAL_WKSLP_REMOTE_STATES eCurrState = poWkslpRemoteData->eRemoteNodeState;

    // Can only switch to standby if remote node is awake
    if(eCurrState == EAL_WKSLP_REMOTE_STATE_AWAKE)
    {
        // Set goto_standby flag
        poWkslpRemoteData->bGotoStandby = true;
    }
    else
    {
        eEalStatus = ADI_EAL_STATUS_WKSLP_NODE_NOT_AWAKE;
    }

    return eEalStatus;
}

/**
 * @brief       Sets the remote node to sleep mode
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               Call completed successfully
 *              - #ADI_EAL_STATUS_WKSLP_NODE_NOT_AWAKE  Remote node / controller node not awake to switch to sleep
 */
ADI_EAL_WKSLP_CODE
ADI_EAL_STATUS adi_eal_wkslpSleepNode(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    // Get the current state of the node
    EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData = &gaoEalWkslpData[nInstNum].aoEalRemoteWkslpData[nRemoteNum];
    EAL_WKSLP_REMOTE_STATES eCurrState = poWkslpRemoteData->eRemoteNodeState;

    // Can only switch to sleep if remote node is awake
    if(eCurrState == EAL_WKSLP_REMOTE_STATE_AWAKE)
    {
        // Set goto_sleep flag
        poWkslpRemoteData->bGotoSleep = true;
    }
    else
    {
        eEalStatus = ADI_EAL_STATUS_WKSLP_NODE_NOT_AWAKE;
    }

    return eEalStatus;
}

/**
 * @brief       Sends a wake-up sequence pulse from remote node
 *              to wake up all the nodes in the network
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS               Call completed successfully
 *              - #ADI_EAL_STATUS_WKSLP_NODE_NOT_AWAKE  Controller node / remote node is not awake to send wake-up pulse
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL         Network buffer is full, retry after sometime
 *              - #ADI_EAL_STATUS_START_FAILED          Failed to start creating the E2B packet
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED       Failed to add register map write command
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_WKSLP_CODE
ADI_EAL_STATUS adi_eal_wkslpWakeNetwork(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    // Get the current state of the node
    EAL_WKSLP_REMOTE_DATA *poWkslpRemoteData = &gaoEalWkslpData[nInstNum].aoEalRemoteWkslpData[nRemoteNum];
    EAL_WKSLP_REMOTE_STATES eCurrState = poWkslpRemoteData->eRemoteNodeState;

    // Can only send wake-up pulse if remote node is awake
    if(eCurrState == EAL_WKSLP_REMOTE_STATE_AWAKE)
    {
        // Send wake up sequence command from remote
        eEalStatus = SendCommandRemote(nInstNum, nRemoteNum, EAL_WKSLP_CMD_SEND_WUS);
    }
    else
    {
        eEalStatus = ADI_EAL_STATUS_WKSLP_NODE_NOT_AWAKE;
    }

    return eEalStatus;
}

/**
 * @brief       Get the remote node status (AWAKE / SLEEP / STANDBY)
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [out] peNodeStatus        Pointer to the node status
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           Call completed successfully
 */
ADI_EAL_WKSLP_CODE
ADI_EAL_STATUS adi_eal_wkslpGetNodeStatus(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_EAL_WKSLP_NODE_STATUS *peNodeStatus)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");
    ADI_DBG_REQUIRE(peNodeStatus != NULL, "Pointer passed for the node status is NULL");

    // Get the current status of the remote node
    *peNodeStatus = gaoEalWkslpData[nInstNum].aoEalRemoteWkslpData[nRemoteNum].eRemoteNodeStatus;

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Get the remote node reason for sleep / wake
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           Successfully added the request to queue
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Buffer not available to queue the request
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API to transmit the queued ethernet frame.
 */
ADI_EAL_WKSLP_CODE
ADI_EAL_STATUS adi_eal_wkslpGetReasonForWkslp(uint32_t nInstNum, uint32_t nRemoteNum)
{
    // ~~~~~ Local variables ~~~~~
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");

    // Network frame configurations
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    // E2B message options
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void)(ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS)));
    oMsgOpts.bMailbox       = false;
    oMsgOpts.bSync          = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    // Read the reason for wake / sleep register
    uint16_t anRegAddr[1u] = { (uint16_t)IO_SLEEP_HISTORY_NW_ADDR};

    // Configurations for the register map message
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)(ADI_MEMSET(&oRegCfg, 0, sizeof(ADI_E2BCORE_REGMAP_IO_CONFIG)));
    oRegCfg.eType       = ADI_E2BCORE_RND_READ,
    oRegCfg.nLength     = 1u,
    oRegCfg.anStartAddr = &anRegAddr[0u],

    // Set flag to indicate top-level EAL file to forward ping response event
    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = true;

    // Send the register map message
    eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);

    // Clear the notify flag
    if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = false;
    }

    return eEalStatus;
}

/**
 * @brief       Notify the current state of remote node (Awake / sleep / standby)
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  eState              Current state of the node
 *
 */
ADI_EAL_WKSLP_CODE
void adi_eal_wkslpNotifyNodeState(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_WKSLP_NODE_STATUS eState)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");

    // Get the wake-sleep data of the remote node
    EAL_WKSLP_REMOTE_DATA *poWkslpData = &gaoEalWkslpData[nInstNum].aoEalRemoteWkslpData[nRemoteNum];
    (void) ADI_MEMSET(poWkslpData, 0, sizeof(EAL_WKSLP_REMOTE_DATA));

    // Set the node state as awake
    if(eState == ADI_EAL_WKSLP_NODE_AWAKE)
    {
        poWkslpData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_AWAKE;
    }
    // Set the node state as sleeping
    else if(eState == ADI_EAL_WKSLP_NODE_SLEEP)
    {
        poWkslpData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_SLEEP;
    }
    // Set the node state as standby
    else if(eState == ADI_EAL_WKSLP_NODE_STANDBY)
    {
        poWkslpData->eRemoteNodeState = EAL_WKSLP_REMOTE_STATE_STANDBY;
    }
    // Shouldn't come here!
    else
    {
        ADI_DBG_ERROR();
    }

    // Set the status of the remote node
    poWkslpData->eRemoteNodeStatus = eState;
}

// ------------------------ EVENTS ------------------------
/*! \cond PRIVATE */

/**
 * @brief       This function allows the EAL top component to notify the
 *              wake-sleep component of an interrupt arrival
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  eWakeSleepIntEvt    Wake-sleep interrupt event
 */
ADI_EAL_WKSLP_CODE
bool eal_notify_WkslpInterrupt(uint32_t nInstNum, uint32_t nRemoteNum, EAL_WKSLP_INTERRUPT_EVT eWakeSleepIntEvt)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    // Get the config structure
    ADI_EAL_WKSLP_CFG *poWkslpCfg = gaoEalWkslpData[nInstNum].poWkslpCfg;

    bool bInterruptProcessed = false;

    // Check if wake-sleep component is initialized
    if(poWkslpCfg != NULL)
    {
        // Remote node data structure
        EAL_WKSLP_REMOTE_DATA *poRemoteData = &gaoEalWkslpData[nInstNum].aoEalRemoteWkslpData[nRemoteNum];

        ADI_DBG_REQUIRE(poWkslpCfg != NULL, "Config pointer null in callback");
        ADI_DBG_REQUIRE(poWkslpCfg->pfWkslpCallback != NULL, "Wake / sleep callback not registered");

        switch(eWakeSleepIntEvt)
        {
            case EAL_WKSLP_REMOTE_NODE_CONF_AWAKE_EVT:
            {
                // Node confirm awake interrupt event
                poRemoteData->bConfirmAwakeInt = true;
                break;
            }

            case EAL_WKSLP_REMOTE_NODE_RET_FROM_STBY_EVT:
            {
                // Node returned from standby to awake state
                poRemoteData->bReturnedFromStandbyInt = true;
                break;
            }

            case EAL_WKSLP_REQ_SYS_WAKEUP_CONF_EVT:
            {
                // Request to confirm system awake state
                poRemoteData->bReqSysWakeupConfInt = true;
                break;
            }

            case EAL_WKSLP_LINE_BUSY_EVT:
            {
                // Line busy event
                poRemoteData->bLineBusyInt = true;
                break;
            }

            case EAL_WKSLP_SLEEP_ERR_EVT:
            {
                // Sleep error event
                poRemoteData->bSleepErrInt = true;
                break;
            }

            case EAL_WKSLP_STANDBY_ERR_EVT:
            {
                // Standby error event
                poRemoteData->bStandbyErrInt = true;
                break;
            }

            case EAL_WKSLP_PING_RCVD_EVT:
            {
                // Ping received event
                poRemoteData->bPingRcvd = true;
                break;
            }

            default:
            {
                // Unknown event
                ADI_DBG_LOG(LOG_ERROR, stderr, "Unknown wake-sleep interrupt event");
                ADI_DBG_ERROR();
                break;
            }
        }

        bInterruptProcessed = true;
    }

    return bInterruptProcessed;
}

/**
 * @brief       This function allows the EAL top component to notify the
 *              wake-sleep component of a register read event
 *
 * @param [in]  nInstNum            The instance number (Bus index)
 * @param [in]  nRemoteNum          The remote number
 * @param [in]  poRegDat            Pointer to register read data
 */
ADI_EAL_WKSLP_CODE
void eal_notify_WkslpRegReadResp(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
    // Pre-conditions
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is not in supported range");
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    // Get the config structure
    ADI_EAL_WKSLP_CFG *poWkslpCfg = gaoEalWkslpData[nInstNum].poWkslpCfg;

    // Check if wake-sleep component is initialized
    if(poWkslpCfg != NULL)
    {
        ADI_DBG_REQUIRE(poWkslpCfg->pfWkslpCallback != NULL, "Wake / sleep callback not registered");

        if(poRegDat->poInfo->anStartAddr[0U] == LCE_DEVICE_MAC_ADDRESS_LOWER_NW_ADDR)
        {
            if(eal_notify_WkslpInterrupt(nInstNum, nRemoteNum, EAL_WKSLP_PING_RCVD_EVT) == false)
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to notify wake-sleep ping received event");
            }
            else
            {
                /* Nothing to do */
            }
        }
        else if(poRegDat->poInfo->anStartAddr[0U] == IO_SLEEP_HISTORY_NW_ADDR && poRegDat->poInfo->nLength == 1U)
        {
            /* Reason for wake / sleep data */
            ADI_EAL_WKSLP_CBK_EVT_REASON_FOR_WKSLP_DATA oData;
            oData.nInstNum = nInstNum;
            oData.nRemoteNum = nRemoteNum;

            uint32_t *panRegData = poRegDat->poInfo->pBuffer;

            /* Get reason for sleep */
            uint32_t nReasonForSleep = (panRegData[0U] & BITM_IO_SLEEP_HISTORY_REASON_FOR_SLEEP);
            oData.eReasonForSleep = (ADI_REASON_FOR_SLEEP)nReasonForSleep;

            /* Get reason for wake */
            uint32_t nReasonForWake = (panRegData[0U] & BITM_IO_SLEEP_HISTORY_REASON_FOR_WAKE) >>
                                      BITP_IO_SLEEP_HISTORY_REASON_FOR_WAKE;
            oData.eReasonForWake = (ADI_REASON_FOR_WAKE)nReasonForWake;

            /* Clear the flag that routes the register reads from top-level EAL to this module */
            gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp = false;

            /* Notify the application of the reason for wake / sleep */
            poWkslpCfg->pfWkslpCallback(ADI_EAL_WKSLP_CBK_EVT_REASON_FOR_WKSLP, &oData);
        }
        else
        {
            /* Nothing to do */
        }
    }
}


#endif /* ADI_E2B_IFACE_WKSLP_ENABLED */
/*! \endcond */
/** @} */

/**
 * EOF: www.analog.com
 */
