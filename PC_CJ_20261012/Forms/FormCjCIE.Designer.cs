namespace LINMaster.Forms
{
    partial class FormCjCIE
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null)) components.Dispose();
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.txTimer = new System.Windows.Forms.Timer(this.components);
            this.pnlCIE = new System.Windows.Forms.Panel();
            this.pnlCIEBar = new System.Windows.Forms.Panel();
            this.lblCursor = new System.Windows.Forms.Label();
            this.lblSelected = new System.Windows.Forms.Label();
            this.lblCieMode = new System.Windows.Forms.Label();
            this.cmbCieMode = new System.Windows.Forms.ComboBox();
            this.lblCieModeTitle = new System.Windows.Forms.Label();
            this.grpFrame = new System.Windows.Forms.GroupBox();
            this.cmbCS = new System.Windows.Forms.ComboBox();
            this.lblChecksum = new System.Windows.Forms.Label();
            this.cmbFID = new System.Windows.Forms.ComboBox();
            this.lblFrameId = new System.Windows.Forms.Label();
            this.grpGlobal = new System.Windows.Forms.GroupBox();
            this.chkChair4 = new System.Windows.Forms.CheckBox();
            this.chkChair3 = new System.Windows.Forms.CheckBox();
            this.chkChair2 = new System.Windows.Forms.CheckBox();
            this.chkChair1 = new System.Windows.Forms.CheckBox();
            this.grpLocal = new System.Windows.Forms.GroupBox();
            this.chkStrobe = new System.Windows.Forms.CheckBox();
            this.chkLogo = new System.Windows.Forms.CheckBox();
            this.chkBack = new System.Windows.Forms.CheckBox();
            this.chkHeadrest = new System.Windows.Forms.CheckBox();
            this.chkFrtRh = new System.Windows.Forms.CheckBox();
            this.chkFrtLh = new System.Windows.Forms.CheckBox();
            this.grpControl = new System.Windows.Forms.GroupBox();
            this.chkCJFadeEnabled = new System.Windows.Forms.CheckBox();
            this.chkLINErr = new System.Windows.Forms.CheckBox();
            this.chkSOH = new System.Windows.Forms.CheckBox();
            this.cmbCJMode = new System.Windows.Forms.ComboBox();
            this.lblLampMode = new System.Windows.Forms.Label();
            this.grpColor = new System.Windows.Forms.GroupBox();
            this.pnlColorPatch = new System.Windows.Forms.Panel();
            this.nudBrightness = new System.Windows.Forms.NumericUpDown();
            this.lblBr = new System.Windows.Forms.Label();
            this.nudColorY = new System.Windows.Forms.NumericUpDown();
            this.lblCY = new System.Windows.Forms.Label();
            this.nudColorX = new System.Windows.Forms.NumericUpDown();
            this.lblCX = new System.Windows.Forms.Label();
            this.grpOperation = new System.Windows.Forms.GroupBox();
            this.nudFadeOut = new System.Windows.Forms.NumericUpDown();
            this.lblFOUT = new System.Windows.Forms.Label();
            this.nudFadeIn = new System.Windows.Forms.NumericUpDown();
            this.lblFIN = new System.Windows.Forms.Label();
            this.grpTX = new System.Windows.Forms.GroupBox();
            this.btnTxCycle = new System.Windows.Forms.Button();
            this.btnTxOnce = new System.Windows.Forms.Button();
            this.nudInterval = new System.Windows.Forms.NumericUpDown();
            this.lblInterval = new System.Windows.Forms.Label();
            this.grpPreview = new System.Windows.Forms.GroupBox();
            this.lblD4 = new System.Windows.Forms.Label();
            this.lblD3 = new System.Windows.Forms.Label();
            this.lblD2 = new System.Windows.Forms.Label();
            this.lblD1 = new System.Windows.Forms.Label();
            this.txtPreview = new System.Windows.Forms.TextBox();
            this.grpDiagnostic = new System.Windows.Forms.GroupBox();
            this.dgvDiagnostic = new System.Windows.Forms.DataGridView();
            this.btnReadFault = new System.Windows.Forms.Button();
            this.lblD5 = new System.Windows.Forms.Label();
            this.lblStatus = new System.Windows.Forms.Label();
            this.colDiagFid = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagChair = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagFrtLh = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagFrtRh = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagHead = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagBack = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagLogo = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagStrobe = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagResult = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDiagRaw = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.pnlCIEBar.SuspendLayout();
            this.grpFrame.SuspendLayout();
            this.grpGlobal.SuspendLayout();
            this.grpLocal.SuspendLayout();
            this.grpControl.SuspendLayout();
            this.grpColor.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudBrightness)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorX)).BeginInit();
            this.grpOperation.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeOut)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeIn)).BeginInit();
            this.grpTX.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudInterval)).BeginInit();
            this.grpPreview.SuspendLayout();
            this.grpDiagnostic.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvDiagnostic)).BeginInit();
            this.SuspendLayout();
            // 
            // txTimer
            // 
            this.txTimer.Interval = 10;
            this.txTimer.Tick += new System.EventHandler(this.txTimer_Tick);
            // 
            // pnlCIE
            // 
            this.pnlCIE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(10)))), ((int)(((byte)(12)))), ((int)(((byte)(20)))));
            this.pnlCIE.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlCIE.Location = new System.Drawing.Point(12, 12);
            this.pnlCIE.Name = "pnlCIE";
            this.pnlCIE.Size = new System.Drawing.Size(620, 690);
            this.pnlCIE.TabIndex = 0;
            this.pnlCIE.Paint += new System.Windows.Forms.PaintEventHandler(this.pnlCIE_Paint);
            this.pnlCIE.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pnlCIE_MouseClick);
            this.pnlCIE.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pnlCIE_MouseMove);
            this.pnlCIE.Resize += new System.EventHandler(this.pnlCIE_Resize);
            // 
            // pnlCIEBar
            // 
            this.pnlCIEBar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(24)))), ((int)(((byte)(36)))));
            this.pnlCIEBar.Controls.Add(this.lblCursor);
            this.pnlCIEBar.Controls.Add(this.lblSelected);
            this.pnlCIEBar.Controls.Add(this.lblCieMode);
            this.pnlCIEBar.Controls.Add(this.cmbCieMode);
            this.pnlCIEBar.Controls.Add(this.lblCieModeTitle);
            this.pnlCIEBar.Location = new System.Drawing.Point(12, 708);
            this.pnlCIEBar.Name = "pnlCIEBar";
            this.pnlCIEBar.Size = new System.Drawing.Size(620, 78);
            this.pnlCIEBar.TabIndex = 1;
            // 
            // lblCursor
            // 
            this.lblCursor.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(150)))), ((int)(((byte)(170)))), ((int)(((byte)(200)))));
            this.lblCursor.Location = new System.Drawing.Point(382, 45);
            this.lblCursor.Name = "lblCursor";
            this.lblCursor.Size = new System.Drawing.Size(230, 22);
            this.lblCursor.TabIndex = 4;
            this.lblCursor.Text = "Cursor: -";
            this.lblCursor.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // lblSelected
            // 
            this.lblSelected.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(210)))), ((int)(((byte)(80)))));
            this.lblSelected.Location = new System.Drawing.Point(10, 45);
            this.lblSelected.Name = "lblSelected";
            this.lblSelected.Size = new System.Drawing.Size(370, 22);
            this.lblSelected.TabIndex = 3;
            this.lblSelected.Text = "x=0.3127 y=0.3290";
            // 
            // lblCieMode
            // 
            this.lblCieMode.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.lblCieMode.Location = new System.Drawing.Point(300, 12);
            this.lblCieMode.Name = "lblCieMode";
            this.lblCieMode.Size = new System.Drawing.Size(250, 24);
            this.lblCieMode.TabIndex = 2;
            this.lblCieMode.Text = "CIE1931 (xy)";
            // 
            // cmbCieMode
            // 
            this.cmbCieMode.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbCieMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbCieMode.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbCieMode.ForeColor = System.Drawing.Color.White;
            this.cmbCieMode.FormattingEnabled = true;
            this.cmbCieMode.Items.AddRange(new object[] {
            "R/G/B",
            "Cx/Cy",
            "U\'/V\'"});
            this.cmbCieMode.Location = new System.Drawing.Point(82, 9);
            this.cmbCieMode.Name = "cmbCieMode";
            this.cmbCieMode.Size = new System.Drawing.Size(202, 30);
            this.cmbCieMode.TabIndex = 1;
            this.cmbCieMode.SelectedIndexChanged += new System.EventHandler(this.cmbCieMode_SelectedIndexChanged);
            // 
            // lblCieModeTitle
            // 
            this.lblCieModeTitle.AutoSize = true;
            this.lblCieModeTitle.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblCieModeTitle.Location = new System.Drawing.Point(10, 13);
            this.lblCieModeTitle.Name = "lblCieModeTitle";
            this.lblCieModeTitle.Size = new System.Drawing.Size(70, 22);
            this.lblCieModeTitle.TabIndex = 0;
            this.lblCieModeTitle.Text = "Color:";
            // 
            // grpFrame
            // 
            this.grpFrame.Controls.Add(this.cmbCS);
            this.grpFrame.Controls.Add(this.lblChecksum);
            this.grpFrame.Controls.Add(this.cmbFID);
            this.grpFrame.Controls.Add(this.lblFrameId);
            this.grpFrame.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpFrame.Location = new System.Drawing.Point(646, 12);
            this.grpFrame.Name = "grpFrame";
            this.grpFrame.Size = new System.Drawing.Size(650, 70);
            this.grpFrame.TabIndex = 2;
            this.grpFrame.TabStop = false;
            this.grpFrame.Text = "[ CJ Frame ID / Checksum ]";
            // 
            // cmbCS
            // 
            this.cmbCS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbCS.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbCS.Enabled = false;
            this.cmbCS.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbCS.ForeColor = System.Drawing.Color.White;
            this.cmbCS.FormattingEnabled = true;
            this.cmbCS.Items.AddRange(new object[] {
            "Enhanced"});
            this.cmbCS.Location = new System.Drawing.Point(410, 27);
            this.cmbCS.Name = "cmbCS";
            this.cmbCS.Size = new System.Drawing.Size(136, 30);
            this.cmbCS.TabIndex = 3;
            // 
            // lblChecksum
            // 
            this.lblChecksum.AutoSize = true;
            this.lblChecksum.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblChecksum.Location = new System.Drawing.Point(372, 31);
            this.lblChecksum.Name = "lblChecksum";
            this.lblChecksum.Size = new System.Drawing.Size(40, 22);
            this.lblChecksum.TabIndex = 2;
            this.lblChecksum.Text = "CS:";
            // 
            // cmbFID
            // 
            this.cmbFID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbFID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbFID.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbFID.ForeColor = System.Drawing.Color.White;
            this.cmbFID.FormattingEnabled = true;
            this.cmbFID.Items.AddRange(new object[] {
            "0x01 Lamp Control (TX)",
            "0x0A Chair1 Status (RX)",
            "0x0B Chair2 Status (RX)",
            "0x0C Chair3 Status (RX)",
            "0x0D Chair4 Status (RX)"});
            this.cmbFID.Location = new System.Drawing.Point(62, 27);
            this.cmbFID.Name = "cmbFID";
            this.cmbFID.Size = new System.Drawing.Size(296, 30);
            this.cmbFID.TabIndex = 1;
            this.cmbFID.SelectedIndexChanged += new System.EventHandler(this.cmbFID_SelectedIndexChanged);
            // 
            // lblFrameId
            // 
            this.lblFrameId.AutoSize = true;
            this.lblFrameId.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblFrameId.Location = new System.Drawing.Point(10, 31);
            this.lblFrameId.Name = "lblFrameId";
            this.lblFrameId.Size = new System.Drawing.Size(50, 22);
            this.lblFrameId.TabIndex = 0;
            this.lblFrameId.Text = "FID:";
            // 
            // grpGlobal
            // 
            this.grpGlobal.Controls.Add(this.chkChair4);
            this.grpGlobal.Controls.Add(this.chkChair3);
            this.grpGlobal.Controls.Add(this.chkChair2);
            this.grpGlobal.Controls.Add(this.chkChair1);
            this.grpGlobal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpGlobal.Location = new System.Drawing.Point(646, 88);
            this.grpGlobal.Name = "grpGlobal";
            this.grpGlobal.Size = new System.Drawing.Size(650, 58);
            this.grpGlobal.TabIndex = 3;
            this.grpGlobal.TabStop = false;
            this.grpGlobal.Text = "[ D0 Global Group ]";
            // 
            // chkChair4
            // 
            this.chkChair4.AutoSize = true;
            this.chkChair4.Checked = true;
            this.chkChair4.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkChair4.ForeColor = System.Drawing.Color.White;
            this.chkChair4.Location = new System.Drawing.Point(390, 25);
            this.chkChair4.Name = "chkChair4";
            this.chkChair4.Size = new System.Drawing.Size(96, 26);
            this.chkChair4.TabIndex = 3;
            this.chkChair4.Text = "Chair4";
            this.chkChair4.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkChair3
            // 
            this.chkChair3.AutoSize = true;
            this.chkChair3.Checked = true;
            this.chkChair3.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkChair3.ForeColor = System.Drawing.Color.White;
            this.chkChair3.Location = new System.Drawing.Point(266, 25);
            this.chkChair3.Name = "chkChair3";
            this.chkChair3.Size = new System.Drawing.Size(96, 26);
            this.chkChair3.TabIndex = 2;
            this.chkChair3.Text = "Chair3";
            this.chkChair3.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkChair2
            // 
            this.chkChair2.AutoSize = true;
            this.chkChair2.Checked = true;
            this.chkChair2.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkChair2.ForeColor = System.Drawing.Color.White;
            this.chkChair2.Location = new System.Drawing.Point(142, 25);
            this.chkChair2.Name = "chkChair2";
            this.chkChair2.Size = new System.Drawing.Size(96, 26);
            this.chkChair2.TabIndex = 1;
            this.chkChair2.Text = "Chair2";
            this.chkChair2.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkChair1
            // 
            this.chkChair1.AutoSize = true;
            this.chkChair1.Checked = true;
            this.chkChair1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkChair1.ForeColor = System.Drawing.Color.White;
            this.chkChair1.Location = new System.Drawing.Point(18, 25);
            this.chkChair1.Name = "chkChair1";
            this.chkChair1.Size = new System.Drawing.Size(96, 26);
            this.chkChair1.TabIndex = 0;
            this.chkChair1.Text = "Chair1";
            this.chkChair1.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // grpLocal
            // 
            this.grpLocal.Controls.Add(this.chkStrobe);
            this.grpLocal.Controls.Add(this.chkLogo);
            this.grpLocal.Controls.Add(this.chkBack);
            this.grpLocal.Controls.Add(this.chkHeadrest);
            this.grpLocal.Controls.Add(this.chkFrtRh);
            this.grpLocal.Controls.Add(this.chkFrtLh);
            this.grpLocal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpLocal.Location = new System.Drawing.Point(646, 152);
            this.grpLocal.Name = "grpLocal";
            this.grpLocal.Size = new System.Drawing.Size(650, 62);
            this.grpLocal.TabIndex = 4;
            this.grpLocal.TabStop = false;
            this.grpLocal.Text = "[ D1 Local Group ]";
            // 
            // chkStrobe
            // 
            this.chkStrobe.AutoSize = true;
            this.chkStrobe.Checked = true;
            this.chkStrobe.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkStrobe.ForeColor = System.Drawing.Color.White;
            this.chkStrobe.Location = new System.Drawing.Point(462, 27);
            this.chkStrobe.Name = "chkStrobe";
            this.chkStrobe.Size = new System.Drawing.Size(96, 26);
            this.chkStrobe.TabIndex = 5;
            this.chkStrobe.Text = "Strobe";
            this.chkStrobe.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkLogo
            // 
            this.chkLogo.AutoSize = true;
            this.chkLogo.Checked = true;
            this.chkLogo.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkLogo.ForeColor = System.Drawing.Color.White;
            this.chkLogo.Location = new System.Drawing.Point(390, 27);
            this.chkLogo.Name = "chkLogo";
            this.chkLogo.Size = new System.Drawing.Size(76, 26);
            this.chkLogo.TabIndex = 4;
            this.chkLogo.Text = "Logo";
            this.chkLogo.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkBack
            // 
            this.chkBack.AutoSize = true;
            this.chkBack.Checked = true;
            this.chkBack.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkBack.ForeColor = System.Drawing.Color.White;
            this.chkBack.Location = new System.Drawing.Point(320, 27);
            this.chkBack.Name = "chkBack";
            this.chkBack.Size = new System.Drawing.Size(76, 26);
            this.chkBack.TabIndex = 3;
            this.chkBack.Text = "Back";
            this.chkBack.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkHeadrest
            // 
            this.chkHeadrest.AutoSize = true;
            this.chkHeadrest.Checked = true;
            this.chkHeadrest.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkHeadrest.ForeColor = System.Drawing.Color.White;
            this.chkHeadrest.Location = new System.Drawing.Point(210, 27);
            this.chkHeadrest.Name = "chkHeadrest";
            this.chkHeadrest.Size = new System.Drawing.Size(116, 26);
            this.chkHeadrest.TabIndex = 2;
            this.chkHeadrest.Text = "Headrest";
            this.chkHeadrest.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkFrtRh
            // 
            this.chkFrtRh.AutoSize = true;
            this.chkFrtRh.Checked = true;
            this.chkFrtRh.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkFrtRh.ForeColor = System.Drawing.Color.White;
            this.chkFrtRh.Location = new System.Drawing.Point(112, 27);
            this.chkFrtRh.Name = "chkFrtRh";
            this.chkFrtRh.Size = new System.Drawing.Size(96, 26);
            this.chkFrtRh.TabIndex = 1;
            this.chkFrtRh.Text = "FRT_RH";
            this.chkFrtRh.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkFrtLh
            // 
            this.chkFrtLh.AutoSize = true;
            this.chkFrtLh.Checked = true;
            this.chkFrtLh.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkFrtLh.ForeColor = System.Drawing.Color.White;
            this.chkFrtLh.Location = new System.Drawing.Point(12, 27);
            this.chkFrtLh.Name = "chkFrtLh";
            this.chkFrtLh.Size = new System.Drawing.Size(96, 26);
            this.chkFrtLh.TabIndex = 0;
            this.chkFrtLh.Text = "FRT_LH";
            this.chkFrtLh.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // grpControl
            // 
            this.grpControl.Controls.Add(this.chkCJFadeEnabled);
            this.grpControl.Controls.Add(this.chkLINErr);
            this.grpControl.Controls.Add(this.chkSOH);
            this.grpControl.Controls.Add(this.cmbCJMode);
            this.grpControl.Controls.Add(this.lblLampMode);
            this.grpControl.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpControl.Location = new System.Drawing.Point(646, 220);
            this.grpControl.Name = "grpControl";
            this.grpControl.Size = new System.Drawing.Size(650, 64);
            this.grpControl.TabIndex = 5;
            this.grpControl.TabStop = false;
            this.grpControl.Text = "[ D2 Control ]";
            // 
            // chkCJFadeEnabled
            // 
            this.chkCJFadeEnabled.AutoSize = true;
            this.chkCJFadeEnabled.ForeColor = System.Drawing.Color.White;
            this.chkCJFadeEnabled.Location = new System.Drawing.Point(510, 27);
            this.chkCJFadeEnabled.Name = "chkCJFadeEnabled";
            this.chkCJFadeEnabled.Size = new System.Drawing.Size(116, 26);
            this.chkCJFadeEnabled.TabIndex = 4;
            this.chkCJFadeEnabled.Text = "Fade 사용";
            this.chkCJFadeEnabled.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkLINErr
            // 
            this.chkLINErr.AutoSize = true;
            this.chkLINErr.ForeColor = System.Drawing.Color.White;
            this.chkLINErr.Location = new System.Drawing.Point(340, 27);
            this.chkLINErr.Name = "chkLINErr";
            this.chkLINErr.Size = new System.Drawing.Size(76, 26);
            this.chkLINErr.TabIndex = 3;
            this.chkLINErr.Text = "Sync";
            this.chkLINErr.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSOH
            // 
            this.chkSOH.AutoSize = true;
            this.chkSOH.ForeColor = System.Drawing.Color.White;
            this.chkSOH.Location = new System.Drawing.Point(242, 27);
            this.chkSOH.Name = "chkSOH";
            this.chkSOH.Size = new System.Drawing.Size(96, 26);
            this.chkSOH.TabIndex = 2;
            this.chkSOH.Text = "Active";
            this.chkSOH.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // cmbCJMode
            // 
            this.cmbCJMode.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbCJMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbCJMode.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbCJMode.ForeColor = System.Drawing.Color.White;
            this.cmbCJMode.FormattingEnabled = true;
            this.cmbCJMode.Items.AddRange(new object[] {
            "Normal",
            "Flicker"});
            this.cmbCJMode.Location = new System.Drawing.Point(82, 24);
            this.cmbCJMode.Name = "cmbCJMode";
            this.cmbCJMode.Size = new System.Drawing.Size(140, 30);
            this.cmbCJMode.TabIndex = 1;
            this.cmbCJMode.SelectedIndexChanged += new System.EventHandler(this.cmbCJMode_SelectedIndexChanged);
            // 
            // lblLampMode
            // 
            this.lblLampMode.AutoSize = true;
            this.lblLampMode.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblLampMode.Location = new System.Drawing.Point(12, 28);
            this.lblLampMode.Name = "lblLampMode";
            this.lblLampMode.Size = new System.Drawing.Size(60, 22);
            this.lblLampMode.TabIndex = 0;
            this.lblLampMode.Text = "Mode:";
            // 
            // grpColor
            // 
            this.grpColor.Controls.Add(this.pnlColorPatch);
            this.grpColor.Controls.Add(this.nudBrightness);
            this.grpColor.Controls.Add(this.lblBr);
            this.grpColor.Controls.Add(this.nudColorY);
            this.grpColor.Controls.Add(this.lblCY);
            this.grpColor.Controls.Add(this.nudColorX);
            this.grpColor.Controls.Add(this.lblCX);
            this.grpColor.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpColor.Location = new System.Drawing.Point(646, 290);
            this.grpColor.Name = "grpColor";
            this.grpColor.Size = new System.Drawing.Size(650, 66);
            this.grpColor.TabIndex = 6;
            this.grpColor.TabStop = false;
            this.grpColor.Text = "[ D3/D4/D5 Cx/Cy / Brightness ]";
            // 
            // pnlColorPatch
            // 
            this.pnlColorPatch.BackColor = System.Drawing.Color.Black;
            this.pnlColorPatch.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlColorPatch.Location = new System.Drawing.Point(594, 20);
            this.pnlColorPatch.Name = "pnlColorPatch";
            this.pnlColorPatch.Size = new System.Drawing.Size(44, 36);
            this.pnlColorPatch.TabIndex = 6;
            // 
            // nudBrightness
            // 
            this.nudBrightness.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudBrightness.ForeColor = System.Drawing.Color.White;
            this.nudBrightness.Location = new System.Drawing.Point(504, 25);
            this.nudBrightness.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudBrightness.Name = "nudBrightness";
            this.nudBrightness.Size = new System.Drawing.Size(68, 29);
            this.nudBrightness.TabIndex = 5;
            this.nudBrightness.ValueChanged += new System.EventHandler(this.nudColor_ValueChanged);
            // 
            // lblBr
            // 
            this.lblBr.AutoSize = true;
            this.lblBr.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblBr.Location = new System.Drawing.Point(376, 29);
            this.lblBr.Name = "lblBr";
            this.lblBr.Size = new System.Drawing.Size(110, 22);
            this.lblBr.TabIndex = 4;
            this.lblBr.Text = "D5 Bright:";
            // 
            // nudColorY
            // 
            this.nudColorY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudColorY.ForeColor = System.Drawing.Color.White;
            this.nudColorY.Hexadecimal = true;
            this.nudColorY.Location = new System.Drawing.Point(296, 25);
            this.nudColorY.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudColorY.Name = "nudColorY";
            this.nudColorY.Size = new System.Drawing.Size(68, 29);
            this.nudColorY.TabIndex = 3;
            this.nudColorY.ValueChanged += new System.EventHandler(this.nudColor_ValueChanged);
            // 
            // lblCY
            // 
            this.lblCY.AutoSize = true;
            this.lblCY.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblCY.Location = new System.Drawing.Point(214, 29);
            this.lblCY.Name = "lblCY";
            this.lblCY.Size = new System.Drawing.Size(70, 22);
            this.lblCY.TabIndex = 2;
            this.lblCY.Text = "D4 Cy:";
            // 
            // nudColorX
            // 
            this.nudColorX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudColorX.ForeColor = System.Drawing.Color.White;
            this.nudColorX.Hexadecimal = true;
            this.nudColorX.Location = new System.Drawing.Point(108, 25);
            this.nudColorX.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudColorX.Name = "nudColorX";
            this.nudColorX.Size = new System.Drawing.Size(68, 29);
            this.nudColorX.TabIndex = 1;
            this.nudColorX.ValueChanged += new System.EventHandler(this.nudColor_ValueChanged);
            // 
            // lblCX
            // 
            this.lblCX.AutoSize = true;
            this.lblCX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblCX.Location = new System.Drawing.Point(12, 29);
            this.lblCX.Name = "lblCX";
            this.lblCX.Size = new System.Drawing.Size(70, 22);
            this.lblCX.TabIndex = 0;
            this.lblCX.Text = "D3 Cx:";
            // 
            // grpOperation
            // 
            this.grpOperation.Controls.Add(this.nudFadeOut);
            this.grpOperation.Controls.Add(this.lblFOUT);
            this.grpOperation.Controls.Add(this.nudFadeIn);
            this.grpOperation.Controls.Add(this.lblFIN);
            this.grpOperation.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpOperation.Location = new System.Drawing.Point(646, 362);
            this.grpOperation.Name = "grpOperation";
            this.grpOperation.Size = new System.Drawing.Size(650, 62);
            this.grpOperation.TabIndex = 7;
            this.grpOperation.TabStop = false;
            this.grpOperation.Text = "[ D6/D7 Operation Data ]";
            // 
            // nudFadeOut
            // 
            this.nudFadeOut.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudFadeOut.ForeColor = System.Drawing.Color.White;
            this.nudFadeOut.Location = new System.Drawing.Point(396, 24);
            this.nudFadeOut.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudFadeOut.Name = "nudFadeOut";
            this.nudFadeOut.Size = new System.Drawing.Size(80, 29);
            this.nudFadeOut.TabIndex = 3;
            this.nudFadeOut.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudFadeOut.ValueChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // lblFOUT
            // 
            this.lblFOUT.AutoSize = true;
            this.lblFOUT.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblFOUT.Location = new System.Drawing.Point(278, 28);
            this.lblFOUT.Name = "lblFOUT";
            this.lblFOUT.Size = new System.Drawing.Size(120, 22);
            this.lblFOUT.TabIndex = 2;
            this.lblFOUT.Text = "D7 FadeOut:";
            // 
            // nudFadeIn
            // 
            this.nudFadeIn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudFadeIn.ForeColor = System.Drawing.Color.White;
            this.nudFadeIn.Location = new System.Drawing.Point(164, 24);
            this.nudFadeIn.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudFadeIn.Name = "nudFadeIn";
            this.nudFadeIn.Size = new System.Drawing.Size(80, 29);
            this.nudFadeIn.TabIndex = 1;
            this.nudFadeIn.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudFadeIn.ValueChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // lblFIN
            // 
            this.lblFIN.AutoSize = true;
            this.lblFIN.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblFIN.Location = new System.Drawing.Point(42, 28);
            this.lblFIN.Name = "lblFIN";
            this.lblFIN.Size = new System.Drawing.Size(110, 22);
            this.lblFIN.TabIndex = 0;
            this.lblFIN.Text = "D6 FadeIn:";
            // 
            // grpTX
            // 
            this.grpTX.Controls.Add(this.btnTxCycle);
            this.grpTX.Controls.Add(this.btnTxOnce);
            this.grpTX.Controls.Add(this.nudInterval);
            this.grpTX.Controls.Add(this.lblInterval);
            this.grpTX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpTX.Location = new System.Drawing.Point(646, 430);
            this.grpTX.Name = "grpTX";
            this.grpTX.Size = new System.Drawing.Size(650, 68);
            this.grpTX.TabIndex = 8;
            this.grpTX.TabStop = false;
            this.grpTX.Text = "[ LIN TX ]";
            // 
            // btnTxCycle
            // 
            this.btnTxCycle.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(110)))), ((int)(((byte)(190)))));
            this.btnTxCycle.FlatAppearance.BorderSize = 0;
            this.btnTxCycle.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnTxCycle.ForeColor = System.Drawing.Color.White;
            this.btnTxCycle.Location = new System.Drawing.Point(484, 23);
            this.btnTxCycle.Name = "btnTxCycle";
            this.btnTxCycle.Size = new System.Drawing.Size(154, 34);
            this.btnTxCycle.TabIndex = 3;
            this.btnTxCycle.Text = "주기 TX 시작";
            this.btnTxCycle.UseVisualStyleBackColor = false;
            this.btnTxCycle.Click += new System.EventHandler(this.btnTxCycle_Click);
            // 
            // btnTxOnce
            // 
            this.btnTxOnce.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(130)))), ((int)(((byte)(85)))));
            this.btnTxOnce.FlatAppearance.BorderSize = 0;
            this.btnTxOnce.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnTxOnce.ForeColor = System.Drawing.Color.White;
            this.btnTxOnce.Location = new System.Drawing.Point(260, 23);
            this.btnTxOnce.Name = "btnTxOnce";
            this.btnTxOnce.Size = new System.Drawing.Size(120, 34);
            this.btnTxOnce.TabIndex = 2;
            this.btnTxOnce.Text = "즉시 TX";
            this.btnTxOnce.UseVisualStyleBackColor = false;
            this.btnTxOnce.Click += new System.EventHandler(this.btnTxOnce_Click);
            // 
            // nudInterval
            // 
            this.nudInterval.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudInterval.ForeColor = System.Drawing.Color.White;
            this.nudInterval.Location = new System.Drawing.Point(126, 26);
            this.nudInterval.Maximum = new decimal(new int[] {
            60000,
            0,
            0,
            0});
            this.nudInterval.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nudInterval.Name = "nudInterval";
            this.nudInterval.Size = new System.Drawing.Size(100, 29);
            this.nudInterval.TabIndex = 1;
            this.nudInterval.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // lblInterval
            // 
            this.lblInterval.AutoSize = true;
            this.lblInterval.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblInterval.Location = new System.Drawing.Point(12, 30);
            this.lblInterval.Name = "lblInterval";
            this.lblInterval.Size = new System.Drawing.Size(120, 22);
            this.lblInterval.TabIndex = 0;
            this.lblInterval.Text = "Period(ms):";
            // 
            // grpPreview
            // 
            this.grpPreview.Controls.Add(this.lblD4);
            this.grpPreview.Controls.Add(this.lblD3);
            this.grpPreview.Controls.Add(this.lblD2);
            this.grpPreview.Controls.Add(this.lblD1);
            this.grpPreview.Controls.Add(this.txtPreview);
            this.grpPreview.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpPreview.Location = new System.Drawing.Point(646, 504);
            this.grpPreview.Name = "grpPreview";
            this.grpPreview.Size = new System.Drawing.Size(650, 130);
            this.grpPreview.TabIndex = 9;
            this.grpPreview.TabStop = false;
            this.grpPreview.Text = "[ CJ TX Data ]";
            // 
            // lblD4
            // 
            this.lblD4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD4.Location = new System.Drawing.Point(10, 105);
            this.lblD4.Name = "lblD4";
            this.lblD4.Size = new System.Drawing.Size(630, 20);
            this.lblD4.TabIndex = 4;
            this.lblD4.Text = "D6/D7=";
            // 
            // lblD3
            // 
            this.lblD3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD3.Location = new System.Drawing.Point(10, 86);
            this.lblD3.Name = "lblD3";
            this.lblD3.Size = new System.Drawing.Size(630, 20);
            this.lblD3.TabIndex = 3;
            this.lblD3.Text = "D3/D4/D5=";
            // 
            // lblD2
            // 
            this.lblD2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD2.Location = new System.Drawing.Point(10, 67);
            this.lblD2.Name = "lblD2";
            this.lblD2.Size = new System.Drawing.Size(630, 20);
            this.lblD2.TabIndex = 2;
            this.lblD2.Text = "D2=";
            // 
            // lblD1
            // 
            this.lblD1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD1.Location = new System.Drawing.Point(10, 48);
            this.lblD1.Name = "lblD1";
            this.lblD1.Size = new System.Drawing.Size(630, 20);
            this.lblD1.TabIndex = 1;
            this.lblD1.Text = "D0/D1=";
            // 
            // txtPreview
            // 
            this.txtPreview.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(10)))), ((int)(((byte)(14)))), ((int)(((byte)(22)))));
            this.txtPreview.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtPreview.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.txtPreview.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(240)))), ((int)(((byte)(130)))));
            this.txtPreview.Location = new System.Drawing.Point(10, 25);
            this.txtPreview.Name = "txtPreview";
            this.txtPreview.ReadOnly = true;
            this.txtPreview.Size = new System.Drawing.Size(630, 22);
            this.txtPreview.TabIndex = 0;
            // 
            // grpDiagnostic
            // 
            this.grpDiagnostic.Controls.Add(this.lblStatus);
            this.grpDiagnostic.Controls.Add(this.dgvDiagnostic);
            this.grpDiagnostic.Controls.Add(this.btnReadFault);
            this.grpDiagnostic.Controls.Add(this.lblD5);
            this.grpDiagnostic.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpDiagnostic.Location = new System.Drawing.Point(12, 792);
            this.grpDiagnostic.Name = "grpDiagnostic";
            this.grpDiagnostic.Size = new System.Drawing.Size(1289, 176);
            this.grpDiagnostic.TabIndex = 10;
            this.grpDiagnostic.TabStop = false;
            this.grpDiagnostic.Text = "[ Diagnostic RX : COMM / OPEN / SHORT / 5V_FLT / TEMP ]";
            // 
            // dgvDiagnostic
            // 
            this.dgvDiagnostic.AllowUserToAddRows = false;
            this.dgvDiagnostic.AllowUserToDeleteRows = false;
            this.dgvDiagnostic.AllowUserToResizeRows = false;
            this.dgvDiagnostic.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(14)))), ((int)(((byte)(18)))), ((int)(((byte)(26)))));
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(48)))), ((int)(((byte)(66)))));
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Consolas", 9F);
            dataGridViewCellStyle1.ForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(48)))), ((int)(((byte)(66)))));
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.dgvDiagnostic.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.dgvDiagnostic.ColumnHeadersHeight = 24;
            this.dgvDiagnostic.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dgvDiagnostic.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.colDiagFid,
            this.colDiagChair,
            this.colDiagFrtLh,
            this.colDiagFrtRh,
            this.colDiagHead,
            this.colDiagBack,
            this.colDiagLogo,
            this.colDiagStrobe,
            this.colDiagResult,
            this.colDiagRaw});
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(28)))), ((int)(((byte)(40)))));
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Consolas", 9F);
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(55)))), ((int)(((byte)(78)))));
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvDiagnostic.DefaultCellStyle = dataGridViewCellStyle2;
            this.dgvDiagnostic.EnableHeadersVisualStyles = false;
            this.dgvDiagnostic.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(55)))), ((int)(((byte)(65)))), ((int)(((byte)(82)))));
            this.dgvDiagnostic.Location = new System.Drawing.Point(10, 49);
            this.dgvDiagnostic.MultiSelect = false;
            this.dgvDiagnostic.Name = "dgvDiagnostic";
            this.dgvDiagnostic.ReadOnly = true;
            this.dgvDiagnostic.RowHeadersVisible = false;
            this.dgvDiagnostic.RowHeadersWidth = 62;
            this.dgvDiagnostic.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgvDiagnostic.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dgvDiagnostic.Size = new System.Drawing.Size(1262, 116);
            this.dgvDiagnostic.TabIndex = 1;
            this.dgvDiagnostic.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dgvDiagnostic_CellDoubleClick);
            // 
            // btnReadFault
            // 
            this.btnReadFault.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(105)))), ((int)(((byte)(175)))));
            this.btnReadFault.FlatAppearance.BorderSize = 0;
            this.btnReadFault.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnReadFault.ForeColor = System.Drawing.Color.White;
            this.btnReadFault.Location = new System.Drawing.Point(690, 17);
            this.btnReadFault.Name = "btnReadFault";
            this.btnReadFault.Size = new System.Drawing.Size(148, 28);
            this.btnReadFault.TabIndex = 2;
            this.btnReadFault.Text = "고장 Read";
            this.btnReadFault.UseVisualStyleBackColor = false;
            this.btnReadFault.Click += new System.EventHandler(this.btnReadFault_Click);
            // 
            // lblD5
            // 
            this.lblD5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.lblD5.Location = new System.Drawing.Point(10, 22);
            this.lblD5.Name = "lblD5";
            this.lblD5.Size = new System.Drawing.Size(665, 20);
            this.lblD5.TabIndex = 0;
            this.lblD5.Text = "진단 응답 대기";
            // 
            // lblStatus
            // 
            this.lblStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(160)))), ((int)(((byte)(180)))));
            this.lblStatus.Location = new System.Drawing.Point(884, 22);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(216, 21);
            this.lblStatus.TabIndex = 11;
            this.lblStatus.Text = "● LIN 연결 대기";
            // 
            // colDiagFid
            // 
            this.colDiagFid.HeaderText = "FID";
            this.colDiagFid.MinimumWidth = 8;
            this.colDiagFid.Name = "colDiagFid";
            this.colDiagFid.ReadOnly = true;
            this.colDiagFid.Width = 48;
            // 
            // colDiagChair
            // 
            this.colDiagChair.HeaderText = "ID";
            this.colDiagChair.MinimumWidth = 8;
            this.colDiagChair.Name = "colDiagChair";
            this.colDiagChair.ReadOnly = true;
            this.colDiagChair.Width = 58;
            // 
            // colDiagFrtLh
            // 
            this.colDiagFrtLh.HeaderText = "FRT_LH";
            this.colDiagFrtLh.MinimumWidth = 8;
            this.colDiagFrtLh.Name = "colDiagFrtLh";
            this.colDiagFrtLh.ReadOnly = true;
            this.colDiagFrtLh.Width = 140;
            // 
            // colDiagFrtRh
            // 
            this.colDiagFrtRh.HeaderText = "FRT_RH";
            this.colDiagFrtRh.MinimumWidth = 8;
            this.colDiagFrtRh.Name = "colDiagFrtRh";
            this.colDiagFrtRh.ReadOnly = true;
            this.colDiagFrtRh.Width = 140;
            // 
            // colDiagHead
            // 
            this.colDiagHead.HeaderText = "Head";
            this.colDiagHead.MinimumWidth = 8;
            this.colDiagHead.Name = "colDiagHead";
            this.colDiagHead.ReadOnly = true;
            this.colDiagHead.Width = 140;
            // 
            // colDiagBack
            // 
            this.colDiagBack.HeaderText = "Back";
            this.colDiagBack.MinimumWidth = 8;
            this.colDiagBack.Name = "colDiagBack";
            this.colDiagBack.ReadOnly = true;
            this.colDiagBack.Width = 140;
            // 
            // colDiagLogo
            // 
            this.colDiagLogo.HeaderText = "Logo";
            this.colDiagLogo.MinimumWidth = 8;
            this.colDiagLogo.Name = "colDiagLogo";
            this.colDiagLogo.ReadOnly = true;
            this.colDiagLogo.Width = 140;
            // 
            // colDiagStrobe
            // 
            this.colDiagStrobe.HeaderText = "Strobe";
            this.colDiagStrobe.MinimumWidth = 8;
            this.colDiagStrobe.Name = "colDiagStrobe";
            this.colDiagStrobe.ReadOnly = true;
            this.colDiagStrobe.Width = 140;
            // 
            // colDiagResult
            // 
            this.colDiagResult.HeaderText = "Result";
            this.colDiagResult.MinimumWidth = 8;
            this.colDiagResult.Name = "colDiagResult";
            this.colDiagResult.ReadOnly = true;
            this.colDiagResult.Width = 90;
            // 
            // colDiagRaw
            // 
            this.colDiagRaw.HeaderText = "RX Raw Data";
            this.colDiagRaw.MinimumWidth = 8;
            this.colDiagRaw.Name = "colDiagRaw";
            this.colDiagRaw.ReadOnly = true;
            this.colDiagRaw.Width = 200;
            // 
            // FormCjCIE
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 22F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(14)))), ((int)(((byte)(16)))), ((int)(((byte)(24)))));
            this.ClientSize = new System.Drawing.Size(1315, 997);
            this.Controls.Add(this.grpDiagnostic);
            this.Controls.Add(this.grpPreview);
            this.Controls.Add(this.grpTX);
            this.Controls.Add(this.grpOperation);
            this.Controls.Add(this.grpColor);
            this.Controls.Add(this.grpControl);
            this.Controls.Add(this.grpLocal);
            this.Controls.Add(this.grpGlobal);
            this.Controls.Add(this.grpFrame);
            this.Controls.Add(this.pnlCIEBar);
            this.Controls.Add(this.pnlCIE);
            this.Font = new System.Drawing.Font("Consolas", 9F);
            this.Name = "FormCjCIE";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "CJ CIE LIN Control / Diagnostic";
            this.pnlCIEBar.ResumeLayout(false);
            this.pnlCIEBar.PerformLayout();
            this.grpFrame.ResumeLayout(false);
            this.grpFrame.PerformLayout();
            this.grpGlobal.ResumeLayout(false);
            this.grpGlobal.PerformLayout();
            this.grpLocal.ResumeLayout(false);
            this.grpLocal.PerformLayout();
            this.grpControl.ResumeLayout(false);
            this.grpControl.PerformLayout();
            this.grpColor.ResumeLayout(false);
            this.grpColor.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudBrightness)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorX)).EndInit();
            this.grpOperation.ResumeLayout(false);
            this.grpOperation.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeOut)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeIn)).EndInit();
            this.grpTX.ResumeLayout(false);
            this.grpTX.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudInterval)).EndInit();
            this.grpPreview.ResumeLayout(false);
            this.grpPreview.PerformLayout();
            this.grpDiagnostic.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgvDiagnostic)).EndInit();
            this.ResumeLayout(false);

        }

        private System.Windows.Forms.Timer txTimer;
        private System.Windows.Forms.Panel pnlCIE;
        private System.Windows.Forms.Panel pnlCIEBar;
        private System.Windows.Forms.Label lblCieModeTitle;
        private System.Windows.Forms.ComboBox cmbCieMode;
        private System.Windows.Forms.Label lblCieMode;
        private System.Windows.Forms.Label lblSelected;
        private System.Windows.Forms.Label lblCursor;
        private System.Windows.Forms.GroupBox grpFrame;
        private System.Windows.Forms.Label lblFrameId;
        private System.Windows.Forms.ComboBox cmbFID;
        private System.Windows.Forms.Label lblChecksum;
        private System.Windows.Forms.ComboBox cmbCS;
        private System.Windows.Forms.GroupBox grpGlobal;
        private System.Windows.Forms.CheckBox chkChair1;
        private System.Windows.Forms.CheckBox chkChair2;
        private System.Windows.Forms.CheckBox chkChair3;
        private System.Windows.Forms.CheckBox chkChair4;
        private System.Windows.Forms.GroupBox grpLocal;
        private System.Windows.Forms.CheckBox chkFrtLh;
        private System.Windows.Forms.CheckBox chkFrtRh;
        private System.Windows.Forms.CheckBox chkHeadrest;
        private System.Windows.Forms.CheckBox chkBack;
        private System.Windows.Forms.CheckBox chkLogo;
        private System.Windows.Forms.CheckBox chkStrobe;
        private System.Windows.Forms.GroupBox grpControl;
        private System.Windows.Forms.Label lblLampMode;
        private System.Windows.Forms.ComboBox cmbCJMode;
        private System.Windows.Forms.CheckBox chkSOH;
        private System.Windows.Forms.CheckBox chkLINErr;
        private System.Windows.Forms.CheckBox chkCJFadeEnabled;
        private System.Windows.Forms.GroupBox grpColor;
        private System.Windows.Forms.Label lblCX;
        private System.Windows.Forms.NumericUpDown nudColorX;
        private System.Windows.Forms.Label lblCY;
        private System.Windows.Forms.NumericUpDown nudColorY;
        private System.Windows.Forms.Label lblBr;
        private System.Windows.Forms.NumericUpDown nudBrightness;
        private System.Windows.Forms.Panel pnlColorPatch;
        private System.Windows.Forms.GroupBox grpOperation;
        private System.Windows.Forms.Label lblFIN;
        private System.Windows.Forms.NumericUpDown nudFadeIn;
        private System.Windows.Forms.Label lblFOUT;
        private System.Windows.Forms.NumericUpDown nudFadeOut;
        private System.Windows.Forms.GroupBox grpTX;
        private System.Windows.Forms.Label lblInterval;
        private System.Windows.Forms.NumericUpDown nudInterval;
        private System.Windows.Forms.Button btnTxOnce;
        private System.Windows.Forms.Button btnTxCycle;
        private System.Windows.Forms.GroupBox grpPreview;
        private System.Windows.Forms.TextBox txtPreview;
        private System.Windows.Forms.Label lblD1;
        private System.Windows.Forms.Label lblD2;
        private System.Windows.Forms.Label lblD3;
        private System.Windows.Forms.Label lblD4;
        private System.Windows.Forms.GroupBox grpDiagnostic;
        private System.Windows.Forms.Button btnReadFault;
        private System.Windows.Forms.Label lblD5;
        private System.Windows.Forms.DataGridView dgvDiagnostic;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagFid;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagChair;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagFrtLh;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagFrtRh;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagHead;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagBack;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagLogo;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagStrobe;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagResult;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDiagRaw;
    }
}
