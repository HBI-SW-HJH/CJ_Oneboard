using System;
using System.Collections.Generic;
using System.Drawing;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Text;
using System.Windows.Forms;
using LINMaster.LIN;
using QRCoder;

namespace LINMaster.Forms
{
    public partial class FormUDS : Form
    {
        private const int ResponseTimeoutMs = 250;
        private const int DiscoveryResponseTimeoutMs = 150;
        private bool _busy;
        private bool _scanning;
        private bool _verifying;
        internal bool VerificationActive { get { return _verifying || _updating; } }
        private bool _cancelVerification;
        private int _responseDropCount;
        private bool _downloadLogEnabled;
        private int _logDisplayOffset;
        private readonly StringBuilder _log = new StringBuilder();

        private readonly ILINGateway _gateway;
        private readonly Queue<LINFrame> _responses = new Queue<LINFrame>();
        private readonly Queue<LINFrame> _verificationResponses = new Queue<LINFrame>();
        private readonly object _responseLock = new object();
        private readonly List<UdsNode> _nodes = new List<UdsNode>();

        public FormUDS(ILINGateway gateway)
        {
            _gateway = gateway;
            InitializeComponent();
            if (_gateway != null) _gateway.FrameReceived += Gateway_FrameReceived;
            UpdateConnectionState();
        }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            if (_gateway != null) _gateway.FrameReceived -= Gateway_FrameReceived;
            Image oldQr = picProductQr.Image;
            picProductQr.Image = null;
            if (oldQr != null) oldQr.Dispose();
            base.OnFormClosed(e);
        }

        private void Gateway_FrameReceived(LINFrame frame)
        {
            if (frame.Direction == LINDirection.RX &&
                frame.FrameID == UDSProtocol.ResponseFrameId && frame.DataLength == 8)
            {
                lock (_responseLock)
                {
                    if (_responses.Count >= 32)
                    {
                        _responseDropCount++;
                        return;
                    }
                    _responses.Enqueue(frame);
                }
            }
            if (frame.Direction == LINDirection.RX && frame.FrameID >= CJVerificationProtocol.ResponseIdBase && frame.FrameID <= CJVerificationProtocol.ResponseIdBase + 3 && frame.DataLength == 8)
            {
                lock (_responseLock)
                {
                    if (_verificationResponses.Count < 32)
                    {
                        _verificationResponses.Enqueue(frame);
                    }
                }
            }
        }

        private void UpdateConnectionState()
        {
            bool connected = _gateway != null && _gateway.IsConnected;
            lblConnection.Text = connected ? "● LIN 연결됨" : "● LIN 연결 안됨";
            lblConnection.ForeColor = connected ? Color.LimeGreen : Color.OrangeRed;
            UpdateXcpButton();
        }

        private void UpdateXcpButton()
        {
            UdsNode selected = SelectedNode();
            if (_updating)
            {
                return;
            }
            btnUpdate.Enabled = !_scanning && !_verifying && !_readingStatus && _gateway != null && _gateway.IsConnected && selected != null && (selected.Product != null || selected.ExecutionMode == 1);
            btnXcp.Enabled = btnUpdate.Enabled && selected.Product != null && selected.ExecutionMode != 1;
            btnVerification.Enabled = _verifying || btnXcp.Enabled;
            btnWriteProduct.Enabled = btnXcp.Enabled && selected.ExecutionMode != 1;
            if ((selected != null) && (selected.ExecutionMode == 1))
            {
                btnXcp.Enabled = false;
                btnVerification.Enabled = false;
            }
            if (!btnXcp.Enabled)
            {
                return;
            }
            foreach (UdsNode node in _nodes)
            {
                if (node != selected && node.ChairId == selected.ChairId)
                {
                    btnXcp.Enabled = false;
                    btnVerification.Enabled = false;
                    return;
                }
            }
        }

        private void btnScan_Click(object sender, EventArgs e)
        {
            if (!CheckConnected())
            {
                return;
            }

            _nodes.Clear();
            lstNodes.Items.Clear();
            Image oldQr = picProductQr.Image;
            picProductQr.Image = null;
            if (oldQr != null) oldQr.Dispose();
            lblQrInfo.Text = "RX 정보 대기";
            _scanning = true;
            btnXcp.Enabled = false;
            SetStatus(lblScanStatus, "검색 중 0/16", Color.Yellow);
            btnScan.Enabled = false;
            try
            {
                for (int pass = 0; (pass < 2) && (_nodes.Count < 4); pass++)
                {
                    for (byte slot = 0; slot < UDSProtocol.DiscoverySlotCount; slot++)
                    {
                        lblScanStatus.Text = pass == 0 ? string.Format("{0}/16 검색 중", slot + 1) : string.Format("재검색 {0}/16", slot + 1);
                        UDSProtocol.ProductInfo info;
                        uint initNumber;
                        byte[] response = SendService(UDSProtocol.BroadcastNad, UDSProtocol.BuildDiscoveryResult(slot), DiscoveryResponseTimeoutMs, false);
                        if (!UDSProtocol.TryParseTargetRead(response, UDSProtocol.DiscoveryRoutine, out info))
                        {
                            if (!UDSProtocol.TryParseBootDiscovery(response, out initNumber))
                            {
                                continue;
                            }
                        }
                        else
                        {
                            initNumber = info.InitNumber;
                        }
                        int index = _nodes.FindIndex(node => node.InitNumber == initNumber);
                        if (index < 0)
                        {
                            _nodes.Add(new UdsNode { Nad = info == null ? UDSProtocol.BootNad : info.Nad, ChairId = info == null ? (byte)0 : info.Nad, InitNumber = initNumber, Product = info, ExecutionMode = info == null ? 1 : -1 });
                        }
                        else
                        {
                            _nodes[index].Nad = info == null ? UDSProtocol.BootNad : info.Nad;
                            _nodes[index].ChairId = info == null ? (byte)0 : info.Nad;
                            _nodes[index].Product = info;
                            _nodes[index].ExecutionMode = info == null ? 1 : -1;
                        }
                    }
                }

                _nodes.Sort((left, right) => left.ChairId != right.ChairId ? left.ChairId.CompareTo(right.ChairId) : left.InitNumber.CompareTo(right.InitNumber));
                foreach (UdsNode node in _nodes)
                {
                    ReadBoardStatus(node);
                    lstNodes.Items.Add(NodeText(node));
                }

                if (_nodes.Count == 0)
                {
                    SetStatus(lblScanStatus, "검색 완료: 0개 (응답 없음)", Color.OrangeRed);
                }
                else
                {
                    lstNodes.SelectedIndex = 0;
                    SetStatus(lblScanStatus, "검색 완료: " + _nodes.Count + "개", Color.LimeGreen);
                }
            }
            finally
            {
                btnScan.Enabled = true;
                _scanning = false;
                UpdateXcpButton();
            }
        }

        private void lstNodes_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = lstNodes.SelectedIndex;
            if ((index < 0) || (index >= _nodes.Count))
            {
                return;
            }

            if (_nodes[index].Product != null)
            {
                ShowProduct(_nodes[index].Product);
            }
            else
            {
                txtSerial.Text = "";
                txtPart.Text = "";
                txtVersion.Text = "";
                txtInitNumber.Text = string.Format("0x{0:X8}", _nodes[index].InitNumber);
                Image oldQr = picProductQr.Image;
                picProductQr.Image = null;
                if (oldQr != null) oldQr.Dispose();
                lblQrInfo.Text = "BOOT: 제품정보는 App 실행 후 확인";
            }
            nudChairNumber.Enabled = _nodes[index].Product != null;
            if (!_scanning && !_busy && !_verifying && !_updating && !_readingStatus)
            {
                ReadBoardStatus(_nodes[index]);
            }
            txtRuntimeStatus.Text = _nodes[index].RuntimeStatus;
            UpdateXcpButton();
        }

        private byte[] SendService(byte nad, byte[] request, int responseTimeoutMs = ResponseTimeoutMs, bool showTimeout = true)
        {
            if (_busy)
            {
                return null;
            }
            _busy = true;
            try
            {
                _gateway.Poll();
                lock (_responseLock)
                {
                    _responses.Clear();
                }
                var decoder = new UDSProtocol.Decoder(nad);
                foreach (byte[] data in UDSProtocol.Encode(nad, request))
                {
                    if (_updating && _cancelUpdate)
                    {
                        throw new OperationCanceledException();
                    }
                    LINFrame frame = new LINFrame
                    {
                        FrameID = UDSProtocol.RequestFrameId,
                        DataLength = 8,
                        FrameType = LINFrameType.Publisher,
                        ChecksumType = LINChecksumType.Classic
                    };
                    data.CopyTo(frame.Data, 0);
                    if (!_gateway.SendPublisher(frame))
                    {
                        return null;
                    }
                    AppendWire("TX", frame.FrameID, frame.Data);
                    Delay(_updating ? 10 : 20);
                }
                Stopwatch timer = Stopwatch.StartNew();
                long lastReceived = 0;
                long nextHeader = 0;
                while ((timer.ElapsedMilliseconds - lastReceived < responseTimeoutMs) && (timer.ElapsedMilliseconds < Math.Max(4000, responseTimeoutMs + 1000)))
                {
                    if (_updating && _cancelUpdate)
                    {
                        throw new OperationCanceledException();
                    }
                    _gateway.Poll();
                    LINFrame response = null;
                    lock (_responseLock)
                    {
                        if (_responses.Count > 0)
                        {
                            response = _responses.Dequeue();
                        }
                    }
                    if (response != null)
                    {
                        byte[] payload;
                        AppendWire("RX", response.FrameID, response.Data);
                        if (response.Data[0] == nad)
                        {
                            lastReceived = timer.ElapsedMilliseconds;
                        }
                        if (decoder.Feed(response, out payload))
                        {
                            if ((payload.Length == 3) && (payload[0] == 0x7F) && (payload[1] == request[0]))
                            {
                                if (payload[2] == 0x78)
                                {
                                    lastReceived = timer.ElapsedMilliseconds;
                                    continue;
                                }
                                return payload;
                            }
                            if (ServiceMatches(request, payload))
                            {
                                return payload;
                            }
                        }
                    }
                    if (timer.ElapsedMilliseconds >= nextHeader)
                    {
                        if (!_gateway.SendSubscriberRequest(UDSProtocol.ResponseFrameId, 8, LINChecksumType.Classic))
                        {
                            return null;
                        }
                        AppendLog("REQ  55 7D  (Master Header / Classic response)", Color.FromArgb(120, 190, 255));
                        nextHeader = timer.ElapsedMilliseconds + 20;
                    }
                    Application.DoEvents();
                    Thread.Sleep(1);
                }
                if (showTimeout)
                {
                    AppendLog(string.Format("응답 시간 초과 / RX Queue Drop {0}", _responseDropCount), Color.OrangeRed);
                }
                return null;
            }
            finally
            {
                _busy = false;
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (_busy || _scanning || _verifying || _updating || _readingStatus)
            {
                e.Cancel = true;
            }
            base.OnFormClosing(e);
        }

        private void btnWriteProduct_Click(object sender, EventArgs e)
        {
            if (!CheckConnected())
            {
                return;
            }
            UDSProtocol.ProductInfo info;
            UdsNode node = SelectedNode();
            if (node == null || node.Product == null || node.ExecutionMode == 1)
            {
                SetStatus(lblProductStatus, "먼저 Scan 후 보드를 선택하세요.", Color.OrangeRed);
                return;
            }
            try
            {
                info = new UDSProtocol.ProductInfo
                {
                    SerialNumber = txtSerial.Text.Trim(),
                    PartNumber = txtPart.Text.Trim(),
                    Version = node.Product.Version,
                    ChairNumber = (byte)nudChairNumber.Value,
                    InitNumber = node.InitNumber
                };
                info.ToData();
            }
            catch (ArgumentException ex)
            {
                SetStatus(lblProductStatus, ex.Message, Color.OrangeRed);
                return;
            }
            SetStatus(lblProductStatus, "선택한 보드의 전체 정보 저장 중...", Color.Yellow);
            byte[] targetResponse = SendService(UDSProtocol.BroadcastNad, UDSProtocol.BuildTargetProductWrite(info));
            if (!UDSProtocol.IsWritePositiveResponse(targetResponse, UDSProtocol.TargetProductDid))
            {
                SetStatus(lblProductStatus, "선택한 보드의 저장 응답 없음 / 실패", Color.OrangeRed);
                return;
            }
            UDSProtocol.ProductInfo targetVerified;
            if ((!UDSProtocol.TryParseTargetRead(SendService(UDSProtocol.BroadcastNad, UDSProtocol.BuildTargetRead(node.InitNumber)), UDSProtocol.TargetReadRoutine, out targetVerified)) || (!System.Linq.Enumerable.SequenceEqual(info.ToData(), targetVerified.ToData())))
            {
                SetStatus(lblProductStatus, "저장 응답 수신 / 전체 값 재확인 실패", Color.OrangeRed);
                return;
            }
            node.Nad = targetVerified.Nad;
            node.ChairId = targetVerified.Nad;
            node.Product = targetVerified;
            ShowProduct(targetVerified);
            lstNodes.Items[lstNodes.SelectedIndex] = NodeText(node);
            SetStatus(lblProductStatus, "전체 정보 저장 및 재확인 완료", Color.LimeGreen);
            UpdateXcpButton();
        }

        private void btnXcp_Click(object sender, EventArgs e)
        {
            UpdateXcpButton();
            if (!btnXcp.Enabled)
            {
                return;
            }
            using (FormParamTuning tuning = new FormParamTuning(_gateway, SelectedNode().Product.ChairNumber))
            {
                tuning.ShowDialog(this);
            }
        }

        private void ShowProduct(UDSProtocol.ProductInfo info)
        {
            nudChairNumber.Enabled = true;
            txtSerial.Text = info.SerialNumber;
            txtPart.Text = info.PartNumber;
            txtVersion.Text = info.VersionText;
            txtInitNumber.Text = string.Format("0x{0:X8}", info.InitNumber);
            nudChairNumber.Value = info.ChairNumber;
            string qrText = string.Format("SN={0}\nPN={1}\nVER={2}\nCHAIR=0x{3:X2}\nINIT=0x{4:X8}", info.SerialNumber, info.PartNumber, info.VersionText, info.ChairNumber, info.InitNumber);
            using (var generator = new QRCodeGenerator())
            using (var qrData = generator.CreateQrCode(qrText, QRCodeGenerator.ECCLevel.M))
            using (var qrCode = new QRCode(qrData))
            {
                Image oldQr = picProductQr.Image;
                picProductQr.Image = qrCode.GetGraphic(5);
                if (oldQr != null) oldQr.Dispose();
            }
            lblQrInfo.Text = string.Format("Chair 0x{0:X2}  ·  Init {1:X8}", info.ChairNumber, info.InitNumber);
        }

        private static string NodeText(UdsNode node)
        {
            if (node.Product == null)
            {
                return string.Format("BOOT   Init {0:X8}   업데이트 가능", node.InitNumber);
            }
            return string.Format("ID 0x{0:X2}   Init {1:X8}   SN {2}", node.ChairId + 9, node.InitNumber,
                node.Product.SerialNumber.Substring(node.Product.SerialNumber.Length - 4));
        }

        private UdsNode SelectedNode()
        {
            int index = lstNodes.SelectedIndex;
            if ((index < 0) || (index >= _nodes.Count))
            {
                return null;
            }
            return _nodes[index];
        }

        private void AppendWire(string direction, byte frameId, byte[] data)
        {
            byte pid = LINFrame.CalcPID(frameId);
            byte checksum = LINFrame.CalcChecksumClassic(data, 8);
            AppendLog(string.Format("{0}  55 {1:X2}  {2}  {3:X2}", direction, pid,
                BitConverter.ToString(data).Replace('-', ' '), checksum),
                direction == "TX" ? Color.FromArgb(100, 210, 255) : Color.FromArgb(120, 240, 160));
        }

        private void AppendLog(string text, Color color)
        {
            string line = string.Format("[{0:HH:mm:ss.fff}] {1}\r\n", DateTime.Now, text);
            _log.Append(line);
            if (_updating)
            {
                _updateLog.Append(line);
                return;
            }
            rtbLog.SelectionStart = rtbLog.TextLength;
            rtbLog.SelectionColor = color;
            rtbLog.AppendText(line);
            rtbLog.ScrollToCaret();
            _logDisplayOffset = _log.Length;
        }

        private void timerLog_Tick(object sender, EventArgs e)
        {
            if (!_downloadLogEnabled || (_logDisplayOffset >= _log.Length))
            {
                return;
            }
            rtbLog.SelectionStart = rtbLog.TextLength;
            rtbLog.SelectionColor = Color.Gainsboro;
            rtbLog.AppendText(_log.ToString(_logDisplayOffset, _log.Length - _logDisplayOffset));
            rtbLog.ScrollToCaret();
            _logDisplayOffset = _log.Length;
        }

        private void btnToggleLog_Click(object sender, EventArgs e)
        {
            _downloadLogEnabled = !_downloadLogEnabled;
            btnToggleLog.Text = _downloadLogEnabled ? "로그: 활성화" : "로그: 비활성화";
            btnToggleLog.BackColor = _downloadLogEnabled ? Color.FromArgb(35, 125, 100) : Color.FromArgb(65, 72, 92);
            if (_downloadLogEnabled)
            {
                rtbLog.Text = _log.ToString();
                rtbLog.SelectionStart = rtbLog.TextLength;
                rtbLog.ScrollToCaret();
                _logDisplayOffset = _log.Length;
            }
            timerLog.Enabled = _updating && _downloadLogEnabled;
        }

        private void btnSaveLog_Click(object sender, EventArgs e)
        {
            if (_busy || _scanning || _verifying || _updating || _readingStatus)
            {
                SetStatus(lblProductStatus, "통신 작업이 끝난 뒤 로그를 저장해 주세요.", Color.Yellow);
                return;
            }
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = "Text log (*.txt)|*.txt";
                dialog.FileName = string.Format("CJ_UDS_{0:yyyyMMdd_HHmmss}.txt", DateTime.Now);
                if (dialog.ShowDialog(this) != DialogResult.OK)
                {
                    return;
                }
                try
                {
                    File.WriteAllText(dialog.FileName, _log.ToString(), Encoding.UTF8);
                    SetStatus(lblProductStatus, "전체 로그 저장 완료 (화면에서 숨긴 다운로드 로그 포함)", Color.LimeGreen);
                }
                catch (Exception ex) when (ex is IOException || ex is UnauthorizedAccessException)
                {
                    SetStatus(lblProductStatus, "로그 저장 실패: " + ex.Message, Color.OrangeRed);
                }
            }
        }

        private void btnClearLog_Click(object sender, EventArgs e)
        {
            rtbLog.Clear();
            _log.Clear();
            _logDisplayOffset = 0;
        }

        private bool CheckConnected()
        {
            UpdateConnectionState();
            if (_busy || _scanning || _verifying || _updating || _readingStatus)
            {
                return false;
            }
            if ((_gateway != null) && _gateway.IsConnected)
            {
                return true;
            }
            MessageBox.Show("Indie 또는 Peak LIN 연결이 필요합니다.", "UDS",
                MessageBoxButtons.OK, MessageBoxIcon.Information);
            return false;
        }

        private void Delay(int milliseconds)
        {
            Stopwatch timer = Stopwatch.StartNew();
            do
            {
                Application.DoEvents();
                if (_updating && _cancelUpdate)
                {
                    throw new OperationCanceledException();
                }
                if (timer.ElapsedMilliseconds >= milliseconds)
                {
                    break;
                }
                Thread.Sleep(_updating ? 1 : 5);
            }
            while (timer.ElapsedMilliseconds < milliseconds);
        }

        private static void SetStatus(Label label, string text, Color color)
        {
            label.Text = text;
            label.ForeColor = color;
        }

        private sealed class UdsNode
        {
            internal byte Nad;
            internal byte ChairId;
            internal uint InitNumber;
            internal UDSProtocol.ProductInfo Product;
            internal int ExecutionMode = -1;
            internal string RuntimeStatus = "상태 미확인";
        }
    }
}
