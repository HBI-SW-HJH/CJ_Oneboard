/*******************************************************************************
Copyright (c) 2022-2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: network_eth.c
 * \brief: Implementation of ethernet devices of Network layer component
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup network
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "network_internal.h"

#ifdef ETH_EN
/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/

/*================================ PROTOTYPES ===============================*/

/*=================================== DATA ==================================*/

/*=================================== CODE ==================================*/
ADI_NETWORK_ETH_CODE
static void IncrementQIdx(volatile uint16_t *pnIdx, uint16_t nQSize)
{
    /* Increment the queue index by one, and wrap around if it exceeds the maximum queue size - used to manage the circular buffer */
    uint16_t nTempIdx = *pnIdx + 1U;
    *pnIdx = (nTempIdx >= nQSize) ? (nTempIdx - nQSize) : nTempIdx;
}

/*! Initializes ethernet module */
ADI_NETWORK_ETH_CODE
void network_eth_Init(void)
{
    // Nothing to do!
}

/*! Terminates ethernet module */
ADI_NETWORK_ETH_CODE
void network_eth_Terminate(void)
{
    // Nothing to do!
}

/*! Opens an ethernet device */
ADI_NETWORK_ETH_CODE
void network_eth_Open(uint32_t nDevNum)
{
    ADI_PAL_STATUS ePalStatus;
    /* Extract the device index for eth device and invoke the PAL api to open and initialise the ethernet device */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    ePalStatus = adi_pal_openEthAdapter(nDevIdx);
    /* Ensure that the returned PAL status is OK */
    ADI_DBG_ENSURE(ePalStatus == ADI_PAL_OK, "Unable to open desired eth adapter");
}

/*! Closes the ethernet device */
ADI_NETWORK_ETH_CODE
void network_eth_Close(uint32_t nDevNum)
{
    ADI_PAL_STATUS ePalStatus;
    /* Extract the device index for eth device and invoke the PAL api to close the ethernet device */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    ePalStatus = adi_pal_closeEthAdapter(nDevIdx);
    /* Ensure that the returned PAL status is OK */
    ADI_DBG_ENSURE(ePalStatus == ADI_PAL_OK, "Failed to close the network adapter");
}

/*! Periodic tick function */
ADI_NETWORK_ETH_CODE
void network_eth_Run(uint32_t nDevNum)
{
    /* Extract the device index for invoke the run function from PAL */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    adi_pal_ethRun(nDevIdx);
}

/*! Submits an ethernet frame for transmission */
ADI_NETWORK_ETH_CODE
#ifndef ADI_SERDES_MODE
void network_eth_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg)
#else
void network_eth_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg, uint8_t *pBuffer)
#endif
{
    ADI_PAL_STATUS ePalStatus;
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
#ifndef ADI_SERDES_MODE
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    uint16_t nWrIdx = goNwData.anEthWrIdx[nInstNum];
    uint8_t *pBuffer = &ganEthBuff[nInstNum][nWrIdx][0U];
#endif
    uint32_t nSize = 0U;

    /* Pad the frame to 64 bytes to make it minimum sized frame */
    if(poNwFrameCfg->nLenByte < ADI_NETWORK_ETH_FRAME_MIN_SIZE)
    {
        (void)ADI_MEMSET(&pBuffer[poNwFrameCfg->nLenByte], 0,
                         ((size_t)ADI_NETWORK_ETH_FRAME_MIN_SIZE - (size_t)poNwFrameCfg->nLenByte));
        poNwFrameCfg->nLenByte = ADI_NETWORK_ETH_FRAME_MIN_SIZE;
    }

    nSize = (uint32_t)poNwFrameCfg->nLenByte;

    /* Send the ethernet frame */
    ePalStatus = adi_pal_ethSend(nDevIdx, pBuffer, nSize, poNwFrameCfg->eTsReg != ADI_TS_EGRESS_NONE);
    /* Ensure that the pal status is OK */
    ADI_DBG_ENSURE(ePalStatus == ADI_PAL_OK, "Failed to submit TX frame");

#ifndef ADI_SERDES_MODE
    /* Mark the queue slot as used by eth device */
    gabTransmitIdxTaken[nWrIdx][nDevNum] = false;
#endif

    // if bandwidth measurement is enabled, update the statistics
    if(goNwData.aoDevInfo[nDevNum].bEnMeas)
    {
        goNwData.aoDevInfo[nDevNum].oMeas.nTxFrameCnt++;
        goNwData.aoDevInfo[nDevNum].oMeas.nTxBytesCnt += nSize;
    }
}

/*! Gets MAC statistics */
ADI_NETWORK_ETH_CODE
void network_eth_GetMacStatus(uint32_t nDevNum)
{
    /* Extract the device index for eth device and invoke the PAL api to get the MAC stats */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    adi_pal_getEthMacStats(nDevIdx);
}

/*! Gets PHY statistics */
ADI_NETWORK_ETH_CODE
void network_eth_GetPhyStatus(uint32_t nDevNum)
{
    /* Extract the device index for eth device and invoke the PAL api to get the PHY stats */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    adi_pal_getEthPhyStats(nDevIdx);
}

/*! Resets the MAC */
ADI_NETWORK_ETH_CODE
void network_eth_MacReset(uint32_t nDevNum)
{
    /* Extract the device index for eth device and invoke the PAL api to reset the MAC */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    adi_pal_ethMacReset(nDevIdx);
}

/*! Checks if TX buffers are empty */
ADI_NETWORK_ETH_CODE
bool network_eth_AreTxBuffersEmpty(uint32_t nDevNum)
{
    /* Extract the device index for eth device and invoke the PAL api to check if the TX buffer is empty */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    return adi_pal_isEthTxEmpty(nDevIdx);
}

/*! Sets the PLCA */
ADI_NETWORK_ETH_CODE
void network_eth_SetPlca(uint32_t nDevNum, const ADI_NETWORK_MACPHY_PLCA_CFG *poPlcaCfg)
{
    /* Extract the device index for eth device and invoke the PAL api to set PLCA */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    adi_pal_ethSetPlca(nDevIdx, poPlcaCfg->bEnabled, poPlcaCfg->nNodeCount, poPlcaCfg->anPLCAID[0U]);
}

/*! Sets the MAC filter table */
ADI_NETWORK_ETH_CODE
void network_eth_SetFilterTable(uint32_t nDevNum, uint8_t panMacAddr[][6u], uint8_t nNumAddr)
{
    /* Extract the device index for eth device and invoke the PAL api to set the filter table entries */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    adi_pal_ethSetFilterTable(nDevIdx, panMacAddr, nNumAddr);
}

/*! Clears the MAC filter table */
ADI_NETWORK_ETH_CODE
void network_eth_ClearFilterTable(uint32_t nDevNum)
{
    /* Extract the device index for eth device and invoke the PAL api to clear the filter table */
    uint32_t nDevIdx = goNwData.aoDevInfo[nDevNum].nId;
    adi_pal_ethClearFilterTable(nDevIdx);
}

#endif /* ETH_EN */

/** @} */

/**
 * EOF: www.analog.com
 */
