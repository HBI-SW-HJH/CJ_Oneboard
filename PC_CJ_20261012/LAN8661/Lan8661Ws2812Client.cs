using System;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Threading;

namespace LINMaster.LAN8661
{
    /// <summary>
    /// Combined LAN8661 output client for the OSP/ISE/WS2812 configuration.
    /// First WS2812, Click 3 OSP and Click 4 ILaS are controlled through the
    /// acknowledged remote-SPI ports.  First uses SERCOM0 MOSI as a timed
    /// one-wire waveform; it is not an RTP display in this configuration.
    /// </summary>
    public sealed class Lan8661Ws2812Client : IDisposable
    {
        public const int VideoWidth = 20;
        public const int VideoHeight = 10;
        public const int ExistingLedWidth = 10;
        public const int ExistingLedCount = ExistingLedWidth * VideoHeight;
        public const int IlasLedCount = 21;
        public const int BytesPerPixel = 3;
        public const int FrameByteCount = VideoWidth * VideoHeight * BytesPerPixel;
        public const int DefaultRtpPort = 5001;

        private const int RtpHeaderSize = 12;
        private const int ExtendedSequenceSize = 2;
        private const int ScanLineHeaderSize = 6;
        private const byte DynamicPayloadType = 96;

        private readonly object _sync = new object();
        private UdpClient _udpClient;
        private IPEndPoint _targetEndpoint;
        private Lan8661SomeIpSpi _spi;
        private uint _sequence = 1;
        private uint _timestamp;
        private readonly uint _ssrc = 0x4C414E31; // "LAN1"
        private bool _ilasInitialized;

        /// <summary>
        /// ILaS encoder. Input is R/G/B and output is the complete chain of
        /// 96-bit wire packets to send through Click 4/SERCOM3.
        /// </summary>
        public Func<byte, byte, byte, byte[]> IlasFrameEncoder { get; set; }

        public ushort LastIlasAddress { get; private set; }
        public ushort LastIlasConfiguration { get; private set; }
        public ushort LastIlasTemperatureRaw { get; private set; }
        public byte LastOspStatus { get; private set; }

        public Lan8661Ws2812Client()
        {
            IlasFrameEncoder = delegate(byte red, byte green, byte blue)
            {
                return IlasCodec.BuildRgbFrames(1, IlasLedCount, red, green, blue);
            };
        }

        public bool IsConnected
        {
            get
            {
                lock (_sync) return _udpClient != null;
            }
        }

        public bool ControlPortsReady
        {
            get
            {
                lock (_sync) return _spi != null && _spi.IsReady;
            }
        }

        public bool IsIlasProtocolAvailable
        {
            get { return IlasFrameEncoder != null; }
        }

        public bool Click1Ready { get { lock (_sync) return _spi != null && _spi.Click1Ready; } }
        public bool Click2Ready { get { lock (_sync) return _spi != null && _spi.Click2Ready; } }
        public bool OspTransportReady
        {
            get { lock (_sync) return _spi != null && _spi.OspTransportReady; }
        }
        public bool IlasTransportReady
        {
            get { lock (_sync) return _spi != null && _spi.IlasTransportReady; }
        }
        public int OspNodeCount
        {
            get { lock (_sync) return _spi == null ? 0 : _spi.OspNodeCount; }
        }
        public int IlasNodeCount
        {
            get { lock (_sync) return _ilasInitialized ? Math.Max(1, (int)LastIlasAddress) : 0; }
        }

        public string ControlPortSummary
        {
            get
            {
                lock (_sync)
                {
                    if (_spi == null) return "SPI 미연결";
                    string summary = _spi.CapabilitySummary;
                    return summary;
                }
            }
        }

        public string SourceIp { get; private set; } = "192.168.0.10";
        public string TargetIp { get; private set; } = "192.168.0.50";
        public int RtpPort { get; private set; } = DefaultRtpPort;
        public string LastError { get; private set; } = string.Empty;
        public string LastWarning { get; private set; } = string.Empty;

        public bool Connect(string sourceIp, string targetIp, int port, out string message)
        {
            message = string.Empty;
            LastError = string.Empty;
            LastWarning = string.Empty;

            UdpClient newClient = null;
            Lan8661SomeIpSpi newSpi = null;
            try
            {
                IPAddress sourceAddress = IPAddress.Parse(sourceIp);
                IPAddress targetAddress = IPAddress.Parse(targetIp);
                if (sourceAddress.AddressFamily != AddressFamily.InterNetwork ||
                    targetAddress.AddressFamily != AddressFamily.InterNetwork)
                    throw new ArgumentException("IPv4 주소만 사용할 수 있습니다.");
                if (port < 1 || port > 65535)
                    throw new ArgumentOutOfRangeException("port");

                Environment.SetEnvironmentVariable("LAN866X_LOCAL_IPV4",
                    sourceAddress.ToString(), EnvironmentVariableTarget.Process);
                newClient = new UdpClient(new IPEndPoint(sourceAddress, 0));
                // A normal Ethernet adapter may use the same 192.168.0/24
                // subnet as the USB T1S adapter.  Do not gate the connection
                // with an unqualified Ping: Windows can send that Ping through
                // the wrong adapter.  Bind RTP to the explicitly selected
                // source interface and use SOME/IP discovery as the device
                // connectivity check instead.
                int interfaceIndex = FindInterfaceIndex(sourceAddress);
                if (interfaceIndex > 0)
                {
                    try
                    {
                        // IP_UNICAST_IF (31) is present in Winsock but is not
                        // named by the .NET Framework 4.7.2 enum.
                        newClient.Client.SetSocketOption(SocketOptionLevel.IP,
                            (SocketOptionName)31,
                            IPAddress.HostToNetworkOrder(interfaceIndex));
                    }
                    catch (SocketException)
                    {
                        // The source-address bind above remains effective when
                        // IP_UNICAST_IF is unavailable on a Windows build.
                    }
                }
                newSpi = new Lan8661SomeIpSpi();
                string spiError;
                if (!newSpi.TryConnect(targetAddress.ToString(), TimeSpan.FromSeconds(6),
                    true, out spiError))
                    throw new InvalidOperationException("LAN8661 원격 SPI 준비 실패: " + spiError);

                if (newSpi.OspTransportReady)
                {
                    byte ospStatus = 0;
                    bool ospInitialized = false;
                    for (int attempt = 0; attempt < 3 && !ospInitialized; attempt++)
                    {
                        ospInitialized = newSpi.InitializeOsp(out ospStatus, out spiError);
                        if (!ospInitialized) System.Threading.Thread.Sleep(50);
                    }
                    if (!ospInitialized)
                        throw new InvalidOperationException(
                            "OSP 초기화/Read 확인 실패: " + spiError);
                    LastOspStatus = ospStatus;
                }

                lock (_sync)
                {
                    if (_udpClient != null) _udpClient.Dispose();
                    if (_spi != null) _spi.Disconnect();
                    _udpClient = newClient;
                    _targetEndpoint = new IPEndPoint(targetAddress, port);
                    _spi = newSpi;
                    newClient = null;
                    newSpi = null;
                    _sequence = 1;
                    _timestamp = 0;
                    _ilasInitialized = false;
                    LastIlasAddress = 0;
                    LastIlasConfiguration = 0;
                    LastIlasTemperatureRaw = 0;
                    SourceIp = sourceAddress.ToString();
                    TargetIp = targetAddress.ToString();
                    RtpPort = port;
                }

                message = string.Format(
                    "LAN8661 연결됨: {0} -> {1}:{2} ({3})",
                    SourceIp, TargetIp, RtpPort, ControlPortSummary);
                return true;
            }
            catch (Exception ex)
            {
                if (newClient != null) newClient.Dispose();
                if (newSpi != null) newSpi.Disconnect();
                LastError = ex.Message;
                message = ex.Message;
                Disconnect();
                return false;
            }
        }

        public void Disconnect()
        {
            lock (_sync)
            {
                if (_udpClient != null)
                {
                    _udpClient.Dispose();
                    _udpClient = null;
                }
                if (_spi != null)
                {
                    _spi.Disconnect();
                    _spi = null;
                }
                _targetEndpoint = null;
                _ilasInitialized = false;
            }
        }

        private static int FindInterfaceIndex(IPAddress sourceAddress)
        {
            foreach (NetworkInterface networkInterface in NetworkInterface.GetAllNetworkInterfaces())
            {
                IPInterfaceProperties properties = networkInterface.GetIPProperties();
                foreach (UnicastIPAddressInformation address in properties.UnicastAddresses)
                {
                    if (!address.Address.Equals(sourceAddress)) continue;
                    IPv4InterfaceProperties ipv4 = properties.GetIPv4Properties();
                    return ipv4 == null ? 0 : ipv4.Index;
                }
            }
            return 0;
        }

        /// <summary>
        /// Compatibility overload: sends the 20x10 frame to both WS2812 clicks.
        /// </summary>
        public bool SendFrame(byte[] rgb24, int frameIntervalMilliseconds, out string error)
        {
            string warning;
            return SendFrame(rgb24, frameIntervalMilliseconds, 3, false, false,
                0, 0, 0, out error, out warning);
        }

        /// <param name="ws2812TargetMask">bit0=Click1, bit1=Click2</param>
        public bool SendFrame(byte[] rgb24, int frameIntervalMilliseconds,
            int ws2812TargetMask, bool sendOsp, bool sendIlas,
            byte auxiliaryRed, byte auxiliaryGreen, byte auxiliaryBlue,
            out string error, out string warning)
        {
            return SendFrame(rgb24, frameIntervalMilliseconds, ws2812TargetMask,
                sendOsp, sendIlas, auxiliaryRed, auxiliaryGreen, auxiliaryBlue,
                null, null, out error, out warning);
        }

        public bool SendFrame(byte[] rgb24, int frameIntervalMilliseconds,
            int ws2812TargetMask, bool sendOsp, bool sendIlas,
            byte auxiliaryRed, byte auxiliaryGreen, byte auxiliaryBlue,
            byte[] ospRgb24, byte[] ilasRgb24,
            out string error, out string warning)
        {
            return SendFrame(rgb24, frameIntervalMilliseconds, ws2812TargetMask,
                sendOsp, sendIlas, auxiliaryRed, auxiliaryGreen, auxiliaryBlue,
                ospRgb24, ilasRgb24, true, out error, out warning);
        }

        public bool SendFrame(byte[] rgb24, int frameIntervalMilliseconds,
            int ws2812TargetMask, bool sendOsp, bool sendIlas,
            byte auxiliaryRed, byte auxiliaryGreen, byte auxiliaryBlue,
            byte[] ospRgb24, byte[] ilasRgb24, bool verifyOspReadback,
            out string error, out string warning)
        {
            error = string.Empty;
            warning = string.Empty;
            LastError = string.Empty;
            LastWarning = string.Empty;

            if (rgb24 == null || rgb24.Length != FrameByteCount)
            {
                error = "RGB 프레임 크기가 600바이트(20x10x3)가 아닙니다.";
                LastError = error;
                return false;
            }
            if ((ws2812TargetMask & ~3) != 0)
            {
                error = "WS2812 대상 마스크가 올바르지 않습니다.";
                LastError = error;
                return false;
            }
            if (frameIntervalMilliseconds < 1)
                frameIntervalMilliseconds = 1;

            try
            {
                lock (_sync)
                {
                    if (_udpClient == null || _targetEndpoint == null || _spi == null)
                        throw new InvalidOperationException("LAN8661이 연결되지 않았습니다.");
                    if (!_spi.IsReady)
                        throw new InvalidOperationException("LAN8661 원격 SPI 포트가 준비되지 않았습니다.");
                    if (sendIlas && IlasFrameEncoder == null && ws2812TargetMask == 0 && !sendOsp)
                        throw new InvalidOperationException("ISE 색상 제어 인코더가 연결되지 않아 실제 ISE 데이터는 송신할 수 없습니다.");

                    string spiError;
                    if ((ws2812TargetMask & 1) != 0)
                    {
                        if (!_spi.SendWs2812Panel(1, rgb24, 0, VideoWidth,
                            VideoHeight, out spiError))
                            throw new InvalidOperationException(spiError);
                    }
                    if ((ws2812TargetMask & 2) != 0 &&
                        !_spi.SendWs2812Panel(2, rgb24, 10, VideoWidth, VideoHeight, out spiError))
                        throw new InvalidOperationException(spiError);

                    if (sendOsp)
                    {
                        byte ospStatus;
                        bool ospSent = ospRgb24 == null ?
                            _spi.SendOspColor(auxiliaryRed, auxiliaryGreen,
                                auxiliaryBlue, out ospStatus, out spiError) :
                            _spi.SendOspColors(ospRgb24, true, verifyOspReadback,
                                out ospStatus, out spiError);
                        if (!ospSent)
                            throw new InvalidOperationException(spiError);
                        if (verifyOspReadback) LastOspStatus = ospStatus;
                    }

                    if (sendIlas)
                    {
                        if (IlasFrameEncoder == null)
                        {
                            warning = "ISE/ILaS SPI 포트는 준비됐지만 Inova 명령 인코더가 없어 ISE 송신은 생략했습니다.";
                        }
                        else
                        {
                            if (!_ilasInitialized)
                            {
                                ushort address;
                                ushort configuration;
                                if (!_spi.InitializeIlas(out address, out configuration, out spiError))
                                    throw new InvalidOperationException("ISE 초기화/응답 확인 실패: " + spiError);
                                _ilasInitialized = true;
                                LastIlasAddress = address;
                                LastIlasConfiguration = configuration;
                            }
                            byte[] ilasPacket = ilasRgb24 == null ?
                                IlasFrameEncoder(auxiliaryRed, auxiliaryGreen, auxiliaryBlue) :
                                IlasCodec.BuildRgbFrames(1, ilasRgb24);
                            if (!_spi.SendIlasRaw(ilasPacket, out spiError))
                                throw new InvalidOperationException(spiError);
                        }
                    }

                }
                LastWarning = warning;
                return true;
            }
            catch (Exception ex)
            {
                error = ex.Message;
                LastError = error;
                return false;
            }
        }

        public bool PrepareOutputChains(bool prepareOsp, bool prepareIlas,
            out string error)
        {
            error = string.Empty;
            lock (_sync)
            {
                if (_spi == null)
                {
                    error = "LAN8661이 연결되지 않았습니다.";
                    return false;
                }

                string spiError;
                if (prepareOsp && _spi.OspNodeCount <= 0)
                {
                    byte status;
                    if (!_spi.InitializeOsp(out status, out spiError))
                    {
                        error = "OSP 초기화/수량 확인 실패: " + spiError;
                        return false;
                    }
                    LastOspStatus = status;
                }

                if (prepareIlas && !_ilasInitialized)
                {
                    ushort address;
                    ushort configuration;
                    if (!_spi.InitializeIlas(out address, out configuration, out spiError))
                    {
                        error = "ISE 초기화/수량 확인 실패: " + spiError;
                        return false;
                    }
                    if (address == 0)
                    {
                        error = "ISE 초기화 응답의 마지막 LED 주소가 0입니다.";
                        return false;
                    }
                    _ilasInitialized = true;
                    LastIlasAddress = address;
                    LastIlasConfiguration = configuration;
                }
                return true;
            }
        }

        public bool SendIlasRaw(byte[] packet, out string error)
        {
            lock (_sync)
            {
                if (_spi == null)
                {
                    error = "LAN8661이 연결되지 않았습니다.";
                    return false;
                }
                return _spi.SendIlasRaw(packet, out error);
            }
        }

        public bool InitializeIlas(out string error)
        {
            lock (_sync)
            {
                if (_spi == null)
                {
                    error = "LAN8661이 연결되지 않았습니다.";
                    return false;
                }
                ushort address;
                ushort configuration;
                if (!_spi.InitializeIlas(out address, out configuration, out error))
                {
                    _ilasInitialized = false;
                    return false;
                }
                _ilasInitialized = true;
                LastIlasAddress = address;
                LastIlasConfiguration = configuration;
                return true;
            }
        }

        public bool InitializeIlasTransmitOnly(out string error)
        {
            lock (_sync)
            {
                if (_spi == null)
                {
                    error = "LAN8661이 연결되지 않았습니다.";
                    return false;
                }
                if (!_spi.InitializeIlasTransmitOnly(out error))
                {
                    _ilasInitialized = false;
                    return false;
                }
                _ilasInitialized = true;
                LastIlasAddress = 1;
                LastIlasConfiguration = 0x0005;
                return true;
            }
        }

        public bool ReadIlasTemperature(ushort address, out ushort rawTemperature,
            out string error)
        {
            rawTemperature = 0;
            lock (_sync)
            {
                if (_spi == null)
                {
                    error = "LAN8661이 연결되지 않았습니다.";
                    return false;
                }
                if (!_ilasInitialized)
                {
                    error = "ISE가 아직 초기화되지 않았습니다.";
                    return false;
                }
                ushort responseAddress;
                if (!_spi.ReadIlasTemperature(address, out responseAddress,
                    out rawTemperature, out error))
                    return false;
                LastIlasAddress = responseAddress;
                LastIlasTemperatureRaw = rawTemperature;
                return true;
            }
        }

        private byte[] BuildOsirePacket(byte red, byte green, byte blue)
        {
            const int pixelBytes = 3;
            int packetSize = RtpHeaderSize + ExtendedSequenceSize +
                ScanLineHeaderSize + pixelBytes;
            byte[] packet = new byte[packetSize];

            packet[0] = 0x80; // RTP version 2
            packet[1] = (byte)(0x80 | DynamicPayloadType); // Marker + PT 96
            SetUInt16BigEndian(packet, 2, (ushort)(_sequence & 0xFFFF));
            SetUInt32BigEndian(packet, 4, _timestamp);
            SetUInt32BigEndian(packet, 8, _ssrc);
            SetUInt16BigEndian(packet, 12, (ushort)((_sequence >> 16) & 0xFFFF));

            int scanLine = RtpHeaderSize + ExtendedSequenceSize;
            SetUInt16BigEndian(packet, scanLine, pixelBytes);
            SetUInt16BigEndian(packet, scanLine + 2, 0); // line 0
            SetUInt16BigEndian(packet, scanLine + 4, 0); // offset 0, last line

            int pixel = scanLine + ScanLineHeaderSize;
            packet[pixel] = red;
            packet[pixel + 1] = green;
            packet[pixel + 2] = blue;
            return packet;
        }

        private static void SetUInt16BigEndian(byte[] buffer, int offset, ushort value)
        {
            buffer[offset] = (byte)((value >> 8) & 0xFF);
            buffer[offset + 1] = (byte)(value & 0xFF);
        }

        private static void SetUInt32BigEndian(byte[] buffer, int offset, uint value)
        {
            buffer[offset] = (byte)((value >> 24) & 0xFF);
            buffer[offset + 1] = (byte)((value >> 16) & 0xFF);
            buffer[offset + 2] = (byte)((value >> 8) & 0xFF);
            buffer[offset + 3] = (byte)(value & 0xFF);
        }

        public void Dispose()
        {
            Disconnect();
        }
    }
}
