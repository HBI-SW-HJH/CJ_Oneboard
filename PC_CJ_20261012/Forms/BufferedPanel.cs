using System.Windows.Forms;

namespace LINMaster.Forms
{
    // 깜빡임 없는 더블버퍼 패널
    internal sealed class BufferedPanel : Panel
    {
        public BufferedPanel()
        {
            SetStyle(
                ControlStyles.OptimizedDoubleBuffer |
                ControlStyles.UserPaint,
                true);
            UpdateStyles();
        }
    }
}
