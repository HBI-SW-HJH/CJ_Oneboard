#include "ilas_led.h"
#include "iseled.h"

#include <stdarg.h>
#include <stdio.h>

#define ILAS_LED_ACTIVE_COUNT    (84U)
#define ILAS_LED_STORAGE_COUNT   (84U)
#define ILAS_LED_QUEUE_DEPTH     (100U)
#define ILAS_LED_MEM             (ADI_EAL_ISELED_UNICAST_INST_MEM_SIZE(ILAS_LED_QUEUE_DEPTH))
#define ILAS_LED_MEM_WORDS       ((ILAS_LED_MEM + 3U) / 4U)
#define ILAS_LED_INTF_NUM        (1U)
#define ILAS_LED_RUNTIME_FLUSH_TIMEOUT_MS (2ULL)

typedef struct
{
    uint32_t nRemoteNum;
    bool bRemoteValid;
} ILAS_LED_STATE;

static void IlasLedEalCallback(uint32_t nInstNum, ADI_EAL_ISELED_HANDLE hIseledHandle,
    ADI_EAL_ISELED_CBK_EVT eCbkEventType, const void* pArg);
static void IlasLedTraceAppendf(const char* pFmt, ...);
static void IlasLedFlush(uint64_t nTimeoutNs);
static void IlasLedWaitForInitResp(uint32_t nFidIdx, uint64_t nTimeoutMs);
static void IlasLedWaitForProbeResp(uint32_t nFidIdx, uint64_t nTimeoutMs);
static bool IlasLedResolveFidIdx(uint32_t nFidIdx, uint32_t* pnResolvedIdx);

static ILAS_LED_TRACE_LINE_FN gpfTraceLine = NULL;

ADI_MEM_DATA_CRIT_CACHE
static uint32_t ganIlasLedMem[NUM_REMOTE_NODES][ILAS_LED_MEM_WORDS];

ADI_MEM_DATA_CRIT_CACHE
static ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD gaIlasLedRgbPayload[NUM_REMOTE_NODES][ILAS_LED_STORAGE_COUNT];

ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganIlasLedRgbData[NUM_REMOTE_NODES][ILAS_LED_STORAGE_COUNT][3U];

static ADI_EAL_ISELED_HANDLE gahIlasLedHandle[NUM_REMOTE_NODES];
static ILAS_LED_DIAG gaIlasLedDiag[NUM_REMOTE_NODES];
static ILAS_LED_STATE gaIlasLedState[NUM_REMOTE_NODES];
static uint32_t ganIlasFidAlias[NUM_REMOTE_NODES];
static bool gabIlasFidAliasValid[NUM_REMOTE_NODES];
static uint8_t ganIlasLastTempData[NUM_REMOTE_NODES][3U];
static uint16_t ganIlasLastTempValidResp[NUM_REMOTE_NODES];
static uint16_t ganIlasLastTempExpectedResp[NUM_REMOTE_NODES];

void IlasLed_SetTraceLineFn(ILAS_LED_TRACE_LINE_FN pfTraceLine)
{
    gpfTraceLine = pfTraceLine;
}

void IlasLed_ResetAll(void)
{
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        gaIlasLedDiag[i].nOpen = -99;
        gaIlasLedDiag[i].nCallback = -99;
        gaIlasLedDiag[i].nReset = -99;
        gaIlasLedDiag[i].nInit = -99;
        gaIlasLedDiag[i].nInitResp = 0;
        gaIlasLedDiag[i].nProbe = -99;
        gaIlasLedDiag[i].nProbeResp = 0;
        gaIlasLedDiag[i].nSetRgbResp = 0;
        gaIlasLedDiag[i].nSetRgb = -99;
        gaIlasLedDiag[i].nHandleValid = 0;
        gaIlasLedState[i].nRemoteNum = i;
        gaIlasLedState[i].bRemoteValid = false;
        gahIlasLedHandle[i] = NULL;
        ganIlasFidAlias[i] = i;
        gabIlasFidAliasValid[i] = true;
        ganIlasLastTempData[i][0U] = 0U;
        ganIlasLastTempData[i][1U] = 0U;
        ganIlasLastTempData[i][2U] = 0U;
        ganIlasLastTempValidResp[i] = 0U;
        ganIlasLastTempExpectedResp[i] = 0U;
    }
}

void IlasLed_CloseAll(void)
{
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahIlasLedHandle[i] != NULL)
        {
            (void)adi_eal_iseledClose(gahIlasLedHandle[i]);
            gahIlasLedHandle[i] = NULL;
        }
    }
}

bool IlasLed_AnyHandleOpen(void)
{
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahIlasLedHandle[i] != NULL)
        {
            return true;
        }
    }
    return false;
}

bool IlasLed_IsHandleOpen(uint32_t nFidIdx)
{
    uint32_t nResolvedIdx;

    return (IlasLedResolveFidIdx(nFidIdx, &nResolvedIdx) &&
            gahIlasLedHandle[nResolvedIdx] != NULL);
}

bool IlasLed_IsActive(uint32_t nFidIdx)
{
    uint32_t nResolvedIdx;

    return (IlasLedResolveFidIdx(nFidIdx, &nResolvedIdx) &&
            gahIlasLedHandle[nResolvedIdx] != NULL &&
            gaIlasLedDiag[nResolvedIdx].nInit == (int)ADI_EAL_STATUS_SUCCESS &&
            gaIlasLedDiag[nResolvedIdx].nProbe == (int)ADI_EAL_STATUS_SUCCESS &&
            gaIlasLedDiag[nResolvedIdx].nProbeResp > 0);
}

void IlasLed_RemapFidSlots(const uint8_t* pOldFids, const uint8_t* pNewFids, uint8_t nCount)
{
    if (pOldFids == NULL || pNewFids == NULL || nCount == 0U || nCount > NUM_REMOTE_NODES)
    {
        return;
    }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        ganIlasFidAlias[i] = i;
        gabIlasFidAliasValid[i] = false;
    }

    for (uint32_t k = 0U; k < (uint32_t)nCount; k++)
    {
        uint8_t nOldFid = pOldFids[k];
        uint8_t nNewFid = pNewFids[k];
        uint32_t nOldIdx;
        uint32_t nNewIdx;

        if (nOldFid < 0x01U || nOldFid > 0x08U || nNewFid < 0x01U || nNewFid > 0x08U)
        {
            continue;
        }

        nOldIdx = (uint32_t)(nOldFid - 1U);
        nNewIdx = (uint32_t)(nNewFid - 1U);

        ganIlasFidAlias[nNewIdx] = nOldIdx;
        gabIlasFidAliasValid[nNewIdx] = true;
        IlasLedTraceAppendf("ILAS OTP alias FID 0x%02X -> physical FID 0x%02X active=%d handle=%d",
            (unsigned)nNewFid, (unsigned)nOldFid,
            IlasLed_IsActive(nNewIdx) ? 1 : 0,
            gahIlasLedHandle[nOldIdx] != NULL ? 1 : 0);
    }
}

const ILAS_LED_DIAG* IlasLed_GetDiag(uint32_t nFidIdx)
{
    uint32_t nResolvedIdx;

    return IlasLedResolveFidIdx(nFidIdx, &nResolvedIdx) ? &gaIlasLedDiag[nResolvedIdx] : NULL;
}

ADI_EAL_STATUS IlasLed_InitFid(uint32_t nFidIdx, uint32_t nRemoteNum)
{
    ADI_EAL_STATUS eEalStatus;
    ADI_EAL_ISELED_CFG oIseledCfg;
    uint8_t nFid = (uint8_t)(nFidIdx + 1U);

    if (nFidIdx >= NUM_REMOTE_NODES || nRemoteNum >= NUM_REMOTE_NODES)
    {
        return ADI_EAL_STATUS_NULL_PTR;
    }

    (void)ADI_MEMSET(&oIseledCfg, 0, sizeof(oIseledCfg));
    (void)ADI_MEMSET((uint8_t*)&ganIlasLedMem[nFidIdx][0U], 0, ILAS_LED_MEM);

    oIseledCfg.nInstNum = 0U;
    oIseledCfg.nRemoteNum = nRemoteNum;
    oIseledCfg.nIntfNum = ILAS_LED_INTF_NUM;
    oIseledCfg.nIseledCfgQDepth = ILAS_LED_QUEUE_DEPTH;
    oIseledCfg.pEalIseledMemory = (uint8_t*)&ganIlasLedMem[nFidIdx][0U];
    oIseledCfg.nEalIseledMemorySize = ILAS_LED_MEM;

    IlasLedTraceAppendf("FID=0x%02X ISELED open cfg remote=%u intf=%u q=%u mem=%u",
        (unsigned)nFid, (unsigned)oIseledCfg.nRemoteNum, (unsigned)oIseledCfg.nIntfNum,
        (unsigned)oIseledCfg.nIseledCfgQDepth, (unsigned)oIseledCfg.nEalIseledMemorySize);

    eEalStatus = adi_eal_iseledOpen(&oIseledCfg, &gahIlasLedHandle[nFidIdx]);
    gaIlasLedDiag[nFidIdx].nOpen = (int)eEalStatus;
    gaIlasLedDiag[nFidIdx].nHandleValid = (gahIlasLedHandle[nFidIdx] != NULL) ? 1 : 0;
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        IlasLedTraceAppendf("FID=0x%02X ISELED open failed status=%d", (unsigned)nFid, (int)eEalStatus);
        return eEalStatus;
    }

    eEalStatus = adi_eal_iseledRegisterCallback(gahIlasLedHandle[nFidIdx], IlasLedEalCallback);
    gaIlasLedDiag[nFidIdx].nCallback = (int)eEalStatus;
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        IlasLedTraceAppendf("FID=0x%02X ISELED callback failed status=%d", (unsigned)nFid, (int)eEalStatus);
        return eEalStatus;
    }

    gaIlasLedState[nFidIdx].nRemoteNum = nRemoteNum;
    gaIlasLedState[nFidIdx].bRemoteValid = true;
    IseledInitDrv((uint8_t)nRemoteNum, gahIlasLedHandle[nFidIdx]);

    eEalStatus = IseledResetStrip((uint8_t)nRemoteNum);
    gaIlasLedDiag[nFidIdx].nReset = (int)eEalStatus;
    IlasLedTraceAppendf("FID=0x%02X IseledResetStrip status=%d", (unsigned)nFid, (int)eEalStatus);
    IlasLedFlush(100ULL * MS_TO_NS);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        return eEalStatus;
    }

    eEalStatus = IseledInitStrip((uint8_t)nRemoteNum);
    gaIlasLedDiag[nFidIdx].nInit = (int)eEalStatus;
    IlasLedTraceAppendf("FID=0x%02X IseledInitStrip status=%d expected=%u",
        (unsigned)nFid, (int)eEalStatus, (unsigned)APP_ISELED_TOTAL_COUNT);
    IlasLedFlush(100ULL * MS_TO_NS);
    if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        IlasLedWaitForInitResp(nFidIdx, 500ULL);
        if (gaIlasLedDiag[nFidIdx].nInitResp <= 0)
        {
            IlasLedTraceAppendf("FID=0x%02X IseledInitStrip no valid response", (unsigned)nFid);
        }
    }

    gaIlasLedDiag[nFidIdx].nProbeResp = 0;
    eEalStatus = IseledReadTemp((uint8_t)nRemoteNum);
    gaIlasLedDiag[nFidIdx].nProbe = (int)eEalStatus;
    IlasLedTraceAppendf("FID=0x%02X IseledReadTemp probe status=%d expected=%u",
        (unsigned)nFid, (int)eEalStatus, (unsigned)APP_ISELED_TOTAL_COUNT);
    IlasLedFlush(100ULL * MS_TO_NS);
    if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
    {
        IlasLedWaitForProbeResp(nFidIdx, 500ULL);
        if (gaIlasLedDiag[nFidIdx].nProbeResp <= 0)
        {
            IlasLedTraceAppendf("FID=0x%02X IseledReadTemp probe no valid response", (unsigned)nFid);
        }
    }

    return eEalStatus;
}

uint16_t IlasLed_GetActiveCount(void)
{
    return (uint16_t)ILAS_LED_ACTIVE_COUNT;
}
int IlasLed_QueueColorRgbByFid(uint8_t nFid, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    uint32_t nFidIdx = (uint32_t)(nFid - 1U);
    uint32_t nResolvedIdx;
    ADI_EAL_STATUS eEalStatus;

    if (nFid < 0x01U || nFid > 0x08U || nFidIdx >= NUM_REMOTE_NODES ||
        !IlasLedResolveFidIdx(nFidIdx, &nResolvedIdx) ||
        gahIlasLedHandle[nResolvedIdx] == NULL || !gaIlasLedState[nResolvedIdx].bRemoteValid)
    {
        return -1;
    }

    for (uint16_t nLed = 0U; nLed < ILAS_LED_ACTIVE_COUNT; nLed++)
    {
        ganIlasLedRgbData[nResolvedIdx][nLed][0U] = nRed;
        ganIlasLedRgbData[nResolvedIdx][nLed][1U] = nGreen;
        ganIlasLedRgbData[nResolvedIdx][nLed][2U] = nBlue;
        gaIlasLedRgbPayload[nResolvedIdx][nLed].pData = &ganIlasLedRgbData[nResolvedIdx][nLed][0U];
        gaIlasLedRgbPayload[nResolvedIdx][nLed].nAddress = (uint8_t)(nLed + 1U);
        gaIlasLedRgbPayload[nResolvedIdx][nLed].nInstruction = 0x04U;
    }

    eEalStatus = IseledSetRgb((uint8_t)gaIlasLedState[nResolvedIdx].nRemoteNum,
        ADI_EAL_ISELED_NORMAL_MODE,
        &gaIlasLedRgbPayload[nResolvedIdx][0U],
        ILAS_LED_ACTIVE_COUNT,
        0xFFU,
        0x00U);

    gaIlasLedDiag[nResolvedIdx].nSetRgb = (int)eEalStatus;
    IlasLedTraceAppendf("FID=0x%02X IseledSetRgb queued status=%d count=%u RGB=(%u,%u,%u) phys=0x%02X",
        (unsigned)nFid, (int)eEalStatus, (unsigned)ILAS_LED_ACTIVE_COUNT,
        (unsigned)nRed, (unsigned)nGreen, (unsigned)nBlue,
        (unsigned)(nResolvedIdx + 1U));

    return (eEalStatus == ADI_EAL_STATUS_SUCCESS) ? 0 : (int)eEalStatus;
}

void IlasLed_FlushQueued(uint64_t nTimeoutNs)
{
    IlasLedFlush(nTimeoutNs);
}

static void IlasLedWaitForInitResp(uint32_t nFidIdx, uint64_t nTimeoutMs)
{
    uint64_t nElapsedMs = 0ULL;

    while (nFidIdx < NUM_REMOTE_NODES &&
           gaIlasLedDiag[nFidIdx].nInitResp <= 0 &&
           nElapsedMs < nTimeoutMs)
    {
        IlasLedFlush(20ULL * MS_TO_NS);
        nElapsedMs += 20ULL;
    }
}

static void IlasLedWaitForProbeResp(uint32_t nFidIdx, uint64_t nTimeoutMs)
{
    uint64_t nElapsedMs = 0ULL;

    while (nFidIdx < NUM_REMOTE_NODES &&
           gaIlasLedDiag[nFidIdx].nProbeResp <= 0 &&
           nElapsedMs < nTimeoutMs)
    {
        IlasLedFlush(20ULL * MS_TO_NS);
        nElapsedMs += 20ULL;
    }
}

int IlasLed_SetColorRgbByFid(uint8_t nFid, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    int nStatus = IlasLed_QueueColorRgbByFid(nFid, nRed, nGreen, nBlue);
    if (nStatus == 0)
    {
        IlasLed_FlushQueued(ILAS_LED_RUNTIME_FLUSH_TIMEOUT_MS * MS_TO_NS);
    }
    return nStatus;
}

int IlasLed_QueueColorRgbNodesByFid(uint8_t nFid, const uint16_t* pnNodeAddrs,
    const uint8_t* pnRed, const uint8_t* pnGreen, const uint8_t* pnBlue, uint16_t nCount)
{
    uint32_t nFidIdx = (uint32_t)(nFid - 1U);
    uint32_t nResolvedIdx;
    ADI_EAL_STATUS eEalStatus;
    uint16_t nTxCount = 0U;

    if (nFid < 0x01U || nFid > 0x08U || nFidIdx >= NUM_REMOTE_NODES ||
        pnNodeAddrs == NULL || pnRed == NULL || pnGreen == NULL || pnBlue == NULL ||
        !IlasLedResolveFidIdx(nFidIdx, &nResolvedIdx) ||
        gahIlasLedHandle[nResolvedIdx] == NULL || !gaIlasLedState[nResolvedIdx].bRemoteValid)
    {
        return -1;
    }

    for (uint16_t i = 0U; i < nCount && nTxCount < ILAS_LED_ACTIVE_COUNT; i++)
    {
        uint16_t nAddr = pnNodeAddrs[i];
        if (nAddr == 0U || nAddr > ILAS_LED_ACTIVE_COUNT || nAddr > 0xFFU)
        {
            continue;
        }

        ganIlasLedRgbData[nResolvedIdx][nTxCount][0U] = pnRed[i];
        ganIlasLedRgbData[nResolvedIdx][nTxCount][1U] = pnGreen[i];
        ganIlasLedRgbData[nResolvedIdx][nTxCount][2U] = pnBlue[i];
        gaIlasLedRgbPayload[nResolvedIdx][nTxCount].pData = &ganIlasLedRgbData[nResolvedIdx][nTxCount][0U];
        gaIlasLedRgbPayload[nResolvedIdx][nTxCount].nAddress = (uint8_t)nAddr;
        gaIlasLedRgbPayload[nResolvedIdx][nTxCount].nInstruction = 0x04U;
        nTxCount++;
    }

    if (nTxCount == 0U)
    {
        return 0;
    }

    eEalStatus = IseledSetRgb((uint8_t)gaIlasLedState[nResolvedIdx].nRemoteNum,
        ADI_EAL_ISELED_NORMAL_MODE,
        &gaIlasLedRgbPayload[nResolvedIdx][0U],
        nTxCount,
        0xFFU,
        0x00U);

    gaIlasLedDiag[nResolvedIdx].nSetRgb = (int)eEalStatus;
    IlasLedTraceAppendf("FID=0x%02X IseledSetRgbNodes queued status=%d count=%u requested=%u phys=0x%02X",
        (unsigned)nFid, (int)eEalStatus, (unsigned)nTxCount, (unsigned)nCount,
        (unsigned)(nResolvedIdx + 1U));

    return (eEalStatus == ADI_EAL_STATUS_SUCCESS) ? 0 : (int)eEalStatus;
}

int IlasLed_SetColorRgbNodesByFid(uint8_t nFid, const uint16_t* pnNodeAddrs,
    const uint8_t* pnRed, const uint8_t* pnGreen, const uint8_t* pnBlue, uint16_t nCount)
{
    int nStatus = IlasLed_QueueColorRgbNodesByFid(nFid, pnNodeAddrs, pnRed, pnGreen, pnBlue, nCount);
    if (nStatus == 0)
    {
        IlasLed_FlushQueued(ILAS_LED_RUNTIME_FLUSH_TIMEOUT_MS * MS_TO_NS);
    }
    return nStatus;
}

int IlasLed_ReadFirstTempByFid(uint8_t nFid, uint8_t nAddress, uint8_t* pnData0,
    uint8_t* pnData1, uint8_t* pnData2, uint16_t* pnValidResp, uint16_t* pnExpectedResp)
{
    uint32_t nFidIdx = (uint32_t)(nFid - 1U);
    uint32_t nResolvedIdx;
    ADI_EAL_STATUS eEalStatus;
    uint64_t nElapsedMs = 0ULL;

    if (pnData0 == NULL || pnData1 == NULL || pnData2 == NULL ||
        pnValidResp == NULL || pnExpectedResp == NULL)
    {
        return -2;
    }

    *pnData0 = 0U;
    *pnData1 = 0U;
    *pnData2 = 0U;
    *pnValidResp = 0U;
    *pnExpectedResp = 0U;

    if (nFid < 0x01U || nFid > 0x08U || nFidIdx >= NUM_REMOTE_NODES ||
        !IlasLedResolveFidIdx(nFidIdx, &nResolvedIdx) ||
        gahIlasLedHandle[nResolvedIdx] == NULL || !gaIlasLedState[nResolvedIdx].bRemoteValid)
    {
        return -1;
    }

    if (nAddress == 0U)
    {
        nAddress = 1U;
    }

    ganIlasLastTempValidResp[nResolvedIdx] = 0U;
    ganIlasLastTempExpectedResp[nResolvedIdx] = 0U;
    ganIlasLastTempData[nResolvedIdx][0U] = 0U;
    ganIlasLastTempData[nResolvedIdx][1U] = 0U;
    ganIlasLastTempData[nResolvedIdx][2U] = 0U;

    eEalStatus = IseledReadTempOne((uint8_t)gaIlasLedState[nResolvedIdx].nRemoteNum, nAddress);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        return (int)eEalStatus;
    }

    while (ganIlasLastTempValidResp[nResolvedIdx] == 0U && nElapsedMs < 50ULL)
    {
        IlasLedFlush(5ULL * MS_TO_NS);
        nElapsedMs += 5ULL;
    }

    *pnData0 = ganIlasLastTempData[nResolvedIdx][0U];
    *pnData1 = ganIlasLastTempData[nResolvedIdx][1U];
    *pnData2 = ganIlasLastTempData[nResolvedIdx][2U];
    *pnValidResp = ganIlasLastTempValidResp[nResolvedIdx];
    *pnExpectedResp = ganIlasLastTempExpectedResp[nResolvedIdx];

    return (*pnValidResp > 0U) ? 0 : -3;
}
int IlasLed_AppendDiag(char* pBuf, size_t nSize)
{
    char* p = pBuf;
    int nTotal = 0;

    if (pBuf == NULL || nSize == 0U)
    {
        return 0;
    }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        const ILAS_LED_DIAG* poDiag = IlasLed_GetDiag(i);
        int nOpen = -99;
        int nCallback = -99;
        int nReset = -99;
        int nInit = -99;
        int nInitResp = 0;
        int nSetRgb = -99;
        int nSetRgbResp = 0;
        int nHandleValid = 0;

        if (poDiag != NULL)
        {
            nOpen = poDiag->nOpen;
            nCallback = poDiag->nCallback;
            nReset = poDiag->nReset;
            nInit = poDiag->nInit;
            nInitResp = poDiag->nInitResp;
            nSetRgb = poDiag->nSetRgb;
            nSetRgbResp = poDiag->nSetRgbResp;
            nHandleValid = poDiag->nHandleValid;
        }

        int n = snprintf(p, nSize - (size_t)nTotal,
            "|ILAS%02u:Open=%d,CB=%d,Rst=%d,Init=%d,InitResp=%d,SetRgb=%d,SetResp=%d,Hdl=%d",
            (unsigned)(i + 1U),
            nOpen,
            nCallback,
            nReset,
            nInit,
            nInitResp,
            nSetRgb,
            nSetRgbResp,
            nHandleValid);
        if (n <= 0) { break; }
        nTotal += n;
        if ((size_t)nTotal >= nSize)
        {
            pBuf[nSize - 1U] = '\0';
            return (int)(nSize - 1U);
        }
        p = pBuf + nTotal;
    }

    return nTotal;
}

int IlasLed_FormatStatusDiag(uint32_t nFidIdx, char* pBuf, size_t nSize)
{
    const ILAS_LED_DIAG* poDiag;

    if (pBuf == NULL || nSize == 0U || nFidIdx >= NUM_REMOTE_NODES)
    {
        return 0;
    }

    poDiag = IlasLed_GetDiag(nFidIdx);
    if (poDiag == NULL)
    {
        return snprintf(pBuf, nSize,
            "ILAS_DIAG(Open=-99,Rst=-99,Init=-99,Resp=0,Probe=-99,ProbeResp=0,Set=-99,SetResp=0,Hdl=0)");
    }

    return snprintf(pBuf, nSize,
        "ILAS_DIAG(Open=%d,Rst=%d,Init=%d,Resp=%d,Probe=%d,ProbeResp=%d,Set=%d,SetResp=%d,Hdl=%d)",
        poDiag->nOpen,
        poDiag->nReset,
        poDiag->nInit,
        poDiag->nInitResp,
        poDiag->nProbe,
        poDiag->nProbeResp,
        poDiag->nSetRgb,
        poDiag->nSetRgbResp,
        poDiag->nHandleValid);
}

static bool IlasLedResolveFidIdx(uint32_t nFidIdx, uint32_t* pnResolvedIdx)
{
    if (pnResolvedIdx == NULL || nFidIdx >= NUM_REMOTE_NODES || !gabIlasFidAliasValid[nFidIdx])
    {
        return false;
    }

    if (ganIlasFidAlias[nFidIdx] >= NUM_REMOTE_NODES)
    {
        return false;
    }

    *pnResolvedIdx = ganIlasFidAlias[nFidIdx];
    return true;
}

static void IlasLedFlush(uint64_t nTimeoutNs)
{
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
    adi_network_flushTxTimed(nTimeoutNs);
    adi_network_run();
}

static void IlasLedEalCallback(uint32_t nInstNum, ADI_EAL_ISELED_HANDLE hIseledHandle,
    ADI_EAL_ISELED_CBK_EVT eCbkEventType, const void* pArg)
{
    uint32_t nFidIdx = NUM_REMOTE_NODES;

    (void)nInstNum;

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahIlasLedHandle[i] == hIseledHandle)
        {
            nFidIdx = i;
            break;
        }
    }

    if (eCbkEventType == ADI_EAL_ISELED_CBK_EVT_ISELED_READ_RESP)
    {
        const ADI_EAL_ISELED_CBK_EVT_ISELED_READ_RESP_DATA* poResp =
            (const ADI_EAL_ISELED_CBK_EVT_ISELED_READ_RESP_DATA*)pArg;
        uint16_t nValidResp = (poResp != NULL) ? poResp->oTxnHdr.nNumValidResp : 0U;
        if (nFidIdx < NUM_REMOTE_NODES)
        {
            const ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD* poPayload =
                (poResp != NULL) ? (const ADI_E2BCORE_ISELED_FAST_TXN_PAYLOAD*)poResp->pPayload : NULL;
            gaIlasLedDiag[nFidIdx].nInitResp = (int)nValidResp;
            gaIlasLedDiag[nFidIdx].nProbeResp = (int)nValidResp;
            ganIlasLastTempValidResp[nFidIdx] = nValidResp;
            ganIlasLastTempExpectedResp[nFidIdx] =
                (poResp != NULL) ? poResp->oTxnHdr.nExpectedNumResp : 0U;
            if (poPayload != NULL && poPayload->pData != NULL)
            {
                ganIlasLastTempData[nFidIdx][0U] = poPayload->pData[0U];
                ganIlasLastTempData[nFidIdx][1U] = poPayload->pData[1U];
                ganIlasLastTempData[nFidIdx][2U] = poPayload->pData[2U];
            }
        }
        IlasLedTraceAppendf("FID=0x%02X ISELED read response valid=%u expected=%u",
            (unsigned)((nFidIdx < NUM_REMOTE_NODES) ? (nFidIdx + 1U) : 0U),
            (unsigned)nValidResp,
            (unsigned)((poResp != NULL) ? poResp->oTxnHdr.nExpectedNumResp : 0U));
    }
    else if (eCbkEventType == ADI_EAL_ISELED_CBK_EVT_ISELED_WRITE_RESP)
    {
        if (nFidIdx < NUM_REMOTE_NODES)
        {
            gaIlasLedDiag[nFidIdx].nSetRgbResp++;
        }
        IlasLedTraceAppendf("FID=0x%02X ISELED write response event=%d",
            (unsigned)((nFidIdx < NUM_REMOTE_NODES) ? (nFidIdx + 1U) : 0U),
            (int)eCbkEventType);
    }
    else if (eCbkEventType == ADI_EAL_ISELED_CBK_EVT_ERR)
    {
        IlasLedTraceAppendf("FID=0x%02X ISELED callback error",
            (unsigned)((nFidIdx < NUM_REMOTE_NODES) ? (nFidIdx + 1U) : 0U));
    }
}

static void IlasLedTraceAppendf(const char* pFmt, ...)
{
    char sLine[256];
    va_list args;

    if (gpfTraceLine == NULL)
    {
        return;
    }

    va_start(args, pFmt);
    (void)vsnprintf(sLine, sizeof(sLine), pFmt, args);
    va_end(args);
    sLine[sizeof(sLine) - 1U] = '\0';
    gpfTraceLine(sLine);
}


