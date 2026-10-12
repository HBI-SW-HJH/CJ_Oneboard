using System;

namespace LINMaster.LIN
{
    public static class CJLinProtocol
    {
        public const byte ControlFrameId = 0x01;
        public const byte Chair1StatusFrameId = 0x0A;
        public const byte Chair2StatusFrameId = 0x0B;
        public const byte Chair3StatusFrameId = 0x0C;
        public const byte Chair4StatusFrameId = 0x0D;
        public const byte ChairIdChangeOffset = 0x10;
        public const byte DataLength = 8;
        public const byte FlickerDurationMax = 250;
        public const byte MaximumFlickerHz = 100;
        public const byte DefaultFadeTimeUnits = 5;

        public static bool IsStatusFrameId(byte frameId)
        {
            return frameId >= Chair1StatusFrameId && frameId <= Chair4StatusFrameId;
        }

        public static byte[] BuildChairIdChangeData(byte currentId, byte newId)
        {
            if (!IsStatusFrameId(currentId) || !IsStatusFrameId(newId) || currentId == newId)
            {
                throw new ArgumentException("Chair ID must change within 0x0A~0x0D.");
            }
            byte[] data = new byte[DataLength];
            for (int index = 0; index < DataLength; index += 2)
            {
                data[index] = currentId;
                data[index + 1] = newId;
            }
            return data;
        }

        public static int GetChairNumber(byte statusFrameId)
        {
            return IsStatusFrameId(statusFrameId)
                ? statusFrameId - Chair1StatusFrameId + 1
                : 0;
        }

        public static double DurationToPercent(byte duration)
        {
            if (duration == 0) return 100.0;
            return Math.Min(FlickerDurationMax, duration) * 100.0 / FlickerDurationMax;
        }

        public static byte DurationPercentToRaw(byte percent)
        {
            if (percent == 0) return 0;
            return (byte)((Math.Min(100, (int)percent) * FlickerDurationMax + 50) / 100);
        }
    }

    [Flags]
    public enum CJGlobalGroup : byte
    {
        None = 0x00,
        Chair1 = 0x01,
        Chair2 = 0x02,
        Chair3 = 0x04,
        Chair4 = 0x08,
        All = Chair1 | Chair2 | Chair3 | Chair4
    }

    [Flags]
    public enum CJLocalGroup : byte
    {
        None = 0x00,
        FrtLh = 0x01,
        FrtRh = 0x02,
        Headrest = 0x04,
        Back = 0x08,
        Logo = 0x10,
        Strobe = 0x20,
        All = FrtLh | FrtRh | Headrest | Back | Logo | Strobe
    }

    public enum CJLampMode : byte
    {
        Flicker = 0x06,
        Normal = 0x07
    }

    public enum CJColorMethod : byte
    {
        Rgb = 0x00,
        Cie1931 = 0x01,
        Cie1976 = 0x02
    }

    [Flags]
    public enum CJFaultFlags : byte
    {
        None = 0x00,
        Temperature = 0x01,
        SupplyVoltage = 0x02,
        Short = 0x04,
        Open = 0x08,
        Communication = 0x10
    }

    public enum CJStatusPart : byte
    {
        FrtLh = 0,
        FrtRh = 1,
        Headrest = 2,
        Back = 3,
        Logo = 4,
        Strobe = 5
    }

    public sealed class CJStatusFrame
    {
        public byte FrameId { get; private set; }
        public int ChairNumber { get; private set; }
        public CJFaultFlags[] Faults { get; private set; }
        public byte[] RawData { get; private set; }
        public bool ReservedBytesValid { get; private set; }

        private CJStatusFrame()
        {
            Faults = new CJFaultFlags[6];
            RawData = new byte[CJLinProtocol.DataLength];
        }

        public static bool TryParse(LINFrame frame, out CJStatusFrame status)
        {
            status = null;
            if (frame == null || frame.Data == null ||
                !CJLinProtocol.IsStatusFrameId(frame.FrameID) ||
                frame.DataLength != CJLinProtocol.DataLength ||
                frame.ChecksumType != LINChecksumType.Enhanced ||
                !frame.IsValid)
            {
                return false;
            }

            var value = new CJStatusFrame
            {
                FrameId = frame.FrameID,
                ChairNumber = CJLinProtocol.GetChairNumber(frame.FrameID)
            };
            Array.Copy(frame.Data, value.RawData, CJLinProtocol.DataLength);

            for (int index = 0; index < value.Faults.Length; index++)
            {
                if ((frame.Data[index] >> 5) != index)
                    return false;
                value.Faults[index] = (CJFaultFlags)(frame.Data[index] & 0x1F);
            }
            value.ReservedBytesValid = frame.Data[6] == 0 && frame.Data[7] == 0;
            status = value;
            return true;
        }
    }

    public struct CJLampControlCommand
    {
        public const byte FrameId = CJLinProtocol.ControlFrameId;
        public const int MinimumPeriodMs = 10;

        public CJGlobalGroup GlobalGroup;
        public CJLocalGroup LocalGroup;
        public CJLampMode Mode;
        public bool Synchronize;
        public bool Active;
        public bool FadeEnabled;
        public CJColorMethod ColorMethod;
        public byte Component1;
        public byte Component2;
        public byte Component3;
        public byte Operation6;
        public byte Operation7;

        public byte[] ToArray()
        {
            byte control = (byte)((byte)Mode & 0x07);
            if (Synchronize) control |= 0x08;
            if (Active) control |= 0x10;
            if (FadeEnabled) control |= 0x20;
            control |= (byte)(((byte)ColorMethod & 0x03) << 6);

            return new byte[]
            {
                (byte)GlobalGroup, (byte)LocalGroup, control,
                Component1, Component2, Component3,
                (byte)Math.Min(250, (int)Operation6),
                (byte)Math.Min(Mode == CJLampMode.Flicker
                    ? CJLinProtocol.MaximumFlickerHz : 250, (int)Operation7)
            };
        }
    }
}
