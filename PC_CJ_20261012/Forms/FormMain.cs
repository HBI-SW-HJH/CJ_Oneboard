using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO.Ports;
using System.Windows.Forms;
using LINMaster.LIN;
using LINMaster.CANoe;
using LINMaster.CAN;
using LINMaster.USB;
using LINMaster.LAN8661;

namespace LINMaster.Forms
{
    // =========================================================================
    // FormMain – IndieMicro LIN Gateway V2.0 메인 폼
    //
    // [변경 사항]
    //   - FormDeviceSelect 다이얼로그 제거
    //   - grpConn 에 rdoIndieMicro / rdoPeak 라디오 버튼 추가
    //     → 연결 전 언제든지 인터페이스를 전환 가능
    //   - SelectAndCreateGateway() 메서드 제거 → btnConnect_Click 에서 직접 처리
    // =========================================================================
    public partial class FormMain : Form
    {
        // ──────────────────────────────────────────────────────────────────
        // 멤버 변수
        // ──────────────────────────────────────────────────────────────────

        private ILINGateway _gw;
        private FormCIE _cieWin;
        private FormLan8661CIE _lan8661CieWin;
        private FormCjCIE _cjCieWin;
        private FormNodeScan _scanWin;
        private FormUDS _udsWin;
        private FormParamTuning _paramWin;
        private FormMacChange _macChangeWin;

        private readonly List<SchedItem> _schedItems = new List<SchedItem>();
        private int _schedIdx = 0;

        private const int MAX_3D_RETRY = 3;
        private const int TIMEOUT_3D_MS = 200;

        private bool _waiting3DResp = false;
        private DateTime _sent3DTime;
        private int _retry3DCount = 0;
        private LINFrame _last3CFrame = null;
        private byte _last3DDLen = 8;
        private bool _retryFromSched = false;

        private LINWebServer _server = null;   // 모바일 앱 서버
        private TcpBridge _bridge;   // 에뮬레이터 TCP-Serial 브리지

        private enum DeviceType { IndieMicro, Peak, RS485, FT4222, CANoe, CAN, USB, LAN8661 }
        private DeviceType _deviceType = DeviceType.IndieMicro;
        private readonly CANoeBridge _canoeBridge = new CANoeBridge();
        private bool _canoeConnected = false;

        // ── FT4222 ──────────────────────────────────────────────────────
        private FT4222.FT4222Manager _ft4222 = new FT4222.FT4222Manager();
        private readonly Lan8661Ws2812Client _lan8661 = new Lan8661Ws2812Client();
        /// <summary>CIE 창에서 FT4222 LED 동시 제어용</summary>
        public FT4222.FT4222Manager FT4222Device => _ft4222;
        private bool SupportsLedBackendSelection =>
            _deviceType == DeviceType.IndieMicro ||
            _deviceType == DeviceType.Peak ||
            _deviceType == DeviceType.CAN ||
            _deviceType == DeviceType.USB;
        private bool IsAD3306CommandMode => _deviceType == DeviceType.CAN ||
            _deviceType == DeviceType.USB ||
            (chkAdiMode != null && chkAdiMode.Checked &&
             (_deviceType == DeviceType.IndieMicro || _deviceType == DeviceType.Peak)) ||
            IsMicrochipControlMode;
        private bool IsCJControlMode => chkCjMode != null && chkCjMode.Checked &&
            (_deviceType == DeviceType.IndieMicro || _deviceType == DeviceType.Peak);
        private bool IsMicrochipControlMode => chkMicrochipMode != null &&
            chkMicrochipMode.Checked && SupportsLedBackendSelection;
        private AD3306LedBackend SelectedLedBackends
        {
            get
            {
                AD3306LedBackend selected = AD3306LedBackend.Auto;
                if (chkAdiMode != null && chkAdiMode.Checked)
                    selected |= AD3306LedBackend.Adi;
                if (chkMicrochipMode != null && chkMicrochipMode.Checked)
                    selected |= AD3306LedBackend.Microchip;
                return selected;
            }
        }
        private bool _changingControlMode;

        // =========================================================================
        // 생성자 / 초기화
        // =========================================================================

        public FormMain()
        {
            InitializeComponent();

            dgvSched.Rows.Add(true, "02", "Publisher", "Enhanced", "8", "FF 65 3A FA 01 01 FF 7F", "100");
            dgvSched.Rows.Add(true, "02", "Publisher", "Classic", "8", "FF 3A 65 FA 01 01 FF 7F", "200");

            grpTx.Enabled = false;
            grpSched.Enabled = false;

            cmbTxFrameType.SelectedIndex = 0;
            cmbTxCS.SelectedIndex = 1;

            // 초기 상태: IndieMicro 선택 → COM 포트 목록 로드
            RefreshPortList();
            UpdateInterfaceOptionState();
            _ft4222.OspLog += msg => AppLog(msg, System.Drawing.Color.FromArgb(120, 220, 255));

        }
        // ── 에뮬레이터 TCP-Serial 브리지 초기화 ──────────────────────
        private void InitBridge()
        {
            _bridge = new TcpBridge();
            _bridge.OnStatus += msg => AppLog(msg, System.Drawing.Color.FromArgb(255, 200, 80));
            _bridge.OnError += msg => AppLog(msg, System.Drawing.Color.FromArgb(255, 100, 100));
        }


        // =========================================================================
        // 모바일 웹 서버
        // =========================================================================

        private LINWebServer.ConnectResult ServerConnect(string iface, string port, ushort baud)
        {
            try
            {
                if (_gw != null && _gw.IsConnected)
                { StopSchedule(); timerRx.Stop(); _gw.Disconnect(); SetConnUI(false); }

                _deviceType = iface == "Peak" ? DeviceType.Peak
                            : iface == "RS485" ? DeviceType.RS485
                            : iface == "FT4222" ? DeviceType.FT4222
                            : iface == "USB" ? DeviceType.USB
                            : DeviceType.IndieMicro;
                DetachAndDisposeGateway();

                // UI 라디오 버튼 동기화
                rdoPeak.Checked = (_deviceType == DeviceType.Peak);
                rdoRS485.Checked = (_deviceType == DeviceType.RS485);
                rdoFT4222.Checked = (_deviceType == DeviceType.FT4222);
                rdoUSB.Checked = (_deviceType == DeviceType.USB);
                rdoIndieMicro.Checked = (_deviceType == DeviceType.IndieMicro);

                if (_deviceType == DeviceType.IndieMicro)
                {
                    _gw = new LIN.LINGateway(serialPort1);
                }
                else if (_deviceType == DeviceType.RS485)
                {
                    _gw = new LIN.RS485Gateway();
                }
                else if (_deviceType == DeviceType.FT4222)
                {
                    _ft4222.UseOtpMacMode = false;
                    _gw = new LIN.FT4222Gateway(_ft4222);
                }
                else if (_deviceType == DeviceType.USB)
                {
                    _gw = new USBGateway();
                }
                else
                {
                    var pgw = new LIN.PeakLINGateway();
                    pgw.LinBaudRate = baud > 0 ? baud : LIN.PeakLINGateway.DEFAULT_LIN_BAUDRATE;
                    _gw = pgw;
                }

                _gw.FrameReceived += OnFrameReceived;
                _gw.StatusMessage += OnStatusMessage;
                _gw.ErrorMessage += OnErrorMessage;
                _gw.RawPacket += OnRawPacket;

                bool ok = _gw.Connect(port);
                if (ok) { timerRx.Start(); SetConnUI(true); return new LINWebServer.ConnectResult { OK = true }; }
                DetachAndDisposeGateway();
                return new LINWebServer.ConnectResult { OK = false, Error = "연결 시도 실패" };
            }
            catch (Exception ex)
            { return new LINWebServer.ConnectResult { OK = false, Error = ex.Message }; }
        }

        private LINWebServer.SendResult ServerSend(string id, bool subscriber, string cs, int len, string data)
        {
            try
            {
                if (_gw == null || !_gw.IsConnected)
                    return new LINWebServer.SendResult { OK = false, Error = "연결 안됨" };
                if (!TryParseHexByte(id, out byte fid) || fid > 0x3F)
                    return new LINWebServer.SendResult { OK = false, Error = "잘못된 Frame ID" };

                var csType = cs == "Classic" ? LIN.LINChecksumType.Classic : LIN.LINChecksumType.Enhanced;
                if (subscriber)
                {
                    _gw.SendSubscriberRequest(fid, (byte)len, csType);
                    return new LINWebServer.SendResult { OK = true };
                }

                byte[] bytes = ParseHexBytes(data);
                if (bytes == null || bytes.Length == 0)
                    return new LINWebServer.SendResult { OK = false, Error = "데이터 파싱 실패" };

                var frame = new LIN.LINFrame
                {
                    FrameID = fid,
                    DataLength = (byte)Math.Min(bytes.Length, len),
                    ChecksumType = csType,
                    Data = new byte[8]
                };
                Array.Copy(bytes, frame.Data, frame.DataLength);
                bool ok = _gw.SendPublisher(frame);
                return new LINWebServer.SendResult { OK = ok, Error = ok ? null : "전송 실패" };
            }
            catch (Exception ex)
            { return new LINWebServer.SendResult { OK = false, Error = ex.Message }; }
        }


        // =========================================================================
        // 인터페이스 선택 라디오 버튼 변경 이벤트
        // =========================================================================

        /// <summary>
        /// rdoIndieMicro / rdoPeak 변경 시 호출
        /// → grpConn 제목/lblPort 텍스트 변경, 포트/장치 목록 갱신
        /// </summary>
        private void rdoInterface_CheckedChanged(object sender, EventArgs e)
        {
            var rdo = sender as RadioButton;
            if (rdo == null || !rdo.Checked) return;

            // 연결 중에는 전환 불가
            if ((_gw != null && _gw.IsConnected) || _lan8661.IsConnected)
            {
                if (_deviceType == DeviceType.IndieMicro) rdoIndieMicro.Checked = true;
                else if (_deviceType == DeviceType.RS485) rdoRS485.Checked = true;
                else if (_deviceType == DeviceType.FT4222) rdoFT4222.Checked = true;
                else if (_deviceType == DeviceType.CANoe) rdoCANoe.Checked = true;
                else if (_deviceType == DeviceType.CAN) rdoCAN.Checked = true;
                else if (_deviceType == DeviceType.USB) rdoUSB.Checked = true;
                else if (_deviceType == DeviceType.LAN8661) rdoLAN8661.Checked = true;
                else rdoPeak.Checked = true;
                MessageBox.Show("연결 중에는 인터페이스를 변경할 수 없습니다.\n먼저 연결을 해제하세요.",
                    "알림", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if (rdoPeak.Checked)
            {
                _deviceType = DeviceType.Peak;
                grpConn.Text = "[ 연결 설정  (PEAK PLIN-USB) ]";
                lblPort.Text = "PEAK 장치 :";
                lblLinBaud.Visible = true;
                cmbLinBaud.Visible = true;
                cmbLinBaud.Text = "19200";
                RefreshPeakDeviceList();
            }
            else if (rdoRS485.Checked)
            {
                _deviceType = DeviceType.RS485;
                grpConn.Text = "[ 연결 설정  (RS485: 115200,8,N,1) ]";
                lblPort.Text = "COM Port :";
                lblLinBaud.Visible = false;
                cmbLinBaud.Visible = false;
                RefreshPortList();
            }
            else if (rdoFT4222.Checked)
            {
                _deviceType = DeviceType.FT4222;
                grpConn.Text = "[ 연결 설정  (FT4222 / OSP LED) ]";
                lblPort.Text = "FT4222 :";
                lblLinBaud.Visible = false;
                cmbLinBaud.Visible = false;
                RefreshFT4222List();
            }
            else if (rdoCANoe.Checked)
            {
                _deviceType = DeviceType.CANoe;
                grpConn.Text = "[ 연결 설정  (CANoe / AD3304 LED Panel) ]";
                lblPort.Text = "CANoe :";
                lblLinBaud.Visible = false;
                cmbLinBaud.Visible = false;
                cmbPort.Items.Clear();
                cmbPort.Items.Add("CANoe.Application");
                cmbPort.SelectedIndex = 0;
            }
            else if (rdoCAN.Checked)
            {
                _deviceType = DeviceType.CAN;
                grpConn.Text = "[ CAN EXT  (PC TX 0x18FFFF02 / PC RX 0x18FFFF01) ]";
                lblPort.Text = "CAN 장치 :";
                lblLinBaud.Visible = false;
                cmbLinBaud.Visible = false;
                cmbPort.Items.Clear();
                cmbPort.Items.Add("VN5650 / CANoe 20 / CAN1");
                cmbPort.SelectedIndex = 0;
            }
            else if (rdoUSB.Checked)
            {
                _deviceType = DeviceType.USB;
                grpConn.Text = "[ USB 연결 설정  (CP2102N COM: 115200,8,N,1) ]";
                lblPort.Text = "USB COM :";
                lblLinBaud.Visible = false;
                cmbLinBaud.Visible = false;
                RefreshPortList();
            }
            else if (rdoLAN8661.Checked)
            {
                _deviceType = DeviceType.LAN8661;
                grpConn.Text = "[ 연결 설정  (Microchip LAN8661 / OSP RTP + 원격 SPI) ]";
                lblPort.Text = "LAN8661 IP :";
                lblLinBaud.Visible = false;
                cmbLinBaud.Visible = false;
                cmbPort.Items.Clear();
                cmbPort.Items.Add("192.168.0.50");
                cmbPort.SelectedIndex = 0;
            }
            else
            {
                _deviceType = DeviceType.IndieMicro;
                grpConn.Text = "[ 연결 설정  (COM: 115200,8,N,1) ]";
                lblPort.Text = "COM Port :";
                lblLinBaud.Visible = false;
                cmbLinBaud.Visible = false;
                RefreshPortList();
            }

            UpdateInterfaceOptionState();
            UpdateCieControlMode();
        }

        private void UpdateInterfaceOptionState()
        {
            bool linControl = _deviceType == DeviceType.IndieMicro ||
                _deviceType == DeviceType.Peak;

            chkFt4222UseOtp.Visible = true;
            chkAdiMode.Visible = true;
            chkCjMode.Visible = true;
            chkMicrochipMode.Visible = true;
            chkFt4222UseOtp.Enabled = _deviceType == DeviceType.FT4222;
            // ADI Control and Microchip Control use the same Sunshine command
            // transport.  Keep both choices available on every transport that
            // supports backend selection (LIN/Peak/CAN/USB).
            chkAdiMode.Enabled = SupportsLedBackendSelection;
            chkCjMode.Enabled = linControl;
            chkMicrochipMode.Enabled = SupportsLedBackendSelection;
        }

        private void chkAdiMode_CheckedChanged(object sender, EventArgs e)
        {
            if (_changingControlMode) return;
            _changingControlMode = true;
            if (chkAdiMode.Checked)
            {
                chkCjMode.Checked = false;
            }
            _changingControlMode = false;
            UpdateCieControlMode();

            AppLog(IsAD3306CommandMode
                ? "[AD3306 CMD] common 8-byte command enabled"
                : "[AD3306 CMD] disabled; legacy LIN payload enabled",
                IsAD3306CommandMode ? Color.FromArgb(255, 190, 80) : Color.FromArgb(160, 160, 180));
        }

        private void chkCjMode_CheckedChanged(object sender, EventArgs e)
        {
            if (_changingControlMode) return;
            _changingControlMode = true;
            if (chkCjMode.Checked)
            {
                chkAdiMode.Checked = false;
                chkMicrochipMode.Checked = false;
            }
            _changingControlMode = false;
            UpdateCieControlMode();

            AppLog(IsCJControlMode
                ? "[CJ LIN] CJ ZX1 control enabled (FID 0x01, enhanced, 8 bytes)"
                : "[CJ LIN] disabled; previous control mode restored",
                IsCJControlMode ? Color.FromArgb(100, 220, 255) : Color.FromArgb(160, 160, 180));
        }

        private void chkMicrochipMode_CheckedChanged(object sender, EventArgs e)
        {
            if (_changingControlMode) return;
            _changingControlMode = true;
            if (chkMicrochipMode.Checked)
            {
                chkCjMode.Checked = false;
            }
            _changingControlMode = false;
            UpdateCieControlMode();

            AppLog(IsMicrochipControlMode
                ? string.Format("[Microchip CMD] {0} -> CPU -> AD3306 -> LAN8661 -> OSP/ISE", _deviceType)
                : "[Microchip CMD] disabled",
                IsMicrochipControlMode ? Color.FromArgb(70, 220, 210) : Color.FromArgb(160, 160, 180));
        }

        private void UpdateCieControlMode()
        {
            btnOpenNodeScan.Visible = true;
            btnOpenCIE.Visible = true;
            btnOpenParamTuning.Visible = !IsCJControlMode;
            btnOpenAD3301.Visible = !IsCJControlMode;
            btnOpenNodeScan.Text = IsCJControlMode ? "UDS 열기" : "Node Scan 열기";
            btnOpenCIE.Text = IsCJControlMode ? "CJ CIE 색도도 열기" : "CIE 색도도 TX 열기";
            if (_cieWin != null && !_cieWin.IsDisposed)
            {
                _cieWin.UpdateAD3306CommandMode(IsAD3306CommandMode);
                _cieWin.UpdateBackendSelection(SelectedLedBackends);
                _cieWin.Text = "CIE 1931 Color Control";
            }
        }

        // =========================================================================
        // Gateway 이벤트 핸들러
        // =========================================================================

        private void OnFrameReceived(LINFrame frame)
        {
            if ((_udsWin != null) && _udsWin.FirmwareUpdateActive && ((frame.FrameID == 0x3C) || (frame.FrameID == 0x3D)))
            {
                return;
            }
            if (_cjCieWin != null && !_cjCieWin.IsDisposed)
                _cjCieWin.HandleCJFrame(frame);

            if (_udsWin != null && _udsWin.VerificationActive && (frame.FrameID == 0x01 || (frame.FrameID >= 0x28 && frame.FrameID <= 0x2F)))
                return;

            if (AD3306LedCommand.ServiceReadActive)
                return;

            Color logColor;
            if (frame.Direction == LINDirection.TX)
                logColor = Color.FromArgb(100, 240, 140);
            else if (frame.IsACK && frame.DataLength == 0)
                logColor = Color.FromArgb(180, 180, 100);
            else
                logColor = Color.FromArgb(100, 180, 255);

            if (_waiting3DResp
                && frame.Direction == LINDirection.RX
                && frame.FrameID == 0x3D
                && frame.DataLength > 0)
            {
                _waiting3DResp = false;
                _retry3DCount = 0;
            }

            AppLog(frame.ToString(), logColor);
        }

        private void OnStatusMessage(string msg)
        {
            AppLog(msg, Color.FromArgb(160, 180, 200));
        }

        private void OnErrorMessage(string msg)
        {
            AppLog(msg, Color.FromArgb(255, 100, 100));
        }

        private void OnRawPacket(byte[] data, string dir)
        {
            if ((_udsWin != null) && _udsWin.FirmwareUpdateActive)
            {
                return;
            }
            if (!chkRawLog.Checked) return;
            string hex = BitConverter.ToString(data).Replace("-", " ");
            AppLog(string.Format(dir == "TX" ? "[RAW TX] {0}" : "[RAW RX] {0}", hex),
                dir == "TX" ? Color.FromArgb(80, 200, 100) : Color.FromArgb(80, 160, 255));
        }

        // =========================================================================
        // 포트 / 장치 목록 관리
        // =========================================================================

        private void RefreshPortList()
        {
            string prevPort = cmbPort.Text;
            cmbPort.Items.Clear();

            string[] ports = SerialPort.GetPortNames();
            if (ports.Length > 0)
            {
                cmbPort.Items.AddRange(ports);
                int idx = Array.IndexOf(ports, prevPort);
                cmbPort.SelectedIndex = idx >= 0 ? idx : 0;
            }
            else
            {
                cmbPort.Items.Add("(포트 없음)");
                cmbPort.SelectedIndex = 0;
            }
        }

        private void RefreshPeakDeviceList()
        {
            string prevSel = cmbPort.Text;
            cmbPort.Items.Clear();

            string[] devices = PeakLINGateway.GetAvailableDevices();
            if (devices.Length > 0)
            {
                cmbPort.Items.AddRange(devices);
                int idx = Array.IndexOf(devices, prevSel);
                cmbPort.SelectedIndex = idx >= 0 ? idx : 0;
            }
            else
            {
                cmbPort.Items.Add("(PEAK 장치 없음)");
                cmbPort.SelectedIndex = 0;
            }
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            if (rdoPeak.Checked)
                RefreshPeakDeviceList();
            else if (rdoFT4222.Checked)
                RefreshFT4222List();
            else if (rdoLAN8661.Checked)
            {
                string current = string.IsNullOrWhiteSpace(cmbPort.Text)
                    ? "192.168.0.50" : cmbPort.Text.Trim();
                cmbPort.Items.Clear();
                cmbPort.Items.Add(current);
                cmbPort.SelectedIndex = 0;
            }
            else if (rdoCAN.Checked)
            {
                cmbPort.Items.Clear();
                cmbPort.Items.Add("VN5650 / CANoe 20 / CAN1");
                cmbPort.SelectedIndex = 0;
            }
            else
                RefreshPortList();
        }

        // =========================================================================
        // 연결 / 해제
        // =========================================================================

        /// <summary>
        /// 연결/해제 버튼 클릭
        /// FormDeviceSelect 다이얼로그 제거 →
        /// rdoIndieMicro / rdoPeak 에서 직접 장치 타입 읽어 처리
        /// </summary>
        private async void btnConnect_Click(object sender, EventArgs e)
        {
            
            if (_canoeConnected)
            {
                _canoeConnected = false;
                _canoeBridge.MarkDisconnected();
                SetConnUI(false);
                AppLog("[CANoe] 연결 해제", Color.FromArgb(160, 160, 180));
                return;
            }
            if (_lan8661.IsConnected)
            {
                _lan8661.Disconnect();
                SetConnUI(false);
                if (_lan8661CieWin != null && !_lan8661CieWin.IsDisposed)
                    _lan8661CieWin.UpdateConnectionStatus();
                AppLog("[LAN8661] 연결 해제", Color.FromArgb(160, 160, 180));
                return;
            }
// ── 이미 연결된 경우: 연결 해제 ─────────────────────────────
            if (_gw != null && _gw.IsConnected)
            {
                StopSchedule();
                timerRx.Stop();
                btnConnect.Enabled = false;
                // FT4222는 OspBridge_Close()가 블로킹되므로 백그라운드 실행
                // (FT4222Manager.Disconnect()는 내부에서 직접 Close 호출)
                if (_deviceType == DeviceType.FT4222)
                    await System.Threading.Tasks.Task.Run(() => _gw.Disconnect());
                else
                    _gw.Disconnect();
                // 이벤트 해제 + _gw = null: Dispose()는 Disconnect() 내부에서 완료됐으므로
                // 여기서는 이벤트만 해제하고 참조를 null로 정리한다.
                if (_gw != null)
                {
                    _gw.FrameReceived -= OnFrameReceived;
                    _gw.StatusMessage -= OnStatusMessage;
                    _gw.ErrorMessage -= OnErrorMessage;
                    _gw.RawPacket -= OnRawPacket;
                    _gw = null;
                }
                SetConnUI(false);
                btnConnect.Enabled = true;
                return;
            }

            // ── 장치 타입을 라디오 버튼에서 직접 읽기 ───────────────────
            _deviceType = rdoPeak.Checked ? DeviceType.Peak
                        : rdoRS485.Checked ? DeviceType.RS485
                        : rdoFT4222.Checked ? DeviceType.FT4222
                        : rdoLAN8661.Checked ? DeviceType.LAN8661
                        : rdoCANoe.Checked ? DeviceType.CANoe
                        : rdoCAN.Checked ? DeviceType.CAN
                        : rdoUSB.Checked ? DeviceType.USB
                        : DeviceType.IndieMicro;

            // 기존 게이트웨이 해제
            DetachAndDisposeGateway();

            if (_deviceType == DeviceType.LAN8661)
            {
                string targetIp = string.IsNullOrWhiteSpace(cmbPort.Text)
                    ? "192.168.0.50" : cmbPort.Text.Trim();
                btnConnect.Enabled = false;
                System.Tuple<bool, string> result = await System.Threading.Tasks.Task.Run(() =>
                {
                    string message;
                    bool success = _lan8661.Connect("192.168.0.10", targetIp,
                        Lan8661Ws2812Client.DefaultRtpPort, out message);
                    return System.Tuple.Create(success, message);
                });
                btnConnect.Enabled = true;

                if (!result.Item1)
                {
                    SetConnUI(false);
                    AppLog("[LAN8661 연결 실패] " + result.Item2, Color.FromArgb(255, 100, 100));
                    MessageBox.Show(result.Item2, "LAN8661",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                SetConnUI(true);
                if (_lan8661CieWin != null && !_lan8661CieWin.IsDisposed)
                    _lan8661CieWin.UpdateConnectionStatus();
                AppLog("[LAN8661] " + result.Item2, Color.FromArgb(120, 220, 255));
                return;
            }


            if (_deviceType == DeviceType.CANoe)
            {
                string message;
                btnConnect.Enabled = false;
                bool okCANoe = _canoeBridge.TryAttach(allowStart: true, out message)
                             && _canoeBridge.TryPreparePanel(out message);
                btnConnect.Enabled = true;

                if (!okCANoe)
                {
                    AppLog("[CANoe 연결 실패] " + message, Color.FromArgb(255, 100, 100));
                    MessageBox.Show("CANoe 20 연결/Init 실패: " + message + "\r\n\r\n대상 configuration:\r\n" + CANoeBridge.TargetConfigurationPath + "\r\n\r\nCANoe와 PC 프로그램은 같은 권한으로 실행해야 합니다.",
                        "CANoe", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    SetConnUI(false);
                    return;
                }

                _canoeConnected = true;
                _canoeBridge.MarkInitialized();
                SetConnUI(true);
                AppLog("[CANoe] 연결 완료 + Init 요청(BtnInit=1)", Color.FromArgb(120, 220, 255));
                if (_cieWin != null && !_cieWin.IsDisposed)
                    _cieWin.UpdateGateway(null);
                if (_cjCieWin != null && !_cjCieWin.IsDisposed)
                    _cjCieWin.UpdateGateway(null);
                return;
            }

            // 새 게이트웨이 인스턴스 생성
            try
            {
                if (_deviceType == DeviceType.IndieMicro)
                {
                    _gw = new LINGateway(serialPort1);
                }
                else if (_deviceType == DeviceType.RS485)
                {
                    _gw = new RS485Gateway();
                }
                else if (_deviceType == DeviceType.FT4222)
                {
                    _ft4222.UseOtpMacMode = chkFt4222UseOtp.Checked;
                    _gw = new FT4222Gateway(_ft4222);
                }
                else if (_deviceType == DeviceType.USB)
                {
                    _gw = new USBGateway();
                }
                else if (_deviceType == DeviceType.CAN)
                {
                    _gw = new CANoeCANGateway(_canoeBridge);
                }
                else
                {
                    var peakGw = new PeakLINGateway();
                    // LIN Baud Rate 콤보박스 값 적용
                    ushort linBaud = PeakLINGateway.DEFAULT_LIN_BAUDRATE;
                    if (cmbLinBaud.SelectedItem != null)
                        ushort.TryParse(cmbLinBaud.SelectedItem.ToString(), out linBaud);
                    peakGw.LinBaudRate = linBaud;
                    _gw = peakGw;
                }
            }
            catch (DllNotFoundException ex)
            {
                MessageBox.Show(ex.Message, "PEAK 드라이버 없음",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            catch (Exception ex)
            {
                MessageBox.Show("게이트웨이 초기화 오류: " + ex.Message, "오류",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 이벤트 연결
            _gw.FrameReceived += OnFrameReceived;
            _gw.StatusMessage += OnStatusMessage;
            _gw.ErrorMessage += OnErrorMessage;
            _gw.RawPacket += OnRawPacket;

            // FT4222 는 자동 검색으로 연결 (포트 선택 불필요)
            string port = _deviceType == DeviceType.FT4222 ? null : cmbPort.Text;
            if (_deviceType != DeviceType.FT4222
                && (string.IsNullOrEmpty(port)
                    || port == "(포트 없음)"
                    || port == "(PEAK 장치 없음)"
                    || port == "(FT4222 장치 없음)"))
            {
                MessageBox.Show(
                    _deviceType == DeviceType.Peak
                        ? "PEAK PLIN-USB 장치를 선택하세요."
                        : "COM 포트를 선택하세요.",
                    "경고", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                DetachAndDisposeGateway();
                return;
            }

            // 연결 시도
            bool ok = false;
            btnConnect.Enabled = false;
            try
            {
                if (_deviceType == DeviceType.FT4222 ||
                    _deviceType == DeviceType.CAN)
                {
                    // Hardware discovery and CANoe COM preparation can wait for
                    // a timeout. Keep that work off the UI thread.
                    ok = await System.Threading.Tasks.Task.Run(
                        () => _gw.Connect(port));
                }
                else
                {
                    ok = _gw.Connect(port);
                }
            }
            catch (Exception ex)
            {
                AppLog("[연결 오류] " + ex.Message,
                    System.Drawing.Color.FromArgb(255, 100, 100));
            }
            finally
            {
                btnConnect.Enabled = true;
            }
            if (ok)
            {
                timerRx.Start();
                SetConnUI(true);
                // 이미 열린 CIE 창이 있으면 새 게이트웨이 참조 갱신
                if (_cieWin != null && !_cieWin.IsDisposed)
                {
                    _cieWin.UpdateGateway(_gw);
                    UpdateCieControlMode();
                }
                if (_cjCieWin != null && !_cjCieWin.IsDisposed)
                    _cjCieWin.UpdateGateway(_gw);
            }
            else
            {
                string err = (_deviceType == DeviceType.FT4222 && _ft4222 != null)
                           ? _ft4222.LastError
                           : "연결 실패";
                if (!string.IsNullOrEmpty(err))
                    AppLog("[연결 실패] " + err, System.Drawing.Color.FromArgb(255, 100, 100));
                DetachAndDisposeGateway();
            }
        }

        /// <summary>게이트웨이 이벤트 해제 후 Dispose</summary>
        private void DetachAndDisposeGateway()
        {
            if (_gw == null) return;
            _gw.FrameReceived -= OnFrameReceived;
            _gw.StatusMessage -= OnStatusMessage;
            _gw.ErrorMessage -= OnErrorMessage;
            _gw.RawPacket -= OnRawPacket;
            _gw.Dispose();
            _gw = null;
        }

        private void SetConnUI(bool connected)
        {
            if (InvokeRequired) { BeginInvoke(new Action(() => SetConnUI(connected))); return; }
            if (connected)
            {
                string devLabel = (_deviceType == DeviceType.Peak) ? "[PEAK] "
                                : (_deviceType == DeviceType.RS485) ? "[RS485] "
                                : (_deviceType == DeviceType.FT4222) ? "[FT4222] "
                                : (_deviceType == DeviceType.CANoe) ? "[CANoe] "
                                : (_deviceType == DeviceType.CAN) ? "[CAN] "
                                : (_deviceType == DeviceType.USB) ? "[USB] "
                                : (_deviceType == DeviceType.LAN8661) ? "[LAN8661] "
                                : "[IndieMicro] ";
                btnConnect.Text = "연결 해제";
                btnConnect.BackColor = Color.FromArgb(160, 50, 50);
                lblStatusDot.BackColor = Color.LimeGreen;
                lblStatusText.ForeColor = Color.LimeGreen;

                // FT4222 연결 시 MAC / NodeId 읽어서 상태 표시
                if (_deviceType == DeviceType.FT4222 && _ft4222 != null)
                {
                    if (_ft4222.ReadMacNode(out ulong mac, out byte nodeId))
                    {
                        string masterMacStr = FormatMac(FT4222.FT4222Manager.CONTROLLER_MAC);
                        string nodeMacStr = FormatMac(mac);
                        //lblStatusText.Text = string.Format("연결됨 – {0}{1}  │  Master MAC: {2}\r\n│  Node MAC: {3}  │  Node ID: {4}",devLabel, _gw.PortName, masterMacStr, nodeMacStr, nodeId);
                        lblStatusText.Text = string.Format("연결됨 – {0}  │  Master MAC: {1}\r\n│  Node MAC: {2} ", _gw.PortName, masterMacStr, nodeMacStr);
                    }
                    else
                    {
                        lblStatusText.Text = "연결됨 – " + devLabel + _gw.PortName
                                           + "  │  MAC/Node 읽기 실패";
                    }
                }
                else if (_deviceType == DeviceType.CANoe)
                {
                    lblStatusText.Text = "연결됨 - " + devLabel + "CANoe.Application / Init 요청 완료";
                }
                else if (_deviceType == DeviceType.LAN8661)
                {
                    lblStatusText.Text = string.Format("연결됨 – {0}{1} → {2}:{3}",
                        devLabel, _lan8661.SourceIp, _lan8661.TargetIp, _lan8661.RtpPort);
                }
                else
                {
                    lblStatusText.Text = "연결됨 – " + devLabel + _gw.PortName;
                }

                // 연결 중 인터페이스 라디오 버튼 비활성
                rdoIndieMicro.Enabled = false;
                rdoPeak.Enabled = false;
                rdoRS485.Enabled = false;
                rdoFT4222.Enabled = false;
                rdoCANoe.Enabled = false;
                rdoCAN.Enabled = false;
                rdoUSB.Enabled = false;
                rdoLAN8661.Enabled = false;
                cmbLinBaud.Enabled = false;
            }
            else
            {
                btnConnect.Text = "연결";
                btnConnect.BackColor = Color.FromArgb(50, 130, 60);
                lblStatusDot.BackColor = Color.Gray;
                lblStatusText.Text = "연결 안됨";
                lblStatusText.ForeColor = Color.FromArgb(160, 160, 180);

                // 연결 해제 후 라디오 버튼 활성
                rdoIndieMicro.Enabled = true;
                rdoPeak.Enabled = true;
                rdoRS485.Enabled = true;
                rdoFT4222.Enabled = true;
                rdoCANoe.Enabled = true;
                rdoCAN.Enabled = true;
                rdoUSB.Enabled = true;
                rdoLAN8661.Enabled = true;
                cmbLinBaud.Enabled = true;
            }

            grpTx.Enabled = connected && _deviceType != DeviceType.CANoe &&
                _deviceType != DeviceType.LAN8661;
            grpSched.Enabled = connected && _deviceType != DeviceType.CANoe &&
                _deviceType != DeviceType.LAN8661;

            if (_deviceType == DeviceType.FT4222)
                UpdateFT4222UI(connected);

            // 모바일 앱 상태 브로드캐스트
            if (_server != null && _server.IsRunning)
            {
                string iface = _deviceType == DeviceType.Peak ? "Peak"
                            : _deviceType == DeviceType.RS485 ? "RS485"
                            : _deviceType == DeviceType.FT4222 ? "FT4222"
                            : _deviceType == DeviceType.CANoe ? "CANoe"
                            : _deviceType == DeviceType.CAN ? "CAN"
                            : _deviceType == DeviceType.USB ? "USB"
                            : _deviceType == DeviceType.LAN8661 ? "LAN8661"
                            : "IndieMicro";
                string port = _deviceType == DeviceType.CANoe ? "CANoe.Application"
                    : _deviceType == DeviceType.LAN8661 ? _lan8661.TargetIp
                    : ((_gw != null && _gw.IsConnected) ? _gw.PortName : "");
                _server.BroadcastStatus(connected, port, iface);
            }
        }

        // =========================================================================
        // 수동 TX
        // =========================================================================

        private void cmbTxFrameType_SelectedIndexChanged(object sender, EventArgs e)
        {
            bool isPublisher = (cmbTxFrameType.SelectedIndex == 0);
            txtTxData.Enabled = isPublisher;
            btnTxSend.Enabled = isPublisher;
            btnTxRequest.Enabled = !isPublisher;
        }

        private void btnTxSend_Click(object sender, EventArgs e)
        {
            if (!CheckConnected()) return;
            if (!TryParseID(txtTxID.Text, out byte id)) return;

            byte[] data = ParseHexBytes(txtTxData.Text);
            if (data == null || data.Length == 0 || data.Length > 8)
            {
                MessageBox.Show("Data: HEX 공백구분 1~8 바이트\n예: FF 65 3A FA 01 01 FF 7F");
                return;
            }

            LINFrame frame = new LINFrame();
            frame.FrameID = id;
            frame.DataLength = (byte)data.Length;

            if (id == 0x3C)
                frame.ChecksumType = LINChecksumType.Classic;
            else if (cmbTxCS.SelectedIndex == 0)
                frame.ChecksumType = LINChecksumType.Classic;
            else
                frame.ChecksumType = LINChecksumType.Enhanced;

            Array.Copy(data, frame.Data, data.Length);

            if (id == 0x3C)
            {
                _retry3DCount = 0;
                Send3CAndRequest3D(frame, (byte)nudTxDLen.Value, false);
            }
            else
            {
                _gw.SendPublisher(frame);
            }
        }

        private static void Delay(int ms)
        {
            DateTime until = DateTime.Now.AddMilliseconds(ms);
            while (DateTime.Now < until)
                System.Windows.Forms.Application.DoEvents();
        }

        // =========================================================================
        // 0x3C → 0x3D 헬퍼
        // =========================================================================

        private void Send3CAndRequest3D(LINFrame frame3C, byte dLen3D, bool fromSched)
        {
            _last3CFrame = frame3C;
            _last3DDLen = dLen3D;
            _retryFromSched = fromSched;
            _gw.SendPublisher(frame3C);
            Delay(10);
            _gw.SendSubscriberRequest(0x3D, dLen3D, LINChecksumType.Classic);
            _waiting3DResp = true;
            _sent3DTime = DateTime.Now;
        }

        private void Check3DTimeout()
        {
            if (_udsWin != null && _udsWin.FirmwareUpdateActive) return;
            if (!_waiting3DResp) return;
            if ((DateTime.Now - _sent3DTime).TotalMilliseconds < TIMEOUT_3D_MS) return;

            _waiting3DResp = false;
            _retry3DCount++;

            if (_retry3DCount > MAX_3D_RETRY)
            {
                AppLog(string.Format("[경고] 0x3D 무응답 – {0}회 재시도 초과, 전송 중단", MAX_3D_RETRY), Color.OrangeRed);
                _retry3DCount = 0;
                _last3CFrame = null;
                return;
            }

            AppLog(string.Format("[재시도 {0}/{1}] 0x3D 무응답 → 0x3C 부터 재송신",
                _retry3DCount, MAX_3D_RETRY), Color.Orange);

            if (_last3CFrame != null && _gw.IsConnected)
                Send3CAndRequest3D(_last3CFrame, _last3DDLen, _retryFromSched);
        }

        private void btnTxRequest_Click(object sender, EventArgs e)
        {
            if (!CheckConnected()) return;
            if (!TryParseID(txtTxID.Text, out byte id)) return;

            if (id == 0x3D)
            {
                AppLog("[INFO] 0x3D는 0x3C 전송 시 자동으로만 발송됩니다.", Color.Orange);
                return;
            }

            LINChecksumType cs = (cmbTxCS.SelectedIndex == 0)
                ? LINChecksumType.Classic : LINChecksumType.Enhanced;
            _gw.SendSubscriberRequest(id, (byte)nudTxDLen.Value, cs);
        }

        // =========================================================================
        // 스케줄 TX
        // =========================================================================

        private void btnSchedAdd_Click(object sender, EventArgs e)
        {
            dgvSched.Rows.Add(true, "01", "Publisher", "Enhanced", "8", "FF FF FF FF FF FF FF FF", "100");
        }

        private void btnSchedDel_Click(object sender, EventArgs e)
        {
            if (dgvSched.SelectedRows.Count > 0 && !dgvSched.SelectedRows[0].IsNewRow)
                dgvSched.Rows.Remove(dgvSched.SelectedRows[0]);
        }

        private void btnSchedStart_Click(object sender, EventArgs e)
        {
            if (!CheckConnected()) return;

            if (timer1.Enabled) { StopSchedule(); return; }

            _schedItems.Clear();

            foreach (DataGridViewRow row in dgvSched.Rows)
            {
                if (row.IsNewRow) continue;
                if (!bool.TryParse(row.Cells["colEn"].Value?.ToString(), out bool en) || !en) continue;
                if (!TryParseHexByte(row.Cells["colID"].Value?.ToString(), out byte id)) continue;

                bool isPub = (row.Cells["colFType"].Value?.ToString() != "Subscriber");
                LINChecksumType cs = row.Cells["colCS"].Value?.ToString() == "Classic"
                    ? LINChecksumType.Classic : LINChecksumType.Enhanced;

                byte[] data = ParseHexBytes(row.Cells["colData"].Value?.ToString()) ?? new byte[] { 0xFF };
                byte.TryParse(row.Cells["colDLen"].Value?.ToString(), out byte dLen);
                if (dLen == 0 || dLen > 8) dLen = 8;
                int.TryParse(row.Cells["colDelay"].Value?.ToString(), out int delay);
                if (delay < 10) delay = 10;

                _schedItems.Add(new SchedItem
                {
                    ID = id,
                    IsPublisher = isPub,
                    CsType = cs,
                    Data = data,
                    DataLen = dLen,
                    DelayMs = delay
                });
            }

            if (_schedItems.Count == 0)
            { MessageBox.Show("활성화된 항목이 없습니다."); return; }

            _schedIdx = 0;
            timer1.Interval = _schedItems[0].DelayMs;
            timer1.Start();

            btnSchedStart.Text = "■  스케줄 중지";
            btnSchedStart.BackColor = Color.FromArgb(150, 50, 50);
            AppLog(string.Format("[스케줄 시작] {0}개 항목", _schedItems.Count), Color.Yellow);
        }

        private void StopSchedule()
        {
            timer1.Stop();
            btnSchedStart.Text = "▶  스케줄 시작";
            btnSchedStart.BackColor = Color.FromArgb(50, 110, 190);
            AppLog("[스케줄 중지]", Color.Yellow);
        }

        // =========================================================================
        // 타이머 Tick
        // =========================================================================

        private void timerRx_Tick(object sender, EventArgs e)
        {
            _gw?.Poll();
            Check3DTimeout();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (_udsWin != null && _udsWin.FirmwareUpdateActive) return;
            if (_schedItems.Count == 0 || _gw == null || !_gw.IsConnected) return;
            if (_waiting3DResp) return;

            SchedItem s = _schedItems[_schedIdx];

            if (s.IsPublisher)
            {
                LINFrame frame = new LINFrame();
                frame.FrameID = s.ID;
                frame.DataLength = s.DataLen;
                frame.ChecksumType = (s.ID == 0x3C) ? LINChecksumType.Classic : s.CsType;
                Array.Copy(s.Data, frame.Data, Math.Min(s.Data.Length, s.DataLen));

                if (s.ID == 0x3C)
                    Send3CAndRequest3D(frame, s.DataLen, true);
                else
                    _gw.SendPublisher(frame);
            }
            else
            {
                if (s.ID != 0x3D)
                    _gw.SendSubscriberRequest(s.ID, s.DataLen, s.CsType);
            }

            _schedIdx = (_schedIdx + 1) % _schedItems.Count;
            timer1.Interval = _schedItems[_schedIdx].DelayMs;
        }

        // =========================================================================
        // 로그
        // =========================================================================

        private void AppLog(string text, Color? col = null)
        {
            if (IsDisposed || !IsHandleCreated) return;

            // 백그라운드 스레드(OspBridge 내부 스레드 포함)에서 호출되면
            // UI 스레드로 마샬링하여 크로스 스레드 예외를 방지한다.
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => AppLog(text, col)));
                return;
            }

            Color c = col ?? Color.White;
            rtbLog.SelectionStart = rtbLog.TextLength;
            rtbLog.SelectionLength = 0;
            rtbLog.SelectionColor = c;
            rtbLog.AppendText(text + "\r\n");
            rtbLog.ScrollToCaret();

            if (rtbLog.Lines.Length > 1500)
                rtbLog.Text = string.Join("\r\n", rtbLog.Lines, rtbLog.Lines.Length - 700, 700);

            // 모바일 앱으로 브로드캐스트
            if (_server != null && _server.IsRunning)
            {
                string hex = string.Format("#{0:X2}{1:X2}{2:X2}", c.R, c.G, c.B);
                bool isRaw = text.StartsWith("[RAW");
                _server.BroadcastLog(text, hex, isRaw);
            }
        }

        private void btnClearLog_Click(object sender, EventArgs e) { rtbLog.Clear(); }

        private void btnSaveLog_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog dlg = new SaveFileDialog())
            {
                dlg.Title = "로그 저장";
                dlg.Filter = "텍스트 파일 (*.txt)|*.txt|로그 파일 (*.log)|*.log|모든 파일 (*.*)|*.*";
                dlg.FilterIndex = 1;
                dlg.DefaultExt = "txt";
                dlg.FileName = "LINMaster_Log_" + DateTime.Now.ToString("yyyyMMdd_HHmmss");
                dlg.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);

                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        System.IO.File.WriteAllText(dlg.FileName, rtbLog.Text, System.Text.Encoding.UTF8);
                        AppLog(string.Format("[로그 저장] {0}", dlg.FileName), Color.FromArgb(100, 200, 255));
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("저장 실패:\n" + ex.Message, "오류",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        // =========================================================================
        // 하위 창 열기
        // =========================================================================

        private void btnOpenNodeScan_Click(object sender, EventArgs e)
        {
            if (IsCJControlMode)
            {
                if (_udsWin == null || _udsWin.IsDisposed)
                {
                    _udsWin = new FormUDS(_gw);
                    _udsWin.Location = new Point(Left + Width + 4, Top);
                    _udsWin.Show();
                }
                else _udsWin.BringToFront();
                return;
            }

            if (_scanWin == null || _scanWin.IsDisposed)
            {
                _scanWin = new FormNodeScan(_gw);
                _scanWin.Location = new Point(Left + Width + 4, Top);
                _scanWin.Show();
            }
            else _scanWin.BringToFront();
        }

        private void btnOpenCIE_Click(object sender, EventArgs e)
        {
            if (_deviceType == DeviceType.LAN8661)
            {
                if (!_lan8661.IsConnected)
                {
                    MessageBox.Show("LAN8661을 먼저 연결하세요.", "LAN8661",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                if (_lan8661CieWin == null || _lan8661CieWin.IsDisposed)
                {
                    _lan8661CieWin = new FormLan8661CIE(_lan8661);
                    _lan8661CieWin.StatusMessage += msg =>
                        AppLog(msg, Color.FromArgb(120, 220, 255));
                    _lan8661CieWin.Location = new Point(Left + Width + 4, Top);
                    _lan8661CieWin.Show();
                }
                else
                {
                    _lan8661CieWin.UpdateConnectionStatus();
                    _lan8661CieWin.BringToFront();
                }
                return;
            }

            if (IsCJControlMode)
            {
                if (_cjCieWin == null || _cjCieWin.IsDisposed)
                {
                    _cjCieWin = new FormCjCIE(_gw);
                    _cjCieWin.OspMessage += msg => AppLog(msg, Color.FromArgb(120, 220, 255));
                    _cjCieWin.Location = new Point(Left + Width + 4, Top);
                    _cjCieWin.Show();
                }
                else _cjCieWin.BringToFront();
                return;
            }

            if (_cieWin == null || _cieWin.IsDisposed)
            {
                _cieWin = new FormCIE(_gw, _ft4222, _canoeBridge,
                    IsAD3306CommandMode);
                _cieWin.UpdateBackendSelection(SelectedLedBackends);
                _cieWin.Text = "CIE 1931 Color Control";
                _cieWin.OspMessage += msg => AppLog(msg, Color.FromArgb(120, 220, 255));
                _cieWin.Location = new Point(Left + Width + 4, Top);
                _cieWin.Show();
            }
            else _cieWin.BringToFront();
        }

        private void btnOpenParamTuning_Click(object sender, EventArgs e)
        {
            if (_paramWin == null || _paramWin.IsDisposed)
            {
                _paramWin = new FormParamTuning(_gw);
                _paramWin.Location = new Point(Left + Width + 4, Top);
                _paramWin.Show();
            }
            else _paramWin.BringToFront();
        }

        private void btnOpenAD3301_Click(object sender, EventArgs e)
        {
            if (_macChangeWin == null || _macChangeWin.IsDisposed)
            {
                _macChangeWin = new FormMacChange(_gw, _ft4222);
                _macChangeWin.Location = new Point(Left + Width + 4, Top + 220);
                _macChangeWin.Show();
            }
            else _macChangeWin.BringToFront();
        }

        // =========================================================================
        // FT4222 연결 / UI
        // =========================================================================

        private async void btnFT4222Connect_Click(object sender, EventArgs e)
        {
            // 중복 클릭 방지 (Connect/Disconnect 진행 중 버튼 비활성)
            btnFT4222Connect.Enabled = false;

            if (_ft4222.IsConnected)
            {
                // OspBridge_Close()가 내부 스레드 종료를 기다리므로
                // 백그라운드에서 실행해 UI 스레드 프리즈를 방지한다.
                await System.Threading.Tasks.Task.Run(() => _ft4222.Disconnect());
                UpdateFT4222UI(false);
                btnFT4222Connect.Enabled = true;
                return;
            }

            // OspBridge_InitFixed()가 discovery 타임아웃 동안 블로킹하므로
            // 백그라운드에서 실행해 UI 스레드 프리즈를 방지한다.
            bool ok = await System.Threading.Tasks.Task.Run(() => _ft4222.Connect(null));
            if (ok)
            {
                UpdateFT4222UI(true);

                // 연결 후 MAC / NodeId 읽어서 lblFT4222Status 에 표시
                if (_ft4222.ReadMacNode(out ulong mac, out byte nodeId))
                {
                    string masterMacStr = FormatMac(FT4222.FT4222Manager.CONTROLLER_MAC);
                    string nodeMacStr = FormatMac(mac);
                    lblFT4222Status.Text = string.Format(
                        "● FT4222 연결됨  │  Master MAC: {0}  │  Node MAC: {1}  │  Node ID: {2}",
                        masterMacStr, nodeMacStr, nodeId);
                }
            }
            else
            {
                string detail = string.IsNullOrEmpty(_ft4222.LastError)
                    ? "  • ftd2xx.dll / LibFT4222-64.dll 이 실행 파일 폴더에 있는지\r\n" +
                      "  • FTDI CDM 드라이버(WHQL)가 설치되었는지\r\n" +
                      "  • USB 케이블 및 FT4222 모듈 연결 상태\r\n" +
                      "  • 빌드 플랫폼이 AnyCPU 또는 x64인지 (64-bit DLL 사용 중)"
                    : _ft4222.LastError;
                MessageBox.Show(
                    "FT4222 연결 실패.\r\n\r\n" + detail,
                    "FT4222 연결 오류",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            btnFT4222Connect.Enabled = true;
        }

        private void btnFT4222Refresh_Click(object sender, EventArgs e) => RefreshFT4222List();

        internal void RefreshFT4222List()
        {
            ComboBox target = rdoFT4222 != null && rdoFT4222.Checked ? cmbPort : cmbFT4222Port;
            string prev = target.Text;
            target.Items.Clear();

            var devs = FT4222.FT4222Manager.EnumerateDevices();
            if (devs.Count > 0)
            {
                target.Items.AddRange(devs.ToArray());
                int idx = target.Items.IndexOf(prev);
                target.SelectedIndex = idx >= 0 ? idx : 0;
            }
            else
            {
                target.Items.Add("(FT4222 장치 없음)");
                target.SelectedIndex = 0;
            }
        }

        private void UpdateFT4222UI(bool connected)
        {
            if (InvokeRequired) { BeginInvoke(new Action(() => UpdateFT4222UI(connected))); return; }
            if (connected)
            {
                btnFT4222Connect.Text = "FT4222 연결 해제";
                btnFT4222Connect.BackColor = System.Drawing.Color.FromArgb(160, 50, 50);
                lblFT4222Status.Text = "● FT4222 연결됨";
                lblFT4222Status.ForeColor = System.Drawing.Color.LimeGreen;
            }
            else
            {
                btnFT4222Connect.Text = "FT4222 연결";
                btnFT4222Connect.BackColor = System.Drawing.Color.FromArgb(30, 90, 160);
                lblFT4222Status.Text = "● FT4222 연결 안됨";
                lblFT4222Status.ForeColor = System.Drawing.Color.FromArgb(160, 160, 180);
            }
            // CIE 창이 열려있으면 FT4222 상태 동기화
            if (_cieWin != null && !_cieWin.IsDisposed)
                _cieWin.UpdateFT4222Status(connected);
        }

        // =========================================================================
        // 폼 종료
        // =========================================================================

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (_udsWin != null && _udsWin.FirmwareUpdateActive)
            {
                e.Cancel = true;
                return;
            }
            _server?.Stop();
            _bridge?.Stop();
            StopSchedule();
            timerRx.Stop();
            if (_cieWin != null) _cieWin.Close();
            if (_lan8661CieWin != null) _lan8661CieWin.Close();
            if (_cjCieWin != null) _cjCieWin.Close();
            if (_scanWin != null) _scanWin.Close();
            if (_udsWin != null) _udsWin.Close();
            if (_paramWin != null) _paramWin.Close();
            if (_macChangeWin != null) _macChangeWin.Close();
            if (_gw != null) { _gw.Disconnect(); _gw.Dispose(); }
            _ft4222?.Dispose();
            _lan8661.Dispose();
        }

        // =========================================================================
        // 유틸리티
        // =========================================================================

        private bool CheckConnected()
        {
            if (_gw != null && _gw.IsConnected) return true;
            AppLog("[ERR] 연결 안됨", Color.OrangeRed);
            return false;
        }

        private bool TryParseID(string s, out byte id)
        {
            if (TryParseHexByte(s, out id) && id <= 0x3F) return true;
            MessageBox.Show("Frame ID: 0x00~0x3F 범위의 HEX 값 입력\n예: 02  21  3C");
            id = 0;
            return false;
        }

        private static bool TryParseHexByte(string s, out byte result)
        {
            result = 0;
            if (string.IsNullOrWhiteSpace(s)) return false;
            try
            {
                string clean = s.Trim().Replace("0x", "").Replace("0X", "");
                result = Convert.ToByte(clean, 16);
                return true;
            }
            catch { return false; }
        }

        private static string FormatMac(ulong mac)
        {
            return string.Format(
                "{0:X2}:{1:X2}:{2:X2}:{3:X2}:{4:X2}:{5:X2}",
                (mac >> 40) & 0xFF, (mac >> 32) & 0xFF,
                (mac >> 24) & 0xFF, (mac >> 16) & 0xFF,
                (mac >> 8) & 0xFF, mac & 0xFF);
        }

        private static byte[] ParseHexBytes(string s)
        {
            if (string.IsNullOrWhiteSpace(s)) return null;
            string[] tokens = s.Trim().Split(
                new char[] { ' ', ',', '\t' }, StringSplitOptions.RemoveEmptyEntries);
            var list = new List<byte>();
            foreach (string tok in tokens)
            {
                try
                {
                    string clean = tok.Replace("0x", "").Replace("0X", "");
                    list.Add(Convert.ToByte(clean, 16));
                }
                catch { return null; }
            }
            return list.Count > 0 ? list.ToArray() : null;
        }
    }

    // =========================================================================
    // SchedItem
    // =========================================================================
    internal class SchedItem
    {
        public byte ID { get; set; }
        public bool IsPublisher { get; set; }
        public LINChecksumType CsType { get; set; }
        public byte[] Data { get; set; }
        public byte DataLen { get; set; }
        public int DelayMs { get; set; }
    }
}












