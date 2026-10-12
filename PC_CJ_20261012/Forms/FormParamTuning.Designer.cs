namespace LINMaster.Forms
{
    partial class FormParamTuning
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle7 = new System.Windows.Forms.DataGridViewCellStyle();
            this.pnlTop = new System.Windows.Forms.Panel();
            this.btnLoadMap = new System.Windows.Forms.Button();
            this.lblMapStatus = new System.Windows.Forms.Label();
            this.lblConnStatus = new System.Windows.Forms.Label();
            this.lblNADCap = new System.Windows.Forms.Label();
            this.nudNAD = new System.Windows.Forms.NumericUpDown();
            this.lblSearchCap = new System.Windows.Forms.Label();
            this.txtSearch = new System.Windows.Forms.TextBox();
            this.lblCount = new System.Windows.Forms.Label();
            this.chkUseXcp = new System.Windows.Forms.CheckBox();
            this.lblXcpTx = new System.Windows.Forms.Label();
            this.nudXcpTxId = new System.Windows.Forms.NumericUpDown();
            this.lblXcpRx = new System.Windows.Forms.Label();
            this.nudXcpRxId = new System.Windows.Forms.NumericUpDown();
            this.lblXcpExt = new System.Windows.Forms.Label();
            this.nudXcpAddrExt = new System.Windows.Forms.NumericUpDown();
            this.dgvSymbols = new System.Windows.Forms.DataGridView();
            this.colName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colAddr = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colSize = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colValue = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.grpFavorites = new System.Windows.Forms.GroupBox();
            this.dgvFavorites = new System.Windows.Forms.DataGridView();
            this.favColName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.favColAddr = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.favColSize = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.favColValue = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.btnFavAdd = new System.Windows.Forms.Button();
            this.btnFavRemove = new System.Windows.Forms.Button();
            this.btnFavReadAll = new System.Windows.Forms.Button();
            this.grpReadWrite = new System.Windows.Forms.GroupBox();
            this.lblCurValCap = new System.Windows.Forms.Label();
            this.lblCurrentVal = new System.Windows.Forms.Label();
            this.btnRead = new System.Windows.Forms.Button();
            this.lblWriteCap = new System.Windows.Forms.Label();
            this.txtWriteVal = new System.Windows.Forms.TextBox();
            this.btnHexDec = new System.Windows.Forms.Button();
            this.btnWrite = new System.Windows.Forms.Button();
            this.lblHint = new System.Windows.Forms.Label();
            this.grpDetail = new System.Windows.Forms.GroupBox();
            this.lblSelNameCap = new System.Windows.Forms.Label();
            this.lblSelName = new System.Windows.Forms.Label();
            this.lblSelAddrCap = new System.Windows.Forms.Label();
            this.lblSelAddr = new System.Windows.Forms.Label();
            this.lblSelSizeCap = new System.Windows.Forms.Label();
            this.lblSelSize = new System.Windows.Forms.Label();
            this.pnlBottom = new System.Windows.Forms.Panel();
            this.lblStatus = new System.Windows.Forms.Label();
            this.btnClose = new System.Windows.Forms.Button();
            this.tabXcpMain = new System.Windows.Forms.TabControl();
            this.tabPageDaq = new System.Windows.Forms.TabPage();
            this.grpDaq = new System.Windows.Forms.GroupBox();
            this.dgvDaq = new System.Windows.Forms.DataGridView();
            this.daqName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.daqAddr = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.daqSize = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.daqValue = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.btnDaqAdd = new System.Windows.Forms.Button();
            this.btnDaqRemove = new System.Windows.Forms.Button();
            this.lblDaqStatus = new System.Windows.Forms.Label();
            this.tabPageChecksum = new System.Windows.Forms.TabPage();
            this.grpChecksum = new System.Windows.Forms.GroupBox();
            this.lblCsAddrCap = new System.Windows.Forms.Label();
            this.txtCsAddress = new System.Windows.Forms.TextBox();
            this.lblCsLenCap = new System.Windows.Forms.Label();
            this.txtCsLength = new System.Windows.Forms.TextBox();
            this.lblCsTypeCap = new System.Windows.Forms.Label();
            this.cmbCsType = new System.Windows.Forms.ComboBox();
            this.btnCalcChecksum = new System.Windows.Forms.Button();
            this.lblCsResult = new System.Windows.Forms.Label();
            this.tabPageSeed = new System.Windows.Forms.TabPage();
            this.grpSeedKey = new System.Windows.Forms.GroupBox();
            this.lblSkResource = new System.Windows.Forms.Label();
            this.cmbSkResource = new System.Windows.Forms.ComboBox();
            this.btnGetSeed = new System.Windows.Forms.Button();
            this.lblSeedVal = new System.Windows.Forms.Label();
            this.lblKeyVal = new System.Windows.Forms.Label();
            this.btnUnlock = new System.Windows.Forms.Button();
            this.lblSkStatus = new System.Windows.Forms.Label();
            this.tabPageGraph = new System.Windows.Forms.TabPage();
            this.grpGraph = new System.Windows.Forms.GroupBox();
            this.pnlGraph = new System.Windows.Forms.Panel();
            this.lblGraphVarCap = new System.Windows.Forms.Label();
            this.cmbGraphVar = new System.Windows.Forms.ComboBox();
            this.btnGraphFavStart = new System.Windows.Forms.Button();
            this.btnGraphAdd = new System.Windows.Forms.Button();
            this.btnGraphStart = new System.Windows.Forms.Button();
            this.btnGraphStop = new System.Windows.Forms.Button();
            this.btnGraphClear = new System.Windows.Forms.Button();
            this.chkGraphAutoScale = new System.Windows.Forms.CheckBox();
            this.lblGraphYMin = new System.Windows.Forms.Label();
            this.txtGraphYMin = new System.Windows.Forms.TextBox();
            this.lblGraphYMax = new System.Windows.Forms.Label();
            this.txtGraphYMax = new System.Windows.Forms.TextBox();
            this.chkGraphXAutoScale = new System.Windows.Forms.CheckBox();
            this.lblGraphXMin = new System.Windows.Forms.Label();
            this.txtGraphXMin = new System.Windows.Forms.TextBox();
            this.lblGraphXMax = new System.Windows.Forms.Label();
            this.txtGraphXMax = new System.Windows.Forms.TextBox();
            this.lblGraphStatus = new System.Windows.Forms.Label();
            this.nudGraphInterval = new System.Windows.Forms.NumericUpDown();
            this.lblGraphInterval = new System.Windows.Forms.Label();
            this.pnlTop.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudNAD)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudXcpTxId)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudXcpRxId)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudXcpAddrExt)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSymbols)).BeginInit();
            this.grpFavorites.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvFavorites)).BeginInit();
            this.grpReadWrite.SuspendLayout();
            this.grpDetail.SuspendLayout();
            this.pnlBottom.SuspendLayout();
            this.tabXcpMain.SuspendLayout();
            this.tabPageDaq.SuspendLayout();
            this.grpDaq.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvDaq)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudGraphInterval)).BeginInit();
            this.tabPageChecksum.SuspendLayout();
            this.grpChecksum.SuspendLayout();
            this.tabPageSeed.SuspendLayout();
            this.grpSeedKey.SuspendLayout();
            this.tabPageGraph.SuspendLayout();
            this.grpGraph.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlTop
            // 
            this.pnlTop.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.pnlTop.Controls.Add(this.btnLoadMap);
            this.pnlTop.Controls.Add(this.lblMapStatus);
            this.pnlTop.Controls.Add(this.lblConnStatus);
            this.pnlTop.Controls.Add(this.lblNADCap);
            this.pnlTop.Controls.Add(this.nudNAD);
            this.pnlTop.Controls.Add(this.lblSearchCap);
            this.pnlTop.Controls.Add(this.txtSearch);
            this.pnlTop.Controls.Add(this.lblCount);
            this.pnlTop.Controls.Add(this.chkUseXcp);
            this.pnlTop.Controls.Add(this.lblXcpTx);
            this.pnlTop.Controls.Add(this.nudXcpTxId);
            this.pnlTop.Controls.Add(this.lblXcpRx);
            this.pnlTop.Controls.Add(this.nudXcpRxId);
            this.pnlTop.Controls.Add(this.lblXcpExt);
            this.pnlTop.Controls.Add(this.nudXcpAddrExt);
            this.pnlTop.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlTop.Location = new System.Drawing.Point(0, 0);
            this.pnlTop.Name = "pnlTop";
            this.pnlTop.Size = new System.Drawing.Size(1606, 78);
            this.pnlTop.TabIndex = 1;
            // 
            // btnLoadMap
            // 
            this.btnLoadMap.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(61)))), ((int)(((byte)(133)))), ((int)(((byte)(198)))));
            this.btnLoadMap.FlatAppearance.BorderSize = 0;
            this.btnLoadMap.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnLoadMap.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnLoadMap.ForeColor = System.Drawing.Color.White;
            this.btnLoadMap.Location = new System.Drawing.Point(8, 8);
            this.btnLoadMap.Name = "btnLoadMap";
            this.btnLoadMap.Size = new System.Drawing.Size(150, 28);
            this.btnLoadMap.TabIndex = 0;
            this.btnLoadMap.Text = "심볼 파일 열기";
            this.btnLoadMap.UseVisualStyleBackColor = false;
            this.btnLoadMap.Click += new System.EventHandler(this.btnLoadMap_Click);
            // 
            // lblMapStatus
            // 
            this.lblMapStatus.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblMapStatus.ForeColor = System.Drawing.Color.Silver;
            this.lblMapStatus.Location = new System.Drawing.Point(168, 12);
            this.lblMapStatus.Name = "lblMapStatus";
            this.lblMapStatus.Size = new System.Drawing.Size(460, 22);
            this.lblMapStatus.TabIndex = 1;
            this.lblMapStatus.Text = ".out 또는 .map 파일을 열어주세요.";
            // 
            // lblConnStatus
            // 
            this.lblConnStatus.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblConnStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblConnStatus.Location = new System.Drawing.Point(630, 12);
            this.lblConnStatus.Name = "lblConnStatus";
            this.lblConnStatus.Size = new System.Drawing.Size(120, 22);
            this.lblConnStatus.TabIndex = 2;
            this.lblConnStatus.Text = "○ 연결 안됨";
            // 
            // lblNADCap
            // 
            this.lblNADCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblNADCap.ForeColor = System.Drawing.Color.Silver;
            this.lblNADCap.Location = new System.Drawing.Point(8, 48);
            this.lblNADCap.Name = "lblNADCap";
            this.lblNADCap.Size = new System.Drawing.Size(36, 22);
            this.lblNADCap.TabIndex = 3;
            this.lblNADCap.Text = "NAD:";
            // 
            // nudNAD
            // 
            this.nudNAD.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudNAD.Hexadecimal = true;
            this.nudNAD.Location = new System.Drawing.Point(48, 46);
            this.nudNAD.Maximum = new decimal(new int[] {
            127,
            0,
            0,
            0});
            this.nudNAD.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudNAD.Name = "nudNAD";
            this.nudNAD.Size = new System.Drawing.Size(56, 29);
            this.nudNAD.TabIndex = 4;
            this.nudNAD.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // lblSearchCap
            // 
            this.lblSearchCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSearchCap.ForeColor = System.Drawing.Color.Silver;
            this.lblSearchCap.Location = new System.Drawing.Point(116, 48);
            this.lblSearchCap.Name = "lblSearchCap";
            this.lblSearchCap.Size = new System.Drawing.Size(46, 22);
            this.lblSearchCap.TabIndex = 5;
            this.lblSearchCap.Text = "검색:";
            // 
            // txtSearch
            // 
            this.txtSearch.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(55)))), ((int)(((byte)(65)))));
            this.txtSearch.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtSearch.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtSearch.ForeColor = System.Drawing.Color.White;
            this.txtSearch.Location = new System.Drawing.Point(162, 46);
            this.txtSearch.Name = "txtSearch";
            this.txtSearch.Size = new System.Drawing.Size(200, 29);
            this.txtSearch.TabIndex = 6;
            this.txtSearch.TextChanged += new System.EventHandler(this.txtSearch_TextChanged);
            // 
            // lblCount
            // 
            this.lblCount.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblCount.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.lblCount.Location = new System.Drawing.Point(374, 48);
            this.lblCount.Name = "lblCount";
            this.lblCount.Size = new System.Drawing.Size(200, 22);
            this.lblCount.TabIndex = 7;
            this.lblCount.Text = "0 / 0개";
            // 
            // chkUseXcp
            // 
            this.chkUseXcp.AutoSize = true;
            this.chkUseXcp.BackColor = System.Drawing.Color.Transparent;
            this.chkUseXcp.Checked = true;
            this.chkUseXcp.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkUseXcp.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.chkUseXcp.ForeColor = System.Drawing.Color.Gold;
            this.chkUseXcp.Location = new System.Drawing.Point(635, 45);
            this.chkUseXcp.Name = "chkUseXcp";
            this.chkUseXcp.Size = new System.Drawing.Size(66, 26);
            this.chkUseXcp.TabIndex = 10;
            this.chkUseXcp.Text = "XCP";
            this.chkUseXcp.UseVisualStyleBackColor = false;
            this.chkUseXcp.CheckedChanged += new System.EventHandler(this.chkUseXcp_CheckedChanged);
            // 
            // lblXcpTx
            // 
            this.lblXcpTx.AutoSize = true;
            this.lblXcpTx.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblXcpTx.ForeColor = System.Drawing.Color.Silver;
            this.lblXcpTx.Location = new System.Drawing.Point(685, 47);
            this.lblXcpTx.Name = "lblXcpTx";
            this.lblXcpTx.Size = new System.Drawing.Size(36, 20);
            this.lblXcpTx.TabIndex = 11;
            this.lblXcpTx.Text = "TX:";
            // 
            // nudXcpTxId
            // 
            this.nudXcpTxId.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(55)))), ((int)(((byte)(65)))));
            this.nudXcpTxId.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.nudXcpTxId.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudXcpTxId.ForeColor = System.Drawing.Color.White;
            this.nudXcpTxId.Hexadecimal = true;
            this.nudXcpTxId.Location = new System.Drawing.Point(713, 43);
            this.nudXcpTxId.Maximum = new decimal(new int[] {
            63,
            0,
            0,
            0});
            this.nudXcpTxId.Name = "nudXcpTxId";
            this.nudXcpTxId.Size = new System.Drawing.Size(56, 27);
            this.nudXcpTxId.TabIndex = 12;
            this.nudXcpTxId.Value = new decimal(new int[] {
            60,
            0,
            0,
            0});
            this.nudXcpTxId.ValueChanged += new System.EventHandler(this.nudXcpTxId_ValueChanged);
            // 
            // lblXcpRx
            // 
            this.lblXcpRx.AutoSize = true;
            this.lblXcpRx.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblXcpRx.ForeColor = System.Drawing.Color.Silver;
            this.lblXcpRx.Location = new System.Drawing.Point(775, 47);
            this.lblXcpRx.Name = "lblXcpRx";
            this.lblXcpRx.Size = new System.Drawing.Size(36, 20);
            this.lblXcpRx.TabIndex = 13;
            this.lblXcpRx.Text = "RX:";
            // 
            // nudXcpRxId
            // 
            this.nudXcpRxId.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(55)))), ((int)(((byte)(65)))));
            this.nudXcpRxId.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.nudXcpRxId.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudXcpRxId.ForeColor = System.Drawing.Color.White;
            this.nudXcpRxId.Hexadecimal = true;
            this.nudXcpRxId.Location = new System.Drawing.Point(803, 43);
            this.nudXcpRxId.Maximum = new decimal(new int[] {
            63,
            0,
            0,
            0});
            this.nudXcpRxId.Name = "nudXcpRxId";
            this.nudXcpRxId.Size = new System.Drawing.Size(56, 27);
            this.nudXcpRxId.TabIndex = 14;
            this.nudXcpRxId.Value = new decimal(new int[] {
            61,
            0,
            0,
            0});
            this.nudXcpRxId.ValueChanged += new System.EventHandler(this.nudXcpRxId_ValueChanged);
            // 
            // lblXcpExt
            // 
            this.lblXcpExt.AutoSize = true;
            this.lblXcpExt.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblXcpExt.ForeColor = System.Drawing.Color.Silver;
            this.lblXcpExt.Location = new System.Drawing.Point(865, 47);
            this.lblXcpExt.Name = "lblXcpExt";
            this.lblXcpExt.Size = new System.Drawing.Size(45, 20);
            this.lblXcpExt.TabIndex = 15;
            this.lblXcpExt.Text = "EXT:";
            // 
            // nudXcpAddrExt
            // 
            this.nudXcpAddrExt.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(55)))), ((int)(((byte)(65)))));
            this.nudXcpAddrExt.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.nudXcpAddrExt.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudXcpAddrExt.ForeColor = System.Drawing.Color.White;
            this.nudXcpAddrExt.Hexadecimal = true;
            this.nudXcpAddrExt.Location = new System.Drawing.Point(897, 43);
            this.nudXcpAddrExt.Maximum = new decimal(new int[] {
            63,
            0,
            0,
            0});
            this.nudXcpAddrExt.Name = "nudXcpAddrExt";
            this.nudXcpAddrExt.Size = new System.Drawing.Size(56, 27);
            this.nudXcpAddrExt.TabIndex = 16;
            this.nudXcpAddrExt.ValueChanged += new System.EventHandler(this.nudXcpAddrExt_ValueChanged);
            // 
            // dgvSymbols
            // 
            this.dgvSymbols.AllowUserToAddRows = false;
            this.dgvSymbols.AllowUserToDeleteRows = false;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(35)))), ((int)(((byte)(40)))), ((int)(((byte)(48)))));
            this.dgvSymbols.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.dgvSymbols.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(55)))));
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Consolas", 8.5F);
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.LightSkyBlue;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.dgvSymbols.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle2;
            this.dgvSymbols.ColumnHeadersHeight = 26;
            this.dgvSymbols.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dgvSymbols.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.colName,
            this.colAddr,
            this.colSize,
            this.colValue});
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Consolas", 8.5F);
            dataGridViewCellStyle3.ForeColor = System.Drawing.Color.LightGray;
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(61)))), ((int)(((byte)(133)))), ((int)(((byte)(198)))));
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvSymbols.DefaultCellStyle = dataGridViewCellStyle3;
            this.dgvSymbols.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.dgvSymbols.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(65)))), ((int)(((byte)(75)))));
            this.dgvSymbols.Location = new System.Drawing.Point(12, 84);
            this.dgvSymbols.MultiSelect = false;
            this.dgvSymbols.Name = "dgvSymbols";
            this.dgvSymbols.ReadOnly = true;
            this.dgvSymbols.RowHeadersVisible = false;
            this.dgvSymbols.RowHeadersWidth = 62;
            this.dgvSymbols.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dgvSymbols.Size = new System.Drawing.Size(695, 612);
            this.dgvSymbols.TabIndex = 0;
            this.dgvSymbols.SelectionChanged += new System.EventHandler(this.dgvSymbols_SelectionChanged);
            // 
            // colName
            // 
            this.colName.HeaderText = "변수명";
            this.colName.MinimumWidth = 8;
            this.colName.Name = "colName";
            this.colName.ReadOnly = true;
            this.colName.Width = 350;
            // 
            // colAddr
            // 
            this.colAddr.HeaderText = "주소";
            this.colAddr.MinimumWidth = 8;
            this.colAddr.Name = "colAddr";
            this.colAddr.ReadOnly = true;
            this.colAddr.Width = 150;
            // 
            // colSize
            // 
            this.colSize.HeaderText = "크기(byte)";
            this.colSize.MinimumWidth = 8;
            this.colSize.Name = "colSize";
            this.colSize.ReadOnly = true;
            this.colSize.Width = 80;
            // 
            // colValue
            // 
            this.colValue.HeaderText = "현재값";
            this.colValue.MinimumWidth = 8;
            this.colValue.Name = "colValue";
            this.colValue.ReadOnly = true;
            this.colValue.Width = 90;
            // 
            // grpFavorites
            // 
            this.grpFavorites.Controls.Add(this.dgvFavorites);
            this.grpFavorites.Controls.Add(this.btnFavAdd);
            this.grpFavorites.Controls.Add(this.btnFavRemove);
            this.grpFavorites.Controls.Add(this.btnFavReadAll);
            this.grpFavorites.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpFavorites.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.grpFavorites.Location = new System.Drawing.Point(12, 864);
            this.grpFavorites.Name = "grpFavorites";
            this.grpFavorites.Size = new System.Drawing.Size(350, 221);
            this.grpFavorites.TabIndex = 0;
            this.grpFavorites.TabStop = false;
            this.grpFavorites.Text = " 즐겨찾기 ";
            // 
            // dgvFavorites
            // 
            this.dgvFavorites.AllowUserToAddRows = false;
            this.dgvFavorites.AllowUserToDeleteRows = false;
            this.dgvFavorites.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(55)))));
            dataGridViewCellStyle4.Font = new System.Drawing.Font("Consolas", 8F);
            dataGridViewCellStyle4.ForeColor = System.Drawing.Color.LightSkyBlue;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.dgvFavorites.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle4;
            this.dgvFavorites.ColumnHeadersHeight = 22;
            this.dgvFavorites.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dgvFavorites.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.favColName,
            this.favColAddr,
            this.favColSize,
            this.favColValue});
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            dataGridViewCellStyle5.Font = new System.Drawing.Font("Consolas", 8F);
            dataGridViewCellStyle5.ForeColor = System.Drawing.Color.LightSkyBlue;
            dataGridViewCellStyle5.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(120)))), ((int)(((byte)(60)))));
            dataGridViewCellStyle5.SelectionForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle5.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvFavorites.DefaultCellStyle = dataGridViewCellStyle5;
            this.dgvFavorites.Font = new System.Drawing.Font("Consolas", 8F);
            this.dgvFavorites.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(65)))), ((int)(((byte)(75)))));
            this.dgvFavorites.Location = new System.Drawing.Point(12, 24);
            this.dgvFavorites.MultiSelect = false;
            this.dgvFavorites.Name = "dgvFavorites";
            this.dgvFavorites.ReadOnly = true;
            this.dgvFavorites.RowHeadersVisible = false;
            this.dgvFavorites.RowHeadersWidth = 62;
            this.dgvFavorites.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dgvFavorites.Size = new System.Drawing.Size(328, 154);
            this.dgvFavorites.TabIndex = 0;
            this.dgvFavorites.SelectionChanged += new System.EventHandler(this.dgvFavorites_SelectionChanged);
            // 
            // favColName
            // 
            this.favColName.HeaderText = "변수명";
            this.favColName.MinimumWidth = 8;
            this.favColName.Name = "favColName";
            this.favColName.ReadOnly = true;
            this.favColName.Width = 140;
            // 
            // favColAddr
            // 
            this.favColAddr.HeaderText = "주소";
            this.favColAddr.MinimumWidth = 8;
            this.favColAddr.Name = "favColAddr";
            this.favColAddr.ReadOnly = true;
            this.favColAddr.Width = 90;
            // 
            // favColSize
            // 
            this.favColSize.HeaderText = "크기";
            this.favColSize.MinimumWidth = 8;
            this.favColSize.Name = "favColSize";
            this.favColSize.ReadOnly = true;
            this.favColSize.Width = 46;
            // 
            // favColValue
            // 
            this.favColValue.HeaderText = "값";
            this.favColValue.MinimumWidth = 8;
            this.favColValue.Name = "favColValue";
            this.favColValue.ReadOnly = true;
            this.favColValue.Width = 48;
            // 
            // btnFavAdd
            // 
            this.btnFavAdd.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(100)))), ((int)(((byte)(50)))));
            this.btnFavAdd.FlatAppearance.BorderSize = 0;
            this.btnFavAdd.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnFavAdd.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnFavAdd.ForeColor = System.Drawing.Color.White;
            this.btnFavAdd.Location = new System.Drawing.Point(6, 184);
            this.btnFavAdd.Name = "btnFavAdd";
            this.btnFavAdd.Size = new System.Drawing.Size(90, 26);
            this.btnFavAdd.TabIndex = 1;
            this.btnFavAdd.Text = "★ 추가";
            this.btnFavAdd.UseVisualStyleBackColor = false;
            this.btnFavAdd.Click += new System.EventHandler(this.btnFavAdd_Click);
            // 
            // btnFavRemove
            // 
            this.btnFavRemove.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(50)))), ((int)(((byte)(50)))));
            this.btnFavRemove.FlatAppearance.BorderSize = 0;
            this.btnFavRemove.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnFavRemove.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnFavRemove.ForeColor = System.Drawing.Color.White;
            this.btnFavRemove.Location = new System.Drawing.Point(102, 184);
            this.btnFavRemove.Name = "btnFavRemove";
            this.btnFavRemove.Size = new System.Drawing.Size(90, 26);
            this.btnFavRemove.TabIndex = 2;
            this.btnFavRemove.Text = "✕ 제거";
            this.btnFavRemove.UseVisualStyleBackColor = false;
            this.btnFavRemove.Click += new System.EventHandler(this.btnFavRemove_Click);
            // 
            // btnFavReadAll
            // 
            this.btnFavReadAll.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(80)))), ((int)(((byte)(110)))));
            this.btnFavReadAll.FlatAppearance.BorderSize = 0;
            this.btnFavReadAll.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnFavReadAll.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnFavReadAll.ForeColor = System.Drawing.Color.White;
            this.btnFavReadAll.Location = new System.Drawing.Point(198, 184);
            this.btnFavReadAll.Name = "btnFavReadAll";
            this.btnFavReadAll.Size = new System.Drawing.Size(136, 26);
            this.btnFavReadAll.TabIndex = 3;
            this.btnFavReadAll.Text = "⟳ 전체 READ";
            this.btnFavReadAll.UseVisualStyleBackColor = false;
            this.btnFavReadAll.Click += new System.EventHandler(this.btnFavReadAll_Click);
            // 
            // grpReadWrite
            // 
            this.grpReadWrite.Controls.Add(this.lblCurValCap);
            this.grpReadWrite.Controls.Add(this.lblCurrentVal);
            this.grpReadWrite.Controls.Add(this.btnRead);
            this.grpReadWrite.Controls.Add(this.lblWriteCap);
            this.grpReadWrite.Controls.Add(this.txtWriteVal);
            this.grpReadWrite.Controls.Add(this.btnHexDec);
            this.grpReadWrite.Controls.Add(this.btnWrite);
            this.grpReadWrite.Controls.Add(this.lblHint);
            this.grpReadWrite.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpReadWrite.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.grpReadWrite.Location = new System.Drawing.Point(361, 702);
            this.grpReadWrite.Name = "grpReadWrite";
            this.grpReadWrite.Size = new System.Drawing.Size(340, 158);
            this.grpReadWrite.TabIndex = 1;
            this.grpReadWrite.TabStop = false;
            this.grpReadWrite.Text = " READ / WRITE ";
            // 
            // lblCurValCap
            // 
            this.lblCurValCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblCurValCap.ForeColor = System.Drawing.Color.Silver;
            this.lblCurValCap.Location = new System.Drawing.Point(8, 28);
            this.lblCurValCap.Name = "lblCurValCap";
            this.lblCurValCap.Size = new System.Drawing.Size(56, 22);
            this.lblCurValCap.TabIndex = 0;
            this.lblCurValCap.Text = "현재값:";
            // 
            // lblCurrentVal
            // 
            this.lblCurrentVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCurrentVal.ForeColor = System.Drawing.Color.LimeGreen;
            this.lblCurrentVal.Location = new System.Drawing.Point(68, 28);
            this.lblCurrentVal.Name = "lblCurrentVal";
            this.lblCurrentVal.Size = new System.Drawing.Size(190, 22);
            this.lblCurrentVal.TabIndex = 1;
            this.lblCurrentVal.Text = "--";
            // 
            // btnRead
            // 
            this.btnRead.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(120)))), ((int)(((byte)(80)))));
            this.btnRead.FlatAppearance.BorderSize = 0;
            this.btnRead.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnRead.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnRead.ForeColor = System.Drawing.Color.White;
            this.btnRead.Location = new System.Drawing.Point(264, 26);
            this.btnRead.Name = "btnRead";
            this.btnRead.Size = new System.Drawing.Size(70, 26);
            this.btnRead.TabIndex = 2;
            this.btnRead.Text = "READ";
            this.btnRead.UseVisualStyleBackColor = false;
            this.btnRead.Click += new System.EventHandler(this.btnRead_Click);
            // 
            // lblWriteCap
            // 
            this.lblWriteCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblWriteCap.ForeColor = System.Drawing.Color.Silver;
            this.lblWriteCap.Location = new System.Drawing.Point(8, 66);
            this.lblWriteCap.Name = "lblWriteCap";
            this.lblWriteCap.Size = new System.Drawing.Size(56, 22);
            this.lblWriteCap.TabIndex = 3;
            this.lblWriteCap.Text = "쓸 값:";
            // 
            // txtWriteVal
            // 
            this.txtWriteVal.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(55)))), ((int)(((byte)(65)))));
            this.txtWriteVal.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtWriteVal.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtWriteVal.ForeColor = System.Drawing.Color.White;
            this.txtWriteVal.Location = new System.Drawing.Point(68, 64);
            this.txtWriteVal.Name = "txtWriteVal";
            this.txtWriteVal.Size = new System.Drawing.Size(126, 29);
            this.txtWriteVal.TabIndex = 4;
            // 
            // btnHexDec
            // 
            this.btnHexDec.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(75)))), ((int)(((byte)(85)))));
            this.btnHexDec.FlatAppearance.BorderSize = 0;
            this.btnHexDec.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnHexDec.Font = new System.Drawing.Font("Consolas", 8F);
            this.btnHexDec.ForeColor = System.Drawing.Color.White;
            this.btnHexDec.Location = new System.Drawing.Point(200, 62);
            this.btnHexDec.Name = "btnHexDec";
            this.btnHexDec.Size = new System.Drawing.Size(58, 26);
            this.btnHexDec.TabIndex = 5;
            this.btnHexDec.Text = "HEX⇔";
            this.btnHexDec.UseVisualStyleBackColor = false;
            this.btnHexDec.Click += new System.EventHandler(this.btnHexDec_Click);
            // 
            // btnWrite
            // 
            this.btnWrite.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(70)))), ((int)(((byte)(40)))));
            this.btnWrite.FlatAppearance.BorderSize = 0;
            this.btnWrite.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnWrite.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnWrite.ForeColor = System.Drawing.Color.White;
            this.btnWrite.Location = new System.Drawing.Point(264, 62);
            this.btnWrite.Name = "btnWrite";
            this.btnWrite.Size = new System.Drawing.Size(70, 26);
            this.btnWrite.TabIndex = 6;
            this.btnWrite.Text = "WRITE";
            this.btnWrite.UseVisualStyleBackColor = false;
            this.btnWrite.Click += new System.EventHandler(this.btnWrite_Click);
            // 
            // lblHint
            // 
            this.lblHint.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblHint.ForeColor = System.Drawing.Color.DimGray;
            this.lblHint.Location = new System.Drawing.Point(8, 100);
            this.lblHint.Name = "lblHint";
            this.lblHint.Size = new System.Drawing.Size(326, 32);
            this.lblHint.TabIndex = 7;
            this.lblHint.Text = "※ WRITE는 최대 2바이트(uint16).\r\n큰 구조체는 개별 오프셋을 사용하세요.";
            // 
            // grpDetail
            // 
            this.grpDetail.Controls.Add(this.lblSelNameCap);
            this.grpDetail.Controls.Add(this.lblSelName);
            this.grpDetail.Controls.Add(this.lblSelAddrCap);
            this.grpDetail.Controls.Add(this.lblSelAddr);
            this.grpDetail.Controls.Add(this.lblSelSizeCap);
            this.grpDetail.Controls.Add(this.lblSelSize);
            this.grpDetail.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpDetail.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.grpDetail.Location = new System.Drawing.Point(12, 702);
            this.grpDetail.Name = "grpDetail";
            this.grpDetail.Size = new System.Drawing.Size(340, 158);
            this.grpDetail.TabIndex = 2;
            this.grpDetail.TabStop = false;
            this.grpDetail.Text = " 선택된 변수 ";
            // 
            // lblSelNameCap
            // 
            this.lblSelNameCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSelNameCap.ForeColor = System.Drawing.Color.Silver;
            this.lblSelNameCap.Location = new System.Drawing.Point(8, 24);
            this.lblSelNameCap.Name = "lblSelNameCap";
            this.lblSelNameCap.Size = new System.Drawing.Size(56, 22);
            this.lblSelNameCap.TabIndex = 0;
            this.lblSelNameCap.Text = "이름:";
            // 
            // lblSelName
            // 
            this.lblSelName.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSelName.ForeColor = System.Drawing.Color.White;
            this.lblSelName.Location = new System.Drawing.Point(64, 24);
            this.lblSelName.Name = "lblSelName";
            this.lblSelName.Size = new System.Drawing.Size(270, 22);
            this.lblSelName.TabIndex = 1;
            // 
            // lblSelAddrCap
            // 
            this.lblSelAddrCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSelAddrCap.ForeColor = System.Drawing.Color.Silver;
            this.lblSelAddrCap.Location = new System.Drawing.Point(8, 52);
            this.lblSelAddrCap.Name = "lblSelAddrCap";
            this.lblSelAddrCap.Size = new System.Drawing.Size(56, 22);
            this.lblSelAddrCap.TabIndex = 2;
            this.lblSelAddrCap.Text = "주소:";
            // 
            // lblSelAddr
            // 
            this.lblSelAddr.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSelAddr.ForeColor = System.Drawing.Color.Yellow;
            this.lblSelAddr.Location = new System.Drawing.Point(64, 52);
            this.lblSelAddr.Name = "lblSelAddr";
            this.lblSelAddr.Size = new System.Drawing.Size(270, 22);
            this.lblSelAddr.TabIndex = 3;
            // 
            // lblSelSizeCap
            // 
            this.lblSelSizeCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSelSizeCap.ForeColor = System.Drawing.Color.Silver;
            this.lblSelSizeCap.Location = new System.Drawing.Point(8, 80);
            this.lblSelSizeCap.Name = "lblSelSizeCap";
            this.lblSelSizeCap.Size = new System.Drawing.Size(56, 22);
            this.lblSelSizeCap.TabIndex = 4;
            this.lblSelSizeCap.Text = "크기:";
            // 
            // lblSelSize
            // 
            this.lblSelSize.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSelSize.ForeColor = System.Drawing.Color.LightGreen;
            this.lblSelSize.Location = new System.Drawing.Point(64, 80);
            this.lblSelSize.Name = "lblSelSize";
            this.lblSelSize.Size = new System.Drawing.Size(270, 22);
            this.lblSelSize.TabIndex = 5;
            // 
            // pnlBottom
            // 
            this.pnlBottom.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(25)))), ((int)(((byte)(28)))), ((int)(((byte)(35)))));
            this.pnlBottom.Controls.Add(this.lblStatus);
            this.pnlBottom.Controls.Add(this.btnClose);
            this.pnlBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlBottom.Location = new System.Drawing.Point(0, 1080);
            this.pnlBottom.Name = "pnlBottom";
            this.pnlBottom.Size = new System.Drawing.Size(1606, 36);
            this.pnlBottom.TabIndex = 2;
            // 
            // lblStatus
            // 
            this.lblStatus.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblStatus.ForeColor = System.Drawing.Color.Cyan;
            this.lblStatus.Location = new System.Drawing.Point(8, 8);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(870, 22);
            this.lblStatus.TabIndex = 0;
            this.lblStatus.Text = "준비";
            // 
            // btnClose
            // 
            this.btnClose.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(75)))), ((int)(((byte)(85)))));
            this.btnClose.FlatAppearance.BorderSize = 0;
            this.btnClose.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnClose.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnClose.ForeColor = System.Drawing.Color.White;
            this.btnClose.Location = new System.Drawing.Point(882, 4);
            this.btnClose.Name = "btnClose";
            this.btnClose.Size = new System.Drawing.Size(80, 26);
            this.btnClose.TabIndex = 1;
            this.btnClose.Text = "닫기";
            this.btnClose.UseVisualStyleBackColor = false;
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // tabXcpMain
            // 
            this.tabXcpMain.Controls.Add(this.tabPageDaq);
            this.tabXcpMain.Controls.Add(this.tabPageChecksum);
            this.tabXcpMain.Controls.Add(this.tabPageSeed);
            this.tabXcpMain.Controls.Add(this.tabPageGraph);
            this.tabXcpMain.Font = new System.Drawing.Font("Consolas", 9F);
            this.tabXcpMain.Location = new System.Drawing.Point(713, 84);
            this.tabXcpMain.Name = "tabXcpMain";
            this.tabXcpMain.SelectedIndex = 0;
            this.tabXcpMain.Size = new System.Drawing.Size(870, 994);
            this.tabXcpMain.TabIndex = 99;
            // 
            // tabPageDaq
            // 
            this.tabPageDaq.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(26)))), ((int)(((byte)(34)))));
            this.tabPageDaq.Controls.Add(this.grpDaq);
            this.tabPageDaq.Location = new System.Drawing.Point(4, 31);
            this.tabPageDaq.Name = "tabPageDaq";
            this.tabPageDaq.Size = new System.Drawing.Size(862, 959);
            this.tabPageDaq.TabIndex = 0;
            this.tabPageDaq.Text = "  📡 DAQ";
            // 
            // grpDaq
            // 
            this.grpDaq.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            this.grpDaq.Controls.Add(this.dgvDaq);
            this.grpDaq.Controls.Add(this.btnDaqAdd);
            this.grpDaq.Controls.Add(this.btnDaqRemove);
            this.grpDaq.Controls.Add(this.lblDaqStatus);
            this.grpDaq.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpDaq.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.grpDaq.Location = new System.Drawing.Point(8, 8);
            this.grpDaq.Name = "grpDaq";
            this.grpDaq.Size = new System.Drawing.Size(840, 932);
            this.grpDaq.TabIndex = 0;
            this.grpDaq.TabStop = false;
            this.grpDaq.Text = "DAQ – 실시간 데이터 수집";
            // 
            // dgvDaq
            // 
            this.dgvDaq.AllowUserToAddRows = false;
            this.dgvDaq.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dgvDaq.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(25)))), ((int)(((byte)(28)))), ((int)(((byte)(35)))));
            dataGridViewCellStyle6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            dataGridViewCellStyle6.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            dataGridViewCellStyle6.ForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle6.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle6.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            this.dgvDaq.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle6;
            this.dgvDaq.ColumnHeadersHeight = 34;
            this.dgvDaq.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.daqName,
            this.daqAddr,
            this.daqSize,
            this.daqValue});
            dataGridViewCellStyle7.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            dataGridViewCellStyle7.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            dataGridViewCellStyle7.ForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle7.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(61)))), ((int)(((byte)(133)))), ((int)(((byte)(198)))));
            dataGridViewCellStyle7.SelectionForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle7.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvDaq.DefaultCellStyle = dataGridViewCellStyle7;
            this.dgvDaq.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(64)))), ((int)(((byte)(72)))));
            this.dgvDaq.Location = new System.Drawing.Point(8, 22);
            this.dgvDaq.Name = "dgvDaq";
            this.dgvDaq.ReadOnly = true;
            this.dgvDaq.RowHeadersVisible = false;
            this.dgvDaq.RowHeadersWidth = 62;
            this.dgvDaq.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dgvDaq.Size = new System.Drawing.Size(715, 876);
            this.dgvDaq.TabIndex = 0;
            // 
            // daqName
            // 
            this.daqName.HeaderText = "변수명";
            this.daqName.MinimumWidth = 8;
            this.daqName.Name = "daqName";
            this.daqName.ReadOnly = true;
            // 
            // daqAddr
            // 
            this.daqAddr.HeaderText = "주소";
            this.daqAddr.MinimumWidth = 8;
            this.daqAddr.Name = "daqAddr";
            this.daqAddr.ReadOnly = true;
            // 
            // daqSize
            // 
            this.daqSize.HeaderText = "크기";
            this.daqSize.MinimumWidth = 8;
            this.daqSize.Name = "daqSize";
            this.daqSize.ReadOnly = true;
            // 
            // daqValue
            // 
            this.daqValue.HeaderText = "실시간 값";
            this.daqValue.MinimumWidth = 8;
            this.daqValue.Name = "daqValue";
            this.daqValue.ReadOnly = true;
            // 
            // btnDaqAdd
            // 
            this.btnDaqAdd.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(61)))), ((int)(((byte)(133)))), ((int)(((byte)(198)))));
            this.btnDaqAdd.FlatAppearance.BorderSize = 0;
            this.btnDaqAdd.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnDaqAdd.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnDaqAdd.ForeColor = System.Drawing.Color.White;
            this.btnDaqAdd.Location = new System.Drawing.Point(732, 22);
            this.btnDaqAdd.Name = "btnDaqAdd";
            this.btnDaqAdd.Size = new System.Drawing.Size(100, 28);
            this.btnDaqAdd.TabIndex = 1;
            this.btnDaqAdd.Text = "추가";
            this.btnDaqAdd.UseVisualStyleBackColor = false;
            this.btnDaqAdd.Click += new System.EventHandler(this.BtnDaqAdd_Click);
            // 
            // btnDaqRemove
            // 
            this.btnDaqRemove.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(50)))), ((int)(((byte)(50)))));
            this.btnDaqRemove.FlatAppearance.BorderSize = 0;
            this.btnDaqRemove.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnDaqRemove.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnDaqRemove.ForeColor = System.Drawing.Color.White;
            this.btnDaqRemove.Location = new System.Drawing.Point(732, 56);
            this.btnDaqRemove.Name = "btnDaqRemove";
            this.btnDaqRemove.Size = new System.Drawing.Size(100, 28);
            this.btnDaqRemove.TabIndex = 2;
            this.btnDaqRemove.Text = "제거";
            this.btnDaqRemove.UseVisualStyleBackColor = false;
            this.btnDaqRemove.Click += new System.EventHandler(this.BtnDaqRemove_Click);
            // 
            // lblDaqStatus
            // 
            this.lblDaqStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblDaqStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblDaqStatus.Location = new System.Drawing.Point(7, 901);
            this.lblDaqStatus.Name = "lblDaqStatus";
            this.lblDaqStatus.Size = new System.Drawing.Size(716, 18);
            this.lblDaqStatus.TabIndex = 7;
            this.lblDaqStatus.Text = "대기 중";
            // 
            // tabPageChecksum
            // 
            this.tabPageChecksum.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(26)))), ((int)(((byte)(34)))));
            this.tabPageChecksum.Controls.Add(this.grpChecksum);
            this.tabPageChecksum.Location = new System.Drawing.Point(4, 31);
            this.tabPageChecksum.Name = "tabPageChecksum";
            this.tabPageChecksum.Size = new System.Drawing.Size(862, 959);
            this.tabPageChecksum.TabIndex = 1;
            this.tabPageChecksum.Text = "  🔒 체크썸";
            // 
            // grpChecksum
            // 
            this.grpChecksum.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            this.grpChecksum.Controls.Add(this.lblCsAddrCap);
            this.grpChecksum.Controls.Add(this.txtCsAddress);
            this.grpChecksum.Controls.Add(this.lblCsLenCap);
            this.grpChecksum.Controls.Add(this.txtCsLength);
            this.grpChecksum.Controls.Add(this.lblCsTypeCap);
            this.grpChecksum.Controls.Add(this.cmbCsType);
            this.grpChecksum.Controls.Add(this.btnCalcChecksum);
            this.grpChecksum.Controls.Add(this.lblCsResult);
            this.grpChecksum.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpChecksum.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.grpChecksum.Location = new System.Drawing.Point(8, 8);
            this.grpChecksum.Name = "grpChecksum";
            this.grpChecksum.Size = new System.Drawing.Size(840, 130);
            this.grpChecksum.TabIndex = 0;
            this.grpChecksum.TabStop = false;
            this.grpChecksum.Text = "BUILD_CHECKSUM";
            // 
            // lblCsAddrCap
            // 
            this.lblCsAddrCap.AutoSize = true;
            this.lblCsAddrCap.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblCsAddrCap.ForeColor = System.Drawing.Color.Silver;
            this.lblCsAddrCap.Location = new System.Drawing.Point(8, 24);
            this.lblCsAddrCap.Name = "lblCsAddrCap";
            this.lblCsAddrCap.Size = new System.Drawing.Size(96, 20);
            this.lblCsAddrCap.TabIndex = 0;
            this.lblCsAddrCap.Text = "주소 (hex):";
            // 
            // txtCsAddress
            // 
            this.txtCsAddress.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.txtCsAddress.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtCsAddress.ForeColor = System.Drawing.Color.White;
            this.txtCsAddress.Location = new System.Drawing.Point(110, 22);
            this.txtCsAddress.Name = "txtCsAddress";
            this.txtCsAddress.Size = new System.Drawing.Size(110, 29);
            this.txtCsAddress.TabIndex = 1;
            this.txtCsAddress.Text = "0x20000000";
            // 
            // lblCsLenCap
            // 
            this.lblCsLenCap.AutoSize = true;
            this.lblCsLenCap.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblCsLenCap.ForeColor = System.Drawing.Color.Silver;
            this.lblCsLenCap.Location = new System.Drawing.Point(230, 24);
            this.lblCsLenCap.Name = "lblCsLenCap";
            this.lblCsLenCap.Size = new System.Drawing.Size(114, 20);
            this.lblCsLenCap.TabIndex = 2;
            this.lblCsLenCap.Text = "길이 (bytes):";
            // 
            // txtCsLength
            // 
            this.txtCsLength.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.txtCsLength.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtCsLength.ForeColor = System.Drawing.Color.White;
            this.txtCsLength.Location = new System.Drawing.Point(340, 22);
            this.txtCsLength.Name = "txtCsLength";
            this.txtCsLength.Size = new System.Drawing.Size(70, 29);
            this.txtCsLength.TabIndex = 3;
            this.txtCsLength.Text = "256";
            // 
            // lblCsTypeCap
            // 
            this.lblCsTypeCap.AutoSize = true;
            this.lblCsTypeCap.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblCsTypeCap.ForeColor = System.Drawing.Color.Silver;
            this.lblCsTypeCap.Location = new System.Drawing.Point(8, 56);
            this.lblCsTypeCap.Name = "lblCsTypeCap";
            this.lblCsTypeCap.Size = new System.Drawing.Size(42, 20);
            this.lblCsTypeCap.TabIndex = 4;
            this.lblCsTypeCap.Text = "타입:";
            // 
            // cmbCsType
            // 
            this.cmbCsType.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.cmbCsType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbCsType.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbCsType.ForeColor = System.Drawing.Color.White;
            this.cmbCsType.Items.AddRange(new object[] {
            "ADD_11 (0x01)",
            "ADD_12 (0x02)",
            "ADD_14 (0x03)",
            "ADD_22 (0x04)",
            "ADD_24 (0x05)",
            "ADD_44 (0x06)",
            "CRC_16 (0x07)",
            "CRC_16_CITT (0x08)",
            "CRC_32 (0x09)",
            "USER (0xFF)"});
            this.cmbCsType.Location = new System.Drawing.Point(110, 54);
            this.cmbCsType.Name = "cmbCsType";
            this.cmbCsType.Size = new System.Drawing.Size(150, 30);
            this.cmbCsType.TabIndex = 5;
            // 
            // btnCalcChecksum
            // 
            this.btnCalcChecksum.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(61)))), ((int)(((byte)(133)))), ((int)(((byte)(198)))));
            this.btnCalcChecksum.FlatAppearance.BorderSize = 0;
            this.btnCalcChecksum.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCalcChecksum.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnCalcChecksum.ForeColor = System.Drawing.Color.White;
            this.btnCalcChecksum.Location = new System.Drawing.Point(270, 52);
            this.btnCalcChecksum.Name = "btnCalcChecksum";
            this.btnCalcChecksum.Size = new System.Drawing.Size(100, 28);
            this.btnCalcChecksum.TabIndex = 6;
            this.btnCalcChecksum.Text = "계산";
            this.btnCalcChecksum.UseVisualStyleBackColor = false;
            this.btnCalcChecksum.Click += new System.EventHandler(this.BtnCalcChecksum_Click);
            // 
            // lblCsResult
            // 
            this.lblCsResult.Font = new System.Drawing.Font("Consolas", 9.5F, System.Drawing.FontStyle.Bold);
            this.lblCsResult.ForeColor = System.Drawing.Color.Lime;
            this.lblCsResult.Location = new System.Drawing.Point(8, 92);
            this.lblCsResult.Name = "lblCsResult";
            this.lblCsResult.Size = new System.Drawing.Size(820, 22);
            this.lblCsResult.TabIndex = 7;
            this.lblCsResult.Text = "결과: --";
            // 
            // tabPageSeed
            // 
            this.tabPageSeed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(26)))), ((int)(((byte)(34)))));
            this.tabPageSeed.Controls.Add(this.grpSeedKey);
            this.tabPageSeed.Location = new System.Drawing.Point(4, 31);
            this.tabPageSeed.Name = "tabPageSeed";
            this.tabPageSeed.Size = new System.Drawing.Size(862, 959);
            this.tabPageSeed.TabIndex = 2;
            this.tabPageSeed.Text = "  🔑 Seed/Key";
            // 
            // grpSeedKey
            // 
            this.grpSeedKey.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            this.grpSeedKey.Controls.Add(this.lblSkResource);
            this.grpSeedKey.Controls.Add(this.cmbSkResource);
            this.grpSeedKey.Controls.Add(this.btnGetSeed);
            this.grpSeedKey.Controls.Add(this.lblSeedVal);
            this.grpSeedKey.Controls.Add(this.lblKeyVal);
            this.grpSeedKey.Controls.Add(this.btnUnlock);
            this.grpSeedKey.Controls.Add(this.lblSkStatus);
            this.grpSeedKey.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpSeedKey.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.grpSeedKey.Location = new System.Drawing.Point(8, 8);
            this.grpSeedKey.Name = "grpSeedKey";
            this.grpSeedKey.Size = new System.Drawing.Size(840, 155);
            this.grpSeedKey.TabIndex = 0;
            this.grpSeedKey.TabStop = false;
            this.grpSeedKey.Text = "Seed / Key  (GET_SEED → UNLOCK)";
            // 
            // lblSkResource
            // 
            this.lblSkResource.AutoSize = true;
            this.lblSkResource.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblSkResource.ForeColor = System.Drawing.Color.Silver;
            this.lblSkResource.Location = new System.Drawing.Point(8, 28);
            this.lblSkResource.Name = "lblSkResource";
            this.lblSkResource.Size = new System.Drawing.Size(90, 20);
            this.lblSkResource.TabIndex = 0;
            this.lblSkResource.Text = "Resource:";
            // 
            // cmbSkResource
            // 
            this.cmbSkResource.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.cmbSkResource.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbSkResource.Font = new System.Drawing.Font("Consolas", 9F);
            this.cmbSkResource.ForeColor = System.Drawing.Color.White;
            this.cmbSkResource.Items.AddRange(new object[] {
            "CAL/PAG  (0x01)",
            "DAQ      (0x04)",
            "STIM     (0x08)",
            "PGM      (0x10)"});
            this.cmbSkResource.Location = new System.Drawing.Point(110, 26);
            this.cmbSkResource.Name = "cmbSkResource";
            this.cmbSkResource.Size = new System.Drawing.Size(180, 30);
            this.cmbSkResource.TabIndex = 1;
            // 
            // btnGetSeed
            // 
            this.btnGetSeed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(61)))), ((int)(((byte)(133)))), ((int)(((byte)(198)))));
            this.btnGetSeed.FlatAppearance.BorderSize = 0;
            this.btnGetSeed.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGetSeed.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnGetSeed.ForeColor = System.Drawing.Color.White;
            this.btnGetSeed.Location = new System.Drawing.Point(300, 24);
            this.btnGetSeed.Name = "btnGetSeed";
            this.btnGetSeed.Size = new System.Drawing.Size(100, 28);
            this.btnGetSeed.TabIndex = 2;
            this.btnGetSeed.Text = "GET_SEED";
            this.btnGetSeed.UseVisualStyleBackColor = false;
            this.btnGetSeed.Click += new System.EventHandler(this.BtnGetSeed_Click);
            // 
            // lblSeedVal
            // 
            this.lblSeedVal.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblSeedVal.ForeColor = System.Drawing.Color.Silver;
            this.lblSeedVal.Location = new System.Drawing.Point(8, 62);
            this.lblSeedVal.Name = "lblSeedVal";
            this.lblSeedVal.Size = new System.Drawing.Size(820, 18);
            this.lblSeedVal.TabIndex = 3;
            this.lblSeedVal.Text = "Seed: --";
            // 
            // lblKeyVal
            // 
            this.lblKeyVal.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblKeyVal.ForeColor = System.Drawing.Color.Silver;
            this.lblKeyVal.Location = new System.Drawing.Point(8, 82);
            this.lblKeyVal.Name = "lblKeyVal";
            this.lblKeyVal.Size = new System.Drawing.Size(820, 18);
            this.lblKeyVal.TabIndex = 4;
            this.lblKeyVal.Text = "Key:  --";
            // 
            // btnUnlock
            // 
            this.btnUnlock.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(90)))), ((int)(((byte)(20)))));
            this.btnUnlock.Enabled = false;
            this.btnUnlock.FlatAppearance.BorderSize = 0;
            this.btnUnlock.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnUnlock.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnUnlock.ForeColor = System.Drawing.Color.White;
            this.btnUnlock.Location = new System.Drawing.Point(8, 108);
            this.btnUnlock.Name = "btnUnlock";
            this.btnUnlock.Size = new System.Drawing.Size(120, 28);
            this.btnUnlock.TabIndex = 5;
            this.btnUnlock.Text = "UNLOCK";
            this.btnUnlock.UseVisualStyleBackColor = false;
            this.btnUnlock.Click += new System.EventHandler(this.BtnUnlock_Click);
            // 
            // lblSkStatus
            // 
            this.lblSkStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblSkStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblSkStatus.Location = new System.Drawing.Point(140, 115);
            this.lblSkStatus.Name = "lblSkStatus";
            this.lblSkStatus.Size = new System.Drawing.Size(690, 18);
            this.lblSkStatus.TabIndex = 6;
            this.lblSkStatus.Text = "대기 중";
            // 
            // tabPageGraph
            // 
            this.tabPageGraph.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(26)))), ((int)(((byte)(34)))));
            this.tabPageGraph.Controls.Add(this.grpGraph);
            this.tabPageGraph.Location = new System.Drawing.Point(4, 31);
            this.tabPageGraph.Name = "tabPageGraph";
            this.tabPageGraph.Size = new System.Drawing.Size(862, 959);
            this.tabPageGraph.TabIndex = 3;
            this.tabPageGraph.Text = "  📈 그래프";
            // 
            // grpGraph
            // 
            this.grpGraph.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(26)))), ((int)(((byte)(34)))));
            this.grpGraph.Controls.Add(this.pnlGraph);
            this.grpGraph.Controls.Add(this.lblGraphVarCap);
            this.grpGraph.Controls.Add(this.cmbGraphVar);
            this.grpGraph.Controls.Add(this.btnGraphFavStart);
            this.grpGraph.Controls.Add(this.btnGraphAdd);
            this.grpGraph.Controls.Add(this.btnGraphStart);
            this.grpGraph.Controls.Add(this.btnGraphStop);
            this.grpGraph.Controls.Add(this.btnGraphClear);
            this.grpGraph.Controls.Add(this.chkGraphAutoScale);
            this.grpGraph.Controls.Add(this.lblGraphYMin);
            this.grpGraph.Controls.Add(this.txtGraphYMin);
            this.grpGraph.Controls.Add(this.lblGraphYMax);
            this.grpGraph.Controls.Add(this.txtGraphYMax);
            this.grpGraph.Controls.Add(this.chkGraphXAutoScale);
            this.grpGraph.Controls.Add(this.lblGraphXMin);
            this.grpGraph.Controls.Add(this.txtGraphXMin);
            this.grpGraph.Controls.Add(this.lblGraphXMax);
            this.grpGraph.Controls.Add(this.txtGraphXMax);
            this.grpGraph.Controls.Add(this.lblGraphStatus);
            this.grpGraph.Controls.Add(this.nudGraphInterval);
            this.grpGraph.Controls.Add(this.lblGraphInterval);
            this.grpGraph.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpGraph.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.grpGraph.Location = new System.Drawing.Point(8, 8);
            this.grpGraph.Name = "grpGraph";
            this.grpGraph.Size = new System.Drawing.Size(840, 948);
            this.grpGraph.TabIndex = 0;
            this.grpGraph.TabStop = false;
            this.grpGraph.Text = "DAQ 실시간 그래프";
            // 
            // pnlGraph
            // 
            this.pnlGraph.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(15)))), ((int)(((byte)(18)))), ((int)(((byte)(24)))));
            this.pnlGraph.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlGraph.Location = new System.Drawing.Point(8, 22);
            this.pnlGraph.Name = "pnlGraph";
            this.pnlGraph.Size = new System.Drawing.Size(824, 771);
            this.pnlGraph.TabIndex = 0;
            this.pnlGraph.Paint += new System.Windows.Forms.PaintEventHandler(this.PnlGraph_Paint);
            // 
            // lblGraphVarCap
            // 
            this.lblGraphVarCap.AutoSize = true;
            this.lblGraphVarCap.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblGraphVarCap.ForeColor = System.Drawing.Color.Silver;
            this.lblGraphVarCap.Location = new System.Drawing.Point(18, 821);
            this.lblGraphVarCap.Name = "lblGraphVarCap";
            this.lblGraphVarCap.Size = new System.Drawing.Size(42, 20);
            this.lblGraphVarCap.TabIndex = 1;
            this.lblGraphVarCap.Text = "변수:";
            // 
            // cmbGraphVar
            // 
            this.cmbGraphVar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.cmbGraphVar.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbGraphVar.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.cmbGraphVar.ForeColor = System.Drawing.Color.White;
            this.cmbGraphVar.Location = new System.Drawing.Point(65, 819);
            this.cmbGraphVar.Name = "cmbGraphVar";
            this.cmbGraphVar.Size = new System.Drawing.Size(200, 28);
            this.cmbGraphVar.TabIndex = 2;
            this.cmbGraphVar.DropDown += new System.EventHandler(this.CmbGraphVar_DropDown);
            // 
            // btnGraphFavStart
            // 
            this.btnGraphFavStart.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(70)))), ((int)(((byte)(20)))));
            this.btnGraphFavStart.FlatAppearance.BorderSize = 0;
            this.btnGraphFavStart.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGraphFavStart.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnGraphFavStart.ForeColor = System.Drawing.Color.White;
            this.btnGraphFavStart.Location = new System.Drawing.Point(270, 817);
            this.btnGraphFavStart.Name = "btnGraphFavStart";
            this.btnGraphFavStart.Size = new System.Drawing.Size(140, 28);
            this.btnGraphFavStart.TabIndex = 3;
            this.btnGraphFavStart.Text = "★ 즐겨찾기 그래프";
            this.btnGraphFavStart.UseVisualStyleBackColor = false;
            this.btnGraphFavStart.Click += new System.EventHandler(this.BtnGraphFavStart_Click);
            // 
            // btnGraphAdd
            // 
            this.btnGraphAdd.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(90)))), ((int)(((byte)(50)))));
            this.btnGraphAdd.FlatAppearance.BorderSize = 0;
            this.btnGraphAdd.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGraphAdd.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnGraphAdd.ForeColor = System.Drawing.Color.White;
            this.btnGraphAdd.Location = new System.Drawing.Point(416, 799);
            this.btnGraphAdd.Name = "btnGraphAdd";
            this.btnGraphAdd.Size = new System.Drawing.Size(100, 28);
            this.btnGraphAdd.TabIndex = 4;
            this.btnGraphAdd.Text = "+ 추가";
            this.btnGraphAdd.UseVisualStyleBackColor = false;
            this.btnGraphAdd.Click += new System.EventHandler(this.BtnGraphAdd_Click);
            // 
            // btnGraphStart
            // 
            this.btnGraphStart.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(120)))), ((int)(((byte)(50)))));
            this.btnGraphStart.FlatAppearance.BorderSize = 0;
            this.btnGraphStart.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGraphStart.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnGraphStart.ForeColor = System.Drawing.Color.White;
            this.btnGraphStart.Location = new System.Drawing.Point(538, 817);
            this.btnGraphStart.Name = "btnGraphStart";
            this.btnGraphStart.Size = new System.Drawing.Size(100, 28);
            this.btnGraphStart.TabIndex = 5;
            this.btnGraphStart.Text = "▶ 시작";
            this.btnGraphStart.UseVisualStyleBackColor = false;
            this.btnGraphStart.Click += new System.EventHandler(this.BtnGraphStart_Click);
            // 
            // btnGraphStop
            // 
            this.btnGraphStop.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(50)))), ((int)(((byte)(50)))));
            this.btnGraphStop.Enabled = false;
            this.btnGraphStop.FlatAppearance.BorderSize = 0;
            this.btnGraphStop.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGraphStop.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnGraphStop.ForeColor = System.Drawing.Color.White;
            this.btnGraphStop.Location = new System.Drawing.Point(660, 817);
            this.btnGraphStop.Name = "btnGraphStop";
            this.btnGraphStop.Size = new System.Drawing.Size(100, 28);
            this.btnGraphStop.TabIndex = 6;
            this.btnGraphStop.Text = "■ 정지";
            this.btnGraphStop.UseVisualStyleBackColor = false;
            this.btnGraphStop.Click += new System.EventHandler(this.BtnGraphStop_Click);
            // 
            // btnGraphClear
            // 
            this.btnGraphClear.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(60)))), ((int)(((byte)(80)))));
            this.btnGraphClear.FlatAppearance.BorderSize = 0;
            this.btnGraphClear.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGraphClear.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnGraphClear.ForeColor = System.Drawing.Color.White;
            this.btnGraphClear.Location = new System.Drawing.Point(416, 840);
            this.btnGraphClear.Name = "btnGraphClear";
            this.btnGraphClear.Size = new System.Drawing.Size(100, 28);
            this.btnGraphClear.TabIndex = 7;
            this.btnGraphClear.Text = " - 지우기";
            this.btnGraphClear.UseVisualStyleBackColor = false;
            this.btnGraphClear.Click += new System.EventHandler(this.BtnGraphClear_Click);
            // 
            // chkGraphAutoScale
            // 
            this.chkGraphAutoScale.AutoSize = true;
            this.chkGraphAutoScale.Checked = true;
            this.chkGraphAutoScale.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkGraphAutoScale.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkGraphAutoScale.ForeColor = System.Drawing.Color.Silver;
            this.chkGraphAutoScale.Location = new System.Drawing.Point(22, 883);
            this.chkGraphAutoScale.Name = "chkGraphAutoScale";
            this.chkGraphAutoScale.Size = new System.Drawing.Size(83, 24);
            this.chkGraphAutoScale.TabIndex = 8;
            this.chkGraphAutoScale.Text = "자동범위";
            this.chkGraphAutoScale.CheckedChanged += new System.EventHandler(this.chkGraphAutoScale_CheckedChanged);
            // 
            // lblGraphYMin
            // 
            this.lblGraphYMin.AutoSize = true;
            this.lblGraphYMin.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblGraphYMin.ForeColor = System.Drawing.Color.Silver;
            this.lblGraphYMin.Location = new System.Drawing.Point(115, 882);
            this.lblGraphYMin.Name = "lblGraphYMin";
            this.lblGraphYMin.Size = new System.Drawing.Size(63, 20);
            this.lblGraphYMin.TabIndex = 9;
            this.lblGraphYMin.Text = "Y Min:";
            // 
            // txtGraphYMin
            // 
            this.txtGraphYMin.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.txtGraphYMin.Enabled = false;
            this.txtGraphYMin.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtGraphYMin.ForeColor = System.Drawing.Color.White;
            this.txtGraphYMin.Location = new System.Drawing.Point(181, 879);
            this.txtGraphYMin.Name = "txtGraphYMin";
            this.txtGraphYMin.Size = new System.Drawing.Size(60, 29);
            this.txtGraphYMin.TabIndex = 10;
            this.txtGraphYMin.Text = "0";
            // 
            // lblGraphYMax
            // 
            this.lblGraphYMax.AutoSize = true;
            this.lblGraphYMax.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblGraphYMax.ForeColor = System.Drawing.Color.Silver;
            this.lblGraphYMax.Location = new System.Drawing.Point(248, 881);
            this.lblGraphYMax.Name = "lblGraphYMax";
            this.lblGraphYMax.Size = new System.Drawing.Size(63, 20);
            this.lblGraphYMax.TabIndex = 11;
            this.lblGraphYMax.Text = "Y Max:";
            // 
            // txtGraphYMax
            // 
            this.txtGraphYMax.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.txtGraphYMax.Enabled = false;
            this.txtGraphYMax.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtGraphYMax.ForeColor = System.Drawing.Color.White;
            this.txtGraphYMax.Location = new System.Drawing.Point(317, 879);
            this.txtGraphYMax.Name = "txtGraphYMax";
            this.txtGraphYMax.Size = new System.Drawing.Size(60, 29);
            this.txtGraphYMax.TabIndex = 12;
            this.txtGraphYMax.Text = "100";
            // 
            // chkGraphXAutoScale
            // 
            this.chkGraphXAutoScale.AutoSize = true;
            this.chkGraphXAutoScale.Checked = true;
            this.chkGraphXAutoScale.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkGraphXAutoScale.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkGraphXAutoScale.ForeColor = System.Drawing.Color.Silver;
            this.chkGraphXAutoScale.Location = new System.Drawing.Point(402, 881);
            this.chkGraphXAutoScale.Name = "chkGraphXAutoScale";
            this.chkGraphXAutoScale.Size = new System.Drawing.Size(77, 24);
            this.chkGraphXAutoScale.TabIndex = 14;
            this.chkGraphXAutoScale.Text = "X 자동";
            this.chkGraphXAutoScale.CheckedChanged += new System.EventHandler(this.chkGraphXAutoScale_CheckedChanged);
            // 
            // lblGraphXMin
            // 
            this.lblGraphXMin.AutoSize = true;
            this.lblGraphXMin.Enabled = false;
            this.lblGraphXMin.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblGraphXMin.ForeColor = System.Drawing.Color.Silver;
            this.lblGraphXMin.Location = new System.Drawing.Point(497, 881);
            this.lblGraphXMin.Name = "lblGraphXMin";
            this.lblGraphXMin.Size = new System.Drawing.Size(63, 20);
            this.lblGraphXMin.TabIndex = 15;
            this.lblGraphXMin.Text = "X Min:";
            // 
            // txtGraphXMin
            // 
            this.txtGraphXMin.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.txtGraphXMin.Enabled = false;
            this.txtGraphXMin.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtGraphXMin.ForeColor = System.Drawing.Color.White;
            this.txtGraphXMin.Location = new System.Drawing.Point(560, 879);
            this.txtGraphXMin.Name = "txtGraphXMin";
            this.txtGraphXMin.Size = new System.Drawing.Size(50, 29);
            this.txtGraphXMin.TabIndex = 16;
            this.txtGraphXMin.Text = "0";
            // 
            // lblGraphXMax
            // 
            this.lblGraphXMax.AutoSize = true;
            this.lblGraphXMax.Enabled = false;
            this.lblGraphXMax.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblGraphXMax.ForeColor = System.Drawing.Color.Silver;
            this.lblGraphXMax.Location = new System.Drawing.Point(616, 881);
            this.lblGraphXMax.Name = "lblGraphXMax";
            this.lblGraphXMax.Size = new System.Drawing.Size(95, 30);
            this.lblGraphXMax.TabIndex = 17;
            this.lblGraphXMax.Text = "X Max:";
            // 
            // txtGraphXMax
            // 
            this.txtGraphXMax.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.txtGraphXMax.Enabled = false;
            this.txtGraphXMax.Font = new System.Drawing.Font("Consolas", 9F);
            this.txtGraphXMax.ForeColor = System.Drawing.Color.White;
            this.txtGraphXMax.Location = new System.Drawing.Point(710, 878);
            this.txtGraphXMax.Name = "txtGraphXMax";
            this.txtGraphXMax.Size = new System.Drawing.Size(50, 29);
            this.txtGraphXMax.TabIndex = 18;
            this.txtGraphXMax.Text = "300";
            // 
            // lblGraphStatus
            // 
            this.lblGraphStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblGraphStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblGraphStatus.Location = new System.Drawing.Point(4, 925);
            this.lblGraphStatus.Name = "lblGraphStatus";
            this.lblGraphStatus.Size = new System.Drawing.Size(820, 18);
            this.lblGraphStatus.TabIndex = 13;
            this.lblGraphStatus.Text = "대기 중 – 변수를 추가하고 시작하세요";
            // 
            // nudGraphInterval
            // 
            this.nudGraphInterval.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this.nudGraphInterval.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudGraphInterval.ForeColor = System.Drawing.Color.White;
            this.nudGraphInterval.Location = new System.Drawing.Point(774, 817);
            this.nudGraphInterval.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.nudGraphInterval.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.nudGraphInterval.Name = "nudGraphInterval";
            this.nudGraphInterval.Size = new System.Drawing.Size(70, 29);
            this.nudGraphInterval.TabIndex = 20;
            this.nudGraphInterval.Value = new decimal(new int[] {
            200,
            0,
            0,
            0});
            // 
            // lblGraphInterval
            // 
            this.lblGraphInterval.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblGraphInterval.ForeColor = System.Drawing.Color.Silver;
            this.lblGraphInterval.Location = new System.Drawing.Point(774, 800);
            this.lblGraphInterval.Name = "lblGraphInterval";
            this.lblGraphInterval.Size = new System.Drawing.Size(70, 16);
            this.lblGraphInterval.TabIndex = 21;
            this.lblGraphInterval.Text = "주기(ms)";
            // 
            // FormParamTuning
            // 
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(34)))), ((int)(((byte)(42)))));
            this.ClientSize = new System.Drawing.Size(1606, 1116);
            this.Controls.Add(this.dgvSymbols);
            this.Controls.Add(this.grpFavorites);
            this.Controls.Add(this.grpReadWrite);
            this.Controls.Add(this.grpDetail);
            this.Controls.Add(this.pnlTop);
            this.Controls.Add(this.pnlBottom);
            this.Controls.Add(this.tabXcpMain);
            this.Font = new System.Drawing.Font("Consolas", 9F);
            this.MinimumSize = new System.Drawing.Size(900, 600);
            this.Name = "FormParamTuning";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Parameter Tuning  –  변수명 기반 RAM Read/Write  (.out / .map)";
            this.pnlTop.ResumeLayout(false);
            this.pnlTop.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudNAD)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudXcpTxId)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudXcpRxId)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudXcpAddrExt)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSymbols)).EndInit();
            this.grpFavorites.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgvFavorites)).EndInit();
            this.grpReadWrite.ResumeLayout(false);
            this.grpReadWrite.PerformLayout();
            this.grpDetail.ResumeLayout(false);
            this.pnlBottom.ResumeLayout(false);
            this.tabXcpMain.ResumeLayout(false);
            this.tabPageDaq.ResumeLayout(false);
            this.grpDaq.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dgvDaq)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudGraphInterval)).EndInit();
            this.tabPageChecksum.ResumeLayout(false);
            this.grpChecksum.ResumeLayout(false);
            this.grpChecksum.PerformLayout();
            this.tabPageSeed.ResumeLayout(false);
            this.grpSeedKey.ResumeLayout(false);
            this.grpSeedKey.PerformLayout();
            this.tabPageGraph.ResumeLayout(false);
            this.grpGraph.ResumeLayout(false);
            this.grpGraph.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel              pnlTop;
        private System.Windows.Forms.Button             btnLoadMap;
        private System.Windows.Forms.Label              lblMapStatus;
        private System.Windows.Forms.Label              lblConnStatus;
        private System.Windows.Forms.Label              lblNADCap;
        private System.Windows.Forms.NumericUpDown      nudNAD;
        private System.Windows.Forms.Label              lblSearchCap;
        private System.Windows.Forms.TextBox            txtSearch;
        private System.Windows.Forms.Label              lblCount;
        private System.Windows.Forms.DataGridView       dgvSymbols;
        private System.Windows.Forms.DataGridViewTextBoxColumn colName;
        private System.Windows.Forms.DataGridViewTextBoxColumn colAddr;
        private System.Windows.Forms.DataGridViewTextBoxColumn colSize;
        private System.Windows.Forms.DataGridViewTextBoxColumn colValue;
        private System.Windows.Forms.GroupBox           grpDetail;
        private System.Windows.Forms.Label              lblSelNameCap;
        private System.Windows.Forms.Label              lblSelName;
        private System.Windows.Forms.Label              lblSelAddrCap;
        private System.Windows.Forms.Label              lblSelAddr;
        private System.Windows.Forms.Label              lblSelSizeCap;
        private System.Windows.Forms.Label              lblSelSize;
        private System.Windows.Forms.GroupBox           grpReadWrite;
        private System.Windows.Forms.Label              lblCurValCap;
        private System.Windows.Forms.Label              lblCurrentVal;
        private System.Windows.Forms.Button             btnRead;
        private System.Windows.Forms.Label              lblWriteCap;
        private System.Windows.Forms.TextBox            txtWriteVal;
        private System.Windows.Forms.Button             btnHexDec;
        private System.Windows.Forms.Button             btnWrite;
        private System.Windows.Forms.Label              lblHint;
        private System.Windows.Forms.GroupBox           grpFavorites;
        private System.Windows.Forms.DataGridView       dgvFavorites;
        private System.Windows.Forms.DataGridViewTextBoxColumn favColName;
        private System.Windows.Forms.DataGridViewTextBoxColumn favColAddr;
        private System.Windows.Forms.DataGridViewTextBoxColumn favColSize;
        private System.Windows.Forms.DataGridViewTextBoxColumn favColValue;
        private System.Windows.Forms.Button             btnFavAdd;
        private System.Windows.Forms.Button             btnFavRemove;
        private System.Windows.Forms.Button             btnFavReadAll;
        private System.Windows.Forms.Panel              pnlBottom;
        private System.Windows.Forms.Label              lblStatus;
        private System.Windows.Forms.Button             btnClose;
        private System.Windows.Forms.CheckBox           chkUseXcp;
        private System.Windows.Forms.Label              lblXcpTx;
        private System.Windows.Forms.NumericUpDown      nudXcpTxId;
        private System.Windows.Forms.Label              lblXcpRx;
        private System.Windows.Forms.NumericUpDown      nudXcpRxId;
        private System.Windows.Forms.Label              lblXcpExt;
        private System.Windows.Forms.NumericUpDown      nudXcpAddrExt;
        private System.Windows.Forms.TabControl         tabXcpMain;
        private System.Windows.Forms.TabPage            tabPageDaq;
        private System.Windows.Forms.TabPage            tabPageChecksum;
        private System.Windows.Forms.TabPage            tabPageSeed;
        private System.Windows.Forms.TabPage            tabPageGraph;
        private System.Windows.Forms.GroupBox           grpDaq;
        private System.Windows.Forms.DataGridView       dgvDaq;
        private System.Windows.Forms.Button             btnDaqAdd;
        private System.Windows.Forms.Button             btnDaqRemove;
        private System.Windows.Forms.Label              lblDaqStatus;
        private System.Windows.Forms.NumericUpDown      nudGraphInterval;
        private System.Windows.Forms.Label              lblGraphInterval;
        private System.Windows.Forms.GroupBox           grpChecksum;
        private System.Windows.Forms.Label              lblCsAddrCap;
        private System.Windows.Forms.TextBox            txtCsAddress;
        private System.Windows.Forms.Label              lblCsLenCap;
        private System.Windows.Forms.TextBox            txtCsLength;
        private System.Windows.Forms.Label              lblCsTypeCap;
        private System.Windows.Forms.ComboBox           cmbCsType;
        private System.Windows.Forms.Button             btnCalcChecksum;
        private System.Windows.Forms.Label              lblCsResult;
        private System.Windows.Forms.GroupBox           grpSeedKey;
        private System.Windows.Forms.Label              lblSkResource;
        private System.Windows.Forms.ComboBox           cmbSkResource;
        private System.Windows.Forms.Button             btnGetSeed;
        private System.Windows.Forms.Label              lblSeedVal;
        private System.Windows.Forms.Label              lblKeyVal;
        private System.Windows.Forms.Button             btnUnlock;
        private System.Windows.Forms.Label              lblSkStatus;
        private System.Windows.Forms.GroupBox           grpGraph;
        private System.Windows.Forms.Panel              pnlGraph;
        private System.Windows.Forms.Label              lblGraphVarCap;
        private System.Windows.Forms.ComboBox           cmbGraphVar;
        private System.Windows.Forms.Button             btnGraphFavStart;
        private System.Windows.Forms.Button             btnGraphAdd;
        private System.Windows.Forms.Button             btnGraphStart;
        private System.Windows.Forms.Button             btnGraphStop;
        private System.Windows.Forms.Button             btnGraphClear;
        private System.Windows.Forms.CheckBox           chkGraphAutoScale;
        private System.Windows.Forms.Label              lblGraphYMin;
        private System.Windows.Forms.TextBox            txtGraphYMin;
        private System.Windows.Forms.Label              lblGraphYMax;
        private System.Windows.Forms.TextBox            txtGraphYMax;
        private System.Windows.Forms.CheckBox           chkGraphXAutoScale;
        private System.Windows.Forms.Label              lblGraphXMin;
        private System.Windows.Forms.TextBox            txtGraphXMin;
        private System.Windows.Forms.Label              lblGraphXMax;
        private System.Windows.Forms.TextBox            txtGraphXMax;
        private System.Windows.Forms.Label              lblGraphStatus;
        private System.Windows.Forms.DataGridViewTextBoxColumn daqName;
        private System.Windows.Forms.DataGridViewTextBoxColumn daqAddr;
        private System.Windows.Forms.DataGridViewTextBoxColumn daqSize;
        private System.Windows.Forms.DataGridViewTextBoxColumn daqValue;
    }
}
