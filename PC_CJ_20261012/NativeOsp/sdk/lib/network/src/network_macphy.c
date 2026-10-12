/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: network_macphy.c
 * \brief: Implementation of MAC-PHY devices of Network layer component
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup network
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "network_internal.h"

#ifdef MACPHY_EN
/*================================= DEFINES =================================*/
#define LATEST_REV_NUM      (0x9U)
#define MACPHY_RESET_EN     (false)
#define FRAME_PADDING_BYTES (24ULL)   /*!< Preamble + SFD + Frame check seq + Inter packet gap */

/// TODO: Update the timeout values
#define WAITING_TO_SLEEP_TIMEOUT_NS         (200ULL  * MS_TO_NS)     /*!< Timeout for getting line busy error / sleep error interrupt for a goto_sleep command*/
#define WAITING_TO_STANDBY_TIMEOUT_NS       (100ULL  * MS_TO_NS)     /*!< Timeout for getting line busy error / standby error interrupt for a goto_standby command */
#define STANDBY_PING_RESP_TIMEOUT_NS        (100ULL  * MS_TO_NS)     /*!< Max time to wait for ping response (for confirmation that node has gone to standby) */
#define SLEEP_PING_RESP_TIMEOUT_NS          (100ULL  * MS_TO_NS)     /*!< Max time to wait for ping response (for confirmation that node has gone to sleep) */

#define NUM_OTP_BLOCKS      (61u)

/*================================ DATA TYPES ===============================*/
/*! Wake-sleep commands */
typedef enum WKSLP_CMDS
{
    WKSLP_CMD_STANDBY,      /*!< Set node to standby */
    WKSLP_CMD_SLEEP,        /*!< Set node to sleep */
    WKSLP_CMD_SEND_WUP,     /*!< Send Wake-up pulse */
} WKSLP_CMDS;

/*=================================== DATA ==================================*/
/*! OA-SPI configurations */
ADI_NETWORK_MACPHY_DATA
static ADI_OASPI_CFG gaoOaspiCfg[MAX_MACPHY_INST];

/*! OA-SPI register data */
ADI_NETWORK_MACPHY_DATA
static uint32_t ganRegData[NETWORK_CTRL_Q_SIZE][NETWORK_MAX_NUM_REGS];

/*! Masked Write data array */
ADI_NETWORK_MACPHY_DATA
static ADI_NETWORK_MASKED_WR_DATA gaoMaskedWrData[NETWORK_CTRL_Q_SIZE];

/*! Ctrl Descriptor array */
ADI_NETWORK_MACPHY_DATA
static ADI_OASPI_CTRL_DESC gaoOaspiCtrlDesc[NETWORK_CTRL_Q_SIZE];

/*! TX Data Descriptor array */
ADI_NETWORK_MACPHY_DATA
static ADI_OASPI_DATA_DESC gaoOaspiDataDesc[MAX_MACPHY_INST][NETWORK_DATA_Q_SIZE];

#ifdef ADI_NETWORK_PRIO_Q_EN
/*! TX HP Data Descriptor array */
ADI_NETWORK_MACPHY_DATA
static ADI_OASPI_DATA_DESC gaoOaspiDataDescHp[MAX_MACPHY_INST][NETWORK_DATA_HP_Q_SIZE];
#endif
#ifndef ADI_SERDES_MODE
/*! Stores the ethernet buffer index for OASPI data descriptors */
ADI_NETWORK_MACPHY_DATA
static uint32_t ganOaspiDataDescBufIdx[NETWORK_DATA_Q_SIZE];

#ifdef ADI_NETWORK_PRIO_Q_EN
/*! Stores the ethernet buffer index for OASPI HP data descriptors (bit 31 set = HP flag) */
ADI_NETWORK_MACPHY_DATA
static uint32_t ganOaspiDataDescBufIdxHp[NETWORK_DATA_HP_Q_SIZE];
#endif
#endif

/*! RX Data Descriptor array */
ADI_NETWORK_MACPHY_DATA
static ADI_OASPI_DATA_DESC gaoOaspiRxDesc[MAX_MACPHY_INST][NETWORK_RX_Q_SIZE];
/*================================ PROTOTYPES ===============================*/
static void IncrementQIdx(volatile uint16_t *pnIdx, uint16_t nQSize);
static uint32_t GetDevNumFromInstNum(uint32_t nInstNum);
static void HandleErrorCallback(uint32_t nDevNum, void *pData);
static void HandleMiscCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void OaspiCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleMaskedWrRegCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleStatsRegRead(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleTimeStampCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleWkSlpCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleSscCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleSqiCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleBootloaderCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleTicRegRead(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static void HandleTDRegCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam);
static bool ParseStatus0RegData(uint32_t nDevNum, uint32_t nInstNum, uint32_t nStatus0Data, bool *pbStatusErr);
static bool ParseStatus1RegData(uint32_t nDevNum, uint32_t nInstNum, uint32_t nStatus1Data, bool *pbStatusErr);
static ADI_NETWORK_ERR SendWkslpCommand(uint32_t nDevNum, WKSLP_CMDS eWkslpCmd);
static ADI_NETWORK_ERR PingNode(uint32_t nDevNum);
static ADI_NETWORK_ERR EnablePlca(uint32_t nDevNum);
static void ResetMacPhy(uint32_t nInstNum, uint32_t nDevNum);
static void EnableInterrupts(uint32_t nInstNum, uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg);
static void UpdateConfigRegisters(uint32_t nInstNum, uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg);
static ADI_NETWORK_ERR ConfigureNode(uint32_t nInstNum, uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg);
static void ControllerAwakeStateHandler(ADI_NETWORK_WKSLP_DATA *poWkslpData);
static void ControllerStandByStateHandler(uint32_t nDevNum, ADI_NETWORK_WKSLP_DATA *poWkslpData);
static void ControllerSleepStateHandler(uint32_t nDevNum, ADI_NETWORK_WKSLP_DATA *poWkslpData);
static void ControllerPingHandler(uint32_t nDevNum, ADI_NETWORK_WKSLP_DATA *poWkslpData);
static void WkSlp_Run(uint32_t nDevNum);
static void CheckSscStatus(uint32_t nDevNum);
static void CheckSqiStatus(uint32_t nDevNum);
/*=============================== STATIC CODE ===============================*/
ADI_NETWORK_MACPHY_CODE
static void IncrementQIdx(volatile uint16_t *pnIdx, uint16_t nQSize)
{
    /* Increment the queue index by one, and wrap around if it exceeds the maximum queue size - used to manage the circular buffer */
    uint16_t nTempIdx = *pnIdx + 1U;
    *pnIdx = (nTempIdx >= nQSize) ? (nTempIdx - nQSize) : nTempIdx;
}

/*! Get device number from instance number */
ADI_NETWORK_MACPHY_CODE
static uint32_t GetDevNumFromInstNum(uint32_t nInstNum)
{
    uint32_t nDevNum = 0xFFFFFFFFU;

    /* Loop through the devices discovered and return the device number
       that has the instance number index and device type as ADI_NETWORK_TYPE_MACPHY */
    for(uint32_t i = 0U; i < goNwData.nNumDevices; i++)
    {
        if(goNwData.aoDevInfo[i].eNwType == ADI_NETWORK_TYPE_MACPHY && goNwData.aoDevInfo[i].nCtrlIdx == nInstNum)
        {
            nDevNum = i;
            break; /* Break if the device number is found */
        }
    }

    /* Assert if the device is not corresponding to instance number */
    ADI_DBG_ENSURE(nDevNum != 0xFFFFFFFFU, "No mapping between device number and instance number");

    return nDevNum;
}

/*! OAPSI error handler function */
ADI_NETWORK_MACPHY_CODE
static void HandleErrorCallback(uint32_t nDevNum, void *pData)
{
    /* If the part is in reset sequence , just set the error state to true and don't give a callback */
    if(goNwData.aoDevInfo[nDevNum].bInResetSequence)
    {
        goNwData.aoDevInfo[nDevNum].bInErrState = true;
        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        goNwData.aoDevInfo[nDevNum].bInErrState = true;
        ADI_OASPI_EVT_ERR_DATA *poErrDat = (ADI_OASPI_EVT_ERR_DATA *)pData;

        switch(poErrDat->eError)
        {
            case ADI_OASPI_ERR_DATA_HDR_BAD:
            {
                // Give an error callback indicating header bad status
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_DATA_HDR_BAD;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
                break;
            }

            case ADI_OASPI_ERR_REG_WRITE_FAILED:
            {
                // Give an error callback indicating register write has failed - echoed control chunks didn't match with the sent control chunks for register write
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_REG_WRITE_FAILED;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);

                IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
                break;
            }

            case ADI_OASPI_ERR_REG_READ_FAILED:
            {
                // Give an error callback indicating register read has failed - either echoed control header didn't match or register read data in protected format is incorrect
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_REG_READ_FAILED;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);

                IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
                break;
            }

            case ADI_OASPI_ERR_PARITY_FAILED:
            {
                // Give an error callback indicating parity error in RX chunks
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_PARITY_ERR;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
                break;
            }

            case ADI_OASPI_ERR_RX_FRAME_DROP:
            {
                // Give an error callback indicating frame drop has been set in RX chunk footer
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_DROP_RX_FRAME;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
                break;
            }

            case ADI_OASPI_ERR_RX_FRAME_CRC_FAILED:
            {
                // Give an error callback indicating the CRC verification in the received frame has failed
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_RX_FRAME_CRC_FAILED;
                oErrDat.pData = poErrDat->pData;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
                break;
            }

            case ADI_OASPI_ERR_SYNC_LOST:
            {
                // Give an error callback indicating MAC-PHY sync has been lost and requires reconfiguration
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_SYNC_LOST;
                oErrDat.pData = poErrDat->pData;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
                break;
            }

            default:
            {
                /* Unexpected OASPI error type in HandleErrorCallback */
                ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected OASPI error type 0x%X in HandleErrorCallback", poErrDat->eError);
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
                oErrDat.pData = NULL;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
                break;
            }
        }
    }
}

/*! Callback function for miscellaneous register readbacks */
ADI_NETWORK_MACPHY_CODE
static void HandleMiscCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");
    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        /* Check if the address matches scratch register that stores the revision number */
        if(poDesc->nStartAddr == (uint16_t)MAC_SCRATCH_0_ADDR)
        {
            ADI_NETWORK_MACPHY_REV_NUM *poMacPhyRevNum = &goNwData.aoDevInfo[nDevNum].oRevNumber;
            poMacPhyRevNum->nMajorRevNum = poDesc->pRegData[0U];
            poMacPhyRevNum->nMinorRevNum = poDesc->pRegData[1U];

            if(goNwData.aoDevInfo[nDevNum].bInitComplete)
            {
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_REVISION_NUM, poMacPhyRevNum);
            }
            else
            {
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "MAC-PHY Major Revision Number: 0x" UINT32_HEX_FORMATTER ", Minor Revision Number: 0x"
                            UINT32_HEX_FORMATTER "\r\n", poMacPhyRevNum->nMajorRevNum, poMacPhyRevNum->nMinorRevNum);
            }
        }
        /* Check if the address matches GPTP config register */
        else if(poDesc->nStartAddr == (uint16_t)IO_GPTP_CONFIG_ADDR)
        {
            /* Mark the response as received */
            goNwData.aoDevInfo[nDevNum].bIsGPTPConfigRdRecv = true;
        }
        /* Check if the address matches config 0 register */
        else if(poDesc->nStartAddr == (uint16_t)MAC_CONFIG0_ADDR)
        {
            // The previous read was in init
            // The CONFIG0 register should have the correct data
            ADI_DBG_REQUIRE((poDesc->pRegData[0u] & ((uint32_t)(goNwData.paoNwMacPhyCfg[nInstNum]->eCPS) << BITP_MAC_CONFIG0_CPS))
                            != 0u,
                            "CONFIG0 register data is wrong");
            // ADI_DBG_REQUIRE((poDesc->pRegData[0u] & BITM_MAC_CONFIG0_FTSE) != 0u, "CONFIG0 register data is wrong");
            // ADI_DBG_REQUIRE((poDesc->pRegData[0u] & BITM_MAC_CONFIG0_FTSS) != 0u, "CONFIG0 register data is wrong");
            ADI_DBG_REQUIRE((poDesc->pRegData[0u] & BITM_MAC_CONFIG0_SYNC) != 0u, "CONFIG0 register data is wrong");

            //ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "CONFIG0 = 0x" UINT32_HEX_FORMATTER "\r\n", poDesc->pRegData[0U]);

            // Set the init complete flag
            goNwData.aoDevInfo[nDevNum].bInitComplete = true;
        }
        /* Check if the address matches the status registers */
        else if(poDesc->nStartAddr == (uint16_t)MAC_STATUS0_ADDR)
        {
            // The previous read was from Status registers
            // Process the data in the status registers and check if there is any error condition
            bool bErr = false;
            bool bQueueFull = ParseStatus0RegData(nDevNum, nInstNum, poDesc->pRegData[0U], &bErr);

            if(!bQueueFull)
            {
                bQueueFull = ParseStatus1RegData(nDevNum, nInstNum, poDesc->pRegData[1U], &bErr);
            }

            /* If the control queue was full, give a queue full error callback */
            if(bQueueFull)
            {
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_CTRL_Q_FULL;
                oErrDat.pData = NULL;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }

            // If there is an error set in status registers, give the error callback to application
            if(bErr)
            {
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_STATUS_ERR;
                oErrDat.pData = &poDesc->pRegData[0U];
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }
        }
        else
        {
            /* Unexpected register address in misc callback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleMiscCallback", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        HandleErrorCallback(nDevNum, pData);
    }
}

/*! Callback function to perform MAC-PHY register masked write at given bit postions with read data */
ADI_NETWORK_MACPHY_CODE
static void HandleMaskedWrRegCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    ADI_DBG_REQUIRE(pData != NULL, "Pointer to data is null");
    ADI_DBG_REQUIRE(pCbParam != NULL, "Pointer to callback parameter is null");

    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    switch(eEvent)
    {
        case ADI_OASPI_EVT_REG_READ:
        {
            uint8_t  nRegIdx = 0U;
            ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
            /* Get write data from descriptor's pCbParam */
            ADI_NETWORK_MASKED_WR_DATA *poMaskedWrData = (ADI_NETWORK_MASKED_WR_DATA *)pCbParam;

            /* Submit a new control descriptor for write request */
            ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nStartAddr   = poDesc->nStartAddr;
            poCtrlDesc->nLength      = poDesc->nLength;
            poCtrlDesc->nMMS         = poDesc->nMMS;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = OaspiCallback;

            for(nRegIdx = 0; nRegIdx < poDesc->nLength ; nRegIdx++)
            {
                /* Set control write register data with masked write data and read data */
                poCtrlDesc->pRegData[nRegIdx] = ((poDesc->pRegData[nRegIdx] & (~poMaskedWrData->anRegMapMask[nRegIdx])) |
                                                 (poMaskedWrData->anRegWrData[nRegIdx] & poMaskedWrData->anRegMapMask[nRegIdx]));
            }

            if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                /* Give an error callback to application indicating no space available in queue */
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_NO_SPACE;
                oErrDat.pData = (void *)poCtrlDesc;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);

            }

            break;
        }

        case ADI_OASPI_EVT_ERR:
        {
            /* Give an error callback indicating register read has failed */
            ADI_OASPI_EVT_ERR_DATA *poErrDat = (ADI_OASPI_EVT_ERR_DATA *)pData;
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_REG_READ_FAILED;
            oErrDat.pData = poErrDat->pData;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);

            break;
        }

        default:
        {
            /* Unexpected event in HandleMaskedWrRegCallback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected event 0x%X in HandleMaskedWrRegCallback", eEvent);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            break;
        }

    }

    /* Increment the CtrlRdIdx */
    IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);

}

/*! Callback from OA-SPI module */
ADI_NETWORK_MACPHY_CODE
static void OaspiCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    switch(eEvent)
    {
        case ADI_OASPI_EVT_TRANSMIT_CHUNKS:
        {
            /* The data to be transferred on the SPI are returned as OASPI chunks and ready to send */
            uint32_t nSpiId = goNwData.aoDevInfo[nDevNum].nId;
            ADI_OASPI_EVT_TRANSMIT_CHUNKS_DATA *poTrxData = (ADI_OASPI_EVT_TRANSMIT_CHUNKS_DATA *)pData;
            ADI_PAL_STATUS ePalStatus = adi_pal_spiSend(nSpiId, &poTrxData->pTxBuff[0U], &poTrxData->pRxBuff[0U],
                                        poTrxData->nSpiTrxLen);

            /* Check if PAL status is OK */
            if(ePalStatus != ADI_PAL_OK)
            {
                // Give an error callback
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_SPI_SEND_FAILED;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }

            break;
        }

        case ADI_OASPI_EVT_REG_READ:
        {
            ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;

            /* Give a callback for register read */
            ADI_NETWORK_EVT_READ_REG_DATA oRegData;
            oRegData.nLength = poDesc->nLength;
            oRegData.nMMS    = poDesc->nMMS;
            oRegData.nStartAddr = poDesc->nStartAddr;
            oRegData.pRegData = poDesc->pRegData;
            ADI_NETWORK_CBK pCbFunc = (ADI_NETWORK_CBK)pCbParam;
            pCbFunc(nDevNum, ADI_NETWORK_EVT_READ_REG, &oRegData);

            IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
            break;
        }

        case ADI_OASPI_EVT_REG_WRITE:
        {
            /* Give a callback for register write */
            ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
            ADI_NETWORK_EVT_WRITE_REG_DATA oRegData;
            oRegData.nLength = poDesc->nLength;
            oRegData.nMMS    = poDesc->nMMS;
            oRegData.nStartAddr = poDesc->nStartAddr;
            oRegData.pRegData = poDesc->pRegData;
            ADI_NETWORK_CBK pCbFunc = (ADI_NETWORK_CBK)pCbParam;
            pCbFunc(nDevNum, ADI_NETWORK_EVT_WRITE_REG, &oRegData);

            IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
            break;
        }

        case ADI_OASPI_EVT_TX_FRAME:
        {
            // Extract the TX frame descriptor
            ADI_OASPI_DATA_DESC *poDesc = (ADI_OASPI_DATA_DESC *)pData;

            // If Bandwidth measurement is enabled, update the TX frame count and TX bytes count
            if(goNwData.aoDevInfo[nDevNum].bEnMeas)
            {
                goNwData.aoDevInfo[nDevNum].oMeas.nTxFrameCnt++;
                goNwData.aoDevInfo[nDevNum].oMeas.nTxBytesCnt += (poDesc->nFrameSize + FRAME_PADDING_BYTES);
            }

            // Give a callback to the application for TX frame transmitted successfully
            ADI_NETWORK_EVT_TX_SUCCESS_DATA oTxDat;
            oTxDat.nLength = poDesc->nFrameSize;
            oTxDat.pRxData = poDesc->pFrameBuff;
            oTxDat.pTimeStamp = NULL;

            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TX_SUCCESS, &oTxDat);

            ADI_DBG_REQUIRE(prj_osal_lockMutex(ghNwTxIdxMutex) == PRJ_OSAL_STATUS_SUCCESS, "Failed to lock mutex");
#ifndef ADI_SERDES_MODE
            /* MISRA 11.3 deviation: pCbParam is always set to &ganOaspiDataDescBufIdx[Hp] (uint32_t) */
            uint32_t nRawBufIdx = *((uint32_t *)poDesc->pCbParam);

#ifdef ADI_NETWORK_PRIO_Q_EN

            /* Check HP flag (bit 31) to determine which pool to free */
            if((nRawBufIdx & 0x80000000U) != 0U)
            {
                /* HP pool: clear bit 31 to get actual index */
                uint32_t nBufIdx = nRawBufIdx & 0x7FFFFFFFU;

                if(nBufIdx < NETWORK_DATA_HP_Q_SIZE)
                {
                    gabTransmitIdxTakenHp[nBufIdx][nDevNum] = false;
                }
                else
                {
                    /* Nothing to do - defensive guard against invalid index */
                }
            }
            else
#endif /* ADI_NETWORK_PRIO_Q_EN */
            {
                /* LP pool */
                if(nRawBufIdx < NETWORK_DATA_Q_SIZE)
                {
                    gabTransmitIdxTaken[nRawBufIdx][nDevNum] = false;
                }
                else
                {
                    /* Nothing to do - defensive guard against invalid index */
                }
            }

#else
            IncrementQIdx(&goNwData.anDataDescRdIdx[nInstNum], NETWORK_DATA_Q_SIZE);
#endif
            ADI_DBG_REQUIRE(prj_osal_unlockMutex(ghNwTxIdxMutex) == PRJ_OSAL_STATUS_SUCCESS, "Failed to unlock mutex");

            break;
        }

        case ADI_OASPI_EVT_RX_FRAME:
        {
            ADI_OASPI_STATUS eOaspiStatus;
            // Extract the RX frame descriptor
            ADI_OASPI_DATA_DESC *poDesc = (ADI_OASPI_DATA_DESC *)pData;

            // If Bandwidth measurement is enabled, update the RX frame count and RX bytes count
            if(goNwData.aoDevInfo[nDevNum].bEnMeas)
            {
                goNwData.aoDevInfo[nDevNum].oMeas.nRxFrameCnt++;
                goNwData.aoDevInfo[nDevNum].oMeas.nRxBytesCnt += (poDesc->nFrameSize + FRAME_PADDING_BYTES);
            }

            // Create the RX structure and give a callback to the application
            ADI_NETWORK_EVT_ETH10_RX_RECEIVED_DATA oRxDat;
            ADI_TS_DATA oTsDat;
            oTsDat.nSec     = poDesc->nIngressTimestampSec;
            oTsDat.nNanoSec = poDesc->nIngressTimestampNanosec;
            oRxDat.nLength  = poDesc->nFrameSize;
            oRxDat.pRxData  = poDesc->pFrameBuff;
            oRxDat.pTimeStamp = &oTsDat;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ETH10_RX_RECEIVED, &oRxDat);
            IncrementQIdx(&goNwData.nRxRdIdx[nInstNum], NETWORK_RX_Q_SIZE);

            // Submit another descriptor
            poDesc = &gaoOaspiRxDesc[nInstNum][goNwData.nRxWrIdx[nInstNum]];
            eOaspiStatus = adi_oaspi_SubmitRxDesc(nInstNum, poDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nRxWrIdx[nInstNum], NETWORK_RX_Q_SIZE);
            }
            else
            {
                /* RX descriptor submission failed - give error callback */
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to submit RX descriptor - queue full");
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_CTRL_Q_FULL;
                oErrDat.pData = NULL;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }

            break;
        }

        case ADI_OASPI_EVT_ERR:
        {
            /* Handle error callbacks from OASPI */
            HandleErrorCallback(nDevNum, pData);
            break;
        }

        case ADI_OASPI_EVT_INT:
        {
            /* Incase of extended bit set in the SPI chunks, read the status registers for more information */
            ADI_OASPI_STATUS eOaspiStatus;
            ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nLength      = 2U;
            poCtrlDesc->nMMS         = MAC_STATUS0_MMS;
            poCtrlDesc->nStartAddr   = MAC_STATUS0_ADDR;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = HandleMiscCallback;
            eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                /* Status register read failed - interrupt will re-trigger on next SPI cycle */
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to read STATUS registers - control queue full");
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_CTRL_Q_FULL;
                oErrDat.pData = NULL;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }

            break;
        }

#if 0

        case ADI_OASPI_EVT_SOF:
        {
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SOF, NULL);
            break;
        }

#endif

        default:
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected OASPI event %d in OaspiCallback", eEvent);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            break;
        }
    }
}

/*! Callback function for register readbacks for MAC,PHY,PLCA,PCS and PTP stats */
ADI_NETWORK_MACPHY_CODE
static void HandleStatsRegRead(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");

    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        if(poDesc->nStartAddr == (uint16_t)MAC_RX_FRM_CNT_ADDR)
        {
            // The previous read was MAC status registers
            ADI_NETWORK_EVT_MAC_STATUS_DATA oStatusData;
            adi_common_extractMacStatus(poDesc->pRegData, &oStatusData);
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_MAC_STATUS, &oStatusData);
        }
        else if(poDesc->nStartAddr == (uint16_t)PHY_OA_PLCA_CTRL1_ADDR)
        {
            // The previous read was PLCA status registers
            ADI_NETWORK_EVT_PLCA_STATUS_DATA oStatusData;
            adi_common_extractPlcaStatus(poDesc->pRegData, &oStatusData);
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_PLCA_STATUS, &oStatusData);
        }
        else if(poDesc->nStartAddr == (uint16_t)PHY_STD_STATUS_REGISTER_ADDR)
        {
            // The previous read was PHY status register
            ADI_NETWORK_EVT_PHY_STATUS_DATA oStatusData;
            adi_common_extractPhyStatus(poDesc->pRegData, &oStatusData);
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_PHY_STATUS, &oStatusData);
        }
        else if(poDesc->nStartAddr == (uint16_t)PHY_STD_PCS_STATUS_ADDR)
        {
            // The previous read was PCS status registers
            ADI_NETWORK_EVT_PCS_STATUS_DATA oStatusData;
            adi_common_extractPcsStatus(poDesc->pRegData, &oStatusData);
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_PCS_STATUS, &oStatusData);
        }
        else if(poDesc->nStartAddr == (uint16_t)IO_GPTP_SYNC_COUNT_ADDR)
        {
            // The previous read was PTP status registers
            ADI_NETWORK_EVT_PTP_STATUS_DATA oStatusData;
            adi_common_extractPtpStatus(poDesc->pRegData, &oStatusData);
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_PTP_STATUS, &oStatusData);
        }
        else
        {
            /* Unexpected register address in HandleStatsRegRead */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleStatsRegRead", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        HandleErrorCallback(nDevNum, pData);
    }
}

/*! Callback function for register readbacks for Egress timestamps/ timestamp captured by TS_CAPT trigger */
ADI_NETWORK_MACPHY_CODE
static void HandleTimeStampCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");

    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    /* If the event is reg read, proceed to process read data */
    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        if(poDesc->nStartAddr == (uint16_t)MAC_TTSCAH_ADDR)
        {
            // The register read was Egress A timestamp registers
            ADI_NETWORK_EVT_TS_CAPTD_DATA oTsCaptdData;
            ADI_TS_DATA oTsData;
            oTsData.nSec = poDesc->pRegData[0U];
            oTsData.nNanoSec = poDesc->pRegData[1U];
            oTsCaptdData.eChannel = ADI_TS_EGRESS_A;
            oTsCaptdData.poTimestamp = &oTsData;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TS_CAPTD, &oTsCaptdData);
        }
        else if(poDesc->nStartAddr == (uint16_t)MAC_TTSCBH_ADDR)
        {
            // The register read was Egress B timestamp registers
            ADI_NETWORK_EVT_TS_CAPTD_DATA oTsCaptdData;
            ADI_TS_DATA oTsData;
            oTsData.nSec = poDesc->pRegData[0U];
            oTsData.nNanoSec = poDesc->pRegData[1U];
            oTsCaptdData.eChannel = ADI_TS_EGRESS_B;
            oTsCaptdData.poTimestamp = &oTsData;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TS_CAPTD, &oTsCaptdData);
        }
        else if(poDesc->nStartAddr == (uint16_t)MAC_TTSCCH_ADDR)
        {
            // The register read was Egress C timestamp registers
            ADI_NETWORK_EVT_TS_CAPTD_DATA oTsCaptdData;
            ADI_TS_DATA oTsData;
            oTsData.nSec = poDesc->pRegData[0U];
            oTsData.nNanoSec = poDesc->pRegData[1U];
            oTsCaptdData.eChannel = ADI_TS_EGRESS_C;
            oTsCaptdData.poTimestamp = &oTsData;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TS_CAPTD, &oTsCaptdData);
        }
        else if(poDesc->nStartAddr == (uint16_t)MAC_TS_EXT_CAPT0_ADDR)
        {
            // External Timestamp capture event - the register read was of the timestamp captured on asserting the TS_CAPT pin
            ADI_TS_DATA oExtTsData;
            oExtTsData.nNanoSec = poDesc->pRegData[0U];
            oExtTsData.nSec     = poDesc->pRegData[1U];
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TS_EXT_CAPT, &oExtTsData);
        }
        else
        {
            /* Unexpected register address in HandleTimeStampCallback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleTimeStampCallback", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        /* The event must be ADI_OASPI_EVT_ERR, handle it in error callback */
        HandleErrorCallback(nDevNum, pData);
    }
}

/*! Callback function readbacks of wakesleep registers */
ADI_NETWORK_MACPHY_CODE
static void HandleWkSlpCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");

    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        if(poDesc->nStartAddr == (uint16_t)IO_SLEEP_HISTORY_ADDR)
        {
            /* Get reason for sleep */
            uint32_t nReasonForSleep = (poDesc->pRegData[0U] & BITM_IO_SLEEP_HISTORY_REASON_FOR_SLEEP);
            goNwData.aoDevInfo[nDevNum].oData.eReasonForSleep = (ADI_REASON_FOR_SLEEP)nReasonForSleep;

            /* Get reason for wake */
            uint32_t nReasonForWake = (poDesc->pRegData[0U] & BITM_IO_SLEEP_HISTORY_REASON_FOR_WAKE) >>
                                      BITP_IO_SLEEP_HISTORY_REASON_FOR_WAKE;
            goNwData.aoDevInfo[nDevNum].oData.eReasonForWake = (ADI_REASON_FOR_WAKE)nReasonForWake;

            /* This check is done to ensure callback is made only when the app tries to get the reason for wake/sleep */
            if(!goNwData.aoDevInfo[nDevNum].bWkslpHistoryCheckEn)
            {
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_REASON_FOR_WKSLP, &goNwData.aoDevInfo[nDevNum].oData);
            }
            else
            {
                goNwData.aoDevInfo[nDevNum].bWkslpHistoryCheckEn = false;
            }
        }
        else if(poDesc->nStartAddr == (uint16_t)MAC_PHYID_ADDR)
        {
            /* Only update the bPingRcvd, if ping checking is enabled from wakesleep statemachine */
            if(goNwData.aoDevInfo[nDevNum].oWkslpData.bPingCheckEn == true)
            {
                goNwData.aoDevInfo[nDevNum].oWkslpData.bPingRcvd = true;
            }
            else
            {
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_PING_RESP, NULL);
            }
        }
        else
        {
            /* Unexpected register address in HandleWkSlpCallback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleWkSlpCallback", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else if(eEvent == ADI_OASPI_EVT_ERR)
    {
        ADI_OASPI_EVT_ERR_DATA *poErrDat = (ADI_OASPI_EVT_ERR_DATA *)pData;
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)poErrDat->pData;

        /* This check will make sure if the ping is done by wakesleep state machine and
           the ping check is enabled to only verify whether the node has successfully gone to sleep / standby state
           Else route the error to application whether it is a read for reason for wakesleep / ping response */
        if(goNwData.aoDevInfo[nDevNum].oWkslpData.bPingCheckEn == true && poDesc->nStartAddr == (uint16_t)MAC_PHYID_ADDR)
        {
            goNwData.aoDevInfo[nDevNum].oWkslpData.bPingRcvd = false;
            goNwData.aoDevInfo[nDevNum].oWkslpData.bOaspiErr = true;
            IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            HandleErrorCallback(nDevNum, pData);
        }
    }
    else
    {
        /* Unexpected event in HandleWkSlpCallback */
        ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected event 0x%X in HandleWkSlpCallback", eEvent);
        ADI_NETWORK_EVT_ERR_DATA oErrDat;
        oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
        oErrDat.pData = NULL;
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
    }
}

/*! Callback function readbacks of safe state controller registers */
ADI_NETWORK_MACPHY_CODE
static void HandleSscCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");
    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        if(poDesc->nStartAddr == (uint16_t)IO_SSC_SAFE_CHECKS_TRIGGER_ADDR)
        {
            // The previous read was safe state controller status
            ADI_NETWORK_EVT_SSC_STATUS_DATA oSscStatusData;
            adi_common_extractSscStatus(poDesc->pRegData, &oSscStatusData);
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SSC_STATUS, &oSscStatusData);
        }
        else if(poDesc->nStartAddr == (uint16_t)IO_SSC_ALIVE_COUNTER_RB_ADDR)
        {
            // The previous read was alive counter value
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SSC_ALIVE_COUNTER_RB, poDesc->pRegData);
        }
        else if(poDesc->nStartAddr == (uint16_t)IO_SSC_GENERAL_RB_ADDR)
        {
            // Safe mode check - to confirm whether the state is functional or safe
            ADI_NETWORK_SSC_DATA *poSscData =  &goNwData.aoDevInfo[nDevNum].oSscData;
            poSscData->bSafeMode = (poDesc->pRegData[0]  & BITM_IO_SSC_GENERAL_RB_SSC_MODE_RB) != 0U;

            /* Adding this check, so that return to functional state irq callback
            isn't issued without actually returning to functional from safe */
            if(!poSscData->bFuncMode)
            {
                poSscData->bReturnToFunc = (poDesc->pRegData[0]  & BITM_IO_SSC_GENERAL_RB_SSC_MODE_RB) == 0U;
            }
        }
        else
        {
            /* Unexpected register address in HandleSscCallback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleSscCallback", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        HandleErrorCallback(nDevNum, pData);
    }

}

/*! Callback function readbacks of SQI registers */
ADI_NETWORK_MACPHY_CODE
static void HandleSqiCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");
    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    /* If the event is reg read, proceed to process read data */
    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        if(poDesc->nStartAddr == (uint16_t)PHY_OA_DCQ_JM_RB0_ADDR)
        {
            // Unpause the jitter monitor
            ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nLength      = 1U;
            poCtrlDesc->nMMS         = PHY_OA_DCQ_JM_CFG0_MMS;
            poCtrlDesc->nStartAddr   = PHY_OA_DCQ_JM_CFG0_ADDR;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pRegData[0U] = ((uint32_t)goNwData.aoDevInfo[nDevNum].oSqiData.nSqiSelPlcaId <<
                                        BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_PLCA_ID_SEL) | BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN;
            poCtrlDesc->pfCallback   = OaspiCallback;
            ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to unpause the SQI integrator - control queue full");
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_CTRL_Q_FULL;
                oErrDat.pData = NULL;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }

            ADI_SQI_STATUS oSqiStatus;
            uint32_t nSqiLvl = ((poDesc->pRegData[4U] & BITM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX) >>
                                BITP_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX);
            oSqiStatus.bSqiValidFlag = ((poDesc->pRegData[0U] & BITM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID) >>
                                        BITP_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_VALID) == 1U ? true : false;
            oSqiStatus.bSqiSettleFlag = ((poDesc->pRegData[0U] & BITM_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED) >>
                                         BITP_PHY_OA_DCQ_JM_RB0_RB_JM_OUT_SETTLED)  == 1U ? true : false;
            oSqiStatus.eSqiLvl = (ADI_SQI_LVL)nSqiLvl;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SQI_STATUS, &oSqiStatus);
        }
        else if(poDesc->nStartAddr == (uint16_t)PHY_OA_DCQ_JM_RB4_ADDR)
        {
            // SQI index check - to confirm whether the level has come to good
            ADI_NETWORK_SQI_DATA  *poSqiData = &goNwData.aoDevInfo[nDevNum].oSqiData;

            /* Adding this check, so that return to good state irq callback
            isn't issued without actually returning to good from warning state */
            if(!poSqiData->bInGoodLvl)
            {
                poSqiData->bSQIGoodLvl = (((poDesc->pRegData[0]  & BITM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX) >>
                                           BITP_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX) == (uint32_t)ADI_SQI_LVL_GOOD) ? true : false;
            }

            poSqiData->bSQIWarningLvl = (((poDesc->pRegData[0] & BITM_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX) >>
                                          BITP_PHY_OA_DCQ_JM_RB4_RB_JM_QUALITY_INDEX) == (uint32_t)ADI_SQI_LVL_WARNING) ? true : false;
        }
        else
        {
            /* Unexpected register address in SQI callback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleSqiCallback", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        /* The event must be ADI_OASPI_EVT_ERR, handle it in error callback */
        HandleErrorCallback(nDevNum, pData);
    }
}

/*! Callback function readbacks of bootloader registers */
ADI_NETWORK_MACPHY_CODE
static void HandleBootloaderCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");
    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        if(poDesc->nStartAddr == (uint16_t)IO_HV_BOOTLOADER_STATUS_0_ADDR)
        {
            // Status of block 0
            goNwData.aoDevInfo[nDevNum].nBootloaderStatus[0u] = (uint8_t)ADI_NETWORK_BOOTLOADER_VALID;

            // Status of block 1
            goNwData.aoDevInfo[nDevNum].nBootloaderStatus[1u] = (uint8_t)(poDesc->pRegData[0u] & 0x3U);

            if(goNwData.aoDevInfo[nDevNum].nBootloaderStatus[1u] != (uint8_t)ADI_NETWORK_BOOTLOADER_CORRUPTED)
            {
                goNwData.aoDevInfo[nDevNum].nBootloaderStatus[1u] = (uint8_t)ADI_NETWORK_BOOTLOADER_VALID;
            }

            // Status of block 2
            goNwData.aoDevInfo[nDevNum].nBootloaderStatus[2u] = (uint8_t)((poDesc->pRegData[0u] & 0xCU) >> 2u);

            if(goNwData.aoDevInfo[nDevNum].nBootloaderStatus[2u] != (uint8_t)ADI_NETWORK_BOOTLOADER_CORRUPTED)
            {
                goNwData.aoDevInfo[nDevNum].nBootloaderStatus[2u] = (uint8_t)ADI_NETWORK_BOOTLOADER_VALID;
            }
        }
        else if(poDesc->nStartAddr == (uint16_t)IO_BOOTLOADER_STATUS_0_ADDR)
        {
            uint8_t nBootloaderStatusReg = 0U;
            uint8_t nBitfieldCnt = 0u;
            uint8_t nBlockIdx = 3u;

            for(uint8_t nBlockNum = 0U; nBlockNum < NUM_OTP_BLOCKS; nBlockNum++)
            {
                // 2 bit status per block, 8 blocks per register
                if(nBitfieldCnt == 8U)
                {
                    nBitfieldCnt = 0U;
                    nBootloaderStatusReg++;
                }

                // 2 bit status per block, 8 blocks per register
                goNwData.aoDevInfo[nDevNum].nBootloaderStatus[nBlockIdx] =
                    (uint8_t)((poDesc->pRegData[nBootloaderStatusReg] >> (2U * nBitfieldCnt)) & 0x3U);

                // Simplified status
                if(goNwData.aoDevInfo[nDevNum].nBootloaderStatus[nBlockIdx] != (uint8_t)ADI_NETWORK_BOOTLOADER_CORRUPTED)
                {
                    goNwData.aoDevInfo[nDevNum].nBootloaderStatus[nBlockIdx] = (uint8_t)ADI_NETWORK_BOOTLOADER_VALID;
                }

                nBitfieldCnt++;
                nBlockIdx++;
            }

            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_BOOTLOADER_STATUS, &goNwData.aoDevInfo[nDevNum].nBootloaderStatus[0u]);
        }
        else
        {
            /* Unexpected register address in HandleBootloaderCallback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleBootloaderCallback", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        HandleErrorCallback(nDevNum, pData);
    }
}

/*! Callback function for TIC register reads */
ADI_NETWORK_MACPHY_CODE
static void HandleTicRegRead(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    /* Preconditions */
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");
    /* Get the device number from instance number */
    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    /* If the event is reg read, proceed to process read data */
    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");

        /* Check if the address matches with the TIC status registers containing wake sleep interrupts and bootloader done information */
        if(poDesc->nStartAddr == (uint16_t)IO_TIC_STATUS_FLAGS_28_AND_29_ADDR)
        {
            // Extract wake-sleep IRQ(s) information
            ADI_NETWORK_WKSLP_DATA *poWkslpData = &goNwData.aoDevInfo[nDevNum].oWkslpData;
            poWkslpData->bConfirmAwakeInt = (poDesc->pRegData[0U] & CONFIRM_AWAKE_RISING_IRQ_MASK) != 0U;
            poWkslpData->bLineBusyInt = (poDesc->pRegData[0U] & LINE_BUSY_RISING_IRQ_MASK) != 0U;
            poWkslpData->bReqSysWakeupConfInt = (poDesc->pRegData[0U] & REQ_SYS_WK_CONF_RISING_IRQ_MASK) != 0U;
            poWkslpData->bReturnedFromStandbyInt = (poDesc->pRegData[0U] & RET_FROM_STDBY_RISING_IRQ_MASK) != 0U;
            poWkslpData->bSleepErrInt = (poDesc->pRegData[0U] & SLEEP_ERR_RISING_IRQ_MASK) != 0U;
            poWkslpData->bStandbyErrInt = (poDesc->pRegData[0U] & STANDBY_ERR_RISING_IRQ_MASK) != 0U;

            // Check if the bootloader done bit is set
            if((poDesc->pRegData[0U] & ((uint32_t)1U << ENUM_IO_TIC_STATUS_FLAGS_28_AND_29_TIC_STATUS_FLAGS_29_BOOTLOADER_DONE)) !=
                    0U)
            {
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_BOOTLOADER_DONE, NULL);
            }

            // Clear the interrupt(s)
            ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nLength      = 3U;
            poCtrlDesc->nMMS         = IO_TIC_CLEAR_28_MISC0_REG0_MMS;
            poCtrlDesc->nStartAddr   = IO_TIC_CLEAR_28_MISC0_REG0_ADDR;
            poCtrlDesc->pRegData[0U] = poDesc->pRegData[0U] & BITM_IO_TIC_STATUS_FLAGS_28_AND_29_TIC_STATUS_FLAGS_28;
            poCtrlDesc->pRegData[1U] = IO_TIC_CLEAR_28_MISC0_REG1_VOL_MASK;
            poCtrlDesc->pRegData[2U] = IO_TIC_CLEAR_29_MISC1_REG0_VOL_MASK;
            poCtrlDesc->pfCallback   = OaspiCallback;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear wake sleep/Bootloader IRQ(s) - control queue full");
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_CTRL_Q_FULL;
                oErrDat.pData = NULL;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }
        }
        /* Check if the address matches with the TIC status registers containing SSC/SQI interrupts information */
        else if(poDesc->nStartAddr == (uint16_t)IO_TIC_STATUS_FLAGS_34_AND_35_ADDR)
        {
            bool bTicQueueFull = false;

            // Clear the interrupt(s)
            ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nLength      = 1U;
            poCtrlDesc->nMMS         = IO_TIC_CLEAR_34_GPIO1_REG0_MMS;
            poCtrlDesc->nStartAddr   = IO_TIC_CLEAR_34_GPIO1_REG0_ADDR;
            poCtrlDesc->pRegData[0U] = IO_TIC_CLEAR_34_GPIO1_REG0_VOL_MASK;
            poCtrlDesc->pfCallback   = OaspiCallback;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear SSC/SQI IRQ(s) - control queue full");
                bTicQueueFull = true;
            }

            // Extract safe state controller IRQ(s),if it is enabled.
            if(!bTicQueueFull && goNwData.paoNwMacPhyCfg[nInstNum]->bEnableSscInt)
            {
                ADI_NETWORK_SSC_DATA *poSscData =  &goNwData.aoDevInfo[nDevNum].oSscData;
                poSscData->bUnsuccessfulreturnToFunc = (poDesc->pRegData[0] & SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_MASK) != 0U;
                // Readback the safe mode status to confirm the current state
                poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
                poCtrlDesc->nLength      = 1U;
                poCtrlDesc->nMMS         = IO_SSC_GENERAL_RB_MMS;
                poCtrlDesc->nStartAddr   = IO_SSC_GENERAL_RB_ADDR;
                poCtrlDesc->pCbParam     = goNwData.pfCbk;
                poCtrlDesc->pfCallback   = HandleSscCallback;
                eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

                if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
                {
                    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
                }
                else
                {
                    ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to readback the status register for SSC - control queue full");
                    bTicQueueFull = true;
                }
            }

            // Extract SQI IRQ(s),if it is enabled.
            if(!bTicQueueFull && goNwData.paoNwMacPhyCfg[nInstNum]->bEnableSqiInt)
            {
                ADI_NETWORK_SQI_DATA  *poSqiData = &goNwData.aoDevInfo[nDevNum].oSqiData;
                poSqiData->bSQIBadLvl = (poDesc->pRegData[0] & SQI_RISING_BAD_IRQ_MASK) != 0U;
                // Readback the sqi index to confirm the current level
                poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
                poCtrlDesc->nLength      = 1U;
                poCtrlDesc->nMMS         = PHY_OA_DCQ_JM_RB4_MMS;
                poCtrlDesc->nStartAddr   = PHY_OA_DCQ_JM_RB4_ADDR;
                poCtrlDesc->pCbParam     = goNwData.pfCbk;
                poCtrlDesc->pfCallback   = HandleSqiCallback;
                eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

                if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
                {
                    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
                }
                else
                {
                    ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to readback the SQI status - control queue full");
                    bTicQueueFull = true;
                }
            }

            /* If any submission to the control queue failed, give an error callback */
            if(bTicQueueFull)
            {
                ADI_NETWORK_EVT_ERR_DATA oErrDat;
                oErrDat.eErrType = ADI_NETWORK_MACPHY_CTRL_Q_FULL;
                oErrDat.pData = NULL;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
            }
        }
        else
        {
            /* Unexpected register address in TIC callback */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected register address 0x%X in HandleTicRegRead", poDesc->nStartAddr);
            ADI_NETWORK_EVT_ERR_DATA oErrDat;
            oErrDat.eErrType = ADI_NETWORK_MACPHY_UNEXPECTED_EVT;
            oErrDat.pData = NULL;
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
        }

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        /* The event must be ADI_OASPI_EVT_ERR, handle it in error callback */
        HandleErrorCallback(nDevNum, pData);
    }
}

/*! Callback function for topology discovery registers */
ADI_NETWORK_MACPHY_CODE
static void HandleTDRegCallback(uint32_t nInstNum, ADI_OASPI_EVT eEvent, void *pData, void *pCbParam)
{
    ADI_DBG_REQUIRE(eEvent == ADI_OASPI_EVT_REG_READ || eEvent == ADI_OASPI_EVT_ERR, "Unexpected event");

    uint32_t nDevNum = GetDevNumFromInstNum(nInstNum);

    if(eEvent == ADI_OASPI_EVT_REG_READ)
    {
        ADI_OASPI_CTRL_DESC *poDesc = (ADI_OASPI_CTRL_DESC *)pData;

        /* Preconditions - check whether the pointer to descriptor is valid and the register details for this callback is relavent */
        ADI_DBG_REQUIRE(poDesc != NULL, "Descriptor pointer is NULL");
        ADI_DBG_REQUIRE(poDesc->nStartAddr == (uint16_t)PHY_OA_TD_STAT_ADDR && poDesc->nMMS == (uint8_t)PHY_OA_TD_STAT_MMS
                        && poDesc->nLength == 5U, "Invalid descriptor for TD register read");

        do
        {
            /* Get the automatic mode status bit and check if there is any error */
            uint32_t nAutoErr = (poDesc->pRegData[0U] & BITM_PHY_OA_TD_STAT_TD_AUTO_ERR) >> BITP_PHY_OA_TD_STAT_TD_AUTO_ERR;

            if(nAutoErr == 1U)
            {
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TOPO_DISC_AUTO_MEAS_FAILED, NULL);
                break;
            }

            /* Get the delay result */
            if(goNwData.aoDevInfo[nDevNum].oTopoDiscData.bGetDelayResult)
            {
                goNwData.aoDevInfo[nDevNum].oTopoDiscData.bGetDelayResult = false;
                uint32_t nDelayMeasErr = (poDesc->pRegData[0U] & BITM_PHY_OA_TD_STAT_TD_DLYM_ERR) >> BITP_PHY_OA_TD_STAT_TD_DLYM_ERR;
                uint32_t nDelayMeasStatus = (poDesc->pRegData[0U] & BITM_PHY_OA_TD_STAT_TD_DLYM_DONE) >>
                                            BITP_PHY_OA_TD_STAT_TD_DLYM_DONE;

                /* If there is a delay measurement error / delay measurement is not done, flag it as delay measurement failure */
                if(nDelayMeasErr == 1U || nDelayMeasStatus != 1U)
                {
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TOPO_DISC_DELAY_FAILED, NULL);
                }
                /* Else calculate the delay */
                else
                {
                    uint32_t nDelayPulses = (poDesc->pRegData[3U] & BITM_PHY_OA_TD_DLY_RES_LOW_TD_DLYM_MR) |
                                            (poDesc->pRegData[4U] & BITM_PHY_OA_TD_DLY_RES_UP_TD_DLYM_MR) << BITL_PHY_OA_TD_DLY_RES_LOW_TD_DLYM_MR;
                    double nDelay = (((double)goNwData.aoDevInfo[nDevNum].oTopoDiscData.nTopoMeasDuration + 1.0f) * (double)MS_TO_NS) /
                                    (double)nDelayPulses;
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TOPO_DISC_DELAY_MEAS, &nDelay);
                }
            }
            /* Get the distance result */
            else if(goNwData.aoDevInfo[nDevNum].oTopoDiscData.bGetDistanceResult)
            {
                goNwData.aoDevInfo[nDevNum].oTopoDiscData.bGetDistanceResult = false;
                uint32_t nDistMeasErr = (poDesc->pRegData[0U] & BITM_PHY_OA_TD_STAT_TD_DM_ERR) >> BITP_PHY_OA_TD_STAT_TD_DM_ERR;
                uint32_t nDistMeasStatus = (poDesc->pRegData[0U] & BITM_PHY_OA_TD_STAT_TD_DM_DONE) >> BITP_PHY_OA_TD_STAT_TD_DM_DONE;

                /* If there is a distance measurement error / distance measurement is not done, flag it as distance measurement failure */
                if(nDistMeasErr == 1U || nDistMeasStatus != 1U)
                {
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TOPO_DISC_DIST_FAILED, NULL);
                }
                /* Else calculate the distance */
                else
                {
                    uint32_t nDistMeasPulses = (poDesc->pRegData[1U] & BITM_PHY_OA_TD_DIST_RES_LOW_TD_DIST_MR) |
                                               (poDesc->pRegData[2U] & BITM_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR) << BITL_PHY_OA_TD_DIST_RES_UP_TD_DIST_MR;
                    double nDistance = (((double)goNwData.aoDevInfo[nDevNum].oTopoDiscData.nTopoMeasDuration + 1.0f) * (double)MS_TO_NS) /
                                       (double)nDistMeasPulses;
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_TOPO_DISC_DIST_MEAS,
                                   &nDistance); /* Note: This distance is absolute register value translation - the actual distance can be computed by dividing by 2 and subtracting their internal delays */
                }
            }
            else
            {
                // Nothing to do
            }
        } while(false);

        IncrementQIdx(&goNwData.nCtrlRdIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        HandleErrorCallback(nDevNum, pData);
    }
}

/*! Parser function for STATUS0 register
 * @return true if queue full error occurred, false otherwise */
ADI_NETWORK_MACPHY_CODE
static bool ParseStatus0RegData(uint32_t nDevNum, uint32_t nInstNum, uint32_t nStatus0Data, bool *pbStatusErr)
{
    /* OASPI operation status */
    ADI_OASPI_STATUS eOaspiStatus;
    /* Queue full error flag */
    bool bQueueFull = false;

    /* ==================== Parse STATUS0 register for error conditions ==================== */
    /* If one of the following errors are flagged, MACPHY is in an errored state
       Errors that are checked are:
       - Transmit protocol error
       - Host TX FIFO overflow error
       - Host TX FIFO underrun error
       - Receive buffer overflow error
       - Loss of frame error
       - Header error
       - Control data protection error
       - Transmit frame check sequence error */
    uint32_t nStatus0Error = BITM_MAC_STATUS0_TXPE | BITM_MAC_STATUS0_TXBOE | BITM_MAC_STATUS0_TXBUE |
                             BITM_MAC_STATUS0_RXBOE | BITM_MAC_STATUS0_LOFE | BITM_MAC_STATUS0_HDRE |
                             BITM_MAC_STATUS0_CDPE | BITM_MAC_STATUS0_TXFCSE;

    /* Check if any error bits are set */
    if(((nStatus0Data) & nStatus0Error) != 0u)
    {
        /* Set the error flag to indicate MACPHY error state */
        *pbStatusErr = true;
    }

    /* ==================== Handle Reset Complete Bit ==================== */
    /* If reset complete bit is set, clear it */
    if(((nStatus0Data) & (uint32_t)BITM_MAC_STATUS0_RESETC) != 0u)
    {
        /* Get control descriptor from queue */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        /* Configure single register write */
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_STATUS0_MMS;
        poCtrlDesc->nStartAddr   = MAC_STATUS0_ADDR;
        poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS0_RESETC;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        /* Write to clear reset complete bit */
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        /* Check if write operation succeeded */
        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            /* Increment the control write index */
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            /* Write failed - control queue is full */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear reset complete bit - control queue full");
            bQueueFull = true;
        }
    }

    /* ==================== Handle Timestamp Capture Available A ==================== */
    /* If transmit timestamp capture available A is set, clear the status bit and read back the egress timestamp from register A */
    if(!bQueueFull && (((nStatus0Data) & (uint32_t)BITM_MAC_STATUS0_TTSCAA) != 0u))
    {
        /* Step 1: Clear TTSCAA status bit */
        /* Get control descriptor from queue */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        /* Configure descriptor for STATUS0 write */
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_STATUS0_MMS;
        poCtrlDesc->nStartAddr   = MAC_STATUS0_ADDR;
        poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS0_TTSCAA;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        /* Write to clear timestamp capture available bit */
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        /* Check if clear operation succeeded */
        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            /* Advance control queue write index */
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            /* Queue full - cannot clear bit */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear TTSCAA bit - control queue full");
            bQueueFull = true;
        }

        /* Step 2: Read timestamp capture register A if clear succeeded */
        if(!bQueueFull)
        {
            /* Prepare descriptor to read timestamp registers */
            poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            /* Read 2 registers (high and low timestamp bytes) */
            poCtrlDesc->nLength      = 2U;
            poCtrlDesc->nMMS         = MAC_TTSCAH_MMS;
            poCtrlDesc->nStartAddr   = MAC_TTSCAH_ADDR;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = HandleTimeStampCallback;
            /* Read timestamp capture register A */
            eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to read TTSCA registers - control queue full");
                bQueueFull = true;
            }
        }
    }

    /* If transmit timestamp capture available B is set, clear the status bit and read back the egress timestamp from register B */
    if(!bQueueFull && (((nStatus0Data) & (uint32_t)BITM_MAC_STATUS0_TTSCAB) != 0u))
    {
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_STATUS0_MMS;
        poCtrlDesc->nStartAddr   = MAC_STATUS0_ADDR;
        poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS0_TTSCAB;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear TTSCAB bit - control queue full");
            bQueueFull = true;
        }

        if(!bQueueFull)
        {
            poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nLength      = 2U;
            poCtrlDesc->nMMS         = MAC_TTSCBH_MMS;
            poCtrlDesc->nStartAddr   = MAC_TTSCBH_ADDR;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = HandleTimeStampCallback;
            eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to read TTSCB registers - control queue full");
                bQueueFull = true;
            }
        }
    }

    /* If transmit timestamp capture available C is set, clear the status bit and read back the egress timestamp from register C */
    if(!bQueueFull && (((nStatus0Data) & (uint32_t)BITM_MAC_STATUS0_TTSCAC) != 0u))
    {
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_STATUS0_MMS;
        poCtrlDesc->nStartAddr   = MAC_STATUS0_ADDR;
        poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS0_TTSCAC;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear TTSCAC bit - control queue full");
            bQueueFull = true;
        }

        if(!bQueueFull)
        {
            poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nLength      = 2U;
            poCtrlDesc->nMMS         = MAC_TTSCCH_MMS;
            poCtrlDesc->nStartAddr   = MAC_TTSCCH_ADDR;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = HandleTimeStampCallback;
            eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to read TTSCC registers - control queue full");
                bQueueFull = true;
            }
        }
    }

    return bQueueFull;
}

/*! Parser function for STATUS1 register
 * @return true if queue full error occurred, false otherwise */
ADI_NETWORK_MACPHY_CODE
static bool ParseStatus1RegData(uint32_t nDevNum, uint32_t nInstNum, uint32_t nStatus1Data, bool *pbStatusErr)
{
    /* OASPI operation status */
    ADI_OASPI_STATUS eOaspiStatus;
    /* Queue full error flag */
    bool bQueueFull = false;

    /* ==================== Parse STATUS1 register for interrupt statuses ==================== */

    /* ==================== Handle Vendor Specific Interrupt ==================== */
    /* Vendor specific interrupt triggered.
       Need to read the transaction interrupt controller registers */
    if(((nStatus1Data) & (uint32_t)BITM_MAC_STATUS1_VS_INT) != 0U)
    {
        /* Step 1: Clear vendor specific interrupt bit */
        /* Get control descriptor from queue */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        /* Configure descriptor for STATUS1 write */
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_STATUS1_MMS;
        poCtrlDesc->nStartAddr   = MAC_STATUS1_ADDR;
        poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS1_VS_INT;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        /* Write to clear VS interrupt bit */
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        /* Check if clear operation succeeded */
        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            /* Advance control queue write index */
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            /* Queue full - cannot clear bit */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear VS_INT bit - control queue full");
            bQueueFull = true;
        }

        /* Step 2: Read TIC status registers for wake-sleep interrupts */
        /* Read the TIC status registers containing the Wakesleep interrupts information */
        if(!bQueueFull)
        {
            /* Prepare descriptor for TIC status read */
            poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            /* Configure for reading wake-sleep interrupt flags */
            poCtrlDesc->nLength      = 1U;
            poCtrlDesc->nMMS         = IO_TIC_STATUS_FLAGS_28_AND_29_MMS;
            poCtrlDesc->nStartAddr   = IO_TIC_STATUS_FLAGS_28_AND_29_ADDR;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = HandleTicRegRead;
            /* Read wake-sleep interrupt status */
            eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

            /* Check if read operation succeeded */
            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                /* Advance control queue write index */
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                /* Queue full - cannot read */
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to read wake-sleep interrupt registers - control queue full");
                bQueueFull = true;
            }
        }

        /* Step 3: Read TIC status registers for SSC/SQI if enabled */
        /* If SSC/SQI interrupts are enabled, then read the TIC status registers associated with it */
        if(!bQueueFull &&
                (goNwData.paoNwMacPhyCfg[nInstNum]->bEnableSscInt || goNwData.paoNwMacPhyCfg[nInstNum]->bEnableSqiInt))
        {
            /* Prepare descriptor for SSC/SQI status read */
            poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            /* Configure for reading SSC/SQI interrupt flags */
            poCtrlDesc->nLength      = 1U;
            poCtrlDesc->nMMS         = IO_TIC_STATUS_FLAGS_34_AND_35_MMS;
            poCtrlDesc->nStartAddr   = IO_TIC_STATUS_FLAGS_34_AND_35_ADDR;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = HandleTicRegRead;
            /* Read SSC/SQI interrupt status */
            eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

            /* Check if read operation succeeded */
            if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
            {
                /* Advance control queue write index */
                IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            }
            else
            {
                /* Queue full - cannot read */
                ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to read SSC and SQI interrupt registers - control queue full");
                bQueueFull = true;
            }
        }

        /* Step 4: Handle application-configured vendor specific interrupt */
        /* Check if the application configured VS interrupt has been asserted */
        if(goNwData.paoNwMacPhyCfg[nInstNum]->bEnableAppInt)
        {
            /* Give a callback for application configured VS interrupt */
            goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_APP_VS_INT, NULL);
        }

    }

    /* ==================== Parse STATUS1 register for error conditions ==================== */
    /* If one of the following errors are flagged, MACPHY is in an errored state
       Errors that are checked are
       - Rx MAC inter frame gap error
       - ECC error on reading the frame size from an Rx FIFO
       - ECC error on reading the frame size from an Tx FIFO
       - Transmit max collision retry reached
       - Read access error
       - Link status changed */
    uint32_t nStatus1Error = BITM_MAC_STATUS1_RX_IFG_ERR | BITM_MAC_STATUS1_MAX_RR_ERR |
                             BITM_MAC_STATUS1_RX_ECC_ERR | BITM_MAC_STATUS1_TX_ECC_ERR | BITM_MAC_STATUS1_RD_ACC_ERR;

    if(((nStatus1Data) & nStatus1Error) != 0U)
    {
        // Set the error flag
        *pbStatusErr = true;
    }

    if(!bQueueFull && (((nStatus1Data) & (uint32_t)BITM_MAC_STATUS1_TX_RDY) != 0u))
    {
        /* TX ready, clear the status bit */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_STATUS1_MMS;
        poCtrlDesc->nStartAddr   = MAC_STATUS1_ADDR;
        poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS1_TX_RDY;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear TX ready bit - control queue full");
            bQueueFull = true;
        }
    }

    if(!bQueueFull && (((nStatus1Data) & (uint32_t)BITM_MAC_STATUS1_RX_RDY) != 0u))
    {
        /* P1 RX ready, clear the status bit */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_STATUS1_MMS;
        poCtrlDesc->nStartAddr   = MAC_STATUS1_ADDR;
        poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS1_RX_RDY;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
        else
        {
            ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to clear RX ready bit - control queue full");
            bQueueFull = true;
        }
    }

    return bQueueFull;
}

ADI_NETWORK_MACPHY_CODE
static ADI_NETWORK_ERR SendWkslpCommand(uint32_t nDevNum, WKSLP_CMDS eWkslpCmd)
{
    uint32_t nRegData;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    ADI_OASPI_STATUS eOaspiStatus = ADI_OASPI_STATUS_SUCCESS;
    ADI_OASPI_CTRL_DESC *poCtrlDesc;

    if(eWkslpCmd == WKSLP_CMD_SLEEP)
    {
        // Set the register data to go to sleep mode
        nRegData = BITM_IO_SLEEPWAKE_CMD_SLPWK_CMD_GOTO_SLEEP;
    }
    else if(eWkslpCmd == WKSLP_CMD_STANDBY)
    {
        // Disable backdoor mode before sending standby command
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = 0xAU;
        poCtrlDesc->nStartAddr   = 0xC849U;
        poCtrlDesc->pRegData[0U] = 0xA000U;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        /* Check if status is success, to increment queue index */
        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_OK;
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }

        // Set the register data to go to standby mode
        nRegData = BITM_IO_SLEEPWAKE_CMD_SLPWK_CMD_GOTO_STANDBY;
    }
    else if(eWkslpCmd == WKSLP_CMD_SEND_WUP)
    {
        // Set the register data to send wake up pulse on the network
        nRegData = BITM_IO_SLEEPWAKE_CMD_SLPWK_CMD_SEND_WUS;
    }
    else
    {
        /* Invalid command */
        ADI_DBG_LOG(LOG_ERROR, stderr, "Invalid wake/sleep command: %d", eWkslpCmd);
        eOaspiStatus = ADI_OASPI_STATUS_UNK_ERROR;
    }

    // Send the wake sleep command to MACPHY
    if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
    {
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = IO_SLEEPWAKE_CMD_MMS;
        poCtrlDesc->nStartAddr   = IO_SLEEPWAKE_CMD_ADDR;
        poCtrlDesc->pRegData[0U] = nRegData;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    }

    /* Check if status is success, to increment queue index */
    if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
    {
        eNwErr = ADI_NETWORK_OK;
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }

    return eNwErr;
}

ADI_NETWORK_MACPHY_CODE
static ADI_NETWORK_ERR PingNode(uint32_t nDevNum)
{
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;

    // Read the PHY ID register as a ping test
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_PHYID_MMS;
    poCtrlDesc->nStartAddr   = MAC_PHYID_ADDR;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleWkSlpCallback; // Readback is handled in wakesleep callback
    ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

    // Check if the status is success, to increment the queue index
    if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
    {
        eNwErr = ADI_NETWORK_OK;
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }

    return eNwErr;
}

ADI_NETWORK_MACPHY_CODE
static ADI_NETWORK_ERR EnablePlca(uint32_t nDevNum)
{
    /* Get the instance number for the given device number */
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /* Set the network error to be returned as OK initially */
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        /* Enable PLCA */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_PLCA_CTRL0_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = PHY_OA_PLCA_CTRL0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = goNwData.aoDevInfo[nDevNum].bIsPlcaEn == true ? BITM_PHY_OA_PLCA_CTRL0_PLCAEN : 0U;

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* If PLCA enable is set , erase the copy of enable bit from ao regmap
        so that it doesn't flood the network with beacons before the whole network wakes up */
        if(goNwData.aoDevInfo[nDevNum].bIsPlcaEn)
        {
            /* Clearing the PLCA enable bit copy from ao regmap */
            poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nStartAddr   = 0xB70D;
            poCtrlDesc->nLength      = 1U;
            poCtrlDesc->nMMS         = 0xAu;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = OaspiCallback;
            poCtrlDesc->pRegData[0U] = 0U;

            // Check if the status is success, to increment the queue index
            if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
            {
                eNwErr = ADI_NETWORK_NO_SPACE;
                break;
            }

            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
    } while(false);

    return eNwErr;
}

ADI_NETWORK_MACPHY_CODE
static void ResetMacPhy(uint32_t nInstNum, uint32_t nDevNum)
{
    goNwData.aoDevInfo[nDevNum].bWkslpHistoryCheckEn = true;
    ADI_OASPI_CTRL_DESC *poCtrlDesc;
    /* Reading the wakesleep history to perform a register write after LV die reset */
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_SLEEP_HISTORY_MMS;
    poCtrlDesc->nStartAddr   = IO_SLEEP_HISTORY_ADDR;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    /* Callback function for this OASPI to notify about the register read data */
    poCtrlDesc->pfCallback   = HandleWkSlpCallback;
    ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);
    // Check if the status is success, to increment the queue index
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to read wake sleep history register");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    /* Perform resets for the MACPHY */

    goNwData.aoDevInfo[nDevNum].bInResetSequence = true;
    /* Reset LV IO - Causes Bootloader to Run! */
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_RESET_CONTROL_MMS;
    poCtrlDesc->nStartAddr   = IO_RESET_CONTROL_ADDR;
    poCtrlDesc->pRegData[0U] = 0xA55AU;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed : Reset LV IO ");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    /* NOTE:
        Flush the buffers out to complete the above write.
        Since this is a full reset, the OA-SPI comms might fail.
        The error-flag might be set in the OaspiCallback and must be cleared here. */
    adi_network_flushTxBuffer(nDevNum, 0ULL);
    goNwData.aoDevInfo[nDevNum].bInErrState = false;
    goNwData.aoDevInfo[nDevNum].bInResetSequence = false;

    /* Delay 1.2ms while bootloader runs */
    adi_network_flushTxTimed(1200ULL * US_TO_NS);

    // ~~~~~~~~~~~~ Inits ~~~~~~~~~~~~
    // Clear reset complete status and LOFE status bit also, since the LV die reset will also cause LOFE to be set
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_STATUS0_MMS;
    poCtrlDesc->nStartAddr   = MAC_STATUS0_ADDR;
    poCtrlDesc->pRegData[0U] = BITM_MAC_STATUS0_RESETC | BITM_MAC_STATUS0_LOFE;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to reset MAC-PHY");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_SLEEP_HISTORY_MMS;
    poCtrlDesc->nStartAddr   = IO_SLEEP_HISTORY_ADDR;
    poCtrlDesc->pRegData[0U] = (uint32_t)(goNwData.aoDevInfo[nDevNum].oData.eReasonForSleep) |
                               ((uint32_t)(goNwData.aoDevInfo[nDevNum].oData.eReasonForWake) << BITP_IO_SLEEP_HISTORY_REASON_FOR_WAKE);
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to write sleep history register");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
}

ADI_NETWORK_MACPHY_CODE
static void EnableInterrupts(uint32_t nInstNum, uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg)
{
    // Enable interrupts for the MACPHY
    ADI_OASPI_CTRL_DESC *poCtrlDesc;
    ADI_OASPI_STATUS eOaspiStatus;

    // Unmask critical interrupts
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_IMASK0_MMS;
    poCtrlDesc->nStartAddr   = MAC_IMASK0_ADDR;
    poCtrlDesc->pRegData[0U] = poNwMacPhyCfg->bEnableRxBufferOverflowInt == false ? BITM_MAC_IMASK0_RXBOEM :
                               (uint32_t)0U; // Mask RX buffer overflow interrupt if the user has not enabled it in the MACPHY configuration
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to unmask critical interrupts");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    // Disable sending interrupts over T1S
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_TIC_CONTROL1_MMS;
    poCtrlDesc->nStartAddr   = IO_TIC_CONTROL1_ADDR;
    poCtrlDesc->pRegData[0U] = (IO_TIC_CONTROL1_RESET_VAL & BITM_IO_TIC_CONTROL1_TIC_RESEND_PERIOD) |
                               BITM_IO_TIC_CONTROL1_TIC_EBTP_INT_DISABLE;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to enable oaspi interrupt triggered through tic");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    // Enabling safe state controller and SQI interrupts, if the given macphy configuration has enabled them
    if(poNwMacPhyCfg->bEnableSscInt || poNwMacPhyCfg->bEnableSqiInt)
    {
        goNwData.aoDevInfo[nDevNum].oSqiData.bInGoodLvl = true; // SQI is in good level by default
        goNwData.aoDevInfo[nDevNum].oSscData.bFuncMode  = true; // SSC is in functional mode by default

        // For SSC, enable rising and falling interrupts for mode change and unsuccessful return to function
        uint32_t nEnSscInt = (uint32_t)(SSC_MODE_CHANGE_FLAG_RISING_IRQ_TIC_FIELD_MASK |
                                        SSC_MODE_CHANGE_FLAG_FALLING_IRQ_TIC_FIELD_MASK |
                                        SSC_UNSUCCESSFUL_RETURN_FUNC_RISING_IRQ_TIC_FIELD_MASK);

        // For SQI, enable rising and falling interrupts for bad and warning levels
        uint32_t nEnSqiInt = (uint32_t)(SQI_RISING_WARNING_IRQ_TIC_FIELD_MASK |
                                        SQI_RISING_BAD_IRQ_TIC_FIELD_MASK |
                                        SQI_FALLING_WARNING_IRQ_TIC_FIELD_MASK);
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = IO_TIC_ENABLE_34_GPIO1_REG0_MMS;
        poCtrlDesc->nStartAddr   = IO_TIC_ENABLE_34_GPIO1_REG0_ADDR;
        poCtrlDesc->pRegData[0U] = (poNwMacPhyCfg->bEnableSscInt ? nEnSscInt : 0U) |
                                   (poNwMacPhyCfg->bEnableSqiInt ? nEnSqiInt : 0U);
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
        ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to enable SQI and SSC interrupts");
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }

    // Unmask VS_INT (vendor specific interrupt)
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_IMASK1_MMS;
    poCtrlDesc->nStartAddr   = MAC_IMASK1_ADDR;
    poCtrlDesc->pRegData[0U] = MAC_IMASK1_RESET_VAL & ~BITM_MAC_IMASK1_VS_INT_MASK;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to unmask VS interrupt");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
}

ADI_NETWORK_MACPHY_CODE
static void UpdateConfigRegisters(uint32_t nInstNum, uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg)
{
    ADI_OASPI_CTRL_DESC *poCtrlDesc;
    ADI_OASPI_STATUS eOaspiStatus;

    /* Default value of TX credit threshold when no priority FIFOs are enabled */
    uint32_t nTxcThresh = ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_8;

    /* Update CONFIG2 register - fields that are updated as per init are forward frames that are not matching to any mac address to controller, HOST_CRC_APPEND, TX_RDY_ON_EMPTY, LO_PRIO_FIFO_CRC_APPEND
       Note : when BITM_MAC_CONFIG2_FWD_UNK2HOST is set, the effect of filter table for HOST is null */
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_CONFIG2_MMS;
    poCtrlDesc->nStartAddr   = MAC_CONFIG2_ADDR;
    poCtrlDesc->pRegData[0U] =  0x7C2000U /* Hardcoded based on reset value */ |
                                (poNwMacPhyCfg->bAppendCrcbyHost == false ? (BITM_MAC_CONFIG2_HOST_CRC_APPEND |
                                    BITM_MAC_CONFIG2_LO_PRIO_FIFO_CRC_APPEND) : 0U) |
                                BITM_MAC_CONFIG2_FWD_UNK2HOST | BITM_MAC_CONFIG2_TX_RDY_ON_EMPTY;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to update CONFIG2 reg");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    /* Update nTxcThresh when priority FIFOs are enabled */
    if(poNwMacPhyCfg->bEnablePrioFifos)
    {
        /* When priority is enabled and 64 byte CPS is selected the default of TX credit threshold is 8, if CPS is 32 byte the default is 16 */
        if(poNwMacPhyCfg->eCPS == ADI_NETWORK_CPS_32BYTES)
        {
            nTxcThresh = ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_16;
        }
    }

    /* Update CONFIG0 register - fields that are updated as per init are chunk payload size, TX credit threshold, RX and TX cut through mode, frame timestamp enable and TX FCS validation
       Frame timestamp select is always set to 1 for 64-bit timestamps
       sync bit is also set to notify MAC_PHY that the configuration is done */
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_CONFIG0_MMS;
    poCtrlDesc->nStartAddr   = MAC_CONFIG0_ADDR;
    poCtrlDesc->pRegData[0U] = ((uint32_t)poNwMacPhyCfg->eCPS   << BITP_MAC_CONFIG0_CPS)   |
                               ((uint32_t)poNwMacPhyCfg->bFTSE  << BITP_MAC_CONFIG0_FTSE)  |
                               ((uint32_t)poNwMacPhyCfg->bRXCTE << BITP_MAC_CONFIG0_RXCTE) |
                               ((uint32_t)poNwMacPhyCfg->bTXCTE << BITP_MAC_CONFIG0_TXCTE) |
                               ((uint32_t)(!poNwMacPhyCfg->bUse32BitTimestamp) << BITP_MAC_CONFIG0_FTSS)
                               | /* FTSS=1 for 64-bit (default), FTSS=0 for 32-bit */
                               (uint32_t)BITM_MAC_CONFIG0_SYNC |
                               ((uint32_t)nTxcThresh << BITP_MAC_CONFIG0_TXCTHRESH) |
                               ((poNwMacPhyCfg->bAppendCrcbyHost == true && poNwMacPhyCfg->bEnablePrioFifos == false) ? BITM_MAC_CONFIG0_TXFCSVE : 0U);
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to update CONFIG0 reg");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    /* Update PLCA_PRIORITY register - enable MAC frame priority FIFOs if configured */
    if(poNwMacPhyCfg->bEnablePrioFifos)
    {
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_PLCA_PRIORITY_MMS;
        poCtrlDesc->nStartAddr   = MAC_PLCA_PRIORITY_ADDR;
        poCtrlDesc->pRegData[0U] = MAC_PLCA_PRIORITY_RESET_VAL | BITM_MAC_PLCA_PRIORITY_FRAME_PRIORITY_EN;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
        ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to update PLCA_PRIORITY reg");
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }
}
ADI_NETWORK_MACPHY_CODE
static ADI_NETWORK_ERR ConfigureNode(uint32_t nInstNum, uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg)
{
    // TODO: Update it to a do-while loop!
    ADI_OASPI_CTRL_DESC *poCtrlDesc;
    ADI_OASPI_STATUS eOaspiStatus;

    // ~~~~~~~~~~~~ Reset ~~~~~~~~~~~~
#if MACPHY_RESET_EN == true
    // Perform MAC-PHY soft reset
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_RESET_MMS;
    poCtrlDesc->nStartAddr   = MAC_RESET_ADDR;
    poCtrlDesc->pRegData[0U] = BITM_MAC_RESET_SWRESET;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to reset MAC-PHY");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    adi_pal_waitMicroSec(10ULL); // TODO : to remove this reset block
#endif
    uint32_t anBootloaderCfgWrData[1U] =
    {
        ((uint32_t)ENUM_IO_BOOTLOADER_CFG_DEVICE_MODE_MAC_PHY << BITP_IO_BOOTLOADER_CFG_DEVICE_MODE)
    };
    uint32_t anBootloaderCfgMask[1U] = { BITM_IO_BOOTLOADER_CFG_DEVICE_MODE };
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    /* Reset the MAC-PHY */
    ResetMacPhy(nInstNum, nDevNum);

    /* Enable critical and user programmable interrupts */
    EnableInterrupts(nInstNum, nDevNum, poNwMacPhyCfg);

    /* Update CONFIG0 and CONFIG1 register data */
    UpdateConfigRegisters(nInstNum, nDevNum, poNwMacPhyCfg);

    // Enable MAC timer signals
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_MAC_INTERFACE_MMS;
    poCtrlDesc->nStartAddr   = IO_MAC_INTERFACE_ADDR;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pRegData[0U] = ((uint32_t)((poNwMacPhyCfg->bMacTimer) ? 1U : 0U) |
                                IO_MAC_INTERFACE_RESET_VAL);
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to enable MAC Timer");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    // Disable gPTP Counter
    if(!poNwMacPhyCfg->bGPTPCntrEn)
    {
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_TS_CFG_MMS;
        poCtrlDesc->nStartAddr   = MAC_TS_CFG_ADDR;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = 0U;
        eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
        ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to enable gPTP Counter");
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }

    // Enable wake button
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = AO_CONFIGURATION_FIELDS_1_MMS;
    poCtrlDesc->nStartAddr   = AO_CONFIGURATION_FIELDS_1_ADDR;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = AO_CONFIGURATION_FIELDS_1_RESET_VAL | BITM_AO_CONFIGURATION_FIELDS_1_CFG_VALID;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to enable wake pin");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    // Read back CONFIG0 register
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_CONFIG0_MMS;
    poCtrlDesc->nStartAddr   = MAC_CONFIG0_ADDR;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleMiscCallback;
    eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to read CONFIG0 reg");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    // Wait for the initialization writes to complete
    while(!goNwData.aoDevInfo[nDevNum].bInitComplete && !goNwData.aoDevInfo[nDevNum].bInErrState)
    {
        adi_network_run();
        adi_pal_waitMicroSec(10ULL);
    }

    /* Notify OA-SPI layer that an IRQ might have
       been asserted before SW started up */
    (void) adi_oaspi_ProcessIrq(nInstNum);

    /* Wait for the max gPTP sync interval */
    adi_network_flushTxTimed(125ULL * MS_TO_NS);

    /* Reduce the gPTP relock threshold to 10us */
    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 2U;
    poCtrlDesc->nMMS         = IO_GPTP_RELOCK_MMS;
    poCtrlDesc->nStartAddr   = IO_GPTP_RELOCK_ADDR;
    poCtrlDesc->pRegData[0U] = 0x2710U; //10us
    poCtrlDesc->pRegData[1U] = 0x0U;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);
    ADI_DBG_REQUIRE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to reconfigure the gPTP relock threshold");
    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    // ~~~~~~~~~~~~ Set the device mode ~~~~~~~~~~~~
    // Perform a non-blocking masked write to preserve OTP-programmed bits (e.g., WUS bit).
    eNwErr = network_macphy_MaskedWriteRegister(nDevNum,
             IO_BOOTLOADER_CFG_ADDR,
             1U,
             IO_BOOTLOADER_CFG_MMS,
             &anBootloaderCfgWrData[0U],
             &anBootloaderCfgMask[0U]);
    ADI_DBG_REQUIRE(eNwErr == ADI_NETWORK_OK, "Failed to submit masked write for IO_BOOTLOADER_CFG register");
    adi_network_flushTxBuffer(nDevNum, 20ULL * MS_TO_NS);

    /* Indicate OA-SPI to start sending data frames since the
       SYNC bit has been set and the MAC-PHY is configured */
    adi_oaspi_SetSync(nInstNum, true);

    /* Disable macloopback after configuring node */
    (void)network_macphy_EnableMacLoopBack(nDevNum, false);
    adi_network_flushTxBuffer(nDevNum, 20ULL * MS_TO_NS);

    // Set Plca en bit
    if(goNwData.aoDevInfo[nDevNum].bIsPlcaEn)
    {
        (void)EnablePlca(nDevNum);
    }

    return ADI_NETWORK_OK;
}

/* Handler function for awake state in wake sleep state machine */
ADI_NETWORK_MACPHY_CODE
static void ControllerAwakeStateHandler(ADI_NETWORK_WKSLP_DATA *poWkslpData)
{
    // 1. If goto_sleep is set:
    //      * Next state = SEND_SLEEP_CMD
    // 2. If goto_standby is set:
    //      * Next state = SEND_STANDBY_CMD
    if(poWkslpData->bGotoSleep == true)
    {
        poWkslpData->bGotoSleep = false;
        poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_SEND_SLEEP_CMD;
    }
    else if(poWkslpData->bGotoStandby == true)
    {
        poWkslpData->bGotoStandby = false;
        poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_SEND_STANDBY_CMD;
    }
    else
    {
        // Nothing to do
    }
}

ADI_NETWORK_MACPHY_CODE
static void ControllerStandByStateHandler(uint32_t nDevNum, ADI_NETWORK_WKSLP_DATA *poWkslpData)
{
    ADI_NETWORK_CTRL_STATES eCurrState = poWkslpData->eNodeState;

    switch(eCurrState)
    {
        case ADI_NETWORK_CTRL_STATE_STANDBY:
        {
            // If returned_from_standby is set:
            //  * Next state = AWAKE
            if(poWkslpData->bReturnedFromStandbyInt == true)
            {
                poWkslpData->bReturnedFromStandbyInt = false;
                // Notify application of standby to awake transition
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_AWAKE, &poWkslpData->eNodeState);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            }

            break;
        }

        case ADI_NETWORK_CTRL_STATE_SEND_STANDBY_CMD:
        {
            // Send standby command, save the current time, next state = Waiting_to_standby
            ADI_NETWORK_ERR eNwErr = SendWkslpCommand(nDevNum, WKSLP_CMD_STANDBY);

            if(eNwErr == ADI_NETWORK_OK)
            {
                adi_pal_getCurrTime(&poWkslpData->nStartTimeNs);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_WAITING_TO_STANDBY;
            }
            else
            {
                // Retry in the next run cycle
                // TODO: Add max retries
            }

            break;
        }

        case ADI_NETWORK_CTRL_STATE_WAITING_TO_STANDBY:
        {
            // 1. If line_busy or standby_err is set:
            //      * Give a callback with Standby transition failure event
            //      * Next State = AWAKE
            // 2. If timeout:
            //      * Next state = PING_NODE
            //      * Set context = WAITING_TO_STANDBY

            uint64_t nCurrTimeNs;
            adi_pal_getCurrTime(&nCurrTimeNs);

            if(nCurrTimeNs - poWkslpData->nStartTimeNs > WAITING_TO_STANDBY_TIMEOUT_NS)
            {
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_PING_NODE;
                poWkslpData->eNodeContext = ADI_NETWORK_CTRL_CTX_WAITING_TO_STANDBY;
            }
            else if(poWkslpData->bLineBusyInt == true)
            {
                poWkslpData->bLineBusyInt = false;
                ADI_NETWORK_WKSLP_FAIL_STATUS eFailStatus = ADI_NETWORK_WKSLP_LINE_BUSY;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_STANDBY_FAILED, &eFailStatus);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            }
            else if(poWkslpData->bStandbyErrInt == true)
            {
                poWkslpData->bStandbyErrInt = false;
                ADI_NETWORK_WKSLP_FAIL_STATUS eFailStatus = ADI_NETWORK_WKSLP_STANDBY_ERR;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_STANDBY_FAILED, &eFailStatus);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            }
            else
            {
                // Nothing to do
            }

            break;
        }

        default:
        {
            /* Unexpected state in ControllerStandByStateHandler */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected state 0x%X in ControllerStandByStateHandler", eCurrState);
            poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            break;
        }
    }
}

ADI_NETWORK_MACPHY_CODE
static void ControllerSleepStateHandler(uint32_t nDevNum, ADI_NETWORK_WKSLP_DATA *poWkslpData)
{
    ADI_NETWORK_CTRL_STATES eCurrState = poWkslpData->eNodeState;

    switch(eCurrState)
    {
        case ADI_NETWORK_CTRL_STATE_SLEEP:
        {
            // If confirm_awake / req_system_wakeup_conf is set:
            //  * Next state = PING_NODE
            //  * Set context = CONFIRM_AWAKE
            if(poWkslpData->bConfirmAwakeInt == true || poWkslpData->bReqSysWakeupConfInt == true)
            {
                poWkslpData->bConfirmAwakeInt = false;
                poWkslpData->bReqSysWakeupConfInt = false;
                // Notify application of standby to awake transition
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_AWAKE, &poWkslpData->eNodeState);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
                goNwData.aoDevInfo[nDevNum].bInErrState = false;
                goNwData.aoDevInfo[nDevNum].bInitComplete = false;
            }

            break;
        }

        case ADI_NETWORK_CTRL_STATE_SEND_SLEEP_CMD:
        {
            // Send sleep command, save the current time, next state = Waiting_to_sleep
            ADI_NETWORK_ERR eNwErr = SendWkslpCommand(nDevNum, WKSLP_CMD_SLEEP);

            if(eNwErr == ADI_NETWORK_OK)
            {
                adi_pal_getCurrTime(&poWkslpData->nStartTimeNs);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_WAITING_TO_SLEEP;
            }
            else
            {
                // Retry in the next run cycle
                // TODO: Add max retries
            }

            break;
        }

        case ADI_NETWORK_CTRL_STATE_WAITING_TO_SLEEP:
        {
            // 1. If line_busy or sleep_err is set:
            //      * Give a callback with sleep transition failure event
            //      * Next State = AWAKE
            // 2. If timeout:
            //      * Next state = PING_NODE
            //      * Set context = WAITING_TO_SLEEP

            uint64_t nCurrTimeNs;
            adi_pal_getCurrTime(&nCurrTimeNs);

            if(nCurrTimeNs - poWkslpData->nStartTimeNs > WAITING_TO_SLEEP_TIMEOUT_NS)
            {
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_PING_NODE;
                poWkslpData->eNodeContext = ADI_NETWORK_CTRL_CTX_WAITING_TO_SLEEP;
            }
            else if(poWkslpData->bLineBusyInt == true)
            {
                poWkslpData->bLineBusyInt = false;
                ADI_NETWORK_WKSLP_FAIL_STATUS eFailStatus = ADI_NETWORK_WKSLP_LINE_BUSY;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SLEEP_FAILED, &eFailStatus);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            }
            else if(poWkslpData->bSleepErrInt == true)
            {
                poWkslpData->bSleepErrInt = false;
                ADI_NETWORK_WKSLP_FAIL_STATUS eFailStatus = ADI_NETWORK_WKSLP_SLEEP_ERR;
                goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SLEEP_FAILED, &eFailStatus);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            }
            else
            {
                // Nothing to do
            }

            break;
        }

        default:
        {
            /* Unexpected state in ControllerSleepStateHandler */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected state 0x%X in ControllerSleepStateHandler", eCurrState);
            poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            break;
        }
    }
}

ADI_NETWORK_MACPHY_CODE
static void ControllerPingHandler(uint32_t nDevNum, ADI_NETWORK_WKSLP_DATA *poWkslpData)
{
    ADI_NETWORK_CTRL_STATES eCurrState = poWkslpData->eNodeState;

    switch(eCurrState)
    {
        case ADI_NETWORK_CTRL_STATE_PING_NODE:
        {
            // Enable ping check, this will help to differentiate between generic ping and ping check from wakesleep statemachine
            poWkslpData->bPingCheckEn = true;

            // Ping node and set the next state to waiting for ping response
            ADI_NETWORK_ERR eNwErr = PingNode(nDevNum);

            if(eNwErr == ADI_NETWORK_OK)
            {
                poWkslpData->bPingRcvd = false;
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_WAITING_FOR_PING_RESP;
            }
            else
            {
                poWkslpData->bPingCheckEn = false;
                // Retry in the next run cycle
                // TODO: Add max retries
            }

            break;
        }

        case ADI_NETWORK_CTRL_STATE_WAITING_FOR_PING_RESP:
        {
            // If context == waiting_to_standby
            //  * If ping response is received, it is an error condition.
            //    Give a callback and switch to awake state
            //  * If OA-SPI error is detected, switch to standby state
            // If context == waiting_to_sleep
            //  * If ping response is received, it is an error condition.
            //    Give a callback and switch to awake state
            //  * If OA-SPI error is detected, switch to sleep state

            if(poWkslpData->eNodeContext == ADI_NETWORK_CTRL_CTX_WAITING_TO_STANDBY)
            {
                if(poWkslpData->bOaspiErr == true)
                {
                    poWkslpData->bOaspiErr = false;
                    poWkslpData->bPingCheckEn = false;
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_STANDBY, NULL);
                    poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_STANDBY;
                }
                else if(poWkslpData->bPingRcvd == true)
                {
                    poWkslpData->bPingRcvd = false;
                    poWkslpData->bPingCheckEn = false;
                    ADI_NETWORK_WKSLP_FAIL_STATUS eFailStatus = ADI_NETWORK_WKSLP_PING_RESP_ERR;
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_STANDBY_FAILED, &eFailStatus);
                    poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
                }
                else
                {
                    /* Nothing to do */
                }
            }
            else if(poWkslpData->eNodeContext == ADI_NETWORK_CTRL_CTX_WAITING_TO_SLEEP)
            {
                if(poWkslpData->bOaspiErr == true)
                {
                    poWkslpData->bOaspiErr = false;
                    poWkslpData->bPingCheckEn = false;
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SLEEP, NULL);
                    poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_SLEEP;
                }
                else if(poWkslpData->bPingRcvd == true)
                {
                    poWkslpData->bPingRcvd = false;
                    poWkslpData->bPingCheckEn = false;
                    ADI_NETWORK_WKSLP_FAIL_STATUS eFailStatus = ADI_NETWORK_WKSLP_PING_RESP_ERR;
                    goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SLEEP_FAILED, &eFailStatus);
                    poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
                }
                else
                {
                    /* Nothing to do */
                }
            }
            else
            {
                /* Unexpected state in ControllerPingHandler */
                ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected state 0x%X in ControllerPingHandler", eCurrState);
                poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            }

            break;
        }

        default:
        {
            /* Unexpected state in ControllerPingHandler */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected state 0x%X in ControllerPingHandler default", eCurrState);
            poWkslpData->eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
            break;
        }
    }
}

ADI_NETWORK_MACPHY_CODE
static void WkSlp_Run(uint32_t nDevNum)
{
    ADI_NETWORK_DEVINFO *poDevInfo = &goNwData.aoDevInfo[nDevNum];
    ADI_NETWORK_WKSLP_DATA *poWkslpData = &poDevInfo->oWkslpData;
    ADI_NETWORK_CTRL_STATES eCurrState = poWkslpData->eNodeState;

    /* State machine for handling wakesleep states */
    switch(eCurrState)
    {
        case ADI_NETWORK_CTRL_STATE_AWAKE:
        {
            /* Awake state handler - check if go to sleep / standby has been called and changes the state to either send to sleep / standby state */
            ControllerAwakeStateHandler(poWkslpData);
            break;
        }

        case ADI_NETWORK_CTRL_STATE_STANDBY:
        case ADI_NETWORK_CTRL_STATE_SEND_STANDBY_CMD:
        case ADI_NETWORK_CTRL_STATE_WAITING_TO_STANDBY:
        {
            /* Standby state handler - sends standby command, waits for the confirmation that the controller has gone to standby mode and handles if there are any errors associated with it */
            ControllerStandByStateHandler(nDevNum, poWkslpData);
            break;
        }

        case ADI_NETWORK_CTRL_STATE_SLEEP:
        case ADI_NETWORK_CTRL_STATE_SEND_SLEEP_CMD:
        case ADI_NETWORK_CTRL_STATE_WAITING_TO_SLEEP:
        {
            /* Sleep state handler - sends sleep command, waits for the confirmation that the controller has gone to sleep mode and handles if there are any errors associated with it */
            ControllerSleepStateHandler(nDevNum, poWkslpData);
            break;
        }

        case ADI_NETWORK_CTRL_STATE_PING_NODE:
        case ADI_NETWORK_CTRL_STATE_WAITING_FOR_PING_RESP:
        {
            /* Sends ping commands, and confirms if a response is received - useful in confirming sleep and standby states transistions */
            ControllerPingHandler(nDevNum, poWkslpData);
            break;
        }

        default:
        {
            /* Unexpected state in WkSlp_Run */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unexpected state 0x%X in WkSlp_Run", eCurrState);
            break;
        }
    }
}

/* Checker function for SSC IRQ callbacks */
ADI_NETWORK_MACPHY_CODE
static void CheckSscStatus(uint32_t nDevNum)
{
    /* By default it is assumed that the controller is in functional mode in the init sequence */
    ADI_NETWORK_DEVINFO *poDevInfo = &goNwData.aoDevInfo[nDevNum];
    ADI_NETWORK_SSC_DATA *poSscData = &poDevInfo->oSscData;

    /* If safe mode is set to true, give a callback to notify the app for the same -
    safe mode variable is maintained internally and gets it's value from interrupt context */
    if(poSscData->bSafeMode)
    {
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SSC_SAFE_STATE, NULL);
        poSscData->bSafeMode = false;
        poSscData->bFuncMode = false;
    }

    /* If functional mode is set to true, give a callback to notify the app for the same -
       bReturnToFunc is maintained internally to check if the controller has returned to functional mode(gets it value from interrupt context)
       as intially we assume it is in functional mode */
    if(poSscData->bReturnToFunc)
    {
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SSC_RETURN_TO_FUNC, NULL);
        poSscData->bReturnToFunc = false;
        poSscData->bFuncMode = true;
    }

    /* If the return to functional mode is failed, give a callback to notify the same -
       happens usually if the trigger that is enabled for safe state transistion is still present */
    if(poSscData->bUnsuccessfulreturnToFunc)
    {
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SSC_UNSUCCESSFUL_RETURN_TO_FUNC, NULL);
        poSscData->bUnsuccessfulreturnToFunc = false;
    }
}

/* Checker function for SQI IRQ callbacks */
ADI_NETWORK_MACPHY_CODE
static void CheckSqiStatus(uint32_t nDevNum)
{
    /* By default it is assumed that the controller has good SQI level in the init sequence */
    ADI_NETWORK_DEVINFO *poDevInfo = &goNwData.aoDevInfo[nDevNum];
    ADI_NETWORK_SQI_DATA *poSqiData = &poDevInfo->oSqiData;

    /* If the SQI level is good, give a callback to notify the app for the same */
    if(poSqiData->bSQIGoodLvl)
    {
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SQI_LVL_RETURN_GOOD, NULL);
        poSqiData->bSQIGoodLvl = false;
        poSqiData->bInGoodLvl = true;
    }

    /* If the SQI level transitioned to warning level, give a callback to notify the app for the same */
    if(poSqiData->bSQIWarningLvl)
    {
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SQI_LVL_WARNING, NULL);
        poSqiData->bSQIWarningLvl = false;
        poSqiData->bInGoodLvl = false;
    }

    /* If the SQI level transitioned to bad level, give a callback to notify the app for the same */
    if(poSqiData->bSQIBadLvl)
    {
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_SQI_LVL_BAD, NULL);
        poSqiData->bSQIBadLvl = false;
    }
}

/*=================================== CODE ==================================*/
/*! Initializes MAC-PHY module */
ADI_NETWORK_MACPHY_CODE
void network_macphy_Init(void)
{
    // Initialize control descriptors
    for(uint16_t i = 0U; i < NETWORK_CTRL_Q_SIZE; i++)
    {
        (void)ADI_MEMSET(&gaoOaspiCtrlDesc[i], 0, sizeof(gaoOaspiCtrlDesc[i]));
        gaoOaspiCtrlDesc[i].pfCallback = OaspiCallback; // By default the pointer to callback function is set to OaspiCallback
        /* The memory for register data field in control descriptors is allocated from the global array (ganRegData) */
        gaoOaspiCtrlDesc[i].pRegData   = &ganRegData[i][0U];
    }

    for(uint16_t j = 0u; j < MAX_MACPHY_INST; j++)
    {
        // Initialize TX data descriptors
        for(uint16_t i = 0U; i < NETWORK_DATA_Q_SIZE; i++)
        {
            (void)ADI_MEMSET(&gaoOaspiDataDesc[j][i], 0, sizeof(gaoOaspiDataDesc[j][i]));
            // By default the pointer to callback function is set to OaspiCallback
            gaoOaspiDataDesc[j][i].pfCallback = OaspiCallback;
#ifndef ADI_SERDES_MODE
            /* The memory for frame buffer field in TX data descriptors is allocated from TX ethernet buffers */
            gaoOaspiDataDesc[j][i].pFrameBuff = &ganEthBuff[j][i][0U];
#endif
        }

        /* Initialize HP TX data descriptors */
#ifdef ADI_NETWORK_PRIO_Q_EN

        for(uint16_t i = 0U; i < NETWORK_DATA_HP_Q_SIZE; i++)
        {
            (void)ADI_MEMSET(&gaoOaspiDataDescHp[j][i], 0, sizeof(gaoOaspiDataDescHp[j][i]));
            gaoOaspiDataDescHp[j][i].pfCallback = OaspiCallback;
#ifndef ADI_SERDES_MODE
            /* The memory for frame buffer field in HP TX data descriptors is allocated from HP TX ethernet buffers */
            gaoOaspiDataDescHp[j][i].pFrameBuff = &ganEthBuffHp[j][i][0U];
#endif
        }

#endif /* ADI_NETWORK_PRIO_Q_EN */
    }

    for(uint16_t j = 0u; j < MAX_MACPHY_INST; j++)
    {
        // Initialize RX data descriptors
        for(uint16_t i = 0U; i < NETWORK_RX_Q_SIZE; i++)
        {
            (void)ADI_MEMSET(&gaoOaspiRxDesc[j][i], 0, sizeof(gaoOaspiRxDesc[j][i]));
            // By default the pointer to callback function is set to OaspiCallback
            gaoOaspiRxDesc[j][i].pfCallback = OaspiCallback;
            /* The memory for frame buffer field in RX data descriptors is allocated from receive ethernet buffers */
            gaoOaspiRxDesc[j][i].pFrameBuff = &ganEthRxBuff[j][i][0U];
        }
    }
}

/*! Terminates MAC-PHY module */
ADI_NETWORK_MACPHY_CODE
void network_macphy_Terminate(void)
{
    // Clear all descriptors
    (void) ADI_MEMSET(&gaoOaspiCtrlDesc[0U], 0, sizeof(gaoOaspiCtrlDesc));
    (void) ADI_MEMSET(&gaoOaspiDataDesc, 0, sizeof(gaoOaspiDataDesc));
    (void) ADI_MEMSET(&gaoOaspiRxDesc, 0, sizeof(gaoOaspiRxDesc));
}

/*! Opens a MAC-PHY device */
ADI_NETWORK_MACPHY_CODE
void network_macphy_Open(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg)
{
    ADI_OASPI_STATUS eOaspiStatus;
    ADI_PAL_STATUS ePalStatus;
    ADI_OASPI_DATA_DESC *poDesc = NULL;

    // MAC-PHY instance number
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    uint32_t nSpiId   = goNwData.aoDevInfo[nDevNum].nId;

    goNwData.aoDevInfo[nDevNum].bInErrState = false;
    goNwData.aoDevInfo[nDevNum].bInitComplete = false;
    (void)ADI_MEMSET(&goNwData.aoDevInfo[nDevNum].oWkslpData, 0, sizeof(ADI_NETWORK_WKSLP_DATA));
    goNwData.aoDevInfo[nDevNum].oWkslpData.eNodeState = ADI_NETWORK_CTRL_STATE_AWAKE;
    goNwData.paoNwMacPhyCfg[nInstNum] = poNwMacPhyCfg;

    // Pre-conditions
    ADI_DBG_REQUIRE(goNwData.paoNwMacPhyCfg[nInstNum]->nSpiFreq != 0U, "Spi frequency is zero");
    ADI_DBG_REQUIRE(goNwData.paoNwMacPhyCfg[nInstNum]->nClkPha <= 1U, "Invalid CPHA setting");
    ADI_DBG_REQUIRE(goNwData.paoNwMacPhyCfg[nInstNum]->nClkPol <= 1U, "Invalid CPOL setting");

    if(goNwData.paoNwMacPhyCfg[nInstNum]->bEnablePrioFifos == true)
    {
        ADI_DBG_REQUIRE(goNwData.paoNwMacPhyCfg[nInstNum]->eCPS >= ADI_NETWORK_CPS_32BYTES,
                        "Priority FIFO not supported with chunk size 8 or 16 bytes");
    }

    // Initialize OA-SPI module
    gaoOaspiCfg[nInstNum].pCbk = OaspiCallback;
    gaoOaspiCfg[nInstNum].nCPS = (uint8_t)goNwData.paoNwMacPhyCfg[nInstNum]->eCPS;
    gaoOaspiCfg[nInstNum].nChunkSize = (uint8_t)(1U << gaoOaspiCfg[nInstNum].nCPS);
    gaoOaspiCfg[nInstNum].bUse32BitTimestamp = goNwData.paoNwMacPhyCfg[nInstNum]->bUse32BitTimestamp;
    gaoOaspiCfg[nInstNum].bAppendCrcbyHost = goNwData.paoNwMacPhyCfg[nInstNum]->bAppendCrcbyHost;
    gaoOaspiCfg[nInstNum].bValidateCrcbyHost = goNwData.paoNwMacPhyCfg[nInstNum]->bValidateCrcbyHost;
    gaoOaspiCfg[nInstNum].bEnablePrioFifos = goNwData.paoNwMacPhyCfg[nInstNum]->bEnablePrioFifos;
    goNwData.aoDevInfo[nDevNum].nChunkSize = gaoOaspiCfg[nInstNum].nChunkSize;
    adi_oaspi_Init(nInstNum, &gaoOaspiCfg[nInstNum]);

    /* Update maximum transmit credits (defaults before FIFO sizes are configured) */
    if(goNwData.paoNwMacPhyCfg[nInstNum]->bEnablePrioFifos)
    {
        adi_oaspi_ConfigureMaxTxc(nInstNum, 29U, 31U);
    }
    else
    {
        adi_oaspi_ConfigureMaxTxc(nInstNum, 31U, 0U);
    }

    // Open the SPI device
    ePalStatus = adi_pal_spiOpen(nSpiId, goNwData.paoNwMacPhyCfg[nInstNum]->nSpiFreq,
                                 goNwData.paoNwMacPhyCfg[nInstNum]->nClkPol,
                                 goNwData.paoNwMacPhyCfg[nInstNum]->nClkPha);
    ADI_DBG_ENSURE(ePalStatus == ADI_PAL_OK, "Failed to open SPI device");

    // Configure the node
    (void)ConfigureNode(nInstNum, nDevNum, poNwMacPhyCfg);

    // Submit RX descriptors
    for(uint16_t i = 0U; i < NETWORK_RX_Q_SIZE; i++)
    {
        poDesc = &gaoOaspiRxDesc[nInstNum][goNwData.nRxWrIdx[nInstNum]];
        eOaspiStatus = adi_oaspi_SubmitRxDesc(nInstNum, poDesc);
        ADI_DBG_ENSURE(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS, "Failed to submit RX descriptor");
        IncrementQIdx(&goNwData.nRxWrIdx[nInstNum], NETWORK_RX_Q_SIZE);
    }
}

/*! Closes a MAC-PHY device */
ADI_NETWORK_MACPHY_CODE
void network_macphy_Close(uint32_t nDevNum)
{
    ADI_PAL_STATUS ePalStatus;

    // MAC-PHY instance number
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    uint32_t nSpiId   = goNwData.aoDevInfo[nDevNum].nId;

    // Terminate the OA-SPI instance
    adi_oaspi_Terminate(nInstNum);

    // Close the SPI device
    ePalStatus = adi_pal_spiClose(nSpiId);
    ADI_DBG_ENSURE(ePalStatus == ADI_PAL_OK, "Failed to close the Spi device");
}

/*! Periodic tick function */
ADI_NETWORK_MACPHY_CODE
void network_macphy_Run(uint32_t nDevNum)
{
    ADI_NETWORK_DEVINFO *poDevInfo = &goNwData.aoDevInfo[nDevNum];

    // MAC-PHY instance number
    uint32_t nInstNum = poDevInfo->nCtrlIdx;

    // Run the Wake-Sleep state machine only if it enabled
    if(goNwData.paoNwMacPhyCfg[nInstNum]->bEnableWkSlp)
    {
        // Tick the Wake-Sleep state machine
        WkSlp_Run(nDevNum);
    }

    if(goNwData.paoNwMacPhyCfg[nInstNum]->bEnableSscInt)
    {
        // Check safe state controller status
        CheckSscStatus(nDevNum);
    }

    if(goNwData.paoNwMacPhyCfg[nInstNum]->bEnableSqiInt)
    {
        // Check SQI status
        CheckSqiStatus(nDevNum);
    }

    // Tick the OA-SPI state machine
    adi_oaspi_Run(nInstNum);

    // Tick SPI PAL for any events
    adi_pal_spiRun(poDevInfo->nId);
}

/*! Writes to MAC-PHY register(s) */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_WriteRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    uint32_t  *panRegData,
    ADI_NETWORK_CBK pfCbk
)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    if(nLength <= NETWORK_MAX_REG_LEN)
    {
        /********************* Copy the register write request to the control descriptor *********************/
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = nStartAddr;
        poCtrlDesc->nLength      = nLength;
        poCtrlDesc->nMMS         = nMMS;
        poCtrlDesc->pCbParam     = (void *)pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        /* Copy the register data value to the descriptor's reg data field */
        (void)ADI_MEMCPY(poCtrlDesc->pRegData, panRegData, (size_t)nLength * sizeof(uint32_t));
        ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_WriteReg(nInstNum, poCtrlDesc);

        // Check if the status is success, to increment the queue index
        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_OK;
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
    }

    return eNwErr;
}

/*! Reads from MAC-PHY register(s) */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_ReadRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    ADI_NETWORK_CBK pfCbk
)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    if(nLength <= NETWORK_MAX_REG_LEN)
    {
        /********************* Copy the register read request to the control descriptor *********************/
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = nStartAddr;
        poCtrlDesc->nLength      = nLength;
        poCtrlDesc->nMMS         = nMMS;
        poCtrlDesc->pCbParam     = pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc); /* Submit the read request to OASPI */

        // Check if the status is success, to increment the queue index and to proceed further
        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_OK;
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

            // Check whether the register address fall in the HV die register map range - if so then read the register twice
            if(nStartAddr >= OTP_OMC_ADDR && nStartAddr <= AO_CONFIGURATION_FIELDS_1_ADDR)
            {
                eNwErr = ADI_NETWORK_NO_SPACE;

                // HV die register needs to be read twice
                poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
                poCtrlDesc->nStartAddr   = nStartAddr;
                poCtrlDesc->nLength      = nLength;
                poCtrlDesc->nMMS         = nMMS;
                poCtrlDesc->pCbParam     = goNwData.pfCbk;
                poCtrlDesc->pfCallback   = OaspiCallback;
                eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

                // Check if the status is success, to increment the queue index
                if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
                {
                    eNwErr = ADI_NETWORK_OK;
                    IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
                }
            }
        }
    }

    return eNwErr;
}

/*! Performs MAC-PHY register read and masked write at given bit postions */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_MaskedWriteRegister(
    uint32_t  nDevNum,
    uint16_t  nStartAddr,
    uint8_t   nLength,
    uint8_t   nMMS,
    uint32_t  *panRegData,
    uint32_t  *panRegMask
)
{
    ADI_DBG_REQUIRE(panRegData != NULL, "Register data pointer is NULL");
    ADI_DBG_REQUIRE(panRegMask != NULL, "Register mask pointer is NULL");

    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    ADI_NETWORK_MASKED_WR_DATA *poMaskedWrData = &gaoMaskedWrData[goNwData.nCtrlWrIdx];

    if(nLength <= NETWORK_MAX_REG_LEN)
    {
        poCtrlDesc->nStartAddr   = nStartAddr;
        poCtrlDesc->nLength      = nLength;
        poCtrlDesc->nMMS         = nMMS;
        poCtrlDesc->pCbParam     = poMaskedWrData;
        /* Set masked write callback function */
        poCtrlDesc->pfCallback   = HandleMaskedWrRegCallback;

        /* Set register mask */
        (void)ADI_MEMCPY(&poMaskedWrData->anRegMapMask[0], &panRegMask[0],
                         (size_t)nLength * sizeof(uint32_t));
        /* Set register data */
        (void)ADI_MEMCPY(&poMaskedWrData->anRegWrData[0], &panRegData[0],
                         (size_t)nLength * sizeof(uint32_t));

        if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
        {
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
            eNwErr = ADI_NETWORK_OK;
        }
    }

    return eNwErr;
}

/*! Submits an ethernet frame for transmission */
ADI_NETWORK_MACPHY_CODE
#ifndef ADI_SERDES_MODE
void network_macphy_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg)
{
    ADI_OASPI_STATUS eOaspiStatus;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

#ifdef ADI_NETWORK_PRIO_Q_EN
    /* Determine if this is an HP route */
    bool bIsHp = false;

    if(goNwData.paoNwMacPhyCfg[nInstNum] != NULL)
    {
        if((goNwData.paoNwMacPhyCfg[nInstNum]->bEnablePrioFifos) &&
            (poNwFrameCfg->eMacRoutingInfo == ADI_MACPHY_ROUTE_PORT1_HP))
        {
            bIsHp = true;
        }
        else
        {
            /* Nothing to do - bIsHp remains false */
        }
    }
    else
    {
        /* Nothing to do - bIsHp remains false */
    }

    if(bIsHp)
    {
        /* HP path: use HP descriptor and buffer arrays */
        uint16_t nWrIdx = goNwData.anEthWrIdxHp[nInstNum];

        ADI_OASPI_DATA_DESC *poDataDesc = &gaoOaspiDataDescHp[nInstNum][nWrIdx];
        poDataDesc->nFrameSize      = poNwFrameCfg->nLenByte;
        poDataDesc->nVendorSpecific = (uint8_t)poNwFrameCfg->eMacRoutingInfo;
        poDataDesc->eTsReg          = poNwFrameCfg->eTsReg;

        /* Encode HP pool identity: bit 31 set = HP flag */
        ganOaspiDataDescBufIdxHp[nWrIdx] = (uint32_t)nWrIdx | 0x80000000U;
        poDataDesc->pCbParam        = &ganOaspiDataDescBufIdxHp[nWrIdx];

        eOaspiStatus = adi_oaspi_SubmitTxDesc(nInstNum, poDataDesc);
    }
    else
#endif /* ADI_NETWORK_PRIO_Q_EN */
    {
        /* LP path: use existing LP descriptor and buffer arrays */
        uint16_t nWrIdx = goNwData.anEthWrIdx[nInstNum];

        ADI_OASPI_DATA_DESC *poDataDesc = &gaoOaspiDataDesc[nInstNum][nWrIdx];
        poDataDesc->nFrameSize      = poNwFrameCfg->nLenByte;
        poDataDesc->nVendorSpecific = (uint8_t)poNwFrameCfg->eMacRoutingInfo;
        poDataDesc->eTsReg          = poNwFrameCfg->eTsReg;

        ganOaspiDataDescBufIdx[nWrIdx] = nWrIdx;
        poDataDesc->pCbParam        = &ganOaspiDataDescBufIdx[nWrIdx];

        eOaspiStatus = adi_oaspi_SubmitTxDesc(nInstNum, poDataDesc);
    }

    if(eOaspiStatus != ADI_OASPI_STATUS_SUCCESS)
    {
        ADI_DBG_LOG(LOG_ERROR, stderr, "Failed to submit TX descriptor - queue full");
        ADI_NETWORK_EVT_ERR_DATA oErrDat;
        oErrDat.eErrType = ADI_NETWORK_MACPHY_CTRL_Q_FULL;
        oErrDat.pData = NULL;
        goNwData.pfCbk(nDevNum, ADI_NETWORK_EVT_ERR, &oErrDat);
    }
    else
    {
        /* Nothing to do */
    }
}
#else
ADI_NETWORK_ERR network_macphy_SubmitTxFrame(uint32_t nDevNum, ADI_NETWORK_FRAME_CFG *poNwFrameCfg, uint8_t *pBuffer)
{
    ADI_OASPI_STATUS eOaspiStatus;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    /* Check for TX data descriptors */
    uint16_t nWrIdx = goNwData.anDataDescWrIdx[nInstNum];
    uint16_t nRdIdx = goNwData.anDataDescRdIdx[nInstNum];
    uint16_t nSpaceUsedUp = NETWORK_DATA_Q_SIZE + nWrIdx - nRdIdx;
    nSpaceUsedUp = (nSpaceUsedUp < NETWORK_DATA_Q_SIZE) ? (nSpaceUsedUp) : (nSpaceUsedUp - NETWORK_DATA_Q_SIZE);

    /* If there is space available in TX data descriptor queue, submit the frame for transmission */
    if(nSpaceUsedUp != (NETWORK_DATA_Q_SIZE - 1U))
    {
        /* Take a data descriptor from gaoOaspiDataDesc array and fill frame size, vendor specific bit and egress timestamp capture settings */
        ADI_OASPI_DATA_DESC *poDataDesc = &gaoOaspiDataDesc[nInstNum][nWrIdx];
        poDataDesc->nFrameSize      = poNwFrameCfg->nLenByte;
        poDataDesc->nVendorSpecific = (uint8_t)poNwFrameCfg->eMacRoutingInfo;
        poDataDesc->eTsReg          = poNwFrameCfg->eTsReg;
        poDataDesc->pFrameBuff      = pBuffer;

        eOaspiStatus = adi_oaspi_SubmitTxDesc(nInstNum, poDataDesc);

        if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
        {
            IncrementQIdx(&goNwData.anDataDescWrIdx[nInstNum], NETWORK_DATA_Q_SIZE);
        }
        else if(eOaspiStatus == ADI_OASPI_STATUS_QUEUE_FULL)
        {
            eNwErr = ADI_NETWORK_MACPHY_OASPI_TX_QUEUE_FULL;
        }
        else
        {
            /* unknown error from OASPI */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Unknown OASPI error 0x%X on TX descriptor submission", eOaspiStatus);
            eNwErr = ADI_NETWORK_MACPHY_OASPI_TX_QUEUE_FULL;
        }
    }
    else
    {
        eNwErr = ADI_NETWORK_MACPHY_TX_DESCRIPTOR_UNAVAILABLE;
    }

    return eNwErr;
}

/*! Resets OASPI TX queue and descriptor indexes */
ADI_NETWORK_MACPHY_CODE
void network_macphy_ResetOaspiQueue(uint32_t nDevNum)
{
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /* Reset TX descriptor indexes */
    goNwData.anDataDescWrIdx[nInstNum] = 0U;
    goNwData.anDataDescRdIdx[nInstNum] = 0U;

    /* Reset OASPI queues as well */
    adi_oaspi_ResetDataQueue(nInstNum);
}
#endif

/*! Gets MAC status */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetMacStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Read MAC status registers
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = MAC_RX_FRM_CNT_ADDR;
    poCtrlDesc->nLength      = NUM_MAC_STATUS_REG;
    poCtrlDesc->nMMS         = MAC_RX_FRM_CNT_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleStatsRegRead; // Callback function to handle the read status register

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        eNwErr =  ADI_NETWORK_NO_SPACE;
    }

    return eNwErr;
}

/*! Gets PLCA status */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetPlcaStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Read PLCA status registers
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_OA_PLCA_CTRL1_ADDR;
    poCtrlDesc->nLength      = NUM_PHY_STATUS_REG;
    poCtrlDesc->nMMS         = PHY_OA_PLCA_CTRL1_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleStatsRegRead; // Callback function to handle the read status register data

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        eNwErr = ADI_NETWORK_NO_SPACE;
    }

    return eNwErr;
}

/*! Gets PTP status */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetPtpStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Read PTP status registers
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = IO_GPTP_SYNC_COUNT_ADDR;
    poCtrlDesc->nLength      = 21U;
    poCtrlDesc->nMMS         = IO_GPTP_SYNC_COUNT_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleStatsRegRead; // Callback function to handle the read status register data

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        eNwErr = ADI_NETWORK_NO_SPACE;
    }

    return eNwErr;
}

/*! Resets the MAC */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_MacReset(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /******************** Perform MAC reset ********************/
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_RESET_MMS;
    poCtrlDesc->nStartAddr   = MAC_RESET_ADDR;
    poCtrlDesc->pRegData[0U] = BITM_MAC_RESET_SWRESET;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        // Increment the queue index by one and set the return status as OK
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Resets the HV Die */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_HVDieReset(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /******************** Perform HV Die reset ********************/
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_HV_RESET_CONTROL_MMS;
    poCtrlDesc->nStartAddr   = IO_HV_RESET_CONTROL_ADDR;
    poCtrlDesc->pRegData[0U] = HV_DIE_RESET;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        // Increment the queue index by one and set the return status as OK
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Resets the LV Die */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_LVDieReset(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /******************** Perform LV Die reset ********************/
    // OASPI block gets reset as part of LV Die reset, so on completion of LV Die reset, it is better to clear LOFE bit in the status register
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_RESET_CONTROL_MMS;
    poCtrlDesc->nStartAddr   = IO_RESET_CONTROL_ADDR;
    poCtrlDesc->pRegData[0U] = LV_DIE_RESET;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Checks if TX buffers are empty */
ADI_NETWORK_MACPHY_CODE
bool network_macphy_AreTxBuffersEmpty(uint32_t nDevNum)
{
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    bool bEmpty = false;

    // Check if control buffer is empty
    bEmpty = adi_oaspi_IsCtrlTxBufEmpty(nInstNum);

    // Check if data buffer is empty
    bEmpty = bEmpty && adi_oaspi_IsDataTxBufEmpty(nInstNum);

    // Check if HW TX buffer is empty
    bEmpty = bEmpty && adi_oaspi_IsHWTxBufEmpty(nInstNum);

    return bEmpty;
}

/*! Sets the PLCA */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetPlcaMulti(uint32_t nDevNum, const ADI_NETWORK_MACPHY_PLCA_CFG *poPlcaCfg)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        // Get the instance number for the given device number
        uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
        // Set the PLCA node count and local ID
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_PLCA_CTRL1_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = PHY_OA_PLCA_CTRL1_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = ((uint32_t)poPlcaCfg->nNodeCount << BITP_PHY_OA_PLCA_CTRL1_PLCANODECOUNT) |
                                   ((uint32_t)poPlcaCfg->anPLCAID[0U] << BITP_PHY_OA_PLCA_CTRL1_PLCALOCALID);

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        // Set multiple PLCA local IDs and also set the PLCA local ID enable mask, to selectively enable the "PLCA local IDs" that are configured
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_PLCA_CTRL2_ADDR;
        poCtrlDesc->nLength      = 4U;
        poCtrlDesc->nMMS         = PHY_OA_PLCA_CTRL2_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = (uint32_t)poPlcaCfg->anPLCAID[1U] |
                                   ((uint32_t)poPlcaCfg->anPLCAID[2U] << BITP_PHY_OA_PLCA_CTRL2_PLCALOCALID2);
        poCtrlDesc->pRegData[1U] = (uint32_t)poPlcaCfg->anPLCAID[3U] |
                                   ((uint32_t)poPlcaCfg->anPLCAID[4U] << BITP_PHY_OA_PLCA_CTRL3_PLCALOCALID4);
        poCtrlDesc->pRegData[2U] = (uint32_t)poPlcaCfg->anPLCAID[5U] |
                                   ((uint32_t)poPlcaCfg->anPLCAID[6U] << BITP_PHY_OA_PLCA_CTRL4_PLCALOCALID6);
        poCtrlDesc->pRegData[3U] = (uint32_t)poPlcaCfg->anPLCAID[7U] |
                                   ((uint32_t)poPlcaCfg->nPlcaIdEn << BITP_PHY_OA_PLCA_CTRL5_PLCALOCALID_EN);

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* Mark the PLCA as enabled, will be useful incase if the node is getting
        reconfigured as the copy of it gets deleted from OA register */
        goNwData.aoDevInfo[nDevNum].bIsPlcaEn = poPlcaCfg->bEnabled;
        eNwErr = EnablePlca(nDevNum);  /* Enable the PLCA on controller */
    } while(false);

    return eNwErr;
}

/*! Sets the MAC filter table */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetFilterTable(uint32_t nDevNum, uint8_t anMacAddr[][6u], uint8_t nNumAddr)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
        /* Update config 2 register to override FWD_UNK2HOST set in init sequence so that the filter table rules take effect */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = MAC_CONFIG2_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = MAC_CONFIG2_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = 0x7C2000U /* Hardcoded based on reset value */ |
                                   (goNwData.paoNwMacPhyCfg[nInstNum]->bAppendCrcbyHost == false ? BITM_MAC_CONFIG2_HOST_CRC_APPEND |
                                    BITM_MAC_CONFIG2_LO_PRIO_FIFO_CRC_APPEND : 0U) |
                                   BITM_MAC_CONFIG2_TX_RDY_ON_EMPTY;

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* Note: We are starting from filter table entry 1 below since entry 0 might be programmed for PTP filtering in OTP */
        uint8_t nRegIdx = 0U;
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = MAC_ADDR_FILT_UPR_1_ADDR;
        poCtrlDesc->nLength      = nNumAddr * 2U;
        poCtrlDesc->nMMS         = MAC_ADDR_FILT_UPR_1_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;

        /* Set the filter table entries */
        for(uint8_t i = 0U; i < nNumAddr; i++)
        {
            uint16_t anMacAddrUpper = ((uint16_t)anMacAddr[i][0u] << 8u) | (uint16_t)anMacAddr[i][1u];
            uint32_t anMacAddrLower = ((uint32_t)anMacAddr[i][2u] << 24u) | ((uint32_t)anMacAddr[i][3u] << 16u) |
                                      ((uint32_t)anMacAddr[i][4u] << 8u) | (uint32_t)anMacAddr[i][5u];
            poCtrlDesc->pRegData[nRegIdx++] = ((uint32_t)anMacAddrUpper << BITP_MAC_ADDR_FILT_UPR_1_MAC_ADDR) |
                                              BITM_MAC_ADDR_FILT_UPR_1_TO_HOST | /* Forward  frames to HOST matching with this MAC address */
                                              BITM_MAC_ADDR_FILT_UPR_1_APPLY2PORT; /* Enable the rule to apply for HOST */
            poCtrlDesc->pRegData[nRegIdx++] = anMacAddrLower;
        }

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;
}

/*! Clears the MAC filter table */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_ClearFilterTable(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Update the config 2 register to set the FWD_UNK2HOST bit
    // This will essentially make the filter table rules ineffective and all frames will be forwarded to HOST
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = MAC_CONFIG2_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_CONFIG2_MMS;
    poCtrlDesc->pRegData[0U] = 0x7C2000U /* Hardcoded based on reset value */ |
                               (goNwData.paoNwMacPhyCfg[nInstNum]->bAppendCrcbyHost == false ? BITM_MAC_CONFIG2_HOST_CRC_APPEND |
                                BITM_MAC_CONFIG2_LO_PRIO_FIFO_CRC_APPEND : 0U) |
                               BITM_MAC_CONFIG2_FWD_UNK2HOST | BITM_MAC_CONFIG2_TX_RDY_ON_EMPTY;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enables VLAN filtering */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetVlanFilter(uint32_t nDevNum, uint16_t nVlanId1, uint16_t nVlanId2,
        bool bDropFrames, bool bEnable)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Set the VLAN filter table entries
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = MAC_RXFILT_VID_TABLE_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_RXFILT_VID_TABLE_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = ((uint32_t)nVlanId1 << BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY1) |
                               ((uint32_t)nVlanId2 << BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_ENTRY2) |
                               ((uint32_t)(bDropFrames == true ? 1U : 0U) << BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_RULE) | /* Rule for VLAN filter */
                               ((uint32_t)(bEnable == true ? 1U : 0U) << BITP_MAC_RXFILT_VID_TABLE_RXFILT_VID_EN); /* Enable VLAN filtering */

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enables EtherType filtering */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetEtherTypeFilter(uint32_t nDevNum, uint16_t nEtherType1,
        uint16_t nEtherType2, bool bDropFrames, bool bEnable)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Set the EtherType filter table entries
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = MAC_RXFILT_ETYPE_TABLE_ADDR;
    poCtrlDesc->nLength      = 2U;
    poCtrlDesc->nMMS         = MAC_RXFILT_ETYPE_TABLE_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = ((uint32_t)nEtherType1 << BITP_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY1) |
                               ((uint32_t)nEtherType2 << BITP_MAC_RXFILT_ETYPE_TABLE_RXFILT_ETYPE_ENTRY2);
    poCtrlDesc->pRegData[1U] = ((uint32_t)(bDropFrames == true ? 1U : 0U) << BITP_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_RULE)
                               | /* Rule for VLAN filter */
                               ((uint32_t)(bEnable == true ? 1U : 0U) << BITP_MAC_RXFILT_ETYPE_CNRL_RXFILT_ETYPE_EN); /* Enable EtherType filtering */

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Send the controller node to sleep mode */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SleepNode(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_NETWORK_WKSLP_DATA *poWkslpData = &goNwData.aoDevInfo[nDevNum].oWkslpData;
    /* Set the OASPI sync as false - so that OASPI state machine doesn't proceed sending any frames out when the node is going to sleep state */
    adi_oaspi_SetSync(nInstNum, false);

    /* Only send to sleep if the current state of the node is awake */
    if(poWkslpData->eNodeState == ADI_NETWORK_CTRL_STATE_AWAKE)
    {
        poWkslpData->bGotoSleep = true;
    }
    /* Else set the network error as node not awake */
    else
    {
        eNwErr = ADI_NETWORK_NODE_NOT_AWAKE;
    }

    return eNwErr;
}

/*! Send the controller node to standby mode */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_StandbyNode(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    ADI_NETWORK_WKSLP_DATA *poWkslpData = &goNwData.aoDevInfo[nDevNum].oWkslpData;

    /* Only send to standby if the current state of the node is awake */
    if(poWkslpData->eNodeState == ADI_NETWORK_CTRL_STATE_AWAKE)
    {
        poWkslpData->bGotoStandby = true;
    }
    /* Else set the network error as node not awake */
    else
    {
        eNwErr = ADI_NETWORK_NODE_NOT_AWAKE;
    }

    return eNwErr;
}

/*! Send a wake-up pulse from the controller node onto T1S bus */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SendWakeUpPulse(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr;
    ADI_NETWORK_WKSLP_DATA *poWkslpData = &goNwData.aoDevInfo[nDevNum].oWkslpData;

    /* Only send the wake up pulse if the current state of the node is awake */
    if(poWkslpData->eNodeState == ADI_NETWORK_CTRL_STATE_AWAKE)
    {
        eNwErr = SendWkslpCommand(nDevNum, WKSLP_CMD_SEND_WUP);
    }
    /* Else set the network error as node not awake */
    else
    {
        eNwErr = ADI_NETWORK_NODE_NOT_AWAKE;
    }

    return eNwErr;
}

/*! Trigger wake-up pin of controller node */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_TriggerWakeUpPin(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_GPIO_WRITE_FAILED;
    uint32_t nId = goNwData.aoDevInfo[nDevNum].nId;
    /* Trigger the wake pin - generates a pulse on the GPIO to wake the node */
    ADI_PAL_STATUS ePalStatus = adi_pal_triggerWakePin(nId);

    /* Check if  the pal status returned is OK */
    if(ePalStatus == ADI_PAL_OK)
    {
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Ping controller node */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_PingControllerNode(uint32_t nDevNum)
{
    /* Ping the node */
    return PingNode(nDevNum);
}

/*! Get reason for sleep / wake */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetReasonForWkslp(uint32_t nDevNum)
{
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    /* Get the reason for wake and sleep */
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_SLEEP_HISTORY_MMS;
    poCtrlDesc->nStartAddr   = IO_SLEEP_HISTORY_ADDR;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    /* Callback function for this OASPI to notify about the register read data */
    poCtrlDesc->pfCallback   = HandleWkSlpCallback;

    ADI_OASPI_STATUS eOaspiStatus = adi_oaspi_ReadReg(nInstNum, poCtrlDesc);

    // Check if the status is success, to increment the queue index
    if(eOaspiStatus == ADI_OASPI_STATUS_SUCCESS)
    {
        eNwErr = ADI_NETWORK_OK;
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }

    return eNwErr;
}

/*! Set MAC FIFO(s) size in MACPHY Mode */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetMacFIFOSize_Macphy(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_MACPHY_CONFIG *poFIFOSize)
{
    // Precondition to make sure the allocation doesn't exceed the maximum size avaliable in MACPHY
    ADI_DBG_REQUIRE(((uint32_t)poFIFOSize->eHighPriorityTXSize + (uint32_t)poFIFOSize->eLowPriorityTXSize +
                     (uint32_t)poFIFOSize->eRXFIFOSize + (uint32_t)poFIFOSize->ePtpRxFifoSize) <= 8U,
                    " Allocation of sizes for FIFO is bad! ");
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Configure the size of MAC FIFOs
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = MAC_FIFO_SIZE_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_FIFO_SIZE_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    /* Set the FIFO sizes */
    poCtrlDesc->pRegData[0U] = (uint32_t)(((uint32_t)poFIFOSize->eRXFIFOSize << BITP_MAC_FIFO_SIZE_T1S_2_HOST_SIZE) |
                                          ((uint32_t)poFIFOSize->ePtpRxFifoSize << BITP_MAC_FIFO_SIZE_T1S_2_PTP_SIZE) |
                                          ((uint32_t)poFIFOSize->eLowPriorityTXSize << BITP_MAC_FIFO_SIZE_LOW_TX_FIFO_SIZE) |
                                          ((uint32_t)poFIFOSize->eHighPriorityTXSize << BITP_MAC_FIFO_SIZE_HIGH_TX_FIFO_SIZE));

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        // Configure the maximum TXC values based on the LP and HP FIFO sizes
        uint8_t nMaxTxcLp = (poFIFOSize->eLowPriorityTXSize == ADI_NETWORK_MACPHY_FIFO_SIZE_0K) ? 0U :
                            (poFIFOSize->eLowPriorityTXSize == ADI_NETWORK_MACPHY_FIFO_SIZE_2K) ? 29U : 31U;

        nMaxTxcLp = ((nMaxTxcLp == 29U) && (goNwData.aoDevInfo[nDevNum].nChunkSize < 64U)) ? 31U : nMaxTxcLp;

        uint8_t nMaxTxcHp = (poFIFOSize->eHighPriorityTXSize == ADI_NETWORK_MACPHY_FIFO_SIZE_0K) ? 0U :
                            (poFIFOSize->eHighPriorityTXSize == ADI_NETWORK_MACPHY_FIFO_SIZE_2K) ? 29U : 31U;

        nMaxTxcHp = ((nMaxTxcHp == 29U) && (goNwData.aoDevInfo[nDevNum].nChunkSize < 64U)) ? 31U : nMaxTxcHp;

        // Notify the OASPI driver about the maximum TXC values
        adi_oaspi_ConfigureMaxTxc(nInstNum, nMaxTxcLp, nMaxTxcHp);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Set MAC FIFO(s) size in DUAL Mode */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetMacFIFOSize_DualMode(uint32_t nDevNum,
        ADI_NETWORK_MACPHY_FIFO_SIZE_DUAL_MODE_CONFIG *poFIFOSize)
{
    // Precondition to make sure the allocation doesn't exceed the maximum size avaliable in MACPHY
    ADI_DBG_REQUIRE(((uint32_t)poFIFOSize->eOaspiToT1sOrLceSize + (uint32_t)poFIFOSize->eT1sToOaspiSize +
                     (uint32_t)poFIFOSize->eLceToOaspiSize + (uint32_t)poFIFOSize->eLceToT1sSize +
                     (uint32_t)poFIFOSize->eT1sToLceSize) <= 8U,
                    " Allocation of sizes for FIFO is bad! ");
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Configure the size of MAC FIFOs
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = MAC_FIFO_SIZE_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_FIFO_SIZE_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    /* Set the FIFO sizes */
    poCtrlDesc->pRegData[0U] = (uint32_t)((uint32_t)poFIFOSize->eOaspiToT1sOrLceSize |
                                          ((uint32_t)poFIFOSize->eT1sToOaspiSize << 4u) |
                                          ((uint32_t)poFIFOSize->eLceToOaspiSize << 12u) |
                                          ((uint32_t)poFIFOSize->eLceToT1sSize << 20u) |
                                          ((uint32_t)poFIFOSize->eT1sToLceSize << 24u)) ;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;

        // Configure the maximum TXC value based on the FIFO size (single FIFO mode)
        uint8_t nMaxTxc = (poFIFOSize->eOaspiToT1sOrLceSize == ADI_NETWORK_MACPHY_FIFO_SIZE_0K) ? 0U :
                          (poFIFOSize->eOaspiToT1sOrLceSize == ADI_NETWORK_MACPHY_FIFO_SIZE_2K) ? 29U : 31U;

        nMaxTxc = ((nMaxTxc == 29U) && (goNwData.aoDevInfo[nDevNum].nChunkSize < 64U)) ? 31U : nMaxTxc;

        // Notify the OASPI driver about the maximum TXC value (HP=0 for single FIFO)
        adi_oaspi_ConfigureMaxTxc(nInstNum, nMaxTxc, 0U);
    }

    return eNwErr;
}

/*! Reconfigure the node */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_Reconfigure(uint32_t nDevNum, const ADI_NETWORK_MACPHY_CFG *poNwMacPhyCfg)
{
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /* Re-initialize OA-SPI */
    // adi_oaspi_ReInit(nInstNum);

    /* Make the error state and init status as false */
    goNwData.aoDevInfo[nDevNum].bInErrState = false;
    goNwData.aoDevInfo[nDevNum].bInitComplete = false;
    goNwData.paoNwMacPhyCfg[nInstNum] = poNwMacPhyCfg;

    /* Set the sync bit in OASPI as false - this will be set to true in configure node function */
    adi_oaspi_SetSync(nInstNum, false); // TODO: Check if this is needed

    return ConfigureNode(nInstNum, nDevNum, poNwMacPhyCfg);
}

/*! Sets PLCA Transmit Opportunity Timer */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetPlcaToTimer(uint32_t nDevNum, uint32_t nToTime)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Set the PLCA transmit opportunity time
    // This is the time in which the node has to perform a commit, inorder to transfer it's data during it's slot, otherwise the transmit opurtunity goes to the next node
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_OA_PLCA_TOTMR_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_OA_PLCA_TOTMR_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = nToTime << BITP_PHY_OA_PLCA_TOTMR_PLCATOTIMER;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Sets PLCA Burst Timer */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetPlcaBurstTimer(uint32_t nDevNum, uint8_t nMaxBurstCount, uint8_t nBurstTime)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Set the PLCA burst timer
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_OA_PLCA_BURST_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_OA_PLCA_BURST_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    /* Configure max burst count and burst timer */
    poCtrlDesc->pRegData[0U] = ((uint32_t)nMaxBurstCount << BITP_PHY_OA_PLCA_BURST_PLCAMAXBURSTCNT) |
                               ((uint32_t)nBurstTime << BITP_PHY_OA_PLCA_BURST_PLCABURSTTIMER);

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Sets PLCA leader mode by performing MAC-PHY register read and masked write at given bit postions */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetPlcaLeaderMode(uint32_t nDevNum, bool bPlcaLeader, bool bPlcaLeaderModeEn)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    ADI_NETWORK_MASKED_WR_DATA *poMaskedWrData = &gaoMaskedWrData[goNwData.nCtrlWrIdx];

    /* Set the PLCA leader mode */
    poCtrlDesc->nStartAddr   = PHY_OA_PLCA_EXT_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_OA_PLCA_EXT_MMS;
    poCtrlDesc->pCbParam     = poMaskedWrData;
    /* Set masked write callback function */
    poCtrlDesc->pfCallback   = HandleMaskedWrRegCallback;

    /* Set register data mask */
    poMaskedWrData->anRegMapMask[0] = ((uint32_t)BITM_PHY_OA_PLCA_EXT_PLCALEADER) | ((uint32_t)
                                      BITM_PHY_OA_PLCA_EXT_PLCALEADEREN);
    /* Set register data */
    poMaskedWrData->anRegWrData[0] = ((uint32_t)(bPlcaLeader == true ? BITM_PHY_OA_PLCA_EXT_PLCALEADER : 0U)) | ((
                                         uint32_t)(bPlcaLeaderModeEn == true ? BITM_PHY_OA_PLCA_EXT_PLCALEADEREN : 0U));

    /* Read register data to perform masked write */
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enable PLCA precedence mode by performing MAC-PHY register read and masked write at given bit postions */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnablePlcaPrecedence(uint32_t nDevNum, bool bPlcaPrecedenceEn)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    ADI_NETWORK_MASKED_WR_DATA *poMaskedWrData = &gaoMaskedWrData[goNwData.nCtrlWrIdx];

    /* Enable PLCA precedence mode */
    poCtrlDesc->nStartAddr   = PHY_OA_PLCA_EXT_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_OA_PLCA_EXT_MMS;
    poCtrlDesc->pCbParam     = poMaskedWrData;
    /* Set masked write callback function */
    poCtrlDesc->pfCallback   = HandleMaskedWrRegCallback;

    /* Set register data mask */
    poMaskedWrData->anRegMapMask[0] = (uint32_t)(BITM_PHY_OA_PLCA_EXT_PLCAPRECEN);
    /* Set register data */
    poMaskedWrData->anRegWrData[0] = (uint32_t)(bPlcaPrecedenceEn  == true ? BITM_PHY_OA_PLCA_EXT_PLCAPRECEN : 0U);

    /* Read register data to perform masked write */
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Gets PHY Status */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetPhyStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Get the PHY status
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_STD_STATUS_REGISTER_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_STD_STATUS_REGISTER_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleStatsRegRead; // Callback function to handle the read status register data

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Gets PCS Status */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetPcsStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Get the PCS status
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_STD_PCS_STATUS_ADDR;
    poCtrlDesc->nLength      = 3U;
    poCtrlDesc->nMMS         = PHY_STD_PCS_STATUS_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleStatsRegRead; // Callback function to handle the read status register data

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enables the Test Mode Control */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnableTestMode(uint32_t nDevNum, ADI_NETWORK_TESTMODE eTestMode)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_OASPI_CTRL_DESC *poCtrlDesc;

    do
    {
        // Update the tweaks register for square wave and wake up tone test modes only
        if(eTestMode == ADI_NETWORK_TEST_SEND_SQUARE_WAVE || eTestMode == ADI_NETWORK_TEST_WAKE_UP_TONE)
        {
            poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
            poCtrlDesc->nStartAddr   = PHY_OA_TWEAKS_1_ADDR;
            poCtrlDesc->nLength      = 1U;
            poCtrlDesc->nMMS         = PHY_OA_TWEAKS_1_MMS;
            poCtrlDesc->pCbParam     = goNwData.pfCbk;
            poCtrlDesc->pfCallback   = OaspiCallback;
            poCtrlDesc->pRegData[0U] = PHY_OA_TWEAKS_1_RESET_VAL | ((uint32_t)1u << 12u);

            // Check if the status is success, to increment the queue index
            if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
            {
                eNwErr = ADI_NETWORK_NO_SPACE;
                break;
            }

            /* Increment the control descriptor queue by one */
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }

        // Set the test mode based on the input from application
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_STD_T1S_TEST_MODE_CONTROL_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = PHY_STD_T1S_TEST_MODE_CONTROL_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = (uint32_t)eTestMode << BITP_PHY_STD_T1S_TEST_MODE_CONTROL_PMAPATTERNSEL;

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        /* Increment the control descriptor queue by one */
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;

}

/*! Sets PMA Control */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetPmaControl(uint32_t nDevNum, bool bPmaReset, bool bTransmitDisable)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    // Get the descriptor from the OASPI control descriptor queue
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_STD_T1S_PMA_CONTROL_ADDR ;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_STD_T1S_PMA_CONTROL_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    // Configure the PMA control - reset / disable transmit
    poCtrlDesc->pRegData[0U] = ((uint32_t)(bPmaReset ? 1U : 0U) << BITP_PHY_STD_T1S_PMA_CONTROL_SRESET_1) |
                               ((uint32_t)(bTransmitDisable ? 1U : 0U) << BITP_PHY_STD_T1S_PMA_CONTROL_PMATXDISABLE) |
                               PHY_STD_T1S_PMA_CONTROL_RESET_VAL;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enables MAC Loopback */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnableMacLoopBack(uint32_t nDevNum, bool bMacLoopback)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Set the MAC loopback bit - shorts TX and RX path of MAC so that data is looped back
    // When Mac loopback is enabled, the MAC will not receive any data from the PHY - essentially RX path is blocked
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = MAC_LOOP_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_LOOP_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = (bMacLoopback ? 1U : 0U); // Enable/disable mac loopback

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enables PMA Loopback */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnablePmaLoopBack(uint32_t nDevNum, bool bPmaLoopback)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Set the PMA loopback bit - shorts TX and RX path of PMA(sublayer of PHY) so that data is looped back
    // RX path is still open to receive data from the PHY in the loopback mode
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_STD_T1S_PMA_CONTROL_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_STD_T1S_PMA_CONTROL_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    // Set the PMA loopback bit
    poCtrlDesc->pRegData[0U] = (uint32_t)(bPmaLoopback ? 1U : 0U) << BITP_PHY_STD_T1S_PMA_CONTROL_PMALOOPBACK_1 |
                               PHY_STD_T1S_PMA_CONTROL_RESET_VAL;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enables PCS Loopback */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnablePcsLoopBack(uint32_t nDevNum, bool bPcsLoopback)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Set the PCS loopback bit - shorts TX and RX path of PCS(sublayer of PHY) so that data is looped back
    // RX path is still open to receive data from the PHY in the loopback mode
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_STD_PCS_CONTROL_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_STD_PCS_CONTROL_MMS ;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = (uint32_t)(bPcsLoopback ? 1U : 0U) << BITP_PHY_STD_PCS_CONTROL_PCSLOOPBACK;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Captures external timestamp */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_CaptureExtTimestamp(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        uint32_t nSpiId = goNwData.aoDevInfo[nDevNum].nId;
        // Generates a short pulse on the TS_CAPT pin , to trigger timestamp capture on the MACPHY device
        ADI_PAL_STATUS eTimeCptPin = adi_pal_triggerTsCaptPin(nSpiId);

        // Check if return from pal is OK
        if(eTimeCptPin != ADI_PAL_OK)
        {
            eNwErr = ADI_NETWORK_GPIO_WRITE_FAILED;
            break;
        }

        /* Captures external timestamp */
        uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = MAC_TS_EXT_CAPT0_ADDR;
        poCtrlDesc->nLength      = 2U;
        poCtrlDesc->nMMS         = MAC_TS_EXT_CAPT0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = HandleTimeStampCallback; // Callback function to handle the timestamp data

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;
}

/*! Enable TS_TIMER */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnableTsTimer(uint32_t nDevNum, uint32_t nFreq, uint8_t  nDutyCycle,
        uint32_t nStartTime)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
        ADI_OASPI_CTRL_DESC *poCtrlDesc;

        // Calculate the period based on the frequency given for square wave signal
        double nPeriod = (1.0 / (double)nFreq) * (1000000000.0);
        uint32_t nTimePeriod = (uint32_t)(nPeriod);

        // Calculate the high time and low time for square wave signal based on the duty cycle given
        double nTimHigh = ((double)nDutyCycle / 100.0) * nPeriod;
        uint32_t nTimerHigh = (uint32_t)(nTimHigh);
        uint32_t nTimerLow = nTimePeriod - nTimerHigh;

        /* Calculate the quantization error for the given configuration since the high and low time
           need to be programmed in multiples of 16 because the counters are driven by a 62.5 MHz clock
           and increment in steps of 16ns */
        uint32_t nNearest = ((nTimerHigh + nTimerLow) + 15U) & (0xFFFFFFF0U);
        uint32_t QE = nNearest - nTimePeriod;

        // Enable TS_TIMER, generates a square wave signal on the TS_TIMER pin with given frequency and duty cycle
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = MAC_TS_TIMER_HI_ADDR;
        poCtrlDesc->nLength      = 4U;
        poCtrlDesc->nMMS         = MAC_TS_TIMER_HI_MMS ;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = nTimerHigh;
        poCtrlDesc->pRegData[1U] = nTimerLow ;
        poCtrlDesc->pRegData[2U] = QE;
        poCtrlDesc->pRegData[3U] = nStartTime ;

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;
}

/*! Stop Toggling the TS_TIMER output */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_StopTsTimer(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    ADI_NETWORK_MASKED_WR_DATA *poMaskedWrData = &gaoMaskedWrData[goNwData.nCtrlWrIdx];

    /* To set the TS_TIMER_STOP bit to stop the signal on the TS_TIMER pin of
    the MACPHY */
    poCtrlDesc->nStartAddr   = MAC_TS_CFG_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = MAC_TS_CFG_MMS ;
    poCtrlDesc->pCbParam     = poMaskedWrData;
    /* Set masked write callback function */
    poCtrlDesc->pfCallback   = HandleMaskedWrRegCallback;

    /* Set register data mask */
    poMaskedWrData->anRegMapMask[0] = (uint32_t)(BITM_MAC_TS_CFG_TS_TIMER_STOP);
    /* Set register data */
    poMaskedWrData->anRegWrData[0] = (uint32_t)(BITM_MAC_TS_CFG_TS_TIMER_STOP);

    /* Read register data to perform masked write */
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Enables 1 PPS output */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnablePpsOutput(uint32_t nDevNum, uint8_t nSAIF)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    ADI_OASPI_CTRL_DESC *poCtrlDesc;

    do
    {
        /* Enable SA_IFn to output 1PPS (pulse per second) debug signal */
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = 0xC801u;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = 0xAu;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = ((uint32_t)1U << (uint32_t)nSAIF);

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        /* Increment the control descriptor queue by one */
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* Enable 1 PPS output */
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = 0xC840u + ((uint16_t)nSAIF / 2U);
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = 0xAu;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = (nSAIF % 2U == 0U) ? 0x7F37U : 0x377FU;

        // Check if the status is success, to increment the queue index
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        /* Increment the control descriptor queue by one */
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;
}

/*! Sets the configuration required for the safe mode, which are enabling the monitoring of pins, polarity, configuration of SA_IF pins in safe mode, HW_ADC mux and configurations and auto return settings */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_sscSetConfig(uint32_t nDevNum,
        const ADI_NETWORK_MACPHY_SSC_MONITOR_CONFIG *poSscSetupCfg)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    ADI_OASPI_CTRL_DESC *poCtrlDesc;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = IO_SSC_SAFE_SA_IF_CONTROLS_ADDR;
    poCtrlDesc->nLength      = 14U;
    poCtrlDesc->nMMS         = IO_SSC_SAFE_SA_IF_CONTROLS_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    /* Set SAIF pin configuration in safe state */
    poCtrlDesc->pRegData[0U] = (poSscSetupCfg->nSAIFConfig[4] <<
                                BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_4_CONTROLS_MUX) |
                               (poSscSetupCfg->nSAIFConfig[3] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_3_CONTROLS_MUX) |
                               (poSscSetupCfg->nSAIFConfig[2] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_2_CONTROLS_MUX) |
                               (poSscSetupCfg->nSAIFConfig[1] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_SSC_SAFE_SA_IF_1_CONTROLS_MUX) |
                               poSscSetupCfg->nSAIFConfig[0];
    poCtrlDesc->pRegData[1U] = (poSscSetupCfg->nSAIFConfig[9] <<
                                BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_9_CONTROLS_MUX) |
                               (poSscSetupCfg->nSAIFConfig[8] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_8_CONTROLS_MUX) |
                               (poSscSetupCfg->nSAIFConfig[7] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_7_CONTROLS_MUX) |
                               (poSscSetupCfg->nSAIFConfig[6] << BITP_IO_SSC_SAFE_SA_IF_CONTROLS_2_SSC_SAFE_SA_IF_6_CONTROLS_MUX) |
                               poSscSetupCfg->nSAIFConfig[5];
    poCtrlDesc->pRegData[2U] = (poSscSetupCfg->nSAIFConfig[11] <<
                                BITP_IO_SSC_SAFE_SA_IF_CONTROLS_3_SSC_SAFE_SA_IF_11_CONTROLS_MUX) |
                               poSscSetupCfg->nSAIFConfig[10];
    /* Set polorities for the triggers that are monitored for triggering safe sate */
    poCtrlDesc->pRegData[3U] = poSscSetupCfg->nMonitorPolarity & BITM_IO_SSC_MONITOR_POLARITY_SSC_MONITOR_POLARITY;
    poCtrlDesc->pRegData[4U] = poSscSetupCfg->nMonitorPolarity >> 16U;
    /* Set the timeout for the monitoring the ethernet traffic/ device attended bit to trigger safe state */
    poCtrlDesc->pRegData[5U] = poSscSetupCfg->nEthFrameCheckTimeout;
    poCtrlDesc->pRegData[6U] = poSscSetupCfg->nDeviceUnattendedTimeout;
    /* Blank writes to registers that are not used in the seqeunce of multiple writes for safe state controller */
    poCtrlDesc->pRegData[7U] = 0U;
    poCtrlDesc->pRegData[8U] = 0U;
    /* Set auto return to functional state from safe state once the triggers for safe state are gone */
    poCtrlDesc->pRegData[9U] = poSscSetupCfg->nAutoReturnToFunc;
    poCtrlDesc->pRegData[10U] = (((poSscSetupCfg->nAutoReturnToFunc & ADI_SSC_GPTP_UNLOCKED) >> 20U) | ((
                                     poSscSetupCfg->nAutoReturnToFunc &
                                     ADI_SSC_ETH_FRAME_CHECK) >> 29U));
    /* Set whether SAIF take safe state configurations in safe mode , also select triggers that are monitored for safe state*/
    poCtrlDesc->pRegData[11U] = poSscSetupCfg->nSAIFSafeEn | ((uint32_t)poSscSetupCfg->nSQISafeModeLvlTrigger <<
                                BITP_IO_SSC_CONTROLS_SSC_SQI_MON_YELLOW_RED) | ((uint32_t)(poSscSetupCfg->bDetectBeaconAsEthTraffic == true ?
                                    1U : 0U) << BITP_IO_SSC_CONTROLS_SSC_BEACON_TIMEOUT_CHECK) | IO_SSC_CONTROLS_RESET_VAL;
    poCtrlDesc->pRegData[12U] = poSscSetupCfg->nMonitorEnable & BITM_IO_SSC_SAFE_CHECKS_ENABLE_SSC_MONITOR_ENABLE;
    poCtrlDesc->pRegData[13U] = poSscSetupCfg->nMonitorEnable >> 16U;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*!  Manual return to Functional state from safe state */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_sscReturnToFunctional(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /* Manually return to functional state from safe state */
    /* If the trigger for safe state is still alive, then return to functional mode can be unsuccessful */
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = IO_SSC_CONTROLS_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_SSC_CONTROLS_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = (uint32_t)1U << BITP_IO_SSC_CONTROLS_SSC_RETURN_TO_FUNCTIONAL_MODE;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Force safe state mode */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_sscForceSafeState(uint32_t nDevNum, bool bForceSafeState)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Force safe state mode using manual trigger for safe state
    // if bForceSafeState is set to false, then the manual trigger is withdrawn - otherwise enforces the trigger to switch to safe state
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    uint32_t ForceSafeState = (bForceSafeState == true ? 1U : 0U);
    poCtrlDesc->nStartAddr   = IO_SSC_MONITOR_TRIGGERS_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_SSC_MONITOR_TRIGGERS_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = ForceSafeState << BITP_IO_SSC_MONITOR_TRIGGERS_SSC_MANUAL_ENTER_SAFE_MODE;

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Alive counter readback */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_sscAliveCounterRead(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // Read the alive counter value
    // Alive counter value runs only in functional state, but not in safe state
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = IO_SSC_ALIVE_COUNTER_RB_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_SSC_ALIVE_COUNTER_RB_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = HandleSscCallback; // Callback function to handle the alive counter readback

    // Check if the status is success, to increment the queue index
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Check the safe state controller status */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_sscCheckSafeModeStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;

    do
    {
        uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

        /* Read the safe state controller status registers - contains the status of triggers for safe state */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = IO_SSC_SAFE_CHECKS_TRIGGER_ADDR;
        poCtrlDesc->nLength      = 3U;
        poCtrlDesc->nMMS         = IO_SSC_SAFE_CHECKS_TRIGGER_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        /* Callback function to handle the safe state controller status readback data */
        poCtrlDesc->pfCallback   = HandleSscCallback;

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* Clear the sticky readbacks after reading the status registers */
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = IO_SSC_MISC_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = IO_SSC_MISC_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = 1U << BITP_IO_SSC_MISC_SSC_CLEAR_ALL_TRIGGERS;

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    } while(false);

    return eNwErr;
}

/*! Writes to the device unattended bit */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_sscMarkDeviceAttended(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /* Set the device attended bit on MACPHY */
    /* This can be used as a trigger for safe state as this bit periodically monitored
       and if there is timeout (the time with in which the bit has to be set - can be configured by user)
       triggers MACPHY to safe mode, if this enabled to monitor by safe state controller */
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = IO_SSC_MISC_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = IO_SSC_MISC_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;
    poCtrlDesc->pRegData[0U] = 1U;

    /* Check if the status is success, to increment the queue index */
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/*! Configures Threshold and selects the node for SQI level */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_ConfigureSqi(uint32_t nDevNum,
        const ADI_NETWORK_MACPHY_SQI_CFG *poSqiCfg)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    goNwData.aoDevInfo[nDevNum].oSqiData.nSqiSelPlcaId = poSqiCfg->nSqiSelPlcaId; /* PLCA ID's TO to monitor */

    do
    {
        /* Configure the thresholds for good, warning and bad SQI levels and
           settle range which indicates the number of frames to be received in order
           to set the settle flag (which indicates the SQI value is stable to read out) */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_DCQ_JM_CFG1_ADDR;
        poCtrlDesc->nLength      = 5U;
        poCtrlDesc->nMMS         = PHY_OA_DCQ_JM_CFG1_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = (uint32_t)poSqiCfg->eSqiSettleRange;
        poCtrlDesc->pRegData[1U] = (uint32_t)poSqiCfg->nGoodToWarningLvl; /* Good to warning level threshold */
        poCtrlDesc->pRegData[2U] = (uint32_t)poSqiCfg->nWarningToBadLvl;  /* Warning to bad level threshold */
        poCtrlDesc->pRegData[3U] = (uint32_t)poSqiCfg->nBadToWarningLvl;  /* Bad to warning level threshold */
        poCtrlDesc->pRegData[4U] = (uint32_t)poSqiCfg->nWarningToGoodLvl; /* Warning to goof level threshold */

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* Configure the PLCA TO ID's to be monitored and enable raw jitter monitor */
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_DCQ_JM_CFG0_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = PHY_OA_DCQ_JM_CFG0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = (uint32_t)poSqiCfg->nSqiSelPlcaId << BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_PLCA_ID_SEL |
                                   BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN;

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    } while(false);

    return eNwErr;
}

/*! Gets SQI index */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetSqiIndex(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    do
    {
        /* Pause the jitter monitor */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_DCQ_JM_CFG0_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = PHY_OA_DCQ_JM_CFG0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_PAUSE | ((uint32_t)
                                   goNwData.aoDevInfo[nDevNum].oSqiData.nSqiSelPlcaId << BITP_PHY_OA_DCQ_JM_CFG0_CFG_JM_PLCA_ID_SEL) |
                                   BITM_PHY_OA_DCQ_JM_CFG0_CFG_JM_EN;

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* Read back the SQI level and status of valid and settle flag (which indicate the stability of the readback value) */
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_DCQ_JM_RB0_ADDR;
        poCtrlDesc->nLength      = 5U;
        poCtrlDesc->nMMS         = PHY_OA_DCQ_JM_RB0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = HandleSqiCallback; /* Callback function to handle the SQI readback data */

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;
}

/*! Disable SQI */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_disableSqi(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    // TODO: check whether this function is correct?
    do
    {
        /* Configure the SQI block with the default value */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = PHY_OA_DCQ_JM_CFG0_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = PHY_OA_DCQ_JM_CFG0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;
        poCtrlDesc->pRegData[0U] = PHY_OA_DCQ_JM_CFG0_RESET_VAL;

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

    } while(false);

    return eNwErr;
}

/*! Get Revision number */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetRevision(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    do
    {
        /* Read the revision number of the MACPHY */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = MAC_SCRATCH_0_ADDR;
        poCtrlDesc->nLength      = 2U;
        poCtrlDesc->nMMS         = MAC_SCRATCH_0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = HandleMiscCallback; /* Callback function to handle the revision number readback */

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;
}

/*! Get bootloader status */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetBootLoaderStatus(uint32_t nDevNum)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_OK;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    do
    {
        /* HV Die bootloader status */
        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = IO_HV_BOOTLOADER_STATUS_0_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = IO_HV_BOOTLOADER_STATUS_0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = HandleBootloaderCallback; /* Callback function to handle the bootloader status readback */

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);

        /* LV Die bootloader status */
        poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = IO_BOOTLOADER_STATUS_0_ADDR;
        poCtrlDesc->nLength      = 8U;
        poCtrlDesc->nMMS         = IO_BOOTLOADER_STATUS_0_MMS;
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = HandleBootloaderCallback; /* Callback function to handle the bootloader status readback */

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) != ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_NO_SPACE;
            break;
        }

        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    } while(false);

    return eNwErr;
}

/* Start measurement for topology discovery */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_StartTopoDiscMeas(uint32_t nDevNum, TOPO_DISC_MEAS_TYPE eTopoDiscMeasType)
{
    /* Store the time duration in network internal data - this is used in calculating the delay and distance */
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    uint32_t nTopoMeasDuration = (uint32_t)goNwData.aoDevInfo[nDevNum].oTopoDiscData.nTopoMeasDuration;

    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_OA_TD_CTRL_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_OA_TD_CTRL_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;

    /* Populate the regdata depending on the type of measurement */
    switch(eTopoDiscMeasType)
    {
        case TOPO_DISC_INTERNAL_DELAY:
        {
            /* Start internal delay measurement with the time duration set */
            poCtrlDesc->pRegData[0U] = (nTopoMeasDuration << BITP_PHY_OA_TD_CTRL_TD_DM_DUR) |
                                       BITM_PHY_OA_TD_CTRL_TD_DLYM_START |
                                       BITM_PHY_OA_TD_CTRL_TD_EN;
            break;
        }

        case TOPO_DISC_DISTANCE:
        {
            /* Start distance with the time duration set */
            poCtrlDesc->pRegData[0U] = (nTopoMeasDuration << BITP_PHY_OA_TD_CTRL_TD_DM_DUR) |
                                       BITM_PHY_OA_TD_CTRL_TD_DM_START |
                                       BITM_PHY_OA_TD_CTRL_TD_EN;
            break;
        }

        case TOPO_DISC_AUTOMATIC:
        {
            /* Start automatic mode with the time duration set */
            poCtrlDesc->pRegData[0U] = (nTopoMeasDuration << BITP_PHY_OA_TD_CTRL_TD_DM_DUR) |
                                       BITM_PHY_OA_TD_CTRL_TD_AUTO_START |
                                       BITM_PHY_OA_TD_CTRL_TD_EN;
            break;
        }

        default:
        {
            /* Invalid topology discovery mode */
            ADI_DBG_LOG(LOG_ERROR, stderr, "Invalid topology discovery mode: %d", eTopoDiscMeasType);
            break;
        }
    }

    /* If the API is being invoked from a reference node - REF bit indicating the same should be added as the register write */
    if(goNwData.aoDevInfo[nDevNum].oTopoDiscData.bIsTopoRefNode)
    {
        poCtrlDesc->pRegData[0U] |= BITM_PHY_OA_TD_CTRL_TD_REFN;
    }
    else
    {
        /* Nothing to do */
    }

    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        eNwErr = ADI_NETWORK_OK;
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }
    else
    {
        /* Nothing to do */
    }

    return eNwErr;
}

/*  Gets the measurement results for topology discovery */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_GetTopoDiscMeas(uint32_t nDevNum)
{
    /* Reads stats and measurement results */
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /* Read the stats register along with measurement results to see if there is any error */
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_OA_TD_STAT_ADDR;
    poCtrlDesc->nLength      = 5U;
    poCtrlDesc->nMMS         = PHY_OA_TD_STAT_MMS;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    /* Callback function to handle the topology discovery measurement register readback */
    poCtrlDesc->pfCallback   = HandleTDRegCallback;

    /* Check if the status is success, to increment the queue index */
    if((adi_oaspi_ReadReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/* Sets/Clears TD bit that blocks TX path */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_EnableTDBit(uint32_t nDevNum, bool bEnTDBit)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;

    /* Depending on the bEnTDBit - TD enable bit either gets set/cleared */
    /* Set TD-EN bit - is done before starting the measurements */
    /* Clear TD-EN bit - done after a measurement to send/receive the data back from measured nodes */
    ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poCtrlDesc->nStartAddr   = PHY_OA_TD_CTRL_ADDR;
    poCtrlDesc->nLength      = 1U;
    poCtrlDesc->nMMS         = PHY_OA_TD_CTRL_MMS;
    poCtrlDesc->pRegData[0U] = bEnTDBit == true ? BITM_PHY_OA_TD_CTRL_TD_EN : 0U;
    poCtrlDesc->pCbParam     = goNwData.pfCbk;
    poCtrlDesc->pfCallback   = OaspiCallback;

    /* Check if the status is success, to increment the queue index */
    if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
    {
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        eNwErr = ADI_NETWORK_OK;
    }

    return eNwErr;
}

/* Enables/disables gPTP initial rate correction block */
ADI_NETWORK_MACPHY_CODE
ADI_NETWORK_ERR network_macphy_SetGPTPInitialRateCorrection(uint32_t nDevNum, bool bEnable)
{
    ADI_NETWORK_ERR eNwErr = ADI_NETWORK_NO_SPACE;
    uint32_t nInstNum = goNwData.aoDevInfo[nDevNum].nCtrlIdx;
    goNwData.aoDevInfo[nDevNum].bIsGPTPConfigRdRecv = false;

    /* Do a readback of the register */
    ADI_OASPI_CTRL_DESC *poReadGptpConfig = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
    poReadGptpConfig->nStartAddr   = IO_GPTP_CONFIG_ADDR;
    poReadGptpConfig->nLength      = 1U;
    poReadGptpConfig->nMMS         = IO_GPTP_CONFIG_MMS;
    poReadGptpConfig->pCbParam     = goNwData.pfCbk;
    poReadGptpConfig->pfCallback   = HandleMiscCallback; /* Callback function to handle the gPTP config readback */

    /* Check if the status is success, to increment the queue index */
    if((adi_oaspi_ReadReg(nInstNum, poReadGptpConfig)) == ADI_OASPI_STATUS_SUCCESS)
    {
        eNwErr = ADI_NETWORK_OK;
        IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
    }

    /* Do a flush so that the read descriptor is processed */
    adi_network_flushTxBuffer(nDevNum, 0ULL);

    /* Set gPTP initial rate ratio calculation and phase according to the enable bit */
    if(goNwData.aoDevInfo[nDevNum].bIsGPTPConfigRdRecv && (eNwErr == ADI_NETWORK_OK))
    {
        goNwData.aoDevInfo[nDevNum].bIsGPTPConfigRdRecv = false;
        eNwErr = ADI_NETWORK_NO_SPACE;

        ADI_OASPI_CTRL_DESC *poCtrlDesc = &gaoOaspiCtrlDesc[goNwData.nCtrlWrIdx];
        poCtrlDesc->nStartAddr   = IO_GPTP_CONFIG_ADDR;
        poCtrlDesc->nLength      = 1U;
        poCtrlDesc->nMMS         = IO_GPTP_CONFIG_MMS;
        poCtrlDesc->pRegData[0U] = (poReadGptpConfig->pRegData[0U] &
                                    ~(BITM_IO_GPTP_CONFIG_GPTP_RATE_RATIO_CALCULATION_ADJUST_PHASE |
                                      BITM_IO_GPTP_CONFIG_GPTP_INITIAL_RATE_RATIO_CALCULATION)) | (((uint32_t)(bEnable ? 1U : 0U)) <<
                                          BITP_IO_GPTP_CONFIG_GPTP_INITIAL_RATE_RATIO_CALCULATION) | (((uint32_t)(bEnable ? 1U : 0U)) <<
                                              BITP_IO_GPTP_CONFIG_GPTP_RATE_RATIO_CALCULATION_ADJUST_PHASE);
        poCtrlDesc->pCbParam     = goNwData.pfCbk;
        poCtrlDesc->pfCallback   = OaspiCallback;

        /* Check if the status is success, to increment the queue index */
        if((adi_oaspi_WriteReg(nInstNum, poCtrlDesc)) == ADI_OASPI_STATUS_SUCCESS)
        {
            eNwErr = ADI_NETWORK_OK;
            IncrementQIdx(&goNwData.nCtrlWrIdx, NETWORK_CTRL_Q_SIZE);
        }
    }

    return eNwErr;
}

#endif /* MACPHY_EN */
/** @} */

/**
 * EOF: www.analog.com
 */
