
#include "ex_fioOsp.h"
#include "ilas_led.h"
#include "adi_otp.h"
#include <stdarg.h>
#include <stdio.h>

/*================================= DEFINES =================================*/
/*================================ DATA TYPES ===============================*/

/*================================ PROTOTYPES ===============================*/
static void InitSystem();
static void InitThread(void* arg);
static void CreateThreads(void);
static void CreateE2bApp(void);
static void CreateE2bAppFixed(void);
static void CreateE2bAppProgrammed(void);
static int OspBridge_ReinitE2bAndOsp(void);
static void AppThread1Fxn(void* arg);
static void AppThread2Fxn(void* arg);
static void HouseKeepingFxn(void* arg);
static void ConsoleRun(void);
static void PrintHelpContents(void);
static void FioOspDeviceInit(void);
static void FioOspResetFidDiag(void);
static void FioOspResetFidDiagOnly(void);
static uint32_t FioOspMarkDiscoveredFids(void);
static bool FioOspAnyFidHandleOpen(void);
static ADI_EAL_STATUS FioOspInitFid(uint32_t nFidIdx);
static void FioOspSetPwm(void);
static void FioOspReadPwm(void);
static void adi_network_cbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void* pData);
static void EalTopCallbackFunc(uint32_t nInstNum, ADI_EAL_CBK_EVT eTopCbkEvent, const void* pArg);
static void EalRemoteNodeCbk(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_NODE_CBK_EVT eRemoteCbkEvent,
    const void* pArg);
static void EalDiagCallback(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_DIAG_CBK_EVT eDiagCbkEvent,
    const void* pArg);
static void EalFioOspCallBack(uint32_t nInstNum, ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_EAL_FIO_OSP_CBK_EVT eCbkEventType,
    const void* pArg);

static int OspBridge_SetColorRgbByFid_Impl(uint8_t nFid, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);

/* FID + ?몃뱶 二쇱냼(unicast) 湲곕컲 ?⑥씪 LED ?쒖뼱 ??SlaveFlag ?쒖뼱??*/
static int OspBridge_SetColorRgbByNode_Impl(uint8_t nFid, uint16_t nNodeAddr, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
static int OspBridge_SetColorRgbByNodeEx_Impl(uint8_t nFid, uint16_t nNodeAddr, uint8_t nRed, uint8_t nGreen, uint8_t nBlue, bool bFlush);
static void OspBridge_CloseFioHandlesOnly(void);
static uint64_t OspBridge_Mac6To64(const uint8_t* pMac6);
static void OspBridge_Mac64To6(uint64_t nMac, uint8_t* pMac6);
static uint8_t FioOspNextSetReqId(void);
static void FioOspFillPwmData(ADI_FIO_OSP_PWM_DATA* poPwmData, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
static void FioOspSetAllActiveFioOff(void);
static uint8_t gnOtpReadAccessFid = 0U;
static bool gbOtpReadRestoreFixed = false;
static uint8_t gnFioSetPwmReqId = 0x40U;
/*=================================== DATA ==================================*/
/* --------------------- The MAC address mapping table --------------------- */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t ganRemoteMacAddrMap[NUM_REMOTE_NODES][6u];

/* --------------------------- Application data ---------------------------- */
/*! MAC-PHY config */
ADI_MEM_DATA_CRIT_CACHE
static ADI_NETWORK_MACPHY_CFG goMacPhyCfg;

ADI_MEM_DATA_CRIT_CACHE
static APP_DATA goAppData;

/*! Diagnostic Config */
ADI_MEM_DATA_CRIT_CACHE
static ADI_EAL_DIAG_CFG goEalDiagcfg;

/*! EAL FIO OSP config */
ADI_MEM_DATA_CRIT_CACHE
static ADI_EAL_FIO_OSP_CFG goEalFioOspCfg;

ADI_MEM_DATA_CRIT_CACHE
static ADI_EAL_FIO_OSP_HANDLE ghFioOspHandle;

#define OSP_FIO1_ONLY_TEST 1 /* ISELED bridge path; detailed ILAS logic lives in ilas_led.c/iseled.c. */
#define OSP_BRIDGE_DISABLE_ILAS 0 /* Set to 1 only for FIO isolation tests. */
#define ILAS_TARGET_FID_MASK 0xFFU /* bit0=FID01 ... bit7=FID08; probe all possible ILAS FIDs and accept only valid init responses. */
#define OSP_RUNTIME_TX_FLUSH_TIMEOUT_MS 5ULL
#define OSP_FIO_INTF_NUM 0U
#define OSP_FIO_MCAST_INTF_TOPIC ADI_E2BCORE_TOPIC_INTF0
#define OSP_FIO_MCAST_FIFO_TOPIC ADI_E2BCORE_TOPIC_FIFO0

/* ?? FIO 吏꾨떒 援ъ“泥???C#?먯꽌 OspBridge_GetFioDiag()濡??쎌쓬 ?? */
typedef struct
{
    /* FIO0 媛??④퀎 status (0=?깃났, 洹????ㅽ뙣肄붾뱶) */
    int nFio0Open;
    int nFio0Callback;
    int nFio0InstRAM;
    int nFio0Reset;
    int nFio0InitBidir;
    int nFio0AssignedAddr;
    int nFio0SetSetup;
    int nFio0SetState;
    /* FIO1 媛??④퀎 status */
    int nFio1Open;
    int nFio1Callback;
    int nFio1InstRAM;
    int nFio1Reset;
    int nFio1InitBidir;
    int nFio1AssignedAddr;
    int nFio1SetSetup;
    int nFio1SetState;
    /* ?몃뱾 NULL ?щ? (0=NULL, 1=?좏슚) */
    int nFio0HandleValid;
    int nFio1HandleValid;
} OSP_FIO_DIAG;

static OSP_FIO_DIAG goFioDiag = { -99,-99,-99,-99,-99,-99,-99,-99,
                                   -99,-99,-99,-99,-99,-99,-99,-99,
                                   0, 0 };

typedef struct
{
    int nOpen;
    int nCallback;
    int nInstRAM;
    int nReset;
    int nInitBidir;
    int nAssignedAddr;
    int nSetSetup;
    int nSetState;
    int nHandleValid;
} OSP_FID_FIO_DIAG;

static OSP_FID_FIO_DIAG gaFidFioDiag[NUM_REMOTE_NODES];

/* FIO1 OSP ?몃뱾 ??FID 0x03(IF4/IF5, SAIF4/SAIF5) 梨꾨꼸??*/
ADI_MEM_DATA_CRIT_CACHE
static ADI_EAL_FIO_OSP_HANDLE ghFioOspHandle2;

ADI_MEM_DATA_CRIT_CACHE
static ADI_EAL_FIO_OSP_HANDLE gahFidFioOspHandle[NUM_REMOTE_NODES];
/* FIO OSP multicast ?몃뱾 ??FID=0x00 broadcast 1???꾩넚??*/
static ADI_EAL_FIO_OSP_HANDLE ghFioMcastHandle = NULL;
static uint8_t gnFioMcastMem[ADI_EAL_FIO_OSP_MCAST_GROUP_MEM_SIZE(NUM_REMOTE_NODES)];
/* pahFioOspHandles???ъ씤????????ㅽ깮 ?꾨땶 ?꾩뿭 諛곗뿴 ?꾩닔 */
static ADI_EAL_FIO_OSP_HANDLE ganMcastHandles[NUM_REMOTE_NODES];
static ADI_EAL_FIO_OSP_MCAST_CFG goMcastCfg;

ADI_MEM_DATA_CRIT_CACHE
static uint8_t gnFioOspMem[FIO_OSP_MEM];

/* FIO1 硫붾え由?踰꾪띁 */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t gnFioOspMem2[FIO_OSP_MEM];

ADI_MEM_DATA_CRIT_CACHE
static uint32_t ganFidFioOspMem[NUM_REMOTE_NODES][FIO_OSP_MEM_WORDS];

/* Thread stack */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganInitThreadStack[6000];
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganAppThreadStack1[4096];
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganAppThreadStack2[4096];
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganHouseKeepingThreadStack[4500];

/* Task control blocks */
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganInitThreadTcb[PRJ_OSAL_TCB_SIZE];
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganAppThreadTcb1[PRJ_OSAL_TCB_SIZE];
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganAppThreadTcb2[PRJ_OSAL_TCB_SIZE];
ADI_MEM_DATA_CRIT_CACHE
static uint8_t  ganHouseKeepingThreadTcb[PRJ_OSAL_TCB_SIZE];

/* Configurations for the threads */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_THREAD_CFG goInitThreadCfg;
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_THREAD_CFG goAppThreadCfg1;
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_THREAD_CFG goAppThreadCfg2;
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_THREAD_CFG goHouseKeepingThreadCfg;

/* Thread handles */
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_THREAD_HANDLE ghAppThread1;
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_THREAD_HANDLE ghAppThread2;
ADI_MEM_DATA_CRIT_CACHE
static PRJ_OSAL_THREAD_HANDLE ghHouseKeepingThread;

/* Console data */
static uint32_t nFifoLvlData = 0U;
static uint32_t gnCmdLineBuffer[100U];
static ADI_FIO_OSP_PWM_DATA goPwmData;
/* AD3304/AD3301 FID蹂??곌껐 ?곹깭 諛?InitBidir 遺???몃뱶二쇱냼
 * index 0 = FID 0x00, index 7 = FID 0x07
 * NUM_REMOTE_NODES=1 ?좎? ??EAL? ?щ’ 1媛? FID 異붿쟻? ?ш린??蹂꾨룄 愿由?*/
static bool     gbAd3304Connected[8u] = {false,false,false,false,false,false,false,false};
static uint16_t gnFidNodeAddr[8u]     = {0U,0U,0U,0U,0U,0U,0U,0U};
static uint32_t gnFidRemoteNum[8u]    = {0U,0U,0U,0U,0U,0U,0U,0U};

static bool gbOspBridgeMode = false;
static bool gbOspBridgeInitialized = false;
/* FioOspDeviceInit() ? Connect ??1?뚮쭔 ?ㅽ뻾?쒕떎.
 * TX 寃쎈줈?먯꽌 ?ы샇異쒕릺?붾씪??RESET/INIT_BIDIR/SET_SETUP/GO_ACTIVE 瑜?諛섎났?섏? ?딅룄濡?蹂댄샇. */
static bool gbFioOspDeviceInitialized = false;
/* adi_pal_init() / adi_network_init() ? ?꾨줈?몄뒪 ?섎챸 ?숈븞 ??1?뚮쭔 ?몄텧 媛??
 * ?ъ뿰寃??쒖뿉?????뚮옒洹몃줈 ?ы샇異쒖쓣 嫄대꼫?. */
static bool gbSystemInitialized = false;
static uint16_t gnOspBridgeDeviceAddress = INIT_DEV_ADDR;
static char gsOspBridgeLastError[256] = "Not initialized";
static char gsFioTrace[4096];
static size_t gnFioTraceLen = 0U;
/* PLCA Node ID ??CreateE2bApp() ?먯꽌 ?ㅼ젙??媛믪쓣 異붿쟻, OspBridge_SetNodeId() 濡?蹂寃?媛??*/
static uint8_t gnPlcaNodeId = 1U;

#if defined(_WIN32)
#define OSP_BRIDGE_API __declspec(dllexport)
#define OSP_BRIDGE_CALL __stdcall
#else
#define OSP_BRIDGE_API
#define OSP_BRIDGE_CALL
#endif


static void OspStageLog(const char* pMsg)
{
    FILE* pFile = fopen("osp_init_stage.log", "a");
    if (pFile != NULL)
    {
        fprintf(pFile, "%s\n", (pMsg != NULL) ? pMsg : "<null>");
        fclose(pFile);
    }
}
static void OspStageLogf(const char* pFmt, ...)
{
    char sLine[256];
    va_list args;
    va_start(args, pFmt);
    (void)vsnprintf(sLine, sizeof(sLine), pFmt, args);
    va_end(args);
    sLine[sizeof(sLine) - 1U] = '\0';
    OspStageLog(sLine);
}
static void OspBridgeSetError(const char* pMsg)
{
    if (pMsg == NULL)
    {
        pMsg = "Unknown error";
    }

    (void)ADI_STRNCPY(gsOspBridgeLastError, pMsg, sizeof(gsOspBridgeLastError) - 1U);
    gsOspBridgeLastError[sizeof(gsOspBridgeLastError) - 1U] = '\0';
}

static void FioTraceReset(void)
{
    gnFioTraceLen = 0U;
    gsFioTrace[0] = '\0';
}

static void FioTraceAppend(const char* pMsg)
{
    if (pMsg == NULL || gnFioTraceLen >= (sizeof(gsFioTrace) - 1U))
    {
        return;
    }

    int n = snprintf(&gsFioTrace[gnFioTraceLen], sizeof(gsFioTrace) - gnFioTraceLen, "%s\r\n", pMsg);
    if (n > 0)
    {
        size_t nAdd = (size_t)n;
        size_t nRemain = sizeof(gsFioTrace) - gnFioTraceLen - 1U;
        gnFioTraceLen += (nAdd < nRemain) ? nAdd : nRemain;
        gsFioTrace[gnFioTraceLen] = '\0';
    }
}

static void FioTraceAppendf(const char* pFmt, ...)
{
    char sLine[256];
    va_list args;
    va_start(args, pFmt);
    (void)vsnprintf(sLine, sizeof(sLine), pFmt, args);
    va_end(args);
    sLine[sizeof(sLine) - 1U] = '\0';
    FioTraceAppend(sLine);
}
/*=================================== CODE ==================================*/
/**
 * @brief The application main entry point
 */
ADI_MEM_CODE_NOCRIT
int main(void)
{
#ifdef __NUCLEO_STM32__
    /* Initialize the platform abstraction layer */
    adi_pal_init();
#endif /* __NUCLEO_STM32__ */

    PRJ_OSAL_THREAD_HANDLE hThread;

    goInitThreadCfg.bIsOneShot = true;
    goInitThreadCfg.nPriority = PRJ_OSAL_THREAD_MAX_PRIORITY;
    goInitThreadCfg.nStackSize = sizeof(ganInitThreadStack);
    goInitThreadCfg.pStackMem = (void*)&ganInitThreadStack;
    goInitThreadCfg.nTaskControlBlockSize = sizeof(ganInitThreadTcb);
    goInitThreadCfg.pTaskControlBlockMem = (void*)&ganInitThreadTcb;
    goInitThreadCfg.pArgument = NULL;
    goInitThreadCfg.pfThread = InitThread;

    prj_osal_createThread(&goInitThreadCfg, &hThread);

    /* Start the scheduler. */
    prj_osal_startScheduler();

    /* Shouldn't reach here!! */
    ADI_DBG_ERROR();

    return 0;
}

/*=================================== STATIC ==================================*/
/**
 * @brief     Function for Init thread
 *
 * @param [in] arg Pointer for arguments
 */
ADI_MEM_CODE_CRIT
static void InitThread(void* pArg)
{
    /* Initializes the network and configures MACPHY */
    InitSystem();

    /* Initializes E2b stack and configures the network */
    CreateE2bApp();

    /* Create application threads */
    CreateThreads();
}

/** @brief Performs all the system initializations */
ADI_MEM_CODE_CRIT
static void InitSystem(void)
{
    uint32_t* pnNumDevices = NULL;
    ADI_NETWORK_DEVINFO* paoNetworkDevInfo = NULL;
    ADI_E2BCORE_VERSION oStackVersion;
    ADI_E2BCORE_BUILD_INFO oBuildInfo;

    /* Initialize the application data */
    (void)ADI_MEMSET(&goAppData, 0, sizeof(APP_DATA));

    /* ?? 1?뚯꽦 珥덇린??(?꾨줈?몄뒪 ?섎챸 ?숈븞 ????踰? ????????????????????????
     * adi_pal_init() / adi_network_init() ? deinit ?⑥닔媛 DLL???놁쑝誘濡?     * ?ъ뿰寃????ы샇異쒗븯硫??대? mutex ?곕뱶?쎌쑝濡??곴뎄 釉붾줈?밸맖.
     * gbSystemInitialized ?뚮옒洹몃줈 理쒖큹 1?뚮쭔 ?ㅽ뻾?쒕떎.              */
    if (!gbSystemInitialized)
    {
#ifndef __NUCLEO_STM32__
        adi_pal_init();
#endif

        app_utils_init();

        if (!gbOspBridgeMode)
        {
            app_utils_setupConsole();
        }

        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT,
            "\r\n===========================================================================\r\n");
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "                 Example - FIO AMS-OSRAM OSP RGBi LED TEST \r\n\r\n");
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " This application uses SA_IF2 as TX (SIOP) and SA_IF3 as RX (SION) pin\r\n");
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " SIOP must be pulled high (VDD = 5V) on powerup via pull-up resistor ~2.2k & SION pulled to gnd\r\n");
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, " This sets the communication mode to MCU \r\n");
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "    Kodachi Tx'ing: SIO-P is data, Manchester Encoded - SIO-N Unused \r\n");
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "    Kodachi Rx'ing: SIO-P is NRZ data - SIO-N is clock\r\n");
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "===========================================================================\r\n\n");

        adi_e2bcore_getBuildInfo(&oBuildInfo);
        adi_e2bcore_getVersion(&oStackVersion);

        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "App build time   : %s %s\r\n", __DATE__, __TIME__);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "E2blib build time: %s\r\n", oBuildInfo.sBuildTime);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "E2B Stack version: "UINT32_FORMATTER"."UINT32_FORMATTER"."UINT32_FORMATTER"\r\n\r\n",
            oStackVersion.nMajorVersion, oStackVersion.nMinorVersion, oStackVersion.nPatchVersion);

        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Host name: %s \r\n", HOST_NAME_STR);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Os type: %s \r\n", OS_TYPE);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Initialization sequence started\r\n");
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Platform Abstraction Layer initialized\r\n");

        /* Initialize the network layer ????1?뚮쭔 ?몄텧 */
        adi_network_init();
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Network layer initialized\r\n");

        gbSystemInitialized = true;
    }

    /* ?? 留??곌껐留덈떎 ?ㅽ뻾 ?????????????????????????????????????????????????
     * 肄쒕갚 ?щ벑濡앷낵 device open ? Close ??closeDevice 濡??댁젣?섎?濡?     * ?ъ뿰寃곕쭏??諛섎뱶???ㅼ떆 ?ㅼ젙?댁빞 ?쒕떎.                             */
    adi_network_registerCallback(&adi_network_cbk);

    /* Get the list of devices attached to the controller */
    adi_network_getDevices(&pnNumDevices, &paoNetworkDevInfo);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Got the list of network devices\r\n");

    for (uint32_t i = 0u; i < *pnNumDevices; i++)
    {
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\tId: "UINT32_FORMATTER", Desc: %s\r\n", paoNetworkDevInfo[i].nId,
            paoNetworkDevInfo[i].sDesc);
    }

    /* Set the controller's MAC address */
    adi_network_setControllerMacAddr(oEth10BaseT1sNtwrkCfg.anMacAddr);

    (void)ADI_MEMSET(&goMacPhyCfg, 0, sizeof(ADI_NETWORK_MACPHY_CFG));
    goMacPhyCfg.nSpiFreq = 15000000U;
    goMacPhyCfg.nClkPha = 0U;
    goMacPhyCfg.nClkPol = 0U;
    goMacPhyCfg.eCPS = ADI_NETWORK_CPS_32BYTES;
    goMacPhyCfg.bFTSE = true;
    goMacPhyCfg.bUse32BitTimestamp = false;
    goMacPhyCfg.bRXCTE = false;
    goMacPhyCfg.bTXCTE = false;
    goMacPhyCfg.bGPTPCntrEn = false;
    goMacPhyCfg.bMacTimer = false;
    goMacPhyCfg.bEnableRxBufferOverflowInt = true;
    goMacPhyCfg.bEnableSqiInt = false;
    goMacPhyCfg.bEnableSscInt = false;
    goMacPhyCfg.bEnableWkSlp = false;

    adi_network_openDevice(ADI_NETWORK_DEV_IDX, &goMacPhyCfg);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Opened the network device: Id: "UINT32_FORMATTER", Desc: %s\r\n",
        paoNetworkDevInfo[ADI_NETWORK_DEV_IDX].nId,
        paoNetworkDevInfo[ADI_NETWORK_DEV_IDX].sDesc);

#ifdef SNIFFER_MODE_EN
    adi_network_openDevice(ADI_NETWORK_ETH_IDX, &goMacPhyCfg);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Opened the sniffer device: Id: "UINT32_FORMATTER", Desc: %s\r\n",
        paoNetworkDevInfo[ADI_NETWORK_ETH_IDX].nId,
        paoNetworkDevInfo[ADI_NETWORK_ETH_IDX].sDesc);
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Waiting for PHY link-up...\r\n");
    adi_pal_waitMicroSec(5ULL * SEC_TO_US);
#endif

#if defined(__EV_SC594_SOM__)
    adi_pal_platformLedSet(SOM_SC594_LED5, true);
#endif
}

/** @brief Performs all the system initializations */
ADI_MEM_CODE_CRIT
static void CreateE2bApp(void)
{
    ADI_EAL_STATUS eEalStatus;

    /* MAC 二쇱냼??discovery 肄쒕갚?먯꽌 移⑹쑝濡쒕????먮룞 ?섏떊?섎?濡??ш린?쒕뒗 0?쇰줈 珥덇린??*/
    (void)ADI_MEMSET(&ganRemoteMacAddrMap[0][0u], 0, sizeof(ganRemoteMacAddrMap));

    goAppData.nMacCnt = 0U;
    (void)ADI_MEMSET(&ganRemoteMacAddrMap[0][0u], 0, sizeof(ganRemoteMacAddrMap));
    (void)memset(gbAd3304Connected, 0, sizeof(gbAd3304Connected));
    (void)memset(gnFidNodeAddr, 0, sizeof(gnFidNodeAddr));

    /* Initialize EAL */
    adi_eal_init();

    /* Create an instance of EAL mapping instance number with the device index */
    adi_eal_createInstance(0U, ADI_NETWORK_DEV_IDX, &oEth10BaseT1sNtwrkCfg);

#ifdef SNIFFER_MODE_EN
    /* Enable frame duplication */
    adi_eal_setFrameDuplication(0U, true);
#endif

    /* Register for EAL module callback */
    adi_eal_registerCallback(0U, &EalTopCallbackFunc);

    /* Register for node callbacks */
    adi_eal_registerNodeCallback(0U, &EalRemoteNodeCbk);

    /* PLCA NodeCnt = Controller(1) + AD3304 理쒕? 8媛?= 9
     * ?ㅼ젣 ?곌껐???섏? 臾닿??섍쾶 理쒕?媛믪쑝濡??ㅼ젙 ??誘몄뿰寃??щ’? ?묐떟 ?놁쓬?쇰줈 泥섎━?쒕떎. */
    adi_network_setPlca(ADI_NETWORK_DEV_IDX, true, MAX_PLCA_NODE_CNT, CTRL_PLCA_ID);
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 100ull * MS_TO_NS);

    /* Do a LV die reset */
    eEalStatus = adi_eal_resetRemoteLvDie(0U, ADI_EAL_ALL_REMOTES);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to reset remote nodes' LV die");
    adi_network_flushTxTimed(ADI_EAL_LV_RESET_WAIT_TIME_NS);

    /* Set the input enable for the SA_IF pins */
    eEalStatus = adi_eal_setInputEnable(0U, ADI_EAL_ALL_REMOTES, 0xFFFU);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed set the input enable for the SA_IF pins");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* Set bit to sample MAC address LSBs */
    eEalStatus = adi_eal_setMacAddrSampling(0U, ADI_EAL_ALL_REMOTES, true);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Setting bit to sample mac address failed");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* Enable PLCA for both the remote nodes */
    eEalStatus = adi_eal_setPlcaEn(0U, ADI_EAL_ALL_REMOTES, true);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to enable PLCA");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* Perform the discovery and register the MAC address of each remote */
    eEalStatus = adi_eal_startDiscoverySeq(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Disc sequence failed");
    // Wait for discovery to finish
    adi_network_flushTxTimed(ADI_EAL_DISC_TIMEOUT_NS);
    // Stop the discovery sequence
    eEalStatus = adi_eal_stopDiscoverySeq(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Disc sequence failed");
    ADI_DBG_ENSURE(goAppData.nMacCnt >= 1U, "Failed to discover required remote nodes");

    /* discovery ?꾨즺 ??諛쒓껄??紐⑤뱺 FID??PLCA ID ?ㅼ젙
     * FID = MAC 留덉?留?諛붿씠??(0x01~0x08) ??PLCA ID = FID */
    {
        ADI_EAL_NODE_PLCA_ID_CFG oPlcaCfg;
        for (uint32_t i = 0U; i < goAppData.nMacCnt; i++)
        {
            uint8_t nFid = ganRemoteMacAddrMap[i][5u];
            if (nFid < 1U || nFid > 8U) { continue; }
            oPlcaCfg.nMaskOfPlcaIdsUsed = 0x01U;
            oPlcaCfg.anPlcaID[0U] = nFid;  /* PLCA ID = FID (1~8) */
            (void)adi_eal_setRemotePlcaId(0U, &ganRemoteMacAddrMap[i][0u], &oPlcaCfg, 0x0U);
        }
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
    }

    /* Clear MAC addr sampling from SA_IF pins */
    eEalStatus = adi_eal_setMacAddrSampling(0U, ADI_EAL_ALL_REMOTES, false);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to clear sample MAC address bit");

    /* Read the revision number of the remotes */
    eEalStatus = adi_eal_getRevision(0U, ADI_EAL_ALL_REMOTES);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to read revision number");
    adi_network_flushTxTimed(10ULL * MS_TO_NS);

    /* Configure the network */
    eEalStatus = adi_eal_configureNetwork(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to configure network");

    /* Initialize EAL Diag module */
    goEalDiagcfg.pfDiagCallback = &EalDiagCallback;
    (void)adi_eal_diagInit(0U, &goEalDiagcfg);

    PrintHelpContents();

    FioOspDeviceInit();

#if defined(__EV_SC594_SOM__)
    adi_pal_platformLedSet(SOM_SC594_LED4, true);
#endif
}

/**
 * @brief Fixed-MAC variant of CreateE2bApp.
 *
 * 李⑥씠??(vs CreateE2bApp):
 *  1. ganRemoteMacAddrMap ??MEMSET?섏? ?딅뒗??
 *     ??OspBridge_InitFixed() ?먯꽌 誘몃━ 梨꾩슫 怨좎젙 MAC??蹂댁〈?쒕떎.
 *  2. setRemotePlcaId 瑜?startDiscovery ?꾩뿉 癒쇱? ?몄텧?쒕떎.
 *     ??Power-On 吏곹썑 AD3301 ??湲곕낯 MAC?쇰줈 由ъ뀑???곹깭?먯꽌?? *        PLCA ?듭떊??李몄뿬?섏뿬 discovery MAC ?묐떟??蹂대궪 ???덈떎.
 *  3. discovery 肄쒕갚? ?섏떊 MAC??臾댁“嫄??깅줉?섎?濡?
 *     怨좎젙 MAC怨??쇱튂 ?щ? 鍮꾧탳??OspBridge_InitFixed() 媛 ?대떦?쒕떎.
 */
ADI_MEM_CODE_CRIT
static void CreateE2bAppFixed(void)
{
    ADI_EAL_STATUS eEalStatus;

    goAppData.nMacCnt = 0U;
    (void)ADI_MEMSET(&ganRemoteMacAddrMap[0][0u], 0, sizeof(ganRemoteMacAddrMap));
    (void)memset(gbAd3304Connected, 0, sizeof(gbAd3304Connected));
    (void)memset(gnFidNodeAddr, 0, sizeof(gnFidNodeAddr));

    /* ?? MEMSET ?앸왂 ?? ganRemoteMacAddrMap ? ?대? 怨좎젙 MAC?쇰줈 梨꾩썙???덉쓬 */

    /* Initialize EAL */
    adi_eal_init();

    /* Create an instance of EAL mapping instance number with the device index */
    adi_eal_createInstance(0U, ADI_NETWORK_DEV_IDX, &oEth10BaseT1sNtwrkCfg);

#ifdef SNIFFER_MODE_EN
    adi_eal_setFrameDuplication(0U, true);
#endif

    /* Register for EAL module callback */
    adi_eal_registerCallback(0U, &EalTopCallbackFunc);

    /* Register for node callbacks */
    adi_eal_registerNodeCallback(0U, &EalRemoteNodeCbk);

    /* PLCA NodeCnt = MAX (Controller + AD3304 8媛? */
    adi_network_setPlca(ADI_NETWORK_DEV_IDX, true, MAX_PLCA_NODE_CNT, CTRL_PLCA_ID);
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 100ull * MS_TO_NS);

    eEalStatus = adi_eal_resetRemoteLvDie(0U, ADI_EAL_ALL_REMOTES);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to reset remote nodes' LV die");
    adi_network_flushTxTimed(ADI_EAL_LV_RESET_WAIT_TIME_NS);

     /* Set the input enable for the SA_IF pins */
    eEalStatus = adi_eal_setInputEnable(0U, ADI_EAL_ALL_REMOTES, 0xFFFU);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed set the input enable for the SA_IF pins");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* Set bit to sample MAC address LSBs */
    eEalStatus = adi_eal_setMacAddrSampling(0U, ADI_EAL_ALL_REMOTES, true);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Setting bit to sample mac address failed");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* ?? LINMaster ?쒖꽌: setRemotePlcaId瑜?癒쇱? ??
     * discovery ?꾩뿉 怨좎젙 MAC 湲곕컲?쇰줈 PLCA ID瑜??ㅼ젙?댁빞
     * AD3301??PLCA ?듭떊??李몄뿬?섏뿬 discovery ?묐떟??蹂대궦?? */
    /* ?댁쟾 ?⑥씪 setRemotePlcaId ?쒓굅 ???꾨옒 猷⑦봽濡??듯빀 */

    /* FID 0x00~0x07 MAC?먮룄 PLCA ID ?좏뻾 ?깅줉 (?곌껐??蹂대뱶媛 ?묐떟?????덈룄濡?
     * REMOTE_MAC_BASE + FID = ?ㅼ젣 HW MAC (Power-On 湲곕낯媛? */
    {
        uint8_t anFidMac[6u];
        ADI_EAL_NODE_PLCA_ID_CFG oFidPlcaCfg;
        for (uint8_t nFid = 0x01U; nFid <= 0x08U; nFid++)
        {
            app_utils_mac64To8(anFidMac, REMOTE_MAC_BASE + (uint64_t)nFid);
            oFidPlcaCfg.nMaskOfPlcaIdsUsed = 0x01U;
            oFidPlcaCfg.anPlcaID[0U] = nFid;  /* PLCA ID = FID (1~8) */
            (void)adi_eal_setRemotePlcaId(0U, anFidMac, &oFidPlcaCfg, 0x0U);
        }
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
    }

    /* Enable PLCA for both the remote nodes */
    eEalStatus = adi_eal_setPlcaEn(0U, ADI_EAL_ALL_REMOTES, true);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to enable PLCA");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* Perform the discovery and register the MAC address of each remote */
    eEalStatus = adi_eal_startDiscoverySeq(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Disc sequence failed");
    adi_network_flushTxTimed(ADI_EAL_DISC_TIMEOUT_NS);
    eEalStatus = adi_eal_stopDiscoverySeq(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Disc sequence stop failed");
    ADI_DBG_ENSURE(goAppData.nMacCnt >= 1U, "Failed to discover required remote nodes");


    /* Clear MAC addr sampling from SA_IF pins */
    eEalStatus = adi_eal_setMacAddrSampling(0U, ADI_EAL_ALL_REMOTES, false);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to clear sample MAC address bit");

    /* Read the revision number of the remotes */
    eEalStatus = adi_eal_getRevision(0U, ADI_EAL_ALL_REMOTES);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to read revision number");
    adi_network_flushTxTimed(10ULL * MS_TO_NS);

    /* Configure the network */
    eEalStatus = adi_eal_configureNetwork(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to configure network");

    /* Initialize EAL Diag module */
    goEalDiagcfg.pfDiagCallback = &EalDiagCallback;
    (void)adi_eal_diagInit(0U, &goEalDiagcfg);

    PrintHelpContents();

    FioOspDeviceInit();

#if defined(__EV_SC594_SOM__)
    adi_pal_platformLedSet(SOM_SC594_LED4, true);
#endif
}

/**
 * @brief ?꾨줈洹몃옩?쇰줈 ?ㅼ젙??MAC(ganRemoteMacAddrMap)?쇰줈 E2B/PLCA瑜??ш뎄?깊빀?덈떎.
 *        SA_IF ?ㅼ쐞移??섑뵆留곸? ?ъ슜?섏? ?딆뒿?덈떎 (?ㅼ쐞移?媛믪쑝濡???뼱?곗? ?딆쓬).
 *        ?몄텧 ??ganRemoteMacAddrMap 諛?Eth10BaseT1sCfg_SetRemoteMac() ???꾨즺?섏뼱 ?덉뼱???⑸땲??
 */
ADI_MEM_CODE_CRIT
static void CreateE2bAppProgrammed(void)
{
    ADI_EAL_STATUS eEalStatus;

    adi_eal_init();
    adi_eal_createInstance(0U, ADI_NETWORK_DEV_IDX, &oEth10BaseT1sNtwrkCfg);

#ifdef SNIFFER_MODE_EN
    adi_eal_setFrameDuplication(0U, true);
#endif

    adi_eal_registerCallback(0U, &EalTopCallbackFunc);
    adi_eal_registerNodeCallback(0U, &EalRemoteNodeCbk);

    adi_network_setPlca(ADI_NETWORK_DEV_IDX, true, MAX_PLCA_NODE_CNT, CTRL_PLCA_ID);
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 100ull * MS_TO_NS);

    eEalStatus = adi_eal_setInputEnable(0U, ADI_EAL_ALL_REMOTES, 0xFFFU);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed set the input enable for the SA_IF pins");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* ?꾨줈洹몃옩 MAC 寃쎈줈: ?ㅼ쐞移??섑뵆留?鍮꾪솢???좎? */
    eEalStatus = adi_eal_setMacAddrSampling(0U, ADI_EAL_ALL_REMOTES, false);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to disable MAC address sampling");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* FID 0x01~0x08 ?꾩껜??PLCA ID ?좏뻾 ?깅줉 ??programmed MAC 寃쎈줈?먯꽌???숈씪?섍쾶 泥섎━ */
    {
        uint8_t anFidMac[6u];
        ADI_EAL_NODE_PLCA_ID_CFG oFidPlcaCfg;
        for (uint8_t nFid = 0x01U; nFid <= 0x08U; nFid++)
        {
            app_utils_mac64To8(anFidMac, REMOTE_MAC_BASE + (uint64_t)nFid);
            oFidPlcaCfg.nMaskOfPlcaIdsUsed = 0x01U;
            oFidPlcaCfg.anPlcaID[0U] = nFid;  /* PLCA ID = FID */
            (void)adi_eal_setRemotePlcaId(0U, anFidMac, &oFidPlcaCfg, 0x0U);
        }
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
    }

    eEalStatus = adi_eal_setPlcaEn(0U, ADI_EAL_ALL_REMOTES, true);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to enable PLCA");
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    eEalStatus = adi_eal_startDiscoverySeq(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Disc sequence failed");
    adi_network_flushTxTimed(ADI_EAL_DISC_TIMEOUT_NS);
    eEalStatus = adi_eal_stopDiscoverySeq(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Disc sequence stop failed");
    ADI_DBG_ENSURE(goAppData.nMacCnt >= 1U, "Failed to discover required remote nodes");
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to read revision number");
    adi_network_flushTxTimed(10ULL * MS_TO_NS);

    eEalStatus = adi_eal_configureNetwork(0U);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to configure network");

    goEalDiagcfg.pfDiagCallback = &EalDiagCallback;
    (void)adi_eal_diagInit(0U, &goEalDiagcfg);

    FioOspDeviceInit();
}

/**
 * @brief FIO-OSP 瑜??リ퀬 E2B ?ㅽ깮???대졇?ㅺ? ganRemoteMacAddrMap 湲곗??쇰줈 ?ㅼ떆 ?щ┰?덈떎.
 * @return 0=?깃났, ?뚯닔=?ㅻ쪟
 */
ADI_MEM_CODE_CRIT
static void OspBridge_CloseFioHandlesOnly(void)
{
    if (ghFioOspHandle != NULL)
    {
        (void)adi_eal_fioOspClose(ghFioOspHandle);
        ghFioOspHandle = NULL;
    }

    if (ghFioMcastHandle != NULL)
    {
        (void)adi_eal_fioOspClose(ghFioMcastHandle);
        ghFioMcastHandle = NULL;
    }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahFidFioOspHandle[i] != NULL)
        {
            (void)adi_eal_fioOspClose(gahFidFioOspHandle[i]);
            gahFidFioOspHandle[i] = NULL;
        }
    }
    IlasLed_CloseAll();

    ghFioOspHandle2 = NULL;
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++) { ganMcastHandles[i] = NULL; }
    (void)ADI_MEMSET(&goMcastCfg, 0, sizeof(goMcastCfg));
    (void)ADI_MEMSET(gnFioMcastMem, 0, sizeof(gnFioMcastMem));
    gbFioOspDeviceInitialized = false;
}

static uint64_t OspBridge_Mac6To64(const uint8_t* pMac6)
{
    if (pMac6 == NULL)
    {
        return 0ULL;
    }

    return ((uint64_t)pMac6[0U] << 40U) |
           ((uint64_t)pMac6[1U] << 32U) |
           ((uint64_t)pMac6[2U] << 24U) |
           ((uint64_t)pMac6[3U] << 16U) |
           ((uint64_t)pMac6[4U] <<  8U) |
           ((uint64_t)pMac6[5U]);
}

static void OspBridge_Mac64To6(uint64_t nMac, uint8_t* pMac6)
{
    if (pMac6 == NULL)
    {
        return;
    }

    pMac6[0U] = (uint8_t)((nMac >> 40U) & 0xFFU);
    pMac6[1U] = (uint8_t)((nMac >> 32U) & 0xFFU);
    pMac6[2U] = (uint8_t)((nMac >> 24U) & 0xFFU);
    pMac6[3U] = (uint8_t)((nMac >> 16U) & 0xFFU);
    pMac6[4U] = (uint8_t)((nMac >>  8U) & 0xFFU);
    pMac6[5U] = (uint8_t)( nMac         & 0xFFU);
}

static uint8_t FioOspNextSetReqId(void)
{
    gnFioSetPwmReqId++;
    if (gnFioSetPwmReqId == 0U)
    {
        gnFioSetPwmReqId = 1U;
    }
    return gnFioSetPwmReqId;
}

static void FioOspFillPwmData(ADI_FIO_OSP_PWM_DATA* poPwmData, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    if (poPwmData == NULL)
    {
        return;
    }

    poPwmData->nRedPwm = (uint16_t)((((uint32_t)nRed) * 0x7FFFU + 127U) / 255U);
    poPwmData->bRedDayMode = true;
    poPwmData->nGreenPwm = (uint16_t)((((uint32_t)nGreen) * 0x7FFFU + 127U) / 255U);
    poPwmData->bGreenDayMode = true;
    poPwmData->nBluePwm = (uint16_t)((((uint32_t)nBlue) * 0x7FFFU + 127U) / 255U);
    poPwmData->bBlueDayMode = true;

    if (nBlue == 0U && (nRed != 0U || nGreen != 0U) && poPwmData->nBluePwm == 0U)
    {
        poPwmData->nBluePwm = 1U;
    }
}

static void FioOspSetAllActiveFioOff(void)
{
    ADI_FIO_OSP_PWM_DATA oOffData;
    bool bQueued = false;

    FioOspFillPwmData(&oOffData, 0U, 0U, 0U);

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahFidFioOspHandle[i] == NULL)
        {
            continue;
        }

        ADI_EAL_STATUS eStatus = adi_fioOspSetPwm(
            gahFidFioOspHandle[i], (uint16_t)BROADCAST_DEV_ADDR, &oOffData, 0xFFU);
        FioTraceAppendf("FID=0x%02X init OFF SetPwm status=%d", (unsigned)(i + 1U), (int)eStatus);
        if (eStatus == ADI_EAL_STATUS_SUCCESS)
        {
            bQueued = true;
        }
    }

    if (bQueued)
    {
        adi_network_flushTxTimed(80ULL * MS_TO_NS);
        adi_network_run();
        adi_network_flushTxTimed(20ULL * MS_TO_NS);
        adi_network_run();
    }
}

ADI_MEM_CODE_CRIT
static int OspBridge_ReinitE2bAndOsp(void)
{
    /* ???ъ큹湲고솕 ???뚮옒洹?由ъ뀑 ??FioOspDeviceInit ???ㅼ떆 ?ㅽ뻾?섎룄濡?*/
    OspBridge_CloseFioHandlesOnly();

    adi_eal_terminateInstance(0U);
    goAppData.nMacCnt = 0U;

    CreateE2bAppProgrammed();
    if (goAppData.nMacCnt == 0U)
    {
        OspBridgeSetError("E2B rediscovery failed after programmed MAC change");
        return -3;
    }

    FioOspDeviceInit();
    return 0;
}

/**
* @brief Create threads for application and housekeeping tasks
*
*/
ADI_MEM_CODE_CRIT
static void CreateThreads(void)
{
    if (ghAppThread1 != NULL || ghAppThread2 != NULL || ghHouseKeepingThread != NULL)
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] CreateThreads skipped (already created)\r\n");
        return;
    }

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n\r\nSwitching to run mode\r\n");
    goAppData.eAppMode = APP_MODE_RUN;

    goAppThreadCfg1.bIsOneShot = false;
    goAppThreadCfg1.nPriority = 2;
    goAppThreadCfg1.nStackSize = sizeof(ganAppThreadStack1);
    goAppThreadCfg1.pStackMem = (void*)&ganAppThreadStack1;
    goAppThreadCfg1.nTaskControlBlockSize = sizeof(ganAppThreadTcb1);
    goAppThreadCfg1.pTaskControlBlockMem = (void*)&ganAppThreadTcb1;
    goAppThreadCfg1.pArgument = NULL;
    goAppThreadCfg1.pfThread = AppThread1Fxn;

    goAppThreadCfg2.bIsOneShot = false;
    goAppThreadCfg2.nPriority = 2;
    goAppThreadCfg2.nStackSize = sizeof(ganAppThreadStack2);
    goAppThreadCfg2.pStackMem = (void*)&ganAppThreadStack2;
    goAppThreadCfg2.nTaskControlBlockSize = sizeof(ganAppThreadTcb2);
    goAppThreadCfg2.pTaskControlBlockMem = (void*)&ganAppThreadTcb2;
    goAppThreadCfg2.pArgument = NULL;
    goAppThreadCfg2.pfThread = AppThread2Fxn;

    goHouseKeepingThreadCfg.bIsOneShot = false;
    goHouseKeepingThreadCfg.nPriority = 1;
    goHouseKeepingThreadCfg.nStackSize = sizeof(ganHouseKeepingThreadStack);
    goHouseKeepingThreadCfg.pStackMem = (void*)&ganHouseKeepingThreadStack;
    goHouseKeepingThreadCfg.nTaskControlBlockSize = sizeof(ganHouseKeepingThreadTcb);
    goHouseKeepingThreadCfg.pTaskControlBlockMem = (void*)&ganHouseKeepingThreadTcb;
    goHouseKeepingThreadCfg.pArgument = NULL;
    goHouseKeepingThreadCfg.pfThread = HouseKeepingFxn;

    prj_osal_createThread(&goAppThreadCfg1, &ghAppThread1);
    prj_osal_createThread(&goAppThreadCfg2, &ghAppThread2);
    prj_osal_createThread(&goHouseKeepingThreadCfg, &ghHouseKeepingThread);
}

/**
 * @brief      Application thread1 function
 *
 * @param [in] arg Pointer for arguments
 */
ADI_MEM_CODE_CRIT
static void AppThread1Fxn(void* pArg)
{
    /* OspBridge 紐⑤뱶(PC ??DLL 吏곸젒 ?쒖뼱)?먯꽌??C# 痢?SetColorRgbByNode/SetColorRgb媛
     * PWM ?꾩넚???대떦?쒕떎. ?ㅻ젅?쒓? ?먯껜 FioOspSetPwm()??諛섎났 ?몄텧?섎㈃ C# TX?
     * 異⑸룎?섏뿬 遺덊븘?뷀븳 Init ?쒗??RESET?묲NIT_BIDIR?뭆ET_SETUP?묰O_ACTIVE)媛
     * 留??됱긽 ?꾪솚留덈떎 踰꾩뒪???ъ쟾?〓릺誘濡? OspBridge 紐⑤뱶?먯꽌???ㅽ뻾??嫄대꼫?. */
    if (!gbOspBridgeMode && goAppData.eAppMode == APP_MODE_RUN)
    {
        FioOspSetPwm();
    }

    prj_osal_sleepThread(ghAppThread1, 1000U);
}

/**
 * @brief      Application thread2 function
 *
 * @param [in] arg Pointer for arguments
 */
ADI_MEM_CODE_CRIT
static void AppThread2Fxn(void* pArg)
{
    /* OspBridge 紐⑤뱶?먯꽌???먮룞 ReadPwm 猷⑦봽瑜?鍮꾪솢?깊솕?쒕떎.
     * FioOspReadPwm()??諛쒗뻾?섎뒗 READ_PWM/READ_TEMP ?꾨젅?꾩씠
     * C# SetColorRgbByNode TX ?ъ씠???쎌엯?섏뼱 ?⑦궥 ?ㅼ뿼???좊컻?쒕떎. */
    if (!gbOspBridgeMode && goAppData.eAppMode == APP_MODE_RUN)
    {
        FioOspReadPwm();
    }

    prj_osal_sleepThread(ghAppThread2, 1000U);
}

/**
 * @brief      Function for housekeeping tasks
 *
 * @param [in] arg Pointer for arguments
 */
static void HouseKeepingFxn(void* pArg)
{
    /* Function that sends out the frames, clearing the buffer.
    This must be periodically called to actually send out the frames */
    adi_network_run();

    /* See if there is some console data to be sent or processed */
    ConsoleRun();
}

ADI_MEM_CODE_CRIT
static void ConsoleRun(void)
{
    bool bCmdProcessed = app_utils_consoleRun(ADI_NETWORK_DEV_IDX, &goAppData.eAppCmd, &goAppData.eAppMode);

    if (!bCmdProcessed)
    {
        /* Process any application specific command here */
        if (goAppData.eAppCmd == APP_CMD_INTF_STAT)
        {
            /* Interface stats for all the enabled interfaces */
            ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "\r\n");
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT, " ");
            adi_eal_fioOspReadStatus(ghFioOspHandle);
        }
    }

    goAppData.eAppCmd = APP_CMD_NONE;
}


ADI_MEM_CODE_CRIT
static void PrintHelpContents(void)
{
    app_utils_printHelpContents();
}

static void FioOspResetFidDiag(void)
{
    FioOspResetFidDiagOnly();
    IlasLed_ResetAll();
}

static void FioOspResetFidDiagOnly(void)
{
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        gaFidFioDiag[i].nOpen = -99;
        gaFidFioDiag[i].nCallback = -99;
        gaFidFioDiag[i].nInstRAM = -99;
        gaFidFioDiag[i].nReset = -99;
        gaFidFioDiag[i].nInitBidir = -99;
        gaFidFioDiag[i].nAssignedAddr = 0;
        gaFidFioDiag[i].nSetSetup = -99;
        gaFidFioDiag[i].nSetState = -99;
        gaFidFioDiag[i].nHandleValid = 0;
        gahFidFioOspHandle[i] = NULL;
        gnFidNodeAddr[i] = 0U;
        gnFidRemoteNum[i] = i;
    }
}

static uint32_t FioOspMarkDiscoveredFids(void)
{
    uint32_t nMask = 0U;

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        uint8_t nFid = ganRemoteMacAddrMap[i][5U];
        if ((ganRemoteMacAddrMap[i][0U] == 0U) &&
            (ganRemoteMacAddrMap[i][1U] == 0U) &&
            (ganRemoteMacAddrMap[i][2U] == 0U) &&
            (ganRemoteMacAddrMap[i][3U] == 0U) &&
            (ganRemoteMacAddrMap[i][4U] == 0U) &&
            (ganRemoteMacAddrMap[i][5U] == 0U))
        {
            continue;
        }
        FioTraceAppendf("DISC idx=%u MAC=%02X:%02X:%02X:%02X:%02X:%02X last=0x%02X",
            (unsigned)i,
            ganRemoteMacAddrMap[i][0U], ganRemoteMacAddrMap[i][1U],
            ganRemoteMacAddrMap[i][2U], ganRemoteMacAddrMap[i][3U],
            ganRemoteMacAddrMap[i][4U], ganRemoteMacAddrMap[i][5U],
            (unsigned)nFid);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
            "[DIAG] DISC idx=%u MAC=%02X:%02X:%02X:%02X:%02X:%02X last=0x%02X\r\n",
            (unsigned)i,
            ganRemoteMacAddrMap[i][0U], ganRemoteMacAddrMap[i][1U],
            ganRemoteMacAddrMap[i][2U], ganRemoteMacAddrMap[i][3U],
            ganRemoteMacAddrMap[i][4U], ganRemoteMacAddrMap[i][5U],
            (unsigned)nFid);
        if ((nFid >= 0x01U) && (nFid <= 0x08U))
        {
            gbAd3304Connected[nFid - 1U] = true;
            gnFidRemoteNum[nFid - 1U] = (uint32_t)(nFid - 1U);
            FioTraceAppendf("DISC map FID=0x%02X -> discIdx=%u fioRemote=%u PLCA=%u",
                (unsigned)nFid, (unsigned)i, (unsigned)gnFidRemoteNum[nFid - 1U], (unsigned)nFid);
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                "[DIAG] DISC map FID=0x%02X -> discIdx=%u fioRemote=%u PLCA=%u\r\n",
                (unsigned)nFid, (unsigned)i, (unsigned)gnFidRemoteNum[nFid - 1U], (unsigned)nFid);
        }
        else
        {
            FioTraceAppendf("DISC ignore idx=%u: MAC last byte is not FID 0x01..0x08",
                (unsigned)i);
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                "[DIAG] DISC ignore idx=%u: MAC last byte is not FID 0x01..0x08\r\n",
                (unsigned)i);
        }
    }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gbAd3304Connected[i])
        {
            nMask |= (1UL << i);
        }
    }

    if (!gbAd3304Connected[0] &&
        (gbAd3304Connected[1] || gbAd3304Connected[2] || gbAd3304Connected[3]))
    {
        uint8_t anFid1Mac[6U];
        app_utils_mac64To8(anFid1Mac, REMOTE_MAC_BASE + 0x01ULL);
        (void)memcpy(&ganRemoteMacAddrMap[0][0U], anFid1Mac, 6U);
        Eth10BaseT1sCfg_SetRemoteMacByIdx(0U, anFid1Mac);
        (void)adi_eal_writeMacAddr(0U, 0U, anFid1Mac);
        gbAd3304Connected[0] = true;
        gnFidRemoteNum[0] = 0U;
        nMask |= 0x01U;
        if (goAppData.nMacCnt < 4U)
        {
            goAppData.nMacCnt++;
        }
        FioTraceAppendf("DISC fallback FID=0x01 -> remote=0 MAC=%02X:%02X:%02X:%02X:%02X:%02X",
            anFid1Mac[0], anFid1Mac[1], anFid1Mac[2],
            anFid1Mac[3], anFid1Mac[4], anFid1Mac[5]);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
            "[DIAG] DISC fallback FID=0x01 -> remote=0 MAC=%02X:%02X:%02X:%02X:%02X:%02X\r\n",
            anFid1Mac[0], anFid1Mac[1], anFid1Mac[2],
            anFid1Mac[3], anFid1Mac[4], anFid1Mac[5]);
    }

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
        "[DIAG] FID discovery map: nMacCnt=%u mask=0x%02X remote=[%u,%u,%u,%u,%u,%u,%u,%u]\r\n",
        (unsigned)goAppData.nMacCnt, (unsigned)nMask,
        (unsigned)gnFidRemoteNum[0], (unsigned)gnFidRemoteNum[1],
        (unsigned)gnFidRemoteNum[2], (unsigned)gnFidRemoteNum[3],
        (unsigned)gnFidRemoteNum[4], (unsigned)gnFidRemoteNum[5],
        (unsigned)gnFidRemoteNum[6], (unsigned)gnFidRemoteNum[7]);
    FioTraceAppendf("FID discovery map: nMacCnt=%u mask=0x%02X remote=[%u,%u,%u,%u,%u,%u,%u,%u]",
        (unsigned)goAppData.nMacCnt, (unsigned)nMask,
        (unsigned)gnFidRemoteNum[0], (unsigned)gnFidRemoteNum[1],
        (unsigned)gnFidRemoteNum[2], (unsigned)gnFidRemoteNum[3],
        (unsigned)gnFidRemoteNum[4], (unsigned)gnFidRemoteNum[5],
        (unsigned)gnFidRemoteNum[6], (unsigned)gnFidRemoteNum[7]);

    return nMask;
}

static bool FioOspAnyFidHandleOpen(void)
{
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahFidFioOspHandle[i] != NULL || IlasLed_IsHandleOpen(i))
        {
            return true;
        }
    }

    return false;
}

static ADI_EAL_STATUS FioOspInitFid(uint32_t nFidIdx)
{
    ADI_EAL_STATUS eEalStatus;
    ADI_EAL_FIO_OSP_CFG oFioCfg;
    ADI_FIO_OSP_SETUP_REG oSetupReg;
    uint16_t nDeviceAddress = BROADCAST_DEV_ADDR;
    uint8_t nReqId = (uint8_t)(0x20U + nFidIdx);

    (void)ADI_MEMSET(&oFioCfg, 0, sizeof(oFioCfg));
    oFioCfg.nInstNum = 0U;
    oFioCfg.nRemoteNum = gnFidRemoteNum[nFidIdx];
    oFioCfg.nIntfNum = OSP_FIO_INTF_NUM;
    oFioCfg.nFioOspCfgQDepth = FIO_OSP_QUEUE_DEPTH;
    oFioCfg.nFioOspMemorySize = FIO_OSP_MEM;
    oFioCfg.pFioOspMemory = (uint8_t*)&ganFidFioOspMem[nFidIdx][0U];

    (void)ADI_MEMSET((uint8_t*)&ganFidFioOspMem[nFidIdx][0U], 0, FIO_OSP_MEM);

    {
        uint32_t nRemote = gnFidRemoteNum[nFidIdx];
        const uint8_t* pMac = (nRemote < NUM_REMOTE_NODES) ? &ganRemoteMacAddrMap[nRemote][0U] : NULL;
        if (pMac != NULL)
        {
            FioTraceAppendf("FID=0x%02X FIO cfg remote=%u intf=%u PLCA=%u MAC=%02X:%02X:%02X:%02X:%02X:%02X",
                (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
                (unsigned)oFioCfg.nIntfNum, (unsigned)(nFidIdx + 1U),
                pMac[0U], pMac[1U], pMac[2U], pMac[3U], pMac[4U], pMac[5U]);
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                "[DIAG] FID=0x%02X FIO cfg remote=%u intf=%u PLCA=%u MAC=%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
                (unsigned)oFioCfg.nIntfNum, (unsigned)(nFidIdx + 1U),
                pMac[0U], pMac[1U], pMac[2U], pMac[3U], pMac[4U], pMac[5U]);
        }
        else
        {
            FioTraceAppendf("FID=0x%02X FIO cfg remote=%u intf=%u PLCA=%u MAC=<out-of-range>",
                (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
                (unsigned)oFioCfg.nIntfNum, (unsigned)(nFidIdx + 1U));
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                "[DIAG] FID=0x%02X FIO cfg remote=%u intf=%u PLCA=%u MAC=<out-of-range>\r\n",
                (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
                (unsigned)oFioCfg.nIntfNum, (unsigned)(nFidIdx + 1U));
        }
    }

    OspStageLogf("FioOspInitFid: before Open FID=0x%02X remote=%u intf=%u", (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum, (unsigned)oFioCfg.nIntfNum);
    eEalStatus = adi_eal_fioOspOpen(&oFioCfg, &gahFidFioOspHandle[nFidIdx]);
    OspStageLogf("FioOspInitFid: after Open FID=0x%02X status=%d handle=%p", (unsigned)(nFidIdx + 1U), (int)eEalStatus, (void*)gahFidFioOspHandle[nFidIdx]);
    gaFidFioDiag[nFidIdx].nOpen = (int)eEalStatus;
    gaFidFioDiag[nFidIdx].nHandleValid = (gahFidFioOspHandle[nFidIdx] != NULL) ? 1 : 0;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
        "[DIAG] FID=0x%02X fioOspOpen remote=%u intf=%u status=%d handle=%p\r\n",
        (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum, (unsigned)oFioCfg.nIntfNum,
        (int)eEalStatus, (void*)gahFidFioOspHandle[nFidIdx]);
    FioTraceAppendf("FID=0x%02X fioOspOpen remote=%u intf=%u status=%d handle=%p",
        (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum, (unsigned)oFioCfg.nIntfNum,
        (int)eEalStatus, (void*)gahFidFioOspHandle[nFidIdx]);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

    OspStageLogf("FioOspInitFid: before RegisterCallback FID=0x%02X", (unsigned)(nFidIdx + 1U));
    eEalStatus = adi_eal_fioOspRegisterCallback(gahFidFioOspHandle[nFidIdx], EalFioOspCallBack);
    OspStageLogf("FioOspInitFid: after RegisterCallback FID=0x%02X status=%d", (unsigned)(nFidIdx + 1U), (int)eEalStatus);
    gaFidFioDiag[nFidIdx].nCallback = (int)eEalStatus;
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

    OspStageLogf("FioOspInitFid: before ConfigureInstructionRAM FID=0x%02X", (unsigned)(nFidIdx + 1U));
    eEalStatus = adi_eal_fioOspConfigureInstructionRAM(gahFidFioOspHandle[nFidIdx], ADI_EAL_FIO_OSP_CLK_2_4_MHZ);
    OspStageLogf("FioOspInitFid: after ConfigureInstructionRAM FID=0x%02X status=%d", (unsigned)(nFidIdx + 1U), (int)eEalStatus);
    gaFidFioDiag[nFidIdx].nInstRAM = (int)eEalStatus;
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

    OspStageLogf("FioOspInitFid: before ResetQueue FID=0x%02X", (unsigned)(nFidIdx + 1U));
    (void)adi_eal_fioOspResetQueue(gahFidFioOspHandle[nFidIdx]);
    OspStageLogf("FioOspInitFid: after ResetQueue FID=0x%02X", (unsigned)(nFidIdx + 1U));

    OspStageLogf("FioOspInitFid: before ResetCmd FID=0x%02X req=0x%02X", (unsigned)(nFidIdx + 1U), (unsigned)nReqId);
    eEalStatus = adi_fioOspResetCmd(gahFidFioOspHandle[nFidIdx], (uint16_t)BROADCAST_DEV_ADDR, nReqId);
    OspStageLogf("FioOspInitFid: after ResetCmd FID=0x%02X status=%d", (unsigned)(nFidIdx + 1U), (int)eEalStatus);
    gaFidFioDiag[nFidIdx].nReset = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
        "[DIAG] FID=0x%02X ResetCmd remote=%u intf=%u req=0x%02X status=%d\r\n",
        (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
        (unsigned)oFioCfg.nIntfNum, (unsigned)nReqId, (int)eEalStatus);
    FioTraceAppendf("FID=0x%02X ResetCmd remote=%u intf=%u req=0x%02X status=%d",
        (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
        (unsigned)oFioCfg.nIntfNum, (unsigned)nReqId, (int)eEalStatus);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

    OspStageLogf("FioOspInitFid: before InitBidir FID=0x%02X req=0x%02X", (unsigned)(nFidIdx + 1U), (unsigned)(uint8_t)(nReqId + 1U));
    eEalStatus = adi_fioOspInitBidir(gahFidFioOspHandle[nFidIdx], &nDeviceAddress, (uint8_t)(nReqId + 1U));
    OspStageLogf("FioOspInitFid: after InitBidir FID=0x%02X status=%d addr=%u", (unsigned)(nFidIdx + 1U), (int)eEalStatus, (unsigned)nDeviceAddress);
    gaFidFioDiag[nFidIdx].nInitBidir = (int)eEalStatus;
    gaFidFioDiag[nFidIdx].nAssignedAddr = (int)nDeviceAddress;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
        "[DIAG] FID=0x%02X InitBidir remote=%u intf=%u req=0x%02X status=%d addr=%u\r\n",
        (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
        (unsigned)oFioCfg.nIntfNum, (unsigned)(uint8_t)(nReqId + 1U),
        (int)eEalStatus, (unsigned)nDeviceAddress);
    FioTraceAppendf("FID=0x%02X InitBidir remote=%u intf=%u req=0x%02X status=%d addr=%u",
        (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
        (unsigned)oFioCfg.nIntfNum, (unsigned)(uint8_t)(nReqId + 1U),
        (int)eEalStatus, (unsigned)nDeviceAddress);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        uint8_t nRetryReq = (uint8_t)(0x80U + nFidIdx);
        FioTraceAppendf("FID=0x%02X InitBidir retry after status=%d", (unsigned)(nFidIdx + 1U), (int)eEalStatus);
        OspStageLogf("FioOspInitFid: before ResetQueue FID=0x%02X", (unsigned)(nFidIdx + 1U));
    (void)adi_eal_fioOspResetQueue(gahFidFioOspHandle[nFidIdx]);
    OspStageLogf("FioOspInitFid: after ResetQueue FID=0x%02X", (unsigned)(nFidIdx + 1U));
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 20ULL * MS_TO_NS);

        eEalStatus = adi_fioOspResetCmd(gahFidFioOspHandle[nFidIdx], (uint16_t)BROADCAST_DEV_ADDR, nRetryReq);
        gaFidFioDiag[nFidIdx].nReset = (int)eEalStatus;
        FioTraceAppendf("FID=0x%02X Retry ResetCmd remote=%u intf=%u req=0x%02X status=%d",
            (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
            (unsigned)oFioCfg.nIntfNum, (unsigned)nRetryReq, (int)eEalStatus);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
            "[DIAG] FID=0x%02X Retry ResetCmd remote=%u intf=%u req=0x%02X status=%d\r\n",
            (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
            (unsigned)oFioCfg.nIntfNum, (unsigned)nRetryReq, (int)eEalStatus);
        if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

        nDeviceAddress = BROADCAST_DEV_ADDR;
        eEalStatus = adi_fioOspInitBidir(gahFidFioOspHandle[nFidIdx], &nDeviceAddress, (uint8_t)(nRetryReq + 1U));
        gaFidFioDiag[nFidIdx].nInitBidir = (int)eEalStatus;
        gaFidFioDiag[nFidIdx].nAssignedAddr = (int)nDeviceAddress;
        FioTraceAppendf("FID=0x%02X Retry InitBidir remote=%u intf=%u req=0x%02X status=%d addr=%u",
            (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
            (unsigned)oFioCfg.nIntfNum, (unsigned)(uint8_t)(nRetryReq + 1U),
            (int)eEalStatus, (unsigned)nDeviceAddress);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
            "[DIAG] FID=0x%02X Retry InitBidir remote=%u intf=%u req=0x%02X status=%d addr=%u\r\n",
            (unsigned)(nFidIdx + 1U), (unsigned)oFioCfg.nRemoteNum,
            (unsigned)oFioCfg.nIntfNum, (unsigned)(uint8_t)(nRetryReq + 1U),
            (int)eEalStatus, (unsigned)nDeviceAddress);
    }
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

    gnFidNodeAddr[nFidIdx] = nDeviceAddress;
    if (gnOspBridgeDeviceAddress == 0U)
    {
        gnOspBridgeDeviceAddress = nDeviceAddress;
    }

    oSetupReg.ePwmFreq = ADI_FIO_OSP_PWM_FREQ_586HZ;
    oSetupReg.bClkInv = false;
    oSetupReg.bCrcEnable = true;
    oSetupReg.eTempUpdateRate = ADI_FIO_OSP_TEMP_UPDATE_RATE_2_4_KHZ;
    oSetupReg.bShouldDevSleeponComErr = false;
    oSetupReg.bShouldDevSleeponLOSErr = false;
    oSetupReg.bShouldDevSleeponOverTemperature = true;
    oSetupReg.bShouldDevSleeponUnderVoltage = false;

    OspStageLogf("FioOspInitFid: before SetSetup FID=0x%02X addr=%u req=0x%02X", (unsigned)(nFidIdx + 1U), (unsigned)nDeviceAddress, (unsigned)(uint8_t)(nReqId + 2U));
    eEalStatus = adi_fioOspSetSetup(gahFidFioOspHandle[nFidIdx], nDeviceAddress, &oSetupReg, (uint8_t)(nReqId + 2U));
    OspStageLogf("FioOspInitFid: after SetSetup FID=0x%02X status=%d", (unsigned)(nFidIdx + 1U), (int)eEalStatus);
    gaFidFioDiag[nFidIdx].nSetSetup = (int)eEalStatus;
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

    OspStageLogf("FioOspInitFid: before SetState FID=0x%02X addr=%u req=0x%02X", (unsigned)(nFidIdx + 1U), (unsigned)nDeviceAddress, (unsigned)(uint8_t)(nReqId + 3U));
    eEalStatus = adi_fioOspSetState(gahFidFioOspHandle[nFidIdx], nDeviceAddress, FIO_OSP_SET_STATE_ACTIVE, (uint8_t)(nReqId + 3U));
    OspStageLogf("FioOspInitFid: after SetState FID=0x%02X status=%d", (unsigned)(nFidIdx + 1U), (int)eEalStatus);
    gaFidFioDiag[nFidIdx].nSetState = (int)eEalStatus;
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS) { return eEalStatus; }

    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 5ULL * MS_TO_NS);
    return ADI_EAL_STATUS_SUCCESS;
}

ADI_MEM_CODE_CRIT
static void FioOspDeviceInit(void)
{
    /* ??Connect ??1?뚮쭔 ?ㅽ뻾 ??TX 寃쎈줈?먯꽌 ?ы샇異쒕릺?대룄 Init 諛섎났 諛⑹? */
    if (gbFioOspDeviceInitialized)
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FioOspDeviceInit skipped (already initialized)\r\n");
        return;
    }

    ADI_EAL_STATUS eEalStatus;

#if OSP_FIO1_ONLY_TEST
    {
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO-only test mode: ILAS disabled, init FIO nIntfNum=%u\r\n", (unsigned)OSP_FIO_INTF_NUM);

        goFioDiag.nFio0Open = -77;
        goFioDiag.nFio0Callback = -77;
        goFioDiag.nFio0InstRAM = -77;
        goFioDiag.nFio0Reset = -77;
        goFioDiag.nFio0InitBidir = -77;
        goFioDiag.nFio0AssignedAddr = 0;
        goFioDiag.nFio0SetSetup = -77;
        goFioDiag.nFio0SetState = -77;
        goFioDiag.nFio0HandleValid = 0;
        ghFioOspHandle = NULL;

        OspStageLog("FioOspDeviceInit: enter");
        FioOspResetFidDiag();
        FioTraceReset();
        IlasLed_SetTraceLineFn(FioTraceAppend);
        ghFioOspHandle2 = NULL;
        gnOspBridgeDeviceAddress = 0U;
        OspStageLog("FioOspDeviceInit: before MarkDiscoveredFids");
        (void)FioOspMarkDiscoveredFids();
        OspStageLog("FioOspDeviceInit: after MarkDiscoveredFids");

        {
            uint32_t nInitCount = 0U;
            uint32_t nIlasInitCount = 0U;
            uint32_t nFirstActiveIdx = NUM_REMOTE_NODES;

            for (uint32_t nDiscIdx = 0U; nDiscIdx < NUM_REMOTE_NODES; nDiscIdx++)
            {
                uint8_t nFid = ganRemoteMacAddrMap[nDiscIdx][5U];
                if (nFid < 0x01U || nFid > 0x08U)
                {
                    continue;
                }

                uint32_t nFidIdx = (uint32_t)(nFid - 1U);
                if (!gbAd3304Connected[nFidIdx] || gahFidFioOspHandle[nFidIdx] != NULL)
                {
                    continue;
                }
                FioTraceAppendf("FID=0x%02X FIO interface1 init sequence discIdx=%u remote=%u",
                    (unsigned)nFid, (unsigned)nDiscIdx, (unsigned)gnFidRemoteNum[nFidIdx]);

                OspStageLogf("FioOspDeviceInit: before FIO FID=0x%02X remote=%u", (unsigned)nFid, (unsigned)gnFidRemoteNum[nFidIdx]);
                eEalStatus = FioOspInitFid(nFidIdx);
                OspStageLogf("FioOspDeviceInit: after FIO FID=0x%02X status=%d", (unsigned)nFid, (int)eEalStatus);
                if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
                {
                    nInitCount++;
                    if (nFirstActiveIdx >= NUM_REMOTE_NODES)
                    {
                        nFirstActiveIdx = nFidIdx;
                        ghFioOspHandle2 = gahFidFioOspHandle[nFidIdx];
                    }
                }
                else
                {
                    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                        "[DIAG] FID=0x%02X FIO init failed status=%d\r\n",
                        (unsigned)(nFidIdx + 1U), (int)eEalStatus);
                    FioTraceAppendf("FID=0x%02X FIO init failed status=%d",
                        (unsigned)(nFidIdx + 1U), (int)eEalStatus);
                }

#if OSP_BRIDGE_DISABLE_ILAS
                FioTraceAppendf("FID=0x%02X ILAS probe disabled for FIO-only test", (unsigned)nFid);
#else
                if (ILAS_TARGET_FID_MASK == 0x00U || ((ILAS_TARGET_FID_MASK & (1U << nFidIdx)) != 0U))
                {
                    FioTraceAppendf("ILAS probe discIdx=%u -> FID=0x%02X remote=%u",
                        (unsigned)nDiscIdx, (unsigned)nFid, (unsigned)gnFidRemoteNum[nFidIdx]);

                    OspStageLogf("FioOspDeviceInit: before ILAS FID=0x%02X remote=%u", (unsigned)nFid, (unsigned)gnFidRemoteNum[nFidIdx]);
                    eEalStatus = IlasLed_InitFid(nFidIdx, gnFidRemoteNum[nFidIdx]);
                    OspStageLogf("FioOspDeviceInit: after ILAS FID=0x%02X status=%d", (unsigned)nFid, (int)eEalStatus);
                    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
                    {
                        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                            "[ILAS] FID=0x%02X init failed status=%d\r\n",
                            (unsigned)(nFidIdx + 1U), (int)eEalStatus);
                    }
                    else if (IlasLed_IsActive(nFidIdx))
                    {
                        nIlasInitCount++;
                    }
                }
                else
                {
                    FioTraceAppendf("FID=0x%02X ILAS probe skipped by mask 0x%02X",
                        (unsigned)nFid, (unsigned)ILAS_TARGET_FID_MASK);
                }
#endif
            }

            if (nFirstActiveIdx < NUM_REMOTE_NODES)
            {
                goFioDiag.nFio1Open = gaFidFioDiag[nFirstActiveIdx].nOpen;
                goFioDiag.nFio1Callback = gaFidFioDiag[nFirstActiveIdx].nCallback;
                goFioDiag.nFio1InstRAM = gaFidFioDiag[nFirstActiveIdx].nInstRAM;
                goFioDiag.nFio1Reset = gaFidFioDiag[nFirstActiveIdx].nReset;
                goFioDiag.nFio1InitBidir = gaFidFioDiag[nFirstActiveIdx].nInitBidir;
                goFioDiag.nFio1AssignedAddr = gaFidFioDiag[nFirstActiveIdx].nAssignedAddr;
                goFioDiag.nFio1SetSetup = gaFidFioDiag[nFirstActiveIdx].nSetSetup;
                goFioDiag.nFio1SetState = gaFidFioDiag[nFirstActiveIdx].nSetState;
                goFioDiag.nFio1HandleValid = gaFidFioDiag[nFirstActiveIdx].nHandleValid;
            }

            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                "[DIAG] FID LED init complete: Osram=%u ILAS=%u firstOsramFID=0x%02X\r\n",
                (unsigned)nInitCount,
                (unsigned)nIlasInitCount,
                (nFirstActiveIdx < NUM_REMOTE_NODES) ? (unsigned)(nFirstActiveIdx + 1U) : 0U);

            gbFioOspDeviceInitialized = (nInitCount > 0U || nIlasInitCount > 0U);
            if (nInitCount > 0U)
            {
                FioOspSetAllActiveFioOff();
            }

            if (nIlasInitCount > 0U)
            {
                bool bIlasOffQueued = false;
                for (uint8_t nLoopFid = 1U; nLoopFid <= 8U; nLoopFid++)
                {
                    uint32_t nLoopIdx = (uint32_t)(nLoopFid - 1U);
                    if (!IlasLed_IsHandleOpen(nLoopIdx))
                    {
                        continue;
                    }
                    int nOffStatus = IlasLed_QueueColorRgbByFid(nLoopFid, 0U, 0U, 0U);
                    FioTraceAppendf("FID=0x%02X ILAS init OFF queued status=%d", (unsigned)nLoopFid, nOffStatus);
                    if (nOffStatus == 0)
                    {
                        bIlasOffQueued = true;
                    }
                }
                if (bIlasOffQueued)
                {
                    IlasLed_FlushQueued(100ULL * MS_TO_NS);
                }
            }
/* ?? Multicast 洹몃９ ?앹꽦 (OSP_FIO1_ONLY_TEST=1 寃쎈줈) ?? */
            if (nInitCount > 0U)
            {
                uint16_t nMcastCount = 0U;
                for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++) { ganMcastHandles[i] = NULL; }
                for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
                    if (gahFidFioOspHandle[i] != NULL)
                        ganMcastHandles[nMcastCount++] = gahFidFioOspHandle[i];
                if (nMcastCount > 0U)
                {
                    (void)ADI_MEMSET(&goMcastCfg, 0, sizeof(goMcastCfg));
                    goMcastCfg.nNumInst           = nMcastCount;
                    goMcastCfg.pahFioOspHandles   = &ganMcastHandles[0U];
                    goMcastCfg.nIntfTopic         = OSP_FIO_MCAST_INTF_TOPIC;
                    goMcastCfg.nFifoTopic         = OSP_FIO_MCAST_FIFO_TOPIC;
                    goMcastCfg.poIfRegVlanTag      = NULL;
                    goMcastCfg.poFifoRegVlanTag    = NULL;
                    goMcastCfg.poFifoWriteVlanTag  = NULL;
                    goMcastCfg.nFioOspMcastMemSize = ADI_EAL_FIO_OSP_MCAST_GROUP_MEM_SIZE(nMcastCount);
                    goMcastCfg.pFioOspMcastMem     = &gnFioMcastMem[0U];
                    OspStageLogf("FioOspDeviceInit: before MCAST count=%u", (unsigned)nMcastCount);
                    ADI_EAL_STATUS eMcSt = adi_eal_fioOspCreateMcastGroup(&goMcastCfg, &ghFioMcastHandle);
                    OspStageLogf("FioOspDeviceInit: after MCAST status=%d", (int)eMcSt);
                    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                        "[MCAST] CreateMcastGroup: %u handles, status=%d, handle=%p\r\n",
                        (unsigned)nMcastCount, eMcSt, (void*)ghFioMcastHandle);
                    if (eMcSt != ADI_EAL_STATUS_SUCCESS) { ghFioMcastHandle = NULL; }
                    else { adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 20ULL * MS_TO_NS); }
                }
            }
            return;
        }

        ADI_EAL_FIO_OSP_CFG oEalFioOspCfg2;
        (void)ADI_MEMSET(&oEalFioOspCfg2, 0, sizeof(oEalFioOspCfg2));
        oEalFioOspCfg2.nInstNum = 0U;
        oEalFioOspCfg2.nRemoteNum = 0U;
        oEalFioOspCfg2.nIntfNum = 1U;
        oEalFioOspCfg2.nFioOspCfgQDepth = FIO_OSP_QUEUE_DEPTH; /* ??20??0: unicast ?곗냽 TX ???ㅻ쾭?뚮줈??諛⑹? */
        oEalFioOspCfg2.nFioOspMemorySize = FIO_OSP_MEM;
        oEalFioOspCfg2.pFioOspMemory = &gnFioOspMem2[0U];

        /* ??Open() 吏곸쟾 踰꾪띁 珥덇린??(FIO0/FIO1 ?쇰컲 寃쎈줈? ?숈씪???댁쑀) */
        (void)ADI_MEMSET(&gnFioOspMem2[0U], 0, FIO_OSP_MEM);

        eEalStatus = adi_eal_fioOspOpen(&oEalFioOspCfg2, &ghFioOspHandle2);
        goFioDiag.nFio1Open = (int)eEalStatus;
        goFioDiag.nFio1HandleValid = (ghFioOspHandle2 != NULL) ? 1 : 0;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1-only fioOspOpen       status=%d handle=%p\r\n", eEalStatus, (void*)ghFioOspHandle2);
        ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to open FIO OSP (FIO1-only)");

        eEalStatus = adi_eal_fioOspRegisterCallback(ghFioOspHandle2, EalFioOspCallBack);
        goFioDiag.nFio1Callback = (int)eEalStatus;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1-only registerCallback  status=%d\r\n", eEalStatus);
        ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to register callback for FIO OSP (FIO1-only)");

        eEalStatus = adi_eal_fioOspConfigureInstructionRAM(ghFioOspHandle2, ADI_EAL_FIO_OSP_CLK_2_4_MHZ);
        goFioDiag.nFio1InstRAM = (int)eEalStatus;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1-only configureInstRAM   status=%d\r\n", eEalStatus);
        ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to configure instruction ram for OSP (FIO1-only)");

        eEalStatus = adi_eal_fioOspResetQueue(ghFioOspHandle2);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1-only ResetQueue(force)  status=%d\r\n", eEalStatus);

        eEalStatus = adi_fioOspResetCmd(ghFioOspHandle2, (uint16_t)(BROADCAST_DEV_ADDR), 0x0U);
        goFioDiag.nFio1Reset = (int)eEalStatus;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1-only ResetCmd           status=%d\r\n", eEalStatus);
        ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send Reset command (FIO1-only)");

        /* FID蹂??쒖감 InitBidir: discovery?먯꽌 gbAd3304Connected媛 ?ㅼ젙??FID留?泥섎━
         * gnFidNodeAddr[FID-1]??遺?щ맂 ?몃뱶二쇱냼 ?????SetColorRgbByFid unicast???ъ슜 */
        {
            ADI_FIO_OSP_SETUP_REG oSetupFid;
            oSetupFid.ePwmFreq = ADI_FIO_OSP_PWM_FREQ_586HZ;
            oSetupFid.bClkInv = false;
            oSetupFid.bCrcEnable = true;
            oSetupFid.eTempUpdateRate = ADI_FIO_OSP_TEMP_UPDATE_RATE_2_4_KHZ;
            oSetupFid.bShouldDevSleeponComErr = false;
            oSetupFid.bShouldDevSleeponLOSErr = false;
            oSetupFid.bShouldDevSleeponOverTemperature = true;
            oSetupFid.bShouldDevSleeponUnderVoltage = false;

            uint32_t nInitCount = 0U;
            for (uint32_t nFidIdx = 0U; nFidIdx < 8U; nFidIdx++)
            {
                if (!gbAd3304Connected[nFidIdx]) { gnFidNodeAddr[nFidIdx] = 0U; continue; }

                uint8_t nFid = (uint8_t)(nFidIdx + 1U);  /* ?ㅼ젣 FID: 1~8 */
                uint16_t nAddr = (uint16_t)BROADCAST_DEV_ADDR;
                eEalStatus = adi_fioOspInitBidir(ghFioOspHandle2, &nAddr, 0x2U);
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                    "[DIAG] FID=0x%02X InitBidir status=%d addr=%u\r\n",
                    (unsigned)nFid, (int)eEalStatus, (unsigned)nAddr);
                if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
                { gnFidNodeAddr[nFidIdx] = 0U; continue; }

                gnFidNodeAddr[nFidIdx] = nAddr;
                if (nInitCount == 0U) { gnOspBridgeDeviceAddress = nAddr; }
                nInitCount++;

                eEalStatus = adi_fioOspSetSetup(ghFioOspHandle2, nAddr, &oSetupFid, 0x4DU);
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FID=0x%02X SetSetup status=%d\r\n",
                    (unsigned)nFid, (int)eEalStatus);
                eEalStatus = adi_fioOspSetState(ghFioOspHandle2, nAddr, FIO_OSP_SET_STATE_ACTIVE, 0x4U);
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FID=0x%02X SetState status=%d\r\n",
                    (unsigned)nFid, (int)eEalStatus);
                adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 5ULL * MS_TO_NS);
            }
            goFioDiag.nFio1InitBidir    = (nInitCount > 0U) ? 0 : -1;
            goFioDiag.nFio1AssignedAddr = (int)gnOspBridgeDeviceAddress;
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 init complete: %u FID connected\r\n",
                (unsigned)nInitCount);
        }


        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1-only init complete. FIO1=%p\r\n", (void*)ghFioOspHandle2);
        gbFioOspDeviceInitialized = true;
        return;
    }
#endif

    /* ?? FIO0 珥덇린??(IF6/IF7, SAIF6/SAIF7) ?? */
    (void)ADI_MEMSET(&goEalFioOspCfg, 0, sizeof(goEalFioOspCfg));
    goEalFioOspCfg.nInstNum = 0U;
    goEalFioOspCfg.nRemoteNum = 0U;
    goEalFioOspCfg.nIntfNum = 0U;    /* FIO0 = IF6/IF7 */
    goEalFioOspCfg.nFioOspCfgQDepth = FIO_OSP_QUEUE_DEPTH; /* ??20??0: unicast ?곗냽 TX ???ㅻ쾭?뚮줈??諛⑹? */
    goEalFioOspCfg.nFioOspMemorySize = FIO_OSP_MEM;
    goEalFioOspCfg.pFioOspMemory = &gnFioOspMem[0U];

    /* ??Open() 吏곸쟾??踰꾪띁瑜?珥덇린?뷀빐???쒕떎.
     *   Open() ?댄썑??memset?섎㈃ EAL??Open ???ㅼ젙???대? ??援ъ“泥대?
     *   ??뼱?⑥꽌 AVE(AccessViolationException)媛 諛쒖깮?쒕떎.
     *   諛섎뱶??Open() ?몄텧 ?꾩뿉 踰꾪띁瑜??대━?댄빐???댁쟾 ?몄뀡??     *   ?붾쪟 ?곹깭 ?놁씠 EAL??源⑤걮?섍쾶 珥덇린?붾맂?? */
    (void)ADI_MEMSET(&gnFioOspMem[0U], 0, FIO_OSP_MEM);

    eEalStatus = adi_eal_fioOspOpen(&goEalFioOspCfg, &ghFioOspHandle);
    goFioDiag.nFio0Open = (int)eEalStatus;
    goFioDiag.nFio0HandleValid = (ghFioOspHandle != NULL) ? 1 : 0;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 fioOspOpen       status=%d handle=%p\r\n", eEalStatus, (void*)ghFioOspHandle);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to open FIO OSP (FIO0)");

    eEalStatus = adi_eal_fioOspRegisterCallback(ghFioOspHandle, EalFioOspCallBack);
    goFioDiag.nFio0Callback = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 registerCallback  status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to register callback for FIO OSP (FIO0)");

    eEalStatus = adi_eal_fioOspConfigureInstructionRAM(ghFioOspHandle, ADI_EAL_FIO_OSP_CLK_2_4_MHZ);
    goFioDiag.nFio0InstRAM = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 configureInstRAM   status=%d\r\n", eEalStatus);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to configure instruction ram for OSP (FIO0)");

    eEalStatus = adi_eal_fioOspResetQueue(ghFioOspHandle);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 ResetQueue(force)  status=%d\r\n", eEalStatus);

    eEalStatus = adi_fioOspResetCmd(ghFioOspHandle, (uint16_t)(BROADCAST_DEV_ADDR), 0x0U);
    goFioDiag.nFio0Reset = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 ResetCmd           status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send Reset command (FIO0)");

    /* OSP Init Bidirectional ??FIO0 */
    uint16_t nDeviceAddress = BROADCAST_DEV_ADDR;
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Initialising Bidir (FIO0 / IF6-IF7)\r\n");
    eEalStatus = adi_fioOspInitBidir(ghFioOspHandle, &nDeviceAddress, 0x2U);
    goFioDiag.nFio0InitBidir = (int)eEalStatus;
    goFioDiag.nFio0AssignedAddr = (int)nDeviceAddress;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 InitBidir          status=%d assignedAddr=%d\r\n", eEalStatus, (int)nDeviceAddress);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "OSP INIT Status (FIO0) %d \r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send Init Bidirectional command (FIO0)");
    gnOspBridgeDeviceAddress = nDeviceAddress;

    /* OSP Set Register Setup ??FIO0 */
    ADI_FIO_OSP_SETUP_REG oSetupReg;
    oSetupReg.ePwmFreq = ADI_FIO_OSP_PWM_FREQ_586HZ;
    oSetupReg.bClkInv = false;
    oSetupReg.bCrcEnable = true;
    oSetupReg.eTempUpdateRate = ADI_FIO_OSP_TEMP_UPDATE_RATE_2_4_KHZ;
    oSetupReg.bShouldDevSleeponComErr = false;
    oSetupReg.bShouldDevSleeponLOSErr = false;
    oSetupReg.bShouldDevSleeponOverTemperature = true;
    oSetupReg.bShouldDevSleeponUnderVoltage = false;

    eEalStatus = adi_fioOspSetSetup(ghFioOspHandle, (uint16_t)(BROADCAST_DEV_ADDR), &oSetupReg, 0x4DU);
    goFioDiag.nFio0SetSetup = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 SetSetup           status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send setup command (FIO0)");

    eEalStatus = adi_fioOspSetState(ghFioOspHandle, nDeviceAddress, FIO_OSP_SET_STATE_ACTIVE, 0x4U);
    goFioDiag.nFio0SetState = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO0 SetState(ACTIVE)   status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send Go Active command (FIO0)");

    /* ??FIO0 紐낅졊??踰꾩뒪濡??꾩쟾???꾩넚???뚭퉴吏 ?湲?
     *   flush ?놁씠 諛붾줈 FIO1 珥덇린?붾? ?쒖옉?섎㈃ FIO0 ??SetState ?꾨젅?꾩씠
     *   ?꾩쭅 ?먯뿉 ?⑥븘 ?덈뒗 ?곹깭?먯꽌 FIO1 ??ResetCmd 媛 ?볦뿬
     *   ???ㅻ쾭?뚮줈??error 12) 媛 諛쒖깮?쒕떎. */
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 10ULL * MS_TO_NS);

    /* ?? FIO1 珥덇린??(IF4/IF5, SAIF4/SAIF5) ?? */
    ADI_EAL_FIO_OSP_CFG oEalFioOspCfg2;
    (void)ADI_MEMSET(&oEalFioOspCfg2, 0, sizeof(oEalFioOspCfg2));
    oEalFioOspCfg2.nInstNum = 0U;
    oEalFioOspCfg2.nRemoteNum = 0U;
    oEalFioOspCfg2.nIntfNum = 1U;    /* FIO1 = IF4/IF5 */
    oEalFioOspCfg2.nFioOspCfgQDepth = FIO_OSP_QUEUE_DEPTH; /* ??20??0: unicast ?곗냽 TX ???ㅻ쾭?뚮줈??諛⑹? */
    oEalFioOspCfg2.nFioOspMemorySize = FIO_OSP_MEM;
    oEalFioOspCfg2.pFioOspMemory = &gnFioOspMem2[0U];

    /* ??FIO0 ? ?숈씪: Open() 吏곸쟾??踰꾪띁 珥덇린??*/
    (void)ADI_MEMSET(&gnFioOspMem2[0U], 0, FIO_OSP_MEM);

    eEalStatus = adi_eal_fioOspOpen(&oEalFioOspCfg2, &ghFioOspHandle2);
    goFioDiag.nFio1Open = (int)eEalStatus;
    goFioDiag.nFio1HandleValid = (ghFioOspHandle2 != NULL) ? 1 : 0;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 fioOspOpen       status=%d handle=%p\r\n", eEalStatus, (void*)ghFioOspHandle2);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to open FIO OSP (FIO1)");

    eEalStatus = adi_eal_fioOspRegisterCallback(ghFioOspHandle2, EalFioOspCallBack);
    goFioDiag.nFio1Callback = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 registerCallback  status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to register callback for FIO OSP (FIO1)");

    eEalStatus = adi_eal_fioOspConfigureInstructionRAM(ghFioOspHandle2, ADI_EAL_FIO_OSP_CLK_2_4_MHZ);
    goFioDiag.nFio1InstRAM = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 configureInstRAM   status=%d\r\n", eEalStatus);
    ADI_DBG_REQUIRE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to configure instruction ram for OSP (FIO1)");

    eEalStatus = adi_eal_fioOspResetQueue(ghFioOspHandle2);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 ResetQueue(force)  status=%d\r\n", eEalStatus);

    eEalStatus = adi_fioOspResetCmd(ghFioOspHandle2, (uint16_t)(BROADCAST_DEV_ADDR), 0x0U);
    goFioDiag.nFio1Reset = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 ResetCmd           status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send Reset command (FIO1)");

    /* OSP Init Bidirectional ??FIO1 */
    uint16_t nDeviceAddress2 = BROADCAST_DEV_ADDR;
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Initialising Bidir (FIO1 / IF4-IF5)\r\n");
    eEalStatus = adi_fioOspInitBidir(ghFioOspHandle2, &nDeviceAddress2, 0x2U);
    goFioDiag.nFio1InitBidir = (int)eEalStatus;
    goFioDiag.nFio1AssignedAddr = (int)nDeviceAddress2;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 InitBidir          status=%d assignedAddr=%d\r\n", eEalStatus, (int)nDeviceAddress2);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "OSP INIT Status (FIO1) %d \r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send Init Bidirectional command (FIO1)");

    /* OSP Set Register Setup ??FIO1 */
    ADI_FIO_OSP_SETUP_REG oSetupReg2;
    oSetupReg2.ePwmFreq = ADI_FIO_OSP_PWM_FREQ_586HZ;
    oSetupReg2.bClkInv = false;
    oSetupReg2.bCrcEnable = true;
    oSetupReg2.eTempUpdateRate = ADI_FIO_OSP_TEMP_UPDATE_RATE_2_4_KHZ;
    oSetupReg2.bShouldDevSleeponComErr = false;
    oSetupReg2.bShouldDevSleeponLOSErr = false;
    oSetupReg2.bShouldDevSleeponOverTemperature = true;
    oSetupReg2.bShouldDevSleeponUnderVoltage = false;

    eEalStatus = adi_fioOspSetSetup(ghFioOspHandle2, (uint16_t)(BROADCAST_DEV_ADDR), &oSetupReg2, 0x4DU);
    goFioDiag.nFio1SetSetup = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 SetSetup           status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send setup command (FIO1)");

    eEalStatus = adi_fioOspSetState(ghFioOspHandle2, nDeviceAddress2, FIO_OSP_SET_STATE_ACTIVE, 0x4U);
    goFioDiag.nFio1SetState = (int)eEalStatus;
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FIO1 SetState(ACTIVE)   status=%d\r\n", eEalStatus);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send Go Active command (FIO1)");

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] FioOspDeviceInit complete. FIO0=%p FIO1=%p\r\n",
        (void*)ghFioOspHandle, (void*)ghFioOspHandle2);

    /* ?? Multicast 洹몃９ ?앹꽦: ?곌껐???몃뱾 臾띠뼱??FID=0x00 broadcast 1???꾩넚 ?? */
    {
        uint16_t nMcastCount = 0U;
        for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++) { ganMcastHandles[i] = NULL; }
        for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
            if (gahFidFioOspHandle[i] != NULL)
                ganMcastHandles[nMcastCount++] = gahFidFioOspHandle[i];
        if (nMcastCount > 0U)
        {
            (void)ADI_MEMSET(&goMcastCfg, 0, sizeof(goMcastCfg));
            goMcastCfg.nNumInst           = nMcastCount;
            goMcastCfg.pahFioOspHandles   = &ganMcastHandles[0U];
            goMcastCfg.nIntfTopic         = OSP_FIO_MCAST_INTF_TOPIC;
            goMcastCfg.nFifoTopic         = OSP_FIO_MCAST_FIFO_TOPIC;
            goMcastCfg.poIfRegVlanTag      = NULL;
            goMcastCfg.poFifoRegVlanTag    = NULL;
            goMcastCfg.poFifoWriteVlanTag  = NULL;
            goMcastCfg.nFioOspMcastMemSize = ADI_EAL_FIO_OSP_MCAST_GROUP_MEM_SIZE(nMcastCount);
            goMcastCfg.pFioOspMcastMem     = &gnFioMcastMem[0U];
            OspStageLogf("FioOspDeviceInit: before MCAST count=%u", (unsigned)nMcastCount);
                    ADI_EAL_STATUS eMcSt = adi_eal_fioOspCreateMcastGroup(&goMcastCfg, &ghFioMcastHandle);
                    OspStageLogf("FioOspDeviceInit: after MCAST status=%d", (int)eMcSt);
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[MCAST] CreateMcastGroup: %u handles, status=%d, handle=%p\r\n",
                (unsigned)nMcastCount, eMcSt, (void*)ghFioMcastHandle);
            if (eMcSt != ADI_EAL_STATUS_SUCCESS) { ghFioMcastHandle = NULL; }
            else { adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 20ULL * MS_TO_NS); }
        }
    }
    gbFioOspDeviceInitialized = true;
}

ADI_MEM_CODE_CRIT
static void FioOspSetPwm(void)
{
    goPwmData.nBluePwm = LED_INTENSITY * colorBlue[colorNbr];
    goPwmData.bBlueDayMode = true;
    goPwmData.nGreenPwm = LED_INTENSITY * colorGreen[colorNbr];
    goPwmData.bGreenDayMode = true;
    goPwmData.nRedPwm = LED_INTENSITY * colorRed[colorNbr];
    goPwmData.bRedDayMode = true;

    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "| R % 4d | G % 4d | B % 4d |\r\n",
        goPwmData.nRedPwm,
        goPwmData.nGreenPwm,
        goPwmData.nBluePwm);

    colorNbr++;
    if (colorNbr >= COLOR_COUNT_MAX)
        colorNbr = 0;

    ADI_EAL_STATUS eEalStatus;
    uint16_t nDeviceAddress = BROADCAST_DEV_ADDR; // Broadcast address
    eEalStatus = adi_fioOspSetPwm(ghFioOspHandle, nDeviceAddress, &goPwmData, 0xFFU);
    ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to send pwm command");
}

ADI_MEM_CODE_CRIT
static void FioOspReadPwm(void)
{
    ADI_EAL_STATUS eEalStatus;
    ADI_FIO_OSP_PWM_DATA oPwmData;
    uint16_t nDeviceAddress;
    double dTemperature;
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "-----------------------------------\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "| DEVICE |  R  |  G  |  B  | TEMP |\r\n");
    ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "-----------------------------------\r\n");

    for (uint8_t i = 1u; i < NUM_LEDS + 1u; i++)
    {
        nDeviceAddress = i;
        eEalStatus = adi_fioOspReadPwm(ghFioOspHandle, nDeviceAddress, &oPwmData, 0x4EU);
        eEalStatus = adi_fioOspReadTemp(ghFioOspHandle, nDeviceAddress, &dTemperature, 0x48u);
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "| % 4d   | % 4d| % 4d| % 4d|% 4.2f|\r\n", i, oPwmData.nRedPwm, oPwmData.nGreenPwm, oPwmData.nBluePwm, dTemperature);
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "-----------------------------------\r\n\r\n");
        ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to read pwm command");
    }
}

/*================================= CALLBACKS =================================*/
/** EAL Top level callback */
ADI_MEM_CODE_CRIT
static void EalTopCallbackFunc(uint32_t nInstNum, ADI_EAL_CBK_EVT eTopCbkEvent, const void* pArg)
{
    ADI_EAL_STATUS eEalStatus;

    switch (eTopCbkEvent)
    {
    case ADI_EAL_CBK_EVT_MAC_ADDR_RCVD:
    {
        uint8_t* panMacAddr = (uint8_t*)pArg;
        uint8_t nFid = panMacAddr[5u];

        /* ?ㅼ쐞移??ㅼ젙怨?臾닿??섍쾶 discovery ?먯꽌 ?섏떊???ㅼ젣 MAC??諛붾줈 ?깅줉 */
        if (nFid >= 0x01U && nFid <= 0x08U)
        {
            uint32_t i = (uint32_t)(nFid - 1U);
            if (!gbAd3304Connected[i])
            {
                goAppData.nMacCnt++;
            }
            (void)memcpy(&ganRemoteMacAddrMap[i][0u], panMacAddr, 6U);
            Eth10BaseT1sCfg_SetRemoteMacByIdx(i, panMacAddr);
            eEalStatus = adi_eal_writeMacAddr(nInstNum, i, panMacAddr);
            ADI_DBG_ENSURE(eEalStatus == ADI_EAL_STATUS_SUCCESS, "Failed to update the MAC address");
            gbAd3304Connected[i] = true;
        }

        /* FID = MAC 留덉?留?諛붿씠??(0x01~0x08) ???곌껐 ?곹깭 湲곕줉 */
        {
            uint8_t nFid = panMacAddr[5u];
            if (nFid >= 0x01U && nFid <= 0x08U)
            {
                gbAd3304Connected[nFid - 1U] = true;  /* 諛곗뿴 ?몃뜳?? 0~7 */
                ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                    "[AD3304] FID=0x%02X MAC=%02X:%02X:%02X:%02X:%02X:%02X ?곌껐 O\r\n",
                    (unsigned)nFid,
                    panMacAddr[0], panMacAddr[1], panMacAddr[2],
                    panMacAddr[3], panMacAddr[4], panMacAddr[5]);
            }
        }

        break;
    }

    case ADI_EAL_CBK_EVT_ERR:
    default:
    {
        ADI_DBG_ERROR();
        break;
    }
    }
}

/** EAL Node level callback */
ADI_MEM_CODE_CRIT
static void EalRemoteNodeCbk(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_NODE_CBK_EVT eRemoteCbkEvent,
    const void* pArg)
{
    switch (eRemoteCbkEvent)
    {
    case ADI_EAL_NODE_CBK_EVT_PING_RCVD:
    {
        app_utils_logPingRcvd(nRemoteNum);
        break;
    }

    case ADI_EAL_NODE_CBK_EVT_ERR:
    {
        ADI_DBG_ERROR();
        break;

    }

    case ADI_EAL_NODE_CBK_EVT_REVNUM_RCVD:
    {
        ADI_EAL_NODE_REV_NUM* poEalRevNum = (ADI_EAL_NODE_REV_NUM*)pArg;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Remote" UINT32_FORMATTER "  Major Revision Number: 0x" UINT32_HEX_FORMATTER
            ", Minor Revision Number: 0x" UINT32_HEX_FORMATTER "\r\n",
            nRemoteNum + 1U, poEalRevNum->nMajorRevNum, poEalRevNum->nMinorRevNum);
        break;
    }

    default:
    {
        break;
    }
    }
}

static void EalFioOspCallBack(uint32_t nInstNum, ADI_EAL_FIO_OSP_HANDLE hFioOspHandle, ADI_EAL_FIO_OSP_CBK_EVT eCbkEventType,
    const void* pArg)
{
    switch (eCbkEventType)
    {
    case ADI_EAL_FIO_OSP_CBK_EVT_REG_READ_RESP:
    {
        ADI_E2BCORE_REGMAP_IO_CONFIG* poRegData = (ADI_E2BCORE_REGMAP_IO_CONFIG*)pArg;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Register read response: Reg: 0x%02X, Data: 0x%04X\r\n",
            poRegData->anStartAddr, poRegData->pBuffer[0U]);
        break;
    }

    case ADI_EAL_FIO_OSP_CBK_EVT_INTF_STATUS_EVT:
    {
        EAL_FIO_OSP_DATA* poEalFioOspData = (EAL_FIO_OSP_DATA*)hFioOspHandle;
        EAL_FIO_OSP_INST_DATA* poEalFioOspInstData = poEalFioOspData->apoInstData[0U];
        ADI_EAL_FIO_OSP_CBK_EVT_INTF_STATUS_EVT_DATA* poStatsDat = (ADI_EAL_FIO_OSP_CBK_EVT_INTF_STATUS_EVT_DATA*)pArg;
        ADI_PAL_LOG(ADI_CONSOLE_NO_PROMPT, "Remote " UINT32_FORMATTER " FIO OSP interface status: \r\n",
            poEalFioOspInstData->oId.nRemoteNum + 1U);
        adi_eal_diagLogFlexibleIOStatus(poStatsDat);
        app_utils_addLineSeparator(true);
        break;
    }

    default:
    {
        break;
    }
    }
}

/* =========================================================================
 * OspBridge_InitFixed
 *
 * LINMaster_FT4222 諛⑹떇怨??숈씪?섍쾶:
 *   ganRemoteMacAddrMap ??怨좎젙 MAC(REMOTE1_MAC_ADDR)?쇰줈 誘몃━ 梨꾩슫 ?? *   CreateE2bApp() ???몄텧?⑸땲??
 *   Discovery 肄쒕갚?먯꽌 ?섏떊 MAC??洹?媛믨낵 鍮꾧탳?섏뿬 ?쇱튂?섎㈃ ?깅줉 ?깃났.
 *
 * ?깃났 ??OspBridge_GetMacNode() 濡??ㅼ젣 MAC???쎌뼱
 *   - 怨좎젙 MAC怨?媛숈쑝硫? 洹몃?濡?怨꾩냽 ?ъ슜
 *   - ?ㅻⅤ硫? OspBridge_Close() ??OspBridge_Init() (discovery 諛⑹떇) ?쇰줈 ?ъ떆?? *
 * @return 0=?깃났, -2=discovery ?ㅽ뙣, -3=FIO handle ?놁쓬
 * ========================================================================= */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_InitFixed(void)
{
    if (gbOspBridgeInitialized)
    {
        return 0;
    }

    /* ?? 湲곕낯 Power-On MAC???щ’ 0???ㅼ젙 (?덊띁?곗뒪 諛⑹떇 ?좎?) ?? */
    app_utils_mac64To8(&ganRemoteMacAddrMap[APP_REMOTE_1][0u], REMOTE1_MAC_ADDR);

    gbOspBridgeMode = true;
    OspBridgeSetError("Initializing AD3306/AD3301 OSP bridge (fixed MAC mode)");
    OspStageLog("InitFixed: before InitSystem");
    InitSystem();
    OspStageLog("InitFixed: after InitSystem / before CreateE2bAppFixed");
    CreateE2bAppFixed();  /* ??MEMSET ?앸왂 + setRemotePlcaId ?덅죱 踰꾩쟾 */
    OspStageLog("InitFixed: after CreateE2bAppFixed");
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[InitFixed] discovery nMacCnt=%u\r\n", (unsigned)goAppData.nMacCnt);
    if (goAppData.nMacCnt == 0U)
    {
        OspBridgeSetError("AD3306 remote discovery failed: no node found");
        return -2;
    }

#if OSP_FIO1_ONLY_TEST
    if (!FioOspAnyFidHandleOpen())
    {
        OspBridgeSetError("No FID FIO OSP handle was opened");
        return -3;
    }
#else
    if (ghFioOspHandle == NULL || ghFioOspHandle2 == NULL)
    {
        OspBridgeSetError("FIO OSP handle was not opened");
        return -3;
    }
#endif

    /* ??HouseKeeping/AppThread ?앹꽦 ??Close() ???ъ뿰寃????ㅼ?以꾨윭 而⑦뀓?ㅽ듃媛
     *   ?뚮㈇?섎?濡?諛섎뱶???ъ깮?깊빐???쒕떎.
     *   ?꾨씫 ??adi_network_run()???몄텧?섏? ?딆븘 ?꾨젅???먭? flush?섏? ?딄퀬
     *   OSAL mutex ?묎렐 ??AccessViolationException??諛쒖깮?쒕떎. */
    OspStageLog("InitFixed: before CreateThreads");
    CreateThreads();
    OspStageLog("InitFixed: after CreateThreads");

    goAppData.eAppMode = APP_MODE_RUN;
    gbOspBridgeInitialized = true;
    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_InitOtp(void)
{
    if (gbOspBridgeInitialized)
    {
        return 0;
    }

    gbOspBridgeMode = true;
    OspBridgeSetError("Initializing AD3306/AD3301 OSP bridge (OTP MAC mode)");
    InitSystem();
    CreateE2bAppProgrammed();
    if (goAppData.nMacCnt == 0U)
    {
        OspBridgeSetError("AD3306 remote discovery failed in OTP MAC mode");
        return -2;
    }

#if OSP_FIO1_ONLY_TEST
    if (!FioOspAnyFidHandleOpen())
    {
        OspBridgeSetError("No FID FIO OSP handle was opened");
        return -3;
    }
#else
    if (ghFioOspHandle == NULL || ghFioOspHandle2 == NULL)
    {
        OspBridgeSetError("FIO OSP handle was not opened");
        return -3;
    }
#endif

    OspStageLog("InitFixed: before CreateThreads");
    CreateThreads();
    OspStageLog("InitFixed: after CreateThreads");

    goAppData.eAppMode = APP_MODE_RUN;
    gbOspBridgeInitialized = true;
    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_Init(void)
{
    if (gbOspBridgeInitialized)
    {
        return 0;
    }

    gbOspBridgeMode = true;
    OspBridgeSetError("Initializing AD3306/AD3301 OSP bridge");
    InitSystem();
    CreateE2bApp();
    if (goAppData.nMacCnt == 0U)
    {
        OspBridgeSetError("AD3306 remote discovery failed");
        return -2;
    }

#if OSP_FIO1_ONLY_TEST
    if (!FioOspAnyFidHandleOpen())
    {
        OspBridgeSetError("No FID FIO OSP handle was opened");
        return -3;
    }
#else
    if (ghFioOspHandle == NULL || ghFioOspHandle2 == NULL)
    {
        OspBridgeSetError("FIO OSP handle was not opened");
        return -3;
    }
#endif

    /* ??OspBridge_Init() ???숈씪?섍쾶 CreateThreads() ?꾩슂 */
    OspStageLog("InitFixed: before CreateThreads");
    CreateThreads();
    OspStageLog("InitFixed: after CreateThreads");

    goAppData.eAppMode = APP_MODE_RUN;
    gbOspBridgeInitialized = true;
    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetColorRgb(uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    /* 湲곕낯 ?숈옉: FIO0 (IF6/IF7, FID 0x02) 梨꾨꼸濡??꾩넚 */
    return OspBridge_SetColorRgbByFid_Impl(0x02U, nRed, nGreen, nBlue);
}

/* =========================================================================
 * OspBridge_SetColorRgbByFid  (DLL export ??C# DllImport EntryPoint ?쇱튂)
 * OspBridge_SetColorRgbByFid_Impl (?대? 援ы쁽 ??forward declaration ?댁냼)
 *
 * FID 留ㅽ븨:
 *   0x03 ??IF4/IF5 (FIO1, SAIF4/SAIF5) ??ghFioOspHandle2
 *   0x04 ??IF6/IF7 (FIO0, SAIF6/SAIF7) ??ghFioOspHandle  (湲곕낯媛?
 * ========================================================================= */
static int OspBridge_SetColorRgbByFid_Impl(uint8_t nFid, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    ADI_EAL_FIO_OSP_HANDLE hHandle = NULL;
    bool bAnySuccess = false;
    int nLastStatus = -1;
    uint16_t nLedTarget = (uint16_t)BROADCAST_DEV_ADDR;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }

    FioOspFillPwmData(&goPwmData, nRed, nGreen, nBlue);

    FioTraceAppendf("SetColorRgbByFid entry FID=0x%02X RGB=(%u,%u,%u) PWM=(%u,%u,%u)",
        (unsigned)nFid, (unsigned)nRed, (unsigned)nGreen, (unsigned)nBlue,
        (unsigned)goPwmData.nRedPwm, (unsigned)goPwmData.nGreenPwm, (unsigned)goPwmData.nBluePwm);

    if (nFid == 0x00U)
    {
        bool bFioQueued = false;
        bool bIlasQueued = false;
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
        for (uint8_t nLoopFid = 1U; nLoopFid <= 8U; nLoopFid++)
        {
            int nIlasStatus = IlasLed_QueueColorRgbByFid(nLoopFid, nRed, nGreen, nBlue);
            if (nIlasStatus == 0) { bAnySuccess = true; bIlasQueued = true; }
            else { nLastStatus = nIlasStatus; }
        }

        for (uint8_t nLoopFid = 1U; nLoopFid <= 8U; nLoopFid++)
        {
            uint32_t nFidIdx = (uint32_t)(nLoopFid - 1U);
            if (!gbAd3304Connected[nFidIdx] || gahFidFioOspHandle[nFidIdx] == NULL)
            {
                continue;
            }

            ADI_EAL_STATUS eFioStatus = adi_fioOspSetPwm(
                gahFidFioOspHandle[nFidIdx], nLedTarget, &goPwmData, 0xFFU);
            if (eFioStatus == ADI_EAL_STATUS_SUCCESS)
            {
                bFioQueued = true;
            }
            nLastStatus = (int)eFioStatus;
            FioTraceAppendf("OSRAM broadcast SetPwm FID=0x%02X status=%d target=%u handle=%p",
                (unsigned)nLoopFid, (int)eFioStatus, (unsigned)nLedTarget,
                (void*)gahFidFioOspHandle[nFidIdx]);
            ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
                "[OSRAM] Broadcast SetPwm FID=0x%02X status=%d target=%u RGB=(%u,%u,%u)\r\n",
                (unsigned)nLoopFid, (int)eFioStatus, (unsigned)nLedTarget,
                (unsigned)nRed, (unsigned)nGreen, (unsigned)nBlue);
            if (eFioStatus == ADI_EAL_STATUS_SUCCESS) { bAnySuccess = true; }
        }
        if (bFioQueued || bIlasQueued)
        {
            adi_network_flushTxTimed(40ULL * MS_TO_NS);
            adi_network_run();
            adi_network_flushTxTimed(10ULL * MS_TO_NS);
            adi_network_run();
        }

        if (!bAnySuccess)
        {
            OspBridgeSetError("FID=0x00 no Osram/ILAS LED handle ready");
            return nLastStatus;
        }
        OspBridgeSetError("OK");
        return 0;
    }

    if (nFid < 0x01U || nFid > 0x08U ||
        !gbAd3304Connected[nFid - 1U] ||
        (gahFidFioOspHandle[nFid - 1U] == NULL && !IlasLed_IsHandleOpen((uint32_t)(nFid - 1U))))
    {
        char szE[64];
        (void)snprintf(szE, sizeof(szE), "FID=0x%02X no active Osram/ILAS path", (unsigned)nFid);
        OspBridgeSetError(szE);
        return -1;
    }

    hHandle = gahFidFioOspHandle[nFid - 1U];
    if (hHandle != NULL)
    {
        uint32_t nFidIdx = (uint32_t)(nFid - 1U);
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
        ADI_EAL_STATUS eEalStatus = adi_fioOspSetPwm(hHandle, nLedTarget, &goPwmData, 0xFFU);
        if (eEalStatus == ADI_EAL_STATUS_SUCCESS)
        {
            adi_network_flushTxTimed(80ULL * MS_TO_NS);
            adi_network_run();
            adi_network_flushTxTimed(20ULL * MS_TO_NS);
            adi_network_run();
        }
        nLastStatus = (int)eEalStatus;
        FioTraceAppendf("OSRAM SetPwm FID=0x%02X status=%d target=%u nodeAddr=%u handle=%p",
            (unsigned)nFid, (int)eEalStatus, (unsigned)nLedTarget,
            (unsigned)gnFidNodeAddr[nFidIdx], (void*)hHandle);
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
            "[OSRAM] SetPwm FID=0x%02X status=%d target=%u RGB=(%u,%u,%u)\r\n",
            (unsigned)nFid, (int)eEalStatus, (unsigned)nLedTarget,
            (unsigned)nRed, (unsigned)nGreen, (unsigned)nBlue);
        if (eEalStatus == ADI_EAL_STATUS_SUCCESS) { bAnySuccess = true; }
    }

    {
        int nIlasStatus = IlasLed_SetColorRgbByFid(nFid, nRed, nGreen, nBlue);
        if (nIlasStatus == 0) { bAnySuccess = true; }
        else { nLastStatus = nIlasStatus; }
    }

    if (!bAnySuccess)
    {
        OspBridgeSetError("SetColorRgb failed: no active Osram/ILAS path");
        return nLastStatus;
    }

    OspBridgeSetError("OK");
    return 0;

    /* FID=0x00 ??multicast ?몃뱾濡?1??broadcast (紐⑤뱺 ?곌껐 AD3304 ?숈떆) */
    if (nFid == 0x00U)
    {
        if (ghFioMcastHandle == NULL)
        {
            OspBridgeSetError("FID=0x00 multicast handle not ready"); return -1;
        }
        goPwmData.nRedPwm       = (uint16_t)((((uint32_t)nRed)   * 0x7FFFU + 127U) / 255U);
        goPwmData.bRedDayMode   = true;
        goPwmData.nGreenPwm     = (uint16_t)((((uint32_t)nGreen) * 0x7FFFU + 127U) / 255U);
        goPwmData.bGreenDayMode = true;
        goPwmData.nBluePwm      = (uint16_t)((((uint32_t)nBlue)  * 0x7FFFU + 127U) / 255U);
        goPwmData.bBlueDayMode  = true;
        ADI_EAL_STATUS eMcSt = adi_fioOspSetPwm(ghFioMcastHandle, (uint16_t)BROADCAST_DEV_ADDR, &goPwmData, 0xFFU);
        if (eMcSt != ADI_EAL_STATUS_SUCCESS)
        { OspBridgeSetError("FID=0x00 multicast SetPwm failed"); return (int)eMcSt; }
        OspBridgeSetError("OK"); return 0;
    }

#if OSP_FIO1_ONLY_TEST
    /* FID ?좏슚 踰붿쐞: 0x01~0x08, 諛곗뿴 ?몃뜳??= FID-1 */
    if (nFid < 0x01U || nFid > 0x08U ||
        !gbAd3304Connected[nFid - 1U] ||
        gahFidFioOspHandle[nFid - 1U] == NULL)
    {
        char szE[48]; (void)snprintf(szE, sizeof(szE), "FID=0x%02X not connected", (unsigned)nFid);
        OspBridgeSetError(szE); return -1;
    }
    hHandle = gahFidFioOspHandle[nFid - 1U];
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[OSP AD3304 FID=0x%02X] RGB=(%d,%d,%d) remote=%u node=%u handle=%p\r\n",
        (unsigned)nFid, (int)nRed, (int)nGreen, (int)nBlue,
        (unsigned)(nFid - 1U), (unsigned)gnFidNodeAddr[nFid - 1U], (void*)hHandle);
#else
    /* FID???곕씪 梨꾨꼸(FIO ?몃뱾) ?좏깮
     *   0x01 ??IF4/IF5 (FIO1, ghFioOspHandle2)
     *   0x02 ??IF6/IF7 (FIO0, ghFioOspHandle)
     */
    if (nFid == 0x01U)
    {
        /* IF4/IF5 梨꾨꼸 (FIO1) */
        if (ghFioOspHandle2 == NULL)
        {
            OspBridgeSetError("FIO1 handle not open (IF4/IF5)");
            return -1;
        }
        hHandle = ghFioOspHandle2;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] SetColorRgb FID=0x01 -> FIO1 handle=%p RGB=(%d,%d,%d)\r\n",
            (void*)hHandle, (int)nRed, (int)nGreen, (int)nBlue);
    }
    else if (nFid == 0x02U)
    {
        /* IF6/IF7 梨꾨꼸 (FIO0) */
        if (ghFioOspHandle == NULL)
        {
            OspBridgeSetError("FIO0 handle not open (IF6/IF7)");
            return -1;
        }
        hHandle = ghFioOspHandle;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] SetColorRgb FID=0x02 -> FIO0 handle=%p RGB=(%d,%d,%d)\r\n",
            (void*)hHandle, (int)nRed, (int)nGreen, (int)nBlue);
    }
    else
    {
        OspBridgeSetError("Unsupported FID (supported: 0x01=IF4/IF5, 0x02=IF6/IF7)");
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] SetColorRgb FID=0x%02X -> unsupported FID\r\n", (unsigned)nFid);
        return -1;
    }
#endif

    FioOspFillPwmData(&goPwmData, nRed, nGreen, nBlue);

    /* FID 1~8 ??諛곗뿴 ?몃뜳??0~7 */
    uint32_t nFidIdx = (nFid >= 1U && nFid <= 8U) ? (uint32_t)(nFid - 1U) : 8U;
    uint16_t nTarget = (nFidIdx < 8U && gnFidNodeAddr[nFidIdx] != 0U)
                       ? gnFidNodeAddr[nFidIdx] : (uint16_t)BROADCAST_DEV_ADDR;
    ADI_EAL_STATUS eEalStatus = adi_fioOspSetPwm(hHandle, nTarget, &goPwmData, 0xFFU);
    ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "[DIAG] SetPwm FID=0x%02X status=%d (R_pwm=%d G_pwm=%d B_pwm=%d)\r\n",
        (unsigned)nFid, eEalStatus,
        (int)goPwmData.nRedPwm, (int)goPwmData.nGreenPwm, (int)goPwmData.nBluePwm);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_fioOspSetPwm failed");
        return (int)eEalStatus;
    }

    OspBridgeSetError("OK");
    return 0;
}

/* DLL export ?섑띁 ??C# DllImport EntryPoint "OspBridge_SetColorRgbByFid" ? ?쇱튂 */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetColorRgbByFid(uint8_t nFid, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    return OspBridge_SetColorRgbByFid_Impl(nFid, nRed, nGreen, nBlue);
}

/* =========================================================================
 * OspBridge_SetColorRgbByNode_Impl / OspBridge_SetColorRgbByNode
 *
 * SlaveFlag 湲곕컲 ?⑥씪 LED(?몃뱶) unicast ?쒖뼱.
 * OSP ?꾨줈?좎퐳?먯꽌 unicast 二쇱냼(0x001~0x3EF)濡?泥댁씤 ???뱀젙 ?몃뱶留?吏??
 *
 * ?뚮씪誘명꽣:
 *   nFid      ??梨꾨꼸 ?좏깮 (0x03=IF4/IF5, 0x04=IF6/IF7)
 *   nNodeAddr ??OSP ?몃뱶 二쇱냼 (1-based: 1=泥?踰덉㎏ LED, 2=??踰덉㎏ LED, ...)
 *               0 ???꾨떖?섎㈃ broadcast (紐⑤뱺 ?몃뱶???숈씪 ?됱긽 ?꾩넚)
 *   nRed/nGreen/nBlue ??8鍮꾪듃 ?됱긽媛?(0~255)
 *
 * 諛섑솚: 0=?깃났, ?뚯닔=?ㅻ쪟
 * ========================================================================= */
static int OspBridge_SetColorRgbByNode_Impl(uint8_t nFid, uint16_t nNodeAddr,
    uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    return OspBridge_SetColorRgbByNodeEx_Impl(nFid, nNodeAddr, nRed, nGreen, nBlue, true);
}

static int OspBridge_SetColorRgbByNodeEx_Impl(uint8_t nFid, uint16_t nNodeAddr,
    uint8_t nRed, uint8_t nGreen, uint8_t nBlue, bool bFlush)
{
    ADI_EAL_FIO_OSP_HANDLE hHandle = NULL;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }

#if OSP_FIO1_ONLY_TEST
    if (nFid == 0x00U)
    {
        if (ghFioMcastHandle == NULL)
        {
            OspBridgeSetError("FID=0x00 multicast handle not ready");
            return -1;
        }
        hHandle = ghFioMcastHandle;
    }
    else if (nFid < 0x01U || nFid > 0x08U ||
        !gbAd3304Connected[nFid - 1U] ||
        gahFidFioOspHandle[nFid - 1U] == NULL)
    {
        char szE[48]; (void)snprintf(szE, sizeof(szE), "FID=0x%02X not connected", (unsigned)nFid);
        OspBridgeSetError(szE);
        return -1;
    }
    else
    {
        hHandle = gahFidFioOspHandle[nFid - 1U];
    }
#else
    /* FID???곕씪 梨꾨꼸(FIO ?몃뱾) ?좏깮
     *   0x01 ??IF4/IF5 (FIO1, ghFioOspHandle2)
     *   0x02 ??IF6/IF7 (FIO0, ghFioOspHandle)
     */
    if (nFid == 0x01U)
    {
        if (ghFioOspHandle2 == NULL)
        {
            OspBridgeSetError("FIO1 handle not open (IF4/IF5)");
            return -1;
        }
        hHandle = ghFioOspHandle2;
    }
    else if (nFid == 0x02U)
    {
        if (ghFioOspHandle == NULL)
        {
            OspBridgeSetError("FIO0 handle not open (IF6/IF7)");
            return -1;
        }
        hHandle = ghFioOspHandle;
    }
    else
    {
        OspBridgeSetError("Unsupported FID (supported: 0x01=IF4/IF5, 0x02=IF6/IF7)");
        return -1;
    }
#endif

    /* nNodeAddr == 0 ?대㈃ broadcast, 洹??몄뿉??unicast */
    uint16_t nTarget = (nNodeAddr == 0U) ? (uint16_t)BROADCAST_DEV_ADDR : nNodeAddr;

    ADI_FIO_OSP_PWM_DATA oPwmData;
    FioOspFillPwmData(&oPwmData, nRed, nGreen, nBlue);

    ADI_PAL_LOG(ADI_CONSOLE_PROMPT,
        "[DIAG] SetColorRgbByNode FID=0x%02X node=%u RGB=(%d,%d,%d)\\r\\n",
        (unsigned)nFid, (unsigned)nTarget,
        (int)nRed, (int)nGreen, (int)nBlue);

    /* unicast SET_PWM: nId=0xFF ??write-only (?묐떟 ?놁쓬, 鍮좊Ⅸ ?꾩넚) */
    ADI_EAL_STATUS eEalStatus = adi_fioOspSetPwm(hHandle, nTarget, &oPwmData, 0xFFU);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_fioOspSetPwm (unicast) failed");
        return (int)eEalStatus;
    }

    /* unicast TX ??flush ???湲??놁씠 利됱떆 ?꾩넚(0ULL).
     * OspBridge 紐⑤뱶?먯꽌??AppThread2(FioOspReadPwm) ?먮룞 猷⑦봽媛 鍮꾪솢?깊솕?섎?濡?     * READ_PWM ?꾨젅?꾩씠 ?먯뿉 ?욎씠吏 ?딅뒗??
     * LED 1~?섍컻 ?쒖뼱 ?????ㅻ쾭?뚮줈???곕젮 ?놁쓬. */
    if (bFlush)
    {
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
    }

    OspBridgeSetError("OK");
    return 0;
}

/* DLL export ??C# DllImport EntryPoint "OspBridge_SetColorRgbByNode" */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetColorRgbByNode(uint8_t nFid, uint16_t nNodeAddr,
    uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    uint16_t anNode[1U] = { nNodeAddr };
    uint8_t anRed[1U] = { nRed };
    uint8_t anGreen[1U] = { nGreen };
    uint8_t anBlue[1U] = { nBlue };
    bool bAnySuccess = false;
    bool bQueued = false;
    int nLastStatus = -1;

    if (nFid >= 0x01U && nFid <= 0x08U && IlasLed_IsActive((uint32_t)(nFid - 1U)))
    {
        int nStatus = IlasLed_QueueColorRgbNodesByFid(nFid, anNode, anRed, anGreen, anBlue, 1U);
        if (nStatus == 0) { bAnySuccess = true; bQueued = true; }
        else { nLastStatus = nStatus; }
    }

    if (nFid == 0x00U ||
        (nFid >= 0x01U && nFid <= 0x08U && gahFidFioOspHandle[nFid - 1U] != NULL))
    {
        int nStatus = OspBridge_SetColorRgbByNodeEx_Impl(nFid, nNodeAddr, nRed, nGreen, nBlue, false);
        if (nStatus == 0) { bAnySuccess = true; bQueued = true; }
        else { nLastStatus = nStatus; }
    }

    if (bQueued)
    {
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
        adi_network_flushTxTimed(OSP_RUNTIME_TX_FLUSH_TIMEOUT_MS * MS_TO_NS);
        adi_network_run();
    }

    OspBridgeSetError(bAnySuccess ? "OK" : "SetColorRgbByNode failed: no active Osram/ILAS path");
    return bAnySuccess ? 0 : nLastStatus;
}
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetColorRgbByNodes(uint8_t nFid, const uint16_t* pnNodeAddrs,
    const uint8_t* pnRed, const uint8_t* pnGreen, const uint8_t* pnBlue, uint16_t nCount)
{
    bool bAnySuccess = false;
    bool bQueued = false;
    int nLastStatus = -1;

    if (pnNodeAddrs == NULL || pnRed == NULL || pnGreen == NULL || pnBlue == NULL)
    {
        OspBridgeSetError("OspBridge_SetColorRgbByNodes null buffer");
        return -1;
    }
    if (nCount == 0U)
    {
        OspBridgeSetError("OK");
        return 0;
    }

    if (nFid >= 0x01U && nFid <= 0x08U && IlasLed_IsActive((uint32_t)(nFid - 1U)))
    {
        int nStatus = IlasLed_QueueColorRgbNodesByFid(nFid, pnNodeAddrs, pnRed, pnGreen, pnBlue, nCount);
        if (nStatus == 0) { bAnySuccess = true; bQueued = true; }
        else { nLastStatus = nStatus; }
    }

    if (nFid == 0x00U ||
        (nFid >= 0x01U && nFid <= 0x08U && gahFidFioOspHandle[nFid - 1U] != NULL))
    {
        for (uint16_t i = 0U; i < nCount; i++)
        {
            int nStatus = OspBridge_SetColorRgbByNodeEx_Impl(nFid, pnNodeAddrs[i], pnRed[i], pnGreen[i], pnBlue[i], false);
            if (nStatus == 0) { bAnySuccess = true; bQueued = true; }
            else { nLastStatus = nStatus; }
        }
    }

    if (bQueued)
    {
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
        adi_network_flushTxTimed(OSP_RUNTIME_TX_FLUSH_TIMEOUT_MS * MS_TO_NS);
        adi_network_run();
    }

    OspBridgeSetError(bAnySuccess ? "OK" : "SetColorRgbByNodes failed: no active Osram/ILAS path");
    return bAnySuccess ? 0 : nLastStatus;
}
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetFioColorRgbByNodes(uint8_t nFid, const uint16_t* pnNodeAddrs,
    const uint8_t* pnRed, const uint8_t* pnGreen, const uint8_t* pnBlue, uint16_t nCount)
{
    bool bAnySuccess = false;
    int nLastStatus = -1;

    if (pnNodeAddrs == NULL || pnRed == NULL || pnGreen == NULL || pnBlue == NULL)
    {
        OspBridgeSetError("OspBridge_SetFioColorRgbByNodes null buffer");
        return -1;
    }
    if (nCount == 0U)
    {
        OspBridgeSetError("OK");
        return 0;
    }

    if (nFid != 0x00U && (nFid < 0x01U || nFid > 0x08U ||
        gahFidFioOspHandle[nFid - 1U] == NULL))
    {
        OspBridgeSetError("FIO-only target not active");
        return -1;
    }

    if (nFid == 0x00U)
    {
        /* Keep FID=0x00 FIO ordering identical to the original FIO-only project:
         * use the multicast FIO handle once per selected OSP device address instead
         * of expanding the request into per-FID handles. Expanding here changes the
         * physical SF order on multi-remote setups. */
        for (uint16_t i = 0U; i < nCount; i++)
        {
            int nStatus = OspBridge_SetColorRgbByNodeEx_Impl(0x00U, pnNodeAddrs[i], pnRed[i], pnGreen[i], pnBlue[i], false);
            if (nStatus != 0)
            {
                OspBridgeSetError("FIO-only multicast SetColorRgbByNodes failed");
                return nStatus;
            }
            bAnySuccess = true;
        }
    }
    else
    {
        for (uint16_t i = 0U; i < nCount; i++)
        {
            int nStatus = OspBridge_SetColorRgbByNodeEx_Impl(nFid, pnNodeAddrs[i], pnRed[i], pnGreen[i], pnBlue[i], false);
            if (nStatus != 0)
            {
                OspBridgeSetError("FIO-only SetColorRgbByNodes failed");
                return nStatus;
            }
            bAnySuccess = true;
        }
    }

    if (bAnySuccess)
    {
        adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);
    }

    if (!bAnySuccess)
    {
        OspBridgeSetError("FIO-only SetColorRgbByNodes failed: no active FIO handle");
        return nLastStatus;
    }

    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetIlasColorRgbByNodes(uint8_t nFid, const uint16_t* pnNodeAddrs,
    const uint8_t* pnRed, const uint8_t* pnGreen, const uint8_t* pnBlue, uint16_t nCount)
{
#if OSP_BRIDGE_DISABLE_ILAS
    (void)nFid;
    (void)pnNodeAddrs;
    (void)pnRed;
    (void)pnGreen;
    (void)pnBlue;
    (void)nCount;
    OspBridgeSetError("ILAS disabled for FIO-only test");
    return -1;
#else
    bool bAnySuccess = false;
    int nLastStatus = -1;

    if (pnNodeAddrs == NULL || pnRed == NULL || pnGreen == NULL || pnBlue == NULL)
    {
        OspBridgeSetError("OspBridge_SetIlasColorRgbByNodes null buffer");
        return -1;
    }
    if (nCount == 0U)
    {
        OspBridgeSetError("OK");
        return 0;
    }

    if (nFid == 0x00U)
    {
        for (uint8_t nLoopFid = 1U; nLoopFid <= 8U; nLoopFid++)
        {
            if (!IlasLed_IsActive((uint32_t)(nLoopFid - 1U)))
            {
                continue;
            }
            int nStatus = IlasLed_QueueColorRgbNodesByFid(nLoopFid, pnNodeAddrs, pnRed, pnGreen, pnBlue, nCount);
            if (nStatus == 0) { bAnySuccess = true; }
            else { nLastStatus = nStatus; }
        }
    }
    else if (nFid >= 0x01U && nFid <= 0x08U && IlasLed_IsActive((uint32_t)(nFid - 1U)))
    {
        int nStatus = IlasLed_QueueColorRgbNodesByFid(nFid, pnNodeAddrs, pnRed, pnGreen, pnBlue, nCount);
        if (nStatus == 0) { bAnySuccess = true; }
        else { nLastStatus = nStatus; }
    }
    else
    {
        OspBridgeSetError("ILAS-only target not active");
        return -1;
    }

    if (bAnySuccess)
    {
        IlasLed_FlushQueued(OSP_RUNTIME_TX_FLUSH_TIMEOUT_MS * MS_TO_NS);
    }

    OspBridgeSetError(bAnySuccess ? "OK" : "ILAS-only SetColorRgbByNodes failed");
    return bAnySuccess ? 0 : nLastStatus;
#endif
}
OSP_BRIDGE_API uint16_t OSP_BRIDGE_CALL OspBridge_GetIlasLedCount(void)
{
#if OSP_BRIDGE_DISABLE_ILAS
    return 0U;
#else
    return IlasLed_GetActiveCount();
#endif
}
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_Readback(uint16_t* pnRedPwm, uint16_t* pnGreenPwm, uint16_t* pnBluePwm,
    double* pdTemperature)
{
    ADI_EAL_FIO_OSP_HANDLE hReadHandle =
#if OSP_FIO1_ONLY_TEST
        ghFioOspHandle2;
#else
        ghFioOspHandle;
#endif

    if (!gbOspBridgeInitialized || hReadHandle == NULL)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }

    if (pnRedPwm == NULL || pnGreenPwm == NULL || pnBluePwm == NULL || pdTemperature == NULL)
    {
        OspBridgeSetError("Invalid readback pointer");
        return -2;
    }

    ADI_FIO_OSP_PWM_DATA oPwmData;
    (void)ADI_MEMSET(&oPwmData, 0, sizeof(oPwmData));

    /* ??SetPwm ?꾩넚 ?꾨즺 ??ReadPwm ?붿껌 (???ㅻ쾭?뚮줈??諛⑹?) */
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 20ULL * MS_TO_NS);

    ADI_EAL_STATUS eEalStatus = adi_fioOspReadPwm(hReadHandle, gnOspBridgeDeviceAddress, &oPwmData, 0x4EU);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_fioOspReadPwm failed");
        return (int)eEalStatus;
    }

    double dTemperature = 0.0;
    eEalStatus = adi_fioOspReadTemp(hReadHandle, gnOspBridgeDeviceAddress, &dTemperature, 0x48U);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_fioOspReadTemp failed");
        return (int)eEalStatus;
    }

    *pnRedPwm = oPwmData.nRedPwm;
    *pnGreenPwm = oPwmData.nGreenPwm;
    *pnBluePwm = oPwmData.nBluePwm;
    *pdTemperature = dTemperature;

    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ReadbackByFid(uint8_t nFid, uint16_t nNodeAddr,
    uint16_t* pnRedPwm, uint16_t* pnGreenPwm, uint16_t* pnBluePwm, double* pdTemperature)
{
    ADI_EAL_FIO_OSP_HANDLE hHandle;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }

    if (pnRedPwm == NULL || pnGreenPwm == NULL || pnBluePwm == NULL || pdTemperature == NULL)
    {
        OspBridgeSetError("Invalid readback pointer");
        return -2;
    }

#if OSP_FIO1_ONLY_TEST
    if (nFid < 0x01U || nFid > 0x08U ||
        !gbAd3304Connected[nFid - 1U] ||
        gahFidFioOspHandle[nFid - 1U] == NULL)
    {
        char szE[48]; (void)snprintf(szE, sizeof(szE), "FID=0x%02X not connected", (unsigned)nFid);
        OspBridgeSetError(szE);
        return -3;
    }
    hHandle = gahFidFioOspHandle[nFid - 1U];
#else
    if (nFid == 0x01U)
    {
        if (ghFioOspHandle2 == NULL)
        {
            OspBridgeSetError("FIO1 handle not open (IF4/IF5)");
            return -3;
        }
        hHandle = ghFioOspHandle2;
    }
    else if (nFid == 0x02U)
    {
        if (ghFioOspHandle == NULL)
        {
            OspBridgeSetError("FIO0 handle not open (IF6/IF7)");
            return -4;
        }
        hHandle = ghFioOspHandle;
    }
    else
    {
        OspBridgeSetError("Unsupported FID (supported: 0x01=IF4/IF5, 0x02=IF6/IF7)");
        return -5;
    }
#endif

    uint16_t nTarget =
#if OSP_FIO1_ONLY_TEST
        (nNodeAddr == 0U) ? gnFidNodeAddr[nFid - 1U] : nNodeAddr;
#else
        (nNodeAddr == 0U) ? gnOspBridgeDeviceAddress : nNodeAddr;
#endif
    if (nTarget == 0U)
    {
        nTarget = 1U;
    }

    /* ??SetPwm(broadcast) ??ReadPwm(unicast) ?ъ씠?????ㅻ쾭?뚮줈??諛⑹?.
     *   SetPwm ?꾨젅?꾩씠 ?꾩쭅 TX ?먯뿉 ?⑥븘?덈뒗 ?곹깭?먯꽌
     *   ReadPwm ?붿껌??enqueue?섎㈃ ?먭? 苑?李?error 12 媛 諛쒖깮?쒕떎.
     *   flush濡?SetPwm ?꾩넚???꾨즺?쒗궓 ??ReadPwm???ｌ뼱???쒕떎.
     *   [?섏젙] 12?몃뱶 unicast ?곗냽 TX媛 紐⑤몢 ?꾨즺??異⑸텇???쒓컙?쇰줈 利앷? (20??00ms) */
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 100ULL * MS_TO_NS);

    ADI_FIO_OSP_PWM_DATA oPwmData;
    (void)ADI_MEMSET(&oPwmData, 0, sizeof(oPwmData));

    ADI_EAL_STATUS eEalStatus = adi_fioOspReadPwm(hHandle, nTarget, &oPwmData, 0x4EU);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_fioOspReadPwm failed");
        return (int)eEalStatus;
    }

    double dTemperature = 0.0;
    eEalStatus = adi_fioOspReadTemp(hHandle, nTarget, &dTemperature, 0x48U);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_fioOspReadTemp failed");
        return (int)eEalStatus;
    }

    *pnRedPwm = oPwmData.nRedPwm;
    *pnGreenPwm = oPwmData.nGreenPwm;
    *pnBluePwm = oPwmData.nBluePwm;
    *pdTemperature = dTemperature;

    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ReadIlasTempByFid(uint8_t nFid, uint8_t nNodeAddr,
    uint8_t* pnData0, uint8_t* pnData1, uint8_t* pnData2, uint16_t* pnValidResp, uint16_t* pnExpectedResp)
{
#if OSP_BRIDGE_DISABLE_ILAS
    (void)nFid;
    (void)nNodeAddr;
    (void)pnData0;
    (void)pnData1;
    (void)pnData2;
    (void)pnValidResp;
    (void)pnExpectedResp;
    OspBridgeSetError("ILAS disabled");
    return -99;
#else
    int nStatus;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }

    nStatus = IlasLed_ReadFirstTempByFid(nFid, nNodeAddr, pnData0, pnData1, pnData2, pnValidResp, pnExpectedResp);
    OspBridgeSetError((nStatus == 0) ? "OK" : "IlasLed_ReadFirstTempByFid failed");
    return nStatus;
#endif
}

OSP_BRIDGE_API void OSP_BRIDGE_CALL OspBridge_Close(void)
{
    if (!gbOspBridgeInitialized)
    {
        /* ?대? ?ロ? ?덉쑝硫?以묐났 ?몄텧 臾댁떆 */
        return;
    }

    /* 1. FIO OSP ?몃뱾 癒쇱? ?リ린 (E2B ?꾨젅????鍮꾩?) */
    if (ghFioOspHandle != NULL)
    {
        (void)adi_eal_fioOspClose(ghFioOspHandle);
        ghFioOspHandle = NULL;
    }

    /* multicast ?몃뱾 癒쇱? Close (terminateInstance ?? */
    if (ghFioMcastHandle != NULL)
    {
        (void)adi_eal_fioOspClose(ghFioMcastHandle);
        ghFioMcastHandle = NULL;
    }
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahFidFioOspHandle[i] != NULL)
        {
            (void)adi_eal_fioOspClose(gahFidFioOspHandle[i]);
            gahFidFioOspHandle[i] = NULL;
        }
    }
    IlasLed_CloseAll();
    ghFioOspHandle2 = NULL;

    /* 2. EAL ?몄뒪?댁뒪 ?댁젣 */
    adi_eal_terminateInstance(0U);

    /* 3. network device ?リ린 ??openDevice() ??吏?
     *    adi_network_init() / adi_pal_init() ? deinit ?⑥닔媛 ?놁쑝誘濡?     *    ?ы샇異쒗븯吏 ?딅뒗?? (gbSystemInitialized ?뚮옒洹몃줈 蹂댄샇)
     *    closeDevice 留??몄텧??SPI ?몃뱾???댁젣?섎㈃ ?ㅼ쓬 openDevice 媛 ?뺤긽 ?숈옉. */
    adi_network_closeDevice(ADI_NETWORK_DEV_IDX);

    /* 4. RTOS ?ㅼ?以꾨윭 醫낅즺 */
    prj_osal_endScheduler();

    /* 5. ?ъ뿰寃곗쓣 ?꾪빐 ?꾩뿭 ?곹깭 珥덇린??*/
    (void)ADI_MEMSET(&goAppData, 0, sizeof(goAppData));
    (void)ADI_MEMSET(&goMacPhyCfg, 0, sizeof(goMacPhyCfg));
    (void)ADI_MEMSET(&goEalFioOspCfg, 0, sizeof(goEalFioOspCfg));
    (void)ADI_MEMSET(&goEalDiagcfg, 0, sizeof(goEalDiagcfg));

    /* ?ㅽ깮쨌TCB 硫붾え由щ? 0?쇰줈 珥덇린?뷀빐 ?ъ궗????garbage 諛⑹? */
    (void)ADI_MEMSET(ganInitThreadStack, 0, sizeof(ganInitThreadStack));
    (void)ADI_MEMSET(ganAppThreadStack1, 0, sizeof(ganAppThreadStack1));
    (void)ADI_MEMSET(ganAppThreadStack2, 0, sizeof(ganAppThreadStack2));
    (void)ADI_MEMSET(ganHouseKeepingThreadStack, 0, sizeof(ganHouseKeepingThreadStack));
    (void)ADI_MEMSET(ganInitThreadTcb, 0, sizeof(ganInitThreadTcb));
    (void)ADI_MEMSET(ganAppThreadTcb1, 0, sizeof(ganAppThreadTcb1));
    (void)ADI_MEMSET(ganAppThreadTcb2, 0, sizeof(ganAppThreadTcb2));
    (void)ADI_MEMSET(ganHouseKeepingThreadTcb, 0, sizeof(ganHouseKeepingThreadTcb));

    ghAppThread1 = NULL;
    ghAppThread2 = NULL;
    ghHouseKeepingThread = NULL;
    gnOspBridgeDeviceAddress = INIT_DEV_ADDR;
    gnPlcaNodeId = 1U;

    gbOspBridgeInitialized = false;
    gbFioOspDeviceInitialized = false;  /* ???ъ뿰寃???FioOspDeviceInit ?ъ떎???덉슜 */
    (void)memset(gbAd3304Connected, 0, sizeof(gbAd3304Connected));
    (void)memset(gnFidNodeAddr,     0, sizeof(gnFidNodeAddr));
    FioOspResetFidDiag();
    /* multicast ?곹깭 珥덇린??(terminateInstance ?댄썑 ?덉쟾) */
    ghFioMcastHandle = NULL;
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++) { ganMcastHandles[i] = NULL; }
    (void)ADI_MEMSET(&goMcastCfg, 0, sizeof(goMcastCfg));
    (void)ADI_MEMSET(gnFioMcastMem, 0, sizeof(gnFioMcastMem));
    /* gbOspBridgeMode, gbSystemInitialized ??true ?좎? ??     * ?ъ뿰寃???adi_pal_init()/adi_network_init() ?ы샇異?諛⑹? */
    OspBridgeSetError("Closed");
}

OSP_BRIDGE_API const char* OSP_BRIDGE_CALL OspBridge_GetLastError(void)
{
    return gsOspBridgeLastError;
}

OSP_BRIDGE_API const char* OSP_BRIDGE_CALL OspBridge_GetFioTrace(void)
{
    return gsFioTrace;
}

/* =========================================================================
 * OspBridge_GetFioStatus
 *
 * FIO0 / FIO1 梨꾨꼸??珥덇린???곹깭瑜?臾몄옄?대줈 諛섑솚?⑸땲??
 * C#?먯꽌 Init 吏곹썑 ?몄텧?섏뿬 FIO1 梨꾨꼸???뺤긽?곸쑝濡??대졇?붿? 吏꾨떒?⑸땲??
 *
 * 諛섑솚 ?뺤떇:
 *   "FIO0=OK(0x1234ABCD) FIO1=OK(0x5678DCBA)"   ???????뺤긽
 *   "FIO0=OK(0x1234ABCD) FIO1=NULL"              ??FIO1 ?몃뱾 ?놁쓬
 *   "FIO0=NULL FIO1=NULL"                         ??珥덇린???? * ========================================================================= */
OSP_BRIDGE_API const char* OSP_BRIDGE_CALL OspBridge_GetFioStatus(void)
{
    static char sBuf[512];
    char* p = sBuf;

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        int n = snprintf(p, (size_t)(sBuf + sizeof(sBuf) - p),
                         "FID%02u=%s ",
                         (unsigned)(i + 1U),
                         (gahFidFioOspHandle[i] != NULL) ? "OK" : "NULL");
        if (n > 0) { p += n; }
    }
    if (p > sBuf && *(p - 1) == '\n') { *(p - 1) = '\0'; }
    return sBuf;
}

/* =========================================================================
 * OspBridge_GetFioDiag
 *
 * FIO0 / FIO1 Init ???④퀎??status 肄붾뱶瑜?臾몄옄?대줈 諛섑솚?⑸땲??
 * Init 吏곹썑 C#?먯꽌 ?몄텧?섎㈃ ?대뒓 ?④퀎?먯꽌 ?ㅽ뙣?덈뒗吏 ?뺥솗???????덉뒿?덈떎.
 *
 * 諛섑솚 ?덉떆 (紐⑤몢 ?뺤긽):
 *   "F0:Open=0,CB=0,RAM=0,Rst=0,Bidir=0,Addr=1,Setup=0,State=0,Hdl=1|"
 *   "F1:Open=0,CB=0,RAM=0,Rst=0,Bidir=0,Addr=1,Setup=0,State=0,Hdl=1"
 *
 * status=0 ?대㈃ ?깃났, -99 ?대㈃ ?대떦 ?④퀎 誘몄떎?? 洹???媛믪? EAL ?ㅻ쪟肄붾뱶
 * ========================================================================= */
OSP_BRIDGE_API const char* OSP_BRIDGE_CALL OspBridge_GetFioDiag(void)
{
    static char sBuf[4096];
    char* p = sBuf;
    int n = snprintf(p, (size_t)(sBuf + sizeof(sBuf) - p),
        "F0:Open=%d,CB=%d,RAM=%d,Rst=%d,Bidir=%d,Addr=%d,Setup=%d,State=%d,Hdl=%d|"
        "F1:Open=%d,CB=%d,RAM=%d,Rst=%d,Bidir=%d,Addr=%d,Setup=%d,State=%d,Hdl=%d",
        goFioDiag.nFio0Open, goFioDiag.nFio0Callback, goFioDiag.nFio0InstRAM,
        goFioDiag.nFio0Reset, goFioDiag.nFio0InitBidir, goFioDiag.nFio0AssignedAddr,
        goFioDiag.nFio0SetSetup, goFioDiag.nFio0SetState, goFioDiag.nFio0HandleValid,
        goFioDiag.nFio1Open, goFioDiag.nFio1Callback, goFioDiag.nFio1InstRAM,
        goFioDiag.nFio1Reset, goFioDiag.nFio1InitBidir, goFioDiag.nFio1AssignedAddr,
        goFioDiag.nFio1SetSetup, goFioDiag.nFio1SetState, goFioDiag.nFio1HandleValid);
    if (n > 0) { p += n; }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        n = snprintf(p, (size_t)(sBuf + sizeof(sBuf) - p),
                     "|FID%02u:Open=%d,CB=%d,RAM=%d,Rst=%d,Bidir=%d,Addr=%d,Setup=%d,State=%d,Hdl=%d",
                     (unsigned)(i + 1U),
                     gaFidFioDiag[i].nOpen, gaFidFioDiag[i].nCallback,
                     gaFidFioDiag[i].nInstRAM, gaFidFioDiag[i].nReset,
                     gaFidFioDiag[i].nInitBidir, gaFidFioDiag[i].nAssignedAddr,
                     gaFidFioDiag[i].nSetSetup, gaFidFioDiag[i].nSetState,
                     gaFidFioDiag[i].nHandleValid);
        if (n > 0) { p += n; }
    }
    n = IlasLed_AppendDiag(p, (size_t)(sBuf + sizeof(sBuf) - p));
    if (n > 0) { p += n; }
    return sBuf;
}

OSP_BRIDGE_API const char* OSP_BRIDGE_CALL OspBridge_GetFioDiagAll(void)
{
    return OspBridge_GetFioDiag();
}

/* =========================================================================
 * OspBridge_GetMacNode
 *
 * discovery ?꾨즺 ??ganRemoteMacAddrMap ????λ맂 ?ㅼ젣 MAC 二쇱냼?
 * gnPlcaNodeId ????λ맂 PLCA Node ID 瑜?諛섑솚?⑸땲??
 *
 * @param pMac6   [out] 6諛붿씠??踰꾪띁 (MSB first: pMac6[0]=理쒖긽??
 * @param pNodeId [out] PLCA Node ID (0~254)
 * @return 0=?깃났, ?뚯닔=?ㅻ쪟
 * ========================================================================= */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_GetMacNode(uint8_t* pMac6, uint8_t* pNodeId)
{
    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }
    if (pMac6 == NULL || pNodeId == NULL)
    {
        OspBridgeSetError("Invalid pointer");
        return -2;
    }

    /* ?곌껐??泥?踰덉㎏ FID??MAC 諛섑솚 (?놁쑝硫?discovery ?섏떊 MAC) */
    {
        bool bFound = false;
        for (uint32_t nIdx = 0U; nIdx < 8U; nIdx++)
        {
            if (gbAd3304Connected[nIdx])
            {
                uint8_t anFidMac[6u];
                app_utils_mac64To8(anFidMac, REMOTE_MAC_BASE + (uint64_t)(nIdx + 1U));
                /* discovery 肄쒕갚?먯꽌 媛깆떊???ㅼ젣 MAC ?ъ슜 */
                (void)memcpy(pMac6, &ganRemoteMacAddrMap[nIdx][0u], 6u);
                *pNodeId = ganRemoteMacAddrMap[nIdx][5u];
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            (void)memcpy(pMac6, &ganRemoteMacAddrMap[APP_REMOTE_1][0u], 6u);
            *pNodeId = gnPlcaNodeId;
        }
    }

    OspBridgeSetError("OK");
    return 0;
}

/* =========================================================================
 * OspBridge_SetMac
 *
 * AD3301 MAC ???듭떊?쇰줈 蹂寃쏀븯怨?E2B/OSP 瑜???MAC 湲곗??쇰줈 ?ъ큹湲고솕?⑸땲??
 *   1) adi_eal_writeMacAddr() ??AD3301 LCE + AD3306 EAL ?쇱슦?? *   2) regmap / E2B cfg 媛깆떊 (Eth10BaseT1sCfg_SetRemoteMac)
 *   3) E2B + FIO-OSP ??Init (?ㅼ쐞移??섑뵆留??놁쓬)
 *
 * ?꾩썝 ?ъ씤媛 ?꾩뿉??OspBridge_InitFixed() 媛 ?ㅼ쐞移?MAC ?쇰줈 ?ㅼ떆 ?쒖옉?⑸땲??
 *
 * @param pMac6 [in] 6諛붿씠??MAC, MSB first
 * @return 0=?깃났, ?뚯닔=?ㅻ쪟
 * ========================================================================= */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetMac(const uint8_t* pMac6)
{
    ADI_EAL_STATUS eEalStatus;
    int nReinit;
    uint8_t anReadMac[6u];

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }
    if (pMac6 == NULL)
    {
        OspBridgeSetError("Invalid pointer");
        return -2;
    }

    /* 1. AD3301 + EAL: ?꾩옱 ?곌껐 MAC ?쇰줈 ?꾨떖????MAC 湲곕줉 (volatile) */
    {
        uint8_t anMacWr[6U];
        (void)memcpy(anMacWr, pMac6, 6U);
        eEalStatus = adi_eal_writeMacAddr(0U, APP_REMOTE_1, anMacWr);
    }
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_eal_writeMacAddr failed");
        return (int)eEalStatus;
    }
    adi_network_flushTxTimed(50ULL * MS_TO_NS);

    /* 2. ?대? 留?/ regmap / E2B remote config ?숆린??*/
    (void)memcpy(&ganRemoteMacAddrMap[APP_REMOTE_1][0u], pMac6, 6u);
    Eth10BaseT1sCfg_SetRemoteMac(pMac6);

    eEalStatus = adi_eal_setMacAddrSampling(0U, ADI_EAL_ALL_REMOTES, false);
    if (eEalStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_eal_setMacAddrSampling(false) failed");
        return (int)eEalStatus;
    }
    adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 0ULL);

    /* 3. E2B + OSP ?ъ큹湲고솕 ????MAC ?쇰줈 discovery / configureNetwork / FIO init */
    nReinit = OspBridge_ReinitE2bAndOsp();
    if (nReinit != 0)
    {
        return nReinit;
    }

    (void)memcpy(anReadMac, &ganRemoteMacAddrMap[APP_REMOTE_1][0u], 6u);
    if (memcmp(anReadMac, pMac6, 6U) != 0)
    {
        OspBridgeSetError("MAC mismatch after re-init");
        return -4;
    }

    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ApplyMacNodeNoReinit(const uint8_t* pMac6, uint8_t nNodeId)
{
    uint32_t nOldIdx = NUM_REMOTE_NODES;
    uint32_t nNewIdx;
    uint32_t nRemoteNum;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }
    if (pMac6 == NULL)
    {
        OspBridgeSetError("Invalid pointer");
        return -2;
    }
    if (nNodeId == 0U || nNodeId > 254U)
    {
        OspBridgeSetError("Invalid PLCA node ID");
        return -3;
    }
    if (pMac6[5U] < 0x01U || pMac6[5U] > 0x08U || pMac6[5U] != nNodeId)
    {
        OspBridgeSetError("OTP MAC last byte must match Node/FID 0x01..0x08");
        return -4;
    }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gbAd3304Connected[i])
        {
            nOldIdx = i;
            break;
        }
    }
    if (nOldIdx == NUM_REMOTE_NODES)
    {
        OspBridgeSetError("No discovered node to apply OTP MAC/Node");
        return -5;
    }

    nNewIdx = (uint32_t)(pMac6[5U] - 1U);
    nRemoteNum = gnFidRemoteNum[nOldIdx];
    if (nRemoteNum >= NUM_REMOTE_NODES)
    {
        nRemoteNum = nOldIdx;
    }

    /*
     * OTP mode is a logical remap for the PC session.  Do not rewrite the
     * volatile AD3301 MAC or reopen FIO here; doing so resets the working FIO
     * path back into the IF/SA_IF address domain.  Keep the proven physical
     * handle and expose it under the OTP FID.
     */
    if (nOldIdx != nNewIdx)
    {
        gahFidFioOspHandle[nNewIdx] = gahFidFioOspHandle[nOldIdx];
        gahFidFioOspHandle[nOldIdx] = NULL;

        gaFidFioDiag[nNewIdx] = gaFidFioDiag[nOldIdx];
        gaFidFioDiag[nOldIdx].nOpen = -99;
        gaFidFioDiag[nOldIdx].nCallback = -99;
        gaFidFioDiag[nOldIdx].nInstRAM = -99;
        gaFidFioDiag[nOldIdx].nReset = -99;
        gaFidFioDiag[nOldIdx].nInitBidir = -99;
        gaFidFioDiag[nOldIdx].nAssignedAddr = 0;
        gaFidFioDiag[nOldIdx].nSetSetup = -99;
        gaFidFioDiag[nOldIdx].nSetState = -99;
        gaFidFioDiag[nOldIdx].nHandleValid = 0;

        gnFidNodeAddr[nNewIdx] = gnFidNodeAddr[nOldIdx];
        gnFidNodeAddr[nOldIdx] = 0U;
        gnFidRemoteNum[nNewIdx] = nRemoteNum;
        gnFidRemoteNum[nOldIdx] = nOldIdx;

        gbAd3304Connected[nOldIdx] = false;
        (void)ADI_MEMSET(&ganRemoteMacAddrMap[nOldIdx][0U], 0, 6U);
    }

    (void)memcpy(&ganRemoteMacAddrMap[nNewIdx][0U], pMac6, 6U);
    (void)memcpy(&ganRemoteMacAddrMap[APP_REMOTE_1][0U], pMac6, 6U);
    Eth10BaseT1sCfg_SetRemoteMacByIdx(nNewIdx, pMac6);
    Eth10BaseT1sCfg_SetRemoteMac(pMac6);
    gbAd3304Connected[nNewIdx] = true;
    gnFidRemoteNum[nNewIdx] = nRemoteNum;
    gnPlcaNodeId = nNodeId;
    ghFioOspHandle2 = gahFidFioOspHandle[nNewIdx];

    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ApplyFidOffsetNoReinit(uint8_t nOffset)
{
    bool abOldConnected[NUM_REMOTE_NODES];
    ADI_EAL_FIO_OSP_HANDLE ahOldHandle[NUM_REMOTE_NODES];
    OSP_FID_FIO_DIAG aoOldDiag[NUM_REMOTE_NODES];
    uint16_t anOldNodeAddr[NUM_REMOTE_NODES];
    uint32_t anOldRemoteNum[NUM_REMOTE_NODES];
    uint8_t anNewFids[NUM_REMOTE_NODES];
    uint32_t nMoved = 0U;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }
    if (nOffset == 0U || nOffset >= NUM_REMOTE_NODES)
    {
        OspBridgeSetError("Invalid FID offset");
        return -2;
    }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        abOldConnected[i] = gbAd3304Connected[i];
        ahOldHandle[i] = gahFidFioOspHandle[i];
        aoOldDiag[i] = gaFidFioDiag[i];
        anOldNodeAddr[i] = gnFidNodeAddr[i];
        anOldRemoteNum[i] = gnFidRemoteNum[i];
    }

    (void)memset(gbAd3304Connected, 0, sizeof(gbAd3304Connected));
    (void)ADI_MEMSET(&ganRemoteMacAddrMap[0][0U], 0, sizeof(ganRemoteMacAddrMap));
    FioOspResetFidDiagOnly();

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (!abOldConnected[i] || ahOldHandle[i] == NULL)
        {
            continue;
        }

        uint32_t nNewIdx = i + (uint32_t)nOffset;
        if (nNewIdx >= NUM_REMOTE_NODES)
        {
            continue;
        }

        uint8_t anMac[6U];
        uint8_t nFid = (uint8_t)(nNewIdx + 1U);
        app_utils_mac64To8(anMac, REMOTE_MAC_BASE + (uint64_t)nFid);

        gahFidFioOspHandle[nNewIdx] = ahOldHandle[i];
        gaFidFioDiag[nNewIdx] = aoOldDiag[i];
        gnFidNodeAddr[nNewIdx] = anOldNodeAddr[i];
        gnFidRemoteNum[nNewIdx] = anOldRemoteNum[i];
        gbAd3304Connected[nNewIdx] = true;
        (void)memcpy(&ganRemoteMacAddrMap[nNewIdx][0U], anMac, 6U);
        Eth10BaseT1sCfg_SetRemoteMacByIdx(nNewIdx, anMac);
        nMoved++;
    }

    if (nMoved == 0U)
    {
        OspBridgeSetError("No discovered FID handles to remap");
        return -3;
    }

    ghFioOspHandle2 = NULL;
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahFidFioOspHandle[i] != NULL)
        {
            ghFioOspHandle2 = gahFidFioOspHandle[i];
            break;
        }
    }

    OspBridgeSetError("OK");
    return 0;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ApplyMacNodeMapNoReinit(
    const uint8_t* pOldFids,
    const uint8_t* pMac6List,
    const uint8_t* pNodeIds,
    uint8_t nCount)
{
    bool abOldConnected[NUM_REMOTE_NODES];
    bool abNewUsed[NUM_REMOTE_NODES];
    ADI_EAL_FIO_OSP_HANDLE ahOldHandle[NUM_REMOTE_NODES];
    OSP_FID_FIO_DIAG aoOldDiag[NUM_REMOTE_NODES];
    uint16_t anOldNodeAddr[NUM_REMOTE_NODES];
    uint32_t anOldRemoteNum[NUM_REMOTE_NODES];
    uint8_t anNewFids[NUM_REMOTE_NODES];
    uint32_t nMoved = 0U;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }
    if (pOldFids == NULL || pMac6List == NULL || pNodeIds == NULL)
    {
        OspBridgeSetError("Invalid pointer for OTP FID map");
        return -2;
    }
    if (nCount == 0U || nCount > NUM_REMOTE_NODES)
    {
        OspBridgeSetError("Invalid OTP FID map count");
        return -3;
    }

    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        abOldConnected[i] = gbAd3304Connected[i];
        abNewUsed[i] = false;
        ahOldHandle[i] = gahFidFioOspHandle[i];
        aoOldDiag[i] = gaFidFioDiag[i];
        anOldNodeAddr[i] = gnFidNodeAddr[i];
        anOldRemoteNum[i] = gnFidRemoteNum[i];
    }

    for (uint32_t k = 0U; k < (uint32_t)nCount; k++)
    {
        const uint8_t* pMac6 = &pMac6List[k * 6U];
        uint8_t nOldFid = pOldFids[k];
        uint8_t nNodeId = pNodeIds[k];
        uint8_t nNewFid = pMac6[5U];
        uint32_t nOldIdx;
        uint32_t nNewIdx;

        if (nOldFid < 0x01U || nOldFid > 0x08U)
        {
            OspBridgeSetError("Invalid old FID in OTP map");
            return -4;
        }
        if (nNewFid < 0x01U || nNewFid > 0x08U || nNodeId != nNewFid)
        {
            OspBridgeSetError("OTP MAC last byte must match Node/FID 0x01..0x08");
            return -5;
        }

        nOldIdx = (uint32_t)(nOldFid - 1U);
        nNewIdx = (uint32_t)(nNewFid - 1U);
        if (!abOldConnected[nOldIdx] || ahOldHandle[nOldIdx] == NULL)
        {
            OspBridgeSetError("Old FID is not active for OTP map");
            return -6;
        }
        if (abNewUsed[nNewIdx])
        {
            OspBridgeSetError("Duplicate OTP target FID in OTP map");
            return -7;
        }
        abNewUsed[nNewIdx] = true;
    }

    (void)memset(abNewUsed, 0, sizeof(abNewUsed));
    (void)memset(gbAd3304Connected, 0, sizeof(gbAd3304Connected));
    (void)memset(gahFidFioOspHandle, 0, sizeof(gahFidFioOspHandle));
    (void)ADI_MEMSET(&ganRemoteMacAddrMap[0][0U], 0, sizeof(ganRemoteMacAddrMap));
    FioOspResetFidDiagOnly();

    for (uint32_t k = 0U; k < (uint32_t)nCount; k++)
    {
        const uint8_t* pMac6 = &pMac6List[k * 6U];
        uint8_t nOldFid = pOldFids[k];
        uint8_t nNodeId = pNodeIds[k];
        uint8_t nNewFid = pMac6[5U];
        uint32_t nOldIdx;
        uint32_t nNewIdx;

        if (nOldFid < 0x01U || nOldFid > 0x08U)
        {
            OspBridgeSetError("Invalid old FID in OTP map");
            return -4;
        }
        if (nNewFid < 0x01U || nNewFid > 0x08U || nNodeId != nNewFid)
        {
            OspBridgeSetError("OTP MAC last byte must match Node/FID 0x01..0x08");
            return -5;
        }

        nOldIdx = (uint32_t)(nOldFid - 1U);
        nNewIdx = (uint32_t)(nNewFid - 1U);
        if (!abOldConnected[nOldIdx] || ahOldHandle[nOldIdx] == NULL)
        {
            OspBridgeSetError("Old FID is not active for OTP map");
            return -6;
        }
        if (abNewUsed[nNewIdx])
        {
            OspBridgeSetError("Duplicate OTP target FID in OTP map");
            return -7;
        }

        abNewUsed[nNewIdx] = true;
        anNewFids[k] = nNewFid;
        gahFidFioOspHandle[nNewIdx] = ahOldHandle[nOldIdx];
        gaFidFioDiag[nNewIdx] = aoOldDiag[nOldIdx];
        gnFidNodeAddr[nNewIdx] = anOldNodeAddr[nOldIdx];
        gnFidRemoteNum[nNewIdx] = anOldRemoteNum[nOldIdx];
        gbAd3304Connected[nNewIdx] = true;
        (void)memcpy(&ganRemoteMacAddrMap[nNewIdx][0U], pMac6, 6U);
        Eth10BaseT1sCfg_SetRemoteMacByIdx(nNewIdx, pMac6);
        nMoved++;
    }

    IlasLed_RemapFidSlots(pOldFids, anNewFids, nCount);

    if (nMoved == 0U)
    {
        OspBridgeSetError("No FID handles were remapped by OTP map");
        return -8;
    }

    ghFioOspHandle2 = NULL;
    for (uint32_t i = 0U; i < NUM_REMOTE_NODES; i++)
    {
        if (gahFidFioOspHandle[i] != NULL)
        {
            ghFioOspHandle2 = gahFidFioOspHandle[i];
            gnPlcaNodeId = (uint8_t)(i + 1U);
            break;
        }
    }

    OspBridgeSetError("OK");
    return 0;
}

/* =========================================================================
 * OspBridge_SetNodeId
 *
 * AD3301 PLCA Node ID 瑜?蹂寃쏀빀?덈떎.
 *   1) adi_eal_setRemotePlcaId() 濡?EAL ??諛섏쁺
 *   2) ?깃났 ??gnPlcaNodeId 媛깆떊
 *
 * @param nNodeId [in] PLCA Node ID (1~254, 0=controller)
 * @return 0=?깃났, ?뚯닔=?ㅻ쪟
 * ========================================================================= */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_SetNodeId(uint8_t nNodeId)
{
    ADI_EAL_STATUS eStatus;
    ADI_EAL_NODE_PLCA_ID_CFG oPlca;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }

    oPlca.nMaskOfPlcaIdsUsed = 0x01U;
    oPlca.anPlcaID[0U] = nNodeId;

    eStatus = adi_eal_setRemotePlcaId(0U, &ganRemoteMacAddrMap[APP_REMOTE_1][0u], &oPlca, 0x0U);
    if (eStatus != ADI_EAL_STATUS_SUCCESS)
    {
        OspBridgeSetError("adi_eal_setRemotePlcaId failed");
        return (int)eStatus;
    }

    /* ?깃났 ???대? 異붿쟻 媛?媛깆떊 */
    gnPlcaNodeId = nNodeId;

    OspBridgeSetError("OK");
    return 0;
}

/* =========================================================================
 * OspBridge_ProgramMacNodeOtp
 *
 * Programs AD3301 OTP designer blocks for device MAC address and PLCA node ID.
 * This is a one-time permanent operation. If the target designer OTP blocks are
 * already programmed, adi_otp_generateOtpWriteData() returns an error and the
 * programming step is not executed.
 *
 * @param pMac6   [in] Permanent device MAC, MSB first
 * @param nNodeId [in] Permanent PLCA node ID
 * @return 0=success, negative=bridge/local failure, positive=ADI_OTP_STATUS
 * ========================================================================= */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ProgramMacNodeOtp(const uint8_t* pMac6, uint8_t nNodeId)
{
    ADI_OTP_HANDLE hOtp = NULL;
    ADI_OTP_CONFIG oOtpCfg;
    ADI_OTP_REMOTE_DATA oRemoteOtpData;
    ADI_OTP_PGMTIME_REMOTE_CFG oPgmTimeCfg;
    uint64_t anAccessMac[1U];
    uint8_t anCurrentMac[6U];
    uint8_t anMulticastMac[6U];
    uint8_t nPgmTimePlcaId = 1U;
    uint8_t anDesignerRaw[ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS * ADI_OTP_BLOCK_LEN_BYTES];
    bool bDesignerRawValid = false;
    char sOtpError[256] = "OK";
    ADI_OTP_STATUS eOtpStatus;
    int nReturn = 0;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }
    if (pMac6 == NULL)
    {
        OspBridgeSetError("Invalid pointer");
        return -2;
    }
    if (pMac6[5U] < 0x01U || pMac6[5U] > 0x08U)
    {
        OspBridgeSetError("OTP MAC last byte must be FID/Node 0x01..0x08");
        return -3;
    }
    if (nNodeId != pMac6[5U])
    {
        OspBridgeSetError("OTP Node ID must match MAC last byte/FID");
        return -4;
    }

    (void)ADI_MEMSET(&oOtpCfg, 0, sizeof(oOtpCfg));
    (void)ADI_MEMSET(&oRemoteOtpData, 0, sizeof(oRemoteOtpData));
    (void)ADI_MEMSET(&oPgmTimeCfg, 0, sizeof(oPgmTimeCfg));
    (void)ADI_MEMSET(anCurrentMac, 0, sizeof(anCurrentMac));

    if (gnOtpReadAccessFid >= 0x01U && gnOtpReadAccessFid <= 0x08U)
    {
        app_utils_mac64To8(anCurrentMac, REMOTE_MAC_BASE + (uint64_t)gnOtpReadAccessFid);
    }
    else
    {
        (void)memcpy(anCurrentMac, &ganRemoteMacAddrMap[APP_REMOTE_1][0U], 6U);
    }
    if (OspBridge_Mac6To64(anCurrentMac) == 0ULL ||
        anCurrentMac[5U] < 0x01U || anCurrentMac[5U] > 0x08U)
    {
        for (uint32_t nFidIdx = 0U; nFidIdx < NUM_REMOTE_NODES; nFidIdx++)
        {
            if (!gbAd3304Connected[nFidIdx])
            {
                continue;
            }

            uint8_t nFid = (uint8_t)(nFidIdx + 1U);
            for (uint32_t nMapIdx = 0U; nMapIdx < NUM_REMOTE_NODES; nMapIdx++)
            {
                if (ganRemoteMacAddrMap[nMapIdx][5U] == nFid)
                {
                    (void)memcpy(anCurrentMac, &ganRemoteMacAddrMap[nMapIdx][0U], 6U);
                    break;
                }
            }

            if (anCurrentMac[5U] == nFid)
            {
                break;
            }
        }
    }
    anAccessMac[0U] = OspBridge_Mac6To64(anCurrentMac);
    if (anAccessMac[0U] == 0ULL)
    {
        anAccessMac[0U] = REMOTE1_MAC_ADDR;
        OspBridge_Mac64To6(anAccessMac[0U], anCurrentMac);
    }
    if (anCurrentMac[5U] >= 0x01U && anCurrentMac[5U] <= 0x08U)
    {
        nPgmTimePlcaId = anCurrentMac[5U];
    }

    OspBridge_Mac64To6(REMOTE1_MULTIMAC_ADDR, anMulticastMac);

    (void)memcpy(oRemoteOtpData.oOtpDesignerData.anDevMacAddr, pMac6, 6U);
    (void)memcpy(oRemoteOtpData.oOtpDesignerData.anMulticastMacAddr, anMulticastMac, 6U);
    oRemoteOtpData.oOtpDesignerData.nPlcaLocalId = nNodeId;
    oRemoteOtpData.oOtpDesignerData.nPlcaNodeCount = MAX_PLCA_NODE_CNT;
    oRemoteOtpData.oOtpDesignerData.bPlcaEnable = true;
    oRemoteOtpData.oOtpDesignerData.bPlcaLutEn = false;
    oRemoteOtpData.bDesignerBlockWrEn[0U] = true; /* Designer block 0: MAC */
    oRemoteOtpData.bDesignerBlockWrEn[4U] = true; /* Designer block 4: PLCA */

    oPgmTimeCfg.paRemoteMacAddr = anAccessMac;
    OspBridge_Mac64To6(CONTROLLER1_MAC_ADDR, oPgmTimeCfg.anControllerMacAddr);
    oPgmTimeCfg.nRemotePlcaId = nPgmTimePlcaId;
    /*
     * adi_otp_init() resets the remote LV die. For unprogrammed boards the
     * currently reachable MAC/FID comes back from SA_IF sampling after reset,
     * so keep sampling enabled during the OTP setup access phase.
     */
    oPgmTimeCfg.bSampleMacAddr = true;

    oOtpCfg.bPgmOverOaspi = false;
    oOtpCfg.nNumNodes = 1U;
    oOtpCfg.paoRemoteOtpData = &oRemoteOtpData;
    oOtpCfg.poPgmTimeCfg = &oPgmTimeCfg;

    OspBridge_CloseFioHandlesOnly();
    adi_eal_terminateInstance(0U);

    adi_otp_init(&hOtp, &oOtpCfg, ADI_NETWORK_DEV_IDX, false);
    if (hOtp == NULL)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_init failed; accessMAC=%02X:%02X:%02X:%02X:%02X:%02X targetMAC=%02X:%02X:%02X:%02X:%02X:%02X node=%u",
            anCurrentMac[0U], anCurrentMac[1U], anCurrentMac[2U], anCurrentMac[3U], anCurrentMac[4U], anCurrentMac[5U],
            pMac6[0U], pMac6[1U], pMac6[2U], pMac6[3U], pMac6[4U], pMac6[5U], (unsigned)nNodeId);
        OspBridgeSetError(sOtpError);
        nReturn = -5;
        goto restore_osp;
    }
    (void)adi_network_setPlca(ADI_NETWORK_DEV_IDX, true, MAX_PLCA_NODE_CNT, CTRL_PLCA_ID);
    (void)adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 100ULL * MS_TO_NS);

    eOtpStatus = adi_otp_setupRemoteNode(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_setupRemoteNode failed: status=%d (PGM_FAIL=PLCA/ping/MAC-write/network-config), accessMAC=%02X:%02X:%02X:%02X:%02X:%02X targetMAC=%02X:%02X:%02X:%02X:%02X:%02X pgmPLCA=%u otpNode=%u sampleSAIF=1",
            (int)eOtpStatus,
            anCurrentMac[0U], anCurrentMac[1U], anCurrentMac[2U], anCurrentMac[3U], anCurrentMac[4U], anCurrentMac[5U],
            pMac6[0U], pMac6[1U], pMac6[2U], pMac6[3U], pMac6[4U], pMac6[5U],
            (unsigned)nPgmTimePlcaId, (unsigned)nNodeId);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    eOtpStatus = adi_otp_readBlockStatus(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_readBlockStatus failed: status=%d", (int)eOtpStatus);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }
    eOtpStatus = adi_otp_readOtpContents(hOtp, 0U, ADI_OTP_NUM_HV_DATA_BLOCKS,
        ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS, anDesignerRaw);
    bDesignerRawValid = (eOtpStatus == ADI_OTP_STATUS_NO_ERR);

    eOtpStatus = adi_otp_generateOtpWriteData(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        const uint8_t* pMacBlock = &anDesignerRaw[0U * ADI_OTP_BLOCK_LEN_BYTES];
        const uint8_t* pPlcaBlock = &anDesignerRaw[4U * ADI_OTP_BLOCK_LEN_BYTES];
        if (bDesignerRawValid)
        {
            (void)snprintf(sOtpError, sizeof(sOtpError),
                "adi_otp_generateOtpWriteData failed: status=%d; designer block already used. B3(MAC)=%u rawMAC=%02X:%02X:%02X:%02X:%02X:%02X B7(PLCA)=%u rawNode=%u. Only status 0 is writable",
                (int)eOtpStatus,
                (unsigned)oRemoteOtpData.aeBlockBootStatus[ADI_OTP_NUM_HV_DATA_BLOCKS + 0U],
                pMacBlock[10U], pMacBlock[9U], pMacBlock[4U], pMacBlock[3U], pMacBlock[2U], pMacBlock[1U],
                (unsigned)oRemoteOtpData.aeBlockBootStatus[ADI_OTP_NUM_HV_DATA_BLOCKS + 4U],
                (unsigned)pPlcaBlock[1U]);
        }
        else
        {
            (void)snprintf(sOtpError, sizeof(sOtpError),
                "adi_otp_generateOtpWriteData failed: status=%d; designer block already used. B3(MAC)=%u B7(PLCA)=%u. Only status 0 is writable",
                (int)eOtpStatus,
                (unsigned)oRemoteOtpData.aeBlockBootStatus[ADI_OTP_NUM_HV_DATA_BLOCKS + 0U],
                (unsigned)oRemoteOtpData.aeBlockBootStatus[ADI_OTP_NUM_HV_DATA_BLOCKS + 4U]);
        }
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    eOtpStatus = adi_otp_programOtp(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_programOtp failed: status=%d", (int)eOtpStatus);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    eOtpStatus = adi_otp_readVerifyOtp(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_readVerifyOtp failed: status=%d", (int)eOtpStatus);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    eOtpStatus = adi_otp_verifyBootStatus(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_verifyBootStatus failed: status=%d", (int)eOtpStatus);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    (void)memcpy(&ganRemoteMacAddrMap[APP_REMOTE_1][0U], pMac6, 6U);
    Eth10BaseT1sCfg_SetRemoteMac(pMac6);
    gnPlcaNodeId = nNodeId;
    OspBridgeSetError("OK");

terminate_otp:
    if (hOtp != NULL)
    {
        adi_otp_terminate(hOtp);
        hOtp = NULL;
    }

restore_osp:
    if (nReturn == 0)
    {
        (void)memcpy(&ganRemoteMacAddrMap[APP_REMOTE_1][0U], pMac6, 6U);
        Eth10BaseT1sCfg_SetRemoteMac(pMac6);
        gnPlcaNodeId = nNodeId;
    }
    else
    {
        (void)memcpy(&ganRemoteMacAddrMap[APP_REMOTE_1][0U], anCurrentMac, 6U);
        Eth10BaseT1sCfg_SetRemoteMac(anCurrentMac);
    }
    goAppData.nMacCnt = 0U;
    if (gbOtpReadRestoreFixed)
    {
        CreateE2bAppFixed();
    }
    else
    {
        CreateE2bAppProgrammed();
    }
    FioOspDeviceInit();
    if (nReturn != 0)
    {
        OspBridgeSetError(sOtpError);
    }

    return nReturn;
}

/* =========================================================================
 * OspBridge_ReadMacNodeOtp
 *
 * OTP designer 釉붾줉(0=MAC, 4=PLCA)??吏곸젒 ?쎌뼱 諛섑솚?쒕떎.
 * OspBridge_GetMacNode() ? ?щ━ IF Pin(SA_IF) ?섑뵆留?媛믪씠 ?꾨땲?? * "OTP???ㅼ젣濡?援ъ썙吏?媛? ?대떎.
 *
 * ?숈옉? OspBridge_ProgramMacNodeOtp() ??setup ?덈컲??洹몃?濡??ъ슜?섎릺
 * generateOtpWriteData / programOtp / readVerifyOtp ???섑뻾?섏? ?딄퀬
 * adi_otp_readOtpContents() 源뚯?留??ㅽ뻾????OSP 瑜?蹂듦뎄?쒕떎.
 *
 * @param pMac6           [out] 6諛붿씠?? MSB first (NULL ?덉슜)
 * @param pNodeId         [out] PLCA node id (NULL ?덉슜)
 * @param pBootStatusMac  [out] designer block 0(MAC) boot status (NULL ?덉슜)
 * @param pBootStatusPlca [out] designer block 4(PLCA) boot status (NULL ?덉슜)
 * @param pRawDesigner    [out] ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS*ADI_OTP_BLOCK_LEN_BYTES
 *                              諛붿씠???먮낯 ?ㅽ봽 (?덉씠?꾩썐 寃利앹슜, NULL ?덉슜)
 * @return 0=?깃났, ?뚯닔=bridge/local ?ㅽ뙣, ?묒닔=ADI_OTP_STATUS
 * ========================================================================= */
OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ReadMacNodeOtp(
    uint8_t* pMac6, uint8_t* pNodeId,
    uint8_t* pBootStatusMac, uint8_t* pBootStatusPlca,
    uint8_t* pRawDesigner)
{
    ADI_OTP_HANDLE hOtp = NULL;
    ADI_OTP_CONFIG oOtpCfg;
    ADI_OTP_REMOTE_DATA oRemoteOtpData;
    ADI_OTP_PGMTIME_REMOTE_CFG oPgmTimeCfg;
    uint64_t anAccessMac[1U];
    uint8_t  anCurrentMac[6U];
    uint8_t  nPgmTimePlcaId = 1U;
    uint8_t  anDesignerRaw[ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS * ADI_OTP_BLOCK_LEN_BYTES];
    char     sOtpError[256] = "OK";
    ADI_OTP_STATUS eOtpStatus;
    int nReturn = 0;

    if (!gbOspBridgeInitialized)
    {
        OspBridgeSetError("OspBridge_Init was not called or failed");
        return -1;
    }

    (void)ADI_MEMSET(&oOtpCfg, 0, sizeof(oOtpCfg));
    (void)ADI_MEMSET(&oRemoteOtpData, 0, sizeof(oRemoteOtpData));
    (void)ADI_MEMSET(&oPgmTimeCfg, 0, sizeof(oPgmTimeCfg));
    (void)ADI_MEMSET(anCurrentMac, 0, sizeof(anCurrentMac));
    (void)ADI_MEMSET(anDesignerRaw, 0, sizeof(anDesignerRaw));

    /* --- ?묎렐(access) MAC 寃곗젙: ProgramMacNodeOtp ? ?숈씪 濡쒖쭅 --- */
    if (gnOtpReadAccessFid >= 0x01U && gnOtpReadAccessFid <= 0x08U)
    {
        app_utils_mac64To8(anCurrentMac, REMOTE_MAC_BASE + (uint64_t)gnOtpReadAccessFid);
    }
    else
    {
        (void)memcpy(anCurrentMac, &ganRemoteMacAddrMap[APP_REMOTE_1][0U], 6U);
    }
    if (OspBridge_Mac6To64(anCurrentMac) == 0ULL ||
        anCurrentMac[5U] < 0x01U || anCurrentMac[5U] > 0x08U)
    {
        for (uint32_t nFidIdx = 0U; nFidIdx < NUM_REMOTE_NODES; nFidIdx++)
        {
            if (!gbAd3304Connected[nFidIdx])
            {
                continue;
            }

            uint8_t nFid = (uint8_t)(nFidIdx + 1U);
            for (uint32_t nMapIdx = 0U; nMapIdx < NUM_REMOTE_NODES; nMapIdx++)
            {
                if (ganRemoteMacAddrMap[nMapIdx][5U] == nFid)
                {
                    (void)memcpy(anCurrentMac, &ganRemoteMacAddrMap[nMapIdx][0U], 6U);
                    break;
                }
            }

            if (anCurrentMac[5U] == nFid)
            {
                break;
            }
        }
    }
    anAccessMac[0U] = OspBridge_Mac6To64(anCurrentMac);
    if (anAccessMac[0U] == 0ULL)
    {
        anAccessMac[0U] = REMOTE1_MAC_ADDR;
        OspBridge_Mac64To6(anAccessMac[0U], anCurrentMac);
    }
    if (anCurrentMac[5U] >= 0x01U && anCurrentMac[5U] <= 0x08U)
    {
        nPgmTimePlcaId = anCurrentMac[5U];
    }

    /* --- read ?꾩슜: designer write enable ?꾨? false --- */
    oRemoteOtpData.bDesignerBlockWrEn[0U] = false;
    oRemoteOtpData.bDesignerBlockWrEn[4U] = false;

    oPgmTimeCfg.paRemoteMacAddr = anAccessMac;
    OspBridge_Mac64To6(CONTROLLER1_MAC_ADDR, oPgmTimeCfg.anControllerMacAddr);
    oPgmTimeCfg.nRemotePlcaId  = nPgmTimePlcaId;
    /* adi_otp_init() 媛 LV die 瑜?由ъ뀑?섎?濡? 由ъ뀑 ???꾨떖???꾪빐 ?섑뵆留??좎? */
    oPgmTimeCfg.bSampleMacAddr = true;

    oOtpCfg.bPgmOverOaspi    = false;
    oOtpCfg.nNumNodes        = 1U;
    oOtpCfg.paoRemoteOtpData = &oRemoteOtpData;
    oOtpCfg.poPgmTimeCfg     = &oPgmTimeCfg;

    OspBridge_CloseFioHandlesOnly();
    adi_eal_terminateInstance(0U);

    adi_otp_init(&hOtp, &oOtpCfg, ADI_NETWORK_DEV_IDX, false);
    if (hOtp == NULL)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_init failed (read); accessMAC=%02X:%02X:%02X:%02X:%02X:%02X node=%u",
            anCurrentMac[0U], anCurrentMac[1U], anCurrentMac[2U],
            anCurrentMac[3U], anCurrentMac[4U], anCurrentMac[5U],
            (unsigned)nPgmTimePlcaId);
        OspBridgeSetError(sOtpError);
        nReturn = -5;
        goto restore_osp;
    }
    (void)adi_network_setPlca(ADI_NETWORK_DEV_IDX, true, MAX_PLCA_NODE_CNT, CTRL_PLCA_ID);
    (void)adi_network_flushTxBuffer(ADI_NETWORK_DEV_IDX, 100ULL * MS_TO_NS);

    eOtpStatus = adi_otp_setupRemoteNode(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_setupRemoteNode failed (read): status=%d", (int)eOtpStatus);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    eOtpStatus = adi_otp_readBlockStatus(hOtp, 0U);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_readBlockStatus failed (read): status=%d", (int)eOtpStatus);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    eOtpStatus = adi_otp_readOtpContents(hOtp, 0U, ADI_OTP_NUM_HV_DATA_BLOCKS,
        ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS, anDesignerRaw);
    if (eOtpStatus != ADI_OTP_STATUS_NO_ERR)
    {
        (void)snprintf(sOtpError, sizeof(sOtpError),
            "adi_otp_readOtpContents failed: status=%d", (int)eOtpStatus);
        OspBridgeSetError(sOtpError);
        nReturn = (int)eOtpStatus;
        goto terminate_otp;
    }

    /* --- ?뚯떛: ProgramMacNodeOtp 吏꾨떒 硫붿떆吏? ?숈씪??raw ?덉씠?꾩썐 ---
     *   MAC  : block0[10],[9],[4],[3],[2],[1]
     *   Node : block4[1]
     *   (?덉씠?꾩썐???섏떖?섎㈃ pRawDesigner ?ㅽ봽濡?寃利앺븷 寃? */
    {
        const uint8_t* pMacBlock  = &anDesignerRaw[0U * ADI_OTP_BLOCK_LEN_BYTES];
        const uint8_t* pPlcaBlock = &anDesignerRaw[4U * ADI_OTP_BLOCK_LEN_BYTES];

        if (pMac6 != NULL)
        {
            pMac6[0U] = pMacBlock[10U];
            pMac6[1U] = pMacBlock[9U];
            pMac6[2U] = pMacBlock[4U];
            pMac6[3U] = pMacBlock[3U];
            pMac6[4U] = pMacBlock[2U];
            pMac6[5U] = pMacBlock[1U];
        }
        if (pNodeId != NULL)
        {
            *pNodeId = pPlcaBlock[1U];
        }
        if (pBootStatusMac != NULL)
        {
            *pBootStatusMac =
                (uint8_t)oRemoteOtpData.aeBlockBootStatus[ADI_OTP_NUM_HV_DATA_BLOCKS + 0U];
        }
        if (pBootStatusPlca != NULL)
        {
            *pBootStatusPlca =
                (uint8_t)oRemoteOtpData.aeBlockBootStatus[ADI_OTP_NUM_HV_DATA_BLOCKS + 4U];
        }
        if (pRawDesigner != NULL)
        {
            (void)memcpy(pRawDesigner, anDesignerRaw, sizeof(anDesignerRaw));
        }
    }
    OspBridgeSetError("OK");

terminate_otp:
    if (hOtp != NULL)
    {
        adi_otp_terminate(hOtp);
        hOtp = NULL;
    }

restore_osp:
    /* read ??ganRemoteMacAddrMap ??嫄대뱶由ъ? ?딅뒗??(?꾩옱 媛??좎?). */
    goAppData.nMacCnt = 0U;
    if (gbOtpReadRestoreFixed)
    {
        CreateE2bAppFixed();
    }
    else
    {
        CreateE2bAppProgrammed();
    }
    FioOspDeviceInit();
    if (goAppData.nMacCnt > 0U && FioOspAnyFidHandleOpen())
    {
        gbOspBridgeMode = true;
        gbOspBridgeInitialized = true;
    }
    if (nReturn != 0)
    {
        OspBridgeSetError(sOtpError);
    }

    return nReturn;
}

OSP_BRIDGE_API int OSP_BRIDGE_CALL OspBridge_ReadMacNodeOtpByFid(
    uint8_t nAccessFid,
    uint8_t* pMac6, uint8_t* pNodeId,
    uint8_t* pBootStatusMac, uint8_t* pBootStatusPlca,
    uint8_t* pRawDesigner)
{
    int nStatus;

    if (nAccessFid < 0x01U || nAccessFid > 0x08U)
    {
        OspBridgeSetError("Invalid access FID for OTP read");
        return -2;
    }

    gnOtpReadAccessFid = nAccessFid;
    gbOtpReadRestoreFixed = true;
    nStatus = OspBridge_ReadMacNodeOtp(
        pMac6, pNodeId, pBootStatusMac, pBootStatusPlca, pRawDesigner);
    gnOtpReadAccessFid = 0U;
    gbOtpReadRestoreFixed = false;
    return nStatus;
}

/** EAL Diagnostic callback */
ADI_MEM_CODE_CRIT
static void EalDiagCallback(uint32_t nInstNum, uint32_t nRemoteNum, ADI_EAL_DIAG_CBK_EVT eDiagCbkEvent,
    const void* pArg)
{
    switch (eDiagCbkEvent)
    {
    case ADI_EAL_DIAG_CBK_EVT_MAC_STATUS:
    {
        /* Stats data */
        ADI_EAL_DIAG_CBK_EVT_MAC_STATUS_DATA* poStatusDat = (ADI_EAL_DIAG_CBK_EVT_MAC_STATUS_DATA*)pArg;
        app_utils_logRemoteMacStatus(poStatusDat, nRemoteNum);
        break;
    }

    case ADI_EAL_DIAG_CBK_EVT_PHY_STATUS:
    {
        ADI_EAL_DIAG_CBK_EVT_PHY_STATUS_DATA* poStatusDat = (ADI_EAL_DIAG_CBK_EVT_PHY_STATUS_DATA*)pArg;
        app_utils_logRemotePhyStatus(poStatusDat, nRemoteNum);
        break;
    }

    case ADI_EAL_DIAG_CBK_EVT_PLCA_STATUS:
    {
        ADI_EAL_DIAG_CBK_EVT_PLCA_STATUS_DATA* poStatusDat = (ADI_EAL_DIAG_CBK_EVT_PLCA_STATUS_DATA*)pArg;
        app_utils_logRemotePlcaStatus(poStatusDat, nRemoteNum);
        break;
    }

    case ADI_EAL_DIAG_CBK_EVT_PTP_STATUS:
    {
        ADI_EAL_DIAG_CBK_EVT_PTP_STATUS_DATA* poStatusDat = (ADI_EAL_DIAG_CBK_EVT_PTP_STATUS_DATA*)pArg;
        app_utils_logRemotePtpStatus(poStatusDat, nRemoteNum);
        break;
    }

    case ADI_EAL_DIAG_CBK_EVT_PCS_STATUS:
    {
        ADI_EAL_DIAG_CBK_EVT_PCS_STATUS_DATA* poStatusDat = (ADI_EAL_DIAG_CBK_EVT_PCS_STATUS_DATA*)pArg;
        app_utils_logRemotePcsStatus(poStatusDat, nRemoteNum);
        break;
    }

    default:
    {
        break;
    }
    }
}

ADI_MEM_CODE_CRIT
void adi_pal_appCbk(uint32_t nId, ADI_PAL_APP_EVT eEvent, const void* pData)
{
    switch (eEvent)
    {
    case ADI_PAL_EVT_FATAL_ERR:
    {
        if (gbOspBridgeMode)
        {
            gbOspBridgeInitialized = false;
            OspBridgeSetError("ADI E2B/OSP stack reported a fatal initialization error");
            break;
        }

        adi_pal_fatalError();
        break;
    }

    case ADI_PAL_EVT_CLI_TX:
    {
        const ADI_PAL_EVT_CLI_TXDATA* poTxData = (const ADI_PAL_EVT_CLI_TXDATA*)pData;
        app_utils_log(poTxData->pTxData, poTxData->nLength);
        break;
    }

    case ADI_PAL_EVT_CLI_RX:
    {
        const uint8_t* pnRxByte = (const uint8_t*)pData;
        app_utils_processRxByte(*pnRxByte);
        break;
    }

    default:
    {
        break;
    }
    }
}

/**
 * @brief Callback from the network layer
 *
 * @param nDevNum The device number for which this callback is for
 * @param eEvent The type of event this callback is for
 * @param pData The data corresponding to this event
 */
ADI_MEM_CODE_CRIT
static void adi_network_cbk(uint32_t nDevNum, ADI_NETWORK_EVT eEvent, void* pData)
{
    switch (eEvent)
    {
    case ADI_NETWORK_EVT_ETH10_RX_RECEIVED:
    {
        ADI_NETWORK_EVT_ETH10_RX_RECEIVED_DATA* poRxDat = (ADI_NETWORK_EVT_ETH10_RX_RECEIVED_DATA*)pData;
#ifdef SNIFFER_MODE_EN
        /* Send all the received frames (from T1S) to the Ethernet in sniffer mode */
        ADI_NETWORK_FRAME_CFG oNwFrameCfg;
        (void)ADI_MEMSET(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));

        // ~~~~~ Network frame configurations ~~~~~
        oNwFrameCfg.bDuplicate = false;
        oNwFrameCfg.bLenInclMacHdr = true;
        oNwFrameCfg.eMacRoutingInfo = ADI_MACPHY_ROUTE_PORT0_LP;
        oNwFrameCfg.eTsReg = ADI_TS_EGRESS_NONE;
        oNwFrameCfg.nFrameType = ADI_E2BCORE_ETYPE;
        oNwFrameCfg.nLenByte = poRxDat->nLength;
        oNwFrameCfg.panPhysAddr = NULL;

        uint8_t* pBuffer = NULL;

        /* Get a buffer from network layer */
        ADI_NETWORK_ERR eNwErr = adi_network_getTxBuffer(ADI_NETWORK_ETH_IDX, &pBuffer, &oNwFrameCfg);

        /* Check if buffer is available */
        if (eNwErr == ADI_NETWORK_OK)
        {
            /* Copy the frame data */
            ADI_MEMCPY(pBuffer, poRxDat->pRxData, poRxDat->nLength);

            /* Transmit the Ethernet frame */
            adi_network_transmit(ADI_NETWORK_ETH_IDX, &oNwFrameCfg);
        }

#endif
        uint16_t nEthertype = (poRxDat->pRxData[13]) | (poRxDat->pRxData[12] << 8U);

        // Process if it's E2B frame
        if (nEthertype == ADI_E2BCORE_ETYPE)
        {
            adi_e2bcore_processFrame(0U, poRxDat->pRxData, poRxDat->nLength);
        }

        break;
    }

    case ADI_NETWORK_EVT_ETH_RX_RECEIVED:
    {
        /* Nothing to do */
        break;
    }

    case ADI_NETWORK_EVT_TX_SUCCESS:
    {
        /* Nothing to do */
        break;
    }

    case ADI_NETWORK_EVT_MAC_STATUS:
    {
        /* Stats data */
        ADI_NETWORK_EVT_MAC_STATUS_DATA* poStatusDat = (ADI_NETWORK_EVT_MAC_STATUS_DATA*)pData;
        app_utils_logCtrlMacStatus(poStatusDat);
        break;
    }

    case ADI_NETWORK_EVT_PHY_STATUS:
    {
        ADI_NETWORK_EVT_PHY_STATUS_DATA* poStatusDat = (ADI_NETWORK_EVT_PHY_STATUS_DATA*)pData;
        app_utils_logCtrlPhyStatus(poStatusDat);
        break;
    }

    case ADI_NETWORK_EVT_PLCA_STATUS:
    {
        ADI_NETWORK_EVT_PLCA_STATUS_DATA* poStatusDat = (ADI_NETWORK_EVT_PLCA_STATUS_DATA*)pData;
        app_utils_logCtrlPlcaStatus(poStatusDat);
        break;
    }

    case ADI_NETWORK_EVT_PTP_STATUS:
    {
        ADI_NETWORK_EVT_PTP_STATUS_DATA* poStatusDat = (ADI_NETWORK_EVT_PTP_STATUS_DATA*)pData;
        app_utils_logCtrlPtpStatus(poStatusDat);
        break;
    }

    case ADI_NETWORK_EVT_PCS_STATUS:
    {
        ADI_NETWORK_EVT_PCS_STATUS_DATA* poStatusDat = (ADI_NETWORK_EVT_PCS_STATUS_DATA*)pData;
        app_utils_logCtrlPcsStatus(poStatusDat);
        break;
    }

    case ADI_NETWORK_EVT_ERR:
    {
        /* Error event in the network */
        ADI_NETWORK_EVT_ERR_DATA* poNwErrDat = (ADI_NETWORK_EVT_ERR_DATA*)pData;
        ADI_PAL_LOG(ADI_CONSOLE_PROMPT, "Network error: %d\r\n", poNwErrDat->eErrType);
        ADI_DBG_ERROR();
        break;
    }

    default:
    {
        /* nothing to do */
        break;
    }
    }
}

/** RX command callback */
ADI_MEM_CODE_CRIT
void app_utils_rxCmdCbk(uint8_t* anRxData, uint32_t nLength)
{
    bool bCmdProcessed = app_utils_parseRxCommand(anRxData, nLength, &goAppData.eAppCmd, &goAppData.eAppMode);

    if (!bCmdProcessed)
    {
        goAppData.eAppCmd = APP_CMD_INVALID;
    }
}

/* =========================================================================
 * OspBridge_GetAd3304Status
 *
 * discovery ?꾨즺 ??gbAd3304Connected[] 諛곗뿴??臾몄옄?대줈 諛섑솚?⑸땲??
 * C# ?먯꽌 Init 吏곹썑 ?몄텧?섏뿬 ?대뒓 FID 媛 ?곌껐?먮뒗吏 吏꾨떒?⑸땲??
 *
 * 諛섑솚 ?뺤떇 (FID 0x01 ~ 0x08):
 *   "FID01:O FID02:X FID03:X FID04:O FID05:X FID06:X FID07:X FID08:X"
 *   O = ?곌껐?? X = 誘몄뿰寃? * ========================================================================= */
OSP_BRIDGE_API const char* OSP_BRIDGE_CALL OspBridge_GetAd3304Status(void)
{
    /* "FID01:O " 횞 8 = 理쒕? 72??+ ?ъ쑀 */
    static char sBuf[2048];
    char* p = sBuf;
    for (uint32_t i = 0U; i < 8U; i++)
    {
        uint8_t anMac[6U] = {0U, 0U, 0U, 0U, 0U, 0U};
        const char* pDisc = gbAd3304Connected[i] ? "OK " : "NO ";
        const char* pFio = ((gaFidFioDiag[i].nInitBidir == 0) &&
                            (gaFidFioDiag[i].nSetSetup == 0) &&
                            (gaFidFioDiag[i].nSetState == 0) &&
                            (gahFidFioOspHandle[i] != NULL)) ? "ACTIVE" : "FAIL";
        const char* pIlas = IlasLed_IsActive(i) ? "ACTIVE" : "FAIL";

        if (gbAd3304Connected[i])
        {
            /* Discovery callback stores each MAC in its FID slot (FID-1), not in
             * a packed 0..nMacCnt-1 list. Search all slots so sparse FIDs such
             * as 01/02/03/05 still report the correct MAC address. */
            for (uint32_t nMapIdx = 0U; nMapIdx < NUM_REMOTE_NODES; nMapIdx++)
            {
                if (ganRemoteMacAddrMap[nMapIdx][5U] == (uint8_t)(i + 1U))
                {
                    (void)memcpy(anMac, &ganRemoteMacAddrMap[nMapIdx][0U], 6U);
                    break;
                }
            }
        }

        int n = snprintf(p, (size_t)(sBuf + sizeof(sBuf) - p),
                         "FID%02u: MAC=%02X:%02X:%02X:%02X:%02X:%02X  DISC=%s  FIO=%s  ILAS=%s\n",
                         (unsigned)(i + 1U),
                         anMac[0], anMac[1], anMac[2],
                         anMac[3], anMac[4], anMac[5],
                         pDisc, pFio, pIlas);
        if (n > 0) { p += n; }
    }
    /* ??怨듬갚 ?쒓굅 */
    if (p > sBuf && *(p - 1) == ' ') { *(p - 1) = '\0'; }
    return sBuf;
}

/** @} */

/*
* EOF: https://www.analog.com/
*/





