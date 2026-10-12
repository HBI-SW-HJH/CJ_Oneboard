using System;
using System.Collections.Generic;

namespace LINMaster.LIN
{
    // =========================================================================
    // LINProtocol – IndieMicro LIN Gateway V2.0 패킷 빌더 / 파서
    //
    // [역할]
    //   - LINFrame → 0x55 패킷(바이트 배열)으로 변환 (BuildSendPacket, BuildRequestPacket)
    //   - 수신된 바이트 배열 → LINFrame 으로 파싱 (TryParsePacket)
    //
    // [패킷 구조]  (PDF Chapter 6 기반)
    //
    //  ┌────────┬─────────┬─────┬─────────────┬────────────────┐
    //  │ Head   │ FrameID │ PEC │ ControlByte │ Data[0..N-1]   │
    //  │ (1byte)│ (1byte) │(1B) │   (1byte)   │   (0~8 byte)   │
    //  └────────┴─────────┴─────┴─────────────┴────────────────┘
    //
    //  Head:
    //    0x55 = SEND    (PC → Gateway)
    //    0xA5 = ACK/RSP (Gateway → PC)
    //
    //  PEC = (ControlByte + Data[0] + ... + Data[N-1]) & 0xFF
    // =========================================================================
    public static class LINProtocol
    {
        // ── 상수 ──────────────────────────────────────────────────────────
        public const byte HEAD_SEND    = 0x55;  // PC → Gateway (전송/요청)
        public const byte HEAD_ACK_RSP = 0xA5;  // Gateway → PC (응답)


        // =========================================================================
        // TX 패킷 빌드
        // =========================================================================

        /// <summary>
        /// Publisher SEND 패킷 생성
        /// PC → Gateway → LIN 버스 → Slave 에게 데이터 전송
        ///
        /// 패킷: [0x55][FrameID][PEC][ControlByte(Type=Publisher)][Data[0]...[N-1]]
        /// </summary>
        public static byte[] BuildSendPacket(LINFrame frame)
        {
            // FrameType 강제 설정
            frame.FrameType = LINFrameType.Publisher;

            // ControlByte 계산 (DataLen, FrameType, ChecksumType 포함)
            byte ctrlByte = frame.BuildControlByte();

            // PEC 계산 = (ControlByte + 모든 Data 합산) & 0xFF
            byte pec  = LINFrame.CalcPEC(ctrlByte, frame.Data, frame.DataLength);
            frame.PEC = pec;

            // 패킷 조립
            List<byte> pkt = new List<byte>();
            pkt.Add(HEAD_SEND);     // Head: 0x55
            pkt.Add(frame.FrameID); // Frame ID
            pkt.Add(pec);           // PEC
            pkt.Add(ctrlByte);      // ControlByte

            // 데이터 바이트 추가 (DataLength 만큼)
            for (int i = 0; i < frame.DataLength; i++)
            {
                pkt.Add(frame.Data[i]);
            }

            return pkt.ToArray();
        }

        /// <summary>
        /// Subscriber Request 패킷 생성
        /// PC → Gateway → LIN 버스 → Slave 에게 데이터 응답 요청
        ///
        /// 패킷: [0x55][FrameID][PEC][ControlByte(Type=Subscriber)][0xFF * DataLen]
        /// Data 는 0xFF 로 채워서 전송 (실제 값은 Slave 응답으로 수신)
        /// </summary>
        public static byte[] BuildRequestPacket(byte frameID, byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced)
        {
            // 임시 LINFrame 생성 (패킷 계산용)
            LINFrame frame = new LINFrame();
            frame.FrameID      = frameID;
            frame.DataLength   = dataLen;
            frame.FrameType    = LINFrameType.Subscriber;
            frame.ChecksumType = csType;

            // Data 를 0xFF 로 채움 (요청 패킷의 관례)
            for (int i = 0; i < dataLen; i++)
            {
                frame.Data[i] = 0xFF;
            }

            // ControlByte 와 PEC 계산
            byte ctrlByte = frame.BuildControlByte();
            byte pec      = LINFrame.CalcPEC(ctrlByte, frame.Data, dataLen);
            frame.PEC     = pec;

            // 패킷 조립
            List<byte> pkt = new List<byte>();
            pkt.Add(HEAD_SEND);  // Head: 0x55
            pkt.Add(frameID);    // Frame ID
            pkt.Add(pec);        // PEC
            pkt.Add(ctrlByte);   // ControlByte

            // 0xFF * DataLen 추가
            for (int i = 0; i < dataLen; i++)
            {
                pkt.Add(0xFF);
            }

            return pkt.ToArray();
        }


        // =========================================================================
        // RX 패킷 파싱
        // =========================================================================

        /// <summary>
        /// 수신 바이트 배열에서 LINFrame 하나를 파싱
        ///
        /// offset    : raw 배열에서 파싱 시작 위치
        /// available : offset 부터 사용 가능한 바이트 수
        /// frame     : 파싱된 LINFrame (성공 시)
        /// consumed  : 파싱에 사용된 바이트 수 (성공 시)
        ///
        /// 반환값: 파싱 성공이면 true, 데이터 부족이면 false
        /// </summary>
        public static bool TryParsePacket(byte[] raw, int offset, int available,
            out LINFrame frame, out int consumed)
        {
            frame    = null;
            consumed = 0;

            // 최소 패킷 크기: Head(1) + FrameID(1) + PEC(1) + ControlByte(1) = 4바이트
            if (available < 4)
            {
                return false;
            }

            // Head 확인 (0x55 또는 0xA5 만 허용)
            byte head = raw[offset];
            if (head != HEAD_SEND && head != HEAD_ACK_RSP)
            {
                return false;
            }

            // 각 필드 읽기
            byte fid      = raw[offset + 1];  // Frame ID
            byte pecRx    = raw[offset + 2];  // 수신된 PEC
            byte ctrlByte = raw[offset + 3];  // ControlByte

            // ControlByte 파싱 → DataLength, FrameType, ChecksumType 추출
            byte dataLen;
            LINFrameType    frameType;
            LINChecksumType csType;
            LINFrame.ParseControlByte(ctrlByte, out dataLen, out frameType, out csType);

            // DataLength 범위 확인 (0~8)
            if (dataLen > 8)
            {
                return false;
            }

            // 전체 패킷 길이 = 헤더 4바이트 + Data N바이트
            int totalLen = 4 + dataLen;

            // 데이터가 충분하지 않으면 더 기다림
            if (available < totalLen)
            {
                return false;
            }

            // Data 바이트 추출
            byte[] data = new byte[8];
            for (int i = 0; i < dataLen; i++)
            {
                data[i] = raw[offset + 4 + i];
            }

            // PEC 검증
            byte pecCalc = LINFrame.CalcPEC(ctrlByte, data, dataLen);
            bool pecOk   = (pecCalc == pecRx);

            // LINFrame 구성
            frame              = new LINFrame();
            frame.Direction    = LINDirection.RX;
            frame.IsACK        = (head == HEAD_ACK_RSP);  // 0xA5 이면 ACK/RSP
            frame.FrameID      = (byte)(fid & 0x3F);       // 상위 2비트 제거
            frame.PEC          = pecRx;
            frame.DataLength   = dataLen;
            frame.FrameType    = frameType;
            frame.ChecksumType = csType;
            frame.Data         = data;
            frame.IsValid      = pecOk;                    // PEC 일치 여부

            consumed = totalLen;
            return true;
        }
    }


    // =========================================================================
    // LINStreamParser – 스트리밍 수신 버퍼 파서
    //
    // [역할]
    //   COM 포트로 들어오는 바이트는 패킷 경계와 상관없이 도착할 수 있음
    //   → Feed() 로 받은 바이트를 내부 버퍼에 쌓고
    //   → Head(0x55/0xA5)를 찾아 완성된 패킷 단위로 LINFrame 으로 변환
    //   → 완성된 프레임은 FrameReceived 이벤트로 전달
    // =========================================================================
    public class LINStreamParser
    {
        // 내부 수신 버퍼 (바이트가 쌓이면서 패킷 단위로 파싱)
        private readonly List<byte> _buf = new List<byte>();

        /// <summary>완성된 LINFrame 수신 시 발생</summary>
        public event Action<LINFrame> FrameReceived;

        /// <summary>PEC 오류 또는 쓰레기 바이트 발견 시 메시지 전달</summary>
        public event Action<string> ParseError;


        /// <summary>
        /// 수신된 바이트 배열을 버퍼에 추가하고 파싱 시도
        /// RxLoop 에서 매 수신마다 호출
        /// </summary>
        public void Feed(byte[] data)
        {
            _buf.AddRange(data);
            Parse();
        }

        /// <summary>버퍼 초기화 (연결 시 이전 데이터 제거)</summary>
        public void Clear()
        {
            _buf.Clear();
        }

        /// <summary>
        /// 버퍼에서 완성된 패킷을 반복적으로 파싱
        /// 최소 4바이트(헤더)가 있어야 파싱 시도
        /// </summary>
        private void Parse()
        {
            while (_buf.Count >= 4)
            {
                // ── Step 1: Head 바이트(0x55 또는 0xA5) 탐색 ──────────────
                int headIdx = -1;

                for (int i = 0; i < _buf.Count; i++)
                {
                    if (_buf[i] == LINProtocol.HEAD_SEND || _buf[i] == LINProtocol.HEAD_ACK_RSP)
                    {
                        headIdx = i;
                        break;
                    }
                }

                // Head 를 찾지 못하면 버퍼 전체가 쓰레기 → 비움
                if (headIdx < 0)
                {
                    _buf.Clear();
                    break;
                }

                // Head 앞에 쓰레기 바이트가 있으면 제거
                if (headIdx > 0)
                {
                    ParseError?.Invoke(
                        string.Format("버퍼 {0}바이트 버림 (Head 탐색)", headIdx));

                    _buf.RemoveRange(0, headIdx);
                }

                // Head 이후 4바이트 미만이면 더 데이터 기다림
                if (_buf.Count < 4)
                {
                    break;
                }

                // ── Step 2: ControlByte 에서 DataLength 파악 ──────────────
                byte ctrlByte = _buf[3];
                byte dataLen;
                LINFrameType    ft;
                LINChecksumType ct;
                LINFrame.ParseControlByte(ctrlByte, out dataLen, out ft, out ct);

                // DataLength 가 8 초과이면 잘못된 패킷 → 1바이트 넘기고 재시도
                if (dataLen > 8)
                {
                    _buf.RemoveAt(0);
                    continue;
                }

                // ── Step 3: 패킷 전체가 버퍼에 들어올 때까지 대기 ──────────
                int need = 4 + dataLen;  // 헤더 4바이트 + 데이터 N바이트
                if (_buf.Count < need)
                {
                    break;  // 아직 데이터 부족 → 다음 Feed 호출까지 대기
                }

                // ── Step 4: 파싱 시도 ──────────────────────────────────────
                byte[]   raw = _buf.ToArray();
                LINFrame frame;
                int      consumed;

                if (LINProtocol.TryParsePacket(raw, 0, _buf.Count, out frame, out consumed))
                {
                    // PEC 오류가 있어도 프레임은 전달 (오류 플래그 포함)
                    if (!frame.IsValid)
                    {
                        ParseError?.Invoke(
                            string.Format("PEC 오류 ID=0x{0:X2}", frame.FrameID));
                    }

                    // 완성된 프레임 이벤트 발생
                    FrameReceived?.Invoke(frame);

                    // 사용한 바이트 제거
                    _buf.RemoveRange(0, consumed);
                }
                else
                {
                    // 파싱 실패 → 1바이트 넘기고 재시도
                    _buf.RemoveAt(0);
                }
            }
        }
    }
}
