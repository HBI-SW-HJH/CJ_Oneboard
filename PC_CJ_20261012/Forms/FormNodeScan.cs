using System;
using System.Collections.Generic;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;
using LINMaster.LIN;

namespace LINMaster.Forms
{
    // =========================================================================
    // FormNodeScan – MLM Node Scan / LED Control / Device / Calibration / Product Info
    //
    // [탭 구성]
    //   LED Ctrl    : SF 체크박스, Color/Bright/Fade 설정, LIN TX 전송
    //   Device      : LED Efficiency(0~7) GET/SET + Bright READ/WRITE + Device Config
    //   Calibration : RGB XYZ 캘리브레이션 데이터 Read/Write
    //   Product Info: NAD/MasterNAD/SlaveFlag/ColorTable 읽기 + Write(CMD 0xA0)
    //
    // [자동 스캔]
    //   생성자에서 연결 상태이면 1차(300ms) → 미응답 시 2차(500ms) 스캔 자동 실행
    //
    // [0x3D 재송신 정책]
    //   SendDiag() : 0x3C → 0x3D 전송 후 응답 대기 (1회)
    //   SendDiagWithRetry() : 무응답 시 0x3C 부터 최대 MAX_DIAG_RETRY 회 재시도
    //   모든 진단 전송은 SendDiagWithRetry() 를 사용할 것
    //
    // [프로토콜]
    //   0x3C Publisher  : Classic 체크섬
    //   0x3D Subscriber : Classic 체크섬
    //   Color Ctrl      : Enhanced 체크섬, FID = MasterNAD & 0x3F
    // =========================================================================
    public partial class FormNodeScan : Form
    {
        // ─────────────────────────────────────────────────────────────────
        // 상수
        // ─────────────────────────────────────────────────────────────────

        private const int MAX_DIAG_RETRY      = 2;    // 진단 프레임 최대 재시도 횟수
        private const int DIAG_TIMEOUT_MS     = 100;  // 0x3D 응답 타임아웃 (ms)
        private const int DIAG_RETRY_DELAY_MS = 20;   // 재시도 전 대기 시간 (ms)


        // ─────────────────────────────────────────────────────────────────
        // 멤버 변수
        // ─────────────────────────────────────────────────────────────────

        private readonly ILINGateway      _gw;
        private readonly bool             _isCjMode;
        private readonly List<SlaveInfo>  _slaves   = new List<SlaveInfo>();
        private SlaveInfo                 _selected = null;
        private bool                      _dragging = false;
        private Point                     _dragStart;
        private readonly Queue<LINFrame>  _rxQueue  = new Queue<LINFrame>();
        private readonly object           _rxLock   = new object();
        private FormCIE                   _cieWin;
        private System.Windows.Forms.Timer _ledTimer;

        // Calibration: [led 0~7][채널 0=R,1=G,2=B][0=X, 1=Y, 2=Bright]
        private readonly int[,,] _calibXYZ   = new int[8, 3, 3];
        private bool             _calibLoaded = false;


        // =========================================================================
        // 생성자 / 종료
        // =========================================================================

        public FormNodeScan(ILINGateway gw, bool isCjMode = false)
        {
            _gw = gw;
            _isCjMode = isCjMode;
            InitializeComponent();
            ConfigureCjMode();
            _gw.FrameReceived += OnGwFrame;

            if (!_gw.IsConnected) return;

            lblScanStatus.Text      = "자동 스캔 중...";
            lblScanStatus.ForeColor = Color.Yellow;
            Application.DoEvents();

            DoScanOnce(300);

            if (_slaves.Count == 0)
            {
                Delay(100);
                DoScanOnce(500);
            }

            if (_slaves.Count > 0)
            {
                lstSlaves.SelectedIndex = 0;
                lblScanStatus.Text      = string.Format("자동 스캔 완료 – {0}개", _slaves.Count);
                lblScanStatus.ForeColor = Color.LimeGreen;
            }
            else
            {
                lblScanStatus.Text      = "자동 스캔 – 응답 없음";
                lblScanStatus.ForeColor = Color.Gray;
            }
        }

        private void ConfigureCjMode()
        {
            if (!_isCjMode) return;

            Text = "CJ Node Scan / Chair ID";
            grpProdWrite.Text = "[ CJ Chair ID 저장 (0x3C/0x3D, Classic) ]";
            nudWrNAD.Minimum = 1;
            nudWrNAD.Maximum = 4;
            nudWrNAD.Value = 1;
            nudWrMstNAD.Value = 1;
            nudWrMstNAD.Enabled = false;
            nudWrSlaveFlag.Value = 1;
            nudWrSlaveFlag.Enabled = false;
            nudWrColorTbl.Value = 0;
            nudWrColorTbl.Enabled = false;
            nudWrNAD.ValueChanged += (sender, args) => UpdateCjWriteValues();
        }

        private void UpdateCjWriteValues()
        {
            if (!_isCjMode) return;

            int chairId = (int)nudWrNAD.Value;
            nudWrMstNAD.Value = 1;
            nudWrSlaveFlag.Value = 1 << (chairId - 1);
            nudWrColorTbl.Value = 0;
        }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            _gw.FrameReceived -= OnGwFrame;
            _ledTimer?.Stop();
            base.OnFormClosed(e);
        }


        // =========================================================================
        // RX 수신 처리
        // =========================================================================

        /// <summary>LINGateway 프레임 수신 → RX/0x3D/5바이트 이상인 경우만 큐에 저장</summary>
        private void OnGwFrame(LINFrame f)
        {
            if (f.Direction == LINDirection.RX && f.FrameID == 0x3D && f.DataLength >= 5)
            {
                lock (_rxLock) { _rxQueue.Enqueue(f); }
            }
        }

        /// <summary>RX 큐에서 timeoutMs 이내에 프레임을 꺼내 반환, 타임아웃 시 null</summary>
        private LINFrame WaitRx(int timeoutMs = DIAG_TIMEOUT_MS)
        {
            DateTime deadline = DateTime.Now.AddMilliseconds(timeoutMs);
            while (DateTime.Now < deadline)
            {
                Application.DoEvents();
                lock (_rxLock) { if (_rxQueue.Count > 0) return _rxQueue.Dequeue(); }
                Thread.Sleep(1);
            }
            return null;
        }

        /// <summary>ms 동안 UI DoEvents 를 유지하며 대기</summary>
        private static void Delay(int ms)
        {
            DateTime until = DateTime.Now.AddMilliseconds(ms);
            while (DateTime.Now < until) Application.DoEvents();
        }


        // =========================================================================
        // 진단 전송 헬퍼
        // =========================================================================

        /// <summary>
        /// 0x3C Publisher 전송 → 0x3D Subscriber 요청 → 응답 대기 (1회)
        /// 응답 없으면 null 반환. 일반적으로 SendDiagWithRetry() 를 사용할 것.
        /// </summary>
        private LINFrame SendDiag(byte nad, byte[] payload8)
        {
            lock (_rxLock) { _rxQueue.Clear(); }

            var f3C = new LINFrame
            {
                FrameID      = 0x3C,
                DataLength   = 8,
                ChecksumType = LINChecksumType.Classic
            };
            Array.Copy(payload8, f3C.Data, 8);
            _gw.SendPublisher(f3C);

            Delay(10);
            _gw.SendSubscriberRequest(0x3D, 8, LINChecksumType.Classic);

            return WaitRx(DIAG_TIMEOUT_MS);
        }

        /// <summary>
        /// 0x3D 응답 없으면 0x3C 부터 최대 MAX_DIAG_RETRY 회 재시도
        /// 모든 진단 전송은 이 메서드를 통할 것
        /// </summary>
        private LINFrame SendDiagWithRetry(byte nad, byte[] payload8)
        {
            for (int attempt = 0; attempt <= MAX_DIAG_RETRY; attempt++)
            {
                LINFrame rx = SendDiag(nad, payload8);
                if (rx != null && rx.DataLength >= 5) return rx;

                if (attempt < MAX_DIAG_RETRY) Delay(DIAG_RETRY_DELAY_MS);
            }
            return null;
        }

        /// <summary>FID 지정 Publisher 전송 (응답 수신 없음) – 멀티프레임 전송용</summary>
        private void SendLINFrame(byte fid, byte[] data8)
        {
            var f = new LINFrame
            {
                FrameID      = fid,
                DataLength   = 8,
                ChecksumType = LINChecksumType.Classic
            };
            Array.Copy(data8, f.Data, 8);
            _gw.SendPublisher(f);
            Delay(10);
        }


        // =========================================================================
        // 패킷 빌더 (정적 헬퍼)
        // =========================================================================

        // ReadByID:    [NAD][06][B2][ID][FF][7F][FF][FF]
        private static byte[] BuildReadById(byte nad, byte id)
            => new byte[] { nad, 0x06, 0xB2, id, 0xFF, 0x7F, 0xFF, 0xFF };

        // DataDump:    [NAD][05][B4][CMD][P0][P1][P2][P3]
        private static byte[] BuildDataDump(byte nad, byte cmd, byte p0, byte p1, byte p2, byte p3)
            => new byte[] { nad, 0x06, 0xB4, cmd, p0, p1, p2, p3 };

        // DiagPayload: [NAD][06][SID][ID][D0][D1][D2][D3]
        private static byte[] BuildDiagPayload(byte nad, byte sid, byte id,
            byte d0, byte d1, byte d2, byte d3)
            => new byte[] { nad, 0x06, sid, id, d0, d1, d2, d3 };

        private static decimal Clamp(byte v, NumericUpDown n)
            => Math.Max(n.Minimum, Math.Min(n.Maximum, v));

        private static decimal Clamp16(int v, NumericUpDown n)
            => Math.Max(n.Minimum, Math.Min(n.Maximum, v));


        // =========================================================================
        // Node Scan
        // =========================================================================

        /// <summary>Broadcast 스캔 공통 로직 – timeoutMs 동안 응답 수집</summary>
        private void DoScanOnce(int timeoutMs)
        {
            if (_isCjMode)
            {
                for (byte nad = 1; nad <= 4; nad++)
                {
                    LINFrame directed = SendDiag(nad, BuildReadById(nad, 0x3B));
                    if (directed != null && IsValidScanResponse(directed) &&
                        directed.Data[0] == nad && !_slaves.Exists(s => s.NAD == nad))
                    {
                        RegisterSlave(nad, directed);
                    }
                }
                return;
            }

            lock (_rxLock) { _rxQueue.Clear(); }

            var f3C = new LINFrame
            {
                FrameID      = 0x3C,
                DataLength   = 8,
                ChecksumType = LINChecksumType.Classic
            };
            Array.Copy(BuildReadById(0x7F, 0x3B), f3C.Data, 8);
            _gw.SendPublisher(f3C);

            Delay(10);
            _gw.SendSubscriberRequest(0x3D, 8, LINChecksumType.Classic);

            DateTime deadline = DateTime.Now.AddMilliseconds(timeoutMs);
            while (DateTime.Now < deadline)
            {
                Application.DoEvents();
                LINFrame rx = null;
                lock (_rxLock) { if (_rxQueue.Count > 0) rx = _rxQueue.Dequeue(); }

                if (rx != null && IsValidScanResponse(rx))
                {
                    byte nad = rx.Data[0];
                    if (!_slaves.Exists(s => s.NAD == nad))
                        RegisterSlave(nad, rx);
                }
                Thread.Sleep(1);
            }
        }

        /// <summary>스캔 응답 유효성 확인 (D1=0x06, D2=0xF2)</summary>
        private static bool IsValidScanResponse(LINFrame rx)
            => rx.DataLength >= 8 && rx.Data[1] == 0x06 && rx.Data[2] == 0xF2;

        /// <summary>슬레이브를 목록에 등록하고 추가 정보 읽기</summary>
        private void RegisterSlave(byte nad, LINFrame rx)
        {
            var info = new SlaveInfo
            {
                NAD        = nad,
                SlaveFlag  = rx.Data[3],
                MasterNAD  = rx.Data[4],
                TargetID   = rx.Data[5],
                ColorTable = rx.Data[6]
            };

            _slaves.Add(info);
            lstSlaves.Items.Add(string.Format("0x{0:X2}  {1}", nad, NadToName(nad)));
            lblScanStatus.Text = string.Format("발견: 0x{0:X2}", nad);
            Application.DoEvents();

            FetchExtraInfo(info);
        }

        private void btnScan_Click(object sender, EventArgs e)
        {
            if (!_gw.IsConnected) { MessageBox.Show("LIN Gateway 미연결", "알림"); return; }

            _slaves.Clear();
            lstSlaves.Items.Clear();
            ClearDevicePanel();

            btnScan.Enabled = false;
            SetStatus(lblScanStatus, "Broadcast 스캔 중...", Color.Yellow);
            Application.DoEvents();

            DoScanOnce(500);

            if (_slaves.Count > 0)
            {
                SetStatus(lblScanStatus, string.Format("완료 – {0}개", _slaves.Count), Color.LimeGreen);
                lstSlaves.SelectedIndex = 0;
            }
            else
            {
                SetStatus(lblScanStatus, "응답 없음. Add Node 사용", Color.OrangeRed);
            }

            btnScan.Enabled = true;
        }


        // =========================================================================
        // Add Node (수동 추가)
        // =========================================================================

        private void btnAddNode_Click(object sender, EventArgs e)
        {
            using (var dlg = new AddNodeDialog())
            {
                if (dlg.ShowDialog(this) != DialogResult.OK) return;

                byte nad = dlg.NAD;
                if (_slaves.Exists(s => s.NAD == nad))
                {
                    MessageBox.Show("이미 등록된 NAD입니다.");
                    return;
                }

                var info = new SlaveInfo { NAD = nad };

                if (_gw.IsConnected)
                {
                    SetStatus(lblScanStatus, string.Format("정보 읽는 중... 0x{0:X2}", nad), Color.Yellow);
                    Application.DoEvents();

                    var rx = SendDiagWithRetry(nad, BuildReadById(nad, 0x3B));
                    if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF2)
                    {
                        info.SlaveFlag  = rx.Data[3];
                        info.MasterNAD  = rx.Data[4];
                        info.TargetID   = rx.Data[5];
                        info.ColorTable = rx.Data[6];
                        FetchExtraInfo(info);
                    }
                }

                _slaves.Add(info);
                lstSlaves.Items.Add(string.Format("0x{0:X2}  {1}", nad, NadToName(nad)));
                lstSlaves.SelectedIndex = lstSlaves.Items.Count - 1;
                SetStatus(lblScanStatus, string.Format("추가됨: 0x{0:X2}", nad), Color.LimeGreen);
            }
        }

        /// <summary>슬레이브 추가 정보 읽기 (버전/MCU/시리얼/전압/온도)</summary>
        private void FetchExtraInfo(SlaveInfo info)
        {
            byte nad = info.NAD;
            byte infoNad = _isCjMode ? nad : (byte)0x7F;

            // 0x36: 펌웨어 버전
            var rx = SendDiagWithRetry(infoNad, BuildReadById(infoNad, 0x36));
            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF2)
            {
                info.ReleaseVer = rx.Data[3];
                info.Build      = rx.Data[4];
                info.TargetID   = rx.Data[5];
                info.Trim       = rx.Data[6];
            }

            // 0x3A: MCU / ItemType / LINSpec / SDK 버전
            rx = SendDiagWithRetry(infoNad, BuildReadById(infoNad, 0x3A));
            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF2)
            {
                info.MCU      = rx.Data[3];
                info.ItemType = rx.Data[4];
                info.LINSpec  = rx.Data[5];
                info.SDKVer   = rx.Data[6];
            }

            // 0x39: UTC 타임스탬프
            rx = SendDiagWithRetry(infoNad, BuildReadById(infoNad, 0x39));
            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF2)
                info.UTC = (uint)(rx.Data[3] | rx.Data[4] << 8 | rx.Data[5] << 16 | rx.Data[6] << 24);

            // 0x37: LOT 번호
            rx = SendDiagWithRetry(infoNad, BuildReadById(infoNad, 0x37));
            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF2)
                info.LOT = (uint)(rx.Data[3] | rx.Data[4] << 8 | rx.Data[5] << 16 | rx.Data[6] << 24);

            // 0x38: 전압 / 온도
            rx = SendDiagWithRetry(nad, BuildReadById(nad, 0x38));
            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF2)
            {
                info.VoltRaw = (ushort)(rx.Data[3] | rx.Data[4] << 8);
                info.TempRaw = (ushort)(rx.Data[5] | rx.Data[6] << 8);
            }
        }


        // =========================================================================
        // 슬레이브 목록 선택
        // =========================================================================

        private void lstSlaves_SelectedIndexChanged(object sender, EventArgs e)
        {
            int idx = lstSlaves.SelectedIndex;
            if (idx < 0 || idx >= _slaves.Count) return;

            _selected = _slaves[idx];
            ShowDeviceInfo(_selected);
        }


        // =========================================================================
        // 디바이스 정보 표시
        // =========================================================================

        private void ShowDeviceInfo(SlaveInfo s)
        {
            // 상단 Info 바
            lblTypVal.Text     = MCUName(s.MCU);
            lblProjVal.Text    = TargetName(s.TargetID);
            lblSdkVal.Text     = string.Format("v.{0:X2}", s.SDKVer);
            lblReleaseVal.Text = string.Format("{0:X2}", s.ReleaseVer);
            lblMLMVal.Text     = ItemTypeName(s.ItemType);
            lblMLSVal.Text     = string.Format("0x{0:X2}", s.MasterNAD);
            lblFlagVal.Text    = string.Format("0x{0:X2}", s.SlaveFlag);
            lblSerialVal.Text  = s.UTC != 0 ? UnixToStr(s.UTC) : string.Format("LOT:{0}", s.LOT);
            lblTempVal.Text    = string.Format("{0:F1}°C", s.TempRaw);
            lblVoltVal.Text    = string.Format("{0:F2}V",  s.VoltRaw * 0.001);

            // DTC 표시등
            lblDtcOpen.ForeColor  = (s.DTC0 & 0x80) != 0 ? Color.Red : Color.FromArgb(50, 80, 50);
            lblDtcClose.ForeColor = (s.DTC0 & 0x40) != 0 ? Color.Red : Color.FromArgb(50, 80, 50);
            lblDtcCrit.ForeColor  = (s.DTC0 & 0x20) != 0 ? Color.Red : Color.FromArgb(50, 80, 50);
            lblDtcOk.ForeColor    = (s.DTC1 & 0x01) != 0 ? Color.LimeGreen : Color.OrangeRed;

            // Product Info 탭
            lblNADVal.Text       = string.Format("0x{0:X2}  {1}", s.NAD, NadToName(s.NAD));
            lblMasterNADVal.Text = string.Format("0x{0:X2}  {1}", s.MasterNAD, NadToName(s.MasterNAD));
            lblSFVal.Text        = FormatSlaveFlag(s.SlaveFlag);
            lblColorTblVal.Text  = ColorTableName(s.ColorTable);
            lblUTCVal.Text       = string.Format("{0}  (0x{1:X8})", UnixToStr(s.UTC), s.UTC);
            lblLOTVal.Text       = s.LOT.ToString();
            lblTrimVal.Text      = string.Format("{0}  {1}", s.Trim, TrimName(s.Trim));
            panelFlagBits.Invalidate();

            if (_isCjMode)
            {
                nudWrNAD.Value = Math.Max(nudWrNAD.Minimum,
                    Math.Min(nudWrNAD.Maximum, s.NAD));
                UpdateCjWriteValues();
            }

            // LED Ctrl 탭 SF 체크박스
            chkSF1.Checked = (s.SlaveFlag & 0x01) != 0;
            chkSF2.Checked = (s.SlaveFlag & 0x02) != 0;
            chkSF3.Checked = (s.SlaveFlag & 0x04) != 0;
            chkSF4.Checked = (s.SlaveFlag & 0x08) != 0;
            chkSF5.Checked = (s.SlaveFlag & 0x10) != 0;
            chkSF6.Checked = (s.SlaveFlag & 0x20) != 0;
            chkSF7.Checked = (s.SlaveFlag & 0x40) != 0;
            chkSF8.Checked = (s.SlaveFlag & 0x80) != 0;

            UpdateColorPreview();
            UpdateFramePreview();
        }

        private void ClearDevicePanel()
        {
            foreach (var l in new[] {
                lblTypVal, lblProjVal, lblSdkVal, lblReleaseVal,
                lblMLMVal, lblMLSVal,  lblFlagVal, lblSerialVal,
                lblTempVal, lblVoltVal,
                lblNADVal, lblMasterNADVal, lblSFVal,
                lblColorTblVal, lblUTCVal, lblLOTVal, lblTrimVal })
                l.Text = "--";

            lblDtcOpen.ForeColor  =
            lblDtcClose.ForeColor =
            lblDtcCrit.ForeColor  =
            lblDtcOk.ForeColor    = Color.FromArgb(50, 50, 50);
        }


        // =========================================================================
        // Device 탭 – LED Efficiency GET / SET
        // =========================================================================

        /// <summary>
        /// GET: Eff[0~7], Overlap/CurSrc/OLActive, MaxInt 읽기
        /// 스케일: rawEff / 25 * 10 = %  (raw 250 = 100%)
        /// </summary>
        private void btnDevGet_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad = _selected.NAD;
            SetStatus(lblDevStatus, "읽는 중...", Color.Yellow);

            var rx = SendDiagWithRetry(nad, BuildDataDump(nad, 0xA8, 0x01, 0x00, 0xFF, 0xFF));
            if (IsEffResponse(rx))
            {
                nudEff0.Value = rx.Data[4] / 25 * 10;
                nudEff1.Value = rx.Data[5] / 25 * 10;
                nudEff2.Value = rx.Data[6] / 25 * 10;
                nudEff3.Value = rx.Data[7] / 25 * 10;
            }

            rx = SendDiagWithRetry(nad, BuildDataDump(nad, 0xA8, 0x01, 0x01, 0xFF, 0xFF));
            if (IsEffResponse(rx))
            {
                nudEff4.Value = rx.Data[4] / 25 * 10;
                nudEff5.Value = rx.Data[5] / 25 * 10;
                nudEff6.Value = rx.Data[6] / 25 * 10;
                nudEff7.Value = rx.Data[7] / 25 * 10;
            }

            rx = SendDiagWithRetry(nad, BuildDataDump(nad, 0xA8, 0x02, 0x00, 0xFF, 0xFF));
            if (IsEffResponse(rx))
            {
                nudOverlap.Value  = Clamp(rx.Data[4], nudOverlap);
                nudCurSrc.Value   = Clamp(rx.Data[5], nudCurSrc);
                nudOlActive.Value = Clamp(rx.Data[6], nudOlActive);
            }

            rx = SendDiagWithRetry(nad, BuildDataDump(nad, 0xA8, 0x02, 0x01, 0xFF, 0xFF));
            if (IsEffResponse(rx))
                nudMaxInt.Value = Math.Min(rx.Data[6] | (rx.Data[7] << 8), (int)nudMaxInt.Maximum);

            SetStatus(lblDevStatus, "읽기 완료", Color.LimeGreen);
        }

        private static bool IsEffResponse(LINFrame rx)
            => rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4 && rx.Data[3] == 0x55;

        /// <summary>
        /// SET: Efficiency / DeviceConfig 멀티프레임 쓰기
        /// 스케일: % * 25 / 10 = raw  (100% → 250)
        /// </summary>
        private void btnDevSet_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad  = 0x7F;  // Broadcast
            byte used = (byte)nudUsedLed.Value;
            byte cur  = (byte)nudCurSrc.Value;
            byte ovl  = (byte)nudOverlap.Value;
            byte ola  = (byte)nudOlActive.Value;
            byte mi   = (byte)nudMaxInt.Value;

            byte[] eff =
            {
                (byte)(nudEff0.Value * 25 / 10), (byte)(nudEff1.Value * 25 / 10),
                (byte)(nudEff2.Value * 25 / 10), (byte)(nudEff3.Value * 25 / 10),
                (byte)(nudEff4.Value * 25 / 10), (byte)(nudEff5.Value * 25 / 10),
                (byte)(nudEff6.Value * 25 / 10), (byte)(nudEff7.Value * 25 / 10)
            };

            SetStatus(lblDevStatus, "쓰는 중...", Color.Yellow);

            SendDiagWithRetry(nad, BuildDiagPayload(nad, 0xB2, 0x21, 0xFF, 0x7F, 0xFF, 0xFF));
            Delay(10);
            SendLINFrame(0x3C, new byte[] { nad, 0x10, 0x17, 0xB4, 0xAB, 0x88, 0xC6, 0xFA });
            SendLINFrame(0x3C, new byte[] { nad, 0x21, used, cur, ovl, ola, 0xFF, 0xFF });
            Delay(10);
            SendLINFrame(0x3C, new byte[] { nad, 0x22, 0xFF, 0xFF, mi, 0x00, eff[0], eff[1] });
            Delay(10);
            SendLINFrame(0x3C, new byte[] { nad, 0x23, eff[2], eff[3], eff[4], eff[5], eff[6], eff[7] });
            Delay(10);
            SendDiagWithRetry(nad, BuildDiagPayload(nad, 0xB2, 0x22, 0xFF, 0x7F, 0xFF, 0xFF));

            SetStatus(lblDevStatus, "저장 완료", Color.LimeGreen);
        }


        // =========================================================================
        // Calibration 탭
        // =========================================================================

        // ─── Calibration 프로토콜 ────────────────────────────────────────────────
        // TX: [NAD][05][B4][A8][03][SUB_SELECT][choice][FF]
        //   SUB_SELECT = 0x00: Bright 읽기 / 0x01: X/Y 읽기
        //   choice = LED번호*3 + 채널(0=R,1=G,2=B)  →  0x00~0x17
        //
        // RX(Bright): [NAD][06][F4][0x55+choice][Bright_L][Bright_H][FF][FF]
        // RX(X/Y):    [NAD][06][F4][0x55+choice][X_L][X_H][Y_L][Y_H]
        //
        // _calibXYZ[led, ch, 0] = X  (0~65535)
        // _calibXYZ[led, ch, 1] = Y  (0~65535)
        // _calibXYZ[led, ch, 2] = Bright
        // ─────────────────────────────────────────────────────────────────────────

        private void nudCalibLedSel_ValueChanged(object sender, EventArgs e)
            => UpdateCalibInputs((int)nudCalibLedSel.Value);

        private void UpdateCalibInputs(int led)
        {
            if (!_calibLoaded) return;

            NumericUpDown[] nuds =
            {
                nudCalibRX, nudCalibRY, nudCalibRZ,
                nudCalibGX, nudCalibGY, nudCalibGZ,
                nudCalibBX, nudCalibBY, nudCalibBZ
            };
            foreach (var n in nuds) n.ValueChanged -= OnCalibXyzChanged;

            SetCalibNud(led, 0, nudCalibRX, nudCalibRY, nudCalibRZ);
            SetCalibNud(led, 1, nudCalibGX, nudCalibGY, nudCalibGZ);
            SetCalibNud(led, 2, nudCalibBX, nudCalibBY, nudCalibBZ);

            foreach (var n in nuds) n.ValueChanged += OnCalibXyzChanged;

            UpdateCalibCxCy(led);
        }

        /// <summary>채널 단위로 XYZ 원시값 → NUD 표시값 변환 (Bright 비율 적용)</summary>
        private void SetCalibNud(int led, int ch,
            NumericUpDown nudX, NumericUpDown nudY, NumericUpDown nudZ)
        {
            float X      = _calibXYZ[led, ch, 0];
            float Y      = _calibXYZ[led, ch, 1];
            float bright = _calibXYZ[led, ch, 2];

            if (X == 0 || Y == 0 || bright == 0) return;

            float cx    = X / 65535f;
            float cy    = Y / 65535f;
            float ratio = bright / cy;

            nudX.Value = (int)(cx * ratio);
            nudY.Value = (int)(bright);
            nudZ.Value = (int)((1f - cx - cy) * ratio);
        }

        private void UpdateCalibCxCy(int led)
        {
            void Update(Label lbl, int X, int Y, int bright)
            {
                lbl.Text = string.Format("cx:{0:F4}  cy:{1:F4}  Bright:{2}",
                    X / 65535.0, Y / 65535.0, bright);
            }
            Update(lblCalibBrightR, _calibXYZ[led, 0, 0], _calibXYZ[led, 0, 1], _calibXYZ[led, 0, 2]);
            Update(lblCalibBrightG, _calibXYZ[led, 1, 0], _calibXYZ[led, 1, 1], _calibXYZ[led, 1, 2]);
            Update(lblCalibBrightB, _calibXYZ[led, 2, 0], _calibXYZ[led, 2, 1], _calibXYZ[led, 2, 2]);
        }

        private void OnCalibXyzChanged(object sender, EventArgs e)
        {
            int led = (int)nudCalibLedSel.Value;
            float CX, CY, bright;

            CX = (float)((nudCalibRX.Value * 65535) / (nudCalibRX.Value + nudCalibRY.Value + nudCalibRZ.Value));
            CY = (float)((nudCalibRY.Value * 65535) / (nudCalibRX.Value + nudCalibRY.Value + nudCalibRZ.Value));
            bright = (float)nudCalibRY.Value;

            _calibXYZ[led, 0, 0] = (int)CX;
            _calibXYZ[led, 0, 1] = (int)CY;
            _calibXYZ[led, 0, 2] = (int)bright;

            CX = (float)((nudCalibGX.Value * 65535) / (nudCalibGX.Value + nudCalibGY.Value + nudCalibGZ.Value));
            CY = (float)((nudCalibGY.Value * 65535) / (nudCalibGX.Value + nudCalibGY.Value + nudCalibGZ.Value));
            bright = (float)nudCalibGY.Value;

            _calibXYZ[led, 1, 0] = (int)CX;
            _calibXYZ[led, 1, 1] = (int)CY;
            _calibXYZ[led, 1, 2] = (int)bright;

            CX = (float)((nudCalibBX.Value * 65535) / (nudCalibBX.Value + nudCalibBY.Value + nudCalibBZ.Value));
            CY = (float)((nudCalibBY.Value * 65535) / (nudCalibBX.Value + nudCalibBY.Value + nudCalibBZ.Value));
            bright = (float)nudCalibBY.Value;

            _calibXYZ[led, 2, 0] = (int)CX;
            _calibXYZ[led, 2, 1] = (int)CY;
            _calibXYZ[led, 2, 2] = (int)bright;

            UpdateCalibCxCy(led);
            UpdateCalibDisplay(led);
        }

        private void UpdateCalibDisplay(int led)
        {
            if (led < 0 || led >= dgvCalib.Rows.Count) return;

            var row = dgvCalib.Rows[led];
            row.Cells[1].Value = (_calibXYZ[led, 0, 0] / 65535.0).ToString("F4");
            row.Cells[2].Value = (_calibXYZ[led, 0, 1] / 65535.0).ToString("F4");
            row.Cells[3].Value = _calibXYZ[led, 0, 2].ToString();
            row.Cells[4].Value = (_calibXYZ[led, 1, 0] / 65535.0).ToString("F4");
            row.Cells[5].Value = (_calibXYZ[led, 1, 1] / 65535.0).ToString("F4");
            row.Cells[6].Value = _calibXYZ[led, 1, 2].ToString();
            row.Cells[7].Value = (_calibXYZ[led, 2, 0] / 65535.0).ToString("F4");
            row.Cells[8].Value = (_calibXYZ[led, 2, 1] / 65535.0).ToString("F4");
            row.Cells[9].Value = _calibXYZ[led, 2, 2].ToString();
        }

        private void btnCalibGet_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            int led = (int)nudCalibLedSel.Value;
            SetStatus(lblCalibStatus, string.Format("읽는 중 LED{0}...", led), Color.Yellow);

            DoCalibRead(_selected.NAD, led);
            UpdateCalibInputs(led);
            UpdateCalibDisplay(led);

            SetStatus(lblCalibStatus, "읽기 완료", Color.LimeGreen);
        }

        private void btnCalibGetAll_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            _calibLoaded = false;
            SetStatus(lblCalibStatus, "전체 읽는 중...", Color.Yellow);

            DoCalibReadAll(_selected.NAD, choice =>
            {
                nudCalibLedSel.Value = choice / 3;
                SetStatus(lblCalibStatus,
                    string.Format("읽는 중 choice=0x{0:X2}...", choice), Color.Yellow);
                Application.DoEvents();
            });

            for (int led = 0; led < 8; led++) UpdateCalibDisplay(led);

            _calibLoaded = true;
            UpdateCalibInputs((int)nudCalibLedSel.Value);
            SetStatus(lblCalibStatus, "전체 읽기 완료", Color.LimeGreen);
        }

        /// <summary>
        /// 전체 LED × 채널에 대해 ① Bright → ② X/Y 순서로 읽어 _calibXYZ 에 저장
        /// 0x3D 무응답 시 SendDiagWithRetry 에 의해 자동 재시도됨
        /// </summary>
        private void DoCalibReadAll(byte nad, Action<int> onProgress)
        {
            // ① SUB_SELECT=0x00: Bright (choice 0x00~0x17)
            for (int choice = 0; choice < 24; choice++)
            {
                onProgress?.Invoke(choice);
                var rx = SendDiagWithRetry(nad,
                    BuildDataDump(nad, 0xA8, 0x03, 0x00, (byte)choice, 0xFF));

                if (rx != null && rx.DataLength >= 8 &&
                    rx.Data[2] == 0xF4 && rx.Data[3] == (byte)(0x55 + choice))
                    _calibXYZ[choice / 3, choice % 3, 2] = rx.Data[4] | (rx.Data[5] << 8);
            }

            // ② SUB_SELECT=0x01: X/Y (choice 0x00~0x17)
            for (int choice = 0; choice < 24; choice++)
            {
                var rx = SendDiagWithRetry(nad,
                    BuildDataDump(nad, 0xA8, 0x03, 0x01, (byte)choice, 0xFF));

                if (rx != null && rx.DataLength >= 8 &&
                    rx.Data[2] == 0xF4 && rx.Data[3] == (byte)(0x55 + choice))
                {
                    _calibXYZ[choice / 3, choice % 3, 0] = rx.Data[4] | (rx.Data[5] << 8);
                    _calibXYZ[choice / 3, choice % 3, 1] = rx.Data[6] | (rx.Data[7] << 8);
                }
            }
        }

        /// <summary>단일 LED에 대해 Bright → X/Y 순서로 읽기</summary>
        private void DoCalibRead(byte nad, int led)
        {
            for (int ch = 0; ch < 3; ch++)
            {
                byte choice = (byte)(led * 3 + ch);

                var rx = SendDiagWithRetry(nad,
                    BuildDataDump(nad, 0xA8, 0x03, 0x00, choice, 0xFF));
                if (rx != null && rx.DataLength >= 8 &&
                    rx.Data[2] == 0xF4 && rx.Data[3] == (byte)(0x55 + choice))
                    _calibXYZ[led, ch, 2] = rx.Data[4] | (rx.Data[5] << 8);

                rx = SendDiagWithRetry(nad,
                    BuildDataDump(nad, 0xA8, 0x03, 0x01, choice, 0xFF));
                if (rx != null && rx.DataLength >= 8 &&
                    rx.Data[2] == 0xF4 && rx.Data[3] == (byte)(0x55 + choice))
                {
                    _calibXYZ[led, ch, 0] = rx.Data[4] | (rx.Data[5] << 8);
                    _calibXYZ[led, ch, 1] = rx.Data[6] | (rx.Data[7] << 8);
                }
            }
        }

        private void btnCalibSet_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad = _selected.NAD;
            int  led = (int)nudCalibLedSel.Value;
            float CX, CY, bright;

            CX = (float)((nudCalibRX.Value * 65535) / (nudCalibRX.Value + nudCalibRY.Value + nudCalibRZ.Value));
            CY = (float)((nudCalibRY.Value * 65535) / (nudCalibRX.Value + nudCalibRY.Value + nudCalibRZ.Value));
            bright = (float)nudCalibRY.Value;

            _calibXYZ[led, 0, 0] = (int)CX;
            _calibXYZ[led, 0, 1] = (int)CY;
            _calibXYZ[led, 0, 2] = (int)bright;

            CX = (float)((nudCalibGX.Value * 65535) / (nudCalibGX.Value + nudCalibGY.Value + nudCalibGZ.Value));
            CY = (float)((nudCalibGY.Value * 65535) / (nudCalibGX.Value + nudCalibGY.Value + nudCalibGZ.Value));
            bright = (float)nudCalibGY.Value;

            _calibXYZ[led, 1, 0] = (int)CX;
            _calibXYZ[led, 1, 1] = (int)CY;
            _calibXYZ[led, 1, 2] = (int)bright;

            CX = (float)((nudCalibBX.Value * 65535) / (nudCalibBX.Value + nudCalibBY.Value + nudCalibBZ.Value));
            CY = (float)((nudCalibBY.Value * 65535) / (nudCalibBX.Value + nudCalibBY.Value + nudCalibBZ.Value));
            bright = (float)nudCalibBY.Value;

            _calibXYZ[led, 2, 0] = (int)CX;
            _calibXYZ[led, 2, 1] = (int)CY;
            _calibXYZ[led, 2, 2] = (int)bright;

            int rx2 = (int)nudCalibRX.Value, ry2 = (int)nudCalibRY.Value, rz2 = (int)nudCalibRZ.Value;
            int gx2 = (int)nudCalibGX.Value, gy2 = (int)nudCalibGY.Value, gz2 = (int)nudCalibGZ.Value;
            int bx2 = (int)nudCalibBX.Value, by2 = (int)nudCalibBY.Value, bz2 = (int)nudCalibBZ.Value;

            SetStatus(lblCalibStatus, string.Format("쓰는 중 LED{0}...", led), Color.Yellow);

            // 1) 핸드셰이크
            SendDiagWithRetry(0x7F, BuildDiagPayload(0x7F, 0xB2, 0x21, 0xFF, 0x7F, 0xFF, 0xFF));
            Delay(10);

            // 2) 헤더  [7F][10][17][B4][A2][led][FF][FF]
            SendLINFrame(0x3C, new byte[] { 0x7F, 0x10, 0x17, 0xB4, 0xA2, (byte)led, 0xFF, 0xFF });
            Delay(10);

            // 3) X 값  [7F][21][RX_L][RX_H][GX_L][GX_H][BX_L][BX_H]
            SendLINFrame(0x3C, new byte[]
            {
                0x7F, 0x21,
                (byte)(rx2 & 0xFF), (byte)(rx2 >> 8),
                (byte)(gx2 & 0xFF), (byte)(gx2 >> 8),
                (byte)(bx2 & 0xFF), (byte)(bx2 >> 8)
            });
            Delay(10);

            // 4) Y 값  [7F][22][RY_L][RY_H][GY_L][GY_H][BY_L][BY_H]
            SendLINFrame(0x3C, new byte[]
            {
                0x7F, 0x22,
                (byte)(ry2 & 0xFF), (byte)(ry2 >> 8),
                (byte)(gy2 & 0xFF), (byte)(gy2 >> 8),
                (byte)(by2 & 0xFF), (byte)(by2 >> 8)
            });
            Delay(10);

            // 5) Bright 값  [7F][23][RZ_L][RZ_H][GZ_L][GZ_H][BZ_L][BZ_H]
            SendLINFrame(0x3C, new byte[]
            {
                0x7F, 0x23,
                (byte)(rz2 & 0xFF), (byte)(rz2 >> 8),
                (byte)(gz2 & 0xFF), (byte)(gz2 >> 8),
                (byte)(bz2 & 0xFF), (byte)(bz2 >> 8)
            });
            Delay(10);

            // 6) 확인 ReadByID 0x22
            SendDiagWithRetry(0x7F, BuildDiagPayload(0x7F, 0xB2, 0x22, 0xFF, 0x7F, 0xFF, 0xFF));

            // 7) Flash 저장
            
            SendDiagWithRetry(nad, BuildDataDump(nad, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF));
            Delay(120);

            UpdateCalibCxCy(led);
            UpdateCalibDisplay(led);
            SetStatus(lblCalibStatus, "저장 완료 ✓", Color.LimeGreen);
        }


        // =========================================================================
        // LED Ctrl 탭
        // =========================================================================

        private void OnLedCtrlChanged(object sender, EventArgs e)
        {
            UpdateColorPreview();
            UpdateFramePreview();
        }

        private void UpdateColorPreview() { /* 색상 미리보기 – 필요 시 활성화 */ }

        private void UpdateFramePreview()
        {
            if (lblFramePreview == null || _selected == null) return;

            byte rawBright = (byte)((int)nudBright.Value * 0x7D / 100);
            lblFramePreview.Text = string.Format(
                "FID=0x{0:X2}  SF={1:X2}  CX={2:X2}  CY={3:X2}  Bright={4:X2}h({5}%)  FadeIn={6:X2}  FadeOut={7:X2}  D7=00  D8=7F",
                _selected.MasterNAD, GetSF(),
                (byte)nudColorX.Value, (byte)nudColorY.Value,
                rawBright, (int)nudBright.Value,
                (byte)nudFadeIn.Value, (byte)nudFadeOut.Value);
        }

        private byte GetSF()
        {
            byte sf = 0;
            if (chkSF1.Checked) sf |= 0x01;
            if (chkSF2.Checked) sf |= 0x02;
            if (chkSF3.Checked) sf |= 0x04;
            if (chkSF4.Checked) sf |= 0x08;
            if (chkSF5.Checked) sf |= 0x10;
            if (chkSF6.Checked) sf |= 0x20;
            if (chkSF7.Checked) sf |= 0x40;
            if (chkSF8.Checked) sf |= 0x80;
            return sf;
        }

        private void btnLedSend_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;
            SendColorFrame();
        }

        private void btnLedStart_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            if (_ledTimer == null || !_ledTimer.Enabled)
            {
                _ledTimer = new System.Windows.Forms.Timer { Interval = 100 };
                _ledTimer.Tick += (s, ev) => { if (_selected != null) SendColorFrame(); };
                _ledTimer.Start();
                btnLedStart.Text      = "■  LED Ctrl 중지";
                btnLedStart.BackColor = Color.FromArgb(140, 50, 50);
            }
            else
            {
                _ledTimer.Stop();
                btnLedStart.Text      = "▶  LED Ctrl 시작";
                btnLedStart.BackColor = Color.FromArgb(60, 100, 180);
            }
        }

        /// <summary>Color Control 프레임 전송 (FID = MasterNAD & 0x3F, Enhanced)</summary>
        private void SendColorFrame()
        {
            if (_selected == null) return;

            var frame = new LINFrame
            {
                FrameID      = (byte)(_selected.MasterNAD & 0x3F),
                DataLength   = 8,
                ChecksumType = LINChecksumType.Enhanced
            };
            frame.Data[0] = GetSF();
            frame.Data[1] = (byte)nudColorX.Value;
            frame.Data[2] = (byte)nudColorY.Value;
            frame.Data[3] = (byte)((int)nudBright.Value * 0x7D / 100);
            frame.Data[4] = (byte)nudFadeIn.Value;
            frame.Data[5] = (byte)nudFadeOut.Value;
            frame.Data[6] = 0x00;
            frame.Data[7] = 0x7F;

            _gw.SendPublisher(frame);
        }

        private void btnCIE_Click(object sender, EventArgs e)
        {
            if (_cieWin == null || _cieWin.IsDisposed) _cieWin = new FormCIE(_gw);
            _cieWin.Show(this);
            _cieWin.BringToFront();
        }


        // =========================================================================
        // Product Info 탭
        // =========================================================================

        private void panelFlagBits_Paint(object sender, PaintEventArgs e)
        {
            var  g  = e.Graphics;
            g.Clear(panelFlagBits.BackColor);
            byte sf = _selected?.SlaveFlag ?? 0;

            for (int i = 7; i >= 0; i--)
            {
                bool  set = (sf & (1 << i)) != 0;
                Color c   = set ? Color.FromArgb(50, 200, 100) : Color.FromArgb(55, 28, 28);
                int   x   = (7 - i) * 28 + 2;

                using (var br = new SolidBrush(c))
                    g.FillRectangle(br, x, 2, 24, 18);

                using (var fn = new Font("Consolas", 7f, FontStyle.Bold))
                using (var tb = new SolidBrush(Color.White))
                    g.DrawString((i + 1).ToString(), fn, tb, x + 5, 4);
            }
        }

        /// <summary>
        /// WRITE – CMD 0xA0 SET_NAD_INFO
        /// TX: [NAD][05][B4][A0][NewNAD][MstNAD][SlvFlag][ColorTbl]
        /// RX: D3=0x55(OK), D4=NAD, D5=MasterNAD, D6=SlaveFlag
        /// </summary>
        private void btnProdWrite_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad      = _selected.NAD;
            byte newNAD   = (byte)nudWrNAD.Value;
            byte mstNAD   = _isCjMode ? (byte)0x01 : (byte)nudWrMstNAD.Value;
            byte slvFlag  = _isCjMode ? (byte)(1 << (newNAD - 1)) :
                (byte)nudWrSlaveFlag.Value;
            byte colorTbl = _isCjMode ? (byte)0x00 : (byte)nudWrColorTbl.Value;

            SetStatus(lblProdWriteStatus, "쓰는 중...", Color.Yellow);

            var rx = SendDiagWithRetry(nad, BuildDataDump(nad, 0xA0, newNAD, mstNAD, slvFlag, colorTbl));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4 && rx.Data[3] == 0x55)
            {
                _selected.NAD        = rx.Data[4];
                _selected.MasterNAD  = rx.Data[5];
                _selected.SlaveFlag  = rx.Data[6];
                _selected.ColorTable = colorTbl;

                int idx = lstSlaves.SelectedIndex;
                if (idx >= 0)
                    lstSlaves.Items[idx] = string.Format("0x{0:X2}  {1}",
                        _selected.NAD, NadToName(_selected.NAD));

                ShowDeviceInfo(_selected);
                SetStatus(lblProdWriteStatus, "저장 완료 ✓", Color.LimeGreen);
            }
            else
            {
                SetStatus(lblProdWriteStatus, "응답 없음 / 실패", Color.OrangeRed);
            }
        }


        /// <summary>
        /// UTC WRITE – CMD=0xC0 WRITE_UTC_SERIAL
        /// 버튼 클릭 시점의 PC UTC 시각을 Unix Timestamp(32bit LE)로 변환해 전송.
        /// ⚠ 최초 1회만 기록 가능 (이미 기록된 경우 D0=0xFF)
        /// </summary>
        private void btnWrUTC_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            // ── 현재 PC 시각 → UTC Unix Timestamp 32bit ────────────────────
            DateTime nowUtc   = DateTime.UtcNow;
            long     epoch64  = (long)(nowUtc - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalSeconds;
            uint     utcVal   = (uint)(epoch64 & 0xFFFFFFFF);

            byte utc0 = (byte)( utcVal        & 0xFF);
            byte utc1 = (byte)((utcVal >>  8) & 0xFF);
            byte utc2 = (byte)((utcVal >> 16) & 0xFF);
            byte utc3 = (byte)((utcVal >> 24) & 0xFF);

            // 미리보기 레이블 갱신
            lblUtcPreview.Text = string.Format(
                "{0:yyyy-MM-dd HH:mm:ss} UTC  →  0x{1:X8}  [{2:X2} {3:X2} {4:X2} {5:X2}]",
                nowUtc, utcVal, utc0, utc1, utc2, utc3);

            byte nad = _selected.NAD;
            SetStatus(lblUtcLotStatus,
                string.Format("UTC 기록 중... {0:yyyy-MM-dd HH:mm:ss} UTC (0x{1:X8})", nowUtc, utcVal),
                Color.Yellow);

            var rx = SendDiagWithRetry(nad, BuildDataDump(nad, 0xC0, utc0, utc1, utc2, utc3));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4)
            {
                if (rx.Data[3] == 0x55)
                {
                    _selected.UTC = (uint)(rx.Data[4] | rx.Data[5] << 8 | rx.Data[6] << 16 | rx.Data[7] << 24);
                    ShowDeviceInfo(_selected);
                    SetStatus(lblUtcLotStatus,
                        string.Format("UTC 기록 완료 ✓  {0:yyyy-MM-dd HH:mm:ss} UTC  (0x{1:X8})",
                            nowUtc, _selected.UTC),
                        Color.LimeGreen);
                }
                else if (rx.Data[3] == 0xFF)
                {
                    SetStatus(lblUtcLotStatus,
                        "⚠ UTC 이미 기록됨 – 1회 기록 제한. 초기화 필요 시 공장 리셋",
                        Color.OrangeRed);
                }
                else
                {
                    SetStatus(lblUtcLotStatus,
                        string.Format("UTC 기록 실패 D0=0x{0:X2}", rx.Data[3]),
                        Color.OrangeRed);
                }
            }
            else
            {
                SetStatus(lblUtcLotStatus, "응답 없음 / 실패", Color.OrangeRed);
            }
        }

        /// <summary>
        /// LOT WRITE – CMD=0xC1 WRITE_LOT_NUMBER
        /// TX: [NAD][05][B4][C1][LOT_L][LOT_ML][LOT_MH][LOT_H]
        /// RX: D3=0x55(OK) / D3=0xFF(이미 기록됨)
        /// ⚠ 최초 1회만 기록 가능
        /// </summary>
        private void btnWrLOT_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad  = _selected.NAD;
            byte lot0 = (byte)nudWrLOT0.Value;
            byte lot1 = (byte)nudWrLOT1.Value;
            byte lot2 = (byte)nudWrLOT2.Value;
            byte lot3 = (byte)nudWrLOT3.Value;

            uint lotVal = (uint)(lot0 | lot1 << 8 | lot2 << 16 | lot3 << 24);
            SetStatus(lblUtcLotStatus,
                string.Format("LOT 기록 중... (LOT#{0})", lotVal),
                Color.Yellow);

            var rx = SendDiagWithRetry(nad, BuildDataDump(nad, 0xC1, lot0, lot1, lot2, lot3));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4)
            {
                if (rx.Data[3] == 0x55)
                {
                    _selected.LOT = (uint)(rx.Data[4] | rx.Data[5] << 8 | rx.Data[6] << 16 | rx.Data[7] << 24);
                    ShowDeviceInfo(_selected);
                    SetStatus(lblUtcLotStatus,
                        string.Format("LOT 기록 완료 ✓  LOT#{0}  (0x{1:X8})", _selected.LOT, _selected.LOT),
                        Color.LimeGreen);
                }
                else if (rx.Data[3] == 0xFF)
                {
                    SetStatus(lblUtcLotStatus,
                        "⚠ LOT 이미 기록됨 – 1회 기록 제한. 초기화 필요 시 공장 리셋",
                        Color.OrangeRed);
                }
                else
                {
                    SetStatus(lblUtcLotStatus,
                        string.Format("LOT 기록 실패 D0=0x{0:X2}", rx.Data[3]),
                        Color.OrangeRed);
                }
            }
            else
            {
                SetStatus(lblUtcLotStatus, "응답 없음 / 실패", Color.OrangeRed);
            }
        }


        // =========================================================================
        // PWM / Diag 탭
        // =========================================================================

        /// <summary>
        /// SET PWM – CMD=0xAA SET_PWM
        /// TX: [NAD][05][B4][AA][Port][Duty][FF][FF]
        /// 즉시 PWM 출력 적용. Flash 저장 불필요.
        /// </summary>
        private void btnSetPwm_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad  = _selected.NAD;
            byte port = (byte)nudPwmPort.Value;
            byte duty = (byte)nudPwmDuty.Value;

            SetStatus(lblPwmDiagStatus,
                string.Format("SET PWM – Port={0}, Duty={1} (0x{1:X2})...", port, duty),
                Color.Yellow);

            var rx = SendDiagWithRetry(0x7F, BuildDataDump(0x7F, 0xAA, port, duty, 0xFF, 0xFF));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4 && rx.Data[3] == 0x55)
            {
                SetStatus(lblPwmDiagStatus,
                    string.Format("SET PWM 완료 ✓  Port={0}  Duty={1} (0x{1:X2})", port, duty),
                    Color.LimeGreen);
            }
            else if (rx == null)
            {
                SetStatus(lblPwmDiagStatus, "SET PWM – 응답 없음", Color.OrangeRed);
            }
            else
            {
                SetStatus(lblPwmDiagStatus,
                    string.Format("SET PWM – D0=0x{0:X2}", rx.Data.Length > 3 ? rx.Data[3] : 0xFF),
                    Color.OrangeRed);
            }
        }

        /// <summary>
        /// GET PWM – CMD=0x50 GET_PWM
        /// TX: [NAD][05][B4][50][Port][FF][FF][FF]
        /// RX: D1=포트별 현재 PWM 값
        /// </summary>
        private void btnGetPwm_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad  = _selected.NAD;
            byte port = (byte)nudPwmPort.Value;

            SetStatus(lblPwmDiagStatus,
                string.Format("GET PWM – Port={0}...", port),
                Color.Yellow);

            var rx = SendDiagWithRetry(0x7F, BuildDataDump(0x7F, 0x50, port, 0xFF, 0xFF, 0xFF));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4 && rx.Data[3] == 0x55)
            {
                byte pwmVal = rx.Data[4];
                lblPwmVal.Text = string.Format("{0}  (0x{0:X2})", pwmVal);
                SetStatus(lblPwmDiagStatus,
                    string.Format("GET PWM 완료 ✓  Port={0}  PWM={1} (0x{1:X2})", port, pwmVal),
                    Color.LimeGreen);
            }
            else
            {
                lblPwmVal.Text = "--";
                SetStatus(lblPwmDiagStatus, "GET PWM – 응답 없음 / 실패", Color.OrangeRed);
            }
        }

        /// <summary>
        /// SET PWM MAX – CMD=0x51 SET_PWM_MAX
        /// TX: [NAD][05][B4][51][Mask0][Mask1][Mask2][FF]
        /// 포트별 최대 PWM 비트마스크 3바이트 설정
        /// </summary>
        private void btnSetPwmMax_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad   = _selected.NAD;
            byte mask0 = (byte)nudPwmMaxMask0.Value;
            byte mask1 = (byte)nudPwmMaxMask1.Value;
            byte mask2 = (byte)nudPwmMaxMask2.Value;

            SetStatus(lblPwmMaxStatus,
                string.Format("SET PWM MAX 중... [{0:X2} {1:X2} {2:X2}]", mask0, mask1, mask2),
                Color.Yellow);

            var rx = SendDiagWithRetry(0x7F, BuildDataDump(0x7F, 0x51, mask0, mask1, mask2, 0xFF));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4 && rx.Data[3] == 0x55)
            {
                SetStatus(lblPwmMaxStatus,
                    string.Format("완료 ✓  [{0:X2} {1:X2} {2:X2}]", mask0, mask1, mask2),
                    Color.LimeGreen);
            }
            else
            {
                SetStatus(lblPwmMaxStatus, "응답 없음 / 실패", Color.OrangeRed);
            }
        }

        /// <summary>
        /// GET LED PN VOLT – CMD=0xA7 GET_LED_PN_VOLT
        /// TX: [NAD][05][B4][A7][PN_OPTION][FF][FF][FF]
        /// RX: D1~D4 = LED PN 전압 측정값 (ADC 원시값)
        /// PN_OPTION: 0=LED0, 1=LED1, 2=LED2, 3=LED3
        /// </summary>
        private void btnGetPnVolt_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad = _selected.NAD;
            byte opt = (byte)nudPnVoltOpt.Value;

            SetStatus(lblPwmDiagStatus,
                string.Format("GET LED PN VOLT – Option={0}...", opt),
                Color.Yellow);

            var rx = SendDiagWithRetry(0x7F, BuildDataDump(0x7F, 0xA7, opt, 0xFF, 0xFF, 0xFF));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4 && rx.Data[3] == 0x55)
            {
                ushort v0 = (ushort)(rx.Data[4] | rx.Data[5] << 8);
                ushort v1 = (ushort)(rx.Data[6] | rx.Data[7] << 8);
                lblPnVoltVal.Text = string.Format(
                    "Opt={0}  V[L]=0x{1:X4}({1})  V[H]=0x{2:X4}({2})", opt, v0, v1);
                SetStatus(lblPwmDiagStatus,
                    string.Format("GET PN VOLT 완료 ✓  [0x{0:X4}, 0x{1:X4}]", v0, v1),
                    Color.LimeGreen);
            }
            else
            {
                lblPnVoltVal.Text = "--";
                SetStatus(lblPwmDiagStatus, "GET PN VOLT – 응답 없음 / 실패", Color.OrangeRed);
            }
        }

        /// <summary>
        /// GET CURRENT – CMD=0xA9 GET_CURRENT
        /// TX: [NAD][05][B4][A9][FF][FF][FF][FF]
        /// RX: D1~D4 = 전류 측정 원시값
        /// </summary>
        private void btnGetCurrent_Click(object sender, EventArgs e)
        {
            if (!CheckConnectedAndSelected()) return;

            byte nad = _selected.NAD;

            SetStatus(lblPwmDiagStatus, "GET CURRENT...", Color.Yellow);

            var rx = SendDiagWithRetry(0x7F, BuildDataDump(0x7F, 0xA9, 0xFF, 0xFF, 0xFF, 0xFF));

            if (rx != null && rx.DataLength >= 8 && rx.Data[2] == 0xF4 && rx.Data[3] == 0x55)
            {
                ushort cur0 = (ushort)(rx.Data[4] | rx.Data[5] << 8);
                ushort cur1 = (ushort)(rx.Data[6] | rx.Data[7] << 8);
                lblCurrentVal.Text = string.Format(
                    "Raw[0]=0x{0:X4}({0})  Raw[1]=0x{1:X4}({1})", cur0, cur1);
                SetStatus(lblPwmDiagStatus,
                    string.Format("GET CURRENT 완료 ✓  [0x{0:X4}, 0x{1:X4}]", cur0, cur1),
                    Color.LimeGreen);
            }
            else
            {
                lblCurrentVal.Text = "--";
                SetStatus(lblPwmDiagStatus, "GET CURRENT – 응답 없음 / 실패", Color.OrangeRed);
            }
        }

        private void pnlHeader_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left) { _dragging = true; _dragStart = e.Location; }
        }

        private void pnlHeader_MouseMove(object sender, MouseEventArgs e)
        {
            if (_dragging)
                Location = new Point(Left + e.X - _dragStart.X, Top + e.Y - _dragStart.Y);
        }

        private void pnlHeader_MouseUp(object sender, MouseEventArgs e) => _dragging = false;

        private void btnClose_Click(object sender, EventArgs e) => Close();

        // =========================================================================
        // Param Tuning 탭 – 별도 창 열기
        // =========================================================================
        private void btnOpenParamTuning_Click(object sender, EventArgs e)
        {
            var form = new FormParamTuning(_gw);
            form.Show(this);
        }


        // =========================================================================
        // UI 빌더 헬퍼 (동적 컨트롤 생성용)
        // =========================================================================

        private static Label AddLabel(Control parent, string text, int x, int y,
            Color fore, float sz = 8.5f)
        {
            var l = new Label
            {
                Text      = text, Location  = new Point(x, y),
                AutoSize  = true, ForeColor  = fore,
                BackColor = Color.Transparent, Font = new Font("Segoe UI", sz)
            };
            parent.Controls.Add(l);
            return l;
        }

        private static NumericUpDown AddNud(Control parent, int x, int y,
            decimal min, decimal max, decimal val, int w = 80)
        {
            var n = new NumericUpDown
            {
                Location    = new Point(x, y), Width  = w,
                Minimum     = min,             Maximum = max,
                Value       = Math.Max(min, Math.Min(max, val)),
                BackColor   = Color.FromArgb(36, 42, 56),
                ForeColor   = Color.White,
                Font        = new Font("Consolas", 8.5f),
                BorderStyle = BorderStyle.FixedSingle
            };
            parent.Controls.Add(n);
            return n;
        }

        private static Button AddButton(Control parent, string text, int x, int y,
            int w, Color back)
        {
            var b = new Button
            {
                Text      = text, Location  = new Point(x, y),
                Width     = w,    Height    = 28,
                BackColor = back, ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Font      = new Font("Segoe UI", 8.5f, FontStyle.Bold)
            };
            b.FlatAppearance.BorderSize = 0;
            parent.Controls.Add(b);
            return b;
        }


        // =========================================================================
        // 공통 유틸리티
        // =========================================================================

        private bool CheckConnectedAndSelected()
        {
            if (!_gw.IsConnected || _selected == null)
            {
                MessageBox.Show("연결 또는 노드 선택 필요");
                return false;
            }
            return true;
        }

        private static void SetStatus(Label lbl, string text, Color color)
        {
            lbl.Text      = text;
            lbl.ForeColor = color;
        }


        // =========================================================================
        // 이름 변환 헬퍼
        // =========================================================================

        private string NadToName(byte n)
        {
            if (_isCjMode && n >= 1 && n <= 4)
                return "Chair" + n;

            switch (n)
            {
                case 0x01: return "MLMM0";   case 0x02: return "MLMM1";
                case 0x03: return "MLMM2";   case 0x04: return "MLMM3";
                case 0x10: return "FLDML0";  case 0x11: return "FLDML1";
                case 0x14: return "FRDML0";  case 0x18: return "RLDML0";
                case 0x1C: return "RRDML0";  case 0x20: return "FCML0";
                case 0x24: return "RCML0";
                default:   return "Unknown";
            }
        }

        private static string MCUName(byte m)
        {
            switch (m)
            {
                case 0x24: return "iND83213A";
                case 0x23: return "iND83212";
                case 0x22: return "iND83211";
                default:   return string.Format("0x{0:X2}", m);
            }
        }

        private static string TargetName(byte id)
        {
            switch (id)
            {
                case 0x10: return "HKMC NH2";
                case 0x11: return "HKMC NE";
                default:   return string.Format("0x{0:X2}", id);
            }
        }

        private static string ItemTypeName(byte t)
        {
            switch (t)
            {
                case 0x01: return "MoodLamp";
                case 0x02: return "UVC";
                case 0x03: return "Ghost";
                default:   return string.Format("0x{0:X2}", t);
            }
        }

        private static string ColorTableName(byte n)
        {
            switch (n)
            {
                case 0: return "DEFAULT"; case 1: return "HYUNDAI";
                case 2: return "KIA";     case 3: return "GENESIS";
                case 4: return "KGM";
                default: return n.ToString();
            }
        }

        private static string TrimName(byte n) => ColorTableName(n);

        private static string FormatSlaveFlag(byte sf)
        {
            string bits = "";
            for (int i = 7; i >= 0; i--) bits += (sf & (1 << i)) != 0 ? "■" : "□";
            return string.Format("0x{0:X2}  [{1}]", sf, bits);
        }

        private static string UnixToStr(uint u)
        {
            if (u == 0) return "미설정";
            try
            {
                return new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)
                    .AddSeconds(u).ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss");
            }
            catch { return "?"; }
        }
    }


    // =========================================================================
    // AddNodeDialog – NAD 수동 입력 다이얼로그
    // =========================================================================
    internal class AddNodeDialog : Form
    {
        public byte NAD { get; private set; }

        private readonly TextBox _txt;

        public AddNodeDialog()
        {
            Text            = "Add Node";
            Width           = 280;
            Height          = 120;
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox     = false;
            MinimizeBox     = false;
            StartPosition   = FormStartPosition.CenterParent;
            BackColor       = Color.FromArgb(30, 30, 45);
            ForeColor       = Color.White;

            var lbl    = new Label { Text = "NAD (HEX, 예: 18):", Location = new Point(12, 18), AutoSize = true };
            _txt       = new TextBox
            {
                Location = new Point(12, 38), Width = 120, Text = "18",
                BackColor = Color.FromArgb(40, 40, 60), ForeColor = Color.White,
                BorderStyle = BorderStyle.FixedSingle
            };
            var ok     = new Button { Text = "확인",  Location = new Point(145, 36), Width = 60,
                BackColor = Color.FromArgb(50, 100, 60), FlatStyle = FlatStyle.Flat };
            var cancel = new Button { Text = "취소", Location = new Point(210, 36), Width = 50,
                DialogResult = DialogResult.Cancel, BackColor = Color.FromArgb(80, 40, 40),
                FlatStyle = FlatStyle.Flat };

            ok.Click += (s, e) => { if (TryParse()) { DialogResult = DialogResult.OK; Close(); } };

            Controls.AddRange(new Control[] { lbl, _txt, ok, cancel });
            AcceptButton = ok;
            CancelButton = cancel;
        }

        private bool TryParse()
        {
            try
            {
                NAD = Convert.ToByte(_txt.Text.Trim().Replace("0x", "").Replace("0X", ""), 16);
                return true;
            }
            catch { MessageBox.Show("올바른 HEX 값 입력 (01~7C)"); return false; }
        }
    }


    // =========================================================================
    // SlaveInfo – 슬레이브 노드 정보 저장 클래스
    // =========================================================================
    internal class SlaveInfo
    {
        public byte   NAD, SlaveFlag, MasterNAD, TargetID, ColorTable;
        public byte   ReleaseVer, Build, Trim, MCU, ItemType, LINSpec, SDKVer;
        public uint   UTC, LOT;
        public ushort VoltRaw, TempRaw;
        public byte   DTC0 = 0, DTC1 = 0;
    }
}
