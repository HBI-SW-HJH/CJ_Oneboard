using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using LINMaster.LIN;

namespace LINMaster.Forms
{
    internal static class VerificationReport
    {
        [DllImport("user32.dll")]
        private static extern uint GetWindowThreadProcessId(IntPtr window, out uint processId);

        internal sealed class Row
        {
            public string Zone;
            public int Led;
            public string Test;
            public string Color;
            public int Brightness;
            public int Frequency;
            public int Duty;
            public ushort ExpectedRed;
            public ushort ExpectedGreen;
            public ushort ExpectedBlue;
            public ushort ActualRed;
            public ushort ActualGreen;
            public ushort ActualBlue;
            public bool Passed;
            public bool RxAvailable;
            public string Detail;
            public bool TimingReceived;
            public int ExpectedPeriodMs;
            public int ExpectedOnMs;
            public int ExpectedOffMs;
            public int MinPeriodMs;
            public int MaxPeriodMs;
            public int MinOnMs;
            public int MaxOnMs;
            public int MinOffMs;
            public int MaxOffMs;
            public bool TimingPassed;
            public bool HardwarePassed;
        }

        internal static string Save(UDSProtocol.ProductInfo product, DateTime started, IList<Row> rows, IList<Row> detailRows, bool incomplete, string outputBase = null)
        {
            string baseDirectory = outputBase ?? AppDomain.CurrentDomain.BaseDirectory;
            string template = Path.Combine(baseDirectory, "CJ_검사성적서.xlsx");
            if (!File.Exists(template)) throw new FileNotFoundException("성적서 양식이 실행 파일 옆에 없습니다.", template);
            string directory = Path.Combine(baseDirectory, "Reports");
            Directory.CreateDirectory(directory);
            string name = string.Join("_", new[] { Safe(product.SerialNumber), Safe(product.PartNumber), Safe(product.VersionText), string.Format("{0:X2}", product.ChairNumber), string.Format("{0:X8}", product.InitNumber), started.ToString("yyyyMMdd_HHmm") });
            string output = Path.Combine(directory, name + ".xlsx");
            if (File.Exists(output)) throw new IOException("같은 시각(분)의 성적서가 이미 있습니다. 기존 파일은 덮어쓰지 않습니다.");
            string working = Path.Combine(directory, "~" + Guid.NewGuid().ToString("N") + ".xlsx");
            File.Copy(template, working);
            dynamic excel = null;
            dynamic books = null;
            dynamic book = null;
            bool bookClosed = false;
            int excelProcessId = 0;
            bool ownExcelProcess = false;
            HashSet<int> existingExcelProcesses = new HashSet<int>(Process.GetProcessesByName("EXCEL").Select(process => process.Id));
            try
            {
                Type excelType = Type.GetTypeFromProgID("Excel.Application");
                if (excelType == null) throw new InvalidOperationException("성적서 작성에 Microsoft Excel 설치가 필요합니다.");
                excel = Activator.CreateInstance(excelType);
                excel.Visible = false;
                excel.DisplayAlerts = false;
                excel.ScreenUpdating = false;
                uint processId;
                GetWindowThreadProcessId(new IntPtr(Convert.ToInt64(excel.Hwnd)), out processId);
                excelProcessId = (int)processId;
                ownExcelProcess = !existingExcelProcesses.Contains(excelProcessId);
                books = excel.Workbooks;
                book = books.Open(working);
                WriteSummary(book, product, started, rows, incomplete);
                WriteDetail(book, rows, detailRows);
                WriteFlicker(book, rows);
                book.Worksheets[1].Activate();
                book.SaveAs(output, 51);
                book.Close(false);
                bookClosed = true;
                return output;
            }
            finally
            {
                if (!bookClosed && (object)book != null)
                {
                    try { book.Close(false); } catch { }
                }
                if ((object)excel != null)
                {
                    try { excel.Quit(); } catch { }
                }
                book = null;
                books = null;
                excel = null;
                if (File.Exists(working)) File.Delete(working);
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
                GC.WaitForPendingFinalizers();
                if (ownExcelProcess && excelProcessId != 0)
                {
                    try
                    {
                        using (Process process = Process.GetProcessById(excelProcessId))
                        {
                            if (!process.WaitForExit(bookClosed ? 100 : 2000)) process.Kill();
                        }
                    }
                    catch
                    {
                    }
                }
            }
        }

        private static void WriteSummary(dynamic book, UDSProtocol.ProductInfo product, DateTime started, IList<Row> rows, bool incomplete)
        {
            dynamic sheet = book.Worksheets[1];
            try
            {
                sheet.Cells[5, 17].Value2 = started.ToString("yyyy-MM-dd HH:mm");
                sheet.Cells[6, 4].Value2 = product.PartNumber;
                sheet.Cells[6, 13].Value2 = string.Format("Chair 0x{0:X2}  /  Init {1:X8}", product.ChairNumber, product.InitNumber);
                sheet.Cells[7, 4].Value2 = product.VersionText;
                sheet.Cells[8, 4].Value2 = product.SerialNumber;
                sheet.Cells[9, 4].Value2 = incomplete ? "6구역 LED 검사 미완료" : "6구역 LED 제어 및 진단 확인";
                string[] zones = { "FRT LH", "FRT RH", "Headrest", "Back", "Logo", "Strobe" };
                string[] tests = { "구역 On/Off", "R", "G", "B", "W", "Fade In/Out", "Flicker" };
                string[] criteria = { "선택 구역 ON/OFF, 다른 구역 OFF", "10/50/100% Write/Read, 오차 1%", "10/50/100% Write/Read, 오차 1%", "10/50/100% Write/Read, 오차 1%", "10/50/100% Write/Read, 오차 1%", "LED 1 중간 10점 + 전체 LED 최종 RX, 1%", "10/50Hz × 10/90%, TX 시간 ±1ms + 구역/RGBW 정상" };
                int[] counts = { 1, 3, 3, 3, 3, 2, 4 };
                object[,] criteriaValues = new object[zones.Length * tests.Length, 1];
                object[,] countValues = new object[zones.Length * tests.Length, 1];
                object[,] resultValues = new object[zones.Length * tests.Length, 1];
                for (int index = 0; index < zones.Length; index++)
                {
                    for (int section = 0; section < tests.Length; section++)
                    {
                        int row = index * tests.Length + section;
                        List<Row> own = rows.Where(item => item.Zone == zones[index]).ToList();
                        if (section == 0) own = own.Where(item => item.Test == "구역 On/Off").ToList();
                        else if (section < 5) own = own.Where(item => item.Test == "RGBW" && item.Color == tests[section]).ToList();
                        else if (section == 5) own = own.Where(item => item.Test == "Fade In" || item.Test == "Fade Out").ToList();
                        else own = own.Where(item => item.Test == "Flicker").ToList();
                        criteriaValues[row, 0] = criteria[section];
                        countValues[row, 0] = string.Format("{0}/{1}", own.Count(item => item.Passed), counts[section]);
                        resultValues[row, 0] = own.Count == counts[section] ? (own.All(item => item.Passed) ? "O" : "X") : (own.Count == 0 ? "-" : "X");
                    }
                }
                sheet.Range["I12:I53"].Value2 = criteriaValues;
                sheet.Range["S12:S53"].Value2 = countValues;
                sheet.Range["U12:U53"].Value2 = resultValues;
                sheet.Range["U12:U53"].Font.Color = 0;
                for (int row = 0; row < resultValues.GetLength(0); row++)
                {
                    if ((string)resultValues[row, 0] == "X") sheet.Cells[row + 12, 21].Font.Color = 255;
                }
                List<Row> failedRows = rows.Where(item => !item.Passed).Take(5).ToList();
                object[,] failures = new object[5, 1];
                for (int i = 0; i < failedRows.Count; i++) failures[i, 0] = string.Format("{0}: {1} {2}% {3}Hz {4}% - {5}", failedRows[i].Zone, failedRows[i].Test, failedRows[i].Brightness, failedRows[i].Frequency, failedRows[i].Duty, failedRows[i].Detail);
                sheet.Range["D54:D58"].Value2 = failures;
            }
            finally
            {
                sheet = null;
            }
        }

        private static void WriteDetail(dynamic book, IList<Row> rows, IList<Row> detailRows)
        {
            dynamic sheet = book.Worksheets["측정 상세"];
            try
            {
                sheet.Cells[2, 1].Value2 = "색상/Fade는 RGB 값, Flicker는 주기·ON·OFF 시간(ms)을 비교합니다. MCU 측정 시간은 최소~최대입니다.";
                sheet.Range["I4:N4"].Value2 = new object[,] { { "송신 R\n목표 주기(ms)", "송신 G\n목표 ON(ms)", "송신 B\n목표 OFF(ms)", "읽기 R\nMCU 주기(ms)", "읽기 G\nMCU ON(ms)", "읽기 B\nMCU OFF(ms)" } };
                sheet.Range["I4:N4"].WrapText = true;
                sheet.Range["I4:N4"].RowHeight = 32;
                string[] zones = { "FRT LH", "FRT RH", "Headrest", "Back", "Logo", "Strobe" };
                string[] colors = { "R", "G", "B", "W" };
                int[] levels = { 10, 50, 100 };
                int[] frequencies = { 10, 50 };
                int[] duties = { 10, 90 };
                List<Row> ordered = new List<Row>();
                if (detailRows != null && detailRows.Count != 0)
                {
                    string[] tests = { "구역 ON", "구역 OFF", "RGBW", "Fade In Step", "Fade In", "Fade Out Step", "Fade Out", "Flicker" };
                    ordered.AddRange(detailRows);
                    ordered.AddRange(rows.Where(item => item.Test == "Flicker"));
                    ordered = ordered.OrderBy(item => Array.IndexOf(zones, item.Zone)).ThenBy(item => item.Led).ThenBy(item => Array.IndexOf(tests, item.Test)).ThenBy(item => Array.IndexOf(colors, item.Color)).ThenBy(item => item.Brightness).ThenBy(item => item.Frequency).ThenBy(item => item.Duty).ToList();
                }
                else
                {
                    foreach (string zone in zones)
                    {
                        ordered.Add(FindRow(rows, zone, "구역 On/Off", "W", 50, 0, 0));
                        foreach (string color in colors)
                        {
                            foreach (int level in levels) ordered.Add(FindRow(rows, zone, "RGBW", color, level, 0, 0));
                        }
                        ordered.Add(FindRow(rows, zone, "Fade In", "CIE1931", 100, 0, 0));
                        ordered.Add(FindRow(rows, zone, "Fade Out", "W", 0, 0, 0));
                        foreach (int hz in frequencies)
                        {
                            foreach (int duty in duties) ordered.Add(FindRow(rows, zone, "Flicker", "W", 50, hz, duty));
                        }
                    }
                }
                sheet.Range["A5:P1000"].ClearContents();
                object[,] data = new object[ordered.Count, 16];
                for (int row = 0; row < ordered.Count; row++)
                {
                    Row item = ordered[row];
                    data[row, 0] = row + 1;
                    data[row, 1] = item.Zone;
                    data[row, 2] = item.Test == "Flicker" ? (object)"구역 전체" : item.Led;
                    data[row, 3] = item.Test;
                    data[row, 4] = item.Color;
                    data[row, 5] = item.Brightness;
                    data[row, 6] = item.Frequency == 0 ? null : (object)item.Frequency;
                    data[row, 7] = item.Duty == 0 ? null : (object)item.Duty;
                    if (item.Test != "Flicker")
                    {
                        data[row, 8] = item.ExpectedRed;
                        data[row, 9] = item.ExpectedGreen;
                        data[row, 10] = item.ExpectedBlue;
                        data[row, 11] = item.RxAvailable ? (object)item.ActualRed : "미수신";
                        data[row, 12] = item.RxAvailable ? (object)item.ActualGreen : "미수신";
                        data[row, 13] = item.RxAvailable ? (object)item.ActualBlue : "미수신";
                    }
                    else
                    {
                        data[row, 8] = item.Detail == "미검사" ? (object)"미검사" : item.ExpectedPeriodMs;
                        data[row, 9] = item.Detail == "미검사" ? (object)"미검사" : item.ExpectedOnMs;
                        data[row, 10] = item.Detail == "미검사" ? (object)"미검사" : item.ExpectedOffMs;
                        if (item.TimingReceived)
                        {
                            data[row, 11] = string.Format("{0}~{1}", item.MinPeriodMs, item.MaxPeriodMs);
                            data[row, 12] = string.Format("{0}~{1}", item.MinOnMs, item.MaxOnMs);
                            data[row, 13] = string.Format("{0}~{1}", item.MinOffMs, item.MaxOffMs);
                        }
                        else
                        {
                            data[row, 11] = item.Detail == "미검사" ? "미검사" : "미수신";
                            data[row, 12] = data[row, 11];
                            data[row, 13] = data[row, 11];
                        }
                    }
                    data[row, 14] = item.Detail == "미검사" ? "미검사" : item.Passed ? "Pass" : "Fail";
                    data[row, 15] = item.Detail;
                }
                sheet.Range["A5:P" + (ordered.Count + 4)].Value2 = data;
                sheet.Range["O5:O" + (ordered.Count + 4)].FormatConditions.Add(1, 3, "=\"Fail\"").Font.Color = 255;
            }
            finally
            {
                sheet = null;
            }
        }

        private static void WriteFlicker(dynamic book, IList<Row> rows)
        {
            dynamic sheet = null;
            try
            {
                for (int index = 1; index <= book.Worksheets.Count; index++)
                {
                    if (book.Worksheets[index].Name == "플리커 측정") sheet = book.Worksheets[index];
                }
                if ((object)sheet == null)
                {
                    sheet = book.Worksheets.Add(After: book.Worksheets[book.Worksheets.Count]);
                    sheet.Name = "플리커 측정";
                }
                sheet.Cells.ClearContents();
                sheet.Range["A1:R1"].Merge();
                sheet.Cells[1, 1].Value2 = "플리커 검사 — 목표 시간과 MCU TX 시점 측정값 비교";
                sheet.Range["A2:R2"].Merge();
                sheet.Cells[2, 1].Value2 = "주기·ON·OFF 최소/최대 모두 목표 ±1ms 이내 + 해당 구역 On/Off·RGBW 전 항목 정상일 때 Pass. RGB 0으로 판정하지 않음.";
                sheet.Range["A3:R3"].Merge();
                sheet.Cells[3, 1].Value2 = "측정 범위: MCU가 LED에 보낸 ON/OFF 전환 시점. LED 빛 자체나 RX 시간 파형을 측정한 결과는 아님. 값이 없으면 Fail.";
                string[] headers = { "구역", "설정 Hz", "설정 Duty %", "목표 주기 ms", "최소 주기 ms", "최대 주기 ms", "목표 ON ms", "최소 ON ms", "최대 ON ms", "목표 OFF ms", "최소 OFF ms", "최대 OFF ms", "허용 ms", "시간 판정", "구역/RGBW", "최종 판정", "응답", "측정 기준" };
                object[,] heading = new object[1, headers.Length];
                for (int column = 0; column < headers.Length; column++) heading[0, column] = headers[column];
                sheet.Range["A5:R5"].Value2 = heading;
                List<Row> flicker = rows.Where(item => item.Test == "Flicker").ToList();
                object[,] data = new object[flicker.Count, headers.Length];
                for (int index = 0; index < flicker.Count; index++)
                {
                    Row item = flicker[index];
                    data[index, 0] = item.Zone;
                    data[index, 1] = item.Frequency;
                    data[index, 2] = item.Duty;
                    data[index, 3] = item.ExpectedPeriodMs;
                    data[index, 6] = item.ExpectedOnMs;
                    data[index, 9] = item.ExpectedOffMs;
                    if (item.TimingReceived)
                    {
                        data[index, 4] = item.MinPeriodMs;
                        data[index, 5] = item.MaxPeriodMs;
                        data[index, 7] = item.MinOnMs;
                        data[index, 8] = item.MaxOnMs;
                        data[index, 10] = item.MinOffMs;
                        data[index, 11] = item.MaxOffMs;
                    }
                    data[index, 12] = "±1";
                    data[index, 13] = item.TimingPassed ? "Pass" : "Fail";
                    data[index, 14] = item.HardwarePassed ? "Pass" : "Fail";
                    data[index, 15] = item.Passed ? "Pass" : "Fail";
                    data[index, 16] = item.TimingReceived ? "수신" : "없음";
                    data[index, 17] = "MCU TX";
                }
                if (flicker.Count != 0) sheet.Range["A6:R" + (flicker.Count + 5)].Value2 = data;
                sheet.Range["A1:R" + (flicker.Count + 5)].Font.Name = "맑은 고딕";
                sheet.Range["A1:R" + (flicker.Count + 5)].Font.Size = 10;
                sheet.Range["A1:R" + (flicker.Count + 5)].Font.Color = 0;
                sheet.Range["A1:R1"].Font.Size = 16;
                sheet.Range["A1:R1"].Font.Bold = true;
                sheet.Range["A5:R5"].Font.Bold = true;
                sheet.Range["A5:R5"].Interior.Color = 15790320;
                sheet.Range["A5:R5"].WrapText = true;
                sheet.Range["A5:R5"].RowHeight = 32;
                sheet.Range["A:R"].ColumnWidth = 11;
                sheet.Range["A:A"].ColumnWidth = 13;
                sheet.Range["A6:R" + Math.Max(6, flicker.Count + 5)].RowHeight = 20;
                if (flicker.Count != 0)
                {
                    sheet.Range["N6:P" + (flicker.Count + 5)].FormatConditions.Add(1, 3, "=\"Fail\"").Font.Color = 255;
                }
                book.Application.PrintCommunication = false;
                try
                {
                    sheet.PageSetup.Orientation = 2;
                    sheet.PageSetup.Zoom = false;
                    sheet.PageSetup.FitToPagesWide = 1;
                    sheet.PageSetup.FitToPagesTall = 1;
                    sheet.PageSetup.PrintArea = "A1:R" + (flicker.Count + 5);
                }
                finally
                {
                    book.Application.PrintCommunication = true;
                }
            }
            finally
            {
                sheet = null;
            }
        }

        private static Row FindRow(IList<Row> rows, string zone, string test, string color, int level, int hz, int duty)
        {
            Row found = rows.FirstOrDefault(item => item.Zone == zone && item.Test == test && item.Color == color && item.Brightness == level && item.Frequency == hz && item.Duty == duty);
            if (found != null) return found;
            return new Row { Zone = zone, Led = 1, Test = test, Color = color, Brightness = level, Frequency = hz, Duty = duty, Passed = false, Detail = "미검사" };
        }

        private static string Safe(string value)
        {
            foreach (char character in Path.GetInvalidFileNameChars()) value = value.Replace(character, '-');
            return value.Trim();
        }
    }
}
