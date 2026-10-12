using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using LINMaster.LIN;

namespace LINMaster.Forms
{
    public partial class FormUDS
    {
        private bool _updating;
        private bool _cancelUpdate;
        private bool _readingStatus;
        private readonly StringBuilder _updateLog = new StringBuilder();
        internal bool FirmwareUpdateActive { get { return _updating; } }

        private static uint FirmwareCrc(byte[] data)
        {
            uint crc = 0xFFFFFFFF;
            foreach (byte value in data)
            {
                crc ^= value;
                for (int bit = 0; bit < 8; bit++)
                {
                    crc = (crc & 1) != 0 ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
                }
            }
            return crc ^ 0xFFFFFFFF;
        }

        private static void PutUInt32(byte[] data, int offset, uint value)
        {
            data[offset] = (byte)(value >> 24);
            data[offset + 1] = (byte)(value >> 16);
            data[offset + 2] = (byte)(value >> 8);
            data[offset + 3] = (byte)value;
        }

        private static uint GetUInt32(byte[] data, int offset)
        {
            return ((uint)data[offset] << 24) | ((uint)data[offset + 1] << 16) | ((uint)data[offset + 2] << 8) | data[offset + 3];
        }

        private byte[] UpdateService(byte nad, byte[] request, int responseTimeoutMs = 1000)
        {
            for (int attempt = 0; attempt < 3; attempt++)
            {
                if (_updating && _cancelUpdate)
                {
                    throw new OperationCanceledException();
                }
                byte[] response = SendService(nad, request, responseTimeoutMs, false);
                if (response == null)
                {
                    continue;
                }
                if (response[0] == 0x7F)
                {
                    throw new InvalidOperationException(string.Format("UDS {0:X2} 거절: NRC {1:X2}", request[0], response[2]));
                }
                return response;
            }
            throw new InvalidOperationException(string.Format("UDS {0:X2} 응답 없음 (3회). Boot에서 다시 업데이트할 수 있습니다.", request[0]));
        }

        private byte[] SelectUpdateTarget(UdsNode node)
        {
            byte[] request = { 0x31, 0x01, 0xF3, 0x02, 0, 0, 0, 0 };
            PutUInt32(request, 4, node.InitNumber);
            byte[] response = UpdateService(UDSProtocol.BroadcastNad, request);
            if ((response.Length != 9 && response.Length != 10) || GetUInt32(response, 4) != node.InitNumber || response[8] > 1 || (response.Length == 10 && (response[8] != 1 || response[9] != UDSProtocol.BootNad)))
            {
                throw new InvalidOperationException("업데이트 대상 Init_Number가 일치하지 않습니다.");
            }
            node.ExecutionMode = response[8];
            if (response.Length == 10)
            {
                node.Nad = response[9];
            }
            else if (response[8] == 0)
            {
                if (node.Product == null)
                {
                    UDSProtocol.ProductInfo product;
                    if (!UDSProtocol.TryParseTargetRead(UpdateService(UDSProtocol.BroadcastNad, UDSProtocol.BuildTargetRead(node.InitNumber)), UDSProtocol.TargetReadRoutine, out product) || product.InitNumber != node.InitNumber)
                    {
                        throw new InvalidOperationException("App 제품정보 확인 실패.");
                    }
                    node.Product = product;
                }
                node.Nad = node.Product.Nad;
                node.ChairId = node.Product.Nad;
            }
            return response;
        }

        private void btnUpdate_Click(object sender, EventArgs e)
        {
            if (_updating)
            {
                if (btnUpdate.Enabled)
                {
                    _cancelUpdate = true;
                    btnUpdate.Enabled = false;
                    SetStatus(lblProductStatus, "업데이트 중지 중...", Color.Yellow);
                }
                return;
            }
            if (!CheckConnected() || SelectedNode() == null)
            {
                return;
            }
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = "CJ App firmware (*.bin)|*.bin";
                dialog.Title = "선택한 보드에 설치할 CJ_App.bin";
                if (dialog.ShowDialog(this) != DialogResult.OK)
                {
                    return;
                }
                UpdateFirmware(dialog.FileName);
            }
        }

        private void ReadBoardStatus(UdsNode node)
        {
            if (_busy || _updating || _readingStatus || node == null)
            {
                return;
            }
            _readingStatus = true;
            bool nodesEnabled = lstNodes.Enabled;
            lstNodes.Enabled = false;
            node.ExecutionMode = -1;
            node.RuntimeStatus = "상태 미확인";
            try
            {
                byte[] selected = SelectUpdateTarget(node);
                node.ExecutionMode = selected[8];
                byte[] status = UpdateService(node.Nad, new byte[] { 0x22, 0xF2, 0x10 });
                SetBoardStatus(node, status);
            }
            catch (Exception ex)
            {
                node.RuntimeStatus = "상태 읽기 실패: " + ex.Message;
            }
            finally
            {
                _readingStatus = false;
                lstNodes.Enabled = nodesEnabled;
                if (SelectedNode() == node)
                {
                    txtRuntimeStatus.Text = node.RuntimeStatus;
                }
            }
        }

        private void SetBoardStatus(UdsNode node, byte[] status)
        {
            if ((status.Length != 12 && status.Length != 29 && status.Length != 38) || status[3] > 1)
            {
                throw new InvalidOperationException("Boot/App 상태 응답 오류.");
            }
            node.ExecutionMode = status[3];
            string mode = status[3] == 0 ? "APP" : "BOOT";
            node.RuntimeStatus = "실행: " + mode + " | Flag/Count: 미지원 (기존 펌웨어)";
            if (status.Length >= 29)
            {
                node.RuntimeStatus = string.Format("실행: {0} | Flag(Main/Backup): {1} / {2}\r\nHardFault Count(Main/Backup): {3} / {4} | CRC: {5}", mode, GetUInt32(status, 12), GetUInt32(status, 16), GetUInt32(status, 20), GetUInt32(status, 24), status[28] == 3 ? "정상" : "미저장/불일치");
            }
            if (status.Length == 38)
            {
                node.RuntimeStatus += (status[37] & 1) == 0 ? "\r\n복원 정보: 미저장" : string.Format("\r\n복원 횟수: {0} | 백업: {1:N0} B{2} | APP 정상확인: {3}", GetUInt32(status, 29), GetUInt32(status, 33), (status[37] & 8) != 0 ? " (복원 사용됨)" : "", (status[37] & 4) != 0 ? "완료" : "대기");
            }
            if (SelectedNode() == node)
            {
                txtRuntimeStatus.Text = node.RuntimeStatus;
            }
        }

        internal bool UpdateFirmware(string filename)
        {
            if (!CheckConnected() || SelectedNode() == null)
            {
                return false;
            }
            UdsNode node = SelectedNode();
            byte[] image;
            try
            {
                image = File.ReadAllBytes(filename);
            }
            catch (Exception ex)
            {
                SetStatus(lblProductStatus, "BIN 읽기 실패: " + ex.Message, Color.OrangeRed);
                return false;
            }
            if (image.Length < 192 || image.Length > 192 * 1024)
            {
                SetStatus(lblProductStatus, "App BIN 크기 오류 (192B ~ 192KB / 복원 영역 기준).", Color.OrangeRed);
                return false;
            }
            _updating = true;
            _cancelUpdate = false;
            _updateLog.Clear();
            _logDisplayOffset = _log.Length;
            timerLog.Enabled = _downloadLogEnabled;
            progressUpdate.Value = 0;
            bool preciseTimer = timeBeginPeriod(1) == 0;
            btnScan.Enabled = false;
            btnWriteProduct.Enabled = false;
            btnXcp.Enabled = false;
            btnVerification.Enabled = false;
            btnUpdate.Text = "중지";
            btnUpdate.BackColor = Color.FromArgb(165, 60, 55);
            btnUpdate.Enabled = true;
            btnSaveLog.Enabled = false;
            lstNodes.Enabled = false;
            Stopwatch timer = Stopwatch.StartNew();
            SetStatus(lblProductStatus, "업데이트 0% / 대상 보드 확인 중...", Color.Yellow);
            try
            {
                UDSProtocol.ProductInfo original = null;
                byte[] selected = SelectUpdateTarget(node);
                if (selected[8] == 0)
                {
                    if (!UDSProtocol.TryParseTargetRead(UpdateService(UDSProtocol.BroadcastNad, UDSProtocol.BuildTargetRead(node.InitNumber)), UDSProtocol.TargetReadRoutine, out original) || original.ChairNumber != node.Product.ChairNumber || original.InitNumber != node.InitNumber)
                    {
                        throw new InvalidOperationException("보드 정보가 변경되었습니다. 다시 Scan 하십시오.");
                    }
                }
                byte[] info = UpdateService(node.Nad, new byte[] { 0x22, 0xF2, 0x10 });
                if (info.Length != 12 && info.Length != 29 && info.Length != 38)
                {
                    throw new InvalidOperationException("Boot/App 메모리 정보 응답 오류.");
                }
                SetBoardStatus(node, info);
                if (info.Length == 38 && info[3] == 0 && (info[37] & 4) == 0)
                {
                    SetStatus(lblProductStatus, "현재 APP 정상 부팅 확인 중... (부팅 후 10초)", Color.Yellow);
                    Stopwatch confirmation = Stopwatch.StartNew();
                    while ((info[37] & 4) == 0 && confirmation.ElapsedMilliseconds < 12000)
                    {
                        Delay(500);
                        info = UpdateService(node.Nad, new byte[] { 0x22, 0xF2, 0x10 });
                        SetBoardStatus(node, info);
                        if (info.Length != 38 || info[3] != 0)
                        {
                            throw new InvalidOperationException("APP 상태가 변경되었습니다. 다시 Scan 하십시오.");
                        }
                    }
                    if ((info[37] & 4) == 0)
                    {
                        throw new InvalidOperationException("정상 APP 확인 실패. 백업을 시작하지 않았습니다.");
                    }
                }
                uint address = GetUInt32(info, 4);
                uint capacity = GetUInt32(info, 8);
                uint stack = BitConverter.ToUInt32(image, 0);
                uint entry = BitConverter.ToUInt32(image, 4);
                if (image.Length > capacity || stack <= 0x20000000 || stack > 0x20010000 || (stack & 7) != 0 || (entry & 1) == 0 || entry < address || entry >= address + image.Length)
                {
                    throw new InvalidOperationException("선택한 BIN은 현재 App 영역용 이미지가 아닙니다.");
                }
                SetStatus(lblProductStatus, "Boot 진입 중...", Color.Yellow);
                UpdateService(node.Nad, new byte[] { 0x10, 0x02 });
                if (selected[8] == 0)
                {
                    Delay(700);
                    selected = SelectUpdateTarget(node);
                    if (selected[8] != 1)
                    {
                        throw new InvalidOperationException("Boot 진입 확인 실패.");
                    }
                    UpdateService(node.Nad, new byte[] { 0x10, 0x02 });
                }
                byte[] download = { 0x34, 0x00, 0x44, 0, 0, 0, 0, 0, 0, 0, 0 };
                PutUInt32(download, 3, address);
                PutUInt32(download, 7, (uint)image.Length);
                SetStatus(lblProductStatus, "기존 APP 백업 및 다운로드 준비 중...", Color.Yellow);
                byte[] ready = UpdateService(node.Nad, download, 25000);
                if (ready.Length != 4 || ready[1] != 0x20 || ready[2] != 0 || ready[3] != 130)
                {
                    throw new InvalidOperationException("Boot 블록 크기 응답 오류.");
                }
                SetBoardStatus(node, UpdateService(node.Nad, new byte[] { 0x22, 0xF2, 0x10 }));
                byte sequence = 1;
                for (int offset = 0; offset < image.Length; offset += 128)
                {
                    int count = Math.Min(128, image.Length - offset);
                    byte[] block = new byte[count + 2];
                    block[0] = 0x36;
                    block[1] = sequence;
                    Array.Copy(image, offset, block, 2, count);
                    byte[] ack = UpdateService(node.Nad, block);
                    if (ack.Length != 2 || ack[1] != sequence)
                    {
                        throw new InvalidOperationException("전송 블록 번호 불일치.");
                    }
                    sequence = unchecked((byte)(sequence + 1));
                    progressUpdate.Value = (offset + count) * 100 / image.Length;
                    SetStatus(lblProductStatus, string.Format("업데이트 {0}%  ({1:N0}/{2:N0} byte)", progressUpdate.Value, offset + count, image.Length), Color.Yellow);
                }
                Delay(0);
                btnUpdate.Enabled = false;
                SetStatus(lblProductStatus, "전송 100% / CRC 검증 및 App 재시작 중...", Color.Yellow);
                byte[] exit = { 0x37, 0, 0, 0, 0 };
                PutUInt32(exit, 1, FirmwareCrc(image));
                UpdateService(node.Nad, exit);
                info = UpdateService(node.Nad, new byte[] { 0x22, 0xF2, 0x10 });
                SetBoardStatus(node, info);
                if ((info.Length == 38) && ((info[28] != 3) || ((info[37] & 1) == 0)))
                {
                    throw new InvalidOperationException("다운로드 후 복원 상태 정보 오류. Reset하지 않았습니다.");
                }
                UpdateService(node.Nad, new byte[] { 0x11, 0x01 });
                SetStatus(lblProductStatus, "저장 검증 완료. App 재시작 확인 중...", Color.Yellow);
                Delay(2000);
                bool running = false;
                for (int attempt = 0; attempt < 3 && !running; attempt++)
                {
                    byte[] request = { 0x31, 0x01, 0xF3, 0x02, 0, 0, 0, 0 };
                    PutUInt32(request, 4, node.InitNumber);
                    byte[] response = SendService(UDSProtocol.BroadcastNad, request, 1000, false);
                    running = response != null && response.Length == 9 && response[8] == 0 && GetUInt32(response, 4) == node.InitNumber;
                    if (!running) Delay(1000);
                }
                if (!running)
                {
                    throw new InvalidOperationException("App 실행 확인 실패. 다시 Scan하여 Boot 상태를 확인하십시오.");
                }
                UDSProtocol.ProductInfo verified;
                if ((!UDSProtocol.TryParseTargetRead(UpdateService(UDSProtocol.BroadcastNad, UDSProtocol.BuildTargetRead(node.InitNumber)), UDSProtocol.TargetReadRoutine, out verified)) || (verified.InitNumber != node.InitNumber) || ((original != null) && ((original.SerialNumber != verified.SerialNumber) || (original.PartNumber != verified.PartNumber) || (original.ChairNumber != verified.ChairNumber))))
                {
                    throw new InvalidOperationException("업데이트 후 제품 PDS 정보 확인 실패.");
                }
                node.Product = verified;
                node.Nad = verified.Nad;
                node.ChairId = verified.Nad;
                node.ExecutionMode = 0;
                lstNodes.Items[lstNodes.SelectedIndex] = NodeText(node);
                ShowProduct(verified);
                if (info.Length == 38)
                {
                    SetStatus(lblProductStatus, "새 App 정상 부팅 확인 중... (부팅 후 10초)", Color.Yellow);
                    Stopwatch confirmation = Stopwatch.StartNew();
                    while (true)
                    {
                        SelectUpdateTarget(node);
                        byte[] status = UpdateService(node.Nad, new byte[] { 0x22, 0xF2, 0x10 });
                        SetBoardStatus(node, status);
                        if ((status.Length != 38) || (status[28] != 3) || ((status[37] & 1) == 0) || (GetUInt32(status, 29) < GetUInt32(info, 29)))
                        {
                            throw new InvalidOperationException("새 App 정상확인 실패: 복원 상태 정보 오류. 다시 Scan 하십시오.");
                        }
                        if ((GetUInt32(status, 29) > GetUInt32(info, 29)) || (((info[37] & 8) == 0) && ((status[37] & 8) != 0)))
                        {
                            throw new InvalidOperationException("업데이트 실패 / 새 App 실행 실패 → 이전 App 복원됨.");
                        }
                        if ((status[3] != 0) || (GetUInt32(status, 12) != 0) || (GetUInt32(status, 16) != 0) || (GetUInt32(status, 20) != 0) || (GetUInt32(status, 24) != 0))
                        {
                            throw new InvalidOperationException("새 App 정상확인 실패: Boot/다운로드 Flag/HardFault 상태를 확인하십시오.");
                        }
                        if ((status[37] & 4) != 0)
                        {
                            break;
                        }
                        if (confirmation.ElapsedMilliseconds >= 12000)
                        {
                            throw new InvalidOperationException("새 App 정상 부팅 확인 시간 초과. 업데이트 성공으로 처리하지 않았습니다.");
                        }
                        Delay(500);
                    }
                }
                SetStatus(lblProductStatus, string.Format("업데이트 완료 / App 실행·제품정보 확인 OK / {0:F1}초", timer.Elapsed.TotalSeconds), Color.LimeGreen);
                return true;
            }
            catch (OperationCanceledException)
            {
                SetStatus(lblProductStatus, string.Format("업데이트 중지 ({0}%) / 다시 Scan 후 Update로 재시도하십시오.", progressUpdate.Value), Color.Yellow);
                AppendLog("UPDATE STOP: 사용자 요청으로 이후 전송 중지. 완료 요청 및 Reset을 보내지 않습니다.", Color.Yellow);
                return false;
            }
            catch (Exception ex)
            {
                SetStatus(lblProductStatus, ex.Message, Color.OrangeRed);
                AppendLog("UPDATE FAIL: " + ex.Message, Color.OrangeRed);
                return false;
            }
            finally
            {
                if (preciseTimer)
                {
                    timeEndPeriod(1);
                }
                try
                {
                    string directory = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "UpdateLogs");
                    Directory.CreateDirectory(directory);
                    File.WriteAllText(Path.Combine(directory, string.Format("{0}_{1:X8}_{2:yyyyMMdd_HHmmss}.txt", node.Product == null ? "BOOT" : node.Product.ChairNumber.ToString("X2"), node.InitNumber, DateTime.Now)), lblProductStatus.Text + Environment.NewLine + _updateLog.ToString());
                }
                catch (Exception ex) when (ex is IOException || ex is UnauthorizedAccessException)
                {
                    AppendLog("자동 로그 저장 실패: " + ex.Message, Color.OrangeRed);
                }
                timerLog.Stop();
                timerLog_Tick(null, EventArgs.Empty);
                _updating = false;
                _logDisplayOffset = _log.Length;
                btnUpdate.Text = "Update / App BIN";
                btnUpdate.BackColor = Color.FromArgb(160, 95, 35);
                btnSaveLog.Enabled = true;
                btnScan.Enabled = true;
                btnWriteProduct.Enabled = true;
                lstNodes.Enabled = true;
                ReadBoardStatus(node);
                UpdateXcpButton();
            }
        }

        private static bool ServiceMatches(byte[] request, byte[] response)
        {
            if (response == null || response.Length == 0 || response[0] != request[0] + 0x40)
            {
                return false;
            }
            switch (request[0])
            {
                case 0x10: return response.Length == 6 && response[1] == request[1];
                case 0x11:
                case 0x36:
                case 0x3E: return response.Length == 2 && response[1] == request[1];
                case 0x34: return response.Length >= 3;
                case 0x37: return response.Length == 1;
                case 0x31: return response.Length >= 4 && response[1] == request[1] && response[2] == request[2] && response[3] == request[3];
                default: return response.Length >= 3 && request.Length >= 3 && response[1] == request[1] && response[2] == request[2];
            }
        }
    }
}
