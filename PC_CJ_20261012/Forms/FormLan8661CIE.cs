using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Windows.Forms;
using LINMaster.LAN8661;

namespace LINMaster.Forms
{
    /// <summary>
    /// Microchip LAN8661 WS2812/OSP/ISE CIE 제어 창.
    /// 기존 ADI FormCIE와 통신 경로를 완전히 분리한다.
    /// </summary>
    public partial class FormLan8661CIE : Form
    {
        private readonly Lan8661Ws2812Client _client;
        private readonly byte[] _frame = new byte[Lan8661Ws2812Client.FrameByteCount];

        private bool _is1976;
        private bool _hasSelection = true;
        private bool _updatingFields;
        private bool _changingShiftMode;
        private PointF _selectedXy = new PointF(0.3127f, 0.3290f);
        private Bitmap _cieBitmap;
        private Size _cieCacheSize;
        private Rectangle _cieRect;
        private Bitmap _imageBitmap;
        private PointF _imageSelection = PointF.Empty;
        private int _shiftStep;
        private bool _ilasWarningReported;

        public event Action<string> StatusMessage;

        private static readonly float[,] SpectralLocus =
        {
            {0.1741f,0.0050f},{0.1740f,0.0050f},{0.1738f,0.0049f},{0.1733f,0.0048f},
            {0.1726f,0.0051f},{0.1714f,0.0060f},{0.1689f,0.0116f},{0.1644f,0.0235f},
            {0.1566f,0.0579f},{0.1440f,0.1173f},{0.1241f,0.2080f},{0.0913f,0.3288f},
            {0.0454f,0.4950f},{0.0082f,0.5384f},{0.0139f,0.7502f},{0.0743f,0.8338f},
            {0.1547f,0.8521f},{0.2296f,0.7344f},{0.3016f,0.6923f},{0.3731f,0.6245f},
            {0.4441f,0.5547f},{0.5125f,0.4866f},{0.5752f,0.4242f},{0.6270f,0.3725f},
            {0.6658f,0.3340f},{0.6915f,0.3083f},{0.7079f,0.2920f},{0.7140f,0.2859f},
            {0.7100f,0.2900f},{0.6992f,0.3008f},{0.6916f,0.3083f},{0.6863f,0.3137f},
            {0.6830f,0.3170f},{0.6810f,0.3190f}
        };

        public FormLan8661CIE(Lan8661Ws2812Client client)
        {
            if (client == null) throw new ArgumentNullException(nameof(client));
            _client = client;
            InitializeComponent();

            cmbMode.SelectedIndex = 0;
            // Three logical lighting channels: existing WS2812, OSP and ISE.
            cmbFID.SelectedIndex = 0;
            nudColorX.Value = 78;  // D65 x=0.312
            nudColorY.Value = 82;  // D65 y=0.328
            nudFadeIn.Value = 0;
            nudFadeOut.Value = 0;
            chkNoFade.Checked = true;
            nudInterval.Value = 50;

            CheckBox[] shiftModes =
            {
                chkRGBWShift, chkLedShift, chkShiftStack, chkLedShiftUp,
                chkShiftStackUp, chkMove, chkEachControl
            };
            foreach (CheckBox mode in shiftModes)
                mode.CheckedChanged += ShiftMode_CheckedChanged;

            UpdateConnectionStatus();
            UpdateColorFromFields();
            RefreshPreview();
        }

        public void UpdateConnectionStatus()
        {
            bool connected = _client.IsConnected;
            lblFT4222Status.Text = connected
                ? string.Format("● LAN8661 {0} → {1}:{2}  {3}", _client.SourceIp,
                    _client.TargetIp, _client.RtpPort, _client.ControlPortSummary)
                : "● LAN8661 연결 안됨";
            bool ready = connected && _client.ControlPortsReady;
            lblFT4222Status.ForeColor = ready ? Color.LimeGreen : Color.OrangeRed;
            btnTxOnce.Enabled = ready;
            btnTxCycle.Enabled = ready;

            // Never imply that an open SERCOM port is a complete ISELED
            // implementation. ISELED needs the licensed RLC/CRC encoder and
            // initialization sequence in addition to the physical SPI port.
            bool ilasAvailable = connected && _client.IlasTransportReady &&
                _client.IsIlasProtocolAvailable;
            chkIlasTxEnable.Enabled = ilasAvailable;
            if (!ilasAvailable)
                chkIlasTxEnable.Checked = false;

            if (!connected)
                StopTransmission();
        }

        private void TxTimer_Tick(object sender, EventArgs e)
        {
            // Periodic frames use the fast write-only OSP path. The chain was
            // already read/verified on connect and on the first frame.
            TransmitOneFrame(false);
        }

        private void btnTxOnce_Click(object sender, EventArgs e)
        {
            if (!EnsureConnected()) return;
            // A one-shot must never leave a timer running or silently turn into
            // multiple frames. In a shift mode each click advances one step.
            StopTransmission();
            if (TransmitOneFrame(true))
                StatusMessage?.Invoke("[LAN8661] 선택 출력을 1회 송신했습니다.");
        }

        private void btnTxCycle_Click(object sender, EventArgs e)
        {
            if (txTimer.Enabled)
            {
                StopTransmission();
                StatusMessage?.Invoke("[LAN8661] 주기 송신 중지");
                return;
            }
            if (!EnsureConnected()) return;
            StartTransmission();
            if (txTimer.Enabled)
                StatusMessage?.Invoke(string.Format(
                    "[LAN8661] 선택 출력 주기 송신 시작 ({0} ms)", GetInterval()));
        }

        private void StartTransmission()
        {
            txTimer.Interval = GetInterval();
            _shiftStep = 0;
            btnTxCycle.Text = "■ 주기 TX 중지";
            btnTxCycle.BackColor = Color.FromArgb(165, 55, 55);

            // Send immediately once, then let only the periodic button repeat
            // at the exact interval entered in the left-hand control.
            if (TransmitOneFrame(true))
                txTimer.Start();
        }

        private void StopTransmission()
        {
            txTimer.Stop();
            btnTxCycle.Text = "▶ 주기 TX 시작";
            btnTxCycle.BackColor = Color.FromArgb(50, 110, 190);
        }

        private int GetInterval()
        {
            return Math.Max(1, Math.Min(60000, (int)nudInterval.Value));
        }

        private bool TransmitOneFrame(bool verifyOspReadback)
        {
            if (!_client.IsConnected)
            {
                UpdateConnectionStatus();
                return false;
            }

            bool sendOsp = ShouldSendOsp();
            bool sendIlas = ShouldSendIlas();
            string error;
            if (!_client.PrepareOutputChains(sendOsp, sendIlas, out error))
            {
                HandleTransmitError(error);
                return false;
            }

            BuildFrame();
            byte[] ospColors = sendOsp ? BuildLinearColors(_client.OspNodeCount) : null;
            byte[] ilasColors = sendIlas ? BuildLinearColors(_client.IlasNodeCount) : null;
            Color auxiliary = ScaledSelectedColor();
            string warning;
            if (!_client.SendFrame(_frame, GetInterval(), GetWs2812TargetMask(),
                sendOsp, sendIlas, auxiliary.R, auxiliary.G, auxiliary.B,
                ospColors, ilasColors, verifyOspReadback, out error, out warning))
            {
                HandleTransmitError(error);
                return false;
            }

            if (!string.IsNullOrEmpty(warning))
            {
                lblFT4222Status.Text = "● " + warning;
                lblFT4222Status.ForeColor = Color.Khaki;
                if (!_ilasWarningReported)
                {
                    StatusMessage?.Invoke("[LAN8661] " + warning);
                    _ilasWarningReported = true;
                }
            }
            else
            {
                _ilasWarningReported = false;
                UpdateConnectionStatus();
            }

            if (IsShiftModeActive())
                _shiftStep++;
            RefreshPreview();
            return true;
        }

        private void HandleTransmitError(string error)
        {
            StopTransmission();
            lblFT4222Status.Text = "● 송신 오류: " + error;
            lblFT4222Status.ForeColor = Color.OrangeRed;
            StatusMessage?.Invoke("[LAN8661 TX 오류] " + error);
        }

        private bool EnsureConnected()
        {
            if (_client.IsConnected && _client.ControlPortsReady) return true;
            UpdateConnectionStatus();
            MessageBox.Show("메인 화면에서 LAN8661을 먼저 연결하세요.", "LAN8661",
                MessageBoxButtons.OK, MessageBoxIcon.Information);
            return false;
        }

        private void BuildFrame()
        {
            Array.Clear(_frame, 0, _frame.Length);
            if (GetWs2812TargetMask() == 0) return;

            // The physical First panel is a 100-LED serial chain. Use the same
            // one-dimensional effect sequence as OSP/ISE, then map it to the
            // 10x10 canvas in the exact order used by the SPI encoder.
            byte[] linear = BuildLinearColors(Lan8661Ws2812Client.ExistingLedCount);
            for (int index = 0; index < Lan8661Ws2812Client.ExistingLedCount; index++)
            {
                int source = index * 3;
                SetPixel(index % Lan8661Ws2812Client.ExistingLedWidth,
                    index / Lan8661Ws2812Client.ExistingLedWidth,
                    Color.FromArgb(linear[source], linear[source + 1], linear[source + 2]));
            }
        }

        private byte[] BuildLinearColors(int ledCount)
        {
            if (ledCount <= 0) return null;

            byte[] colors = new byte[ledCount * 3];
            Color selected = ScaledSelectedColor();
            Color alternate = Color.FromArgb(selected.B, selected.R, selected.G);
            int width = Math.Max(1, Math.Min(ledCount, (int)nudShiftWidth.Value));
            int movingIndex = _shiftStep % ledCount;
            int bouncePeriod = ledCount <= 1 ? 1 : (ledCount - 1) * 2;
            int bounce = _shiftStep % bouncePeriod;
            if (bounce >= ledCount) bounce = bouncePeriod - bounce;
            int stackLocked;
            int stackMoving;
            GetForwardStackState(ledCount, _shiftStep, out stackLocked, out stackMoving);
            int centerLow = (ledCount - 1) / 2;
            int centerHigh = ledCount / 2;
            int centerDistance = _shiftStep % (centerLow + 1);
            int centerStackLocked;
            int centerStackDistance;
            GetCenterStackState(ledCount, _shiftStep,
                out centerStackLocked, out centerStackDistance);

            Color[] rgbw =
            {
                ScaleColor(Color.Red), ScaleColor(Color.Lime),
                ScaleColor(Color.Blue), ScaleColor(Color.White)
            };

            for (int index = 0; index < ledCount; index++)
            {
                Color color;
                if (chkRGBWShift.Checked)
                    color = rgbw[((index + _shiftStep) / width) % rgbw.Length];
                else if (chkLedShift.Checked)
                    color = index == movingIndex ? selected : Color.Black;
                else if (chkShiftStack.Checked)
                    color = index == stackMoving || index >= ledCount - stackLocked ?
                        selected : Color.Black;
                else if (chkLedShiftUp.Checked)
                    color = index == centerLow - centerDistance ||
                        index == centerHigh + centerDistance ? selected : Color.Black;
                else if (chkShiftStackUp.Checked)
                    color = index < centerStackLocked ||
                        index >= ledCount - centerStackLocked ||
                        index == centerLow - centerStackDistance ||
                        index == centerHigh + centerStackDistance ? selected : Color.Black;
                else if (chkMove.Checked)
                    color = Math.Abs(index - bounce) < width ? selected : Color.Black;
                else if (chkEachControl.Checked)
                    color = (index & 1) == 0 ? selected : alternate;
                else
                    color = selected;

                int offset = index * 3;
                colors[offset] = color.R;
                colors[offset + 1] = color.G;
                colors[offset + 2] = color.B;
            }
            return colors;
        }

        private static void GetForwardStackState(int count, int step,
            out int locked, out int movingIndex)
        {
            int cycle = count * (count + 1) / 2;
            int phase = step % cycle;
            locked = 0;
            int movingCount = count;
            while (phase >= movingCount)
            {
                phase -= movingCount;
                locked++;
                movingCount--;
            }
            movingIndex = phase;
        }

        private static void GetCenterStackState(int count, int step,
            out int lockedPairs, out int distance)
        {
            int centerLow = (count - 1) / 2;
            int cycle = (centerLow + 1) * (centerLow + 2) / 2;
            int phase = step % cycle;
            lockedPairs = 0;
            int travelLength = centerLow + 1;
            while (phase >= travelLength)
            {
                phase -= travelLength;
                lockedPairs++;
                travelLength--;
            }
            distance = phase;
        }

        private int GetWs2812TargetMask()
        {
            return cmbFID.SelectedIndex == 0 || cmbFID.SelectedIndex == 1
                ? 1   // All or existing LED: Click 1/SERCOM0
                : 0;  // OSP or ISE only
        }

        private bool ShouldSendOsp()
        {
            return cmbFID.SelectedIndex == 0 || cmbFID.SelectedIndex == 2;
        }

        private bool ShouldSendIlas()
        {
            return cmbFID.SelectedIndex == 0 || cmbFID.SelectedIndex == 3;
        }

        private void SetPixel(int x, int y, Color color)
        {
            int offset = (y * Lan8661Ws2812Client.VideoWidth + x) * 3;
            _frame[offset] = color.R;
            _frame[offset + 1] = color.G;
            _frame[offset + 2] = color.B;
        }

        private Color ScaledSelectedColor()
        {
            return ScaleColor(CieRgb(_selectedXy.X, _selectedXy.Y));
        }

        private Color ScaleColor(Color color)
        {
            double scale = (double)nudBrightness.Value / 100.0;
            return Color.FromArgb(
                Clamp((int)Math.Round(color.R * scale)),
                Clamp((int)Math.Round(color.G * scale)),
                Clamp((int)Math.Round(color.B * scale)));
        }

        private bool IsShiftModeActive()
        {
            return chkRGBWShift.Checked || chkLedShift.Checked || chkShiftStack.Checked ||
                   chkLedShiftUp.Checked || chkShiftStackUp.Checked || chkMove.Checked;
        }

        private void ShiftMode_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox selected = sender as CheckBox;
            if (_changingShiftMode || selected == null) return;
            if (selected.Checked)
            {
                _changingShiftMode = true;
                CheckBox[] modes =
                {
                    chkRGBWShift, chkLedShift, chkShiftStack, chkLedShiftUp,
                    chkShiftStackUp, chkMove, chkEachControl
                };
                foreach (CheckBox mode in modes)
                    if (!ReferenceEquals(mode, selected)) mode.Checked = false;
                _changingShiftMode = false;
                _shiftStep = 0;
            }
            RefreshPreview();
        }

        private void RefreshPreview()
        {
            Color raw = CieRgb(_selectedXy.X, _selectedXy.Y);
            Color scaled = ScaleColor(raw);
            string target = cmbFID.SelectedItem == null ? "전체" : cmbFID.SelectedItem.ToString();
            string mode = GetShiftModeName();
            string osp = _client.OspTransportReady ?
                (_client.OspNodeCount > 0 ? "Direct/Read " + _client.OspNodeCount : "Port Ready") :
                "Port 없음";
            string ise = _client.IlasTransportReady ?
                (_client.IlasNodeCount > 0 ? "Read " + _client.IlasNodeCount : "Port Ready") :
                "Port 없음";
            txtPreview.Text = string.Format(
                "First SPI 10×10 | OSP={0} STATUS 0x{1:X2} | ISE={2}{3}",
                osp,
                _client.LastOspStatus,
                ise,
                _client.IsIlasProtocolAvailable ? "" : " (encoder 필요)");
            lblD1.Text = "Target     : " + target;
            lblD2.Text = string.Format("CIE xy     : x={0:F3}, y={1:F3}", _selectedXy.X, _selectedXy.Y);
            lblD3.Text = string.Format("Raw RGB    : {0}, {1}, {2}", raw.R, raw.G, raw.B);
            lblD4.Text = string.Format("Output RGB : {0}, {1}, {2}", scaled.R, scaled.G, scaled.B);
            lblD5.Text = string.Format("Brightness : {0}%", nudBrightness.Value);
            lblD6.Text = "Mode       : " + mode;
            lblD7.Text = string.Format("Interval   : {0} ms", GetInterval());
            lblD8.Text = txTimer.Enabled ? "TX Status  : Sending" : "TX Status  : Stopped";
        }

        private string GetShiftModeName()
        {
            if (chkRGBWShift.Checked) return "RGBW Shift";
            if (chkLedShift.Checked) return "Shift";
            if (chkShiftStack.Checked) return "Shift Stack";
            if (chkLedShiftUp.Checked) return "Shift Up";
            if (chkShiftStackUp.Checked) return "Shift Stack Up";
            if (chkMove.Checked) return "Move";
            if (chkEachControl.Checked) return "Each Control";
            return "Solid";
        }

        private void cmbFID_SelectedIndexChanged(object sender, EventArgs e)
        {
            _shiftStep = 0;
            RefreshPreview();
        }

        private void AnyField_Changed(object sender, EventArgs e)
        {
            if (!_updatingFields && (ReferenceEquals(sender, nudBrightness) ||
                ReferenceEquals(sender, nudFadeIn) || ReferenceEquals(sender, nudFadeOut) ||
                ReferenceEquals(sender, chkNoFade)))
                RefreshPreview();
        }

        private void nudColorX_ValueChanged(object sender, EventArgs e)
        {
            UpdateColorFromFields();
        }

        private void nudColorY_ValueChanged(object sender, EventArgs e)
        {
            UpdateColorFromFields();
        }

        private void UpdateColorFromFields()
        {
            if (_updatingFields) return;
            _selectedXy = new PointF((float)nudColorX.Value / 250f, (float)nudColorY.Value / 250f);
            _hasSelection = true;
            pnlColorPatch.BackColor = CieRgb(_selectedXy.X, _selectedXy.Y);
            lblSelXY.Text = string.Format("Selected  x={0:F3}  y={1:F3}", _selectedXy.X, _selectedXy.Y);
            pnlCIE.Invalidate();
            RefreshPreview();
        }

        private void ApplySelectedXy()
        {
            _updatingFields = true;
            nudColorX.Value = Math.Max(nudColorX.Minimum,
                Math.Min(nudColorX.Maximum, (decimal)Math.Round(_selectedXy.X * 250f)));
            nudColorY.Value = Math.Max(nudColorY.Minimum,
                Math.Min(nudColorY.Maximum, (decimal)Math.Round(_selectedXy.Y * 250f)));
            _updatingFields = false;
            pnlColorPatch.BackColor = CieRgb(_selectedXy.X, _selectedXy.Y);
            lblSelXY.Text = string.Format("Selected  x={0:F3}  y={1:F3}", _selectedXy.X, _selectedXy.Y);
            RefreshPreview();
        }

        private void CalcCieRect()
        {
            _cieRect = new Rectangle(44, 10,
                Math.Max(2, pnlCIE.ClientSize.Width - 54),
                Math.Max(2, pnlCIE.ClientSize.Height - 36));
        }

        private PointF CieToPixel(float x, float y)
        {
            return new PointF(_cieRect.Left + x * _cieRect.Width,
                _cieRect.Bottom - y * _cieRect.Height);
        }

        private PointF PixelToCie(int x, int y)
        {
            return new PointF(
                Math.Max(0f, Math.Min(1f, (x - _cieRect.Left) / (float)_cieRect.Width)),
                Math.Max(0f, Math.Min(1f, (_cieRect.Bottom - y) / (float)_cieRect.Height)));
        }

        private static PointF XyToUv(float x, float y)
        {
            float d = -2f * x + 12f * y + 3f;
            return Math.Abs(d) < 1e-6f ? new PointF(0.19f, 0.46f) :
                new PointF(4f * x / d, 9f * y / d);
        }

        private static PointF UvToXy(float u, float v)
        {
            float d = 6f * u - 16f * v + 12f;
            return Math.Abs(d) < 1e-6f ? new PointF(0.3127f, 0.3290f) :
                new PointF(9f * u / d, 4f * v / d);
        }

        private Color CieRgb(float x, float y)
        {
            if (y < 0.0001f) return Color.Black;
            float z = 1f - x - y;
            float bigX = x / y;
            float bigY = 1f;
            float bigZ = z / y;
            float r = 3.2406f * bigX - 1.5372f * bigY - 0.4986f * bigZ;
            float g = -0.9689f * bigX + 1.8758f * bigY + 0.0415f * bigZ;
            float b = 0.0557f * bigX - 0.2040f * bigY + 1.0570f * bigZ;
            return Color.FromArgb(
                Clamp((int)(GammaCorrect(r) * 255f)),
                Clamp((int)(GammaCorrect(g) * 255f)),
                Clamp((int)(GammaCorrect(b) * 255f)));
        }

        private static float GammaCorrect(float value)
        {
            if (value <= 0f) return 0f;
            if (value <= 0.0031308f) return 12.92f * value;
            return (float)(1.055 * Math.Pow(value, 1.0 / 2.4) - 0.055);
        }

        private static int Clamp(int value)
        {
            return value < 0 ? 0 : value > 255 ? 255 : value;
        }

        private PointF[] GetSpectralPixels()
        {
            int count = SpectralLocus.GetLength(0);
            PointF[] points = new PointF[count + 1];
            for (int i = 0; i < count; i++)
            {
                float x = SpectralLocus[i, 0];
                float y = SpectralLocus[i, 1];
                if (_is1976)
                {
                    PointF uv = XyToUv(x, y);
                    x = uv.X;
                    y = uv.Y;
                }
                points[i] = CieToPixel(x, y);
            }
            points[count] = points[0];
            return points;
        }

        private Bitmap BuildCieBitmap()
        {
            int width = _cieRect.Width;
            int height = _cieRect.Height;
            Bitmap bitmap = new Bitmap(width, height);
            PointF[] polygon = new PointF[SpectralLocus.GetLength(0)];
            for (int i = 0; i < polygon.Length; i++)
            {
                float x = SpectralLocus[i, 0];
                float y = SpectralLocus[i, 1];
                if (_is1976)
                {
                    PointF uv = XyToUv(x, y);
                    x = uv.X;
                    y = uv.Y;
                }
                polygon[i] = new PointF(x * width, (1f - y) * height);
            }

            using (GraphicsPath path = new GraphicsPath())
            using (Graphics graphics = Graphics.FromImage(bitmap))
            {
                path.AddPolygon(polygon);
                graphics.Clear(Color.FromArgb(10, 12, 20));
                for (int py = 0; py < height; py += 3)
                {
                    for (int px = 0; px < width; px += 3)
                    {
                        if (!path.IsVisible(px + 1, py + 1)) continue;
                        float x = px / (float)width;
                        float y = 1f - py / (float)height;
                        if (_is1976)
                        {
                            PointF xy = UvToXy(x, y);
                            x = xy.X;
                            y = xy.Y;
                        }
                        using (SolidBrush brush = new SolidBrush(CieRgb(x, y)))
                            graphics.FillRectangle(brush, px, py, 3, 3);
                    }
                }
            }
            return bitmap;
        }

        private void pnlCIE_Paint(object sender, PaintEventArgs e)
        {
            CalcCieRect();
            Graphics graphics = e.Graphics;
            graphics.SmoothingMode = SmoothingMode.AntiAlias;
            graphics.Clear(Color.FromArgb(10, 12, 20));

            Size size = new Size(_cieRect.Width, _cieRect.Height);
            if (_cieBitmap == null || _cieCacheSize != size)
            {
                if (_cieBitmap != null) _cieBitmap.Dispose();
                _cieBitmap = BuildCieBitmap();
                _cieCacheSize = size;
            }
            graphics.DrawImage(_cieBitmap, _cieRect.Left, _cieRect.Top);

            PointF[] spectral = GetSpectralPixels();
            using (Pen border = new Pen(Color.White, 1.2f))
                graphics.DrawLines(border, spectral);
            using (Pen grid = new Pen(Color.FromArgb(45, 180, 180, 200), 0.5f))
            {
                for (float value = 0.1f; value <= 1f; value += 0.1f)
                {
                    graphics.DrawLine(grid, _cieRect.Left + value * _cieRect.Width, _cieRect.Top,
                        _cieRect.Left + value * _cieRect.Width, _cieRect.Bottom);
                    graphics.DrawLine(grid, _cieRect.Left, _cieRect.Bottom - value * _cieRect.Height,
                        _cieRect.Right, _cieRect.Bottom - value * _cieRect.Height);
                }
            }

            if (_hasSelection)
            {
                float x = _selectedXy.X;
                float y = _selectedXy.Y;
                if (_is1976)
                {
                    PointF uv = XyToUv(x, y);
                    x = uv.X;
                    y = uv.Y;
                }
                PointF point = CieToPixel(x, y);
                using (Pen marker = new Pen(Color.White, 2f))
                {
                    graphics.DrawEllipse(marker, point.X - 8, point.Y - 8, 16, 16);
                    graphics.DrawLine(marker, point.X - 13, point.Y, point.X + 13, point.Y);
                    graphics.DrawLine(marker, point.X, point.Y - 13, point.X, point.Y + 13);
                }
            }
        }

        private void pnlCIE_Resize(object sender, EventArgs e)
        {
            if (_cieBitmap != null) _cieBitmap.Dispose();
            _cieBitmap = null;
            pnlCIE.Invalidate();
        }

        private void pnlCIE_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left) return;
            CalcCieRect();
            if (!_cieRect.Contains(e.Location)) return;
            PointF selected = PixelToCie(e.X, e.Y);
            if (_is1976) selected = UvToXy(selected.X, selected.Y);
            _selectedXy = new PointF(
                Math.Max(0f, Math.Min(1f, selected.X)),
                Math.Max(0f, Math.Min(1f, selected.Y)));
            _hasSelection = true;
            ApplySelectedXy();
            pnlCIE.Invalidate();
        }

        private void pnlCIE_MouseMove(object sender, MouseEventArgs e)
        {
            CalcCieRect();
            if (!_cieRect.Contains(e.Location))
            {
                lblCursor.Text = "Cursor: -";
                return;
            }
            PointF point = PixelToCie(e.X, e.Y);
            lblCursor.Text = _is1976
                ? string.Format("Cursor  u'={0:F3}  v'={1:F3}", point.X, point.Y)
                : string.Format("Cursor  x={0:F3}  y={1:F3}", point.X, point.Y);
        }

        private void cmbMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            _is1976 = cmbMode.SelectedIndex == 1;
            if (_cieBitmap != null) _cieBitmap.Dispose();
            _cieBitmap = null;
            lblCIEMode.Text = _is1976 ? "CIE 1976 u'v'" : "CIE 1931 xy";
            pnlCIE.Invalidate();
        }

        private void btnLoadImg_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog dialog = new OpenFileDialog())
            {
                dialog.Title = "이미지 파일 선택";
                dialog.Filter = "이미지|*.png;*.jpg;*.jpeg;*.bmp;*.gif|모든 파일|*.*";
                if (dialog.ShowDialog() != DialogResult.OK) return;
                try
                {
                    if (_imageBitmap != null) _imageBitmap.Dispose();
                    _imageBitmap = new Bitmap(dialog.FileName);
                    _imageSelection = PointF.Empty;
                    lblImgInfo.Text = string.Format("{0} ({1}×{2})", Path.GetFileName(dialog.FileName),
                        _imageBitmap.Width, _imageBitmap.Height);
                    pnlImage.Invalidate();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("이미지 로드 실패: " + ex.Message);
                }
            }
        }

        private Rectangle GetImageRectangle()
        {
            if (_imageBitmap == null) return Rectangle.Empty;
            float scale = Math.Min(pnlImage.ClientSize.Width / (float)_imageBitmap.Width,
                pnlImage.ClientSize.Height / (float)_imageBitmap.Height);
            int width = (int)(_imageBitmap.Width * scale);
            int height = (int)(_imageBitmap.Height * scale);
            return new Rectangle((pnlImage.ClientSize.Width - width) / 2,
                (pnlImage.ClientSize.Height - height) / 2, width, height);
        }

        private void pnlImage_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.Clear(Color.FromArgb(15, 15, 25));
            if (_imageBitmap == null)
            {
                using (Font font = new Font("Consolas", 9f))
                using (Brush brush = new SolidBrush(Color.FromArgb(90, 120, 150)))
                    e.Graphics.DrawString("이미지를 로드한 뒤 픽셀을 클릭하세요.", font, brush, 150, 280);
                return;
            }
            Rectangle rectangle = GetImageRectangle();
            e.Graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
            e.Graphics.DrawImage(_imageBitmap, rectangle);
            if (_imageSelection != PointF.Empty)
            {
                using (Pen marker = new Pen(Color.White, 2f))
                    e.Graphics.DrawEllipse(marker, _imageSelection.X - 8, _imageSelection.Y - 8, 16, 16);
            }
        }

        private void pnlImage_MouseClick(object sender, MouseEventArgs e)
        {
            if (_imageBitmap == null) return;
            Rectangle rectangle = GetImageRectangle();
            if (!rectangle.Contains(e.Location)) return;
            int px = Math.Max(0, Math.Min(_imageBitmap.Width - 1,
                (int)((e.X - rectangle.Left) / (float)rectangle.Width * _imageBitmap.Width)));
            int py = Math.Max(0, Math.Min(_imageBitmap.Height - 1,
                (int)((e.Y - rectangle.Top) / (float)rectangle.Height * _imageBitmap.Height)));
            Color color = _imageBitmap.GetPixel(px, py);
            float x;
            float y;
            RgbToXy(color.R, color.G, color.B, out x, out y);
            _selectedXy = new PointF(x, y);
            _hasSelection = true;
            _imageSelection = e.Location;
            lblImgColor.Text = string.Format("R={0} G={1} B={2} → x={3:F3} y={4:F3}",
                color.R, color.G, color.B, x, y);
            ApplySelectedXy();
            pnlImage.Invalidate();
            pnlCIE.Invalidate();
        }

        private void pnlImage_MouseMove(object sender, MouseEventArgs e)
        {
            if (_imageBitmap == null) return;
            Rectangle rectangle = GetImageRectangle();
            if (!rectangle.Contains(e.Location)) return;
            int px = Math.Max(0, Math.Min(_imageBitmap.Width - 1,
                (int)((e.X - rectangle.Left) / (float)rectangle.Width * _imageBitmap.Width)));
            int py = Math.Max(0, Math.Min(_imageBitmap.Height - 1,
                (int)((e.Y - rectangle.Top) / (float)rectangle.Height * _imageBitmap.Height)));
            Color color = _imageBitmap.GetPixel(px, py);
            lblImgColor.Text = string.Format("Cursor R={0} G={1} B={2} (px {3},{4})",
                color.R, color.G, color.B, px, py);
        }

        private static void RgbToXy(int r, int g, int b, out float x, out float y)
        {
            float red = InverseGamma(r / 255f);
            float green = InverseGamma(g / 255f);
            float blue = InverseGamma(b / 255f);
            float bigX = 0.4124f * red + 0.3576f * green + 0.1805f * blue;
            float bigY = 0.2126f * red + 0.7152f * green + 0.0722f * blue;
            float bigZ = 0.0193f * red + 0.1192f * green + 0.9505f * blue;
            float sum = bigX + bigY + bigZ;
            if (sum < 1e-6f)
            {
                x = 0.3127f;
                y = 0.3290f;
                return;
            }
            x = Math.Max(0f, Math.Min(1f, bigX / sum));
            y = Math.Max(0f, Math.Min(1f, bigY / sum));
        }

        private static float InverseGamma(float value)
        {
            return value <= 0.04045f ? value / 12.92f :
                (float)Math.Pow((value + 0.055) / 1.055, 2.4);
        }

        private void btnSFAll_Click(object sender, EventArgs e) { }
        private void btnSFNone_Click(object sender, EventArgs e) { }
        private void FadeTimer_Tick(object sender, EventArgs e) { }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            StopTransmission();
            if (_cieBitmap != null) _cieBitmap.Dispose();
            if (_imageBitmap != null) _imageBitmap.Dispose();
            base.OnFormClosed(e);
        }
    }
}
