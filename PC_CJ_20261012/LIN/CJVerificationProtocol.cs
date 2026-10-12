using System;

namespace LINMaster.LIN
{
    public static class CJVerificationProtocol
    {
        public const byte RequestIdBase = 0x28;
        public const byte ResponseIdBase = 0x2C;
        public const byte Read = 0x01;
        public const byte Flicker = 0x02;
        public const byte Fade = 0x03;
        public const byte Stop = 0x04;
        public const byte ReadRgb = 0x05;
        public const byte Result = 0x06;
        public const byte Start = 0x07;
        public const byte FadePoint = 0x08;
        public const byte FadeStatus = 0x09;
        public const byte ReadExpected = 0x0A;
        public const byte FadeRgb = 0x0B;
        public const byte ProtocolVersion = 2;
        public const byte AllZones = 0x06;

        public static byte RequestId(byte chairId)
        {
            if (!CJLinProtocol.IsStatusFrameId(chairId)) throw new ArgumentOutOfRangeException("chairId");
            return (byte)(RequestIdBase + chairId - CJLinProtocol.Chair1StatusFrameId);
        }

        public static byte ResponseId(byte chairId)
        {
            if (!CJLinProtocol.IsStatusFrameId(chairId)) throw new ArgumentOutOfRangeException("chairId");
            return (byte)(ResponseIdBase + chairId - CJLinProtocol.Chair1StatusFrameId);
        }
    }
}
