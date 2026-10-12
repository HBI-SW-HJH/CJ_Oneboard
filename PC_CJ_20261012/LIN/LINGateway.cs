using System;
using System.IO.Ports;

namespace LINMaster.LIN
{
    // =========================================================================
    // LINGateway – IndieMicro LIN Gateway V2.0 통신 담당 클래스
    //
    // [역할]
    //   - SerialPort 를 통해 IndieMicro Gateway 와 통신
    //   - LINFrame → 0x55 패킷으로 변환하여 전송 (LINProtocol 이용)
    //   - 수신 바이트는 Poll() 을 통해 읽어 파서에 전달
    //   - 결과를 이벤트(FrameReceived, StatusMessage 등)로 외부에 알림
    //
    // [스레드 구조]
    //   ★ 백그라운드 스레드 없음 ★
    //   FormMain.timerRx.Tick(20ms) → _gw.Poll() 호출 → 수신 바이트 처리
    //   모든 이벤트가 UI 스레드에서 발생 → InvokeRequired 완전 불필요
    //
    // [타이머 구성]  FormMain.Designer.cs 에서 관리
    //   timerRx    : RX 수신 폴링  Interval=20ms  연결 시 Start / 해제 시 Stop
    //   timer1     : 스케줄 TX     Interval=가변   스케줄 시작/중지 시 토글
    // =========================================================================
    public class LINGateway : ILINGateway
    {
        // ──────────────────────────────────────────────────────────────────
        // 멤버 변수
        // ──────────────────────────────────────────────────────────────────

        private SerialPort      _port;          // COM 포트 객체
        private LINStreamParser _parser;        // 수신 바이트 → LINFrame 파서
        private bool            _disposed;      // Dispose 중복 호출 방지
        private bool            _externalPort;  // true = 외부(Designer)에서 주입된 포트
        private readonly bool   _enableModemControlLines;
        private readonly object _ioSync = new object();

        public const int COM_BAUD = 115200;     // PDF 명시 BaudRate
        public int BaudRate { get; set; } = COM_BAUD;


        // ──────────────────────────────────────────────────────────────────
        // 프로퍼티
        // ──────────────────────────────────────────────────────────────────

        /// <summary>현재 포트가 열려있으면 true</summary>
        public bool IsConnected
        {
            get { return _port != null && _port.IsOpen; }
        }

        /// <summary>현재 연결된 COM 포트 이름, 없으면 빈 문자열</summary>
        public string PortName
        {
            get { return _port != null ? _port.PortName : ""; }
        }


        // ──────────────────────────────────────────────────────────────────
        // 이벤트
        // ★ Poll() 은 UI 스레드(timerRx.Tick)에서 호출되므로
        //   아래 이벤트들도 모두 UI 스레드에서 발생
        //   → 핸들러에서 InvokeRequired 체크 불필요
        // ──────────────────────────────────────────────────────────────────

        /// <summary>TX 완료 또는 RX 프레임 수신 시 발생</summary>
        public event Action<LINFrame> FrameReceived;

        /// <summary>연결/해제 상태 메시지</summary>
        public event Action<string> StatusMessage;

        /// <summary>오류 메시지</summary>
        public event Action<string> ErrorMessage;

        /// <summary>RAW 바이트 로그 (byte[], "TX" or "RX")</summary>
        public event Action<byte[], string> RawPacket;


        // =========================================================================
        // 생성자
        // =========================================================================

        /// <summary>기본 생성자 – SerialPort 내부 생성</summary>
        public LINGateway()
            : this(true)
        {
        }

        public LINGateway(bool enableModemControlLines)
        {
            _port         = new SerialPort();
            _externalPort = false;
            _enableModemControlLines = enableModemControlLines;
            InitParser();
        }

        /// <summary>
        /// Designer 컴포넌트(serialPort1) 주입 생성자
        /// Visual Studio Properties 패널에서 BaudRate 등 직접 설정 가능
        /// </summary>
        public LINGateway(SerialPort externalPort)
        {
            if (externalPort != null)
            {
                _port         = externalPort;
                _externalPort = true;
            }
            else
            {
                _port         = new SerialPort();
                _externalPort = false;
            }
            _enableModemControlLines = true;
            InitParser();
        }

        /// <summary>파서 초기화 (생성자 공통)</summary>
        private void InitParser()
        {
            _parser = new LINStreamParser();
            _parser.FrameReceived += OnRxFrame;
            _parser.ParseError    += (msg) => ErrorMessage?.Invoke("[파서] " + msg);
        }


        // =========================================================================
        // 연결 / 해제
        // =========================================================================

        /// <summary>
        /// 지정된 COM 포트로 연결
        /// ★ 스레드를 시작하지 않음
        ///   연결 성공 후 FormMain 이 timerRx.Start() 를 호출해야 수신 시작
        /// </summary>
        public bool Connect(string portName)
        {
            try
            {
                if (!_externalPort)
                {
                    _port.BaudRate     = BaudRate;
                    _port.DataBits     = 8;
                    _port.Parity       = Parity.None;
                    _port.StopBits     = StopBits.One;
                    _port.DtrEnable    = _enableModemControlLines;
                    _port.RtsEnable    = _enableModemControlLines;
                    _port.ReadTimeout  = 0;    // 폴링 방식 → 블로킹 없이 즉시 반환
                    _port.WriteTimeout = 500;
                }

                _port.PortName = portName;
                _port.Open();
                _parser.Clear();  // 이전 수신 버퍼 초기화

                StatusMessage?.Invoke(
                    string.Format("[연결] {0}  BaudRate={1}", portName, _port.BaudRate));
                return true;
            }
            catch (Exception ex)
            {
                ErrorMessage?.Invoke("[연결 실패] " + ex.Message);
                return false;
            }
        }

        /// <summary>
        /// COM 포트 연결 해제
        /// ★ timerRx.Stop() 은 FormMain 이 담당 (Connect/Disconnect 쌍으로 관리)
        /// </summary>
        public void Disconnect()
        {
            try
            {
                _port?.Close();
                StatusMessage?.Invoke("[연결해제]");
            }
            catch (Exception ex)
            {
                ErrorMessage?.Invoke("[해제 오류] " + ex.Message);
            }
        }


        // =========================================================================
        // 수신 폴링 – timerRx.Tick 에서 호출
        // =========================================================================

        /// <summary>
        /// FormMain.timerRx_Tick(Interval=20ms) 에서 주기적으로 호출
        ///
        /// SerialPort.BytesToRead 로 수신 버퍼 크기 확인 후
        /// 데이터가 있으면 읽어서 LINStreamParser 에 전달
        ///
        /// ★ UI 스레드에서 실행되므로 이벤트 발생 시 InvokeRequired 불필요
        /// </summary>
        public void Poll()
        {
            if (!IsConnected) return;

            try
            {
                lock (_ioSync)
                {
                    // 수신 버퍼에 데이터가 없으면 즉시 반환 (CPU 낭비 없음)
                    int avail = _port.BytesToRead;
                    if (avail <= 0) return;

                    byte[] buf = new byte[avail];
                    int n = _port.Read(buf, 0, avail);
                    if (n <= 0) return;

                    // 실제로 읽은 바이트만 사용
                    if (n < buf.Length)
                    {
                        byte[] tmp = new byte[n];
                        Array.Copy(buf, tmp, n);
                        buf = tmp;
                    }

                    // RAW 로그 이벤트 발생
                    try { RawPacket?.Invoke(buf, "RX"); } catch { }

                    // 파서에 전달 → 완성된 패킷이 생기면 OnRxFrame() 이 호출됨
                    try { _parser.Feed(buf); } catch { }
                }
            }
            catch (Exception ex)
            {
                try { ErrorMessage?.Invoke("[수신 오류] " + ex.Message); } catch { }
            }
        }


        // =========================================================================
        // 데이터 송신
        // =========================================================================

        /// <summary>
        /// Publisher 프레임 전송
        /// 패킷: [0x55][FrameID][PEC][ControlByte][Data...]
        /// </summary>
        public bool SendPublisher(LINFrame frame)
        {
            if (!IsConnected)
            {
                ErrorMessage?.Invoke("포트 미연결");
                return false;
            }

            try
            {
                frame.Direction = LINDirection.TX;
                frame.FrameType = LINFrameType.Publisher;

                byte[] pkt = LINProtocol.BuildSendPacket(frame);
                lock (_ioSync)
                    _port.Write(pkt, 0, pkt.Length);

                try { RawPacket?.Invoke(pkt, "TX"); } catch { }
                FrameReceived?.Invoke(frame);
                return true;
            }
            catch (Exception ex)
            {
                try { ErrorMessage?.Invoke("[전송 오류] " + ex.Message); } catch { }
                return false;
            }
        }

        /// <summary>
        /// Subscriber 요청 전송
        /// 패킷: [0x55][FrameID][PEC][ControlByte][0xFF × DataLen]
        /// 실제 응답은 Poll() → OnRxFrame() 경로로 수신됨
        /// </summary>
        public bool SendSubscriberRequest(byte frameID, byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced)
        {
            if (!IsConnected)
            {
                ErrorMessage?.Invoke("포트 미연결");
                return false;
            }

            try
            {
                byte[] pkt = LINProtocol.BuildRequestPacket(frameID, dataLen, csType);

                // 로그용 더미 프레임 생성
                LINFrame logFrame = new LINFrame();
                logFrame.Direction    = LINDirection.TX;
                logFrame.FrameID      = frameID;
                logFrame.DataLength   = dataLen;
                logFrame.FrameType    = LINFrameType.Subscriber;
                logFrame.ChecksumType = csType;
                for (int i = 0; i < dataLen; i++) logFrame.Data[i] = 0xFF;
                logFrame.PEC = LINFrame.CalcPEC(
                    logFrame.BuildControlByte(), logFrame.Data, dataLen);

                lock (_ioSync)
                    _port.Write(pkt, 0, pkt.Length);

                try { RawPacket?.Invoke(pkt, "TX"); } catch { }
                FrameReceived?.Invoke(logFrame);
                return true;
            }
            catch (Exception ex)
            {
                ErrorMessage?.Invoke("[요청 오류] " + ex.Message);
                return false;
            }
        }


        // =========================================================================
        // 내부 콜백
        // =========================================================================

        /// <summary>파서가 완성한 LINFrame → RX 방향 설정 후 이벤트 발생</summary>
        private void OnRxFrame(LINFrame frame)
        {
            frame.Direction = LINDirection.RX;
            FrameReceived?.Invoke(frame);
        }


        // =========================================================================
        // 리소스 해제
        // =========================================================================

        public void Dispose()
        {
            if (!_disposed)
            {
                Disconnect();
                if (!_externalPort) _port?.Dispose();
                _disposed = true;
            }
        }
    }
}
