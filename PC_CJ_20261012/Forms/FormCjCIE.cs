using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows.Forms;
using LINMaster.LIN;

namespace LINMaster.Forms
{
    public partial class FormCjCIE : Form
    {
        private ILINGateway _gw;
        private byte _faultReadMask;
        private bool _changingChairId;
        private CheckBox[] _cjChairBoxes;
        private CheckBox[] _cjZoneBoxes;
        private CJColorMethod _colorMethod = CJColorMethod.Cie1931;
        private bool _is1976;
        private bool _loadingColorFields;
        private bool _hasSelection;
        private byte _rgbRed = 255;
        private byte _rgbGreen = 255;
        private byte _rgbBlue = 255;
        private byte _cieBrightness = 10;
        private byte _fadeInUnits = CJLinProtocol.DefaultFadeTimeUnits;
        private byte _fadeOutUnits = CJLinProtocol.DefaultFadeTimeUnits;
        private byte _appliedFadeInUnits = CJLinProtocol.DefaultFadeTimeUnits;
        private byte _appliedFadeOutUnits = CJLinProtocol.DefaultFadeTimeUnits;
        private byte _flickerDurationPercent = 50;
        private byte _flickerHz = 10;
        private bool _operationFieldsReady;
        private bool _operationModeFlicker;
        private bool _loadingOperationFields;
        private PointF _selectedXy = new PointF(0.3127F, 0.3290F);
        private Rectangle _cieArea;
        private Bitmap _cieBitmap;
        private Size _cieBitmapSize;

        private static readonly float[,] CieBoundary =
        {
            {0.1741F,0.0050F},{0.1740F,0.0050F},{0.1738F,0.0049F},{0.1733F,0.0048F},
            {0.1726F,0.0051F},{0.1714F,0.0060F},{0.1689F,0.0116F},{0.1644F,0.0235F},
            {0.1566F,0.0579F},{0.1440F,0.1173F},{0.1241F,0.2080F},{0.0913F,0.3288F},
            {0.0454F,0.4950F},{0.0082F,0.5384F},{0.0139F,0.7502F},{0.0743F,0.8338F},
            {0.1547F,0.8521F},{0.2296F,0.7344F},{0.3016F,0.6923F},{0.3731F,0.6245F},
            {0.4441F,0.5547F},{0.5125F,0.4866F},{0.5752F,0.4242F},{0.6270F,0.3725F},
            {0.6658F,0.3340F},{0.6915F,0.3083F},{0.7079F,0.2920F},{0.7140F,0.2859F},
            {0.7100F,0.2900F},{0.6992F,0.3008F},{0.6916F,0.3083F},{0.6863F,0.3137F},
            {0.6830F,0.3170F},{0.6810F,0.3190F}
        };

        public event Action<string> OspMessage;

        public FormCjCIE()
            : this(null)
        {
        }

        public FormCjCIE(ILINGateway gw)
        {
            _gw = gw;
            InitializeComponent();
            _cjChairBoxes = new[] { chkChair1, chkChair2, chkChair3, chkChair4 };
            _cjZoneBoxes = new[] { chkFrtLh, chkFrtRh, chkHeadrest, chkBack, chkLogo, chkStrobe };

            cmbFID.SelectedIndex = 0;
            cmbCS.SelectedIndex = 0;
            cmbCJMode.SelectedIndex = 0;
            nudInterval.Value = CJLampControlCommand.MinimumPeriodMs;
            chkSOH.Checked = true;
            _hasSelection = true;
            cmbCieMode.SelectedIndex = 1;
            ApplySelectedColor();
            for (int chair = 0; chair < 4; chair++)
            {
                dgvDiagnostic.Rows.Add(string.Format("0x{0:X2}", 0x0A + chair),
                    "Chair" + (chair + 1), "-", "-", "-", "-", "-", "-", "-", "-");
            }
            ResetDiagnosticDisplay(0);
            dgvDiagnostic.ClearSelection();
            UpdateOperationFields();
            RefreshPacketPreview();
        }

        public void UpdateGateway(ILINGateway gw)
        {
            _gw = gw;
        }

        private CheckBox[] GetSlaveFlagBoxes()
        {
            return _cjZoneBoxes;
        }

        private byte GetSelectedCJFrameId()
        {
            switch (cmbFID.SelectedIndex)
            {
                case 1: return CJLinProtocol.Chair1StatusFrameId;
                case 2: return CJLinProtocol.Chair2StatusFrameId;
                case 3: return CJLinProtocol.Chair3StatusFrameId;
                case 4: return CJLinProtocol.Chair4StatusFrameId;
                default: return CJLinProtocol.ControlFrameId;
            }
        }

        private CJGlobalGroup GetGlobalGroup()
        {
            CJGlobalGroup group = CJGlobalGroup.None;
            for (int index = 0; index < _cjChairBoxes.Length; index++)
            {
                if (_cjChairBoxes[index].Checked)
                    group |= (CJGlobalGroup)(1 << index);
            }
            return group;
        }

        private byte GetFaultReadMask()
        {
            byte frameId = GetSelectedCJFrameId();
            if (frameId == CJLinProtocol.ControlFrameId)
            {
                return 0x0F;
            }
            return (byte)(1U << (frameId - CJLinProtocol.Chair1StatusFrameId));
        }

        private CJLocalGroup GetLocalGroup()
        {
            CJLocalGroup group = CJLocalGroup.None;
            for (int index = 0; index < _cjZoneBoxes.Length; index++)
            {
                if (_cjZoneBoxes[index].Checked)
                    group |= (CJLocalGroup)(1 << index);
            }
            return group;
        }

        private CJLampControlCommand BuildCJCommand()
        {
            bool flicker = cmbCJMode.SelectedIndex == 1;
            return new CJLampControlCommand
            {
                GlobalGroup = GetGlobalGroup(),
                LocalGroup = GetLocalGroup(),
                Mode = flicker ? CJLampMode.Flicker : CJLampMode.Normal,
                Synchronize = chkLINErr.Checked,
                Active = chkSOH.Checked,
                FadeEnabled = chkCJFadeEnabled.Checked,
                ColorMethod = _colorMethod,
                Component1 = (byte)nudColorX.Value,
                Component2 = (byte)nudColorY.Value,
                Component3 = (byte)nudBrightness.Value,
                Operation6 = flicker
                    ? CJLinProtocol.DurationPercentToRaw((byte)nudFadeIn.Value)
                    : (byte)nudFadeIn.Value,
                Operation7 = (byte)nudFadeOut.Value
            };
        }

        private void RefreshPacketPreview()
        {
            byte frameId = GetSelectedCJFrameId();
            if (CJLinProtocol.IsStatusFrameId(frameId))
            {
                txtPreview.Text = string.Format("FID {0:X2} | Subscriber Request | Enhanced | DLC 8", frameId);
                lblD1.Text = "Master: Break + Sync + PID";
                lblD2.Text = "Slave: D0~D7 + Enhanced Checksum";
                lblD3.Text = "D0~D5: LH/RH/Head/Back/Logo/Strobe 진단";
                lblD4.Text = "D6/D7: Reserved = 00 00";
                return;
            }

            CJLampControlCommand command = BuildCJCommand();
            byte[] data = command.ToArray();
            txtPreview.Text = string.Format(
                "FID 01 | {0:X2} {1:X2} {2:X2} {3:X2} {4:X2} {5:X2} {6:X2} {7:X2}",
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
            lblD1.Text = string.Format("D0 Global=0x{0:X2} / D1 Local=0x{1:X2}", data[0], data[1]);
            lblD2.Text = string.Format("D2 Mode/Sync/Active/Fade/{0}=0x{1:X2}",
                GetColorMethodText(), data[2]);
            if (_colorMethod == CJColorMethod.Rgb)
                lblD3.Text = string.Format("D3 R={0:X2} / D4 G={1:X2} / D5 B={2:X2}",
                    data[3], data[4], data[5]);
            else if (_colorMethod == CJColorMethod.Cie1976)
                lblD3.Text = string.Format("D3 U'={0:X2} / D4 V'={1:X2} / D5 Bright={2:X2}",
                    data[3], data[4], data[5]);
            else
                lblD3.Text = string.Format("D3 Cx={0:X2} / D4 Cy={1:X2} / D5 Bright={2:X2}",
                    data[3], data[4], data[5]);
            lblD4.Text = cmbCJMode.SelectedIndex == 1
                ? string.Format("D6 Duration={0} ({1:F1}% ON) / D7={2}Hz{3}",
                    data[6], CJLinProtocol.DurationToPercent(data[6]), data[7],
                    chkCJFadeEnabled.Checked
                        ? string.Format(" / Fade={0}ms (기억값)",
                            GetAppliedFadeTimeMs())
                        : string.Empty)
                : string.Format("D6 FadeIn={0} / D7 FadeOut={1} (x100ms)", data[6], data[7]);
        }

        private void DoTxImmediate(bool unused = false)
        {
            byte frameId = GetSelectedCJFrameId();
            if (_gw == null)
            {
                SetStatus("LIN 연결 안됨", Color.OrangeRed);
                return;
            }

            if (CJLinProtocol.IsStatusFrameId(frameId))
            {
                bool requested = _gw.SendSubscriberRequest(frameId, CJLinProtocol.DataLength,
                    LINChecksumType.Enhanced);
                SetStatus(requested ? "진단 요청 전송 / RX 대기" : "진단 요청 실패",
                    requested ? Color.DeepSkyBlue : Color.OrangeRed);
                OspMessage?.Invoke(string.Format("[CJ DIAG] {0} Chair{1} ID=0x{2:X2} Enhanced DLC=8",
                    requested ? "REQUEST" : "REQUEST FAIL",
                    CJLinProtocol.GetChairNumber(frameId), frameId));
                return;
            }

            CJLampControlCommand command = BuildCJCommand();
            byte[] data = command.ToArray();
            var frame = new LINFrame
            {
                FrameID = CJLinProtocol.ControlFrameId,
                DataLength = CJLinProtocol.DataLength,
                ChecksumType = LINChecksumType.Enhanced
            };
            Array.Copy(data, frame.Data, data.Length);
            bool sent = _gw.SendPublisher(frame);
            if (sent && command.Mode == CJLampMode.Normal && command.FadeEnabled)
            {
                _appliedFadeInUnits = command.Operation6;
                _appliedFadeOutUnits = command.Operation7;
            }
            SetStatus(sent ? "CJ LIN 제어 TX 완료" : "CJ LIN 제어 TX 실패",
                sent ? Color.DeepSkyBlue : Color.OrangeRed);
            OspMessage?.Invoke(string.Format("[CJ LIN] {0} ID=0x01 DATA={1}",
                sent ? "TX" : "TX FAIL", BitConverter.ToString(data).Replace('-', ' ')));
        }

        private async void btnReadFault_Click(object sender, EventArgs e)
        {
            if ((_gw == null) || (!_gw.IsConnected))
            {
                SetStatus("LIN 연결 안됨", Color.OrangeRed);
                return;
            }
            byte selected = GetFaultReadMask();
            btnReadFault.Enabled = false;
            _faultReadMask = 0;
            ResetDiagnosticDisplay(selected);
            try
            {
                for (int index = 0; index < 4; index++)
                {
                    byte mask = (byte)(1 << index);
                    if ((selected & mask) == 0)
                    {
                        continue;
                    }
                    Stopwatch timer = Stopwatch.StartNew();
                    long nextHeader = 0;
                    while ((timer.ElapsedMilliseconds < 150) && ((_faultReadMask & mask) == 0))
                    {
                        if ((IsDisposed) || (_gw == null) || (!_gw.IsConnected))
                        {
                            return;
                        }
                        if (timer.ElapsedMilliseconds >= nextHeader)
                        {
                            byte id = (byte)(0x0A + index);
                            _gw.SendSubscriberRequest(id, 8, LINChecksumType.Enhanced);
                            OspMessage?.Invoke(string.Format("[CJ 고장 Read] Chair{0} FID=0x{1:X2} Enhanced", index + 1, id));
                            nextHeader = timer.ElapsedMilliseconds + 20;
                        }
                        _gw.Poll();
                        await Task.Delay(10);
                    }
                }
                SetStatus(string.Format("고장 Read: 요청 0x{0:X1} / 응답 0x{1:X1}", selected, _faultReadMask & selected),
                    (_faultReadMask & selected) == selected ? Color.LimeGreen : Color.OrangeRed);
            }
            finally
            {
                if (!IsDisposed)
                {
                    btnReadFault.Enabled = true;
                }
            }
        }

        public void HandleCJFrame(LINFrame frame)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<LINFrame>(HandleCJFrame), frame);
                return;
            }
            if (frame == null || frame.Direction != LINDirection.RX)
                return;

            CJStatusFrame status;
            if (!CJStatusFrame.TryParse(frame, out status))
                return;

            bool normal = FaultsAreClear(status);
            _faultReadMask |= (byte)(1 << (status.ChairNumber - 1));
            DataGridViewRow row = dgvDiagnostic.Rows[status.ChairNumber - 1];
            for (int index = 0; index < status.Faults.Length; index++)
            {
                row.Cells[index + 2].Value = FormatFault(status.Faults[index]);
                row.Cells[index + 2].Style.ForeColor = status.Faults[index] == CJFaultFlags.None
                    ? Color.LimeGreen : Color.OrangeRed;
            }
            row.Cells[8].Value = normal ? "NORMAL" : "FAULT";
            row.Cells[8].Style.ForeColor = normal ? Color.LimeGreen : Color.OrangeRed;
            row.Cells[9].Value = BitConverter.ToString(status.RawData).Replace('-', ' ');
            row.DefaultCellStyle.BackColor = Color.FromArgb(20, 28, 40);
            lblD5.Text = string.Format("마지막 RX: Chair{0} / FID 0x{1:X2} / {2}",
                status.ChairNumber, status.FrameId, normal ? "NORMAL" : "FAULT");
            lblD5.ForeColor = normal ? Color.LimeGreen : Color.OrangeRed;
            OspMessage?.Invoke(string.Format("[CJ DIAG RX] Chair{0} ID=0x{1:X2} DATA={2}",
                status.ChairNumber, status.FrameId,
                BitConverter.ToString(status.RawData).Replace('-', ' ')));
            dgvDiagnostic.ClearSelection();
        }

        private async void dgvDiagnostic_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (_changingChairId || (e.RowIndex < 0) || (e.RowIndex >= 4))
            {
                return;
            }
            if ((_gw == null) || (!_gw.IsConnected) || ((_faultReadMask & (1 << e.RowIndex)) == 0))
            {
                SetStatus("먼저 해당 의자의 진단 응답을 확인하세요.", Color.OrangeRed);
                return;
            }

            byte currentId = (byte)(CJLinProtocol.Chair1StatusFrameId + e.RowIndex);
            byte newId;
            using (var dialog = new FormCjIdChange(currentId))
            {
                if (dialog.ShowDialog(this) != DialogResult.OK)
                {
                    return;
                }
                newId = dialog.NewChairId;
            }
            bool targetOccupied = (_faultReadMask & (1 << (newId - CJLinProtocol.Chair1StatusFrameId))) != 0;
            if (targetOccupied && (MessageBox.Show(this, string.Format("ID 0x{0:X2}가 이미 응답 중입니다. 변경하면 두 보드가 같은 ID가 되어 일반 진단 응답이 충돌할 수 있습니다. 그래도 송신하시겠습니까?", newId), "CJ ID 중복 경고", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes))
            {
                return;
            }

            byte[] data = CJLinProtocol.BuildChairIdChangeData(currentId, newId);
            var frame = new LINFrame
            {
                FrameID = (byte)(currentId + CJLinProtocol.ChairIdChangeOffset),
                DataLength = CJLinProtocol.DataLength,
                FrameType = LINFrameType.Publisher,
                ChecksumType = LINChecksumType.Enhanced
            };
            Array.Copy(data, frame.Data, data.Length);
            if (!_gw.SendPublisher(frame))
            {
                SetStatus("의자 ID 변경 명령 송신 실패", Color.OrangeRed);
                return;
            }
            OspMessage?.Invoke(string.Format("[CJ ID 변경 TX] FID=0x{0:X2} DATA={1}", frame.FrameID, BitConverter.ToString(data).Replace('-', ' ')));
            if (targetOccupied)
            {
                SetStatus("변경 명령 송신 완료. 목적 ID 중복으로 개별 응답 확인은 불가합니다. UDS에서 Init Number로 확인하세요.", Color.Orange);
                return;
            }

            _changingChairId = true;
            dgvDiagnostic.Enabled = false;
            btnReadFault.Enabled = false;
            _faultReadMask &= (byte)~(1 << (newId - CJLinProtocol.Chair1StatusFrameId));
            SetStatus(string.Format("ID 0x{0:X2} → 0x{1:X2} 변경 확인 중", currentId, newId), Color.DeepSkyBlue);
            try
            {
                Stopwatch timer = Stopwatch.StartNew();
                long nextHeader = 0;
                while ((timer.ElapsedMilliseconds < 1000) && ((_faultReadMask & (1 << (newId - CJLinProtocol.Chair1StatusFrameId))) == 0))
                {
                    if (IsDisposed || (_gw == null) || (!_gw.IsConnected))
                    {
                        return;
                    }
                    if (timer.ElapsedMilliseconds >= nextHeader)
                    {
                        _gw.SendSubscriberRequest(newId, CJLinProtocol.DataLength, LINChecksumType.Enhanced);
                        nextHeader = timer.ElapsedMilliseconds + 20;
                    }
                    _gw.Poll();
                    await Task.Delay(10);
                }
                if ((_faultReadMask & (1 << (newId - CJLinProtocol.Chair1StatusFrameId))) == 0)
                {
                    SetStatus("새 ID의 진단 응답이 없습니다. 저장 여부를 다시 확인하세요.", Color.OrangeRed);
                    return;
                }
                _faultReadMask &= (byte)~(1 << e.RowIndex);
                for (int column = 2; column < 10; column++)
                {
                    dgvDiagnostic.Rows[e.RowIndex].Cells[column].Value = "-";
                }
                dgvDiagnostic.Rows[e.RowIndex].Cells[8].Value = string.Format("0x{0:X2}로 변경", newId);
                if (GetSelectedCJFrameId() == currentId)
                {
                    cmbFID.SelectedIndex = newId - CJLinProtocol.Chair1StatusFrameId + 1;
                }
                SetStatus(string.Format("ID 0x{0:X2} → 0x{1:X2} 변경 및 진단 응답 확인", currentId, newId), Color.LimeGreen);
            }
            finally
            {
                if (!IsDisposed)
                {
                    dgvDiagnostic.Enabled = true;
                    btnReadFault.Enabled = true;
                }
                _changingChairId = false;
            }
        }

        private static bool FaultsAreClear(CJStatusFrame status)
        {
            for (int index = 0; index < status.Faults.Length; index++)
            {
                if (status.Faults[index] != CJFaultFlags.None)
                    return false;
            }
            return status.ReservedBytesValid;
        }

        private static string FormatFault(CJFaultFlags flags)
        {
            if (flags == CJFaultFlags.None) return "OK";
            string text = string.Empty;
            if ((flags & CJFaultFlags.Communication) != 0) text += "COMM/";
            if ((flags & CJFaultFlags.Open) != 0) text += "OPEN/";
            if ((flags & CJFaultFlags.Short) != 0) text += "SHORT/";
            if ((flags & CJFaultFlags.SupplyVoltage) != 0) text += "5V_FLT/";
            if ((flags & CJFaultFlags.Temperature) != 0) text += "TEMP/";
            return text.TrimEnd('/');
        }

        private void ResetDiagnosticDisplay(byte requestedMask)
        {
            for (int chair = 0; chair < 4; chair++)
            {
                DataGridViewRow row = dgvDiagnostic.Rows[chair];
                bool requested = (requestedMask & (1U << chair)) != 0U;
                for (int column = 2; column < 10; column++)
                {
                    row.Cells[column].Value = "-";
                    row.Cells[column].Style.ForeColor = Color.White;
                }
                row.Cells[8].Value = requested ? "RX 대기" : "요청 안 함";
                row.Cells[8].Style.ForeColor = requested
                    ? Color.DeepSkyBlue : Color.FromArgb(110, 120, 140);
                row.DefaultCellStyle.BackColor = requested
                    ? Color.FromArgb(20, 28, 40) : Color.FromArgb(14, 18, 26);
            }
            lblD5.Text = requestedMask == 0x0FU
                ? "전체 고장 Read: 0x0A → 0x0B → 0x0C → 0x0D"
                : requestedMask == 0U ? "진단 응답 대기" : "개별 고장 Read";
            lblD5.ForeColor = Color.FromArgb(120, 200, 255);
            dgvDiagnostic.ClearSelection();
        }

        private void SetStatus(string text, Color color)
        {
            lblStatus.Text = "● " + text;
            lblStatus.ForeColor = color;
        }

        private void cmbFID_SelectedIndexChanged(object sender, EventArgs e)
        {
            bool controlFrame = GetSelectedCJFrameId() == CJLinProtocol.ControlFrameId;
            grpGlobal.Enabled = controlFrame;
            grpLocal.Enabled = controlFrame;
            grpControl.Enabled = controlFrame;
            grpColor.Enabled = controlFrame;
            grpOperation.Enabled = controlFrame;
            nudInterval.Minimum = controlFrame ? CJLampControlCommand.MinimumPeriodMs : 100;
            if (nudInterval.Value < nudInterval.Minimum)
                nudInterval.Value = nudInterval.Minimum;
            btnTxOnce.Text = controlFrame ? "즉시 TX" : "진단 요청";
            RefreshPacketPreview();
        }

        private void cmbCJMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (_operationFieldsReady)
                SaveOperationFields();
            _operationModeFlicker = cmbCJMode.SelectedIndex == 1;
            UpdateOperationFields();
            RefreshPacketPreview();
        }

        private void SaveOperationFields()
        {
            if (_loadingOperationFields)
                return;
            if (_operationModeFlicker)
            {
                _flickerDurationPercent = (byte)nudFadeIn.Value;
                _flickerHz = (byte)nudFadeOut.Value;
            }
            else
            {
                _fadeInUnits = (byte)nudFadeIn.Value;
                _fadeOutUnits = (byte)nudFadeOut.Value;
            }
        }

        private int GetAppliedFadeTimeMs()
        {
            byte units = chkSOH.Checked ? _appliedFadeInUnits : _appliedFadeOutUnits;
            return units * 100;
        }

        private void UpdateOperationFields()
        {
            bool flicker = _operationModeFlicker;
            _loadingOperationFields = true;
            chkCJFadeEnabled.Enabled = true;
            lblFIN.Text = flicker ? "D6 ON(%):" : "D6 FadeIn:";
            lblFOUT.Text = flicker ? "D7 Hz:" : "D7 FadeOut:";
            nudFadeIn.Maximum = flicker ? 100 : 250;
            nudFadeIn.Increment = 1;
            nudFadeOut.Maximum = flicker ? CJLinProtocol.MaximumFlickerHz : 250;
            if (flicker)
            {
                nudFadeIn.Value = _flickerDurationPercent;
                nudFadeOut.Value = _flickerHz;
            }
            else
            {
                nudFadeIn.Value = _fadeInUnits;
                nudFadeOut.Value = _fadeOutUnits;
            }
            _loadingOperationFields = false;
            _operationFieldsReady = true;
        }

        private void AnyField_Changed(object sender, EventArgs e)
        {
            if (_loadingOperationFields)
                return;
            SaveOperationFields();
            RefreshPacketPreview();
        }

        private void nudColor_ValueChanged(object sender, EventArgs e)
        {
            if (_loadingColorFields) return;

            if (_colorMethod == CJColorMethod.Rgb)
            {
                _rgbRed = (byte)nudColorX.Value;
                _rgbGreen = (byte)nudColorY.Value;
                _rgbBlue = (byte)nudBrightness.Value;
                pnlColorPatch.BackColor = Color.FromArgb(_rgbRed, _rgbGreen, _rgbBlue);
                UpdateSelectedText();
                RefreshPacketPreview();
                return;
            }

            PointF coordinate = new PointF((float)nudColorX.Value / 250F,
                (float)nudColorY.Value / 250F);
            if (_is1976) coordinate = UvToXy(coordinate.X, coordinate.Y);
            _selectedXy = coordinate;
            _cieBrightness = (byte)nudBrightness.Value;
            _hasSelection = true;
            pnlColorPatch.BackColor = CieRgb(coordinate.X, coordinate.Y, _cieBrightness);
            UpdateSelectedText();
            RefreshPacketPreview();
            pnlCIE.Invalidate();
        }

        private void btnTxOnce_Click(object sender, EventArgs e)
        {
            DoTxImmediate();
        }

        private void btnTxCycle_Click(object sender, EventArgs e)
        {
            if (txTimer.Enabled)
            {
                txTimer.Stop();
                btnTxCycle.Text = "주기 TX 시작";
                btnTxCycle.BackColor = Color.FromArgb(50, 110, 190);
                return;
            }

            txTimer.Interval = (int)nudInterval.Value;
            txTimer.Start();
            btnTxCycle.Text = "주기 TX 정지";
            btnTxCycle.BackColor = Color.FromArgb(160, 50, 50);
            DoTxImmediate();
        }

        private void txTimer_Tick(object sender, EventArgs e)
        {
            DoTxImmediate();
        }

        private void cmbCieMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            _colorMethod = (CJColorMethod)cmbCieMode.SelectedIndex;
            _is1976 = _colorMethod == CJColorMethod.Cie1976;
            _cieBitmap?.Dispose();
            _cieBitmap = null;
            UpdateColorFieldMode();
            ApplySelectedColor();
            pnlCIE.Invalidate();
        }

        private void ApplySelectedColor()
        {
            _loadingColorFields = true;
            if (_colorMethod == CJColorMethod.Rgb)
            {
                nudColorX.Value = _rgbRed;
                nudColorY.Value = _rgbGreen;
                nudBrightness.Value = _rgbBlue;
                pnlColorPatch.BackColor = Color.FromArgb(_rgbRed, _rgbGreen, _rgbBlue);
            }
            else
            {
                PointF coordinate = _is1976
                    ? XyToUv(_selectedXy.X, _selectedXy.Y)
                    : _selectedXy;
                nudColorX.Value = ToProtocolByte(coordinate.X);
                nudColorY.Value = ToProtocolByte(coordinate.Y);
                nudBrightness.Value = _cieBrightness;
                pnlColorPatch.BackColor = CieRgb(_selectedXy.X, _selectedXy.Y, _cieBrightness);
            }
            _loadingColorFields = false;
            UpdateSelectedText();
            RefreshPacketPreview();
        }

        private void UpdateColorFieldMode()
        {
            _loadingColorFields = true;
            nudColorX.Maximum = _colorMethod == CJColorMethod.Rgb ? 255 : 250;
            nudColorY.Maximum = nudColorX.Maximum;
            nudBrightness.Minimum = 0;
            nudBrightness.Maximum = 255;
            nudBrightness.Enabled = true;
            if (_colorMethod == CJColorMethod.Rgb)
            {
                grpColor.Text = "[ D3/D4/D5 R/G/B ]";
                lblCX.Text = "D3 R:";
                lblCY.Text = "D4 G:";
                lblBr.Text = "D5 B:";
                lblCieMode.Text = "R/G/B direct";
            }
            else if (_colorMethod == CJColorMethod.Cie1976)
            {
                grpColor.Text = "[ D3/D4/D5 U'/V' / Brightness ]";
                lblCX.Text = "D3 U':";
                lblCY.Text = "D4 V':";
                lblBr.Text = "D5 Bright:";
                lblCieMode.Text = "CIE1976 (u'v')";
            }
            else
            {
                grpColor.Text = "[ D3/D4/D5 Cx/Cy / Brightness ]";
                lblCX.Text = "D3 Cx:";
                lblCY.Text = "D4 Cy:";
                lblBr.Text = "D5 Bright:";
                lblCieMode.Text = "CIE1931 (xy)";
            }
            _loadingColorFields = false;
        }

        private void UpdateSelectedText()
        {
            if (_colorMethod == CJColorMethod.Rgb)
            {
                lblSelected.Text = string.Format("RGB={0},{1},{2}  D3={0:X2} D4={1:X2} D5={2:X2}",
                    _rgbRed, _rgbGreen, _rgbBlue);
                return;
            }

            PointF coordinate = _is1976
                ? XyToUv(_selectedXy.X, _selectedXy.Y)
                : _selectedXy;
            lblSelected.Text = string.Format("{0}={1:F4},{2:F4}  D3={3:X2} D4={4:X2} D5={5:X2}",
                _is1976 ? "u'v'" : "xy", coordinate.X, coordinate.Y,
                (byte)nudColorX.Value, (byte)nudColorY.Value, (byte)nudBrightness.Value);
        }

        private string GetColorMethodText()
        {
            if (_colorMethod == CJColorMethod.Rgb) return "RGB";
            return _colorMethod == CJColorMethod.Cie1976 ? "U'V'" : "CxCy";
        }

        private static byte ToProtocolByte(float value)
        {
            return (byte)Math.Max(0, Math.Min(250, (int)Math.Round(value * 250F)));
        }

        private void CalculateCieArea()
        {
            _cieArea = new Rectangle(44, 10,
                Math.Max(2, pnlCIE.ClientSize.Width - 54),
                Math.Max(2, pnlCIE.ClientSize.Height - 36));
        }

        private PointF CieToPixel(float x, float y)
        {
            return new PointF(_cieArea.Left + (x * _cieArea.Width),
                _cieArea.Bottom - (y * _cieArea.Height));
        }

        private PointF PixelToCie(int x, int y)
        {
            return new PointF(
                Math.Max(0F, Math.Min(1F, (x - _cieArea.Left) / (float)_cieArea.Width)),
                Math.Max(0F, Math.Min(1F, (_cieArea.Bottom - y) / (float)_cieArea.Height)));
        }

        private static PointF XyToUv(float x, float y)
        {
            float denominator = (-2F * x) + (12F * y) + 3F;
            if (Math.Abs(denominator) < 0.000001F) return new PointF(0.19F, 0.46F);
            return new PointF(4F * x / denominator, 9F * y / denominator);
        }

        private static PointF UvToXy(float u, float v)
        {
            float denominator = (6F * u) - (16F * v) + 12F;
            if (Math.Abs(denominator) < 0.000001F) return new PointF(0.3127F, 0.3290F);
            return new PointF(9F * u / denominator, 4F * v / denominator);
        }

        private static Color CieRgb(float x, float y, byte brightness = 255)
        {
            if (brightness == 0) return Color.Black;
            if (y < 0.0001F) return Color.FromArgb(5, 5, 10);
            float z = 1F - x - y;
            float bigX = x / y;
            float bigY = 1F;
            float bigZ = z / y;
            float red = (3.2406F * bigX) - (1.5372F * bigY) - (0.4986F * bigZ);
            float green = (-0.9689F * bigX) + (1.8758F * bigY) + (0.0415F * bigZ);
            float blue = (0.0557F * bigX) - (0.2040F * bigY) + (1.0570F * bigZ);
            return Color.FromArgb(Clamp((int)(Gamma(red) * 255F)) * brightness / 255,
                Clamp((int)(Gamma(green) * 255F)) * brightness / 255,
                Clamp((int)(Gamma(blue) * 255F)) * brightness / 255);
        }

        private static float Gamma(float value)
        {
            if (value <= 0F) return 0F;
            return value <= 0.0031308F
                ? 12.92F * value
                : (float)((1.055 * Math.Pow(value, 1.0 / 2.4)) - 0.055);
        }

        private static int Clamp(int value)
        {
            return value < 0 ? 0 : value > 255 ? 255 : value;
        }

        private PointF[] GetBoundaryPixels()
        {
            int count = CieBoundary.GetLength(0);
            var points = new PointF[count + 1];
            for (int index = 0; index < count; index++)
            {
                float x = CieBoundary[index, 0];
                float y = CieBoundary[index, 1];
                if (_is1976)
                {
                    PointF uv = XyToUv(x, y);
                    x = uv.X;
                    y = uv.Y;
                }
                points[index] = CieToPixel(x, y);
            }
            points[count] = points[0];
            return points;
        }

        private Bitmap BuildCieBitmap()
        {
            int width = _cieArea.Width;
            int height = _cieArea.Height;
            var bitmap = new Bitmap(width, height);
            var boundary = new PointF[CieBoundary.GetLength(0)];
            for (int index = 0; index < boundary.Length; index++)
            {
                float x = CieBoundary[index, 0];
                float y = CieBoundary[index, 1];
                if (_is1976)
                {
                    PointF uv = XyToUv(x, y);
                    x = uv.X;
                    y = uv.Y;
                }
                boundary[index] = new PointF(x * width, (1F - y) * height);
            }

            using (var path = new GraphicsPath())
            using (Graphics graphics = Graphics.FromImage(bitmap))
            {
                path.AddPolygon(boundary);
                graphics.Clear(Color.FromArgb(10, 12, 20));
                for (int py = 0; py < height; py += 2)
                {
                    for (int px = 0; px < width; px += 2)
                    {
                        if (!path.IsVisible(px + 1, py + 1)) continue;
                        float x = px / (float)width;
                        float y = 1F - (py / (float)height);
                        if (_is1976)
                        {
                            PointF xy = UvToXy(x, y);
                            x = xy.X;
                            y = xy.Y;
                        }
                        using (var brush = new SolidBrush(CieRgb(x, y)))
                            graphics.FillRectangle(brush, px, py, 2, 2);
                    }
                }
            }
            return bitmap;
        }

        private void pnlCIE_Paint(object sender, PaintEventArgs e)
        {
            CalculateCieArea();
            Graphics graphics = e.Graphics;
            graphics.SmoothingMode = SmoothingMode.AntiAlias;
            graphics.Clear(Color.FromArgb(10, 12, 20));

            Size size = new Size(_cieArea.Width, _cieArea.Height);
            if (_cieBitmap == null || _cieBitmapSize != size)
            {
                _cieBitmap?.Dispose();
                _cieBitmap = BuildCieBitmap();
                _cieBitmapSize = size;
            }
            graphics.DrawImage(_cieBitmap, _cieArea.Left, _cieArea.Top);

            PointF[] boundary = GetBoundaryPixels();
            using (var pen = new Pen(Color.White, 1.2F)) graphics.DrawLines(pen, boundary);
            using (var pen = new Pen(Color.FromArgb(45, 180, 180, 200), 0.5F))
            {
                for (float value = 0.1F; value <= 1F; value += 0.1F)
                {
                    graphics.DrawLine(pen, _cieArea.Left + (value * _cieArea.Width), _cieArea.Top,
                        _cieArea.Left + (value * _cieArea.Width), _cieArea.Bottom);
                    graphics.DrawLine(pen, _cieArea.Left, _cieArea.Bottom - (value * _cieArea.Height),
                        _cieArea.Right, _cieArea.Bottom - (value * _cieArea.Height));
                }
            }

            if (_hasSelection)
            {
                PointF selected = _is1976
                    ? XyToUv(_selectedXy.X, _selectedXy.Y)
                    : _selectedXy;
                PointF pixel = CieToPixel(selected.X, selected.Y);
                using (var pen = new Pen(Color.White, 1.8F))
                {
                    graphics.DrawEllipse(pen, pixel.X - 8, pixel.Y - 8, 16, 16);
                    graphics.DrawLine(pen, pixel.X - 13, pixel.Y, pixel.X + 13, pixel.Y);
                    graphics.DrawLine(pen, pixel.X, pixel.Y - 13, pixel.X, pixel.Y + 13);
                }
            }
        }

        private void pnlCIE_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left) return;
            CalculateCieArea();
            if (!_cieArea.Contains(e.Location)) return;
            PointF value = PixelToCie(e.X, e.Y);
            _selectedXy = _is1976 ? UvToXy(value.X, value.Y) : value;
            _selectedXy.X = Math.Max(0F, Math.Min(1F, _selectedXy.X));
            _selectedXy.Y = Math.Max(0F, Math.Min(1F, _selectedXy.Y));
            _hasSelection = true;
            if (_colorMethod == CJColorMethod.Rgb)
            {
                Color color = CieRgb(_selectedXy.X, _selectedXy.Y);
                _rgbRed = color.R;
                _rgbGreen = color.G;
                _rgbBlue = color.B;
            }
            ApplySelectedColor();
            pnlCIE.Invalidate();
        }

        private void pnlCIE_MouseMove(object sender, MouseEventArgs e)
        {
            CalculateCieArea();
            if (!_cieArea.Contains(e.Location))
            {
                lblCursor.Text = "Cursor: -";
                return;
            }
            PointF value = PixelToCie(e.X, e.Y);
            lblCursor.Text = _is1976
                ? string.Format("Cursor u'={0:F3} v'={1:F3}", value.X, value.Y)
                : string.Format("Cursor x={0:F3} y={1:F3}", value.X, value.Y);
        }

        private void pnlCIE_Resize(object sender, EventArgs e)
        {
            _cieBitmap?.Dispose();
            _cieBitmap = null;
            pnlCIE.Invalidate();
        }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            txTimer.Stop();
            _cieBitmap?.Dispose();
            base.OnFormClosed(e);
        }
    }
}
