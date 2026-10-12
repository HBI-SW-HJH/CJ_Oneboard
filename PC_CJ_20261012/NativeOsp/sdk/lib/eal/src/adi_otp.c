/*******************************************************************************
Copyright (c) 2022-2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_otp.c
 * \brief: Implementation of the OTP controller driver
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "adi_pal.h"    /* PAL header */
#include "adi_dbg.h"    /* Diagnostics */
#include "adi_memmap.h" /* Memory map */
#include "adi_e2bcore.h"
#include "adi_maclce_reg.h"
#include "adi_network.h"
#include "adi_otp.h"
#include "eal_internal.h"


#ifdef ADI_E2B_IFACE_OTP_ENABLED

/*================================= DEFINES =================================*/
/*! \cond PRIVATE */
#define GET_FIELD_VAL(val, mask, pos) (((val) & (mask)) >> (pos))
#define WRITE_DATA (0u)
#define READ_DATA (1u)

/* EAL instance number to be used by the OTP driver.
This EAL instance is terminated on calling adi_otp_terminate API */
#define EAL_INST_NUM (0U)

/* E2B is configured for only one remote node at a time.
Use remote num as '0' for accessing the remote node through EAL & E2BCore */
#define E2BCFG_REMOTE_NUM (0u)

/* TODO: Verify ADC_MEM_SIZE*/
#define ADC_MEM_SIZE (ADI_EAL_ADC_UNICAST_INST_MEM_SIZE(5U))

#define GET_ID(InstNum, RemoteNum, IntfNum) \
{ \
    .nInstNum   = (InstNum),    \
    .nRemoteNum = (RemoteNum),  \
    .nIntfNum   = (IntfNum)     \
}

#define MAC_64_TO_8(nMacAddr) \
{\
    (uint8_t)(((nMacAddr) & 0xFF0000000000ULL) >> 40U), \
    (uint8_t)(((nMacAddr) & 0x00FF00000000ULL) >> 32U), \
    (uint8_t)(((nMacAddr) & 0x0000FF000000ULL) >> 24U), \
    (uint8_t)(((nMacAddr) & 0x000000FF0000ULL) >> 16U), \
    (uint8_t)(((nMacAddr) & 0x00000000FF00ULL) >> 8U),  \
    (uint8_t)(((nMacAddr) & 0x0000000000FFULL) >> 0U)   \
}
/*================================ DATA TYPES ===============================*/

/*================================ PROTOTYPES ===============================*/
static bool setupRemotePlca(ADI_OTP_MGR *poOtpMgr);
static bool pingRemoteNode(ADI_OTP_MGR *poOtpMgr);
static ADI_OTP_STATUS generateOtpDataForUserScripts(ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static ADI_OTP_STATUS generateUserScriptsRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData, bool bIsUserScript2);
static ADI_OTP_STATUS generateFIOInstructionsRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static ADI_OTP_STATUS generateApplicationRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static void generateDesignerScriptsRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static void otpUtils_AddRegFields(uint8_t *pBuff, const uint32_t nAddr, const uint32_t nData,
                                  const ADI_OTP_DATA_MODE eDataMode);
static ADI_OTP_STATUS programOtpRawData(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
                                        ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static ADI_OTP_STATUS enableOtpMacro(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum);
static ADI_OTP_STATUS disableOtpMacro(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum);
static bool writeRegs(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, uint16_t *panRegmapAddr, uint32_t *panRegmapData,
                      uint8_t nNumRegs, uint8_t *panMMS);
static bool readRegs(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, uint16_t *panRegAddr, uint32_t *pVal, uint8_t nLen,
                     uint8_t *panMMS);
static bool writeOtpBlock(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, uint32_t nBlockNum, uint8_t *pData);
static bool chkOtpProgStatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum);
static bool readOtpSelectedBlocks(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static bool readOtpBlock(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, ADI_OTP_REMOTE_DATA *poRemoteOtpData,
                         uint8_t nBlockNum);
static bool compareOtpReadData(ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static uint8_t ComputeChecksum(uint8_t *pBuff, uint32_t nLen);
static uint8_t ComputeSecdedCode(uint8_t *pBuff, uint8_t nChecksum);
static ADI_OTP_STATUS bootloaderSoftReset(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum);
static ADI_OTP_STATUS pollBootStatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
                                     uint32_t *anOtpStatus, uint8_t nRegs);
static ADI_OTP_STATUS readBootStatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
                                     ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static ADI_OTP_STATUS setReadMode(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, uint8_t nOtpReadType);
static void printOtpData(ADI_OTP_REMOTE_DATA *poRemoteOtpData, uint8_t nDataType);
static void printOtpBlocksRange(ADI_OTP_REMOTE_DATA *poRemoteOtpData, uint8_t nStartBlock,
                                uint8_t nBlockCount);
static void printBlockStatus(ADI_OTP_REMOTE_DATA *poRemoteOtpData);
static bool verifylockstatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum);
static void EalTopCallbackFunc(uint32_t nInstNum, ADI_EAL_CBK_EVT eCbkEventType, const void *pArg);
static void EalRemoteNodeCbk(uint32_t nInstNum, uint32_t nRemoteNum,
                             ADI_EAL_NODE_CBK_EVT eCbkEventType, const void *pArg);
static void EalAdcIntfCbk(uint32_t nInstNum, ADI_EAL_ADC_HANDLE hAdcHandle, ADI_EAL_ADC_CBK_EVT eCbkEventType,
                          const void *pArg);
static  void adi_otp_network_cbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void *pData);
static uint8_t calculateDecodeSyndrome(const uint8_t *pnBlock, const uint8_t anRom[127u][8u]);
static uint8_t checkGeneralParityBit(const uint8_t *pnBlock, uint8_t nBlockLen);

/*=================================== DATA ==================================*/
/*! Internal network structure */
ADI_MEM_DATA_CRIT_CACHE
static ADI_OTP_MGR goOtpMgr;

ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganAdcMemory[ADC_MEM_SIZE];

ADI_MEM_DATA_CRIT_CACHE
static ADI_EAL_ADC_HANDLE ghAdcHandle;

/*! Local reg map address array */
ADI_MEM_DATA_CRIT_CACHE
static uint16_t ganRegmapAddr[20U];

/*! Local reg map MMS array */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganRegmapMMS[20U];

/*! Local reg map mask array */
ADI_MEM_DATA_CRIT_CACHE
static uint32_t ganRegmapMask[20U];

/*! Local reg map data array */
ADI_MEM_DATA_CRIT_CACHE
static uint32_t ganRegmapData[20U];

/*! Local regmap config */
static ADI_E2BCORE_REGMAP_IO_CONFIG goRegCfg;

/*! Hamming codes for ECC */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t anHammingCodes[7U][15U] =
{
    {0x13U, 0x17U, 0x5bU, 0x06U, 0x6aU, 0x73U, 0xdaU, 0x15U, 0x7dU, 0x28U, 0xdcU, 0x7fU, 0x0eU, 0xf2U, 0xc9U},
    {0x26U, 0x2eU, 0xb6U, 0x0cU, 0xd4U, 0xe7U, 0xb4U, 0x2aU, 0xfaU, 0x51U, 0xb8U, 0xfeU, 0x1dU, 0xe5U, 0x92U},
    {0x4cU, 0x5dU, 0x6cU, 0x19U, 0xa9U, 0xcfU, 0x68U, 0x55U, 0xf4U, 0xa3U, 0x71U, 0xfcU, 0x3bU, 0xcbU, 0x24U},
    {0x98U, 0xbaU, 0xd8U, 0x33U, 0x53U, 0x9eU, 0xd0U, 0xabU, 0xe9U, 0x46U, 0xe3U, 0xf8U, 0x77U, 0x96U, 0x48U},
    {0x22U, 0x62U, 0xebU, 0x60U, 0xcdU, 0x4eU, 0x7bU, 0x42U, 0xafU, 0xa5U, 0x1bU, 0x8fU, 0xe1U, 0xdeU, 0x59U},
    {0x44U, 0xc5U, 0xd6U, 0xc1U, 0x9aU, 0x9cU, 0xf6U, 0x85U, 0x5fU, 0x4aU, 0x37U, 0x1fU, 0xc3U, 0xbcU, 0xb2U},
    {0x89U, 0x8bU, 0xadU, 0x83U, 0x35U, 0x39U, 0xedU, 0x0aU, 0xbeU, 0x94U, 0x6eU, 0x3fU, 0x87U, 0x79U, 0x64U},
};

/* ROM table for syndrome calculation (updated with new data, 127x8,
use only bits 1-7 for syndrome) */
ADI_MEM_DATA_CONST
static const uint8_t ganRom[127u][8u] =
{
    {0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u},
    {0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u},
    {0u, 0u, 0u, 0u, 0u, 0u, 1u, 0u},
    {1u, 1u, 1u, 0u, 0u, 0u, 0u, 0u},
    {0u, 1u, 1u, 1u, 1u, 1u, 1u, 0u},
    {0u, 1u, 0u, 0u, 0u, 0u, 1u, 0u},
    {0u, 0u, 0u, 0u, 1u, 0u, 1u, 1u},
    {1u, 0u, 0u, 1u, 0u, 0u, 0u, 0u},
    {0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u},
    {0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u},
    {1u, 0u, 0u, 0u, 0u, 1u, 0u, 0u},
    {1u, 0u, 1u, 0u, 0u, 0u, 1u, 0u},
    {0u, 1u, 0u, 1u, 0u, 1u, 0u, 1u},
    {0u, 1u, 0u, 0u, 1u, 0u, 1u, 1u},
    {0u, 0u, 1u, 1u, 1u, 1u, 0u, 1u},
    {1u, 1u, 0u, 1u, 0u, 0u, 0u, 0u},
    {0u, 1u, 0u, 1u, 1u, 1u, 1u, 1u},
    {1u, 1u, 0u, 0u, 1u, 0u, 0u, 0u},
    {1u, 0u, 1u, 0u, 1u, 1u, 1u, 1u},
    {1u, 1u, 0u, 0u, 0u, 0u, 0u, 1u},
    {0u, 0u, 0u, 0u, 0u, 1u, 0u, 1u},
    {1u, 1u, 0u, 0u, 0u, 1u, 0u, 0u},
    {0u, 0u, 0u, 1u, 0u, 1u, 1u, 1u},
    {1u, 1u, 1u, 0u, 0u, 0u, 1u, 0u},
    {1u, 0u, 0u, 1u, 1u, 0u, 0u, 0u},
    {0u, 0u, 1u, 1u, 0u, 1u, 0u, 1u},
    {1u, 1u, 1u, 1u, 0u, 0u, 1u, 0u},
    {1u, 0u, 1u, 0u, 1u, 0u, 1u, 1u},
    {0u, 1u, 1u, 1u, 1u, 0u, 1u, 0u},
    {0u, 1u, 1u, 1u, 1u, 1u, 0u, 1u},
    {0u, 0u, 0u, 1u, 0u, 1u, 1u, 0u},
    {0u, 0u, 1u, 1u, 0u, 0u, 0u, 0u},
    {0u, 1u, 1u, 0u, 0u, 1u, 0u, 1u},
    {1u, 0u, 1u, 1u, 1u, 1u, 1u, 1u},
    {1u, 1u, 0u, 1u, 1u, 1u, 1u, 0u},
    {0u, 0u, 1u, 0u, 1u, 0u, 0u, 0u},
    {0u, 1u, 0u, 1u, 1u, 0u, 1u, 0u},
    {1u, 1u, 1u, 0u, 1u, 1u, 1u, 1u},
    {1u, 0u, 1u, 1u, 1u, 0u, 0u, 1u},
    {0u, 0u, 1u, 0u, 0u, 0u, 0u, 1u},
    {1u, 0u, 0u, 1u, 0u, 0u, 0u, 1u},
    {0u, 1u, 0u, 0u, 0u, 1u, 0u, 1u},
    {1u, 1u, 1u, 0u, 1u, 0u, 1u, 0u},
    {0u, 0u, 1u, 0u, 0u, 1u, 0u, 0u},
    {0u, 0u, 1u, 1u, 0u, 0u, 0u, 1u},
    {1u, 1u, 0u, 1u, 0u, 1u, 1u, 1u},
    {1u, 0u, 0u, 0u, 1u, 1u, 0u, 0u},
    {1u, 0u, 0u, 1u, 0u, 0u, 1u, 0u},
    {0u, 1u, 1u, 1u, 0u, 1u, 1u, 1u},
    {0u, 1u, 0u, 1u, 1u, 0u, 0u, 0u},
    {1u, 1u, 0u, 1u, 1u, 1u, 0u, 0u},
    {1u, 1u, 1u, 1u, 0u, 1u, 0u, 1u},
    {1u, 0u, 1u, 0u, 1u, 1u, 0u, 0u},
    {1u, 0u, 0u, 0u, 1u, 0u, 1u, 0u},
    {0u, 0u, 1u, 0u, 1u, 1u, 1u, 0u},
    {1u, 1u, 1u, 0u, 1u, 0u, 1u, 1u},
    {1u, 0u, 0u, 1u, 0u, 1u, 0u, 0u},
    {0u, 0u, 0u, 0u, 0u, 1u, 1u, 0u},
    {1u, 0u, 1u, 0u, 1u, 1u, 0u, 1u},
    {0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u},
    {0u, 0u, 0u, 0u, 1u, 0u, 0u, 1u},
    {0u, 1u, 0u, 1u, 0u, 1u, 1u, 0u},
    {0u, 1u, 1u, 1u, 1u, 0u, 1u, 1u},
    {1u, 1u, 1u, 1u, 0u, 0u, 0u, 0u},
    {0u, 1u, 1u, 1u, 1u, 0u, 0u, 0u},
    {1u, 0u, 0u, 1u, 0u, 1u, 0u, 1u},
    {0u, 1u, 0u, 1u, 1u, 1u, 0u, 1u},
    {1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u},
    {0u, 1u, 1u, 1u, 1u, 1u, 0u, 0u},
    {1u, 0u, 1u, 1u, 1u, 1u, 1u, 0u},
    {1u, 1u, 1u, 1u, 0u, 0u, 1u, 1u},
    {1u, 1u, 1u, 0u, 1u, 0u, 0u, 0u},
    {1u, 0u, 1u, 1u, 0u, 0u, 1u, 0u},
    {1u, 0u, 1u, 1u, 1u, 0u, 1u, 0u},
    {0u, 1u, 1u, 0u, 0u, 1u, 1u, 0u},
    {1u, 0u, 0u, 1u, 1u, 1u, 1u, 1u},
    {0u, 1u, 0u, 0u, 1u, 1u, 1u, 1u},
    {0u, 1u, 1u, 1u, 1u, 0u, 0u, 1u},
    {1u, 1u, 1u, 0u, 0u, 1u, 1u, 1u},
    {0u, 1u, 1u, 0u, 0u, 0u, 0u, 1u},
    {1u, 0u, 1u, 0u, 1u, 0u, 1u, 0u},
    {0u, 1u, 0u, 1u, 0u, 0u, 0u, 1u},
    {0u, 1u, 1u, 1u, 0u, 1u, 0u, 0u},
    {0u, 0u, 1u, 0u, 0u, 1u, 0u, 1u},
    {1u, 0u, 0u, 1u, 1u, 1u, 1u, 0u},
    {1u, 0u, 0u, 1u, 1u, 0u, 1u, 0u},
    {0u, 1u, 0u, 1u, 1u, 0u, 0u, 1u},
    {1u, 1u, 1u, 0u, 0u, 1u, 0u, 0u},
    {1u, 1u, 0u, 0u, 1u, 1u, 0u, 1u},
    {1u, 1u, 1u, 1u, 0u, 0u, 0u, 1u},
    {0u, 0u, 1u, 1u, 1u, 0u, 1u, 1u},
    {0u, 0u, 1u, 1u, 0u, 1u, 1u, 1u},
    {0u, 0u, 0u, 1u, 1u, 1u, 0u, 0u},
    {1u, 1u, 0u, 0u, 1u, 1u, 0u, 0u},
    {1u, 1u, 0u, 0u, 1u, 1u, 1u, 0u},
    {1u, 1u, 0u, 1u, 0u, 0u, 1u, 0u},
    {1u, 0u, 1u, 0u, 0u, 1u, 1u, 0u},
    {1u, 0u, 0u, 0u, 1u, 1u, 1u, 1u},
    {1u, 0u, 1u, 0u, 0u, 1u, 1u, 1u},
    {1u, 0u, 1u, 1u, 1u, 0u, 0u, 0u},
    {0u, 0u, 0u, 1u, 1u, 1u, 0u, 1u},
    {0u, 0u, 1u, 1u, 1u, 1u, 0u, 0u},
    {0u, 0u, 1u, 1u, 0u, 0u, 1u, 1u},
    {0u, 0u, 0u, 0u, 1u, 1u, 0u, 1u},
    {1u, 1u, 0u, 1u, 1u, 0u, 1u, 1u},
    {1u, 1u, 1u, 0u, 1u, 1u, 0u, 0u},
    {1u, 0u, 0u, 0u, 1u, 1u, 1u, 0u},
    {0u, 1u, 0u, 0u, 1u, 0u, 1u, 0u},
    {1u, 0u, 1u, 1u, 0u, 1u, 0u, 0u},
    {1u, 1u, 1u, 0u, 1u, 1u, 1u, 0u},
    {1u, 0u, 0u, 1u, 1u, 0u, 0u, 1u},
    {1u, 0u, 0u, 1u, 1u, 0u, 1u, 1u},
    {0u, 1u, 1u, 1u, 0u, 1u, 1u, 0u},
    {0u, 1u, 0u, 1u, 0u, 1u, 0u, 0u},
    {1u, 1u, 1u, 0u, 1u, 0u, 0u, 1u},
    {0u, 1u, 0u, 0u, 0u, 1u, 1u, 0u},
    {1u, 1u, 0u, 0u, 0u, 1u, 1u, 1u},
    {1u, 1u, 1u, 0u, 1u, 1u, 0u, 1u},
    {0u, 1u, 0u, 1u, 0u, 0u, 1u, 1u},
    {0u, 1u, 0u, 0u, 0u, 0u, 1u, 1u},
    {1u, 0u, 1u, 0u, 0u, 0u, 1u, 1u},
    {0u, 1u, 0u, 0u, 1u, 0u, 0u, 1u},
    {0u, 1u, 1u, 0u, 0u, 0u, 1u, 1u},
    {1u, 0u, 1u, 1u, 0u, 1u, 1u, 0u},
    {0u, 0u, 1u, 0u, 1u, 0u, 0u, 1u},
    {0u, 0u, 0u, 0u, 0u, 1u, 1u, 1u},
    {0u, 0u, 0u, 1u, 0u, 0u, 1u, 1u}
};

/*~~~~~~~~~~~~~~~~~ Program Time remote configuration - ADC ~~~~~~~~~~~~~~~~~~~~~~~~~ */
ADI_MEM_DATA_CRIT_CACHE
static ADI_E2BCORE_REG_ADDR_VAL aoPgmTimeRemoteAdcIfRegMap[] =
{
    {
        .nAddr = ADC_CONTROL_NW_ADDR,
        .nVal = ADC_CONTROL_RESET_VAL
    },
    {
        .nAddr = ADC_CONTROL_2_NW_ADDR,
        .nVal = (ADC_CONTROL_2_RESET_VAL &
                 ~(BITM_ADC_CONTROL_2_CAL_EN |
                   BITM_ADC_CONTROL_2_COMMAND_MODE)) |
        (BITM_ADC_CONTROL_2_CAL_EN |
         BITM_ADC_CONTROL_2_COMMAND_MODE)
    },
};
ADI_MEM_DATA_CRIT_CACHE
static ADI_E2BCORE_REG_ADDR_VAL aoPgmTimeRemoteAdc1FifoRegMap[] =
{
    /* 0x0000U : 0x0C008000U */
    {
        .nAddr = FIFO_CONTROL_ADDR,
        .nVal = (FIFO_CONTROL_RESET_VAL &
                 ~(BITM_FIFO_CONTROL_READ_MODE)) |
        ((0x3U << BITP_FIFO_CONTROL_READ_MODE))
    },
    /* 0x0004U : 0x00001002U */
    {
        .nAddr = FIFO_READ_BLOCK_SIZES_ADDR,
        .nVal = (FIFO_READ_BLOCK_SIZES_RESET_VAL &
                 ~(BITM_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE_INSTANTANEOUS |
                   BITM_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE)) |
        ((0x1U << BITP_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE_INSTANTANEOUS) |
         (20U << BITP_FIFO_READ_BLOCK_SIZES_READ_BLOCK_SIZE))
    },
};
ADI_MEM_DATA_CRIT_CACHE
static ADI_E2BCORE_REG_ADDR_VAL aoPgmTimeRemoteRegMap[] =
{
    /* TODO 0x0024U : 0x22FE7001U */
    {
        .nAddr = LCE_DEVICE_MAC_ADDRESS_LOWER_ADDR,
        //.nVal  = (REMOTE1_MAC_ADDR & 0x0000FFFFFFFFull) << BITP_LCE_DEVICE_MAC_ADDRESS_LOWER_DEVICE_MAC_ADDRESS_LOWER
    },
    /* 0x0028U : 0x01E000E0U */
    {
        .nAddr = LCE_DEVICE_MULTICAST_ADDRESS_UPPER_NW_ADDR,
        .nVal = LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL
    },
    /* 0xD004U : 0x00007777U */
    {
        .nAddr = HOST_IF_TO_INTF_MUX2_ADDR,
        .nVal = (HOST_IF_TO_INTF_MUX2_RESET_VAL &
                 ~(BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8 |
                   BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9 |
                   BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10 |
                   BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11 |
                   BITM_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12)) |
        ((0x7U << BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_8) |
         (0x7U << BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_9) |
         (0x7U << BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_10) |
         (0x7U << BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_11) |
         (0x0U << BITP_HOST_IF_TO_INTF_MUX2_TO_INTF_MUX_12))
    },
    /* 0xD008U : 0x000FFFE6U */
    {
        .nAddr = HOST_IF_FROM_INT_MUX_ADDR,
        .nVal = (HOST_IF_FROM_INT_MUX_RESET_VAL &
                 ~(BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0 |
                   BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1 |
                   BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2 |
                   BITM_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3)) |
        ((0xCU << BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_0) |
         (0x1fU << BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_1) |
         (0x1fU << BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_2) |
         (0x1fU << BITP_HOST_IF_FROM_INT_MUX_FROM_INTF_MUX_3))
    },
    /* 0xD02CU : 0x00000FFFU */
    {
        .nAddr = HOST_IF_SA_IF_BUS_CONFIGURATION_ADDR,
        .nVal = (HOST_IF_SA_IF_BUS_CONFIGURATION_RESET_VAL &
                 ~(BITM_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE)) |
        ((0xfffU << BITP_HOST_IF_SA_IF_BUS_CONFIGURATION_SA_IF_INPUT_ENABLE))
    },
    /* 0xFC00U : 0x00000001U */
    {
        .nAddr = 0xfc00U,
        .nVal = 0x1U
    },
    /* 0xFC04U : 0x00000002U */
    {
        .nAddr = 0xfc04U,
        .nVal = 0x2U
    },
    /* 0xFC14U : 0x00000020U */
    {
        .nAddr = 0xfc14U,
        .nVal = 0x20U
    },
    /* 0xB21DU : 0x0000000BU | To map AVDD_div3 to the VMON */
    {
        .nAddr = 0xb21dU,
        .nVal = 0xbU
    },

};
ADI_MEM_DATA_CRIT_CACHE
static ADI_E2BCORE_IF_CONFIG aoPgmTimeRemoteIfConfig[4U] =
{
    [0U] =
    {
        /* ========== CONFIGS FOR INTERFACE: Adc1 ========== */
        .oId = GET_ID(0U, 0U, 0U),
        .bEnabled = true,
        .eIfType = ADI_E2BCORE_IF_TYPE_ADC,
        .nIfTopic = 1,
        .nNumIfReg = (uint32_t)(sizeof(aoPgmTimeRemoteAdcIfRegMap) / sizeof(ADI_E2BCORE_REG_ADDR_VAL)),
        .aoIfRegData = aoPgmTimeRemoteAdcIfRegMap,
        .nFifoTopic = 5,
        .nNumFifoReg = (uint32_t)(sizeof(aoPgmTimeRemoteAdc1FifoRegMap) / sizeof(ADI_E2BCORE_REG_ADDR_VAL)),
        .aoFifoRegData = aoPgmTimeRemoteAdc1FifoRegMap,
        .pfIntfCb = &AdcInterfaceCbk,
    },
    [1U] =
    {
        /* ========== DISABLED INTERFACE 1 ========== */
        .bEnabled = false,
    },
    [2U] =
    {
        /* ========== DISABLED INTERFACE 2 ========== */
        .bEnabled = false,
    },
    [3U] =
    {
        /* ========== DISABLED INTERFACE 3 ========== */
        .bEnabled = false,
    },
};
ADI_MEM_DATA_CRIT_CACHE
static ADI_E2BCORE_REMOTE_CONFIG aoPgmTimeEth10BaseT1sRemoteConfig[1U] =
{
    [0U] =
    {
        /* ========== CONFIGS FOR REMOTE: Remote1 ========== */
        .oId = GET_ID(0U, 0U, 0xFFFFFFFFU),
        /*.anMacAddr    = To be filled in run time */
        .bEnabled = true,
        .nNumRegisters = (uint32_t)(sizeof(aoPgmTimeRemoteRegMap) / sizeof(ADI_E2BCORE_REG_ADDR_VAL)),
        .aoRegData = aoPgmTimeRemoteRegMap,
        .nNumIf = 1,
        .aoIntfConfigs = aoPgmTimeRemoteIfConfig,
        .pfRemoteCb = &RemoteNodeCbk
    },
};
ADI_MEM_DATA_CRIT_CACHE
static ADI_E2BCORE_CFG aoPgmTimeEth10BaseT1sNtwrkCfg =
{
    /* .anMacAddr       = Updated later in adi_otp_init() from PgmTime OTP cfg data */
    .nNumNodes = 1U,
    .aoRemoteCfgs = aoPgmTimeEth10BaseT1sRemoteConfig,
    .pfLibCb = &Eth10BaseT1sCbk,
    .bInitializeNetwork = true,
};
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*! \endcond */
/*=================================== CODE ==================================*/
/*!*************************************************************************************************
 * @brief Initializes the OTP software driver
 * @param [out] phOtpHandle     Returns the OTP handle for use in other OTP APIs
 * @param [in] poOtpCfg         Pointer to the OTP config data
 * @param [in] nDevNum          Index of the device that needs to be used for OTP programming
 * @param [in] bEnableFrameDup  Flag indicating if frames have to be duplicated to all opened network interfaces
 !*/
ADI_EAL_OTP_CODE
void adi_otp_init(ADI_OTP_HANDLE *phOtpHandle, ADI_OTP_CONFIG *poOtpCfg, uint32_t nDevNum, bool bEnableFrameDup)
{
    ADI_EAL_STATUS eEalStatus;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_init\r\n");

    /* Preconditions */
    ADI_DBG_REQUIRE(phOtpHandle != NULL, "Pointer to OTP handle is null");
    ADI_DBG_REQUIRE(poOtpCfg != NULL, "Pointer to Otp Cfg is null");
    ADI_OTP_REMOTE_DATA *poRemoteOtpData;

    ADI_OTP_MGR *poOtpMgr = &goOtpMgr;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == false, "OTP driver is already initialized");

    /* Clears the data */
    (void)ADI_MEMSET(poOtpMgr, 0, sizeof(ADI_OTP_MGR));

    poOtpMgr->poOtpCfg = poOtpCfg;

    /* Clear the variables in remote cfgs which are populated by the OTP driver */
    for(uint8_t i = 0u; i < poOtpCfg->nNumNodes; i++)
    {
        poRemoteOtpData = &poOtpMgr->poOtpCfg->paoRemoteOtpData[i];
        (void)ADI_MEMSET(poRemoteOtpData->anOtpWriteData, 0, ADI_OTP_MAX_NUM_BLOCKS * ADI_OTP_BLOCK_LEN_BYTES);
        (void)ADI_MEMSET(poRemoteOtpData->anOtpReadData, 0, ADI_OTP_MAX_NUM_BLOCKS * ADI_OTP_BLOCK_LEN_BYTES);
        (void)ADI_MEMSET(poRemoteOtpData->abOtpBlockWrEn, 0, ADI_OTP_MAX_NUM_BLOCKS);
        (void)ADI_MEMSET(poRemoteOtpData->aeBlockBootStatus, 0, ADI_OTP_MAX_NUM_BLOCKS * sizeof(ADI_OTP_BLOCK_STATUS));
        poRemoteOtpData->nLastOtpBlockWritten = 0u;
        poRemoteOtpData->nUserScriptStartBlock = 0u;
        poRemoteOtpData->nUserScript2StartBlock = 0u;
        poRemoteOtpData->nFIOInstructionStartBlock = 0u;
        poRemoteOtpData->nNumUserScriptBlocks = 0u;
        poRemoteOtpData->nNumUserScript2Blocks = 0u;
        poRemoteOtpData->nNumFIOInstructionBlocks = 0u;
        poRemoteOtpData->nNumWriteBlocks = 0u;
    }

    /* Set the bInUse flag true */
    poOtpMgr->bInUse = true;
    poOtpMgr->nEalInstNum = EAL_INST_NUM;
    poOtpMgr->nDevNum = nDevNum;
    poOtpMgr->nE2bCfgRemoteNum = E2BCFG_REMOTE_NUM;

    /* Return the manager as handle */
    *phOtpHandle = (void *)poOtpMgr;

    if(poOtpMgr->poOtpCfg->bPgmOverOaspi == false)
    {
        (void)ADI_MEMCPY(aoPgmTimeEth10BaseT1sNtwrkCfg.anMacAddr, poOtpMgr->poOtpCfg->poPgmTimeCfg->anControllerMacAddr, 6u);

        // Create an instance of EAL mapping instance number with the device index
        adi_eal_createInstance(poOtpMgr->nEalInstNum, nDevNum, &aoPgmTimeEth10BaseT1sNtwrkCfg);

        // Enable frame duplication
        adi_eal_setFrameDuplication(poOtpMgr->nEalInstNum, bEnableFrameDup);

        // Register for EAL module callback
        adi_eal_registerCallback(poOtpMgr->nEalInstNum, &EalTopCallbackFunc);

        // Register for node callbacks
        adi_eal_registerNodeCallback(poOtpMgr->nEalInstNum, &EalRemoteNodeCbk);

        // Do a LV die reset
        eEalStatus = adi_eal_resetRemoteLvDie(poOtpMgr->nEalInstNum, ADI_EAL_ALL_REMOTES);
        ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to reset remote nodes' LV die");
        adi_network_flushTxTimed(ADI_EAL_LV_RESET_WAIT_TIME_NS);

        if(poOtpCfg->poPgmTimeCfg->bSampleMacAddr)
        {
            /* Set the input enable for the SA_IF pins */
            eEalStatus = adi_eal_setInputEnable(poOtpMgr->nEalInstNum, ADI_EAL_ALL_REMOTES, 0xFFFU);
            ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed set the input enable for the SA_IF pins");
            adi_network_flushTxBuffer(poOtpMgr->nDevNum, 0ULL);

            // Set bit to sample MAC address LSBs
            eEalStatus = adi_eal_setMacAddrSampling(poOtpMgr->nEalInstNum, ADI_EAL_ALL_REMOTES, true);
            ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Setting bit to sample mac address failed");
        }
    }
    else
    {
        ADI_DBG_ENSURE(poOtpMgr->poOtpCfg->nNumNodes == 1u, "Number of nodes to be programmed over OASPI is greater than 1");
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-OTP controller driver initialized\r\n");

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
}

/*!*************************************************************************************************
 * @brief Terminates the OTP software driver
 * @param [in] hOtpHandle  OTP handle
 */
ADI_EAL_OTP_CODE
void adi_otp_terminate(ADI_OTP_HANDLE hOtpHandle)
{
    ADI_EAL_STATUS eEalStatus;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_terminate\r\n");

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Reset the MAC address defaults */
    aoPgmTimeRemoteRegMap[0U].nVal = LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL;
    aoPgmTimeRemoteRegMap[1U].nVal = LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL;
    uint8_t anDefMcastAddr[] =
    {
        (uint8_t)(((uint32_t)LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0xFF000000U) >> 24U),
        (uint8_t)(((uint32_t)LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0x00FF0000U) >> 16U),
        (uint8_t)(((uint32_t)LCE_MULTICAST_MAC_ADDRESS_LOWER_RESET_VAL & 0xFF000000U) >> 24U),
        (uint8_t)(((uint32_t)LCE_MULTICAST_MAC_ADDRESS_LOWER_RESET_VAL & 0x00FF0000U) >> 16U),
        (uint8_t)(((uint32_t)LCE_MULTICAST_MAC_ADDRESS_LOWER_RESET_VAL & 0x0000FF00U) >> 8U),
        0xFFu
    };
    adi_e2bcore_useMcastAddr(0U, &anDefMcastAddr[0U]);

    if(poOtpCfg->bPgmOverOaspi == false)
    {
        if(poOtpCfg->poPgmTimeCfg->bSampleMacAddr)
        {
            // Set bit to sample MAC address LSBs
            eEalStatus = adi_eal_setMacAddrSampling(poOtpMgr->nEalInstNum, ADI_EAL_ALL_REMOTES, false);
            ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "CLearing bit to sample mac address failed");
        }

        /* Terminate the EAL instance */
        adi_eal_terminateInstance(poOtpMgr->nEalInstNum);
    }

    /* Clears the OTP manager */
    (void)ADI_MEMSET(poOtpMgr, 0, sizeof(ADI_OTP_MGR));

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
}

/*!*************************************************************************************************
 * @brief Change the MAC address of the currently programmed node
 *
 * @param [in] hOtpHandle   OTP handle
 * @param [in] nUcastAddr   Unicast MAC address
 * @param [in] nMcastAddr   Multicast MAC address
 */
ADI_EAL_OTP_CODE
void adi_otp_useRemoteMacAddr(ADI_OTP_HANDLE hOtpHandle, uint64_t nUcastAddr, uint64_t nMcastAddr)
{
    /* Update the unicast MAC address in the E2B config */
    uint8_t anUcastMacAddr[6U] = MAC_64_TO_8(nUcastAddr);
    (void)ADI_MEMCPY(&aoPgmTimeEth10BaseT1sRemoteConfig[0U].anMacAddr[0U], anUcastMacAddr, 6U);
    aoPgmTimeRemoteRegMap[0U].nVal = (uint32_t)((nUcastAddr & 0x0000FFFFFFFFULL) <<
                                     BITP_LCE_DEVICE_MAC_ADDRESS_LOWER_DEVICE_MAC_ADDRESS_LOWER);
    aoPgmTimeRemoteRegMap[1U].nVal &= ~BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER;
    aoPgmTimeRemoteRegMap[1U].nVal |= (uint32_t)((nUcastAddr >> 32U) <<
                                      BITP_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER);

    /* Update the multicast MAC address */
    uint8_t anMcastMacAddr[6U] = MAC_64_TO_8(nMcastAddr);
    adi_e2bcore_useMcastAddr(0U, &anMcastMacAddr[0U]);
    aoPgmTimeRemoteRegMap[1U].nVal &= ~BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_MULTICAST_MAC_ADDRESS_UPPER;
    aoPgmTimeRemoteRegMap[1U].nVal |= (uint32_t)((nMcastAddr >> 32U) <<
                                      BITP_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_MULTICAST_MAC_ADDRESS_UPPER);
}

/*!*************************************************************************************************
 * @brief This API sets up the remote node. It uses the program time MAC address from the OTP config
          to address the remote node.
          It sets the PLCA of the remote node to '1'.
          Pings the remote node to verify the communication.
          Configures the remote node to enable ADC interface for checking AVDD voltage requirement for OTP programming.
 * @param [in] hOtpHandle  Returns the OTP handle for use in other OTP APIs
 * @param [in] nRemoteNum  Remote number
 !*/
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_setupRemoteNode(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_setupRemoteNode (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    /* Validate that the OTP driver is initialized and the driver is not configured to program over OASPI */
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");
    ADI_DBG_REQUIRE(poOtpMgr->poOtpCfg->bPgmOverOaspi == false, "OTP driver is configured to program over OASPI");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Validate that the remote node number is in range */
    ADI_DBG_REQUIRE(nRemoteNum < poOtpCfg->nNumNodes, "Remote num is out of range");

    /* Reset the MAC address defaults */
    aoPgmTimeRemoteRegMap[0U].nVal = LCE_DEVICE_MAC_ADDRESS_LOWER_RESET_VAL;
    aoPgmTimeRemoteRegMap[1U].nVal = LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL;
    uint8_t anMcastMacAddr[] =
    {
        (uint8_t)(((uint32_t)LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0xFF000000U) >> 24U),
        (uint8_t)(((uint32_t)LCE_DEVICE_MULTICAST_ADDRESS_UPPER_RESET_VAL & 0x00FF0000U) >> 16U),
        (uint8_t)(((uint32_t)LCE_MULTICAST_MAC_ADDRESS_LOWER_RESET_VAL & 0xFF000000U) >> 24U),
        (uint8_t)(((uint32_t)LCE_MULTICAST_MAC_ADDRESS_LOWER_RESET_VAL & 0x00FF0000U) >> 16U),
        (uint8_t)(((uint32_t)LCE_MULTICAST_MAC_ADDRESS_LOWER_RESET_VAL & 0x0000FF00U) >> 8U),
        0xFFu,
    };
    adi_e2bcore_useMcastAddr(0U, &anMcastMacAddr[0U]);

    /* Update the MAC address in the E2B config */
    uint8_t anRemoteMacAddr[6U] = MAC_64_TO_8(poOtpCfg->poPgmTimeCfg->paRemoteMacAddr[nRemoteNum]);
    (void)ADI_MEMCPY(&aoPgmTimeEth10BaseT1sRemoteConfig[0U].anMacAddr[0U], anRemoteMacAddr, 6U);
    aoPgmTimeRemoteRegMap[0U].nVal = (uint32_t)((poOtpCfg->poPgmTimeCfg->paRemoteMacAddr[nRemoteNum] & 0x0000FFFFFFFFULL) <<
                                     BITP_LCE_DEVICE_MAC_ADDRESS_LOWER_DEVICE_MAC_ADDRESS_LOWER);

    /* Update the MSB (upper 2 bytes) of the MAC address */
    aoPgmTimeRemoteRegMap[1U].nVal &= ~BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER;
    aoPgmTimeRemoteRegMap[1U].nVal |= (uint32_t)(((poOtpCfg->poPgmTimeCfg->paRemoteMacAddr[nRemoteNum] >> 32U) & (uint64_t)BITM_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER) << BITP_LCE_DEVICE_MULTICAST_ADDRESS_UPPER_DEVICE_MAC_ADDRESS_UPPER);

    do
    {
        /*=========== Write the PLCA ID from Pgm time cfg and enable PLCA ========*/
        if(!setupRemotePlca(poOtpMgr)) /* PLCA ID for the remote node will be set to 1 */
        {
            /* If the write itself has failed, set error and exit */
            eReturn = ADI_OTP_STATUS_PGM_FAIL;
            break;
        }

        /*====================== Try pinging the remote node ======================*/
        if(!pingRemoteNode(poOtpMgr))
        {
            eReturn = ADI_OTP_STATUS_PGM_FAIL;
            break;
        }

        /*====================== Write MAC address to the remote node ==============*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Writing Remote MAC address: ");

        if(adi_eal_writeMacAddr(poOtpMgr->nEalInstNum, 0u, anRemoteMacAddr) != ADI_EAL_STATUS_SUCCESS)
        {
            /* If the write itself has failed, set error and exit */
            eReturn = ADI_OTP_STATUS_PGM_FAIL;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
            break;
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");
        }

        /*====================== Configure the network ==============================*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Configuring the network (remote node): ");

        if(adi_eal_configureNetwork(poOtpMgr->nEalInstNum) != ADI_EAL_STATUS_SUCCESS)
        {
            eReturn = ADI_OTP_STATUS_PGM_FAIL;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
            break;
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Setup of remote node complete\r\n");

    } while(false);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");

    return eReturn;
}

/*!*************************************************************************************************
 * @brief Check the junction temperature for the intended remote node prior to OTP programming. This is to ensure the
          correct junction temperature for OTP programming.
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @param [in] fMinJunctionTemp The minimum value of junction temperature
 * @param [in] fMaxJunctionTemp The maximum value of junction temperature
 * @return Status
 *          - #ADI_OTP_STATUS_NO_ERR             Junction temperature check prior to OTP successful
 *          - #ADI_OTP_JTEMP_CHECK_FAILED        Junction temperature check failed
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_checkJunctionTemp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum, double fMinJunctionTemp,
        double fMaxJunctionTemp)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_checkJunctionTemp (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Preconditions */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");
    ADI_DBG_REQUIRE(poOtpMgr->poOtpCfg->bPgmOverOaspi == false, "OTP driver is configured to program over OASPI");
    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    ADI_DBG_REQUIRE(nRemoteNum < poOtpCfg->nNumNodes, "Remote num is out of range");

    poOtpMgr->nAdcSampleRcvdCount = 0u;

    /* Check the junction temperature for the intended remote node prior to OTP programming. This is to ensure the
       correct junction temperature for OTP programming.*/
    do
    {
        /*====================== Open & configure ADC interface ==============*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Opening ADC interface and start sampling junction temperature: ");

        ADI_EAL_ADC_CFG oAdcCfg;
        (void)ADI_MEMSET(&oAdcCfg, 0, sizeof(oAdcCfg));
        oAdcCfg.nInstNum = poOtpMgr->nEalInstNum;
        oAdcCfg.nRemoteNum = poOtpMgr->nE2bCfgRemoteNum;
        oAdcCfg.nIntfNum = 0U;
        oAdcCfg.nAdcCfgQDepth = 5U;
        oAdcCfg.nEalAdcMemorySize = (uint32_t)ADC_MEM_SIZE;
        oAdcCfg.pEalAdcMemory = &ganAdcMemory[0U];
        /* Open an ADC instance and register a callback */
        (void)adi_eal_adcOpen(&oAdcCfg, &ghAdcHandle);
        (void)adi_eal_adcRegisterCallback(ghAdcHandle, &EalAdcIntfCbk);

        /* Set the round robin configuration for getting ADC samples for temperature channel index */
        ADI_EAL_ADC_ROUND_ROBIN_CFG oCfg;
        (void)ADI_MEMSET(&oCfg, 0, sizeof(ADI_EAL_ADC_ROUND_ROBIN_CFG));
        oCfg.nChannelEn = ((uint16_t)0x1u << TEMPERATURE_CH_IDX); //0x2000U;
        oCfg.nChannelMask = 0x0U;
        oCfg.nClkPeriod = 1U * MS_TO_NS;
        oCfg.nCyclePeriod = 15U * MS_TO_NS;
        oCfg.nNumSamples = NUM_AVDD_SAMPLES;

        /* Configure the round robin sample in the remote node */
        eEalStatus = adi_eal_adcConfigureRoundRobinSampling(ghAdcHandle, &oCfg, 0xEFU);
        adi_network_flushTxBuffer(poOtpMgr->nDevNum, 0ULL);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            eReturn = ADI_OTP_JTEMP_CHECK_FAILED;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
            break;
        }

        /* Start the adc sampling */
        eEalStatus = adi_eal_adcStartSampling(ghAdcHandle, 0xA0);
        adi_network_flushTxBuffer(poOtpMgr->nDevNum, 0ULL);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            eReturn = ADI_OTP_JTEMP_CHECK_FAILED;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
            break;
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");

        /*====================== Wait for ADC samples to be received ==============*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Wait for ADC samples: ");
        adi_network_flushTxTimed(100U * MS_TO_NS);

        /* ADC samples are received - check if number of desired ADC samples are received */
        if(poOtpMgr->nAdcSampleRcvdCount == NUM_AVDD_SAMPLES)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Received %d samples\r\n", poOtpMgr->nAdcSampleRcvdCount);
        }
        else
        {
            eReturn = ADI_OTP_JTEMP_CHECK_FAILED;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed to receive sufficient samples\r\n");
            break;
        }

        /*====================== Check Junction temperature level ================================*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Junction temperature check within limits: ");

        for(uint8_t j = 0; j < poOtpMgr->nAdcSampleRcvdCount; j++)
        {
            double fTempJunction = (0.1849f * (double)poOtpMgr->aAdcSample[j]) - 248.79f;

            /* If the computed value is not in safe range for OTP programming then flag the operation as failed and return approriate error */
            if(fTempJunction < fMinJunctionTemp || fTempJunction > fMaxJunctionTemp)
            {
                //ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "%f  ", fAdcSampleVal);
                // ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Temperature value %f%cC \r\n", fTempJunction, 248U);
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed!! Ensure junction temperature to be between %f%cC - %f%cC", fMinJunctionTemp,
                            248U, fMaxJunctionTemp, 248U);
                eReturn = ADI_OTP_JTEMP_CHECK_FAILED;
                break;
            }
        }

        if(eReturn == ADI_OTP_JTEMP_CHECK_FAILED)
        {
            break;
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Passed.\r\n");
        }

    } while(false);

    /* Eal instance is closed as it can be re-opened in the other flow */
    (void)adi_eal_adcClose(ghAdcHandle);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief Check the AVDD for the intended remote node prior to OTP programming. This is to ensure the
          correct voltage level for OTP programming.
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @param [in] fMinAvddVal The minimum value that AVDD can have
 * @param [in] fMaxAvddVal The maximum value that AVDD can have
 * @return Status
 *          - #ADI_OTP_STATUS_NO_ERR            AVDD check prior to OTP successful
 *          - #ADI_OTP_AVDD_CHECK_FAILED        AVDD voltage check failed
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_checkAVDD(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum, double fMinAvddVal, double fMaxAvddVal)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    ADI_EAL_STATUS eEalStatus;
    float fAdcSampleVal = 0.0f;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_checkAVDD (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Preconditions */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");
    ADI_DBG_REQUIRE(poOtpMgr->poOtpCfg->bPgmOverOaspi == false, "OTP driver is configured to program over OASPI");
    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    ADI_DBG_REQUIRE(nRemoteNum < poOtpCfg->nNumNodes, "Remote num is out of range");

    poOtpMgr->nAdcSampleRcvdCount = 0u;

    /* Check the AVDD for the intended remote node prior to OTP programming. This is to ensure the
    correct voltage level for OTP programming. */
    do
    {
        /*====================== Open & configure ADC interface ==============*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Opening ADC interface and start sampling AVDD: ");

        ADI_EAL_ADC_CFG oAdcCfg;
        (void)ADI_MEMSET(&oAdcCfg, 0, sizeof(oAdcCfg));
        oAdcCfg.nInstNum = poOtpMgr->nEalInstNum;
        oAdcCfg.nRemoteNum = poOtpMgr->nE2bCfgRemoteNum;
        oAdcCfg.nIntfNum = 0U;
        oAdcCfg.nAdcCfgQDepth = 5U;
        oAdcCfg.nEalAdcMemorySize = (uint32_t)ADC_MEM_SIZE;
        oAdcCfg.pEalAdcMemory = &ganAdcMemory[0U];
        /* Open an ADC instance and register a callback */
        (void)adi_eal_adcOpen(&oAdcCfg, &ghAdcHandle);
        (void)adi_eal_adcRegisterCallback(ghAdcHandle, &EalAdcIntfCbk);

        /* Set the round robin configuration for getting ADC samples for VMON channel index */
        ADI_EAL_ADC_ROUND_ROBIN_CFG oCfg;
        (void)ADI_MEMSET(&oCfg, 0, sizeof(ADI_EAL_ADC_ROUND_ROBIN_CFG));
        oCfg.nChannelEn = ((uint16_t)0x1u << VMON_CH_IDX); //0x40U;
        oCfg.nChannelMask = 0x0U;
        oCfg.nClkPeriod = 1U * MS_TO_NS;
        oCfg.nCyclePeriod = 15U * MS_TO_NS;
        oCfg.nNumSamples = NUM_AVDD_SAMPLES;

        /* Configure the round robin sample in the remote node */
        eEalStatus = adi_eal_adcConfigureRoundRobinSampling(ghAdcHandle, &oCfg, 0xABU);
        adi_network_flushTxBuffer(poOtpMgr->nDevNum, 0ULL);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            eReturn = ADI_OTP_AVDD_CHECK_FAILED;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
            break;
        }

        /* Start the adc sampling */
        eEalStatus = adi_eal_adcStartSampling(ghAdcHandle, 0xCDU);
        adi_network_flushTxBuffer(poOtpMgr->nDevNum, 0ULL);

        if(eEalStatus != ADI_EAL_STATUS_SUCCESS)
        {
            eReturn = ADI_OTP_AVDD_CHECK_FAILED;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
            break;
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");

        /*====================== Wait for ADC samples to be received ==============*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Wait for ADC samples: ");
        adi_network_flushTxTimed(100U * MS_TO_NS);

        /* ADC samples are received - check if number of desired ADC samples are received */
        if(poOtpMgr->nAdcSampleRcvdCount == NUM_AVDD_SAMPLES)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Received %d samples\r\n", poOtpMgr->nAdcSampleRcvdCount);
        }
        else
        {
            eReturn = ADI_OTP_AVDD_CHECK_FAILED;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed to receive sufficient samples\r\n");
            break;
        }

        /*====================== Check AVDD level ================================*/
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-AVDD check within limits: ");

        for(uint8_t j = 0; j < poOtpMgr->nAdcSampleRcvdCount; j++)
        {
            float fRegReadVal = (float)poOtpMgr->aAdcSample[j];
            fAdcSampleVal = ((fRegReadVal * (3.63f / 4095.0f)) * 3.0f);

            /* If the computed value is not in safe range for OTP programming then flag the operation as failed and return approriate error */
            if(fAdcSampleVal < fMinAvddVal || fAdcSampleVal > fMaxAvddVal)
            {
                //ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "%f  ", fAdcSampleVal);
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed!! Ensure AVDD to be between %fV - %fV", fMinAvddVal, fMaxAvddVal);
                eReturn = ADI_OTP_AVDD_CHECK_FAILED;
                break;
            }
        }

        if(eReturn == ADI_OTP_AVDD_CHECK_FAILED)
        {
            break;
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Passed.\r\n");
        }

    } while(false);

    /* Eal instance is closed as it can be re-opened in the other flow */
    (void)adi_eal_adcClose(ghAdcHandle);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief   This API must be called before OTP programming starts. It reads the bootloader status for each
 *          OTP block in the remote node. This API also flags any corrupted block and finds the first
 *          empty block for writing user scripts. Detailed boot status of each block is available in
 *          the aeBlockBootStatus field in the Remote cfg data structure.
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @return Status
 *          - #ADI_OTP_STATUS_NO_ERR                   OTP Boot successful without any ECC correcton in any OTP block
 *          - #ADI_OTP_STATUS_BOOT_TIMEOUT             OTP Boot didn't complete within a timeout period after soft reset
 *          - #ADI_OTP_STATUS_BOOT_FAIL                Found a corrupted OTP block
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_readBlockStatus(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    ADI_OTP_REMOTE_DATA *poRemoteOtpData;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_readBlockStatus (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Get the remote data */
    poRemoteOtpData = &poOtpCfg->paoRemoteOtpData[nRemoteNum];

    ADI_OTP_BLOCK_STATUS *paeBlockSt = &poRemoteOtpData->aeBlockBootStatus[0u];
    uint8_t i = 0u;

    eReturn = readBootStatus(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, poRemoteOtpData);
    printBlockStatus(poRemoteOtpData);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Checking for any corrupted blocks\r\n");

    if(eReturn == ADI_OTP_STATUS_NO_ERR)
    {
        /* First check the designer scripts for any corrupted block */
        for(i = ADI_OTP_NUM_HV_DATA_BLOCKS; i < ADI_OTP_NUM_HV_DATA_BLOCKS + ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS; i++)
        {
            /* First check if any block is corrupted or not */
            if(paeBlockSt[i] == ADI_OTP_BLOCK_STATUS_CORRUPTED)
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Block-%d is corrupted. Exiting!\r\n", i);
                eReturn = ADI_OTP_STATUS_PGM_FAIL;
                break;
            }
            else
            {
                /* Block is not corrupted; Good for writing */
            }
        }

        /* If so-far-so-good, find the first empty block for user-scripts */
        poRemoteOtpData->nUserScriptStartBlock = 0u;
        poRemoteOtpData->nUserScript2StartBlock = 0u;
        poRemoteOtpData->nFIOInstructionStartBlock = 0u;

        if(eReturn == ADI_OTP_STATUS_NO_ERR)
        {
            for(i = ADI_OTP_NUM_HV_DATA_BLOCKS + ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS;
                    i < ADI_OTP_MAX_NUM_BLOCKS; i++)
            {
                /* First check if any block is corrupted or not */
                if(paeBlockSt[i] == ADI_OTP_BLOCK_STATUS_CORRUPTED)
                {
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Block-%d is corrupted. Exiting!\r\n", i);
                    eReturn = ADI_OTP_STATUS_PGM_FAIL;
                    break;
                }

                /* If found an uninitialized block */
                if(paeBlockSt[i] == ADI_OTP_BLOCK_STATUS_UNINITIALIZED)
                {
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Found first empty OTP block %d for user scripts\r\n", i);
                    poRemoteOtpData->nUserScriptStartBlock = i;
                    poRemoteOtpData->nUserScript2StartBlock = i;
                    poRemoteOtpData->nFIOInstructionStartBlock = i;
                    break;
                }
            }
        }
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief Generates the raw OTP write data for the remote node
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @return Status
 *          - #ADI_OTP_STATUS_NO_ERR            OTP generate write data was successful
 *          - #ADI_OTP_STATUS_INSUFF_BLOCKS     Number of OTP blocks are not sufficient to program OTP cfg data
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_generateOtpWriteData(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_generateOtpWriteData (Remote-" UINT32_FORMATTER ")\r\n",
                nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Get the remote data */
    ADI_OTP_REMOTE_DATA *poRemoteOtpData = &poOtpCfg->paoRemoteOtpData[nRemoteNum];
    ADI_OTP_BLOCK_STATUS *paeBlockSt = &poRemoteOtpData->aeBlockBootStatus[0u];

    /* Generate the raw OTP data from block data structures */
    /* Clears the raw memory */
    (void)ADI_MEMSET(poRemoteOtpData->anOtpWriteData, 0, sizeof(poRemoteOtpData->anOtpWriteData));
    poRemoteOtpData->nNumUserScriptBlocks = 0U;
    poRemoteOtpData->nNumUserScript2Blocks = 0U;
    poRemoteOtpData->nNumFIOInstructionBlocks = 0U;
    poRemoteOtpData->nNumWriteBlocks = 0U;

    /*============================= Designer scripts =============================*/
    /* Generate raw data for designer scripts */
    generateDesignerScriptsRawData(poRemoteOtpData);

    /* First check if all the to-be-written designer blocks are empty */
    for(uint8_t i = ADI_OTP_NUM_HV_DATA_BLOCKS; i < ADI_OTP_NUM_HV_DATA_BLOCKS + ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS; i++)
    {
        /* If this block needs to be written. Check it must be empty */
        if(poRemoteOtpData->abOtpBlockWrEn[i])
        {
            if(paeBlockSt[i] == ADI_OTP_BLOCK_STATUS_UNINITIALIZED)
            {
                /* Block is empty; Good for writing */
            }
            else
            {
                /* Block is not empty, flag this as a programming failure */
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Block-%d is not empty. Exiting!\r\n", i);
                eReturn = ADI_OTP_STATUS_PGM_FAIL;
                break;
            }
        }
    }

    /*============================= User scripts =============================*/
    if(eReturn == ADI_OTP_STATUS_NO_ERR)
    {
        eReturn = generateOtpDataForUserScripts(poRemoteOtpData);
    }

    /*============================= Application Data =========================*/
    if(eReturn == ADI_OTP_STATUS_NO_ERR && poRemoteOtpData->bAppDataWrEn)
    {
        /* Generate raw data for designer scripts */
        eReturn = generateApplicationRawData(poRemoteOtpData);
    }

    if(eReturn == ADI_OTP_STATUS_NO_ERR)
    {
        printOtpData(poRemoteOtpData, WRITE_DATA);
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief Programs the OTP in the remote node and performs a soft reset.
 * @note  Before invoking this API, please make sure the raw data is generated
 *        by invoking the API adi_otp_generateOtpWriteData(). This function also clears
 *        the aeBlockBootStatus field in the remote OTP configs.
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @return Return status
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_programOtp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_OTP_REMOTE_DATA *poRemoteOtpData;
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    uint64_t nStartTime = 0ULL;
    uint64_t nEndTime = 0ULL;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_programOtp (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Get the remote data */
    poRemoteOtpData = &poOtpCfg->paoRemoteOtpData[nRemoteNum];

    adi_pal_getCurrTime(&nStartTime);
    /* Write raw data to remote OTP */
    eReturn = programOtpRawData(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, poRemoteOtpData);
    adi_pal_getCurrTime(&nEndTime);

    if(eReturn == ADI_OTP_STATUS_NO_ERR)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT,
                    "\t-Programming sequence completed in 0x" UINT32_HEX_FORMATTER UINT32_HEX_FORMATTER " usec\r\n",
                    (uint32_t)(((nEndTime - nStartTime) / (uint64_t)1000) >> 32U), (uint32_t)((nEndTime - nStartTime) / (uint64_t)1000));

        eReturn = bootloaderSoftReset(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum);

        /* Mandatory delay after a soft reset */
        // TODO: how much delay
        adi_pal_waitMicroSec(10ULL * MS_TO_US);
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief Reads and verifies the OTP data from the remote node against the write data
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @return Status
 *          - #ADI_OTP_STATUS_NO_ERR            Reading and verifies the OTP data was successful
 *          - #ADI_OTP_STATUS_READ_FAIL         Readback of OTP controller registers failed
 *          - #ADI_OTP_STATUS_VERIF_FAIL        OTP readback mismatch with the programmed
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_readVerifyOtp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_OTP_REMOTE_DATA *poRemoteOtpData;
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_readVerifyOtp (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Get the remote data */
    poRemoteOtpData = &poOtpCfg->paoRemoteOtpData[nRemoteNum];

    (void)setReadMode(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, ADI_OTP_READTYPE_DIFFERENTIAL_REDUNDANT);

    /* Read the OTP data from remote node */
    if(!readOtpSelectedBlocks(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, poRemoteOtpData))
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-OTP read failed\r\n");
        eReturn = ADI_OTP_STATUS_READ_FAIL;
    }
    else
    {
        printOtpData(poRemoteOtpData, READ_DATA);

        if(compareOtpReadData(poRemoteOtpData) == false)
        {
            eReturn = ADI_OTP_STATUS_VERIF_FAIL;
        }
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief Verifies the OTP lock status by reading lock registers.This function reads the
 *         last word of 63rd OTP block to determine if the OTP is locked.
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @return Status
 *           -#ADI_OTP_STATUS_LOCKED           OTP is locked.
 *           -#ADI_OTP_STATUS_NO_ERR           OTP is not locked.
 *           -#ADI_OTP_STATUS_READ_FAIL        Readback of OTP controller registers failed
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_verifyOtpLock(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_verifyOtpLock (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    static uint32_t aTmpData[1U] = {0u};
    uint8_t lockStatus[2U] = {0u};
    uint8_t nRegs = 0u;

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    /* Set the read mode for OTP data from remote node in Single Ended Mode */
    /* In Differential Redundant mode, the non programmed bytes in OTP block will read the garbage value*/
    (void)setReadMode(poOtpMgr->nEalInstNum, nRemoteNum, ADI_OTP_READTYPE_SINGLE_ENDED);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT,
                "\t-Verifying lock status by reading last word of the last block in single ended mode: ");

    ganRegmapAddr[nRegs] = ADI_OTP_RD_CONTENT_BASE_ADDR + (ADI_OTP_MAX_NUM_BLOCKS - 1u) * 8u +
                           (ADI_OTP_BLOCK_LEN_BYTES / 2u - 1u);
    ganRegmapMMS[nRegs++] = ADI_OTP_RD_CONTENT_MMS;

    /* Read the last word of the last block to check the OTP lock status */
    if(readRegs(poOtpMgr->nEalInstNum, nRemoteNum, ganRegmapAddr, &aTmpData[0U], nRegs, ganRegmapMMS))
    {
        lockStatus[0u] = (uint8_t)GET_FIELD_VAL(aTmpData[0U], 0x00FFU, 0U);
        lockStatus[1u] = (uint8_t)GET_FIELD_VAL(aTmpData[0U], 0xFF00U, 8U);

        /* check the value is non-zero to verify the OTP lock status */
        if(lockStatus[0u] != 0x0u || lockStatus[1u] != 0x0u)
        {
            eReturn = ADI_OTP_STATUS_LOCKED;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "OTP is Locked\r\n");
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "OTP is not Locked\r\n");
        }
    }
    else
    {
        /* If read itself has failed */
        eReturn = ADI_OTP_STATUS_READ_FAIL;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Read failed\r\n");
    }

    /*  Set Read the OTP data from remote node in Differential Redundant Mode*/
    (void)setReadMode(poOtpMgr->nEalInstNum, nRemoteNum, ADI_OTP_READTYPE_DIFFERENTIAL_REDUNDANT);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief Reads the bootloader status for the remote node. Detailed boot status of
 *         each block is available in the aeBlockBootStatus field in the
 *         Remote cfg data structure.
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @return Status
 *          - #ADI_OTP_STATUS_NO_ERR                   OTP Boot successful without any ECC correcton in any OTP block
 *          - #ADI_OTP_STATUS_BOOT_PASS_WITH_ECC_CORR  OTP Boot successful with atleast one ECC corrected block
 *          - #ADI_OTP_STATUS_BOOT_TIMEOUT             OTP Boot didn't complete within a timeout period after soft reset
 *          - #ADI_OTP_STATUS_BOOT_FAIL                OTP Boot fail due to atleast one corrupted OTP block which is not ECC-recoverable
 *                                                     or one block which is written but status is uninitialized
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_verifyBootStatus(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_verifyBootStatus (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    uint8_t i = 0u;

    /* Get the remote data */
    ADI_OTP_REMOTE_DATA *poRemoteOtpData = &poOtpCfg->paoRemoteOtpData[nRemoteNum];
    ADI_OTP_BLOCK_STATUS *paeBlockSt = &poRemoteOtpData->aeBlockBootStatus[0u];

    /* Read the boot loader status */
    eReturn = readBootStatus(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, poRemoteOtpData);

    /* If the return is OK, then proceed further */
    if(eReturn == ADI_OTP_STATUS_NO_ERR)
    {
        printBlockStatus(poRemoteOtpData);

        for(i = ADI_OTP_NUM_HV_DATA_BLOCKS; i < poRemoteOtpData->nUserScriptStartBlock + poRemoteOtpData->nNumUserScriptBlocks;
                i++)
        {
            /* First check if any block is corrupted or not - Fatal error */
            if(paeBlockSt[i] == ADI_OTP_BLOCK_STATUS_CORRUPTED)
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Block-%d status is corrupted. Exiting!\r\n", i);
                eReturn = ADI_OTP_STATUS_BOOT_FAIL; /* Return boot fail status */
                break;
            }

            /* Secondly, check if the block was written and is okay or not */
            if(poRemoteOtpData->abOtpBlockWrEn[i])
            {
                if(paeBlockSt[i] == ADI_OTP_BLOCK_STATUS_UNINITIALIZED)
                {
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Block-%d status is uninitialized but was written. Exiting!\r\n", i);
                    eReturn = ADI_OTP_STATUS_BOOT_FAIL; /* Return boot fail status */
                    break;
                }

                if(paeBlockSt[i] == ADI_OTP_BLOCK_STATUS_RECOVERABLE)
                {
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Block-%d status is ECC recoverable.\r\n", i);
                    eReturn = ADI_OTP_STATUS_BOOT_PASS_WITH_ECC_CORR; /* Return boot pass with ECC correction status */
                }
            }
        }
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief This API locks the OTP from any further writing by writing to the last 16-bit location of
 *         the last OTP block
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @return Return status
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_lockOtp(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_lockOtp (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(OTP_WAL_ADDR == OTP_WAL_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WAH_ADDR == OTP_WAH_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WDL_ADDR == OTP_WDL_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WDH_ADDR == OTP_WDH_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_OMC_ADDR == OTP_OMC_NW_ADDR, "Register address and Register NW address doesn't match");

    uint16_t nAddr = 0U;
    uint8_t nRegs = 0u;

    do
    {
        /* Enable OTP macro */
        eReturn = enableOtpMacro(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum);

        /* Return if OTP macro is not enabled*/
        if(eReturn != ADI_OTP_STATUS_NO_ERR)
        {
            break;
        }

        /* Write 0xADDA to the last word of the last block */
        uint16_t nBlockBaseAddr = ADI_OTP_MACRO_BASE_ADDR + (uint16_t)((ADI_OTP_MAX_NUM_BLOCKS - 1u) * ADI_OTP_BLOCK_LEN_BYTES /
                                  2u * 4u);
        uint16_t i = ADI_OTP_BLOCK_LEN_BYTES - 2u;

        /* Address location */
        nAddr = nBlockBaseAddr + 4u * i / 2u;
        ganRegmapAddr[nRegs] = OTP_WAL_ADDR;
        ganRegmapMMS[nRegs] = OTP_WAL_MMS;
        ganRegmapData[nRegs++] = (uint8_t)GET_FIELD_VAL(nAddr, 0x00FFU, 0U);
        ganRegmapAddr[nRegs] = OTP_WAH_ADDR;
        ganRegmapMMS[nRegs] = OTP_WAH_MMS;
        ganRegmapData[nRegs++] = (uint8_t)GET_FIELD_VAL(nAddr, 0xFF00U, 8U);

        /* Data */
        ganRegmapAddr[nRegs] = OTP_WDL_ADDR;
        ganRegmapMMS[nRegs] = OTP_WDL_MMS;
        ganRegmapData[nRegs++] = 0xADU;
        ganRegmapAddr[nRegs] = OTP_WDH_ADDR;
        ganRegmapMMS[nRegs] = OTP_WDH_MMS;
        ganRegmapData[nRegs++] = 0xDAU;

        /* OTP write command */
        ganRegmapAddr[nRegs] = OTP_OMC_ADDR;
        ganRegmapMMS[nRegs] = OTP_OMC_MMS;
        ganRegmapData[nRegs++] = 0x03U;

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Writing 0xADDA to the last word of the last OTP block\r\n");

        /* Write all the above regs in a single frame */
        if(!writeRegs(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, ganRegmapAddr, ganRegmapData, nRegs, ganRegmapMMS))
        {
            /* If write itself fails, then fatal error; break with failure */
            eReturn = ADI_OTP_STATUS_PGM_FAIL;
            break;
        }

        /* Check for the prog completed status */
        if(!chkOtpProgStatus(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum))
        {
            eReturn = ADI_OTP_STATUS_PGM_FAIL;
            break;
        }

        /* Write all the above regs in a single frame */
        if(!verifylockstatus(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum))
        {
            /* If verify fails, then fatal error; break with failure */
            eReturn = ADI_OTP_STATUS_PGM_FAIL;
            break;
        }

        /* Disable OTP macro */
        eReturn = disableOtpMacro(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum);

        if(eReturn != ADI_OTP_STATUS_NO_ERR)
        {
            break;
        }

    } while(false);

    if(eReturn != ADI_OTP_STATUS_NO_ERR)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Failed.\r\n");
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/*!*************************************************************************************************
 * @brief This API reads the raw contents of the selected OTP block range. It does not check the boot status
 *         of any block. Readback of uninitialized blocks will give random data which application must discard.
 * @param [in] hOtpHandle  OTP handle
 * @param [in] nRemoteNum  Remote number
 * @param [in] nStartBlock  Start block number to read the contents
 * @param [in] nBlockCount No. of blocks to read from the start block
 * @param [out] pBuffer  Buffer pointer to store the read contents.
 * @return Status
 *          - #ADI_OTP_STATUS_NO_ERR            Reading and verifies the OTP data was successful
 *          - #ADI_OTP_STATUS_READ_FAIL         Readback of OTP controller registers failed
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_readOtpContents(ADI_OTP_HANDLE hOtpHandle, uint32_t nRemoteNum, uint8_t nStartBlock,
                                       uint8_t nBlockCount, uint8_t *pBuffer)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    bool bSuccess;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "API-->adi_otp_readOtpContents (Remote-" UINT32_FORMATTER ")\r\n", nRemoteNum + 1U);

    /* Validate the pointer to OTP handle */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");

    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");

    ADI_DBG_REQUIRE(nStartBlock + nBlockCount <= ADI_OTP_MAX_NUM_BLOCKS, "Invalid arguments (start block or count)");

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Get the remote data */
    ADI_OTP_REMOTE_DATA *poRemoteOtpData = &poOtpCfg->paoRemoteOtpData[nRemoteNum];

    /* Read the OTP data from remote node */
    (void)setReadMode(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, ADI_OTP_READTYPE_DIFFERENTIAL_REDUNDANT);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Reading OTP blocks:");

    for(uint8_t i = nStartBlock; i < nStartBlock + nBlockCount; i++)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", i);
        /* Read the OTP block */
        bSuccess = readOtpBlock(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, poRemoteOtpData, i);

        if(!bSuccess)
        {
            break;
        }
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");

    if(!bSuccess)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Failed\r\n");

        eReturn = ADI_OTP_STATUS_READ_FAIL;
    }
    else
    {
        if(pBuffer != NULL)
        {
            (void)ADI_MEMCPY(pBuffer, &poRemoteOtpData->anOtpReadData[nStartBlock][0u],
                             (size_t)nBlockCount * ADI_OTP_BLOCK_LEN_BYTES);
        }

        /* Print the read data */
        printOtpBlocksRange(poRemoteOtpData, nStartBlock, nBlockCount);
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    return eReturn;
}

/**
 * @brief Reads user section data from OTP memory, with SECDED and general
 * parity error correction.
 *
 * This API reads blocks from OTP, verifies SECDED and general parity, attempts
 * single-bit error correction if needed, and copies valid payload data to the
 * output buffer. It returns status codes indicating if data was read without
 * error, with single-bit correction, or if a double-bit error was detected.
 *
 * @param[in]  hOtpHandle     OTP handle
 * @param[in]  nRemoteNum     Remote Number
 * @param[in]  nStartBlockNum Start block number of OTP user section.
 * @param[out] pRdDataBuffer  Pointer to the buffer to store the read data from OTP user section.
 * @param[in]  nRdDataLen     Data length to be read from OTP user section in bytes.
 * @return                    ADI_OTP_STATUS_NO_ERR, ADI_OTP_STATUS_ECC_SINGLE_BIT_CORRECTED,
 *                            ADI_OTP_STATUS_ECC_DOUBLE_BIT_ERROR, or ADI_OTP_STATUS_READ_FAIL
 */
ADI_EAL_OTP_CODE
ADI_OTP_STATUS adi_otp_readUserSectionData(ADI_OTP_HANDLE hOtpHandle,
        uint32_t nRemoteNum, uint8_t nStartBlockNum, uint8_t *pRdDataBuffer,
        uint16_t nRdDataLen)
{
    uint16_t nReadDataIndex = ADI_VAL_ZERO;
    uint16_t nRemainingBytes = ADI_VAL_ZERO;
    uint8_t  nBytesToCopy = ADI_VAL_ZERO;
    uint8_t nBlockNum = ADI_VAL_ZERO;
    uint8_t nRdSecded = ADI_VAL_ZERO;
    uint8_t nRdChecksum = ADI_VAL_ZERO;
    uint8_t nComputedChecksum = ADI_VAL_ZERO;
    uint8_t nComputedSecded = ADI_VAL_ZERO;
    uint8_t *pnOtpBlock = NULL;
    uint8_t *pnOtpBlockPayload = NULL;
    uint8_t nGeneralParity  = ADI_VAL_ZERO;
    bool bSuccess = false;
    bool bValid = false;
    bool bBitError = false;

    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "read OTP user section data (Remote -" UINT32_FORMATTER
                ", Start block number-"UINT32_FORMATTER ",Read data length-"UINT32_FORMATTER")\r\n", nRemoteNum + 1U, nStartBlockNum,
                nRdDataLen);

    /* Validate input pointers and parameters */
    ADI_DBG_REQUIRE(hOtpHandle != NULL, "OTP handle is null");
    ADI_OTP_MGR *poOtpMgr = (ADI_OTP_MGR *)hOtpHandle;
    ADI_DBG_REQUIRE(poOtpMgr->bInUse == true, "OTP driver is not initialized");
    ADI_DBG_REQUIRE(pRdDataBuffer != NULL, "Output buffer is null");
    ADI_DBG_REQUIRE(nRdDataLen <= ((ADI_OTP_MAX_NUM_BLOCKS - (uint32_t)nStartBlockNum) * ADI_OTP_PAYLOAD_LEN_BYTES),
                    "Invalid OTP user section read data length");
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;
    ADI_DBG_REQUIRE(nRemoteNum < poOtpCfg->nNumNodes, "Invalid remote number");
    ADI_DBG_REQUIRE((nStartBlockNum >= (ADI_OTP_NUM_HV_DATA_BLOCKS + ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS))
                    && (nStartBlockNum < ADI_OTP_MAX_NUM_BLOCKS), "Invalid OTP user section start block");

    /* Set OTP read mode */
    (void)setReadMode(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, ADI_OTP_READTYPE_DIFFERENTIAL_REDUNDANT);

    /* Set number of bytes to read from OTP user section */
    nRemainingBytes = nRdDataLen;

    /* Get the remote data structure */
    ADI_OTP_REMOTE_DATA *poRemoteOtpData = &poOtpCfg->paoRemoteOtpData[nRemoteNum];

    /* Iterate through OTP blocks starting from the specified block */
    for(nBlockNum = nStartBlockNum; nBlockNum < ADI_OTP_MAX_NUM_BLOCKS; nBlockNum++)
    {

        /* Read the block from OTP */
        bSuccess = readOtpBlock(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum, poRemoteOtpData, nBlockNum);

        if(!bSuccess)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[ECC] Block"UINT32_FORMATTER": Read failed\r\n", nBlockNum);
            eReturn = ADI_OTP_STATUS_READ_FAIL;
            break;
        }

        /* initial block read data */
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[ECC] Block"UINT32_FORMATTER": Initial data: ", nBlockNum);

        for(uint8_t nIndex = ADI_VAL_ZERO; nIndex < ADI_OTP_BLOCK_LEN_BYTES; nIndex++)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "%02X ", poRemoteOtpData->anOtpReadData[nBlockNum][nIndex]);
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");

        nRdSecded = (poRemoteOtpData->anOtpReadData[nBlockNum][0u] & ADI_OTP_SECDED_DATA_MASK);
        pnOtpBlock = poRemoteOtpData->anOtpReadData[nBlockNum];
        pnOtpBlockPayload = &poRemoteOtpData->anOtpReadData[nBlockNum][1u];

        /* Extract and compute ECC fields */
        nRdChecksum = poRemoteOtpData->anOtpReadData[nBlockNum][ADI_OTP_PAYLOAD_LEN_BYTES + 1u];
        nComputedChecksum = ComputeChecksum(pnOtpBlockPayload, ADI_OTP_PAYLOAD_LEN_BYTES);
        nComputedSecded = (ComputeSecdedCode(pnOtpBlockPayload, nRdChecksum) & ADI_OTP_SECDED_DATA_MASK);

        /* Check if block is valid (no error) */
        bValid = (nRdSecded == nComputedSecded) && (nRdChecksum == nComputedChecksum);

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT,
                    "[ECC] Block "UINT32_FORMATTER": Valid="UINT32_FORMATTER", Stored SECDED="UINT32_FORMATTER", Computed SECDED="UINT32_FORMATTER", Stored Checksum=0x%02X, Computed Checksum=0x%02X\r\n",
                    nBlockNum, bValid, nRdSecded, nComputedSecded, nRdChecksum, nComputedChecksum);

        /* Check for read OTP block data is valid */
        if(bValid)
        {
            /* No error detected */
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT,
                        "[ECC] Block"UINT32_FORMATTER": NO_ERROR (valid, general parity even)\r\n", nBlockNum);
        }
        else
        {
            /* Try to correct single-bit error by flipping each bit and checking syndrome */
            bBitError = false;
            uint8_t anBlockData[ADI_OTP_BLOCK_LEN_BYTES];
            uint8_t nByteIdx = ADI_VAL_ZERO;
            uint8_t nBitIdx = ADI_VAL_ZERO;
            uint8_t nSyndrome = ADI_VAL_ZERO;

            for (uint8_t nIdx = 0u; nIdx < 128u; ++nIdx)
            {
                (void)ADI_MEMCPY(anBlockData, pnOtpBlock, ADI_OTP_BLOCK_LEN_BYTES);

                nByteIdx = nIdx / 8u;
                nBitIdx = nIdx % 8u;
                anBlockData[nByteIdx] ^= (1u << (7u - nBitIdx));

                nSyndrome = calculateDecodeSyndrome(anBlockData, ganRom);

                /* Check for bit error found and corrected */
                if(nSyndrome == 0u)
                {
                 (void)ADI_MEMCPY(pnOtpBlock, anBlockData, ADI_OTP_BLOCK_LEN_BYTES);
                    bBitError = true;
                    break;
                }
            }

            /* Check general parity after correction */
            nGeneralParity = checkGeneralParityBit(pnOtpBlock, ADI_OTP_BLOCK_LEN_BYTES);

            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[ECC] Block "UINT32_FORMATTER": General parity check: "UINT32_FORMATTER"\r\n",
                        nBlockNum, nGeneralParity);

            /* Check for bit error correction */
            if(bBitError)
            {
                /* Check for computed general parity is even */
                if(nGeneralParity == 0u)
                {
                    /* Single-bit error detected and corrected */
                    nRdSecded = (pnOtpBlock[0u] & ADI_OTP_SECDED_DATA_MASK);
                    pnOtpBlockPayload = &pnOtpBlock[1u];
                    nRdChecksum = pnOtpBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1u];
                    nComputedChecksum = ComputeChecksum(pnOtpBlockPayload, ADI_OTP_PAYLOAD_LEN_BYTES);
                    nComputedSecded = (ComputeSecdedCode(pnOtpBlockPayload, nRdChecksum) & ADI_OTP_SECDED_DATA_MASK);
                    bValid = (nRdSecded == nComputedSecded) && (nRdChecksum == nComputedChecksum);

                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT,
                    "[ECC] Block "UINT32_FORMATTER": SINGLE_BIT_ERROR (corrected)\r\n", nBlockNum);
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[ECC] Block "UINT32_FORMATTER": Valid after correction="UINT32_FORMATTER"\r\n", nBlockNum, bValid);

                    eReturn = ADI_OTP_STATUS_ECC_SINGLE_BIT_CORRECTED;
                }
                else
                {

                /* Double-bit error detected (uncorrectable) */
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[ECC] Block "UINT32_FORMATTER": DOUBLE_BIT_ERROR (uncorrectable)\r\n",
                            nBlockNum);
                eReturn = ADI_OTP_STATUS_ECC_DOUBLE_BIT_ERROR;
                break;
                }
            }
        }

            if(bValid)
            {
                /* Copy valid payload to output buffer */
                nBytesToCopy = (uint8_t)((nRemainingBytes > ADI_OTP_PAYLOAD_LEN_BYTES) ? ADI_OTP_PAYLOAD_LEN_BYTES : nRemainingBytes);
                (void)ADI_MEMCPY(&pRdDataBuffer[nReadDataIndex], pnOtpBlockPayload, nBytesToCopy);
                nReadDataIndex += nBytesToCopy;
                nRemainingBytes -= nBytesToCopy;

                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[ECC] Block "UINT32_FORMATTER": Data copied ("UINT32_FORMATTER" bytes)\r\n", nBlockNum, nBytesToCopy);
            }
            else
            {
                /* Block is invalid after correction, skip copying */
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[ECC] Block "UINT32_FORMATTER": Invalid after correction. Skipping.\r\n", nBlockNum);
                eReturn = ADI_OTP_STATUS_READ_FAIL;
                break;
            }

            /* Check for reading OTP data complete */
            if(nRemainingBytes == ADI_VAL_ZERO)
            {
                break;
            }
    }
    return eReturn;
}

/*=============================== STATIC CODE ===============================*/
/*! \cond PRIVATE */
/*! @brief Writes remote node PLCA */
ADI_EAL_OTP_CODE
static bool setupRemotePlca(ADI_OTP_MGR *poOtpMgr)
{
    bool bReturn = true;

    /* Get the OTP cfg */
    ADI_OTP_CONFIG *poOtpCfg = poOtpMgr->poOtpCfg;

    /* Write the PLCA ID from Pgm time cfg and enable PLCA */
    ganRegmapAddr[0U] = PHY_OA_PLCA_CTRL1_NW_ADDR;
    ganRegmapMask[0U] = BITM_PHY_OA_PLCA_CTRL1_PLCALOCALID;
    ganRegmapData[0U] = poOtpCfg->poPgmTimeCfg->nRemotePlcaId;
    /* Enable PLCA */
    ganRegmapAddr[1U] = PHY_OA_PLCA_CTRL0_NW_ADDR;
    ganRegmapMask[1U] = BITM_PHY_OA_PLCA_CTRL0_PLCAEN;
    ganRegmapData[1U] = BITM_PHY_OA_PLCA_CTRL0_PLCAEN;

    ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
    oRegCfg.anStartAddr = ganRegmapAddr;
    oRegCfg.nLength = 2U;
    oRegCfg.pBuffer = ganRegmapData;
    oRegCfg.anMask = ganRegmapMask;
    oRegCfg.eType = ADI_E2BCORE_RND_MASK_WRITE;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Setting up remote PLCA ID as 1: ");

    /* Write the PLCA ID */
    if(adi_eal_sendRegTransaction(poOtpMgr->nEalInstNum, 0u, &oRegCfg) != ADI_EAL_STATUS_SUCCESS)
    {
        bReturn = false;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
    }
    else
    {
        adi_network_flushTxBuffer(poOtpMgr->nDevNum, 0U);
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");
    }

    return bReturn;
}

/*! @brief Ping remote node and check response */
ADI_EAL_OTP_CODE
static bool pingRemoteNode(ADI_OTP_MGR *poOtpMgr)
{
    bool bReturn = true;

    /* Ping the remote node and wait for ping response */
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Pinging remote node: ");
    poOtpMgr->bRemotePingRespRcvd = false;

    if(adi_eal_pingRemote(poOtpMgr->nEalInstNum, poOtpMgr->nE2bCfgRemoteNum) != ADI_EAL_STATUS_SUCCESS)
    {
        bReturn = false;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
    }
    else /* Wait for ping response */
    {
        uint64_t nCurrTimeNs, nStartTimeNs;
        adi_pal_getCurrTime(&nStartTimeNs);

        while(poOtpMgr->bRemotePingRespRcvd == false)
        {
            adi_network_run();
            adi_pal_getCurrTime(&nCurrTimeNs);

            /* Check if response is received or else timeout has occured */
            if(poOtpMgr->bRemotePingRespRcvd)
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Response received\r\n");
                break;
            }
            else if(nCurrTimeNs - nStartTimeNs > ADI_OTP_PING_RESP_TIMEOUT_MS * MS_TO_NS)
            {
                bReturn = false;
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed with timeout\r\n");
                break;
            }
            else
            {
                /* Nothing to do */
            }
        }
    }

    return bReturn;
}

/*! @brief Generates the raw OTP data for designer scripts */
ADI_EAL_OTP_CODE
static void generateDesignerScriptsRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    ADI_OTP_DATA_DESIGNER *pDesigner = &poRemoteOtpData->oOtpDesignerData;

    uint8_t *pBlock = NULL;
    uint8_t nDesBlockIndex = 0u;
    uint8_t nTgtOtpBlock = 0u;
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Designer scripts target OTP blocks:");

    /*================================================================================*/
    /********************** Designer Script 0 : Mapped to Block 3 *********************/
    /*================================================================================*/
    nDesBlockIndex = 0u;
    nTgtOtpBlock = nDesBlockIndex + ADI_OTP_NUM_HV_DATA_BLOCKS;

    /* Check if write is required for this block */
    if(poRemoteOtpData->bDesignerBlockWrEn[nDesBlockIndex])
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nTgtOtpBlock);

        pBlock = &poRemoteOtpData->anOtpWriteData[nTgtOtpBlock][0U];

        /* Device MAC address */
        pBlock[1U] = pDesigner->anDevMacAddr[5U]; /* LSB of MAC */
        pBlock[2U] = pDesigner->anDevMacAddr[4U];
        pBlock[3U] = pDesigner->anDevMacAddr[3U];
        pBlock[4U] = pDesigner->anDevMacAddr[2U];
        pBlock[9U] = pDesigner->anDevMacAddr[1U];
        pBlock[10U] = pDesigner->anDevMacAddr[0U]; /* MSB of MAC */

        /* Multicast MAC address */
        pBlock[5U] = pDesigner->anMulticastMacAddr[5U];
        pBlock[6U] = pDesigner->anMulticastMacAddr[4U];
        pBlock[7U] = pDesigner->anMulticastMacAddr[3U];
        pBlock[8U] = pDesigner->anMulticastMacAddr[2U];
        pBlock[11U] = pDesigner->anMulticastMacAddr[1U];
        pBlock[12U] = pDesigner->anMulticastMacAddr[0U];

        /* Checksum */
        pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

        /* SECDED data */
        pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

        /* Enable writing of this block */
        poRemoteOtpData->abOtpBlockWrEn[nTgtOtpBlock] = true;
        poRemoteOtpData->nNumWriteBlocks++;
    }

    /*================================================================================*/
    /********************** Designer Script 1 : Mapped to Block 4 *********************/
    /*================================================================================*/
    nDesBlockIndex = 1u;
    nTgtOtpBlock = nDesBlockIndex + ADI_OTP_NUM_HV_DATA_BLOCKS;

    /* Check if write is required for this block */
    if(poRemoteOtpData->bDesignerBlockWrEn[nDesBlockIndex])
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nTgtOtpBlock);

        pBlock = &poRemoteOtpData->anOtpWriteData[nTgtOtpBlock][0U];
        /* Boot loader config word */
        pBlock[1U] = (uint8_t)GET_FIELD_VAL(pDesigner->nBootloaderCfg, 0x00FFU, 0U);
        pBlock[2U] = (uint8_t)GET_FIELD_VAL(pDesigner->nBootloaderCfg, 0xFF00U, 8U);

        /* Device ID */
        pBlock[3U] = (uint8_t)GET_FIELD_VAL(pDesigner->nDeviceId, 0x00FFU, 0U);
        pBlock[4U] = (uint8_t)GET_FIELD_VAL(pDesigner->nDeviceId, 0xFF00U, 8U);

        /* Device version */
        pBlock[5U] = pDesigner->nDeviceVer;

        /* PHY identifier */
        (void)ADI_MEMCPY(&pBlock[7U], &pDesigner->anPhyIdentifier[0U], ADI_OTP_PHY_ID_LEN_BYTES);

        /* AO map config fields 0 & 1 */
        pBlock[13U] = pDesigner->nAoCfgFields0;
        pBlock[14U] = pDesigner->nAoCfgFields1;

        /* Checksum */
        pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

        /* SECDED data */
        pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

        /* Enable writing of this block */
        poRemoteOtpData->abOtpBlockWrEn[nTgtOtpBlock] = true;
        poRemoteOtpData->nNumWriteBlocks++;
    }

    /*================================================================================*/
    /********************** Designer Script 2 : Mapped to Block 5 *********************/
    /*================================================================================*/
    nDesBlockIndex = 2u;
    nTgtOtpBlock = nDesBlockIndex + ADI_OTP_NUM_HV_DATA_BLOCKS;

    /* Check if write is required for this block */
    if(poRemoteOtpData->bDesignerBlockWrEn[nDesBlockIndex])
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nTgtOtpBlock);

        pBlock = &poRemoteOtpData->anOtpWriteData[nTgtOtpBlock][0U];

        /* OEM data */
        (void)ADI_MEMCPY(&pBlock[1U], &pDesigner->anOemData[0U], ADI_OTP_OEM_DATA_LEN_BYTES);

        /* Checksum */
        pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

        /* SECDED data */
        pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

        /* Enable writing of this block */
        poRemoteOtpData->abOtpBlockWrEn[nTgtOtpBlock] = true;
        poRemoteOtpData->nNumWriteBlocks++;
    }

    /*================================================================================*/
    /********************** Designer Script 3 : Mapped to Block 6 *********************/
    /*================================================================================*/
    nDesBlockIndex = 3u;
    nTgtOtpBlock = nDesBlockIndex + ADI_OTP_NUM_HV_DATA_BLOCKS;

    /* Check if write is required for this block */
    if(poRemoteOtpData->bDesignerBlockWrEn[nDesBlockIndex])
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nTgtOtpBlock);

        pBlock = &poRemoteOtpData->anOtpWriteData[nTgtOtpBlock][0U];

        /* Manufacturer data */
        (void)ADI_MEMCPY(&pBlock[1U], &pDesigner->anMfgData[0U], ADI_OTP_MFG_DATA_LEN_BYTES);

        /* Checksum */
        pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

        /* SECDED data */
        pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

        /* Enable writing of this block */
        poRemoteOtpData->abOtpBlockWrEn[nTgtOtpBlock] = true;
        poRemoteOtpData->nNumWriteBlocks++;
    }

    /*================================================================================*/
    /********************** Designer Script 4 : Mapped to Block 7 *********************/
    /*================================================================================*/
    nDesBlockIndex = 4u;
    nTgtOtpBlock = nDesBlockIndex + ADI_OTP_NUM_HV_DATA_BLOCKS;

    /* Check if write is required for this block */
    if(poRemoteOtpData->bDesignerBlockWrEn[nDesBlockIndex])
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nTgtOtpBlock);

        pBlock = &poRemoteOtpData->anOtpWriteData[nTgtOtpBlock][0U];

        /* PLCA local node ID */
        pBlock[1U] = poRemoteOtpData->oOtpDesignerData.nPlcaLocalId;

        /* PLCA node count */
        pBlock[2U] = poRemoteOtpData->oOtpDesignerData.nPlcaNodeCount;

        /* PLCA LUT codes - Each byte holds two codes of 4-bit each */
        uint8_t *pCodes = &poRemoteOtpData->oOtpDesignerData.anPlcaLutCode[0u];

        for(uint8_t i = 0u; i < ADI_OTP_PLCA_LUT_CODE_LEN / 2u; i++)
        {
            pBlock[3U + i] = ((pCodes[2u * i + 1u] & 0x0Fu) << 4u) | ((pCodes[2u * i] & 0x0Fu));
        }

        /* PLCA LUT enable */
        pBlock[11U] = (uint8_t)(poRemoteOtpData->oOtpDesignerData.bPlcaLutEn) & 0x01u;

        /* PLCA enable */
        pBlock[14U] = ((uint8_t)(poRemoteOtpData->oOtpDesignerData.bPlcaEnable) & 0x01u) << 7u;

        /* Checksum */
        pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

        /* SECDED data */
        pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

        /* Enable writing of this block */
        poRemoteOtpData->abOtpBlockWrEn[nTgtOtpBlock] = true;
        poRemoteOtpData->nNumWriteBlocks++;
    }

    /*================================================================================*/

    /* Update last OTP block written */
    poRemoteOtpData->nLastOtpBlockWritten = ADI_OTP_NUM_HV_DATA_BLOCKS + ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS - 1u;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");
}

/*! @brief Generates the raw OTP data for user scripts 1 & 2 */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS generateUserScriptsRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData, bool bIsUserScript2)
{
    uint8_t nBlockIdx;
    uint8_t nNumOtpUserData;
    uint8_t i, k;
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    uint8_t *pBlock = NULL;
    ADI_OTP_DATA_USER *poUserData;

    if(bIsUserScript2)
    {
        nBlockIdx = poRemoteOtpData->nUserScript2StartBlock;
        nNumOtpUserData = poRemoteOtpData->nNumOtpUserData2;
    }
    else
    {
        nBlockIdx = poRemoteOtpData->nUserScriptStartBlock;
        nNumOtpUserData = poRemoteOtpData->nNumOtpUserData;
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-User scripts target OTP blocks:");

    for(i = 0U; i < nNumOtpUserData; i++)
    {
        uint32_t nRegRemaining = 0U;
        uint8_t nOneRegSpace = 0U;
        uint8_t nNumRegsInOneBlock = 0U;
        ADI_E2BCORE_REG_ADDR_VAL *aoRegData = NULL;

        if(bIsUserScript2)
        {
            poUserData = &poRemoteOtpData->paoOtpUserData2[i];
        }
        else
        {
            poUserData = &poRemoteOtpData->paoOtpUserData[i];
        }

        nRegRemaining = poUserData->nNumRegs;
        nOneRegSpace = (2U + ((uint8_t)poUserData->eDataMode + 1u) * 2u);
        nNumRegsInOneBlock = (ADI_OTP_PAYLOAD_LEN_BYTES - 1u) / nOneRegSpace;
        aoRegData = poUserData->aoRegData;

        /* Script format */
        uint8_t nScriptFormat = (((uint8_t)ADI_OTP_BLOCK_TYPE_USER_SCRIPT) << 6U) |
                                (((uint8_t)poUserData->eDataMode & 0x01U) << 5U) |
                                (((uint8_t)poUserData->nIdx & 0xFU) << 0U);

        uint8_t nRegCnt = 0U;

        while(nRegRemaining > 0U)
        {
            /* Check if block index exceeds the max OTP blocks before accessing the array */
            if(nBlockIdx >= ADI_OTP_MAX_NUM_BLOCKS)
            {
                /* Calculate additional blocks needed for remaining registers */
                uint8_t nExceededBlocks = (uint8_t)((nRegRemaining + nNumRegsInOneBlock - 1U) / nNumRegsInOneBlock);
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Insufficient empty blocks for writing user scripts (request exceeds available blocks by %d)\n\r\n\r",
                            nExceededBlocks);
                eReturn = ADI_OTP_STATUS_INSUFF_BLOCKS;
                break;
            }

            /* Check if target block is empty */
            if(poRemoteOtpData->aeBlockBootStatus[nBlockIdx] != ADI_OTP_BLOCK_STATUS_UNINITIALIZED)
            {
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "\n\r\t-: Block-%d is not empty. Exiting!\r\n", nBlockIdx);
                eReturn = ADI_OTP_STATUS_PGM_FAIL;
                break;
            }

            /* Find if remaining registers are less than number of regs that can be written in one block */
            uint8_t n = (uint8_t)((nRegRemaining < nNumRegsInOneBlock) ? nRegRemaining : nNumRegsInOneBlock);

            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nBlockIdx);

            /* Generate block payload from the n registers */
            pBlock = &poRemoteOtpData->anOtpWriteData[nBlockIdx][0U];
            k = 1U;

            /* First fill all the 14 bytes of payload as 0xFF */
            (void)ADI_MEMSET(&pBlock[k], 0xFF, ADI_OTP_PAYLOAD_LEN_BYTES);

            pBlock[k++] = nScriptFormat;

            for(uint8_t j = 0U; j < n; j++)
            {
                otpUtils_AddRegFields(&pBlock[k], aoRegData[nRegCnt].nAddr,
                    aoRegData[nRegCnt].nVal, poUserData->eDataMode);
                nRegCnt++;
                k += nOneRegSpace;
            }

            /* Reduce the remaining registers by above written 'n' registers */
            nRegRemaining -= n;

            /* Compute checksum */
            pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

            /* Compute SECDED data */
            pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

            /* Enable writing to this block */
            poRemoteOtpData->abOtpBlockWrEn[nBlockIdx] = true;

            /* Make the last block written as block index */
            poRemoteOtpData->nLastOtpBlockWritten = nBlockIdx;

            /* Point to the next block */
            nBlockIdx++;

            /* If the data is for user script 1 mark the current block index as the start point for FIO data / user script 2 space data */
            if(bIsUserScript2 == false)
            {
                poRemoteOtpData->nFIOInstructionStartBlock = nBlockIdx;
                poRemoteOtpData->nUserScript2StartBlock = nBlockIdx;
                poRemoteOtpData->nNumUserScriptBlocks++;
            }
            else
            {
                poRemoteOtpData->nNumUserScript2Blocks++;
            }
        }

        if((eReturn == ADI_OTP_STATUS_INSUFF_BLOCKS) || (eReturn == ADI_OTP_STATUS_PGM_FAIL))
        {
            break;
        }

        poUserData += sizeof(ADI_OTP_DATA_USER);
    }

    poRemoteOtpData->nNumWriteBlocks += (bIsUserScript2 == false) ?  poRemoteOtpData->nNumUserScriptBlocks :
                                        poRemoteOtpData->nNumUserScript2Blocks;
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");
    return eReturn;
}

/*! @brief Generates the raw OTP data for user scripts */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS generateOtpDataForUserScripts(ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    if(poRemoteOtpData->bUserDataWrEn)
    {
        /* Generate user script data. Checking of sufficient empty blocks is done while generation only */
        eReturn = generateUserScriptsRawData(poRemoteOtpData, false);
    }

    if(eReturn == ADI_OTP_STATUS_NO_ERR && poRemoteOtpData->bFIODataWrEn)
    {
        /* Generate Flexible I/O instructions OTP data. Checking of sufficient empty blocks is done while generation only */
        eReturn = generateFIOInstructionsRawData(poRemoteOtpData);
    }

    if(eReturn == ADI_OTP_STATUS_NO_ERR && poRemoteOtpData->bUserData2WrEn)
    {
        /* Generate user script data. Checking of sufficient empty blocks is done while generation only */
        eReturn = generateUserScriptsRawData(poRemoteOtpData, true);
    }

    return eReturn;
}

/*! @brief Generates the raw OTP data for FIO instructions */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS generateFIOInstructionsRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    uint8_t nBlockIdx = poRemoteOtpData->nFIOInstructionStartBlock;
    uint8_t i, k;
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    uint8_t *pBlock = NULL;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Flexible IO instructions target OTP blocks:");

    for(i = 0U; i < poRemoteOtpData->nNumOtpFIOData; i++)
    {
        ADI_OTP_DATA_FIO *poFIOData = &poRemoteOtpData->paoOtpFIOData[i];
        uint32_t nNumInstructionRemaining = poFIOData->nNumInstruction;
        uint8_t nOneFIOInstSpace = 2U;
        uint8_t nNumFIOInstInOneBlock = (ADI_OTP_PAYLOAD_LEN_BYTES - 2U) / nOneFIOInstSpace;

        uint8_t nInstCnt = 0U;

        while(nNumInstructionRemaining > 0U)
        {
            /* Check if block index exceeds the max OTP blocks before accessing the array */
            if(nBlockIdx >= ADI_OTP_MAX_NUM_BLOCKS)
            {
                /* Calculate additional blocks needed for remaining FIO instructions */
                uint8_t nExceededBlocks = (uint8_t)((nNumInstructionRemaining + nNumFIOInstInOneBlock - 1U) / nNumFIOInstInOneBlock);
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Insufficient empty blocks for writing flexible IO instructions (request exceeds available blocks by %d)\n\r\n\r",
                            nExceededBlocks);
                eReturn = ADI_OTP_STATUS_INSUFF_BLOCKS;
                break;
            }

            /* Script format */
            uint8_t nHeader1 = 0x00U |
                               (((uint8_t)ADI_OTP_BLOCK_TYPE_FLEXIO_PROG) << 6U) |
                               (((uint8_t)(nNumInstructionRemaining == poFIOData->nNumInstruction) & 0x01U) << 4U) |
                               (((uint8_t)poFIOData->nIdx & 0xFU) << 0U);
            uint8_t nHeader2 = 0x00U;

            /* Check if target block is empty */
            if(poRemoteOtpData->aeBlockBootStatus[nBlockIdx] != ADI_OTP_BLOCK_STATUS_UNINITIALIZED)
            {
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "\n\r\t-: Block-%d is not empty. Exiting!\r\n", nBlockIdx);
                eReturn = ADI_OTP_STATUS_PGM_FAIL;
                break;
            }

            /* Find if remaining FIO instructions are less than number of FIO instructions that can be written in one block */
            uint8_t n = (uint8_t)((nNumInstructionRemaining < nNumFIOInstInOneBlock) ? nNumInstructionRemaining :
                                  nNumFIOInstInOneBlock);

            /* Reduce the remaining registers by above written 'n' registers */
            nNumInstructionRemaining -= n;

            nHeader1 |= ((uint8_t)(nNumInstructionRemaining == 0U)) << 5U;
            nHeader2 |= (((uint8_t)n / 2U) << 6U);

            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nBlockIdx);

            /* Generate block payload from the n registers */
            pBlock = &poRemoteOtpData->anOtpWriteData[nBlockIdx][0U];
            k = 1U;

            /* First fill all the 14 bytes of payload as 0xFF */
            (void)ADI_MEMSET(&pBlock[k], 0xFF, ADI_OTP_PAYLOAD_LEN_BYTES);

            pBlock[k++] = nHeader1;
            pBlock[k++] = nHeader2;

            for(uint8_t j = 0U; j < n; j++)
            {
                pBlock[k++] = (uint8_t)(poFIOData->panInstructionMemData[nInstCnt] & 0xffU);
                pBlock[k++] = (uint8_t)((poFIOData->panInstructionMemData[nInstCnt] & 0xFF00U) >> 8U);
                nInstCnt++;
            }

            /* Checksum */
            pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

            /* SECDED data */
            pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

            /* Enable writing to this block */
            poRemoteOtpData->abOtpBlockWrEn[nBlockIdx] = true;
            poRemoteOtpData->nNumFIOInstructionBlocks++;

            poRemoteOtpData->nLastOtpBlockWritten = nBlockIdx;

            /* Point to the next block */
            nBlockIdx++;
        }
    }

    poRemoteOtpData->nUserScript2StartBlock = nBlockIdx;
    poRemoteOtpData->nNumWriteBlocks += poRemoteOtpData->nNumFIOInstructionBlocks;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");

    return eReturn;
}

/*! @brief Generates the raw OTP data meant for application */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS generateApplicationRawData(ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    /* User blocks start with the */
    uint8_t nBlockIdx = poRemoteOtpData->nAppDataTgtOtpBlock;
    uint16_t k;
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    uint8_t *pBlock = NULL;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Application data target OTP blocks:");

    /* Check if sufficient blocks are available */
    uint16_t nBlocksNeeded = (poRemoteOtpData->nAppDataLenBytes + ADI_OTP_BLOCK_LEN_BYTES - 1u) / ADI_OTP_BLOCK_LEN_BYTES;
    uint16_t nRemainingBytes = poRemoteOtpData->nAppDataLenBytes;

    if(nBlockIdx >= ADI_OTP_MAX_NUM_BLOCKS || nBlocksNeeded > ADI_OTP_MAX_NUM_BLOCKS - (uint16_t)nBlockIdx)
    {
        /* Not enough OTP blocks. Exit */
        eReturn = ADI_OTP_STATUS_INSUFF_BLOCKS;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n\t-Insufficient blocks for writing application data\n\r\n\r");
    }

    if(nBlockIdx <= poRemoteOtpData->nLastOtpBlockWritten)
    {
        eReturn = ADI_OTP_STATUS_APP_TARGET_BLOCK_USED;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n\t-Target block specified to program application data is already used\n\r\n\r");
    }

    /* Check if there is at least one uninitialized block between the last written block and app data.
     * The bootloader requires an uninitialized block as a marker to stop processing before app data. */
    if((eReturn == ADI_OTP_STATUS_NO_ERR) && (nBlockIdx == (poRemoteOtpData->nLastOtpBlockWritten + 1U)))
    {
        eReturn = ADI_OTP_STATUS_INSUFF_BLOCKS;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n\t-At least one uninitialized block required before application data (bootloader marker)\n\r\n\r");
    }

    /* Proceed if there is no error */
    if(eReturn == ADI_OTP_STATUS_NO_ERR)
    {
        k = 0u;

        while(nRemainingBytes > 0u)
        {
            /* Check if target block is empty */
            if(poRemoteOtpData->aeBlockBootStatus[nBlockIdx] != ADI_OTP_BLOCK_STATUS_UNINITIALIZED)
            {
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "\r\n\t-: Block-%d is not empty. Exit programming!\r\n", nBlockIdx);
                eReturn = ADI_OTP_STATUS_PGM_FAIL;
                break;
            }

            uint16_t n = (uint8_t)((nRemainingBytes < ADI_OTP_PAYLOAD_LEN_BYTES) ? nRemainingBytes : ADI_OTP_PAYLOAD_LEN_BYTES);

            pBlock = &poRemoteOtpData->anOtpWriteData[nBlockIdx][0U];
            /* Copy application data */
            (void)ADI_MEMCPY(&pBlock[1U], &poRemoteOtpData->panAppData[k], n);
            (void)ADI_MEMCPY(&pBlock[1U], &poRemoteOtpData->panAppData[k], n);

            /* Pad remaining bytes with 0x00 */
            if(n < ADI_OTP_PAYLOAD_LEN_BYTES)
            {
                (void)ADI_MEMSET(&pBlock[1U + n], 0x00,(uint32_t)((uint32_t)ADI_OTP_PAYLOAD_LEN_BYTES - (uint32_t)n));
            }

            /* Checksum */
            pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U] = ComputeChecksum(&pBlock[1U], ADI_OTP_PAYLOAD_LEN_BYTES);

            /* SECDED data */
            pBlock[0U] = ComputeSecdedCode(&pBlock[1U], pBlock[ADI_OTP_PAYLOAD_LEN_BYTES + 1U]);

            /* Print the block number */
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", nBlockIdx);

            /* Enable writing to this block */
            poRemoteOtpData->abOtpBlockWrEn[nBlockIdx] = true;
            poRemoteOtpData->nNumWriteBlocks++;

            poRemoteOtpData->nLastOtpBlockWritten = nBlockIdx;

            /* Point to the next block */
            nRemainingBytes -= n;
            k += n;
            nBlockIdx++;
        }
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");

    return eReturn;
}

/*! @brief Generate raw data for the reg addr-value pair */
ADI_EAL_OTP_CODE
static void otpUtils_AddRegFields(uint8_t *pBuff, const uint32_t nAddr, const uint32_t nData,
                                  const ADI_OTP_DATA_MODE eDataMode)
{
    /* Generate raw data for the reg addr-value pair  */
    uint8_t nAddrLen = 2U;
    uint8_t nDataLen = ((uint8_t)eDataMode + 1U) * 2U;
    /* Temp variables to store addr and data*/
    uint32_t nTmpAddr = nAddr;
    uint32_t nTmpData = nData;
    uint8_t j = 0U;

    /*******************  Copying address to the buffer *******************/
    for(uint8_t i = 0U; i < nAddrLen; i++)
    {
        pBuff[j++] = (uint8_t)(nTmpAddr & 0x0000FFU);
        nTmpAddr = nTmpAddr >> 8U;
    }

    /*******************  Copying data to the buffer *******************/
    for(uint8_t i = 0U; i < nDataLen; i++)
    {
        pBuff[j++] = (uint8_t)(nTmpData & 0x0000FFU);
        nTmpData = nTmpData >> 8U;
    }
}

/*! @brief Programs the remode node OTP */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS programOtpRawData(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
                                        ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    /* Variable to return the status */
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Starting programming sequence\r\n");

    do
    {
        /* Enable OTP macro */
        eReturn = enableOtpMacro(nInstNum, nE2bCfgRemoteNum);

        /* Return if OTP macro is not enabled*/
        if(eReturn != ADI_OTP_STATUS_NO_ERR)
        {
            break;
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Writing %d OTP blocks:", poRemoteOtpData->nNumWriteBlocks);

        /* Write OTP blocks */
        for(uint8_t i = ADI_OTP_NUM_HV_DATA_BLOCKS;
                i <= poRemoteOtpData->nLastOtpBlockWritten; i++)
        {
            if(poRemoteOtpData->abOtpBlockWrEn[i])
            {
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", i);

                /* Exit if any error occurrs during writing */
                if(!writeOtpBlock(nInstNum, nE2bCfgRemoteNum, i, &poRemoteOtpData->anOtpWriteData[i][0u]))
                {
                    eReturn = ADI_OTP_STATUS_PGM_FAIL;
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \r\n\t-OTP write failed. Exiting programming routine!\r\n");
                    break;
                }
            }
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \r\n");

        /* Break if any error occurred during the write */
        if(eReturn != ADI_OTP_STATUS_NO_ERR)
        {
            break;
        }

        /* Disable OTP macro */
        eReturn = disableOtpMacro(nInstNum, nE2bCfgRemoteNum);

        if(eReturn != ADI_OTP_STATUS_NO_ERR)
        {
            break;
        }

    }

    while(false);

    return eReturn;
}

/*! @brief Reads the remode node OTP data */
ADI_EAL_OTP_CODE
static bool readOtpSelectedBlocks(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    bool bSuccess = true;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Reading OTP blocks:");

    for(uint8_t i = ADI_OTP_NUM_HV_DATA_BLOCKS; i <= poRemoteOtpData->nLastOtpBlockWritten; i++)
    {
        /* Read only if the block is written */
        if(poRemoteOtpData->abOtpBlockWrEn[i])
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %d,", i);
            /* Read the block data */
            bSuccess = readOtpBlock(nInstNum, nE2bCfgRemoteNum, poRemoteOtpData, i);

            /* Break if any error occurred during the read */
            if(!bSuccess)
            {
                break;
            }
        }
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\b \n\r");
    return bSuccess;
}

/*! @brief Reads one OTP block */
ADI_EAL_OTP_CODE
static bool readOtpBlock(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, ADI_OTP_REMOTE_DATA *poRemoteOtpData,
                         uint8_t nBlockNum)
{
    bool bSuccess = true;
    /* Temporary data to store the read data */
    uint32_t aTmpData[ADI_OTP_BLOCK_LEN_BYTES / 2u] = {0u};
    static uint16_t anBlockReadAddr[ADI_OTP_BLOCK_LEN_BYTES / 2u] = {0u};
    static uint8_t anBlockReadMMS[ADI_OTP_BLOCK_LEN_BYTES / 2u] = {0u};

    /* Read 8 words for each block */
    uint8_t nReadLen = ADI_OTP_BLOCK_LEN_BYTES / 2u;

    for(uint8_t j = 0U; j < nReadLen; j++)
    {
        /* Fill the read address and MMS */
        anBlockReadAddr[j] = ADI_OTP_RD_CONTENT_BASE_ADDR + ((uint16_t)nBlockNum * 8u) + j;
        anBlockReadMMS[j] = ADI_OTP_RD_CONTENT_MMS;
    }

    /* Read the block data */
    if(readRegs(nInstNum, nE2bCfgRemoteNum, anBlockReadAddr, &aTmpData[0u], nReadLen, anBlockReadMMS))
    {
        /* Fill the read block data from the 8 words */
        for(uint8_t j = 0u; j < nReadLen; j++)
        {
            poRemoteOtpData->anOtpReadData[nBlockNum][2u * j] = (uint8_t)GET_FIELD_VAL(aTmpData[j], 0x00FFU, 0U);
            poRemoteOtpData->anOtpReadData[nBlockNum][2u * j + 1u] = (uint8_t)GET_FIELD_VAL(aTmpData[j], 0xFF00U, 8U);
        }
    }
    else
    {
        bSuccess = false;
    }

    return bSuccess;
}

/*! @brief Compares the read OTP block data against write data */
ADI_EAL_OTP_CODE
static bool compareOtpReadData(ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    bool bSuccess = true;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Comparing read data with write data: ");

    for(uint8_t i = ADI_OTP_NUM_HV_DATA_BLOCKS; i < poRemoteOtpData->nLastOtpBlockWritten; i++)
    {
        /* Compare only if the block is written */
        if(poRemoteOtpData->abOtpBlockWrEn[i])
        {
            /* Compare the read and write data */
            if(ADI_MEMCMP(&poRemoteOtpData->anOtpWriteData[i][0u], &poRemoteOtpData->anOtpReadData[i][0u],
                          ADI_OTP_BLOCK_LEN_BYTES) != 0)
            {
                /* Print the block number where the comparison failed */
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed at block-%d\r\n", i);
                bSuccess = false;
                break;
            }
        }
    }

    /* Print the result */
    if(bSuccess)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Pass\r\n");
    }

    return bSuccess;
}

/*! @brief Enable OTP macro and enable programming voltage */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS enableOtpMacro(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    /* Variable to store number of register writes */
    uint8_t nRegs = 0u;
    ADI_DBG_REQUIRE(IO_HV_OTP_CONFIG_1_ADDR == IO_HV_OTP_CONFIG_1_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WRC_ADDR == OTP_WRC_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_OMC_ADDR == OTP_OMC_NW_ADDR, "Register address and Register NW address doesn't match");

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Enable OTP macro and programming voltage: ");
    ganRegmapAddr[nRegs] = IO_HV_OTP_CONFIG_1_ADDR;
    ganRegmapMMS[nRegs] = IO_HV_OTP_CONFIG_1_MMS;
    ganRegmapData[nRegs++] = 0x01U << BITP_IO_HV_OTP_CONFIG_1_OTP_ACCESS_MUX_CTRL;
    //| 0x00U << BITP_IO_HV_OTP_CONFIG_1_BOOTLOADER_SIMPLIFIED_STATUS;

    /* OTPC Write control register */
    ganRegmapAddr[nRegs] = OTP_WRC_ADDR;
    ganRegmapMMS[nRegs] = OTP_WRC_MMS;
    ganRegmapData[nRegs++] = OTP_WRC_RESET_VAL | (0x01U << BITP_OTP_WRC_VPP_EN_REG);

    /* Places the OTP Controller in program mode */
    ganRegmapAddr[nRegs] = OTP_OMC_ADDR;
    ganRegmapMMS[nRegs] = OTP_OMC_MMS;
    ganRegmapData[nRegs++] = 0x01U << BITP_OTP_OMC_PROG_MODE_REG;

    /* Write all the above regs in a single frame */
    if(!writeRegs(nInstNum, nE2bCfgRemoteNum, ganRegmapAddr, ganRegmapData, nRegs, ganRegmapMMS))
    {
        eReturn = ADI_OTP_STATUS_PGM_FAIL;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
    }
    else
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");
    }

    return eReturn;
}

/*! @brief Disable programming voltage and disable OTP macro */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS disableOtpMacro(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    /* Variable to store number of register writes */
    uint8_t nRegs = 0u;
    ADI_DBG_REQUIRE(IO_HV_OTP_CONFIG_1_ADDR == IO_HV_OTP_CONFIG_1_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WRC_ADDR == OTP_WRC_NW_ADDR, "Register address and Register NW address doesn't match");

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Disabling OTP macro and programming voltage: ");
    /* OTPC Write control register */
    ganRegmapAddr[nRegs] = OTP_WRC_ADDR;
    ganRegmapMMS[nRegs] = OTP_WRC_MMS;
    ganRegmapData[nRegs++] = OTP_WRC_RESET_VAL | (0x00U << BITP_OTP_WRC_VPP_EN_REG);

    /* OTP Access Mux Control */
    ganRegmapAddr[nRegs] = IO_HV_OTP_CONFIG_1_ADDR;
    ganRegmapMMS[nRegs] = IO_HV_OTP_CONFIG_1_MMS;
    ganRegmapData[nRegs++] = 0x00U << BITP_IO_HV_OTP_CONFIG_1_OTP_ACCESS_MUX_CTRL;

    /* Write all the above regs in a single frame */
    if(!writeRegs(nInstNum, nE2bCfgRemoteNum, ganRegmapAddr, ganRegmapData, nRegs, ganRegmapMMS))
    {
        eReturn = ADI_OTP_STATUS_PGM_FAIL;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
    }
    else
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");
    }

    return eReturn;
}

ADI_EAL_OTP_CODE
static bool writeOtpBlock(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, uint32_t nBlockNum, uint8_t *pData)
{
    bool bSuccess = true;
    uint8_t nRegs = 0u;
    ADI_DBG_REQUIRE(OTP_WAL_ADDR == OTP_WAL_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WAH_ADDR == OTP_WAH_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WDL_ADDR == OTP_WDL_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WDH_ADDR == OTP_WDH_NW_ADDR, "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_OMC_ADDR == OTP_OMC_NW_ADDR, "Register address and Register NW address doesn't match");

    /* TODO: Get the block base address: OTP memory is word addressable and each word takes 4 locations */
    uint16_t nBlockBaseAddr = ADI_OTP_MACRO_BASE_ADDR + (uint16_t)(nBlockNum * ADI_OTP_BLOCK_LEN_BYTES / 2u * 4u);
    uint16_t nAddr = 0U;

    for(uint16_t i = 0u; i < ADI_OTP_BLOCK_LEN_BYTES; i += 2u)
    {
        nRegs = 0u;

        /***********************  Address location ***********************/
        nAddr = nBlockBaseAddr + 4u * i / 2u;
        ganRegmapAddr[nRegs] = OTP_WAL_ADDR;
        ganRegmapMMS[nRegs] = OTP_WAL_MMS;
        ganRegmapData[nRegs++] = (uint8_t)GET_FIELD_VAL(nAddr, 0x00FFU, 0U);
        ganRegmapAddr[nRegs] = OTP_WAH_ADDR;
        ganRegmapMMS[nRegs] = OTP_WAH_MMS;
        ganRegmapData[nRegs++] = (uint8_t)GET_FIELD_VAL(nAddr, 0xFF00U, 8U);

        /***************************** Data *****************************/
        ganRegmapAddr[nRegs] = OTP_WDL_ADDR;
        ganRegmapMMS[nRegs] = OTP_WDL_MMS;
        ganRegmapData[nRegs++] = pData[i];
        ganRegmapAddr[nRegs] = OTP_WDH_ADDR;
        ganRegmapMMS[nRegs] = OTP_WDH_MMS;
        ganRegmapData[nRegs++] = pData[i + 1u];

        /*********************** OTP write command ***********************/
        ganRegmapAddr[nRegs] = OTP_OMC_ADDR;
        ganRegmapMMS[nRegs] = OTP_OMC_MMS;
        ganRegmapData[nRegs++] = 0x03U;

        /* Write all the above regs in a single frame */
        if(!writeRegs(nInstNum, nE2bCfgRemoteNum, ganRegmapAddr, ganRegmapData, nRegs, ganRegmapMMS))
        {
            /* If write itself fails, then fatal error; break with failure */
            bSuccess = false;
            break;
        }

        /* Check for the prog completed status */
        bSuccess = chkOtpProgStatus(nInstNum, nE2bCfgRemoteNum);

        /* BreaK from the loop if the check for prog completed status has failed */
        if(!bSuccess)
        {
            break;
        }
    }

    return bSuccess;
}

ADI_EAL_OTP_CODE
static bool chkOtpProgStatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum)
{
    bool bSuccess = true;
    uint32_t nWriteSuccessPstMask = BITM_OTP_PST_PROG_OVER | BITM_OTP_PST_CMD_DONE;
    bool bWait = true;
    uint64_t nCurrTimeNs, nStartTimeNs;
    static uint32_t nOtpStatus;
    ADI_DBG_REQUIRE(OTP_PST_ADDR == OTP_PST_NW_ADDR, "Register address and Register NW address doesn't match");

    ganRegmapAddr[0u] = OTP_PST_ADDR;
    ganRegmapMMS[0u] = OTP_PST_MMS;

    adi_pal_getCurrTime(&nStartTimeNs);

    while(bWait)
    {
        adi_network_run();
        adi_pal_getCurrTime(&nCurrTimeNs);
        /* Delay between each status poll */
        adi_pal_waitMicroSec(ADI_OTP_REG_PST_POLL_DELAY_MS * MS_TO_US);

        adi_pal_getCurrTime(&nCurrTimeNs);

        /* Check if read is not finished within timeout period */
        if(nCurrTimeNs - nStartTimeNs > ADI_OTP_REG_PGM_DONE_TIMEOUT_MS * MS_TO_NS)
        {
            bSuccess = false;
            bWait = false;
        }
        else
        {
            /* Read the OTP PST register */
            if(readRegs(nInstNum, nE2bCfgRemoteNum, &ganRegmapAddr[0u], &nOtpStatus, 1u, &ganRegmapMMS[0u]))
            {
                /* Check if prog_over & cmd_done are set and fatal_error is not set */
                if((nOtpStatus & BITM_OTP_PST_FATAL_ERROR) == BITM_OTP_PST_FATAL_ERROR)
                {
                    /* fatal error */
                    bSuccess = false;
                    bWait = false;
                }
                else if((nOtpStatus & nWriteSuccessPstMask) == nWriteSuccessPstMask)
                {
                    /* OTP write successful */
                    bWait = false;
                }
                else
                {
                    /* Nothing to do */
                }
            }
            else
            {
                /* Read itself failed, so fatal error; break with failure */
                bSuccess = false;
                bWait = false;
            }
        }
    }

    return bSuccess;
}

static bool readRegs(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
    uint16_t *panRegAddr, uint32_t *pVal, uint8_t nLen, uint8_t *panMMS)
{
    bool bSuccess = true;

    /* If the OTP is programmed over OASPI - control transactions are required to read the registers */
    if(goOtpMgr.poOtpCfg->bPgmOverOaspi == true)
    {
        uint64_t nCurrTime, nStartTime;
        bool bTimedOut = false;

        /* Loop through the number of registers and try to read them one by one */
        for(uint8_t nLC = 0; nLC < nLen; nLC++)
        {
            goOtpMgr.oRegIo.bReadRespReceived = false;

            /* Do a local read of the registers */
            if(adi_network_readControllerRegister(goOtpMgr.nDevNum, panRegAddr[nLC], 1U, panMMS[nLC], &adi_otp_network_cbk) != ADI_NETWORK_OK)
            {
                bSuccess = false;
                break;
            }

            adi_pal_getCurrTime(&nCurrTime);
            nStartTime = nCurrTime;

            /* Wait for the reg read response */
            while(true)
            {
                adi_network_run();
                /* Get the current and compare with the start time to see if the response has timed out */
                adi_pal_getCurrTime(&nCurrTime);

                if(nCurrTime - nStartTime > ADI_OTP_REG_READ_RESP_TIMEOUT_MS * MS_TO_NS)
                {
                    bTimedOut = true;
                }

                if(bTimedOut || goOtpMgr.oRegIo.bReadRespReceived)
                {
                    break;
                }
            }

            /* If the response has timed out, break from the loop and return the error */
            if(bTimedOut)
            {
                bSuccess = false;
                break;
            }

            ADI_DBG_REQUIRE(goOtpMgr.oRegIo.anAddr[0u] == panRegAddr[nLC], "Reg read start address mismatch");
            pVal[nLC] = goOtpMgr.oRegIo.anData[0u];
        }
    }
    else
    {
       /*Regmap config with the given register address to read, and the number of registers to be read - used if the OTP is programmed over the network */
        goRegCfg.anMask = NULL;
        goRegCfg.anStartAddr = panRegAddr;
        goRegCfg.eType = ADI_E2BCORE_RND_READ;
        goRegCfg.nLength = nLen;
        goRegCfg.pBuffer = pVal;

        /* If the OTP is programmed over the network, do a register read command using EAL */
        if(adi_eal_readRegWithTimeout(nInstNum, nE2bCfgRemoteNum, &goRegCfg,
            ADI_OTP_REG_READ_RESP_TIMEOUT_MS * MS_TO_NS) != ADI_EAL_STATUS_SUCCESS)
        {
            bSuccess = false;
        }
    }
    return bSuccess;
}

static bool writeRegs(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, uint16_t *panRegmapAddr,
                      uint32_t *panRegmapData,
                      uint8_t nNumRegs, uint8_t *panMMS)
{
    bool bSuccess = true;

    /* Get the device number mapped to this instance */
    uint32_t nDevNum = gaoEalData[nInstNum].nDevNum;

    /* If the OTP programming is over OASPI, then local writes are performed using OASPI control transactions */
    if(goOtpMgr.poOtpCfg->bPgmOverOaspi == true)
    {
        /* Loop through the registers to be written one by one */
        for(uint8_t nLC = 0; nLC < nNumRegs; nLC++)
        {
            /* Perform a local write */
            if(adi_network_writeControllerRegister(goOtpMgr.nDevNum, panRegmapAddr[nLC], 1U, panMMS[nLC], &panRegmapData[nLC],
                                                   &adi_otp_network_cbk) != ADI_NETWORK_OK)
            {
                bSuccess = false;
                break;
            }
        }
    }
    else
    {
        /* Create a regmap config with the given register address to write, and the number of registers to be written along with register data
        - used if the OTP is programmed over the network */
        ADI_E2BCORE_REGMAP_IO_CONFIG oRegCfg;
        oRegCfg.anStartAddr = panRegmapAddr;
        oRegCfg.nLength = nNumRegs;
        oRegCfg.pBuffer = panRegmapData;
        oRegCfg.anMask = NULL;
        oRegCfg.eType = ADI_E2BCORE_RND_WRITE;

        /* If the OTP programming is over the network, then perform a register write command using EAL */
        if(adi_eal_sendRegTransaction(nInstNum, nE2bCfgRemoteNum, &oRegCfg) != ADI_EAL_STATUS_SUCCESS)
        {
            bSuccess = false;
        }
    }

    if(bSuccess == true)
    {
        adi_network_flushTxBuffer(nDevNum, 0U);
    }

    return bSuccess;
}

/*! @brief EAL notification to OTP controller with the reg read response */
ADI_EAL_OTP_CODE
void eal_notify_OtpRegReadResp(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum, ADI_E2BCORE_CBK_REGREADDAT *poRegDat)
{
    /* EAL invokes this function to notify the module with the register read response
       if gaoEalData[nInstNum].aoEalNodeData[nRemoteNum].bNotifyOtp is set*/
    uint8_t nLength;

    /* REGMAP_IO type */
    goOtpMgr.oRegIo.eType = poRegDat->poInfo->eType;

    nLength = poRegDat->poInfo->nLength;

    goOtpMgr.oRegIo.anAddr[0U] = poRegDat->poInfo->anStartAddr[0U];

    (void)ADI_MEMCPY((void *)goOtpMgr.oRegIo.anData, (void *)poRegDat->poInfo->pBuffer, (size_t)nLength * (size_t)4u);
    goOtpMgr.oRegIo.nNumData = nLength;
    goOtpMgr.oRegIo.bReadRespReceived = true;
}

/*! @brief Calculate checksum */
ADI_EAL_OTP_CODE
static uint8_t ComputeChecksum(uint8_t *pBuff, uint32_t nLen)
{
    /* variable to store the checksum value */
    uint8_t nVal = 0U;
    uint32_t i = 0u;

    /* loop through the length and find the checksum value */
    for(i = 0u; i < nLen; i++)
    {
        nVal = nVal + pBuff[i];
    }

    return nVal;
}

ADI_EAL_OTP_CODE
static uint8_t Exor(uint8_t nNum)
{
    /* Perform XOR operation between bits of the given 8 bit number and return the computed result */
    uint8_t nRes =
        ((nNum & 0x01U) >> 0U) ^
        ((nNum & 0x02U) >> 1U) ^
        ((nNum & 0x04U) >> 2U) ^
        ((nNum & 0x08U) >> 3U) ^
        ((nNum & 0x10U) >> 4U) ^
        ((nNum & 0x20U) >> 5U) ^
        ((nNum & 0x40U) >> 6U) ^
        ((nNum & 0x80U) >> 7U);

    return nRes;
}

/*! @brief Compute SECDED code for 15-bytes (14-byte data and one byte checksum) */
ADI_EAL_OTP_CODE
static uint8_t ComputeSecdedCode(uint8_t *pBuff, uint8_t nChecksum)
{
    /* Temp variable to store data */
    uint8_t anData[15u];

    (void)ADI_MEMCPY(anData, pBuff, 14U);
    anData[14u] = nChecksum;

    /* Compute secded code for 14 bytes of data and one byte of checksum */
    uint8_t nHamming = 0U;

    for(uint8_t i = 0U; i < 7U; i++)
    {
        uint8_t nRes = 0U;

        for(uint8_t j = 0U; j < 15U; j++)
        {
            nRes ^= Exor(anHammingCodes[i][j] & anData[j]);
        }

        nHamming |= (nRes << i);
    }

    /* Compute the parity */
    uint8_t nParity = 0U;
    nParity ^= Exor(nHamming);

    for(uint8_t i = 0U; i < 15U; i++)
    {
        nParity ^= Exor(anData[i]);
    }

    nHamming |= (nParity << 7u);

    return nHamming; /* Return the computed secded code */
}

/*! @brief Performs a bootloader soft reset */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS bootloaderSoftReset(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    /* Variable to store number of register writes */
    uint8_t nRegs = 0u;
    ADI_DBG_REQUIRE(IO_HV_BLOCK_RESET_CONTROL_ADDR == IO_HV_BLOCK_RESET_CONTROL_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BLOCK_RESET_CONTROL_ADDR == IO_BLOCK_RESET_CONTROL_NW_ADDR,
                    "Register address and Register NW address doesn't match");

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Bootloader soft reset: ");
    /* Register writes for resetting the bootloader */
    ganRegmapAddr[nRegs] = IO_HV_BLOCK_RESET_CONTROL_ADDR;
    ganRegmapMMS[nRegs] = IO_HV_BLOCK_RESET_CONTROL_MMS;
    ganRegmapData[nRegs++] = 0x01U << BITP_IO_HV_BLOCK_RESET_CONTROL_BOOT_LOADER_SOFT_RESET;

    ganRegmapAddr[nRegs] = IO_BLOCK_RESET_CONTROL_ADDR;
    ganRegmapMMS[nRegs] = IO_BLOCK_RESET_CONTROL_MMS;
    ganRegmapData[nRegs++] = 0x01U << BITP_IO_BLOCK_RESET_CONTROL_BOOT_LOADER_SOFT_RESET;

    /* Write all the above regs in a single frame */
    if(!writeRegs(nInstNum, nE2bCfgRemoteNum, ganRegmapAddr, ganRegmapData, nRegs, ganRegmapMMS))
    {
        /* If write fails return the status */
        eReturn = ADI_OTP_STATUS_PGM_FAIL;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Fail\n\r");
    }
    else
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\n\r");
    }

    return eReturn;
}

/*! @brief Polls bootloader status and exits in case of time out */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS pollBootStatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
                                     uint32_t *anOtpStatus, uint8_t nRegs)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    bool bWait = true;
    uint64_t nCurrTimeNs, nStartTimeNs;

    adi_pal_getCurrTime(&nStartTimeNs);

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Reading bootloader done status: ");

    while(bWait)
    {
        /* Delay between each boot_done_status poll */
        adi_pal_waitMicroSec(3U * MS_TO_US);

        adi_pal_getCurrTime(&nCurrTimeNs);

        /* Check if Bootloader did not finish within timeout period */
        if(nCurrTimeNs - nStartTimeNs > ADI_OTP_BOOTLOADER_DONE_TIMEOUT_MS * MS_TO_NS)
        {
            eReturn = ADI_OTP_STATUS_BOOT_TIMEOUT;
            bWait = false;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Timeout\r\n");
        }
        else
        {
            /* Read the boot_loader done status for both HV & LV dies */
            if(readRegs(nInstNum, nE2bCfgRemoteNum, ganRegmapAddr, &anOtpStatus[0u], nRegs, ganRegmapMMS))
            {
                /* First check if bootload_timeout is set */
                if((anOtpStatus[1u] & BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT) ==
                        BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_TIMEOUT)
                {
                    bWait = false;
                    eReturn = ADI_OTP_STATUS_BOOT_TIMEOUT;
                }
                /* Check if boot_loader done status is set for both HV and LV die */
                else if(((anOtpStatus[1u] & BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE) ==
                         BITM_IO_HV_BOOTLOADER_STATUS_1_BOOTLOAD_DONE) &&
                        ((anOtpStatus[9u] & BITM_IO_BOOTLOADER_STATUS_7_BOOTLOAD_DONE) == BITM_IO_BOOTLOADER_STATUS_7_BOOTLOAD_DONE))
                {
                    /* Both boot_loader_done set - Success */
                    bWait = false;
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\r\n");
                }
                else
                {
                    /* Keep polling */
                }
            }
            else
            {
                /* Read itself failed, so fatal error; break with failure */
                eReturn = ADI_OTP_STATUS_READ_FAIL;
                bWait = false;
                ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Read failed\r\n");
            }
        }
    }

    return eReturn;
}

/*! @brief Reads the OTP boot status */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS readBootStatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,
                                     ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    uint32_t anOtpStatus[10U] = {0u};
    uint8_t nRegs = 0u;
    /* Pointer to store the block boot status */
    ADI_OTP_BLOCK_STATUS *paeBlockSt = &poRemoteOtpData->aeBlockBootStatus[0u];
    ADI_DBG_REQUIRE(IO_HV_BOOTLOADER_STATUS_0_ADDR == IO_HV_BOOTLOADER_STATUS_0_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_HV_BOOTLOADER_STATUS_1_ADDR == IO_HV_BOOTLOADER_STATUS_1_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_0_ADDR == IO_BOOTLOADER_STATUS_0_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_1_ADDR == IO_BOOTLOADER_STATUS_1_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_2_ADDR == IO_BOOTLOADER_STATUS_2_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_3_ADDR == IO_BOOTLOADER_STATUS_3_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_4_ADDR == IO_BOOTLOADER_STATUS_4_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_5_ADDR == IO_BOOTLOADER_STATUS_5_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_6_ADDR == IO_BOOTLOADER_STATUS_6_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(IO_BOOTLOADER_STATUS_7_ADDR == IO_BOOTLOADER_STATUS_7_NW_ADDR,
                    "Register address and Register NW address doesn't match");

    /* HV die boot_loader status regs */
    ganRegmapMMS[nRegs] = IO_HV_BOOTLOADER_STATUS_0_MMS;
    ganRegmapAddr[nRegs++] = IO_HV_BOOTLOADER_STATUS_0_ADDR;
    ganRegmapMMS[nRegs] = IO_HV_BOOTLOADER_STATUS_1_MMS;
    ganRegmapAddr[nRegs++] = IO_HV_BOOTLOADER_STATUS_1_ADDR;

    /* LV die boot_loader status regs */
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_0_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_0_ADDR;
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_1_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_1_ADDR;
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_2_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_2_ADDR;
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_3_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_3_ADDR;
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_4_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_4_ADDR;
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_5_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_5_ADDR;
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_6_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_6_ADDR;
    ganRegmapMMS[nRegs] = IO_BOOTLOADER_STATUS_7_MMS;
    ganRegmapAddr[nRegs++] = IO_BOOTLOADER_STATUS_7_ADDR;

    /* Poll the bootloader status */
    eReturn = pollBootStatus(nInstNum, nE2bCfgRemoteNum, &anOtpStatus[0u], nRegs);

    /* If boot_loader done status is set before timeout, get the boot status for each block */
    if(eReturn == ADI_OTP_STATUS_NO_ERR)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Reading block boot status\r\n");

        /***** Get boot status of HV data blocks ******/
        /* Block-0 is not managed by boot controller so set the status as always valid */
        paeBlockSt[0u] = ADI_OTP_BLOCK_STATUS_VALID;

        uint32_t nTemp = (anOtpStatus[0u] >> 0u) & 0x00000003u;
        paeBlockSt[1u] = (ADI_OTP_BLOCK_STATUS)nTemp;
        nTemp = (anOtpStatus[0u] >> 2u) & 0x00000003u;
        paeBlockSt[2u] = (ADI_OTP_BLOCK_STATUS)nTemp;

        /***** Get boot status of LV data blocks, which starts from Block-3 (4th block) ******/
        uint8_t nBlock = 3u;

        /* From IO_BOOTLOADER_STATUS_0_ADDR to IO_BOOTLOADER_STATUS_6_ADDR : Each reg has status for 8 blocks */
        for(uint8_t i = 2u; i <= 8u; i++)
        {
            for(uint8_t j = 0u; j < 8u; j++)
            {
                nTemp = (anOtpStatus[i] >> (2u * j)) & 0x00000003u;
                paeBlockSt[nBlock++] = (ADI_OTP_BLOCK_STATUS)nTemp;
            }
        }

        /* IO_BOOTLOADER_STATUS_7_ADDR has status for last 5 blocks */
        for(uint8_t j = 0u; j < 5u; j++)
        {
            nTemp = (anOtpStatus[9u] >> (2u * j)) & 0x00000003u;
            paeBlockSt[nBlock++] = (ADI_OTP_BLOCK_STATUS)nTemp;
        }
    }

    return eReturn;
}

/*! @brief Verify the OTP lock status */
static bool verifylockstatus(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum)
{
    bool bSuccess = true;
    uint32_t lockStatus[2U] = {0u};
    uint8_t nRegs = 0u;
    ADI_DBG_REQUIRE(OTP_WDL_ADDR == OTP_WDL_NW_ADDR,
                    "Register address and Register NW address doesn't match");
    ADI_DBG_REQUIRE(OTP_WDH_ADDR == OTP_WDH_NW_ADDR,
                    "Register address and Register NW address doesn't match");

    ganRegmapAddr[nRegs] = OTP_WDL_ADDR;
    ganRegmapMMS[nRegs++] = OTP_WDL_MMS;
    ganRegmapAddr[nRegs] = OTP_WDH_ADDR;
    ganRegmapMMS[nRegs++] = OTP_WDH_MMS;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Verifying lock status : ");

    /* Read the boot_loader done status for both HV & LV dies */
    if(readRegs(nInstNum, nE2bCfgRemoteNum, ganRegmapAddr, &lockStatus[0u], nRegs, ganRegmapMMS))
    {
        /* check the value is 0xAD and 0xDA to verify the OTP lock status */
        if(lockStatus[0u] == 0xADu && lockStatus[1u] == 0xDAu)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Verified\r\n");
        }
        else
        {
            bSuccess = false;
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
        }
    }
    else
    {
        /* If read itself has failed, mark the operation as failure and return bSuccess as false */
        bSuccess = false;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\r\n");
    }

    return bSuccess;
}

/*! @brief Sets the read mde for otp_rd_content register map */
ADI_EAL_OTP_CODE
static ADI_OTP_STATUS setReadMode(uint32_t nInstNum, uint32_t nE2bCfgRemoteNum,uint8_t nOtpReadType)
{
    ADI_OTP_STATUS eReturn = ADI_OTP_STATUS_NO_ERR;
    /* Variable to store number of register writes */
    uint8_t nRegs = 0u;
    ADI_DBG_REQUIRE(IO_HV_OTP_CONFIG_0_ADDR == IO_HV_OTP_CONFIG_0_NW_ADDR,
                    "Register address and Register NW address doesn't match");

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Setting OTP read mode: ");
    /* Register writes for resetting the bootloader */
    ganRegmapAddr[nRegs] = IO_HV_OTP_CONFIG_0_ADDR;
    ganRegmapMMS[nRegs] = IO_HV_OTP_CONFIG_0_MMS;
    ganRegmapData[nRegs++] = nOtpReadType;

    if(!writeRegs(nInstNum, nE2bCfgRemoteNum, ganRegmapAddr, ganRegmapData, nRegs, ganRegmapMMS))
    {
        /* If write fails return the status */
        eReturn = ADI_OTP_STATUS_PGM_FAIL;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Failed\n\r");
    }
    else
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Done\n\r");
    }

    return eReturn;
}

/*! @brief Debug function for printing OTP data */
ADI_EAL_OTP_CODE
static void printOtpData(ADI_OTP_REMOTE_DATA *poRemoteOtpData, uint8_t nDataType)
{
    /* Print the OTP data for either the generated raw OTP data / to read and verify the OTP data */
    if(nDataType == WRITE_DATA)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Print OTP write data (%d write blocks):\r\n",
                    poRemoteOtpData->nNumWriteBlocks);
    }
    else
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Print OTP read data:\r\n");
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\tBlock-0,1,2: ADI data\r\n");

    /* Loop through the OTP blocks and print the data */
    for(uint8_t i = ADI_OTP_NUM_HV_DATA_BLOCKS; i <= poRemoteOtpData->nLastOtpBlockWritten; i++)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\tBlock-%-2d: ", i);

        if(poRemoteOtpData->abOtpBlockWrEn[i])
        {
            for(uint8_t j = 0u; j < ADI_OTP_BLOCK_LEN_BYTES; j++)
            {
                if(nDataType == WRITE_DATA)
                {
                    /* WRITE_DATA */
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "%02X  ", poRemoteOtpData->anOtpWriteData[i][j]);
                }
                else
                {
                    /* READ_DATA */
                    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "%02X  ", poRemoteOtpData->anOtpReadData[i][j]);
                }
            }
        }
        else
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "- - -");
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    }
}

/*! @brief Debug function for printing OTP data */
ADI_EAL_OTP_CODE
static void printOtpBlocksRange(ADI_OTP_REMOTE_DATA *poRemoteOtpData, uint8_t nStartBlock,
                                uint8_t nBlockCount)
{
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Print OTP read data:\r\n");

    /* Loop thorugh the OTP blocks given as parameters */
    for(uint8_t i = nStartBlock; i < nStartBlock + nBlockCount; i++)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t\tBlock-%-2d: ", i);

        /* Log the block data byte by byte */
        for(uint8_t j = 0u; j < ADI_OTP_BLOCK_LEN_BYTES; j++)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "%02X  ", poRemoteOtpData->anOtpReadData[i][j]);
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
    }
}

/*! @brief Print the boot status of all the blocks */
ADI_EAL_OTP_CODE
static void printBlockStatus(ADI_OTP_REMOTE_DATA *poRemoteOtpData)
{
    ADI_OTP_BLOCK_STATUS *paeBlockSt = &poRemoteOtpData->aeBlockBootStatus[0u];
    /* Print all the block status */
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\t-Print OTP block boot status: \n\r\t\t");

    /* Loop through all the bootloader blocks and print the boot status for each one of them */
    for(uint8_t j = 0u; j < ADI_OTP_MAX_NUM_BLOCKS; j++)
    {
        /* Log the boot status */
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " %2d:%d  |", j, paeBlockSt[j]);

        if((j + 1u) % 8u == 0u)
        {
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\n\r\t\t");
        }
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r");
}

/** EAL Top level callback */
ADI_EAL_OTP_CODE
static void EalTopCallbackFunc(uint32_t nInstNum, ADI_EAL_CBK_EVT eCbkEventType, const void *pArg)
{
    /* Implements top level callback for OTP driver */
    switch(eCbkEventType)
    {
        case ADI_EAL_CBK_EVT_MAC_ADDR_RCVD:
        {
            /* Should not come here as we are not doing full MAC discovery in the OTP driver */
            ADI_DBG_ERROR();
            break;
        }

        default:
        {
            ADI_DBG_ERROR();
            break;
        }
    }
}

/** EAL Node level callback */
ADI_EAL_OTP_CODE
static void EalRemoteNodeCbk(uint32_t nInstNum, uint32_t nRemoteNum,
                             ADI_EAL_NODE_CBK_EVT eCbkEventType, const void *pArg)
{
    /* Implements the node level callback for OTP driver - handling events such as ping responses and reg read responses */
    switch(eCbkEventType)
    {
        case ADI_EAL_NODE_CBK_EVT_PING_RCVD:
        {
            goOtpMgr.bRemotePingRespRcvd = true; /* Set the ping response recevied status as true */
            break;
        }

        case ADI_EAL_NODE_CBK_EVT_REG_READ_RESP:
        {
            break;
        }

        default:
        {
            ADI_DBG_ERROR();
            break;
        }
    }
}

/** ADC interface callback */
ADI_EAL_OTP_CODE
static void EalAdcIntfCbk(uint32_t nInstNum, ADI_EAL_ADC_HANDLE hAdcHandle, ADI_EAL_ADC_CBK_EVT eCbkEventType,
                          const void *pArg)
{
    switch(eCbkEventType)
    {
        case ADI_EAL_ADC_CBK_EVT_ADC_WRITE_RESP:
        {
            // const ADI_EAL_ADC_CBK_EVT_ADC_WRITE_RESP_DATA *poCfg = (const ADI_EAL_ADC_CBK_EVT_ADC_WRITE_RESP_DATA *)pArg;
            // ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\nEVT_WRITE_RESP (E2bId: %d)\r\n", poCfg->oTxnHdr.nE2bId);
            break;
        }

        case ADI_EAL_ADC_CBK_EVT_ADC_SAMPLE:
        {
            const ADI_EAL_ADC_CBK_EVT_ADC_SAMPLE_DATA *poCfg = (const ADI_EAL_ADC_CBK_EVT_ADC_SAMPLE_DATA *)pArg;
            ADI_E2BCORE_ADC_TXN_RX_PAYLOAD *poAdcPayload = (ADI_E2BCORE_ADC_TXN_RX_PAYLOAD *)poCfg->pPayload;
            ADI_DBG_REQUIRE(poAdcPayload->nChIdx == VMON_CH_IDX || poAdcPayload->nChIdx == TEMPERATURE_CH_IDX,
            "Received ADC sample from unexpected channel index");

            // uint64_t nCurrTime;
            // adi_pal_getCurrTime(&nCurrTime);
            // ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "(" UINT64_FORMATTER ") EVT_SAMPLE: %d, %d\r\n", nCurrTime, poAdcPayload->nChIdx,
            //             poAdcPayload->nAdcData);

            if(goOtpMgr.nAdcSampleRcvdCount < NUM_AVDD_SAMPLES)
            {
                goOtpMgr.aAdcSample[goOtpMgr.nAdcSampleRcvdCount++] = poAdcPayload->nAdcData;
            }

            break;
        }

        default:
        {
            ADI_DBG_ERROR();
            break;
        }
    }
}

/* Callback for controller write/read registers */
static void adi_otp_network_cbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void *pData)
{
    ADI_DBG_ENSURE(eEvent == ADI_NETWORK_EVT_READ_REG || eEvent == ADI_NETWORK_EVT_WRITE_REG, "Network Error");

    if(eEvent == ADI_NETWORK_EVT_READ_REG)
    {
        /* ------------------------------------------------------------ */
        uint8_t nLength;
        ADI_NETWORK_EVT_READ_REG_DATA *poRegDat = (ADI_NETWORK_EVT_READ_REG_DATA *)pData;

        nLength = poRegDat->nLength;

        goOtpMgr.oRegIo.anAddr[0U] = poRegDat->nStartAddr;
        (void)ADI_MEMCPY((void *)goOtpMgr.oRegIo.anData, (void *)poRegDat->pRegData, ((size_t)nLength *  sizeof(uint32_t)));
        goOtpMgr.oRegIo.bReadRespReceived = true;
    }
}

/**
 * @brief Calculates the syndrome of a 16-byte OTP memory block using the
 * provided ROM lookup table.
 *
 * The syndrome is used for SECDED error detection and correction. This function
 * processes all bits in the block, skipping the general parity bit, and
 * accumulates the syndrome using the provided ROM table.
 *
 * @param pnBlock Pointer to the 16-byte OTP memory block.
 * @param anRom   ROM lookup table for syndrome calculation (127x8).
 * @return      7-bit syndrome as a byte (bits 0-6 valid).
 */
ADI_EAL_OTP_CODE
static uint8_t calculateDecodeSyndrome(const uint8_t *pnBlock, const uint8_t anRom[127u][8u])
{
    /* Syndrome calculation uses LFSR (Linear Feedback Shift Register) algorithm */
    /* to detect and locate single-bit errors in BCH(127,120) SECDED code */
    uint8_t anSyndrome[7u] = {0u};
    uint8_t anXorWord[7u] = {1u, 0u, 0u, 0u, 0u, 0u, 0u};  /* LFSR state */
    uint8_t anNewXorWord[7u] = {0u};
    uint8_t nSyndrome_byte = ADI_VAL_ZERO;
    uint8_t nBit_index = ADI_VAL_ZERO;
    uint8_t nBit = ADI_VAL_ZERO;

    /* Process all 127 data bits (excluding first bit which is general parity) */
    for(uint8_t j = 0u; j < 16u; ++j)
    {
        for(uint8_t b = 0u; b < 8u; ++b)
        {
            /* Skip bit 0 - reserved for general parity in SECDED scheme */
            if(j == 0u && b == 0u)
            {
                nBit_index++;
                continue;
            }

            nBit = (pnBlock[j] >> (7u - b)) & 1u;

            /* Accumulate syndrome by XORing LFSR state when bit is set */
            if(nBit != 0u)
            {
                for(uint8_t s = 0u; s < 7u; ++s)
                {
                    anSyndrome[s] ^= anXorWord[s];
                }
            }

            /* Advance LFSR: rotate left and apply polynomial feedback from ROM */
            for(uint8_t s = 0u; s < 7u; ++s)
            {
                anNewXorWord[s] = anXorWord[(s + 6u) % 7u];
            }

            /* Apply generator polynomial feedback from lookup table */
            anNewXorWord[3u] = ganRom[nBit_index][0u];

            for(uint8_t s = 0u; s < 7u; ++s)
            {
                anXorWord[s] = anNewXorWord[s];
            }

            nBit_index++;
        }
    }

    /* Pack 7-bit syndrome array into byte for return */
    for(uint8_t s = 0u; s < 7u; ++s)
    {
        nSyndrome_byte |= (anSyndrome[s] & 1u) << s;
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[SYNDBG] Final syndrome byte: 0x%02X\n", nSyndrome_byte);

    return nSyndrome_byte;
}

/**
 * @brief Checks the general parity bit of a memory block.
 *
 * Returns 0 if the number of ones in the block is even, 1 if odd.
 * Used to distinguish between single and double bit errors in SECDED.
 *
 * @param pnBlock Pointer to the memory block.
 * @param nBlockLen   Length of the block in bytes (should be 16 for OTP blocks).
 * @return      0 if even parity, 1 if odd parity.
 */
ADI_EAL_OTP_CODE
static uint8_t checkGeneralParityBit(const uint8_t *pnBlock, uint8_t nBlockLen)
{
    /* Count total 1-bits across entire block for parity calculation */
    /* In SECDED: even parity = no error or double-bit error, odd = single-bit error */
    uint8_t nNumOnes = ADI_VAL_ZERO;
    uint8_t nIndex = ADI_VAL_ZERO;
    uint8_t nParity = ADI_VAL_ZERO;

    for(nIndex = ADI_VAL_ZERO; nIndex < nBlockLen; nIndex++)
    {
        uint8_t nData = pnBlock[nIndex];

        /* Accumulate bit count using population count algorithm */
        for(uint8_t j = 0u; j < 8u; j++)
        {
            nNumOnes += (nData >> j) & 1u;
        }
    }

    /* Parity is odd/even indicator: 0=even number of bits, 1=odd number */
    nParity = nNumOnes % 2u;

    /* Log parity calculation result */
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "[PARITY DBG] check_general_parity_bit: ones=%d, parity=%d\r\n", nNumOnes, nParity);

    return nParity;
}


#endif /* ADI_E2B_IFACE_OTP_ENABLED */
/*! \endcond */

/** @} */

/**
 * EOF: www.analog.com
 */
