using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.IO;

namespace LINMaster.FT4222
{
    // =========================================================================
    // FT4222Manager – FT4222H USB-SPI 브리지를 통한 OSP LED 제어
    //
    // [연결 체계]
    //   PC  →  USB  →  FT4222H  →  SPI(CLK/MOSI/CS)  →  AD3306  →  AD3301  →  LED(RGB)
    //
    // [OSP 프레임 구조]  (ex_fioOsp/utils/fioOsp.c 포팅)
    //   Byte0 : Preamble(0xA0) | DevAddr[9:6]
    //   Byte1 : DevAddr[5:0]<<2 | PSI[2:1]
    //   Byte2 : PSI[0]<<7 | CMD[6:0]
    //   Byte3~N-1 : Payload
    //   ByteN : CRC (ADI_CRC-8, poly=0x2F)
    //
    // [PWM SET 명령 (0x4F) 페이로드]
    //   [3] R_LOW   [4] R_HIGH(bit7=DayMode)
    //   [5] G_LOW   [6] G_HIGH(bit7=DayMode)
    //   [7] B_LOW   [8] B_HIGH(bit7=DayMode)
    //
    // [초기화 시퀀스]
    //   1. RESET    (0x00) – 브로드캐스트(addr=0)
    //   2. INIT_BIDIR(0x02) – addr=1, 응답 6바이트 수신 → 디바이스 주소 확인
    //   3. GO_ACTIVE (0x05) – 확인된 주소
    //   4. SET_SETUP (0x4D) – CRC 활성, PWM 586Hz
    //   5. SET_PWM   (0x4F) – 원하는 색상
    //
    // =========================================================================
    // ★ 수정 이력 (버그 수정)
    // ─────────────────────────────────────────────────────────────────────────
    // [버그1 수정] FT4222_SetSuspendOut — _handle(Interface A)에 잘못 호출 제거
    //             + bool → [MarshalAs(UnmanagedType.Bool)] 마샬링 수정
    //   원인: C++ DLL의 BOOL은 4바이트 int이지만, C# bool은 기본 마샬링 시
    //         1바이트로 처리됨. 이로 인해 DLL 내부에서 파라미터 해석이 틀어져
    //         SetSuspendOut이 실제로 동작하지 않음.
    //         결과적으로 FT4222가 SPI 모드로 전환되지 않아
    //         SPIMaster_Init에서 INVALID_PARAMETER(코드=6) 반환.
    //         또한 SetSuspendOut은 Interface B(GPIO 포트)에만 호출해야 하며,
    //         Interface A(SPI 포트)에 호출하면 동일하게 코드=6 반환.
    //
    // [버그2 수정] FT4222_SPIMaster_SingleReadWrite isEndTransaction 동일 문제
    //   원인: 위와 동일한 bool 마샬링 오류.
    //         isEndTransaction이 false로 해석될 수 있어 CS 신호가
    //         올바르게 토글되지 않을 수 있음.
    //
    // [버그3 수정] FT4222_STATUS / FT_STATUS enum 전체 코드 정의
    //   원인: 기존 enum에 OK = 0 하나만 정의되어 있어
    //         에러 발생 시 숫자(6)로만 표시되어 진단 불가.
    // =========================================================================

    public class FT4222Manager : IDisposable
    {
        // ─── P/Invoke : ftd2xx.dll ──────────────────────────────────────────
        private const string FTD2XX = "ftd2xx.dll";
        private const string LIBFT4222 = "LibFT4222-64.dll";
        private const string OSP_BRIDGE = "OspBridge.dll";
        private const uint FT_OPEN_BY_SERIAL_NUMBER = 0x00000001;
        private const uint FT_OPEN_BY_DESCRIPTION = 0x00000002;

        // ─── 설정 파일 (MAC / NodeId 영구 저장) ──────────────────────────────
        private static readonly string SettingsPath = Path.Combine(
            AppDomain.CurrentDomain.BaseDirectory, "ad3301_settings.ini");

        private static void SaveSettings(ulong mac, byte nodeId)
        {
            try
            {
                File.WriteAllLines(SettingsPath, new[]
                {
                    "[AD3301]",
                    "MAC=" + mac.ToString(),
                    "NodeId=" + nodeId.ToString()
                });
            }
            catch { }
        }

        private static bool LoadSettings(out ulong mac, out byte nodeId)
        {
            mac = 0;
            nodeId = 0;
            if (!File.Exists(SettingsPath)) return false;
            try
            {
                foreach (string line in File.ReadAllLines(SettingsPath))
                {
                    if (line.StartsWith("MAC="))
                        ulong.TryParse(line.Substring(4), out mac);
                    else if (line.StartsWith("NodeId="))
                        byte.TryParse(line.Substring(7), out nodeId);
                }
                return mac != 0 || nodeId != 0;
            }
            catch { return false; }
        }

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_Init();

        /// <summary>
        /// LINMaster 방식: 고정 MAC(REMOTE1_MAC_ADDR)으로 discovery → Power-On 직후도 성공.
        /// NativeOsp 빌드 시 ex_fioOsp.c에 추가된 함수.
        /// </summary>
        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_InitFixed();

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_InitOtp();

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_SetColorRgb(byte r, byte g, byte b);

        // ── FID 기반 채널 선택 (AD3304 x8: FID 0x01~0x08) ─────
        // OspBridge.dll이 OspBridge_SetColorRgbByFid를 export하는 경우 사용.
        // export하지 않으면 EntryPointNotFoundException이 발생하므로
        // SetColorRgbByFid()에서 try/catch로 폴백 처리함.
        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_SetColorRgbByFid")]
        private static extern int OspBridge_SetColorRgbByFid_Native(byte fid, byte r, byte g, byte b);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_SetColorRgbByNode")]
        private static extern int OspBridge_SetColorRgbByNode_Native(byte fid, ushort nodeAddr, byte r, byte g, byte b);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_SetColorRgbByNodes")]
        private static extern int OspBridge_SetColorRgbByNodes_Native(
            byte fid,
            [In] ushort[] nodeAddrs,
            [In] byte[] reds,
            [In] byte[] greens,
            [In] byte[] blues,
            ushort count);
        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_SetFioColorRgbByNodes")]
        private static extern int OspBridge_SetFioColorRgbByNodes_Native(
            byte fid,
            [In] ushort[] nodeAddrs,
            [In] byte[] reds,
            [In] byte[] greens,
            [In] byte[] blues,
            ushort count);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_SetIlasColorRgbByNodes")]
        private static extern int OspBridge_SetIlasColorRgbByNodes_Native(
            byte fid,
            [In] ushort[] nodeAddrs,
            [In] byte[] reds,
            [In] byte[] greens,
            [In] byte[] blues,
            ushort count);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_GetIlasLedCount")]
        private static extern ushort OspBridge_GetIlasLedCount_Native();

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_Readback(out ushort rPwm, out ushort gPwm, out ushort bPwm, out double temperature);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_ReadbackByFid")]
        private static extern int OspBridge_ReadbackByFid_Native(byte fid, ushort nodeAddr,
            out ushort rPwm, out ushort gPwm, out ushort bPwm, out double temperature);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_ReadIlasTempByFid")]
        private static extern int OspBridge_ReadIlasTempByFid_Native(byte fid, byte nodeAddr,
            out byte data0, out byte data1, out byte data2, out ushort validResp, out ushort expectedResp);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern void OspBridge_Close();

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr OspBridge_GetLastError();

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_GetFioStatus")]
        private static extern IntPtr OspBridge_GetFioStatus_Native();

        /// <summary>FIO0 Init 단계 status 코드를 반환 (진단용, IF0/IF1 채널)</summary>
        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_GetFioDiag")]
        private static extern IntPtr OspBridge_GetFioDiag_Native();

        /// <summary>FID별 상세 FIO 초기화 진단 문자열을 반환</summary>
        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_GetFioDiagAll")]
        private static extern IntPtr OspBridge_GetFioDiagAll_Native();

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_GetFioTrace")]
        private static extern IntPtr OspBridge_GetFioTrace_Native();

        /// <summary>AD3304 8개 연결 상태 문자열을 반환 (예: "FID01:O|FID02:X|...")</summary>
        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall, EntryPoint = "OspBridge_GetAd3304Status")]
        private static extern IntPtr OspBridge_GetAd3304Status_Native();

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_GetMacNode([Out] byte[] mac6, out byte nodeId);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_SetMac([In] byte[] mac6);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_SetNodeId(byte nodeId);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_ApplyMacNodeNoReinit([In] byte[] mac6, byte nodeId);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_ApplyFidOffsetNoReinit(byte offset);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_ApplyMacNodeMapNoReinit(
            [In] byte[] oldFids,
            [In] byte[] mac6List,
            [In] byte[] nodeIds,
            byte count);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_ProgramMacNodeOtp([In] byte[] mac6, byte nodeId);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_ReadMacNodeOtp(
            [Out] byte[] mac6, out byte nodeId,
            out byte bootStatusMac, out byte bootStatusPlca,
            [Out] byte[] rawDesigner);

        [DllImport(OSP_BRIDGE, CallingConvention = CallingConvention.StdCall)]
        private static extern int OspBridge_ReadMacNodeOtpByFid(
            byte accessFid,
            [Out] byte[] mac6, out byte nodeId,
            out byte bootStatusMac, out byte bootStatusPlca,
            [Out] byte[] rawDesigner);

        [DllImport(FTD2XX)] private static extern FT_STATUS FT_CreateDeviceInfoList(out uint numDevs);
        [DllImport(FTD2XX)]
        private static extern FT_STATUS FT_GetDeviceInfoDetail(
            uint index, out uint flags, out uint type, out uint id,
            out uint locId, [MarshalAs(UnmanagedType.LPStr)] System.Text.StringBuilder sn,
            [MarshalAs(UnmanagedType.LPStr)] System.Text.StringBuilder desc, out IntPtr handle);
        [DllImport(FTD2XX)]
        private static extern FT_STATUS FT_OpenEx(
            [MarshalAs(UnmanagedType.LPStr)] string arg, uint flags, out IntPtr handle);
        [DllImport(FTD2XX, EntryPoint = "FT_OpenEx")]
        private static extern FT_STATUS FT_OpenExByLocation(
            IntPtr arg, uint flags, out IntPtr handle);
        [DllImport(FTD2XX)] private static extern FT_STATUS FT_Close(IntPtr handle);
        [DllImport(FTD2XX)] private static extern FT_STATUS FT_SetTimeouts(IntPtr handle, uint readTimeout, uint writeTimeout);

        [DllImport(LIBFT4222)]
        private static extern FT4222_STATUS FT4222_SPIMaster_Init(
            IntPtr ftHandle, SpiMode ioLine, ClkDiv clkDiv, CPOL cpol, CPHA cpha, byte ssnActiveLow);

        // ★ [버그2 수정] isEndTransaction: bool → [MarshalAs(UnmanagedType.Bool)] bool
        //   C++ BOOL(4바이트 int)과 C# bool(1바이트)의 크기 불일치를 해소
        [DllImport(LIBFT4222)]
        private static extern FT4222_STATUS FT4222_SPIMaster_SingleReadWrite(
            IntPtr ftHandle, byte[] readBuf, byte[] writeBuf, ushort sizeToTransfer,
            out ushort sizeTransferred,
            [MarshalAs(UnmanagedType.Bool)] bool isEndTransaction);   // ★ 수정

        [DllImport(LIBFT4222)] private static extern FT4222_STATUS FT4222_UnInitialize(IntPtr ftHandle);
        [DllImport(LIBFT4222)] private static extern FT4222_STATUS FT4222_SetClock(IntPtr ftHandle, SYS_CLK clk);
        [DllImport(LIBFT4222)] private static extern FT4222_STATUS FT4222_ChipReset(IntPtr ftHandle);

        // ★ [버그1 수정] enable: bool → [MarshalAs(UnmanagedType.Bool)] bool
        //   C++ 원형: FT4222_STATUS WINAPI FT4222_SetSuspendOut(FT_HANDLE ftHandle, BOOL enable);
        //   BOOL = Win32 typedef int → 4바이트.  C# bool 기본 마샬링은 1바이트이므로
        //   [MarshalAs(UnmanagedType.Bool)] 로 4바이트 정수로 변환해야 함.
        //   이 선언이 잘못되면 SetSuspendOut이 동작하지 않아 FT4222가
        //   SPI 모드로 전환되지 않고 SPIMaster_Init에서 INVALID_PARAMETER(6) 반환.
        [DllImport(LIBFT4222)]
        private static extern FT4222_STATUS FT4222_SetSuspendOut(
            IntPtr ftHandle,
            [MarshalAs(UnmanagedType.Bool)] bool enable);             // ★ 수정

        [DllImport(LIBFT4222)]
        private static extern FT4222_STATUS FT4222_GPIO_Init(
            IntPtr ftHandle, GPIO_Dir[] gpioDir);

        // GPIO_Write: FID 기반 IF 채널 CS 핀 제어용
        [DllImport(LIBFT4222)]
        private static extern FT4222_STATUS FT4222_GPIO_Write(
            IntPtr ftHandle, uint portNum,
            [MarshalAs(UnmanagedType.Bool)] bool bValue);

        private enum GPIO_Dir { GPIO_OUTPUT = 0, GPIO_INPUT = 1 }

        // ─── 열거형 ──────────────────────────────────────────────────────────

        // ★ [버그3 수정] FT_STATUS 전체 코드 정의
        private enum FT_STATUS
        {
            FT_OK = 0,
            FT_INVALID_HANDLE = 1,
            FT_DEVICE_NOT_FOUND = 2,
            FT_DEVICE_NOT_OPENED = 3,
            FT_IO_ERROR = 4,
            FT_INSUFFICIENT_RESOURCES = 5,
            FT_INVALID_PARAMETER = 6,
            FT_INVALID_BAUD_RATE = 7,
            FT_DEVICE_NOT_OPENED_FOR_ERASE = 8,
            FT_DEVICE_NOT_OPENED_FOR_WRITE = 9,
            FT_FAILED_TO_WRITE_DEVICE = 10,
            FT_EEPROM_READ_FAILED = 11,
            FT_EEPROM_WRITE_FAILED = 12,
            FT_EEPROM_ERASE_FAILED = 13,
            FT_EEPROM_NOT_PRESENT = 14,
            FT_EEPROM_NOT_PROGRAMMED = 15,
            FT_INVALID_ARGS = 16,
            FT_NOT_SUPPORTED = 17,
            FT_OTHER_ERROR = 18,
        }

        // ★ [버그3 수정] FT4222_STATUS 전체 코드 정의
        //   에러코드 6 = FT4222_INVALID_PARAMETER
        //   → FT4222_SetSuspendOut이 정상 동작하지 않거나
        //     Interface A(SPI)에 잘못 호출하면 SPIMaster_Init 시 이 코드 반환.
        private enum FT4222_STATUS
        {
            FT4222_OK = 0,
            FT4222_INVALID_HANDLE = 1,
            FT4222_DEVICE_NOT_FOUND = 2,
            FT4222_DEVICE_NOT_OPENED = 3,
            FT4222_IO_ERROR = 4,
            FT4222_INSUFFICIENT_RESOURCES = 5,
            FT4222_INVALID_PARAMETER = 6,   // ← SPIMaster_Init 실패 시 반환됐던 코드
            FT4222_INVALID_BAUD_RATE = 7,
            FT4222_DEVICE_NOT_OPENED_FOR_ERASE = 8,
            FT4222_DEVICE_NOT_OPENED_FOR_WRITE = 9,
            FT4222_FAILED_TO_WRITE_DEVICE = 10,
            FT4222_EEPROM_READ_FAILED = 11,
            FT4222_EEPROM_WRITE_FAILED = 12,
            FT4222_EEPROM_ERASE_FAILED = 13,
            FT4222_EEPROM_NOT_PRESENT = 14,
            FT4222_EEPROM_NOT_PROGRAMMED = 15,
            FT4222_INVALID_ARGS = 16,
            FT4222_NOT_SUPPORTED = 17,
            FT4222_OTHER_ERROR = 18,
            FT4222_DEVICE_LIST_NOT_READY = 19,
            // FT4222H 전용 확장 코드
            FT4222_DEVICE_NOT_SUPPORTED = 1000,
            FT4222_CLK_NOT_SUPPORTED = 1001,
            FT4222_VENDER_CMD_NOT_SUPPORTED = 1002,
            FT4222_IS_NOT_SPI_MODE = 1003,
            FT4222_IS_NOT_I2C_MODE = 1004,
            FT4222_IS_NOT_SPI_SINGLE_MODE = 1005,
            FT4222_IS_NOT_SPI_MULTI_MODE = 1006,
            FT4222_WRONG_I2C_ADDR = 1007,
            FT4222_INVAILD_FUNCTION = 1008,
            FT4222_INVALID_POINTER = 1009,
            FT4222_EXCEEDED_MAX_TRANSFER_SIZE = 1010,
            FT4222_FAILED_TO_READ_DEVICE = 1011,
            FT4222_I2C_NOT_SUPPORTED_IN_THIS_MODE = 1012,
            FT4222_GPIO_NOT_SUPPORTED_IN_THIS_MODE = 1013,
            FT4222_GPIO_EXCEEDED_MAX_PORTNUM = 1014,
            FT4222_GPIO_WRITE_NOT_SUPPORTED = 1015,
            FT4222_GPIO_PULLUP_INVALID_IN_INPUTMODE = 1016,
            FT4222_GPIO_PULLDOWN_INVALID_IN_OUTPUTMODE = 1017,
            FT4222_GPIO_OPEN_DRAIN_INVALID_IN_OUTPUTMODE = 1018,
            FT4222_INTERRUPT_NOT_SUPPORTED = 1019,
            FT4222_GPIO_INPUT_NOT_SUPPORTED = 1020,
            FT4222_EVENT_NOT_SUPPORTED = 1021,
            FT4222_FUN_NOT_SUPPORT = 1022,
        }

        private enum SpiMode { NONE = 0, SINGLE = 1, DUAL = 2, QUAD = 4 }
        private enum ClkDiv
        {
            CLK_NONE = 0,
            CLK_DIV_2 = 1,
            CLK_DIV_4 = 2,
            CLK_DIV_8 = 3,
            CLK_DIV_16 = 4,
            CLK_DIV_32 = 5,
            CLK_DIV_64 = 6,
            CLK_DIV_128 = 7,
            CLK_DIV_256 = 8,
            CLK_DIV_512 = 9,
        }
        private enum CPOL { CLK_IDLE_LOW = 0, CLK_IDLE_HIGH = 1 }
        private enum CPHA { CLK_LEADING = 0, CLK_TRAILING = 1 }
        private enum SYS_CLK { SYS_CLK_60 = 0, SYS_CLK_24 = 1, SYS_CLK_48 = 2, SYS_CLK_80 = 3 }

        // ─── OSP 명령 코드 ────────────────────────────────────────────────────
        private const byte CMD_RESET = 0x00;
        private const byte CMD_INIT_BIDIR = 0x02;
        private const byte CMD_GO_ACTIVE = 0x05;
        private const byte CMD_SET_SETUP = 0x4D;
        private const byte CMD_SET_PWM = 0x4F;
        private const byte PREAMBLE = 0xA0;
        private const ushort BROADCAST_ADDR = 0;
        private const ushort INIT_ADDR = 1;

        // ─── 상태 ─────────────────────────────────────────────────────────────
        private IntPtr _handle = IntPtr.Zero;   // Interface A (SPI)
        private IntPtr _handleGpio = IntPtr.Zero;   // Interface B (GPIO)
        private bool _initialized = false;
        private bool _nativeOspBridge = false;
        private ushort _deviceAddr = 1;             // INIT_BIDIR 후 확인된 디바이스 주소
        private bool _disposed = false;

        public bool IsConnected => _handle != IntPtr.Zero && _initialized;
        private static bool UseNativeOspBridge => true;

        /// <summary>마지막 실패 원인 메시지 (UI 표시용)</summary>
        public string LastError { get; private set; } = string.Empty;
        public event Action<string> OspLog;

        // =========================================================================
        // 장치 탐색
        // =========================================================================

        /// <summary>연결된 FT4222 장치 목록 반환 (Interface A — SPI용만, UI 표시용)</summary>
        public static List<string> EnumerateDevices()
        {
            var list = new List<string>();
            try
            {
                FT_STATUS st = FT_CreateDeviceInfoList(out uint num);
                if (st != FT_STATUS.FT_OK) return list;
                for (uint i = 0; i < num; i++)
                {
                    var sn = new System.Text.StringBuilder(64);
                    var desc = new System.Text.StringBuilder(64);
                    FT_GetDeviceInfoDetail(i, out _, out _, out _, out uint locId, sn, desc, out _);
                    string d = desc.ToString();
                    if (IsFT4222InterfaceA(d))
                        list.Add(d + " Loc=0x" + locId.ToString("X") + " [" + sn.ToString() + "]");
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("[FT4222] EnumerateDevices 오류: " + ex.Message);
            }
            return list;
        }

        /// <summary>FT4222 SPI 인터페이스(A) 여부 판정</summary>
        private static bool IsFT4222InterfaceA(string desc)
        {
            if (string.IsNullOrEmpty(desc)) return false;
            if (!desc.Contains("FT4222") && !desc.Contains("4222")) return false;
            // 다중 인터페이스 모드: "FT4222 A"(SPI), "FT4222 B"(GPIO)
            // " B"로 끝나면 GPIO → 제외
            return !desc.TrimEnd().EndsWith(" B", StringComparison.OrdinalIgnoreCase);
        }

        /// <summary>
        /// UI 표시용 문자열 "FT4222 A [SN]" 에서
        /// FT_OpenEx에 넘길 순수 Description "FT4222 A" 만 추출
        /// </summary>
        private static string ExtractDescription(string displayName)
        {
            if (string.IsNullOrEmpty(displayName)) return displayName;
            int bracket = displayName.LastIndexOf(" [");
            return bracket >= 0 ? displayName.Substring(0, bracket).TrimEnd() : displayName;
        }

        private static string ExtractSerial(string displayName)
        {
            if (string.IsNullOrEmpty(displayName)) return null;
            int start = displayName.LastIndexOf('[');
            int end = displayName.LastIndexOf(']');
            if (start < 0 || end <= start) return null;
            string serial = displayName.Substring(start + 1, end - start - 1).Trim();
            return serial.Length == 0 ? null : serial;
        }

        // =========================================================================
        // 연결 / 해제
        // =========================================================================

        // AD3301 Power-On 직후 기본 MAC 주소 (고정값) = REMOTE1_MAC_ADDR
        public const ulong CONTROLLER_MAC = 0x00E022FE70F0UL;
        private const ulong FIXED_REMOTE_MAC = 0x00E022FE7000UL;
        public bool UseOtpMacMode { get; set; }
        public bool TxRxFeedbackEnabled { get; set; } = false;
        private DateTime _lastFioRxFeedbackUtc = DateTime.MinValue;
        private DateTime _lastIlasRxFeedbackUtc = DateTime.MinValue;
        private const int TX_RX_FEEDBACK_MIN_INTERVAL_MS = 200;

        private void ResetNativeTraceLogs()
        {
            string baseDir = AppDomain.CurrentDomain.BaseDirectory;
            string projectDir = Path.GetFullPath(Path.Combine(baseDir, @"..\..\"));
            string[] paths =
            {
                Path.Combine(projectDir, "osp_spi_trace.log"),
                Path.Combine(projectDir, "osp_init_stage.log"),
                Path.Combine(baseDir, "osp_spi_trace.log"),
                Path.Combine(baseDir, "osp_init_stage.log"),
                Path.Combine(Environment.CurrentDirectory, "osp_spi_trace.log"),
                Path.Combine(Environment.CurrentDirectory, "osp_init_stage.log"),
                Path.Combine(Path.GetTempPath(), "ospbridge_trace_debug.txt")
            };

            var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            foreach (string path in paths)
            {
                if (string.IsNullOrWhiteSpace(path) || !seen.Add(path))
                    continue;

                try
                {
                    File.WriteAllText(path, string.Empty);
                }
                catch
                {
                    // Trace files are diagnostic only; connection must continue even if one is locked.
                }
            }
        }

        /// <summary>
        /// [연결 전략] OspBridge_InitFixed() 한 번으로 모든 케이스 처리
        ///   - Power-On 직후 (기본 MAC ..00) : 고정 MAC으로 PLCA 설정 → discovery 성공
        ///   - HW MAC 변경 (예: ..08)        : 고정 MAC으로 PLCA 설정 → discovery에서 실제 MAC 수신
        ///                                    → 실제 MAC으로 PLCA ID 재설정 → 연결 완료
        /// InitFixed 성공 시점에 실제 MAC으로 연결이 완료되므로 추가 재연결 불필요.
        /// </summary>
        private bool TryConnectOspBridge()
        {
            if (UseOtpMacMode)
                OspLog?.Invoke("[FT4222] Connect mode = OTP MAC/Node");
            else
                OspLog?.Invoke("[FT4222] Connect mode = IF/FID input");
            ResetNativeTraceLogs();
            // OspBridge_InitFixed() 내부(CreateE2bAppFixed)에서:
            //   고정 MAC(..00)으로 setRemotePlcaId → setPlcaEn → startDiscovery
            //   콜백: 실제 수신 MAC으로 ganRemoteMacAddrMap 갱신 + writeMacAddr
            //   실제 MAC ≠ 고정 MAC 이면 실제 MAC 기준 setRemotePlcaId 재설정
            // → InitFixed 성공 시점에 실제 MAC(기본이든 HW 변경이든)으로 연결 완료.
            const int discoveryRetryCount = 3;
            int stFixed = -2;
            for (int attempt = 1; attempt <= discoveryRetryCount; attempt++)
            {
                OspLog?.Invoke(string.Format(
                    "[FT4222] OspBridge_InitFixed 시도 {0}/{1}...",
                    attempt, discoveryRetryCount));
                stFixed = OspBridge_InitFixed();
                if (stFixed == 0 || stFixed != -2)
                    break;

                if (attempt < discoveryRetryCount)
                {
                    OspLog?.Invoke("[FT4222] Discovery 응답 없음, 300ms 후 재시도...");
                    System.Threading.Thread.Sleep(300);
                }
            }
            if (stFixed != 0)
            {
                LastError = "OspBridge_InitFixed 실패: " + stFixed + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[FT4222] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            OspLog?.Invoke("[FT4222] OspBridge_InitFixed 성공");

            // AD3304 연결 상태 + FIO 활성 상태 출력 (C DLL 요약 문자열)
            if (UseOtpMacMode && !ApplyOtpMacNodeAfterInit())
                return false;

            string ad3304Status = GetAd3304Status();
            OspLog?.Invoke("[AD3304] === 연결/FIO 상태 ===");
            foreach (var line in ad3304Status.Split('\n'))
            {
                if (!string.IsNullOrWhiteSpace(line))
                    OspLog?.Invoke("  " + line.Trim());
            }

            // FIO 초기화 결과와 FID별 상태 요약은 연결 확인용으로 유지한다.
            // 실제 MAC 읽어서 로그/저장 (연결은 이미 완료된 상태)
            byte[] macBuf = new byte[6];
            byte nodeId;
            int stGet = OspBridge_GetMacNode(macBuf, out nodeId);
            ulong actualMac = stGet == 0
                ? ((ulong)macBuf[0] << 40 | (ulong)macBuf[1] << 32 | (ulong)macBuf[2] << 24
                  | (ulong)macBuf[3] << 16 | (ulong)macBuf[4] << 8 | macBuf[5])
                : 0;

            if (actualMac != 0)
                SaveSettings(actualMac, nodeId);

            if (actualMac == 0 || actualMac == FIXED_REMOTE_MAC)
                OspLog?.Invoke(string.Format("[FT4222] Master MAC={0:X12} / Node MAC={1:X12} (기본 MAC) → 연결 완료",
                    CONTROLLER_MAC, actualMac));
            else
                OspLog?.Invoke(string.Format("[FT4222] Master MAC={0:X12} / Node MAC={1:X12} (HW 변경 MAC) → 연결 완료",
                    CONTROLLER_MAC, actualMac));

            return true;
        }

        /// <summary>Init 성공 직후: GetMacNode로 실제 MAC을 읽어 ini에 저장합니다.</summary>
        private bool ApplyOtpMacNodeAfterInit()
        {
            List<byte> activeFids = GetActiveFids();
            if (activeFids.Count > 0)
                return ApplyOtpMacNodeMapAfterInit(activeFids);

            if (!ReadMacNodeOtpNative(out ulong otpMac, out byte otpNode,
                    out byte bootMac, out byte bootPlca))
                return false;

            if (otpMac == 0 || otpNode == 0 || otpNode > 254)
            {
                LastError = string.Format("OTP MAC/Node is empty or invalid: MAC={0:X12}, Node={1}", otpMac, otpNode);
                OspLog?.Invoke("[OTP ERR] " + LastError);
                return false;
            }

            byte macFid = (byte)(otpMac & 0xFF);
            if (macFid != otpNode)
            {
                LastError = string.Format("OTP MAC last byte and Node mismatch: MAC={0:X12}, Node={1}", otpMac, otpNode);
                OspLog?.Invoke("[OTP ERR] " + LastError);
                return false;
            }

            if (bootMac == 0 || bootPlca == 0)
            {
                OspLog?.Invoke(string.Format(
                    "[OTP WARN] bootStatus(MAC/PLCA)={0}/{1}. OTP value will still be applied.",
                    bootMac, bootPlca));
            }

            OspLog?.Invoke(string.Format("[OTP] Applying OTP MAC/Node to active network: MAC={0:X12} Node={1}",
                otpMac, otpNode));

            if (!ApplyMacNodeNoReinitNative(otpMac, otpNode))
                return false;

            SaveSettings(otpMac, otpNode);
            OspLog?.Invoke(string.Format("[OTP] Active MAC/Node now uses OTP value: MAC={0:X12} Node={1}",
                otpMac, otpNode));
            return true;
        }

        private bool ApplyOtpMacNodeMapAfterInit(List<byte> activeFids)
        {
            List<ulong> otpMacs = new List<ulong>();
            List<byte> otpNodes = new List<byte>();
            HashSet<byte> targetFids = new HashSet<byte>();

            OspLog?.Invoke(string.Format("[OTP] Reading OTP MAC/Node for {0} active FID(s)", activeFids.Count));
            foreach (byte physicalFid in activeFids)
            {
                if (!ReadMacNodeOtpByFidNative(physicalFid, out ulong otpMac, out byte otpNode,
                        out byte bootMac, out byte bootPlca))
                    return false;

                if (otpMac == 0 || otpNode == 0 || otpNode > 8)
                {
                    LastError = string.Format("OTP MAC/Node is empty or invalid at physical FID 0x{0:X2}: MAC={1:X12}, Node={2}",
                        physicalFid, otpMac, otpNode);
                    OspLog?.Invoke("[OTP ERR] " + LastError);
                    return false;
                }

                byte otpFid = (byte)(otpMac & 0xFF);
                if (otpFid != otpNode || otpFid < 1 || otpFid > 8)
                {
                    LastError = string.Format("OTP MAC last byte and Node mismatch at physical FID 0x{0:X2}: MAC={1:X12}, Node={2}",
                        physicalFid, otpMac, otpNode);
                    OspLog?.Invoke("[OTP ERR] " + LastError);
                    return false;
                }

                if (!targetFids.Add(otpFid))
                {
                    LastError = string.Format("Duplicate OTP target FID 0x{0:X2}", otpFid);
                    OspLog?.Invoke("[OTP ERR] " + LastError);
                    return false;
                }

                if (bootMac == 0 || bootPlca == 0)
                {
                    OspLog?.Invoke(string.Format(
                        "[OTP WARN] Physical FID 0x{0:X2}: bootStatus(MAC/PLCA)={1}/{2}. OTP value will still be applied.",
                        physicalFid, bootMac, bootPlca));
                }

                OspLog?.Invoke(string.Format("[OTP] Physical FID 0x{0:X2} -> OTP MAC={1:X12} Node={2}",
                    physicalFid, otpMac, otpNode));
                otpMacs.Add(otpMac);
                otpNodes.Add(otpNode);
            }

            if (!ApplyMacNodeMapNoReinitNative(activeFids, otpMacs, otpNodes))
                return false;

            if (otpMacs.Count > 0)
                SaveSettings(otpMacs[0], otpNodes[0]);

            OspLog?.Invoke("[OTP] Active FIDs now use each node's OTP MAC/Node map");
            return true;
        }

        private bool ApplyFidOffsetNoReinitNative(byte offset, int activeCount)
        {
            OspLog?.Invoke(string.Format("[OTP] Applying multi-node FID offset +{0} to {1} active nodes", offset, activeCount));
            int st = OspBridge_ApplyFidOffsetNoReinit(offset);
            if (st != 0)
            {
                LastError = "OspBridge_ApplyFidOffsetNoReinit failed: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            OspLog?.Invoke(string.Format("[OTP] Active FIDs remapped by +{0}", offset));
            return true;
        }

        private bool ReadMacNodeOtpByFidNative(byte physicalFid, out ulong mac, out byte nodeId,
            out byte bootStatusMac, out byte bootStatusPlca)
        {
            mac = 0;
            nodeId = 0;
            bootStatusMac = 0;
            bootStatusPlca = 0;

            byte[] buf = new byte[6];
            byte[] raw = new byte[8 * 16];
            int st = OspBridge_ReadMacNodeOtpByFid(physicalFid, buf, out nodeId,
                out bootStatusMac, out bootStatusPlca, raw);
            if (st != 0)
            {
                LastError = string.Format("OspBridge_ReadMacNodeOtpByFid(FID=0x{0:X2}) failed: {1}\r\n{2}",
                    physicalFid, st, GetNativeBridgeError());
                OspLog?.Invoke("[OTP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }

            mac = ((ulong)buf[0] << 40)
                | ((ulong)buf[1] << 32)
                | ((ulong)buf[2] << 24)
                | ((ulong)buf[3] << 16)
                | ((ulong)buf[4] << 8)
                | (ulong)buf[5];

            return true;
        }

        private bool ReadMacNodeOtpNative(out ulong mac, out byte nodeId,
            out byte bootStatusMac, out byte bootStatusPlca)
        {
            mac = 0;
            nodeId = 0;
            bootStatusMac = 0;
            bootStatusPlca = 0;

            byte[] buf = new byte[6];
            byte[] raw = new byte[8 * 16];
            int st = OspBridge_ReadMacNodeOtp(buf, out nodeId,
                out bootStatusMac, out bootStatusPlca, raw);
            if (st != 0)
            {
                LastError = "OspBridge_ReadMacNodeOtp failed: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OTP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }

            mac = ((ulong)buf[0] << 40)
                | ((ulong)buf[1] << 32)
                | ((ulong)buf[2] << 24)
                | ((ulong)buf[3] << 16)
                | ((ulong)buf[4] << 8)
                | (ulong)buf[5];

            OspLog?.Invoke(string.Format(
                "[OTP] ReadMacNodeOtp: MAC={0:X12} Node={1} bootStatus(MAC/PLCA)={2}/{3}",
                mac, nodeId, bootStatusMac, bootStatusPlca));
            return true;
        }

        private bool WriteMacNative(ulong mac)
        {
            byte[] buf = Mac64To8(mac);
            int st = OspBridge_SetMac(buf);
            if (st != 0)
            {
                LastError = "OspBridge_SetMac failed: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            return true;
        }

        private bool WriteNodeIdNative(byte nodeId)
        {
            int st = OspBridge_SetNodeId(nodeId);
            if (st != 0)
            {
                LastError = "OspBridge_SetNodeId failed: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            return true;
        }

        private bool ApplyMacNodeNoReinitNative(ulong mac, byte nodeId)
        {
            byte[] buf = Mac64To8(mac);
            int st = OspBridge_ApplyMacNodeNoReinit(buf, nodeId);
            if (st != 0)
            {
                LastError = "OspBridge_ApplyMacNodeNoReinit failed: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            return true;
        }

        private bool ApplyMacNodeMapNoReinitNative(List<byte> physicalFids, List<ulong> otpMacs, List<byte> otpNodes)
        {
            if (physicalFids == null || otpMacs == null || otpNodes == null ||
                physicalFids.Count == 0 || physicalFids.Count != otpMacs.Count || physicalFids.Count != otpNodes.Count)
            {
                LastError = "Invalid OTP MAC/Node map";
                OspLog?.Invoke("[OTP ERR] " + LastError);
                return false;
            }

            byte[] oldFids = physicalFids.ToArray();
            byte[] macList = new byte[otpMacs.Count * 6];
            byte[] nodeIds = otpNodes.ToArray();
            for (int i = 0; i < otpMacs.Count; i++)
            {
                byte[] mac6 = Mac64To8(otpMacs[i]);
                Buffer.BlockCopy(mac6, 0, macList, i * 6, 6);
            }

            OspLog?.Invoke("[OTP] Applying per-FID OTP MAC/Node map to active network");
            int st = OspBridge_ApplyMacNodeMapNoReinit(oldFids, macList, nodeIds, (byte)physicalFids.Count);
            if (st != 0)
            {
                LastError = "OspBridge_ApplyMacNodeMapNoReinit failed: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            LogOtpIlasAliasTrace();
            return true;
        }

        private void LogOtpIlasAliasTrace()
        {
            try
            {
                IntPtr p = OspBridge_GetFioTrace_Native();
                string trace = p == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(p);
                if (string.IsNullOrWhiteSpace(trace))
                    return;

                foreach (string line in trace.Split(new[] { "\r\n", "\n" }, StringSplitOptions.RemoveEmptyEntries))
                {
                    if (line.Contains("ILAS OTP alias"))
                        OspLog?.Invoke("[OTP] " + line.Trim());
                }
            }
            catch
            {
                // Diagnostic only.
            }
        }

        private void AfterInitSuccess()
        {
            try
            {
                byte[] buf = new byte[6];
                byte nodeId;
                int st = OspBridge_GetMacNode(buf, out nodeId);
                if (st == 0)
                {
                    ulong mac = ((ulong)buf[0] << 40) | ((ulong)buf[1] << 32)
                              | ((ulong)buf[2] << 24) | ((ulong)buf[3] << 16)
                              | ((ulong)buf[4] << 8) | (ulong)buf[5];
                    SaveSettings(mac, nodeId);
                    OspLog?.Invoke(string.Format("[FT4222] MAC={0:X12} NodeId={1} → ini 저장 완료", mac, nodeId));
                }
            }
            catch { }
        }

        /// <summary>ulong MAC을 byte[6] (MSB first) 으로 변환합니다.</summary>
        private static byte[] Mac64To8(ulong mac)
        {
            byte[] b = new byte[6];
            for (int i = 0; i < 6; i++)
                b[i] = (byte)((mac >> (40 - 8 * i)) & 0xFF);
            return b;
        }

        /// <summary>FT4222 열기 + SPI Master 초기화 + OSP 디바이스 초기화</summary>
        public bool Connect(string deviceDescription = null)
        {
            if (IsConnected) return true;
            LastError = string.Empty;
            try
            {
                if (UseNativeOspBridge)
                {
                    if (TryConnectOspBridge())
                    {
                        _nativeOspBridge = true;
                        _initialized = true;
                        _handle = new IntPtr(1);
                        return true;
                    }
                    return false;
                }

                // 첫 번째 FT4222 Interface A 장치 자동 선택
                string openArg = deviceDescription ?? FindFirstFT4222();
                if (openArg == null)
                {
                    LastError = "FT4222 장치를 찾을 수 없습니다.\r\n" +
                                "USB 연결 및 드라이버 설치 상태를 확인하세요.";
                    return false;
                }

                // UI 표시용 문자열 "FT4222 A [SN]"에서 Serial을 우선 사용하고,
                // Serial이 없을 때만 Description으로 연다.
                string desc = ExtractDescription(openArg);
                string serial = ExtractSerial(openArg);

                FT_STATUS ftSt = serial != null
                    ? FT_OpenEx(serial, FT_OPEN_BY_SERIAL_NUMBER, out _handle)
                    : FT_OpenEx(desc, FT_OPEN_BY_DESCRIPTION, out _handle);
                if (ftSt != FT_STATUS.FT_OK)
                {
                    LastError = $"FT_OpenEx 실패: {ftSt}\r\n장치: {openArg}";
                    _handle = IntPtr.Zero;
                    return false;
                }

                // Interface B(GPIO) 핸들 열기 — "FT4222 A" → "FT4222 B" 치환
                string descB = desc.TrimEnd().EndsWith(" A", StringComparison.OrdinalIgnoreCase)
                    ? desc.TrimEnd().Substring(0, desc.TrimEnd().Length - 2).TrimEnd() + " B"
                    : null;
                if (descB != null)
                    FT_OpenEx(descB, FT_OPEN_BY_DESCRIPTION, out _handleGpio); // 실패해도 SPI는 계속 진행

                // ★ ex_fioOsp(adi_pal_gui.c)와 동일한 초기화 순서:
                //   FT_SetTimeouts → FT4222_SetClock → FT4222_SPIMaster_Init
                //   SetTimeouts 누락 시 INVALID_PARAMETER(6) 발생 가능

                FT_STATUS toSt = FT_SetTimeouts(_handle, 1000, 1000);
                System.Diagnostics.Debug.WriteLine($"[FT4222] SetTimeouts: {toSt}");
                if (toSt != FT_STATUS.FT_OK)
                {
                    LastError = $"FT_SetTimeouts 실패: {toSt}";
                    FT_Close(_handle); _handle = IntPtr.Zero;
                    if (_handleGpio != IntPtr.Zero) { FT_Close(_handleGpio); _handleGpio = IntPtr.Zero; }
                    return false;
                }

                // ex_fioOsp 15MHz 설정과 동일: SYS_CLK_60 / CLK_DIV_4 = 15MHz
                FT4222_STATUS clkSt = FT4222_SetClock(_handle, SYS_CLK.SYS_CLK_60);
                System.Diagnostics.Debug.WriteLine($"[FT4222] SetClock: {clkSt}");

                FT4222_STATUS spiSt = FT4222_SPIMaster_Init(
                    _handle, SpiMode.SINGLE, ClkDiv.CLK_DIV_4,
                    CPOL.CLK_IDLE_LOW, CPHA.CLK_LEADING, 0x01);
                System.Diagnostics.Debug.WriteLine($"[FT4222] SPIMaster_Init: {spiSt}");

                if (spiSt != FT4222_STATUS.FT4222_OK)
                {
                    LastError = $"FT4222_SPIMaster_Init 실패: {spiSt} (코드={(int)spiSt})\r\n" +
                                "Interface A(SPI) 포트가 맞는지 확인하세요.";
                    FT_Close(_handle); _handle = IntPtr.Zero;
                    if (_handleGpio != IntPtr.Zero) { FT_Close(_handleGpio); _handleGpio = IntPtr.Zero; }
                    return false;
                }

                // ★ [버그1 수정 적용] GPIO 핸들(Interface B)에만 SetSuspendOut 호출
                //   [MarshalAs(UnmanagedType.Bool)] 덕분에 C++ BOOL(4바이트)로 올바르게 전달됨
                if (_handleGpio != IntPtr.Zero)
                {
                    FT4222_STATUS suspSt = FT4222_SetSuspendOut(_handleGpio, false);
                    System.Diagnostics.Debug.WriteLine($"[FT4222] SetSuspendOut(B): {suspSt}");
                    FT4222_GPIO_Init(_handleGpio, new GPIO_Dir[] {
                        GPIO_Dir.GPIO_OUTPUT, GPIO_Dir.GPIO_OUTPUT,
                        GPIO_Dir.GPIO_OUTPUT, GPIO_Dir.GPIO_OUTPUT });
                }

                // OSP 초기화 시퀀스
                OspReset();
                System.Threading.Thread.Sleep(5);
                OspInitBidir();
                System.Threading.Thread.Sleep(5);
                OspSetSetup(BROADCAST_ADDR);
                System.Threading.Thread.Sleep(5);
                OspGoActive(_deviceAddr);

                _initialized = true;
                return true;
            }
            catch (DllNotFoundException ex)
            {
                LastError = $"DLL 로드 실패: {ex.Message}\r\n" +
                            "ftd2xx.dll / LibFT4222-64.dll 이 실행 파일 폴더에 있는지 확인하세요.";
                Disconnect();
                return false;
            }
            catch (BadImageFormatException ex)
            {
                LastError = $"DLL 비트 불일치: {ex.Message}\r\n" +
                            "LibFT4222-64.dll은 64비트 전용입니다.\r\n" +
                            "프로젝트 빌드 플랫폼을 x64로 설정하세요. (AnyCPU + 32비트 우선 실행 환경에서 실패)";
                Disconnect();
                return false;
            }
            catch (Exception ex)
            {
                LastError = $"예외 발생: {ex.GetType().Name}\r\n{ex.Message}";
                Disconnect();
                return false;
            }
        }

        public void Disconnect()
        {
            if (_nativeOspBridge)
            {
                // Disconnect()는 항상 FormMain에서 Task.Run() 위에서 호출되므로
                // 여기서는 직접 OspBridge_Close()를 호출한다.
                // (내부에서 또 Task.Run().Wait()를 하면 스레드풀 중첩 대기 → 데드락)
                try { OspBridge_Close(); } catch { }

                _nativeOspBridge = false;
                _initialized = false;
                _handle = IntPtr.Zero;
                _handleGpio = IntPtr.Zero;
                return;
            }

            _initialized = false;
            if (_handle != IntPtr.Zero)
            {
                try { FT4222_UnInitialize(_handle); } catch { }
                try { FT_Close(_handle); } catch { }
                _handle = IntPtr.Zero;
            }
            if (_handleGpio != IntPtr.Zero)
            {
                try { FT4222_UnInitialize(_handleGpio); } catch { }
                try { FT_Close(_handleGpio); } catch { }
                _handleGpio = IntPtr.Zero;
            }
        }

        private string FindFirstFT4222()
        {
            try
            {
                FT_STATUS st = FT_CreateDeviceInfoList(out uint num);
                if (st != FT_STATUS.FT_OK)
                {
                    LastError = $"FT_CreateDeviceInfoList 실패: {st}";
                    return null;
                }
                for (uint i = 0; i < num; i++)
                {
                    var sn = new System.Text.StringBuilder(64);
                    var desc = new System.Text.StringBuilder(64);
                    FT_GetDeviceInfoDetail(i, out _, out _, out _, out _, sn, desc, out _);
                    string d = desc.ToString();
                    if (IsFT4222InterfaceA(d))
                        return d + " [" + sn.ToString() + "]";
                }
                LastError = $"FT4222 Interface A 장치를 찾지 못했습니다. (열거된 장치 수: {num})";
            }
            catch (Exception ex)
            {
                LastError = $"장치 열거 중 예외: {ex.GetType().Name} — {ex.Message}";
            }
            return null;
        }

        // =========================================================================
        // 고수준 API – LED 색상 설정
        // =========================================================================

        /// <summary>
        /// RGB 색상(0~255)을 OSP PWM으로 변환하여 전송
        /// PWM 레인지: 0x0000~0x7FFF  (여기서는 상위 8비트만 사용)
        /// </summary>
        public bool SetColorRgb(byte r, byte g, byte b)
        {
            if (!IsConnected) return false;
            if (_nativeOspBridge)
            {
                OspLog?.Invoke(string.Format("[OSP TX] RGB=({0},{1},{2}) PWM=({3},{4},{5}) DayMode=OFF",
                    r, g, b, LedByteToPwm(r), LedByteToPwm(g), LedByteToPwm(b)));

                int bridgeStatus = OspBridge_SetColorRgb(r, g, b);
                if (bridgeStatus == 0)
                {
                    LogFirstLedFeedbackAfterTx(0x00, readFio: true, readIlas: false);
                    return true;
                }

                LastError = "OspBridge_SetColorRgb 실패: " + bridgeStatus + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            ushort rPwm = LedByteToPwm(r);
            ushort gPwm = LedByteToPwm(g);
            ushort bPwm = LedByteToPwm(b);
            return OspSetPwm(BROADCAST_ADDR, rPwm, gPwm, bPwm, dayMode: false);
        }

        // =========================================================================
        // FID 기반 채널 선택 LED 제어
        // FID 0x01~0x08 → IF0/IF1 (SAIF4/SAIF5 핀쌍)
        // =========================================================================

        /// <summary>
        /// FID 값으로 IF0/IF1 채널을 통해 RGB LED를 제어합니다.
        /// OspBridge.dll이 OspBridge_SetColorRgbByFid를 지원하면 해당 함수 사용.
        /// 지원하지 않으면 단일 채널 OspBridge_SetColorRgb로 폴백합니다.
        /// </summary>
        /// <param name="fid">0x01~0x08 = AD3304 FID (IF0/IF1 채널)</param>
        public bool SetColorRgbByFid(byte fid, byte r, byte g, byte b)
        {
            if (!IsConnected) return false;
            if (!_nativeOspBridge)
            {
                // 직접 SPI 모드는 단일 채널만 지원 - FID 무시하고 기존 경로 사용
                OspLog?.Invoke(string.Format("[OSP FID=0x{0:X2}] RGB=({1},{2},{3}) - SPI 직접 모드(단일채널)", fid, r, g, b));
                ushort rPwm = LedByteToPwm(r);
                ushort gPwm = LedByteToPwm(g);
                ushort bPwm = LedByteToPwm(b);
                // [Bug Fix] FID로 IF 채널 GPIO CS 핀 전환
                SelectIfChannel(fid);
                return OspSetPwm(BROADCAST_ADDR, rPwm, gPwm, bPwm, dayMode: false);
            }

            OspLog?.Invoke(fid == 0x00
                ? string.Format("[OSP AD3304 BROADCAST FID=0x00] RGB=({0},{1},{2})", r, g, b)
                : string.Format("[OSP AD3304 FID=0x{0:X2}] RGB=({1},{2},{3})", fid, r, g, b));

            try
            {
                // OspBridge.dll이 OspBridge_SetColorRgbByFid를 export하는 경우
                int stFid = OspBridge_SetColorRgbByFid_Native(fid, r, g, b);
                if (stFid == 0)
                {
                    LogFirstLedFeedbackAfterTx(fid, readFio: true, readIlas: true);
                    return true;
                }
                LastError = string.Format("OspBridge_SetColorRgbByFid(FID=0x{0:X2}) 실패: {1}", fid, stFid)
                          + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            catch (EntryPointNotFoundException)
            {
                // DLL이 FID 함수를 미지원 → 채널 구분 없이 기존 함수로 폴백
                OspLog?.Invoke(string.Format(
                    "[OSP WARN] OspBridge_SetColorRgbByFid 미지원 DLL. FID=0x{0:X2} → OspBridge_SetColorRgb 폴백", fid));
                int st = OspBridge_SetColorRgb(r, g, b);
                if (st == 0)
                {
                    LogFirstLedFeedbackAfterTx(fid, readFio: true, readIlas: true);
                    return true;
                }
                LastError = "OspBridge_SetColorRgb(폴백) 실패: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
        }

        /// <summary>
        /// FID 채널 + 특정 노드(OSP 주소) 지정하여 RGB 전송
        /// NativeOspBridge 모드: FID로 채널 선택 후 broadcast (OSP unicast는 DLL 미지원이므로 FID 분기만 수행)
        /// 직접 SPI 모드: nodeAddr로 unicast 전송
        /// </summary>
        public bool SetColorRgbByNode(byte fid, ushort nodeAddr, byte r, byte g, byte b)
        {
            if (!IsConnected) return false;

            if (_nativeOspBridge)
            {
                // NativeOspBridge: DLL이 unicast를 지원하지 않으므로 FID 채널로 broadcast
                OspLog?.Invoke(fid == 0x00
                    ? string.Format("[OSP BROADCAST FID=0x00] RGB=({0},{1},{2}) node={3}", r, g, b, nodeAddr)
                    : string.Format("[OSP FID=0x{0:X2}] RGB=({1},{2},{3}) node={4}", fid, r, g, b, nodeAddr));

                try
                {
                    int st = OspBridge_SetColorRgbByNode_Native(fid, nodeAddr, r, g, b);
                    if (st == 0)
                    {
                        return true;
                    }

                    LastError = string.Format("OspBridge_SetColorRgbByNode(FID=0x{0:X2}, node={1}) failed: {2}",
                                  fid, nodeAddr, st)
                              + "\r\n" + GetNativeBridgeError();
                    OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                    return false;
                }
                catch (EntryPointNotFoundException)
                {
                    LastError = "OspBridge_SetColorRgbByNode not exported by OspBridge.dll";
                    OspLog?.Invoke("[OSP ERR] " + LastError);
                    return false;
                }
            }

            // 직접 SPI 모드: nodeAddr unicast
            ushort rPwm = LedByteToPwm(r);
            ushort gPwm = LedByteToPwm(g);
            ushort bPwm = LedByteToPwm(b);
            OspLog?.Invoke(string.Format(
                "[OSP FID=0x{0:X2}] RGB=({1},{2},{3}) node={4} PWM=({5},{6},{7})",
                fid, r, g, b, nodeAddr, rPwm, gPwm, bPwm));
            // [Bug Fix] FID로 IF 채널 GPIO CS 핀 전환 후 SPI 전송
            SelectIfChannel(fid);
            return OspSetPwm(nodeAddr, rPwm, gPwm, bPwm, dayMode: false);
        }

        /// <summary>
        /// FID 챔널 + 특정 노드(OSP 주소) 지정하여 CIE xy + Brightness 전송
        /// </summary>
        public bool SetColorRgbByNodes(byte fid, ushort[] nodeAddrs, byte[] reds, byte[] greens, byte[] blues, int count)
        {
            if (!IsConnected) return false;
            if (nodeAddrs == null || reds == null || greens == null || blues == null)
            {
                LastError = "SetColorRgbByNodes null buffer";
                return false;
            }
            if (count <= 0) return true;
            if (count > nodeAddrs.Length || count > reds.Length || count > greens.Length || count > blues.Length)
            {
                LastError = "SetColorRgbByNodes count exceeds buffer length";
                return false;
            }
            if (count > ushort.MaxValue)
            {
                LastError = "SetColorRgbByNodes count too large";
                return false;
            }

            if (_nativeOspBridge)
            {
                try
                {
                    int st = OspBridge_SetColorRgbByNodes_Native(fid, nodeAddrs, reds, greens, blues, (ushort)count);
                    if (st == 0)
                    {
                        LogFirstLedFeedbackAfterTx(fid, readFio: true, readIlas: true);
                        return true;
                    }

                    LastError = string.Format("OspBridge_SetColorRgbByNodes(FID=0x{0:X2}, count={1}) failed: {2}",
                                  fid, count, st)
                              + "\r\n" + GetNativeBridgeError();
                    OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                    return false;
                }
                catch (EntryPointNotFoundException)
                {
                    for (int i = 0; i < count; i++)
                    {
                        if (!SetColorRgbByNode(fid, nodeAddrs[i], reds[i], greens[i], blues[i]))
                            return false;
                    }
                    return true;
                }
            }

            bool ok = true;
            for (int i = 0; i < count; i++)
            {
                if (!SetColorRgbByNode(fid, nodeAddrs[i], reds[i], greens[i], blues[i]))
                    ok = false;
            }
            return ok;
        }

        public bool SetFioColorRgbByNodes(byte fid, ushort[] nodeAddrs, byte[] reds, byte[] greens, byte[] blues, int count)
        {
            return SetSplitColorRgbByNodes(fid, nodeAddrs, reds, greens, blues, count, true);
        }

        public bool SetIlasColorRgbByNodes(byte fid, ushort[] nodeAddrs, byte[] reds, byte[] greens, byte[] blues, int count)
        {
            return SetSplitColorRgbByNodes(fid, nodeAddrs, reds, greens, blues, count, false);
        }

        private bool SetSplitColorRgbByNodes(byte fid, ushort[] nodeAddrs, byte[] reds, byte[] greens, byte[] blues, int count, bool fioOnly)
        {
            if (!IsConnected) return false;
            if (nodeAddrs == null || reds == null || greens == null || blues == null)
            {
                LastError = fioOnly ? "SetFioColorRgbByNodes null buffer" : "SetIlasColorRgbByNodes null buffer";
                return false;
            }
            if (count <= 0) return true;
            if (count > nodeAddrs.Length || count > reds.Length || count > greens.Length || count > blues.Length)
            {
                LastError = fioOnly ? "SetFioColorRgbByNodes count exceeds buffer length" : "SetIlasColorRgbByNodes count exceeds buffer length";
                return false;
            }
            if (count > ushort.MaxValue)
            {
                LastError = fioOnly ? "SetFioColorRgbByNodes count too large" : "SetIlasColorRgbByNodes count too large";
                return false;
            }
            if (!_nativeOspBridge)
            {
                LastError = fioOnly ? "FIO-only native bridge not active" : "ILAS-only native bridge not active";
                return false;
            }

            try
            {
                int st;
                if (fioOnly)
                {
                    st = OspBridge_SetFioColorRgbByNodes_Native(fid, nodeAddrs, reds, greens, blues, (ushort)count);
                }
                else
                {
                    st = OspBridge_SetIlasColorRgbByNodes_Native(
                        fid, nodeAddrs, reds, greens, blues, (ushort)count);
                }
                if (st == 0)
                {
                    LogFirstLedFeedbackAfterTx(fid, readFio: fioOnly, readIlas: !fioOnly);
                    return true;
                }

                LastError = string.Format("{0}(FID=0x{1:X2}, count={2}) failed: {3}",
                              fioOnly ? "OspBridge_SetFioColorRgbByNodes" : "OspBridge_SetIlasColorRgbByNodes",
                              fid, count, st)
                          + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            catch (EntryPointNotFoundException)
            {
                LastError = fioOnly ? "OspBridge_SetFioColorRgbByNodes missing" : "OspBridge_SetIlasColorRgbByNodes missing";
                return false;
            }
        }
        public bool SetColorCIEByNode(byte fid, ushort nodeAddr, float cx, float cy, float brightness01)
        {
            if (!IsConnected) return false;

            if (brightness01 < 0f) brightness01 = 0f;
            if (brightness01 > 1f) brightness01 = 1f;
            if (cy < 1e-4f) cy = 1e-4f;

            float Y = 1f;
            float X = cx / cy * Y;
            float Z = (1f - cx - cy) / cy * Y;

            float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
            float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
            float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

            if (r < 0f) r = 0f;
            if (g < 0f) g = 0f;
            if (b < 0f) b = 0f;

            float max = Math.Max(r, Math.Max(g, b));
            if (max > 1e-6f) { r /= max; g /= max; b /= max; }

            if (_nativeOspBridge)
            {
                byte R = FloatToLedByte(r * brightness01);
                byte G = FloatToLedByte(g * brightness01);
                byte B = FloatToLedByte(b * brightness01);
                OspLog?.Invoke(string.Format(
                    "[CIE RGB FID=0x{0:X2}] x={1:F4} y={2:F4} bri={3:F3} -> RGB=({4},{5},{6}) node={7}",
                    fid, cx, cy, brightness01, R, G, B, nodeAddr));
                return SetColorRgbByNode(fid, nodeAddr, R, G, B);
            }

            ushort rPwm = FloatToPwm(r * brightness01);
            ushort gPwm = FloatToPwm(g * brightness01);
            ushort bPwm = FloatToPwm(b * brightness01);
            OspLog?.Invoke(string.Format(
                "[CIE PWM FID=0x{0:X2}] x={1:F4} y={2:F4} bri={3:F3} -> PWM=({4},{5},{6}) node={7}",
                fid, cx, cy, brightness01, rPwm, gPwm, bPwm, nodeAddr));
            // [Bug Fix] FID로 IF 채널 GPIO CS 핀 전환
            SelectIfChannel(fid);
            return OspSetPwm(nodeAddr, rPwm, gPwm, bPwm, dayMode: false);
        }

        /// <summary>
        /// FID 값으로 IF0/IF1 채널을 통해 CIE xy + Brightness → RGB PWM 전송
        /// (FormCIE DoTx()에서 호출)
        /// FID 0x01~0x08 → IF0/IF1 채널
        /// </summary>
        public bool SetColorCIEByFid(byte fid, float cx, float cy, float brightness01)
        {
            if (!IsConnected) return false;

            if (brightness01 < 0f) brightness01 = 0f;
            if (brightness01 > 1f) brightness01 = 1f;
            if (cy < 1e-4f) cy = 1e-4f;

            float Y = 1f;
            float X = cx / cy * Y;
            float Z = (1f - cx - cy) / cy * Y;

            float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
            float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
            float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

            if (r < 0f) r = 0f;
            if (g < 0f) g = 0f;
            if (b < 0f) b = 0f;

            float max = Math.Max(r, Math.Max(g, b));
            if (max > 1e-6f) { r /= max; g /= max; b /= max; }

            if (_nativeOspBridge)
            {
                byte R = FloatToLedByte(r * brightness01);
                byte G = FloatToLedByte(g * brightness01);
                byte B = FloatToLedByte(b * brightness01);
                OspLog?.Invoke(string.Format(
                    "[CIE RGB FID=0x{0:X2}] x={1:F4} y={2:F4} bri={3:F3} -> RGB=({4},{5},{6}) IF0/IF1",
                    fid, cx, cy, brightness01, R, G, B));
                return SetColorRgbByFid(fid, R, G, B);
            }

            // 직접 SPI 모드
            ushort rPwm = FloatToPwm(r * brightness01);
            ushort gPwm = FloatToPwm(g * brightness01);
            ushort bPwm = FloatToPwm(b * brightness01);
            OspLog?.Invoke(string.Format(
                "[CIE PWM FID=0x{0:X2}] x={1:F4} y={2:F4} bri={3:F3} -> PWM=({4},{5},{6}) IF0/IF1",
                fid, cx, cy, brightness01, rPwm, gPwm, bPwm));
            // [Bug Fix] FID로 IF 채널 GPIO CS 핀 전환
            SelectIfChannel(fid);
            return OspSetPwm(BROADCAST_ADDR, rPwm, gPwm, bPwm, dayMode: false);
        }

        public bool ReadOspFeedback(byte fid = 0x01, ushort nodeAddr = 1, bool logErrors = true)
        {
            if (!IsConnected || !_nativeOspBridge) return false;

            // [수정] 12노드 unicast TX가 모두 flush되기 충분한 시간으로 증가 (10→50ms)
            System.Threading.Thread.Sleep(50);

            int st;
            ushort rPwm;
            ushort gPwm;
            ushort bPwm;
            double temp;

            try
            {
                st = OspBridge_ReadbackByFid_Native(fid, nodeAddr, out rPwm, out gPwm, out bPwm, out temp);
            }
            catch (EntryPointNotFoundException)
            {
                st = OspBridge_Readback(out rPwm, out gPwm, out bPwm, out temp);
            }

            if (st == 0)
            {
                byte r = PwmToLedByte(rPwm);
                byte g = PwmToLedByte(gPwm);
                byte b = PwmToLedByte(bPwm);
                OspLog?.Invoke(string.Format(
                    "[OSP RX] FID=0x{0:X2} node={1} RGB=({2},{3},{4}) PWM=({5},{6},{7}) TEMP={8:F2}C",
                    fid, nodeAddr, r, g, b, rPwm, gPwm, bPwm, temp));
                return true;
            }

            LastError = string.Format("OspBridge_ReadbackByFid(FID=0x{0:X2}, node={1}) 실패: {2}",
                        fid, nodeAddr, st)
                      + "\r\n" + GetNativeBridgeError();
            if (logErrors)
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
            return false;
        }

        public bool ReadIlasTempFeedback(byte fid = 0x01, byte nodeAddr = 1)
        {
            if (!IsConnected || !_nativeOspBridge) return false;
            if (fid < 1 || fid > 8 || !IsIlasActiveFid(fid)) return false;
            if (nodeAddr == 0) nodeAddr = 1;

            try
            {
                byte data0;
                byte data1;
                byte data2;
                ushort validResp;
                ushort expectedResp;
                int st = OspBridge_ReadIlasTempByFid_Native(fid, nodeAddr,
                    out data0, out data1, out data2, out validResp, out expectedResp);

                if (st == 0)
                {
                    double tempC = DecodeIlasTemperatureC(data0, data1, data2);
                    OspLog?.Invoke(string.Format(
                        "[ILAS RX] FID=0x{0:X2} node={1} TEMP={2:F2}C RAW=[{3:X2} {4:X2} {5:X2}] Resp={6}/{7}",
                        fid, nodeAddr, tempC, data0, data1, data2, validResp, expectedResp));
                    return true;
                }

                LastError = string.Format("OspBridge_ReadIlasTempByFid(FID=0x{0:X2}, node={1}) 실패: {2}",
                            fid, nodeAddr, st)
                          + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[ILAS ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            catch (EntryPointNotFoundException)
            {
                OspLog?.Invoke("[ILAS ERR] OspBridge_ReadIlasTempByFid 미지원 DLL");
                return false;
            }
        }

        private static double DecodeIlasTemperatureC(byte data0, byte data1, byte data2)
        {
            _ = data0;
            _ = data2;
            // seddLED3.0 datasheet exposes the ADC/read flow but not the conversion table.
            // Observed ILAS temp code 0x58 aligns with ambient when interpreted as code - 55.
            return data1 - 55.0;
        }

        private void LogFirstLedFeedbackAfterTx(byte fid, bool readFio, bool readIlas)
        {
            if (!TxRxFeedbackEnabled)
                return;

            DateTime now = DateTime.UtcNow;

            if (readFio)
            {
                byte fioFid = ResolveFirstActiveFid(fid, wantFio: true);
                if (fioFid != 0 &&
                    (now - _lastFioRxFeedbackUtc).TotalMilliseconds >= TX_RX_FEEDBACK_MIN_INTERVAL_MS)
                {
                    _lastFioRxFeedbackUtc = now;
                    ReadOspFeedback(fioFid, 1, logErrors: false);
                }
            }

            if (readIlas)
            {
                byte ilasFid = ResolveFirstActiveFid(fid, wantFio: false);
                if (ilasFid != 0 &&
                    (now - _lastIlasRxFeedbackUtc).TotalMilliseconds >= TX_RX_FEEDBACK_MIN_INTERVAL_MS)
                {
                    _lastIlasRxFeedbackUtc = now;
                    ReadIlasTempFeedback(ilasFid, 1);
                }
            }
        }

        private byte ResolveFirstActiveFid(byte fid, bool wantFio)
        {
            if (fid >= 1 && fid <= 8)
                return wantFio ? (IsFioActiveFid(fid) ? fid : (byte)0) : (IsIlasActiveFid(fid) ? fid : (byte)0);

            for (byte f = 1; f <= 8; f++)
            {
                if (wantFio ? IsFioActiveFid(f) : IsIlasActiveFid(f))
                    return f;
            }
            return 0;
        }

        public string GetAd3304Status()
        {
            try
            {
                IntPtr p = OspBridge_GetAd3304Status_Native();
                return p != IntPtr.Zero ? System.Runtime.InteropServices.Marshal.PtrToStringAnsi(p) : "(null)";
            }
            catch { return "(GetAd3304Status 미지원 DLL)"; }
        }

        /// <summary>Discovery와 FIO 초기화가 모두 성공한 FID 목록을 반환한다.</summary>
        public List<byte> GetActiveFids()
        {
            var result = new List<byte>();
            string status = GetAd3304Status();
            if (string.IsNullOrWhiteSpace(status)) return result;

            foreach (string rawLine in status.Split('\n'))
            {
                string line = rawLine.Trim();
                if (!line.StartsWith("FID", StringComparison.OrdinalIgnoreCase) ||
                    line.IndexOf("DISC=OK", StringComparison.OrdinalIgnoreCase) < 0 ||
                    (line.IndexOf("FIO=ACTIVE", StringComparison.OrdinalIgnoreCase) < 0 &&
                     line.IndexOf("ILAS=ACTIVE", StringComparison.OrdinalIgnoreCase) < 0))
                    continue;

                if (line.Length >= 5 && byte.TryParse(line.Substring(3, 2), out byte fid) &&
                    fid >= 1 && fid <= 8)
                    result.Add(fid);
            }
            return result;
        }

        public int GetIlasLedCount()
        {
            if (!_nativeOspBridge) return 0;
            try
            {
                int count = OspBridge_GetIlasLedCount_Native();
                return count < 0 ? 0 : count;
            }
            catch { return 0; }
        }

        public bool IsIlasActiveFid(byte fid)
        {
            if (fid < 1 || fid > 8) return false;
            string status = GetAd3304Status();
            if (string.IsNullOrWhiteSpace(status)) return false;

            string prefix = string.Format("FID{0:00}:", fid);
            foreach (string rawLine in status.Split('\n'))
            {
                string line = rawLine.Trim();
                if (line.StartsWith(prefix, StringComparison.OrdinalIgnoreCase) &&
                    line.IndexOf("ILAS=ACTIVE", StringComparison.OrdinalIgnoreCase) >= 0)
                    return true;
            }
            return false;
        }

        public bool IsFioActiveFid(byte fid)
        {
            if (fid < 1 || fid > 8) return false;
            string status = GetAd3304Status();
            if (string.IsNullOrWhiteSpace(status)) return false;

            string prefix = string.Format("FID{0:00}:", fid);
            foreach (string rawLine in status.Split('\n'))
            {
                string line = rawLine.Trim();
                if (line.StartsWith(prefix, StringComparison.OrdinalIgnoreCase) &&
                    line.IndexOf("FIO=ACTIVE", StringComparison.OrdinalIgnoreCase) >= 0)
                    return true;
            }
            return false;
        }
        private bool TryGetFirstDiscoveredMac(out ulong mac, out byte nodeId)
        {
            mac = 0;
            nodeId = 0;
            string status = GetAd3304Status();
            if (string.IsNullOrWhiteSpace(status)) return false;

            foreach (string rawLine in status.Split('\n'))
            {
                string line = rawLine.Trim();
                if (!line.StartsWith("FID", StringComparison.OrdinalIgnoreCase) ||
                    line.IndexOf("DISC=OK", StringComparison.OrdinalIgnoreCase) < 0)
                    continue;

                if (line.Length >= 5 && byte.TryParse(line.Substring(3, 2), out byte fid))
                    nodeId = fid;

                const string marker = "MAC=";
                int macPos = line.IndexOf(marker, StringComparison.OrdinalIgnoreCase);
                if (macPos < 0) continue;

                string macText = line.Substring(macPos + marker.Length, Math.Min(17, line.Length - macPos - marker.Length));
                string cleaned = macText.Replace(":", "").Replace("-", "").Trim();
                if (cleaned.Length == 12 && ulong.TryParse(cleaned, System.Globalization.NumberStyles.HexNumber, null, out mac))
                {
                    if (nodeId == 0)
                        nodeId = (byte)(mac & 0xFF);
                    return mac != 0;
                }
            }
            return false;
        }

        private static string GetNativeBridgeError()
        {
            try
            {
                IntPtr p = OspBridge_GetLastError();
                return p == IntPtr.Zero ? string.Empty : Marshal.PtrToStringAnsi(p);
            }
            catch
            {
                return string.Empty;
            }
        }

        private static bool IsFioDiagOk(string diagPart)
        {
            if (string.IsNullOrEmpty(diagPart)) return false;
            return diagPart.Contains("Open=0")
                && diagPart.Contains("CB=0")
                && diagPart.Contains("RAM=0")
                && diagPart.Contains("Rst=0")
                && diagPart.Contains("Bidir=0")
                && diagPart.Contains("Setup=0")
                && diagPart.Contains("State=0")
                && diagPart.Contains("Hdl=1");
        }

        private static ushort LedByteToPwm(byte v)
        {
            return (ushort)((v * 0x7FFFu + 127u) / 255u);
        }

        private static byte PwmToLedByte(ushort pwm)
        {
            uint v = ((uint)pwm * 255u + 0x3FFFu) / 0x7FFFu;
            return (byte)(v > 255u ? 255u : v);
        }

        /// <summary>
        /// CIE xy + Brightness → RGB PWM 전송
        /// (FormCIE에서 호출)
        /// </summary>
        public bool SetColorCIE(float cx, float cy, float brightness01)
        {
            if (!IsConnected) return false;

            if (brightness01 < 0f) brightness01 = 0f;
            if (brightness01 > 1f) brightness01 = 1f;
            if (cy < 1e-4f) cy = 1e-4f;
            float Y = 1f;
            float X = cx / cy * Y;
            float Z = (1f - cx - cy) / cy * Y;

            float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
            float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
            float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

            if (r < 0f) r = 0f;
            if (g < 0f) g = 0f;
            if (b < 0f) b = 0f;

            float max = Math.Max(r, Math.Max(g, b));
            if (max > 1e-6f)
            {
                r /= max;
                g /= max;
                b /= max;
            }

            // NativeOspBridge 모드: DLL이 byte만 받으므로 기존 경로 유지
            if (_nativeOspBridge)
            {
                byte R = FloatToLedByte(r * brightness01);
                byte G = FloatToLedByte(g * brightness01);
                byte B = FloatToLedByte(b * brightness01);
                OspLog?.Invoke(string.Format("[CIE RGB] x={0:F4} y={1:F4} brightness={2:F3} -> RGB=({3},{4},{5})",
                    cx, cy, brightness01, R, G, B));
                return SetColorRgb(R, G, B);
            }

            // 직접 SPI 모드: float -> PWM 직접 변환 (32768단계, byte 양자화 없음)
            ushort rPwm = FloatToPwm(r * brightness01);
            ushort gPwm = FloatToPwm(g * brightness01);
            ushort bPwm = FloatToPwm(b * brightness01);

            OspLog?.Invoke(string.Format("[CIE RGB] x={0:F4} y={1:F4} brightness={2:F3} -> PWM=({3},{4},{5})",
                cx, cy, brightness01, rPwm, gPwm, bPwm));

            return OspSetPwm(BROADCAST_ADDR, rPwm, gPwm, bPwm, dayMode: false);
        }

        private static float GammaCorrect(float c)
        {
            if (c <= 0f) return 0f;
            if (c <= 0.0031308f) return 12.92f * c;
            return (float)(1.055 * Math.Pow(c, 1.0 / 2.4) - 0.055);
        }

        private static byte FloatToLedByte(float v)
        {
            int i = (int)(v * 255f + 0.5f);
            return (byte)(i < 0 ? 0 : i > 255 ? 255 : i);
        }

        private static ushort FloatToPwm(float v)
        {
            if (v < 0f) v = 0f;
            if (v > 1f) v = 1f;
            return (ushort)(v * 0x7FFF + 0.5f);  // 0~32767, 32768단계
        }

        // =========================================================================
        // OSP 명령 구성 – fioOsp.c 포팅
        // =========================================================================

        private void OspReset()
        {
            byte[] tx = BuildFrame(BROADCAST_ADDR, CMD_RESET, null);
            SpiWrite(tx);
        }

        private void OspInitBidir()
        {
            byte[] tx = BuildFrame(INIT_ADDR, CMD_INIT_BIDIR, null);
            byte[] rx = SpiWriteRead(tx, 6);
            if (rx != null && rx.Length >= 6)
            {
                _deviceAddr = (ushort)(((rx[1] & 0xFC) >> 2) | ((rx[2] & 0xC0) << 4));
                if (_deviceAddr == 0) _deviceAddr = 1;
            }
        }

        private void OspGoActive(ushort addr)
        {
            byte[] tx = BuildFrame(addr, CMD_GO_ACTIVE, null);
            SpiWrite(tx);
        }

        private void OspSetSetup(ushort addr)
        {
            byte setupByte = 0x20; // bit5=CrcEnable
            byte[] tx = BuildFrame(addr, CMD_SET_SETUP, new byte[] { setupByte });
            SpiWrite(tx);
        }

        /// <summary>
        /// IF0/IF1 채널 고정 GPIO CS select.
        /// 모든 FID(0x01~0x08) → IF0/IF1 단일 채널 사용.
        /// If _handleGpio is Zero, skip (single-channel HW).
        /// </summary>
        private bool SelectIfChannel(byte fid)
        {
            if (_handleGpio == IntPtr.Zero) return true;
            // IF0/IF1 채널 고정: Pin0=HIGH, Pin2=LOW
            FT4222_GPIO_Write(_handleGpio, 0, true);   // Pin0: IF0/IF1 CS ON
            FT4222_GPIO_Write(_handleGpio, 2, false);  // Pin2: OFF
            System.Threading.Thread.Sleep(1);
            OspLog?.Invoke(string.Format("[CH SEL] FID=0x{0:X2} -> IF0/IF1 (Pin0=HIGH, Pin2=LOW)", fid));
            return true;
        }

        private bool OspSetPwm(ushort addr, ushort rPwm, ushort gPwm, ushort bPwm, bool dayMode)
        {
            byte dm = dayMode ? (byte)0x80 : (byte)0x00;
            /*
            byte[] payload = new byte[6]
            {
                (byte)(((rPwm >> 8) & 0x7F) | dm), (byte)(rPwm & 0xFF),
                (byte)(((gPwm >> 8) & 0x7F) | dm), (byte)(gPwm & 0xFF),
                (byte)(((bPwm >> 8) & 0x7F) | dm), (byte)(bPwm & 0xFF),
            };
            */
            byte[] payload = new byte[6]
            {
                (byte)(rPwm & 0xFF), (byte)(((rPwm >> 8) & 0x7F) | dm),
                (byte)(gPwm & 0xFF), (byte)(((gPwm >> 8) & 0x7F) | dm),
                (byte)(bPwm & 0xFF), (byte)(((bPwm >> 8) & 0x7F) | dm),
            };
            byte[] tx = BuildFrame(addr, CMD_SET_PWM, payload);
            return SpiWrite(tx);
        }

        // =========================================================================
        // OSP 프레임 빌더
        // =========================================================================

        private static byte[] BuildFrame(ushort addr, byte cmd, byte[] payload)
        {
            int payLen = payload == null ? 0 : payload.Length;
            int totalLen = 4 + payLen;          // header(3) + payload + CRC(1)
            byte[] buf = new byte[totalLen];

            byte psi = totalLen == 12 ? (byte)7 : (byte)(totalLen - 4);

            buf[0] = (byte)(PREAMBLE | ((addr >> 6) & 0x0F));
            buf[1] = (byte)(((addr & 0x3F) << 2) | ((psi >> 1) & 0x03));
            buf[2] = (byte)(((psi & 0x01) << 7) | (cmd & 0x7F));

            for (int i = 0; i < payLen; i++)
                buf[3 + i] = payload[i];

            buf[totalLen - 1] = CalcCrc(buf, totalLen);
            return buf;
        }

        private static byte CalcCrc(byte[] buf, int len)
        {
            byte crc = 0x00;
            for (int i = 0; i < len - 1; i++)
            {
                byte extract = buf[i];
                for (int mask = 8; mask > 0; mask--)
                {
                    bool bit = ((extract >> (mask - 1)) & 0x01) == ((crc >> 7) & 0x01);
                    crc = (byte)((crc << 1) & 0xFF);
                    if (!bit) crc ^= 0x2F;
                }
            }
            return crc;
        }

        // =========================================================================
        // SPI 트랜잭션
        // =========================================================================

        private bool SpiWrite(byte[] tx)
        {
            if (_handle == IntPtr.Zero) return false;
            byte[] rx = new byte[tx.Length];
            // ★ [버그2 수정 적용] isEndTransaction=true 가 BOOL(4바이트)로 올바르게 전달됨
            var st = FT4222_SPIMaster_SingleReadWrite(
                _handle, rx, tx, (ushort)tx.Length, out _, true);
            return st == FT4222_STATUS.FT4222_OK;
        }

        private byte[] SpiWriteRead(byte[] tx, int rxLen)
        {
            if (_handle == IntPtr.Zero) return null;
            int total = tx.Length + rxLen;
            byte[] txBuf = new byte[total];
            Array.Copy(tx, txBuf, tx.Length);
            byte[] rxBuf = new byte[total];

            // ★ [버그2 수정 적용]
            var st = FT4222_SPIMaster_SingleReadWrite(
                _handle, rxBuf, txBuf, (ushort)total, out _, true);
            if (st != FT4222_STATUS.FT4222_OK) return null;

            byte[] result = new byte[rxLen];
            Array.Copy(rxBuf, tx.Length, result, 0, rxLen);
            return result;
        }

        // =========================================================================
        // AD3301 MAC Address / PLCA Node ID  Read / Write
        // =========================================================================

        /// <summary>
        /// OspBridge_GetMacNode 를 호출해 AD3301 의 현재 MAC 주소와 PLCA Node ID 를 반환합니다.
        /// Discovery 완료 후(Init 성공) 유효한 값이 반환됩니다.
        /// </summary>
        /// <param name="mac">6바이트 MAC을 ulong 으로 반환 (MSB = bit47)</param>
        /// <param name="nodeId">PLCA Node ID (0~254)</param>
        public bool ReadMacNode(out ulong mac, out byte nodeId)
        {
            mac = 0;
            nodeId = 0;

            if (!IsConnected || !_nativeOspBridge)
            {
                LastError = "FT4222 미연결 또는 OspBridge 초기화 안 됨";
                return false;
            }

            byte[] buf = new byte[6];
            int st = OspBridge_GetMacNode(buf, out nodeId);
            if (st != 0)
            {
                LastError = "OspBridge_GetMacNode 실패: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }

            mac = ((ulong)buf[0] << 40)
                | ((ulong)buf[1] << 32)
                | ((ulong)buf[2] << 24)
                | ((ulong)buf[3] << 16)
                | ((ulong)buf[4] << 8)
                | (ulong)buf[5];

            if (mac == 0 && TryGetFirstDiscoveredMac(out ulong discoveredMac, out byte discoveredNodeId))
            {
                mac = discoveredMac;
                nodeId = discoveredNodeId;
            }

            // 읽기 성공 시 ini 에 저장 (다음 연결 시 활용)
            SaveSettings(mac, nodeId);
            OspLog?.Invoke(string.Format("[OSP] ReadMacNode: MAC={0:X12} NodeId={1} → ini 저장", mac, nodeId));
            return true;
        }

        /// <summary>
        /// OspBridge_SetMac 를 호출해 AD3301 MAC 주소를 변경합니다.
        /// </summary>
        /// <param name="mac">6바이트 MAC (MSB = bit47), 예) 0x00E022FE7000UL</param>
        public bool WriteMac(ulong mac)
        {
            if (!IsConnected || !_nativeOspBridge)
            {
                LastError = "FT4222 미연결 또는 OspBridge 초기화 안 됨";
                return false;
            }

            byte[] buf = new byte[6];
            for (int i = 0; i < 6; i++)
                buf[i] = (byte)((mac >> (40 - 8 * i)) & 0xFF);

            int st = OspBridge_SetMac(buf);
            if (st != 0)
            {
                LastError = "OspBridge_SetMac 실패: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            return true;
        }

        /// <summary>
        /// OspBridge_SetNodeId 를 호출해 AD3301 PLCA Node ID 를 변경합니다.
        /// </summary>
        /// <param name="nodeId">PLCA Node ID (1~254)</param>
        public bool WriteNodeId(byte nodeId)
        {
            if (!IsConnected || !_nativeOspBridge)
            {
                LastError = "FT4222 미연결 또는 OspBridge 초기화 안 됨";
                return false;
            }

            int st = OspBridge_SetNodeId(nodeId);
            if (st != 0)
            {
                LastError = "OspBridge_SetNodeId 실패: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }
            return true;
        }

        /// <summary>
        /// AD3301 OTP에 MAC 주소와 PLCA Node ID를 영구 프로그램합니다.
        /// OTP는 1회성 영역이므로 성공 후 값을 되돌릴 수 없습니다.
        /// </summary>
        public bool ProgramMacNodeOtp(ulong mac, byte nodeId)
        {
            if (!IsConnected || !_nativeOspBridge)
            {
                LastError = "FT4222 미연결 또는 OspBridge 초기화 안 됨";
                return false;
            }
            if (nodeId == 0 || nodeId > 254)
            {
                LastError = "OTP PLCA Node ID는 1~254 범위여야 합니다.";
                return false;
            }

            byte[] buf = new byte[6];
            for (int i = 0; i < 6; i++)
                buf[i] = (byte)((mac >> (40 - 8 * i)) & 0xFF);

            int st = OspBridge_ProgramMacNodeOtp(buf, nodeId);
            if (st != 0)
            {
                LastError = "OspBridge_ProgramMacNodeOtp 실패: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OSP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }

            SaveSettings(mac, nodeId);
            OspLog?.Invoke(string.Format("[OSP OTP] MAC={0:X12} NodeId={1} 영구 프로그램 완료", mac, nodeId));
            return true;
        }

        /// <summary>
        /// OTP designer 블록(0=MAC, 4=PLCA)에 실제로 구워진 값을 읽습니다.
        /// ReadMacNode() 가 반환하는 IF Pin(SA_IF) 샘플링 값과 달리,
        /// 전원을 껐다 켜도 남는 "OTP에 영구 저장된 값" 입니다.
        /// </summary>
        /// <param name="mac">읽은 MAC (MSB = bit47)</param>
        /// <param name="nodeId">읽은 PLCA Node ID</param>
        /// <param name="bootStatusMac">MAC 블록 boot status (0=미사용/writable, 그 외=programmed)</param>
        /// <param name="bootStatusPlca">PLCA 블록 boot status</param>
        /// <param name="raw">designer 블록 원본 덤프 (레이아웃 검증용)</param>
        public bool ReadMacNodeOtp(out ulong mac, out byte nodeId,
            out byte bootStatusMac, out byte bootStatusPlca, out byte[] raw)
        {
            mac = 0; nodeId = 0; bootStatusMac = 0; bootStatusPlca = 0;
            // ADI_OTP_NUM_DESIGNER_SCRIPTS_BLOCKS * ADI_OTP_BLOCK_LEN_BYTES.
            // SDK 값과 다르면 네이티브가 memcpy 하는 길이에 맞춰 조정하세요.
            raw = new byte[8 * 16];

            if (!IsConnected || !_nativeOspBridge)
            {
                LastError = "FT4222 미연결 또는 OspBridge 초기화 안 됨";
                return false;
            }

            byte[] buf = new byte[6];
            int st = OspBridge_ReadMacNodeOtp(buf, out nodeId,
                out bootStatusMac, out bootStatusPlca, raw);
            if (st != 0)
            {
                LastError = "OspBridge_ReadMacNodeOtp 실패: " + st + "\r\n" + GetNativeBridgeError();
                OspLog?.Invoke("[OTP ERR] " + LastError.Replace("\r\n", " / "));
                return false;
            }

            mac = ((ulong)buf[0] << 40)
                | ((ulong)buf[1] << 32)
                | ((ulong)buf[2] << 24)
                | ((ulong)buf[3] << 16)
                | ((ulong)buf[4] << 8)
                | (ulong)buf[5];

            OspLog?.Invoke(string.Format(
                "[OTP] ReadMacNodeOtp: MAC={0:X12} Node={1} bootStatus(MAC/PLCA)={2}/{3}",
                mac, nodeId, bootStatusMac, bootStatusPlca));
            return true;
        }

        // =========================================================================
        // IDisposable
        // =========================================================================

        public void Dispose()
        {
            if (!_disposed) { Disconnect(); _disposed = true; }
        }
    }
}
