/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: eal_utils.c
 * @brief: The utility functions for EAL
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

/*! \cond PRIVATE */
/*================================= INCLUDES ================================*/
#include "eal_internal.h"

/*================================= DEFINES =================================*/
/* Array of function pointers for e2bcore inteface API's */
static ADI_E2BCORE_ADD_INTF_MSG gafInfAPI[ADI_E2BCORE_IF_TYPE_COUNT] =
{
    [ADI_E2BCORE_IF_TYPE_UNDEF] =
    NULL,
    [ADI_E2BCORE_IF_TYPE_SPI] =
#ifdef ADI_E2B_IFACE_SPI_ENABLED
    &adi_e2bcore_addSpiMult,
#else
    NULL,
#endif
    [ADI_E2BCORE_IF_TYPE_FIO] =
#ifdef ADI_E2B_IFACE_FIO_ENABLED
    &adi_e2bcore_addFlexibleIOMult,
#else
    NULL,
#endif
    [ADI_E2BCORE_IF_TYPE_I2C] =
#ifdef ADI_E2B_IFACE_I2C_ENABLED
    &adi_e2bcore_addI2cMult,
#else
    NULL,
#endif
    [ADI_E2BCORE_IF_TYPE_ISELED] =
#ifdef ADI_E2B_IFACE_ISELED_ENABLED
    &adi_e2bcore_addIseledMult,
#else
    NULL,
#endif
    [ADI_E2BCORE_IF_TYPE_LIN] =
#ifdef ADI_E2B_IFACE_LIN_ENABLED
    &adi_e2bcore_addLinMult,
#else
    NULL,
#endif
    [ADI_E2BCORE_IF_TYPE_UART] =
#ifdef ADI_E2B_IFACE_UART_ENABLED
    &adi_e2bcore_adduartMult,
#else
    NULL,
#endif
    [ADI_E2BCORE_IF_TYPE_PORT] =
#ifdef ADI_E2B_IFACE_PORT_ENABLED
    &adi_e2bcore_addMultiplePortCtrlTxns,
#else
    NULL,
#endif
    [ADI_E2BCORE_IF_TYPE_ADC] =
#ifdef ADI_E2B_IFACE_ADC_ENABLED
    &adi_e2bcore_addAdcMult
#else
    NULL
#endif
};
/*=================================== PROTOTYPES =============================*/
static ADI_EAL_STATUS SendStatusReadFrame(ADI_E2BCORE_ID *poId, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
        ADI_E2BCORE_MSGOPTS *poMsgOpts, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, uint32_t nFifoTopic);
#ifdef ADI_E2B_IFACE_FIO_ENABLED
static ADI_EAL_STATUS SetFioRamAccessEnable(ADI_E2BCORE_ID *poId);
static ADI_EAL_STATUS WriteFioInstructionRam(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData);
static ADI_EAL_STATUS SetFioRegsAfterPgmInstRam(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData);
static ADI_EAL_STATUS ResetFlexibleIoInterface(ADI_E2BCORE_ID *poId);
static ADI_EAL_STATUS DisableAccessToInstRam(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData);
static ADI_EAL_STATUS SetFioStateMachineSettings(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_SM_CFG *poSmcCfg, uint8_t nSmIdx);
static ADI_EAL_STATUS SetFioClockDivider(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData);
static ADI_EAL_STATUS SetupFioAutomaticTrigger(ADI_E2BCORE_ID *poId);
static ADI_EAL_STATUS EnableTimeTag(ADI_E2BCORE_ID *poId);
static ADI_EAL_STATUS EnableFlexibleIoInterface(ADI_E2BCORE_ID *poId);
#endif /* ADI_E2B_IFACE_FIO_ENABLED */

/*=================================== CODE ==================================*/
/* Fills the frame configuration structure */
ADI_EAL_CODE
void eal_utils_fillNetworkCfg(uint32_t nInstNum, uint32_t nRemoteNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
                              uint32_t *pnVlanTag)
{
    bool bIsLocalLceConfigured =
        gaoEalData[nInstNum].poE2bCoreCfg->bIsLocalLCEConfigured; // Indicates if the node is dual mode

    (void)ADI_MEMSET(poNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    poNwFrameCfg->bDuplicate     = gaoEalData[nInstNum].bEnableFrameDup;
    poNwFrameCfg->bLenInclMacHdr = false;

    /* If the frame isn't a mulicast, eMacRoutingInfo is either set to ADI_MACPHY_ROUTE_PORT0_LP / ADI_MACPHY_ROUTE_PORT1_HP
    based on the type of node addressed and destination address will be a unicast address of the respective node */
    if(nRemoteNum != ADI_EAL_ALL_REMOTES)
    {
        poNwFrameCfg->eMacRoutingInfo = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].bIsLocalLCE ?
                                        ADI_MACPHY_ROUTE_PORT1_HP : ADI_MACPHY_ROUTE_PORT0_LP;
        poNwFrameCfg->panPhysAddr = &gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].anMacAddr[0u];
    }
    /* If the frame is destined to a multicast address eMacRoutingInfo is set to ADI_MACPHY_ROUTE_BOTH / ADI_MACPHY_ROUTE_PORT0_LP
    depending on whether the controller is in dual mode or not */
    else
    {
        poNwFrameCfg->eMacRoutingInfo = bIsLocalLceConfigured ? ADI_MACPHY_ROUTE_BOTH : ADI_MACPHY_ROUTE_PORT0_LP;
        poNwFrameCfg->panPhysAddr = &ganAdiE2bMulticastAddr[0u];
    }

    poNwFrameCfg->eTsReg          = ADI_TS_EGRESS_NONE;
    poNwFrameCfg->nFrameType      = ADI_E2BCORE_ETYPE;
    poNwFrameCfg->nLenByte        = ADI_EAL_MAX_FRAME_LENGTH;

    /* Add VLAN info if there is a valid tag */
    if(EXTRACT_VLAN_TPID(*pnVlanTag) == VLAN_C_TAG_TPID)
    {
        poNwFrameCfg->nNumVlanTags    = 1U;
        poNwFrameCfg->panVlanTagArr   = &pnVlanTag[0U];
    }
}

/* Creates a regmap io transaction and transmits */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_sendRegmapMessage(uint32_t nInstNum,
        ADI_NETWORK_FRAME_CFG *pNwFrameCfg, ADI_E2BCORE_MSGOPTS *poRegMsgOpts, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg)
{
    uint8_t *pBuffer = NULL;
    uint32_t nActPktLen = 0U;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    bool bShouldReleaseNwMutex = false;

    /* Get the device number mapped to this instance */
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;

    do
    {
        /* Get Buffer from network layer */
        if(adi_network_getTxBuffer(nDevNum, &pBuffer, pNwFrameCfg) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Start creating e2b packet */
        if(adi_e2bcore_startPacket(nInstNum, pNwFrameCfg->panPhysAddr, pBuffer, pNwFrameCfg->nLenByte,
                                   false) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* Add the register message */
        if(adi_e2bcore_addRegmapIO(nInstNum, poRegMsgOpts, poRegCfg, 1u) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }

        /* Finished with e2b messages */
        adi_e2bcore_finishPacket(nInstNum, &nActPktLen,
                                 pNwFrameCfg->bLenInclMacHdr, pNwFrameCfg->nNumVlanTags);

        /* Update the actual length of the buffer used */
        pNwFrameCfg->nLenByte = (uint16_t)nActPktLen;

        /* Ask network layer to transmit the frame */
        adi_network_transmit(nDevNum, pNwFrameCfg);
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/* Gets interface and FIFO status */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_readInfStatus(ADI_E2BCORE_ID *poId, uint16_t nStatusRegAddr, uint8_t nLength,
                                       bool bIsMcast, uint32_t anTopics[2U], uint32_t nVlanTag)
{
    /*~~~~~~~~~~ Local variables ~~~~~~~~~~*/
    uint16_t anRegAddr;
    uint16_t anFifoRegAddr;
    uint32_t nInstNum;
    uint32_t nRemoteNum;
    ADI_E2BCORE_REGMAP_IO_CONFIG aoRegmapCfg[2U];
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    ADI_E2BCORE_MSGOPTS oMsgOpts;

    /* Get the instance number, remote number and interface number */
    nInstNum = poId->nInstNum;
    nRemoteNum = poId->nRemoteNum;

    /* Set the parameters for getting a buffer from network layer
       and transmitting the e2b packet */
    eal_utils_fillNetworkCfg(nInstNum, bIsMcast ? ADI_EAL_ALL_REMOTES : nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* ~~~~~ e2bcore message options ~~~~~ */
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(ADI_E2BCORE_MSGOPTS));
    oMsgOpts.bMailbox       = false;
    oMsgOpts.bSync          = true;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic         = anTopics[0U];

    anRegAddr = (uint16_t)nStatusRegAddr;

    /* Set the parameters for the FIO regmap transaction */
    (void)ADI_MEMSET(&aoRegmapCfg, 0, sizeof(aoRegmapCfg));
    aoRegmapCfg[0u].eType       = ADI_E2BCORE_BURST_READ;
    aoRegmapCfg[0u].nLength     = nLength;
    aoRegmapCfg[0u].anStartAddr = &anRegAddr;

    anFifoRegAddr = (uint16_t)FIFO_STATUS_NW_ADDR;

    /* Set the parameters for the FIFO transaction */
    aoRegmapCfg[1u].eType       = ADI_E2BCORE_BURST_READ;
    aoRegmapCfg[1u].nLength     = 5U;
    aoRegmapCfg[1u].anStartAddr = &anFifoRegAddr;

    /* Create the frame and send it */
    return SendStatusReadFrame(poId, &oNwFrameCfg, &oMsgOpts, &aoRegmapCfg[0U], anTopics[1U]);
}

/* Send a NOP resync transaction */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_sendNopResyncTxn(ADI_E2BCORE_ID *poId)
{
    /*~~~~~~~~~~ Local variables ~~~~~~~~~~*/
    ADI_NETWORK_FRAME_CFG   oNwFrameCfg;
    ADI_E2BCORE_MSGOPTS     oMsgOpts;
    ADI_E2BCORE_TXN_CONFIG  oTxnCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nFifoWriteVlanTag;

    /* Set the parameters for getting a buffer from network layer
      and transmitting the e2b packet */
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the parameters for the fifo message header */
    (void)ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = true;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_FIFO0 + poId->nIntfNum;

    /* Set transaction config to resync */
    (void)ADI_MEMSET(&oTxnCfg, 0, sizeof(ADI_E2BCORE_TXN_CONFIG));
    oTxnCfg.oTxnHdr.bAreTxnsPacked = true;
    oTxnCfg.oTxnHdr.nNumPackedTxns = 0U;
    oTxnCfg.oTxnHdr.bResyncTillThisTxn = true;

    return eal_utils_sendFifoFrame(poId, &oNwFrameCfg, &oTxnCfg, &oMsgOpts);
}

/* Sends FIFO message */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_sendFifoFrame(ADI_E2BCORE_ID *poId,
                                       ADI_NETWORK_FRAME_CFG *pNwFrameCfg,
                                       ADI_E2BCORE_TXN_CONFIG *poTxnCfg,
                                       ADI_E2BCORE_MSGOPTS *pFifoMsgOpts)
{
    /*~~~~~~~~~~ Local variables ~~~~~~~~~~*/
    uint8_t *pBuffer = NULL;
    uint32_t nInstNum;
    uint32_t nRemoteNum;
    uint32_t nDevNum;
    uint32_t nActPktLen = 0U;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    bool bShouldReleaseNwMutex = false;

    /* Get the instance number, remote number and interface number */
    nInstNum = poId->nInstNum;
    nRemoteNum = poId->nRemoteNum;

    /* Get the device number mapped to this instance */
    nDevNum = gaoEalData[nInstNum].nDevNum;

    ADI_E2BCORE_ADD_INTF_MSG AddInfMsgAPI =
        gafInfAPI[gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[poId->nIntfNum].eIfType];

    do
    {
        /* Check if the interface API is valid */
        if(AddInfMsgAPI == NULL)
        {
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            break;
        }

        /* Get a buffer from the network layer for filling the e2b data */
        if(adi_network_getTxBuffer(nDevNum, &pBuffer, pNwFrameCfg) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Start filling e2b data into the buffer */
        if(adi_e2bcore_startPacket(nInstNum, pNwFrameCfg->panPhysAddr, pBuffer, pNwFrameCfg->nLenByte,
                                   false) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* Add the interface transaction */
        if(AddInfMsgAPI(nInstNum, pFifoMsgOpts, 1U, poTxnCfg) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            break;
        }

        /* Finish adding e2b data into the buffer, and get the total e2b packet length */
        adi_e2bcore_finishPacket(nInstNum, &nActPktLen,
                                 pNwFrameCfg->bLenInclMacHdr, pNwFrameCfg->nNumVlanTags);

        /* Update the length for the network layer call */
        pNwFrameCfg->nLenByte = (uint16_t)nActPktLen;

        /* Transmit the frame. This adds the complete frame into the queue */
        adi_network_transmit(nDevNum, pNwFrameCfg);

    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/* Sends Regmap and FIFO message in a single frame */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_sendRegmapFifoFrame(ADI_E2BCORE_ID *poId,
        ADI_NETWORK_FRAME_CFG *pNwFrameCfg,
        ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg,
        ADI_E2BCORE_TXN_CONFIG *poTxnCfg,
        uint32_t *panTopics)
{
    /*~~~~~~~~~~ Local variables ~~~~~~~~~~*/
    uint8_t *pBuffer = NULL;
    uint32_t nInstNum;
    uint32_t nRemoteNum;
    uint32_t nDevNum;
    uint32_t nActPktLen = 0U;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    bool bShouldReleaseNwMutex = false;

    /* Get the instance number, remote number and interface number */
    nInstNum = poId->nInstNum;
    nRemoteNum = poId->nRemoteNum;

    /* Get the device number mapped to this instance */
    nDevNum = gaoEalData[nInstNum].nDevNum;

    ADI_E2BCORE_ADD_INTF_MSG AddInfMsgAPI =
        gafInfAPI[gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs[nRemoteNum].aoIntfConfigs[poId->nIntfNum].eIfType];

    /* Set the interface message options */
    ADI_E2BCORE_MSGOPTS oIntfMsgOpts;
    (void)ADI_MEMSET(&oIntfMsgOpts, 0, sizeof(oIntfMsgOpts));
    oIntfMsgOpts.bSync          = false;
    oIntfMsgOpts.bTopicOverRide = true;
    oIntfMsgOpts.nTopic         = panTopics[0U];

    /* Set the FIFO message options */
    ADI_E2BCORE_MSGOPTS oFifoMsgOpts;
    (void)ADI_MEMSET(&oFifoMsgOpts, 0, sizeof(oFifoMsgOpts));
    oFifoMsgOpts.bSync          = false;
    oFifoMsgOpts.bTopicOverRide = true;
    oFifoMsgOpts.nTopic         = panTopics[1U];

    do
    {
        /* Check if the interface API is valid */
        if(AddInfMsgAPI == NULL)
        {
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            break;
        }

        /* Get a buffer from the network layer for filling the e2b data */
        if(adi_network_getTxBuffer(nDevNum, &pBuffer, pNwFrameCfg) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Start filling e2b data into the buffer */
        if(adi_e2bcore_startPacket(nInstNum, pNwFrameCfg->panPhysAddr, pBuffer, pNwFrameCfg->nLenByte,
                                   false) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* Add the register map transaction */
        if(adi_e2bcore_addRegmapIO(nInstNum, &oIntfMsgOpts, poRegCfg, 1U) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }

        /* Add the interface transaction */
        if(AddInfMsgAPI(nInstNum, &oFifoMsgOpts, 1U, poTxnCfg) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
            break;
        }

        /* Finish adding e2b data into the buffer, and get the total e2b packet length */
        adi_e2bcore_finishPacket(nInstNum, &nActPktLen,
                                 pNwFrameCfg->bLenInclMacHdr, pNwFrameCfg->nNumVlanTags);

        /* Update the length for the network layer call */
        pNwFrameCfg->nLenByte = (uint16_t)nActPktLen;

        /* Transmit the frame. This adds the complete frame into the queue */
        adi_network_transmit(nDevNum, pNwFrameCfg);

    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

/* Wait for timeout of register read response */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_waitForTimeout(bool *pbWaitingFlag, uint32_t nTimeout)
{
    uint64_t nCurrTime, nStartTime;
    bool bTimedOut = false;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    /* Get the starting time */
    adi_pal_getCurrTime(&nCurrTime);
    nStartTime = nCurrTime;

    /* Wait for reg read response */
    while(true)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTime);

        /* Check if the response has timed-out */
        if(nCurrTime - nStartTime > nTimeout)
        {
            bTimedOut = true;
        }

        /* If timed-out or response received, exit the loop */
        if(bTimedOut || !(*pbWaitingFlag))
        {
            break;
        }
    }

    if(bTimedOut)
    {
        eEalStatus = ADI_EAL_STATUS_RX_TIMEOUT;
    }

    return eEalStatus;
}

/* Gets buffer from network layer and starts E2B message - note that network mutex is not released incase if there is an error, this has to be taken from EAL */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_startE2bMessage(uint32_t nInstNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
        ADI_E2BCORE_MSGOPTS *poFifoMsgOpts)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint8_t *pBuffer = NULL;

    /* Get the device number mapped to this instance */
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;

    do
    {
        /* Get a buffer from the network layer for filling the e2b data */
        ADI_NETWORK_ERR eNwErr = adi_network_getTxBuffer(nDevNum, &pBuffer, poNwFrameCfg);

        if(eNwErr != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Start filling e2b data into the buffer */
        if(adi_e2bcore_startPacket(nInstNum, poNwFrameCfg->panPhysAddr, pBuffer, poNwFrameCfg->nLenByte,
                                   false) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* Start e2b FIFO write message */
        if(adi_e2bcore_startMessage(nInstNum, poFifoMsgOpts, E2B_MSGTYPE_FIFO_WRITE_IO) != ADI_E2BCORE_ERR_NONE)
        {
            eEalStatus = ADI_EAL_STATUS_FIFO_FAILED;
        }
    } while(false);

    return eEalStatus;
}

/* Finishes E2B message and transmits */
ADI_EAL_CODE
void eal_utils_finishMessageAndTransmit(uint32_t nInstNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
                                        ADI_E2BCORE_MSGOPTS *poFifoMsgOpts)
{
    uint32_t nActPktLen = 0U;

    /* Get the device number mapped to this instance */
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;

    /* Finish adding all UART transactions into the FIFO message */
    adi_e2bcore_finishMessage(nInstNum, poFifoMsgOpts);

    /* Finish adding e2b data into the buffer, and get the total e2b packet length */
    adi_e2bcore_finishPacket(nInstNum, &nActPktLen,
                             poNwFrameCfg->bLenInclMacHdr, poNwFrameCfg->nNumVlanTags);

    /* Update the length for the network layer call */
    poNwFrameCfg->nLenByte = (uint16_t)nActPktLen;

    /* Transmit the frame. This adds the complete frame into the queue */
    adi_network_transmit(nDevNum, poNwFrameCfg);
}

/* Get the value for the remote node registers address from config file only */
ADI_EAL_CODE
bool eal_utils_getConfigRemoteRegVal(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nRegAddr, uint32_t *pRegData)
{
    ADI_E2BCORE_REMOTE_CONFIG *poRemoteConfig;
    ADI_E2BCORE_REG_ADDR_VAL *poRegData;
    uint32_t nNumRegisters;
    bool bIsRegisterConfigured = false;

    /* Get the remote config to get the register values */
    poRemoteConfig = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs;

    /* Get the pointer to register address value pairs for node level registers */
    poRegData = poRemoteConfig[nRemoteNum].aoRegData;
    nNumRegisters = poRemoteConfig[nRemoteNum].nNumRegisters;

    /* Loop through the remote node registers to get the value corresponding to the address given */
    for(uint8_t i = 0u; i < nNumRegisters; i++)
    {
        if(poRegData[i].nAddr == nRegAddr)
        {
            *pRegData = poRegData[i].nVal;
            bIsRegisterConfigured = true;
            break;
        }
    }

    return bIsRegisterConfigured;
}

/* Setter function for the remote node register's - used to update the config file only, doesn't do an actual write to the remote register */
/* If the register is not present in the config file, it doesnt add a new reg value pair to the config file */
ADI_EAL_CODE
bool eal_utils_setConfigRemoteRegVal(uint32_t nInstNum, uint32_t nRemoteNum, uint16_t nRegAddr,
                                     uint32_t nRegData)
{
    ADI_DBG_REQUIRE(nRemoteNum < ADI_EAL_MAX_NUM_NODES, "Remote number is not in supported range");

    ADI_E2BCORE_REMOTE_CONFIG *poRemoteConfig;
    ADI_E2BCORE_REG_ADDR_VAL *poRegData;
    uint32_t nNumRegisters;
    bool bIsRegisterConfigured = false;

    /* Get the remote config to get the register values */
    poRemoteConfig = gaoEalData[nInstNum].poE2bCoreCfg->aoRemoteCfgs;

    /* Get the pointer to register address value pairs for node level registers */
    poRegData = poRemoteConfig[nRemoteNum].aoRegData;
    nNumRegisters = poRemoteConfig[nRemoteNum].nNumRegisters;

    /* Loop through the remote node registers to get the value corresponding to the address given */
    for(uint8_t i = 0u; i < nNumRegisters; i++)
    {
        if(poRegData[i].nAddr == nRegAddr)
        {
            poRegData[i].nVal = nRegData;
            bIsRegisterConfigured = true;
            break;
        }
    }

    return bIsRegisterConfigured;
}

/* Get the value for the interface level address from config file only */
ADI_EAL_CODE
bool eal_utils_getConfigInfRegVal(ADI_E2BCORE_ID *poId, uint16_t nRegAddr, uint32_t *pRegData, bool bIsFifoReg)
{
    /* local variables */
    ADI_E2BCORE_IF_CONFIG *poInfConfig;
    bool bIsRegisterConfigured = false;
    ADI_E2BCORE_REG_ADDR_VAL *poRegData;
    uint32_t nNumRegisters;

    /* Get the interface config to get the register values */
    poInfConfig = gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs;

    /* Get the FIFO registers for that interface */
    if(bIsFifoReg)
    {
        poRegData = poInfConfig[poId->nIntfNum].aoFifoRegData;
        nNumRegisters = poInfConfig[poId->nIntfNum].nNumFifoReg;
    }
    /* Get the interface registers for that interface */
    else
    {
        poRegData = poInfConfig[poId->nIntfNum].aoIfRegData;
        nNumRegisters = poInfConfig[poId->nIntfNum].nNumIfReg;
    }

    /* Loop through the registers to get the value corresponding to the address given */
    for(uint8_t i = 0u; i < nNumRegisters; i++)
    {
        if(poRegData[i].nAddr == nRegAddr)
        {
            *pRegData = poRegData[i].nVal;
            bIsRegisterConfigured = true;
            break;
        }
    }

    return bIsRegisterConfigured;
}

/* Setter function for the interface level register's - used to update the config file only, doesn't do an actual write to the interface register */
ADI_EAL_CODE
bool eal_utils_setConfigInfRegVal(ADI_E2BCORE_ID *poId, uint16_t nRegAddr, uint32_t nRegData, bool bIsFifoReg)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(poId != NULL, "Pointer to ID is NULL");

    /* local variables */
    ADI_E2BCORE_IF_CONFIG *poInfConfig;
    bool bIsRegisterConfigured = false;
    ADI_E2BCORE_REG_ADDR_VAL *poRegData;
    uint32_t nNumRegisters;

    /* Get the interface config to get the register values */
    poInfConfig = gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs;

    /* Get the FIFO registers for that interface */
    if(bIsFifoReg)
    {
        poRegData = poInfConfig[poId->nIntfNum].aoFifoRegData;
        nNumRegisters = poInfConfig[poId->nIntfNum].nNumFifoReg;
    }
    /* Get the interface registers for that interface */
    else
    {
        poRegData = poInfConfig[poId->nIntfNum].aoIfRegData;
        nNumRegisters = poInfConfig[poId->nIntfNum].nNumIfReg;
    }

    /* Loop through the registers to get the value corresponding to the address given */
    for(uint8_t i = 0u; i < nNumRegisters; i++)
    {
        if(poRegData[i].nAddr == nRegAddr)
        {
            poRegData[i].nVal = nRegData;
            bIsRegisterConfigured = true;
            break;
        }
    }

    return bIsRegisterConfigured;
}

/* Compute the parity of the provided data */
ADI_EAL_CODE
uint8_t eal_utils_computeParity(ADI_EAL_PARITY_TYPE eParityType, uint8_t nTxData)
{
    /* Local variable to compute parity */
    uint8_t nParity;
    /* Computation of the parity */
    nParity = nTxData ^ (nTxData >> 1u);
    nParity = nParity ^ (nParity >> 2u);
    nParity = nParity ^ (nParity >> 4u);
    nParity = nParity | 0xfeu;

    /* Check if the parity type required is ODD or EVEN and give an approriate parity bit */
    if(eParityType == ADI_EAL_ODD_PARITY)
    {
        if(nParity == 0xffu)
        {
            nParity = 0u;
        }
        else
        {
            nParity = 1u;
        }
    }
    else
    {
        if(nParity != 0xffu)
        {
            nParity = 0u;
        }
        else
        {
            nParity = 1u;
        }
    }

    return nParity;
}

/* Compute the parity of the provided data (upto 16-bits) */
ADI_EAL_CODE
uint8_t eal_utils_computeParity16(ADI_EAL_PARITY_TYPE eParityType, uint16_t nData)
{
    uint8_t nParity;
    uint16_t nTemp = nData;

    /* Computation of the parity - handles up to 16 bits */
    nTemp = nTemp ^ (nTemp >> 1U);
    nTemp = nTemp ^ (nTemp >> 2U);
    nTemp = nTemp ^ (nTemp >> 4U);
    nTemp = nTemp ^ (nTemp >> 8U);
    nParity = (uint8_t)(nTemp & 0x01U);

    /* Check if the parity type required is ODD or EVEN and give an appropriate parity bit */
    if(eParityType == ADI_EAL_ODD_PARITY)
    {
        nParity = (nParity == 0U) ? 1U : 0U;
    }

    return nParity;
}

#ifdef ADI_E2B_IFACE_FIO_ENABLED

/* Configures the flexible IO registers */
ADI_EAL_CODE
ADI_EAL_STATUS eal_utils_configureFlexibleIO(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nDevNum = gaoEalData[poId->nInstNum].nDevNum;

    do
    {
        /* ~~~~~ Set the RAM access enable before programming the instruction RAM ~~~~~ */
        if((eEalStatus = SetFioRamAccessEnable(poId)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);

        /* ~~~~~ Write the instruction RAM ~~~~~ */
        if((eEalStatus = WriteFioInstructionRam(poId, poFioInstRamData)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);

        /* ~~~~~ Set the flexible IO registers after programming the instruction RAM */
        if((eEalStatus = SetFioRegsAfterPgmInstRam(poId, poFioInstRamData)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);
    } while(false);

    return eEalStatus;
}

#endif /* ADI_E2B_IFACE_FIO_ENABLED */

/* Converts VLAN CFG structure to uint32_t type */
ADI_EAL_CODE
uint32_t eal_utils_convertVlanTags(ADI_VLAN_CFG *poVlanCfg)
{
    /* Convert vlan tags from ADI_VLAN_CFG structure to uint32_t tags */
    return ((uint32_t)poVlanCfg->nTPID << 16U) | (((uint32_t)poVlanCfg->nPCP & 0x07U) << 13U) |
           (poVlanCfg->bDEI ? 0X1000U : 0U) |
           ((uint32_t)poVlanCfg->nVlanId & 0xFFFU);
}

/*============================= STATIC FUNCTIONS ============================*/
/* Reads interface status */
ADI_EAL_CODE
static ADI_EAL_STATUS SendStatusReadFrame(ADI_E2BCORE_ID *poId, ADI_NETWORK_FRAME_CFG *poNwFrameCfg,
        ADI_E2BCORE_MSGOPTS *poMsgOpts, ADI_E2BCORE_REGMAP_IO_CONFIG *poRegCfg, uint32_t nFifoTopic)
{
    /* Local variables */
    uint8_t *pBuffer = NULL;
    uint32_t nActPktLen = 0U;
    bool bShouldReleaseNwMutex = false;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;
    uint32_t nInstNum;
    uint32_t nDevNum;

    /* Get the instance number */
    nInstNum = poId->nInstNum;

    /* Get the device number mapped to this instance */
    nDevNum = gaoEalData[nInstNum].nDevNum;

    do
    {
        /* Get Buffer from network layer */
        if(adi_network_getTxBuffer(nDevNum, &pBuffer, poNwFrameCfg) != ADI_NETWORK_OK)
        {
            eEalStatus = ADI_EAL_STATUS_NTW_BUFF_FULL;
            break;
        }

        /* Start creating e2b packet */
        if(adi_e2bcore_startPacket(nInstNum, poNwFrameCfg->panPhysAddr, pBuffer, poNwFrameCfg->nLenByte,
                                   false) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_START_FAILED;
            break;
        }

        /* Add the FIO register message */
        if(adi_e2bcore_addRegmapIO(nInstNum, poMsgOpts, &poRegCfg[0U], 1U) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }

        /* Add the FIFO register message */
        poMsgOpts->nTopic = nFifoTopic;

        if(adi_e2bcore_addRegmapIO(nInstNum, poMsgOpts, &poRegCfg[1U], 1U) != ADI_E2BCORE_ERR_NONE)
        {
            bShouldReleaseNwMutex = true;
            eEalStatus = ADI_EAL_STATUS_REGMAPIO_FAILED;
            break;
        }

        /* Finished with e2b messages */
        adi_e2bcore_finishPacket(nInstNum, &nActPktLen,
                                 poNwFrameCfg->bLenInclMacHdr, poNwFrameCfg->nNumVlanTags);

        /* Update the actual length of the buffer used */
        poNwFrameCfg->nLenByte = (uint16_t)nActPktLen;

        /* Ask network layer to transmit the frame */
        adi_network_transmit(nDevNum, poNwFrameCfg);

        /* Flush the network layer buffers */
        adi_network_flushTxBuffer(nDevNum, 0ULL);
    } while(false);

    /* If network layer mutex was acquired to fill a frame,
       but couldn't be completed release the mutex */
    RELEASE_NW_MUTEX();

    return eEalStatus;
}

#ifdef ADI_E2B_IFACE_FIO_ENABLED

/* Set the RAM access enable before programming the instruction RAM */
ADI_EAL_CODE
static ADI_EAL_STATUS SetFioRamAccessEnable(ADI_E2BCORE_ID *poId)
{
    /* ~~~~~ Enable access to instruction RAM ~~~~~ */
    /* Fill the address, mask and data */
    uint16_t nAddr = FLEXIO_CONTROL_2_NW_ADDR;
    uint32_t nMask = BITM_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE | BITM_FLEXIO_CONTROL_2_PROGRAM_VALID;
    uint32_t nData = ((uint32_t)1U << BITP_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE) |
                     ((uint32_t)0U << BITP_FLEXIO_CONTROL_2_PROGRAM_VALID);

    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/* Write FIO instruction RAM */
ADI_EAL_CODE
static ADI_EAL_STATUS WriteFioInstructionRam(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData)
{
    /* Start address of the instruction RAM */
    uint16_t nStartAddr = 0x1000U;

    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_BURST_WRITE;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.nLength = (uint8_t)poFioInstRamData->nNumInstMemEntries;
    oRegCfg.pBuffer = &poFioInstRamData->panInstructionMemData[0U];

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/* Set the flexible IO registers after programming the instruction RAM */
ADI_EAL_CODE
static ADI_EAL_STATUS SetFioRegsAfterPgmInstRam(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData)
{
    ADI_EAL_STATUS eEalStatus;
    uint32_t nDevNum = gaoEalData[poId->nInstNum].nDevNum;

    do
    {
        /* ~~~~~ Reset the Flexible IO interface after programming the flexible IO RAM ~~~~~ */
        if((eEalStatus = ResetFlexibleIoInterface(poId)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);

        /* ~~~~~ Disable access to instruction RAM and set FIFO TX ROUTER ~~~~~ */
        if((eEalStatus = DisableAccessToInstRam(poId, poFioInstRamData)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);

        /* ~~~~~ Set the State machine settings ~~~~~ */
        for(uint8_t i = 0U; i < poFioInstRamData->nNumActiveStateMachines; i++)
        {
            if((eEalStatus = SetFioStateMachineSettings(poId, &poFioInstRamData->aoSmCfgs[i], i)) != ADI_EAL_STATUS_SUCCESS)
            {
                break;
            }

            adi_network_flushTxBuffer(nDevNum, 0ULL);
        }

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        /* ~~~~~ Setup clock divider for SM0, SM1, SM2 & SM3 ~~~~~ */
        if((eEalStatus = SetFioClockDivider(poId, poFioInstRamData)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);

        /* ~~~~~ Setup automatic trigger mode ~~~~~ */
        if((eEalStatus = SetupFioAutomaticTrigger(poId)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);

        /* ~~~~~ If Timetag is enabled in configurations. Enable the Time tag here ~~~~~ */
        if((eEalStatus = EnableTimeTag(poId)) != ADI_EAL_STATUS_SUCCESS)
        {
            break;
        }

        adi_network_flushTxBuffer(nDevNum, 0ULL);

        /* ~~~~~ Enable the FIO interface ~~~~~ */
        eEalStatus = EnableFlexibleIoInterface(poId);
    } while(false);

    return eEalStatus;
}

/* Reset the Flexible IO interface */
ADI_EAL_CODE
static ADI_EAL_STATUS ResetFlexibleIoInterface(ADI_E2BCORE_ID *poId)
{
    /* Fill the address, mask and data */
    uint16_t nAddr = FLEXIO_CONTROL_NW_ADDR;
    uint32_t nMask = BITM_FLEXIO_CONTROL_SOFT_RESET;
    uint32_t nData = ((uint32_t)0xAU << BITP_FLEXIO_CONTROL_SOFT_RESET);

    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/* Disable access to instruction RAM and set FIFO TX ROUTER */
ADI_EAL_CODE
static ADI_EAL_STATUS DisableAccessToInstRam(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData)
{
    /* ~~~~~ Disable access to instruction RAM ~~~~~ */
    /* Fill the address, mask and data */
    uint16_t nAddr = FLEXIO_CONTROL_2_NW_ADDR;
    uint32_t nMask = BITM_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE |
                     BITM_FLEXIO_CONTROL_2_PROGRAM_VALID |
                     BITM_FLEXIO_CONTROL_2_FIFO_TX_ROUTER_FIXED_SM;
    uint32_t nData = ((uint32_t)0U << BITP_FLEXIO_CONTROL_2_RAM_ACCESS_ENABLE) |
                     ((uint32_t)1U << BITP_FLEXIO_CONTROL_2_PROGRAM_VALID) |
                     (poFioInstRamData->bFifoTxRouterFixedSm ? (uint32_t)BITM_FLEXIO_CONTROL_2_FIFO_TX_ROUTER_FIXED_SM : (uint32_t)0U);

    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/* Set the State machine settings */
ADI_EAL_CODE
static ADI_EAL_STATUS SetFioStateMachineSettings(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_SM_CFG *poSmcCfg, uint8_t nSmIdx)
{
    uint16_t anAddr[] =
    {
        (uint16_t)(FLEXIO_CONTROL_SM0_NW_ADDR + (uint16_t)nSmIdx * 4U),
        (uint16_t)(FLEXIO_CONTROL_SM0_1_NW_ADDR + (uint16_t)nSmIdx * 4U),
    };
    uint32_t anData[] =
    {
        /* Set the register fields for FLEXIO statemachine */
        (((uint32_t)poSmcCfg->nPinOutBasepin << BITP_FLEXIO_CONTROL_SM0_PINOUT_BASEPIN0) |
        ((uint32_t)poSmcCfg->nPinInBasepin << BITP_FLEXIO_CONTROL_SM0_PININ_BASEPIN0) |
        ((uint32_t)poSmcCfg->nSidesetBasepin << BITP_FLEXIO_CONTROL_SM0_SIDESET_BASEPIN0) |
        ((uint32_t)poSmcCfg->nProgramStart << BITP_FLEXIO_CONTROL_SM0_PROGRAM_START0) |
        ((uint32_t)poSmcCfg->nProgramSize << BITP_FLEXIO_CONTROL_SM0_PROGRAM_SIZE0) |
        ((uint32_t)(poSmcCfg->bDelaySidesetSingleMode ? 0U : 1U) << BITP_FLEXIO_CONTROL_SM0_DELAY_SIDESET_MODE0) |
        ((uint32_t)poSmcCfg->nDelaySidesetBits << BITP_FLEXIO_CONTROL_SM0_DELAY_SIDESET_BITS0) |
        ((uint32_t)poSmcCfg->nJmpBasepin << BITP_FLEXIO_CONTROL_SM0_JMP_BASEPIN0)),
        (((uint32_t)poSmcCfg->nAutopullThreshold << BITP_FLEXIO_CONTROL_SM0_1_AUTOPULL_THRESHOLD0) |
        ((uint32_t)poSmcCfg->nAutopushThreshold << BITP_FLEXIO_CONTROL_SM0_1_AUTOPUSH_THRESHOLD0) |
        ((uint32_t)poSmcCfg->nPullThreshold << BITP_FLEXIO_CONTROL_SM0_1_PULL_THRESHOLD0) |
        ((uint32_t)poSmcCfg->nPushThreshold << BITP_FLEXIO_CONTROL_SM0_1_PUSH_THRESHOLD0) |
        ((uint32_t)(poSmcCfg->bOsrShiftRight ? 0U : 1U) << BITP_FLEXIO_CONTROL_SM0_1_OSR_SHIFT_DIRECTION0) |
        ((uint32_t)(poSmcCfg->bIsrShiftRight ? 0U : 1U) << BITP_FLEXIO_CONTROL_SM0_1_ISR_SHIFT_DIRECTION0) |
        ((uint32_t)(poSmcCfg->bEnableAutopush ? 1U : 0U) << BITP_FLEXIO_CONTROL_SM0_1_AUTOPUSH0) |
        ((uint32_t)(poSmcCfg->bEnableAutopull ? 1U : 0U) << BITP_FLEXIO_CONTROL_SM0_1_AUTOPULL0)),
    };

    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_RND_WRITE;
    oRegCfg.nLength = 2U;
    oRegCfg.anStartAddr = &anAddr[0U];
    oRegCfg.pBuffer = &anData[0U];

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/* Set the clock divider settings */
ADI_EAL_CODE
static ADI_EAL_STATUS SetFioClockDivider(ADI_E2BCORE_ID *poId, ADI_EAL_FIO_INST_RAM_DATA *poFioInstRamData)
{
    /* Fill the address, data, and mask */
    uint16_t nStartAddr = FLEXIO_CLKDIV0_1_NW_ADDR;
    uint32_t anData[] =
    {
        ((uint32_t)poFioInstRamData->aoSmCfgs[0U].nClkDiv << BITP_FLEXIO_CLKDIV0_1_CLKDIV0) |
        ((uint32_t)poFioInstRamData->aoSmCfgs[1U].nClkDiv << BITP_FLEXIO_CLKDIV0_1_CLKDIV1),
        ((uint32_t)poFioInstRamData->aoSmCfgs[2U].nClkDiv << BITP_FLEXIO_CLKDIV2_3_CLKDIV2) |
        ((uint32_t)poFioInstRamData->aoSmCfgs[3U].nClkDiv << BITP_FLEXIO_CLKDIV2_3_CLKDIV3),
    };
    uint32_t anMask[] =
    {
        poFioInstRamData->nNumActiveStateMachines > 1U ? BITM_FLEXIO_CLKDIV0_1_CLKDIV0 |
        BITM_FLEXIO_CLKDIV0_1_CLKDIV1 : BITM_FLEXIO_CLKDIV0_1_CLKDIV0,
        poFioInstRamData->nNumActiveStateMachines > 3U ? BITM_FLEXIO_CLKDIV2_3_CLKDIV2 |
        BITM_FLEXIO_CLKDIV2_3_CLKDIV3 : BITM_FLEXIO_CLKDIV2_3_CLKDIV2,
    };
    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_BURST_MASK_WRITE;
    oRegCfg.nLength = poFioInstRamData->nNumActiveStateMachines > 2U ? 2U : 1U;
    oRegCfg.anStartAddr = &nStartAddr;
    oRegCfg.pBuffer = &anData[0U];
    oRegCfg.anMask = &anMask[0U];

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/* Setup automatic trigger mode */
ADI_EAL_CODE
static ADI_EAL_STATUS SetupFioAutomaticTrigger(ADI_E2BCORE_ID *poId)
{
    /* Fill the address, mask and data */
    uint16_t nAddr = FLEXIO_CONTROL_NW_ADDR;
    uint32_t nMask = BITM_FLEXIO_CONTROL_AUTOMATIC_TRIGGER;
    uint32_t nData = ((uint32_t)1U << BITP_FLEXIO_CONTROL_AUTOMATIC_TRIGGER);

    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

/* Enable Time tag if enabled in configurations */
ADI_EAL_CODE
static ADI_EAL_STATUS EnableTimeTag(ADI_E2BCORE_ID *poId)
{
    /* Transaction config queue info */
    uint32_t nFioCtrlReg = 0U;
    bool bEnableTimeTag = false;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_OPEN_FAILED;

    if(eal_utils_getConfigInfRegVal(poId, FLEXIO_CONTROL_NW_ADDR, &nFioCtrlReg, false) == true)
    {
        eEalStatus = ADI_EAL_STATUS_SUCCESS;
    }
    else
    {
        /* Nothing to do */
    }

    if(eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        /*Check if time tag is enabled */
        bEnableTimeTag = ((nFioCtrlReg & BITM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG) != 0U);

        /* Fill the address, mask and data */
        uint16_t nAddr = FLEXIO_CONTROL_NW_ADDR;
        uint32_t nMask = BITM_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG;
        uint32_t nData = ((uint32_t)bEnableTimeTag << BITP_FLEXIO_CONTROL_ENABLE_READ_TIME_TAG);

        /* Set the register map message configurations */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        (void)memset(&oRegCfg, 0, sizeof(oRegCfg));
        oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
        oRegCfg.nLength = 1U;
        oRegCfg.anStartAddr = &nAddr;
        oRegCfg.anMask = &nMask;
        oRegCfg.pBuffer = &nData;

        /* Set the network frame configuration */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        uint32_t nVlanTag =
            gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
        eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

        /* Set the message options */
        ADI_E2BCORE_MSGOPTS oMsgOpts;
        (void) memset(&oMsgOpts, 0, sizeof(oMsgOpts));
        oMsgOpts.bSync = false;
        oMsgOpts.bTopicOverRide = true;
        oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

        /* Send the register map transaction */
        eEalStatus = eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
    }
    else
    {
        /* Nothing to do */
    }

    return eEalStatus;
}

/* Enable the FIO interface */
ADI_EAL_CODE
static ADI_EAL_STATUS EnableFlexibleIoInterface(ADI_E2BCORE_ID *poId)
{
    /* Fill the address, mask and data */
    uint16_t nAddr = FLEXIO_CONTROL_NW_ADDR;
    uint32_t nMask = BITM_FLEXIO_CONTROL_ENABLE;
    uint32_t nData = ((uint32_t)1U << BITP_FLEXIO_CONTROL_ENABLE);

    /* Set the register map message configurations */
    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    (void)ADI_MEMSET(&oRegCfg, 0, sizeof(oRegCfg));
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;
    oRegCfg.nLength = 1U;
    oRegCfg.anStartAddr = &nAddr;
    oRegCfg.anMask = &nMask;
    oRegCfg.pBuffer = &nData;

    /* Set the network frame configuration */
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;
    uint32_t nVlanTag =
        gaoEalData[poId->nInstNum].poE2bCoreCfg->aoRemoteCfgs[poId->nRemoteNum].aoIntfConfigs[poId->nIntfNum].nIfRegVlanTag;
    eal_utils_fillNetworkCfg(poId->nInstNum, poId->nRemoteNum, &oNwFrameCfg, &nVlanTag);

    /* Set the message options */
    ADI_E2BCORE_MSGOPTS oMsgOpts;
    (void) ADI_MEMSET(&oMsgOpts, 0, sizeof(oMsgOpts));
    oMsgOpts.bSync = false;
    oMsgOpts.bTopicOverRide = true;
    oMsgOpts.nTopic = ADI_E2BCORE_TOPIC_INTF0 + poId->nIntfNum;

    /* Send the register map transaction */
    return eal_utils_sendRegmapMessage(poId->nInstNum, &oNwFrameCfg, &oMsgOpts, &oRegCfg);
}

#endif /* ADI_E2B_IFACE_FIO_ENABLED */

/*! \endcond */
/** @} */

/*
 * EOF: www.analog.com
*/