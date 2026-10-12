/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_common.c
 * \brief: Common library code
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup common
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "adi_common.h"
#include "adi_pal.h"
#include "adi_memmap.h"
#include "adi_dbg.h"

/*================================== DEFINES ================================*/

/*================================ DATA TYPES ===============================*/

/*=================================== CODE ==================================*/
ADI_COMMON_CODE
void adi_common_logMacStatus(ADI_MAC_STATUS_DATA *poStatusDat)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tMAC status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Frame count               :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Broadcast frame count     :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxBroadcastFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Multicast frame count     :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxMulticastFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Unicast frame count       :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxUnicastFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Single collision count    :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxSingleColCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Multiple collision count  :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxMultipleColCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Deferred transmit count   :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxDeferredXmitCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Late collision count      :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxLateColCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Excess collision count    :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxExcessColCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tTX Under-run count           :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nTxUnrCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX Frame count               :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX Broadcast frame count     :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxBroadcastFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX Multicast frame count     :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxMulticastFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX Unicast frame count       :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxUnicastFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX short error count         :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxShortErrCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX CRC error count           :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxCRCErrFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX align error count         :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxAlignErrFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX long error count          :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxLongErrFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX PHY error count           :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxPhyErrFrmCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX drop host full count      :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxDropHostFullCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX drop port full count      :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxDropPortFullCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX drop filter table count   :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxDropFilterTableCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX IFG error count           :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxInterFrameGapErrCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX preamble error count      :  " UINT32_FORMATTER "\r\n",
                poStatusDat->nRxPreambleErrCnt);
}

ADI_COMMON_CODE
void adi_common_logPhyStatus(ADI_PHY_STATUS_DATA *poStatusDat)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPHY status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSpeed 10Mbps half-duplex          :  %d\r\n", poStatusDat->bSpeed10m);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tUnidirectional Ability            :  %d\r\n", poStatusDat->bUniDirAbility);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPreamble Suppresion               :  %d\r\n", poStatusDat->bPreambleSuppr);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tLink Negotiation Complete         :  %d\r\n", poStatusDat->bLinkNegComplete);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRemote Fault Condition Detected   :  %d\r\n", poStatusDat->bRemoteJabber);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tAuto Negotiation Ability          :  %d\r\n", poStatusDat->bAutoNegAbility);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tLink Status                       :  %d\r\n", poStatusDat->bLinkStatus);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tLocal Jabber Event Detected       :  %d\r\n", poStatusDat->bLocalJabber);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tExtended Capabilities             :  %d\r\n", poStatusDat->bExtendedCapab);

}

ADI_COMMON_CODE
void adi_common_logPlcaStatus(ADI_PLCA_STATUS_DATA *poStatusDat)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPLCA status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPLCA status                  :  %d\r\n", poStatusDat->bPlcaStatus);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tBeacon received before TO    :  %d\r\n",
                poStatusDat->bBeaconRcvdBeforeTO);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tUnexpected beacon received   :  %d\r\n",
                poStatusDat->bUnexpBeaconRcvd);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRX during local node TO      :  %d\r\n", poStatusDat->bRxInTO);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPLCA node ID                 :  %d\r\n", poStatusDat->nPlcaNodeId);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPLCA node count              :  %d\r\n",
                poStatusDat->nPlcaNodeCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPLCA TO timer                :  %d\r\n",
                poStatusDat->nPlcaTOTimer);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPLCA burst timer             :  %d\r\n",
                poStatusDat->nPlcaBurstTimer);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPLCA max burst count         :  %d\r\n",
                poStatusDat->nPlcaMaxBurstCnt);
}

ADI_COMMON_CODE
void adi_common_logPcsStatus(ADI_PCS_STATUS_DATA *poStatusDat)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPCS status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tJabDetected                    : %d \r\n", poStatusDat->bJabDetected);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tCorruptedTxCnt                 : %d \r\n", poStatusDat->nCorruptedTxCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tRemoteJabCnt                   : %d \r\n", poStatusDat->nRemoteJabCnt);
}

ADI_COMMON_CODE
void adi_common_logPtpStatus(ADI_PTP_STATUS_DATA *poStatusDat)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPTP status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tPTP locked                      : " "%d" "\r\n", poStatusDat->bLocked);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSync count                      : " UINT32_FORMATTER "\r\n",
                poStatusDat->nSyncCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tFollow-up count                 : " UINT32_FORMATTER "\r\n",
                poStatusDat->nFollowUpCnt);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of locks                 : " "%d" "\r\n", poStatusDat->nNumLocks);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tNumber of unlocks               : " "%d" "\r\n",
                poStatusDat->nNumUnlocks);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSequence ID when status changed : " "%d" "\r\n",
                poStatusDat->nStatusChangeSeqId);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tCurrent time error              : " UINT32_FORMATTER "\r\n",
                poStatusDat->nGptpTimeErrNs);
}

ADI_COMMON_CODE
void adi_common_logSscStatus(ADI_SSC_STATUS_DATA *poStatusData)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSSC Status:\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSafe Mode                       : " "%d" "\r\n", poStatusData->bSafeMode);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 0                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[0]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 1                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[1]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 2                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[2]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 3                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[3]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 4                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[4]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 5                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[5]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 6                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[6]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 7                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[7]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 8                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[8]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 9                          : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[9]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 10                         : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[10]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSAIF 11                         : " "%d" "\r\n",
                poStatusData->bTriggered_SAIF[11]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tHW_ADC_0                        : " "%d" "\r\n",
                poStatusData->bTriggered_HW_ADC[0]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tHW_ADC_1                        : " "%d" "\r\n",
                poStatusData->bTriggered_HW_ADC[1]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tHW_ADC_2                        : " "%d" "\r\n",
                poStatusData->bTriggered_HW_ADC[2]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tHW_ADC_3                        : " "%d" "\r\n",
                poStatusData->bTriggered_HW_ADC[3]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSW_ADC_0                        : " "%d" "\r\n",
                poStatusData->bTriggered_SW_ADC[0]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSW_ADC_1                        : " "%d" "\r\n",
                poStatusData->bTriggered_SW_ADC[1]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSW_ADC_2                        : " "%d" "\r\n",
                poStatusData->bTriggered_SW_ADC[2]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSW_ADC_3                        : " "%d" "\r\n",
                poStatusData->bTriggered_SW_ADC[3]);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tGPTP unlocked                   : " "%d" "\r\n",
                poStatusData->bTriggered_GPTP_UNLOCKED);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tOTP Config                      : " "%d" "\r\n",
                poStatusData->bTriggered_OTP_CONFIG);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tSQI lvl                         : " "%d" "\r\n",
                poStatusData->bTriggered_SQI_LVL);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tManual                          : " "%d" "\r\n", poStatusData->bTriggered_MANUAL);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tEth_frame_Check_Timeout         : " "%d" "\r\n",
                poStatusData->bTriggered_Eth_frame_Check_Timeout);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tDevice_Unattended_Timeout       : " "%d" "\r\n",
                poStatusData->bTriggered_Device_Unattended_Timeout);

}

ADI_COMMON_CODE
void adi_common_extractMacStatus(uint32_t *panData, ADI_MAC_STATUS_DATA *poStatusData)
{
    /* Get all the RX frame statistics */
    poStatusData->nRxFrmCnt              = panData[0u];   /* RxFrmCnt */
    poStatusData->nRxBroadcastFrmCnt     = panData[1u];   /* RxBroadcastFrmCnt */
    poStatusData->nRxMulticastFrmCnt     = panData[2u];   /* RxMulticastFrmCnt */
    poStatusData->nRxUnicastFrmCnt       = panData[3u];   /* RxUnicastFrmCnt */

    /* Get the RX error statistics */
    poStatusData->nRxCRCErrFrmCnt        = panData[4u];   /* RxCRCErrFrmCnt */
    poStatusData->nRxAlignErrFrmCnt      = panData[5u];   /* RxAlignErrFrmCnt */
    poStatusData->nRxPreambleErrCnt      = panData[6u];   /* RxPreambleErrCnt */
    poStatusData->nRxShortErrCnt         = panData[7u];   /* RxShortErrCnt */
    poStatusData->nRxLongErrFrmCnt       = panData[8u];   /* RxLongErrFrmCnt */
    poStatusData->nRxPhyErrFrmCnt        = panData[9u];   /* RxPhyErrFrmCnt */

    /* Get the RX frame drop statistics */
    poStatusData->nRxDropHostFullCnt     = panData[10u];  /* RxDropHostFullCnt */
    poStatusData->nRxDropPortFullCnt     = panData[11u];  /* RxDropPortFullCnt */
    poStatusData->nRxDropFilterTableCnt  = panData[12u];  /* RxDropFilterTableCnt */
    poStatusData->nRxInterFrameGapErrCnt = panData[13u];  /* RxInterFrameGapErrCnt */

    /* Get all the TX frame statistics */
    poStatusData->nTxFrmCnt              = panData[16u];  /* TxFrmCnt */
    poStatusData->nTxBroadcastFrmCnt     = panData[17u];  /* TxBroadcastFrmCnt */
    poStatusData->nTxMulticastFrmCnt     = panData[18u];  /* TxMulticastFrmCnt */
    poStatusData->nTxUnicastFrmCnt       = panData[19u];  /* TxUnicastFrmCnt */

    /* Get the TX error statistics */
    poStatusData->nTxSingleColCnt        = panData[20u];  /* TxSingleColCnt */
    poStatusData->nTxMultipleColCnt      = panData[21u];  /* TxMultipleColCnt */
    poStatusData->nTxDeferredXmitCnt     = panData[22u];  /* TxDeferredXmitCnt */
    poStatusData->nTxLateColCnt          = panData[23u];  /* TxLateColCnt */
    poStatusData->nTxExcessColCnt        = panData[24u];  /* TxExcessColCnt */
    poStatusData->nTxUnrCnt              = panData[25u];  /* TxUnrCnt */
}

ADI_COMMON_CODE
void adi_common_extractPlcaStatus(uint32_t *panData, ADI_PLCA_STATUS_DATA *poStatusData)
{
    poStatusData->nPlcaNodeId            = (uint8_t)(panData[0U] & 0x00FFU);
    poStatusData->nPlcaNodeCnt           = (uint8_t)((panData[0U] & 0xFF00U) >> 8U);
    poStatusData->bPlcaStatus            = ((panData[1U] & 0x8000U) == 0U) ? false : true;
    poStatusData->nPlcaTOTimer           = (uint8_t)(panData[2U] & 0x00FFU);
    poStatusData->nPlcaBurstTimer        = (uint8_t)(panData[3U] & 0x00FFU);
    poStatusData->nPlcaMaxBurstCnt       = (uint8_t)((panData[3U] & 0xFF00U) >> 8U);
    poStatusData->bBeaconRcvdBeforeTO    = ((panData[4U] & 0x01U) == 0U) ? false : true;
    poStatusData->bUnexpBeaconRcvd       = ((panData[4U] & 0x02U) == 0U) ? false : true;
    poStatusData->bRxInTO                = ((panData[4U] & 0x04U) == 0U) ? false : true;
}

ADI_COMMON_CODE
void adi_common_extractPhyStatus(uint32_t *panData, ADI_PHY_STATUS_DATA *poStatusData)
{
    poStatusData->bSpeed10m        = ((panData[0U] & 0x0800U) == 0U) ? false : true;
    poStatusData->bUniDirAbility   = ((panData[0U] & 0x0080U) == 0U) ? false : true;
    poStatusData->bPreambleSuppr   = ((panData[0U] & 0x0040U) == 0U) ? false : true;
    poStatusData->bLinkNegComplete = ((panData[0U] & 0x0020U) == 0U) ? false : true;
    poStatusData->bRemoteJabber    = ((panData[0U] & 0x0010U) == 0U) ? false : true;
    poStatusData->bAutoNegAbility  = ((panData[0U] & 0x0008U) == 0U) ? false : true;
    poStatusData->bLinkStatus      = ((panData[0U] & 0x0004U) == 0U) ? false : true;
    poStatusData->bLocalJabber     = ((panData[0U] & 0x0002U) == 0U) ? false : true;
    poStatusData->bExtendedCapab   = ((panData[0U] & 0x0001U) == 0U) ? false : true;
}

ADI_COMMON_CODE
void adi_common_extractPcsStatus(uint32_t *panData, ADI_PCS_STATUS_DATA *poStatusData)
{
    poStatusData->bJabDetected     = ((panData[0U] & 0x0080U) == 0U) ? false : true;
    poStatusData->nRemoteJabCnt    = (uint16_t)panData[1U];
    poStatusData->nCorruptedTxCnt  = (uint16_t)panData[2U];
}

ADI_COMMON_CODE
void adi_common_extractPtpStatus(uint32_t *panData, ADI_PTP_STATUS_DATA *poStatusData)
{
    poStatusData->nSyncCnt = (panData[1U] << 16U) | panData[0U];
    poStatusData->nFollowUpCnt = (panData[5U] << 16U) | panData[4U];
    poStatusData->nNumLocks = (uint16_t)panData[6U];
    poStatusData->nNumUnlocks = (uint16_t)panData[7U];
    poStatusData->nStatusChangeSeqId = (uint16_t)panData[8U];
    uint32_t nGptpTimeErr = (uint32_t)((panData[16U] << 16U) | panData[15U]);
    poStatusData->nGptpTimeErrNs = (int32_t)nGptpTimeErr;
    poStatusData->bLocked = ((panData[20U] & 0x100U) == 0U) ? false : true;
}

ADI_COMMON_CODE
void adi_common_extractSscStatus(uint32_t *panData, ADI_SSC_STATUS_DATA *poStatusData)
{
    poStatusData->bTriggered_SAIF[0]                      = (panData[0U] & 0x00000001u) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[1]                      = ((panData[0U] & 0x00000002u) >> 1U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[2]                      = ((panData[0U] & 0x00000004u) >> 2U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[3]                      = ((panData[0U] & 0x00000008u) >> 3U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[4]                      = ((panData[0U] & 0x00000010u) >> 4U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[5]                      = ((panData[0U] & 0x00000020u) >> 5U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[6]                      = ((panData[0U] & 0x00000040u) >> 6U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[7]                      = ((panData[0U] & 0x00000080u) >> 7U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[8]                      = ((panData[0U] & 0x00000100u) >> 8U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[9]                      = ((panData[0U] & 0x00000200u) >> 9U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[10]                     = ((panData[0U] & 0x00000400u) >> 10U) == 1U ? true : false;
    poStatusData->bTriggered_SAIF[11]                     = ((panData[0U] & 0x00000800u) >> 11U) == 1U ? true : false;
    poStatusData->bTriggered_HW_ADC[0]                    = ((panData[0U] & 0x00001000u) >> 12U) == 1U ? true : false;
    poStatusData->bTriggered_HW_ADC[1]                    = ((panData[0U] & 0x00002000u) >> 13U) == 1U ? true : false;
    poStatusData->bTriggered_HW_ADC[2]                    = ((panData[0U] & 0x00004000u) >> 14U) == 1U ? true : false;
    poStatusData->bTriggered_HW_ADC[3]                    = ((panData[0U] & 0x00008000u) >> 15U) == 1U ? true : false;
    poStatusData->bTriggered_SW_ADC[0]                    = (panData[1U] & 0x00000001u) == 1U ? true : false;
    poStatusData->bTriggered_SW_ADC[1]                    = ((panData[1U] & 0x00000002u) >> 1U) == 1U ? true : false;
    poStatusData->bTriggered_SW_ADC[2]                    = ((panData[1U] & 0x00000004u) >> 2U) == 1U ? true : false;
    poStatusData->bTriggered_SW_ADC[3]                    = ((panData[1U] & 0x00000008u) >> 3U) == 1U ? true : false;
    poStatusData->bTriggered_GPTP_UNLOCKED                = ((panData[1U] & 0x00000010u) >> 4U) == 1U ? true : false;
    poStatusData->bTriggered_OTP_CONFIG                   = ((panData[1U] & 0x00000020u) >> 5U) == 1U ? true : false;
    poStatusData->bTriggered_SQI_LVL                      = ((panData[1U] & 0x00000040u) >> 6U) == 1U ? true : false;
    poStatusData->bTriggered_MANUAL                       = ((panData[1U] & 0x00000080u) >> 7U) == 1U ? true : false;
    poStatusData->bTriggered_Eth_frame_Check_Timeout      = ((panData[1U] & 0x00004000u) >> 14U) == 1U ? true : false;
    poStatusData->bTriggered_Device_Unattended_Timeout    = ((panData[1U] & 0x00008000u) >> 15U) == 1U ? true : false;
    poStatusData->bSafeMode                               = (panData[2U] & 0x00000001u) == 1U ? true : false;
    poStatusData->nStatusOfSSC                            = panData[0U] | (panData[1U] << 16U);
}

/** @} */

/**
 * EOF: www.analog.com
 */
