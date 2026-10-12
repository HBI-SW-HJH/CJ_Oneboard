using System;
using System.Windows.Forms;

namespace LINMaster
{
    static class Program
    {
        [STAThread]
        static void Main()
        {
            // Select the USB 10BASE-T1S adapter before the native SOME/IP
            // library is loaded.  The PC's normal Ethernet adapter uses the
            // same 192.168.0/24 subnet and otherwise wins route selection.
            Environment.SetEnvironmentVariable("LAN866X_LOCAL_IPV4",
                "192.168.0.10", EnvironmentVariableTarget.Process);

            // ── 전역 예외 핸들러 등록 ─────────────────────────────────────
            // PEAK DLL 호출 시 PLinMng.exe 서비스 미실행, 드라이버 오류 등으로
            // 발생하는 SEH/CLR 예외가 폼을 강제 종료하는 것을 방지.
            Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);

            Application.ThreadException += (s, e) =>
            {
                string msg = "[예외] " + e.Exception.GetType().Name
                           + "\n" + e.Exception.Message;
                MessageBox.Show(msg, "오류 발생",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            };

            AppDomain.CurrentDomain.UnhandledException += (s, e) =>
            {
                string msg = "[치명적 예외]\n" + e.ExceptionObject?.ToString();
                MessageBox.Show(msg, "치명적 오류",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            };
            // ─────────────────────────────────────────────────────────────

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Forms.FormMain());
        }
    }
}
