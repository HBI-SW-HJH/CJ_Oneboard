using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using LINMaster.Forms;

internal static class UpdateCompletionHost
{
    [STAThread]
    private static int Main(string[] args)
    {
        AppDomain.CurrentDomain.AssemblyResolve += (sender, e) =>
        {
            string name = new AssemblyName(e.Name).Name;
            return Assembly.LoadFrom(Path.Combine(args[0], name + (name == "LINMaster" ? ".exe" : ".dll")));
        };
        try
        {
            Run(args[1]);
            return 0;
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
            return 1;
        }
    }

    private static void Put(byte[] data, int offset, uint value)
    {
        for (int i = 0; i < 4; i++)
        {
            data[offset + i] = (byte)(value >> (24 - i * 8));
        }
    }

    private static void Run(string output)
    {
        Directory.CreateDirectory(output);
        byte[] image = new byte[257];
        BitConverter.GetBytes(0x2000FFF8U).CopyTo(image, 0);
        BitConverter.GetBytes(0x000100C1U).CopyTo(image, 4);
        for (int i = 8; i < image.Length; i++)
        {
            image[i] = (byte)i;
        }
        string filename = Path.Combine(output, "SIMULATED_ONLY.bin");
        File.WriteAllBytes(filename, image);
        string[] scenarios = { "normal_delayed", "rollback_immediate", "rollback_late", "prior_recovery_history", "normal_after_prior_recovery", "saturated_restore_counter", "confirmation_timeout", "invalid_final_crc", "missing_final_extension", "final_hardfault", "final_pending_flag", "legacy_12", "legacy_29", "boot_only_delayed", "decreased_restore_counter", "invalid_pre_reset_crc", "final_boot_mode", "missing_recovery_format" };
        foreach (string scenario in scenarios)
        {
            bool successExpected = scenario == "normal_delayed" || scenario == "prior_recovery_history" || scenario == "normal_after_prior_recovery" || scenario == "legacy_12" || scenario == "legacy_29" || scenario == "boot_only_delayed";
            bool restoredExpected = scenario == "rollback_immediate" || scenario == "rollback_late" || scenario == "saturated_restore_counter";
            int finalReads = 0;
            using (var bus = new IndependentBus(false))
            using (var form = new FormUDS(bus))
            {
                bus.Boot[0] = scenario == "boot_only_delayed";
                bus.StatusOverride = (board, status) =>
                {
                    if (board != 0)
                    {
                        return status;
                    }
                    bool afterReset = bus.Updates[board] != 0;
                    uint count = scenario == "saturated_restore_counter" ? uint.MaxValue : 5U;
                    if (afterReset)
                    {
                        finalReads++;
                        if ((scenario == "normal_delayed" || scenario == "boot_only_delayed") && finalReads < 4)
                        {
                            status[37] = 3;
                        }
                        if (scenario == "rollback_immediate" || (scenario == "rollback_late" && finalReads >= 3))
                        {
                            count++;
                            status[37] = 15;
                        }
                        if (scenario == "rollback_late" && finalReads < 3)
                        {
                            status[37] = 3;
                        }
                        if (scenario == "saturated_restore_counter" || scenario == "prior_recovery_history")
                        {
                            status[37] = 15;
                        }
                        if (scenario == "confirmation_timeout")
                        {
                            status[37] = 3;
                        }
                        if (scenario == "invalid_final_crc")
                        {
                            status[28] = 1;
                        }
                        if (scenario == "final_hardfault")
                        {
                            Put(status, 20, 1);
                            Put(status, 24, 1);
                        }
                        if (scenario == "final_pending_flag")
                        {
                            Put(status, 12, 1);
                            Put(status, 16, 1);
                        }
                        if (scenario == "decreased_restore_counter")
                        {
                            count--;
                        }
                        if (scenario == "final_boot_mode")
                        {
                            status[3] = 1;
                        }
                        if (scenario == "missing_recovery_format")
                        {
                            status[37] = 6;
                        }
                    }
                    else
                    {
                        if (scenario == "prior_recovery_history" || (scenario == "normal_after_prior_recovery" && !bus.Boot[board]))
                        {
                            status[37] = 15;
                        }
                        if (scenario == "invalid_pre_reset_crc" && bus.Boot[board])
                        {
                            status[28] = 0;
                        }
                    }
                    Put(status, 29, count);
                    if (scenario == "legacy_12")
                    {
                        return status.Take(12).ToArray();
                    }
                    if (scenario == "legacy_29" || (scenario == "missing_final_extension" && afterReset))
                    {
                        return status.Take(29).ToArray();
                    }
                    return status;
                };
                BootIndependentHost.Call(form, "btnScan_Click", null, EventArgs.Empty);
                IList nodes = (IList)BootIndependentHost.Get(form, "_nodes");
                int target = -1;
                for (int i = 0; i < nodes.Count; i++)
                {
                    if ((uint)BootIndependentHost.Get(nodes[i], "InitNumber") == IndependentBus.FirstKey)
                    {
                        target = i;
                    }
                }
                BootIndependentHost.Check(target >= 0, "Test target not found");
                ((ListBox)BootIndependentHost.Get(form, "lstNodes")).SelectedIndex = target;
                Stopwatch elapsed = Stopwatch.StartNew();
                bool success = (bool)BootIndependentHost.Call(form, "UpdateFirmware", filename);
                string result = ((Label)BootIndependentHost.Get(form, "lblProductStatus")).Text;
                Color color = ((Label)BootIndependentHost.Get(form, "lblProductStatus")).ForeColor;
                string log = ((StringBuilder)BootIndependentHost.Get(form, "_updateLog")).ToString();
                File.WriteAllText(Path.Combine(output, scenario + ".txt"), result + Environment.NewLine + log, Encoding.UTF8);
                BootIndependentHost.Check(success == successExpected, scenario + " unexpected result: " + result);
                BootIndependentHost.Check(color == (successExpected ? Color.LimeGreen : Color.OrangeRed), scenario + " result color");
                if (!successExpected)
                {
                    BootIndependentHost.Check(log.Contains("UPDATE FAIL: " + result), scenario + " missing failure log");
                }
                if (restoredExpected)
                {
                    BootIndependentHost.Check(result.Contains("이전 App 복원됨") && !result.Contains("업데이트 완료"), scenario + " misleading success label");
                }
                if (scenario == "normal_delayed" || scenario == "boot_only_delayed")
                {
                    BootIndependentHost.Check(finalReads >= 4, "PC did not wait for App confirmation");
                }
                if (scenario == "confirmation_timeout")
                {
                    BootIndependentHost.Check(result.Contains("시간 초과") && elapsed.ElapsedMilliseconds >= 12000, "Confirmation timeout was not enforced");
                }
                if (scenario == "invalid_pre_reset_crc")
                {
                    BootIndependentHost.Check(bus.Boot[0] && bus.Updates[0] == 0, "Invalid metadata must not cause Reset");
                }
                BootIndependentHost.Check(bus.BadAddressRequests == 0 && bus.BootProductReads == 0, "App/Boot routing regression");
                BootIndependentHost.Check(bus.Updates.Skip(1).All(value => value == 0), "Non-target board updated");
                Console.WriteLine("PASS {0}: {1}", scenario, result);
            }
        }
        Console.WriteLine("PASS: 18 completion/rollback/legacy/error scenarios; no hardware accessed");
    }
}
