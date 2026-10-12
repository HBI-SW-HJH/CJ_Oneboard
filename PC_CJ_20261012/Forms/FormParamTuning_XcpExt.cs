using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using LINMaster.LIN;

// ============================================================================
// FormParamTuning – XCP 확장 기능 (partial class)
//
// [추가 기능]
//  1. DAQ (Data Acquisition) – 실시간 주기적 데이터 수신
//     - SET_DAQ_PTR / WRITE_DAQ / SET_DAQ_LIST_MODE / START_STOP_DAQ_LIST
//     - START_STOP_SYNCH 로 일괄 시작/정지
//     - 결과를 dgvDaq DataGridView 에 실시간 표시
//
//  2. Checksum (BUILD_CHECKSUM)
//     - XCP Part 2 spec: ADD_BYTE / ADD_WORD / ADD_DWORD /
//       CRC_16 / CRC_16_CITT / CRC_32 / CRC_32P4 / USER_DEFINED
//     - 주소·길이를 지정하면 슬레이브가 계산한 체크섬을 반환
//
//  3. Seed/Key (GET_SEED / UNLOCK)
//     - resource: MEM(0x02) / PGM(0x10) / DAQ(0x04) 선택 가능
//     - 기본 Key 알고리즘: KEY = ~SEED  (가장 범용)
//     - 사용자 정의 알고리즘으로 교체 가능 (ComputeKey 메서드)
// ============================================================================

namespace LINMaster.Forms
{
    public partial class FormParamTuning : Form
    {
        // ====================================================================
        // XCP 추가 커맨드 상수
        // ====================================================================
        private const byte XCP_CMD_BUILD_CHECKSUM      = 0xF3;
        private const byte XCP_CMD_GET_SEED            = 0xF8;
        private const byte XCP_CMD_UNLOCK              = 0xF7;
        private const byte XCP_CMD_SET_DAQ_PTR         = 0xE2;
        private const byte XCP_CMD_WRITE_DAQ           = 0xE1;
        private const byte XCP_CMD_SET_DAQ_LIST_MODE   = 0xE0;
        private const byte XCP_CMD_START_STOP_DAQ_LIST = 0xDE;
        private const byte XCP_CMD_START_STOP_SYNCH    = 0xDD;
        private const byte XCP_CMD_GET_DAQ_CLOCK       = 0xDC;
        private const byte XCP_CMD_READ_DAQ            = 0xDB;
        private const byte XCP_CMD_CLEAR_DAQ_LIST      = 0xE3;
        private const byte XCP_CMD_GET_DAQ_LIST_INFO   = 0xDA;

        // Seed/Key Resource Byte (XCP Part 2, Table 9)
        private const byte XCP_RESOURCE_CAL_PAG = 0x01;  // Calibration / Parameter
        private const byte XCP_RESOURCE_DAQ     = 0x04;  // DAQ
        private const byte XCP_RESOURCE_STIM    = 0x08;  // Stimulation
        private const byte XCP_RESOURCE_PGM     = 0x10;  // Flash Programming

        // Checksum Type (XCP Part 2, Table 10)
        private const byte XCP_CS_ADD_11  = 0x01;  // ADD_11 byte sum
        private const byte XCP_CS_ADD_12  = 0x02;  // ADD_12
        private const byte XCP_CS_ADD_14  = 0x03;  // ADD_14
        private const byte XCP_CS_ADD_22  = 0x04;  // ADD_22
        private const byte XCP_CS_ADD_24  = 0x05;  // ADD_24
        private const byte XCP_CS_ADD_44  = 0x06;  // ADD_44
        private const byte XCP_CS_CRC_16  = 0x07;  // CRC-16
        private const byte XCP_CS_CRC_16_CITT = 0x08;
        private const byte XCP_CS_CRC_32  = 0x09;  // CRC-32
        private const byte XCP_CS_USER    = 0xFF;  // User defined

        // ====================================================================
        // DAQ 멤버
        // ====================================================================
        private System.Windows.Forms.Timer  _daqTimer;
        private List<DaqEntry>              _daqEntries = new List<DaqEntry>();
        private bool                        _daqRunning  = false;

        // ====================================================================
        // Seed/Key 멤버
        // ====================================================================
        private byte[] _lastSeed = null;
#pragma warning disable CS0414
        private bool                        _daqInitialized = false;
#pragma warning restore CS0414


        // ====================================================================
        // DAQ Entry 클래스
        // ====================================================================
        private class DaqEntry
        {
            public MapSymbol Symbol   { get; set; }
            public string    LastVal  { get; set; } = "--";
            public int       OdtIdx  { get; set; }   // ODT 인덱스 (0-based)
            public int       EntryIdx{ get; set; }   // ODT 내 엔트리
        }

        // ====================================================================
        // 초기화 – FormParamTuning 생성자에서 호출할 것

        // ====================================================================
        // DAQ 이벤트 핸들러
        // ====================================================================
        private void BtnDaqAdd_Click(object sender, EventArgs e)
        {
            var s = GetSelectedSymbol();
            if (s == null) { SetStatus("변수를 선택하세요"); return; }
            if (_daqEntries.Any(d => d.Symbol.Address == s.Address && d.Symbol.Name == s.Name))
            { SetStatus("이미 DAQ 목록에 있습니다"); return; }

            int odtIdx   = _daqEntries.Count;   // 간단히 1변수 = 1 ODT
            int entryIdx = 0;

            var entry = new DaqEntry { Symbol = s, OdtIdx = odtIdx, EntryIdx = entryIdx };
            _daqEntries.Add(entry);

            int row = dgvDaq.Rows.Add(s.Name, s.AddressHex, s.SizeStr, "--");
            dgvDaq.Rows[row].Tag = entry;
            SetStatus($"DAQ 추가: {s.Name}  ({_daqEntries.Count}개)");
        }

        private void BtnDaqRemove_Click(object sender, EventArgs e)
        {
            if (dgvDaq.SelectedRows.Count == 0) return;
            var row = dgvDaq.SelectedRows[0];
            if (row.Tag is DaqEntry de) _daqEntries.Remove(de);
            dgvDaq.Rows.Remove(row);
            _daqInitialized = false;
        }

        private void BtnDaqStart_Click(object sender, EventArgs e)
        {
            if (_gw == null || !_gw.IsConnected) { SetStatus("연결 안됨"); return; }
            if (!UseXcpMode) { SetStatus("XCP 모드를 활성화하세요"); return; }
            if (_daqEntries.Count == 0) { SetStatus("DAQ 목록이 비어 있습니다"); return; }

            if (!EnsureXcpConnected()) return;

            // ── DAQ 초기화 시퀀스 ──────────────────────────────────
            if (!InitDaqSequence()) return;

            // ── START_STOP_SYNCH: 모두 시작 ───────────────────────
            // 0x00 = START (모든 선택된 DAQ list)
            if (!XcpStartStopSynch(0x00))
            {
                SetStatus("DAQ START 실패"); return;
            }

            _daqRunning           = true;
            _daqTimer.Interval    = (int)nudGraphInterval.Value;
            _daqTimer.Start();

            lblDaqStatus.Text      = $"▶ DAQ 수집 중  (주기: {_daqTimer.Interval}ms)";
            lblDaqStatus.ForeColor = Color.Lime;
            SetStatus("DAQ 시작");
        }

        private void BtnDaqStop_Click(object sender, EventArgs e)
        {
            StopDaq();
            SetStatus("DAQ 정지");
        }

        // ── 정지 공통 처리 ──────────────────────────────────────────────
        private void StopDaq()
        {
            _daqTimer.Stop();
            _daqRunning = false;

            if (_gw?.IsConnected == true && UseXcpMode)
            {
                // START_STOP_SYNCH: 0x01 = STOP all
                XcpStartStopSynch(0x01);
            }

            lblDaqStatus.Text      = "■ 정지됨";
            lblDaqStatus.ForeColor = Color.OrangeRed;
        }

        // ── DAQ Timer Tick ──────────────────────────────────────────────
        private void DaqTimer_Tick(object sender, EventArgs e)
        {
            if (!_daqRunning || _gw == null || !_gw.IsConnected) { StopDaq(); return; }

            foreach (DataGridViewRow row in dgvDaq.Rows)
            {
                if (!(row.Tag is DaqEntry de)) continue;
                var s = de.Symbol;

                // SHORT_UPLOAD 으로 폴링 (슬레이브가 ODT 자동 전송 미지원 시 대체)
                if (XcpShortUpload(s.Address, s.Size, out uint val, out string hex))
                {
                    row.Cells["daqValue"].Value = hex;
                    de.LastVal = hex;
                }
                else
                {
                    row.Cells["daqValue"].Value = "ERR";
                }
                Application.DoEvents();
            }
        }

        // ====================================================================
        // DAQ 초기화 시퀀스  (XCP Part 2, Section 7)
        // DAQ list 0 에 모든 변수를 ODT entry로 등록
        // ====================================================================
        private bool InitDaqSequence()
        {
            // 1) CLEAR_DAQ_LIST – DAQ list 0 초기화
            var clearCmd = new byte[8];
            clearCmd[0] = XCP_CMD_CLEAR_DAQ_LIST;
            clearCmd[1] = 0x00;   // reserved
            clearCmd[2] = 0x00;   // DAQ list number low
            clearCmd[3] = 0x00;   // DAQ list number high
            var clrRx = SendXcpCtoWithRetry(clearCmd);
            if (!IsPositiveXcpResponse(clrRx, out string clrErr))
            {
                SetStatus($"CLEAR_DAQ_LIST 실패: {clrErr}");
                return false;
            }

            // 2) 각 변수를 ODT 엔트리로 등록
            for (int i = 0; i < _daqEntries.Count; i++)
            {
                var de = _daqEntries[i];
                var s  = de.Symbol;

                // SET_DAQ_PTR  [CMD][0x00][DAQ_L][DAQ_H][ODT_NUM][ODT_ENTRY]
                var ptrCmd = new byte[8];
                ptrCmd[0] = XCP_CMD_SET_DAQ_PTR;
                ptrCmd[1] = 0x00;
                ptrCmd[2] = 0x00;   // DAQ list 0 (low)
                ptrCmd[3] = 0x00;   // DAQ list 0 (high)
                ptrCmd[4] = (byte)i;   // ODT index
                ptrCmd[5] = 0x00;      // ODT entry 0
                var ptrRx = SendXcpCtoWithRetry(ptrCmd);
                if (!IsPositiveXcpResponse(ptrRx, out string ptrErr))
                {
                    SetStatus($"SET_DAQ_PTR 실패 (변수 {i}): {ptrErr}"); return false;
                }

                // WRITE_DAQ  [CMD][BitOffset][SizeByte][AddrExt][Addr0..3]
                int sz = Math.Max(1, Math.Min(s.Size > 0 ? s.Size : 1, 4));
                var wdCmd = new byte[8];
                wdCmd[0] = XCP_CMD_WRITE_DAQ;
                wdCmd[1] = 0xFF;             // bit offset = 0xFF (byte aligned)
                wdCmd[2] = (byte)sz;
                wdCmd[3] = GetXcpAddrExt();
                wdCmd[4] = (byte)(s.Address & 0xFF);
                wdCmd[5] = (byte)((s.Address >> 8)  & 0xFF);
                wdCmd[6] = (byte)((s.Address >> 16) & 0xFF);
                wdCmd[7] = (byte)((s.Address >> 24) & 0xFF);
                var wdRx = SendXcpCtoWithRetry(wdCmd);
                if (!IsPositiveXcpResponse(wdRx, out string wdErr))
                {
                    SetStatus($"WRITE_DAQ 실패 ({s.Name}): {wdErr}"); return false;
                }
            }

            // 3) SET_DAQ_LIST_MODE  [CMD][Mode][DAQ_L][DAQ_H][EventCh][Prescaler][Priority]
            //    Mode = 0x10: timestamp 활성화 | 0x00: 기본
            var modeCmd = new byte[8];
            modeCmd[0] = XCP_CMD_SET_DAQ_LIST_MODE;
            modeCmd[1] = 0x00;   // Mode: basic
            modeCmd[2] = 0x00;   // DAQ list 0 low
            modeCmd[3] = 0x00;   // DAQ list 0 high
            modeCmd[4] = 0x00;   // Event channel 0
            modeCmd[5] = 0x01;   // Prescaler = 1
            modeCmd[6] = 0x00;   // Priority
            var modeRx = SendXcpCtoWithRetry(modeCmd);
            if (!IsPositiveXcpResponse(modeRx, out string modeErr))
            {
                SetStatus($"SET_DAQ_LIST_MODE 실패: {modeErr}"); return false;
            }

            // 4) START_STOP_DAQ_LIST  [CMD][Mode=0x02 SELECT][DAQ_L][DAQ_H]
            //    Mode 0x02 = SELECT (나중에 SYNCH 로 일괄 시작)
            var ssCmd = new byte[8];
            ssCmd[0] = XCP_CMD_START_STOP_DAQ_LIST;
            ssCmd[1] = 0x02;   // SELECT
            ssCmd[2] = 0x00;   // DAQ 0
            ssCmd[3] = 0x00;
            var ssRx = SendXcpCtoWithRetry(ssCmd);
            if (!IsPositiveXcpResponse(ssRx, out string ssErr))
            {
                SetStatus($"START_STOP_DAQ_LIST 실패: {ssErr}"); return false;
            }

            _daqInitialized = true;
            return true;
        }

        // START_STOP_SYNCH  (mode: 0x00=START, 0x01=STOP_ALL, 0x02=PREPARE)
        private bool XcpStartStopSynch(byte mode)
        {
            var cmd = new byte[8];
            cmd[0] = XCP_CMD_START_STOP_SYNCH;
            cmd[1] = mode;
            var rx = SendXcpCtoWithRetry(cmd);
            return IsPositiveXcpResponse(rx, out _);
        }

        // ====================================================================
        // Checksum 이벤트 핸들러
        // ====================================================================
        private void BtnCalcChecksum_Click(object sender, EventArgs e)
        {
            if (_gw == null || !_gw.IsConnected) { SetStatus("연결 안됨"); return; }
            if (!UseXcpMode) { SetStatus("XCP 모드를 활성화하세요"); return; }
            if (!EnsureXcpConnected()) return;

            // 주소 파싱
            string addrStr = txtCsAddress.Text.Trim();
            if (!TryParseHexOrDec(addrStr, out uint address))
            { SetStatus("주소 파싱 오류"); return; }

            // 길이 파싱
            if (!uint.TryParse(txtCsLength.Text.Trim(), out uint blockLen) || blockLen == 0)
            { SetStatus("길이 파싱 오류"); return; }

            // 체크섬 타입
            byte csType = GetChecksumTypeByte();

            // SET_MTA 로 주소 설정
            var setMta = new byte[8];
            setMta[0] = XCP_CMD_SET_MTA;
            setMta[2] = 0x00;
            setMta[3] = GetXcpAddrExt();
            setMta[4] = (byte)(address & 0xFF);
            setMta[5] = (byte)((address >> 8)  & 0xFF);
            setMta[6] = (byte)((address >> 16) & 0xFF);
            setMta[7] = (byte)((address >> 24) & 0xFF);
            var mtaRx = SendXcpCtoWithRetry(setMta);
            if (!IsPositiveXcpResponse(mtaRx, out string mtaErr))
            { SetStatus($"SET_MTA 실패: {mtaErr}"); return; }

            // BUILD_CHECKSUM  [CMD][0x00][0x00][0x00][Len0..3]
            var csCmd = new byte[8];
            csCmd[0] = XCP_CMD_BUILD_CHECKSUM;
            csCmd[1] = 0x00;
            csCmd[2] = 0x00;
            csCmd[3] = 0x00;
            csCmd[4] = (byte)(blockLen & 0xFF);
            csCmd[5] = (byte)((blockLen >> 8)  & 0xFF);
            csCmd[6] = (byte)((blockLen >> 16) & 0xFF);
            csCmd[7] = (byte)((blockLen >> 24) & 0xFF);

            SetStatus("BUILD_CHECKSUM 요청 중...");
            // 슬레이브 계산 시간이 길 수 있으므로 타임아웃 2배
            var csRx = SendXcpCtoWithRetry(csCmd);
            if (!IsPositiveXcpResponse(csRx, out string csErr))
            { SetStatus($"BUILD_CHECKSUM 실패: {csErr}"); lblCsResult.Text = "결과: 오류"; return; }

            // 응답: [0xFF][0x00][0x00][ChecksumType][Checksum 0..3]
            byte rxType = csRx.DataLength > 3 ? csRx.Data[3] : (byte)0;
            uint result = 0;
            if (csRx.DataLength >= 8)
                result = (uint)(csRx.Data[4] | csRx.Data[5] << 8 | csRx.Data[6] << 16 | csRx.Data[7] << 24);
            else if (csRx.DataLength >= 6)
                result = (uint)(csRx.Data[4] | csRx.Data[5] << 8);
            else if (csRx.DataLength >= 5)
                result = csRx.Data[4];

            string typeName = ChecksumTypeName(rxType);
            lblCsResult.Text = $"결과: 0x{result:X8}  (타입: {typeName}, 주소: 0x{address:X8}, 길이: {blockLen}bytes)";
            SetStatus($"체크섬 완료: 0x{result:X8}");
        }

        private byte GetChecksumTypeByte()
        {
            // 콤보 인덱스 → 타입 바이트
            byte[] map = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF };
            int idx = cmbCsType?.SelectedIndex ?? 6;
            return (idx >= 0 && idx < map.Length) ? map[idx] : (byte)0x07;
        }

        private static string ChecksumTypeName(byte t)
        {
            switch (t)
            {
                case 0x01: return "ADD_11";
                case 0x02: return "ADD_12";
                case 0x03: return "ADD_14";
                case 0x04: return "ADD_22";
                case 0x05: return "ADD_24";
                case 0x06: return "ADD_44";
                case 0x07: return "CRC_16";
                case 0x08: return "CRC_16_CITT";
                case 0x09: return "CRC_32";
                case 0xFF: return "USER";
                default:   return $"0x{t:X2}";
            }
        }

        // ====================================================================
        // Seed/Key 이벤트 핸들러
        // ====================================================================
        private void BtnGetSeed_Click(object sender, EventArgs e)
        {
            if (_gw == null || !_gw.IsConnected) { SetStatus("연결 안됨"); return; }
            if (!UseXcpMode) { SetStatus("XCP 모드를 활성화하세요"); return; }
            if (!EnsureXcpConnected()) return;

            byte resource = GetSelectedResource();

            // GET_SEED  [CMD][Mode=0x00=First][Resource]
            var cmd = new byte[8];
            cmd[0] = XCP_CMD_GET_SEED;
            cmd[1] = 0x00;      // Mode 0 = first request
            cmd[2] = resource;

            var rx = SendXcpCtoWithRetry(cmd);
            if (!IsPositiveXcpResponse(rx, out string err))
            {
                lblSkStatus.Text      = $"GET_SEED 실패: {err}";
                lblSkStatus.ForeColor = Color.OrangeRed;
                SetStatus($"Seed 획득 실패: {err}");
                return;
            }

            // 응답: [0xFF][Length][Seed0][Seed1]...[SeedN]
            int seedLen = rx.DataLength > 1 ? rx.Data[1] : 0;
            seedLen = Math.Min(seedLen, rx.DataLength - 2);
            _lastSeed = new byte[seedLen];
            for (int i = 0; i < seedLen; i++)
                _lastSeed[i] = (i + 2 < rx.DataLength) ? rx.Data[i + 2] : (byte)0;

            string seedHex = BitConverter.ToString(_lastSeed).Replace("-", " ");
            lblSeedVal.Text      = $"Seed: {seedHex}";
            lblSeedVal.ForeColor = Color.Yellow;

            // Key 계산
            byte[] key = ComputeKey(_lastSeed, resource);
            string keyHex = BitConverter.ToString(key).Replace("-", " ");
            lblKeyVal.Text      = $"Key:  {keyHex}";
            lblKeyVal.ForeColor = Color.Lime;

            btnUnlock.Enabled      = true;
            lblSkStatus.Text       = "Seed 수신 완료 – UNLOCK 버튼을 눌러 잠금 해제";
            lblSkStatus.ForeColor  = Color.Silver;
            SetStatus($"Seed 수신: {seedHex}");
        }

        private void BtnUnlock_Click(object sender, EventArgs e)
        {
            if (_lastSeed == null || _lastSeed.Length == 0) { SetStatus("먼저 GET_SEED를 실행하세요"); return; }
            if (_gw == null || !_gw.IsConnected) { SetStatus("연결 안됨"); return; }

            byte resource = GetSelectedResource();
            byte[] key    = ComputeKey(_lastSeed, resource);

            // UNLOCK  [CMD][Length][Key0][Key1]...[KeyN]  (최대 6바이트 Key)
            int sendLen = Math.Min(key.Length, 6);
            var cmd = new byte[8];
            cmd[0] = XCP_CMD_UNLOCK;
            cmd[1] = (byte)sendLen;
            for (int i = 0; i < sendLen; i++)
                cmd[2 + i] = key[i];

            var rx = SendXcpCtoWithRetry(cmd);
            if (!IsPositiveXcpResponse(rx, out string err))
            {
                lblSkStatus.Text      = $"UNLOCK 실패: {err}";
                lblSkStatus.ForeColor = Color.OrangeRed;
                SetStatus($"UNLOCK 실패: {err}");
                return;
            }

            // 응답 [0xFF][CurrentResourceProtection]
            byte prot = rx.DataLength > 1 ? rx.Data[1] : (byte)0xFF;
            bool unlocked = (prot & resource) == 0;

            lblSkStatus.Text      = unlocked
                ? $"✔ UNLOCK 성공  (보호 해제 resource: 0x{resource:X2})"
                : $"⚠ UNLOCK 응답 OK, 그러나 보호 상태: 0x{prot:X2}";
            lblSkStatus.ForeColor = unlocked ? Color.Lime : Color.Orange;
            btnUnlock.Enabled     = false;
            _lastSeed             = null;
            SetStatus(unlocked ? "UNLOCK 성공" : "UNLOCK 경고: 보호 상태 확인 필요");
        }

        // ── Key 계산 알고리즘 ──────────────────────────────────────────────
        // 가장 범용: KEY = bitwise NOT of SEED
        // 필요 시 이 메서드만 교체하면 됩니다.
        private byte[] ComputeKey(byte[] seed, byte resource)
        {
            byte[] key = new byte[seed.Length];
            for (int i = 0; i < seed.Length; i++)
                key[i] = (byte)(~seed[i]);
            return key;

            // ── 대체 예시 ──────────────────────────────────
            // XOR 고정값: for (int i = 0; i < seed.Length; i++) key[i] = (byte)(seed[i] ^ 0xA5);
            // SHIFT      : for (int i = 0; i < seed.Length; i++) key[i] = (byte)((seed[i] << 1) | (seed[i] >> 7));
        }

        // ── Resource 선택 ────────────────────────────────────────────────
        private byte GetSelectedResource()
        {
            int idx = cmbSkResource?.SelectedIndex ?? 0;
            byte[] res = { XCP_RESOURCE_CAL_PAG, XCP_RESOURCE_DAQ, XCP_RESOURCE_STIM, XCP_RESOURCE_PGM };
            return (idx >= 0 && idx < res.Length) ? res[idx] : XCP_RESOURCE_CAL_PAG;
        }

        // ====================================================================
        // 헬퍼
        // ====================================================================

        /// <summary>
        /// "0x1A2B" 형식(16진수) 또는 "1234" 형식(10진수) 문자열을 uint로 파싱합니다.
        /// </summary>
        private static bool TryParseHexOrDec(string s, out uint result)
        {
            result = 0;
            if (string.IsNullOrWhiteSpace(s)) return false;
            s = s.Trim();
            if (s.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ||
                s.StartsWith("0X", StringComparison.OrdinalIgnoreCase))
            {
                return uint.TryParse(s.Substring(2),
                    System.Globalization.NumberStyles.HexNumber,
                    System.Globalization.CultureInfo.InvariantCulture,
                    out result);
            }
            return uint.TryParse(s,
                System.Globalization.NumberStyles.Integer,
                System.Globalization.CultureInfo.InvariantCulture,
                out result);
        }

    }
}
