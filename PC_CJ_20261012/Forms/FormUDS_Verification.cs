using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using LINMaster.LIN;

namespace LINMaster.Forms
{
    public partial class FormUDS
    {
        [DllImport("winmm.dll")]
        private static extern uint timeBeginPeriod(uint period);
        [DllImport("winmm.dll")]
        private static extern uint timeEndPeriod(uint period);

        private static readonly string[] VerificationZones = { "FRT LH", "FRT RH", "Headrest", "Back", "Logo", "Strobe" };
        private static readonly string[] VerificationColors = { "R", "G", "B", "W" };
        private static readonly int[] VerificationLevels = { 10, 50, 100 };
        private static readonly byte[] VerificationNodeCounts = { 1, 1, 1, 1, 24, 8 };
        private readonly List<VerificationReport.Row> _verificationRows = new List<VerificationReport.Row>();
        private readonly List<VerificationReport.Row> _verificationDetailRows = new List<VerificationReport.Row>();
        private byte _verificationSequence;
        private long _verificationReadWaitMs;
        private long _verificationEffectWaitMs;
        private int _verificationTxCount;
        private int _verificationRxCount;
        private int _verificationHeaderCount;

        private void btnVerification_Click(object sender, EventArgs e)
        {
            if (_verifying)
            {
                _cancelVerification = true;
                btnVerification.Text = "중단 중...";
                return;
            }
            if (!CheckConnected()) return;
            UdsNode node = SelectedNode();
            UpdateXcpButton();
            if (node == null || !btnVerification.Enabled) return;

            _verifying = true;
            _cancelVerification = false;
            _verificationRows.Clear();
            _verificationDetailRows.Clear();
            _verificationReadWaitMs = 0;
            _verificationEffectWaitMs = 0;
            _verificationTxCount = 0;
            _verificationRxCount = 0;
            _verificationHeaderCount = 0;
            btnScan.Enabled = false;
            btnWriteProduct.Enabled = false;
            btnXcp.Enabled = false;
            btnVerification.Text = "검사 중지";
            SetStatus(lblProductStatus, "LED 자동검사 시작", Color.Yellow);
            DateTime started = DateTime.Now;
            Stopwatch testTimer = Stopwatch.StartNew();
            long zoneMs = 0;
            long colorMs = 0;
            long fadeMs = 0;
            long flickerMs = 0;
            string failure = null;
            bool timerPeriodSet = timeBeginPeriod(1U) == 0U;
            try
            {
                if (!SendVerificationCommand(node.Product.ChairNumber, CJVerificationProtocol.Start, 0, 0, 0, 0)) throw new InvalidOperationException("자동검사 시작 송신 실패");
                byte[] protocol = WaitVerificationResponse(node.Product.ChairNumber, 0x87, CJVerificationProtocol.ProtocolVersion, 0, 400, 0, 0x32);
                if (protocol == null || protocol[2] != 0x56)
                {
                    throw new InvalidOperationException("고속 자동검사 펌웨어 확인 실패. 최신 IAR 펌웨어를 다운로드해 주세요.");
                }
                Delay(20);
                RunZoneVerification(node.Product.ChairNumber);
                zoneMs = testTimer.ElapsedMilliseconds;
                if (!_cancelVerification) RunStaticVerification(node.Product.ChairNumber);
                colorMs = testTimer.ElapsedMilliseconds - zoneMs;
                if (!_cancelVerification) RunFadeVerification(node.Product.ChairNumber);
                fadeMs = testTimer.ElapsedMilliseconds - zoneMs - colorMs;
                if (!_cancelVerification) RunFlickerVerification(node.Product.ChairNumber);
                flickerMs = testTimer.ElapsedMilliseconds - zoneMs - colorMs - fadeMs;
            }
            catch (Exception ex)
            {
                failure = ex.Message;
                AppendLog("Verification 중단: " + failure, Color.OrangeRed);
            }
            finally
            {
                if (timerPeriodSet)
                {
                    timeEndPeriod(1U);
                }
                testTimer.Stop();
                SendVerificationCommand(node.Product.ChairNumber, CJVerificationProtocol.Stop, 0, 0, 0, 0);
                SendVerificationLamp(node.Product.ChairNumber, 0x3F, 0x07, 0, 0, 0, 0, 0);
                AppendLog(string.Format("검사 {0:F1}s: 구역 {1:F1}s, RGBW {2:F1}s, Fade {3:F1}s, Flicker {4:F1}s", testTimer.Elapsed.TotalSeconds, zoneMs / 1000.0, colorMs / 1000.0, fadeMs / 1000.0, flickerMs / 1000.0), Color.LightBlue);
                AppendLog(string.Format("LIN 왕복 대기: RGB read {0:F1}s, 효과 계측 {1:F1}s / TX {2}, 응답 {3}, 헤더 {4} (효과 대기에는 실제 계측시간 포함)", _verificationReadWaitMs / 1000.0, _verificationEffectWaitMs / 1000.0, _verificationTxCount, _verificationRxCount, _verificationHeaderCount), Color.LightBlue);
                try
                {
                    Stopwatch reportTimer = Stopwatch.StartNew();
                    string path = VerificationReport.Save(node.Product, started, _verificationRows, _verificationDetailRows, _cancelVerification || failure != null);
                    reportTimer.Stop();
                    AppendLog(string.Format("성적서 저장 {0:F1}s / 전체 {1:F1}s", reportTimer.Elapsed.TotalSeconds, testTimer.Elapsed.TotalSeconds + reportTimer.Elapsed.TotalSeconds), Color.LightBlue);
                    int failed = _verificationRows.FindAll(row => !row.Passed).Count;
                    SetStatus(lblProductStatus, string.Format("검사 {0}: Pass {1} / Fail {2}\r\n{3}", _cancelVerification ? "취소" : (failure == null ? "완료" : "중단"), _verificationRows.Count - failed, failed, path), failure == null && !_cancelVerification && failed == 0 ? Color.LimeGreen : Color.OrangeRed);
                    AppendLog("성적서: " + path, Color.FromArgb(120, 220, 180));
                }
                catch (Exception ex)
                {
                    SetStatus(lblProductStatus, "성적서 저장 실패: " + ex.Message, Color.OrangeRed);
                }
                _verifying = false;
                btnScan.Enabled = true;
                btnWriteProduct.Enabled = true;
                btnVerification.Text = "Verification / LED 자동검사 · Excel 성적서";
                UpdateXcpButton();
            }
        }

        private void RunZoneVerification(byte chair)
        {
            bool[] zonePassed = { true, true, true, true, true, true };
            VerificationRead[] offReads = new VerificationRead[VerificationZones.Length];
            if (!SendVerificationLamp(chair, 0x3F, 0x07, 0, 0, 0, 0, 0)) throw new InvalidOperationException("구역 검사 준비 실패");
            Delay(30);
            for (byte selected = 0; selected < VerificationZones.Length && !_cancelVerification; selected++)
            {
                if (!SendVerificationLamp(chair, (byte)(1 << selected), 0x17, 128, 128, 128, 0, 0)) throw new InvalidOperationException("구역 ON 송신 실패");
                Delay(35);
                for (byte zone = 0; zone < VerificationZones.Length && !_cancelVerification; zone++)
                {
                    byte nodesToRead = zone == selected ? VerificationNodeCounts[zone] : (byte)1;
                    for (byte led = 0; led < nodesToRead && !_cancelVerification; led++)
                    {
                        VerificationRead read = ReadVerificationLedFast(chair, zone, led);
                        ushort expected = zone == selected ? (ushort)(zone < 4 ? 128 * 257 : 128 * 32767 / 255) : (ushort)0;
                        bool pass = VerificationReadPassed(read, expected, expected, expected);
                        if (!pass) zonePassed[zone] = false;
                        if (zone == selected) AddVerificationDetailRow(zone, led, "구역 ON", "W", 50, 0, 0, expected, expected, expected, read, pass, "선택 구역 ON readback");
                        if (read.Complete && (read.Status & 0x80) != 0) throw new InvalidOperationException("온도 또는 전압 고장: 과열 방지를 위해 중단");
                    }
                }
                if (!SendVerificationLamp(chair, (byte)(1 << selected), 0x07, 0, 0, 0, 0, 0)) throw new InvalidOperationException("구역 OFF 송신 실패");
                Delay(25);
                for (byte led = 0; led < VerificationNodeCounts[selected] && !_cancelVerification; led++)
                {
                    VerificationRead read = ReadVerificationLedFast(chair, selected, led);
                    if (led == 0) offReads[selected] = read;
                    bool pass = VerificationReadPassed(read, 0, 0, 0);
                    if (!pass) zonePassed[selected] = false;
                    AddVerificationDetailRow(selected, led, "구역 OFF", "W", 0, 0, 0, 0, 0, 0, read, pass, "선택 구역 OFF readback");
                }
            }
            if (_cancelVerification) return;
            for (byte zone = 0; zone < VerificationZones.Length; zone++)
            {
                AddVerificationRow(zone, 0, "구역 On/Off", "W", 50, 0, 0, 0, 0, 0, offReads[zone], zonePassed[zone], string.Format("6구역 교차 확인, {0}개 LED ON/OFF readback", VerificationNodeCounts[zone]));
            }
        }

        private void RunStaticVerification(byte chair)
        {
            for (int color = 0; color < VerificationColors.Length && !_cancelVerification; color++)
            {
                foreach (int level in VerificationLevels)
                {
                    if (_cancelVerification) return;
                    byte value = (byte)((level * 255 + 50) / 100);
                    byte red = (color == 0 || color == 3) ? value : (byte)0;
                    byte green = (color == 1 || color == 3) ? value : (byte)0;
                    byte blue = (color == 2 || color == 3) ? value : (byte)0;
                    if (!SendVerificationLamp(chair, 0x3F, 0x17, red, green, blue, 0, 0)) throw new InvalidOperationException("LED 명령 송신 실패");
                    Delay(35);
                    for (byte zone = 0; zone < VerificationZones.Length && !_cancelVerification; zone++)
                    {
                        ushort scale = (ushort)(zone < 4 ? 257 : 0);
                        ushort expectedRed = zone < 4 ? (ushort)(red * scale) : (ushort)(red * 32767 / 255);
                        ushort expectedGreen = zone < 4 ? (ushort)(green * scale) : (ushort)(green * 32767 / 255);
                        ushort expectedBlue = zone < 4 ? (ushort)(blue * scale) : (ushort)(blue * 32767 / 255);
                        VerificationRead reportRead = null;
                        bool pass = true;
                        for (byte led = 0; led < VerificationNodeCounts[zone] && !_cancelVerification; led++)
                        {
                            VerificationRead read = ReadVerificationLedFast(chair, zone, led);
                            if (led == 0) reportRead = read;
                            bool ledPass = VerificationReadPassed(read, expectedRed, expectedGreen, expectedBlue);
                            if (!ledPass) pass = false;
                            AddVerificationDetailRow(zone, led, "RGBW", VerificationColors[color], level, 0, 0, expectedRed, expectedGreen, expectedBlue, read, ledPass, "LED별 Write/Read 및 진단 확인");
                            if (read.Complete && (read.Status & 0x80) != 0) throw new InvalidOperationException("온도 또는 전압 고장: 과열 방지를 위해 중단");
                        }
                        AddVerificationRow(zone, 0, "RGBW", VerificationColors[color], level, 0, 0, expectedRed, expectedGreen, expectedBlue, reportRead, pass, string.Format("{0}개 LED RX/PWM/Open/Short {1}", VerificationNodeCounts[zone], pass ? "정상" : "불일치 또는 진단 고장"));
                    }
                }
            }
        }

        private void RunFadeVerification(byte chair)
        {
            if (!SendVerificationLamp(chair, 0x3F, 0x07, 0, 0, 0, 0, 0)) throw new InvalidOperationException("Fade 준비 송신 실패");
            Delay(30);
            if (!SendVerificationCommand(chair, CJVerificationProtocol.Fade, CJVerificationProtocol.AllZones, 0, 0, 0)) throw new InvalidOperationException("Fade 계측 시작 실패");
            Delay(20);
            for (byte step = 0; step < 10 && !_cancelVerification; step++)
            {
                if (!SendVerificationLamp(chair, 0x3F, 0x77, (byte)(72 + step * 2), 80, 255, 1, 1)) throw new InvalidOperationException("Fade 좌표 송신 실패");
                Delay(90);
            }
            if (_cancelVerification) return;
            byte[] timing = WaitVerificationResponse(chair, 0x83, 0, 0, 1500, 50);
            Delay(150);
            for (byte zone = 0; zone < VerificationZones.Length && !_cancelVerification; zone++)
            {
                if (zone != 0)
                {
                    if (!SendVerificationCommand(chair, CJVerificationProtocol.Result, zone, 0, 0, 0)) throw new InvalidOperationException("Fade 결과 요청 실패");
                    timing = WaitVerificationResponse(chair, 0x83, zone, 0, 400);
                }
                int samples = timing == null ? 0 : (timing[2] << 8) | timing[3];
                int changes = timing == null ? 0 : (timing[4] << 8) | timing[5];
                VerificationRead reportRead = null;
                bool pass = timing != null && changes >= 8;
                for (byte step = 0; step < 10 && !_cancelVerification; step++)
                {
                    VerificationRead read = ReadVerificationFadePoint(chair, zone, step);
                    bool stepPass = VerificationReadPassed(read, read.Expected[0], read.Expected[1], read.Expected[2]);
                    if (!stepPass) pass = false;
                    AddVerificationDetailRow(zone, 0, "Fade In Step", string.Format("Step {0}/10", step + 1), 100, 0, 0, read.Expected[0], read.Expected[1], read.Expected[2], read, stepPass, string.Format("LED 1 실제 RX: 계측 시작 후 {0}ms, 읽기 {1}ms, TX/RX 허용 1%", read.SampleTimeMs, read.ReadDurationMs == 0 ? "<1" : read.ReadDurationMs.ToString()));
                }
                for (byte led = 0; led < VerificationNodeCounts[zone] && !_cancelVerification; led++)
                {
                    VerificationRead read = ReadVerificationLed(chair, zone, led);
                    if (led == 0) reportRead = read;
                    bool ledPass = VerificationReadPassed(read, read.Expected[0], read.Expected[1], read.Expected[2]);
                    if (!ledPass) pass = false;
                    AddVerificationDetailRow(zone, led, "Fade In", "CIE1931", 100, 0, 0, read.Expected[0], read.Expected[1], read.Expected[2], read, ledPass, "Fade In 최종 Write/Read 확인");
                }
                if (_cancelVerification) return;
                AddVerificationRow(zone, 0, "Fade In", "CIE1931", 100, 0, 0, reportRead.Expected[0], reportRead.Expected[1], reportRead.Expected[2], reportRead, pass, string.Format("LED 1 중간 10점 실제 RX, 최종 {0}개 LED RX, 허용 1%; 변화 {1}, 관찰 {2}회", VerificationNodeCounts[zone], changes, samples));
            }
            if (_cancelVerification) return;
            if (!SendVerificationCommand(chair, CJVerificationProtocol.Fade, CJVerificationProtocol.AllZones, 0, 0, 0)) throw new InvalidOperationException("Fade Out 계측 시작 실패");
            if (!SendVerificationLamp(chair, 0x3F, 0x67, 90, 80, 255, 1, 10)) throw new InvalidOperationException("Fade Out 송신 실패");
            timing = WaitVerificationResponse(chair, 0x83, 0, 0, 1500, 1000);
            for (byte zone = 0; zone < VerificationZones.Length && !_cancelVerification; zone++)
            {
                if (zone != 0)
                {
                    if (!SendVerificationCommand(chair, CJVerificationProtocol.Result, zone, 0, 0, 0)) throw new InvalidOperationException("Fade Out 결과 요청 실패");
                    timing = WaitVerificationResponse(chair, 0x83, zone, 0, 400);
                }
                int samples = timing == null ? 0 : (timing[2] << 8) | timing[3];
                int changes = timing == null ? 0 : (timing[4] << 8) | timing[5];
                VerificationRead reportRead = null;
                bool pass = timing != null && changes >= 8;
                for (byte step = 0; step < 10 && !_cancelVerification; step++)
                {
                    VerificationRead read = ReadVerificationFadePoint(chair, zone, step);
                    bool stepPass = VerificationReadPassed(read, read.Expected[0], read.Expected[1], read.Expected[2]);
                    if (!stepPass) pass = false;
                    AddVerificationDetailRow(zone, 0, "Fade Out Step", string.Format("Step {0}/10", step + 1), 0, 0, 0, read.Expected[0], read.Expected[1], read.Expected[2], read, stepPass, string.Format("LED 1 실제 RX: 계측 시작 후 {0}ms, 읽기 {1}ms, TX/RX 허용 1%", read.SampleTimeMs, read.ReadDurationMs == 0 ? "<1" : read.ReadDurationMs.ToString()));
                }
                for (byte led = 0; led < VerificationNodeCounts[zone] && !_cancelVerification; led++)
                {
                    VerificationRead read = ReadVerificationLed(chair, zone, led);
                    if (led == 0) reportRead = read;
                    bool ledPass = VerificationReadPassed(read, read.Expected[0], read.Expected[1], read.Expected[2]);
                    if (!ledPass) pass = false;
                    AddVerificationDetailRow(zone, led, "Fade Out", "W", 0, 0, 0, read.Expected[0], read.Expected[1], read.Expected[2], read, ledPass, "Fade Out 최종 Write/Read 확인");
                }
                if (_cancelVerification) return;
                AddVerificationRow(zone, 0, "Fade Out", "W", 0, 0, 0, reportRead.Expected[0], reportRead.Expected[1], reportRead.Expected[2], reportRead, pass, string.Format("LED 1 중간 10점 실제 RX, 최종 {0}개 LED RX, 허용 1%; 변화 {1}, 관찰 {2}회", VerificationNodeCounts[zone], changes, samples));
            }
        }

        private void RunFlickerVerification(byte chair)
        {
            foreach (byte hz in new byte[] { 10, 50 })
            {
                foreach (byte duty in new byte[] { 10, 90 })
                {
                    if (_cancelVerification) return;
                    ushort captureMs = (ushort)(4000 / hz);
                    if (!SendVerificationLamp(chair, 0x3F, 0x16, 128, 128, 128, (byte)(duty * 250 / 100), hz)) throw new InvalidOperationException("Flicker 명령 송신 실패");
                    Delay(30);
                    if (!SendVerificationCommand(chair, CJVerificationProtocol.Flicker, CJVerificationProtocol.AllZones, 0, 0, 0, captureMs)) throw new InvalidOperationException("Flicker 계측 시작 실패");
                    byte[] timing = WaitVerificationResponse(chair, 0x82, 0, 0, captureMs + 500, captureMs);
                    for (byte zone = 0; zone < VerificationZones.Length && !_cancelVerification; zone++)
                    {
                        if (zone != 0)
                        {
                            if (!SendVerificationCommand(chair, CJVerificationProtocol.Result, zone, 0, 0, 0)) throw new InvalidOperationException("Flicker 결과 요청 실패");
                            timing = WaitVerificationResponse(chair, 0x82, zone, 0, 400);
                        }
                        int minPeriod = timing == null ? 0 : timing[2];
                        int maxPeriod = timing == null ? 0 : timing[3];
                        int minOn = timing == null ? 0 : timing[4];
                        int maxOn = timing == null ? 0 : timing[5];
                        int minOff = timing == null ? 0 : timing[6];
                        int maxOff = timing == null ? 0 : timing[7];
                        int expectedPeriod = 1000 / hz;
                        int expectedOn = (expectedPeriod * duty + 50) / 100;
                        int expectedOff = expectedPeriod - expectedOn;
                        List<VerificationReport.Row> rgbwRows = _verificationRows.FindAll(item => item.Zone == VerificationZones[zone] && item.Test == "RGBW");
                        bool hardwarePassed = _verificationRows.Exists(item => item.Zone == VerificationZones[zone] && item.Test == "구역 On/Off" && item.Passed) && rgbwRows.Count == 12 && rgbwRows.TrueForAll(item => item.Passed);
                        bool timingPassed = timing != null && minPeriod > 0 && minPeriod <= maxPeriod && minOn > 0 && minOn <= maxOn && minOff > 0 && minOff <= maxOff && Math.Abs(minPeriod - expectedPeriod) <= 1 && Math.Abs(maxPeriod - expectedPeriod) <= 1 && Math.Abs(minOn - expectedOn) <= 1 && Math.Abs(maxOn - expectedOn) <= 1 && Math.Abs(minOff - expectedOff) <= 1 && Math.Abs(maxOff - expectedOff) <= 1;
                        AddVerificationRow(zone, 0, "Flicker", "W", 50, hz, duty, 0, 0, 0, null, timingPassed && hardwarePassed, string.Format("MCU TX 시점 계측: 주기 {0}~{1}/{2}ms, ON {3}~{4}/{5}ms, OFF {6}~{7}/{8}ms, ±1ms; 시간 {9}, 구역/RGBW {10}. 플리커 측정 시트 참조", minPeriod, maxPeriod, expectedPeriod, minOn, maxOn, expectedOn, minOff, maxOff, expectedOff, timingPassed ? "Pass" : "Fail", hardwarePassed ? "Pass" : "Fail"));
                        VerificationReport.Row row = _verificationRows[_verificationRows.Count - 1];
                        row.TimingReceived = timing != null;
                        row.ExpectedPeriodMs = expectedPeriod;
                        row.ExpectedOnMs = expectedOn;
                        row.ExpectedOffMs = expectedOff;
                        row.MinPeriodMs = minPeriod;
                        row.MaxPeriodMs = maxPeriod;
                        row.MinOnMs = minOn;
                        row.MaxOnMs = maxOn;
                        row.MinOffMs = minOff;
                        row.MaxOffMs = maxOff;
                        row.TimingPassed = timingPassed;
                        row.HardwarePassed = hardwarePassed;
                    }
                }
            }
        }

        private VerificationRead ReadVerificationLedFast(byte chair, byte zone, byte led)
        {
            VerificationRead read = new VerificationRead();
            for (int attempt = 0; (attempt < 3) && !_cancelVerification && _gateway.IsConnected; attempt++)
            {
                _verificationSequence++;
                if (_verificationSequence == 0)
                {
                    _verificationSequence = 1;
                }
                if (!SendVerificationCommand(chair, CJVerificationProtocol.ReadRgb, zone, led, 0, 1, _verificationSequence))
                {
                    return read;
                }
                byte[] result = WaitVerificationResponse(chair, 0x85, zone, _verificationSequence, 400, 10);
                if (result == null)
                {
                    if (_cancelVerification || !_gateway.IsConnected)
                    {
                        return read;
                    }
                    AppendLog(string.Format("{0} LED {1}: 검사 응답 미수신 ({2}/3)", VerificationZones[zone], led + 1, attempt + 1), Color.OrangeRed);
                    continue;
                }
                read.Status = result[0];
                read.Actual[0] = (ushort)((result[1] << 8) | result[2]);
                read.Actual[1] = (ushort)((result[3] << 8) | result[4]);
                read.Actual[2] = (ushort)((result[5] << 8) | result[6]);
                read.Complete = true;
                return read;
            }
            return read;
        }

        private VerificationRead ReadVerificationLed(byte chair, byte zone, byte led)
        {
            VerificationRead read = ReadVerificationLedFast(chair, zone, led);
            if (!read.Complete)
            {
                return read;
            }
            read.Complete = false;
            _verificationSequence++;
            if (_verificationSequence == 0)
            {
                _verificationSequence = 1;
            }
            if (!SendVerificationCommand(chair, CJVerificationProtocol.ReadExpected, zone, led, 0, 0, _verificationSequence)) return read;
            byte[] result = WaitVerificationResponse(chair, 0x85, zone, _verificationSequence, 400);
            if (result == null) return read;
            read.Expected[0] = (ushort)((result[1] << 8) | result[2]);
            read.Expected[1] = (ushort)((result[3] << 8) | result[4]);
            read.Expected[2] = (ushort)((result[5] << 8) | result[6]);
            read.Complete = true;
            return read;
        }

        private VerificationRead ReadVerificationFadePoint(byte chair, byte zone, byte step)
        {
            VerificationRead read = new VerificationRead();
            for (byte source = 0; source < 2; source++)
            {
                _verificationSequence++;
                if (_verificationSequence == 0)
                {
                    _verificationSequence = 1;
                }
                if (!SendVerificationCommand(chair, CJVerificationProtocol.FadeRgb, zone, step, source, 0, _verificationSequence)) return read;
                byte[] result = WaitVerificationResponse(chair, 0x85, zone, _verificationSequence, 400);
                if (result == null) return read;
                for (byte color = 0; color < 3; color++)
                {
                    if (source == 0)
                    {
                        read.Expected[color] = (ushort)((result[1 + color * 2] << 8) | result[2 + color * 2]);
                    }
                    else
                    {
                        read.Actual[color] = (ushort)((result[1 + color * 2] << 8) | result[2 + color * 2]);
                    }
                }
            }
            if (!SendVerificationCommand(chair, CJVerificationProtocol.FadeStatus, zone, step, 0, 0)) return read;
            byte[] status = WaitVerificationResponse(chair, 0x89, zone, step, 400);
            if (status == null) return read;
            read.Status = status[3];
            read.SampleTimeMs = (status[4] << 8) | status[5];
            read.ReadDurationMs = (status[6] << 8) | status[7];
            read.Complete = true;
            return read;
        }

        private static bool VerificationReadPassed(VerificationRead read, ushort expectedRed, ushort expectedGreen, ushort expectedBlue)
        {
            return read != null && read.Complete && (read.Status & 0xF1) == 0x01 && WithinOnePercent(expectedRed, read.Actual[0]) && WithinOnePercent(expectedGreen, read.Actual[1]) && WithinOnePercent(expectedBlue, read.Actual[2]);
        }

        private static string VerificationReadState(VerificationRead read)
        {
            if (read == null) return string.Empty;
            if (!read.Complete) return " / 응답 없음";
            string state = string.Empty;
            if ((read.Status & 0x41) != 0x01) state += " 통신 오류";
            if ((read.Status & 0x10) != 0) state += " Open";
            if ((read.Status & 0x20) != 0) state += " Short";
            if ((read.Status & 0x80) != 0) state += " 온도/전압";
            return state.Length == 0 ? " / RX·진단 정상" : " / 진단:" + state;
        }

        private static bool WithinOnePercent(ushort expected, ushort actual)
        {
            if (expected == 0) return actual == 0;
            int tolerance = (expected + 99) / 100;
            return Math.Abs((int)actual - expected) <= tolerance;
        }

        private bool SendVerificationLamp(byte chair, byte zoneMask, byte control, byte component1, byte component2, byte component3, byte operation6, byte operation7)
        {
            LINFrame frame = new LINFrame { FrameID = CJLinProtocol.ControlFrameId, DataLength = 8, FrameType = LINFrameType.Publisher, ChecksumType = LINChecksumType.Enhanced };
            byte[] data = { (byte)(1 << (chair - 0x0A)), zoneMask, control, component1, component2, component3, operation6, operation7 };
            Array.Copy(data, frame.Data, 8);
            if (!_gateway.IsConnected || !_gateway.SendPublisher(frame)) return false;
            _verificationTxCount++;
            return true;
        }

        private bool SendVerificationCommand(byte chair, byte command, byte zone, byte led, byte color, byte fresh, ushort captureMs = 0)
        {
            if (!_gateway.IsConnected) return false;
            LINFrame frame = new LINFrame { FrameID = CJVerificationProtocol.RequestId(chair), DataLength = 8, FrameType = LINFrameType.Publisher, ChecksumType = LINChecksumType.Enhanced };
            frame.Data[0] = command;
            frame.Data[1] = zone;
            frame.Data[2] = led;
            frame.Data[3] = color;
            frame.Data[4] = fresh;
            frame.Data[5] = (byte)captureMs;
            frame.Data[6] = (byte)(captureMs >> 8);
            lock (_responseLock) _verificationResponses.Clear();
            if (!_gateway.SendPublisher(frame)) return false;
            _verificationTxCount++;
            return true;
        }

        private byte[] WaitVerificationResponse(byte chair, byte opcode, byte zone, byte led, int timeoutMs, int readyAfterMs = 0, int data3 = -1)
        {
            Stopwatch timer = Stopwatch.StartNew();
            long nextHeader = readyAfterMs;
            long nextUi = 0;
            try
            {
                while (timer.ElapsedMilliseconds < timeoutMs && !_cancelVerification)
                {
                    if (!_gateway.IsConnected) return null;
                    _gateway.Poll();
                    LINFrame frame = null;
                    lock (_responseLock)
                    {
                        if (_verificationResponses.Count != 0) frame = _verificationResponses.Dequeue();
                    }
                    if (frame != null && frame.FrameID == CJVerificationProtocol.ResponseId(chair))
                    {
                        nextHeader = Math.Max(readyAfterMs, timer.ElapsedMilliseconds);
                    }
                    if (frame != null && frame.FrameID == CJVerificationProtocol.ResponseId(chair) && frame.IsValid)
                    {
                        if (opcode == 0x85 && frame.Data[7] == led)
                        {
                            _verificationRxCount++;
                            return (byte[])frame.Data.Clone();
                        }
                        if (opcode != 0x85 && frame.Data[0] == opcode && frame.Data[1] == zone && ((opcode != 0x81 && opcode != 0x88 && opcode != 0x89) || frame.Data[2] == led) && (data3 < 0 || frame.Data[3] == data3))
                        {
                            _verificationRxCount++;
                            return (byte[])frame.Data.Clone();
                        }
                    }
                    if (timer.ElapsedMilliseconds >= nextHeader)
                    {
                        if (_gateway.SendSubscriberRequest(CJVerificationProtocol.ResponseId(chair), 8, LINChecksumType.Enhanced)) _verificationHeaderCount++;
                        nextHeader = timer.ElapsedMilliseconds + 30;
                    }
                    if (timer.ElapsedMilliseconds >= nextUi)
                    {
                        Application.DoEvents();
                        nextUi = timer.ElapsedMilliseconds + 10;
                    }
                    Thread.Sleep(1);
                }
                return null;
            }
            finally
            {
                if (opcode == 0x85 || opcode == 0x81) _verificationReadWaitMs += timer.ElapsedMilliseconds;
                else _verificationEffectWaitMs += timer.ElapsedMilliseconds;
            }
        }

        private void AddVerificationRow(byte zone, byte led, string test, string color, int level, int hz, int duty, ushort expectedRed, ushort expectedGreen, ushort expectedBlue, VerificationRead read, bool pass, string detail)
        {
            _verificationRows.Add(new VerificationReport.Row { Zone = VerificationZones[zone], Led = led + 1, Test = test, Color = color, Brightness = level, Frequency = hz, Duty = duty, ExpectedRed = expectedRed, ExpectedGreen = expectedGreen, ExpectedBlue = expectedBlue, ActualRed = read == null ? (ushort)0 : read.Actual[0], ActualGreen = read == null ? (ushort)0 : read.Actual[1], ActualBlue = read == null ? (ushort)0 : read.Actual[2], Passed = pass, RxAvailable = read != null && read.Complete && (read.Status & 0x01) != 0, Detail = detail + VerificationReadState(read) });
            if (_verificationRows.Count % 12 == 0 || !pass) SetStatus(lblProductStatus, string.Format("{0} LED {1}: {2} {3}% / 누적 {4}건", VerificationZones[zone], led + 1, test, level, _verificationRows.Count), pass ? Color.Yellow : Color.OrangeRed);
        }

        private void AddVerificationDetailRow(byte zone, byte led, string test, string color, int level, int hz, int duty, ushort expectedRed, ushort expectedGreen, ushort expectedBlue, VerificationRead read, bool pass, string detail)
        {
            _verificationDetailRows.Add(new VerificationReport.Row { Zone = VerificationZones[zone], Led = led + 1, Test = test, Color = color, Brightness = level, Frequency = hz, Duty = duty, ExpectedRed = expectedRed, ExpectedGreen = expectedGreen, ExpectedBlue = expectedBlue, ActualRed = read == null ? (ushort)0 : read.Actual[0], ActualGreen = read == null ? (ushort)0 : read.Actual[1], ActualBlue = read == null ? (ushort)0 : read.Actual[2], Passed = pass, RxAvailable = read != null && read.Complete && (read.Status & 0x01) != 0, Detail = detail + VerificationReadState(read) });
        }

        private sealed class VerificationRead
        {
            internal bool Complete;
            internal byte Status;
            internal int SampleTimeMs;
            internal int ReadDurationMs;
            internal readonly ushort[] Expected = new ushort[3];
            internal readonly ushort[] Actual = new ushort[3];
        }
    }
}
