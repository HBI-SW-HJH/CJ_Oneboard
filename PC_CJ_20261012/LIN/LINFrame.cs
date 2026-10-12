using System;

namespace LINMaster.LIN
{
    // =========================================================================
    // LINFrame – LIN 버스 프레임 하나를 표현하는 데이터 클래스
    //
    // [IndieMicro Gateway 패킷 구조]  (User Guide V0.1, Chapter 6)
    //
    //  ┌────────┬─────────┬─────┬─────────────┬────────────────┐
    //  │ Head   │ FrameID │ PEC │ ControlByte │ Data[0..Len-1] │
    //  │ (1byte)│ (1byte) │(1B) │   (1byte)   │   (0~8 byte)   │
    //  └────────┴─────────┴─────┴─────────────┴────────────────┘
    //
    //  Head:
    //    0x55 = SEND    (PC → Gateway: 데이터 전송 또는 요청)
    //    0xA5 = ACK/RSP (Gateway → PC: 응답)
    //
    //  FrameID:  0x00 ~ 0x3F  (LIN ID, parity bit 미포함)
    //
    //  PEC:  (ControlByte + Data[0] + Data[1] + ... + Data[N-1]) & 0xFF
    //
    //  ControlByte:
    //    bit[7:4] = DataLength  (0~8)
    //    bit[3:1] = FrameType
    //                001 = Publisher  (데이터 전송)
    //                000 = Subscriber (데이터 요청)
    //    bit[0]   = ChecksumType
    //                0 = Classic   (LIN 1.x 방식)
    //                1 = Enhanced  (LIN 2.x 방식, ID 포함)
    //
    // [TX 흐름 – Publisher]
    //   PC → SEND(0x55, FID, PEC, CtrlByte, Data) → Gateway → LIN 버스 → Slave
    //   Gateway → ACK(0xA5, FID, PEC, CtrlByte(DataLen=0)) → PC
    //
    // [TX 흐름 – Subscriber]
    //   PC → Request(0x55, FID, PEC, CtrlByte(Type=Sub), 0xFF*N) → Gateway → LIN 버스
    //   Slave → 응답 데이터 → Gateway
    //   Gateway → RSP(0xA5, FID, PEC, CtrlByte, Data) → PC
    // =========================================================================

    // ── 열거형 ──────────────────────────────────────────────────────────────

    /// <summary>프레임 방향: TX = PC가 보낸 것, RX = PC가 받은 것</summary>
    public enum LINDirection { TX, RX }

    /// <summary>프레임 종류: Publisher = 데이터 전송, Subscriber = 데이터 요청</summary>
    public enum LINFrameType { Publisher = 1, Subscriber = 0 }

    /// <summary>체크섬 방식: Classic = LIN 1.x, Enhanced = LIN 2.x (ID 포함)</summary>
    public enum LINChecksumType { Classic = 0, Enhanced = 1 }

    [Flags]
    public enum AD3306LedTarget : byte
    {
        None = 0x00,
        Fio = 0x01,
        Ilas = 0x02,
        Both = Fio | Ilas
    }

    [Flags]
    public enum AD3306LedBackend : byte
    {
        Auto = 0x00,
        Adi = 0x01,
        Microchip = 0x02,
        Both = Adi | Microchip
    }

    public enum AD3306LedFunction : byte
    {
        Stop = 0x00,
        Solid = 0x01,
        RgbwShift = 0x02,
        LedShift = 0x03,
        LedShiftUp = 0x04,
        ShiftStack = 0x05,
        ShiftStackUp = 0x06,
        Move = 0x07,
        Off = 0x08,
        Touch = 0x09
    }

    public enum AD3306FadeMode : byte
    {
        None = 0x00,
        FadeIn = 0x01,
        FadeOut = 0x02,
        FadeInOut = 0x03
    }

    // Shared PC/CPU wire contract used unchanged by LIN, CAN, and USB.
    public struct AD3306LedCommand
    {
        public const byte FrameId = 0x3A;
        public static volatile bool ServiceReadActive;

        public byte TargetAndFid;
        public byte LedNumber;
        public byte Cx;
        public byte Cy;
        public byte Brightness;
        public byte Function;
        public byte Period; // milliseconds, 1..255
        public byte Fade;

        public byte[] ToArray()
        {
            return new byte[]
            {
                TargetAndFid, LedNumber, Cx, Cy,
                Brightness, Function, Period, Fade
            };
        }

        public static byte PackTargetAndFid(AD3306LedTarget target, byte fid)
        {
            return PackTargetAndFid(target, fid, AD3306LedBackend.Auto);
        }

        public static byte PackTargetAndFid(AD3306LedTarget target, byte fid,
            AD3306LedBackend backend)
        {
            // Wire value 0 already means every detected backend.  Use it for
            // the UI's Both selection because wire value 3 + ISE would create
            // 0xE0, which is reserved for Sunshine service/Read requests.
            AD3306LedBackend encodedBackend = backend == AD3306LedBackend.Both
                ? AD3306LedBackend.Auto
                : backend;
            byte high = (byte)(((byte)target & 0x03) |
                               (((byte)encodedBackend & 0x03) << 2));
            return (byte)((high << 4) | (fid & 0x0F));
        }

        // Fade mode uses bits 7:6. Time uses bits 5:0 in 50 ms units.
        public static byte PackFade(AD3306FadeMode mode, int milliseconds)
        {
            int units = Math.Max(0, Math.Min(63, (milliseconds + 49) / 50));
            return (byte)((((byte)mode & 0x03) << 6) | units);
        }
    }


    // =========================================================================
    // LINFrame 클래스
    // =========================================================================
    public class LINFrame
    {
        // ──────────────────────────────────────────────────────────────────
        // 프로퍼티
        // ──────────────────────────────────────────────────────────────────

        public DateTime        Timestamp    { get; set; }  // 프레임 생성 시각
        public LINDirection    Direction    { get; set; }  // TX / RX
        public byte            FrameID      { get; set; }  // LIN ID (0x00~0x3F, parity 미포함)
        public byte            PEC          { get; set; }  // ControlByte + Data 합산 체크섬
        public byte            DataLength   { get; set; }  // 실제 데이터 바이트 수 (0~8)
        public LINFrameType    FrameType    { get; set; }  // Publisher / Subscriber
        public LINChecksumType ChecksumType { get; set; }  // Classic / Enhanced
        public byte[]          Data         { get; set; }  // 데이터 버퍼 (최대 8바이트)
        public bool            IsACK        { get; set; }  // true = Gateway 응답 패킷 (Head=0xA5)
        public bool            IsValid      { get; set; }  // true = PEC 검증 통과


        // =========================================================================
        // 생성자
        // =========================================================================

        public LINFrame()
        {
            Timestamp = DateTime.Now;
            Data      = new byte[8];  // 항상 8바이트 할당 (사용하는 만큼만 유효)
            IsValid   = true;
        }


        // =========================================================================
        // ControlByte 빌드 / 파싱
        // =========================================================================

        /// <summary>
        /// 현재 DataLength, FrameType, ChecksumType 값으로 ControlByte 생성
        ///
        /// 비트 구성:
        ///   bit[7:4] = DataLength  → (DataLength &amp; 0x0F) &lt;&lt; 4
        ///   bit[3:1] = FrameType   → ((int)FrameType &amp; 0x07) &lt;&lt; 1
        ///   bit[0]   = Checksum    → (int)ChecksumType &amp; 0x01
        /// </summary>
        public byte BuildControlByte()
        {
            int dl   = (DataLength & 0x0F) << 4;         // 상위 4비트: DataLength
            int ft   = ((int)FrameType & 0x07) << 1;     // bit[3:1]: FrameType
            int cs   = (int)ChecksumType & 0x01;          // bit[0]: ChecksumType

            return (byte)(dl | ft | cs);
        }

        /// <summary>
        /// ControlByte 를 파싱하여 DataLength, FrameType, ChecksumType 추출
        /// </summary>
        public static void ParseControlByte(byte ctrl,
            out byte dataLen,
            out LINFrameType frameType,
            out LINChecksumType csType)
        {
            // 상위 4비트 → DataLength
            dataLen = (byte)((ctrl >> 4) & 0x0F);

            // bit[3:1] → FrameType (1=Publisher, 0=Subscriber)
            int ftBits = (ctrl >> 1) & 0x07;
            if (ftBits == 1)
            {
                frameType = LINFrameType.Publisher;
            }
            else
            {
                frameType = LINFrameType.Subscriber;
            }

            // bit[0] → ChecksumType (1=Enhanced, 0=Classic)
            if ((ctrl & 0x01) == 1)
            {
                csType = LINChecksumType.Enhanced;
            }
            else
            {
                csType = LINChecksumType.Classic;
            }
        }


        // =========================================================================
        // PEC 계산 (패킷 오류 검사)
        // =========================================================================

        /// <summary>
        /// IndieMicro 프로토콜의 PEC 계산
        /// PEC = (ControlByte + Data[0] + Data[1] + ... + Data[N-1]) &amp; 0xFF
        /// </summary>
        public static byte CalcPEC(byte controlByte, byte[] data, int dataLen)
        {
            int sum = controlByte;

            for (int i = 0; i < dataLen; i++)
            {
                sum += data[i];
            }

            // 하위 8비트만 사용
            return (byte)(sum & 0xFF);
        }


        // =========================================================================
        // LIN 버스 Checksum 계산 (LIN 규격)
        // =========================================================================

        /// <summary>
        /// Classic Checksum (LIN 1.x)
        /// Data 바이트만 포함 (ID 제외)
        /// </summary>
        public static byte CalcChecksumClassic(byte[] data, int len)
        {
            int s = 0;

            for (int i = 0; i < len; i++)
            {
                s += data[i];

                // 256 초과 시 캐리 처리 (inverted sum carry)
                if (s > 0xFF)
                {
                    s -= 0xFF;
                }
            }

            // 1의 보수
            return (byte)(0xFF - s);
        }

        /// <summary>
        /// Enhanced Checksum (LIN 2.x)
        /// PID(ID + parity) + Data 바이트 포함
        /// </summary>
        public static byte CalcChecksumEnhanced(byte pid, byte[] data, int len)
        {
            int s = pid;  // PID 포함

            for (int i = 0; i < len; i++)
            {
                s += data[i];

                if (s > 0xFF)
                {
                    s -= 0xFF;
                }
            }

            return (byte)(0xFF - s);
        }


        // =========================================================================
        // PID 계산 (LIN 2.x Protected ID)
        // =========================================================================

        /// <summary>
        /// LIN 2.x PID 계산 – Frame ID 에 Parity bit 2개 추가
        ///
        /// P0 = ID[0] XOR ID[1] XOR ID[2] XOR ID[4]
        /// P1 = NOT(ID[1] XOR ID[3] XOR ID[4] XOR ID[5])
        ///
        /// PID = ID[5:0] | P0[6] | P1[7]
        /// </summary>
        public static byte CalcPID(byte id)
        {
            id = (byte)(id & 0x3F);  // 상위 2비트 제거 (ID 는 6비트)

            int p0 = ((id >> 0) ^ (id >> 1) ^ (id >> 2) ^ (id >> 4)) & 1;
            int p1 = (~((id >> 1) ^ (id >> 3) ^ (id >> 4) ^ (id >> 5))) & 1;

            return (byte)(id | (p0 << 6) | (p1 << 7));
        }


        // =========================================================================
        // 유틸리티
        // =========================================================================

        /// <summary>
        /// Data 배열을 "FF 65 3A ..." 형식의 HEX 문자열로 변환
        /// DataLength 만큼만 출력
        /// </summary>
        public string DataToHexString()
        {
            string result = "";

            for (int i = 0; i < DataLength; i++)
            {
                result += Data[i].ToString("X2");

                // 마지막 바이트가 아니면 공백 추가
                if (i < DataLength - 1)
                {
                    result += " ";
                }
            }

            return result;
        }

        /// <summary>
        /// 로그 출력용 문자열 표현
        /// 예: [13:22:33.123] TX  ID=0x02  Type=Publisher   CS=Enhanced  Len=8  Data=[FF 65 3A ...]  PEC=0xAA
        /// </summary>
        public override string ToString()
        {
            string dir = (Direction == LINDirection.TX) ? "TX" : "RX";

            return string.Format(
                "[{0:HH:mm:ss.fff}] {1}  ID= 0x{2:X2}  Type={3,-10} CS={4,-8} Len={5}  Data=[ {6} ]  PEC=0x{7:X2}",
                Timestamp,
                dir,
                FrameID,
                FrameType.ToString(),
                ChecksumType.ToString(),
                DataLength,
                DataToHexString(),
                PEC);
        }
    }
}
