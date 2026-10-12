using System;
using LINMaster.CANoe;
using LINMaster.LIN;

namespace LINMaster.CAN
{
    // CANoe owns the VN5650 hardware and only transports the same 8-byte
    // AD3306LedCommand used by LIN and USB.
    public sealed class CANoeCANGateway : ILINGateway
    {
        public const uint PcTransmitId = 0x18FFFF02;
        public const uint PcReceiveId = 0x18FFFF01;
        public const int BitRate = 500000;

        private const string CanNamespace = "SunshineCAN";
        private readonly CANoeBridge _bridge;
        private bool _connected;
        private ulong _lastRxCount;
        private ulong _lastBulkGeneration;
        private bool _disposed;

        public CANoeCANGateway(CANoeBridge bridge)
        {
            _bridge = bridge ?? throw new ArgumentNullException(nameof(bridge));
        }

        public bool IsConnected => _connected && _bridge.IsAttached;
        public string PortName => "VN5650 / CANoe 20 / CAN1";

        public event Action<LINFrame> FrameReceived;
        public event Action<string> StatusMessage;
        public event Action<string> ErrorMessage;
        public event Action<byte[], string> RawPacket;

        public bool Connect(string portName)
        {
            string message;
            if (!_bridge.TryAttach(allowStart: true, out message)
                || !_bridge.TryPrepareCan(out message))
            {
                ErrorMessage?.Invoke("[CAN connect failed] " + message);
                _connected = false;
                return false;
            }

            object count;
            _lastRxCount = _bridge.TryGetSystemVariable(
                CanNamespace, "RxCount", out count, out message)
                ? Convert.ToUInt64(count)
                : 0;
            _lastBulkGeneration = _bridge.TryGetSystemVariable(
                CanNamespace, "BulkGeneration", out count, out message)
                ? Convert.ToUInt64(count)
                : 0;

            _connected = true;
            StatusMessage?.Invoke(
                "[CAN connected] VN5650 CAN1, 500 kbit/s, PC TX=0x18FFFF02x, PC RX=0x18FFFF01x");
            return true;
        }

        public void Disconnect()
        {
            if (!_connected) return;
            _connected = false;
            _bridge.MarkDisconnected();
            StatusMessage?.Invoke("[CAN disconnected]");
        }

        public void Poll()
        {
            if (!IsConnected) return;

            string message;
            object bulkGenerationValue;
            if (_bridge.TryGetSystemVariable(
                CanNamespace, "BulkGeneration", out bulkGenerationValue, out message))
            {
                ulong bulkGeneration = Convert.ToUInt64(bulkGenerationValue);
                if (bulkGeneration != _lastBulkGeneration)
                {
                    _lastBulkGeneration = bulkGeneration;
                    ReceiveBulkFrames();
                    return;
                }
            }

            object countValue;
            if (!_bridge.TryGetSystemVariable(
                CanNamespace, "RxCount", out countValue, out message))
            {
                ErrorMessage?.Invoke("[CAN receive failed] " + message);
                return;
            }

            ulong count = Convert.ToUInt64(countValue);
            if (count == _lastRxCount) return;
            _lastRxCount = count;

            byte[] data = new byte[8];
            for (int i = 0; i < data.Length; i++)
            {
                object value;
                if (!_bridge.TryGetSystemVariable(
                    CanNamespace, "RxData" + i, out value, out message))
                {
                    ErrorMessage?.Invoke("[CAN receive failed] " + message);
                    return;
                }
                data[i] = Convert.ToByte(value);
            }

            LINFrame frame = BuildLogFrame(data, LINDirection.RX);
            FrameReceived?.Invoke(frame);
            RawPacket?.Invoke((byte[])data.Clone(), "RX");
            StatusMessage?.Invoke("[CAN RX] EXT 0x18FFFF01, DLC 8");
        }

        private void ReceiveBulkFrames()
        {
            string message;
            object lengthValue;
            object overflowValue;
            byte[] bulkData;
            if (!_bridge.TryGetSystemVariable(CanNamespace, "BulkLength", out lengthValue, out message) ||
                !_bridge.TryGetSystemVariable(CanNamespace, "BulkOverflow", out overflowValue, out message) ||
                !_bridge.TryGetSystemVariableData(CanNamespace, "RxBulk", out bulkData, out message))
            {
                ErrorMessage?.Invoke("[CAN bulk receive failed] " + message);
                return;
            }

            int length = Convert.ToInt32(lengthValue);
            if (Convert.ToInt32(overflowValue) != 0)
            {
                ErrorMessage?.Invoke("[CAN bulk receive failed] CANoe receive buffer overflow");
                return;
            }
            if (length < 0 || length > bulkData.Length || (length & 0x07) != 0)
            {
                ErrorMessage?.Invoke("[CAN bulk receive failed] invalid buffered length " + length);
                return;
            }

            int frameCount = length / 8;
            for (int offset = 0; offset < length; offset += 8)
            {
                byte[] data = new byte[8];
                Array.Copy(bulkData, offset, data, 0, data.Length);
                FrameReceived?.Invoke(BuildLogFrame(data, LINDirection.RX));
                RawPacket?.Invoke((byte[])data.Clone(), "RX");
            }
            StatusMessage?.Invoke("[CAN bulk RX] EXT 0x18FFFF01, " + frameCount + " frames");
        }

        public bool SendPublisher(LINFrame frame)
        {
            if (!IsConnected)
            {
                ErrorMessage?.Invoke("CAN is not connected.");
                return false;
            }
            if (frame == null || frame.Data == null || frame.DataLength != 8)
            {
                ErrorMessage?.Invoke("CAN LED command must contain exactly 8 bytes.");
                return false;
            }

            byte[] data = new byte[8];
            Array.Copy(frame.Data, data, data.Length);

            string message;
            if (!_bridge.TrySendCanCommand(data, out message))
            {
                ErrorMessage?.Invoke("[CAN send failed] " + message);
                return false;
            }

            FrameReceived?.Invoke(BuildLogFrame(data, LINDirection.TX));
            RawPacket?.Invoke((byte[])data.Clone(), "TX");
            StatusMessage?.Invoke("[CAN TX] EXT 0x18FFFF02, DLC 8");
            return true;
        }

        public bool SendSubscriberRequest(
            byte frameID,
            byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced)
        {
            ErrorMessage?.Invoke("CAN uses the board response on EXT ID 0x18FFFF01; a LIN subscriber request is not used.");
            return false;
        }

        public void Dispose()
        {
            if (_disposed) return;
            Disconnect();
            _disposed = true;
        }

        private static LINFrame BuildLogFrame(byte[] data, LINDirection direction)
        {
            LINFrame frame = new LINFrame
            {
                Timestamp = DateTime.Now,
                Direction = direction,
                FrameID = AD3306LedCommand.FrameId,
                DataLength = 8,
                FrameType = LINFrameType.Publisher,
                ChecksumType = LINChecksumType.Enhanced,
                IsValid = true
            };
            Array.Copy(data, frame.Data, data.Length);
            return frame;
        }
    }
}
