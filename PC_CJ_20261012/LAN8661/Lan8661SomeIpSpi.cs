using System;
using System.Collections.Generic;
using System.Net;
using System.Runtime.InteropServices;
using System.Threading;

namespace LINMaster.LAN8661
{
    /// <summary>
    /// Thin .NET Framework compatible wrapper around Microchip's V1.10.0
    /// LAN866x native SOME/IP client.  Only the SPI functions needed by the
    /// LAN8661 application are exposed here.
    /// </summary>
    internal sealed class Lan8661SomeIpSpi
    {
        private const string NativeLibrary = "lan866x_csharp_native.dll";
        private const byte UnusedPin = 0xFF;
        private const int MaximumSpiPayload = 1400;
        private const int MaximumSpiTimeoutPayload = 64;
        // Four SPI bits per WS2812 bit at 3.125 MHz produce a 1.28 us LED-bit
        // period.  This keeps T0H (0.32 us) well separated from T1H (0.96 us).
        // The former 3-bit/2.083 MHz encoding made T0H about 0.48 us, close to
        // the receiver threshold; some repeated frames were decoded as all 1s
        // and appeared as a full-bright white panel.
        private const uint Ws2812SpiClock = 3125000;
        private const uint OspSpiClock = 5000000;
        private const uint IlasSpiClock = 1923000;

        private static readonly object FactorySync = new object();
        private static readonly DeviceCallback DeviceAvailableCallback = OnDeviceChanged;
        private static readonly DeviceCallback DeviceUnavailableCallback = OnDeviceChanged;
        private static bool _factoryInitialized;

        private readonly object _sync = new object();
        private IntPtr _client;
        private ushort _click1Handle;
        private ushort _click2Handle;
        private ushort _click3OspHandle;
        private ushort _click4IlasHandle;
        private uint _click1WriteId;
        private uint _click2WriteId;
        private uint _ospWriteId;
        private uint _ilasWriteId;
        private bool _ospInitialized;
        private int _ospNodeCount;
        private byte[] _lastOspColors;
        private byte _lastOspLedStatus;
        private string _lastOspPwmHex = string.Empty;
        private string _openFailures = string.Empty;

        public bool IsReady
        {
            get
            {
                lock (_sync)
                {
                    return _client != IntPtr.Zero &&
                           NativeMethods.lan866x_is_node_available(_client) != 0 &&
                           (_click1Handle != 0 || _click2Handle != 0 ||
                            _click3OspHandle != 0 || _click4IlasHandle != 0);
                }
            }
        }

        public bool Click1Ready { get { lock (_sync) return _click1Handle != 0; } }
        public bool Click2Ready { get { lock (_sync) return _click2Handle != 0; } }
        public bool OspTransportReady { get { lock (_sync) return _click3OspHandle != 0; } }
        public bool IlasTransportReady { get { lock (_sync) return _click4IlasHandle != 0; } }
        public int OspNodeCount { get { lock (_sync) return _ospNodeCount; } }
        public byte LastOspLedStatus { get { lock (_sync) return _lastOspLedStatus; } }
        public string LastOspPwmHex { get { lock (_sync) return _lastOspPwmHex; } }

        public string CapabilitySummary
        {
            get
            {
                lock (_sync)
                {
                    return string.Format("First={0}, OSP={1}, ISE={2}",
                        _click1Handle != 0 ? "Ready" : "없음",
                        _click3OspHandle != 0 ? (_ospInitialized ?
                            "Active/Read OK (" + _ospNodeCount + " nodes)" : "Port Ready") : "없음",
                        _click4IlasHandle != 0 ? "Ready" : "없음") +
                        (string.IsNullOrEmpty(_openFailures) ? string.Empty :
                            " [" + _openFailures + "]");
                }
            }
        }

        public bool TryConnect(string targetIp, TimeSpan timeout, out string error)
        {
            return TryConnect(targetIp, timeout, true, out error);
        }

        public bool TryConnect(string targetIp, TimeSpan timeout,
            bool openFirstSpi, out string error)
        {
            error = string.Empty;
            try
            {
                EnsureFactoryInitialized();
                IntPtr client = FindTarget(targetIp, timeout);
                if (client == IntPtr.Zero)
                {
                    error = "Microchip SOME/IP에서 " + targetIp + " 장치를 찾지 못했습니다.";
                    return false;
                }

                lock (_sync)
                {
                    ResetState();
                    _client = client;

                    // The hybrid application reserves SERCOM0 for its native
                    // WS2812 display engine. Do not release or reconfigure PA03
                    // in that mode: doing so can create a false first pixel.
                    ReleasePins(openFirstSpi ?
                        new byte[] { 3, 8, 9, 12, 13 } :
                        new byte[] { 8, 9, 12, 13 });

                    if (openFirstSpi)
                    {
                        // Legacy PC-direct fallback only.
                        ConfigureDigitalPin(3, 2, 3); // pull-down, 4 mA
                        _click1Handle = TryOpenSpi(UnusedPin, UnusedPin,
                            UnusedPin, 3, 0, Ws2812SpiClock, "Click1/SERCOM0");
                    }
                    else
                    {
                        _click1Handle = 0;
                    }
                    // The requested product configuration has one existing
                    // LED channel only; SERCOM1/Click2 is intentionally unused.
                    _click2Handle = 0;

                    // Click 3 OSP: PA08 is the bidirectional data wire and PA09
                    // is the clock.  5 MHz SPI produces the 2.5 Mbit/s OSP
                    // Manchester stream and is validated by status/PWM reads.
                    ConfigureDigitalPin(8, 1, 3); // MCU-mode idle DATA high
                    ConfigureDigitalPin(9, 2, 3); // MCU-mode idle CLK low
                    _click3OspHandle = TryOpenSpi(8, 9, UnusedPin, 8,
                        0, OspSpiClock, "OSP/SERCOM2");

                    // Click 4: ILaS uses SERCOM3 P0 for bidirectional DATA and P1
                    // for SCK. Mode 1 is required to sample the upstream reply;
                    // modes 0/3 return stable but invalid RLC data on this board.
                    // 1.923 MHz is the supported rate nearest nominal 2 Mbit/s.
                    _click4IlasHandle = TryOpenSpi(12, 13, UnusedPin, 12,
                        1, IlasSpiClock, "ISE/SERCOM3");

                    if (_click1Handle == 0 && _click2Handle == 0 &&
                        _click3OspHandle == 0 && _click4IlasHandle == 0)
                        throw new InvalidOperationException(
                            "활성화된 원격 SPI 포트를 찾지 못했습니다. " + _openFailures);
                }
                return true;
            }
            catch (Exception ex)
            {
                lock (_sync) ResetState();
                error = ex.Message;
                return false;
            }
        }

        public void Disconnect()
        {
            lock (_sync) ResetState();
        }

        public bool InitializeOsp(out byte status, out string error)
        {
            status = 0;
            error = string.Empty;
            try
            {
                lock (_sync)
                {
                    EnsureOspReady();
                    InitializeOspLocked(out status);
                    return true;
                }
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        public bool SendOspColor(byte red, byte green, byte blue,
            out byte status, out string error)
        {
            return SendOspColor(red, green, blue, true, out status, out error);
        }

        public bool SendOspColor(byte red, byte green, byte blue, bool dayCurrent,
            out byte status, out string error)
        {
            status = 0;
            error = string.Empty;
            try
            {
                lock (_sync)
                {
                    EnsureOspReady();
                    if (!_ospInitialized)
                        InitializeOspLocked(out status);

                    ushort redPwm = ScaleOspPwm(red);
                    ushort greenPwm = ScaleOspPwm(green);
                    ushort bluePwm = ScaleOspPwm(blue);
                    byte current = dayCurrent ? (byte)0x80 : (byte)0x00;
                    byte[] payload =
                    {
                        (byte)(current | (redPwm >> 8)), (byte)redPwm,
                        (byte)(current | (greenPwm >> 8)), (byte)greenPwm,
                        (byte)(current | (bluePwm >> 8)), (byte)bluePwm
                    };
                    // One PC OSP channel represents the complete daisy chain.
                    WriteOsp(OspCommand(0, 0x4F, payload), "OSP SETPWM broadcast");

                    // OSIRE E3731i needs up to 1 ms to run the LED open/short
                    // check after PWM crosses the diagnostic threshold.
                    Thread.Sleep(2);

                    // Verify both ends of the chain. A valid last-node response
                    // also proves that the broadcast traversed the full chain.
                    int[] verifyAddresses = _ospNodeCount > 1 ?
                        new int[] { 1, _ospNodeCount } : new int[] { 1 };
                    _lastOspLedStatus = 0;
                    foreach (int address in verifyAddresses)
                    {
                        byte[] pwm = ReadOsp(OspCommand(address, 0x4E), 10, 0x4E,
                            "OSP READPWM node " + address, address);
                        for (int i = 0; i < 6; i++)
                        {
                            if (pwm[i + 3] != payload[i])
                                throw new InvalidOperationException("OSP PWM Read-back 불일치 (node " +
                                    address + ").");
                        }
                        _lastOspPwmHex = BitConverter.ToString(pwm, 3, 6);
                        byte[] ledState = ReadOsp(OspCommand(address, 0x46), 5, 0x46,
                            "OSP READLEDST node " + address, address);
                        _lastOspLedStatus |= ledState[3];
                        byte[] state = ReadOsp(OspCommand(address, 0x40), 5, 0x40,
                            "OSP READSTATUS node " + address, address);
                        status = state[3];
                        if ((status & 0xC0) != 0x80)
                            throw new InvalidOperationException("OSP가 ACTIVE 상태가 아닙니다 (node " +
                                address + ", STATUS=0x" + status.ToString("X2") + ").");
                    }
                    if (_lastOspLedStatus != 0)
                        throw new InvalidOperationException("OSP LED 출력 진단 오류 (LEDSTATUS=0x" +
                            _lastOspLedStatus.ToString("X2") + ").");
                    return true;
                }
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        public bool SendOspColors(byte[] rgb24, bool dayCurrent,
            out byte status, out string error)
        {
            return SendOspColors(rgb24, dayCurrent, true, out status, out error);
        }

        public bool SendOspColors(byte[] rgb24, bool dayCurrent, bool verifyReadback,
            out byte status, out string error)
        {
            status = 0;
            error = string.Empty;
            try
            {
                lock (_sync)
                {
                    EnsureOspReady();
                    if (!_ospInitialized)
                        InitializeOspLocked(out status);
                    if (rgb24 == null || rgb24.Length != _ospNodeCount * 3)
                        throw new ArgumentException("OSP RGB 배열 크기가 검출 노드 수(" +
                            _ospNodeCount + ")와 일치하지 않습니다.", "rgb24");

                    byte current = dayCurrent ? (byte)0x80 : (byte)0x00;
                    bool uniform = true;
                    for (int offset = 3; offset < rgb24.Length && uniform; offset += 3)
                        uniform = rgb24[offset] == rgb24[0] &&
                            rgb24[offset + 1] == rgb24[1] &&
                            rgb24[offset + 2] == rgb24[2];

                    if (uniform)
                    {
                        WriteOsp(OspCommand(0, 0x4F,
                            BuildOspPwmPayload(rgb24[0], rgb24[1], rgb24[2], current)),
                            "OSP SETPWM broadcast");
                    }
                    else
                    {
                        var commands = new List<byte[]>();
                        for (int address = 1; address <= _ospNodeCount; address++)
                        {
                            int offset = (address - 1) * 3;
                            bool changed = _lastOspColors == null ||
                                _lastOspColors[offset] != rgb24[offset] ||
                                _lastOspColors[offset + 1] != rgb24[offset + 1] ||
                                _lastOspColors[offset + 2] != rgb24[offset + 2];
                            if (!changed) continue;
                            byte[] payload = BuildOspPwmPayload(rgb24[offset],
                                rgb24[offset + 1], rgb24[offset + 2], current);
                            commands.Add(Manchester(OspCommand(address, 0x4F, payload)));
                        }
                        WriteOspBatches(commands);
                    }

                    _lastOspColors = (byte[])rgb24.Clone();

                    if (!verifyReadback) return true;

                    Thread.Sleep(2);
                    int[] verifyAddresses = _ospNodeCount > 1 ?
                        new int[] { 1, _ospNodeCount } : new int[] { 1 };
                    _lastOspLedStatus = 0;
                    foreach (int address in verifyAddresses)
                    {
                        int offset = (address - 1) * 3;
                        byte[] expected = BuildOspPwmPayload(rgb24[offset],
                            rgb24[offset + 1], rgb24[offset + 2], current);
                        byte[] pwm = ReadOsp(OspCommand(address, 0x4E), 10, 0x4E,
                            "OSP READPWM node " + address, address);
                        for (int i = 0; i < 6; i++)
                            if (pwm[i + 3] != expected[i])
                                throw new InvalidOperationException(
                                    "OSP PWM Read-back 불일치 (node " + address + ").");
                        _lastOspPwmHex = BitConverter.ToString(pwm, 3, 6);

                        byte[] ledState = ReadOsp(OspCommand(address, 0x46), 5, 0x46,
                            "OSP READLEDST node " + address, address);
                        _lastOspLedStatus |= ledState[3];
                        byte[] state = ReadOsp(OspCommand(address, 0x40), 5, 0x40,
                            "OSP READSTATUS node " + address, address);
                        status = state[3];
                        if ((status & 0xC0) != 0x80)
                            throw new InvalidOperationException("OSP가 ACTIVE 상태가 아닙니다 (node " +
                                address + ", STATUS=0x" + status.ToString("X2") + ").");
                    }
                    if (_lastOspLedStatus != 0)
                        throw new InvalidOperationException("OSP LED 출력 진단 오류 (LEDSTATUS=0x" +
                            _lastOspLedStatus.ToString("X2") + ").");
                    return true;
                }
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        public bool SendWs2812Panel(int panel, byte[] rgb24, int xOffset,
            int canvasWidth, int canvasHeight, out string error)
        {
            error = string.Empty;
            if (rgb24 == null || rgb24.Length != canvasWidth * canvasHeight * 3)
            {
                error = "WS2812 RGB 프레임 크기가 올바르지 않습니다.";
                return false;
            }
            if (panel != 1 && panel != 2)
            {
                error = "WS2812 Click 번호는 1 또는 2여야 합니다.";
                return false;
            }

            try
            {
                byte[] encoded = EncodeWs2812Panel(rgb24, xOffset, canvasWidth, canvasHeight);
                lock (_sync)
                {
                    EnsureReady();
                    ushort handle = panel == 1 ? _click1Handle : _click2Handle;
                    if (handle == 0)
                        throw new InvalidOperationException(
                            "WS2812 Click " + panel + " 포트가 현재 보드 설정에 없습니다.");
                    uint writeId = panel == 1 ? _click1WriteId : _click2WriteId;
                    WriteSpi(handle, writeId, encoded, "WS2812 Click " + panel);
                    if (panel == 1) _click1WriteId++; else _click2WriteId++;
                }
                return true;
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        /// <summary>Sends an already encoded 96-bit ILaS downlink packet.</summary>
        public bool SendIlasRaw(byte[] packet, out string error)
        {
            error = string.Empty;
            try
            {
                if (packet == null || packet.Length == 0)
                    throw new ArgumentException("ILaS 패킷이 비어 있습니다.");
                lock (_sync)
                {
                    EnsureReady();
                    if (_click4IlasHandle == 0)
                        throw new InvalidOperationException(
                            "ISE/ILaS 전송 포트가 현재 보드 설정에 없습니다.");
                    WriteSpi(_click4IlasHandle, _ilasWriteId, packet, "ILaS");
                    _ilasWriteId++;
                }
                return true;
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        public bool InitializeIlas(out ushort detectedAddress,
            out ushort configuration, out string error)
        {
            detectedAddress = 0;
            configuration = 0;
            error = string.Empty;
            try
            {
                lock (_sync)
                {
                    EnsureIlasReady();
                    WriteSpi(_click4IlasHandle, _ilasWriteId,
                        IlasCodec.BuildResetFrame(), "ILaS RESET");
                    _ilasWriteId++;
                    Thread.Sleep(2);

                    // INIT may produce one upstream reply per node. Sending it
                    // TX-only avoids a receive overrun when the chain is longer
                    // than the requested 5-byte response. Verify the bus with a
                    // chain temperature read after initialization instead.
                    WriteSpi(_click4IlasHandle, _ilasWriteId,
                        IlasCodec.BuildInitFrame(1, 0x000005), "ILaS INIT TX-only");
                    _ilasWriteId++;
                    Thread.Sleep(100);
                    byte[] response = ExchangeSpiWithTimeout(_click4IlasHandle,
                        _ilasWriteId, IlasCodec.BuildReadTemperatureFrame(), 5,
                        10000, "ILaS INIT verify READ_TEMP");
                    _ilasWriteId++;
                    ushort responseData;
                    if (!IlasCodec.TryDecodeUpstream(response, out detectedAddress,
                        out responseData, out error))
                        return false;
                    configuration = 0x0005;
                    return true;
                }
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        /// <summary>
        /// Sends RESET and INIT without requesting the upstream reply.  This is
        /// useful for separating a working downstream path from a failed ILaS
        /// upstream/half-duplex path.
        /// </summary>
        public bool InitializeIlasTransmitOnly(out string error)
        {
            error = string.Empty;
            try
            {
                lock (_sync)
                {
                    EnsureIlasReady();
                    WriteSpi(_click4IlasHandle, _ilasWriteId,
                        IlasCodec.BuildResetFrame(), "ILaS RESET TX-only");
                    _ilasWriteId++;
                    Thread.Sleep(2);
                    WriteSpi(_click4IlasHandle, _ilasWriteId,
                        IlasCodec.BuildInitFrame(1, 0x000005), "ILaS INIT TX-only");
                    _ilasWriteId++;
                    Thread.Sleep(100);
                    return true;
                }
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        public bool ReadIlasTemperature(ushort address, out ushort responseAddress,
            out ushort rawTemperature, out string error)
        {
            responseAddress = 0;
            rawTemperature = 0;
            error = string.Empty;
            try
            {
                lock (_sync)
                {
                    EnsureIlasReady();
                    byte[] response = ExchangeSpiWithTimeout(_click4IlasHandle,
                        _ilasWriteId, IlasCodec.BuildReadTemperatureFrame(),
                        5, 10000, "ILaS READ_TEMP");
                    _ilasWriteId++;
                    return IlasCodec.TryDecodeUpstream(response, out responseAddress,
                        out rawTemperature, out error);
                }
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        private static byte[] EncodeWs2812Panel(byte[] rgb24, int xOffset,
            int canvasWidth, int canvasHeight)
        {
            const int panelWidth = Lan8661Ws2812Client.ExistingLedWidth;
            // WS2812 variants used on the Apps Board need a long low interval to
            // latch the complete 10x10 frame. 80 bytes gives about 205 us at
            // 3.125 MHz. The 1200-byte encoded frame plus both reset intervals
            // is 1360 bytes, below both the PC wrapper and endpoint FIFO limits.
            const int resetBytes = 80;
            int ledCount = panelWidth * canvasHeight;
            byte[] output = new byte[(ledCount * 3 * 4) + (resetBytes * 2)];
            int destination = resetBytes;

            for (int y = 0; y < canvasHeight; y++)
            {
                for (int x = 0; x < panelWidth; x++)
                {
                    int source = ((y * canvasWidth) + xOffset + x) * 3;
                    // WS2812 serial order is G, R, B.
                    destination = EncodeWsByte(rgb24[source + 1], output, destination);
                    destination = EncodeWsByte(rgb24[source], output, destination);
                    destination = EncodeWsByte(rgb24[source + 2], output, destination);
                }
            }
            return output;
        }

        private static int EncodeWsByte(byte value, byte[] output, int offset)
        {
            // Four SPI bits encode one WS2812 bit at 3.125 MHz:
            // 0 -> 1000, 1 -> 1110. Eight WS bits fit in four SPI bytes.
            uint encoded = 0;
            for (int bit = 7; bit >= 0; bit--)
            {
                encoded <<= 4;
                encoded |= (value & (1 << bit)) != 0 ? 0xEu : 0x8u;
            }
            output[offset++] = (byte)((encoded >> 24) & 0xFF);
            output[offset++] = (byte)((encoded >> 16) & 0xFF);
            output[offset++] = (byte)((encoded >> 8) & 0xFF);
            output[offset++] = (byte)(encoded & 0xFF);
            return offset;
        }

        private void InitializeOspLocked(out byte status)
        {
            EnsureOspReady();
            _ospInitialized = false;
            _ospNodeCount = 0;
            _lastOspColors = null;

            WriteOsp(OspCommand(0, 0x00), "OSP RESET broadcast");
            Thread.Sleep(2);
            // Match the previously working ADI implementation exactly.
            WriteOsp(OspCommand(0, 0x4D, new byte[] { 0x24 }),
                "OSP SETSETUP broadcast");

            byte[] init = ExchangeSpi(_click3OspHandle, _ospWriteId,
                Manchester(OspCommand(1, 0x02)), 6, "OSP INITBIDIR");
            _ospWriteId++;
            // The MCU-mode INIT response can be sampled one bit late by the
            // remote SPI peripheral. Realign it and obtain the last node address.
            if (!TryDecodeOspInitResponse(init, out _ospNodeCount))
                throw new InvalidOperationException("OSP INIT 응답/체인 노드 수를 해석할 수 없습니다 (" +
                    BitConverter.ToString(init) + ").");
            Thread.Sleep(20);

            byte[] setup = ReadOsp(OspCommand(1, 0x4C), 5, 0x4C, "OSP READSETUP");
            if (setup[3] != 0x24)
                throw new InvalidOperationException("OSP SETUP Read-back 불일치.");

            WriteOsp(OspCommand(0, 0x05), "OSP GOACTIVE broadcast");
            Thread.Sleep(2);
            byte[] state = ReadOsp(OspCommand(1, 0x40), 5, 0x40, "OSP READSTATUS");
            status = state[3];
            if ((status & 0xC0) != 0x80 || (status & 0x0F) != 0)
                throw new InvalidOperationException("OSP 초기 상태 오류 (STATUS=0x" +
                    status.ToString("X2") + ").");

            // Always start the complete chain from optical OFF; day-current bits match ADI.
            WriteOsp(OspCommand(0, 0x4F,
                new byte[] { 0x80, 0, 0x80, 0, 0x80, 0 }), "OSP initial OFF broadcast");
            _ospInitialized = true;
        }

        private void WriteOsp(byte[] telegram, string name)
        {
            WriteSpi(_click3OspHandle, _ospWriteId, Manchester(telegram), name);
            _ospWriteId++;
        }

        private void WriteOspBatches(List<byte[]> commands)
        {
            const int maxCommandsPerTransfer = 17;
            const int interFrameBytes = 2;

            for (int first = 0; first < commands.Count; first += maxCommandsPerTransfer)
            {
                int count = Math.Min(maxCommandsPerTransfer, commands.Count - first);
                int length = 0;
                for (int i = 0; i < count; i++)
                    length += commands[first + i].Length + (i == 0 ? 0 : interFrameBytes);

                byte[] stream = new byte[length];
                int destination = 0;
                for (int i = 0; i < count; i++)
                {
                    if (i != 0)
                    {
                        stream[destination++] = 0xFF;
                        stream[destination++] = 0xFF;
                    }
                    byte[] command = commands[first + i];
                    Buffer.BlockCopy(command, 0, stream, destination, command.Length);
                    destination += command.Length;
                }

                // Keep the acknowledged path, but acknowledge one complete OSP
                // display update instead of one network round trip per node.
                WriteSpi(_click3OspHandle, _ospWriteId, stream,
                    "OSP SETPWM batch " + (first / maxCommandsPerTransfer + 1));
                _ospWriteId++;
            }
        }

        private byte[] ReadOsp(byte[] telegram, ushort length, byte command, string name,
            int expectedAddress = 1)
        {
            // A response traversing the LVDS chain can reach the MCU-mode SPI
            // sampler one bit late. Keep the exact response length (extra clocks
            // make the slave transaction time out) and restore the trailing CRC
            // bit from the CRC calculated over the recovered message.
            ushort wireLength = length;
            byte[] raw = ExchangeSpi(_click3OspHandle, _ospWriteId,
                Manchester(telegram), wireLength, name);
            _ospWriteId++;
            byte[] response;
            if (!TryNormalizeOspResponse(raw, length, command, expectedAddress, out response))
                throw new InvalidOperationException(name + " 응답 형식/CRC가 올바르지 않습니다 (RAW=" +
                    BitConverter.ToString(raw) + ").");
            return response;
        }

        private static ushort ScaleOspPwm(byte value)
        {
            return (ushort)(((uint)value * 32767U + 127U) / 255U);
        }

        private static byte[] BuildOspPwmPayload(byte red, byte green, byte blue,
            byte current)
        {
            ushort redPwm = ScaleOspPwm(red);
            ushort greenPwm = ScaleOspPwm(green);
            ushort bluePwm = ScaleOspPwm(blue);
            return new byte[]
            {
                (byte)(current | (redPwm >> 8)), (byte)redPwm,
                (byte)(current | (greenPwm >> 8)), (byte)greenPwm,
                (byte)(current | (bluePwm >> 8)), (byte)bluePwm
            };
        }

        private static byte[] OspCommand(int address, byte command, params byte[][] payloadParts)
        {
            int payloadLength = 0;
            foreach (byte[] part in payloadParts) payloadLength += part == null ? 0 : part.Length;
            if (address < 0 || address > 1023 || payloadLength > 8 || payloadLength == 7)
                throw new ArgumentOutOfRangeException("payloadParts");
            int psi = payloadLength == 8 ? 7 : payloadLength;
            byte[] message = new byte[payloadLength + 4];
            message[0] = (byte)(0xA0 | ((address >> 6) & 0x0F));
            message[1] = (byte)(((address & 0x3F) << 2) | ((psi & 6) >> 1));
            message[2] = (byte)(((psi & 1) << 7) | (command & 0x7F));
            int offset = 3;
            foreach (byte[] part in payloadParts)
            {
                if (part == null) continue;
                Buffer.BlockCopy(part, 0, message, offset, part.Length);
                offset += part.Length;
            }
            message[message.Length - 1] = OspCrc(message, message.Length - 1);
            return message;
        }

        private static byte OspCrc(byte[] data, int count)
        {
            int crc = 0;
            for (int i = 0; i < count; i++)
            {
                crc ^= data[i];
                for (int bit = 0; bit < 8; bit++)
                    crc = ((crc << 1) ^ ((crc & 0x80) != 0 ? 0x2F : 0)) & 0xFF;
            }
            return (byte)crc;
        }

        private static byte[] Manchester(byte[] telegram)
        {
            byte[] wire = new byte[telegram.Length * 2];
            for (int i = 0; i < telegram.Length; i++)
            {
                int encoded = 0;
                for (int bit = 7; bit >= 0; bit--)
                    encoded = (encoded << 2) |
                        ((telegram[i] & (1 << bit)) != 0 ? 1 : 2);
                wire[i * 2] = (byte)(encoded >> 8);
                wire[i * 2 + 1] = (byte)encoded;
            }
            return wire;
        }

        private static bool ValidOspResponse(byte[] response, int length, byte command,
            int expectedAddress)
        {
            if (response == null || response.Length != length || length < 4 ||
                (response[0] & 0xF0) != 0xA0 || (response[2] & 0x7F) != command)
                return false;
            int address = ((response[0] & 0x0F) << 6) | (response[1] >> 2);
            int psi = ((response[1] & 3) << 1) | (response[2] >> 7);
            int payloadLength = psi == 7 ? 8 : psi;
            return address == expectedAddress && payloadLength == length - 4 &&
                OspCrc(response, length - 1) == response[length - 1];
        }

        private static bool TryNormalizeOspResponse(byte[] raw, int length, byte command,
            int expectedAddress, out byte[] response)
        {
            response = null;
            if (raw == null || raw.Length < length) return false;
            for (int shift = 0; shift < 8; shift++)
            {
                byte[] candidate = new byte[length];
                for (int i = 0; i < length; i++)
                {
                    int next = i + 1 < raw.Length ? raw[i + 1] : 0;
                    candidate[i] = (byte)(((raw[i] << shift) & 0xFF) |
                        (next >> (8 - shift)));
                }
                if (ValidOspResponse(candidate, length, command, expectedAddress))
                {
                    response = candidate;
                    return true;
                }
                if (shift > 0)
                {
                    byte calculatedCrc = OspCrc(candidate, length - 1);
                    int knownMask = (0xFF << shift) & 0xFF;
                    if ((candidate[length - 1] & knownMask) ==
                        (calculatedCrc & knownMask))
                    {
                        candidate[length - 1] = calculatedCrc;
                        if (ValidOspResponse(candidate, length, command, expectedAddress))
                        {
                            response = candidate;
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        private static bool TryDecodeOspInitResponse(byte[] raw, out int nodeCount)
        {
            nodeCount = 0;
            if (raw == null || raw.Length != 6) return false;
            for (int shift = 0; shift < 8; shift++)
            {
                byte[] message = new byte[raw.Length];
                for (int i = 0; i < raw.Length; i++)
                {
                    int next = i + 1 < raw.Length ? raw[i + 1] : 0;
                    message[i] = (byte)(((raw[i] << shift) & 0xFF) |
                        (next >> (8 - shift)));
                }
                int address = ((message[0] & 0x0F) << 6) | (message[1] >> 2);
                int psi = ((message[1] & 3) << 1) | (message[2] >> 7);
                if ((message[0] & 0xF0) == 0xA0 && address > 0 &&
                    psi == 2 && (message[2] & 0x7F) == 0x02 &&
                    OspCrc(message, 5) == message[5])
                {
                    nodeCount = address;
                    return true;
                }
            }
            return false;
        }

        private static void EnsureFactoryInitialized()
        {
            lock (FactorySync)
            {
                if (_factoryInitialized) return;
                NativeMethods.lan866x_factory_set_callbacks(
                    DeviceAvailableCallback, DeviceUnavailableCallback, IntPtr.Zero);
                _factoryInitialized = true;
            }
        }

        private static IntPtr FindTarget(string targetIp, TimeSpan timeout)
        {
            DateTime deadline = DateTime.UtcNow + timeout;
            do
            {
                int count;
                NativeMethods.lan866x_get_devices(null, 0, out count);
                if (count > 0)
                {
                    IntPtr[] devices = new IntPtr[count];
                    int copied = NativeMethods.lan866x_get_devices(devices, devices.Length, out count);
                    for (int i = 0; i < copied; i++)
                    {
                        byte[] address = new byte[4];
                        ushort port;
                        int length = NativeMethods.lan866x_get_ip_address(devices[i], address, out port);
                        if (length == 4 && new IPAddress(address).ToString() == targetIp &&
                            NativeMethods.lan866x_is_node_available(devices[i]) != 0)
                            return devices[i];
                    }
                }
                Thread.Sleep(200);
            }
            while (DateTime.UtcNow < deadline);
            return IntPtr.Zero;
        }

        private void ReleasePins(byte[] pins)
        {
            ReleaseDigitalPinsVar input = new ReleaseDigitalPinsVar
            {
                PinIdListLength = (ushort)pins.Length,
                PinIdList = new byte[16]
            };
            Buffer.BlockCopy(pins, 0, input.PinIdList, 0, pins.Length);
            CheckReturnCode(NativeMethods.lan866x_releasedigitalpins(_client, ref input),
                "ReleaseDigitalPins");
        }

        private void ConfigureDigitalPin(byte pin, byte mode, byte driveStrength)
        {
            ConfigDigitalPinVar input = new ConfigDigitalPinVar
            {
                PinId = pin,
                Mode = mode,
                DriveStrength = driveStrength
            };
            CheckReturnCode(NativeMethods.lan866x_configdigitalpin(_client, ref input),
                "ConfigDigitalPin PA" + pin.ToString("00"));
        }

        private ushort OpenSpi(byte miso, byte sck, byte cs, byte mosi,
            byte mode, uint clock, string name)
        {
            OpenSpiVar input = new OpenSpiVar
            {
                PinIdMiso = miso,
                PinIdSck = sck,
                PinIdCs = cs,
                PinIdMosi = mosi,
                Mode = mode,
                ClockSpeed = clock
            };
            OpenSpiReply reply;
            CheckReturnCode(NativeMethods.lan866x_openspi(_client, ref input, out reply),
                "OpenSpi " + name);
            if (reply.HandleSpi == 0)
                throw new InvalidOperationException("OpenSpi " + name + "에서 유효한 핸들을 받지 못했습니다.");
            return reply.HandleSpi;
        }

        private ushort TryOpenSpi(byte miso, byte sck, byte cs, byte mosi,
            byte mode, uint clock, string name)
        {
            try
            {
                return OpenSpi(miso, sck, cs, mosi, mode, clock, name);
            }
            catch (InvalidOperationException ex)
            {
                // A release package intentionally contains only two remote SPI
                // ports because the V1.4.0 OSIRE app rejects a third one.
                if (_openFailures.Length != 0) _openFailures += "; ";
                _openFailures += ex.Message;
                return 0;
            }
        }

        private void WriteSpi(ushort handle, uint writeId, byte[] data, string name)
        {
            ExchangeSpi(handle, writeId, data, 0, name);
        }

        private byte[] ExchangeSpi(ushort handle, uint writeId, byte[] data,
            ushort readLength, string name)
        {
            if (data.Length > MaximumSpiPayload)
                throw new ArgumentOutOfRangeException("data", name + " SPI 데이터가 1400바이트를 초과합니다.");
            if (readLength > MaximumSpiPayload)
                throw new ArgumentOutOfRangeException("readLength");
            WriteAndReadSpiVar input = new WriteAndReadSpiVar
            {
                HandleSpi = handle,
                ReadDataLength = readLength,
                WriteId = writeId,
                WriteDataLength = (ushort)data.Length,
                WriteData = new byte[MaximumSpiPayload]
            };
            Buffer.BlockCopy(data, 0, input.WriteData, 0, data.Length);
            WriteAndReadSpiReply reply;
            CheckReturnCode(NativeMethods.lan866x_writeandreadspi(_client, ref input, out reply),
                "WriteAndReadSpi " + name);
            if (reply.ReadId != writeId)
                throw new InvalidOperationException(name + " SPI 응답 WriteId 불일치.");
            if (reply.ReadDataLength != readLength)
                throw new InvalidOperationException(name + " SPI 수신 길이 불일치 (요청=" +
                    readLength + ", 수신=" + reply.ReadDataLength + ").");
            byte[] result = new byte[reply.ReadDataLength];
            if (result.Length != 0)
                Buffer.BlockCopy(reply.ReadData, 0, result, 0, result.Length);
            return result;
        }

        private byte[] ExchangeSpiWithTimeout(ushort handle, uint writeId,
            byte[] data, ushort readLength, uint timeoutInUs, string name)
        {
            if (data == null || data.Length > MaximumSpiTimeoutPayload)
                throw new ArgumentOutOfRangeException("data", name +
                    " SPI timeout API 데이터는 64바이트 이하여야 합니다.");
            if (readLength > MaximumSpiTimeoutPayload)
                throw new ArgumentOutOfRangeException("readLength");
            WriteAndReadSpiTimeoutVar input = new WriteAndReadSpiTimeoutVar
            {
                HandleSpi = handle,
                WriteId = writeId,
                ReadDataLength = readLength,
                TimeoutInUs = timeoutInUs,
                WriteDataLength = (ushort)data.Length,
                WriteData = new byte[MaximumSpiTimeoutPayload]
            };
            Buffer.BlockCopy(data, 0, input.WriteData, 0, data.Length);
            WriteAndReadSpiTimeoutReply reply;
            CheckReturnCode(NativeMethods.lan866x_writeandreadspitimeout(
                _client, ref input, out reply), "WriteAndReadSpiTimeout " + name);
            if (reply.ReadId != writeId)
                throw new InvalidOperationException(name + " SPI 응답 WriteId 불일치.");
            if (reply.ReadDataLength != readLength)
                throw new InvalidOperationException(name + " SPI 수신 길이 불일치 (요청=" +
                    readLength + ", 수신=" + reply.ReadDataLength + ").");
            byte[] result = new byte[reply.ReadDataLength];
            if (result.Length != 0)
                Buffer.BlockCopy(reply.ReadData, 0, result, 0, result.Length);
            return result;
        }

        private void EnsureReady()
        {
            if (_client == IntPtr.Zero || NativeMethods.lan866x_is_node_available(_client) == 0)
                throw new InvalidOperationException("LAN8661 SOME/IP 장치가 연결되어 있지 않습니다.");
        }

        private void EnsureIlasReady()
        {
            EnsureReady();
            if (_click4IlasHandle == 0)
                throw new InvalidOperationException(
                    "ISE/ILaS 전송 포트가 현재 보드 설정에 없습니다.");
        }

        private void EnsureOspReady()
        {
            EnsureReady();
            if (_click3OspHandle == 0)
                throw new InvalidOperationException(
                    "OSP 직접 전송 포트가 현재 보드 설정에 없습니다.");
        }

        private void ResetState()
        {
            _client = IntPtr.Zero;
            _click1Handle = 0;
            _click2Handle = 0;
            _click3OspHandle = 0;
            _click4IlasHandle = 0;
            _click1WriteId = 0;
            _click2WriteId = 0;
            _ospWriteId = 0;
            _ilasWriteId = 0;
            _ospInitialized = false;
            _ospNodeCount = 0;
            _lastOspColors = null;
            _lastOspLedStatus = 0;
            _lastOspPwmHex = string.Empty;
            _openFailures = string.Empty;
        }

        private static void CheckReturnCode(int returnCode, string operation)
        {
            if (returnCode != 0)
                throw new InvalidOperationException(operation + " 실패 (Microchip ReturnCode=0x" +
                    returnCode.ToString("X2") + ")." + (returnCode == 0x31 ?
                    " 보드의 SPI 요청/응답 FIFO 용량이 부족합니다. 버퍼 수정 패키지를 적용하세요." : string.Empty));
        }

        private static void OnDeviceChanged(IntPtr client, IntPtr user)
        {
            // Discovery state is read synchronously by FindTarget/IsReady.
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void DeviceCallback(IntPtr client, IntPtr user);

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct ReleaseDigitalPinsVar
        {
            public ushort PinIdListLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            public byte[] PinIdList;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct ConfigDigitalPinVar
        {
            public byte PinId;
            public byte Mode;
            public byte DriveStrength;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct OpenSpiVar
        {
            public byte PinIdMiso;
            public byte PinIdSck;
            public byte PinIdCs;
            public byte PinIdMosi;
            public byte Mode;
            public uint ClockSpeed;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct OpenSpiReply
        {
            public ushort HandleSpi;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct WriteAndReadSpiVar
        {
            public ushort HandleSpi;
            public ushort ReadDataLength;
            public uint WriteId;
            public ushort WriteDataLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MaximumSpiPayload)]
            public byte[] WriteData;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct WriteAndReadSpiReply
        {
            public uint ReadId;
            public ushort ReadDataLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MaximumSpiPayload)]
            public byte[] ReadData;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct WriteAndReadSpiTimeoutVar
        {
            public ushort HandleSpi;
            public uint WriteId;
            public ushort ReadDataLength;
            public uint TimeoutInUs;
            public ushort WriteDataLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MaximumSpiTimeoutPayload)]
            public byte[] WriteData;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct WriteAndReadSpiTimeoutReply
        {
            public uint ReadId;
            public ushort ReadDataLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = MaximumSpiTimeoutPayload)]
            public byte[] ReadData;
        }

        private static class NativeMethods
        {
            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_get_devices(
                [Out] IntPtr[] buffer, int bufferLen, out int outCount);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_is_node_available(IntPtr client);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_get_ip_address(
                IntPtr client, [Out] byte[] address, out ushort port);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern void lan866x_factory_set_callbacks(
                DeviceCallback onAvailable, DeviceCallback onUnavailable, IntPtr user);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_releasedigitalpins(
                IntPtr client, ref ReleaseDigitalPinsVar input);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_configdigitalpin(
                IntPtr client, ref ConfigDigitalPinVar input);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_openspi(
                IntPtr client, ref OpenSpiVar input, out OpenSpiReply output);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_writeandreadspi(
                IntPtr client, ref WriteAndReadSpiVar input, out WriteAndReadSpiReply output);

            [DllImport(NativeLibrary, CallingConvention = CallingConvention.Cdecl)]
            internal static extern int lan866x_writeandreadspitimeout(
                IntPtr client, ref WriteAndReadSpiTimeoutVar input,
                out WriteAndReadSpiTimeoutReply output);
        }
    }
}
