/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: iseled.c
 * \brief: This is the main file for ISELED driver
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "iseled.h"
/*=================================== DATA ==================================*/

/* Copy of the ISELED interface handle for EAL access */
ADI_MEM_DATA_NOCRIT_CACHE
static ADI_EAL_ISELED_HANDLE ghIseledHandle[MAX_NUM_ISELED];

ADI_MEM_DATA_NOCRIT_CACHE
static ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD aoFastModeData[APP_ISELED_TOTAL_COUNT];

ADI_MEM_DATA_NOCRIT_CACHE
static uint8_t ganData[APP_ISELED_TOTAL_COUNT][3u];

/*=================================== CODE ==================================*/
ADI_MEM_CODE_CRIT
void IseledInitDrv(uint8_t nRemoteNum, ADI_EAL_ISELED_HANDLE hIseledHandle)
{
    ghIseledHandle[nRemoteNum] = hIseledHandle;
}

ADI_MEM_CODE_CRIT
ADI_EAL_STATUS IseledResetStrip(uint8_t nRemoteNum)
{
    ADI_EAL_STATUS eEalStatus;
    uint8_t anData[3U] =  {0x00u, 0x00u, 0x00u};

    ADI_EAL_ISELED_MODE eMode = ADI_EAL_ISELED_NORMAL_MODE;

    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD aoFastModeData[] =
    {
        {
            .pData         = anData,
            .nAddress      = 0x0u,
            .nInstruction  = 0x2u,
        }
    };

    ADI_EAL_ISELED_IO_DATA oIseledIOData;
    oIseledIOData.eMode = eMode;
    oIseledIOData.nNumIseledCmds = 1U;
    oIseledIOData.pIseledCmds = aoFastModeData;
    oIseledIOData.nExpectedNumResp = 0U;
    oIseledIOData.apIseledResps = NULL;

    eEalStatus = adi_eal_iseledWrite(ghIseledHandle[nRemoteNum], &oIseledIOData, 0x01u);

    return eEalStatus;
}

ADI_MEM_CODE_CRIT
ADI_EAL_STATUS IseledInitStrip(uint8_t nRemoteNum)
{
    ADI_EAL_STATUS eEalStatus;
    uint8_t anData[3U] =  {0x00u, 0x00u, 0x05u};
    ADI_MEMCPY(ganData[0], anData, 3u);
    ADI_EAL_ISELED_MODE eMode = ADI_EAL_ISELED_NORMAL_MODE;

    aoFastModeData[0].pData        = &ganData[0u][0u];
    aoFastModeData[0].nAddress     = 0x01u;
    aoFastModeData[0].nInstruction = 0x09u;

    for(uint16_t i = 0u; i < APP_ISELED_TOTAL_COUNT; i++)
    {
        aoFastModeData[i].pData = &ganData[i][0u];
    }

    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD *apIseledResp[] = {&aoFastModeData[0U]};

    ADI_EAL_ISELED_IO_DATA oIseledIOData;
    oIseledIOData.eMode = eMode;
    oIseledIOData.nNumIseledCmds = 1U;
    oIseledIOData.pIseledCmds = aoFastModeData;
    oIseledIOData.nExpectedNumResp = APP_ISELED_TOTAL_COUNT;
    oIseledIOData.apIseledResps = apIseledResp;

    eEalStatus = adi_eal_iseledWrite(ghIseledHandle[nRemoteNum], &oIseledIOData, 0x02u);
    return eEalStatus;
}

ADI_MEM_CODE_CRIT
ADI_EAL_STATUS IseledReadTemp(uint8_t nRemoteNum)
{
    ADI_EAL_STATUS eEalStatus;
    uint8_t anData[3U] =  {0x00u, 0x00u, 0x00u};

    ADI_MEMCPY(ganData[0], anData, 3u);

    ADI_EAL_ISELED_MODE eMode = ADI_EAL_ISELED_NORMAL_MODE;

    aoFastModeData[0U].pData        = &ganData[0u][0u];
    aoFastModeData[0U].nAddress     = 0x0u;
    aoFastModeData[0U].nInstruction = 0x8u;

    for(uint16_t i = 0u; i < APP_ISELED_TOTAL_COUNT; i++)
    {
        aoFastModeData[i].pData = &ganData[i][0u];
    }

    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD *apIseledResp[] = {&aoFastModeData[0U]};

    ADI_EAL_ISELED_IO_DATA oIseledIOData;
    oIseledIOData.eMode = eMode;
    oIseledIOData.nNumIseledCmds = 1U;
    oIseledIOData.pIseledCmds = aoFastModeData;
    oIseledIOData.nExpectedNumResp = APP_ISELED_TOTAL_COUNT;
    oIseledIOData.apIseledResps = apIseledResp;

    eEalStatus = adi_eal_iseledWrite(ghIseledHandle[nRemoteNum], &oIseledIOData, 0x07u);
    return eEalStatus;
}

ADI_MEM_CODE_CRIT
ADI_EAL_STATUS IseledReadTempOne(uint8_t nRemoteNum, uint8_t nAddress)
{
    ADI_EAL_STATUS eEalStatus;
    uint8_t anData[3U] =  {0x00u, 0x00u, 0x00u};

    ADI_MEMCPY(ganData[0], anData, 3u);

    ADI_EAL_ISELED_MODE eMode = ADI_EAL_ISELED_NORMAL_MODE;

    aoFastModeData[0U].pData        = &ganData[0u][0u];
    aoFastModeData[0U].nAddress     = nAddress;
    aoFastModeData[0U].nInstruction = 0x8u;

    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD *apIseledResp[] = {&aoFastModeData[0U]};

    ADI_EAL_ISELED_IO_DATA oIseledIOData;
    oIseledIOData.eMode = eMode;
    oIseledIOData.nNumIseledCmds = 1U;
    oIseledIOData.pIseledCmds = aoFastModeData;
    oIseledIOData.nExpectedNumResp = 1U;
    oIseledIOData.apIseledResps = apIseledResp;

    eEalStatus = adi_eal_iseledWrite(ghIseledHandle[nRemoteNum], &oIseledIOData, 0x08u);
    return eEalStatus;
}

ADI_MEM_CODE_CRIT
ADI_EAL_STATUS IseledSetRgb(uint8_t nRemoteNum, ADI_EAL_ISELED_MODE eMode,
                            ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD *poFastModeData, uint16_t nNumpackTxn, uint8_t nId, uint16_t nExpectedNumResp)
{
    ADI_EAL_STATUS eEalStatus = ADI_EAL_STATUS_SUCCESS;

    ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD *apIseledResp[] = {poFastModeData};

    ADI_EAL_ISELED_IO_DATA oIseledIOData;
    oIseledIOData.eMode = eMode;
    oIseledIOData.nNumIseledCmds = nNumpackTxn;
    oIseledIOData.pIseledCmds = poFastModeData;
    oIseledIOData.nExpectedNumResp = nExpectedNumResp;
    oIseledIOData.apIseledResps = apIseledResp;

    eEalStatus = adi_eal_iseledWrite(ghIseledHandle[nRemoteNum], &oIseledIOData, nId);
    return eEalStatus;
}

/** @} */

/*
 * EOF: https://www.analog.com/
*/
