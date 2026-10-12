using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using LINMaster.Forms;
using LINMaster.LIN;

internal static class Entry
{
    [STAThread]
    private static int Main(string[] args)
    {
        AppDomain.CurrentDomain.AssemblyResolve += (s, e) =>
        {
            string name = new AssemblyName(e.Name).Name;
            string file = Path.Combine(args[0], name + (name == "LINMaster" ? ".exe" : ".dll"));
            return File.Exists(file) ? Assembly.LoadFrom(file) : null;
        };
        try
        {
            Tests.Run(args[1], args[2]);
            return 0;
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
            return 1;
        }
    }
}

internal static class Tests
{
    internal const BindingFlags Members = BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance;
    internal static object Field(object obj, string name) { return obj.GetType().GetField(name, Members).GetValue(obj); }
    internal static void Set(object obj, string name, object value) { obj.GetType().GetField(name, Members).SetValue(obj, value); }
    internal static object Call(object obj, string name, params object[] args) { return obj.GetType().GetMethod(name, Members).Invoke(obj, args); }
    internal static void Check(bool ok, string label) { if (!ok) throw new Exception(label); Console.WriteLine("PASS: " + label); }

    internal static void Run(string directory, string fixtureDirectory)
    {
        byte[] image = new byte[256];
        BitConverter.GetBytes(0x20010000U).CopyTo(image, 0);
        BitConverter.GetBytes(0x000100C1U).CopyTo(image, 4);
        string imageFile = Path.Combine(directory, "SIMULATED_ONLY.bin");
        File.WriteAllBytes(imageFile, image);
        Type protocol = typeof(FormUDS).Assembly.GetType("LINMaster.LIN.UDSProtocol");
        Type productType = protocol.GetNestedType("ProductInfo", BindingFlags.NonPublic);
        foreach (string name in new[] { "AppIdentityWire.bin", "BootIdentityWire.bin" })
        {
            byte[] wire = File.ReadAllBytes(Path.Combine(fixtureDirectory, name));
            object[] parse = { wire, (ushort)0xF301, null };
            Check((bool)protocol.GetMethod("TryParseTargetRead", BindingFlags.NonPublic | BindingFlags.Static).Invoke(null, parse), name + " actual firmware response parsed");
            object info = parse[2];
            uint expectedInit = name == "AppIdentityWire.bin" ? 0x82DA5C68U : 0x12345678U;
            Check((string)Field(info, "PartNumber") == "ZX1-0025" && (byte)Field(info, "ChairNumber") == 0x0C && (uint)Field(info, "InitNumber") == expectedInit, name + " App/legacy Boot fields match");
            byte[] data = (byte[])Call(info, "ToData");
            Check(data.Length == 29 && System.Linq.Enumerable.SequenceEqual(data, System.Linq.Enumerable.Skip(wire, 4)), name + " PC round trip is byte-exact");
            byte[] read = new byte[32]; read[0] = 0x62; read[1] = 0xF2; read[2] = 2;
            data.CopyTo(read, 3);
            Check(productType.GetMethod("FromResponse", BindingFlags.NonPublic | BindingFlags.Static).Invoke(null, new object[] { read }) != null, "F202 read parsed");
            Set(info, "PartNumber", "CJ01-0001");
            bool rejected = false;
            try { Call(info, "ToData"); } catch (TargetInvocationException ex) { rejected = ex.InnerException is ArgumentException; }
            Check(rejected, "Nine-character write rejected");
            object[] legacy = { new byte[34], (ushort)0xF301, null };
            Check(!(bool)protocol.GetMethod("TryParseTargetRead", BindingFlags.NonPublic | BindingFlags.Static).Invoke(null, legacy), "Old Scan response length rejected");
        }
        for (int mode = 0; mode < 2; mode++)
        {
            using (var gateway = new FakeGateway(1, 2, 0, mode != 0))
            using (var form = new FormUDS(gateway))
            {
                Call(form, "btnScan_Click", null, EventArgs.Empty);
                IList nodes = (IList)Field(form, "_nodes");
                Check(nodes.Count == 4, "Actual Scan finds all four " + (mode == 0 ? "App" : "Boot") + " nodes");
                for (int i = 0; i < 4; i++)
                {
                    object product = Field(nodes[i], "Product");
                    Check((byte)Field(product, "ChairNumber") == 0x0A + i && (uint)Field(product, "InitNumber") == 0xCCF026D4U + (uint)i && (string)Field(product, "PartNumber") == "ZX1-0025", "Scan preserves independent Chair/Init/Part");
                    Check((int)Field(nodes[i], "ExecutionMode") == mode, "Board-status read after Scan");
                }
                ((ListBox)Field(form, "lstNodes")).SelectedIndex = 0;
                Check(((TextBox)Field(form, "txtPart")).MaxLength == 8, "Part textbox length eight");
                Check(((PictureBox)Field(form, "picProductQr")).Image != null, "Scan QR generated");
                if (mode == 0)
                {
                    ((TextBox)Field(form, "txtPart")).Text = "ZX1-0026";
                    ((TextBox)Field(form, "txtSerial")).Text = "26J02-0042";
                    ((NumericUpDown)Field(form, "nudChairNumber")).Value = 13;
                    Call(form, "btnWriteProduct_Click", null, EventArgs.Empty);
                    string status = ((Label)Field(form, "lblProductStatus")).Text;
                    Check(status.Contains("재확인 완료"), "Actual PC write and targeted readback: " + status);
                    object saved = Field(nodes[0], "Product");
                    Check((string)Field(saved, "PartNumber") == "ZX1-0026" && (byte)Field(saved, "ChairNumber") == 13 && (uint)Field(saved, "InitNumber") == 0xCCF026D4U, "Write updates cache and preserves target");
                }
            }
        }
        for (int test = 0; test < 8; test++)
        {
            ushort before = test == 2 ? (ushort)2 : (ushort)1;
            ushort after = test == 0 ? (ushort)1 : test == 2 ? (ushort)1 : test == 3 ? (ushort)0x1234 : (ushort)2;
            using (var gateway = new FakeGateway(before, after, test))
            using (var form = new FormUDS(gateway))
            {
                object product = productType.GetMethod("FromData", BindingFlags.NonPublic | BindingFlags.Static).Invoke(null, new object[] { gateway.Product(false), 0 });
                object node = Activator.CreateInstance(typeof(FormUDS).GetNestedType("UdsNode", BindingFlags.NonPublic), true);
                Set(node, "Product", product);
                Set(node, "InitNumber", 0xCCF026D4U);
                Set(node, "ChairId", (byte)1);
                Set(node, "Nad", (byte)1);
                ((IList)Field(form, "_nodes")).Add(node);
                Set(form, "_scanning", true);
                ((ListBox)Field(form, "lstNodes")).Items.Add("SIMULATED BOARD");
                ((ListBox)Field(form, "lstNodes")).SelectedIndex = 0;
                Set(form, "_scanning", false);
                bool success = (bool)Call(form, "UpdateFirmware", imageFile);
                string label = new[] { "same version", "0001 -> 0002", "0002 -> 0001 rollback", "full 16-bit version 1234", "serial mismatch", "part mismatch", "chair mismatch", "Init mismatch" }[test];
                string status = ((Label)Field(form, "lblProductStatus")).Text;
                Check(success == (test < 4), label + " outcome: " + status);
                if (test < 4)
                {
                    Check((ushort)Field(Field(node, "Product"), "Version") == after, label + " cached version refreshed");
                    Check(((TextBox)Field(form, "txtVersion")).Text == string.Format("{0}.{1}.{2}.{3}", after >> 12, (after >> 8) & 15, (after >> 4) & 15, after & 15), label + " displayed version refreshed");
                    Check(((TextBox)Field(form, "txtVersion")).ReadOnly, "version remains read only");
                    Check(((PictureBox)Field(form, "picProductQr")).Image != null, "QR generated");
                }
                else
                {
                    Check(status.Contains("제품 PDS 정보 확인 실패"), label + " rejected specifically by identity check");
                    Check((ushort)Field(Field(node, "Product"), "Version") == before, label + " did not replace cache");
                }
                File.WriteAllText(Path.Combine(directory, "sim_" + test + ".txt"), ((StringBuilder)Field(form, "_log")).ToString(), Encoding.UTF8);
            }
        }
    }
}

internal sealed class FakeGateway : ILINGateway
{
    private readonly ushort before;
    private readonly ushort after;
    private readonly int test;
    private bool boot;
    private bool complete;
    private byte[] writtenProduct;
    private readonly Dictionary<byte, object> decoders = new Dictionary<byte, object>();
    private readonly Queue<byte[]> responses = new Queue<byte[]>();
    private readonly Type protocol = typeof(FormUDS).Assembly.GetType("LINMaster.LIN.UDSProtocol");
    public FakeGateway(ushort initial, ushort updated, int scenario, bool initiallyBoot = false) { before = initial; after = updated; test = scenario; boot = initiallyBoot; }
    public bool IsConnected { get { return true; } }
    public string PortName { get { return "SIMULATED; NO HARDWARE"; } }
    public event Action<LINFrame> FrameReceived;
    public event Action<string> StatusMessage { add { } remove { } }
    public event Action<string> ErrorMessage { add { } remove { } }
    public event Action<byte[], string> RawPacket { add { } remove { } }
    public bool Connect(string name) { return true; }
    public void Disconnect() { }
    public void Dispose() { }
    public void Poll() { }

    internal byte[] Product(bool final)
    {
        if (writtenProduct != null) return (byte[])writtenProduct.Clone();
        byte[] data = new byte[29];
        Encoding.ASCII.GetBytes(final && test == 4 ? "26I16-0002" : "26I16-0001").CopyTo(data, 0);
        Encoding.ASCII.GetBytes(final && test == 5 ? "ZX1-0026" : "ZX1-0025").CopyTo(data, 11);
        data[10] = data[19] = data[22] = data[24] = (byte)'*';
        ushort version = final ? after : before;
        data[20] = (byte)(version >> 8); data[21] = (byte)version;
        data[23] = final && test == 6 ? (byte)11 : (byte)10;
        data[25] = 0xCC; data[26] = 0xF0; data[27] = 0x26; data[28] = final && test == 7 ? (byte)0xD5 : (byte)0xD4;
        return data;
    }

    public bool SendPublisher(LINFrame frame)
    {
        byte nad = frame.Data[0];
        if (!decoders.ContainsKey(nad))
        {
            decoders[nad] = Activator.CreateInstance(protocol.GetNestedType("Decoder", BindingFlags.NonPublic), Tests.Members, null, new object[] { nad }, null);
        }
        var input = new LINFrame { FrameID = 0x3D, DataLength = 8, Direction = LINDirection.RX };
        frame.Data.CopyTo(input.Data, 0);
        object[] feed = { input, null };
        bool ready = (bool)Tests.Call(decoders[nad], "Feed", feed);
        if (!ready) return true;
        byte[] request = (byte[])feed[1];
        byte[] response;
        switch (request[0])
        {
            case 0x2E:
                Tests.Check(request.Length == 32 && request[1] == 0xF2 && request[2] == 5, "Write request is 32 bytes");
                Tests.Check(request[28] == 0xCC && request[29] == 0xF0 && request[30] == 0x26 && request[31] == 0xD4, "Write target at new offset");
                writtenProduct = new byte[29]; Array.Copy(request, 3, writtenProduct, 0, 29);
                response = new byte[] { 0x6E, 0xF2, 5 };
                break;
            case 0x31:
                if (request[3] == 0)
                {
                    if (request[4] >= 4) return true;
                    response = new byte[33];
                    response[0] = 0x71; response[1] = 3; response[2] = 0xF3;
                    Product(false).CopyTo(response, 4);
                    response[27] = (byte)(0x0A + request[4]);
                    response[32] = (byte)(0xD4 + request[4]);
                }
                else if (request[3] == 1)
                {
                    response = new byte[33];
                    response[0] = 0x71; response[1] = 3; response[2] = 0xF3; response[3] = 1;
                    Product(complete).CopyTo(response, 4);
                }
                else
                {
                    response = new byte[9];
                    request.CopyTo(response, 0);
                    response[0] = 0x71; response[8] = boot ? (byte)1 : (byte)0;
                }
                break;
            case 0x22:
                response = new byte[38];
                response[0] = 0x62; response[1] = 0xF2; response[2] = 0x10;
                response[3] = boot ? (byte)1 : (byte)0; response[5] = 1; response[9] = 3;
                response[28] = 3; response[37] = 7;
                break;
            case 0x10:
                boot = true;
                response = new byte[] { 0x50, 2, 0, 250, 1, 244 };
                break;
            case 0x34: response = new byte[] { 0x74, 0x20, 0, 130 }; break;
            case 0x36: response = new byte[] { 0x76, request[1] }; break;
            case 0x37: response = new byte[] { 0x77 }; break;
            case 0x11:
                boot = false; complete = true;
                response = new byte[] { 0x51, 1 };
                break;
            default: throw new Exception("Unexpected simulated SID " + request[0]);
        }
        var encoded = (IEnumerable)protocol.GetMethod("Encode", BindingFlags.NonPublic | BindingFlags.Static).Invoke(null, new object[] { nad, response });
        foreach (byte[] data in encoded) responses.Enqueue(data);
        return true;
    }

    public bool SendSubscriberRequest(byte id, byte length, LINChecksumType checksum = LINChecksumType.Enhanced)
    {
        if (responses.Count != 0)
        {
            var frame = new LINFrame { FrameID = id, DataLength = 8, Direction = LINDirection.RX };
            responses.Dequeue().CopyTo(frame.Data, 0);
            FrameReceived(frame);
        }
        return true;
    }
}
