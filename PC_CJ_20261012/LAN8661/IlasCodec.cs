using System;
using System.Collections.Generic;

namespace LINMaster.LAN8661
{
    /// <summary>
    /// ISELED/ILaS wire-frame codec for the INLC10AQ generation.
    /// A downstream command is emitted as a 96-bit, MSB-first SPI stream:
    /// idle, frame sync, frequency sync, RLC4/5 payload, optional CRC, idle.
    /// </summary>
    internal static class IlasCodec
    {
        private static readonly byte[] RlcEncode =
        {
            0x04, 0x09, 0x14, 0x0A,
            0x11, 0x0B, 0x0E, 0x19,
            0x12, 0x13, 0x16, 0x0D,
            0x1A, 0x1B, 0x1C, 0x1D
        };

        private static readonly sbyte[] RlcDecode = BuildDecodeTable();
        private static readonly byte[] Crc8Table = BuildCrc8Table();
        private static readonly byte[] Crc4Table =
        {
            0x0, 0x3, 0x6, 0x5, 0xC, 0xF, 0xA, 0x9,
            0xB, 0x8, 0xD, 0xE, 0x7, 0x4, 0x1, 0x2
        };

        public static byte[] BuildResetFrame()
        {
            return BuildDownstreamFrame(0x02, 0, 0, false);
        }

        public static byte[] BuildInitFrame(ushort firstAddress, uint configuration)
        {
            return BuildDownstreamFrame(0x09, firstAddress, configuration, true);
        }

        public static byte[] BuildRgbFrame(ushort address, byte red, byte green, byte blue)
        {
            uint data = ((uint)red << 16) | ((uint)green << 8) | blue;
            return BuildDownstreamFrame(0x04, address, data, true);
        }

        public static byte[] BuildRgbFrames(ushort firstAddress, int count,
            byte red, byte green, byte blue)
        {
            if (count <= 0) throw new ArgumentOutOfRangeException("count");
            if (firstAddress > 0x0FFF || firstAddress + count - 1 > 0x0FFF)
                throw new ArgumentOutOfRangeException("firstAddress");

            byte[] frames = new byte[count * 12];
            for (int index = 0; index < count; index++)
            {
                byte[] frame = BuildRgbFrame((ushort)(firstAddress + index), red, green, blue);
                Buffer.BlockCopy(frame, 0, frames, index * frame.Length, frame.Length);
            }
            return frames;
        }

        public static byte[] BuildRgbFrames(ushort firstAddress, byte[] rgb24)
        {
            if (rgb24 == null || rgb24.Length == 0 || (rgb24.Length % 3) != 0)
                throw new ArgumentException("ISE RGB 배열은 LED당 R/G/B 3바이트여야 합니다.", "rgb24");

            int count = rgb24.Length / 3;
            if (firstAddress > 0x0FFF || firstAddress + count - 1 > 0x0FFF)
                throw new ArgumentOutOfRangeException("firstAddress");

            byte[] frames = new byte[count * 12];
            for (int index = 0; index < count; index++)
            {
                int rgbOffset = index * 3;
                byte[] frame = BuildRgbFrame((ushort)(firstAddress + index),
                    rgb24[rgbOffset], rgb24[rgbOffset + 1], rgb24[rgbOffset + 2]);
                Buffer.BlockCopy(frame, 0, frames, index * frame.Length, frame.Length);
            }
            return frames;
        }

        public static byte[] BuildReadTemperatureFrame()
        {
            // The reference driver issues temperature acquisition as a chain
            // read at address zero; responding nodes return their own address.
            return BuildDownstreamFrame(0x08, 0, 0, true);
        }

        public static byte[] BuildDownstreamFrame(byte instruction, ushort address,
            uint data, bool crcEnabled)
        {
            if (instruction > 0x0F) throw new ArgumentOutOfRangeException("instruction");
            if (address > 0x0FFF) throw new ArgumentOutOfRangeException("address");
            if (data > 0xFFFFFF) throw new ArgumentOutOfRangeException("data");

            uint encodedInstruction = EncodeNibbles(instruction, 1);
            uint encodedAddress = EncodeNibbles(address, 3);
            uint encodedData = EncodeNibbles(data, 6);
            uint encodedCrc = crcEnabled
                ? EncodeNibbles(ComputeDownstreamCrc(encodedInstruction, encodedAddress, encodedData), 2)
                : 0;

            BitWriter writer = new BitWriter(12);
            writer.Write(0x3F, 6);             // idle before frame
            writer.Write(0, 15);               // frame sync
            writer.Write(0x15, 5);             // frequency sync: 10101
            writer.Write(encodedInstruction, 5);
            writer.Write(encodedAddress, 15);
            writer.Write(encodedData, 30);
            if (crcEnabled) writer.Write(encodedCrc, 10);
            writer.WriteOnes(96 - writer.BitCount);
            return writer.ToArray();
        }

        public static bool TryDecodeUpstream(byte[] raw, out ushort address,
            out ushort data, out string error)
        {
            address = 0;
            data = 0;
            error = string.Empty;
            if (raw == null || raw.Length < 5)
            {
                error = "ILaS 응답이 5바이트보다 짧습니다.";
                return false;
            }

            bool[] msbBits = ToBits(raw, false);
            if (TryDecodeUpstreamBits(msbBits, out address, out data)) return true;

            // Some SPI controllers report slave data LSB-first. Accept that form
            // only when the RLC values and upstream CRC both validate.
            bool[] lsbBits = ToBits(raw, true);
            if (TryDecodeUpstreamBits(lsbBits, out address, out data)) return true;

            error = "ILaS 응답에서 유효한 Frequency Sync/RLC/CRC 프레임을 찾지 못했습니다. RAW=" +
                BitConverter.ToString(raw);
            return false;
        }

        private static bool TryDecodeUpstreamBits(bool[] bits, out ushort address, out ushort data)
        {
            address = 0;
            data = 0;
            // The half-duplex SPI receiver normally starts at frequency sync.
            // Searching also tolerates leading frame-sync or controller padding.
            for (int start = 0; start + 40 <= bits.Length; start++)
            {
                if (ReadBits(bits, start, 5) != 0x15) continue;
                uint encodedAddress = ReadBits(bits, start + 5, 15);
                uint encodedData = ReadBits(bits, start + 20, 15);
                uint encodedCrc = ReadBits(bits, start + 35, 5);
                byte crc = ComputeUpstreamCrc(encodedAddress, encodedData);
                if (RlcEncode[crc] != encodedCrc) continue;

                uint decodedAddress;
                uint decodedData;
                if (!TryDecodeNibbles(encodedAddress, 3, out decodedAddress) ||
                    !TryDecodeNibbles(encodedData, 3, out decodedData))
                    continue;
                address = (ushort)decodedAddress;
                data = (ushort)decodedData;
                return true;
            }
            return false;
        }

        private static uint EncodeNibbles(uint value, int nibbleCount)
        {
            uint encoded = 0;
            for (int i = 0; i < nibbleCount; i++)
            {
                encoded |= (uint)RlcEncode[value & 0x0F] << (i * 5);
                value >>= 4;
            }
            return encoded;
        }

        private static bool TryDecodeNibbles(uint encoded, int nibbleCount, out uint value)
        {
            value = 0;
            for (int i = 0; i < nibbleCount; i++)
            {
                int code = (int)(encoded & 0x1F);
                int decoded = RlcDecode[code];
                if (decoded < 0) return false;
                value |= (uint)decoded << (i * 4);
                encoded >>= 5;
            }
            return true;
        }

        private static byte ComputeDownstreamCrc(uint instruction, uint address, uint data)
        {
            // Matches the reference driver's crc8Long downstream path. The
            // 50 encoded bits are right-aligned in seven bytes.
            ulong encoded = ((ulong)instruction << 45) | ((ulong)address << 30) | data;
            byte crc = 0x27;
            for (int shift = 48; shift >= 0; shift -= 8)
                crc = Crc8Table[crc ^ (byte)(encoded >> shift)];
            return (byte)~crc;
        }

        private static byte ComputeUpstreamCrc(uint address, uint data)
        {
            // Address and data are 15 RLC bits each. The reference driver uses
            // a 4-bit CRC with initial value 7 and final XOR F.
            uint encoded = (address << 15) | data;
            byte crc = 0x07;
            for (int shift = 28; shift >= 0; shift -= 4)
                crc = Crc4Table[crc ^ (byte)((encoded >> shift) & 0x0F)];
            return (byte)(crc ^ 0x0F);
        }

        private static byte[] BuildCrc8Table()
        {
            byte[] table = new byte[256];
            for (int i = 0; i < table.Length; i++)
            {
                int value = i;
                for (int bit = 0; bit < 8; bit++)
                    value = (value & 0x80) != 0 ? ((value << 1) ^ 0x07) : (value << 1);
                table[i] = (byte)value;
            }
            return table;
        }

        private static sbyte[] BuildDecodeTable()
        {
            sbyte[] table = new sbyte[32];
            for (int i = 0; i < table.Length; i++) table[i] = -1;
            for (int i = 0; i < RlcEncode.Length; i++) table[RlcEncode[i]] = (sbyte)i;
            return table;
        }

        private static bool[] ToBits(byte[] data, bool lsbFirst)
        {
            bool[] bits = new bool[data.Length * 8];
            int offset = 0;
            for (int i = 0; i < data.Length; i++)
            {
                if (lsbFirst)
                {
                    for (int bit = 0; bit < 8; bit++) bits[offset++] = (data[i] & (1 << bit)) != 0;
                }
                else
                {
                    for (int bit = 7; bit >= 0; bit--) bits[offset++] = (data[i] & (1 << bit)) != 0;
                }
            }
            return bits;
        }

        private static uint ReadBits(bool[] bits, int start, int count)
        {
            uint value = 0;
            for (int i = 0; i < count; i++) value = (value << 1) | (bits[start + i] ? 1U : 0U);
            return value;
        }

        private sealed class BitWriter
        {
            private readonly byte[] _buffer;
            public int BitCount { get; private set; }

            public BitWriter(int byteCount)
            {
                _buffer = new byte[byteCount];
            }

            public void Write(uint value, int count)
            {
                for (int bit = count - 1; bit >= 0; bit--)
                {
                    if ((value & (1U << bit)) != 0)
                        _buffer[BitCount >> 3] |= (byte)(1 << (7 - (BitCount & 7)));
                    BitCount++;
                }
            }

            public void WriteOnes(int count)
            {
                for (int i = 0; i < count; i++) Write(1, 1);
            }

            public byte[] ToArray()
            {
                if (BitCount != _buffer.Length * 8)
                    throw new InvalidOperationException("ILaS 프레임 길이가 96비트가 아닙니다.");
                return _buffer;
            }
        }
    }
}
