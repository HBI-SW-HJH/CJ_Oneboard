namespace LINMaster.Forms
{
    partial class FormUDS
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
            this.timerLog = new System.Windows.Forms.Timer(this.components);
            this.pnlHeader = new System.Windows.Forms.Panel();
            this.lblTitle = new System.Windows.Forms.Label();
            this.lblProtocol = new System.Windows.Forms.Label();
            this.lblConnection = new System.Windows.Forms.Label();
            this.pnlLeft = new System.Windows.Forms.Panel();
            this.lblNodeTitle = new System.Windows.Forms.Label();
            this.lstNodes = new System.Windows.Forms.ListBox();
            this.btnScan = new System.Windows.Forms.Button();
            this.lblScanStatus = new System.Windows.Forms.Label();
            this.pnlContent = new System.Windows.Forms.Panel();
            this.grpLog = new System.Windows.Forms.GroupBox();
            this.rtbLog = new System.Windows.Forms.RichTextBox();
            this.btnClearLog = new System.Windows.Forms.Button();
            this.btnToggleLog = new System.Windows.Forms.Button();
            this.btnSaveLog = new System.Windows.Forms.Button();
            this.grpProduct = new System.Windows.Forms.GroupBox();
            this.grpQr = new System.Windows.Forms.GroupBox();
            this.picProductQr = new System.Windows.Forms.PictureBox();
            this.lblQrInfo = new System.Windows.Forms.Label();
            this.lblSerialCaption = new System.Windows.Forms.Label();
            this.txtSerial = new System.Windows.Forms.TextBox();
            this.lblPartCaption = new System.Windows.Forms.Label();
            this.txtPart = new System.Windows.Forms.TextBox();
            this.lblVersionCaption = new System.Windows.Forms.Label();
            this.txtVersion = new System.Windows.Forms.TextBox();
            this.lblChairNumberCaption = new System.Windows.Forms.Label();
            this.nudChairNumber = new System.Windows.Forms.NumericUpDown();
            this.lblInitNumberCaption = new System.Windows.Forms.Label();
            this.txtInitNumber = new System.Windows.Forms.TextBox();
            this.btnWriteProduct = new System.Windows.Forms.Button();
            this.btnXcp = new System.Windows.Forms.Button();
            this.btnVerification = new System.Windows.Forms.Button();
            this.btnUpdate = new System.Windows.Forms.Button();
            this.txtRuntimeStatus = new System.Windows.Forms.TextBox();
            this.lblProductStatus = new System.Windows.Forms.Label();
            this.progressUpdate = new System.Windows.Forms.ProgressBar();
            this.grpProduct.SuspendLayout();
            this.grpQr.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picProductQr)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudChairNumber)).BeginInit();
            this.pnlHeader.SuspendLayout();
            this.pnlLeft.SuspendLayout();
            this.pnlContent.SuspendLayout();
            this.grpLog.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlHeader
            // 
            this.pnlHeader.BackColor = System.Drawing.Color.FromArgb(23, 30, 48);
            this.pnlHeader.Controls.Add(this.lblConnection);
            this.pnlHeader.Controls.Add(this.lblProtocol);
            this.pnlHeader.Controls.Add(this.lblTitle);
            this.pnlHeader.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlHeader.Location = new System.Drawing.Point(0, 0);
            this.pnlHeader.Name = "pnlHeader";
            this.pnlHeader.Size = new System.Drawing.Size(1300, 82);
            this.pnlHeader.TabIndex = 0;
            // 
            // lblTitle
            // 
            this.lblTitle.AutoSize = true;
            this.lblTitle.Font = new System.Drawing.Font("Consolas", 17F, System.Drawing.FontStyle.Bold);
            this.lblTitle.ForeColor = System.Drawing.Color.FromArgb(100, 220, 255);
            this.lblTitle.Location = new System.Drawing.Point(18, 12);
            this.lblTitle.Name = "lblTitle";
            this.lblTitle.Size = new System.Drawing.Size(335, 40);
            this.lblTitle.TabIndex = 0;
            this.lblTitle.Text = "CJ UDS Product Information";
            // 
            // lblProtocol
            // 
            this.lblProtocol.AutoSize = true;
            this.lblProtocol.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblProtocol.ForeColor = System.Drawing.Color.FromArgb(170, 190, 215);
            this.lblProtocol.Location = new System.Drawing.Point(22, 53);
            this.lblProtocol.Name = "lblProtocol";
            this.lblProtocol.Size = new System.Drawing.Size(600, 22);
            this.lblProtocol.TabIndex = 1;
            this.lblProtocol.Text = "LIN 3C/3D Classic | Init Number Scan | Product Information";
            // 
            // lblConnection
            // 
            this.lblConnection.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblConnection.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.lblConnection.ForeColor = System.Drawing.Color.OrangeRed;
            this.lblConnection.Location = new System.Drawing.Point(1065, 27);
            this.lblConnection.Name = "lblConnection";
            this.lblConnection.Size = new System.Drawing.Size(215, 28);
            this.lblConnection.TabIndex = 2;
            this.lblConnection.Text = "● LIN 연결 안됨";
            this.lblConnection.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // pnlLeft
            // 
            this.pnlLeft.BackColor = System.Drawing.Color.FromArgb(18, 23, 38);
            this.pnlLeft.Controls.Add(this.lblScanStatus);
            this.pnlLeft.Controls.Add(this.btnScan);
            this.pnlLeft.Controls.Add(this.lstNodes);
            this.pnlLeft.Controls.Add(this.lblNodeTitle);
            this.pnlLeft.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlLeft.Location = new System.Drawing.Point(0, 82);
            this.pnlLeft.Name = "pnlLeft";
            this.pnlLeft.Padding = new System.Windows.Forms.Padding(14);
            this.pnlLeft.Size = new System.Drawing.Size(280, 618);
            this.pnlLeft.TabIndex = 1;
            // 
            // lblNodeTitle
            // 
            this.lblNodeTitle.AutoSize = true;
            this.lblNodeTitle.Font = new System.Drawing.Font("Consolas", 11F, System.Drawing.FontStyle.Bold);
            this.lblNodeTitle.ForeColor = System.Drawing.Color.FromArgb(120, 220, 180);
            this.lblNodeTitle.Location = new System.Drawing.Point(14, 18);
            this.lblNodeTitle.Name = "lblNodeTitle";
            this.lblNodeTitle.Size = new System.Drawing.Size(204, 26);
            this.lblNodeTitle.TabIndex = 0;
            this.lblNodeTitle.Text = "UDS Board Scan";
            // 
            // lstNodes
            // 
            this.lstNodes.BackColor = System.Drawing.Color.FromArgb(32, 38, 53);
            this.lstNodes.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lstNodes.Font = new System.Drawing.Font("Consolas", 9F);
            this.lstNodes.ForeColor = System.Drawing.Color.White;
            this.lstNodes.FormattingEnabled = true;
            this.lstNodes.ItemHeight = 22;
            this.lstNodes.Location = new System.Drawing.Point(14, 55);
            this.lstNodes.Name = "lstNodes";
            this.lstNodes.Size = new System.Drawing.Size(252, 420);
            this.lstNodes.TabIndex = 1;
            this.lstNodes.SelectedIndexChanged += new System.EventHandler(this.lstNodes_SelectedIndexChanged);
            // 
            // btnScan
            // 
            this.btnScan.BackColor = System.Drawing.Color.FromArgb(45, 105, 175);
            this.btnScan.FlatAppearance.BorderSize = 0;
            this.btnScan.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnScan.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.btnScan.ForeColor = System.Drawing.Color.White;
            this.btnScan.Location = new System.Drawing.Point(14, 492);
            this.btnScan.Name = "btnScan";
            this.btnScan.Size = new System.Drawing.Size(252, 40);
            this.btnScan.TabIndex = 2;
            this.btnScan.Text = "Init Number Scan";
            this.btnScan.UseVisualStyleBackColor = false;
            this.btnScan.Click += new System.EventHandler(this.btnScan_Click);
            // 
            // lblScanStatus
            // 
            this.lblScanStatus.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblScanStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblScanStatus.Location = new System.Drawing.Point(14, 544);
            this.lblScanStatus.Name = "lblScanStatus";
            this.lblScanStatus.Size = new System.Drawing.Size(252, 42);
            this.lblScanStatus.TabIndex = 3;
            this.lblScanStatus.Text = "대기 중";
            this.lblScanStatus.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // pnlContent
            // 
            this.pnlContent.BackColor = System.Drawing.Color.FromArgb(13, 17, 29);
            this.pnlContent.Controls.Add(this.grpProduct);
            this.pnlContent.Controls.Add(this.grpQr);
            this.pnlContent.Controls.Add(this.grpLog);
            this.pnlContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlContent.Location = new System.Drawing.Point(280, 82);
            this.pnlContent.Name = "pnlContent";
            this.pnlContent.Padding = new System.Windows.Forms.Padding(16);
            this.pnlContent.Size = new System.Drawing.Size(1020, 618);
            this.pnlContent.TabIndex = 2;
            // 
            // grpLog
            // 
            this.grpLog.Controls.Add(this.btnClearLog);
            this.grpLog.Controls.Add(this.btnToggleLog);
            this.grpLog.Controls.Add(this.btnSaveLog);
            this.grpLog.Controls.Add(this.rtbLog);
            this.grpLog.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.grpLog.ForeColor = System.Drawing.Color.FromArgb(180, 190, 210);
            this.grpLog.Location = new System.Drawing.Point(16, 426);
            this.grpLog.Name = "grpLog";
            this.grpLog.Size = new System.Drawing.Size(988, 242);
            this.grpLog.TabIndex = 2;
            this.grpLog.TabStop = false;
            this.grpLog.Text = "[ LIN Log : Master Header / Data / Classic Checksum ]";
            // 
            // rtbLog
            // 
            this.rtbLog.BackColor = System.Drawing.Color.FromArgb(8, 12, 21);
            this.rtbLog.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.rtbLog.Font = new System.Drawing.Font("Consolas", 9F);
            this.rtbLog.ForeColor = System.Drawing.Color.Gainsboro;
            this.rtbLog.Location = new System.Drawing.Point(14, 31);
            this.rtbLog.Name = "rtbLog";
            this.rtbLog.ReadOnly = true;
            this.rtbLog.MaxLength = 2147483647;
            this.rtbLog.WordWrap = false;
            this.rtbLog.Size = new System.Drawing.Size(960, 156);
            this.rtbLog.TabIndex = 0;
            this.rtbLog.Text = "";
            // 
            // btnClearLog
            // 
            this.btnClearLog.BackColor = System.Drawing.Color.FromArgb(65, 72, 92);
            this.btnClearLog.FlatAppearance.BorderSize = 0;
            this.btnClearLog.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnClearLog.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnClearLog.ForeColor = System.Drawing.Color.White;
            this.btnClearLog.Location = new System.Drawing.Point(844, 198);
            this.btnClearLog.Name = "btnClearLog";
            this.btnClearLog.Size = new System.Drawing.Size(130, 31);
            this.btnClearLog.TabIndex = 1;
            this.btnClearLog.Text = "로그 지우기";
            this.btnClearLog.UseVisualStyleBackColor = false;
            this.btnClearLog.Click += new System.EventHandler(this.btnClearLog_Click);
            this.btnToggleLog.Name = "btnToggleLog";
            this.btnToggleLog.Location = new System.Drawing.Point(554, 198);
            this.btnToggleLog.Size = new System.Drawing.Size(140, 31);
            this.btnToggleLog.BackColor = System.Drawing.Color.FromArgb(65, 72, 92);
            this.btnToggleLog.ForeColor = System.Drawing.Color.White;
            this.btnToggleLog.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnToggleLog.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnToggleLog.FlatAppearance.BorderSize = 0;
            this.btnToggleLog.Text = "로그: 비활성화";
            this.btnToggleLog.Click += new System.EventHandler(this.btnToggleLog_Click);
            this.btnSaveLog.Name = "btnSaveLog";
            this.btnSaveLog.Location = new System.Drawing.Point(704, 198);
            this.btnSaveLog.Size = new System.Drawing.Size(130, 31);
            this.btnSaveLog.BackColor = System.Drawing.Color.FromArgb(45, 105, 175);
            this.btnSaveLog.ForeColor = System.Drawing.Color.White;
            this.btnSaveLog.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnSaveLog.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSaveLog.FlatAppearance.BorderSize = 0;
            this.btnSaveLog.Text = "로그 저장";
            this.btnSaveLog.Click += new System.EventHandler(this.btnSaveLog_Click);
            this.timerLog.Interval = 250;
            this.timerLog.Tick += new System.EventHandler(this.timerLog_Tick);
            // 
            // grpProduct : native Designer controls (editable in the toolbox).
            this.grpProduct.Controls.Add(this.lblSerialCaption);
            this.grpProduct.Controls.Add(this.txtSerial);
            this.grpProduct.Controls.Add(this.lblPartCaption);
            this.grpProduct.Controls.Add(this.txtPart);
            this.grpProduct.Controls.Add(this.lblVersionCaption);
            this.grpProduct.Controls.Add(this.txtVersion);
            this.grpProduct.Controls.Add(this.lblChairNumberCaption);
            this.grpProduct.Controls.Add(this.nudChairNumber);
            this.grpProduct.Controls.Add(this.lblInitNumberCaption);
            this.grpProduct.Controls.Add(this.txtInitNumber);
            this.grpProduct.Controls.Add(this.btnWriteProduct);
            this.grpProduct.Controls.Add(this.btnXcp);
            this.grpProduct.Controls.Add(this.btnVerification);
            this.grpProduct.Controls.Add(this.btnUpdate);
            this.grpProduct.Controls.Add(this.txtRuntimeStatus);
            this.grpProduct.Controls.Add(this.lblProductStatus);
            this.grpProduct.Controls.Add(this.progressUpdate);
            this.grpProduct.Location = new System.Drawing.Point(16, 14);
            this.grpProduct.Name = "grpProduct";
            this.grpProduct.Size = new System.Drawing.Size(672, 400);
            this.grpProduct.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.grpProduct.ForeColor = System.Drawing.Color.FromArgb(120, 220, 180);
            this.grpProduct.TabIndex = 2;
            this.grpProduct.TabStop = false;
            this.grpProduct.Text = "[ Product Information / One PDS Record ]";
            this.lblSerialCaption.Name = "lblSerialCaption";
            this.lblSerialCaption.Text = "Serial Number";
            this.lblSerialCaption.Location = new System.Drawing.Point(14, 32);
            this.lblSerialCaption.Size = new System.Drawing.Size(132, 26);
            this.lblSerialCaption.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lblPartCaption.Name = "lblPartCaption";
            this.lblPartCaption.Text = "Part Number";
            this.lblPartCaption.Location = new System.Drawing.Point(338, 32);
            this.lblPartCaption.Size = new System.Drawing.Size(120, 26);
            this.lblPartCaption.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lblVersionCaption.Name = "lblVersionCaption";
            this.lblVersionCaption.Text = "Version";
            this.lblVersionCaption.Location = new System.Drawing.Point(14, 78);
            this.lblVersionCaption.Size = new System.Drawing.Size(132, 26);
            this.lblVersionCaption.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lblChairNumberCaption.Name = "lblChairNumberCaption";
            this.lblChairNumberCaption.Text = "Chair Number";
            this.lblChairNumberCaption.Location = new System.Drawing.Point(338, 78);
            this.lblChairNumberCaption.Size = new System.Drawing.Size(120, 26);
            this.lblChairNumberCaption.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.txtSerial.Name = "txtSerial";
            this.txtSerial.Location = new System.Drawing.Point(146, 32);
            this.txtSerial.Size = new System.Drawing.Size(172, 28);
            this.txtSerial.BackColor = System.Drawing.Color.FromArgb(32, 38, 53);
            this.txtSerial.ForeColor = System.Drawing.Color.White;
            this.txtSerial.MaxLength = 10;
            this.txtSerial.Text = "26I16-0001";
            this.txtPart.Name = "txtPart";
            this.txtPart.Location = new System.Drawing.Point(458, 32);
            this.txtPart.Size = new System.Drawing.Size(196, 28);
            this.txtPart.BackColor = System.Drawing.Color.FromArgb(32, 38, 53);
            this.txtPart.ForeColor = System.Drawing.Color.White;
            this.txtPart.MaxLength = 8;
            this.txtPart.Text = "ZX1-0025";
            this.txtVersion.Name = "txtVersion";
            this.txtVersion.Location = new System.Drawing.Point(146, 78);
            this.txtVersion.Size = new System.Drawing.Size(172, 28);
            this.txtVersion.BackColor = System.Drawing.Color.FromArgb(32, 38, 53);
            this.txtVersion.ForeColor = System.Drawing.Color.White;
            this.txtVersion.MaxLength = 11;
            this.txtVersion.Text = "0.0.0.1";
            this.txtVersion.ReadOnly = true;
            this.txtVersion.BackColor = System.Drawing.Color.FromArgb(24, 29, 42);
            this.txtVersion.ForeColor = System.Drawing.Color.FromArgb(130, 220, 255);
            this.nudChairNumber.Name = "nudChairNumber";
            this.nudChairNumber.Location = new System.Drawing.Point(458, 78);
            this.nudChairNumber.Size = new System.Drawing.Size(100, 28);
            this.nudChairNumber.Hexadecimal = true;
            this.nudChairNumber.Minimum = 10;
            this.nudChairNumber.Maximum = 13;
            this.nudChairNumber.Value = 10;
            this.nudChairNumber.BackColor = System.Drawing.Color.FromArgb(32, 38, 53);
            this.nudChairNumber.ForeColor = System.Drawing.Color.White;
            this.lblInitNumberCaption.Name = "lblInitNumberCaption";
            this.lblInitNumberCaption.Text = "Init Number";
            this.lblInitNumberCaption.Location = new System.Drawing.Point(14, 116);
            this.lblInitNumberCaption.Size = new System.Drawing.Size(132, 26);
            this.lblInitNumberCaption.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.txtInitNumber.Name = "txtInitNumber";
            this.txtInitNumber.Location = new System.Drawing.Point(146, 116);
            this.txtInitNumber.Size = new System.Drawing.Size(172, 28);
            this.txtInitNumber.BackColor = System.Drawing.Color.FromArgb(24, 29, 42);
            this.txtInitNumber.ForeColor = System.Drawing.Color.FromArgb(130, 220, 255);
            this.txtInitNumber.ReadOnly = true;
            this.txtInitNumber.Text = "Read Only";
            this.btnWriteProduct.Name = "btnWriteProduct";
            this.btnWriteProduct.Location = new System.Drawing.Point(14, 225);
            this.btnWriteProduct.Size = new System.Drawing.Size(316, 36);
            this.btnWriteProduct.BackColor = System.Drawing.Color.FromArgb(35, 125, 100);
            this.btnWriteProduct.ForeColor = System.Drawing.Color.White;
            this.btnWriteProduct.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnWriteProduct.FlatAppearance.BorderSize = 0;
            this.btnWriteProduct.Text = "Write(쓰기)";
            this.btnWriteProduct.UseVisualStyleBackColor = false;
            this.btnWriteProduct.Click += new System.EventHandler(this.btnWriteProduct_Click);
            this.btnXcp.Name = "btnXcp";
            this.btnXcp.Location = new System.Drawing.Point(338, 225);
            this.btnXcp.Size = new System.Drawing.Size(316, 36);
            this.btnXcp.BackColor = System.Drawing.Color.FromArgb(45, 105, 175);
            this.btnXcp.ForeColor = System.Drawing.Color.White;
            this.btnXcp.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnXcp.FlatAppearance.BorderSize = 0;
            this.btnXcp.Text = "XCP Read";
            this.btnXcp.Enabled = false;
            this.btnXcp.UseVisualStyleBackColor = false;
            this.btnXcp.Click += new System.EventHandler(this.btnXcp_Click);
            this.btnVerification.Name = "btnVerification";
            this.btnVerification.Location = new System.Drawing.Point(14, 270);
            this.btnVerification.Size = new System.Drawing.Size(316, 36);
            this.btnVerification.BackColor = System.Drawing.Color.FromArgb(115, 75, 165);
            this.btnVerification.ForeColor = System.Drawing.Color.White;
            this.btnVerification.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnVerification.FlatAppearance.BorderSize = 0;
            this.btnVerification.Text = "Verification / LED 자동검사";
            this.btnVerification.Enabled = false;
            this.btnVerification.UseVisualStyleBackColor = false;
            this.btnVerification.Click += new System.EventHandler(this.btnVerification_Click);
            this.btnUpdate.Name = "btnUpdate";
            this.btnUpdate.Location = new System.Drawing.Point(338, 270);
            this.btnUpdate.Size = new System.Drawing.Size(316, 36);
            this.btnUpdate.BackColor = System.Drawing.Color.FromArgb(160, 95, 35);
            this.btnUpdate.ForeColor = System.Drawing.Color.White;
            this.btnUpdate.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnUpdate.FlatAppearance.BorderSize = 0;
            this.btnUpdate.Text = "Update / App BIN";
            this.btnUpdate.Enabled = false;
            this.btnUpdate.Click += new System.EventHandler(this.btnUpdate_Click);
            this.txtRuntimeStatus.Name = "txtRuntimeStatus";
            this.txtRuntimeStatus.Location = new System.Drawing.Point(14, 153);
            this.txtRuntimeStatus.Size = new System.Drawing.Size(640, 64);
            this.txtRuntimeStatus.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtRuntimeStatus.BackColor = System.Drawing.Color.FromArgb(24, 29, 42);
            this.txtRuntimeStatus.ForeColor = System.Drawing.Color.FromArgb(130, 220, 255);
            this.txtRuntimeStatus.ReadOnly = true;
            this.txtRuntimeStatus.Multiline = true;
            this.txtRuntimeStatus.Text = "Boot / App 상태: 보드 선택 후 확인\r\nFlag: 0=정상 / 1=업데이트 대기 / 2=HardFault 복구 대기";
            this.lblProductStatus.Name = "lblProductStatus";
            this.lblProductStatus.Location = new System.Drawing.Point(14, 317);
            this.lblProductStatus.Size = new System.Drawing.Size(640, 42);
            this.lblProductStatus.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblProductStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblProductStatus.Text = "Serial / Part / Chair만 수정 가능. Version / Init / 상태는 읽기 전용입니다.";
            this.progressUpdate.Name = "progressUpdate";
            this.progressUpdate.Location = new System.Drawing.Point(14, 367);
            this.progressUpdate.Size = new System.Drawing.Size(640, 18);
            this.progressUpdate.Minimum = 0;
            this.progressUpdate.Maximum = 100;
            this.progressUpdate.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.progressUpdate.TabIndex = 20;
            // grpQr
            this.grpQr.Controls.Add(this.picProductQr);
            this.grpQr.Controls.Add(this.lblQrInfo);
            this.grpQr.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.grpQr.ForeColor = System.Drawing.Color.FromArgb(120, 220, 180);
            this.grpQr.Location = new System.Drawing.Point(704, 14);
            this.grpQr.Name = "grpQr";
            this.grpQr.Size = new System.Drawing.Size(300, 300);
            this.grpQr.TabIndex = 3;
            this.grpQr.TabStop = false;
            this.grpQr.Text = "[ 선택 보드 RX QR ]";
            // picProductQr
            this.picProductQr.BackColor = System.Drawing.Color.White;
            this.picProductQr.Location = new System.Drawing.Point(20, 28);
            this.picProductQr.Name = "picProductQr";
            this.picProductQr.Size = new System.Drawing.Size(260, 240);
            this.picProductQr.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picProductQr.TabIndex = 0;
            this.picProductQr.TabStop = false;
            // lblQrInfo
            this.lblQrInfo.ForeColor = System.Drawing.Color.FromArgb(170, 190, 215);
            this.lblQrInfo.Location = new System.Drawing.Point(12, 272);
            this.lblQrInfo.Name = "lblQrInfo";
            this.lblQrInfo.Size = new System.Drawing.Size(276, 20);
            this.lblQrInfo.TabIndex = 1;
            this.lblQrInfo.Text = "RX 정보 대기";
            this.lblQrInfo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // FormUDS
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(13, 17, 29);
            this.ClientSize = new System.Drawing.Size(1300, 772);
            this.Controls.Add(this.pnlContent);
            this.Controls.Add(this.pnlLeft);
            this.Controls.Add(this.pnlHeader);
            this.Font = new System.Drawing.Font("Consolas", 9F);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "FormUDS";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "CJ UDS - Board Scan & Product Information";
            this.pnlHeader.ResumeLayout(false);
            this.pnlHeader.PerformLayout();
            this.pnlLeft.ResumeLayout(false);
            this.pnlLeft.PerformLayout();
            this.pnlContent.ResumeLayout(false);
            this.grpProduct.ResumeLayout(false);
            this.grpProduct.PerformLayout();
            this.grpQr.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.picProductQr)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudChairNumber)).EndInit();
            this.grpLog.ResumeLayout(false);
            this.ResumeLayout(false);
        }

        #endregion

        private System.Windows.Forms.Panel pnlHeader;
        private System.Windows.Forms.Label lblTitle;
        private System.Windows.Forms.Label lblProtocol;
        private System.Windows.Forms.Label lblConnection;
        private System.Windows.Forms.Panel pnlLeft;
        private System.Windows.Forms.Label lblNodeTitle;
        private System.Windows.Forms.ListBox lstNodes;
        private System.Windows.Forms.Button btnScan;
        private System.Windows.Forms.Label lblScanStatus;
        private System.Windows.Forms.Panel pnlContent;
        private System.Windows.Forms.GroupBox grpLog;
        private System.Windows.Forms.RichTextBox rtbLog;
        private System.Windows.Forms.Button btnClearLog;
        private System.Windows.Forms.Button btnToggleLog;
        private System.Windows.Forms.Button btnSaveLog;
        private System.Windows.Forms.Timer timerLog;
        private System.Windows.Forms.GroupBox grpProduct;
        private System.Windows.Forms.GroupBox grpQr;
        private System.Windows.Forms.PictureBox picProductQr;
        private System.Windows.Forms.Label lblQrInfo;
        private System.Windows.Forms.Label lblSerialCaption;
        private System.Windows.Forms.TextBox txtSerial;
        private System.Windows.Forms.Label lblPartCaption;
        private System.Windows.Forms.TextBox txtPart;
        private System.Windows.Forms.Label lblVersionCaption;
        private System.Windows.Forms.TextBox txtVersion;
        private System.Windows.Forms.Label lblChairNumberCaption;
        private System.Windows.Forms.NumericUpDown nudChairNumber;
        private System.Windows.Forms.Label lblInitNumberCaption;
        private System.Windows.Forms.TextBox txtInitNumber;
        private System.Windows.Forms.Button btnWriteProduct;
        private System.Windows.Forms.Button btnXcp;
        private System.Windows.Forms.Button btnVerification;
        private System.Windows.Forms.Button btnUpdate;
        private System.Windows.Forms.TextBox txtRuntimeStatus;
        private System.Windows.Forms.Label lblProductStatus;
        private System.Windows.Forms.ProgressBar progressUpdate;
    }
}
