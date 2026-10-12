// Runs the production FormUDS against an in-memory four-board LIN bus. No hardware.
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using LINMaster.Forms;
using LINMaster.LIN;

internal static class BootIndependentHost
{
    internal const BindingFlags Members = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance;
    internal static object Get(object target, string field) { return target.GetType().GetField(field, Members).GetValue(target); }
    internal static void Set(object target, string field, object value) { target.GetType().GetField(field, Members).SetValue(target, value); }
    internal static object Call(object target, string method, params object[] args) { return target.GetType().GetMethod(method, Members).Invoke(target, args); }
    internal static void Check(bool value, string message) { if (!value) throw new Exception(message); }

    [STAThread]
    private static int Main(string[] args)
    {
        AppDomain.CurrentDomain.AssemblyResolve += (sender, e) =>
        {
            string name = new AssemblyName(e.Name).Name;
            return Assembly.LoadFrom(Path.Combine(args[0], name + (name == "LINMaster" ? ".exe" : ".dll")));
        };
        try { Run(args[1]); return 0; }
        catch (Exception ex) { Console.WriteLine(ex); return 1; }
    }

    private static void Run(string output)
    {
        Directory.CreateDirectory(output);
        byte[] image = new byte[257];
        BitConverter.GetBytes(0x2000FFF8U).CopyTo(image, 0);
        BitConverter.GetBytes(0x000100C1U).CopyTo(image, 4);
        for (int i = 8; i < image.Length; i++) image[i] = (byte)i;
        string filename = Path.Combine(output, "SIMULATED_ONLY.bin");
        File.WriteAllBytes(filename, image);
        for (int scenario = 0; scenario < 4; scenario++)
        {
            using (var bus = new IndependentBus(scenario == 1))
            using (var form = new FormUDS(bus))
            {
                if (scenario >= 2) bus.Boot[0] = true;
                Call(form, "btnScan_Click", null, EventArgs.Empty);
                IList nodes = (IList)Get(form, "_nodes");
                Check(nodes.Count == 4, "Scan must find four App/Boot nodes");
                for (int target = 0; target < (scenario < 2 ? 4 : 1); target++)
                {
                    int index = -1;
                    for (int i = 0; i < nodes.Count; i++) if ((uint)Get(nodes[i], "InitNumber") == IndependentBus.FirstKey + target) index = i;
                    Check(index >= 0, "CPU key not found");
                    ((ListBox)Get(form, "lstNodes")).SelectedIndex = index;
                    Check(((Button)Get(form, "btnUpdate")).Enabled, "Update must be enabled without Boot product information");
                    if (bus.Boot[target]) Check(Get(nodes[index], "Product") == null, "Boot must not invent product information");
                    bus.RejectCrc = scenario == 2;
                    bus.CancelBlock = scenario == 3 ? (Action)(() => Set(form, "_cancelUpdate", true)) : null;
                    bool success = (bool)Call(form, "UpdateFirmware", filename);
                    string status = ((Label)Get(form, "lblProductStatus")).Text;
                    Check(success == (scenario < 2), "Unexpected update outcome: " + status);
                    if (success)
                    {
                        Check(!bus.Boot[target] && bus.Updates[target] == 1, "Target must return to App");
                        Check((byte)Get(nodes[index], "Nad") == target + 1, "App NAD must be restored");
                        Check((ushort)Get(Get(nodes[index], "Product"), "Version") == 2, "New App product response must be displayed");
                    }
                    else
                    {
                        Check(bus.Boot[target] && bus.Updates[target] == 0, "Failure/cancel must leave Boot without reset");
                        bus.RejectCrc = false;
                        bus.CancelBlock = null;
                        Call(form, "btnScan_Click", null, EventArgs.Empty);
                        nodes = (IList)Get(form, "_nodes");
                        for (int i = 0; i < nodes.Count; i++) if ((uint)Get(nodes[i], "InitNumber") == IndependentBus.FirstKey) ((ListBox)Get(form, "lstNodes")).SelectedIndex = i;
                        Check((bool)Call(form, "UpdateFirmware", filename), "Retry from Boot-only Scan failed");
                    }
                    for (int i = target + 1; i < 4; i++) Check(bus.Updates[i] == 0, "Non-target board was updated");
                    Console.WriteLine("PASS scenario {0}, chair {1:X2}: App/Boot routing, CPU selection, transfer, peers, product readback", scenario, target + 10);
                }
                Check(bus.BootProductReads == 0, "PC must never request product PDS in independent Boot");
                Check(bus.BadAddressRequests == 0, "PC used an App NAD for Boot, or Boot NAD for App");
            }
        }
        Console.WriteLine("PASS: 8 normal updates + CRC rejection/retry + cancel/rescan/retry; no hardware accessed");
    }
}

internal sealed class IndependentBus : ILINGateway
{
    internal const uint FirstKey = 0x13572468;
    internal readonly bool[] Boot = new bool[4];
    internal readonly int[] Updates = new int[4];
    internal int BootProductReads, BadAddressRequests;
    internal bool RejectCrc;
    internal Action CancelBlock;
    internal Func<int, byte[], byte[]> StatusOverride = null;
    private int selected = -1;
    private bool session;
    private int expectedLength;
    private byte sequence;
    private readonly List<byte> received = new List<byte>();
    private readonly Queue<byte[]> replies = new Queue<byte[]>();
    private readonly Dictionary<byte, object> decoders = new Dictionary<byte, object>();
    private readonly Type protocol = typeof(FormUDS).Assembly.GetType("LINMaster.LIN.UDSProtocol");
    public IndependentBus(bool allBoot) { for (int i = 0; i < 4; i++) Boot[i] = allBoot; }
    public bool IsConnected { get { return true; } }
    public string PortName { get { return "SIMULATED ONLY"; } }
    public event Action<LINFrame> FrameReceived;
    public event Action<string> StatusMessage { add { } remove { } }
    public event Action<string> ErrorMessage { add { } remove { } }
    public event Action<byte[], string> RawPacket { add { } remove { } }
    public bool Connect(string name) { return true; }
    public void Disconnect() { }
    public void Dispose() { }
    public void Poll() { }
    private static uint Read32(byte[] data, int offset) { return ((uint)data[offset] << 24) | ((uint)data[offset + 1] << 16) | ((uint)data[offset + 2] << 8) | data[offset + 3]; }
    private static void Write32(byte[] data, int offset, uint value) { for (int i = 0; i < 4; i++) data[offset + i] = (byte)(value >> (24 - i * 8)); }
    private byte[] Product(int board)
    {
        byte[] data = new byte[33];
        data[0] = 0x71; data[1] = 3; data[2] = 0xF3;
        System.Text.Encoding.ASCII.GetBytes("26J07-000" + (board + 1)).CopyTo(data, 4);
        System.Text.Encoding.ASCII.GetBytes("ZX1-0025").CopyTo(data, 15);
        data[14] = data[23] = data[26] = data[28] = (byte)'*';
        data[25] = (byte)(Updates[board] + 1); data[27] = (byte)(board + 10);
        Write32(data, 29, FirstKey + (uint)board);
        return data;
    }
    private void Reply(byte nad, byte[] response)
    {
        var frames = (IEnumerable)protocol.GetMethod("Encode", BindingFlags.Static | BindingFlags.NonPublic).Invoke(null, new object[] { nad, response });
        foreach (byte[] frame in frames) replies.Enqueue(frame);
    }
    public bool SendPublisher(LINFrame frame)
    {
        byte nad = frame.Data[0];
        if (!decoders.ContainsKey(nad)) decoders[nad] = Activator.CreateInstance(protocol.GetNestedType("Decoder", BindingFlags.NonPublic), BootIndependentHost.Members, null, new object[] { nad }, null);
        var input = new LINFrame { FrameID = 0x3D, DataLength = 8, Direction = LINDirection.RX };
        frame.Data.CopyTo(input.Data, 0);
        object[] feed = { input, null };
        if (!(bool)BootIndependentHost.Call(decoders[nad], "Feed", feed)) return true;
        byte[] request = (byte[])feed[1];
        byte[] response;
        if (nad == 0x7F)
        {
            BootIndependentHost.Check(request[0] == 0x31, "Unexpected broadcast service");
            if (request[3] == 0)
            {
                int board = request[4];
                if (board >= 4) return true;
                if (Boot[board])
                {
                    response = new byte[] { 0x71, 3, 0xF3, 0, 0, 0, 0, 0, 1 };
                    Write32(response, 4, FirstKey + (uint)board);
                }
                else response = Product(board);
            }
            else
            {
                uint key = Read32(request, 4);
                if (key < FirstKey || key >= FirstKey + 4) { selected = -1; return true; }
                int board = (int)(key - FirstKey);
                if (request[3] == 1)
                {
                    if (Boot[board]) { BootProductReads++; throw new Exception("Boot product read is forbidden"); }
                    response = Product(board); response[3] = 1;
                }
                else
                {
                    selected = board; session = false;
                    response = new byte[Boot[board] ? 10 : 9];
                    request.CopyTo(response, 0); response[0] = 0x71; response[8] = Boot[board] ? (byte)1 : (byte)0;
                    if (Boot[board]) response[9] = 0x7D;
                }
            }
            Reply(nad, response);
            return true;
        }
        BootIndependentHost.Check(selected >= 0, "Update service before CPU selection");
        if (nad != (Boot[selected] ? 0x7D : selected + 1)) { BadAddressRequests++; return true; }
        switch (request[0])
        {
            case 0x22:
                BootIndependentHost.Check(request[1] == 0xF2 && request[2] == 0x10, "Only update status is allowed in Boot");
                response = new byte[38]; response[0] = 0x62; response[1] = 0xF2; response[2] = 0x10;
                response[3] = Boot[selected] ? (byte)1 : (byte)0;
                Write32(response, 4, 0x10000); Write32(response, 8, 0x30000); response[28] = 3; response[37] = 7;
                if (StatusOverride != null)
                {
                    response = StatusOverride(selected, response);
                }
                break;
            case 0x10:
                if (!Boot[selected]) { Boot[selected] = true; session = false; }
                else session = true;
                response = new byte[] { 0x50, 2, 0, 250, 1, 244 };
                break;
            case 0x34:
                BootIndependentHost.Check(Boot[selected] && session, "Download before Boot session");
                expectedLength = (int)Read32(request, 7); received.Clear(); sequence = 1;
                response = new byte[] { 0x74, 0x20, 0, 130 };
                break;
            case 0x36:
                BootIndependentHost.Check(Boot[selected] && session && request[1] == sequence++, "Transfer sequence mismatch");
                for (int i = 2; i < request.Length; i++) received.Add(request[i]);
                response = new byte[] { 0x76, request[1] };
                if (CancelBlock != null) CancelBlock();
                break;
            case 0x37:
                BootIndependentHost.Check(received.Count == expectedLength, "Image length mismatch");
                uint crc = 0xFFFFFFFF;
                foreach (byte value in received) { crc ^= value; for (int bit = 0; bit < 8; bit++) crc = (crc & 1) != 0 ? (crc >> 1) ^ 0xEDB88320 : crc >> 1; }
                BootIndependentHost.Check(Read32(request, 1) == (crc ^ 0xFFFFFFFF), "Image CRC mismatch");
                response = RejectCrc ? new byte[] { 0x7F, 0x37, 0x72 } : new byte[] { 0x77 };
                break;
            case 0x11:
                BootIndependentHost.Check(Boot[selected] && !RejectCrc, "Reset before completion");
                Boot[selected] = false; Updates[selected]++;
                response = new byte[] { 0x51, 1 };
                break;
            default: throw new Exception("Unexpected SID " + request[0]);
        }
        Reply(nad, response);
        return true;
    }
    public bool SendSubscriberRequest(byte id, byte length, LINChecksumType checksum = LINChecksumType.Enhanced)
    {
        if (replies.Count > 0)
        {
            var frame = new LINFrame { FrameID = id, DataLength = 8, Direction = LINDirection.RX };
            replies.Dequeue().CopyTo(frame.Data, 0);
            FrameReceived(frame);
        }
        return true;
    }
}
