namespace LINMaster.Forms
{
    partial class FormMain
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null)) components.Dispose();
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.timerRx = new System.Windows.Forms.Timer(this.components);
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.grpConn = new System.Windows.Forms.GroupBox();
            this.lblInterface = new System.Windows.Forms.Label();
            this.rdoIndieMicro = new System.Windows.Forms.RadioButton();
            this.rdoPeak = new System.Windows.Forms.RadioButton();
            this.rdoRS485 = new System.Windows.Forms.RadioButton();
            this.rdoFT4222 = new System.Windows.Forms.RadioButton();
            this.rdoCANoe = new System.Windows.Forms.RadioButton();
            this.rdoCAN = new System.Windows.Forms.RadioButton();
            this.rdoUSB = new System.Windows.Forms.RadioButton();
            this.rdoLAN8661 = new System.Windows.Forms.RadioButton();
            this.chkFt4222UseOtp = new System.Windows.Forms.CheckBox();
            this.chkAdiMode = new System.Windows.Forms.CheckBox();
            this.chkCjMode = new System.Windows.Forms.CheckBox();
            this.chkMicrochipMode = new System.Windows.Forms.CheckBox();
            this.lblControlOptions = new System.Windows.Forms.Label();
            this.lblPort = new System.Windows.Forms.Label();
            this.cmbPort = new System.Windows.Forms.ComboBox();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.lblLinBaud = new System.Windows.Forms.Label();
            this.cmbLinBaud = new System.Windows.Forms.ComboBox();
            this.lblStatusDot = new System.Windows.Forms.Label();
            this.lblStatusText = new System.Windows.Forms.Label();
            this.btnConnect = new System.Windows.Forms.Button();
            this.grpFT4222 = new System.Windows.Forms.GroupBox();
            this.lblFT4222 = new System.Windows.Forms.Label();
            this.cmbFT4222Port = new System.Windows.Forms.ComboBox();
            this.btnFT4222Refresh = new System.Windows.Forms.Button();
            this.btnFT4222Connect = new System.Windows.Forms.Button();
            this.lblFT4222Status = new System.Windows.Forms.Label();
            this.grpTx = new System.Windows.Forms.GroupBox();
            this.lblTxID = new System.Windows.Forms.Label();
            this.txtTxID = new System.Windows.Forms.TextBox();
            this.lblTxType = new System.Windows.Forms.Label();
            this.cmbTxFrameType = new System.Windows.Forms.ComboBox();
            this.lblTxCS = new System.Windows.Forms.Label();
            this.cmbTxCS = new System.Windows.Forms.ComboBox();
            this.lblTxDLen = new System.Windows.Forms.Label();
            this.nudTxDLen = new System.Windows.Forms.NumericUpDown();
            this.lblTxData = new System.Windows.Forms.Label();
            this.txtTxData = new System.Windows.Forms.TextBox();
            this.btnTxSend = new System.Windows.Forms.Button();
            this.btnTxRequest = new System.Windows.Forms.Button();
            this.grpSched = new System.Windows.Forms.GroupBox();
            this.dgvSched = new System.Windows.Forms.DataGridView();
            this.colEn = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.colID = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colFType = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.colCS = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.colDLen = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colDelay = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.btnSchedAdd = new System.Windows.Forms.Button();
            this.btnSchedDel = new System.Windows.Forms.Button();
            this.btnSchedStart = new System.Windows.Forms.Button();
            this.grpLog = new System.Windows.Forms.GroupBox();
            this.rtbLog = new System.Windows.Forms.RichTextBox();
            this.chkRawLog = new System.Windows.Forms.CheckBox();
            this.btnClearLog = new System.Windows.Forms.Button();
            this.btnSaveLog = new System.Windows.Forms.Button();
            this.btnOpenNodeScan = new System.Windows.Forms.Button();
            this.btnOpenCIE = new System.Windows.Forms.Button();
            this.btnOpenAD3301 = new System.Windows.Forms.Button();
            this.btnOpenParamTuning = new System.Windows.Forms.Button();
            this.lblServerUrl = new System.Windows.Forms.Label();
            this.lblBridgeStatus = new System.Windows.Forms.Label();
            this.grpConn.SuspendLayout();
            this.grpFT4222.SuspendLayout();
            this.grpTx.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudTxDLen)).BeginInit();
            this.grpSched.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSched)).BeginInit();
            this.grpLog.SuspendLayout();
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // timerRx
            // 
            this.timerRx.Interval = 20;
            this.timerRx.Tick += new System.EventHandler(this.timerRx_Tick);
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 115200;
            this.serialPort1.DtrEnable = true;
            this.serialPort1.ReadTimeout = 500;
            this.serialPort1.RtsEnable = true;
            this.serialPort1.WriteTimeout = 500;
            // 
            // grpConn
            // 
            this.grpConn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(34)))), ((int)(((byte)(34)))), ((int)(((byte)(48)))));
            this.grpConn.Controls.Add(this.lblInterface);
            this.grpConn.Controls.Add(this.rdoIndieMicro);
            this.grpConn.Controls.Add(this.rdoPeak);
            this.grpConn.Controls.Add(this.rdoRS485);
            this.grpConn.Controls.Add(this.rdoFT4222);
            this.grpConn.Controls.Add(this.rdoCANoe);
            this.grpConn.Controls.Add(this.rdoCAN);
            this.grpConn.Controls.Add(this.rdoUSB);
            this.grpConn.Controls.Add(this.rdoLAN8661);
            this.grpConn.Controls.Add(this.chkFt4222UseOtp);
            this.grpConn.Controls.Add(this.chkAdiMode);
            this.grpConn.Controls.Add(this.chkCjMode);
            this.grpConn.Controls.Add(this.chkMicrochipMode);
            this.grpConn.Controls.Add(this.lblControlOptions);
            this.grpConn.Controls.Add(this.lblPort);
            this.grpConn.Controls.Add(this.cmbPort);
            this.grpConn.Controls.Add(this.btnRefresh);
            this.grpConn.Controls.Add(this.lblLinBaud);
            this.grpConn.Controls.Add(this.cmbLinBaud);
            this.grpConn.Controls.Add(this.lblStatusDot);
            this.grpConn.Controls.Add(this.lblStatusText);
            this.grpConn.Controls.Add(this.btnConnect);
            this.grpConn.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpConn.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpConn.Location = new System.Drawing.Point(12, 12);
            this.grpConn.Name = "grpConn";
            this.grpConn.Size = new System.Drawing.Size(1018, 227);
            this.grpConn.TabIndex = 0;
            this.grpConn.TabStop = false;
            this.grpConn.Text = "[ 연결 설정  (COM: 115200,8,N,1) ]";
            // 
            // lblInterface
            // 
            this.lblInterface.AutoSize = true;
            this.lblInterface.BackColor = System.Drawing.Color.Transparent;
            this.lblInterface.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblInterface.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblInterface.Location = new System.Drawing.Point(8, 26);
            this.lblInterface.Name = "lblInterface";
            this.lblInterface.Size = new System.Drawing.Size(95, 22);
            this.lblInterface.TabIndex = 10;
            this.lblInterface.Text = "인터페이스:";
            // 
            // rdoIndieMicro
            // 
            this.rdoIndieMicro.AutoSize = true;
            this.rdoIndieMicro.BackColor = System.Drawing.Color.Transparent;
            this.rdoIndieMicro.Checked = true;
            this.rdoIndieMicro.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoIndieMicro.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(220)))), ((int)(((byte)(140)))));
            this.rdoIndieMicro.Location = new System.Drawing.Point(115, 26);
            this.rdoIndieMicro.Name = "rdoIndieMicro";
            this.rdoIndieMicro.Size = new System.Drawing.Size(85, 26);
            this.rdoIndieMicro.TabIndex = 11;
            this.rdoIndieMicro.TabStop = true;
            this.rdoIndieMicro.Text = "Indie";
            this.rdoIndieMicro.UseVisualStyleBackColor = false;
            this.rdoIndieMicro.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // rdoPeak
            // 
            this.rdoPeak.AutoSize = true;
            this.rdoPeak.BackColor = System.Drawing.Color.Transparent;
            this.rdoPeak.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoPeak.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(200)))), ((int)(((byte)(80)))));
            this.rdoPeak.Location = new System.Drawing.Point(205, 26);
            this.rdoPeak.Name = "rdoPeak";
            this.rdoPeak.Size = new System.Drawing.Size(75, 26);
            this.rdoPeak.TabIndex = 12;
            this.rdoPeak.Text = "Peak";
            this.rdoPeak.UseVisualStyleBackColor = false;
            this.rdoPeak.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // rdoRS485
            // 
            this.rdoRS485.AutoSize = true;
            this.rdoRS485.BackColor = System.Drawing.Color.Transparent;
            this.rdoRS485.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoRS485.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(140)))), ((int)(((byte)(60)))));
            this.rdoRS485.Location = new System.Drawing.Point(645, 26);
            this.rdoRS485.Name = "rdoRS485";
            this.rdoRS485.Size = new System.Drawing.Size(85, 26);
            this.rdoRS485.TabIndex = 13;
            this.rdoRS485.Text = "RS485";
            this.rdoRS485.UseVisualStyleBackColor = false;
            this.rdoRS485.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // rdoFT4222
            // 
            this.rdoFT4222.AutoSize = true;
            this.rdoFT4222.BackColor = System.Drawing.Color.Transparent;
            this.rdoFT4222.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoFT4222.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(220)))), ((int)(((byte)(150)))));
            this.rdoFT4222.Location = new System.Drawing.Point(535, 26);
            this.rdoFT4222.Name = "rdoFT4222";
            this.rdoFT4222.Size = new System.Drawing.Size(95, 26);
            this.rdoFT4222.TabIndex = 14;
            this.rdoFT4222.Text = "FT4222";
            this.rdoFT4222.UseVisualStyleBackColor = false;
            this.rdoFT4222.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // rdoCANoe
            // 
            this.rdoCANoe.AutoSize = true;
            this.rdoCANoe.BackColor = System.Drawing.Color.Transparent;
            this.rdoCANoe.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoCANoe.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.rdoCANoe.Location = new System.Drawing.Point(290, 26);
            this.rdoCANoe.Name = "rdoCANoe";
            this.rdoCANoe.Size = new System.Drawing.Size(85, 26);
            this.rdoCANoe.TabIndex = 15;
            this.rdoCANoe.Text = "CANoe";
            this.rdoCANoe.UseVisualStyleBackColor = false;
            this.rdoCANoe.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // rdoCAN
            // 
            this.rdoCAN.AutoSize = true;
            this.rdoCAN.BackColor = System.Drawing.Color.Transparent;
            this.rdoCAN.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoCAN.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(170)))), ((int)(((byte)(80)))));
            this.rdoCAN.Location = new System.Drawing.Point(385, 26);
            this.rdoCAN.Name = "rdoCAN";
            this.rdoCAN.Size = new System.Drawing.Size(65, 26);
            this.rdoCAN.TabIndex = 16;
            this.rdoCAN.Text = "CAN";
            this.rdoCAN.UseVisualStyleBackColor = false;
            this.rdoCAN.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // rdoUSB
            // 
            this.rdoUSB.AutoSize = true;
            this.rdoUSB.BackColor = System.Drawing.Color.Transparent;
            this.rdoUSB.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoUSB.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(220)))), ((int)(((byte)(255)))));
            this.rdoUSB.Location = new System.Drawing.Point(460, 26);
            this.rdoUSB.Name = "rdoUSB";
            this.rdoUSB.Size = new System.Drawing.Size(65, 26);
            this.rdoUSB.TabIndex = 17;
            this.rdoUSB.Text = "USB";
            this.rdoUSB.UseVisualStyleBackColor = false;
            this.rdoUSB.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // rdoLAN8661
            // 
            this.rdoLAN8661.AutoSize = true;
            this.rdoLAN8661.BackColor = System.Drawing.Color.Transparent;
            this.rdoLAN8661.Font = new System.Drawing.Font("Consolas", 9F);
            this.rdoLAN8661.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(220)))), ((int)(((byte)(210)))));
            this.rdoLAN8661.Location = new System.Drawing.Point(740, 26);
            this.rdoLAN8661.Name = "rdoLAN8661";
            this.rdoLAN8661.Size = new System.Drawing.Size(105, 26);
            this.rdoLAN8661.TabIndex = 18;
            this.rdoLAN8661.Text = "LAN8661";
            this.rdoLAN8661.UseVisualStyleBackColor = false;
            this.rdoLAN8661.CheckedChanged += new System.EventHandler(this.rdoInterface_CheckedChanged);
            // 
            // chkFt4222UseOtp
            // 
            this.chkFt4222UseOtp.AutoSize = true;
            this.chkFt4222UseOtp.BackColor = System.Drawing.Color.Transparent;
            this.chkFt4222UseOtp.Font = new System.Drawing.Font("Consolas", 9F);
            this.chkFt4222UseOtp.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(220)))), ((int)(((byte)(150)))));
            this.chkFt4222UseOtp.Location = new System.Drawing.Point(115, 57);
            this.chkFt4222UseOtp.Name = "chkFt4222UseOtp";
            this.chkFt4222UseOtp.Size = new System.Drawing.Size(156, 26);
            this.chkFt4222UseOtp.TabIndex = 16;
            this.chkFt4222UseOtp.Text = "OTP MAC/Node";
            this.chkFt4222UseOtp.UseVisualStyleBackColor = false;
            // 
            // chkAdiMode
            // 
            this.chkAdiMode.AutoSize = true;
            this.chkAdiMode.BackColor = System.Drawing.Color.Transparent;
            this.chkAdiMode.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.chkAdiMode.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(190)))), ((int)(((byte)(80)))));
            this.chkAdiMode.Location = new System.Drawing.Point(300, 57);
            this.chkAdiMode.Name = "chkAdiMode";
            this.chkAdiMode.Size = new System.Drawing.Size(146, 26);
            this.chkAdiMode.TabIndex = 17;
            this.chkAdiMode.Text = "ADI Control";
            this.chkAdiMode.UseVisualStyleBackColor = false;
            this.chkAdiMode.CheckedChanged += new System.EventHandler(this.chkAdiMode_CheckedChanged);
            // 
            // chkCjMode
            // 
            this.chkCjMode.AutoSize = true;
            this.chkCjMode.BackColor = System.Drawing.Color.Transparent;
            this.chkCjMode.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.chkCjMode.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(220)))), ((int)(((byte)(255)))));
            this.chkCjMode.Location = new System.Drawing.Point(455, 57);
            this.chkCjMode.Name = "chkCjMode";
            this.chkCjMode.Size = new System.Drawing.Size(136, 26);
            this.chkCjMode.TabIndex = 18;
            this.chkCjMode.Text = "CJ Control";
            this.chkCjMode.UseVisualStyleBackColor = false;
            this.chkCjMode.CheckedChanged += new System.EventHandler(this.chkCjMode_CheckedChanged);
            // 
            // chkMicrochipMode
            // 
            this.chkMicrochipMode.AutoSize = true;
            this.chkMicrochipMode.BackColor = System.Drawing.Color.Transparent;
            this.chkMicrochipMode.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.chkMicrochipMode.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(220)))), ((int)(((byte)(210)))));
            this.chkMicrochipMode.Location = new System.Drawing.Point(600, 57);
            this.chkMicrochipMode.Name = "chkMicrochipMode";
            this.chkMicrochipMode.Size = new System.Drawing.Size(206, 26);
            this.chkMicrochipMode.TabIndex = 19;
            this.chkMicrochipMode.Text = "Microchip Control";
            this.chkMicrochipMode.UseVisualStyleBackColor = false;
            this.chkMicrochipMode.CheckedChanged += new System.EventHandler(this.chkMicrochipMode_CheckedChanged);
            // 
            // lblControlOptions
            // 
            this.lblControlOptions.AutoSize = true;
            this.lblControlOptions.BackColor = System.Drawing.Color.Transparent;
            this.lblControlOptions.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblControlOptions.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblControlOptions.Location = new System.Drawing.Point(8, 57);
            this.lblControlOptions.Name = "lblControlOptions";
            this.lblControlOptions.Size = new System.Drawing.Size(50, 22);
            this.lblControlOptions.TabIndex = 19;
            this.lblControlOptions.Text = "옵션:";
            // 
            // lblPort
            // 
            this.lblPort.AutoSize = true;
            this.lblPort.BackColor = System.Drawing.Color.Transparent;
            this.lblPort.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblPort.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblPort.Location = new System.Drawing.Point(6, 88);
            this.lblPort.Name = "lblPort";
            this.lblPort.Size = new System.Drawing.Size(110, 22);
            this.lblPort.TabIndex = 0;
            this.lblPort.Text = "COM Port :";
            // 
            // cmbPort
            // 
            this.cmbPort.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbPort.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbPort.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbPort.ForeColor = System.Drawing.Color.White;
            this.cmbPort.Location = new System.Drawing.Point(114, 85);
            this.cmbPort.Name = "cmbPort";
            this.cmbPort.Size = new System.Drawing.Size(168, 30);
            this.cmbPort.TabIndex = 1;
            // 
            // btnRefresh
            // 
            this.btnRefresh.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(90)))), ((int)(((byte)(140)))));
            this.btnRefresh.FlatAppearance.BorderSize = 0;
            this.btnRefresh.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnRefresh.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnRefresh.ForeColor = System.Drawing.Color.White;
            this.btnRefresh.Location = new System.Drawing.Point(288, 88);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(82, 24);
            this.btnRefresh.TabIndex = 2;
            this.btnRefresh.Text = "새로고침";
            this.btnRefresh.UseVisualStyleBackColor = false;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // lblLinBaud
            // 
            this.lblLinBaud.AutoSize = true;
            this.lblLinBaud.BackColor = System.Drawing.Color.Transparent;
            this.lblLinBaud.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblLinBaud.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(200)))), ((int)(((byte)(80)))));
            this.lblLinBaud.Location = new System.Drawing.Point(6, 119);
            this.lblLinBaud.Name = "lblLinBaud";
            this.lblLinBaud.Size = new System.Drawing.Size(110, 22);
            this.lblLinBaud.TabIndex = 13;
            this.lblLinBaud.Text = "LIN Baud :";
            this.lblLinBaud.Visible = false;
            // 
            // cmbLinBaud
            // 
            this.cmbLinBaud.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(50)))), ((int)(((byte)(30)))));
            this.cmbLinBaud.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbLinBaud.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbLinBaud.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(220)))), ((int)(((byte)(100)))));
            this.cmbLinBaud.Items.AddRange(new object[] {
            "2400",
            "9600",
            "10400",
            "19200",
            "20000"});
            this.cmbLinBaud.Location = new System.Drawing.Point(114, 116);
            this.cmbLinBaud.Name = "cmbLinBaud";
            this.cmbLinBaud.Size = new System.Drawing.Size(100, 30);
            this.cmbLinBaud.TabIndex = 14;
            this.cmbLinBaud.Visible = false;
            // 
            // lblStatusDot
            // 
            this.lblStatusDot.BackColor = System.Drawing.Color.Gray;
            this.lblStatusDot.Location = new System.Drawing.Point(8, 151);
            this.lblStatusDot.Name = "lblStatusDot";
            this.lblStatusDot.Size = new System.Drawing.Size(14, 14);
            this.lblStatusDot.TabIndex = 3;
            // 
            // lblStatusText
            // 
            this.lblStatusText.AutoSize = true;
            this.lblStatusText.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblStatusText.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(160)))), ((int)(((byte)(180)))));
            this.lblStatusText.Location = new System.Drawing.Point(33, 147);
            this.lblStatusText.Name = "lblStatusText";
            this.lblStatusText.Size = new System.Drawing.Size(80, 22);
            this.lblStatusText.TabIndex = 4;
            this.lblStatusText.Text = "연결 안됨";
            // 
            // btnConnect
            // 
            this.btnConnect.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(130)))), ((int)(((byte)(60)))));
            this.btnConnect.FlatAppearance.BorderSize = 0;
            this.btnConnect.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnConnect.Font = new System.Drawing.Font("Consolas", 11F, System.Drawing.FontStyle.Bold);
            this.btnConnect.ForeColor = System.Drawing.Color.White;
            this.btnConnect.Location = new System.Drawing.Point(6, 183);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(362, 34);
            this.btnConnect.TabIndex = 5;
            this.btnConnect.Text = "연결";
            this.btnConnect.UseVisualStyleBackColor = false;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // grpFT4222
            // 
            this.grpFT4222.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(34)))), ((int)(((byte)(34)))), ((int)(((byte)(48)))));
            this.grpFT4222.Controls.Add(this.lblFT4222);
            this.grpFT4222.Controls.Add(this.cmbFT4222Port);
            this.grpFT4222.Controls.Add(this.btnFT4222Refresh);
            this.grpFT4222.Controls.Add(this.btnFT4222Connect);
            this.grpFT4222.Controls.Add(this.lblFT4222Status);
            this.grpFT4222.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpFT4222.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(220)))), ((int)(((byte)(150)))));
            this.grpFT4222.Location = new System.Drawing.Point(1322, 36);
            this.grpFT4222.Name = "grpFT4222";
            this.grpFT4222.Size = new System.Drawing.Size(340, 168);
            this.grpFT4222.TabIndex = 100;
            this.grpFT4222.TabStop = false;
            this.grpFT4222.Text = "[ FT4222  →  AD3306  →  LED ]";
            this.grpFT4222.Visible = false;
            // 
            // lblFT4222
            // 
            this.lblFT4222.AutoSize = true;
            this.lblFT4222.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblFT4222.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(190)))), ((int)(((byte)(215)))));
            this.lblFT4222.Location = new System.Drawing.Point(10, 28);
            this.lblFT4222.Name = "lblFT4222";
            this.lblFT4222.Size = new System.Drawing.Size(60, 22);
            this.lblFT4222.TabIndex = 0;
            this.lblFT4222.Text = "장치 :";
            // 
            // cmbFT4222Port
            // 
            this.cmbFT4222Port.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(65)))));
            this.cmbFT4222Port.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbFT4222Port.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbFT4222Port.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.cmbFT4222Port.Location = new System.Drawing.Point(72, 25);
            this.cmbFT4222Port.Name = "cmbFT4222Port";
            this.cmbFT4222Port.Size = new System.Drawing.Size(178, 30);
            this.cmbFT4222Port.TabIndex = 101;
            // 
            // btnFT4222Refresh
            // 
            this.btnFT4222Refresh.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(70)))), ((int)(((byte)(100)))));
            this.btnFT4222Refresh.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnFT4222Refresh.Font = new System.Drawing.Font("Consolas", 9F);
            this.btnFT4222Refresh.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(210)))), ((int)(((byte)(255)))));
            this.btnFT4222Refresh.Location = new System.Drawing.Point(255, 24);
            this.btnFT4222Refresh.Name = "btnFT4222Refresh";
            this.btnFT4222Refresh.Size = new System.Drawing.Size(70, 24);
            this.btnFT4222Refresh.TabIndex = 102;
            this.btnFT4222Refresh.Text = "새로고침";
            this.btnFT4222Refresh.UseVisualStyleBackColor = false;
            this.btnFT4222Refresh.Click += new System.EventHandler(this.btnFT4222Refresh_Click);
            // 
            // btnFT4222Connect
            // 
            this.btnFT4222Connect.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(90)))), ((int)(((byte)(160)))));
            this.btnFT4222Connect.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnFT4222Connect.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnFT4222Connect.ForeColor = System.Drawing.Color.White;
            this.btnFT4222Connect.Location = new System.Drawing.Point(10, 56);
            this.btnFT4222Connect.Name = "btnFT4222Connect";
            this.btnFT4222Connect.Size = new System.Drawing.Size(315, 30);
            this.btnFT4222Connect.TabIndex = 103;
            this.btnFT4222Connect.Text = "FT4222 연결";
            this.btnFT4222Connect.UseVisualStyleBackColor = false;
            this.btnFT4222Connect.Click += new System.EventHandler(this.btnFT4222Connect_Click);
            // 
            // lblFT4222Status
            // 
            this.lblFT4222Status.AutoSize = true;
            this.lblFT4222Status.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblFT4222Status.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(160)))), ((int)(((byte)(180)))));
            this.lblFT4222Status.Location = new System.Drawing.Point(10, 96);
            this.lblFT4222Status.Name = "lblFT4222Status";
            this.lblFT4222Status.Size = new System.Drawing.Size(170, 22);
            this.lblFT4222Status.TabIndex = 104;
            this.lblFT4222Status.Text = "● FT4222 연결 안됨";
            // 
            // grpTx
            // 
            this.grpTx.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(34)))), ((int)(((byte)(34)))), ((int)(((byte)(48)))));
            this.grpTx.Controls.Add(this.lblTxID);
            this.grpTx.Controls.Add(this.txtTxID);
            this.grpTx.Controls.Add(this.lblTxType);
            this.grpTx.Controls.Add(this.cmbTxFrameType);
            this.grpTx.Controls.Add(this.lblTxCS);
            this.grpTx.Controls.Add(this.cmbTxCS);
            this.grpTx.Controls.Add(this.lblTxDLen);
            this.grpTx.Controls.Add(this.nudTxDLen);
            this.grpTx.Controls.Add(this.lblTxData);
            this.grpTx.Controls.Add(this.txtTxData);
            this.grpTx.Controls.Add(this.btnTxSend);
            this.grpTx.Controls.Add(this.btnTxRequest);
            this.grpTx.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpTx.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpTx.Location = new System.Drawing.Point(9, 247);
            this.grpTx.Name = "grpTx";
            this.grpTx.Size = new System.Drawing.Size(425, 229);
            this.grpTx.TabIndex = 1;
            this.grpTx.TabStop = false;
            this.grpTx.Text = "[ 수동 TX / RX 요청 ]";
            // 
            // lblTxID
            // 
            this.lblTxID.AutoSize = true;
            this.lblTxID.BackColor = System.Drawing.Color.Transparent;
            this.lblTxID.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblTxID.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblTxID.Location = new System.Drawing.Point(8, 26);
            this.lblTxID.Name = "lblTxID";
            this.lblTxID.Size = new System.Drawing.Size(120, 22);
            this.lblTxID.TabIndex = 0;
            this.lblTxID.Text = "Frame ID  :";
            // 
            // txtTxID
            // 
            this.txtTxID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.txtTxID.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtTxID.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtTxID.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtTxID.ForeColor = System.Drawing.Color.White;
            this.txtTxID.Location = new System.Drawing.Point(134, 24);
            this.txtTxID.Name = "txtTxID";
            this.txtTxID.Size = new System.Drawing.Size(56, 29);
            this.txtTxID.TabIndex = 1;
            this.txtTxID.Text = "02";
            // 
            // lblTxType
            // 
            this.lblTxType.AutoSize = true;
            this.lblTxType.BackColor = System.Drawing.Color.Transparent;
            this.lblTxType.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblTxType.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblTxType.Location = new System.Drawing.Point(8, 54);
            this.lblTxType.Name = "lblTxType";
            this.lblTxType.Size = new System.Drawing.Size(120, 22);
            this.lblTxType.TabIndex = 2;
            this.lblTxType.Text = "Frame Type:";
            // 
            // cmbTxFrameType
            // 
            this.cmbTxFrameType.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbTxFrameType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbTxFrameType.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbTxFrameType.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbTxFrameType.ForeColor = System.Drawing.Color.White;
            this.cmbTxFrameType.Items.AddRange(new object[] {
            "Publisher (전송)",
            "Subscriber (요청)"});
            this.cmbTxFrameType.Location = new System.Drawing.Point(134, 51);
            this.cmbTxFrameType.Name = "cmbTxFrameType";
            this.cmbTxFrameType.Size = new System.Drawing.Size(150, 30);
            this.cmbTxFrameType.TabIndex = 3;
            this.cmbTxFrameType.SelectedIndexChanged += new System.EventHandler(this.cmbTxFrameType_SelectedIndexChanged);
            // 
            // lblTxCS
            // 
            this.lblTxCS.AutoSize = true;
            this.lblTxCS.BackColor = System.Drawing.Color.Transparent;
            this.lblTxCS.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblTxCS.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblTxCS.Location = new System.Drawing.Point(8, 82);
            this.lblTxCS.Name = "lblTxCS";
            this.lblTxCS.Size = new System.Drawing.Size(120, 22);
            this.lblTxCS.TabIndex = 4;
            this.lblTxCS.Text = "Checksum  :";
            // 
            // cmbTxCS
            // 
            this.cmbTxCS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbTxCS.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbTxCS.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbTxCS.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbTxCS.ForeColor = System.Drawing.Color.White;
            this.cmbTxCS.Items.AddRange(new object[] {
            "Classic",
            "Enhanced"});
            this.cmbTxCS.Location = new System.Drawing.Point(134, 77);
            this.cmbTxCS.Name = "cmbTxCS";
            this.cmbTxCS.Size = new System.Drawing.Size(130, 30);
            this.cmbTxCS.TabIndex = 5;
            // 
            // lblTxDLen
            // 
            this.lblTxDLen.AutoSize = true;
            this.lblTxDLen.BackColor = System.Drawing.Color.Transparent;
            this.lblTxDLen.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblTxDLen.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblTxDLen.Location = new System.Drawing.Point(8, 110);
            this.lblTxDLen.Name = "lblTxDLen";
            this.lblTxDLen.Size = new System.Drawing.Size(130, 22);
            this.lblTxDLen.TabIndex = 6;
            this.lblTxDLen.Text = "Data Length:";
            // 
            // nudTxDLen
            // 
            this.nudTxDLen.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudTxDLen.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudTxDLen.ForeColor = System.Drawing.Color.White;
            this.nudTxDLen.Location = new System.Drawing.Point(134, 108);
            this.nudTxDLen.Maximum = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.nudTxDLen.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudTxDLen.Name = "nudTxDLen";
            this.nudTxDLen.Size = new System.Drawing.Size(56, 29);
            this.nudTxDLen.TabIndex = 7;
            this.nudTxDLen.Value = new decimal(new int[] {
            8,
            0,
            0,
            0});
            // 
            // lblTxData
            // 
            this.lblTxData.AutoSize = true;
            this.lblTxData.BackColor = System.Drawing.Color.Transparent;
            this.lblTxData.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblTxData.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblTxData.Location = new System.Drawing.Point(8, 138);
            this.lblTxData.Name = "lblTxData";
            this.lblTxData.Size = new System.Drawing.Size(120, 22);
            this.lblTxData.TabIndex = 8;
            this.lblTxData.Text = "Data (HEX):";
            // 
            // txtTxData
            // 
            this.txtTxData.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.txtTxData.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtTxData.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtTxData.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtTxData.ForeColor = System.Drawing.Color.White;
            this.txtTxData.Location = new System.Drawing.Point(102, 135);
            this.txtTxData.Name = "txtTxData";
            this.txtTxData.Size = new System.Drawing.Size(246, 29);
            this.txtTxData.TabIndex = 9;
            this.txtTxData.Text = "FF 65 3A FA 01 01 FF 7F";
            // 
            // btnTxSend
            // 
            this.btnTxSend.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(120)))), ((int)(((byte)(200)))));
            this.btnTxSend.FlatAppearance.BorderSize = 0;
            this.btnTxSend.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnTxSend.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnTxSend.ForeColor = System.Drawing.Color.White;
            this.btnTxSend.Location = new System.Drawing.Point(8, 168);
            this.btnTxSend.Name = "btnTxSend";
            this.btnTxSend.Size = new System.Drawing.Size(156, 28);
            this.btnTxSend.TabIndex = 10;
            this.btnTxSend.Text = "SEND (Publisher)";
            this.btnTxSend.UseVisualStyleBackColor = false;
            this.btnTxSend.Click += new System.EventHandler(this.btnTxSend_Click);
            // 
            // btnTxRequest
            // 
            this.btnTxRequest.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(100)))), ((int)(((byte)(40)))));
            this.btnTxRequest.FlatAppearance.BorderSize = 0;
            this.btnTxRequest.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnTxRequest.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnTxRequest.ForeColor = System.Drawing.Color.White;
            this.btnTxRequest.Location = new System.Drawing.Point(170, 168);
            this.btnTxRequest.Name = "btnTxRequest";
            this.btnTxRequest.Size = new System.Drawing.Size(158, 28);
            this.btnTxRequest.TabIndex = 11;
            this.btnTxRequest.Text = "REQUEST (Sub)";
            this.btnTxRequest.UseVisualStyleBackColor = false;
            this.btnTxRequest.Click += new System.EventHandler(this.btnTxRequest_Click);
            // 
            // grpSched
            // 
            this.grpSched.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(34)))), ((int)(((byte)(34)))), ((int)(((byte)(48)))));
            this.grpSched.Controls.Add(this.dgvSched);
            this.grpSched.Controls.Add(this.btnSchedAdd);
            this.grpSched.Controls.Add(this.btnSchedDel);
            this.grpSched.Controls.Add(this.btnSchedStart);
            this.grpSched.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpSched.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpSched.Location = new System.Drawing.Point(440, 247);
            this.grpSched.Name = "grpSched";
            this.grpSched.Size = new System.Drawing.Size(590, 229);
            this.grpSched.TabIndex = 2;
            this.grpSched.TabStop = false;
            this.grpSched.Text = "[ 스케줄 TX ]";
            // 
            // dgvSched
            // 
            this.dgvSched.AllowUserToAddRows = false;
            this.dgvSched.AllowUserToDeleteRows = false;
            this.dgvSched.AllowUserToResizeColumns = false;
            this.dgvSched.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(42)))), ((int)(((byte)(42)))), ((int)(((byte)(58)))));
            this.dgvSched.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.dgvSched.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(26)))), ((int)(((byte)(40)))));
            this.dgvSched.BorderStyle = System.Windows.Forms.BorderStyle.None;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(36)))));
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.dgvSched.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle2;
            this.dgvSched.ColumnHeadersHeight = 24;
            this.dgvSched.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dgvSched.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.colEn,
            this.colID,
            this.colFType,
            this.colCS,
            this.colDLen,
            this.colData,
            this.colDelay});
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(38)))), ((int)(((byte)(38)))), ((int)(((byte)(54)))));
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            dataGridViewCellStyle3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(100)))), ((int)(((byte)(180)))));
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvSched.DefaultCellStyle = dataGridViewCellStyle3;
            this.dgvSched.EnableHeadersVisualStyles = false;
            this.dgvSched.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(50)))), ((int)(((byte)(70)))));
            this.dgvSched.Location = new System.Drawing.Point(10, 22);
            this.dgvSched.Name = "dgvSched";
            this.dgvSched.RowHeadersVisible = false;
            this.dgvSched.RowHeadersWidth = 62;
            this.dgvSched.Size = new System.Drawing.Size(549, 165);
            this.dgvSched.TabIndex = 0;
            // 
            // colEn
            // 
            this.colEn.FalseValue = false;
            this.colEn.HeaderText = "사용";
            this.colEn.MinimumWidth = 8;
            this.colEn.Name = "colEn";
            this.colEn.TrueValue = true;
            this.colEn.Width = 36;
            // 
            // colID
            // 
            this.colID.HeaderText = "ID";
            this.colID.MinimumWidth = 8;
            this.colID.Name = "colID";
            this.colID.Width = 38;
            // 
            // colFType
            // 
            this.colFType.HeaderText = "Type";
            this.colFType.Items.AddRange(new object[] {
            "Publisher",
            "Subscriber"});
            this.colFType.MinimumWidth = 8;
            this.colFType.Name = "colFType";
            this.colFType.Width = 90;
            // 
            // colCS
            // 
            this.colCS.HeaderText = "CS";
            this.colCS.Items.AddRange(new object[] {
            "Classic",
            "Enhanced"});
            this.colCS.MinimumWidth = 8;
            this.colCS.Name = "colCS";
            this.colCS.Width = 90;
            // 
            // colDLen
            // 
            this.colDLen.HeaderText = "Len";
            this.colDLen.MinimumWidth = 8;
            this.colDLen.Name = "colDLen";
            this.colDLen.Width = 45;
            // 
            // colData
            // 
            this.colData.HeaderText = "Data (HEX)";
            this.colData.MinimumWidth = 8;
            this.colData.Name = "colData";
            this.colData.Width = 200;
            // 
            // colDelay
            // 
            this.colDelay.HeaderText = "ms";
            this.colDelay.MinimumWidth = 8;
            this.colDelay.Name = "colDelay";
            this.colDelay.Width = 38;
            // 
            // btnSchedAdd
            // 
            this.btnSchedAdd.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(44)))), ((int)(((byte)(88)))), ((int)(((byte)(54)))));
            this.btnSchedAdd.FlatAppearance.BorderSize = 0;
            this.btnSchedAdd.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSchedAdd.Font = new System.Drawing.Font("Consolas", 9F);
            this.btnSchedAdd.ForeColor = System.Drawing.Color.White;
            this.btnSchedAdd.Location = new System.Drawing.Point(10, 193);
            this.btnSchedAdd.Name = "btnSchedAdd";
            this.btnSchedAdd.Size = new System.Drawing.Size(74, 24);
            this.btnSchedAdd.TabIndex = 1;
            this.btnSchedAdd.Text = "+ 추가";
            this.btnSchedAdd.UseVisualStyleBackColor = false;
            this.btnSchedAdd.Click += new System.EventHandler(this.btnSchedAdd_Click);
            // 
            // btnSchedDel
            // 
            this.btnSchedDel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(44)))), ((int)(((byte)(44)))));
            this.btnSchedDel.FlatAppearance.BorderSize = 0;
            this.btnSchedDel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSchedDel.Font = new System.Drawing.Font("Consolas", 9F);
            this.btnSchedDel.ForeColor = System.Drawing.Color.White;
            this.btnSchedDel.Location = new System.Drawing.Point(90, 193);
            this.btnSchedDel.Name = "btnSchedDel";
            this.btnSchedDel.Size = new System.Drawing.Size(74, 24);
            this.btnSchedDel.TabIndex = 2;
            this.btnSchedDel.Text = "- 삭제";
            this.btnSchedDel.UseVisualStyleBackColor = false;
            this.btnSchedDel.Click += new System.EventHandler(this.btnSchedDel_Click);
            // 
            // btnSchedStart
            // 
            this.btnSchedStart.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(110)))), ((int)(((byte)(190)))));
            this.btnSchedStart.FlatAppearance.BorderSize = 0;
            this.btnSchedStart.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSchedStart.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.btnSchedStart.ForeColor = System.Drawing.Color.White;
            this.btnSchedStart.Location = new System.Drawing.Point(181, 193);
            this.btnSchedStart.Name = "btnSchedStart";
            this.btnSchedStart.Size = new System.Drawing.Size(150, 25);
            this.btnSchedStart.TabIndex = 3;
            this.btnSchedStart.Text = "▶  스케줄 시작";
            this.btnSchedStart.UseVisualStyleBackColor = false;
            this.btnSchedStart.Click += new System.EventHandler(this.btnSchedStart_Click);
            // 
            // grpLog
            // 
            this.grpLog.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(34)))), ((int)(((byte)(34)))), ((int)(((byte)(48)))));
            this.grpLog.Controls.Add(this.rtbLog);
            this.grpLog.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpLog.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpLog.Location = new System.Drawing.Point(6, 473);
            this.grpLog.Name = "grpLog";
            this.grpLog.Size = new System.Drawing.Size(1024, 428);
            this.grpLog.TabIndex = 3;
            this.grpLog.TabStop = false;
            this.grpLog.Text = "[ TX / RX 로그 ]";
            // 
            // rtbLog
            // 
            this.rtbLog.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(11)))), ((int)(((byte)(14)))), ((int)(((byte)(20)))));
            this.rtbLog.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.rtbLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.rtbLog.Font = new System.Drawing.Font("Consolas", 9F);
            this.rtbLog.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(240)))), ((int)(((byte)(140)))));
            this.rtbLog.Location = new System.Drawing.Point(3, 25);
            this.rtbLog.Name = "rtbLog";
            this.rtbLog.ReadOnly = true;
            this.rtbLog.Size = new System.Drawing.Size(1018, 400);
            this.rtbLog.TabIndex = 0;
            this.rtbLog.Text = "";
            this.rtbLog.WordWrap = false;
            // 
            // chkRawLog
            // 
            this.chkRawLog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.chkRawLog.AutoSize = true;
            this.chkRawLog.Font = new System.Drawing.Font("Consolas", 9F);
            this.chkRawLog.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.chkRawLog.Location = new System.Drawing.Point(10, 913);
            this.chkRawLog.Name = "chkRawLog";
            this.chkRawLog.Size = new System.Drawing.Size(106, 26);
            this.chkRawLog.TabIndex = 10;
            this.chkRawLog.Text = "RAW 표시";
            this.chkRawLog.UseVisualStyleBackColor = false;
            // 
            // btnClearLog
            // 
            this.btnClearLog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnClearLog.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(40)))), ((int)(((byte)(40)))));
            this.btnClearLog.FlatAppearance.BorderSize = 0;
            this.btnClearLog.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnClearLog.Font = new System.Drawing.Font("Consolas", 9F);
            this.btnClearLog.ForeColor = System.Drawing.Color.White;
            this.btnClearLog.Location = new System.Drawing.Point(110, 913);
            this.btnClearLog.Name = "btnClearLog";
            this.btnClearLog.Size = new System.Drawing.Size(112, 26);
            this.btnClearLog.TabIndex = 11;
            this.btnClearLog.Text = "로그 지우기";
            this.btnClearLog.UseVisualStyleBackColor = false;
            this.btnClearLog.Click += new System.EventHandler(this.btnClearLog_Click);
            // 
            // btnSaveLog
            // 
            this.btnSaveLog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnSaveLog.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(60)))), ((int)(((byte)(100)))));
            this.btnSaveLog.FlatAppearance.BorderSize = 0;
            this.btnSaveLog.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSaveLog.Font = new System.Drawing.Font("Consolas", 9F);
            this.btnSaveLog.ForeColor = System.Drawing.Color.White;
            this.btnSaveLog.Location = new System.Drawing.Point(228, 913);
            this.btnSaveLog.Name = "btnSaveLog";
            this.btnSaveLog.Size = new System.Drawing.Size(114, 26);
            this.btnSaveLog.TabIndex = 12;
            this.btnSaveLog.Text = "💾 로그 저장";
            this.btnSaveLog.UseVisualStyleBackColor = false;
            this.btnSaveLog.Click += new System.EventHandler(this.btnSaveLog_Click);
            // 
            // btnOpenNodeScan
            // 
            this.btnOpenNodeScan.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnOpenNodeScan.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(90)))), ((int)(((byte)(150)))));
            this.btnOpenNodeScan.FlatAppearance.BorderSize = 0;
            this.btnOpenNodeScan.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnOpenNodeScan.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnOpenNodeScan.ForeColor = System.Drawing.Color.White;
            this.btnOpenNodeScan.Location = new System.Drawing.Point(348, 913);
            this.btnOpenNodeScan.Name = "btnOpenNodeScan";
            this.btnOpenNodeScan.Size = new System.Drawing.Size(124, 26);
            this.btnOpenNodeScan.TabIndex = 12;
            this.btnOpenNodeScan.Text = "Node Scan 열기";
            this.btnOpenNodeScan.UseVisualStyleBackColor = false;
            this.btnOpenNodeScan.Click += new System.EventHandler(this.btnOpenNodeScan_Click);
            // 
            // btnOpenCIE
            // 
            this.btnOpenCIE.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnOpenCIE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(120)))), ((int)(((byte)(80)))));
            this.btnOpenCIE.FlatAppearance.BorderSize = 0;
            this.btnOpenCIE.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnOpenCIE.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnOpenCIE.ForeColor = System.Drawing.Color.White;
            this.btnOpenCIE.Location = new System.Drawing.Point(478, 914);
            this.btnOpenCIE.Name = "btnOpenCIE";
            this.btnOpenCIE.Size = new System.Drawing.Size(125, 26);
            this.btnOpenCIE.TabIndex = 13;
            this.btnOpenCIE.Text = "CIE 색도도 TX 열기";
            this.btnOpenCIE.UseVisualStyleBackColor = false;
            this.btnOpenCIE.Click += new System.EventHandler(this.btnOpenCIE_Click);
            // 
            // btnOpenAD3301
            // 
            this.btnOpenAD3301.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnOpenAD3301.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(110)))), ((int)(((byte)(80)))));
            this.btnOpenAD3301.FlatAppearance.BorderSize = 0;
            this.btnOpenAD3301.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnOpenAD3301.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnOpenAD3301.ForeColor = System.Drawing.Color.White;
            this.btnOpenAD3301.Location = new System.Drawing.Point(760, 914);
            this.btnOpenAD3301.Name = "btnOpenAD3301";
            this.btnOpenAD3301.Size = new System.Drawing.Size(100, 26);
            this.btnOpenAD3301.TabIndex = 30;
            this.btnOpenAD3301.Text = "AD3301 설정";
            this.btnOpenAD3301.UseVisualStyleBackColor = false;
            this.btnOpenAD3301.Click += new System.EventHandler(this.btnOpenAD3301_Click);
            // 
            // btnOpenParamTuning
            // 
            this.btnOpenParamTuning.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnOpenParamTuning.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(60)))), ((int)(((byte)(140)))));
            this.btnOpenParamTuning.FlatAppearance.BorderSize = 0;
            this.btnOpenParamTuning.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnOpenParamTuning.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnOpenParamTuning.ForeColor = System.Drawing.Color.White;
            this.btnOpenParamTuning.Location = new System.Drawing.Point(609, 914);
            this.btnOpenParamTuning.Name = "btnOpenParamTuning";
            this.btnOpenParamTuning.Size = new System.Drawing.Size(143, 26);
            this.btnOpenParamTuning.TabIndex = 14;
            this.btnOpenParamTuning.Text = "Param Tuning 열기";
            this.btnOpenParamTuning.UseVisualStyleBackColor = false;
            this.btnOpenParamTuning.Click += new System.EventHandler(this.btnOpenParamTuning_Click);
            // 
            // lblServerUrl
            // 
            this.lblServerUrl.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblServerUrl.AutoSize = true;
            this.lblServerUrl.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblServerUrl.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(100)))), ((int)(((byte)(100)))));
            this.lblServerUrl.Location = new System.Drawing.Point(866, 918);
            this.lblServerUrl.Name = "lblServerUrl";
            this.lblServerUrl.Size = new System.Drawing.Size(0, 22);
            this.lblServerUrl.TabIndex = 21;
            // 
            // lblBridgeStatus
            // 
            this.lblBridgeStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblBridgeStatus.AutoSize = true;
            this.lblBridgeStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblBridgeStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(100)))), ((int)(((byte)(100)))));
            this.lblBridgeStatus.Location = new System.Drawing.Point(748, 942);
            this.lblBridgeStatus.Name = "lblBridgeStatus";
            this.lblBridgeStatus.Size = new System.Drawing.Size(0, 20);
            this.lblBridgeStatus.TabIndex = 23;
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 22F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(26)))), ((int)(((byte)(38)))));
            this.ClientSize = new System.Drawing.Size(1039, 952);
            this.Controls.Add(this.grpConn);
            this.Controls.Add(this.grpFT4222);
            this.Controls.Add(this.grpTx);
            this.Controls.Add(this.grpSched);
            this.Controls.Add(this.grpLog);
            this.Controls.Add(this.chkRawLog);
            this.Controls.Add(this.btnClearLog);
            this.Controls.Add(this.btnSaveLog);
            this.Controls.Add(this.btnOpenNodeScan);
            this.Controls.Add(this.btnOpenCIE);
            this.Controls.Add(this.btnOpenParamTuning);
            this.Controls.Add(this.btnOpenAD3301);
            this.Controls.Add(this.lblServerUrl);
            this.Controls.Add(this.lblBridgeStatus);
            this.Font = new System.Drawing.Font("Consolas", 9F);
            this.MinimumSize = new System.Drawing.Size(900, 640);
            this.Name = "FormMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "LIN Master Tool  |  IndieMicro LIN Gateway V2.0";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            this.grpConn.ResumeLayout(false);
            this.grpConn.PerformLayout();
            this.grpFT4222.ResumeLayout(false);
            this.grpFT4222.PerformLayout();
            this.grpTx.ResumeLayout(false);
            this.grpTx.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudTxDLen)).EndInit();
            this.grpSched.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgvSched)).EndInit();
            this.grpLog.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox    grpConn;
        private System.Windows.Forms.Label       lblInterface;
        private System.Windows.Forms.RadioButton rdoIndieMicro;
        private System.Windows.Forms.RadioButton rdoPeak;
        private System.Windows.Forms.RadioButton rdoRS485;
        private System.Windows.Forms.RadioButton rdoFT4222;
        private System.Windows.Forms.RadioButton rdoCANoe;
        private System.Windows.Forms.RadioButton rdoCAN;
        private System.Windows.Forms.RadioButton rdoUSB;
        private System.Windows.Forms.RadioButton rdoLAN8661;
        private System.Windows.Forms.CheckBox    chkFt4222UseOtp;
        private System.Windows.Forms.CheckBox    chkAdiMode;
        private System.Windows.Forms.CheckBox    chkCjMode;
        private System.Windows.Forms.CheckBox    chkMicrochipMode;
        private System.Windows.Forms.Label       lblControlOptions;
        private System.Windows.Forms.Label       lblPort;
        private System.Windows.Forms.ComboBox    cmbPort;
        private System.Windows.Forms.Button      btnRefresh;
        private System.Windows.Forms.Label       lblLinBaud;
        private System.Windows.Forms.ComboBox    cmbLinBaud;
        private System.Windows.Forms.Label       lblStatusDot;
        private System.Windows.Forms.Label       lblStatusText;
        private System.Windows.Forms.Button      btnConnect;

        private System.Windows.Forms.GroupBox      grpTx;
        private System.Windows.Forms.Label         lblTxID;
        private System.Windows.Forms.TextBox       txtTxID;
        private System.Windows.Forms.Label         lblTxType;
        private System.Windows.Forms.ComboBox      cmbTxFrameType;
        private System.Windows.Forms.Label         lblTxCS;
        private System.Windows.Forms.ComboBox      cmbTxCS;
        private System.Windows.Forms.Label         lblTxDLen;
        private System.Windows.Forms.NumericUpDown nudTxDLen;
        private System.Windows.Forms.Label         lblTxData;
        private System.Windows.Forms.TextBox       txtTxData;
        private System.Windows.Forms.Button        btnTxSend;
        private System.Windows.Forms.Button        btnTxRequest;

        private System.Windows.Forms.GroupBox      grpSched;
        private System.Windows.Forms.DataGridView  dgvSched;
        private System.Windows.Forms.Button        btnSchedAdd;
        private System.Windows.Forms.Button        btnSchedDel;
        private System.Windows.Forms.Button        btnSchedStart;

        private System.Windows.Forms.GroupBox    grpLog;
        private System.Windows.Forms.RichTextBox rtbLog;
        private System.Windows.Forms.CheckBox    chkRawLog;
        private System.Windows.Forms.Button      btnClearLog;
        private System.Windows.Forms.Button      btnSaveLog;
        private System.Windows.Forms.Button      btnOpenNodeScan;
        private System.Windows.Forms.Button      btnOpenCIE;
        private System.Windows.Forms.Button      btnOpenParamTuning;
        private System.Windows.Forms.Button      btnOpenAD3301;

        private System.Windows.Forms.DataGridViewCheckBoxColumn  colEn;
        private System.Windows.Forms.DataGridViewTextBoxColumn   colID;
        private System.Windows.Forms.DataGridViewComboBoxColumn  colFType;
        private System.Windows.Forms.DataGridViewComboBoxColumn  colCS;
        private System.Windows.Forms.DataGridViewTextBoxColumn   colDLen;
        private System.Windows.Forms.DataGridViewTextBoxColumn   colData;
        private System.Windows.Forms.DataGridViewTextBoxColumn   colDelay;
        private System.Windows.Forms.Label       lblBridgeStatus;
        private System.Windows.Forms.Label       lblServerUrl;
        internal System.Windows.Forms.Timer      timer1;
        internal System.Windows.Forms.Timer      timerRx;
        internal System.IO.Ports.SerialPort      serialPort1;

        // ── FT4222 UI 컨트롤 ──────────────────────────────────────────
        private System.Windows.Forms.GroupBox grpFT4222;
        private System.Windows.Forms.Label    lblFT4222;
        internal System.Windows.Forms.ComboBox cmbFT4222Port;
        private System.Windows.Forms.Button   btnFT4222Refresh;
        private System.Windows.Forms.Button   btnFT4222Connect;
        internal System.Windows.Forms.Label   lblFT4222Status;
    }
}
