using System;
using System.Windows.Forms;

namespace LINMaster.Forms
{
    public partial class FormCjIdChange : Form
    {
        private readonly byte _currentId;

        public byte NewChairId
        {
            get { return (byte)nudNewChairId.Value; }
        }

        public FormCjIdChange(byte currentId)
        {
            InitializeComponent();
            _currentId = currentId;
            lblCurrentId.Text = string.Format("0x{0:X2}", currentId);
            nudNewChairId.Value = currentId;
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            if (NewChairId == _currentId)
            {
                MessageBox.Show(this, "현재 ID와 다른 ID를 선택하세요.", "CJ ID 변경", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            DialogResult = DialogResult.OK;
            Close();
        }
    }
}
