using System;

namespace LINMaster.LIN
{
    internal static class UDSProtocol
    {
        internal const ushort ProductInfoDid = 0xF202;
        internal const ushort TargetProductDid = 0xF205;
        internal const int MaxMessageLength = 130;
        internal const int ProductDataLength = 29;

        internal sealed class ProductInfo
        {
            internal string SerialNumber = "26I16-0001";
            internal string PartNumber = "ZX1-0025";
            internal ushort Version = 0x0001;
            internal byte ChairNumber = 0x0A;
            internal uint InitNumber;

            internal byte Nad
            {
                get { return (byte)(ChairNumber - 9); }
            }

            internal string VersionText
            {
                get { return string.Format("{0}.{1}.{2}.{3}", (Version >> 12) & 15, (Version >> 8) & 15, (Version >> 4) & 15, Version & 15); }
            }

            internal byte[] ToData()
            {
                if ((!System.Text.RegularExpressions.Regex.IsMatch(SerialNumber ?? "", @"^\d{2}[A-L](0[1-9]|[12]\d|3[01])-\d{4}$")) || (PartNumber == null) || (PartNumber.Length != 8) || (ChairNumber < 0x0A) || (ChairNumber > 0x0D))
                {
                    throw new ArgumentException("Serial: 26I16-0001 / Part: ASCII 8자 / Chair: 0x0A~0x0D");
                }
                foreach (char c in PartNumber)
                {
                    if ((c < ' ') || (c > '~'))
                    {
                        throw new ArgumentException("Part Number는 ASCII 8자입니다.");
                    }
                }
                if ((InitNumber == 0) || (InitNumber == uint.MaxValue))
                {
                    throw new ArgumentException("Init Number를 먼저 Read하십시오.");
                }
                byte[] data = new byte[ProductDataLength];
                System.Text.Encoding.ASCII.GetBytes(SerialNumber).CopyTo(data, 0);
                data[10] = (byte)'*';
                System.Text.Encoding.ASCII.GetBytes(PartNumber).CopyTo(data, 11);
                data[19] = (byte)'*';
                data[20] = (byte)(Version >> 8);
                data[21] = (byte)Version;
                data[22] = (byte)'*';
                data[23] = ChairNumber;
                data[24] = (byte)'*';
                data[25] = (byte)(InitNumber >> 24);
                data[26] = (byte)(InitNumber >> 16);
                data[27] = (byte)(InitNumber >> 8);
                data[28] = (byte)InitNumber;
                return data;
            }

            internal static ProductInfo FromResponse(byte[] payload)
            {
                if ((payload == null) || (payload.Length != ProductDataLength + 3) || (payload[0] != 0x62) || (payload[1] != (byte)(ProductInfoDid >> 8)) || (payload[2] != (byte)(ProductInfoDid & 0xFF)))
                {
                    return null;
                }
                return FromData(payload, 3);
            }

            internal static ProductInfo FromData(byte[] data, int offset)
            {
                if ((data == null) || (offset < 0) || ((offset + ProductDataLength) > data.Length) || (data[offset + 10] != (byte)'*') || (data[offset + 19] != (byte)'*') || (data[offset + 22] != (byte)'*') || (data[offset + 24] != (byte)'*'))
                {
                    return null;
                }
                ProductInfo info = new ProductInfo
                {
                    SerialNumber = System.Text.Encoding.ASCII.GetString(data, offset, 10),
                    PartNumber = System.Text.Encoding.ASCII.GetString(data, offset + 11, 8),
                    Version = (ushort)((data[offset + 20] << 8) | data[offset + 21]),
                    ChairNumber = data[offset + 23],
                    InitNumber = ((uint)data[offset + 25] << 24) | ((uint)data[offset + 26] << 16) | ((uint)data[offset + 27] << 8) | data[offset + 28]
                };
                try
                {
                    info.ToData();
                    return info;
                }
                catch (ArgumentException)
                {
                    return null;
                }
            }
        }

        internal static byte[] BuildService(byte sid, ushort did, byte[] data = null)
        {
            byte[] payload = new byte[3 + (data == null ? 0 : data.Length)];
            payload[0] = sid;
            payload[1] = (byte)(did >> 8);
            payload[2] = (byte)did;
            if (data != null)
            {
                data.CopyTo(payload, 3);
            }
            return payload;
        }

        internal static System.Collections.Generic.List<byte[]> Encode(byte nad, byte[] payload)
        {
            if ((nad == 0) || (nad > BroadcastNad) || (payload == null) || (payload.Length == 0) || (payload.Length > MaxMessageLength))
            {
                throw new ArgumentException("Invalid LIN diagnostic payload.");
            }
            var frames = new System.Collections.Generic.List<byte[]>();
            int offset = 0;
            int sequence = 1;
            while (offset < payload.Length)
            {
                byte[] frame = new byte[8];
                for (int i = 0; i < frame.Length; i++)
                {
                    frame[i] = 0xFF;
                }
                frame[0] = nad;
                int start;
                int count;
                if (payload.Length <= 6)
                {
                    frame[1] = (byte)payload.Length;
                    start = 2;
                    count = payload.Length;
                }
                else if (offset == 0)
                {
                    frame[1] = 0x10;
                    frame[2] = (byte)payload.Length;
                    start = 3;
                    count = 5;
                }
                else
                {
                    frame[1] = (byte)(0x20 | sequence);
                    sequence = (sequence + 1) & 15;
                    start = 2;
                    count = Math.Min(6, payload.Length - offset);
                }
                Array.Copy(payload, offset, frame, start, count);
                offset += count;
                frames.Add(frame);
            }
            return frames;
        }

        internal sealed class Decoder
        {
            private readonly byte _nad;
            private readonly byte[] _data = new byte[MaxMessageLength];
            private int _length;
            private int _count;
            private int _sequence;

            internal Decoder(byte nad)
            {
                _nad = nad;
            }

            internal bool Feed(LINFrame frame, out byte[] payload)
            {
                payload = null;
                if ((!IsResponse(frame)) || (frame.Data[0] != _nad))
                {
                    return false;
                }
                byte[] data = frame.Data;
                int type = data[1] >> 4;
                if (type == 0)
                {
                    _length = 0;
                    int count = data[1] & 15;
                    if ((count < 1) || (count > 6))
                    {
                        return false;
                    }
                    payload = new byte[count];
                    Array.Copy(data, 2, payload, 0, count);
                    return true;
                }
                if (type == 1)
                {
                    _length = ((data[1] & 15) << 8) | data[2];
                    _count = 0;
                    if ((_length < 7) || (_length > MaxMessageLength))
                    {
                        _length = 0;
                        return false;
                    }
                    Array.Copy(data, 3, _data, 0, 5);
                    _count = 5;
                    _sequence = 1;
                    return false;
                }
                if ((type != 2) || (_length == 0))
                {
                    return false;
                }
                if ((data[1] & 15) != _sequence)
                {
                    _length = 0;
                    return false;
                }
                int remaining = Math.Min(6, _length - _count);
                Array.Copy(data, 2, _data, _count, remaining);
                _count += remaining;
                _sequence = (_sequence + 1) & 15;
                if (_count != _length)
                {
                    return false;
                }
                payload = new byte[_length];
                Array.Copy(_data, payload, _length);
                _length = 0;
                return true;
            }
        }

        internal const byte RequestFrameId = 0x3C;
        internal const byte ResponseFrameId = 0x3D;
        internal const byte BroadcastNad = 0x7F;
        internal const byte BootNad = 0x7D;
        internal const byte DiscoverySlotCount = 16;
        internal const ushort DiscoveryRoutine = 0xF300;
        internal const ushort TargetReadRoutine = 0xF301;

        internal const byte WriteDataByIdentifier = 0x2E;

        internal static byte[] BuildDiscoveryResult(byte slot)
        {
            return new byte[] { 0x31, 0x03, 0xF3, 0x00, slot };
        }

        internal static byte[] BuildTargetRead(uint initNumber)
        {
            return new byte[] { 0x31, 0x03, 0xF3, 0x01,
                (byte)(initNumber >> 24), (byte)(initNumber >> 16),
                (byte)(initNumber >> 8), (byte)initNumber };
        }

        internal static byte[] BuildTargetProductWrite(ProductInfo info)
        {
            return BuildService(WriteDataByIdentifier, TargetProductDid, info.ToData());
        }

        internal static bool TryParseTargetRead(byte[] payload, ushort routine, out ProductInfo info)
        {
            info = null;
            if ((payload == null) || (payload.Length != ProductDataLength + 4) || (payload[0] != 0x71) || (payload[1] != 0x03) || (payload[2] != (byte)(routine >> 8)) || (payload[3] != (byte)routine))
            {
                return false;
            }
            info = ProductInfo.FromData(payload, 4);
            return info != null;
        }

        internal static bool TryParseBootDiscovery(byte[] payload, out uint initNumber)
        {
            initNumber = 0;
            if ((payload == null) || (payload.Length != 9) || (payload[0] != 0x71) || (payload[1] != 0x03) || (payload[2] != 0xF3) || (payload[3] != 0x00) || (payload[8] != 1))
            {
                return false;
            }
            initNumber = ((uint)payload[4] << 24) | ((uint)payload[5] << 16) | ((uint)payload[6] << 8) | payload[7];
            return (initNumber != 0) && (initNumber != uint.MaxValue);
        }

        internal static bool IsWritePositiveResponse(byte[] payload, ushort did)
        {
            return (payload != null) && (payload.Length == 3) && (payload[0] == 0x6E) && (payload[1] == (byte)(did >> 8)) && (payload[2] == (byte)did);
        }

        private static bool IsResponse(LINFrame frame)
        {
            return frame != null && frame.FrameID == ResponseFrameId &&
                frame.DataLength == 8;
        }

    }
}
