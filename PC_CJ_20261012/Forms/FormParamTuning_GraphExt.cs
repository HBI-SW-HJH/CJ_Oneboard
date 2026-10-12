using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using LINMaster.LIN;

// ============================================================================
// FormParamTuning – DAQ 그래프 기능 (partial class)
// Designer에서 편집 가능하도록 컨트롤은 Designer.cs에 선언
// 이 파일에는 데이터/로직/렌더링만 포함
// ============================================================================

namespace LINMaster.Forms
{
    public partial class FormParamTuning : Form
    {
        // ====================================================================
        // 그래프 상수 & 데이터 구조
        // ====================================================================
        private const int GRAPH_MAX_POINTS   = 300;
        private const int GRAPH_COLORS_COUNT = 8;

        private class GraphSeries
        {
            public string        Name      { get; set; }
            public MapSymbol     Symbol    { get; set; }
            public List<double>  Values    { get; set; } = new List<double>();
            public List<DateTime>Times     { get; set; } = new List<DateTime>();
            public Color         LineColor { get; set; }
            public double        LastValue { get; set; } = double.NaN;
        }

        private List<GraphSeries>              _graphSeries  = new List<GraphSeries>();
        private System.Windows.Forms.Timer     _graphTimer;
        private bool                           _graphRunning = false;
        private int                            _favGraphIdx  = 0;
        private bool                           _graphFavMode = false;
        private bool                           _graphTickBusy = false;  // 재진입 방지

        private static readonly Color[] _seriesColors = new Color[]
        {
            Color.Lime, Color.Cyan, Color.Orange, Color.HotPink,
            Color.Yellow, Color.SkyBlue, Color.Salmon, Color.MediumPurple
        };

        // ====================================================================
        // chkGraphAutoScale 이벤트 핸들러 (Designer 이벤트 탭에서 연결됨)
        // ====================================================================
        private void chkGraphAutoScale_CheckedChanged(object sender, EventArgs e)
        {
            bool auto = chkGraphAutoScale.Checked;
            txtGraphYMin.Enabled = !auto;
            txtGraphYMax.Enabled = !auto;
            pnlGraph.Refresh();
        }

        private void chkGraphXAutoScale_CheckedChanged(object sender, EventArgs e)
        {
            bool auto = chkGraphXAutoScale.Checked;
            txtGraphXMin.Enabled  = !auto;
            txtGraphXMax.Enabled  = !auto;
            lblGraphXMin.Enabled  = !auto;
            lblGraphXMax.Enabled  = !auto;
            pnlGraph.Refresh();
        }

        // ====================================================================
        // 그래프 버튼 이벤트 핸들러
        // ====================================================================
        private void CmbGraphVar_DropDown(object sender, EventArgs e)
        {
            cmbGraphVar.Items.Clear();
            foreach (var de in _daqEntries)
                if (!cmbGraphVar.Items.Contains(de.Symbol.Name))
                    cmbGraphVar.Items.Add(de.Symbol.Name);
            foreach (DataGridViewRow row in dgvFavorites.Rows)
                if (row.Tag is MapSymbol s && !cmbGraphVar.Items.Contains(s.Name))
                    cmbGraphVar.Items.Add(s.Name);
        }

        private void BtnGraphAdd_Click(object sender, EventArgs e)
        {
            MapSymbol sym = ResolveGraphSymbol(cmbGraphVar.Text);
            if (sym == null) { SetStatus("그래프: 변수를 선택하세요"); return; }
            AddGraphSeries(sym);
        }

        private void BtnGraphFavStart_Click(object sender, EventArgs e)
        {
            if (_favorites.Count == 0) { SetStatus("즐겨찾기가 비어 있습니다"); return; }
            _graphSeries.Clear();
            pnlGraph.Refresh();
            foreach (var fav in _favorites)
                AddGraphSeries(fav);
            _graphFavMode = true;
            _favGraphIdx  = 0;
            StartGraph();
        }

        private void AddGraphSeries(MapSymbol sym)
        {
            if (_graphSeries.Any(g => g.Symbol.Name == sym.Name && g.Symbol.Address == sym.Address))
            { SetStatus($"그래프: {sym.Name} 이미 추가됨"); return; }
            Color col = _seriesColors[_graphSeries.Count % _seriesColors.Length];
            _graphSeries.Add(new GraphSeries { Name = sym.Name, Symbol = sym, LineColor = col });
            SetStatus($"그래프 추가: {sym.Name}  (총 {_graphSeries.Count}개)");
            pnlGraph.Refresh();
        }

        private MapSymbol ResolveGraphSymbol(string name)
        {
            if (string.IsNullOrEmpty(name)) return null;
            var fromDaq = _daqEntries.FirstOrDefault(d => d.Symbol.Name == name)?.Symbol;
            if (fromDaq != null) return fromDaq;
            foreach (DataGridViewRow row in dgvFavorites.Rows)
                if (row.Tag is MapSymbol s && s.Name == name) return s;
            return null;
        }

        private void BtnGraphStart_Click(object sender, EventArgs e) => StartGraph();

        private void StartGraph()
        {
            if (_gw == null || !_gw.IsConnected) { SetStatus("그래프: 연결 안됨"); return; }
            if (_graphSeries.Count == 0) { SetStatus("그래프: 변수를 먼저 추가하세요"); return; }

            // 재시작 시 이전 데이터 초기화 → 처음부터 다시 그림
            foreach (var gs in _graphSeries)
            {
                gs.Values.Clear();
                gs.Times.Clear();
                gs.LastValue = double.NaN;
            }
            _favGraphIdx = 0;

            _graphRunning            = true;
            _graphTimer.Interval     = (int)nudGraphInterval.Value;
            _graphTimer.Start();
            btnGraphStart.Enabled    = false;
            btnGraphStop.Enabled     = true;
            lblGraphStatus.Text      = $"▶ 그래프 수집 중  ({_graphSeries.Count}개 변수, 주기: {_graphTimer.Interval}ms)";
            lblGraphStatus.ForeColor = Color.Lime;
            SetStatus("그래프 시작");
        }

        private void BtnGraphStop_Click(object sender, EventArgs e)
        {
            _graphTimer.Stop();
            _graphRunning            = false;
            _graphFavMode            = false;
            btnGraphStart.Enabled    = true;
            btnGraphStop.Enabled     = false;
            lblGraphStatus.Text      = "■ 정지됨  (재시작 시 처음부터 다시 그립니다)";
            lblGraphStatus.ForeColor = Color.OrangeRed;
            SetStatus("그래프 정지");

            // 수집된 데이터 초기화 → 재시작 시 처음부터 다시 그림
            foreach (var gs in _graphSeries)
            {
                gs.Values.Clear();
                gs.Times.Clear();
                gs.LastValue = double.NaN;
            }
            _favGraphIdx = 0;
            pnlGraph.Refresh();
        }

        private void BtnGraphClear_Click(object sender, EventArgs e)
        {
            _graphTimer.Stop();
            _graphRunning            = false;
            _graphFavMode            = false;
            _graphSeries.Clear();
            btnGraphStart.Enabled    = true;
            btnGraphStop.Enabled     = false;
            lblGraphStatus.Text      = "초기화됨";
            lblGraphStatus.ForeColor = Color.Gray;
            pnlGraph.Refresh();
        }

        // ====================================================================
        // 그래프 타이머 Tick  (async void – UI 스레드 블로킹 없음)
        // ====================================================================
        private async void GraphTimer_Tick(object sender, EventArgs e)
        {
            // 이전 Tick이 아직 실행 중이면 건너뜀 (재진입 방지)
            if (_graphTickBusy) return;
            // 폼이 닫히는 중이면 타이머만 멈추고 반환
            if (IsDisposed || Disposing)
            {
                _graphTimer?.Stop();
                _graphRunning = false;
                return;
            }
            _graphTickBusy = true;
            try
            {
                if (!_graphRunning || _gw == null || !_gw.IsConnected)
                {
                    _graphTimer.Stop();
                    _graphRunning = false;
                    if (!IsDisposed && !Disposing)
                    {
                        btnGraphStart.Enabled    = true;
                        btnGraphStop.Enabled     = false;
                        lblGraphStatus.Text      = "■ 연결 끊김 – 정지됨  (재시작 시 처음부터 다시 그립니다)";
                        lblGraphStatus.ForeColor = Color.OrangeRed;

                        // 데이터 초기화 → 재시작 시 처음부터 다시 그림
                        foreach (var gs in _graphSeries)
                        {
                            gs.Values.Clear();
                            gs.Times.Clear();
                            gs.LastValue = double.NaN;
                        }
                        _favGraphIdx = 0;
                        pnlGraph.Refresh();
                    }
                    return;
                }

                if (_graphFavMode && _graphSeries.Count > 0)
                {
                    int idx = _favGraphIdx % _graphSeries.Count;
                    await ReadGraphPointAsync(_graphSeries[idx]).ConfigureAwait(true);
                    _favGraphIdx++;
                }
                else
                {
                    foreach (var gs in _graphSeries)
                        await ReadGraphPointAsync(gs).ConfigureAwait(true);
                }
                pnlGraph.Refresh();
            }
            finally { _graphTickBusy = false; }
        }

        private async Task ReadGraphPointAsync(GraphSeries gs)
        {
            byte nad = GetNAD();
            double val = double.NaN;

            if (UseXcpMode)
            {
                var (ok, xcpVal) = await XcpShortUploadAsync(gs.Symbol.Address, gs.Symbol.Size)
                                        .ConfigureAwait(true);
                if (ok) val = xcpVal;
            }
            else
            {
                byte[] payload = BuildReadRam(nad, gs.Symbol.Address);
                LINFrame rx = await SendDiagWithRetryAsync(payload).ConfigureAwait(true);
                if (rx != null)
                {
                    uint raw = (uint)(rx.Data[4] | rx.Data[5] << 8 | rx.Data[6] << 16 | rx.Data[7] << 24);
                    val = MaskValueForSize(raw, gs.Symbol.Size);
                }
            }

            if (!double.IsNaN(val))
            {
                gs.LastValue = val;
                gs.Values.Add(val);
                gs.Times.Add(DateTime.Now);
                int maxPoints = 300; // 기본값 300
                int.TryParse(txtGraphXMax?.Text, out maxPoints);

                if (gs.Values.Count > maxPoints)
                {
                    gs.Values.RemoveAt(0);
                    gs.Times.RemoveAt(0);
                }
                // 폼이 닫히는 중이면 UI 컨트롤 접근 건너뜀
                if (!IsDisposed && !Disposing)
                //if (_graphRunning)
                {
                    foreach (DataGridViewRow row in dgvDaq.Rows)
                        if (row.Tag is DaqEntry de && de.Symbol.Name == gs.Symbol.Name)
                        { row.Cells["daqValue"].Value = $"0x{(uint)val:X}"; break; }
                    foreach (DataGridViewRow row in dgvFavorites.Rows)
                        if (row.Tag is MapSymbol ms && ms.Name == gs.Symbol.Name)
                        { row.Cells["favColValue"].Value = $"0x{(uint)val:X}"; break; }
                }
            }
        }

        // ====================================================================
        // 그래프 렌더링 (Paint 이벤트)
        // ====================================================================
        private void PnlGraph_Paint(object sender, PaintEventArgs e)
        {
            var rc  = pnlGraph.ClientRectangle;

            // ── 오프스크린 버퍼에 그린 뒤 한 번에 화면에 복사 (깜빡임 제거) ──
            using (var ctx = BufferedGraphicsManager.Current.Allocate(e.Graphics, rc))
            {
                DrawGraph(ctx.Graphics, rc);
                ctx.Render(e.Graphics);
            }
        }

        private void DrawGraph(Graphics g, Rectangle rc)
        {
            int padL = 58, padR = 12, padT = 12, padB = 30;
            int plotW = rc.Width  - padL - padR;
            int plotH = rc.Height - padT - padB;

            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            using (var bgBrush = new SolidBrush(Color.FromArgb(15, 18, 24)))
                g.FillRectangle(bgBrush, rc);

            using (var gridPen = new Pen(Color.FromArgb(40, 45, 55), 1))
            {
                for (int i = 0; i <= 5; i++)
                {
                    int y = padT + (int)(plotH * i / 5.0);
                    g.DrawLine(gridPen, padL, y, padL + plotW, y);
                }
                for (int i = 0; i <= 10; i++)
                {
                    int x = padL + (int)(plotW * i / 10.0);
                    g.DrawLine(gridPen, x, padT, x, padT + plotH);
                }
            }
            using (var borderPen = new Pen(Color.FromArgb(60, 70, 85), 1))
                g.DrawRectangle(borderPen, padL, padT, plotW, plotH);

            if (_graphSeries.Count == 0)
            {
                using (var f = new Font("Consolas", 9f))
                    g.DrawString("변수를 추가하고 그래프를 시작하세요", f, Brushes.Gray, padL + 10, padT + plotH / 2 - 8);
                return;
            }

            // ── Y축 범위 ────────────────────────────────────────────────
            double yMin = 0, yMax = 1;
            if (chkGraphAutoScale != null && chkGraphAutoScale.Checked)
            {
                bool first = true;
                foreach (var gs in _graphSeries)
                    foreach (double v in gs.Values)
                    {
                        if (first) { yMin = yMax = v; first = false; }
                        else { if (v < yMin) yMin = v; if (v > yMax) yMax = v; }
                    }
                if (Math.Abs(yMax - yMin) < 1e-9) { yMin -= 1; yMax += 1; }
                double margin = (yMax - yMin) * 0.05;
                yMin -= margin; yMax += margin;
            }
            else
            {
                double.TryParse(txtGraphYMin?.Text, out yMin);
                double.TryParse(txtGraphYMax?.Text, out yMax);
                if (yMax <= yMin) yMax = yMin + 1;
            }

            // ── X축 범위 ────────────────────────────────────────────────
            // 전체 포인트 수 중 보여줄 구간 [xStart, xEnd) 결정
            int totalPts = _graphSeries.Count > 0 ? _graphSeries.Max(s => s.Values.Count) : 0;
            int xStart, xEnd;
            bool xAuto = chkGraphXAutoScale == null || chkGraphXAutoScale.Checked;
            if (xAuto)
            {
                xStart = 0;
                xEnd   = Math.Max(totalPts, 2);
            }
            else
            {
                int.TryParse(txtGraphXMin?.Text, out xStart);
                int.TryParse(txtGraphXMax?.Text, out xEnd);
                if (xEnd <= xStart) xEnd = xStart + 1;
                xStart = Math.Max(0, xStart);
                xEnd   = Math.Min(xEnd, Math.Max(totalPts, xEnd));
            }
            int xRange = Math.Max(xEnd - xStart, 1);

            using (var axFont = new Font("Consolas", 7.5f))
            using (var axBrush = new SolidBrush(Color.FromArgb(150, 160, 180)))
            {
                // Y축 레이블
                for (int i = 0; i <= 5; i++)
                {
                    double val = yMax - (yMax - yMin) * i / 5.0;
                    int    y   = padT + (int)(plotH * i / 5.0);
                    string lbl = val >= 1e6 ? $"{val/1e6:F1}M" : val >= 1e3 ? $"{val/1e3:F1}K" : $"{val:F1}";
                    var sz = g.MeasureString(lbl, axFont);
                    g.DrawString(lbl, axFont, axBrush, padL - sz.Width - 2, y - sz.Height / 2);
                }
                // X축 레이블 (포인트 인덱스 또는 샘플 번호)
                for (int i = 0; i <= 10; i++)
                {
                    int sampleIdx = xStart + (int)(xRange * i / 10.0);
                    int px = padL + (int)(plotW * i / 10.0);
                    string lbl = sampleIdx.ToString();
                    var sz = g.MeasureString(lbl, axFont);
                    g.DrawString(lbl, axFont, axBrush, px - sz.Width / 2, padT + plotH + 2);
                }
            }

            foreach (var gs in _graphSeries)
            {
                if (gs.Values.Count < 2) continue;
                var pts = new List<PointF>();
                for (int i = 0; i < gs.Values.Count; i++)
                {
                    int absIdx = i; // Values 내 절대 인덱스
                    if (absIdx < xStart || absIdx >= xEnd) continue;
                    float x = padL + (float)(plotW * (absIdx - xStart) / (double)xRange);
                    float y = padT + (float)(plotH * (1.0 - (gs.Values[i] - yMin) / (yMax - yMin)));
                    y = Math.Max(padT, Math.Min(padT + plotH, y));
                    pts.Add(new PointF(x, y));
                }
                if (pts.Count < 2) continue;
                using (var pen = new Pen(gs.LineColor, 1.5f))
                    g.DrawLines(pen, pts.ToArray());
                var last = pts[pts.Count - 1];
                using (var lf = new Font("Consolas", 8f, FontStyle.Bold))
                using (var lb = new SolidBrush(gs.LineColor))
                    g.DrawString($"{gs.Name}: {gs.LastValue:F1}", lf, lb,
                        Math.Max(padL, last.X - 50), Math.Max(padT, last.Y - 14));
            }

            using (var legFont = new Font("Consolas", 7.5f))
            {
                int lx = padL + 4, ly = padT + 4;
                foreach (var gs in _graphSeries)
                {
                    using (var lb = new SolidBrush(gs.LineColor))
                    using (var pen = new Pen(gs.LineColor, 2))
                    {
                        g.DrawLine(pen, lx, ly + 6, lx + 16, ly + 6);
                        g.DrawString(gs.Name, legFont, lb, lx + 20, ly);
                    }
                    ly += 16;
                    if (ly > padT + plotH - 16) break;
                }
            }
        }
    }
}
