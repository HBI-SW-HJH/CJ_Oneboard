using System;
using System.Drawing;
using System.Windows.Forms;
using LINMaster.LIN;
using LINMaster.FT4222;

namespace LINMaster.Forms
{
    // =========================================================================
    // FormMacChange - ADI MAC Address / PLCA Node ID read / write
    //
    // [통신 경로]
    //   FormMain 의 FT4222Manager → OspBridge.dll (NativeOsp C DLL)
    //   → FT4222 SPI → AD3306 E2B → AD3301
    //
    //   LIN DataDump(SID=0xB4) 방식은 FT4222 경로에서 동작하지 않으므로
    //   OspBridge_GetMacNode / SetMac / SetNodeId API 를 직접 사용합니다.
    //
    // [OspBridge API]
    //   OspBridge_GetMacNode(mac6, nodeId) – discovery 후 실제 MAC + PLCA Node 반환
    //   OspBridge_SetMac(mac6)             – adi_eal_writeMacAddr + E2B/OSP 재초기화
    //   OspBridge_SetNodeId(nodeId)        – EAL 레이어로 PLCA Node ID 변경
    // =========================================================================
    public partial class FormMacChange : Form
    {
        // ─────────────────────────────────────────────────────────────────
        // 멤버
        // ─────────────────────────────────────────────────────────────────
        private readonly ILINGateway   _gw;
        /// <summary>FormMain 의 FT4222 인스턴스 (LIN 게이트웨이 종류와 무관하게 OSP/MAC 경로는 여기로 통일)</summary>
        private readonly FT4222Manager _ft;

        // 현재 읽은 값 (표시용)
        private ulong _currentMac  = 0;
        private byte  _currentNode = 0;

        // =========================================================================
        // 생성자
        // =========================================================================
        public FormMacChange(ILINGateway gw, FT4222Manager ft4222)
        {
            InitializeComponent();
            _gw = gw ?? throw new ArgumentNullException(nameof(gw));
            _ft = ft4222 ?? throw new ArgumentNullException(nameof(ft4222));
            nudNewNode.Enabled = false;

            UpdateButtonStates();
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            base.OnVisibleChanged(e);
            if (Visible)
                UpdateButtonStates();
        }

        // =========================================================================
        // 폼 종료
        // =========================================================================
        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            base.OnFormClosed(e);
        }

        // =========================================================================
        // 버튼 상태 업데이트
        // =========================================================================
        private void UpdateButtonStates()
        {
            bool connected = _ft.IsConnected;
            btnRead.Enabled      = connected;
            btnReadOtp.Enabled   = connected;
            btnWriteMac.Enabled  = connected;
            btnWriteNode.Enabled = connected;
            btnWriteAll.Enabled  = connected;
            btnProgramOtp.Enabled = connected;

            if (!connected)
                SetStatus("FT4222(USB)가 연결되지 않았습니다. 메인 폼에서 FT4222 연결 후 다시 시도하세요.", Color.OrangeRed);
        }

        // =========================================================================
        // 상태 표시
        // =========================================================================
        private void SetStatus(string msg, Color color)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => SetStatus(msg, color)));
                return;
            }
            lblStatus.Text      = msg;
            lblStatus.ForeColor = color;
        }

        // =========================================================================
        // READ – 현재 MAC 주소 / Node ID 읽기
        // =========================================================================
        private void btnRead_Click(object sender, EventArgs e)
        {
            if (!_ft.IsConnected)
            {
                SetStatus("FT4222 미연결", Color.OrangeRed);
                return;
            }

            btnRead.Enabled = false;
            SetStatus("읽는 중...", Color.Yellow);

            try
            {
                if (_ft.ReadMacNode(out ulong mac, out byte nodeId))
                {
                    _currentMac  = mac;
                    byte fidNode = GetNodeFromMac(mac);
                    _currentNode = fidNode;

                    string macStr = FormatMac(mac);
                    lblCurrentMac.Text  = macStr;
                    lblCurrentNode.Text = $"0x{fidNode:X2}  ({fidNode})";

                    // 쓰기 입력창에 현재값 미리 채움
                    txtNewMac.Text   = macStr;
                    nudNewNode.Value = fidNode;

                    SetStatus($"Read complete  MAC={macStr}  Node/FID={fidNode}", Color.LimeGreen);
                }
                else
                {
                    lblCurrentMac.Text  = "읽기 실패";
                    lblCurrentNode.Text = "읽기 실패";
                    SetStatus("읽기 실패: " + _ft.LastError, Color.OrangeRed);
                }
            }
            finally
            {
                btnRead.Enabled = true;
            }
        }

        // =========================================================================
        // READ OTP – OTP designer 블록에 구워진 값 읽기 (IF Pin 값 아님)
        // =========================================================================
        private void btnReadOtp_Click(object sender, EventArgs e)
        {
            if (!_ft.IsConnected)
            {
                SetStatus("FT4222 미연결", Color.OrangeRed);
                return;
            }

            btnReadOtp.Enabled = false;
            SetStatus("OTP 읽는 중...", Color.Yellow);
            Cursor = Cursors.WaitCursor;

            try
            {
                if (_ft.ReadMacNodeOtp(out ulong mac, out byte nodeId,
                        out byte bootMac, out byte bootPlca, out byte[] raw))
                {
                    _currentMac  = mac;
                    _currentNode = nodeId;

                    string macStr = FormatMac(mac);
                    lblCurrentMac.Text  = macStr + "  [OTP]";
                    lblCurrentNode.Text = $"0x{nodeId:X2}  ({nodeId})  [OTP]";

                    // 레이아웃 검증용 원본 덤프 (상태 로그에서 확인)
                    SetStatus(
                        $"OTP read  MAC={macStr}  Node={nodeId}  bootStatus(MAC/PLCA)={bootMac}/{bootPlca}",
                        Color.LimeGreen);
                }
                else
                {
                    lblCurrentMac.Text  = "OTP 읽기 실패";
                    lblCurrentNode.Text = "OTP 읽기 실패";
                    SetStatus("OTP 읽기 실패: " + _ft.LastError, Color.OrangeRed);
                }
            }
            finally
            {
                Cursor = Cursors.Default;
                btnReadOtp.Enabled = _ft.IsConnected;
            }
        }

        // =========================================================================
        // WRITE MAC – MAC 주소만 쓰기
        // =========================================================================
        private void btnWriteMac_Click(object sender, EventArgs e)
        {
            if (!TryParseMac(txtNewMac.Text.Trim(), out ulong newMac))
            {
                SetStatus("MAC 주소 형식 오류  예) 00:E0:22:FE:70:00", Color.OrangeRed);
                return;
            }

            if (!ConfirmWrite($"MAC 주소를 변경합니다.\n\n현재: {FormatMac(_currentMac)}\n변경: {FormatMac(newMac)}\n\n계속하시겠습니까?"))
                return;

            SetStatus("MAC 쓰는 중...", Color.Yellow);
            Cursor = Cursors.WaitCursor;

            try
            {
                if (_ft.WriteMac(newMac))
                {
                    _currentMac        = newMac;
                    lblCurrentMac.Text = FormatMac(newMac);
                    SetStatus($"MAC 변경 완료 → {FormatMac(newMac)}", Color.LimeGreen);
                }
                else
                {
                    SetStatus("MAC 쓰기 실패: " + _ft.LastError, Color.OrangeRed);
                }
            }
            finally
            {
                Cursor = Cursors.Default;
            }
        }

        // =========================================================================
        // WRITE NODE – Node ID만 쓰기
        // =========================================================================
        private void btnWriteNode_Click(object sender, EventArgs e)
        {
            if (!TryParseMac(txtNewMac.Text.Trim(), out ulong newMac))
            {
                SetStatus("MAC format error. Example: 00:E0:22:FE:70:00", Color.OrangeRed);
                return;
            }
            if (!TryGetNodeFromMac(newMac, out byte newNode))
            {
                SetStatus("MAC last byte must be FID/Node 0x01..0x08.", Color.OrangeRed);
                return;
            }
            nudNewNode.Value = newNode;

            if (!ConfirmWrite($"PLCA Node ID를 변경합니다.\n\n현재: {_currentNode} (0x{_currentNode:X2})\n변경: {newNode} (0x{newNode:X2})\n\n계속하시겠습니까?"))
                return;

            SetStatus("Node ID 쓰는 중...", Color.Yellow);
            Cursor = Cursors.WaitCursor;

            try
            {
                if (_ft.WriteNodeId(newNode))
                {
                    _currentNode        = newNode;
                    lblCurrentNode.Text = $"0x{newNode:X2}  ({newNode})";
                    SetStatus($"Node ID 변경 완료 → {newNode}", Color.LimeGreen);
                }
                else
                {
                    SetStatus("Node ID 쓰기 실패: " + _ft.LastError, Color.OrangeRed);
                }
            }
            finally
            {
                Cursor = Cursors.Default;
            }
        }

        // =========================================================================
        // WRITE ALL – MAC + Node ID 동시 쓰기
        // =========================================================================
        private void btnWriteAll_Click(object sender, EventArgs e)
        {
            if (!TryParseMac(txtNewMac.Text.Trim(), out ulong newMac))
            {
                SetStatus("MAC 주소 형식 오류  예) 00:E0:22:FE:70:00", Color.OrangeRed);
                return;
            }

            if (!TryGetNodeFromMac(newMac, out byte newNode))
            {
                SetStatus("MAC last byte must be FID/Node 0x01..0x08.", Color.OrangeRed);
                return;
            }
            nudNewNode.Value = newNode;

            if (!ConfirmWrite($"MAC 주소와 Node ID를 동시에 변경합니다.\n\n" +
                              $"현재 MAC : {FormatMac(_currentMac)}\n변경 MAC : {FormatMac(newMac)}\n\n" +
                              $"현재 Node: {_currentNode} (0x{_currentNode:X2})\n변경 Node: {newNode} (0x{newNode:X2})\n\n" +
                              $"계속하시겠습니까?"))
                return;

            SetStatus("MAC + Node ID 쓰는 중...", Color.Yellow);
            Cursor = Cursors.WaitCursor;

            try
            {
                bool macOk  = _ft.WriteMac(newMac);
                bool nodeOk = macOk && _ft.WriteNodeId(newNode);

                if (macOk && nodeOk)
                {
                    _currentMac         = newMac;
                    _currentNode        = newNode;
                    lblCurrentMac.Text  = FormatMac(newMac);
                    lblCurrentNode.Text = $"0x{newNode:X2}  ({newNode})";
                    SetStatus($"전체 변경 완료  MAC={FormatMac(newMac)}  Node={newNode}", Color.LimeGreen);
                }
                else
                {
                    SetStatus("쓰기 실패: " + _ft.LastError, Color.OrangeRed);
                }
            }
            finally
            {
                Cursor = Cursors.Default;
            }
        }

        // =========================================================================
        // OTP WRITE - MAC + Node ID permanent one-time programming
        // =========================================================================
        private void btnProgramOtp_Click(object sender, EventArgs e)
        {
            if (!TryParseMac(txtNewMac.Text.Trim(), out ulong newMac))
            {
                SetStatus("MAC format error. Example: 00:E0:22:FE:70:00", Color.OrangeRed);
                return;
            }

            if (!TryGetNodeFromMac(newMac, out byte newNode))
            {
                SetStatus("OTP MAC last byte must be FID/Node 0x01..0x08.", Color.OrangeRed);
                return;
            }
            nudNewNode.Value = newNode;

            string msg =
                "This will permanently write MAC Address and PLCA Node ID to OTP.\n\n" +
                $"MAC : {FormatMac(newMac)}\n" +
                $"Node/FID: {newNode} (0x{newNode:X2})\n\n" +
                "After success, the value remains after power cycle and the same OTP area cannot be written again.\n" +
                "Continue?";

            if (!ConfirmOtpWrite(msg))
                return;

            SetStatus("OTP programming... Do not power off or disconnect USB.", Color.Yellow);
            Cursor = Cursors.WaitCursor;
            btnProgramOtp.Enabled = false;

            try
            {
                if (_ft.ProgramMacNodeOtp(newMac, newNode))
                {
                    _currentMac = newMac;
                    _currentNode = newNode;
                    lblCurrentMac.Text = FormatMac(newMac);
                    lblCurrentNode.Text = $"0x{newNode:X2}  ({newNode})";
                    SetStatus($"OTP complete  MAC={FormatMac(newMac)}  Node={newNode}", Color.LimeGreen);
                }
                else
                {
                    SetStatus("OTP failed: " + _ft.LastError, Color.OrangeRed);
                }
            }
            finally
            {
                Cursor = Cursors.Default;
                btnProgramOtp.Enabled = _ft.IsConnected;
            }
        }

        // =========================================================================
        // MAC 파싱 / 포맷 유틸
        // =========================================================================
        private static string FormatMac(ulong mac)
        {
            return string.Format("{0:X2}:{1:X2}:{2:X2}:{3:X2}:{4:X2}:{5:X2}",
                (mac >> 40) & 0xFF,
                (mac >> 32) & 0xFF,
                (mac >> 24) & 0xFF,
                (mac >> 16) & 0xFF,
                (mac >>  8) & 0xFF,
                 mac        & 0xFF);
        }

        private static byte GetNodeFromMac(ulong mac)
        {
            return (byte)(mac & 0xFF);
        }

        private static bool TryGetNodeFromMac(ulong mac, out byte nodeId)
        {
            nodeId = GetNodeFromMac(mac);
            return nodeId >= 0x01 && nodeId <= 0x08;
        }

        /// <summary>
        /// "00:E0:22:FE:70:00" 또는 "00E022FE7000" 형식을 파싱합니다.
        /// </summary>
        private static bool TryParseMac(string input, out ulong mac)
        {
            mac = 0;
            if (string.IsNullOrWhiteSpace(input)) return false;

            string cleaned = input.Replace(":", "").Replace("-", "").Replace(" ", "").Trim();
            if (cleaned.Length != 12) return false;

            try
            {
                mac = Convert.ToUInt64(cleaned, 16);
                return true;
            }
            catch
            {
                return false;
            }
        }

        // =========================================================================
        // UI 헬퍼
        // =========================================================================
        private bool ConfirmWrite(string message)
        {
            var result = MessageBox.Show(message, "AD3301 쓰기 확인",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Warning,
                MessageBoxDefaultButton.Button2);
            return result == DialogResult.Yes;
        }

        private bool ConfirmOtpWrite(string message)
        {
            var first = MessageBox.Show(message, "OTP One-Time Write",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Warning,
                MessageBoxDefaultButton.Button2);
            if (first != DialogResult.Yes)
                return false;

            var second = MessageBox.Show("Final confirmation. Once OTP Write starts, it cannot be reverted.",
                "OTP Final Confirmation",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Stop,
                MessageBoxDefaultButton.Button2);
            return second == DialogResult.Yes;
        }

        // =========================================================================
        // MAC 입력 실시간 검증
        // =========================================================================
        private void txtNewMac_Leave(object sender, EventArgs e)
        {
            string txt = txtNewMac.Text.Trim();
            if (string.IsNullOrEmpty(txt)) return;

            if (TryParseMac(txt, out ulong mac))
            {
                txtNewMac.Text      = FormatMac(mac);
                txtNewMac.ForeColor = Color.White;
                if (TryGetNodeFromMac(mac, out byte nodeId))
                {
                    nudNewNode.Value = nodeId;
                    SetStatus($"Node/FID auto = {nodeId} from MAC last byte", Color.LimeGreen);
                }
                else
                {
                    SetStatus("MAC last byte must be FID/Node 0x01..0x08.", Color.OrangeRed);
                }
            }
            else
            {
                txtNewMac.ForeColor = Color.OrangeRed;
                SetStatus("MAC 주소 형식 오류  예) 00:E0:22:FE:70:00  또는  00E022FE7000", Color.OrangeRed);
            }
        }

        private void txtNewMac_Enter(object sender, EventArgs e)
        {
            txtNewMac.ForeColor = Color.White;
        }
    }
}
