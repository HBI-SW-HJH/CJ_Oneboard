using System;
using System.IO.Ports;

namespace LINMaster.LIN
{
    // =========================================================================
    // RS485Gateway – USB to RS485 직접 통신 담당 클래스
    //
    // [역할]
    //   - SerialPort (115200, 8N1) 를 통해 RS485 장치와 통신
    //   - LINGateway와 동일한 ILINGateway 인터페이스 구현
    //   - 동일한 LIN 패킷 포맷(LINProtocol) 을 사용하여 송수신
    //   - 수신 바이트는 Poll() 을 통해 읽어 파서에 전달
    //
    // [스레드 구조]
    //   ★ 백그라운드 스레드 없음 ★
    //   FormMain.timerRx.Tick(20ms) → _gw.Poll() 호출 → 수신 바이트 처리
    //   모든 이벤트가 UI 스레드에서 발생 → InvokeRequired 완전 불필요
    //
    // [통신 설정]
    //   BaudRate : 115200
    //   DataBits : 8
    //   Parity   : None
    //   StopBits : 1
    // =========================================================================
    public class RS485Gateway : ILINGateway
    {
        // ──────────────────────────────────────────────────────────────────
        // 멤버 변수
        // ──────────────────────────────────────────────────────────────────

        private SerialPort      _port;
        private LINStreamParser _parser;
        private bool            _disposed;

        public const int RS485_BAUD = 115200;   // RS485 통신 속도


        // ──────────────────────────────────────────────────────────────────
        // 프로퍼티
        // ──────────────────────────────────────────────────────────────────

        public bool IsConnected
        {
            get { return _port != null && _port.IsOpen; }
        }

        public string PortName
        {
            get { return _port != null ? _port.PortName : ""; }
        }


        // ──────────────────────────────────────────────────────────────────
        // 이벤트
        // ──────────────────────────────────────────────────────────────────

        public event Action<LINFrame> FrameReceived;
        public event Action<string>   StatusMessage;
        public event Action<string>   ErrorMessage;
        public event Action<byte[], string> RawPacket;


        // =========================================================================
        // 생성자
        // =========================================================================

        public RS485Gateway()
        {
            _port = new SerialPort();
            InitParser();
        }

        private void InitParser()
        {
            _parser = new LINStreamParser();
            _parser.FrameReceived += OnRxFrame;
            _parser.ParseError    += (msg) => ErrorMessage?.Invoke("[파서] " + msg);
        }


        // =========================================================================
        // 연결 / 해제
        // =========================================================================

        public bool Connect(string portName)
        {
            try
            {
                _port.BaudRate     = RS485_BAUD;
                _port.DataBits     = 8;
                _port.Parity       = Parity.None;
                _port.StopBits     = StopBits.One;
                _port.DtrEnable    = true;
                _port.RtsEnable    = true;
                _port.ReadTimeout  = 0;    // 폴링 방식 → 블로킹 없이 즉시 반환
                _port.WriteTimeout = 500;
                _port.PortName     = portName;
                _port.Open();
                _parser.Clear();

                StatusMessage?.Invoke(
                    string.Format("[RS485 연결] {0}  BaudRate={1}", portName, RS485_BAUD));
                return true;
            }
            catch (Exception ex)
            {
                ErrorMessage?.Invoke("[RS485 연결 실패] " + ex.Message);
                return false;
            }
        }

        public void Disconnect()
        {
            try
            {
                _port?.Close();
                StatusMessage?.Invoke("[RS485 연결해제]");
            }
            catch (Exception ex)
            {
                ErrorMessage?.Invoke("[RS485 해제 오류] " + ex.Message);
            }
        }


        // =========================================================================
        // 수신 폴링 – timerRx.Tick 에서 호출
        // =========================================================================

        public void Poll()
        {
            if (!IsConnected) return;

            try
            {
                int avail = _port.BytesToRead;
                if (avail <= 0) return;

                byte[] buf = new byte[avail];
                int    n   = _port.Read(buf, 0, avail);
                if (n <= 0) return;

                if (n < buf.Length)
                {
                    byte[] tmp = new byte[n];
                    Array.Copy(buf, tmp, n);
                    buf = tmp;
                }

                try { RawPacket?.Invoke(buf, "RX"); } catch { }
                try { _parser.Feed(buf); } catch { }
            }
            catch (Exception ex)
            {
                try { ErrorMessage?.Invoke("[RS485 수신 오류] " + ex.Message); } catch { }
            }
        }


        // =========================================================================
        // 데이터 송신
        // =========================================================================

        public bool SendPublisher(LINFrame frame)
        {
            if (!IsConnected)
            {
                ErrorMessage?.Invoke("RS485 포트 미연결");
                return false;
            }

            try
            {
                frame.Direction = LINDirection.TX;
                frame.FrameType = LINFrameType.Publisher;

                byte[] pkt = LINProtocol.BuildSendPacket(frame);
                _port.Write(pkt, 0, pkt.Length);

                try { RawPacket?.Invoke(pkt, "TX"); } catch { }
                FrameReceived?.Invoke(frame);
                return true;
            }
            catch (Exception ex)
            {
                try { ErrorMessage?.Invoke("[RS485 전송 오류] " + ex.Message); } catch { }
                return false;
            }
        }

        public bool SendSubscriberRequest(byte frameID, byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced)
        {
            if (!IsConnected)
            {
                ErrorMessage?.Invoke("RS485 포트 미연결");
                return false;
            }

            try
            {
                byte[] pkt = LINProtocol.BuildRequestPacket(frameID, dataLen, csType);

                LINFrame logFrame = new LINFrame();
                logFrame.Direction    = LINDirection.TX;
                logFrame.FrameID      = frameID;
                logFrame.DataLength   = dataLen;
                logFrame.FrameType    = LINFrameType.Subscriber;
                logFrame.ChecksumType = csType;
                for (int i = 0; i < dataLen; i++) logFrame.Data[i] = 0xFF;
                logFrame.PEC = LINFrame.CalcPEC(
                    logFrame.BuildControlByte(), logFrame.Data, dataLen);

                _port.Write(pkt, 0, pkt.Length);

                try { RawPacket?.Invoke(pkt, "TX"); } catch { }
                FrameReceived?.Invoke(logFrame);
                return true;
            }
            catch (Exception ex)
            {
                ErrorMessage?.Invoke("[RS485 요청 오류] " + ex.Message);
                return false;
            }
        }


        // =========================================================================
        // 내부 콜백
        // =========================================================================

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
                _port?.Dispose();
                _disposed = true;
            }
        }
    }
}
