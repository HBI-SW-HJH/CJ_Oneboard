using System;
using System.Net;
using System.Net.Sockets;
using System.IO.Ports;
using System.Threading;

namespace LINMaster
{
    // =========================================================================
    // TcpBridge – Android 에뮬레이터 테스트용 TCP-Serial 브리지
    //
    // [동작]
    //   LINMaster.exe 하단 [🔌 에뮬레이터 브리지] 버튼 클릭
    //   → TCP :7788 서버 시작
    //   → 에뮬레이터 앱이 adb reverse 를 통해 localhost:7788 연결
    //   → TCP ↔ COM 포트 바이트 그대로 포워딩
    //
    // [사용 순서]
    //   1. LINMaster.exe 하단 [🔌 에뮬레이터 브리지] 클릭 (COM 포트 선택됨 상태)
    //   2. CMD: adb reverse tcp:7788 tcp:7788
    //   3. 에뮬레이터 앱 → 연결 모드: TCP → localhost:7788 → [연결]
    // =========================================================================
    public class TcpBridge : IDisposable
    {
        public const int TCP_PORT = 7788;

        private TcpListener   _listener;
        private SerialPort    _serial;
        private Thread        _acceptThread;
        private Thread        _rxThread;
        private Thread        _txThread;
        private TcpClient     _client;
        private NetworkStream _stream;
        private bool          _running;
        private bool          _disposed;

        public bool   IsRunning    => _running;
        public string CurrentPort  => _serial?.PortName ?? "";

        public event Action<string> OnStatus;
        public event Action<string> OnError;

        // ── 시작 ──────────────────────────────────────────────────────
        public bool Start(string comPort, int baud = 115200)
        {
            if (_running) return true;
            try
            {
                // COM 포트 열기
                _serial = new SerialPort(comPort, baud, Parity.None, 8, StopBits.One)
                {
                    DtrEnable    = true,
                    RtsEnable    = true,
                    ReadTimeout  = 50,
                    WriteTimeout = 500
                };
                _serial.Open();

                // TCP 서버 시작
                _listener = new TcpListener(IPAddress.Any, TCP_PORT);
                _listener.Start();
                _running = true;

                OnStatus?.Invoke($"[브리지] 시작됨 – {comPort} ↔ TCP:{TCP_PORT}");
                OnStatus?.Invoke($"[안내] CMD 에서 실행: adb reverse tcp:{TCP_PORT} tcp:{TCP_PORT}");

                // 클라이언트 수락 스레드
                _acceptThread = new Thread(AcceptLoop) { IsBackground = true };
                _acceptThread.Start();
                return true;
            }
            catch (Exception ex)
            {
                OnError?.Invoke($"[브리지 오류] {ex.Message}");
                _serial?.Close();
                return false;
            }
        }

        // ── 중지 ──────────────────────────────────────────────────────
        public void Stop()
        {
            _running = false;
            try { _client?.Close(); }      catch { }
            try { _listener?.Stop(); }     catch { }
            try { _serial?.Close(); }      catch { }
            _client = null; _stream = null;
            OnStatus?.Invoke("[브리지] 중지됨");
        }

        // ── 클라이언트 수락 루프 ──────────────────────────────────────
        private void AcceptLoop()
        {
            while (_running)
            {
                try
                {
                    _listener.Server.Blocking = true;
                    var client = _listener.AcceptTcpClient();
                    client.NoDelay = true;

                    // 이전 연결 정리
                    try { _client?.Close(); } catch { }
                    _rxThread?.Abort();
                    _txThread?.Abort();

                    _client = client;
                    _stream = client.GetStream();
                    _serial?.DiscardInBuffer();
                    _serial?.DiscardOutBuffer();

                    OnStatus?.Invoke($"[브리지] 에뮬레이터 연결됨 – {client.Client.RemoteEndPoint}");

                    // TCP → Serial (앱 → COM)
                    _txThread = new Thread(() => TcpToSerial(client, _stream))
                    { IsBackground = true };
                    _txThread.Start();

                    // Serial → TCP (COM → 앱)
                    _rxThread = new Thread(() => SerialToTcp(_stream))
                    { IsBackground = true };
                    _rxThread.Start();
                }
                catch (Exception ex)
                {
                    if (_running)
                        OnError?.Invoke($"[브리지] Accept 오류: {ex.Message}");
                    Thread.Sleep(500);
                }
            }
        }

        // ── TCP → Serial ──────────────────────────────────────────────
        private void TcpToSerial(TcpClient client, NetworkStream stream)
        {
            var buf = new byte[4096];
            try
            {
                while (_running && client.Connected)
                {
                    int n = stream.Read(buf, 0, buf.Length);
                    if (n <= 0) break;
                    _serial?.Write(buf, 0, n);
                }
            }
            catch { }
            OnStatus?.Invoke("[브리지] 에뮬레이터 연결 해제");
        }

        // ── Serial → TCP ──────────────────────────────────────────────
        private void SerialToTcp(NetworkStream stream)
        {
            var buf = new byte[4096];
            try
            {
                while (_running)
                {
                    int n = 0;
                    try { n = _serial.Read(buf, 0, buf.Length); }
                    catch (TimeoutException) { continue; }
                    if (n <= 0) continue;
                    stream.Write(buf, 0, n);
                    stream.Flush();
                }
            }
            catch { }
        }

        public void Dispose()
        {
            if (!_disposed) { Stop(); _disposed = true; }
        }
    }
}
