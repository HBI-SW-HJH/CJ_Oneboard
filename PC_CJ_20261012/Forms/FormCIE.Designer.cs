namespace LINMaster.Forms
{
    partial class FormCIE
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
            this.txTimer = new System.Windows.Forms.Timer(this.components);
            this.fadeTimer = new System.Windows.Forms.Timer(this.components);
            this.lblFT4222Status = new System.Windows.Forms.Label();
            this.tabColor = new System.Windows.Forms.TabControl();
            this.tabCIE = new System.Windows.Forms.TabPage();
            this.pnlCIE = new System.Windows.Forms.Panel();
            this.pnlCIEBar = new System.Windows.Forms.Panel();
            this.lblModeL = new System.Windows.Forms.Label();
            this.cmbMode = new System.Windows.Forms.ComboBox();
            this.lblCIEMode = new System.Windows.Forms.Label();
            this.lblSelXY = new System.Windows.Forms.Label();
            this.lblCursor = new System.Windows.Forms.Label();
            this.tabImg = new System.Windows.Forms.TabPage();
            this.pnlImage = new System.Windows.Forms.Panel();
            this.pnlImgBar = new System.Windows.Forms.Panel();
            this.btnLoadImg = new System.Windows.Forms.Button();
            this.lblImgInfo = new System.Windows.Forms.Label();
            this.lblImgColor = new System.Windows.Forms.Label();
            this.pnlColorPatch = new System.Windows.Forms.Panel();
            this.lblPatchL = new System.Windows.Forms.Label();
            this.chkOspTxEnable = new System.Windows.Forms.CheckBox();
            this.chkIlasTxEnable = new System.Windows.Forms.CheckBox();
            this.chkRxEnable = new System.Windows.Forms.CheckBox();
            this.grpFID = new System.Windows.Forms.GroupBox();
            this.lblFID = new System.Windows.Forms.Label();
            this.cmbFID = new System.Windows.Forms.ComboBox();
            this.txtManualFID = new System.Windows.Forms.TextBox();
            this.lblCS = new System.Windows.Forms.Label();
            this.cmbCS = new System.Windows.Forms.ComboBox();
            this.grpSF = new System.Windows.Forms.GroupBox();
            this.grpShift = new System.Windows.Forms.GroupBox();
            this.chkRGBWShift = new System.Windows.Forms.CheckBox();
            this.chkLedShift = new System.Windows.Forms.CheckBox();
            this.chkShiftStack = new System.Windows.Forms.CheckBox();
            this.chkLedShiftUp = new System.Windows.Forms.CheckBox();
            this.chkShiftStackUp = new System.Windows.Forms.CheckBox();
            this.chkMove = new System.Windows.Forms.CheckBox();
            this.chkEachControl = new System.Windows.Forms.CheckBox();
            this.chkTouch = new System.Windows.Forms.CheckBox();
            this.nudShiftWidth = new System.Windows.Forms.NumericUpDown();
            this.lblShiftWidth = new System.Windows.Forms.Label();
            this.chkSF1 = new System.Windows.Forms.CheckBox();
            this.chkSF2 = new System.Windows.Forms.CheckBox();
            this.chkSF3 = new System.Windows.Forms.CheckBox();
            this.chkSF4 = new System.Windows.Forms.CheckBox();
            this.chkSF5 = new System.Windows.Forms.CheckBox();
            this.chkSF6 = new System.Windows.Forms.CheckBox();
            this.chkSF7 = new System.Windows.Forms.CheckBox();
            this.chkSF8 = new System.Windows.Forms.CheckBox();
            this.chkSF9 = new System.Windows.Forms.CheckBox();
            this.chkSF10 = new System.Windows.Forms.CheckBox();
            this.chkSF11 = new System.Windows.Forms.CheckBox();
            this.chkSF12 = new System.Windows.Forms.CheckBox();
            this.chkSF13 = new System.Windows.Forms.CheckBox();
            this.chkSF14 = new System.Windows.Forms.CheckBox();
            this.chkSF15 = new System.Windows.Forms.CheckBox();
            this.chkSF16 = new System.Windows.Forms.CheckBox();
            this.chkSF17 = new System.Windows.Forms.CheckBox();
            this.chkSF18 = new System.Windows.Forms.CheckBox();
            this.chkSF19 = new System.Windows.Forms.CheckBox();
            this.chkSF20 = new System.Windows.Forms.CheckBox();
            this.chkSF21 = new System.Windows.Forms.CheckBox();
            this.chkSF22 = new System.Windows.Forms.CheckBox();
            this.chkSF23 = new System.Windows.Forms.CheckBox();
            this.chkSF24 = new System.Windows.Forms.CheckBox();
            this.chkSF25 = new System.Windows.Forms.CheckBox();
            this.btnSFAll = new System.Windows.Forms.Button();
            this.btnSFNone = new System.Windows.Forms.Button();
            this.grpColor = new System.Windows.Forms.GroupBox();
            this.lblCX = new System.Windows.Forms.Label();
            this.nudColorX = new System.Windows.Forms.NumericUpDown();
            this.lblCY = new System.Windows.Forms.Label();
            this.nudColorY = new System.Windows.Forms.NumericUpDown();
            this.lblBr = new System.Windows.Forms.Label();
            this.nudBrightness = new System.Windows.Forms.NumericUpDown();
            this.grpFade = new System.Windows.Forms.GroupBox();
            this.lblFIN = new System.Windows.Forms.Label();
            this.nudFadeIn = new System.Windows.Forms.NumericUpDown();
            this.lblFOUT = new System.Windows.Forms.Label();
            this.nudFadeOut = new System.Windows.Forms.NumericUpDown();
            this.chkNoFade = new System.Windows.Forms.CheckBox();
            this.grpStatus = new System.Windows.Forms.GroupBox();
            this.chkSOH = new System.Windows.Forms.CheckBox();
            this.chkLINErr = new System.Windows.Forms.CheckBox();
            this.grpPreview = new System.Windows.Forms.GroupBox();
            this.txtPreview = new System.Windows.Forms.TextBox();
            this.lblD1 = new System.Windows.Forms.Label();
            this.lblD2 = new System.Windows.Forms.Label();
            this.lblD3 = new System.Windows.Forms.Label();
            this.lblD4 = new System.Windows.Forms.Label();
            this.lblD5 = new System.Windows.Forms.Label();
            this.lblD6 = new System.Windows.Forms.Label();
            this.lblD7 = new System.Windows.Forms.Label();
            this.lblD8 = new System.Windows.Forms.Label();
            this.grpTouch = new System.Windows.Forms.GroupBox();
            this.lblTouch = new System.Windows.Forms.Label();
            this.touchTrackBar = new System.Windows.Forms.TrackBar();
            this.lblTouchValue = new System.Windows.Forms.Label();
            this.grpTX = new System.Windows.Forms.GroupBox();
            this.lblIvL = new System.Windows.Forms.Label();
            this.nudInterval = new System.Windows.Forms.NumericUpDown();
            this.lblMs = new System.Windows.Forms.Label();
            this.btnTxOnce = new System.Windows.Forms.Button();
            this.btnTxCycle = new System.Windows.Forms.Button();
            this.sunshinePanel = new System.Windows.Forms.Panel();
            this.sunSelectionGroupBox = new System.Windows.Forms.GroupBox();
            this.sunTargetLabel = new System.Windows.Forms.Label();
            this.sunTargetComboBox = new System.Windows.Forms.ComboBox();
            this.sunAllRadioButton = new System.Windows.Forms.RadioButton();
            this.sunIndividualRadioButton = new System.Windows.Forms.RadioButton();
            this.sunGroupRadioButton = new System.Windows.Forms.RadioButton();
            this.sunIndividualLabel = new System.Windows.Forms.Label();
            this.sunIndividualTextBox = new System.Windows.Forms.TextBox();
            this.sunApplyButton = new System.Windows.Forms.Button();
            this.sunTuningCheckBox = new System.Windows.Forms.CheckBox();
            this.sunTemperatureCheckBox = new System.Windows.Forms.CheckBox();
            this.sunOspCurrentModeGroupBox = new System.Windows.Forms.GroupBox();
            this.sunDayRadioButton = new System.Windows.Forms.RadioButton();
            this.sunNightRadioButton = new System.Windows.Forms.RadioButton();
            this.sunHintLabel = new System.Windows.Forms.Label();
            this.sunLedSelectionGroupBox = new System.Windows.Forms.GroupBox();
            this.sunLedSelectCheckBox1 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox2 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox3 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox4 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox5 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox6 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox7 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox8 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox9 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox10 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox11 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox12 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox13 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox14 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox15 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox16 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox17 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox18 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox19 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox20 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox21 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox22 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox23 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox24 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox25 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox26 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox27 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox28 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox29 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox30 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox31 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox32 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox33 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox34 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox35 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox36 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox37 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox38 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox39 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox40 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox41 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox42 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox43 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox44 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox45 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox46 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox47 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox48 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox49 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox50 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox51 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox52 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox53 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox54 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox55 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox56 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox57 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox58 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox59 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox60 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox61 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox62 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox63 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox64 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox65 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox66 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox67 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox68 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox69 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox70 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox71 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox72 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox73 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox74 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox75 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox76 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox77 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox78 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox79 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox80 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox81 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox82 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox83 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectCheckBox84 = new System.Windows.Forms.CheckBox();
            this.sunLedSelectAllButton = new System.Windows.Forms.Button();
            this.sunLedSelectClearButton = new System.Windows.Forms.Button();
            this.sunLedSelectInvertButton = new System.Windows.Forms.Button();
            this.sunLedSelectHintLabel = new System.Windows.Forms.Label();
            this.sunGroupsGroupBox = new System.Windows.Forms.GroupBox();
            this.sunGroupCheckBox1 = new System.Windows.Forms.CheckBox();
            this.sunGroupCheckBox2 = new System.Windows.Forms.CheckBox();
            this.sunGroupCheckBox3 = new System.Windows.Forms.CheckBox();
            this.sunGroupCheckBox4 = new System.Windows.Forms.CheckBox();
            this.sunGroupPointer2 = new System.Windows.Forms.Label();
            this.sunGroupBarPanel = new System.Windows.Forms.Panel();
            this.sunGroupSegment1 = new System.Windows.Forms.Panel();
            this.sunGroupSegment2 = new System.Windows.Forms.Panel();
            this.sunGroupSegment4 = new System.Windows.Forms.Panel();
            this.sunGroupSegment3 = new System.Windows.Forms.Panel();
            this.sunGroupPointer1 = new System.Windows.Forms.Label();
            this.sunGroupPointer3 = new System.Windows.Forms.Label();
            this.sunGroupValue1 = new System.Windows.Forms.Label();
            this.sunGroupValue2 = new System.Windows.Forms.Label();
            this.sunGroupValue3 = new System.Windows.Forms.Label();
            this.sunGroupStartLabel = new System.Windows.Forms.Label();
            this.sunGroupEndLabel = new System.Windows.Forms.Label();
            this.sunReadGroupBox = new System.Windows.Forms.GroupBox();
            this.sunReadButton = new System.Windows.Forms.Button();
            this.sunStatusLabel = new System.Windows.Forms.Label();
            this.sunReadGrid = new System.Windows.Forms.DataGridView();
            this.Type = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.LED = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TuneR = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TuneG = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TuneB = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.PWMR = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.PWMG = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.PWMB = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Temp = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Open = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Short = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Status = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.OTPCathode = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tabColor.SuspendLayout();
            this.tabCIE.SuspendLayout();
            this.pnlCIEBar.SuspendLayout();
            this.tabImg.SuspendLayout();
            this.pnlImgBar.SuspendLayout();
            this.grpFID.SuspendLayout();
            this.grpSF.SuspendLayout();
            this.grpShift.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudShiftWidth)).BeginInit();
            this.grpColor.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBrightness)).BeginInit();
            this.grpFade.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeIn)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeOut)).BeginInit();
            this.grpStatus.SuspendLayout();
            this.grpPreview.SuspendLayout();
            this.grpTouch.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.touchTrackBar)).BeginInit();
            this.grpTX.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudInterval)).BeginInit();
            this.sunshinePanel.SuspendLayout();
            this.sunSelectionGroupBox.SuspendLayout();
            this.sunOspCurrentModeGroupBox.SuspendLayout();
            this.sunLedSelectionGroupBox.SuspendLayout();
            this.sunGroupsGroupBox.SuspendLayout();
            this.sunGroupBarPanel.SuspendLayout();
            this.sunReadGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.sunReadGrid)).BeginInit();
            this.SuspendLayout();
            // 
            // txTimer
            // 
            this.txTimer.Interval = 10;
            this.txTimer.Tick += new System.EventHandler(this.TxTimer_Tick);
            // 
            // fadeTimer
            // 
            this.fadeTimer.Interval = 10;
            this.fadeTimer.Tick += new System.EventHandler(this.FadeTimer_Tick);
            // 
            // lblFT4222Status
            // 
            this.lblFT4222Status.AutoSize = true;
            this.lblFT4222Status.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblFT4222Status.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(160)))), ((int)(((byte)(180)))));
            this.lblFT4222Status.Location = new System.Drawing.Point(6, 556);
            this.lblFT4222Status.Name = "lblFT4222Status";
            this.lblFT4222Status.Size = new System.Drawing.Size(147, 20);
            this.lblFT4222Status.TabIndex = 99;
            this.lblFT4222Status.Text = "● FT4222 연결 안됨";
            // 
            // tabColor
            // 
            this.tabColor.Controls.Add(this.tabCIE);
            this.tabColor.Controls.Add(this.tabImg);
            this.tabColor.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.tabColor.Location = new System.Drawing.Point(4, 12);
            this.tabColor.Name = "tabColor";
            this.tabColor.SelectedIndex = 0;
            this.tabColor.Size = new System.Drawing.Size(640, 731);
            this.tabColor.TabIndex = 0;
            // 
            // tabCIE
            // 
            this.tabCIE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(12)))), ((int)(((byte)(14)))), ((int)(((byte)(22)))));
            this.tabCIE.Controls.Add(this.pnlCIE);
            this.tabCIE.Controls.Add(this.pnlCIEBar);
            this.tabCIE.Font = new System.Drawing.Font("Consolas", 9F);
            this.tabCIE.Location = new System.Drawing.Point(4, 31);
            this.tabCIE.Name = "tabCIE";
            this.tabCIE.Size = new System.Drawing.Size(632, 696);
            this.tabCIE.TabIndex = 0;
            this.tabCIE.Text = "  CIE 색도도  ";
            // 
            // pnlCIE
            // 
            this.pnlCIE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(10)))), ((int)(((byte)(12)))), ((int)(((byte)(20)))));
            this.pnlCIE.Location = new System.Drawing.Point(0, 0);
            this.pnlCIE.Name = "pnlCIE";
            this.pnlCIE.Size = new System.Drawing.Size(632, 628);
            this.pnlCIE.TabIndex = 0;
            this.pnlCIE.Paint += new System.Windows.Forms.PaintEventHandler(this.pnlCIE_Paint);
            this.pnlCIE.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pnlCIE_MouseClick);
            this.pnlCIE.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pnlCIE_MouseMove);
            this.pnlCIE.Resize += new System.EventHandler(this.pnlCIE_Resize);
            // 
            // pnlCIEBar
            // 
            this.pnlCIEBar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(16)))), ((int)(((byte)(18)))), ((int)(((byte)(28)))));
            this.pnlCIEBar.Controls.Add(this.lblModeL);
            this.pnlCIEBar.Controls.Add(this.cmbMode);
            this.pnlCIEBar.Controls.Add(this.lblCIEMode);
            this.pnlCIEBar.Controls.Add(this.lblSelXY);
            this.pnlCIEBar.Controls.Add(this.lblCursor);
            this.pnlCIEBar.Location = new System.Drawing.Point(0, 628);
            this.pnlCIEBar.Name = "pnlCIEBar";
            this.pnlCIEBar.Size = new System.Drawing.Size(632, 68);
            this.pnlCIEBar.TabIndex = 1;
            // 
            // lblModeL
            // 
            this.lblModeL.AutoSize = true;
            this.lblModeL.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblModeL.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblModeL.Location = new System.Drawing.Point(6, 8);
            this.lblModeL.Name = "lblModeL";
            this.lblModeL.Size = new System.Drawing.Size(60, 22);
            this.lblModeL.TabIndex = 0;
            this.lblModeL.Text = "Mode:";
            // 
            // cmbMode
            // 
            this.cmbMode.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbMode.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbMode.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbMode.ForeColor = System.Drawing.Color.White;
            this.cmbMode.Items.AddRange(new object[] {
            "CIE1931 (xy)",
            "CIE1976 (u\'v\')"});
            this.cmbMode.Location = new System.Drawing.Point(50, 5);
            this.cmbMode.Name = "cmbMode";
            this.cmbMode.Size = new System.Drawing.Size(170, 30);
            this.cmbMode.TabIndex = 1;
            this.cmbMode.SelectedIndexChanged += new System.EventHandler(this.cmbMode_SelectedIndexChanged);
            // 
            // lblCIEMode
            // 
            this.lblCIEMode.AutoSize = true;
            this.lblCIEMode.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCIEMode.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(220)))), ((int)(((byte)(120)))));
            this.lblCIEMode.Location = new System.Drawing.Point(230, 8);
            this.lblCIEMode.Name = "lblCIEMode";
            this.lblCIEMode.Size = new System.Drawing.Size(80, 22);
            this.lblCIEMode.TabIndex = 2;
            this.lblCIEMode.Text = "CIE1931";
            // 
            // lblSelXY
            // 
            this.lblSelXY.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblSelXY.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(210)))), ((int)(((byte)(80)))));
            this.lblSelXY.Location = new System.Drawing.Point(6, 34);
            this.lblSelXY.Name = "lblSelXY";
            this.lblSelXY.Size = new System.Drawing.Size(620, 18);
            this.lblSelXY.TabIndex = 3;
            this.lblSelXY.Text = "색도도를 클릭하면 색상 좌표가 TX 데이터에 자동 반영됩니다";
            // 
            // lblCursor
            // 
            this.lblCursor.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblCursor.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(180)))), ((int)(((byte)(220)))));
            this.lblCursor.Location = new System.Drawing.Point(6, 50);
            this.lblCursor.Name = "lblCursor";
            this.lblCursor.Size = new System.Drawing.Size(400, 16);
            this.lblCursor.TabIndex = 4;
            this.lblCursor.Text = "Cursor: -";
            // 
            // tabImg
            // 
            this.tabImg.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(12)))), ((int)(((byte)(14)))), ((int)(((byte)(22)))));
            this.tabImg.Controls.Add(this.pnlImage);
            this.tabImg.Controls.Add(this.pnlImgBar);
            this.tabImg.Font = new System.Drawing.Font("Consolas", 9F);
            this.tabImg.Location = new System.Drawing.Point(4, 31);
            this.tabImg.Name = "tabImg";
            this.tabImg.Size = new System.Drawing.Size(632, 696);
            this.tabImg.TabIndex = 1;
            this.tabImg.Text = "  이미지 색상 선택  ";
            // 
            // pnlImage
            // 
            this.pnlImage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(15)))), ((int)(((byte)(15)))), ((int)(((byte)(25)))));
            this.pnlImage.Location = new System.Drawing.Point(0, 0);
            this.pnlImage.Name = "pnlImage";
            this.pnlImage.Size = new System.Drawing.Size(632, 628);
            this.pnlImage.TabIndex = 0;
            this.pnlImage.Paint += new System.Windows.Forms.PaintEventHandler(this.pnlImage_Paint);
            this.pnlImage.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pnlImage_MouseClick);
            this.pnlImage.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pnlImage_MouseMove);
            // 
            // pnlImgBar
            // 
            this.pnlImgBar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(16)))), ((int)(((byte)(18)))), ((int)(((byte)(28)))));
            this.pnlImgBar.Controls.Add(this.btnLoadImg);
            this.pnlImgBar.Controls.Add(this.lblImgInfo);
            this.pnlImgBar.Controls.Add(this.lblImgColor);
            this.pnlImgBar.Location = new System.Drawing.Point(0, 628);
            this.pnlImgBar.Name = "pnlImgBar";
            this.pnlImgBar.Size = new System.Drawing.Size(632, 68);
            this.pnlImgBar.TabIndex = 1;
            // 
            // btnLoadImg
            // 
            this.btnLoadImg.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(110)))), ((int)(((byte)(170)))));
            this.btnLoadImg.FlatAppearance.BorderSize = 0;
            this.btnLoadImg.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnLoadImg.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnLoadImg.ForeColor = System.Drawing.Color.White;
            this.btnLoadImg.Location = new System.Drawing.Point(6, 8);
            this.btnLoadImg.Name = "btnLoadImg";
            this.btnLoadImg.Size = new System.Drawing.Size(120, 26);
            this.btnLoadImg.TabIndex = 0;
            this.btnLoadImg.Text = "이미지 로드";
            this.btnLoadImg.UseVisualStyleBackColor = false;
            this.btnLoadImg.Click += new System.EventHandler(this.btnLoadImg_Click);
            // 
            // lblImgInfo
            // 
            this.lblImgInfo.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblImgInfo.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblImgInfo.Location = new System.Drawing.Point(134, 11);
            this.lblImgInfo.Name = "lblImgInfo";
            this.lblImgInfo.Size = new System.Drawing.Size(490, 18);
            this.lblImgInfo.TabIndex = 1;
            this.lblImgInfo.Text = "PNG / JPG / BMP 지원 – 클릭하면 해당 색상이 TX에 자동 반영";
            // 
            // lblImgColor
            // 
            this.lblImgColor.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblImgColor.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(210)))), ((int)(((byte)(80)))));
            this.lblImgColor.Location = new System.Drawing.Point(6, 44);
            this.lblImgColor.Name = "lblImgColor";
            this.lblImgColor.Size = new System.Drawing.Size(620, 18);
            this.lblImgColor.TabIndex = 2;
            // 
            // pnlColorPatch
            // 
            this.pnlColorPatch.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(60)))), ((int)(((byte)(60)))));
            this.pnlColorPatch.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlColorPatch.Location = new System.Drawing.Point(650, 6);
            this.pnlColorPatch.Name = "pnlColorPatch";
            this.pnlColorPatch.Size = new System.Drawing.Size(68, 48);
            this.pnlColorPatch.TabIndex = 10;
            // 
            // lblPatchL
            // 
            this.lblPatchL.AutoSize = true;
            this.lblPatchL.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblPatchL.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(130)))), ((int)(((byte)(150)))), ((int)(((byte)(180)))));
            this.lblPatchL.Location = new System.Drawing.Point(724, 18);
            this.lblPatchL.Name = "lblPatchL";
            this.lblPatchL.Size = new System.Drawing.Size(222, 40);
            this.lblPatchL.TabIndex = 11;
            this.lblPatchL.Text = "선택된 색상 미리보기\n(CIE 또는 이미지 클릭으로 갱신)";
            // 
            // chkOspTxEnable
            // 
            this.chkOspTxEnable.AutoSize = true;
            this.chkOspTxEnable.Checked = true;
            this.chkOspTxEnable.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkOspTxEnable.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkOspTxEnable.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkOspTxEnable.Location = new System.Drawing.Point(1040, 38);
            this.chkOspTxEnable.Name = "chkOspTxEnable";
            this.chkOspTxEnable.Size = new System.Drawing.Size(62, 24);
            this.chkOspTxEnable.TabIndex = 12;
            this.chkOspTxEnable.Text = "OSP";
            this.chkOspTxEnable.UseVisualStyleBackColor = false;
            // 
            // chkIlasTxEnable
            // 
            this.chkIlasTxEnable.AutoSize = true;
            this.chkIlasTxEnable.Checked = true;
            this.chkIlasTxEnable.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkIlasTxEnable.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkIlasTxEnable.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkIlasTxEnable.Location = new System.Drawing.Point(1095, 38);
            this.chkIlasTxEnable.Name = "chkIlasTxEnable";
            this.chkIlasTxEnable.Size = new System.Drawing.Size(71, 24);
            this.chkIlasTxEnable.TabIndex = 13;
            this.chkIlasTxEnable.Text = "ILAS";
            this.chkIlasTxEnable.UseVisualStyleBackColor = false;
            // 
            // chkRxEnable
            // 
            this.chkRxEnable.AutoSize = true;
            this.chkRxEnable.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkRxEnable.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkRxEnable.Location = new System.Drawing.Point(1157, 38);
            this.chkRxEnable.Name = "chkRxEnable";
            this.chkRxEnable.Size = new System.Drawing.Size(53, 24);
            this.chkRxEnable.TabIndex = 14;
            this.chkRxEnable.Text = "RX";
            this.chkRxEnable.UseVisualStyleBackColor = false;
            // 
            // grpFID
            // 
            this.grpFID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpFID.Controls.Add(this.lblFID);
            this.grpFID.Controls.Add(this.cmbFID);
            this.grpFID.Controls.Add(this.txtManualFID);
            this.grpFID.Controls.Add(this.lblCS);
            this.grpFID.Controls.Add(this.cmbCS);
            this.grpFID.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpFID.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpFID.Location = new System.Drawing.Point(658, 62);
            this.grpFID.Name = "grpFID";
            this.grpFID.Size = new System.Drawing.Size(540, 60);
            this.grpFID.TabIndex = 12;
            this.grpFID.TabStop = false;
            this.grpFID.Text = "[ Frame ID / Checksum ]";
            // 
            // lblFID
            // 
            this.lblFID.AutoSize = true;
            this.lblFID.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblFID.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblFID.Location = new System.Drawing.Point(8, 28);
            this.lblFID.Name = "lblFID";
            this.lblFID.Size = new System.Drawing.Size(60, 22);
            this.lblFID.TabIndex = 0;
            this.lblFID.Text = "FID :";
            // 
            // cmbFID
            // 
            this.cmbFID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbFID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbFID.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbFID.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbFID.ForeColor = System.Drawing.Color.White;
            this.cmbFID.Items.AddRange(new object[] {
            "Broadcast (0x00)",
            "MLMM1  (0x01)",
            "MLMM2  (0x02)",
            "MLMM3  (0x03)",
            "MLMM4  (0x04)",
            "MLMM5  (0x05)",
            "MLMM6  (0x06)",
            "MLMM7  (0x07)",
            "MLMM8  (0x08)",
            "Manual"});
            this.cmbFID.Location = new System.Drawing.Point(70, 24);
            this.cmbFID.Name = "cmbFID";
            this.cmbFID.Size = new System.Drawing.Size(160, 30);
            this.cmbFID.TabIndex = 1;
            this.cmbFID.SelectedIndexChanged += new System.EventHandler(this.cmbFID_SelectedIndexChanged);
            // 
            // txtManualFID
            // 
            this.txtManualFID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.txtManualFID.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtManualFID.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtManualFID.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtManualFID.ForeColor = System.Drawing.Color.White;
            this.txtManualFID.Location = new System.Drawing.Point(238, 25);
            this.txtManualFID.Name = "txtManualFID";
            this.txtManualFID.Size = new System.Drawing.Size(56, 29);
            this.txtManualFID.TabIndex = 2;
            this.txtManualFID.Text = "01";
            // 
            // lblCS
            // 
            this.lblCS.AutoSize = true;
            this.lblCS.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblCS.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblCS.Location = new System.Drawing.Point(302, 28);
            this.lblCS.Name = "lblCS";
            this.lblCS.Size = new System.Drawing.Size(40, 22);
            this.lblCS.TabIndex = 3;
            this.lblCS.Text = "CS:";
            // 
            // cmbCS
            // 
            this.cmbCS.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.cmbCS.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbCS.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmbCS.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbCS.ForeColor = System.Drawing.Color.White;
            this.cmbCS.Items.AddRange(new object[] {
            "Classic",
            "Enhanced"});
            this.cmbCS.Location = new System.Drawing.Point(330, 24);
            this.cmbCS.Name = "cmbCS";
            this.cmbCS.Size = new System.Drawing.Size(120, 30);
            this.cmbCS.TabIndex = 4;
            // 
            // grpSF
            // 
            this.grpSF.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpSF.Controls.Add(this.grpShift);
            this.grpSF.Controls.Add(this.chkSF1);
            this.grpSF.Controls.Add(this.chkSF2);
            this.grpSF.Controls.Add(this.chkSF3);
            this.grpSF.Controls.Add(this.chkSF4);
            this.grpSF.Controls.Add(this.chkSF5);
            this.grpSF.Controls.Add(this.chkSF6);
            this.grpSF.Controls.Add(this.chkSF7);
            this.grpSF.Controls.Add(this.chkSF8);
            this.grpSF.Controls.Add(this.chkSF9);
            this.grpSF.Controls.Add(this.chkSF10);
            this.grpSF.Controls.Add(this.chkSF11);
            this.grpSF.Controls.Add(this.chkSF12);
            this.grpSF.Controls.Add(this.chkSF13);
            this.grpSF.Controls.Add(this.chkSF14);
            this.grpSF.Controls.Add(this.chkSF15);
            this.grpSF.Controls.Add(this.chkSF16);
            this.grpSF.Controls.Add(this.chkSF17);
            this.grpSF.Controls.Add(this.chkSF18);
            this.grpSF.Controls.Add(this.chkSF19);
            this.grpSF.Controls.Add(this.chkSF20);
            this.grpSF.Controls.Add(this.chkSF21);
            this.grpSF.Controls.Add(this.chkSF22);
            this.grpSF.Controls.Add(this.chkSF23);
            this.grpSF.Controls.Add(this.chkSF24);
            this.grpSF.Controls.Add(this.chkSF25);
            this.grpSF.Controls.Add(this.btnSFAll);
            this.grpSF.Controls.Add(this.btnSFNone);
            this.grpSF.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpSF.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpSF.Location = new System.Drawing.Point(659, 128);
            this.grpSF.Name = "grpSF";
            this.grpSF.Size = new System.Drawing.Size(540, 158);
            this.grpSF.TabIndex = 13;
            this.grpSF.TabStop = false;
            this.grpSF.Text = "[ DATA1/2  SlaveFlag  (LED 1~12 체크된 LED만 변경) ]";
            // 
            // grpShift
            // 
            this.grpShift.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpShift.Controls.Add(this.chkRGBWShift);
            this.grpShift.Controls.Add(this.chkLedShift);
            this.grpShift.Controls.Add(this.chkShiftStack);
            this.grpShift.Controls.Add(this.chkLedShiftUp);
            this.grpShift.Controls.Add(this.chkShiftStackUp);
            this.grpShift.Controls.Add(this.chkMove);
            this.grpShift.Controls.Add(this.chkEachControl);
            this.grpShift.Controls.Add(this.chkTouch);
            this.grpShift.Controls.Add(this.nudShiftWidth);
            this.grpShift.Controls.Add(this.lblShiftWidth);
            this.grpShift.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpShift.Location = new System.Drawing.Point(11, 96);
            this.grpShift.Name = "grpShift";
            this.grpShift.Size = new System.Drawing.Size(523, 58);
            this.grpShift.TabIndex = 20;
            this.grpShift.TabStop = false;
            this.grpShift.Text = "[ Shift Mode ]";
            // 
            // chkRGBWShift
            // 
            this.chkRGBWShift.AutoSize = true;
            this.chkRGBWShift.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkRGBWShift.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.chkRGBWShift.Location = new System.Drawing.Point(8, 16);
            this.chkRGBWShift.Name = "chkRGBWShift";
            this.chkRGBWShift.Size = new System.Drawing.Size(125, 24);
            this.chkRGBWShift.TabIndex = 17;
            this.chkRGBWShift.Text = "RGBW_Shift";
            this.chkRGBWShift.UseVisualStyleBackColor = false;
            this.chkRGBWShift.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkLedShift
            // 
            this.chkLedShift.AutoSize = true;
            this.chkLedShift.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkLedShift.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.chkLedShift.Location = new System.Drawing.Point(7, 35);
            this.chkLedShift.Name = "chkLedShift";
            this.chkLedShift.Size = new System.Drawing.Size(80, 24);
            this.chkLedShift.TabIndex = 18;
            this.chkLedShift.Text = "Shift";
            this.chkLedShift.UseVisualStyleBackColor = false;
            // 
            // chkShiftStack
            // 
            this.chkShiftStack.AutoSize = true;
            this.chkShiftStack.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkShiftStack.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.chkShiftStack.Location = new System.Drawing.Point(69, 35);
            this.chkShiftStack.Name = "chkShiftStack";
            this.chkShiftStack.Size = new System.Drawing.Size(134, 24);
            this.chkShiftStack.TabIndex = 22;
            this.chkShiftStack.Text = "Shift Stack";
            this.chkShiftStack.UseVisualStyleBackColor = false;
            // 
            // chkLedShiftUp
            // 
            this.chkLedShiftUp.AutoSize = true;
            this.chkLedShiftUp.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkLedShiftUp.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.chkLedShiftUp.Location = new System.Drawing.Point(173, 35);
            this.chkLedShiftUp.Name = "chkLedShiftUp";
            this.chkLedShiftUp.Size = new System.Drawing.Size(107, 24);
            this.chkLedShiftUp.TabIndex = 21;
            this.chkLedShiftUp.Text = "Shift Up";
            this.chkLedShiftUp.UseVisualStyleBackColor = false;
            // 
            // chkShiftStackUp
            // 
            this.chkShiftStackUp.AutoSize = true;
            this.chkShiftStackUp.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkShiftStackUp.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.chkShiftStackUp.Location = new System.Drawing.Point(256, 35);
            this.chkShiftStackUp.Name = "chkShiftStackUp";
            this.chkShiftStackUp.Size = new System.Drawing.Size(161, 24);
            this.chkShiftStackUp.TabIndex = 23;
            this.chkShiftStackUp.Text = "Shift Stack Up";
            this.chkShiftStackUp.UseVisualStyleBackColor = false;
            // 
            // chkMove
            // 
            this.chkMove.AutoSize = true;
            this.chkMove.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkMove.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.chkMove.Location = new System.Drawing.Point(109, 16);
            this.chkMove.Name = "chkMove";
            this.chkMove.Size = new System.Drawing.Size(71, 24);
            this.chkMove.TabIndex = 24;
            this.chkMove.Text = "Move";
            this.chkMove.UseVisualStyleBackColor = false;
            // 
            // chkEachControl
            // 
            this.chkEachControl.AutoSize = true;
            this.chkEachControl.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkEachControl.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(220)))), ((int)(((byte)(120)))));
            this.chkEachControl.Location = new System.Drawing.Point(174, 16);
            this.chkEachControl.Name = "chkEachControl";
            this.chkEachControl.Size = new System.Drawing.Size(143, 24);
            this.chkEachControl.TabIndex = 25;
            this.chkEachControl.Text = "Each Control";
            this.chkEachControl.UseVisualStyleBackColor = false;
            // 
            // chkTouch
            // 
            this.chkTouch.AutoSize = true;
            this.chkTouch.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkTouch.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(240)))), ((int)(((byte)(180)))));
            this.chkTouch.Location = new System.Drawing.Point(320, 16);
            this.chkTouch.Name = "chkTouch";
            this.chkTouch.Size = new System.Drawing.Size(80, 24);
            this.chkTouch.TabIndex = 26;
            this.chkTouch.Text = "Touch";
            this.chkTouch.UseVisualStyleBackColor = false;
            // 
            // nudShiftWidth
            // 
            this.nudShiftWidth.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudShiftWidth.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.nudShiftWidth.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudShiftWidth.ForeColor = System.Drawing.Color.White;
            this.nudShiftWidth.Location = new System.Drawing.Point(469, 31);
            this.nudShiftWidth.Maximum = new decimal(new int[] {
            25,
            0,
            0,
            0});
            this.nudShiftWidth.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudShiftWidth.Name = "nudShiftWidth";
            this.nudShiftWidth.Size = new System.Drawing.Size(48, 27);
            this.nudShiftWidth.TabIndex = 19;
            this.nudShiftWidth.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // lblShiftWidth
            // 
            this.lblShiftWidth.AutoSize = true;
            this.lblShiftWidth.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblShiftWidth.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblShiftWidth.Location = new System.Drawing.Point(424, 35);
            this.lblShiftWidth.Name = "lblShiftWidth";
            this.lblShiftWidth.Size = new System.Drawing.Size(63, 20);
            this.lblShiftWidth.TabIndex = 20;
            this.lblShiftWidth.Text = "Width:";
            // 
            // chkSF1
            // 
            this.chkSF1.AutoSize = true;
            this.chkSF1.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF1.Location = new System.Drawing.Point(8, 28);
            this.chkSF1.Name = "chkSF1";
            this.chkSF1.Size = new System.Drawing.Size(62, 24);
            this.chkSF1.TabIndex = 0;
            this.chkSF1.Text = "SF1";
            this.chkSF1.UseVisualStyleBackColor = false;
            this.chkSF1.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF2
            // 
            this.chkSF2.AutoSize = true;
            this.chkSF2.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF2.Location = new System.Drawing.Point(64, 28);
            this.chkSF2.Name = "chkSF2";
            this.chkSF2.Size = new System.Drawing.Size(62, 24);
            this.chkSF2.TabIndex = 1;
            this.chkSF2.Text = "SF2";
            this.chkSF2.UseVisualStyleBackColor = false;
            this.chkSF2.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF3
            // 
            this.chkSF3.AutoSize = true;
            this.chkSF3.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF3.Location = new System.Drawing.Point(120, 28);
            this.chkSF3.Name = "chkSF3";
            this.chkSF3.Size = new System.Drawing.Size(62, 24);
            this.chkSF3.TabIndex = 2;
            this.chkSF3.Text = "SF3";
            this.chkSF3.UseVisualStyleBackColor = false;
            this.chkSF3.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF4
            // 
            this.chkSF4.AutoSize = true;
            this.chkSF4.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF4.Location = new System.Drawing.Point(176, 28);
            this.chkSF4.Name = "chkSF4";
            this.chkSF4.Size = new System.Drawing.Size(62, 24);
            this.chkSF4.TabIndex = 3;
            this.chkSF4.Text = "SF4";
            this.chkSF4.UseVisualStyleBackColor = false;
            this.chkSF4.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF5
            // 
            this.chkSF5.AutoSize = true;
            this.chkSF5.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF5.Location = new System.Drawing.Point(232, 28);
            this.chkSF5.Name = "chkSF5";
            this.chkSF5.Size = new System.Drawing.Size(62, 24);
            this.chkSF5.TabIndex = 4;
            this.chkSF5.Text = "SF5";
            this.chkSF5.UseVisualStyleBackColor = false;
            this.chkSF5.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF6
            // 
            this.chkSF6.AutoSize = true;
            this.chkSF6.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF6.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF6.Location = new System.Drawing.Point(288, 28);
            this.chkSF6.Name = "chkSF6";
            this.chkSF6.Size = new System.Drawing.Size(62, 24);
            this.chkSF6.TabIndex = 5;
            this.chkSF6.Text = "SF6";
            this.chkSF6.UseVisualStyleBackColor = false;
            this.chkSF6.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF7
            // 
            this.chkSF7.AutoSize = true;
            this.chkSF7.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF7.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF7.Location = new System.Drawing.Point(344, 28);
            this.chkSF7.Name = "chkSF7";
            this.chkSF7.Size = new System.Drawing.Size(62, 24);
            this.chkSF7.TabIndex = 6;
            this.chkSF7.Text = "SF7";
            this.chkSF7.UseVisualStyleBackColor = false;
            this.chkSF7.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF8
            // 
            this.chkSF8.AutoSize = true;
            this.chkSF8.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF8.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF8.Location = new System.Drawing.Point(400, 28);
            this.chkSF8.Name = "chkSF8";
            this.chkSF8.Size = new System.Drawing.Size(62, 24);
            this.chkSF8.TabIndex = 7;
            this.chkSF8.Text = "SF8";
            this.chkSF8.UseVisualStyleBackColor = false;
            this.chkSF8.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF9
            // 
            this.chkSF9.AutoSize = true;
            this.chkSF9.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF9.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF9.Location = new System.Drawing.Point(8, 52);
            this.chkSF9.Name = "chkSF9";
            this.chkSF9.Size = new System.Drawing.Size(62, 24);
            this.chkSF9.TabIndex = 10;
            this.chkSF9.Text = "SF9";
            this.chkSF9.UseVisualStyleBackColor = false;
            this.chkSF9.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF10
            // 
            this.chkSF10.AutoSize = true;
            this.chkSF10.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF10.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF10.Location = new System.Drawing.Point(64, 52);
            this.chkSF10.Name = "chkSF10";
            this.chkSF10.Size = new System.Drawing.Size(71, 24);
            this.chkSF10.TabIndex = 11;
            this.chkSF10.Text = "SF10";
            this.chkSF10.UseVisualStyleBackColor = false;
            this.chkSF10.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF11
            // 
            this.chkSF11.AutoSize = true;
            this.chkSF11.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF11.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF11.Location = new System.Drawing.Point(120, 52);
            this.chkSF11.Name = "chkSF11";
            this.chkSF11.Size = new System.Drawing.Size(71, 24);
            this.chkSF11.TabIndex = 12;
            this.chkSF11.Text = "SF11";
            this.chkSF11.UseVisualStyleBackColor = false;
            this.chkSF11.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF12
            // 
            this.chkSF12.AutoSize = true;
            this.chkSF12.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF12.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF12.Location = new System.Drawing.Point(176, 52);
            this.chkSF12.Name = "chkSF12";
            this.chkSF12.Size = new System.Drawing.Size(71, 24);
            this.chkSF12.TabIndex = 13;
            this.chkSF12.Text = "SF12";
            this.chkSF12.UseVisualStyleBackColor = false;
            this.chkSF12.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF13
            // 
            this.chkSF13.AutoSize = true;
            this.chkSF13.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF13.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF13.Location = new System.Drawing.Point(232, 52);
            this.chkSF13.Name = "chkSF13";
            this.chkSF13.Size = new System.Drawing.Size(71, 24);
            this.chkSF13.TabIndex = 43;
            this.chkSF13.Text = "SF13";
            this.chkSF13.UseVisualStyleBackColor = false;
            this.chkSF13.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF14
            // 
            this.chkSF14.AutoSize = true;
            this.chkSF14.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF14.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF14.Location = new System.Drawing.Point(288, 52);
            this.chkSF14.Name = "chkSF14";
            this.chkSF14.Size = new System.Drawing.Size(71, 24);
            this.chkSF14.TabIndex = 44;
            this.chkSF14.Text = "SF14";
            this.chkSF14.UseVisualStyleBackColor = false;
            this.chkSF14.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF15
            // 
            this.chkSF15.AutoSize = true;
            this.chkSF15.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF15.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF15.Location = new System.Drawing.Point(344, 52);
            this.chkSF15.Name = "chkSF15";
            this.chkSF15.Size = new System.Drawing.Size(71, 24);
            this.chkSF15.TabIndex = 45;
            this.chkSF15.Text = "SF15";
            this.chkSF15.UseVisualStyleBackColor = false;
            this.chkSF15.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF16
            // 
            this.chkSF16.AutoSize = true;
            this.chkSF16.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF16.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF16.Location = new System.Drawing.Point(400, 52);
            this.chkSF16.Name = "chkSF16";
            this.chkSF16.Size = new System.Drawing.Size(71, 24);
            this.chkSF16.TabIndex = 46;
            this.chkSF16.Text = "SF16";
            this.chkSF16.UseVisualStyleBackColor = false;
            this.chkSF16.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF17
            // 
            this.chkSF17.AutoSize = true;
            this.chkSF17.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF17.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF17.Location = new System.Drawing.Point(8, 76);
            this.chkSF17.Name = "chkSF17";
            this.chkSF17.Size = new System.Drawing.Size(71, 24);
            this.chkSF17.TabIndex = 47;
            this.chkSF17.Text = "SF17";
            this.chkSF17.UseVisualStyleBackColor = false;
            this.chkSF17.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF18
            // 
            this.chkSF18.AutoSize = true;
            this.chkSF18.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF18.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF18.Location = new System.Drawing.Point(64, 76);
            this.chkSF18.Name = "chkSF18";
            this.chkSF18.Size = new System.Drawing.Size(71, 24);
            this.chkSF18.TabIndex = 48;
            this.chkSF18.Text = "SF18";
            this.chkSF18.UseVisualStyleBackColor = false;
            this.chkSF18.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF19
            // 
            this.chkSF19.AutoSize = true;
            this.chkSF19.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF19.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF19.Location = new System.Drawing.Point(120, 76);
            this.chkSF19.Name = "chkSF19";
            this.chkSF19.Size = new System.Drawing.Size(71, 24);
            this.chkSF19.TabIndex = 49;
            this.chkSF19.Text = "SF19";
            this.chkSF19.UseVisualStyleBackColor = false;
            this.chkSF19.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF20
            // 
            this.chkSF20.AutoSize = true;
            this.chkSF20.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF20.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF20.Location = new System.Drawing.Point(176, 76);
            this.chkSF20.Name = "chkSF20";
            this.chkSF20.Size = new System.Drawing.Size(71, 24);
            this.chkSF20.TabIndex = 50;
            this.chkSF20.Text = "SF20";
            this.chkSF20.UseVisualStyleBackColor = false;
            this.chkSF20.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF21
            // 
            this.chkSF21.AutoSize = true;
            this.chkSF21.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF21.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF21.Location = new System.Drawing.Point(232, 76);
            this.chkSF21.Name = "chkSF21";
            this.chkSF21.Size = new System.Drawing.Size(71, 24);
            this.chkSF21.TabIndex = 51;
            this.chkSF21.Text = "SF21";
            this.chkSF21.UseVisualStyleBackColor = false;
            this.chkSF21.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF22
            // 
            this.chkSF22.AutoSize = true;
            this.chkSF22.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF22.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF22.Location = new System.Drawing.Point(288, 76);
            this.chkSF22.Name = "chkSF22";
            this.chkSF22.Size = new System.Drawing.Size(71, 24);
            this.chkSF22.TabIndex = 52;
            this.chkSF22.Text = "SF22";
            this.chkSF22.UseVisualStyleBackColor = false;
            this.chkSF22.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF23
            // 
            this.chkSF23.AutoSize = true;
            this.chkSF23.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF23.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF23.Location = new System.Drawing.Point(344, 76);
            this.chkSF23.Name = "chkSF23";
            this.chkSF23.Size = new System.Drawing.Size(71, 24);
            this.chkSF23.TabIndex = 53;
            this.chkSF23.Text = "SF23";
            this.chkSF23.UseVisualStyleBackColor = false;
            this.chkSF23.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF24
            // 
            this.chkSF24.AutoSize = true;
            this.chkSF24.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF24.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF24.Location = new System.Drawing.Point(400, 76);
            this.chkSF24.Name = "chkSF24";
            this.chkSF24.Size = new System.Drawing.Size(71, 24);
            this.chkSF24.TabIndex = 54;
            this.chkSF24.Text = "SF24";
            this.chkSF24.UseVisualStyleBackColor = false;
            this.chkSF24.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkSF25
            // 
            this.chkSF25.AutoSize = true;
            this.chkSF25.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF25.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF25.Location = new System.Drawing.Point(458, 76);
            this.chkSF25.Name = "chkSF25";
            this.chkSF25.Size = new System.Drawing.Size(71, 24);
            this.chkSF25.TabIndex = 55;
            this.chkSF25.Text = "SF25";
            this.chkSF25.UseVisualStyleBackColor = false;
            this.chkSF25.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // btnSFAll
            // 
            this.btnSFAll.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(100)))), ((int)(((byte)(60)))));
            this.btnSFAll.FlatAppearance.BorderSize = 0;
            this.btnSFAll.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSFAll.Font = new System.Drawing.Font("Consolas", 8F);
            this.btnSFAll.ForeColor = System.Drawing.Color.White;
            this.btnSFAll.Location = new System.Drawing.Point(458, 24);
            this.btnSFAll.Name = "btnSFAll";
            this.btnSFAll.Size = new System.Drawing.Size(36, 22);
            this.btnSFAll.TabIndex = 14;
            this.btnSFAll.Text = "ALL";
            this.btnSFAll.UseVisualStyleBackColor = false;
            this.btnSFAll.Click += new System.EventHandler(this.btnSFAll_Click);
            // 
            // btnSFNone
            // 
            this.btnSFNone.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(40)))), ((int)(((byte)(40)))));
            this.btnSFNone.FlatAppearance.BorderSize = 0;
            this.btnSFNone.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSFNone.Font = new System.Drawing.Font("Consolas", 8F);
            this.btnSFNone.ForeColor = System.Drawing.Color.White;
            this.btnSFNone.Location = new System.Drawing.Point(498, 24);
            this.btnSFNone.Name = "btnSFNone";
            this.btnSFNone.Size = new System.Drawing.Size(36, 22);
            this.btnSFNone.TabIndex = 15;
            this.btnSFNone.Text = "CLR";
            this.btnSFNone.UseVisualStyleBackColor = false;
            this.btnSFNone.Click += new System.EventHandler(this.btnSFNone_Click);
            // 
            // grpColor
            // 
            this.grpColor.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpColor.Controls.Add(this.lblCX);
            this.grpColor.Controls.Add(this.nudColorX);
            this.grpColor.Controls.Add(this.lblCY);
            this.grpColor.Controls.Add(this.nudColorY);
            this.grpColor.Controls.Add(this.lblBr);
            this.grpColor.Controls.Add(this.nudBrightness);
            this.grpColor.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpColor.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpColor.Location = new System.Drawing.Point(658, 288);
            this.grpColor.Name = "grpColor";
            this.grpColor.Size = new System.Drawing.Size(540, 47);
            this.grpColor.TabIndex = 14;
            this.grpColor.TabStop = false;
            this.grpColor.Text = "[ DATA2/3/4  ColorX / ColorY / Brightness  (0x00~0xFA = 0~100%) ]";
            // 
            // lblCX
            // 
            this.lblCX.AutoSize = true;
            this.lblCX.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblCX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblCX.Location = new System.Drawing.Point(9, 18);
            this.lblCX.Name = "lblCX";
            this.lblCX.Size = new System.Drawing.Size(110, 22);
            this.lblCX.TabIndex = 0;
            this.lblCX.Text = "D2 ColorX:";
            // 
            // nudColorX
            // 
            this.nudColorX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudColorX.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudColorX.ForeColor = System.Drawing.Color.White;
            this.nudColorX.Hexadecimal = true;
            this.nudColorX.Location = new System.Drawing.Point(89, 15);
            this.nudColorX.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudColorX.Name = "nudColorX";
            this.nudColorX.Size = new System.Drawing.Size(64, 29);
            this.nudColorX.TabIndex = 1;
            this.nudColorX.ValueChanged += new System.EventHandler(this.nudColorX_ValueChanged);
            // 
            // lblCY
            // 
            this.lblCY.AutoSize = true;
            this.lblCY.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblCY.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblCY.Location = new System.Drawing.Point(163, 18);
            this.lblCY.Name = "lblCY";
            this.lblCY.Size = new System.Drawing.Size(110, 22);
            this.lblCY.TabIndex = 2;
            this.lblCY.Text = "D3 ColorY:";
            // 
            // nudColorY
            // 
            this.nudColorY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudColorY.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudColorY.ForeColor = System.Drawing.Color.White;
            this.nudColorY.Hexadecimal = true;
            this.nudColorY.Location = new System.Drawing.Point(243, 15);
            this.nudColorY.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudColorY.Name = "nudColorY";
            this.nudColorY.Size = new System.Drawing.Size(64, 29);
            this.nudColorY.TabIndex = 3;
            this.nudColorY.ValueChanged += new System.EventHandler(this.nudColorY_ValueChanged);
            // 
            // lblBr
            // 
            this.lblBr.AutoSize = true;
            this.lblBr.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblBr.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblBr.Location = new System.Drawing.Point(317, 18);
            this.lblBr.Name = "lblBr";
            this.lblBr.Size = new System.Drawing.Size(110, 22);
            this.lblBr.TabIndex = 4;
            this.lblBr.Text = "D4 Bright:";
            // 
            // nudBrightness
            // 
            this.nudBrightness.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudBrightness.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudBrightness.ForeColor = System.Drawing.Color.White;
            this.nudBrightness.Hexadecimal = true;
            this.nudBrightness.Location = new System.Drawing.Point(431, 15);
            this.nudBrightness.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudBrightness.Name = "nudBrightness";
            this.nudBrightness.Size = new System.Drawing.Size(64, 29);
            this.nudBrightness.TabIndex = 5;
            this.nudBrightness.ValueChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // grpFade
            // 
            this.grpFade.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpFade.Controls.Add(this.lblFIN);
            this.grpFade.Controls.Add(this.nudFadeIn);
            this.grpFade.Controls.Add(this.lblFOUT);
            this.grpFade.Controls.Add(this.nudFadeOut);
            this.grpFade.Controls.Add(this.chkNoFade);
            this.grpFade.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpFade.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpFade.Location = new System.Drawing.Point(658, 341);
            this.grpFade.Name = "grpFade";
            this.grpFade.Size = new System.Drawing.Size(540, 62);
            this.grpFade.TabIndex = 15;
            this.grpFade.TabStop = false;
            this.grpFade.Text = "[ DATA5/6/7  FadeInTime / FadeOutTime / NoFadeFlag (×10ms) ]";
            // 
            // lblFIN
            // 
            this.lblFIN.AutoSize = true;
            this.lblFIN.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblFIN.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblFIN.Location = new System.Drawing.Point(8, 28);
            this.lblFIN.Name = "lblFIN";
            this.lblFIN.Size = new System.Drawing.Size(110, 22);
            this.lblFIN.TabIndex = 0;
            this.lblFIN.Text = "D5 FadeIn:";
            // 
            // nudFadeIn
            // 
            this.nudFadeIn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudFadeIn.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudFadeIn.ForeColor = System.Drawing.Color.White;
            this.nudFadeIn.Location = new System.Drawing.Point(88, 25);
            this.nudFadeIn.Maximum = new decimal(new int[] {
            254,
            0,
            0,
            0});
            this.nudFadeIn.Name = "nudFadeIn";
            this.nudFadeIn.Size = new System.Drawing.Size(64, 29);
            this.nudFadeIn.TabIndex = 1;
            this.nudFadeIn.ValueChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // lblFOUT
            // 
            this.lblFOUT.AutoSize = true;
            this.lblFOUT.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblFOUT.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblFOUT.Location = new System.Drawing.Point(162, 28);
            this.lblFOUT.Name = "lblFOUT";
            this.lblFOUT.Size = new System.Drawing.Size(120, 22);
            this.lblFOUT.TabIndex = 2;
            this.lblFOUT.Text = "D6 FadeOut:";
            // 
            // nudFadeOut
            // 
            this.nudFadeOut.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudFadeOut.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudFadeOut.ForeColor = System.Drawing.Color.White;
            this.nudFadeOut.Location = new System.Drawing.Point(248, 25);
            this.nudFadeOut.Maximum = new decimal(new int[] {
            254,
            0,
            0,
            0});
            this.nudFadeOut.Name = "nudFadeOut";
            this.nudFadeOut.Size = new System.Drawing.Size(64, 29);
            this.nudFadeOut.TabIndex = 3;
            this.nudFadeOut.ValueChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkNoFade
            // 
            this.chkNoFade.AutoSize = true;
            this.chkNoFade.Font = new System.Drawing.Font("Consolas", 9F);
            this.chkNoFade.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkNoFade.Location = new System.Drawing.Point(326, 28);
            this.chkNoFade.Name = "chkNoFade";
            this.chkNoFade.Size = new System.Drawing.Size(266, 26);
            this.chkNoFade.TabIndex = 4;
            this.chkNoFade.Text = "D7 bit7 NoFadeInOutFlag";
            this.chkNoFade.UseVisualStyleBackColor = false;
            this.chkNoFade.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // grpStatus
            // 
            this.grpStatus.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpStatus.Controls.Add(this.chkSOH);
            this.grpStatus.Controls.Add(this.chkLINErr);
            this.grpStatus.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpStatus.Location = new System.Drawing.Point(658, 409);
            this.grpStatus.Name = "grpStatus";
            this.grpStatus.Size = new System.Drawing.Size(540, 50);
            this.grpStatus.TabIndex = 16;
            this.grpStatus.TabStop = false;
            this.grpStatus.Text = "[ DATA8  LIN_MLMMnSOH / LIN_MLMMnLINError ]";
            // 
            // chkSOH
            // 
            this.chkSOH.AutoSize = true;
            this.chkSOH.Font = new System.Drawing.Font("Consolas", 9F);
            this.chkSOH.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(240)))), ((int)(((byte)(140)))));
            this.chkSOH.Location = new System.Drawing.Point(8, 24);
            this.chkSOH.Name = "chkSOH";
            this.chkSOH.Size = new System.Drawing.Size(216, 26);
            this.chkSOH.TabIndex = 0;
            this.chkSOH.Text = "bit0 SOH=OK (0x01)";
            this.chkSOH.UseVisualStyleBackColor = false;
            this.chkSOH.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // chkLINErr
            // 
            this.chkLINErr.AutoSize = true;
            this.chkLINErr.Font = new System.Drawing.Font("Consolas", 9F);
            this.chkLINErr.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(120)))), ((int)(((byte)(100)))));
            this.chkLINErr.Location = new System.Drawing.Point(230, 24);
            this.chkLINErr.Name = "chkLINErr";
            this.chkLINErr.Size = new System.Drawing.Size(186, 26);
            this.chkLINErr.TabIndex = 1;
            this.chkLINErr.Text = "bit7 LINError=1";
            this.chkLINErr.UseVisualStyleBackColor = false;
            this.chkLINErr.CheckedChanged += new System.EventHandler(this.AnyField_Changed);
            // 
            // grpPreview
            // 
            this.grpPreview.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(14)))), ((int)(((byte)(16)))), ((int)(((byte)(26)))));
            this.grpPreview.Controls.Add(this.txtPreview);
            this.grpPreview.Controls.Add(this.lblD1);
            this.grpPreview.Controls.Add(this.lblD2);
            this.grpPreview.Controls.Add(this.lblD3);
            this.grpPreview.Controls.Add(this.lblD4);
            this.grpPreview.Controls.Add(this.lblD5);
            this.grpPreview.Controls.Add(this.lblD6);
            this.grpPreview.Controls.Add(this.lblD7);
            this.grpPreview.Controls.Add(this.lblD8);
            this.grpPreview.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpPreview.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpPreview.Location = new System.Drawing.Point(658, 465);
            this.grpPreview.Name = "grpPreview";
            this.grpPreview.Size = new System.Drawing.Size(540, 144);
            this.grpPreview.TabIndex = 17;
            this.grpPreview.TabStop = false;
            this.grpPreview.Text = "[ 패킷 미리보기 (8 Bytes) ]";
            // 
            // txtPreview
            // 
            this.txtPreview.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(10)))), ((int)(((byte)(14)))), ((int)(((byte)(22)))));
            this.txtPreview.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtPreview.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.txtPreview.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(240)))), ((int)(((byte)(130)))));
            this.txtPreview.Location = new System.Drawing.Point(8, 22);
            this.txtPreview.Name = "txtPreview";
            this.txtPreview.ReadOnly = true;
            this.txtPreview.Size = new System.Drawing.Size(522, 24);
            this.txtPreview.TabIndex = 0;
            // 
            // lblD1
            // 
            this.lblD1.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD1.Location = new System.Drawing.Point(8, 48);
            this.lblD1.Name = "lblD1";
            this.lblD1.Size = new System.Drawing.Size(258, 18);
            this.lblD1.TabIndex = 1;
            this.lblD1.Text = "D1=";
            // 
            // lblD2
            // 
            this.lblD2.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD2.Location = new System.Drawing.Point(8, 68);
            this.lblD2.Name = "lblD2";
            this.lblD2.Size = new System.Drawing.Size(258, 18);
            this.lblD2.TabIndex = 2;
            this.lblD2.Text = "D2=";
            // 
            // lblD3
            // 
            this.lblD3.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD3.Location = new System.Drawing.Point(8, 88);
            this.lblD3.Name = "lblD3";
            this.lblD3.Size = new System.Drawing.Size(258, 18);
            this.lblD3.TabIndex = 3;
            this.lblD3.Text = "D3=";
            // 
            // lblD4
            // 
            this.lblD4.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD4.Location = new System.Drawing.Point(8, 108);
            this.lblD4.Name = "lblD4";
            this.lblD4.Size = new System.Drawing.Size(258, 18);
            this.lblD4.TabIndex = 4;
            this.lblD4.Text = "D4=";
            // 
            // lblD5
            // 
            this.lblD5.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD5.Location = new System.Drawing.Point(274, 48);
            this.lblD5.Name = "lblD5";
            this.lblD5.Size = new System.Drawing.Size(258, 18);
            this.lblD5.TabIndex = 5;
            this.lblD5.Text = "D5=";
            // 
            // lblD6
            // 
            this.lblD6.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD6.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD6.Location = new System.Drawing.Point(274, 68);
            this.lblD6.Name = "lblD6";
            this.lblD6.Size = new System.Drawing.Size(258, 18);
            this.lblD6.TabIndex = 6;
            this.lblD6.Text = "D6=";
            // 
            // lblD7
            // 
            this.lblD7.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD7.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD7.Location = new System.Drawing.Point(274, 88);
            this.lblD7.Name = "lblD7";
            this.lblD7.Size = new System.Drawing.Size(258, 18);
            this.lblD7.TabIndex = 7;
            this.lblD7.Text = "D7=";
            // 
            // lblD8
            // 
            this.lblD8.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblD8.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(185)))), ((int)(((byte)(210)))));
            this.lblD8.Location = new System.Drawing.Point(274, 108);
            this.lblD8.Name = "lblD8";
            this.lblD8.Size = new System.Drawing.Size(258, 18);
            this.lblD8.TabIndex = 8;
            this.lblD8.Text = "D8=";
            // 
            // grpTouch
            // 
            this.grpTouch.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpTouch.Controls.Add(this.lblTouch);
            this.grpTouch.Controls.Add(this.touchTrackBar);
            this.grpTouch.Controls.Add(this.lblTouchValue);
            this.grpTouch.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpTouch.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(240)))), ((int)(((byte)(180)))));
            this.grpTouch.Location = new System.Drawing.Point(659, 615);
            this.grpTouch.Name = "grpTouch";
            this.grpTouch.Size = new System.Drawing.Size(540, 58);
            this.grpTouch.TabIndex = 18;
            this.grpTouch.TabStop = false;
            this.grpTouch.Text = "[ Touch Drag - 즉시 TX ]";
            // 
            // lblTouch
            // 
            this.lblTouch.AutoSize = true;
            this.lblTouch.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblTouch.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(210)))), ((int)(((byte)(220)))));
            this.lblTouch.Location = new System.Drawing.Point(8, 23);
            this.lblTouch.Name = "lblTouch";
            this.lblTouch.Size = new System.Drawing.Size(70, 22);
            this.lblTouch.TabIndex = 0;
            this.lblTouch.Text = "Touch:";
            // 
            // touchTrackBar
            // 
            this.touchTrackBar.Enabled = false;
            this.touchTrackBar.Location = new System.Drawing.Point(72, 12);
            this.touchTrackBar.Maximum = 100;
            this.touchTrackBar.Name = "touchTrackBar";
            this.touchTrackBar.Size = new System.Drawing.Size(390, 69);
            this.touchTrackBar.TabIndex = 1;
            this.touchTrackBar.TickFrequency = 5;
            this.touchTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
            this.touchTrackBar.Scroll += new System.EventHandler(this.touchTrackBar_Scroll);
            // 
            // lblTouchValue
            // 
            this.lblTouchValue.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblTouchValue.ForeColor = System.Drawing.Color.White;
            this.lblTouchValue.Location = new System.Drawing.Point(464, 23);
            this.lblTouchValue.Name = "lblTouchValue";
            this.lblTouchValue.Size = new System.Drawing.Size(68, 22);
            this.lblTouchValue.TabIndex = 2;
            this.lblTouchValue.Text = "0%";
            this.lblTouchValue.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // grpTX
            // 
            this.grpTX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpTX.Controls.Add(this.lblIvL);
            this.grpTX.Controls.Add(this.nudInterval);
            this.grpTX.Controls.Add(this.lblMs);
            this.grpTX.Controls.Add(this.btnTxOnce);
            this.grpTX.Controls.Add(this.btnTxCycle);
            this.grpTX.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpTX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpTX.Location = new System.Drawing.Point(659, 679);
            this.grpTX.Name = "grpTX";
            this.grpTX.Size = new System.Drawing.Size(540, 64);
            this.grpTX.TabIndex = 18;
            this.grpTX.TabStop = false;
            this.grpTX.Text = "[ TX ]";
            // 
            // lblIvL
            // 
            this.lblIvL.AutoSize = true;
            this.lblIvL.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblIvL.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblIvL.Location = new System.Drawing.Point(8, 21);
            this.lblIvL.Name = "lblIvL";
            this.lblIvL.Size = new System.Drawing.Size(90, 22);
            this.lblIvL.TabIndex = 0;
            this.lblIvL.Text = "주기(ms):";
            // 
            // nudInterval
            // 
            this.nudInterval.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudInterval.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudInterval.ForeColor = System.Drawing.Color.White;
            this.nudInterval.Location = new System.Drawing.Point(78, 18);
            this.nudInterval.Maximum = new decimal(new int[] {
            60000,
            0,
            0,
            0});
            this.nudInterval.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudInterval.Name = "nudInterval";
            this.nudInterval.Size = new System.Drawing.Size(80, 29);
            this.nudInterval.TabIndex = 1;
            this.nudInterval.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // lblMs
            // 
            this.lblMs.AutoSize = true;
            this.lblMs.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblMs.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(155)))), ((int)(((byte)(175)))));
            this.lblMs.Location = new System.Drawing.Point(162, 21);
            this.lblMs.Name = "lblMs";
            this.lblMs.Size = new System.Drawing.Size(30, 22);
            this.lblMs.TabIndex = 2;
            this.lblMs.Text = "ms";
            // 
            // btnTxOnce
            // 
            this.btnTxOnce.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(120)))), ((int)(((byte)(200)))));
            this.btnTxOnce.FlatAppearance.BorderSize = 0;
            this.btnTxOnce.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnTxOnce.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnTxOnce.ForeColor = System.Drawing.Color.White;
            this.btnTxOnce.Location = new System.Drawing.Point(194, 13);
            this.btnTxOnce.Name = "btnTxOnce";
            this.btnTxOnce.Size = new System.Drawing.Size(100, 36);
            this.btnTxOnce.TabIndex = 3;
            this.btnTxOnce.Text = "1회 TX";
            this.btnTxOnce.UseVisualStyleBackColor = false;
            this.btnTxOnce.Click += new System.EventHandler(this.btnTxOnce_Click);
            // 
            // btnTxCycle
            // 
            this.btnTxCycle.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(110)))), ((int)(((byte)(190)))));
            this.btnTxCycle.FlatAppearance.BorderSize = 0;
            this.btnTxCycle.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnTxCycle.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnTxCycle.ForeColor = System.Drawing.Color.White;
            this.btnTxCycle.Location = new System.Drawing.Point(300, 13);
            this.btnTxCycle.Name = "btnTxCycle";
            this.btnTxCycle.Size = new System.Drawing.Size(234, 36);
            this.btnTxCycle.TabIndex = 4;
            this.btnTxCycle.Text = "▶ 주기 TX 시작";
            this.btnTxCycle.UseVisualStyleBackColor = false;
            this.btnTxCycle.Click += new System.EventHandler(this.btnTxCycle_Click);
            // 
            // sunshinePanel
            // 
            this.sunshinePanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(18)))), ((int)(((byte)(21)))), ((int)(((byte)(31)))));
            this.sunshinePanel.Controls.Add(this.sunSelectionGroupBox);
            this.sunshinePanel.Controls.Add(this.sunLedSelectionGroupBox);
            this.sunshinePanel.Controls.Add(this.sunGroupsGroupBox);
            this.sunshinePanel.Controls.Add(this.sunReadGroupBox);
            this.sunshinePanel.Location = new System.Drawing.Point(1225, 0);
            this.sunshinePanel.Name = "sunshinePanel";
            this.sunshinePanel.Size = new System.Drawing.Size(1045, 743);
            this.sunshinePanel.TabIndex = 20;
            // 
            // sunSelectionGroupBox
            // 
            this.sunSelectionGroupBox.Controls.Add(this.sunTargetLabel);
            this.sunSelectionGroupBox.Controls.Add(this.sunTargetComboBox);
            this.sunSelectionGroupBox.Controls.Add(this.sunAllRadioButton);
            this.sunSelectionGroupBox.Controls.Add(this.sunIndividualRadioButton);
            this.sunSelectionGroupBox.Controls.Add(this.sunGroupRadioButton);
            this.sunSelectionGroupBox.Controls.Add(this.sunIndividualLabel);
            this.sunSelectionGroupBox.Controls.Add(this.sunIndividualTextBox);
            this.sunSelectionGroupBox.Controls.Add(this.sunApplyButton);
            this.sunSelectionGroupBox.Controls.Add(this.sunTuningCheckBox);
            this.sunSelectionGroupBox.Controls.Add(this.sunTemperatureCheckBox);
            this.sunSelectionGroupBox.Controls.Add(this.sunOspCurrentModeGroupBox);
            this.sunSelectionGroupBox.Controls.Add(this.sunHintLabel);
            this.sunSelectionGroupBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.sunSelectionGroupBox.Location = new System.Drawing.Point(5, 5);
            this.sunSelectionGroupBox.Name = "sunSelectionGroupBox";
            this.sunSelectionGroupBox.Size = new System.Drawing.Size(1035, 92);
            this.sunSelectionGroupBox.TabIndex = 0;
            this.sunSelectionGroupBox.TabStop = false;
            this.sunSelectionGroupBox.Text = "[ Sunshine LED 제어 범위 ]";
            // 
            // sunTargetLabel
            // 
            this.sunTargetLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(205)))), ((int)(((byte)(225)))));
            this.sunTargetLabel.Location = new System.Drawing.Point(10, 22);
            this.sunTargetLabel.Name = "sunTargetLabel";
            this.sunTargetLabel.Size = new System.Drawing.Size(45, 22);
            this.sunTargetLabel.TabIndex = 0;
            this.sunTargetLabel.Text = "대상";
            this.sunTargetLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // sunTargetComboBox
            // 
            this.sunTargetComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.sunTargetComboBox.Items.AddRange(new object[] {
            "OSP / FIO (25)",
            "ISELED / ILAS (84)"});
            this.sunTargetComboBox.Location = new System.Drawing.Point(55, 22);
            this.sunTargetComboBox.Name = "sunTargetComboBox";
            this.sunTargetComboBox.Size = new System.Drawing.Size(155, 30);
            this.sunTargetComboBox.TabIndex = 1;
            // 
            // sunAllRadioButton
            // 
            this.sunAllRadioButton.Checked = true;
            this.sunAllRadioButton.ForeColor = System.Drawing.Color.White;
            this.sunAllRadioButton.Location = new System.Drawing.Point(230, 23);
            this.sunAllRadioButton.Name = "sunAllRadioButton";
            this.sunAllRadioButton.Size = new System.Drawing.Size(70, 25);
            this.sunAllRadioButton.TabIndex = 2;
            this.sunAllRadioButton.TabStop = true;
            this.sunAllRadioButton.Text = "전체";
            this.sunAllRadioButton.UseVisualStyleBackColor = true;
            // 
            // sunIndividualRadioButton
            // 
            this.sunIndividualRadioButton.ForeColor = System.Drawing.Color.White;
            this.sunIndividualRadioButton.Location = new System.Drawing.Point(310, 23);
            this.sunIndividualRadioButton.Name = "sunIndividualRadioButton";
            this.sunIndividualRadioButton.Size = new System.Drawing.Size(70, 25);
            this.sunIndividualRadioButton.TabIndex = 3;
            this.sunIndividualRadioButton.Text = "개별";
            this.sunIndividualRadioButton.UseVisualStyleBackColor = true;
            // 
            // sunGroupRadioButton
            // 
            this.sunGroupRadioButton.ForeColor = System.Drawing.Color.White;
            this.sunGroupRadioButton.Location = new System.Drawing.Point(390, 23);
            this.sunGroupRadioButton.Name = "sunGroupRadioButton";
            this.sunGroupRadioButton.Size = new System.Drawing.Size(70, 25);
            this.sunGroupRadioButton.TabIndex = 4;
            this.sunGroupRadioButton.Text = "그룹";
            this.sunGroupRadioButton.UseVisualStyleBackColor = true;
            // 
            // sunIndividualLabel
            // 
            this.sunIndividualLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(205)))), ((int)(((byte)(225)))));
            this.sunIndividualLabel.Location = new System.Drawing.Point(10, 56);
            this.sunIndividualLabel.Name = "sunIndividualLabel";
            this.sunIndividualLabel.Size = new System.Drawing.Size(145, 25);
            this.sunIndividualLabel.TabIndex = 5;
            this.sunIndividualLabel.Text = "개별 LED (, 구분)";
            this.sunIndividualLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // sunIndividualTextBox
            // 
            this.sunIndividualTextBox.Location = new System.Drawing.Point(160, 54);
            this.sunIndividualTextBox.Name = "sunIndividualTextBox";
            this.sunIndividualTextBox.Size = new System.Drawing.Size(665, 29);
            this.sunIndividualTextBox.TabIndex = 6;
            // 
            // sunApplyButton
            // 
            this.sunApplyButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(115)))), ((int)(((byte)(160)))));
            this.sunApplyButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunApplyButton.ForeColor = System.Drawing.Color.White;
            this.sunApplyButton.Location = new System.Drawing.Point(840, 51);
            this.sunApplyButton.Name = "sunApplyButton";
            this.sunApplyButton.Size = new System.Drawing.Size(180, 34);
            this.sunApplyButton.TabIndex = 7;
            this.sunApplyButton.Text = "선택 설정 적용";
            this.sunApplyButton.UseVisualStyleBackColor = false;
            // 
            // sunTuningCheckBox
            // 
            this.sunTuningCheckBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(215)))), ((int)(((byte)(110)))));
            this.sunTuningCheckBox.Location = new System.Drawing.Point(472, 19);
            this.sunTuningCheckBox.Name = "sunTuningCheckBox";
            this.sunTuningCheckBox.Size = new System.Drawing.Size(155, 25);
            this.sunTuningCheckBox.TabIndex = 8;
            this.sunTuningCheckBox.Text = "OTP 튜닝 적용";
            this.sunTuningCheckBox.UseVisualStyleBackColor = true;
            // 
            // sunTemperatureCheckBox
            // 
            this.sunTemperatureCheckBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(215)))), ((int)(((byte)(110)))));
            this.sunTemperatureCheckBox.Location = new System.Drawing.Point(625, 19);
            this.sunTemperatureCheckBox.Name = "sunTemperatureCheckBox";
            this.sunTemperatureCheckBox.Size = new System.Drawing.Size(105, 25);
            this.sunTemperatureCheckBox.TabIndex = 9;
            this.sunTemperatureCheckBox.Text = "온도 보정";
            this.sunTemperatureCheckBox.UseVisualStyleBackColor = true;
            // 
            // sunOspCurrentModeGroupBox
            // 
            this.sunOspCurrentModeGroupBox.Controls.Add(this.sunDayRadioButton);
            this.sunOspCurrentModeGroupBox.Controls.Add(this.sunNightRadioButton);
            this.sunOspCurrentModeGroupBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(215)))), ((int)(((byte)(110)))));
            this.sunOspCurrentModeGroupBox.Location = new System.Drawing.Point(730, 10);
            this.sunOspCurrentModeGroupBox.Name = "sunOspCurrentModeGroupBox";
            this.sunOspCurrentModeGroupBox.Size = new System.Drawing.Size(290, 39);
            this.sunOspCurrentModeGroupBox.TabIndex = 9;
            this.sunOspCurrentModeGroupBox.TabStop = false;
            this.sunOspCurrentModeGroupBox.Text = "[ OSP 전류 모드 ]";
            // 
            // sunDayRadioButton
            // 
            this.sunDayRadioButton.Checked = true;
            this.sunDayRadioButton.ForeColor = System.Drawing.Color.White;
            this.sunDayRadioButton.Location = new System.Drawing.Point(128, 11);
            this.sunDayRadioButton.Name = "sunDayRadioButton";
            this.sunDayRadioButton.Size = new System.Drawing.Size(70, 24);
            this.sunDayRadioButton.TabIndex = 0;
            this.sunDayRadioButton.TabStop = true;
            this.sunDayRadioButton.Text = "주간";
            this.sunDayRadioButton.UseVisualStyleBackColor = true;
            // 
            // sunNightRadioButton
            // 
            this.sunNightRadioButton.ForeColor = System.Drawing.Color.White;
            this.sunNightRadioButton.Location = new System.Drawing.Point(207, 11);
            this.sunNightRadioButton.Name = "sunNightRadioButton";
            this.sunNightRadioButton.Size = new System.Drawing.Size(70, 24);
            this.sunNightRadioButton.TabIndex = 1;
            this.sunNightRadioButton.Text = "야간";
            this.sunNightRadioButton.UseVisualStyleBackColor = true;
            // 
            // sunHintLabel
            // 
            this.sunHintLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(205)))), ((int)(((byte)(225)))));
            this.sunHintLabel.Location = new System.Drawing.Point(667, 20);
            this.sunHintLabel.Name = "sunHintLabel";
            this.sunHintLabel.Size = new System.Drawing.Size(1005, 24);
            this.sunHintLabel.TabIndex = 9;
            this.sunHintLabel.Text = "※ 기본은 전체 제어이며 기존 동작과 동일합니다.";
            // 
            // sunLedSelectionGroupBox
            // 
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox1);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox2);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox3);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox4);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox5);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox6);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox7);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox8);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox9);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox10);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox11);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox12);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox13);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox14);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox15);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox16);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox17);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox18);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox19);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox20);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox21);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox22);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox23);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox24);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox25);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox26);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox27);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox28);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox29);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox30);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox31);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox32);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox33);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox34);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox35);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox36);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox37);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox38);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox39);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox40);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox41);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox42);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox43);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox44);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox45);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox46);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox47);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox48);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox49);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox50);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox51);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox52);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox53);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox54);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox55);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox56);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox57);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox58);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox59);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox60);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox61);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox62);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox63);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox64);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox65);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox66);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox67);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox68);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox69);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox70);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox71);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox72);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox73);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox74);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox75);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox76);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox77);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox78);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox79);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox80);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox81);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox82);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox83);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectCheckBox84);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectAllButton);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectClearButton);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectInvertButton);
            this.sunLedSelectionGroupBox.Controls.Add(this.sunLedSelectHintLabel);
            this.sunLedSelectionGroupBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.sunLedSelectionGroupBox.Location = new System.Drawing.Point(3, 103);
            this.sunLedSelectionGroupBox.Name = "sunLedSelectionGroupBox";
            this.sunLedSelectionGroupBox.Size = new System.Drawing.Size(1035, 200);
            this.sunLedSelectionGroupBox.TabIndex = 1;
            this.sunLedSelectionGroupBox.TabStop = false;
            this.sunLedSelectionGroupBox.Text = "[ 개별 LED 선택 - 클릭 / 드래그 ]";
            // 
            // sunLedSelectCheckBox1
            // 
            this.sunLedSelectCheckBox1.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox1.AutoCheck = false;
            this.sunLedSelectCheckBox1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox1.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox1.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox1.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox1.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox1.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox1.Location = new System.Drawing.Point(10, 51);
            this.sunLedSelectCheckBox1.Name = "sunLedSelectCheckBox1";
            this.sunLedSelectCheckBox1.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox1.TabIndex = 4;
            this.sunLedSelectCheckBox1.Tag = 1;
            this.sunLedSelectCheckBox1.Text = "1";
            this.sunLedSelectCheckBox1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox1.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox2
            // 
            this.sunLedSelectCheckBox2.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox2.AutoCheck = false;
            this.sunLedSelectCheckBox2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox2.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox2.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox2.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox2.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox2.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox2.Location = new System.Drawing.Point(82, 51);
            this.sunLedSelectCheckBox2.Name = "sunLedSelectCheckBox2";
            this.sunLedSelectCheckBox2.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox2.TabIndex = 5;
            this.sunLedSelectCheckBox2.Tag = 2;
            this.sunLedSelectCheckBox2.Text = "2";
            this.sunLedSelectCheckBox2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox2.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox3
            // 
            this.sunLedSelectCheckBox3.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox3.AutoCheck = false;
            this.sunLedSelectCheckBox3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox3.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox3.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox3.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox3.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox3.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox3.Location = new System.Drawing.Point(154, 51);
            this.sunLedSelectCheckBox3.Name = "sunLedSelectCheckBox3";
            this.sunLedSelectCheckBox3.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox3.TabIndex = 6;
            this.sunLedSelectCheckBox3.Tag = 3;
            this.sunLedSelectCheckBox3.Text = "3";
            this.sunLedSelectCheckBox3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox3.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox4
            // 
            this.sunLedSelectCheckBox4.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox4.AutoCheck = false;
            this.sunLedSelectCheckBox4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox4.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox4.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox4.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox4.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox4.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox4.Location = new System.Drawing.Point(226, 51);
            this.sunLedSelectCheckBox4.Name = "sunLedSelectCheckBox4";
            this.sunLedSelectCheckBox4.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox4.TabIndex = 7;
            this.sunLedSelectCheckBox4.Tag = 4;
            this.sunLedSelectCheckBox4.Text = "4";
            this.sunLedSelectCheckBox4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox4.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox5
            // 
            this.sunLedSelectCheckBox5.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox5.AutoCheck = false;
            this.sunLedSelectCheckBox5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox5.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox5.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox5.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox5.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox5.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox5.Location = new System.Drawing.Point(298, 51);
            this.sunLedSelectCheckBox5.Name = "sunLedSelectCheckBox5";
            this.sunLedSelectCheckBox5.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox5.TabIndex = 8;
            this.sunLedSelectCheckBox5.Tag = 5;
            this.sunLedSelectCheckBox5.Text = "5";
            this.sunLedSelectCheckBox5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox5.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox6
            // 
            this.sunLedSelectCheckBox6.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox6.AutoCheck = false;
            this.sunLedSelectCheckBox6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox6.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox6.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox6.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox6.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox6.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox6.Location = new System.Drawing.Point(370, 51);
            this.sunLedSelectCheckBox6.Name = "sunLedSelectCheckBox6";
            this.sunLedSelectCheckBox6.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox6.TabIndex = 9;
            this.sunLedSelectCheckBox6.Tag = 6;
            this.sunLedSelectCheckBox6.Text = "6";
            this.sunLedSelectCheckBox6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox6.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox7
            // 
            this.sunLedSelectCheckBox7.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox7.AutoCheck = false;
            this.sunLedSelectCheckBox7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox7.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox7.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox7.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox7.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox7.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox7.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox7.Location = new System.Drawing.Point(442, 51);
            this.sunLedSelectCheckBox7.Name = "sunLedSelectCheckBox7";
            this.sunLedSelectCheckBox7.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox7.TabIndex = 10;
            this.sunLedSelectCheckBox7.Tag = 7;
            this.sunLedSelectCheckBox7.Text = "7";
            this.sunLedSelectCheckBox7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox7.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox8
            // 
            this.sunLedSelectCheckBox8.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox8.AutoCheck = false;
            this.sunLedSelectCheckBox8.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox8.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox8.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox8.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox8.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox8.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox8.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox8.Location = new System.Drawing.Point(514, 51);
            this.sunLedSelectCheckBox8.Name = "sunLedSelectCheckBox8";
            this.sunLedSelectCheckBox8.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox8.TabIndex = 11;
            this.sunLedSelectCheckBox8.Tag = 8;
            this.sunLedSelectCheckBox8.Text = "8";
            this.sunLedSelectCheckBox8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox8.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox9
            // 
            this.sunLedSelectCheckBox9.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox9.AutoCheck = false;
            this.sunLedSelectCheckBox9.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox9.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox9.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox9.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox9.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox9.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox9.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox9.Location = new System.Drawing.Point(586, 51);
            this.sunLedSelectCheckBox9.Name = "sunLedSelectCheckBox9";
            this.sunLedSelectCheckBox9.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox9.TabIndex = 12;
            this.sunLedSelectCheckBox9.Tag = 9;
            this.sunLedSelectCheckBox9.Text = "9";
            this.sunLedSelectCheckBox9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox9.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox10
            // 
            this.sunLedSelectCheckBox10.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox10.AutoCheck = false;
            this.sunLedSelectCheckBox10.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox10.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox10.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox10.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox10.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox10.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox10.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox10.Location = new System.Drawing.Point(658, 51);
            this.sunLedSelectCheckBox10.Name = "sunLedSelectCheckBox10";
            this.sunLedSelectCheckBox10.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox10.TabIndex = 13;
            this.sunLedSelectCheckBox10.Tag = 10;
            this.sunLedSelectCheckBox10.Text = "10";
            this.sunLedSelectCheckBox10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox10.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox11
            // 
            this.sunLedSelectCheckBox11.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox11.AutoCheck = false;
            this.sunLedSelectCheckBox11.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox11.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox11.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox11.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox11.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox11.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox11.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox11.Location = new System.Drawing.Point(730, 51);
            this.sunLedSelectCheckBox11.Name = "sunLedSelectCheckBox11";
            this.sunLedSelectCheckBox11.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox11.TabIndex = 14;
            this.sunLedSelectCheckBox11.Tag = 11;
            this.sunLedSelectCheckBox11.Text = "11";
            this.sunLedSelectCheckBox11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox11.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox12
            // 
            this.sunLedSelectCheckBox12.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox12.AutoCheck = false;
            this.sunLedSelectCheckBox12.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox12.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox12.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox12.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox12.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox12.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox12.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox12.Location = new System.Drawing.Point(802, 51);
            this.sunLedSelectCheckBox12.Name = "sunLedSelectCheckBox12";
            this.sunLedSelectCheckBox12.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox12.TabIndex = 15;
            this.sunLedSelectCheckBox12.Tag = 12;
            this.sunLedSelectCheckBox12.Text = "12";
            this.sunLedSelectCheckBox12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox12.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox13
            // 
            this.sunLedSelectCheckBox13.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox13.AutoCheck = false;
            this.sunLedSelectCheckBox13.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox13.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox13.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox13.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox13.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox13.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox13.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox13.Location = new System.Drawing.Point(874, 51);
            this.sunLedSelectCheckBox13.Name = "sunLedSelectCheckBox13";
            this.sunLedSelectCheckBox13.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox13.TabIndex = 16;
            this.sunLedSelectCheckBox13.Tag = 13;
            this.sunLedSelectCheckBox13.Text = "13";
            this.sunLedSelectCheckBox13.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox13.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox14
            // 
            this.sunLedSelectCheckBox14.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox14.AutoCheck = false;
            this.sunLedSelectCheckBox14.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox14.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox14.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox14.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox14.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox14.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox14.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox14.Location = new System.Drawing.Point(946, 51);
            this.sunLedSelectCheckBox14.Name = "sunLedSelectCheckBox14";
            this.sunLedSelectCheckBox14.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox14.TabIndex = 17;
            this.sunLedSelectCheckBox14.Tag = 14;
            this.sunLedSelectCheckBox14.Text = "14";
            this.sunLedSelectCheckBox14.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox14.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox15
            // 
            this.sunLedSelectCheckBox15.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox15.AutoCheck = false;
            this.sunLedSelectCheckBox15.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox15.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox15.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox15.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox15.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox15.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox15.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox15.Location = new System.Drawing.Point(10, 74);
            this.sunLedSelectCheckBox15.Name = "sunLedSelectCheckBox15";
            this.sunLedSelectCheckBox15.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox15.TabIndex = 18;
            this.sunLedSelectCheckBox15.Tag = 15;
            this.sunLedSelectCheckBox15.Text = "15";
            this.sunLedSelectCheckBox15.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox15.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox16
            // 
            this.sunLedSelectCheckBox16.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox16.AutoCheck = false;
            this.sunLedSelectCheckBox16.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox16.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox16.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox16.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox16.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox16.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox16.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox16.Location = new System.Drawing.Point(82, 74);
            this.sunLedSelectCheckBox16.Name = "sunLedSelectCheckBox16";
            this.sunLedSelectCheckBox16.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox16.TabIndex = 19;
            this.sunLedSelectCheckBox16.Tag = 16;
            this.sunLedSelectCheckBox16.Text = "16";
            this.sunLedSelectCheckBox16.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox16.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox17
            // 
            this.sunLedSelectCheckBox17.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox17.AutoCheck = false;
            this.sunLedSelectCheckBox17.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox17.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox17.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox17.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox17.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox17.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox17.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox17.Location = new System.Drawing.Point(154, 74);
            this.sunLedSelectCheckBox17.Name = "sunLedSelectCheckBox17";
            this.sunLedSelectCheckBox17.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox17.TabIndex = 20;
            this.sunLedSelectCheckBox17.Tag = 17;
            this.sunLedSelectCheckBox17.Text = "17";
            this.sunLedSelectCheckBox17.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox17.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox18
            // 
            this.sunLedSelectCheckBox18.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox18.AutoCheck = false;
            this.sunLedSelectCheckBox18.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox18.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox18.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox18.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox18.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox18.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox18.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox18.Location = new System.Drawing.Point(226, 74);
            this.sunLedSelectCheckBox18.Name = "sunLedSelectCheckBox18";
            this.sunLedSelectCheckBox18.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox18.TabIndex = 21;
            this.sunLedSelectCheckBox18.Tag = 18;
            this.sunLedSelectCheckBox18.Text = "18";
            this.sunLedSelectCheckBox18.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox18.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox19
            // 
            this.sunLedSelectCheckBox19.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox19.AutoCheck = false;
            this.sunLedSelectCheckBox19.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox19.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox19.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox19.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox19.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox19.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox19.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox19.Location = new System.Drawing.Point(298, 74);
            this.sunLedSelectCheckBox19.Name = "sunLedSelectCheckBox19";
            this.sunLedSelectCheckBox19.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox19.TabIndex = 22;
            this.sunLedSelectCheckBox19.Tag = 19;
            this.sunLedSelectCheckBox19.Text = "19";
            this.sunLedSelectCheckBox19.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox19.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox20
            // 
            this.sunLedSelectCheckBox20.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox20.AutoCheck = false;
            this.sunLedSelectCheckBox20.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox20.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox20.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox20.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox20.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox20.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox20.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox20.Location = new System.Drawing.Point(370, 74);
            this.sunLedSelectCheckBox20.Name = "sunLedSelectCheckBox20";
            this.sunLedSelectCheckBox20.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox20.TabIndex = 23;
            this.sunLedSelectCheckBox20.Tag = 20;
            this.sunLedSelectCheckBox20.Text = "20";
            this.sunLedSelectCheckBox20.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox20.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox21
            // 
            this.sunLedSelectCheckBox21.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox21.AutoCheck = false;
            this.sunLedSelectCheckBox21.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox21.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox21.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox21.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox21.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox21.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox21.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox21.Location = new System.Drawing.Point(442, 74);
            this.sunLedSelectCheckBox21.Name = "sunLedSelectCheckBox21";
            this.sunLedSelectCheckBox21.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox21.TabIndex = 24;
            this.sunLedSelectCheckBox21.Tag = 21;
            this.sunLedSelectCheckBox21.Text = "21";
            this.sunLedSelectCheckBox21.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox21.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox22
            // 
            this.sunLedSelectCheckBox22.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox22.AutoCheck = false;
            this.sunLedSelectCheckBox22.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox22.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox22.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox22.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox22.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox22.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox22.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox22.Location = new System.Drawing.Point(514, 74);
            this.sunLedSelectCheckBox22.Name = "sunLedSelectCheckBox22";
            this.sunLedSelectCheckBox22.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox22.TabIndex = 25;
            this.sunLedSelectCheckBox22.Tag = 22;
            this.sunLedSelectCheckBox22.Text = "22";
            this.sunLedSelectCheckBox22.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox22.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox23
            // 
            this.sunLedSelectCheckBox23.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox23.AutoCheck = false;
            this.sunLedSelectCheckBox23.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox23.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox23.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox23.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox23.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox23.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox23.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox23.Location = new System.Drawing.Point(586, 74);
            this.sunLedSelectCheckBox23.Name = "sunLedSelectCheckBox23";
            this.sunLedSelectCheckBox23.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox23.TabIndex = 26;
            this.sunLedSelectCheckBox23.Tag = 23;
            this.sunLedSelectCheckBox23.Text = "23";
            this.sunLedSelectCheckBox23.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox23.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox24
            // 
            this.sunLedSelectCheckBox24.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox24.AutoCheck = false;
            this.sunLedSelectCheckBox24.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox24.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox24.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox24.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox24.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox24.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox24.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox24.Location = new System.Drawing.Point(658, 74);
            this.sunLedSelectCheckBox24.Name = "sunLedSelectCheckBox24";
            this.sunLedSelectCheckBox24.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox24.TabIndex = 27;
            this.sunLedSelectCheckBox24.Tag = 24;
            this.sunLedSelectCheckBox24.Text = "24";
            this.sunLedSelectCheckBox24.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox24.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox25
            // 
            this.sunLedSelectCheckBox25.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox25.AutoCheck = false;
            this.sunLedSelectCheckBox25.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox25.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox25.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox25.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox25.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox25.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox25.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox25.Location = new System.Drawing.Point(730, 74);
            this.sunLedSelectCheckBox25.Name = "sunLedSelectCheckBox25";
            this.sunLedSelectCheckBox25.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox25.TabIndex = 28;
            this.sunLedSelectCheckBox25.Tag = 25;
            this.sunLedSelectCheckBox25.Text = "25";
            this.sunLedSelectCheckBox25.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox25.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox26
            // 
            this.sunLedSelectCheckBox26.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox26.AutoCheck = false;
            this.sunLedSelectCheckBox26.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox26.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox26.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox26.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox26.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox26.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox26.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox26.Location = new System.Drawing.Point(802, 74);
            this.sunLedSelectCheckBox26.Name = "sunLedSelectCheckBox26";
            this.sunLedSelectCheckBox26.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox26.TabIndex = 29;
            this.sunLedSelectCheckBox26.Tag = 26;
            this.sunLedSelectCheckBox26.Text = "26";
            this.sunLedSelectCheckBox26.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox26.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox27
            // 
            this.sunLedSelectCheckBox27.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox27.AutoCheck = false;
            this.sunLedSelectCheckBox27.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox27.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox27.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox27.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox27.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox27.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox27.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox27.Location = new System.Drawing.Point(874, 74);
            this.sunLedSelectCheckBox27.Name = "sunLedSelectCheckBox27";
            this.sunLedSelectCheckBox27.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox27.TabIndex = 30;
            this.sunLedSelectCheckBox27.Tag = 27;
            this.sunLedSelectCheckBox27.Text = "27";
            this.sunLedSelectCheckBox27.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox27.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox28
            // 
            this.sunLedSelectCheckBox28.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox28.AutoCheck = false;
            this.sunLedSelectCheckBox28.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox28.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox28.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox28.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox28.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox28.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox28.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox28.Location = new System.Drawing.Point(946, 74);
            this.sunLedSelectCheckBox28.Name = "sunLedSelectCheckBox28";
            this.sunLedSelectCheckBox28.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox28.TabIndex = 31;
            this.sunLedSelectCheckBox28.Tag = 28;
            this.sunLedSelectCheckBox28.Text = "28";
            this.sunLedSelectCheckBox28.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox28.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox29
            // 
            this.sunLedSelectCheckBox29.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox29.AutoCheck = false;
            this.sunLedSelectCheckBox29.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox29.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox29.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox29.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox29.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox29.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox29.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox29.Location = new System.Drawing.Point(10, 97);
            this.sunLedSelectCheckBox29.Name = "sunLedSelectCheckBox29";
            this.sunLedSelectCheckBox29.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox29.TabIndex = 32;
            this.sunLedSelectCheckBox29.Tag = 29;
            this.sunLedSelectCheckBox29.Text = "29";
            this.sunLedSelectCheckBox29.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox29.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox30
            // 
            this.sunLedSelectCheckBox30.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox30.AutoCheck = false;
            this.sunLedSelectCheckBox30.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox30.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox30.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox30.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox30.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox30.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox30.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox30.Location = new System.Drawing.Point(82, 97);
            this.sunLedSelectCheckBox30.Name = "sunLedSelectCheckBox30";
            this.sunLedSelectCheckBox30.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox30.TabIndex = 33;
            this.sunLedSelectCheckBox30.Tag = 30;
            this.sunLedSelectCheckBox30.Text = "30";
            this.sunLedSelectCheckBox30.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox30.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox31
            // 
            this.sunLedSelectCheckBox31.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox31.AutoCheck = false;
            this.sunLedSelectCheckBox31.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox31.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox31.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox31.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox31.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox31.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox31.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox31.Location = new System.Drawing.Point(154, 97);
            this.sunLedSelectCheckBox31.Name = "sunLedSelectCheckBox31";
            this.sunLedSelectCheckBox31.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox31.TabIndex = 34;
            this.sunLedSelectCheckBox31.Tag = 31;
            this.sunLedSelectCheckBox31.Text = "31";
            this.sunLedSelectCheckBox31.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox31.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox32
            // 
            this.sunLedSelectCheckBox32.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox32.AutoCheck = false;
            this.sunLedSelectCheckBox32.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox32.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox32.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox32.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox32.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox32.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox32.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox32.Location = new System.Drawing.Point(226, 97);
            this.sunLedSelectCheckBox32.Name = "sunLedSelectCheckBox32";
            this.sunLedSelectCheckBox32.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox32.TabIndex = 35;
            this.sunLedSelectCheckBox32.Tag = 32;
            this.sunLedSelectCheckBox32.Text = "32";
            this.sunLedSelectCheckBox32.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox32.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox33
            // 
            this.sunLedSelectCheckBox33.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox33.AutoCheck = false;
            this.sunLedSelectCheckBox33.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox33.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox33.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox33.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox33.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox33.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox33.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox33.Location = new System.Drawing.Point(298, 97);
            this.sunLedSelectCheckBox33.Name = "sunLedSelectCheckBox33";
            this.sunLedSelectCheckBox33.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox33.TabIndex = 36;
            this.sunLedSelectCheckBox33.Tag = 33;
            this.sunLedSelectCheckBox33.Text = "33";
            this.sunLedSelectCheckBox33.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox33.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox34
            // 
            this.sunLedSelectCheckBox34.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox34.AutoCheck = false;
            this.sunLedSelectCheckBox34.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox34.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox34.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox34.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox34.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox34.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox34.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox34.Location = new System.Drawing.Point(370, 97);
            this.sunLedSelectCheckBox34.Name = "sunLedSelectCheckBox34";
            this.sunLedSelectCheckBox34.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox34.TabIndex = 37;
            this.sunLedSelectCheckBox34.Tag = 34;
            this.sunLedSelectCheckBox34.Text = "34";
            this.sunLedSelectCheckBox34.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox34.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox35
            // 
            this.sunLedSelectCheckBox35.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox35.AutoCheck = false;
            this.sunLedSelectCheckBox35.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox35.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox35.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox35.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox35.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox35.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox35.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox35.Location = new System.Drawing.Point(442, 97);
            this.sunLedSelectCheckBox35.Name = "sunLedSelectCheckBox35";
            this.sunLedSelectCheckBox35.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox35.TabIndex = 38;
            this.sunLedSelectCheckBox35.Tag = 35;
            this.sunLedSelectCheckBox35.Text = "35";
            this.sunLedSelectCheckBox35.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox35.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox36
            // 
            this.sunLedSelectCheckBox36.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox36.AutoCheck = false;
            this.sunLedSelectCheckBox36.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox36.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox36.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox36.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox36.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox36.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox36.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox36.Location = new System.Drawing.Point(514, 97);
            this.sunLedSelectCheckBox36.Name = "sunLedSelectCheckBox36";
            this.sunLedSelectCheckBox36.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox36.TabIndex = 39;
            this.sunLedSelectCheckBox36.Tag = 36;
            this.sunLedSelectCheckBox36.Text = "36";
            this.sunLedSelectCheckBox36.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox36.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox37
            // 
            this.sunLedSelectCheckBox37.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox37.AutoCheck = false;
            this.sunLedSelectCheckBox37.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox37.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox37.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox37.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox37.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox37.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox37.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox37.Location = new System.Drawing.Point(586, 97);
            this.sunLedSelectCheckBox37.Name = "sunLedSelectCheckBox37";
            this.sunLedSelectCheckBox37.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox37.TabIndex = 40;
            this.sunLedSelectCheckBox37.Tag = 37;
            this.sunLedSelectCheckBox37.Text = "37";
            this.sunLedSelectCheckBox37.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox37.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox38
            // 
            this.sunLedSelectCheckBox38.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox38.AutoCheck = false;
            this.sunLedSelectCheckBox38.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox38.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox38.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox38.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox38.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox38.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox38.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox38.Location = new System.Drawing.Point(658, 97);
            this.sunLedSelectCheckBox38.Name = "sunLedSelectCheckBox38";
            this.sunLedSelectCheckBox38.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox38.TabIndex = 41;
            this.sunLedSelectCheckBox38.Tag = 38;
            this.sunLedSelectCheckBox38.Text = "38";
            this.sunLedSelectCheckBox38.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox38.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox39
            // 
            this.sunLedSelectCheckBox39.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox39.AutoCheck = false;
            this.sunLedSelectCheckBox39.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox39.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox39.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox39.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox39.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox39.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox39.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox39.Location = new System.Drawing.Point(730, 97);
            this.sunLedSelectCheckBox39.Name = "sunLedSelectCheckBox39";
            this.sunLedSelectCheckBox39.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox39.TabIndex = 42;
            this.sunLedSelectCheckBox39.Tag = 39;
            this.sunLedSelectCheckBox39.Text = "39";
            this.sunLedSelectCheckBox39.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox39.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox40
            // 
            this.sunLedSelectCheckBox40.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox40.AutoCheck = false;
            this.sunLedSelectCheckBox40.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox40.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox40.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox40.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox40.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox40.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox40.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox40.Location = new System.Drawing.Point(802, 97);
            this.sunLedSelectCheckBox40.Name = "sunLedSelectCheckBox40";
            this.sunLedSelectCheckBox40.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox40.TabIndex = 43;
            this.sunLedSelectCheckBox40.Tag = 40;
            this.sunLedSelectCheckBox40.Text = "40";
            this.sunLedSelectCheckBox40.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox40.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox41
            // 
            this.sunLedSelectCheckBox41.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox41.AutoCheck = false;
            this.sunLedSelectCheckBox41.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox41.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox41.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox41.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox41.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox41.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox41.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox41.Location = new System.Drawing.Point(874, 97);
            this.sunLedSelectCheckBox41.Name = "sunLedSelectCheckBox41";
            this.sunLedSelectCheckBox41.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox41.TabIndex = 44;
            this.sunLedSelectCheckBox41.Tag = 41;
            this.sunLedSelectCheckBox41.Text = "41";
            this.sunLedSelectCheckBox41.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox41.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox42
            // 
            this.sunLedSelectCheckBox42.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox42.AutoCheck = false;
            this.sunLedSelectCheckBox42.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox42.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox42.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox42.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox42.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox42.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox42.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox42.Location = new System.Drawing.Point(946, 97);
            this.sunLedSelectCheckBox42.Name = "sunLedSelectCheckBox42";
            this.sunLedSelectCheckBox42.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox42.TabIndex = 45;
            this.sunLedSelectCheckBox42.Tag = 42;
            this.sunLedSelectCheckBox42.Text = "42";
            this.sunLedSelectCheckBox42.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox42.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox43
            // 
            this.sunLedSelectCheckBox43.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox43.AutoCheck = false;
            this.sunLedSelectCheckBox43.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox43.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox43.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox43.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox43.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox43.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox43.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox43.Location = new System.Drawing.Point(10, 120);
            this.sunLedSelectCheckBox43.Name = "sunLedSelectCheckBox43";
            this.sunLedSelectCheckBox43.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox43.TabIndex = 46;
            this.sunLedSelectCheckBox43.Tag = 43;
            this.sunLedSelectCheckBox43.Text = "43";
            this.sunLedSelectCheckBox43.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox43.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox44
            // 
            this.sunLedSelectCheckBox44.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox44.AutoCheck = false;
            this.sunLedSelectCheckBox44.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox44.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox44.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox44.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox44.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox44.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox44.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox44.Location = new System.Drawing.Point(82, 120);
            this.sunLedSelectCheckBox44.Name = "sunLedSelectCheckBox44";
            this.sunLedSelectCheckBox44.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox44.TabIndex = 47;
            this.sunLedSelectCheckBox44.Tag = 44;
            this.sunLedSelectCheckBox44.Text = "44";
            this.sunLedSelectCheckBox44.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox44.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox45
            // 
            this.sunLedSelectCheckBox45.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox45.AutoCheck = false;
            this.sunLedSelectCheckBox45.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox45.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox45.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox45.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox45.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox45.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox45.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox45.Location = new System.Drawing.Point(154, 120);
            this.sunLedSelectCheckBox45.Name = "sunLedSelectCheckBox45";
            this.sunLedSelectCheckBox45.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox45.TabIndex = 48;
            this.sunLedSelectCheckBox45.Tag = 45;
            this.sunLedSelectCheckBox45.Text = "45";
            this.sunLedSelectCheckBox45.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox45.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox46
            // 
            this.sunLedSelectCheckBox46.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox46.AutoCheck = false;
            this.sunLedSelectCheckBox46.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox46.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox46.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox46.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox46.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox46.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox46.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox46.Location = new System.Drawing.Point(226, 120);
            this.sunLedSelectCheckBox46.Name = "sunLedSelectCheckBox46";
            this.sunLedSelectCheckBox46.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox46.TabIndex = 49;
            this.sunLedSelectCheckBox46.Tag = 46;
            this.sunLedSelectCheckBox46.Text = "46";
            this.sunLedSelectCheckBox46.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox46.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox47
            // 
            this.sunLedSelectCheckBox47.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox47.AutoCheck = false;
            this.sunLedSelectCheckBox47.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox47.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox47.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox47.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox47.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox47.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox47.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox47.Location = new System.Drawing.Point(298, 120);
            this.sunLedSelectCheckBox47.Name = "sunLedSelectCheckBox47";
            this.sunLedSelectCheckBox47.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox47.TabIndex = 50;
            this.sunLedSelectCheckBox47.Tag = 47;
            this.sunLedSelectCheckBox47.Text = "47";
            this.sunLedSelectCheckBox47.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox47.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox48
            // 
            this.sunLedSelectCheckBox48.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox48.AutoCheck = false;
            this.sunLedSelectCheckBox48.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox48.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox48.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox48.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox48.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox48.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox48.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox48.Location = new System.Drawing.Point(370, 120);
            this.sunLedSelectCheckBox48.Name = "sunLedSelectCheckBox48";
            this.sunLedSelectCheckBox48.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox48.TabIndex = 51;
            this.sunLedSelectCheckBox48.Tag = 48;
            this.sunLedSelectCheckBox48.Text = "48";
            this.sunLedSelectCheckBox48.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox48.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox49
            // 
            this.sunLedSelectCheckBox49.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox49.AutoCheck = false;
            this.sunLedSelectCheckBox49.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox49.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox49.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox49.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox49.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox49.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox49.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox49.Location = new System.Drawing.Point(442, 120);
            this.sunLedSelectCheckBox49.Name = "sunLedSelectCheckBox49";
            this.sunLedSelectCheckBox49.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox49.TabIndex = 52;
            this.sunLedSelectCheckBox49.Tag = 49;
            this.sunLedSelectCheckBox49.Text = "49";
            this.sunLedSelectCheckBox49.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox49.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox50
            // 
            this.sunLedSelectCheckBox50.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox50.AutoCheck = false;
            this.sunLedSelectCheckBox50.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox50.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox50.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox50.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox50.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox50.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox50.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox50.Location = new System.Drawing.Point(514, 120);
            this.sunLedSelectCheckBox50.Name = "sunLedSelectCheckBox50";
            this.sunLedSelectCheckBox50.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox50.TabIndex = 53;
            this.sunLedSelectCheckBox50.Tag = 50;
            this.sunLedSelectCheckBox50.Text = "50";
            this.sunLedSelectCheckBox50.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox50.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox51
            // 
            this.sunLedSelectCheckBox51.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox51.AutoCheck = false;
            this.sunLedSelectCheckBox51.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox51.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox51.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox51.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox51.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox51.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox51.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox51.Location = new System.Drawing.Point(586, 120);
            this.sunLedSelectCheckBox51.Name = "sunLedSelectCheckBox51";
            this.sunLedSelectCheckBox51.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox51.TabIndex = 54;
            this.sunLedSelectCheckBox51.Tag = 51;
            this.sunLedSelectCheckBox51.Text = "51";
            this.sunLedSelectCheckBox51.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox51.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox52
            // 
            this.sunLedSelectCheckBox52.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox52.AutoCheck = false;
            this.sunLedSelectCheckBox52.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox52.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox52.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox52.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox52.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox52.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox52.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox52.Location = new System.Drawing.Point(658, 120);
            this.sunLedSelectCheckBox52.Name = "sunLedSelectCheckBox52";
            this.sunLedSelectCheckBox52.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox52.TabIndex = 55;
            this.sunLedSelectCheckBox52.Tag = 52;
            this.sunLedSelectCheckBox52.Text = "52";
            this.sunLedSelectCheckBox52.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox52.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox53
            // 
            this.sunLedSelectCheckBox53.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox53.AutoCheck = false;
            this.sunLedSelectCheckBox53.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox53.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox53.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox53.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox53.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox53.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox53.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox53.Location = new System.Drawing.Point(730, 120);
            this.sunLedSelectCheckBox53.Name = "sunLedSelectCheckBox53";
            this.sunLedSelectCheckBox53.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox53.TabIndex = 56;
            this.sunLedSelectCheckBox53.Tag = 53;
            this.sunLedSelectCheckBox53.Text = "53";
            this.sunLedSelectCheckBox53.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox53.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox54
            // 
            this.sunLedSelectCheckBox54.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox54.AutoCheck = false;
            this.sunLedSelectCheckBox54.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox54.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox54.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox54.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox54.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox54.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox54.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox54.Location = new System.Drawing.Point(802, 120);
            this.sunLedSelectCheckBox54.Name = "sunLedSelectCheckBox54";
            this.sunLedSelectCheckBox54.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox54.TabIndex = 57;
            this.sunLedSelectCheckBox54.Tag = 54;
            this.sunLedSelectCheckBox54.Text = "54";
            this.sunLedSelectCheckBox54.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox54.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox55
            // 
            this.sunLedSelectCheckBox55.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox55.AutoCheck = false;
            this.sunLedSelectCheckBox55.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox55.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox55.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox55.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox55.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox55.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox55.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox55.Location = new System.Drawing.Point(874, 120);
            this.sunLedSelectCheckBox55.Name = "sunLedSelectCheckBox55";
            this.sunLedSelectCheckBox55.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox55.TabIndex = 58;
            this.sunLedSelectCheckBox55.Tag = 55;
            this.sunLedSelectCheckBox55.Text = "55";
            this.sunLedSelectCheckBox55.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox55.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox56
            // 
            this.sunLedSelectCheckBox56.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox56.AutoCheck = false;
            this.sunLedSelectCheckBox56.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox56.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox56.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox56.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox56.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox56.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox56.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox56.Location = new System.Drawing.Point(946, 120);
            this.sunLedSelectCheckBox56.Name = "sunLedSelectCheckBox56";
            this.sunLedSelectCheckBox56.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox56.TabIndex = 59;
            this.sunLedSelectCheckBox56.Tag = 56;
            this.sunLedSelectCheckBox56.Text = "56";
            this.sunLedSelectCheckBox56.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox56.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox57
            // 
            this.sunLedSelectCheckBox57.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox57.AutoCheck = false;
            this.sunLedSelectCheckBox57.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox57.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox57.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox57.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox57.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox57.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox57.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox57.Location = new System.Drawing.Point(10, 143);
            this.sunLedSelectCheckBox57.Name = "sunLedSelectCheckBox57";
            this.sunLedSelectCheckBox57.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox57.TabIndex = 60;
            this.sunLedSelectCheckBox57.Tag = 57;
            this.sunLedSelectCheckBox57.Text = "57";
            this.sunLedSelectCheckBox57.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox57.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox58
            // 
            this.sunLedSelectCheckBox58.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox58.AutoCheck = false;
            this.sunLedSelectCheckBox58.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox58.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox58.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox58.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox58.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox58.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox58.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox58.Location = new System.Drawing.Point(82, 143);
            this.sunLedSelectCheckBox58.Name = "sunLedSelectCheckBox58";
            this.sunLedSelectCheckBox58.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox58.TabIndex = 61;
            this.sunLedSelectCheckBox58.Tag = 58;
            this.sunLedSelectCheckBox58.Text = "58";
            this.sunLedSelectCheckBox58.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox58.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox59
            // 
            this.sunLedSelectCheckBox59.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox59.AutoCheck = false;
            this.sunLedSelectCheckBox59.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox59.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox59.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox59.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox59.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox59.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox59.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox59.Location = new System.Drawing.Point(154, 143);
            this.sunLedSelectCheckBox59.Name = "sunLedSelectCheckBox59";
            this.sunLedSelectCheckBox59.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox59.TabIndex = 62;
            this.sunLedSelectCheckBox59.Tag = 59;
            this.sunLedSelectCheckBox59.Text = "59";
            this.sunLedSelectCheckBox59.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox59.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox60
            // 
            this.sunLedSelectCheckBox60.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox60.AutoCheck = false;
            this.sunLedSelectCheckBox60.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox60.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox60.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox60.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox60.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox60.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox60.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox60.Location = new System.Drawing.Point(226, 143);
            this.sunLedSelectCheckBox60.Name = "sunLedSelectCheckBox60";
            this.sunLedSelectCheckBox60.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox60.TabIndex = 63;
            this.sunLedSelectCheckBox60.Tag = 60;
            this.sunLedSelectCheckBox60.Text = "60";
            this.sunLedSelectCheckBox60.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox60.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox61
            // 
            this.sunLedSelectCheckBox61.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox61.AutoCheck = false;
            this.sunLedSelectCheckBox61.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox61.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox61.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox61.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox61.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox61.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox61.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox61.Location = new System.Drawing.Point(298, 143);
            this.sunLedSelectCheckBox61.Name = "sunLedSelectCheckBox61";
            this.sunLedSelectCheckBox61.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox61.TabIndex = 64;
            this.sunLedSelectCheckBox61.Tag = 61;
            this.sunLedSelectCheckBox61.Text = "61";
            this.sunLedSelectCheckBox61.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox61.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox62
            // 
            this.sunLedSelectCheckBox62.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox62.AutoCheck = false;
            this.sunLedSelectCheckBox62.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox62.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox62.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox62.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox62.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox62.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox62.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox62.Location = new System.Drawing.Point(370, 143);
            this.sunLedSelectCheckBox62.Name = "sunLedSelectCheckBox62";
            this.sunLedSelectCheckBox62.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox62.TabIndex = 65;
            this.sunLedSelectCheckBox62.Tag = 62;
            this.sunLedSelectCheckBox62.Text = "62";
            this.sunLedSelectCheckBox62.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox62.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox63
            // 
            this.sunLedSelectCheckBox63.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox63.AutoCheck = false;
            this.sunLedSelectCheckBox63.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox63.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox63.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox63.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox63.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox63.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox63.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox63.Location = new System.Drawing.Point(442, 143);
            this.sunLedSelectCheckBox63.Name = "sunLedSelectCheckBox63";
            this.sunLedSelectCheckBox63.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox63.TabIndex = 66;
            this.sunLedSelectCheckBox63.Tag = 63;
            this.sunLedSelectCheckBox63.Text = "63";
            this.sunLedSelectCheckBox63.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox63.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox64
            // 
            this.sunLedSelectCheckBox64.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox64.AutoCheck = false;
            this.sunLedSelectCheckBox64.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox64.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox64.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox64.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox64.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox64.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox64.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox64.Location = new System.Drawing.Point(514, 143);
            this.sunLedSelectCheckBox64.Name = "sunLedSelectCheckBox64";
            this.sunLedSelectCheckBox64.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox64.TabIndex = 67;
            this.sunLedSelectCheckBox64.Tag = 64;
            this.sunLedSelectCheckBox64.Text = "64";
            this.sunLedSelectCheckBox64.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox64.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox65
            // 
            this.sunLedSelectCheckBox65.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox65.AutoCheck = false;
            this.sunLedSelectCheckBox65.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox65.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox65.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox65.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox65.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox65.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox65.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox65.Location = new System.Drawing.Point(586, 143);
            this.sunLedSelectCheckBox65.Name = "sunLedSelectCheckBox65";
            this.sunLedSelectCheckBox65.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox65.TabIndex = 68;
            this.sunLedSelectCheckBox65.Tag = 65;
            this.sunLedSelectCheckBox65.Text = "65";
            this.sunLedSelectCheckBox65.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox65.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox66
            // 
            this.sunLedSelectCheckBox66.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox66.AutoCheck = false;
            this.sunLedSelectCheckBox66.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox66.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox66.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox66.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox66.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox66.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox66.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox66.Location = new System.Drawing.Point(658, 143);
            this.sunLedSelectCheckBox66.Name = "sunLedSelectCheckBox66";
            this.sunLedSelectCheckBox66.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox66.TabIndex = 69;
            this.sunLedSelectCheckBox66.Tag = 66;
            this.sunLedSelectCheckBox66.Text = "66";
            this.sunLedSelectCheckBox66.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox66.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox67
            // 
            this.sunLedSelectCheckBox67.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox67.AutoCheck = false;
            this.sunLedSelectCheckBox67.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox67.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox67.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox67.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox67.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox67.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox67.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox67.Location = new System.Drawing.Point(730, 143);
            this.sunLedSelectCheckBox67.Name = "sunLedSelectCheckBox67";
            this.sunLedSelectCheckBox67.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox67.TabIndex = 70;
            this.sunLedSelectCheckBox67.Tag = 67;
            this.sunLedSelectCheckBox67.Text = "67";
            this.sunLedSelectCheckBox67.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox67.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox68
            // 
            this.sunLedSelectCheckBox68.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox68.AutoCheck = false;
            this.sunLedSelectCheckBox68.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox68.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox68.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox68.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox68.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox68.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox68.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox68.Location = new System.Drawing.Point(802, 143);
            this.sunLedSelectCheckBox68.Name = "sunLedSelectCheckBox68";
            this.sunLedSelectCheckBox68.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox68.TabIndex = 71;
            this.sunLedSelectCheckBox68.Tag = 68;
            this.sunLedSelectCheckBox68.Text = "68";
            this.sunLedSelectCheckBox68.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox68.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox69
            // 
            this.sunLedSelectCheckBox69.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox69.AutoCheck = false;
            this.sunLedSelectCheckBox69.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox69.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox69.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox69.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox69.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox69.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox69.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox69.Location = new System.Drawing.Point(874, 143);
            this.sunLedSelectCheckBox69.Name = "sunLedSelectCheckBox69";
            this.sunLedSelectCheckBox69.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox69.TabIndex = 72;
            this.sunLedSelectCheckBox69.Tag = 69;
            this.sunLedSelectCheckBox69.Text = "69";
            this.sunLedSelectCheckBox69.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox69.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox70
            // 
            this.sunLedSelectCheckBox70.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox70.AutoCheck = false;
            this.sunLedSelectCheckBox70.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox70.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox70.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox70.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox70.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox70.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox70.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox70.Location = new System.Drawing.Point(946, 143);
            this.sunLedSelectCheckBox70.Name = "sunLedSelectCheckBox70";
            this.sunLedSelectCheckBox70.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox70.TabIndex = 73;
            this.sunLedSelectCheckBox70.Tag = 70;
            this.sunLedSelectCheckBox70.Text = "70";
            this.sunLedSelectCheckBox70.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox70.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox71
            // 
            this.sunLedSelectCheckBox71.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox71.AutoCheck = false;
            this.sunLedSelectCheckBox71.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox71.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox71.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox71.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox71.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox71.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox71.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox71.Location = new System.Drawing.Point(10, 166);
            this.sunLedSelectCheckBox71.Name = "sunLedSelectCheckBox71";
            this.sunLedSelectCheckBox71.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox71.TabIndex = 74;
            this.sunLedSelectCheckBox71.Tag = 71;
            this.sunLedSelectCheckBox71.Text = "71";
            this.sunLedSelectCheckBox71.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox71.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox72
            // 
            this.sunLedSelectCheckBox72.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox72.AutoCheck = false;
            this.sunLedSelectCheckBox72.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox72.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox72.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox72.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox72.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox72.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox72.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox72.Location = new System.Drawing.Point(82, 166);
            this.sunLedSelectCheckBox72.Name = "sunLedSelectCheckBox72";
            this.sunLedSelectCheckBox72.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox72.TabIndex = 75;
            this.sunLedSelectCheckBox72.Tag = 72;
            this.sunLedSelectCheckBox72.Text = "72";
            this.sunLedSelectCheckBox72.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox72.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox73
            // 
            this.sunLedSelectCheckBox73.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox73.AutoCheck = false;
            this.sunLedSelectCheckBox73.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox73.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox73.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox73.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox73.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox73.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox73.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox73.Location = new System.Drawing.Point(154, 166);
            this.sunLedSelectCheckBox73.Name = "sunLedSelectCheckBox73";
            this.sunLedSelectCheckBox73.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox73.TabIndex = 76;
            this.sunLedSelectCheckBox73.Tag = 73;
            this.sunLedSelectCheckBox73.Text = "73";
            this.sunLedSelectCheckBox73.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox73.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox74
            // 
            this.sunLedSelectCheckBox74.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox74.AutoCheck = false;
            this.sunLedSelectCheckBox74.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox74.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox74.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox74.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox74.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox74.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox74.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox74.Location = new System.Drawing.Point(226, 166);
            this.sunLedSelectCheckBox74.Name = "sunLedSelectCheckBox74";
            this.sunLedSelectCheckBox74.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox74.TabIndex = 77;
            this.sunLedSelectCheckBox74.Tag = 74;
            this.sunLedSelectCheckBox74.Text = "74";
            this.sunLedSelectCheckBox74.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox74.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox75
            // 
            this.sunLedSelectCheckBox75.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox75.AutoCheck = false;
            this.sunLedSelectCheckBox75.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox75.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox75.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox75.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox75.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox75.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox75.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox75.Location = new System.Drawing.Point(298, 166);
            this.sunLedSelectCheckBox75.Name = "sunLedSelectCheckBox75";
            this.sunLedSelectCheckBox75.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox75.TabIndex = 78;
            this.sunLedSelectCheckBox75.Tag = 75;
            this.sunLedSelectCheckBox75.Text = "75";
            this.sunLedSelectCheckBox75.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox75.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox76
            // 
            this.sunLedSelectCheckBox76.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox76.AutoCheck = false;
            this.sunLedSelectCheckBox76.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox76.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox76.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox76.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox76.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox76.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox76.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox76.Location = new System.Drawing.Point(370, 166);
            this.sunLedSelectCheckBox76.Name = "sunLedSelectCheckBox76";
            this.sunLedSelectCheckBox76.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox76.TabIndex = 79;
            this.sunLedSelectCheckBox76.Tag = 76;
            this.sunLedSelectCheckBox76.Text = "76";
            this.sunLedSelectCheckBox76.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox76.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox77
            // 
            this.sunLedSelectCheckBox77.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox77.AutoCheck = false;
            this.sunLedSelectCheckBox77.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox77.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox77.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox77.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox77.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox77.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox77.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox77.Location = new System.Drawing.Point(442, 166);
            this.sunLedSelectCheckBox77.Name = "sunLedSelectCheckBox77";
            this.sunLedSelectCheckBox77.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox77.TabIndex = 80;
            this.sunLedSelectCheckBox77.Tag = 77;
            this.sunLedSelectCheckBox77.Text = "77";
            this.sunLedSelectCheckBox77.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox77.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox78
            // 
            this.sunLedSelectCheckBox78.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox78.AutoCheck = false;
            this.sunLedSelectCheckBox78.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox78.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox78.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox78.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox78.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox78.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox78.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox78.Location = new System.Drawing.Point(514, 166);
            this.sunLedSelectCheckBox78.Name = "sunLedSelectCheckBox78";
            this.sunLedSelectCheckBox78.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox78.TabIndex = 81;
            this.sunLedSelectCheckBox78.Tag = 78;
            this.sunLedSelectCheckBox78.Text = "78";
            this.sunLedSelectCheckBox78.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox78.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox79
            // 
            this.sunLedSelectCheckBox79.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox79.AutoCheck = false;
            this.sunLedSelectCheckBox79.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox79.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox79.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox79.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox79.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox79.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox79.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox79.Location = new System.Drawing.Point(586, 166);
            this.sunLedSelectCheckBox79.Name = "sunLedSelectCheckBox79";
            this.sunLedSelectCheckBox79.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox79.TabIndex = 82;
            this.sunLedSelectCheckBox79.Tag = 79;
            this.sunLedSelectCheckBox79.Text = "79";
            this.sunLedSelectCheckBox79.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox79.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox80
            // 
            this.sunLedSelectCheckBox80.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox80.AutoCheck = false;
            this.sunLedSelectCheckBox80.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox80.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox80.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox80.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox80.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox80.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox80.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox80.Location = new System.Drawing.Point(658, 166);
            this.sunLedSelectCheckBox80.Name = "sunLedSelectCheckBox80";
            this.sunLedSelectCheckBox80.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox80.TabIndex = 83;
            this.sunLedSelectCheckBox80.Tag = 80;
            this.sunLedSelectCheckBox80.Text = "80";
            this.sunLedSelectCheckBox80.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox80.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox81
            // 
            this.sunLedSelectCheckBox81.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox81.AutoCheck = false;
            this.sunLedSelectCheckBox81.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox81.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox81.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox81.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox81.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox81.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox81.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox81.Location = new System.Drawing.Point(730, 166);
            this.sunLedSelectCheckBox81.Name = "sunLedSelectCheckBox81";
            this.sunLedSelectCheckBox81.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox81.TabIndex = 84;
            this.sunLedSelectCheckBox81.Tag = 81;
            this.sunLedSelectCheckBox81.Text = "81";
            this.sunLedSelectCheckBox81.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox81.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox82
            // 
            this.sunLedSelectCheckBox82.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox82.AutoCheck = false;
            this.sunLedSelectCheckBox82.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox82.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox82.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox82.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox82.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox82.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox82.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox82.Location = new System.Drawing.Point(802, 166);
            this.sunLedSelectCheckBox82.Name = "sunLedSelectCheckBox82";
            this.sunLedSelectCheckBox82.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox82.TabIndex = 85;
            this.sunLedSelectCheckBox82.Tag = 82;
            this.sunLedSelectCheckBox82.Text = "82";
            this.sunLedSelectCheckBox82.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox82.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox83
            // 
            this.sunLedSelectCheckBox83.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox83.AutoCheck = false;
            this.sunLedSelectCheckBox83.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox83.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox83.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox83.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox83.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox83.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox83.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox83.Location = new System.Drawing.Point(874, 166);
            this.sunLedSelectCheckBox83.Name = "sunLedSelectCheckBox83";
            this.sunLedSelectCheckBox83.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox83.TabIndex = 86;
            this.sunLedSelectCheckBox83.Tag = 83;
            this.sunLedSelectCheckBox83.Text = "83";
            this.sunLedSelectCheckBox83.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox83.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectCheckBox84
            // 
            this.sunLedSelectCheckBox84.Appearance = System.Windows.Forms.Appearance.Button;
            this.sunLedSelectCheckBox84.AutoCheck = false;
            this.sunLedSelectCheckBox84.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(50)))), ((int)(((byte)(65)))));
            this.sunLedSelectCheckBox84.Cursor = System.Windows.Forms.Cursors.Hand;
            this.sunLedSelectCheckBox84.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(120)))));
            this.sunLedSelectCheckBox84.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(125)))), ((int)(((byte)(190)))));
            this.sunLedSelectCheckBox84.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectCheckBox84.Font = new System.Drawing.Font("Consolas", 7.5F, System.Drawing.FontStyle.Bold);
            this.sunLedSelectCheckBox84.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectCheckBox84.Location = new System.Drawing.Point(946, 166);
            this.sunLedSelectCheckBox84.Name = "sunLedSelectCheckBox84";
            this.sunLedSelectCheckBox84.Size = new System.Drawing.Size(64, 20);
            this.sunLedSelectCheckBox84.TabIndex = 87;
            this.sunLedSelectCheckBox84.Tag = 84;
            this.sunLedSelectCheckBox84.Text = "84";
            this.sunLedSelectCheckBox84.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.sunLedSelectCheckBox84.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectAllButton
            // 
            this.sunLedSelectAllButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(115)))), ((int)(((byte)(160)))));
            this.sunLedSelectAllButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectAllButton.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectAllButton.Location = new System.Drawing.Point(10, 20);
            this.sunLedSelectAllButton.Name = "sunLedSelectAllButton";
            this.sunLedSelectAllButton.Size = new System.Drawing.Size(100, 26);
            this.sunLedSelectAllButton.TabIndex = 0;
            this.sunLedSelectAllButton.Text = "전체 선택";
            this.sunLedSelectAllButton.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectClearButton
            // 
            this.sunLedSelectClearButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(75)))), ((int)(((byte)(90)))));
            this.sunLedSelectClearButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectClearButton.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectClearButton.Location = new System.Drawing.Point(115, 20);
            this.sunLedSelectClearButton.Name = "sunLedSelectClearButton";
            this.sunLedSelectClearButton.Size = new System.Drawing.Size(100, 26);
            this.sunLedSelectClearButton.TabIndex = 1;
            this.sunLedSelectClearButton.Text = "전체 해제";
            this.sunLedSelectClearButton.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectInvertButton
            // 
            this.sunLedSelectInvertButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(95)))), ((int)(((byte)(85)))), ((int)(((byte)(125)))));
            this.sunLedSelectInvertButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunLedSelectInvertButton.ForeColor = System.Drawing.Color.White;
            this.sunLedSelectInvertButton.Location = new System.Drawing.Point(220, 20);
            this.sunLedSelectInvertButton.Name = "sunLedSelectInvertButton";
            this.sunLedSelectInvertButton.Size = new System.Drawing.Size(100, 26);
            this.sunLedSelectInvertButton.TabIndex = 2;
            this.sunLedSelectInvertButton.Text = "선택 반전";
            this.sunLedSelectInvertButton.UseVisualStyleBackColor = false;
            // 
            // sunLedSelectHintLabel
            // 
            this.sunLedSelectHintLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(205)))), ((int)(((byte)(225)))));
            this.sunLedSelectHintLabel.Location = new System.Drawing.Point(335, 22);
            this.sunLedSelectHintLabel.Name = "sunLedSelectHintLabel";
            this.sunLedSelectHintLabel.Size = new System.Drawing.Size(685, 22);
            this.sunLedSelectHintLabel.TabIndex = 3;
            this.sunLedSelectHintLabel.Text = "클릭 또는 마우스를 누른 채 이동하면 선택/해제됩니다.";
            this.sunLedSelectHintLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // sunGroupsGroupBox
            // 
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupCheckBox1);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupCheckBox2);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupCheckBox3);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupCheckBox4);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupPointer2);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupBarPanel);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupPointer1);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupPointer3);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupValue1);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupValue2);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupValue3);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupStartLabel);
            this.sunGroupsGroupBox.Controls.Add(this.sunGroupEndLabel);
            this.sunGroupsGroupBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.sunGroupsGroupBox.Location = new System.Drawing.Point(5, 306);
            this.sunGroupsGroupBox.Name = "sunGroupsGroupBox";
            this.sunGroupsGroupBox.Size = new System.Drawing.Size(1035, 135);
            this.sunGroupsGroupBox.TabIndex = 2;
            this.sunGroupsGroupBox.TabStop = false;
            this.sunGroupsGroupBox.Text = "[ 4 Group - 3개 포인터로 범위 설정 ]";
            // 
            // sunGroupCheckBox1
            // 
            this.sunGroupCheckBox1.Checked = true;
            this.sunGroupCheckBox1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.sunGroupCheckBox1.ForeColor = System.Drawing.Color.White;
            this.sunGroupCheckBox1.Location = new System.Drawing.Point(20, 28);
            this.sunGroupCheckBox1.Name = "sunGroupCheckBox1";
            this.sunGroupCheckBox1.Size = new System.Drawing.Size(110, 25);
            this.sunGroupCheckBox1.TabIndex = 0;
            this.sunGroupCheckBox1.Text = "Group 1";
            // 
            // sunGroupCheckBox2
            // 
            this.sunGroupCheckBox2.ForeColor = System.Drawing.Color.White;
            this.sunGroupCheckBox2.Location = new System.Drawing.Point(310, 28);
            this.sunGroupCheckBox2.Name = "sunGroupCheckBox2";
            this.sunGroupCheckBox2.Size = new System.Drawing.Size(110, 25);
            this.sunGroupCheckBox2.TabIndex = 1;
            this.sunGroupCheckBox2.Text = "Group 2";
            // 
            // sunGroupCheckBox3
            // 
            this.sunGroupCheckBox3.ForeColor = System.Drawing.Color.White;
            this.sunGroupCheckBox3.Location = new System.Drawing.Point(560, 28);
            this.sunGroupCheckBox3.Name = "sunGroupCheckBox3";
            this.sunGroupCheckBox3.Size = new System.Drawing.Size(110, 25);
            this.sunGroupCheckBox3.TabIndex = 2;
            this.sunGroupCheckBox3.Text = "Group 3";
            // 
            // sunGroupCheckBox4
            // 
            this.sunGroupCheckBox4.ForeColor = System.Drawing.Color.White;
            this.sunGroupCheckBox4.Location = new System.Drawing.Point(808, 28);
            this.sunGroupCheckBox4.Name = "sunGroupCheckBox4";
            this.sunGroupCheckBox4.Size = new System.Drawing.Size(110, 25);
            this.sunGroupCheckBox4.TabIndex = 3;
            this.sunGroupCheckBox4.Text = "Group 4";
            // 
            // sunGroupPointer2
            // 
            this.sunGroupPointer2.Cursor = System.Windows.Forms.Cursors.SizeWE;
            this.sunGroupPointer2.ForeColor = System.Drawing.Color.White;
            this.sunGroupPointer2.Location = new System.Drawing.Point(525, 59);
            this.sunGroupPointer2.Name = "sunGroupPointer2";
            this.sunGroupPointer2.Size = new System.Drawing.Size(20, 20);
            this.sunGroupPointer2.TabIndex = 6;
            this.sunGroupPointer2.Tag = 1;
            this.sunGroupPointer2.Text = "▼";
            // 
            // sunGroupBarPanel
            // 
            this.sunGroupBarPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(46)))));
            this.sunGroupBarPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.sunGroupBarPanel.Controls.Add(this.sunGroupSegment1);
            this.sunGroupBarPanel.Controls.Add(this.sunGroupSegment2);
            this.sunGroupBarPanel.Controls.Add(this.sunGroupSegment4);
            this.sunGroupBarPanel.Controls.Add(this.sunGroupSegment3);
            this.sunGroupBarPanel.Location = new System.Drawing.Point(14, 82);
            this.sunGroupBarPanel.Name = "sunGroupBarPanel";
            this.sunGroupBarPanel.Size = new System.Drawing.Size(1010, 22);
            this.sunGroupBarPanel.TabIndex = 4;
            // 
            // sunGroupSegment1
            // 
            this.sunGroupSegment1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(55)))), ((int)(((byte)(125)))), ((int)(((byte)(180)))));
            this.sunGroupSegment1.Location = new System.Drawing.Point(5, 1);
            this.sunGroupSegment1.Name = "sunGroupSegment1";
            this.sunGroupSegment1.Size = new System.Drawing.Size(283, 20);
            this.sunGroupSegment1.TabIndex = 0;
            // 
            // sunGroupSegment2
            // 
            this.sunGroupSegment2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(65)))), ((int)(((byte)(150)))), ((int)(((byte)(115)))));
            this.sunGroupSegment2.Location = new System.Drawing.Point(288, 1);
            this.sunGroupSegment2.Name = "sunGroupSegment2";
            this.sunGroupSegment2.Size = new System.Drawing.Size(242, 20);
            this.sunGroupSegment2.TabIndex = 1;
            // 
            // sunGroupSegment4
            // 
            this.sunGroupSegment4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(145)))), ((int)(((byte)(80)))), ((int)(((byte)(155)))));
            this.sunGroupSegment4.Location = new System.Drawing.Point(772, 1);
            this.sunGroupSegment4.Name = "sunGroupSegment4";
            this.sunGroupSegment4.Size = new System.Drawing.Size(241, 20);
            this.sunGroupSegment4.TabIndex = 3;
            // 
            // sunGroupSegment3
            // 
            this.sunGroupSegment3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(165)))), ((int)(((byte)(125)))), ((int)(((byte)(55)))));
            this.sunGroupSegment3.Location = new System.Drawing.Point(530, 1);
            this.sunGroupSegment3.Name = "sunGroupSegment3";
            this.sunGroupSegment3.Size = new System.Drawing.Size(242, 20);
            this.sunGroupSegment3.TabIndex = 2;
            // 
            // sunGroupPointer1
            // 
            this.sunGroupPointer1.Cursor = System.Windows.Forms.Cursors.SizeWE;
            this.sunGroupPointer1.ForeColor = System.Drawing.Color.White;
            this.sunGroupPointer1.Location = new System.Drawing.Point(283, 59);
            this.sunGroupPointer1.Name = "sunGroupPointer1";
            this.sunGroupPointer1.Size = new System.Drawing.Size(20, 20);
            this.sunGroupPointer1.TabIndex = 5;
            this.sunGroupPointer1.Tag = 0;
            this.sunGroupPointer1.Text = "▼";
            // 
            // sunGroupPointer3
            // 
            this.sunGroupPointer3.Cursor = System.Windows.Forms.Cursors.SizeWE;
            this.sunGroupPointer3.ForeColor = System.Drawing.Color.White;
            this.sunGroupPointer3.Location = new System.Drawing.Point(767, 62);
            this.sunGroupPointer3.Name = "sunGroupPointer3";
            this.sunGroupPointer3.Size = new System.Drawing.Size(20, 20);
            this.sunGroupPointer3.TabIndex = 7;
            this.sunGroupPointer3.Tag = 2;
            this.sunGroupPointer3.Text = "▼";
            // 
            // sunGroupValue1
            // 
            this.sunGroupValue1.ForeColor = System.Drawing.Color.White;
            this.sunGroupValue1.Location = new System.Drawing.Point(273, 37);
            this.sunGroupValue1.Name = "sunGroupValue1";
            this.sunGroupValue1.Size = new System.Drawing.Size(40, 22);
            this.sunGroupValue1.TabIndex = 8;
            this.sunGroupValue1.Text = "7";
            this.sunGroupValue1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // sunGroupValue2
            // 
            this.sunGroupValue2.ForeColor = System.Drawing.Color.White;
            this.sunGroupValue2.Location = new System.Drawing.Point(514, 37);
            this.sunGroupValue2.Name = "sunGroupValue2";
            this.sunGroupValue2.Size = new System.Drawing.Size(40, 22);
            this.sunGroupValue2.TabIndex = 9;
            this.sunGroupValue2.Text = "13";
            this.sunGroupValue2.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // sunGroupValue3
            // 
            this.sunGroupValue3.ForeColor = System.Drawing.Color.White;
            this.sunGroupValue3.Location = new System.Drawing.Point(762, 37);
            this.sunGroupValue3.Name = "sunGroupValue3";
            this.sunGroupValue3.Size = new System.Drawing.Size(40, 22);
            this.sunGroupValue3.TabIndex = 10;
            this.sunGroupValue3.Text = "19";
            this.sunGroupValue3.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // sunGroupStartLabel
            // 
            this.sunGroupStartLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(225)))), ((int)(((byte)(235)))));
            this.sunGroupStartLabel.Location = new System.Drawing.Point(10, 104);
            this.sunGroupStartLabel.Name = "sunGroupStartLabel";
            this.sunGroupStartLabel.Size = new System.Drawing.Size(100, 22);
            this.sunGroupStartLabel.TabIndex = 11;
            this.sunGroupStartLabel.Text = "Start 1";
            // 
            // sunGroupEndLabel
            // 
            this.sunGroupEndLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(225)))), ((int)(((byte)(235)))));
            this.sunGroupEndLabel.Location = new System.Drawing.Point(920, 104);
            this.sunGroupEndLabel.Name = "sunGroupEndLabel";
            this.sunGroupEndLabel.Size = new System.Drawing.Size(100, 22);
            this.sunGroupEndLabel.TabIndex = 12;
            this.sunGroupEndLabel.Text = "End 25";
            this.sunGroupEndLabel.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // sunReadGroupBox
            // 
            this.sunReadGroupBox.Controls.Add(this.sunReadButton);
            this.sunReadGroupBox.Controls.Add(this.sunStatusLabel);
            this.sunReadGroupBox.Controls.Add(this.sunReadGrid);
            this.sunReadGroupBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.sunReadGroupBox.Location = new System.Drawing.Point(5, 447);
            this.sunReadGroupBox.Name = "sunReadGroupBox";
            this.sunReadGroupBox.Size = new System.Drawing.Size(1035, 297);
            this.sunReadGroupBox.TabIndex = 3;
            this.sunReadGroupBox.TabStop = false;
            this.sunReadGroupBox.Text = "[ LED Read / Open·Short 진단 ]";
            // 
            // sunReadButton
            // 
            this.sunReadButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(65)))), ((int)(((byte)(125)))), ((int)(((byte)(85)))));
            this.sunReadButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sunReadButton.ForeColor = System.Drawing.Color.White;
            this.sunReadButton.Location = new System.Drawing.Point(10, 21);
            this.sunReadButton.Name = "sunReadButton";
            this.sunReadButton.Size = new System.Drawing.Size(190, 34);
            this.sunReadButton.TabIndex = 0;
            this.sunReadButton.Text = "전체 Scan + Read";
            this.sunReadButton.UseVisualStyleBackColor = false;
            // 
            // sunStatusLabel
            // 
            this.sunStatusLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(190)))), ((int)(((byte)(205)))));
            this.sunStatusLabel.Location = new System.Drawing.Point(215, 26);
            this.sunStatusLabel.Name = "sunStatusLabel";
            this.sunStatusLabel.Size = new System.Drawing.Size(805, 25);
            this.sunStatusLabel.TabIndex = 1;
            this.sunStatusLabel.Text = "대기";
            // 
            // sunReadGrid
            // 
            this.sunReadGrid.AllowUserToAddRows = false;
            this.sunReadGrid.AllowUserToDeleteRows = false;
            this.sunReadGrid.AllowUserToOrderColumns = true;
            this.sunReadGrid.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.sunReadGrid.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(25)))), ((int)(((byte)(28)))), ((int)(((byte)(38)))));
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.WhiteSmoke;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Consolas", 9F);
            dataGridViewCellStyle1.ForeColor = System.Drawing.Color.Black;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.sunReadGrid.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.sunReadGrid.ColumnHeadersHeight = 38;
            this.sunReadGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.sunReadGrid.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Type,
            this.LED,
            this.TuneR,
            this.TuneG,
            this.TuneB,
            this.PWMR,
            this.PWMG,
            this.PWMB,
            this.Temp,
            this.Open,
            this.Short,
            this.Status,
            this.OTPCathode});
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle2.BackColor = System.Drawing.Color.White;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Consolas", 9F);
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.Black;
            dataGridViewCellStyle2.Padding = new System.Windows.Forms.Padding(2, 0, 2, 0);
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.Color.LightSteelBlue;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.Color.Black;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.sunReadGrid.DefaultCellStyle = dataGridViewCellStyle2;
            this.sunReadGrid.EnableHeadersVisualStyles = false;
            this.sunReadGrid.GridColor = System.Drawing.Color.Silver;
            this.sunReadGrid.Location = new System.Drawing.Point(10, 69);
            this.sunReadGrid.Name = "sunReadGrid";
            this.sunReadGrid.ReadOnly = true;
            this.sunReadGrid.RowHeadersVisible = false;
            this.sunReadGrid.RowHeadersWidth = 62;
            this.sunReadGrid.RowTemplate.Height = 24;
            this.sunReadGrid.Size = new System.Drawing.Size(1010, 224);
            this.sunReadGrid.TabIndex = 2;
            // 
            // Type
            // 
            this.Type.FillWeight = 95F;
            this.Type.HeaderText = "Path / FID";
            this.Type.MinimumWidth = 80;
            this.Type.Name = "Type";
            this.Type.ReadOnly = true;
            // 
            // LED
            // 
            this.LED.FillWeight = 45F;
            this.LED.HeaderText = "LED";
            this.LED.MinimumWidth = 40;
            this.LED.Name = "LED";
            this.LED.ReadOnly = true;
            // 
            // TuneR
            // 
            this.TuneR.FillWeight = 62F;
            this.TuneR.HeaderText = "Tune R";
            this.TuneR.MinimumWidth = 55;
            this.TuneR.Name = "TuneR";
            this.TuneR.ReadOnly = true;
            // 
            // TuneG
            // 
            this.TuneG.FillWeight = 62F;
            this.TuneG.HeaderText = "Tune G";
            this.TuneG.MinimumWidth = 55;
            this.TuneG.Name = "TuneG";
            this.TuneG.ReadOnly = true;
            // 
            // TuneB
            // 
            this.TuneB.FillWeight = 62F;
            this.TuneB.HeaderText = "Tune B";
            this.TuneB.MinimumWidth = 55;
            this.TuneB.Name = "TuneB";
            this.TuneB.ReadOnly = true;
            // 
            // PWMR
            // 
            this.PWMR.FillWeight = 55F;
            this.PWMR.HeaderText = "PWM R";
            this.PWMR.MinimumWidth = 48;
            this.PWMR.Name = "PWMR";
            this.PWMR.ReadOnly = true;
            // 
            // PWMG
            // 
            this.PWMG.FillWeight = 55F;
            this.PWMG.HeaderText = "PWM G";
            this.PWMG.MinimumWidth = 48;
            this.PWMG.Name = "PWMG";
            this.PWMG.ReadOnly = true;
            // 
            // PWMB
            // 
            this.PWMB.FillWeight = 55F;
            this.PWMB.HeaderText = "PWM B";
            this.PWMB.MinimumWidth = 48;
            this.PWMB.Name = "PWMB";
            this.PWMB.ReadOnly = true;
            // 
            // Temp
            // 
            this.Temp.FillWeight = 90F;
            this.Temp.HeaderText = "Temperature";
            this.Temp.MinimumWidth = 80;
            this.Temp.Name = "Temp";
            this.Temp.ReadOnly = true;
            // 
            // Open
            // 
            this.Open.FillWeight = 55F;
            this.Open.HeaderText = "Open";
            this.Open.MinimumWidth = 50;
            this.Open.Name = "Open";
            this.Open.ReadOnly = true;
            // 
            // Short
            // 
            this.Short.FillWeight = 55F;
            this.Short.HeaderText = "Short";
            this.Short.MinimumWidth = 50;
            this.Short.Name = "Short";
            this.Short.ReadOnly = true;
            // 
            // Status
            // 
            this.Status.FillWeight = 125F;
            this.Status.HeaderText = "Status";
            this.Status.MinimumWidth = 95;
            this.Status.Name = "Status";
            this.Status.ReadOnly = true;
            // 
            // OTPCathode
            // 
            this.OTPCathode.FillWeight = 230F;
            this.OTPCathode.HeaderText = "Detail";
            this.OTPCathode.MinimumWidth = 150;
            this.OTPCathode.Name = "OTPCathode";
            this.OTPCathode.ReadOnly = true;
            // 
            // FormCIE
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 22F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(14)))), ((int)(((byte)(16)))), ((int)(((byte)(24)))));
            this.ClientSize = new System.Drawing.Size(2270, 756);
            this.Controls.Add(this.sunshinePanel);
            this.Controls.Add(this.tabColor);
            this.Controls.Add(this.pnlColorPatch);
            this.Controls.Add(this.lblPatchL);
            this.Controls.Add(this.chkOspTxEnable);
            this.Controls.Add(this.chkIlasTxEnable);
            this.Controls.Add(this.chkRxEnable);
            this.Controls.Add(this.grpFID);
            this.Controls.Add(this.grpSF);
            this.Controls.Add(this.grpColor);
            this.Controls.Add(this.grpFade);
            this.Controls.Add(this.grpStatus);
            this.Controls.Add(this.grpPreview);
            this.Controls.Add(this.grpTouch);
            this.Controls.Add(this.grpTX);
            this.Controls.Add(this.lblFT4222Status);
            this.Font = new System.Drawing.Font("Consolas", 9F);
            this.MinimumSize = new System.Drawing.Size(900, 580);
            this.Name = "FormCIE";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Mood Lamp LIN TX  (MLMMn_Comm1)  |  IndieMicro LIN Gateway V2.0";
            this.tabColor.ResumeLayout(false);
            this.tabCIE.ResumeLayout(false);
            this.pnlCIEBar.ResumeLayout(false);
            this.pnlCIEBar.PerformLayout();
            this.tabImg.ResumeLayout(false);
            this.pnlImgBar.ResumeLayout(false);
            this.grpFID.ResumeLayout(false);
            this.grpFID.PerformLayout();
            this.grpSF.ResumeLayout(false);
            this.grpSF.PerformLayout();
            this.grpShift.ResumeLayout(false);
            this.grpShift.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudShiftWidth)).EndInit();
            this.grpColor.ResumeLayout(false);
            this.grpColor.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBrightness)).EndInit();
            this.grpFade.ResumeLayout(false);
            this.grpFade.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeIn)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeOut)).EndInit();
            this.grpStatus.ResumeLayout(false);
            this.grpStatus.PerformLayout();
            this.grpPreview.ResumeLayout(false);
            this.grpPreview.PerformLayout();
            this.grpTouch.ResumeLayout(false);
            this.grpTouch.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.touchTrackBar)).EndInit();
            this.grpTX.ResumeLayout(false);
            this.grpTX.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudInterval)).EndInit();
            this.sunshinePanel.ResumeLayout(false);
            this.sunSelectionGroupBox.ResumeLayout(false);
            this.sunSelectionGroupBox.PerformLayout();
            this.sunOspCurrentModeGroupBox.ResumeLayout(false);
            this.sunLedSelectionGroupBox.ResumeLayout(false);
            this.sunGroupsGroupBox.ResumeLayout(false);
            this.sunGroupBarPanel.ResumeLayout(false);
            this.sunReadGroupBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.sunReadGrid)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }
        #endregion

        private System.Windows.Forms.TabControl     tabColor;
        private System.Windows.Forms.TabPage        tabCIE;
        private System.Windows.Forms.TabPage        tabImg;
        private System.Windows.Forms.Panel          pnlCIE;
        private System.Windows.Forms.Panel          pnlCIEBar;
        private System.Windows.Forms.Label          lblModeL;
        private System.Windows.Forms.ComboBox       cmbMode;
        private System.Windows.Forms.Label          lblCIEMode;
        private System.Windows.Forms.Label          lblSelXY;
        private System.Windows.Forms.Label          lblCursor;
        private System.Windows.Forms.Panel          pnlImage;
        private System.Windows.Forms.Panel          pnlImgBar;
        private System.Windows.Forms.Button         btnLoadImg;
        private System.Windows.Forms.Label          lblImgInfo;
        private System.Windows.Forms.Label          lblImgColor;
        private System.Windows.Forms.Panel          pnlColorPatch;
        private System.Windows.Forms.Label          lblPatchL;
        private System.Windows.Forms.CheckBox       chkOspTxEnable;
        private System.Windows.Forms.CheckBox       chkIlasTxEnable;
        private System.Windows.Forms.CheckBox       chkRxEnable;
        private System.Windows.Forms.GroupBox       grpFID;
        private System.Windows.Forms.Label          lblFID;
        private System.Windows.Forms.ComboBox       cmbFID;
        private System.Windows.Forms.TextBox        txtManualFID;
        private System.Windows.Forms.Label          lblCS;
        private System.Windows.Forms.ComboBox       cmbCS;
        private System.Windows.Forms.GroupBox       grpSF;
        private System.Windows.Forms.CheckBox       chkSF1, chkSF2, chkSF3, chkSF4;
        private System.Windows.Forms.CheckBox       chkSF5, chkSF6, chkSF7, chkSF8;
        private System.Windows.Forms.CheckBox       chkSF9,  chkSF10, chkSF11, chkSF12;
        private System.Windows.Forms.CheckBox       chkSF13, chkSF14, chkSF15, chkSF16;
        private System.Windows.Forms.CheckBox       chkSF17, chkSF18, chkSF19, chkSF20;
        private System.Windows.Forms.CheckBox       chkSF21, chkSF22, chkSF23, chkSF24, chkSF25;
        private System.Windows.Forms.Button         btnSFAll;
        private System.Windows.Forms.Button         btnSFNone;
        private System.Windows.Forms.CheckBox       chkRGBWShift;
        private System.Windows.Forms.CheckBox       chkLedShift;
        private System.Windows.Forms.CheckBox       chkShiftStack;
        private System.Windows.Forms.CheckBox       chkLedShiftUp;
        private System.Windows.Forms.CheckBox       chkShiftStackUp;
        private System.Windows.Forms.CheckBox       chkMove;
        private System.Windows.Forms.CheckBox       chkEachControl;
        private System.Windows.Forms.CheckBox       chkTouch;
        private System.Windows.Forms.GroupBox       grpShift;
        private System.Windows.Forms.NumericUpDown  nudShiftWidth;
        private System.Windows.Forms.Label          lblShiftWidth;
        private System.Windows.Forms.GroupBox       grpColor;
        private System.Windows.Forms.Label          lblCX;
        private System.Windows.Forms.NumericUpDown  nudColorX;
        private System.Windows.Forms.Label          lblCY;
        private System.Windows.Forms.NumericUpDown  nudColorY;
        private System.Windows.Forms.Label          lblBr;
        private System.Windows.Forms.NumericUpDown  nudBrightness;
        private System.Windows.Forms.GroupBox       grpFade;
        private System.Windows.Forms.Label          lblFIN;
        private System.Windows.Forms.NumericUpDown  nudFadeIn;
        private System.Windows.Forms.Label          lblFOUT;
        private System.Windows.Forms.NumericUpDown  nudFadeOut;
        private System.Windows.Forms.CheckBox       chkNoFade;
        private System.Windows.Forms.GroupBox       grpStatus;
        private System.Windows.Forms.CheckBox       chkSOH;
        private System.Windows.Forms.CheckBox       chkLINErr;
        private System.Windows.Forms.GroupBox       grpPreview;
        private System.Windows.Forms.TextBox        txtPreview;
        private System.Windows.Forms.Label          lblD1, lblD2, lblD3, lblD4;
        private System.Windows.Forms.Label          lblD5, lblD6, lblD7, lblD8;
        private System.Windows.Forms.GroupBox       grpTouch;
        private System.Windows.Forms.Label          lblTouch;
        private System.Windows.Forms.TrackBar       touchTrackBar;
        private System.Windows.Forms.Label          lblTouchValue;
        private System.Windows.Forms.GroupBox       grpTX;
        private System.Windows.Forms.Label          lblIvL;
        private System.Windows.Forms.NumericUpDown  nudInterval;
        private System.Windows.Forms.Label          lblMs;
        private System.Windows.Forms.Button         btnTxOnce;
        private System.Windows.Forms.Button         btnTxCycle;

        // ── Designer 구성 요소 트레이 (도구상자에서 드래그한 비-UI 컴포넌트) ──
        internal System.Windows.Forms.Timer         txTimer;   // 주기 TX 타이머
        internal System.Windows.Forms.Timer         fadeTimer; // Fade In/Out 타이머

        // ── FT4222 상태 레이블 ─────────────────────────────────────────
        internal System.Windows.Forms.Label         lblFT4222Status;
        private System.Windows.Forms.Panel          sunshinePanel;
        private System.Windows.Forms.GroupBox       sunSelectionGroupBox;
        private System.Windows.Forms.Label          sunTargetLabel;
        private System.Windows.Forms.ComboBox       sunTargetComboBox;
        private System.Windows.Forms.RadioButton    sunAllRadioButton;
        private System.Windows.Forms.RadioButton    sunIndividualRadioButton;
        private System.Windows.Forms.RadioButton    sunGroupRadioButton;
        private System.Windows.Forms.Label          sunIndividualLabel;
        private System.Windows.Forms.TextBox        sunIndividualTextBox;
        private System.Windows.Forms.Button         sunApplyButton;
        private System.Windows.Forms.CheckBox       sunTuningCheckBox;
        private System.Windows.Forms.CheckBox       sunTemperatureCheckBox;
        private System.Windows.Forms.GroupBox       sunOspCurrentModeGroupBox;
        private System.Windows.Forms.RadioButton    sunDayRadioButton;
        private System.Windows.Forms.RadioButton    sunNightRadioButton;
        private System.Windows.Forms.Label          sunHintLabel;
        private System.Windows.Forms.GroupBox       sunLedSelectionGroupBox;
        private System.Windows.Forms.Button         sunLedSelectAllButton;
        private System.Windows.Forms.Button         sunLedSelectClearButton;
        private System.Windows.Forms.Button         sunLedSelectInvertButton;
        private System.Windows.Forms.Label          sunLedSelectHintLabel;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox1;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox2;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox3;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox4;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox5;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox6;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox7;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox8;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox9;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox10;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox11;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox12;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox13;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox14;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox15;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox16;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox17;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox18;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox19;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox20;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox21;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox22;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox23;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox24;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox25;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox26;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox27;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox28;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox29;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox30;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox31;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox32;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox33;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox34;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox35;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox36;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox37;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox38;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox39;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox40;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox41;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox42;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox43;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox44;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox45;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox46;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox47;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox48;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox49;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox50;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox51;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox52;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox53;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox54;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox55;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox56;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox57;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox58;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox59;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox60;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox61;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox62;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox63;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox64;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox65;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox66;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox67;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox68;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox69;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox70;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox71;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox72;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox73;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox74;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox75;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox76;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox77;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox78;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox79;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox80;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox81;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox82;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox83;
        private System.Windows.Forms.CheckBox       sunLedSelectCheckBox84;
        private System.Windows.Forms.GroupBox       sunGroupsGroupBox;
        private System.Windows.Forms.CheckBox       sunGroupCheckBox1;
        private System.Windows.Forms.CheckBox       sunGroupCheckBox2;
        private System.Windows.Forms.CheckBox       sunGroupCheckBox3;
        private System.Windows.Forms.CheckBox       sunGroupCheckBox4;
        private System.Windows.Forms.Panel          sunGroupBarPanel;
        private System.Windows.Forms.Panel          sunGroupSegment1;
        private System.Windows.Forms.Panel          sunGroupSegment2;
        private System.Windows.Forms.Panel          sunGroupSegment3;
        private System.Windows.Forms.Panel          sunGroupSegment4;
        private System.Windows.Forms.Label          sunGroupPointer1;
        private System.Windows.Forms.Label          sunGroupPointer2;
        private System.Windows.Forms.Label          sunGroupPointer3;
        private System.Windows.Forms.Label          sunGroupValue1;
        private System.Windows.Forms.Label          sunGroupValue2;
        private System.Windows.Forms.Label          sunGroupValue3;
        private System.Windows.Forms.Label          sunGroupStartLabel;
        private System.Windows.Forms.Label          sunGroupEndLabel;
        private System.Windows.Forms.GroupBox       sunReadGroupBox;
        private System.Windows.Forms.Button         sunReadButton;
        private System.Windows.Forms.Label          sunStatusLabel;
        private System.Windows.Forms.DataGridView   sunReadGrid;
        private System.Windows.Forms.DataGridViewTextBoxColumn Type;
        private System.Windows.Forms.DataGridViewTextBoxColumn LED;
        private System.Windows.Forms.DataGridViewTextBoxColumn TuneR;
        private System.Windows.Forms.DataGridViewTextBoxColumn TuneG;
        private System.Windows.Forms.DataGridViewTextBoxColumn TuneB;
        private System.Windows.Forms.DataGridViewTextBoxColumn PWMR;
        private System.Windows.Forms.DataGridViewTextBoxColumn PWMG;
        private System.Windows.Forms.DataGridViewTextBoxColumn PWMB;
        private System.Windows.Forms.DataGridViewTextBoxColumn Temp;
        private System.Windows.Forms.DataGridViewTextBoxColumn Open;
        private System.Windows.Forms.DataGridViewTextBoxColumn Short;
        private System.Windows.Forms.DataGridViewTextBoxColumn Status;
        private System.Windows.Forms.DataGridViewTextBoxColumn OTPCathode;
    }
}
