/*******************************************************************************
Copyright (c) 2021-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal.c
 * @brief: The ADI EAL APIs
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"

/*! \cond PRIVATE */
/* ----------------------- DEFINES ------------------------ */
#define MAC_ADDR_MAX_ENTRIES    (15U)                 /*!< Indicates the maximum number of MAC addresses in filter table excluding PTP filtering in OTP */
#define CTRL_PLCA_ID            (0U)                  /*!< Indicates the PLCA ID of the controller node */
#define CFG_UPDATE_TIMEOUT_NS   (500ULL * MS_TO_NS)   /*!< Indicates the timeout for config update */
#define PLCA_STABILIZE_TIME_US  (5ULL * MS_TO_US)     /*!< Indicates the time in microseconds for the plca to stabilize */
#define REMOTE_MAC_ADDR_MASK    \
{\
    (uint8_t)(((uint32_t)LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0x0000FF00U) >> 8U),  \
    (uint8_t)(((uint32_t)LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0x000000FFU) >> 0U),  \
    (uint8_t)(((uint32_t)LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0xFF000000U) >> 24U), \
    (uint8_t)(((uint32_t)LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0x00FF0000U) >> 16U), \
    (uint8_t)(((uint32_t)LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0x0000FF00U) >> 8U),  \
    (uint8_t)(((uint32_t)LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0x000000FFU)),        \
}                                                   /*!< Indicates the remote mac address mask */

/* ---------------------- DATA TYPES ---------------------- */

/* ------------------------- DATA ------------------------- */
/**
 * @brief EAL top internal data structure
 */
ADI_EAL_DATA
EAL_DATA gaoEalData[ADI_EAL_MAX_NUM_INSTANCES];
/**
 * @brief Array to hold the regmap transaction addresses
 */
ADI_EAL_DATA
static uint16_t ganRegmapAddr[ADI_EAL_MAX_NUM_INSTANCES][E2B_REGMAP_MAX];
/**
 * @brief Array to hold the regmap transaction masks
 */
ADI_EAL_DATA
static uint32_t ganRegmapMask[ADI_EAL_MAX_NUM_INSTANCES][E2B_REGMAP_MAX];
/**
 * @brief Array to hold the regmap transaction data
 */
ADI_EAL_DATA
static uint32_t ganRegmapData[ADI_EAL_MAX_NUM_INSTANCES][E2B_REGMAP_MAX];

/* --------------------- PRIVATE CODE --------------------- */
/**
 * @brief Applies topic override for node-level register transactions to address
 *        the silicon anomaly where pre-configured LCE topics require an explicit
 *        topic_node=0 (TOPIC_NODE) override on every RegMap IO message.
 *
 * @details This helper is called from all EAL APIs that build node-level register
 *          transactions.  The flag bLCETopicConfigured transitions through two phases:
 *            - @b Pre-configuration: set from poE2bCoreCfg->bNodeConfigured at
 *              adi_eal_createInstance() time (true for OTP-programmed nodes, false
 *              otherwise).  When false, bTopicOverRide is disabled and the topic
 *              override is not applied.
 *            - @b Post-configuration: unconditionally set to true at the end of
 *              adi_eal_configureNetwork(), after which bTopicOverRide is always enabled.
 *
 * @param [in]     nInstNum   Instance number
 * @param [in,out] poMsgOpts  Pointer to message options structure to be configured
 */
ADI_EAL_CODE
void adi_eal_configureTopicOverride(uint32_t nInstNum, ADI_E2BCORE_MSGOPTS *poMsgOpts)
{
    poMsgOpts->nTopic = ADI_E2BCORE_TOPIC_NODE;
    poMsgOpts->bTopicOverRide = gaoEalData[nInstNum].bLCETopicConfigured;
}

/** @brief Discover the MAC address of the remote nodes */
ADI_EAL_CODE
static ADI_EAL_STATUS DiscoverMacAddr(uint32_t nInstNum)
{
    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U];
    eal_utils_fillNetworkCfg(nInstNum, ADI_EAL_ALL_REMOTES, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */

    /* Read the MAC address registers for discovery */
    ganRegmapAddr[nInstNum][0U] = LCE_DEVICE_MAC_ADDRESS_LOWER_NW_ADDR;
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = 2U;
    aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];

    /* Create the frame and send it */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/** @brief Add reads of the node configurations */
ADI_EAL_CODE
static ADI_EAL_STATUS AddReadNodeCfgs(uint32_t nInstNum, ADI_E2BCORE_REMOTE_CONFIG *poRemoteCfg)
{
    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    uint8_t nNumRegs = 0U;

    /* ~~~~~ e2bcore message options ~~~~~ */
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* Loop through all the registers in the config structure */
    for(uint32_t i = 0u; i < poRemoteCfg->nNumRegisters; i++)
    {
        /* Read all registers from the configuration */
        /* structure except MAC address registers */
        uint16_t nAddr = poRemoteCfg->aoRegData[i].nAddr;

        if(nAddr != LCE_DEVICE_MAC_ADDRESS_LOWER_NW_ADDR && nAddr != LCE_DEVICE_MULTICAST_ADDRESS_UPPER_NW_ADDR)
        {
            ganRegmapAddr[nInstNum][nNumRegs] = nAddr;
            nNumRegs++;
        }
    }

    /* ~~~~~ Register map message configuration ~~~~~ */
    aoRegMapCfg[0U].eType = ADI_E2BCORE_RND_READ;
    aoRegMapCfg[0U].nLength = nNumRegs;
    aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];

    /* Add the register message */
    if(adi_e2bcore_addRegmapIO(nInstNum, &oMsgOpts, aoRegMapCfg, 1U) != ADI_E2BCORE_ERR_NONE)
    {
        eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
    }

    return eEalStatus;
}

/** @brief Add reads of the interface configurations */
ADI_EAL_CODE
static ADI_EAL_STATUS AddReadIntfCfgs(uint32_t nInstNum, ADI_E2BCORE_REMOTE_CONFIG *poRemoteCfg)
{
    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));

    for(uint32_t i = 0U; i < poRemoteCfg->nNumIf; i++)
    {
        ADI_E2BCORE_IF_CONFIG *poIntfCfg = &poRemoteCfg->aoIntfConfigs[i];

        if(poIntfCfg->bEnabled == true)
        {
            /* ~~~~ Interface registers ~~~~ */

            /* ~~~~~ e2bcore message options ~~~~~ */
            oMsgOpts.bMailbox = false;
            oMsgOpts.bSync = true;
            oMsgOpts.bTopicOverRide = true;
            oMsgOpts.nTopic = poIntfCfg->nIfTopic;

            for(uint32_t j = 0U; j < poIntfCfg->nNumIfReg; j++)
            {
                ganRegmapAddr[nInstNum][j] = poIntfCfg->aoIfRegData[j].nAddr;
            }

            aoRegMapCfg[0U].eType = ADI_E2BCORE_RND_READ;
            aoRegMapCfg[0U].nLength = (uint8_t)poIntfCfg->nNumIfReg;
            aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];

            /* Add the regmap message */
            if(adi_e2bcore_addRegmapIO(nInstNum, &oMsgOpts, aoRegMapCfg, 1U) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
                break;
            }

            /* ~~~~ FIFO registers ~~~~ */

            /* ~~~~~ e2bcore message options ~~~~~ */
            oMsgOpts.bMailbox = false;
            oMsgOpts.bSync = true;
            oMsgOpts.bTopicOverRide = true;
            oMsgOpts.nTopic = poIntfCfg->nFifoTopic;

            for(uint32_t j = 0U; j < poIntfCfg->nNumFifoReg; j++)
            {
                ganRegmapAddr[nInstNum][j] = poIntfCfg->aoFifoRegData[j].nAddr;
            }

            aoRegMapCfg[0U].eType = ADI_E2BCORE_RND_READ;
            aoRegMapCfg[0U].nLength = (uint8_t)poIntfCfg->nNumFifoReg;
            aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];

            /* Add the regmap message */
            if(adi_e2bcore_addRegmapIO(nInstNum, &oMsgOpts, aoRegMapCfg, 1U) != ADI_E2BCORE_ERR_NONE)
            {
                eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
                break;
            }
        }
    }

    return eEalStatus;
}

/** @brief Discover configurations of a remote node */
ADI_EAL_CODE
static ADI_EAL_STATUS DiscoverRemoteCfg(uint32_t nInstNum, ADI_E2BCORE_REMOTE_CONFIG *pRemoteCfg)
{
    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
    uint32_t nActLength = 0U;
    uint8_t *pBuffer = NULL;
    uint8_t *panMacAddr;

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;

    /* Reset the buffer index and pointer */
    pBuffer = NULL;

    /* Get the remote MAC address */
    panMacAddr = pRemoteCfg->anMacAddr;

    /* Set the destination MAC address */
    oNwFrameCfg.panPhysAddr = panMacAddr;

    /* Set MAC routing info */
    oNwFrameCfg.eMacRoutingInfo = pRemoteCfg->bIsLocalLCE ? ADI_MACPHY_ROUTE_PORT1_HP : ADI_MACPHY_ROUTE_PORT0_LP;

    do
    {
        if(adi_network_getTxBuffer(nDevNum, &pBuffer, &oNwFrameCfg) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        if(adi_e2bcore_startPacket(nInstNum, panMacAddr, pBuffer, oNwFrameCfg.nLenByte, false) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* Add read of node configurations */
        eEalStatus = AddReadNodeCfgs(nInstNum, pRemoteCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Add read of interface configurations */
        eEalStatus = AddReadIntfCfgs(nInstNum, pRemoteCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

#ifndef ADI_SERDES_MODE

        /* Add an idle message at the end */
        /** @note: We need this to work-around an issue in case of local LCE access */
        if(adi_e2bcore_addIdleMessage(nInstNum) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }

#endif
        /* Finish adding all messages to the packet */
        adi_e2bcore_finishPacket(nInstNum, &nActLength,
                                 oNwFrameCfg.bLenInclMacHdr, oNwFrameCfg.nNumVlanTags);

        /* Update the actual length used */
        oNwFrameCfg.nLenByte = (uint16_t)nActLength;

        /* Transmit the frame */
        adi_network_transmit(nDevNum, &oNwFrameCfg);

        /* Flush the network layer buffers */
        adi_network_flushTxBuffer(nDevNum, 0ULL);
    } while(false);

    return eEalStatus;
}

/** @brief Discover the configurations */
ADI_EAL_CODE
static ADI_EAL_STATUS DiscoverCfgs(uint32_t nInstNum)
{
    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    ADI_E2BCORE_CFG *poCfg = gaoEalData[nInstNum].poE2bCoreCfg;

    /* Loop through all the nodes */
    for(uint32_t i = 0U; i < poCfg->nNumNodes; i++)
    {
        /* Get the remote configuration */
        ADI_E2BCORE_REMOTE_CONFIG *pRemoteCfg = &poCfg->aoRemoteCfgs[i];

        if(pRemoteCfg->bEnabled == true)
        {
            /* Discover the remote configuration */
            eEalStatus = DiscoverRemoteCfg(nInstNum, pRemoteCfg);

            if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }
        }
    }

    /* Incase of any failure in the above sequence, the network mutex must be released */
    if(eEalStatus != ADI_EAL_STATUS_NTW_BUFF_FULL && eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        (void)adi_network_releaseMutex();
    }

    return eEalStatus;
}

/**
 * @brief       Clears the interrupt so that it can get triggered again in the future
 *
 * @param [in]  poID            Pointer to the ID of the node
 * @param [in]  nAddr           Address of the interrupt clear register
 * @param [in]  nData           Register data to clear the interrupt
 */
ADI_EAL_CODE
static void ClearInterrupt(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bIsEdgeIrq = (poIntDat->eType == ADI_E2BCORE_INT_RISING) || (poIntDat->eType == ADI_E2BCORE_INT_FALLING);
    bool bIsHighOrRisingIrq = (poIntDat->eType == ADI_E2BCORE_INT_HIGH) || (poIntDat->eType == ADI_E2BCORE_INT_RISING);
    uint16_t nClearIrqAddr = 0U;
    uint32_t nClearIrqData = (uint32_t)poIntDat->nMask << (bIsHighOrRisingIrq ? 0U : 8U);

    /* Get the address to clear the inetrrupt for the corresponding irq identifier and irq level */
    if(poIntDat->nIdentifier < 31U)
    {
        /* All interrupts till identifier = 30 are in order! */
        nClearIrqAddr = IO_TIC_CLEAR_0_FIFO0_READ_REG0_NW_ADDR +
                        ((uint16_t)poIntDat->nIdentifier * 2U);
    }
    else if(poIntDat->nIdentifier == 31U)
    {
        nClearIrqAddr = IO_TIC_CLEAR_31_ADC_CONTROLLER_REG0_NW_ADDR;
    }
    else if(poIntDat->nIdentifier == 32U)
    {
        nClearIrqAddr = IO_TIC_CLEAR_32_LCE_SMC_SQI_REG0_NW_ADDR;
    }
    else if(poIntDat->nIdentifier == 33U)
    {
        nClearIrqAddr = IO_TIC_CLEAR_33_GPIO0_REG0_NW_ADDR;
    }
    else if(poIntDat->nIdentifier == 34U)
    {
        nClearIrqAddr = IO_TIC_CLEAR_34_GPIO1_REG0_NW_ADDR;
    }
    else
    {
        /* Nothing to do! */
        ADI_DBG_REQUIRE(false, "Invalid interrupt identifier");
        ADI_DBG_ERROR();
    }

    if(!bIsEdgeIrq)
    {
        nClearIrqAddr += 1U;
    }

    /* Register map transaction configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegCfg[1U];
    (void)ADI_MEMSET(&aoRegCfg[0U], 0, sizeof(aoRegCfg));
    aoRegCfg[0U].eType       = ADI_E2BCORE_RND_WRITE;
    aoRegCfg[0U].anStartAddr = &nClearIrqAddr;
    aoRegCfg[0U].pBuffer     = &nClearIrqData;
    aoRegCfg[0U].nLength     = 1u;

    /* Send the frame to clear the interrupt */
    if(adi_eal_sendRegTransaction(poID->nInstNum, poID->nRemoteNum, &aoRegCfg[0U]) != ADI_EAL_STATUS_SUCCESS)
    {
        ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to send reg transaction to clear interrupt");
    }
}

/* Invoke node callback */
ADI_EAL_CODE
static void InvokeNodeCbk(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_NODE_CBK_EVT eNodeCbkEvt, const void *pData)
{
    /* Get the node callback function */
    ADI_EAL_NODE_CALLBACK_FUNC pfEalNodeCbk = gaoEalData[nInstNum].pfEalNodeCbk;

    /* Invoke the node callback */
    if(pfEalNodeCbk != NULL)
    {
        pfEalNodeCbk(nInstNum, nRemoteNum, eNodeCbkEvt, pData);
    }
}

/** Handle top-level error event */
ADI_EAL_CODE
static void HandleTopLevelErrCbk(uint32_t nInstNum, void *poParam)
{
    /* Extract the error data */
    ADI_E2BCORE_CBK_ERRDAT *poDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Get the top-level callback function */
    ADI_EAL_TOP_CALLBACK_FUNC pfEalTopCbk = gaoEalData[nInstNum].pfEalTopCbk;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "ADI_E2BCORE_ERR: %d\n\r", poDat->eError);

    /* Notify the application of the error */
    if(pfEalTopCbk != NULL)
    {
        pfEalTopCbk(nInstNum, ADI_EAL_CBK_EVT_ERR, poDat);
    }
}

/** Handle MAC address received event */
ADI_EAL_CODE
static void HandleMacAddrRcvdCbk(uint32_t nInstNum, void *poParam)
{
    /* Received MAC address as part of discovery */
    uint8_t *panMacAddr = (uint8_t *)poParam;
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tMAC address discovery for instance %d: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                nInstNum, panMacAddr[0u], panMacAddr[1u], panMacAddr[2u], panMacAddr[3u], panMacAddr[4u], panMacAddr[5u]);

    ADI_EAL_TOP_CALLBACK_FUNC pfEalTopCbk = gaoEalData[nInstNum].pfEalTopCbk;

    /* Notify the application of the MAC address received event */
    if(pfEalTopCbk != NULL)
    {
        pfEalTopCbk(nInstNum, ADI_EAL_CBK_EVT_MAC_ADDR_RCVD, panMacAddr);
    }
}

/* Handle get init buffer event */
ADI_EAL_CODE
static void HandleInitGetBufCbk(uint32_t nInstNum, void *poParam)
{
    /* Pointer to the buffer location */
    uint8_t *pBuffer = NULL;
    /* Length of the packet required by the application */
    uint16_t nLength = ADI_EAL_MAX_FRAME_LENGTH;
    /* Get the get buffer data */
    ADI_E2BCORE_CBK_INITGETBUFDAT *poGetBufDat = (ADI_E2BCORE_CBK_INITGETBUFDAT *)poParam;
    /* Get the device number */
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
    /* Config for getting buffer from network layer */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = nLength;
    oNwFrameCfg.panPhysAddr     = poGetBufDat->panMacAddr;
    oNwFrameCfg.nNumVlanTags    = (uint8_t)poGetBufDat->nNumVlanTags;
    /* Request buffer from network layer for e2bcore to do the initializations */
    ADI_NETWORK_ERR eNwErr = adi_network_getTxBuffer(nDevNum, &pBuffer, &oNwFrameCfg);

    if(eNwErr == ADI_NETWORK_OK)
    {
        poGetBufDat->pBuffer = pBuffer;
        poGetBufDat->nLength = oNwFrameCfg.nLenByte;
    }
    else
    {
        /* Explicitly set buffer to NULL so e2bcore propagates the error */
        poGetBufDat->pBuffer = NULL;
    }
}

/* Handle init transmit event */
ADI_EAL_CODE
static void HandleInitTransmitCbk(uint32_t nInstNum, void *poParam)
{
    /* Transmit the initialization packets generated by e2bcore */
    ADI_E2BCORE_CBK_INITTRANSMITDAT *poTransmitDat = (ADI_E2BCORE_CBK_INITTRANSMITDAT *)poParam;
    /* Get the device number */
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
    /* Config for getting buffer from network layer */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    /* Update the destination address and the actual length of the frame */
    oNwFrameCfg.panPhysAddr = poTransmitDat->anDestAddr;
    oNwFrameCfg.nLenByte = (uint16_t)(poTransmitDat->nActLength);

    /* Check if dual routing should be enabled */
    if(gaoEalData[nInstNum].poE2bCoreCfg->bIsLocalLCEConfigured
            && (ADI_MEMCMP(poTransmitDat->anDestAddr, &ganAdiE2bMulticastAddr[0U], 6u) == 0))
    {
        oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_BOTH;
    }
    else if(poTransmitDat->bToLocalLCE)
    {
        oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT1_HP;
    }
    else
    {
        /* oNwFrameCfg.eMacRoutingInfo is set to ADI_MACPHY_ROUTE_PORT0_LP already */
    }

    /* Check if VLAN tag is applicable - by seeing if VLAN tag has CTAG TPID */
    if((EXTRACT_VLAN_TPID(poTransmitDat->nVlanTag)) == VLAN_C_TAG_TPID)
    {
        oNwFrameCfg.nNumVlanTags  = 1U;
        oNwFrameCfg.panVlanTagArr = &poTransmitDat->nVlanTag;
    }

    /* Transmit the frame */
    adi_network_transmit(nDevNum, &oNwFrameCfg);
    poTransmitDat->bError = false;
}

/* Handle remote error event */
ADI_EAL_CODE
static void HandleRemoteErrCbk(const ADI_E2BCORE_ID *poID, void *poParam)
{
    /* Instance and remote number */
    uint32_t nInstNum = poID->nInstNum;
    uint32_t nRemoteNum = poID->nRemoteNum;

    ADI_E2BCORE_CBK_ERRDAT *poDat = (ADI_E2BCORE_CBK_ERRDAT *)poParam;

    /* Give a callback to the application with the error information */
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "ADI_E2BCORE_ERR: %d\n\r", poDat->eError);
    InvokeNodeCbk(nInstNum, nRemoteNum, ADI_EAL_NODE_CBK_EVT_ERR, poDat);
}

/* Process FIFO write threshold interrupts */
ADI_EAL_CODE
static bool ProcessFifoWriteThresholdInterrupts(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bInterruptProcessed = false;

    /* Get the interface number from the identifier */
    uint8_t nIntfNum = (poIntDat->nIdentifier - FIFO0_WRITE_THRESHOLDS_ID) / 2U;

    /* Check if rising edge is detected in WRITE_THRESHOLD_2 */
    if(((poIntDat->nMask & (1U << ENUM_IO_TIC_CLEAR_1_FIFO0_WRITE_REG0_TIC_CLEAR_1_RISING_FIFO0_WRITE_THRESHOLD_2_RISING))
            != 0x00U) &&
            (poIntDat->eType == ADI_E2BCORE_INT_RISING))
    {
#ifdef ADI_E2B_IFACE_FIO_PWM_ENABLED
        /* Check if the IRQ is for interface FLEXIO PWM, and route it accordingly*/
        if(gaoEalData[poID->nInstNum].aoEalNodeData[poID->nRemoteNum].aeIntfType[nIntfNum] == EAL_INTERFACE_TYPE_FIO_PWM)
        {
            if((eal_notify_FioPwmInterrupt(poID->nInstNum, poID->nRemoteNum, nIntfNum, EAL_FIO_PWM_HIGH_IRQ_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif

#ifdef ADI_E2B_IFACE_FIO_PDM_ENABLED
        /* Check if the IRQ is for interface FLEXIO PDM, and route it accordingly*/
        if(gaoEalData[poID->nInstNum].aoEalNodeData[poID->nRemoteNum].aeIntfType[nIntfNum] == EAL_INTERFACE_TYPE_FIO_PDM)
        {
            if((eal_notify_FioPdmInterrupt(poID->nInstNum, poID->nRemoteNum, nIntfNum,
                                           EAL_FIO_PDM_WRITE_FIFO_HIGH_IRQ_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif
    }
    /* Check if falling edge is detected in WRITE_THRESHOLD_1 */
    else if(((poIntDat->nMask & (1U <<
                                 ENUM_IO_TIC_CLEAR_1_FIFO0_WRITE_REG0_TIC_CLEAR_1_FALLING_FIFO0_WRITE_THRESHOLD_1_FALLING)) !=
             0x00U) && (poIntDat->eType == ADI_E2BCORE_INT_FALLING))
    {
#ifdef ADI_E2B_IFACE_FIO_PWM_ENABLED
        /* Check if the IRQ is for interface FLEXIO PWM, and route it accordingly*/
        if(gaoEalData[poID->nInstNum].aoEalNodeData[poID->nRemoteNum].aeIntfType[nIntfNum] == EAL_INTERFACE_TYPE_FIO_PWM)
        {
            if((eal_notify_FioPwmInterrupt(poID->nInstNum, poID->nRemoteNum, nIntfNum, EAL_FIO_PWM_LOW_IRQ_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif

#ifdef ADI_E2B_IFACE_FIO_PDM_ENABLED
        /* Check if the IRQ is for interface FLEXIO PDM, and route it accordingly*/
        if(gaoEalData[poID->nInstNum].aoEalNodeData[poID->nRemoteNum].aeIntfType[nIntfNum] == EAL_INTERFACE_TYPE_FIO_PDM)
        {
            if((eal_notify_FioPdmInterrupt(poID->nInstNum, poID->nRemoteNum, nIntfNum,
                                           EAL_FIO_PDM_WRITE_FIFO_LOW_IRQ_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif
    }
    else
    {
        /* Nothing to do */
    }

    return bInterruptProcessed;
}

/* Process FIFO Read threshold interrupts */
ADI_EAL_CODE
static bool ProcessFifoReadThresholdInterrupts(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bInterruptProcessed = false;

    /* Get the interface number from the identifier */
    uint8_t nIntfNum = (poIntDat->nIdentifier - FIFO0_READ_THRESHOLDS_ID) / 2U;

    /* Check if rising edge is detected in READ_THRESHOLD_FULL */
    if(((poIntDat->nMask & (1U << ENUM_IO_TIC_CLEAR_0_FIFO0_READ_REG0_TIC_CLEAR_0_RISING_FIFO0_READ_FULL_RISING))
            != 0x00U) &&
            (poIntDat->eType == ADI_E2BCORE_INT_RISING))
    {
#ifdef ADI_E2B_IFACE_FIO_PDM_ENABLED
        /* Check if the IRQ is for interface FLEXIO PDM, and route it accordingly*/
        if(gaoEalData[poID->nInstNum].aoEalNodeData[poID->nRemoteNum].aeIntfType[nIntfNum] == EAL_INTERFACE_TYPE_FIO_PDM)
        {
            if((eal_notify_FioPdmInterrupt(poID->nInstNum, poID->nRemoteNum, nIntfNum,
                                           EAL_FIO_PDM_READ_FIFO_FULL_IRQ_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif
    }
    else
    {
        /* Nothing to do here */
    }

    return bInterruptProcessed;
}

ADI_EAL_CODE
static bool ProcessFlexibleIoInterrupts(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bInterruptProcessed = false;

#ifdef ADI_E2B_IFACE_FIO_ENABLED
    /* FIO0 is 25 and FIO1 is 26. */
    uint32_t nIntfNum = (uint32_t)poIntDat->nIdentifier - FLEXIO_RISING_CONFIGURABLE_INTERRUPT_0_IRQ_ID;

    if(poIntDat->eType == ADI_E2BCORE_INT_HIGH)
    {
        /* Confirm_awake interrupt */
        if((poIntDat->nMask & FLEXIO_RISING_CONFIGURABLE_INTERRUPT_0_IRQ_MASK) != 0u)
        {
            /* Validate callback is registered before invoking */
            if(gaoEalData[poID->nInstNum].aoEalNodeData[poID->nRemoteNum].aeIntfCbk[nIntfNum] != NULL)
            {
                if(gaoEalData[poID->nInstNum].aoEalNodeData[poID->nRemoteNum].aeIntfCbk[nIntfNum](poID->nInstNum,
                        poID->nRemoteNum, nIntfNum, ADI_EAL_NODE_CBK_EVT_FLEXIO_IRQS))
                {
                    bInterruptProcessed = true;
                }
            }
        }
    }
#else
    (void)poID;
    (void)poIntDat;
#endif

    return bInterruptProcessed;
}

/* Process sleep wake interrupts */
ADI_EAL_CODE
static bool ProcessSleepWakeInterrupts(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bInterruptProcessed = false;

#ifdef ADI_E2B_IFACE_WKSLP_ENABLED
    if(poIntDat->eType == ADI_E2BCORE_INT_RISING)
    {
        /* Confirm_awake interrupt */
        if((poIntDat->nMask & CONFIRM_AWAKE_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_WkslpInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_WKSLP_REMOTE_NODE_CONF_AWAKE_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* line_busy interrupt */
        if((poIntDat->nMask & LINE_BUSY_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_WkslpInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_WKSLP_LINE_BUSY_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* sleep_err interrupt */
        if((poIntDat->nMask & SLEEP_ERR_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_WkslpInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_WKSLP_SLEEP_ERR_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* standby_err interrupt */
        if((poIntDat->nMask & STANDBY_ERR_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_WkslpInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_WKSLP_STANDBY_ERR_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* return_from_standby interrupt */
        if((poIntDat->nMask & RET_FROM_STDBY_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_WkslpInterrupt(poID->nInstNum, poID->nRemoteNum,
                                          EAL_WKSLP_REMOTE_NODE_RET_FROM_STBY_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* system wake up request confirmation interrupt */
        if((poIntDat->nMask & REQ_SYS_WK_CONF_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_WkslpInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_WKSLP_REQ_SYS_WAKEUP_CONF_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
    }
#else
    (void)poID;
    (void)poIntDat;
#endif

    return bInterruptProcessed;
}

/* Process misc interrupts */
ADI_EAL_CODE
static bool ProcessMiscInterrupts(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bInterruptProcessed = false;

    /* BOOTLOADER_DONE interrupt */
    if((poIntDat->nMask & (1U << ENUM_IO_TIC_CLEAR_29_MISC1_REG0_TIC_CLEAR_29_RISING_BOOTLOADER_DONE_RISING)) != 0U)
    {
        if(poIntDat->eType == ADI_E2BCORE_INT_RISING)
        {
            /* Notify the application of bootloader_done event */
            InvokeNodeCbk(poID->nInstNum, poID->nRemoteNum, ADI_EAL_NODE_CBK_EVT_BOOTLOADER_DONE, NULL);
            bInterruptProcessed = true;
        }
    }

    return bInterruptProcessed;
}

/* Process LCE / SMC / FIO interrupts */
ADI_EAL_CODE
static bool ProcessLceSmcFioInterrupts(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bInterruptProcessed = false;

#ifdef ADI_E2B_IFACE_SMC_ENABLED
    /* SMC interrupts */
    if(
        (poIntDat->nIdentifier == SMC_IRQ_ID) &&
        (poIntDat->eType == ADI_E2BCORE_INT_RISING)
    )
    {
        /* Region collision interrupt */
        if((poIntDat->nMask & SMC_RISING_REGION_COLLISION_IRQ_MASK) != 0u)
        {
            if((eal_notify_SmcInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SMC_REGION_COLLISION_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* Iteration end interrupt */
        if((poIntDat->nMask & SMC_RISING_ITERATION_END_IRQ_MASK) != 0u)
        {
            if((eal_notify_SmcInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SMC_ITERATION_END_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* Iteration skip interrupt */
        if((poIntDat->nMask & SMC_RISING_ITERATION_SKIP_IRQ_MASK) != 0u)
        {
            if((eal_notify_SmcInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SMC_ITERATION_SKIP_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* Configuration change interrupt */
        if((poIntDat->nMask & SMC_RISING_CONFIG_CHANGE_IRQ_MASK) != 0u)
        {
            if((eal_notify_SmcInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SMC_CONFIGURATION_CHANGE_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
    }
#else
    (void)poID;
    (void)poIntDat;
#endif

    return bInterruptProcessed;
}

/* Process GPIO / SQI / SSC interrupts */
ADI_EAL_CODE
static bool ProcessGpioSqiSscInterrupts(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBK_INTDAT *poIntDat)
{
    bool bInterruptProcessed = false;

    /* Rising edge interrupts */
    if(poIntDat->eType == ADI_E2BCORE_INT_RISING)
    {
#ifdef ADI_E2B_IFACE_SSC_ENABLED
        /* SSC mode change interrupt */
        if((poIntDat->nMask & SSC_MODE_CHANGE_FLAG_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_SscInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SSC_MODE_CHANGE_TO_SAFE_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* SSC unsuccessful return to functional mode interrupt */
        if((poIntDat->nMask & SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_MASK) != 0u)
        {
            if((eal_notify_SscInterrupt(poID->nInstNum, poID->nRemoteNum,
                                        EAL_SSC_UNSUCCESSFUL_RETURN_TO_FUNC_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif

#ifdef ADI_E2B_IFACE_SQI_ENABLED
        /* SQI level warning interrupt */
        if((poIntDat->nMask & SQI_RISING_WARNING_IRQ_MASK) != 0u)
        {
            if((eal_notify_SqiInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SQI_ENTERED_WARNING_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* SQI level bad interrupt */
        if((poIntDat->nMask & SQI_RISING_BAD_IRQ_MASK) != 0u)
        {
            if((eal_notify_SqiInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SQI_ENTERED_BAD_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif
    }
    /* Falling edge interrupts */
    else if(poIntDat->eType == ADI_E2BCORE_INT_FALLING)
    {
#ifdef ADI_E2B_IFACE_SSC_ENABLED
        /* SSC mode change interrupt */
        if((poIntDat->nMask & SSC_MODE_CHANGE_FLAG_FALLING_IRQ_MASK) != 0u)
        {
            if((eal_notify_SscInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SSC_MODE_CHANGE_TO_FUNC_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif

#ifdef ADI_E2B_IFACE_SQI_ENABLED
        /* SQI level warning interrupt */
        if((poIntDat->nMask & SQI_FALLING_WARNING_IRQ_MASK) != 0u)
        {
            if((eal_notify_SqiInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SQI_EXITED_WARNING_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }

        /* SQI level bad interrupt */
        if((poIntDat->nMask & SQI_FALLING_BAD_IRQ_MASK) != 0u)
        {
            if((eal_notify_SqiInterrupt(poID->nInstNum, poID->nRemoteNum, EAL_SQI_EXITED_BAD_EVT)) == true)
            {
                bInterruptProcessed = true;
            }
        }
#endif
    }
    else
    {
        /* Nothing to do */
    }

    return bInterruptProcessed;
}

/* Handle interrupt event */
ADI_EAL_CODE
static void HandleInterruptsCbk(const ADI_E2BCORE_ID *poID, void *poParam)
{
    /* Extract the interrupt data */
    ADI_E2BCORE_CBK_INTDAT *poIntDat = (ADI_E2BCORE_CBK_INTDAT *)poParam;

    /* Clear the received interrupt */
    ClearInterrupt(poID, poIntDat);

    /* Flag indicating if the interrupt is processed by EAL
       or will need to be routed to application */
    bool bInterruptProcessed = false;

    switch(poIntDat->nIdentifier)
    {
        /* FIFO Read thresholds interrupts */
        case 0U:
        {
            bInterruptProcessed = ProcessFifoReadThresholdInterrupts(poID, poIntDat);
            break;
        }

        /* FIFO write thresholds interrupts */
        case 1U:
        case 3U:
        case 5U:
        case 7U:
        {
            bInterruptProcessed = ProcessFifoWriteThresholdInterrupts(poID, poIntDat);
            break;
        }

        /* Flexible IO Interrupts*/
        case 25u:
        {
            bInterruptProcessed = ProcessFlexibleIoInterrupts(poID, poIntDat);
            break;
        }

        /* Sleep wake interrupts */
        case 28U:
        {
            bInterruptProcessed = ProcessSleepWakeInterrupts(poID, poIntDat);
            break;
        }

        /* Misc interrupts (E.g., BOOTLOADER_DONE, GPTP_LOCK, etc. ) */
        case 29U:
        {
            bInterruptProcessed = ProcessMiscInterrupts(poID, poIntDat);
            break;
        }

        /* LCE / SMC / FIO interrupts */
        case 32U:
        {
            bInterruptProcessed = ProcessLceSmcFioInterrupts(poID, poIntDat);
            break;
        }

        /* GPIO / SQI / SSC interrupts */
        case 34U:
        {
            bInterruptProcessed = ProcessGpioSqiSscInterrupts(poID, poIntDat);
            break;
        }

        default:
        {
            /* Nothing to do */
            break;
        }
    }

    if(!bInterruptProcessed)
    {
        /* Notify the application */
        InvokeNodeCbk(poID->nInstNum, poID->nRemoteNum, ADI_EAL_NODE_CBK_EVT_UNK_IRQ, poParam);
    }
}

/* Notify wake sleep module */
ADI_EAL_CODE
static void NotifyWakeSleepModule(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
#ifdef ADI_E2B_IFACE_WKSLP_ENABLED
    /* If notify wake sleep module flag is set, route the register read information */
    if(gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyWkslp == true)
    {
        eal_notify_WkslpRegReadResp(nInstNum, nRemoteNum, poRegDat);
    }
#else
    (void)nInstNum;
    (void)nRemoteNum;
    (void)poRegDat;
#endif
}

/* Notify OTP module */
ADI_EAL_CODE
static void NotifyOtpModule(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
#ifdef ADI_E2B_IFACE_OTP_ENABLED
    /* If notify OTP module flag is set, route the register read information */
    if(gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyOtp == true)
    {
        eal_notify_OtpRegReadResp(nInstNum, nRemoteNum, poRegDat);
    }
#else
    (void)nInstNum;
    (void)nRemoteNum;
    (void)poRegDat;
#endif
}

/* Notify Diagnostics module */
ADI_EAL_CODE
static void NotifyDiagModule(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
#ifdef ADI_E2B_IFACE_DIAG_ENABLED
    /* If address matches the diagnostic registers, route the register read information */
    if(
        (poRegDat->poInfo->anStartAddr[0U] == PHY_OA_PLCA_CTRL1_NW_ADDR) ||
        (poRegDat->poInfo->anStartAddr[0U] == MAC_RX_FRM_CNT_NW_ADDR) ||
        (poRegDat->poInfo->anStartAddr[0U] == PHY_STD_STATUS_REGISTER_NW_ADDR) ||
        (poRegDat->poInfo->anStartAddr[0U] == PHY_STD_PCS_STATUS_NW_ADDR) ||
        (poRegDat->poInfo->anStartAddr[0U] == IO_GPTP_SYNC_COUNT_NW_ADDR))
    {
        eal_notify_DiagRegReadResp(nInstNum, nRemoteNum, poRegDat);
    }
#else
    (void)nInstNum;
    (void)nRemoteNum;
    (void)poRegDat;
#endif
}

/* Notify SSC module */
ADI_EAL_CODE
static void NotifySSCModule(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
#ifdef ADI_E2B_IFACE_SSC_ENABLED
    /* If notify SSC module flag is set, route the register read information */
    if(gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifySsc == true)
    {
        eal_notify_sscRegReadResp(nInstNum, nRemoteNum, poRegDat);
    }
#else
    (void)nInstNum;
    (void)nRemoteNum;
    (void)poRegDat;
#endif
}

/* Notify SQI module */
ADI_EAL_CODE
static void NotifySQIModule(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
#ifdef ADI_E2B_IFACE_SQI_ENABLED
    /* If notify SQI module flag is set, route the register read information */
    if(gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifySqi == true)
    {
        eal_notify_sqiRegReadResp(nInstNum, nRemoteNum, poRegDat);
    }
#else
    (void)nInstNum;
    (void)nRemoteNum;
    (void)poRegDat;
#endif
}

/* Notify topology discovery module */
ADI_EAL_CODE
static void NotifyTopDiscModule(uint32_t nInstNum, uint32_t nRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
#ifdef ADI_E2B_IFACE_TOPDISC_ENABLED
    /* If notify topology discovery module flag is set, route the register read information */
    if(gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyTopoDisc == true)
    {
        eal_notify_TopoDiscRegReadResp(nInstNum, nRemoteNum, poRegDat);
    }
#else
    (void)nInstNum;
    (void)nRemoteNum;
    (void)poRegDat;
#endif
}

/* Handle remote register read event */
ADI_EAL_CODE
static void HandleRemoteRegReadCbk(const ADI_E2BCORE_ID *poID, void *poParam)
{
    uint32_t nInstNum = poID->nInstNum;
    uint32_t nRemoteNum = poID->nRemoteNum;
    ADI_E2BCORE_CBK_REGREADDAT *poRegDat = (ADI_E2BCORE_CBK_REGREADDAT *)poParam;

    /* Notify reg read API to return */
    if(gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bWaitingForResp == true)
    {
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg = gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].poRegCfg;
        (void)ADI_MEMCPY(poRegCfg->pBuffer, poRegDat->poInfo->pBuffer, poRegCfg->nLength * sizeof(uint32_t));
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bWaitingForResp = false;
    }

    /* Notify Wake-sleep module */
    NotifyWakeSleepModule(nInstNum, nRemoteNum, poRegDat);

    /* Notify OTP module */
    NotifyOtpModule(nInstNum, nRemoteNum, poRegDat);

    /* Notify Diag module */
    NotifyDiagModule(nInstNum, nRemoteNum, poRegDat);

    /* Notify SSC Module */
    NotifySSCModule(nInstNum, nRemoteNum, poRegDat);

    /* Notify SQI Module */
    NotifySQIModule(nInstNum, nRemoteNum, poRegDat);

    /* Notify topology discovery Module */
    NotifyTopDiscModule(nInstNum, nRemoteNum, poRegDat);

    /* Ping response */
    if(poRegDat->poInfo->anStartAddr[0U] == MAC_PHYID_NW_ADDR && poRegDat->poInfo->nLength == 1U)
    {
        InvokeNodeCbk(nInstNum, nRemoteNum, ADI_EAL_NODE_CBK_EVT_PING_RCVD, NULL);
    }
    /* Revision number response */
    else if(poRegDat->poInfo->anStartAddr[0U] == MAC_SCRATCH_0_NW_ADDR && poRegDat->poInfo->nLength == 2U)
    {
        ADI_EAL_NODE_REV_NUM oEalRevNum;
        oEalRevNum.nMajorRevNum = poRegDat->poInfo->pBuffer[0U];
        oEalRevNum.nMinorRevNum = poRegDat->poInfo->pBuffer[1U];
        InvokeNodeCbk(nInstNum, nRemoteNum, ADI_EAL_NODE_CBK_EVT_REVNUM_RCVD, &oEalRevNum);
    }
    /* PLCA LUT readback */
    else if(poRegDat->poInfo->anStartAddr[0U] == PHY_OA_PLCA_LUT_DBG_NW_ADDR && poRegDat->poInfo->nLength == 1U)
    {
        uint8_t nPlcaIdFromLut = (uint8_t)(poRegDat->poInfo->pBuffer[0U] & BITM_PHY_OA_PLCA_LUT_DBG_RB_EFFECTIVE_PLCA_ID0);
        InvokeNodeCbk(nInstNum, nRemoteNum, ADI_EAL_NODE_CBK_EVT_LUT_PLCA_ID_RCVD, &nPlcaIdFromLut);
    }
    else
    {
        /* Nothing to do */
    }

    /* Route register read to the application always */
    InvokeNodeCbk(nInstNum, nRemoteNum, ADI_EAL_NODE_CBK_EVT_REG_READ_RESP, poParam);
}

/* ----------------------- CALLBACKS ---------------------- */
/** Callback from network layer */
ADI_EAL_CODE
static void NetworkCbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void *pData)
{
    /* TODO */
    ADI_DBG_ERROR();

    switch(eEvent)
    {
        case ADI_NETWORK_EVT_READ_REG:
        {
            break;
        }

        case ADI_NETWORK_EVT_WRITE_REG:
        {
            break;
        }

        default:
        {
            /* Shouldn't come here since EAL only should have */
            /* registered callbacks for register read / write */
            ADI_DBG_ERROR();
            break;
        }
    }
}

/**
 * @brief Callback from e2bcore at the instance level
 *
 * @param poID          e2bcore instance index source of the callback
 * @param eType         Event type for this callback
 * @param poParam       Data associated with the above event
 * @param bIsTxFrame    Flag indicating if the callback is because of TX / RX frame processing
 */
ADI_EAL_CODE
void Eth10BaseT1sCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                     void *pCbParam)
{
    /* --------------- Library callback for Eth10BaseT1sCbk --------------- */
    /* Instance number */
    uint32_t nInstNum = poID->nInstNum;

    /* ---------------- Code ---------------- */
    /* Handle the different cases of callback events */
    switch(eType)
    {
        /* Top-level error callback */
        case ADI_E2BCORE_CBK_ERR:
        {
            HandleTopLevelErrCbk(nInstNum, poParam);
            break;
        }

        /* Init get buffer callback */
        case ADI_E2BCORE_CBK_INITGETBUF:
        {
            HandleInitGetBufCbk(nInstNum, poParam);
            break;
        }

        /* Init transmit callback */
        case ADI_E2BCORE_CBK_INITTRANSMIT:
        {
            HandleInitTransmitCbk(nInstNum, poParam);
            break;
        }

        /* MAC address received callback */
        case ADI_E2BCORE_CBK_MAC_ADDR_RCVD:
        {
            HandleMacAddrRcvdCbk(nInstNum, poParam);
            break;
        }

        default:
        {
            /* Shouldn't come here! */
            ADI_DBG_ERROR();
            break;
        }
    }
}

/**
 * @brief Callback from e2bcore at the remote node level
 *
 * @param poID          e2bcore instance, remote source index of the callback
 * @param eType         Event type for this callback
 * @param poParam       Data associated with the above event
 * @param bIsTxFrame    Flag indicating if the callback is because of TX / RX frame processing
 */
ADI_EAL_CODE
void RemoteNodeCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame,
                   void *pCbParam)
{
    /* --------------- Callback for RemoteNodes --------------- */
    switch(eType)
    {
        /* Handle error event */
        case ADI_E2BCORE_CBK_ERR:
        {
            HandleRemoteErrCbk(poID, poParam);
            break;
        }

        /* Handle interrupt event */
        case ADI_E2BCORE_CBK_INT:
        {
            HandleInterruptsCbk(poID, poParam);
            break;
        }

        /* Handle register read event */
        case ADI_E2BCORE_CBK_REGREAD:
        {
            HandleRemoteRegReadCbk(poID, poParam);
            break;
        }

        /* Shouldn't come here */
        default:
        {
            ADI_DBG_ERROR();
            break;
        }
    }
}

/*! \endcond */
/* ---------------------- PUBLIC CODE --------------------- */
/**
 * @brief       Initializes the EAL
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_CODE
void adi_eal_init(void)
{
    /* Clear the internal EAL data */
    (void) ADI_MEMSET(gaoEalData, 0, sizeof(gaoEalData));
}

/**
 * @brief       Creates an instance of EAL
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nDevNum         Index of the device that needs to be mapped to this instance
 * @param [in]  poE2bCoreCfg    e2bcore configuration structure
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_CODE
void adi_eal_createInstance(uint32_t nInstNum, uint32_t nDevNum, ADI_E2BCORE_CFG *poE2bCoreCfg)
{
    /* Preconditions: */
    /*  * Validate the instance number */
    /*  * Validate the e2bcore cfg structure */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(poE2bCoreCfg != NULL, "Pointer to e2bcore cfg is null");

    EAL_DATA *poEalData = &gaoEalData[nInstNum];

    (void) ADI_MEMSET(poEalData, 0, sizeof(EAL_DATA));

    /* Save the pointer to the network configuration */
    poEalData->poE2bCoreCfg = poE2bCoreCfg;

    /* Set the instance to be "in use" */
    poEalData->bInUse = true;

    /* Initialize LCE topic override flag: true when nodes are OTP-programmed (LCE topics pre-configured) */
    poEalData->bLCETopicConfigured = poE2bCoreCfg->bNodeConfigured;

    /* Create a mapping between instance and device number */
    poEalData->nDevNum = nDevNum;

    /* Initialize the e2bcore layer */
    adi_e2bcore_initStack(nInstNum, poE2bCoreCfg);

    /* Initialize the PLCA mapping depending on the config */
    for(uint32_t i = 0U; i < poE2bCoreCfg->nNumNodes; i++)
    {
        poEalData->anRemotePlcaMap[i] = (uint8_t)(i + 1U); // Remote N will have PLCA ID = N + 1
    }

    /* Set the default number of nodes */
    poEalData->nNumDiscNodes = poE2bCoreCfg->nNumNodes;

    /* Initialise the mutex for this EAL instance */
    poEalData->oEalMutexCfg.pMem = &poEalData->anEalMutexBuffer[0U];
    poEalData->oEalMutexCfg.nMemSize = PRJ_OSAL_MUTEX_MEM_SIZE;
    (void)prj_osal_createMutex(&poEalData->oEalMutexCfg, &poEalData->hEalMutex);
}

/**
 * @brief       Terminates the EAL instance
 *
 * @param [in]  nInstNum        EAL instance number
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_CODE
void adi_eal_terminateInstance(uint32_t nInstNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use to terminate");

    /* Mutex cleanup */
    (void)prj_osal_destroyMutex(gaoEalData[nInstNum].hEalMutex);

    /* Terminate the e2bcore instance */
    adi_e2bcore_terminateStack(nInstNum);

    (void) ADI_MEMSET(&gaoEalData[nInstNum], 0, sizeof(EAL_DATA));
}

/**
 * @brief       Register callback for EAL top-level events
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  pfEalTopCbk     Pointer to the callback function
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_CODE
void adi_eal_registerCallback(uint32_t nInstNum, ADI_EAL_TOP_CALLBACK_FUNC pfEalTopCbk)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the function pointer */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(pfEalTopCbk != NULL, "Function pointer is NULL");

    gaoEalData[nInstNum].pfEalTopCbk = pfEalTopCbk;
}

/**
 * @brief       Register callback for EAL node events
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  pfEalNodeCbk     Pointer to the callback function
 * @note        This is a local operation that completes synchronously without network communication.
 */
ADI_EAL_CODE
void adi_eal_registerNodeCallback(uint32_t nInstNum, ADI_EAL_NODE_CALLBACK_FUNC pfEalNodeCbk)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the function pointer */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(pfEalNodeCbk != NULL, "Function pointer is NULL");

    gaoEalData[nInstNum].pfEalNodeCbk = pfEalNodeCbk;
}

/**
 * @brief       Enable / disable frame duplication
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  bEnableFrameDup Flag indicating if frames have to be duplicated to all opened network interfaces
 */
ADI_EAL_CODE
void adi_eal_setFrameDuplication(uint32_t nInstNum, bool bEnableFrameDup)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    gaoEalData[nInstNum].bEnableFrameDup = bEnableFrameDup;
}

/**
 * @brief       Sets the input enable for the SA_IF pins
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote node index (Set to #ADI_EAL_ALL_REMOTES for multicast operation)
 * @param [in]  nInputEnable    Set the input enable value for each SA_IF pin
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Input enable set successfully
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setInputEnable(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nInputEnable)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;

    if(nRemoteNum == ADI_EAL_ALL_REMOTES)
    {
        oNwFrameCfg.panPhysAddr = &ganAdiE2bMulticastAddr[0U];

        if((EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U])) ==
                VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U];
        }
    }
    else
    {
        oNwFrameCfg.panPhysAddr = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].anMacAddr[0U];

        if((EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag)) ==
                VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        }
    }

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */

    /* Enable the input pads for sampling MAC address from SA_IF pins */
    ganRegmapAddr[nInstNum][0U] = HOST_IF_SA_IF_BUS_CONFIGURATION_NW_ADDR;
    ganRegmapMask[nInstNum][0U] = BITM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE;
    ganRegmapData[nInstNum][0U] = (uint32_t)nInputEnable << BITP_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE;

    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_MASK_WRITE;
    aoRegMapCfg[0U].nLength = 1U;
    aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];
    aoRegMapCfg[0U].anMask  = &ganRegmapMask[nInstNum][0U];
    aoRegMapCfg[0U].pBuffer = &ganRegmapData[nInstNum][0U];

    /* Send the register map message */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief       Enable / Disable sampling MAC address from the SA_IF pins
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote node index (Set to #ADI_EAL_ALL_REMOTES for multicast operation)
 * @param [in]  bEnableSampling Set to true if MAC address LSBs have to be sampled from SA_IF pins
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Set / clear successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map write request
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setMacAddrSampling(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnableSampling)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;

    if(nRemoteNum == ADI_EAL_ALL_REMOTES)
    {
        oNwFrameCfg.panPhysAddr = &ganAdiE2bMulticastAddr[0U];

        if((EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U])) ==
                VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U];
        }
    }
    else
    {
        oNwFrameCfg.panPhysAddr = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].anMacAddr[0U];

        if(EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag) == VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        }
    }

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */

    /* Write to LCE_CFG register to enable */
    /* sampling of SA_IF pins for MAC address */
    ganRegmapAddr[nInstNum][0U] = LCE_CFG_NW_ADDR;
    ganRegmapMask[nInstNum][0U] = BITM_LCE_CFG_EXTERNAL_MAC_DEVICE_ADDRESS_LSB_SET;
    ganRegmapData[nInstNum][0U] = bEnableSampling ? BITM_LCE_CFG_EXTERNAL_MAC_DEVICE_ADDRESS_LSB_SET : 0U;

    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_MASK_WRITE;
    aoRegMapCfg[0U].nLength = 1U;
    aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];
    aoRegMapCfg[0U].anMask  = &ganRegmapMask[nInstNum][0U];
    aoRegMapCfg[0U].pBuffer = &ganRegmapData[nInstNum][0U];

    /* Create the frame and send it */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief       Get the PLCA ID set from the LUT
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote node index (Set to #ADI_EAL_ALL_REMOTES for multicast operation)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Sent the request successfully
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_getRemotePlcaLutId(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Read PHY_OA_PLCA_LUT_READBACK */
    uint16_t nAddr = PHY_OA_PLCA_LUT_DBG_NW_ADDR;

    /* Set the register configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_READ;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                        gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Always route it to go to T1S */
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* Send the register map message */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/**
 * @brief       Sets the PLCA ID of a remote node
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  panMacAddr      MAC address of the node
 * @param [in]  poPlcaIdCfg     Pointer to PLCA ID configuration
 * @param [in]  nVlanTag        Vlan tag to be sent, if not applicable set the parameter to zero
 *
 * @pre Unused fields in poPlcaIdCfg must be set to 0 to avoid writing junk data to PLCA registers
 *
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Successfully updated the PLCA of remote nodes
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map write request
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setRemotePlcaId(uint32_t nInstNum, uint8_t *panMacAddr, ADI_EAL_NODE_PLCA_ID_CFG *poPlcaIdCfg,
                                       uint32_t nVlanTag)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the pointer to arry of PLCA IDs */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(poPlcaIdCfg != NULL, "Pointer to PLCA ID configuration is NULL");

    uint16_t anAddr[5U];
    uint32_t anMask[5U];
    uint32_t anData[5U];

    /* Set the register map configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 5U;
    oRegCfg.anStartAddr = &anAddr[0U];
    oRegCfg.anMask = &anMask[0U];
    oRegCfg.pBuffer = &anData[0U];

    /* Set the address, mask and data for the PLCA IDs */
    anAddr[0U] = PHY_OA_PLCA_CTRL1_NW_ADDR;
    anMask[0U] = BITM_PHY_OA_PLCA_CTRL1_PLCALOCALID;
    anData[0U] = (uint32_t)poPlcaIdCfg->anPlcaID[0U] << BITP_PHY_OA_PLCA_CTRL1_PLCALOCALID;
    anAddr[1U] = PHY_OA_PLCA_CTRL2_NW_ADDR;
    anMask[1U] = BITM_PHY_OA_PLCA_CTRL2_PLCALOCALID1 | BITM_PHY_OA_PLCA_CTRL2_PLCALOCALID2;
    anData[1U] = ((uint32_t)poPlcaIdCfg->anPlcaID[1U] << BITP_PHY_OA_PLCA_CTRL2_PLCALOCALID1) |
                 ((uint32_t)poPlcaIdCfg->anPlcaID[2U] << BITP_PHY_OA_PLCA_CTRL2_PLCALOCALID2);
    anAddr[2U] = PHY_OA_PLCA_CTRL3_NW_ADDR;
    anMask[2U] = BITM_PHY_OA_PLCA_CTRL3_PLCALOCALID3 | BITM_PHY_OA_PLCA_CTRL3_PLCALOCALID4;
    anData[2U] = ((uint32_t)poPlcaIdCfg->anPlcaID[3U] << BITP_PHY_OA_PLCA_CTRL3_PLCALOCALID3) |
                 ((uint32_t)poPlcaIdCfg->anPlcaID[4U] << BITP_PHY_OA_PLCA_CTRL3_PLCALOCALID4);
    anAddr[3U] = PHY_OA_PLCA_CTRL4_NW_ADDR;
    anMask[3U] = BITM_PHY_OA_PLCA_CTRL4_PLCALOCALID5 | BITM_PHY_OA_PLCA_CTRL4_PLCALOCALID6;
    anData[3U] = ((uint32_t)poPlcaIdCfg->anPlcaID[5U] << BITP_PHY_OA_PLCA_CTRL4_PLCALOCALID5) |
                 ((uint32_t)poPlcaIdCfg->anPlcaID[6U] << BITP_PHY_OA_PLCA_CTRL4_PLCALOCALID6);
    anAddr[4U] = PHY_OA_PLCA_CTRL5_NW_ADDR;
    anMask[4U] = BITM_PHY_OA_PLCA_CTRL5_PLCALOCALID7 | BITM_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN;
    anData[4U] = ((uint32_t)poPlcaIdCfg->anPlcaID[7U] << BITP_PHY_OA_PLCA_CTRL5_PLCALOCALID7) |
                 ((uint32_t)poPlcaIdCfg->nMaskOfPlcaIdsUsed << BITP_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN);

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;
    oNwFrameCfg.panPhysAddr     = panMacAddr;

    /* Add VLAN info */
    if(EXTRACT_VLAN_TPID(nVlanTag) == VLAN_C_TAG_TPID)
    {
        oNwFrameCfg.nNumVlanTags     = 1U;
        oNwFrameCfg.panVlanTagArr    = &nVlanTag;
    }

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* Send the register map message */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/**
 * @brief       Sets the PLCA_EN bit to enable / disable PLCA
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  bPlcaEn         Enable / disable PLCA
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the PLCA_EN bit
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setPlcaEn(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaEn)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;

    if(nRemoteNum == ADI_EAL_ALL_REMOTES)
    {
        oNwFrameCfg.panPhysAddr = &ganAdiE2bMulticastAddr[0U];

        if((EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U])) ==
                VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U];
        }
    }
    else
    {
        oNwFrameCfg.panPhysAddr = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].anMacAddr[0U];

        if(EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag) == VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        }
    }

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */

    /* Set / clear PLCA_EN field */
    ganRegmapAddr[nInstNum][0U] = PHY_OA_PLCA_CTRL0_NW_ADDR;
    ganRegmapMask[nInstNum][0U] = BITM_PHY_OA_PLCA_CTRL0_PLCAEN;
    ganRegmapData[nInstNum][0U] = bPlcaEn ? BITM_PHY_OA_PLCA_CTRL0_PLCAEN : 0U;

    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_MASK_WRITE;
    aoRegMapCfg[0U].nLength = 1U;
    aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];
    aoRegMapCfg[0U].anMask  = &ganRegmapMask[nInstNum][0U];
    aoRegMapCfg[0U].pBuffer = &ganRegmapData[nInstNum][0U];

    /* Send the register map message */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief       Sets the PLCA Node count
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  nPlcaNodeCount  PLCA node count
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the PLCA node count
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setPlcaNodeCnt(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t nPlcaNodeCount)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set PLCA node count field */
    uint16_t nAddr = PHY_OA_PLCA_CTRL1_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_PLCA_CTRL1_PLCANODECOUNT;
    uint32_t nData = (uint32_t)nPlcaNodeCount << BITP_PHY_OA_PLCA_CTRL1_PLCANODECOUNT;

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Write the registers */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Sets the PLCA TO timer (in bit times)
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  nPlcaToTimer    PLCA TO timer
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the PLCA TO timer
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setPlcaToTimer(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t nPlcaToTimer)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set PLCA node count field */
    uint16_t nAddr = PHY_OA_PLCA_TOTMR_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_PLCA_TOTMR_PLCATOTIMER;
    uint32_t nData = (uint32_t)nPlcaToTimer << BITP_PHY_OA_PLCA_TOTMR_PLCATOTIMER;

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Write the registers */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Sets the PLCA burst mode
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  nPlcaBurstTimer PLCA burst timer (in bit times)
 * @param [in]  nPlcaMaxBurstCnt    PLCA burst count
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the PLCA burst mode
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setPlcaBurstMode(uint32_t nInstNum, uint32_t nRemoteNum,
                                        uint8_t nPlcaBurstTimer, uint8_t nPlcaMaxBurstCnt)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set PLCA node count field */
    uint16_t nAddr = PHY_OA_PLCA_BURST_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_PLCA_BURST_PLCABURSTTIMER | BITM_PHY_OA_PLCA_BURST_PLCAMAXBURSTCNT;
    uint32_t nData = ((uint32_t)nPlcaBurstTimer << BITP_PHY_OA_PLCA_BURST_PLCABURSTTIMER) |
                     ((uint32_t)nPlcaMaxBurstCnt << BITP_PHY_OA_PLCA_BURST_PLCAMAXBURSTCNT);

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Write the registers */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Sets the PLCA precedence mode
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  bPlcaPrecedenceModeEn   Enable / disable PLCA precedence mode
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the PLCA precedence mode
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setPlcaPrecedenceMode(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaPrecedenceModeEn)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set PLCA node count field */
    uint16_t nAddr = PHY_OA_PLCA_EXT_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_PLCA_EXT_PLCAPRECEN;
    uint32_t nData = bPlcaPrecedenceModeEn ? BITM_PHY_OA_PLCA_EXT_PLCAPRECEN : 0U;

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Write the registers */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Sets the PLCA leader mode enable bit
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  bPlcaLeaderModeEn   Enable / disable PLCA leader mode enable
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the PLCA leader mode enable bit
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setPlcaLeaderModeEn(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaLeaderModeEn)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set PLCA node count field */
    uint16_t nAddr = PHY_OA_PLCA_EXT_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_PLCA_EXT_PLCALEADEREN;
    uint32_t nData = bPlcaLeaderModeEn ? BITM_PHY_OA_PLCA_EXT_PLCALEADEREN : 0U;

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Write the registers */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Sets the node as PLCA leader
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  bPlcaLeader     Enable / disable the node as PLCA leader
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the node as PLCA leader
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setNodeAsPlcaLeader(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaLeader)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set PLCA node count field */
    uint16_t nAddr = PHY_OA_PLCA_EXT_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_PLCA_EXT_PLCALEADER;
    uint32_t nData = bPlcaLeader ? BITM_PHY_OA_PLCA_EXT_PLCALEADER : 0U;

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Write the registers */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Sets the PLCA_LUT_EN bit to enable / disable LUT based PLCA assignment
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  bPlcaLutEn      Enable / disable LUT based PLCA assignment
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the PLCA_LUT_EN bit
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setPlcaLutEn(uint32_t nInstNum, uint32_t nRemoteNum, bool bPlcaLutEn)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set / clear PLCA_LUT_EN field */
    uint16_t nAddr = PHY_OA_PLCA_LUT_CTRL_NW_ADDR;
    uint32_t nMask = BITM_PHY_OA_PLCA_LUT_CTRL_PIN_TO_PLCA_LUT_EN;
    uint32_t nData = bPlcaLutEn ? BITM_PHY_OA_PLCA_LUT_CTRL_PIN_TO_PLCA_LUT_EN : 0U;

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Write the registers */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Enables/disables gPTP initial rate correction block
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Remote number
 * @param [in]  bEnable         Enable / disable gPTP initial rate correction
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS:  Successfully set the gPTP initial rate calculation
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL: Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED: Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED: Failed to add the register map write message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setGptpInitialRateCorrection(uint32_t nInstNum, uint32_t nRemoteNum, bool bEnable)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set gPTP initial rate ratio calculation and phase according to the enable bit */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    uint16_t nStartAddr = IO_GPTP_CONFIG_NW_ADDR;
    uint32_t nMask = BITM_IO_GPTP_CONFIG_GPTP_RATE_RATIO_CALCULATION_ADJUST_PHASE |
                     BITM_IO_GPTP_CONFIG_GPTP_INITIAL_RATE_RATIO_CALCULATION;
    uint32_t nData = (((uint32_t)(bEnable ? 1U : 0U)) << BITP_IO_GPTP_CONFIG_GPTP_INITIAL_RATE_RATIO_CALCULATION) |
                     (((uint32_t)(bEnable ? 1U : 0U)) << BITP_IO_GPTP_CONFIG_GPTP_RATE_RATIO_CALCULATION_ADJUST_PHASE);
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Starts the E2B discovery sequence.
 *              adi_network_flushTxTimed and adi_eal_stopDiscoverySeq to be called (in this order) after this API to ensure
 *              completion of discovery sequence.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Discovery sequence started successfully
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map write request
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_startDiscoverySeq(uint32_t nInstNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Start the discovery sequence */
    adi_e2bcore_startDiscoverySeq(nInstNum);

    /* Disable all nodes */
    ADI_E2BCORE_CFG *poE2bCfg = gaoEalData[nInstNum].poE2bCoreCfg;

    for(uint32_t i = 0U; i < poE2bCfg->nNumNodes; i++)
    {
        poE2bCfg->aoRemoteCfgs[i].bEnabled = false;
    }

    /* Reset the PLCA mapping */
    (void) ADI_MEMSET(gaoEalData[nInstNum].anRemotePlcaMap, 0xFF, sizeof(gaoEalData[nInstNum].anRemotePlcaMap));

    /* Reset number of nodes discovered */
    gaoEalData[nInstNum].nNumDiscNodes = 0U;

    /* Read the MAC address registers */
    eEalStatus = DiscoverMacAddr(nInstNum);

    return eEalStatus;
}

/**
 * @brief       Ends the E2B discovery sequence. This API compulsorily needs to be called after the discovery sequence completes and remotes respond.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Discovery sequence stopped successfully
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_stopDiscoverySeq(uint32_t nInstNum)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    /* Stop the discovery sequence */
    adi_e2bcore_stopDiscoverySeq(nInstNum);

    return eEalStatus;
}

/**
 * @brief       Update the config structure from all remote nodes
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       Update sequence completed
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map read request
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_readCfgsFromRemote(uint32_t nInstNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Start the sequence to update config from remote */
    adi_e2bcore_startCfgUpdateSeq(nInstNum);

    do
    {
        /* Read all the mandatory registers for each remote node */
        eEalStatus = DiscoverCfgs(nInstNum);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Wait for updates to finish */
        adi_network_flushTxTimed(CFG_UPDATE_TIMEOUT_NS);
    } while(false);

    /* Stop the sequence that updates config from remote */
    adi_e2bcore_stopCfgUpdateSeq(nInstNum);

    return eEalStatus;
}

/**
 * @brief       Configure the E2B network, if not already configured
 *
 * @note        If the part is OTP programmed, this API call is optional. It can be used to
 *              configure the network with a new configuration other than what was programmed in OTP.
 *
 * @note        This API flushes the tx buffer internally after successful configuration.
 *              Unlike adi_eal_reconfigureNode, the caller does not need to call
 *              adi_network_flushTxBuffer after this API.
 *
 * @param [in]  nInstNum        EAL instance number
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS            Network configured successfully
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL  E2B core buffer unavailable for configuration packet
 *              - #ADI_EAL_STATUS_INVALID_PARAM      Invalid parameter passed to e2bcore
 *              - #ADI_EAL_STATUS_CFG_FAILED         Configuration failed due to an unknown error in e2bcore
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_configureNetwork(uint32_t nInstNum)
{
    ADI_E2BCORE_ERR eError;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;

    /* Pre-conditions:
     *  - Validate the instance number
     *  - Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Ask e2bcore to generate configuration frames */
    eError = adi_e2bcore_configureNetwork(nInstNum);

    switch(eError)
    {
        case ADI_E2BCORE_ERR_NONE:
        {
            /* Clear out the network layer buffers */
            adi_network_flushTxBuffer(gaoEalData[nInstNum].nDevNum, 0ULL);
            break;
        }

        case ADI_E2BCORE_ERR_BUF_INSUF:
        {
            eStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        case ADI_E2BCORE_ERR_PARAM:
        {
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        case ADI_E2BCORE_ERR_UNK:
        default:
        {
            eStatus = ADI_EAL_STATUS_CFG_FAILED;
            break;
        }
    }

    /* Permanently enable LCE topic override (topic_node=0) for all subsequent node register transactions */
    if(eStatus == ADI_EAL_STATUS_SUCCESS)
    {
        gaoEalData[nInstNum].bLCETopicConfigured = true;
    }
    else
    {
        gaoEalData[nInstNum].bLCETopicConfigured = false;
    }

    return eStatus;
}

/**
 * @brief       Reconfigure remote node
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Index of the remote
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS            Network configured successfully
 *              - #ADI_EAL_STATUS_E2BCORE_BUFF_FULL  E2B core buffer unavailable for configuration packet
 *              - #ADI_EAL_STATUS_INVALID_PARAM      Invalid parameter passed to e2bcore
 *              - #ADI_EAL_STATUS_CFG_FAILED         Configuration failed due to an unknown error in e2bcore
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_reconfigureNode(uint32_t nInstNum, uint32_t nRemoteNum)
{
    ADI_E2BCORE_ERR eError;
    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_SUCCESS;

    /* Pre-conditions:
     *  - Validate the instance number
     *  - Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Ask e2bcore to generate node configuration frames */
    eError = adi_e2bcore_reconfigureNode(nInstNum, nRemoteNum);

    switch(eError)
    {
        case ADI_E2BCORE_ERR_NONE:
        {
            /* Nothing to do */
            break;
        }

        case ADI_E2BCORE_ERR_BUF_INSUF:
        {
            eStatus = ADI_EAL_STATUS_E2BCORE_BUFF_FULL;
            break;
        }

        case ADI_E2BCORE_ERR_PARAM:
        {
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        case ADI_E2BCORE_ERR_UNK:
        default:
        {
            eStatus = ADI_EAL_STATUS_CFG_FAILED;
            break;
        }
    }

    return eStatus;
}

/**
 * @brief       Write the MAC address to the remote node
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Index of the remote for which to update the MAC address
 * @param [in]  panMacAddr      MAC address to write to the remote node
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       MAC address update for the remote
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map write request
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_writeMacAddr(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t *panMacAddr)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the remote number */
    /*  * Validate the MAC address */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");
    ADI_DBG_REQUIRE(panMacAddr != NULL, "MAC address is NULL");

    /* ~~~~~ Local variables ~~~~~ */

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));

    /* Write to LCE_CFG register to enable */
    /* sampling of SA_IF pins for MAC address */
    uint32_t nMacAddrUpper = ((uint32_t)panMacAddr[0U] << 8U) | (uint32_t)panMacAddr[1U];
    uint32_t nMacAddrLower = ((uint32_t)panMacAddr[2U] << 24U) |
                             ((uint32_t)panMacAddr[3U] << 16U) |
                             ((uint32_t)panMacAddr[4U] << 8U) |
                             (uint32_t)panMacAddr[5U];

    ganRegmapAddr[nInstNum][0U] = LCE_DEVICE_MAC_ADDRESS_LOWER_NW_ADDR;
    ganRegmapMask[nInstNum][0U] = LCE_DEVICE_MAC_ADDRESS_LOWER_VOL_MASK;
    ganRegmapData[nInstNum][0U] = nMacAddrLower;
    ganRegmapMask[nInstNum][1U] = BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER;
    ganRegmapData[nInstNum][1U] = nMacAddrUpper;
    aoRegMapCfg[0U].eType       = ADI_E2BCORE_BURST_MASK_WRITE;
    aoRegMapCfg[0U].nLength     = 2U;
    aoRegMapCfg[0U].anMask      = &ganRegmapMask[nInstNum][0U];
    aoRegMapCfg[0U].anStartAddr = &ganRegmapAddr[nInstNum][0U];
    aoRegMapCfg[0U].pBuffer     = &ganRegmapData[nInstNum][0U];

    /* Increment the number of nodes discovered */
    gaoEalData[nInstNum].nNumDiscNodes++;

    /* Enable the node in the config structure */
    gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].bEnabled = true;

    /* Update the MAC address of the remote number in the config structure */
    adi_e2bcore_updateMacAddr(nInstNum, nRemoteNum, panMacAddr);

    /* Create frame to write to the remote node with the mac address */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief       Update the MAC address in internal structures only (without writing to remote node)
 *
 * @note        This API only updates the internal EAL and E2BCore data structures.
 *              It does not send any messages to the remote node, unlike adi_eal_writeMacAddr.
 *
 * @param [in]  nInstNum        Instance number of EAL
 * @param [in]  nRemoteNum      Index of the remote for which to update the MAC address
 * @param [in]  panMacAddr      MAC address to update in internal structures
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       MAC address updated successfully in internal structures
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_updateMacAddr(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t *panMacAddr)
{
    /* Pre-conditions:
     * Validate the instance number
     * Validate the instance is in-use
     * Validate the remote number
     * Validate the MAC address */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");
    ADI_DBG_REQUIRE(panMacAddr != NULL, "MAC address is NULL");

    /* Increment the number of nodes discovered */
    gaoEalData[nInstNum].nNumDiscNodes++;

    /* Enable the node in the config structure */
    gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].bEnabled = true;

    /* Update the MAC address of the remote number in the config structure */
    adi_e2bcore_updateMacAddr(nInstNum, nRemoteNum, panMacAddr);

    return ADI_EAL_STATUS_SUCCESS;
}

/**
 * @brief       Reset HV die of all remote nodes
 *              adi_network_flushTxTimed to be called after this API to ensure
 *              all messages are sent/received
 * @param [in]  nInstNum        EAL instance number
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_resetRemoteHvDie(uint32_t nInstNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    const uint8_t *panMacAddr = &ganAdiE2bMulticastAddr[0u];

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;
    oNwFrameCfg.panPhysAddr     = panMacAddr;

    if((EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U])) ==
            VLAN_C_TAG_TPID)
    {
        oNwFrameCfg.nNumVlanTags    = 1U;
        oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U];
    }

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    /* Write to HV IO reset control register */
    uint16_t anRegAddr[1U] = { (uint16_t)IO_HV_RESET_CONTROL_NW_ADDR };
    uint32_t anRegData[1U] = { 0xA55AU << BITP_IO_HV_RESET_CONTROL_RESET };

    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_WRITE;
    aoRegMapCfg[0U].nLength = 1U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];
    aoRegMapCfg[0U].pBuffer = &anRegData[0U];

    /* Create the frame and send it */
    eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);

    return eEalStatus;
}


/**
 * @brief       Reset LV die of all remote nodes
 *              adi_network_flushTxTimed to be called after this API to ensure
 *              all messages are sent/received
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index / (Set #ADI_EAL_ALL_REMOTES to Reset LV die for all remote nodes)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_resetRemoteLvDie(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;

    if(nRemoteNum == ADI_EAL_ALL_REMOTES)
    {
        oNwFrameCfg.panPhysAddr = &ganAdiE2bMulticastAddr[0U];

        if((EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U])) ==
                VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U];
        }
    }
    else
    {
        /*  * Validate the remote number */
        ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");

        oNwFrameCfg.panPhysAddr = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].anMacAddr[0U];

        if(EXTRACT_VLAN_TPID(gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag) == VLAN_C_TAG_TPID)
        {
            oNwFrameCfg.nNumVlanTags    = 1U;
            oNwFrameCfg.panVlanTagArr   = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        }
    }

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    /* Write to LV IO reset control register */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    uint16_t anRegAddr[1U] = { (uint16_t)IO_RESET_CONTROL_NW_ADDR };
    uint32_t anRegData[1U] = { 0xA55AU << BITP_IO_RESET_CONTROL_RESET };

    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_WRITE;
    aoRegMapCfg[0U].nLength = 1U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];
    aoRegMapCfg[0U].pBuffer = &anRegData[0U];

    /* Create the frame and send it */
    eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);

    return eEalStatus;
}

/**
 * @brief       Enable GPTP timer in all remote nodes
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index (Set to #ADI_EAL_ALL_REMOTES for multicast operation)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_enableGptpTimerRemote(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                        gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    /* Write to MAC TS_CFG register */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    uint16_t anRegAddr[2U] = { (uint16_t)MAC_TS_CFG_NW_ADDR, (uint16_t)MAC_CONFIG0_NW_ADDR };
    uint32_t anRegMask[2U] = { BITM_MAC_TS_CFG_TS_EN, BITM_MAC_CONFIG0_FTSS | BITM_MAC_CONFIG0_FTSE };
    uint32_t anRegData[2U] = { BITM_MAC_TS_CFG_TS_EN, BITM_MAC_CONFIG0_FTSS | BITM_MAC_CONFIG0_FTSE };

    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_MASK_WRITE;
    aoRegMapCfg[0U].nLength = 2U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];
    aoRegMapCfg[0U].anMask  = &anRegMask[0U];
    aoRegMapCfg[0U].pBuffer = &anRegData[0U];

    /* Create the frame and send it */
    eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        adi_network_flushTxBuffer(nDevNum, 0ULL);
    }

    return eEalStatus;
}

/**
 * @brief       Check PHY related registers in remote programmed via OTP
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Index of the remote Note: If need to write to all remote nodes use #ADI_EAL_ALL_REMOTES
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_checkRemoteOtpValues(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                        gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    /* Read remote OTP values */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    uint16_t anRegAddr[4U] = { (uint16_t)0xB111u, (uint16_t)0xB217u, (uint16_t)0xC410u, (uint16_t)0xc484u };

    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_READ;
    aoRegMapCfg[0U].nLength = 4U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    /* Create the frame and send it */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief       Ping remote node(s)
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index (Set to #ADI_EAL_ALL_REMOTES for multicast ping request)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_pingRemote(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                        gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    /* Read the MAC address registers */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    uint16_t anRegAddr[1U] = { (uint16_t)MAC_PHYID_NW_ADDR };
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = 1U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    /* Create the frame and send it */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief       Get revision number
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index (Set to #ADI_EAL_ALL_REMOTES for getting revision number of all nodes)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_getRevision(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                        gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    /* Read the MAC address registers */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    uint16_t anRegAddr[1U] = { (uint16_t)MAC_SCRATCH_0_NW_ADDR };
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_BURST_READ;
    aoRegMapCfg[0U].nLength = 2U;
    aoRegMapCfg[0U].anStartAddr = &anRegAddr[0U];

    /* Create the frame and send it */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief       Read from remote register(s)
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @param [inout] poRegCfg      Register read details (This will be updated with
 *                              the register read data on return of this function)
 * @param [in]  nTimeout        Timeout to wait for reponse (in nanoseconds)
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_readRegWithTimeout(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, uint32_t nTimeout)
{
    /* ~~~~~ Local variables ~~~~~ */
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;
    uint8_t *pBuffer = NULL;
    uint32_t nActLength = 0U;
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));

    ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");

    /* ~~~~~ Network frame configurations ~~~~~ */
    nVlanTag = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* Create the frame and send it */
    do
    {
        /* Acquire EAL mutex */
        if(prj_osal_lockMutex(gaoEalData[nInstNum].hEalMutex) != PRJ_OSAL_STATUS_SUCCESS)
        {
            eEalStatus = ADI_EAL_STATUS_MUTEX_FAILURE;
            break;
        }

        /* Set the waiting for Rx flag to true */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bWaitingForResp = true;

        /* Save the register config pointer */
        gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].poRegCfg = poRegCfg;

        /* Get Buffer from network layer */
        if(adi_network_getTxBuffer(nDevNum, &pBuffer, &oNwFrameCfg) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Start creating e2b packet */
        if(adi_e2bcore_startPacket(nInstNum, oNwFrameCfg.panPhysAddr, pBuffer, oNwFrameCfg.nLenByte,
                                   false) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* Add the register message */
        if(adi_e2bcore_addRegmapIO(nInstNum, &oMsgOpts, poRegCfg, 1U) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }

        /* Finished with e2b messages */
        adi_e2bcore_finishPacket(nInstNum, &nActLength,
                                 oNwFrameCfg.bLenInclMacHdr, oNwFrameCfg.nNumVlanTags);

        /* Update the actual length of the buffer used */
        oNwFrameCfg.nLenByte = (uint16_t)nActLength;

        /* Ask network layer to transmit the frame */
        adi_network_transmit(nDevNum, &oNwFrameCfg);

        /* Wait for the reg read response */
        eEalStatus = eal_utils_waitForTimeout(&gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bWaitingForResp, nTimeout);
    } while(false);

    /* Clear the waiting for Rx flag */
    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bWaitingForResp = false;

    /* Incase of any failure in the above sequence, the network mutex must be released */
    if((eEalStatus == ADI_EAL_STATUS_START_FAILED) || (eEalStatus == ADI_EAL_STATUS_REGMAPIO_FAILED))
    {
        (void)adi_network_releaseMutex();
    }

    /* Release EAL mutex */
    if(prj_osal_unlockMutex(gaoEalData[nInstNum].hEalMutex) != PRJ_OSAL_STATUS_SUCCESS)
    {
        eEalStatus = ADI_EAL_STATUS_MUTEX_FAILURE;
    }

    return eEalStatus;
}

/**
 * @brief       Write/Read remote register(s)
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @param [in]  poRegCfg        Register write details
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS       API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED  Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_sendRegTransaction(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the topic */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                        gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* Create the frame and send it */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, poRegCfg);
}

/**
 * @brief Send a register transaction with topic
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum    EAL instance number
 * @param [in] nRemoteNum  Remote node index
 * @param [in] poRegCfg    Pointer to regmap_io config
 * @param [in] nTopic      Destination topic of the regmap message
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_sendRegTransactionWithTopic(uint32_t nInstNum, uint32_t nRemoteNum,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, uint8_t nTopic)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the topic */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(nTopic < 128U, "Invalid topic!");

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                        gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                        gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = (uint32_t)nTopic;

    /* Create the frame and send it */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, poRegCfg);
}

/**
 * @brief Performs basic configurations for the controller's LCE - Routing regmap responses, acknowledgements, interrupts, FIFO rejects to host and custom MAC address
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in]  nInstNum        EAL instance number
 * @param [in]  nRemoteNum      Remote node index
 * @return      Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setupDualMode(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the remote number */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");

    /* Get the default address from regmap */
    uint8_t nDefaultLCEMacAddr[6u];
    nDefaultLCEMacAddr[0U] = (uint8_t)((LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0xFF00ULL) >> 8U);
    nDefaultLCEMacAddr[1U] = (uint8_t)(LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0x00FFULL);
    nDefaultLCEMacAddr[2U] = (uint8_t)((LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0xFF000000ULL) >> 24U);
    nDefaultLCEMacAddr[3U] = (uint8_t)((LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0x00FF0000ULL) >> 16U);
    nDefaultLCEMacAddr[4U] = (uint8_t)((LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0x0000FF00ULL) >> 8U);
    nDefaultLCEMacAddr[5U] = (uint8_t)((LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL & 0x000000FFULL));

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bDuplicate      = gaoEalData[nInstNum].bEnableFrameDup;
    oNwFrameCfg.bLenInclMacHdr  = false;
    oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT1_HP;
    oNwFrameCfg.eTsReg          = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType      = ADI_E2BCORE_ETYPE;
    oNwFrameCfg.nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;
    oNwFrameCfg.panPhysAddr     = &nDefaultLCEMacAddr[0];

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    /* ~~~~~ Register map message configuration ~~~~~ */
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegMapCfg[1U];
    (void) ADI_MEMSET(&aoRegMapCfg[0U], 0, sizeof(aoRegMapCfg));
    uint8_t *panMacAddr = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].anMacAddr[0u];

    /* Write to MAC address registers */
    uint32_t nMacAddrUpper = (LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL &
                              ~(BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER)) |
                             ((uint32_t)panMacAddr[0U] << 8U) |
                             (uint32_t)panMacAddr[1U];
    uint32_t nMacAddrLower = ((uint32_t)panMacAddr[2U] << 24U) |
                             ((uint32_t)panMacAddr[3U] << 16U) |
                             ((uint32_t)panMacAddr[4U] << 8U) |
                             (uint32_t)panMacAddr[5U];
    /* Write to the routing registers */
    uint32_t nPortDestination = ((uint32_t)ENUM_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG_OASPI <<
                                 BITP_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG)
                                | ((uint32_t)ENUM_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG_OASPI << BITP_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG) |
                                ((uint32_t)ENUM_LCE_CFG_PORT_TARGET_INTERRUPT_CFG_OASPI << BITP_LCE_CFG_PORT_TARGET_INTERRUPT_CFG) |
                                ((uint32_t)ENUM_LCE_CFG_PORT_TARGET_REGMAP_CFG_OASPI << BITP_LCE_CFG_PORT_TARGET_REGMAP_CFG);

    uint32_t nPortDestinationMask = BITM_LCE_CFG_PORT_TARGET_ACKNOWLEDGE_CFG | BITM_LCE_CFG_PORT_TARGET_FIFO_REJECTS_CFG |
                                    BITM_LCE_CFG_PORT_TARGET_INTERRUPT_CFG | BITM_LCE_CFG_PORT_TARGET_REGMAP_CFG;

    uint16_t anRegmapAddr[3U] = {LCE_DEVICE_MAC_ADDRESS_LOWER_NW_ADDR, LCE_DEVICE_MULTICAST_ADDRESS_UPPER_NW_ADDR, LCE_CFG_NW_ADDR};
    uint32_t anRegmapData[3U] = {nMacAddrLower, nMacAddrUpper, nPortDestination};
    uint32_t anRegmapMask[3U] = {BITM_LCE_DEVICE_MAC_ADDRESS_LOWER_DEVICE_MAC_ADDRESS_LOWER, BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER, nPortDestinationMask};
    aoRegMapCfg[0U].eType   = ADI_E2BCORE_RND_MASK_WRITE;
    aoRegMapCfg[0U].nLength = 3U;
    aoRegMapCfg[0U].anStartAddr = &anRegmapAddr[0U];
    aoRegMapCfg[0U].anMask = &anRegmapMask[0U];
    aoRegMapCfg[0U].pBuffer = &anRegmapData[0U];

    /* Update the MAC address of the remote number in the config structure */
    adi_e2bcore_updateMacAddr(nInstNum, nRemoteNum, panMacAddr);

    /* Create frame to write to the remote node with the mac address */
    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &aoRegMapCfg[0U]);
}

/**
 * @brief Set VLAN tag associated with downstream unicast node level register access
 *
 * @param [in] nInstNum    EAL instance number
 * @param [in] nRemoteNum  Remote node index
 * @param [in] poVlanTag   Pointer to VLAN tag for the node register access
 */
ADI_EAL_CODE
void adi_eal_setNodeLvlRegVlan(uint32_t nInstNum, uint32_t nRemoteNum, ADI_VLAN_CFG *poVlanTag)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the remote number */
    /*  * Validate the VLAN tag is not NULL */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");
    ADI_DBG_REQUIRE(poVlanTag != NULL, "Pointer to VLAN tag is NULL");

    gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag = eal_utils_convertVlanTags(poVlanTag);
}

/**
 * @brief Set VLAN tag associated with downstream mulicast node level register access topic
 *
 * @param [in] nInstNum     EAL instance number
 * @param [in] nMcastTopic  Topic used for multicast node level register access
 * @param [in] poVlanTag    Pointer to VLAN tag for the multicast topic
 */
ADI_EAL_CODE
void adi_eal_setMcastNodeLvlRegVlan(uint32_t nInstNum, uint8_t nMcastTopic, ADI_VLAN_CFG *poVlanTag)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the Mcast topic is not using unicast topics / out of range values */
    /*  * Validate the VLAN tag is not NULL */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(nMcastTopic > 8U && nMcastTopic < 128U, "Multicast topics cannot be less than 8 or greater than 127");
    ADI_DBG_REQUIRE(poVlanTag != NULL, "Pointer to VLAN tag is NULL");

    gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[nMcastTopic] = eal_utils_convertVlanTags(poVlanTag);
}

/**
 * @brief Configures the upstream VLAN for register_readbacks,interrupt,acknowledgement,fifo_reject messages for remote nodes
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum           EAL instance number
 * @param [in] nRemoteNum         Remote node index
 * @param [in] poUpstreamVlanTags Pointer to VLAN tag information for upstream messages
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setUpstreamVlan(uint32_t nInstNum, uint32_t nRemoteNum,
                                       ADI_EAL_UPSTREAM_MSG_VLAN_TAG *poUpstreamVlanTags)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    uint16_t nStartAddr = LCE_ACKNOWLEDGE_CTAG_CONFIG_NW_ADDR;
    uint32_t nRegData[4U];

    /* Fill the upstream VLAN information for acknowledgement message */
    nRegData[0U] = (poUpstreamVlanTags->bAckVlanEn ? ((uint32_t)poUpstreamVlanTags->oAckVlanTag.nVlanId &
                    BITM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_ID) : 0U) |
                   (poUpstreamVlanTags->oAckVlanTag.bDEI ? (uint32_t)BITM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_DROP_ELIGIBLE : 0U) |
                   (((uint32_t)poUpstreamVlanTags->oAckVlanTag.nPCP & 0x7U) << BITP_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_CTAG_PRIORITY) |
                   (poUpstreamVlanTags->bAckVlanEn ? BITM_LCE_ACKNOWLEDGE_CTAG_CONFIG_ACK_TRANSMIT_CTAG : 0U);

    /* Fill the upstream VLAN information for FIFO reject message */
    nRegData[1U] = (poUpstreamVlanTags->bFifoRejVlanEn ? ((uint32_t)poUpstreamVlanTags->oFifoRejVlanTag.nVlanId &
                    BITM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_ID) : 0U) |
                   (poUpstreamVlanTags->oFifoRejVlanTag.bDEI ? (uint32_t)BITM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_DROP_ELIGIBLE :
                    0U) |
                   (((uint32_t)poUpstreamVlanTags->oFifoRejVlanTag.nPCP & 0x7U) <<
                    BITP_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_CTAG_PRIORITY) |
                   (poUpstreamVlanTags->bFifoRejVlanEn ? BITM_LCE_FIFO_REJECT_CTAG_CONFIG_FIFO_REJECT_TRANSMIT_CTAG : 0U);

    /* Fill the upstream VLAN information for register readbacks message */
    nRegData[2U] = (poUpstreamVlanTags->bRegRbVlanEn ? ((uint32_t)poUpstreamVlanTags->oRegRbVlanTag.nVlanId &
                    BITM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_ID) : 0U) |
                   (poUpstreamVlanTags->oRegRbVlanTag.bDEI ? (uint32_t)BITM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_DROP_ELIGIBLE :
                    0U) |
                   (((uint32_t)poUpstreamVlanTags->oRegRbVlanTag.nPCP & 0x7U) <<
                    BITP_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_CTAG_PRIORITY) |
                   (poUpstreamVlanTags->bRegRbVlanEn ? BITM_LCE_REGISTERMAP_READ_CTAG_CONFIG_REGMAP_RB_TRANSMIT_CTAG : 0U);

    /* Fill the upstream VLAN information for interrupt message */
    nRegData[3U] = (poUpstreamVlanTags->bIrqVlanEn ? ((uint32_t)poUpstreamVlanTags->oIrqVlanTag.nVlanId &
                    BITM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_ID) : 0U) |
                   (poUpstreamVlanTags->oIrqVlanTag.bDEI ? (uint32_t)BITM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_DROP_ELIGIBLE : 0U) |
                   (((uint32_t)poUpstreamVlanTags->oIrqVlanTag.nPCP & 0x7U) << BITP_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_CTAG_PRIORITY) |
                   (poUpstreamVlanTags->bIrqVlanEn ? BITM_LCE_INTERURPT_CTAG_CONFIG_INTERRUPT_TRANSMIT_CTAG : 0U);

    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 4U;
    oRegCfg.eType = ADI_E2BCORE_BURST_WRITE;
    oRegCfg.pBuffer = &nRegData[0U];

    /* Send the register write */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief       Get the per-entry forwarding bits for the MAC address filter upper register
 *
 * @param [in]  eFwdDest    Forwarding destination enum
 * @return      Bitmask with TO_HOST and/or TO_LCE bits set.
 *              Returns 0 when eFwdDest is ADI_EAL_MAC_FWD_NONE (no forwarding).
 */
ADI_EAL_CODE
static uint32_t GetMacFilterEntryFwdBits(ADI_EAL_MAC_FWD_DEST eFwdDest)
{
    uint32_t nFwdBits = 0U;

    /* Map the forwarding destination enum to per-entry TO_HOST / TO_LCE bits */
    switch(eFwdDest)
    {
        case ADI_EAL_MAC_FWD_HOST:
            /* Route matched frames to HOST port */
            nFwdBits = BITM_MAC_ADDR_FILT_UPR_1_TO_HOST;
            break;

        case ADI_EAL_MAC_FWD_LCE:
            /* Route matched frames to LCE port */
            nFwdBits = BITM_MAC_ADDR_FILT_UPR_1_TO_LCE;
            break;

        case ADI_EAL_MAC_FWD_BOTH_HOST_LCE:
            /* Route matched frames to both HOST and LCE ports */
            nFwdBits = BITM_MAC_ADDR_FILT_UPR_1_TO_HOST | BITM_MAC_ADDR_FILT_UPR_1_TO_LCE;
            break;

        default:
            /* ADI_EAL_MAC_FWD_NONE or unknown: no forwarding bits set, frames are dropped */
            break;
    }

    return nFwdBits;
}

/**
 * @brief Setup the MAC filter table upto a maximum of 15 entries as entry 0 might be programmed for PTP filtering in OTP
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum     EAL instance number
 * @param [in] nRemoteNum   Remote node index
 * @param [in] anMacAddr    Array of the MAC addresses to be entered in the filter table
 * @param [in] nNumAddr     Number of MAC addresses to be entered in the filter table
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setMacFilterTable(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t anMacAddr[][6u],
        uint8_t nNumAddr)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the MAC address filter table entries */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE((nNumAddr != 0U
                     && nNumAddr <= MAC_ADDR_MAX_ENTRIES), "Number of MAC addresses are greater than max entries available");

    ADI_EAL_STATUS eEalStatus;

    do
    {
        /* ~~~~~ Network frame configurations ~~~~~ */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                            gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                            gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* ~~~~~ e2bcore message options ~~~~~ */
        ADI_E2BCORE_MSGOPTS oMsgOpts;
        (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
        oMsgOpts.bMailbox = false;
        oMsgOpts.bSync    = true;
        adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

        /* ~~~~~ Register map message configuration ~~~~~ */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegMapCfg;
        (void) ADI_MEMSET(&oRegMapCfg, 0, sizeof(oRegMapCfg));

        uint32_t anRegmapData[MAC_ADDR_MAX_ENTRIES * 2U];
        uint16_t nStartAddr = MAC_ADDR_FILT_UPR_1_NW_ADDR;

        /* Note: We are starting from filter table entry 1 below since entry 0 might be programmed for PTP filtering in OTP */
        uint8_t nRegIdx = 0U;
        oRegMapCfg.anStartAddr = &nStartAddr;
        oRegMapCfg.nLength = nNumAddr * 2U;
        oRegMapCfg.eType = ADI_E2BCORE_BURST_WRITE;

        /* Set the filter table entries */
        for(uint8_t i = 0U; i < nNumAddr; i++)
        {
            uint16_t nMacAddrUpper = ((uint16_t)anMacAddr[i][0u] << 8u) | (uint16_t)anMacAddr[i][1u];
            uint32_t nMacAddrLower = ((uint32_t)anMacAddr[i][2u] << 24u) | ((uint32_t)anMacAddr[i][3u] << 16u) |
                                     ((uint32_t)anMacAddr[i][4u] << 8u) | (uint32_t)anMacAddr[i][5u];
            anRegmapData[nRegIdx++] = ((uint32_t)nMacAddrUpper << BITP_MAC_ADDR_FILT_UPR_1_MAC_ADDR) |
                                      BITM_MAC_ADDR_FILT_UPR_1_TO_LCE | /* Forward frames to LCE matching with this MAC address */
                                      BITM_MAC_ADDR_FILT_UPR_1_APPLY2PORT; /* Enable the rule to apply for LCE */
            anRegmapData[nRegIdx++] = nMacAddrLower;
        }

        oRegMapCfg.pBuffer = &anRegmapData[0U];
        eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegMapCfg);

    } while(false);

    return eEalStatus;
}

/**
 * @brief Clear the MAC filter table
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum     EAL instance number
 * @param [in] nRemoteNum   Remote node index
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_clearMacFilterTable(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the MAC address filter table entries */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;

    do
    {
        /* ~~~~~ Network frame configurations ~~~~~ */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                            gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                            gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* ~~~~~ e2bcore message options ~~~~~ */
        ADI_E2BCORE_MSGOPTS oMsgOpts;
        (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
        oMsgOpts.bMailbox = false;
        oMsgOpts.bSync    = true;
        adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

        /* Phase 1: Clear all MAC address filter entries (entries 1 to MAC_ADDR_MAX_ENTRIES) */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegMapCfg;
        uint32_t anRegmapData[MAC_ADDR_MAX_ENTRIES * 2U] = {0};
        uint16_t nStartAddr = MAC_ADDR_FILT_UPR_1_NW_ADDR;

        (void) ADI_MEMSET(&oRegMapCfg, 0, sizeof(oRegMapCfg));
        oRegMapCfg.anStartAddr = &nStartAddr;
        oRegMapCfg.nLength = (uint8_t)(MAC_ADDR_MAX_ENTRIES * 2U);
        oRegMapCfg.eType = ADI_E2BCORE_BURST_WRITE;
        oRegMapCfg.pBuffer = &anRegmapData[0U];

        eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegMapCfg);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Reset network frame config for phase 2 */
        (void) ADI_MEMSET(&oNwFrameCfg, 0, sizeof(oNwFrameCfg));
        eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* Phase 2: Restore default behavior - forward unknown traffic to LCE */
        uint32_t nMask = BITM_MAC_CONFIG2_FWD_UNK2LCE;
        uint32_t nRegData = BITM_MAC_CONFIG2_FWD_UNK2LCE;
        nStartAddr = MAC_CONFIG2_NW_ADDR;

        (void) ADI_MEMSET(&oRegMapCfg, 0, sizeof(oRegMapCfg));
        oRegMapCfg.anStartAddr = &nStartAddr;
        oRegMapCfg.anMask = &nMask;
        oRegMapCfg.nLength = 1U;
        oRegMapCfg.pBuffer = &nRegData;
        oRegMapCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

        eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegMapCfg);

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Setup the MAC filter table with per-entry configurable forwarding destination, up to a maximum
 *              of 15 entries as entry 0 might be programmed for PTP filtering in OTP
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum     EAL instance number
 * @param [in] nRemoteNum   Remote node index
 * @param [in] anMacAddr    Array of the MAC addresses to be entered in the filter table
 * @param [in] nNumAddr     Number of MAC addresses to be entered in the filter table
 * @param [in] aeFwdDest    Array of forwarding destinations, one per MAC address entry
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_INVALID_PARAM     Invalid parameter (instance number or address count out of range)
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setMacFilterFwdDestinationTable(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t anMacAddr[][6u],
        uint8_t nNumAddr, ADI_EAL_MAC_FWD_DEST aeFwdDest[])
{
    /* Pre-conditions */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE((nNumAddr != 0U) && (nNumAddr <= MAC_ADDR_MAX_ENTRIES),
                    "Number of MAC addresses are greater than max entries available");
    ADI_DBG_REQUIRE(aeFwdDest != NULL, "Forwarding destination array is NULL");

    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;

    do
    {
        /* Runtime bounds check - guard against release builds where ADI_DBG_REQUIRE may be disabled */
        if(nInstNum >= ADI_EAL_MAX_NUM_INSTANCES)
        {
            eEalStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        if((nNumAddr == 0U) || (nNumAddr > MAC_ADDR_MAX_ENTRIES))
        {
            eEalStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        /* ~~~~~ Network frame configurations ~~~~~ */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                            gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                            gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* ~~~~~ e2bcore message options ~~~~~ */
        ADI_E2BCORE_MSGOPTS oMsgOpts;
        (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
        oMsgOpts.bMailbox = false;
        oMsgOpts.bSync    = true;
        adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

        /* Write MAC address filter table entries with per-entry forwarding destination */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegMapCfg;
        uint32_t anRegmapData[MAC_ADDR_MAX_ENTRIES * 2U];
        uint8_t nRegIdx = 0U;

        for(uint8_t nIdx = 0U; nIdx < nNumAddr; nIdx++)
        {
            uint32_t nFwdBits = GetMacFilterEntryFwdBits(aeFwdDest[nIdx]);
            uint16_t nMacAddrUpper = ((uint16_t)anMacAddr[nIdx][0u] << 8u) | (uint16_t)anMacAddr[nIdx][1u];
            uint32_t nMacAddrLower = ((uint32_t)anMacAddr[nIdx][2u] << 24u) | ((uint32_t)anMacAddr[nIdx][3u] << 16u) |
                                     ((uint32_t)anMacAddr[nIdx][4u] << 8u) | (uint32_t)anMacAddr[nIdx][5u];

            anRegmapData[nRegIdx++] = ((uint32_t)nMacAddrUpper << BITP_MAC_ADDR_FILT_UPR_1_MAC_ADDR) |
                                      nFwdBits |
                                      BITM_MAC_ADDR_FILT_UPR_1_APPLY2PORT;
            anRegmapData[nRegIdx++] = nMacAddrLower;
        }

        /* Note: Starting from filter table entry 1 since entry 0 might be programmed for PTP filtering in OTP */
        uint16_t nStartAddr = MAC_ADDR_FILT_UPR_1_NW_ADDR;
        (void)ADI_MEMSET(&oRegMapCfg, 0, sizeof(oRegMapCfg));
        oRegMapCfg.anStartAddr = &nStartAddr;
        oRegMapCfg.nLength = (uint8_t)(nNumAddr * 2U); /* Max: MAC_ADDR_MAX_ENTRIES * 2 = 30, fits uint8_t */
        oRegMapCfg.eType = ADI_E2BCORE_BURST_WRITE;
        oRegMapCfg.pBuffer = &anRegmapData[0U];

        eEalStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegMapCfg);

    } while(false);

    return eEalStatus;
}

/**
 * @brief       Clear the MAC filter table and restore default unknown traffic forwarding to LCE
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum     EAL instance number
 * @param [in] nRemoteNum   Remote node index
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_INVALID_PARAM     Invalid parameter (instance number out of range)
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_clearMacFilterFwdDestinationTable(uint32_t nInstNum, uint32_t nRemoteNum)
{
    /* Pre-conditions */
    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");

    ADI_EAL_STATUS eStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;

    do
    {
        /* Runtime bounds check - guard against release builds where ADI_DBG_REQUIRE may be disabled */
        if(nInstNum >= ADI_EAL_MAX_NUM_INSTANCES)
        {
            eStatus = ADI_EAL_STATUS_INVALID_PARAM;
            break;
        }

        ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

        /* ~~~~~ Network frame configurations ~~~~~ */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag = nRemoteNum == ADI_EAL_ALL_REMOTES ?
                            gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U] :
                            gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].nNodeRegVlanTag;
        eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* ~~~~~ e2bcore message options ~~~~~ */
        ADI_E2BCORE_MSGOPTS oMsgOpts;
        (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
        oMsgOpts.bMailbox = false;
        oMsgOpts.bSync    = true;
        adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

        /* Phase 1: Clear all MAC address filter entries (entries 1 to MAC_ADDR_MAX_ENTRIES) */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegMapCfg;
        uint32_t anRegmapData[MAC_ADDR_MAX_ENTRIES * 2U] = {0};
        uint16_t nStartAddr = MAC_ADDR_FILT_UPR_1_NW_ADDR;

        (void)ADI_MEMSET(&oRegMapCfg, 0, sizeof(oRegMapCfg));
        oRegMapCfg.anStartAddr = &nStartAddr;
        oRegMapCfg.nLength = (uint8_t)(MAC_ADDR_MAX_ENTRIES * 2U); /* Max: 15 * 2 = 30, fits uint8_t */
        oRegMapCfg.eType = ADI_E2BCORE_BURST_WRITE;
        oRegMapCfg.pBuffer = &anRegmapData[0U];

        eStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegMapCfg);

        if(eStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* Reset network frame config for phase 2 */
        (void)ADI_MEMSET(&oNwFrameCfg, 0, sizeof(oNwFrameCfg));
        eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* Phase 2: Restore default behavior - unknown traffic to LCE only */
        uint32_t nMask = BITM_MAC_CONFIG2_FWD_UNK2LCE | BITM_MAC_CONFIG2_FWD_UNK2HOST;
        uint32_t nRegData = BITM_MAC_CONFIG2_FWD_UNK2LCE;
        nStartAddr = MAC_CONFIG2_NW_ADDR;

        (void)ADI_MEMSET(&oRegMapCfg, 0, sizeof(oRegMapCfg));
        oRegMapCfg.anStartAddr = &nStartAddr;
        oRegMapCfg.anMask = &nMask;
        oRegMapCfg.nLength = 1U;
        oRegMapCfg.pBuffer = &nRegData;
        oRegMapCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

        eStatus = eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegMapCfg);

    } while(false);

    return eStatus;
}

/**
 * @brief Setup the VLAN filter table upto 4 entries and rule for untagged frames
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum     EAL instance number
 * @param [in] nRemoteNum   Remote node index
 * @param [in] anVlanId     Array of VLAN ID's to be proccessed by remote nodes
 * @param [in] eFilterRule  Filter rule for untagged frames (frames without VLAN ID)
 * @param [in] nVlanTag     Vlan tag to be sent, if not applicable set the parameter to ADI_VLAN_TAG_NA
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setVlanIdFilterTable(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t anVlanId[4U],
        ADI_EAL_VLAN_FILTER_RULE eFilterRule, uint32_t nVlanTag)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    uint16_t nStartAddr = LCE_DECODER_VLAN_CFG_NW_ADDR;
    uint32_t nRegData[2U];

    nRegData[0U] = ((uint32_t)anVlanId[0u] & BITM_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_0) |
                   (((uint32_t)anVlanId[1u] & BITM_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_0) << BITP_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_1)
                   |
                   ((uint32_t)eFilterRule << BITP_LCE_DECODER_VLAN_CFG_DECODER_UNTAGGED_VLAN_MODE);
    nRegData[1U] = ((uint32_t)anVlanId[2u] & BITM_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_0) | (((uint32_t)anVlanId[3u] &
                   BITM_LCE_DECODER_VLAN_CFG_DECODER_VLAN_ID_0) << BITP_LCE_DECODER_VLAN_CFG_2_DECODER_VLAN_ID_3);


    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 2U;
    oRegCfg.eType = ADI_E2BCORE_BURST_WRITE;
    oRegCfg.pBuffer = &nRegData[0U];

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/**
 * @brief Clear the VLAN filter table
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum     EAL instance number
 * @param [in] nRemoteNum   Remote node index
 * @param [in] nVlanTag     Vlan tag to be sent, if not applicable set the parameter to ADI_VLAN_TAG_NA
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_clearVlanIdFilterTable(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nVlanTag)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    /* Set reset value to clear VLAN filter table configurations */
    uint16_t nStartAddr = LCE_DECODER_VLAN_CFG_NW_ADDR;
    uint32_t nRegData[2U];
    nRegData[0U] = LCE_DECODER_VLAN_CFG_RESET_VAL;
    nRegData[1U] = LCE_DECODER_VLAN_CFG_2_RESET_VAL;

    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 2U;
    oRegCfg.eType = ADI_E2BCORE_BURST_WRITE;
    oRegCfg.pBuffer = &nRegData[0U];

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    eal_utils_fillNetworkCfg(nInstNum, nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/**
 * @brief Sets the specific VLAN ID for processing tagged 802.1as frames
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum    EAL instance number
 * @param [in] nVlanId     VLAN ID to be processed
 * @param [in] eFilterRule Filter rule for untagged 802.1as frames (frames without VLAN ID)
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_setGptpVlanIdFilter(uint32_t nInstNum, uint16_t nVlanId,
        ADI_EAL_VLAN_FILTER_RULE eFilterRule)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");

    uint16_t nStartAddr = LCE_GPTP_VLAN_CFG_NW_ADDR;
    uint32_t nRegData;

    /* Fill the vlan tag information that has to processed for Gptp frames */
    nRegData = ((uint32_t)nVlanId & BITM_LCE_GPTP_VLAN_CFG_GPTP_VLAN_ID) |
               ((uint32_t)eFilterRule << BITP_LCE_GPTP_VLAN_CFG_GPTP_UNTAGGED_VLAN_MODE);

    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = 1U;
    oRegCfg.eType = ADI_E2BCORE_RND_WRITE;
    oRegCfg.pBuffer = &nRegData;

    /* ~~~~~ Network frame configurations ~~~~~ */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag = gaoEalData[nInstNum].poE2bCoreCfg->anMcastVlanTag[ADI_E2BCORE_MAX_TOPICS - 1U];
    eal_utils_fillNetworkCfg(nInstNum, ADI_EAL_ALL_REMOTES, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox = false;
    oMsgOpts.bSync    = true;
    adi_eal_configureTopicOverride(nInstNum, &oMsgOpts);

    return eal_utils_sendRegmapMessage(nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/**
 * @brief Subscribe topic to LCE regmap or one of the four interfaces/FIFOs
 *
 * @note        adi_network_flushTxBuffer or adi_network_flushTxTimed must be called after this API
 *              to transmit the queued ethernet frame.
 *
 * @param [in] nInstNum          EAL instance number
 * @param [in] nRemoteNum        Remote node index
 * @param [in] anTopic           Array of topics - each topic is assigned to the corresponding subscriber in the aeTopicSubscriber
 * @param [in] aeTopicSubscriber Array of subscriber - can be either node regmap /interface /FIFO
 * @param [in] nNumTopicsCfg     Number of topics to be configured
 * @return     Status
 *              - #ADI_EAL_STATUS_SUCCESS           API successful
 *              - #ADI_EAL_STATUS_NTW_BUFF_FULL     Network buffer full (Retry after sometime)
 *              - #ADI_EAL_STATUS_START_FAILED      Failed to start the e2b transaction
 *              - #ADI_EAL_STATUS_REGMAPIO_FAILED   Failed to add the register map message
 */
ADI_EAL_CODE
ADI_EAL_STATUS adi_eal_subscribeTopic(uint32_t nInstNum, uint32_t nRemoteNum, uint8_t anTopic[],
                                      ADI_EAL_TOPIC_SUBSCRIBER aeTopicSubscriber[], uint8_t nNumTopicsCfg)
{
    /* Pre-conditions: */
    /*  * Validate the instance number */
    /*  * Validate the instance is in-use */
    /*  * Validate the remote number */
    /*  * Validate the topic is not out of range value */
    /*  * Validate the topic subscriber is not invalid */

    ADI_DBG_REQUIRE(nInstNum < ADI_EAL_MAX_NUM_INSTANCES, "Instance number is greater than max instances available");
    ADI_DBG_REQUIRE(gaoEalData[nInstNum].bInUse == true, "Instance is not in use");
    ADI_DBG_REQUIRE(nRemoteNum < gaoEalData[nInstNum].poE2bCoreCfg->nNumNodes, "Invalid remote number");

    for(uint8_t i = 0u; i < nNumTopicsCfg; i++)
    {
        ADI_DBG_REQUIRE(anTopic[i] < 127U, "Topics out of range");
        ADI_DBG_REQUIRE((uint32_t)aeTopicSubscriber[i] <= 8U, "Invalid topic subscriber");
        ganRegmapAddr[nInstNum][i] = 0xFC00U + ((uint16_t)anTopic[i] * 4U);
        ganRegmapData[nInstNum][i] = (uint32_t)1U << (uint32_t)aeTopicSubscriber[i];
        ganRegmapMask[nInstNum][i] = (uint32_t)1U << (uint32_t)aeTopicSubscriber[i];
    }

    /* Register map configuration */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = nNumTopicsCfg;
    oRegCfg.anStartAddr = &ganRegmapAddr[nInstNum][0U];
    oRegCfg.pBuffer = &ganRegmapData[nInstNum][0U];
    oRegCfg.anMask = &ganRegmapMask[nInstNum][0U];

    /* Send the register write */
    return adi_eal_sendRegTransaction(nInstNum, nRemoteNum, &oRegCfg);
}

/**
 * @brief Register callback function for interface interrupt
 *
 * @param [in] nInstNum                        EAL instance number
 * @param [in] nRemoteNum                      Remote node index
 * @param [in] nIntfNum                        Interface number
 * @param [in] pfInterfaceInterruptCallback    Callback function pointer for interface interrupt
 */
void adi_eal_registerInterfaceInterruptCbk(uint32_t nInstNum, uint32_t nRemoteNum, uint32_t nIntfNum,
        ADI_EAL_INTERRUPT_CALLBACK_FUNC pfInterfaceInterruptCallback)
{
    ADI_DBG_REQUIRE(pfInterfaceInterruptCallback != NULL, "Function pointer is NULL");

    gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].aeIntfCbk[nIntfNum] = pfInterfaceInterruptCallback;
}

/** @} */

/**
 * EOF: www.analog.com
 */
