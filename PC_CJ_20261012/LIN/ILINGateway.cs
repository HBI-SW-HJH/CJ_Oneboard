using System;

namespace LINMaster.LIN
{
    // =========================================================================
    // ILINGateway – IndieMicro / PEAK LIN 게이트웨이 공통 인터페이스
    //
    // [역할]
    //   - LINGateway (IndieMicro SerialPort 방식) 와
    //     PeakLINGateway (PEAK PLIN-USB 방식) 모두 이 인터페이스를 구현
    //   - FormMain, FormCIE, FormNodeScan 은 ILINGateway 만 참조하므로
    //     장치 선택과 무관하게 동일한 코드로 동작
    // =========================================================================
    public interface ILINGateway : IDisposable
    {
        // ── 상태 ──────────────────────────────────────────────────────────
        bool   IsConnected { get; }
        string PortName    { get; }   // COM 포트 이름 또는 PEAK 장치 이름

        // ── 이벤트 ────────────────────────────────────────────────────────
        event Action<LINFrame> FrameReceived;
        event Action<string>   StatusMessage;
        event Action<string>   ErrorMessage;
        event Action<byte[], string> RawPacket;

        // ── 연결 / 해제 ───────────────────────────────────────────────────
        bool Connect(string portName);
        void Disconnect();

        // ── 수신 폴링 (timerRx.Tick 에서 호출) ───────────────────────────
        void Poll();

        // ── 송신 ─────────────────────────────────────────────────────────
        bool SendPublisher(LINFrame frame);
        bool SendSubscriberRequest(byte frameID, byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced);
    }

    public class FT4222Gateway : ILINGateway
    {
        private readonly FT4222.FT4222Manager _device;
        private string _portName = string.Empty;
        private bool _disposed;

        public FT4222Gateway(FT4222.FT4222Manager device)
        {
            _device = device ?? throw new ArgumentNullException(nameof(device));
        }

        public bool IsConnected => _device.IsConnected;
        public string PortName => _portName;

        /// <summary>FT4222Manager 인스턴스를 직접 접근할 수 있도록 노출합니다. (FormMacChange 전용)</summary>
        public FT4222.FT4222Manager Device => _device;
        public bool UseOtpMacMode
        {
            get => _device.UseOtpMacMode;
            set => _device.UseOtpMacMode = value;
        }

        public event Action<LINFrame> FrameReceived;
        public event Action<string> StatusMessage;
        public event Action<string> ErrorMessage;
        public event Action<byte[], string> RawPacket;

        public bool Connect(string portName)
        {
            string selected = string.IsNullOrWhiteSpace(portName) ? null : portName;
            if (selected != null && selected.StartsWith("(", StringComparison.Ordinal))
                selected = null;

            bool ok = _device.Connect(selected);
            if (ok)
            {
                _portName = selected ?? "FT4222";
                StatusMessage?.Invoke("[FT4222 connected] " + _portName);
                return true;
            }

            ErrorMessage?.Invoke("[FT4222 connect failed] " + _device.LastError);
            _portName = string.Empty;
            return false;
        }

        public void Disconnect()
        {
            _device.Disconnect();
            StatusMessage?.Invoke("[FT4222 disconnected]");
            _portName = string.Empty;
        }

        public void Poll()
        {
            // FT4222/OSP LED control is synchronous, so there is no periodic RX polling.
        }

        public bool SendPublisher(LINFrame frame)
        {
            if (!IsConnected)
            {
                ErrorMessage?.Invoke("FT4222 is not connected.");
                return false;
            }

            frame.Direction = LINDirection.TX;
            frame.FrameType = LINFrameType.Publisher;
            FrameReceived?.Invoke(frame);

            try
            {
                byte[] data = new byte[frame.DataLength];
                Array.Copy(frame.Data, data, frame.DataLength);
                RawPacket?.Invoke(data, "TX");
            }
            catch { }

            return true;
        }

        public bool SendSubscriberRequest(byte frameID, byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced)
        {
            if (!IsConnected)
            {
                ErrorMessage?.Invoke("FT4222 is not connected.");
                return false;
            }

            LINFrame logFrame = new LINFrame();
            logFrame.Direction = LINDirection.TX;
            logFrame.FrameID = frameID;
            logFrame.DataLength = dataLen;
            logFrame.FrameType = LINFrameType.Subscriber;
            logFrame.ChecksumType = csType;
            for (int i = 0; i < dataLen && i < logFrame.Data.Length; i++)
                logFrame.Data[i] = 0xFF;

            FrameReceived?.Invoke(logFrame);
            return true;
        }

        public void Dispose()
        {
            if (_disposed) return;
            Disconnect();
            _disposed = true;
        }
    }
}
