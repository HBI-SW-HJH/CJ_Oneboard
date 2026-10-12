/*****************************************************************************
Copyright (c) 2026 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
*******************************************************************************
 * @file         : adi_Eth10BaseT1s_cfg.h
 * @brief        : This header file is auto generated from
                   EdgeStudio v4.0.0 - ex_OSP_FlexIO.e2bprj project
 * @author       : Automotive Software and Systems team, Bangalore, India
 * @date         : Mon Jun 08 2026 17:24:40 GMT+0900 (한국 표준시)
 * @note         : AD3304 x8 지원 (FID 0x01~0x08)
 *                 연결된 AD3304만 동작 — MAC 마지막 바이트 = FID
******************************************************************************/

/* =========================================================================
   |                   SELECTIVE INTERFACE DEFINES                         |
   ========================================================================= */
#ifndef ADI_E2B_IFACE_FIO_ENABLED
#define ADI_E2B_IFACE_FIO_ENABLED
#endif
#ifndef ADI_E2B_IFACE_FIO_OSP_ENABLED
#define ADI_E2B_IFACE_FIO_OSP_ENABLED
#endif
#ifndef ADI_E2B_IFACE_ISELED_ENABLED
#define ADI_E2B_IFACE_ISELED_ENABLED
#endif
#ifndef ADI_E2B_IFACE_SMC_ENABLED
#define ADI_E2B_IFACE_SMC_ENABLED
#endif
#ifndef ADI_E2B_IFACE_SQI_ENABLED
#define ADI_E2B_IFACE_SQI_ENABLED
#endif
#ifndef ADI_E2B_IFACE_SSC_ENABLED
#define ADI_E2B_IFACE_SSC_ENABLED
#endif
#ifndef ADI_E2B_IFACE_TOPDISC_ENABLED
#define ADI_E2B_IFACE_TOPDISC_ENABLED
#endif
#ifndef ADI_E2B_IFACE_WKSLP_ENABLED
#define ADI_E2B_IFACE_WKSLP_ENABLED
#endif
#ifndef ADI_E2B_IFACE_DIAG_ENABLED
#define ADI_E2B_IFACE_DIAG_ENABLED
#endif

/* =========================================================================
   |                               INCLUDES                                |
   ========================================================================= */
#include "adi_e2bcore.h"                     /*!< E2BCore header */
#include "adi_maclce_reg.h"                  /*!< MACLCE header */

/* =========================================================================
   |                                DEFINES                                |
   ========================================================================= */

#define CONTROLLER1_MAC_ADDR   (0x00e022fe70f0ULL)

/* -------------------------------------------------------------------------
 * AD3304 MAC 주소 테이블
 *   FID = MAC 마지막 바이트 (0x01 ~ 0x08)
 *   EdgeStudio v4.0.0 에서 생성된 프로젝트:
 *     REMOTE1 = 0x...7001 (FID 0x01)
 *     REMOTE2 = 0x...7002 (FID 0x02)
 *     REMOTE3 = 0x...7004 (FID 0x03 — EdgeStudio 4-node export: 0x04)
 *     REMOTE4 = 0x...7008 (FID 0x04 — EdgeStudio 4-node export: 0x08)
 *   실제 8개 AD3304는 FID 0x01~0x08 을 REMOTE_MAC_BASE+FID 로 계산하므로
 *   아래 REMOTE_MAC_BASE / REMOTE1~9 매크로를 8-node 체계로 유지한다.
 * ------------------------------------------------------------------------- */
#define REMOTE_MAC_BASE        (0x00e022fe7000ULL)
#define REMOTE1_MAC_ADDR       (0x00e022fe7000ULL)  /*!< FID 0x00 (예약) */
#define REMOTE2_MAC_ADDR       (0x00e022fe7001ULL)  /*!< FID 0x01 */
#define REMOTE3_MAC_ADDR       (0x00e022fe7002ULL)  /*!< FID 0x02 */
#define REMOTE4_MAC_ADDR       (0x00e022fe7003ULL)  /*!< FID 0x03 */
#define REMOTE5_MAC_ADDR       (0x00e022fe7004ULL)  /*!< FID 0x04 */
#define REMOTE6_MAC_ADDR       (0x00e022fe7005ULL)  /*!< FID 0x05 */
#define REMOTE7_MAC_ADDR       (0x00e022fe7006ULL)  /*!< FID 0x06 */
#define REMOTE8_MAC_ADDR       (0x00e022fe7007ULL)  /*!< FID 0x07 */
#define REMOTE9_MAC_ADDR       (0x00e022fe7008ULL)  /*!< FID 0x08 */
#define REMOTE1_MULTIMAC_ADDR  (0x01e022fe7000ULL)

#define GET_ID(InstNum, RemoteNum, IntfNum) \
{ \
    .nInstNum   = InstNum,    \
    .nRemoteNum = RemoteNum,  \
    .nIntfNum   = IntfNum     \
}

#define MAC_64_TO_8(nMacAddr) \
{\
    (nMacAddr & 0xFF0000000000ULL) >> 40U, \
    (nMacAddr & 0x00FF00000000ULL) >> 32U, \
    (nMacAddr & 0x0000FF000000ULL) >> 24U, \
    (nMacAddr & 0x000000FF0000ULL) >> 16U, \
    (nMacAddr & 0x00000000FF00ULL) >> 8U,  \
    (nMacAddr & 0x0000000000FFULL) >> 0U   \
}

/* =========================================================================
   |                                DATA                                   |
   ========================================================================= */
extern ADI_E2BCORE_CFG oEth10BaseT1sNtwrkCfg;

/* =========================================================================
   |                          CALLBACK PROTOTYPES                          |
   ========================================================================= */
extern void Eth10BaseT1sCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame, void *pCbParam);
extern void RemoteNodeCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame, void *pCbParam);
extern void FioOspInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame, void *pCbParam);
extern void IseledInterfaceCbk(const ADI_E2BCORE_ID *poID, ADI_E2BCORE_CBKTYPE eType, void *poParam, bool bIsTxFrame, void *pCbParam);

/* =========================================================================
   |                     MAC RUNTIME UPDATE (AD3304)                       |
   ========================================================================= */
/*!
 * @brief aoRemote1RegMap 내 MAC 관련 필드를 런타임에 갱신합니다.
 *        OspBridge_SetMac() 에서 adi_eal_configureNetwork() 재호출 전에 사용.
 * @param pMac6 [in] 6바이트 MAC 주소, MSB first
 */
extern void Eth10BaseT1sCfg_SetRemoteMac(const uint8_t *pMac6);
/*!< 인덱스(0~7)에 해당하는 Remote regmap MAC을 갱신 */
extern void Eth10BaseT1sCfg_SetRemoteMacByIdx(uint32_t nIdx, const uint8_t *pMac6);

/** @} */

/*
 * EOF: www.analog.com
*/
