using System;
using System.Runtime.InteropServices;
using System.Text;

namespace LINMaster.LIN
{
    // =========================================================================
    // PeakLINGateway – PEAK PLIN-USB 인터페이스를 사용하는 LIN 게이트웨이
    //
    // [공식 PLinApi.cs 기반으로 전면 재작성]
    //
    // [주요 수정 내역]
    //   1. hClient 타입: ushort → byte (HLINCLIENT = System.Byte)
    //   2. 체크섬 타입 값 수정:
    //        Classic  = 1 (cstClassic)  ← 기존 0 은 잘못된 값
    //        Enhanced = 2 (cstEnhanced) ← 기존 1 은 잘못된 값
    //   3. TLINRcvMsg 필드 순서 수정: Type 필드가 첫 번째
    //   4. TLINFrameEntry.Flags: byte → ushort
    //   5. TLINMsg.FrameId: raw ID → PID (Parity 포함)
    //   6. LIN_SetClientFilter: from/to 2개 → 단일 UInt64 bitmask
    //   7. 일반 송수신은 LIN_Write, 고속 연속 Read는 하드웨어 Schedule 사용
    //      → LIN_CalculateChecksum 을 struct 재정의 후 정상 호출
    //   8. Connect 시 ResetHardwareConfig 호출 → 이전 세션 상태 초기화
    //      → 첫 번째 연결에서 바로 인식되도록 수정
    //   9. 고유 클라이언트 이름(GUID) 사용 → 이전 잔여 클라이언트와 충돌 방지
    // =========================================================================
    public class PeakLINGateway : ILINGateway
    {
        private const string PLIN_DLL = "PLinApi.dll";

        // ── 오류 코드 (TLINError) ──────────────────────────────────────────
        private const int ERR_OK           = 0;
        private const int ERR_RCV_EMPTY    = 3;   // 수신 큐 비어있음

        // ── 하드웨어 모드 (TLINHardwareMode) ──────────────────────────────
        private const byte MODE_NONE   = 0;
        private const byte MODE_SLAVE  = 1;
        private const byte MODE_MASTER = 2;

        // ── 방향 (TLINDirection) ───────────────────────────────────────────
        private const byte DIR_PUBLISHER  = 1;
        private const byte DIR_SUBSCRIBER = 2;

        // ── 체크섬 타입 (TLINChecksumType) ────────────────────────────────
        // 공식 헤더 기준:
        //   cstCustom   = 0
        //   cstClassic  = 1  ← 주의: 기존 코드의 0 은 잘못된 값이었음
        //   cstEnhanced = 2  ← 주의: 기존 코드의 1 은 잘못된 값이었음
        //   cstAuto     = 3
        private const byte CS_CLASSIC  = 1;
        private const byte CS_ENHANCED = 2;

        // ── 메시지 타입 (TLINMsgType) ──────────────────────────────────────
        private const byte MSGTYPE_STANDARD = 0;

        private const byte SLOT_UNCONDITIONAL = 0;
        private const int FAST_SCHEDULE_NUMBER = 0;

        // ── Frame Flags ────────────────────────────────────────────────────
        private const ushort FRAME_FLAG_NONE            = 0;
        private const ushort FRAME_FLAG_RESPONSE_ENABLE = 1;
        private const ushort FRAME_FLAG_IGNORE_INIT     = 4;

        // ── 하드웨어 파라미터 (TLINHardwareParam) ─────────────────────────
        private const ushort HWP_BAUDRATE = 6;
        private const ushort HWP_MODE     = 7;

        // ── 수신 필터: 모든 64개 Frame ID 허용 ────────────────────────────
        private const ulong FILTER_ALL = 0xFFFFFFFFFFFFFFFF;

        // ─────────────────────────────────────────────────────────────────
        // P/Invoke 구조체 (공식 PLinApi.cs 와 정확히 동일한 레이아웃)
        // ─────────────────────────────────────────────────────────────────

        // TLINMsg – 송신 메시지
        // FrameId = PID (Protected ID, 6bit ID + 2bit Parity)
        [StructLayout(LayoutKind.Sequential)]
        private struct TLINMsg
        {
            public byte FrameId;        // PID (ID + Parity bits)
            public byte Length;
            public byte Direction;      // TLINDirection (U1)
            public byte ChecksumType;   // TLINChecksumType (U1)
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] Data;
            public byte Checksum;       // LIN_CalculateChecksum 으로 채워짐
        }

        // TLINRcvMsg – 수신 메시지
        // ★ Type 필드가 첫 번째! (기존 코드에서 FrameId 가 첫 번째로 잘못 정의됨)
        [StructLayout(LayoutKind.Sequential)]
        private struct TLINRcvMsg
        {
            public byte   Type;         // TLINMsgType (U1) ← 첫 번째 필드
            public byte   FrameId;      // PID
            public byte   Length;
            public byte   Direction;    // TLINDirection (U1)
            public byte   ChecksumType; // TLINChecksumType (U1)
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] Data;
            public byte   Checksum;
            public int    ErrorFlags;   // TLINMsgErrors (I4)
            public ulong  TimeStamp;    // 마이크로초
            public ushort hHw;          // Hardware handle
        }

        // TLINFrameEntry – 프레임 등록 (Slave 전용)
        // Flags = ushort (공식 헤더 기준)
        [StructLayout(LayoutKind.Sequential)]
        private struct TLINFrameEntry
        {
            public byte   FrameId;
            public byte   Length;
            public byte   Direction;    // TLINDirection (U1)
            public byte   ChecksumType; // TLINChecksumType (U1)
            public ushort Flags;        // ★ ushort (기존 byte 는 잘못된 정의)
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] InitialData;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct TLINScheduleSlot
        {
            public byte Type;
            public ushort Delay;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] FrameId;
            public byte CountResolve;
            public uint Handle;
        }

        // ─────────────────────────────────────────────────────────────────
        // P/Invoke 선언 (공식 PLinApi.cs 기반)
        // HLINCLIENT = byte,  HLINHW = ushort
        // ─────────────────────────────────────────────────────────────────

        [DllImport(PLIN_DLL, EntryPoint = "LIN_RegisterClient", CallingConvention = CallingConvention.StdCall)]
        private static extern int RegisterClient(
            [MarshalAs(UnmanagedType.LPStr)] string strName,
            IntPtr hWnd,
            out byte hClient);          // ★ byte (HLINCLIENT)

        [DllImport(PLIN_DLL, EntryPoint = "LIN_RemoveClient", CallingConvention = CallingConvention.StdCall)]
        private static extern int RemoveClient(byte hClient);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_ConnectClient", CallingConvention = CallingConvention.StdCall)]
        private static extern int ConnectClient(byte hClient, ushort hHw);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_DisconnectClient", CallingConvention = CallingConvention.StdCall)]
        private static extern int DisconnectClient(byte hClient, ushort hHw);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_InitializeHardware", CallingConvention = CallingConvention.StdCall)]
        private static extern int InitializeHardware(
            byte hClient, ushort hHw, byte mode, ushort baudrate);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_GetAvailableHardware", CallingConvention = CallingConvention.StdCall)]
        private static extern int GetAvailableHardware(
            [Out] ushort[] pBuff,
            ushort wBuffSize,           // 바이트 크기 = pBuff.Length * 2
            out ushort pCount);

        // ★ 단일 UInt64 bitmask (기존 from/to uint 2개는 잘못된 시그니처)
        [DllImport(PLIN_DLL, EntryPoint = "LIN_SetClientFilter", CallingConvention = CallingConvention.StdCall)]
        private static extern int SetClientFilter(
            byte hClient, ushort hHw,
            [MarshalAs(UnmanagedType.I8)] ulong iRcvMask);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_SetFrameEntry", CallingConvention = CallingConvention.StdCall)]
        private static extern int SetFrameEntry(
            byte hClient, ushort hHw, ref TLINFrameEntry pFrameEntry);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_SetSchedule", CallingConvention = CallingConvention.StdCall)]
        private static extern int SetSchedule(
            byte hClient, ushort hHw, int scheduleNumber,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4), In, Out] TLINScheduleSlot[] schedule,
            int slotCount);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_DeleteSchedule", CallingConvention = CallingConvention.StdCall)]
        private static extern int DeleteSchedule(byte hClient, ushort hHw, int scheduleNumber);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_StartSchedule", CallingConvention = CallingConvention.StdCall)]
        private static extern int StartSchedule(byte hClient, ushort hHw, int scheduleNumber);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_SuspendSchedule", CallingConvention = CallingConvention.StdCall)]
        private static extern int SuspendSchedule(byte hClient, ushort hHw);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_ResumeSchedule", CallingConvention = CallingConvention.StdCall)]
        private static extern int ResumeSchedule(byte hClient, ushort hHw);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_Write", CallingConvention = CallingConvention.StdCall)]
        private static extern int Write(
            byte hClient, ushort hHw, ref TLINMsg pMsg);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_Read", CallingConvention = CallingConvention.StdCall)]
        private static extern int Read(
            byte hClient, out TLINRcvMsg pMsg);

        // LIN_CalculateChecksum: struct 레이아웃이 공식 헤더와 동일하므로 정상 사용 가능
        [DllImport(PLIN_DLL, EntryPoint = "LIN_CalculateChecksum", CallingConvention = CallingConvention.StdCall)]
        private static extern int CalculateChecksum(ref TLINMsg pMsg);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_GetErrorText", CallingConvention = CallingConvention.StdCall)]
        private static extern int GetErrorText(
            int dwError, byte bLanguage,
            [MarshalAs(UnmanagedType.LPStr, SizeParamIndex = 3)] StringBuilder pBuff,
            int wBuffSize);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_ResetHardwareConfig", CallingConvention = CallingConvention.StdCall)]
        private static extern int ResetHardwareConfig(byte hClient, ushort hHw);

        [DllImport(PLIN_DLL, EntryPoint = "LIN_GetHardwareParam", CallingConvention = CallingConvention.StdCall)]
        private static extern int GetHardwareParam(
            ushort hHw, ushort wParam, out int pBuff, ushort wBuffSize);

        // ── 멤버 변수 ─────────────────────────────────────────────────────

        private byte   _hClient  = 0;
        private ushort _hHw      = 0;
        private bool   _connected = false;
        private bool   _disposed  = false;
        private string _portName  = "";
        private ushort _baudRate  = 19200;
        private bool _subscriberScheduleConfigured;
        private bool _subscriberScheduleRunning;
        private readonly object _scheduleSync = new object();

        public const ushort DEFAULT_LIN_BAUDRATE = 19200;

        public ushort LinBaudRate
        {
            get { return _baudRate; }
            set { _baudRate = value; }
        }

        public bool   IsConnected { get { return _connected; } }
        public string PortName    { get { return _portName; } }

        public event Action<LINFrame>       FrameReceived;
        public event Action<string>         StatusMessage;
        public event Action<string>         ErrorMessage;
        public event Action<byte[], string> RawPacket;

        // ── 생성자 ────────────────────────────────────────────────────────

        public PeakLINGateway() { CheckDllAvailable(); }

        private void CheckDllAvailable()
        {
            IntPtr h = IntPtr.Zero;
            try { h = NativeMethods.LoadLibrary(PLIN_DLL); } catch { }
            if (h == IntPtr.Zero)
                throw new DllNotFoundException(
                    "PLinApi.dll 을 찾을 수 없습니다.\n" +
                    "PEAK PLIN-USB 드라이버를 설치하거나\n" +
                    "PLinApi.dll 을 실행 파일과 같은 폴더에 복사하세요.");
            try { NativeMethods.FreeLibrary(h); } catch { }
        }

        // ── 연결 ──────────────────────────────────────────────────────────

        public bool Connect(string portName)
        {
            int err;

            // ① 고유 클라이언트 이름으로 등록
            //    Guid 사용 → 이전 잔여 클라이언트와 이름 충돌 방지
            string clientName = "LINMaster_" + Guid.NewGuid().ToString("N").Substring(0, 8);
            byte client = 0;
            try { err = RegisterClient(clientName, IntPtr.Zero, out client); }
            catch (Exception ex) { ErrorMessage?.Invoke("[PEAK] RegisterClient 예외: " + ex.Message); return false; }
            if (err != ERR_OK) { ErrorMessage?.Invoke("[PEAK] 클라이언트 등록 실패: " + ErrText(err)); return false; }
            _hClient = client;

            // ② 연결된 하드웨어 목록 조회
            //    wBuffSize = 배열 바이트 크기 = Length * sizeof(ushort) = Length * 2
            ushort[] hwArr  = new ushort[16];
            ushort   hwCount = 0;
            try { err = GetAvailableHardware(hwArr, (ushort)(hwArr.Length * 2), out hwCount); }
            catch (Exception ex) { TryRemove(client); ErrorMessage?.Invoke("[PEAK] GetAvailableHardware 예외: " + ex.Message); return false; }
            if (err != ERR_OK || hwCount == 0)
            { TryRemove(client); ErrorMessage?.Invoke("[PEAK] PEAK LIN 장치를 찾을 수 없습니다.\n(PLinMng.exe / PLinService 실행 여부 확인)"); return false; }

            // ③ 포트 이름 → 장치 인덱스 ("PEAK#1" → 0)
            int hwIdx = 0;
            if (portName.StartsWith("PEAK#", StringComparison.OrdinalIgnoreCase))
            {
                int n;
                if (int.TryParse(portName.Substring(5), out n) && n >= 1 && n <= hwCount)
                    hwIdx = n - 1;
            }
            ushort hwHandle = hwArr[hwIdx];
            _hHw = hwHandle;

            // ④ 클라이언트 → 하드웨어 연결
            try { err = ConnectClient(client, hwHandle); }
            catch (Exception ex) { TryRemove(client); ErrorMessage?.Invoke("[PEAK] ConnectClient 예외: " + ex.Message); return false; }
            if (err != ERR_OK) { TryRemove(client); ErrorMessage?.Invoke("[PEAK] 장치 연결 실패: " + ErrText(err)); return false; }

            // ⑤ 이전 세션 상태 초기화 → 첫 연결부터 안정적으로 동작
            //    (PLinMng 에 남아있는 이전 Master 설정 제거)
            try { ResetHardwareConfig(client, hwHandle); } catch { }

            // ⑥ 현재 하드웨어 상태 확인
            int curMode = 0, curBaud = 0;
            try { GetHardwareParam(hwHandle, HWP_MODE, out curMode, 0); } catch { }
            try { GetHardwareParam(hwHandle, HWP_BAUDRATE, out curBaud, 0); } catch { }

            // ⑦ Master 모드 + Baud Rate 초기화
            //    (이미 동일 설정이면 생략 가능하나, Reset 후이므로 항상 초기화)
            try { err = InitializeHardware(client, hwHandle, MODE_MASTER, _baudRate); }
            catch (Exception ex)
            { TryDisconnect(client, hwHandle); TryRemove(client);
              ErrorMessage?.Invoke("[PEAK] InitializeHardware 예외: " + ex.Message); return false; }
            if (err != ERR_OK)
            { TryDisconnect(client, hwHandle); TryRemove(client);
              ErrorMessage?.Invoke("[PEAK] 하드웨어 초기화 실패: " + ErrText(err) + "  (현재 BaudRate=" + _baudRate + ")"); return false; }

            // ⑧ 수신 필터: 모든 64개 Frame ID 허용 (단일 bitmask)
            try { SetClientFilter(client, hwHandle, FILTER_ALL); } catch { }

            _connected = true;
            _portName  = portName;
            StatusMessage?.Invoke(string.Format("[PEAK 연결] {0}  LIN BaudRate={1} bps", portName, _baudRate));
            return true;
        }

        private void TryRemove(byte c)                  { try { if (c != 0) RemoveClient(c); } catch { } }
        private void TryDisconnect(byte c, ushort h)    { try { if (c != 0 && h != 0) DisconnectClient(c, h); } catch { } }

        public void Disconnect()
        {
            StopSubscriberSchedule();
            try { if (_hClient != 0 && _hHw != 0) DisconnectClient(_hClient, _hHw); } catch { }
            try { if (_hClient != 0) RemoveClient(_hClient); } catch { }
            _connected = false;
            _hClient   = 0;
            _hHw       = 0;
            StatusMessage?.Invoke("[PEAK 연결해제]");
        }

        // ── 수신 폴링 ─────────────────────────────────────────────────────

        public void Poll()
        {
            if (!_connected) return;
            try
            {
                while (true)
                {
                    TLINRcvMsg rcv;
                    int err;
                    try { err = Read(_hClient, out rcv); }
                    catch { break; }

                    if (err == ERR_RCV_EMPTY || err != ERR_OK) break;
                    if (rcv.Type != MSGTYPE_STANDARD) continue;

                    LINFrame frame = ToLINFrame(rcv);
                    try { RawPacket?.Invoke(RawRx(rcv), "RX"); } catch { }
                    FrameReceived?.Invoke(frame);
                }
            }
            catch (Exception ex)
            { try { ErrorMessage?.Invoke("[PEAK 수신 오류] " + ex.Message); } catch { } }
        }

        // ─────────────────────────────────────────────────────────────────
        // 송신 – Master 모드에서 LIN_Write 단독으로 LIN 버스에 직접 전송
        // (Schedule 불필요 – 공식 레퍼런스 코드와 동일한 방식)
        //
        // 흐름:
        //   1. TLINMsg 구성 (FrameId = PID)
        //   2. LIN_CalculateChecksum → Checksum 필드 자동 계산
        //   3. LIN_Write → LIN 버스로 직접 전송
        // ─────────────────────────────────────────────────────────────────

        public bool SendPublisher(LINFrame frame)
        {
            if (!_connected) { ErrorMessage?.Invoke("[PEAK] 포트 미연결"); return false; }
            try
            {
                frame.Direction = LINDirection.TX;
                frame.FrameType = LINFrameType.Publisher;

                TLINMsg msg = BuildMsg(frame);

                // LIN 체크섬 자동 계산
                int calcErr;
                try { calcErr = CalculateChecksum(ref msg); }
                catch { calcErr = -1; }

                if (calcErr != ERR_OK)
                {
                    // CalculateChecksum 실패 시 수동 계산으로 대체
                    if (frame.ChecksumType == LINChecksumType.Enhanced)
                        msg.Checksum = LINFrame.CalcChecksumEnhanced(LINFrame.CalcPID(frame.FrameID), frame.Data, frame.DataLength);
                    else
                        msg.Checksum = LINFrame.CalcChecksumClassic(frame.Data, frame.DataLength);
                }

                int err;
                try { err = Write(_hClient, _hHw, ref msg); }
                catch (Exception ex) { ErrorMessage?.Invoke("[PEAK TX 예외] " + ex.Message); return false; }
                if (err != ERR_OK) { ErrorMessage?.Invoke("[PEAK TX 오류] " + ErrText(err)); return false; }

                try { RawPacket?.Invoke(RawTx(msg), "TX"); } catch { }
                FrameReceived?.Invoke(frame);
                return true;
            }
            catch (Exception ex)
            { try { ErrorMessage?.Invoke("[PEAK TX 오류] " + ex.Message); } catch { } return false; }
        }

        public bool SendSubscriberRequest(byte frameID, byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced)
        {
            if (!_connected) { ErrorMessage?.Invoke("[PEAK] 포트 미연결"); return false; }
            try
            {
                byte pid = LINFrame.CalcPID(frameID);

                TLINMsg msg = new TLINMsg
                {
                    FrameId      = pid,
                    Length       = dataLen,
                    Direction    = DIR_SUBSCRIBER,
                    ChecksumType = ToPeakCs(csType),
                    Data         = new byte[8],
                    Checksum     = 0
                };

                int err;
                try { err = Write(_hClient, _hHw, ref msg); }
                catch (Exception ex) { ErrorMessage?.Invoke("[PEAK REQ 예외] " + ex.Message); return false; }
                if (err != ERR_OK) { ErrorMessage?.Invoke("[PEAK REQ 오류] " + ErrText(err)); return false; }

                LINFrame log = new LINFrame
                {
                    Direction    = LINDirection.TX,
                    FrameID      = frameID,
                    DataLength   = dataLen,
                    FrameType    = LINFrameType.Subscriber,
                    ChecksumType = csType,
                    Data         = new byte[8]
                };
                FrameReceived?.Invoke(log);
                return true;
            }
            catch (Exception ex)
            { ErrorMessage?.Invoke("[PEAK REQ 오류] " + ex.Message); return false; }
        }

        public bool StartSubscriberSchedule(byte frameID, byte dataLen,
            LINChecksumType csType, ushort delayMs)
        {
            if (!_connected || frameID > 0x3F || dataLen == 0 || dataLen > 8 || delayMs == 0)
                return false;
            lock (_scheduleSync)
            {
                StopSubscriberScheduleInternal();
                TLINFrameEntry entry = new TLINFrameEntry
                {
                    FrameId = frameID,
                    Length = dataLen,
                    Direction = DIR_SUBSCRIBER,
                    ChecksumType = ToPeakCs(csType),
                    Flags = FRAME_FLAG_RESPONSE_ENABLE,
                    InitialData = new byte[8]
                };
                int err = SetFrameEntry(_hClient, _hHw, ref entry);
                if (err != ERR_OK)
                {
                    ErrorMessage?.Invoke("[PEAK Schedule Frame 오류] " + ErrText(err));
                    return false;
                }

                TLINScheduleSlot[] slots = new TLINScheduleSlot[1];
                slots[0] = new TLINScheduleSlot
                {
                    Type = SLOT_UNCONDITIONAL,
                    Delay = delayMs,
                    FrameId = new byte[8],
                    CountResolve = 0,
                    Handle = 0
                };
                slots[0].FrameId[0] = frameID;
                err = SetSchedule(_hClient, _hHw, FAST_SCHEDULE_NUMBER, slots, slots.Length);
                if (err != ERR_OK)
                {
                    ErrorMessage?.Invoke("[PEAK Schedule 설정 오류] " + ErrText(err));
                    return false;
                }
                _subscriberScheduleConfigured = true;
                err = StartSchedule(_hClient, _hHw, FAST_SCHEDULE_NUMBER);
                if (err != ERR_OK)
                {
                    StopSubscriberScheduleInternal();
                    ErrorMessage?.Invoke("[PEAK Schedule 시작 오류] " + ErrText(err));
                    return false;
                }
                _subscriberScheduleRunning = true;
                StatusMessage?.Invoke(string.Format("[PEAK LIN 고속 Read] ID=0x{0:X2}, 주기={1}ms", frameID, delayMs));
                return true;
            }
        }

        public bool SuspendSubscriberSchedule()
        {
            lock (_scheduleSync)
            {
                if (!_subscriberScheduleConfigured || !_subscriberScheduleRunning)
                    return _subscriberScheduleConfigured;
                int err = SuspendSchedule(_hClient, _hHw);
                if (err != ERR_OK)
                    return false;
                _subscriberScheduleRunning = false;
                return true;
            }
        }

        public bool ResumeSubscriberSchedule()
        {
            lock (_scheduleSync)
            {
                if (!_subscriberScheduleConfigured)
                    return false;
                if (_subscriberScheduleRunning)
                    return true;
                int err = ResumeSchedule(_hClient, _hHw);
                if (err != ERR_OK)
                    return false;
                _subscriberScheduleRunning = true;
                return true;
            }
        }

        public void StopSubscriberSchedule()
        {
            lock (_scheduleSync)
            {
                StopSubscriberScheduleInternal();
            }
        }

        private void StopSubscriberScheduleInternal()
        {
            if (_hClient != 0 && _hHw != 0)
            {
                if (_subscriberScheduleRunning)
                {
                    try { SuspendSchedule(_hClient, _hHw); } catch { }
                }
                if (_subscriberScheduleConfigured)
                {
                    try { DeleteSchedule(_hClient, _hHw, FAST_SCHEDULE_NUMBER); } catch { }
                }
            }
            _subscriberScheduleRunning = false;
            _subscriberScheduleConfigured = false;
        }

        // ── 내부 헬퍼 ─────────────────────────────────────────────────────

        private byte ToPeakCs(LINChecksumType cs)
            => cs == LINChecksumType.Enhanced ? CS_ENHANCED : CS_CLASSIC;

        private TLINMsg BuildMsg(LINFrame frame)
        {
            TLINMsg msg = new TLINMsg
            {
                FrameId      = LINFrame.CalcPID(frame.FrameID), // PID (ID + parity)
                Length       = frame.DataLength,
                Direction    = DIR_PUBLISHER,
                ChecksumType = ToPeakCs(frame.ChecksumType),
                Data         = new byte[8],
                Checksum     = 0
            };
            for (int i = 0; i < frame.DataLength; i++)
                msg.Data[i] = frame.Data[i];
            return msg;
        }

        private LINFrame ToLINFrame(TLINRcvMsg rcv)
        {
            return new LINFrame
            {
                Direction    = LINDirection.RX,
                FrameID      = (byte)(rcv.FrameId & 0x3F),   // PID → raw ID
                DataLength   = rcv.Length,
                ChecksumType = rcv.ChecksumType == CS_ENHANCED
                    ? LINChecksumType.Enhanced : LINChecksumType.Classic,
                FrameType    = rcv.Direction == DIR_PUBLISHER
                    ? LINFrameType.Publisher : LINFrameType.Subscriber,
                IsValid      = (rcv.ErrorFlags == 0),
                IsACK        = false,
                Data         = rcv.Data != null ? (byte[])rcv.Data.Clone() : new byte[8],
                PEC          = rcv.Checksum
            };
        }

        private byte[] RawRx(TLINRcvMsg rcv)
        {
            int n = Math.Min((int)rcv.Length, 8);
            byte[] r = new byte[3 + n];
            r[0] = 0xA5; r[1] = rcv.FrameId; r[2] = rcv.Checksum;
            if (rcv.Data != null) Array.Copy(rcv.Data, 0, r, 3, n);
            return r;
        }

        private byte[] RawTx(TLINMsg msg)
        {
            int n = Math.Min((int)msg.Length, 8);
            byte[] r = new byte[3 + n];
            r[0] = 0x55; r[1] = msg.FrameId; r[2] = msg.Checksum;
            if (msg.Data != null) Array.Copy(msg.Data, 0, r, 3, n);
            return r;
        }

        private string ErrText(int err)
        {
            try
            {
                StringBuilder sb = new StringBuilder(256);
                GetErrorText(err, 0, sb, 256);
                string s = sb.ToString();
                return string.IsNullOrEmpty(s) ? string.Format("0x{0:X8}", err) : s;
            }
            catch { return string.Format("0x{0:X8}", err); }
        }

        // ── 정적: 장치 목록 ───────────────────────────────────────────────
        //
        // [개선 사항]
        //   1. DLL 존재 여부만 확인 후 즉시 해제 (LoadLibrary/FreeLibrary 충돌 방지)
        //   2. RegisterClient 실패 시 재시도 로직 추가
        //   3. tmpClient 항상 안전하게 해제 (finally 보장)
        //   4. GetAvailableHardware 버퍼 크기를 바이트 단위로 정확히 계산
        //   5. PLinMng 서비스가 아직 준비되지 않은 경우 짧은 대기 후 재시도
        //   6. 전체를 별도 스레드로 실행하지 않고 호출부에서 STA/MTA 고려

        private static readonly object _deviceQueryLock = new object();

        public static string[] GetAvailableDevices()
        {
            // 동시 다중 호출 방지 (간헐적 크래시 원인 중 하나)
            lock (_deviceQueryLock)
            {
                return GetAvailableDevicesInternal();
            }
        }

        private static string[] GetAvailableDevicesInternal()
        {
            // ① DLL 존재 확인 (LoadLibrary 없이 파일 존재만 체크)
            try
            {
                string dllPath = System.IO.Path.Combine(
                    System.IO.Path.GetDirectoryName(
                        System.Reflection.Assembly.GetExecutingAssembly().Location) ?? "",
                    PLIN_DLL);

                // 실행 파일 폴더에 없으면 System32 등 시스템 경로 탐색
                if (!System.IO.File.Exists(dllPath))
                {
                    string sys32 = System.IO.Path.Combine(
                        Environment.GetFolderPath(Environment.SpecialFolder.System), PLIN_DLL);
                    if (!System.IO.File.Exists(sys32))
                        return new string[0];   // DLL 없음 → 조용히 빈 배열 반환
                }
            }
            catch { return new string[0]; }

            // ② PLinMng 서비스 연결 재시도 (최대 2회, 200ms 간격)
            for (int attempt = 0; attempt < 2; attempt++)
            {
                byte tmpClient = 0;
                bool clientRegistered = false;
                try
                {
                    // 클라이언트 등록
                    string name = "LINQuery_" + Guid.NewGuid().ToString("N").Substring(0, 6);
                    int err = RegisterClient(name, IntPtr.Zero, out tmpClient);

                    if (err != ERR_OK || tmpClient == 0)
                    {
                        // PLinMng 아직 준비 안 됨 → 잠시 대기 후 재시도
                        if (attempt == 0)
                        {
                            System.Threading.Thread.Sleep(200);
                            continue;
                        }
                        return new string[0];
                    }
                    clientRegistered = true;

                    // 하드웨어 목록 조회
                    // wBuffSize = 배열 요소 수 × 2 (ushort = 2 bytes)
                    ushort[] hwArr = new ushort[16];
                    ushort   count = 0;
                    err = GetAvailableHardware(hwArr, (ushort)(hwArr.Length * 2), out count);

                    if (err != ERR_OK || count == 0)
                        return new string[0];

                    // 장치 이름 목록 생성
                    int     safeCount = Math.Min((int)count, hwArr.Length);
                    string[] names    = new string[safeCount];
                    for (int i = 0; i < safeCount; i++)
                        names[i] = string.Format("PEAK#{0}", i + 1);

                    return names;
                }
                catch (Exception ex)
                {
                    // P/Invoke 예외는 드라이버 미설치 또는 관리자 권한 문제
                    System.Diagnostics.Debug.WriteLine("[PEAK] GetAvailableDevices 예외: " + ex.Message);
                    return new string[0];
                }
                finally
                {
                    // ③ 등록된 클라이언트는 finally 에서 반드시 해제
                    //    (예외 발생 여부와 무관하게 항상 실행)
                    if (clientRegistered && tmpClient != 0)
                    {
                        try { RemoveClient(tmpClient); } catch { }
                    }
                }
            }

            return new string[0];
        }

        public void Dispose()
        {
            if (!_disposed) { Disconnect(); _disposed = true; }
        }
    }

    internal static class NativeMethods
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr LoadLibrary(string libname);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern bool FreeLibrary(IntPtr hModule);
    }
}
