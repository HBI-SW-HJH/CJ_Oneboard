#ifndef ILAS_LED_H
#define ILAS_LED_H

#include "ex_fioOsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ILAS_LED_TRACE_LINE_FN)(const char* pLine);

typedef struct
{
    int nOpen;
    int nCallback;
    int nReset;
    int nInit;
    int nInitResp;
    int nProbe;
    int nProbeResp;
    int nSetRgbResp;
    int nSetRgb;
    int nHandleValid;
} ILAS_LED_DIAG;

void IlasLed_SetTraceLineFn(ILAS_LED_TRACE_LINE_FN pfTraceLine);
void IlasLed_ResetAll(void);
void IlasLed_CloseAll(void);
bool IlasLed_AnyHandleOpen(void);
bool IlasLed_IsHandleOpen(uint32_t nFidIdx);
bool IlasLed_IsActive(uint32_t nFidIdx);
void IlasLed_RemapFidSlots(const uint8_t* pOldFids, const uint8_t* pNewFids, uint8_t nCount);
const ILAS_LED_DIAG* IlasLed_GetDiag(uint32_t nFidIdx);
ADI_EAL_STATUS IlasLed_InitFid(uint32_t nFidIdx, uint32_t nRemoteNum);
uint16_t IlasLed_GetActiveCount(void);
int IlasLed_SetColorRgbByFid(uint8_t nFid, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
int IlasLed_QueueColorRgbByFid(uint8_t nFid, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
void IlasLed_FlushQueued(uint64_t nTimeoutNs);
int IlasLed_SetColorRgbNodesByFid(uint8_t nFid, const uint16_t* pnNodeAddrs, const uint8_t* pnRed, const uint8_t* pnGreen, const uint8_t* pnBlue, uint16_t nCount);
int IlasLed_QueueColorRgbNodesByFid(uint8_t nFid, const uint16_t* pnNodeAddrs, const uint8_t* pnRed, const uint8_t* pnGreen, const uint8_t* pnBlue, uint16_t nCount);
int IlasLed_ReadFirstTempByFid(uint8_t nFid, uint8_t nAddress, uint8_t* pnData0, uint8_t* pnData1, uint8_t* pnData2, uint16_t* pnValidResp, uint16_t* pnExpectedResp);
int IlasLed_AppendDiag(char* pBuf, size_t nSize);
int IlasLed_FormatStatusDiag(uint32_t nFidIdx, char* pBuf, size_t nSize);

#ifdef __cplusplus
}
#endif

#endif /* ILAS_LED_H */

