namespace LINMaster.Forms
{
    partial class FormNodeScan
    {
        private System.ComponentModel.IContainer components = null;
        protected override void Dispose(bool disposing)
        { if (disposing && (components != null)) components.Dispose(); base.Dispose(disposing); }

        #region Windows Form Designer generated code
        private void InitializeComponent()
        {
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.pnlInfo = new System.Windows.Forms.Panel();
            this.lblTypCap = new System.Windows.Forms.Label();
            this.lblTypVal = new System.Windows.Forms.Label();
            this.lblProjCap = new System.Windows.Forms.Label();
            this.lblProjVal = new System.Windows.Forms.Label();
            this.lblSdkCap = new System.Windows.Forms.Label();
            this.lblSdkVal = new System.Windows.Forms.Label();
            this.lblRelCap = new System.Windows.Forms.Label();
            this.lblReleaseVal = new System.Windows.Forms.Label();
            this.lblMLMCap = new System.Windows.Forms.Label();
            this.lblMLMVal = new System.Windows.Forms.Label();
            this.lblMLSCap = new System.Windows.Forms.Label();
            this.lblMLSVal = new System.Windows.Forms.Label();
            this.lblFlagCap = new System.Windows.Forms.Label();
            this.lblFlagVal = new System.Windows.Forms.Label();
            this.lblSerialCap = new System.Windows.Forms.Label();
            this.lblSerialVal = new System.Windows.Forms.Label();
            this.lblTempCap = new System.Windows.Forms.Label();
            this.lblTempVal = new System.Windows.Forms.Label();
            this.lblVoltCap = new System.Windows.Forms.Label();
            this.lblVoltVal = new System.Windows.Forms.Label();
            this.lblDtcOpen = new System.Windows.Forms.Label();
            this.lblDtcClose = new System.Windows.Forms.Label();
            this.lblDtcCrit = new System.Windows.Forms.Label();
            this.lblDtcOk = new System.Windows.Forms.Label();
            this.pnlLeft = new System.Windows.Forms.Panel();
            this.lstSlaves = new System.Windows.Forms.ListBox();
            this.lblScanStatus = new System.Windows.Forms.Label();
            this.btnAddNode = new System.Windows.Forms.Button();
            this.btnScan = new System.Windows.Forms.Button();
            this.lblSlaveNodes = new System.Windows.Forms.Label();
            this.tabMain = new System.Windows.Forms.TabControl();
            this.tabLedCtrl = new System.Windows.Forms.TabPage();
            this.grpLedTx = new System.Windows.Forms.GroupBox();
            this.btnLedSend = new System.Windows.Forms.Button();
            this.btnLedStart = new System.Windows.Forms.Button();
            this.lblFramePreview = new System.Windows.Forms.Label();
            this.grpLedColor = new System.Windows.Forms.GroupBox();
            this.lblLedCX = new System.Windows.Forms.Label();
            this.nudColorX = new System.Windows.Forms.NumericUpDown();
            this.lblLedCY = new System.Windows.Forms.Label();
            this.nudColorY = new System.Windows.Forms.NumericUpDown();
            this.lblLedBr = new System.Windows.Forms.Label();
            this.nudBright = new System.Windows.Forms.NumericUpDown();
            this.lblLedFI = new System.Windows.Forms.Label();
            this.nudFadeIn = new System.Windows.Forms.NumericUpDown();
            this.lblLedFO = new System.Windows.Forms.Label();
            this.nudFadeOut = new System.Windows.Forms.NumericUpDown();
            this.grpEfficiency = new System.Windows.Forms.GroupBox();
            this.lblUsedLed = new System.Windows.Forms.Label();
            this.nudUsedLed = new System.Windows.Forms.NumericUpDown();
            this.btnDevGet = new System.Windows.Forms.Button();
            this.btnDevSet = new System.Windows.Forms.Button();
            this.lblOverlap = new System.Windows.Forms.Label();
            this.lblDevStatus = new System.Windows.Forms.Label();
            this.lblEff0 = new System.Windows.Forms.Label();
            this.nudOverlap = new System.Windows.Forms.NumericUpDown();
            this.nudEff0 = new System.Windows.Forms.NumericUpDown();
            this.lblEff1 = new System.Windows.Forms.Label();
            this.lblOlActive = new System.Windows.Forms.Label();
            this.nudEff1 = new System.Windows.Forms.NumericUpDown();
            this.nudMaxInt = new System.Windows.Forms.NumericUpDown();
            this.lblEff2 = new System.Windows.Forms.Label();
            this.nudOlActive = new System.Windows.Forms.NumericUpDown();
            this.nudEff2 = new System.Windows.Forms.NumericUpDown();
            this.lblMaxInt = new System.Windows.Forms.Label();
            this.lblEff3 = new System.Windows.Forms.Label();
            this.lblCurSrc = new System.Windows.Forms.Label();
            this.nudEff3 = new System.Windows.Forms.NumericUpDown();
            this.nudCurSrc = new System.Windows.Forms.NumericUpDown();
            this.lblEff4 = new System.Windows.Forms.Label();
            this.nudEff4 = new System.Windows.Forms.NumericUpDown();
            this.lblEff5 = new System.Windows.Forms.Label();
            this.nudEff5 = new System.Windows.Forms.NumericUpDown();
            this.lblEff6 = new System.Windows.Forms.Label();
            this.nudEff6 = new System.Windows.Forms.NumericUpDown();
            this.lblEff7 = new System.Windows.Forms.Label();
            this.nudEff7 = new System.Windows.Forms.NumericUpDown();
            this.grpSF = new System.Windows.Forms.GroupBox();
            this.chkSF1 = new System.Windows.Forms.CheckBox();
            this.chkSF2 = new System.Windows.Forms.CheckBox();
            this.chkSF3 = new System.Windows.Forms.CheckBox();
            this.chkSF4 = new System.Windows.Forms.CheckBox();
            this.chkSF5 = new System.Windows.Forms.CheckBox();
            this.chkSF6 = new System.Windows.Forms.CheckBox();
            this.chkSF7 = new System.Windows.Forms.CheckBox();
            this.chkSF8 = new System.Windows.Forms.CheckBox();
            this.tabCalib = new System.Windows.Forms.TabPage();
            this.grpCalib = new System.Windows.Forms.GroupBox();
            this.lblCalibLed = new System.Windows.Forms.Label();
            this.nudCalibLedSel = new System.Windows.Forms.NumericUpDown();
            this.lblCalibBrightR = new System.Windows.Forms.Label();
            this.lblCalibBrightG = new System.Windows.Forms.Label();
            this.lblCalibBrightB = new System.Windows.Forms.Label();
            this.lblCalibRX = new System.Windows.Forms.Label();
            this.nudCalibRX = new System.Windows.Forms.NumericUpDown();
            this.lblCalibRY = new System.Windows.Forms.Label();
            this.nudCalibRY = new System.Windows.Forms.NumericUpDown();
            this.lblCalibRZ = new System.Windows.Forms.Label();
            this.nudCalibRZ = new System.Windows.Forms.NumericUpDown();
            this.lblCalibGX = new System.Windows.Forms.Label();
            this.nudCalibGX = new System.Windows.Forms.NumericUpDown();
            this.lblCalibGY = new System.Windows.Forms.Label();
            this.nudCalibGY = new System.Windows.Forms.NumericUpDown();
            this.lblCalibGZ = new System.Windows.Forms.Label();
            this.nudCalibGZ = new System.Windows.Forms.NumericUpDown();
            this.lblCalibBX = new System.Windows.Forms.Label();
            this.nudCalibBX = new System.Windows.Forms.NumericUpDown();
            this.lblCalibBY = new System.Windows.Forms.Label();
            this.nudCalibBY = new System.Windows.Forms.NumericUpDown();
            this.lblCalibBZ = new System.Windows.Forms.Label();
            this.nudCalibBZ = new System.Windows.Forms.NumericUpDown();
            this.btnCalibGet = new System.Windows.Forms.Button();
            this.btnCalibGetAll = new System.Windows.Forms.Button();
            this.btnCalibSet = new System.Windows.Forms.Button();
            this.lblCalibStatus = new System.Windows.Forms.Label();
            this.dgvCalib = new System.Windows.Forms.DataGridView();
            this.tabProdInfo = new System.Windows.Forms.TabPage();
            this.grpProdWrite = new System.Windows.Forms.GroupBox();
            this.lblWrNADCap = new System.Windows.Forms.Label();
            this.nudWrNAD = new System.Windows.Forms.NumericUpDown();
            this.lblWrMstNADCap = new System.Windows.Forms.Label();
            this.nudWrMstNAD = new System.Windows.Forms.NumericUpDown();
            this.lblWrSFCap = new System.Windows.Forms.Label();
            this.nudWrSlaveFlag = new System.Windows.Forms.NumericUpDown();
            this.lblWrColorTblCap = new System.Windows.Forms.Label();
            this.nudWrColorTbl = new System.Windows.Forms.NumericUpDown();
            this.btnProdWrite = new System.Windows.Forms.Button();
            this.lblProdWriteStatus = new System.Windows.Forms.Label();
            this.grpUtcLotWrite = new System.Windows.Forms.GroupBox();
            this.lblWrUTCCap = new System.Windows.Forms.Label();
            this.lblUtcPreview = new System.Windows.Forms.Label();
            this.btnWrUTC = new System.Windows.Forms.Button();
            this.lblWrLOTCap = new System.Windows.Forms.Label();
            this.nudWrLOT0 = new System.Windows.Forms.NumericUpDown();
            this.nudWrLOT1 = new System.Windows.Forms.NumericUpDown();
            this.nudWrLOT2 = new System.Windows.Forms.NumericUpDown();
            this.nudWrLOT3 = new System.Windows.Forms.NumericUpDown();
            this.btnWrLOT = new System.Windows.Forms.Button();
            this.lblUtcLotStatus = new System.Windows.Forms.Label();
            this.lblNADCap = new System.Windows.Forms.Label();
            this.lblNADVal = new System.Windows.Forms.Label();
            this.lblSlaveNADCap = new System.Windows.Forms.Label();
            this.lblMasterNADVal = new System.Windows.Forms.Label();
            this.lblSFCap = new System.Windows.Forms.Label();
            this.lblSFVal = new System.Windows.Forms.Label();
            this.lblColorCap = new System.Windows.Forms.Label();
            this.lblColorTblVal = new System.Windows.Forms.Label();
            this.lblUTCCap = new System.Windows.Forms.Label();
            this.lblUTCVal = new System.Windows.Forms.Label();
            this.lblLOTCap = new System.Windows.Forms.Label();
            this.lblLOTVal = new System.Windows.Forms.Label();
            this.lblTrimCap = new System.Windows.Forms.Label();
            this.lblTrimVal = new System.Windows.Forms.Label();
            this.panelFlagBits = new System.Windows.Forms.Panel();
            this.tabPwmDiag = new System.Windows.Forms.TabPage();
            this.tabParamTuning = new System.Windows.Forms.TabPage();
            this.btnOpenParamTuning = new System.Windows.Forms.Button();
            this.grpPwmCtrl = new System.Windows.Forms.GroupBox();
            this.lblPwmPort = new System.Windows.Forms.Label();
            this.nudPwmPort = new System.Windows.Forms.NumericUpDown();
            this.lblPwmDuty = new System.Windows.Forms.Label();
            this.nudPwmDuty = new System.Windows.Forms.NumericUpDown();
            this.btnSetPwm = new System.Windows.Forms.Button();
            this.btnGetPwm = new System.Windows.Forms.Button();
            this.lblPwmValCap = new System.Windows.Forms.Label();
            this.lblPwmVal = new System.Windows.Forms.Label();
            this.grpPwmMax = new System.Windows.Forms.GroupBox();
            this.lblPwmMaxMask = new System.Windows.Forms.Label();
            this.nudPwmMaxMask0 = new System.Windows.Forms.NumericUpDown();
            this.nudPwmMaxMask1 = new System.Windows.Forms.NumericUpDown();
            this.nudPwmMaxMask2 = new System.Windows.Forms.NumericUpDown();
            this.btnSetPwmMax = new System.Windows.Forms.Button();
            this.lblPwmMaxStatus = new System.Windows.Forms.Label();
            this.grpDiagRead = new System.Windows.Forms.GroupBox();
            this.btnGetPnVolt = new System.Windows.Forms.Button();
            this.lblPnVoltCap = new System.Windows.Forms.Label();
            this.lblPnVoltVal = new System.Windows.Forms.Label();
            this.nudPnVoltOpt = new System.Windows.Forms.NumericUpDown();
            this.btnGetCurrent = new System.Windows.Forms.Button();
            this.lblCurrentCap = new System.Windows.Forms.Label();
            this.lblCurrentVal = new System.Windows.Forms.Label();
            this.lblPwmDiagStatus = new System.Windows.Forms.Label();
            this.pnlInfo.SuspendLayout();
            this.pnlLeft.SuspendLayout();
            this.tabMain.SuspendLayout();
            this.tabLedCtrl.SuspendLayout();
            this.grpLedTx.SuspendLayout();
            this.grpLedColor.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBright)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeIn)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeOut)).BeginInit();
            this.grpEfficiency.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudUsedLed)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudOverlap)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff0)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxInt)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudOlActive)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff3)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCurSrc)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff4)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff5)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff6)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff7)).BeginInit();
            this.grpSF.SuspendLayout();
            this.tabCalib.SuspendLayout();
            this.grpCalib.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibLedSel)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibRX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibRY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibRZ)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibGX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibGY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibGZ)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibBX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibBY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibBZ)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvCalib)).BeginInit();
            this.tabProdInfo.SuspendLayout();
            this.grpProdWrite.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrNAD)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrMstNAD)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrSlaveFlag)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrColorTbl)).BeginInit();
            this.grpUtcLotWrite.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT0)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT3)).BeginInit();
            this.tabPwmDiag.SuspendLayout();
            this.tabParamTuning.SuspendLayout();
            this.grpPwmCtrl.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmPort)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmDuty)).BeginInit();
            this.grpPwmMax.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmMaxMask0)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmMaxMask1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmMaxMask2)).BeginInit();
            this.grpDiagRead.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudPnVoltOpt)).BeginInit();
            this.SuspendLayout();
            // 
            // pnlInfo
            // 
            this.pnlInfo.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(18)))), ((int)(((byte)(22)))), ((int)(((byte)(34)))));
            this.pnlInfo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlInfo.Controls.Add(this.lblTypCap);
            this.pnlInfo.Controls.Add(this.lblTypVal);
            this.pnlInfo.Controls.Add(this.lblProjCap);
            this.pnlInfo.Controls.Add(this.lblProjVal);
            this.pnlInfo.Controls.Add(this.lblSdkCap);
            this.pnlInfo.Controls.Add(this.lblSdkVal);
            this.pnlInfo.Controls.Add(this.lblRelCap);
            this.pnlInfo.Controls.Add(this.lblReleaseVal);
            this.pnlInfo.Controls.Add(this.lblMLMCap);
            this.pnlInfo.Controls.Add(this.lblMLMVal);
            this.pnlInfo.Controls.Add(this.lblMLSCap);
            this.pnlInfo.Controls.Add(this.lblMLSVal);
            this.pnlInfo.Controls.Add(this.lblFlagCap);
            this.pnlInfo.Controls.Add(this.lblFlagVal);
            this.pnlInfo.Controls.Add(this.lblSerialCap);
            this.pnlInfo.Controls.Add(this.lblSerialVal);
            this.pnlInfo.Controls.Add(this.lblTempCap);
            this.pnlInfo.Controls.Add(this.lblTempVal);
            this.pnlInfo.Controls.Add(this.lblVoltCap);
            this.pnlInfo.Controls.Add(this.lblVoltVal);
            this.pnlInfo.Controls.Add(this.lblDtcOpen);
            this.pnlInfo.Controls.Add(this.lblDtcClose);
            this.pnlInfo.Controls.Add(this.lblDtcCrit);
            this.pnlInfo.Controls.Add(this.lblDtcOk);
            this.pnlInfo.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlInfo.Location = new System.Drawing.Point(0, 0);
            this.pnlInfo.Name = "pnlInfo";
            this.pnlInfo.Size = new System.Drawing.Size(1164, 50);
            this.pnlInfo.TabIndex = 2;
            // 
            // lblTypCap
            // 
            this.lblTypCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblTypCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblTypCap.Location = new System.Drawing.Point(8, 4);
            this.lblTypCap.Name = "lblTypCap";
            this.lblTypCap.Size = new System.Drawing.Size(100, 14);
            this.lblTypCap.TabIndex = 0;
            this.lblTypCap.Text = "TYPE";
            // 
            // lblTypVal
            // 
            this.lblTypVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblTypVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblTypVal.Location = new System.Drawing.Point(8, 20);
            this.lblTypVal.Name = "lblTypVal";
            this.lblTypVal.Size = new System.Drawing.Size(100, 16);
            this.lblTypVal.TabIndex = 1;
            this.lblTypVal.Text = "--";
            // 
            // lblProjCap
            // 
            this.lblProjCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblProjCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblProjCap.Location = new System.Drawing.Point(110, 4);
            this.lblProjCap.Name = "lblProjCap";
            this.lblProjCap.Size = new System.Drawing.Size(100, 14);
            this.lblProjCap.TabIndex = 2;
            this.lblProjCap.Text = "PROJ";
            // 
            // lblProjVal
            // 
            this.lblProjVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblProjVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblProjVal.Location = new System.Drawing.Point(110, 20);
            this.lblProjVal.Name = "lblProjVal";
            this.lblProjVal.Size = new System.Drawing.Size(100, 16);
            this.lblProjVal.TabIndex = 3;
            this.lblProjVal.Text = "--";
            // 
            // lblSdkCap
            // 
            this.lblSdkCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblSdkCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblSdkCap.Location = new System.Drawing.Point(220, 4);
            this.lblSdkCap.Name = "lblSdkCap";
            this.lblSdkCap.Size = new System.Drawing.Size(80, 14);
            this.lblSdkCap.TabIndex = 4;
            this.lblSdkCap.Text = "SDK";
            // 
            // lblSdkVal
            // 
            this.lblSdkVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblSdkVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblSdkVal.Location = new System.Drawing.Point(220, 20);
            this.lblSdkVal.Name = "lblSdkVal";
            this.lblSdkVal.Size = new System.Drawing.Size(80, 16);
            this.lblSdkVal.TabIndex = 5;
            this.lblSdkVal.Text = "--";
            // 
            // lblRelCap
            // 
            this.lblRelCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblRelCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblRelCap.Location = new System.Drawing.Point(310, 4);
            this.lblRelCap.Name = "lblRelCap";
            this.lblRelCap.Size = new System.Drawing.Size(100, 14);
            this.lblRelCap.TabIndex = 6;
            this.lblRelCap.Text = "Release";
            // 
            // lblReleaseVal
            // 
            this.lblReleaseVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblReleaseVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblReleaseVal.Location = new System.Drawing.Point(310, 20);
            this.lblReleaseVal.Name = "lblReleaseVal";
            this.lblReleaseVal.Size = new System.Drawing.Size(100, 16);
            this.lblReleaseVal.TabIndex = 7;
            this.lblReleaseVal.Text = "--";
            // 
            // lblMLMCap
            // 
            this.lblMLMCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblMLMCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblMLMCap.Location = new System.Drawing.Point(420, 4);
            this.lblMLMCap.Name = "lblMLMCap";
            this.lblMLMCap.Size = new System.Drawing.Size(100, 14);
            this.lblMLMCap.TabIndex = 8;
            this.lblMLMCap.Text = "ML";
            // 
            // lblMLMVal
            // 
            this.lblMLMVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblMLMVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblMLMVal.Location = new System.Drawing.Point(420, 20);
            this.lblMLMVal.Name = "lblMLMVal";
            this.lblMLMVal.Size = new System.Drawing.Size(100, 16);
            this.lblMLMVal.TabIndex = 9;
            this.lblMLMVal.Text = "--";
            // 
            // lblMLSCap
            // 
            this.lblMLSCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblMLSCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblMLSCap.Location = new System.Drawing.Point(530, 4);
            this.lblMLSCap.Name = "lblMLSCap";
            this.lblMLSCap.Size = new System.Drawing.Size(100, 14);
            this.lblMLSCap.TabIndex = 10;
            this.lblMLSCap.Text = "MLS";
            // 
            // lblMLSVal
            // 
            this.lblMLSVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblMLSVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblMLSVal.Location = new System.Drawing.Point(530, 20);
            this.lblMLSVal.Name = "lblMLSVal";
            this.lblMLSVal.Size = new System.Drawing.Size(100, 16);
            this.lblMLSVal.TabIndex = 11;
            this.lblMLSVal.Text = "--";
            // 
            // lblFlagCap
            // 
            this.lblFlagCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblFlagCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblFlagCap.Location = new System.Drawing.Point(640, 4);
            this.lblFlagCap.Name = "lblFlagCap";
            this.lblFlagCap.Size = new System.Drawing.Size(90, 14);
            this.lblFlagCap.TabIndex = 12;
            this.lblFlagCap.Text = "FLAG";
            // 
            // lblFlagVal
            // 
            this.lblFlagVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblFlagVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblFlagVal.Location = new System.Drawing.Point(640, 20);
            this.lblFlagVal.Name = "lblFlagVal";
            this.lblFlagVal.Size = new System.Drawing.Size(90, 16);
            this.lblFlagVal.TabIndex = 13;
            this.lblFlagVal.Text = "--";
            // 
            // lblSerialCap
            // 
            this.lblSerialCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblSerialCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblSerialCap.Location = new System.Drawing.Point(740, 4);
            this.lblSerialCap.Name = "lblSerialCap";
            this.lblSerialCap.Size = new System.Drawing.Size(200, 14);
            this.lblSerialCap.TabIndex = 14;
            this.lblSerialCap.Text = "Serial";
            // 
            // lblSerialVal
            // 
            this.lblSerialVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblSerialVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblSerialVal.Location = new System.Drawing.Point(740, 20);
            this.lblSerialVal.Name = "lblSerialVal";
            this.lblSerialVal.Size = new System.Drawing.Size(200, 16);
            this.lblSerialVal.TabIndex = 15;
            this.lblSerialVal.Text = "--";
            // 
            // lblTempCap
            // 
            this.lblTempCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblTempCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblTempCap.Location = new System.Drawing.Point(950, 4);
            this.lblTempCap.Name = "lblTempCap";
            this.lblTempCap.Size = new System.Drawing.Size(70, 14);
            this.lblTempCap.TabIndex = 16;
            this.lblTempCap.Text = "Temp.";
            // 
            // lblTempVal
            // 
            this.lblTempVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblTempVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblTempVal.Location = new System.Drawing.Point(950, 20);
            this.lblTempVal.Name = "lblTempVal";
            this.lblTempVal.Size = new System.Drawing.Size(70, 16);
            this.lblTempVal.TabIndex = 17;
            this.lblTempVal.Text = "--";
            // 
            // lblVoltCap
            // 
            this.lblVoltCap.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblVoltCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(90)))), ((int)(((byte)(110)))), ((int)(((byte)(130)))));
            this.lblVoltCap.Location = new System.Drawing.Point(1030, 4);
            this.lblVoltCap.Name = "lblVoltCap";
            this.lblVoltCap.Size = new System.Drawing.Size(70, 14);
            this.lblVoltCap.TabIndex = 18;
            this.lblVoltCap.Text = "Volt.";
            // 
            // lblVoltVal
            // 
            this.lblVoltVal.Font = new System.Drawing.Font("Consolas", 8.5F, System.Drawing.FontStyle.Bold);
            this.lblVoltVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(225)))), ((int)(((byte)(255)))));
            this.lblVoltVal.Location = new System.Drawing.Point(1030, 20);
            this.lblVoltVal.Name = "lblVoltVal";
            this.lblVoltVal.Size = new System.Drawing.Size(70, 16);
            this.lblVoltVal.TabIndex = 19;
            this.lblVoltVal.Text = "--";
            // 
            // lblDtcOpen
            // 
            this.lblDtcOpen.Font = new System.Drawing.Font("Consolas", 8F, System.Drawing.FontStyle.Bold);
            this.lblDtcOpen.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(80)))), ((int)(((byte)(50)))));
            this.lblDtcOpen.Location = new System.Drawing.Point(1110, 8);
            this.lblDtcOpen.Name = "lblDtcOpen";
            this.lblDtcOpen.Size = new System.Drawing.Size(50, 16);
            this.lblDtcOpen.TabIndex = 20;
            this.lblDtcOpen.Text = "OPEN";
            // 
            // lblDtcClose
            // 
            this.lblDtcClose.Font = new System.Drawing.Font("Consolas", 8F, System.Drawing.FontStyle.Bold);
            this.lblDtcClose.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(80)))), ((int)(((byte)(50)))));
            this.lblDtcClose.Location = new System.Drawing.Point(1162, 8);
            this.lblDtcClose.Name = "lblDtcClose";
            this.lblDtcClose.Size = new System.Drawing.Size(50, 16);
            this.lblDtcClose.TabIndex = 21;
            this.lblDtcClose.Text = "CLOSE";
            // 
            // lblDtcCrit
            // 
            this.lblDtcCrit.Font = new System.Drawing.Font("Consolas", 8F, System.Drawing.FontStyle.Bold);
            this.lblDtcCrit.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(80)))), ((int)(((byte)(50)))));
            this.lblDtcCrit.Location = new System.Drawing.Point(1216, 8);
            this.lblDtcCrit.Name = "lblDtcCrit";
            this.lblDtcCrit.Size = new System.Drawing.Size(50, 16);
            this.lblDtcCrit.TabIndex = 22;
            this.lblDtcCrit.Text = "CRIT";
            // 
            // lblDtcOk
            // 
            this.lblDtcOk.Font = new System.Drawing.Font("Consolas", 8F, System.Drawing.FontStyle.Bold);
            this.lblDtcOk.ForeColor = System.Drawing.Color.OrangeRed;
            this.lblDtcOk.Location = new System.Drawing.Point(1258, 8);
            this.lblDtcOk.Name = "lblDtcOk";
            this.lblDtcOk.Size = new System.Drawing.Size(30, 16);
            this.lblDtcOk.TabIndex = 23;
            this.lblDtcOk.Text = "OK";
            // 
            // pnlLeft
            // 
            this.pnlLeft.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(18)))), ((int)(((byte)(22)))), ((int)(((byte)(34)))));
            this.pnlLeft.Controls.Add(this.lstSlaves);
            this.pnlLeft.Controls.Add(this.lblScanStatus);
            this.pnlLeft.Controls.Add(this.btnAddNode);
            this.pnlLeft.Controls.Add(this.btnScan);
            this.pnlLeft.Controls.Add(this.lblSlaveNodes);
            this.pnlLeft.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlLeft.Location = new System.Drawing.Point(0, 50);
            this.pnlLeft.Name = "pnlLeft";
            this.pnlLeft.Size = new System.Drawing.Size(130, 596);
            this.pnlLeft.TabIndex = 1;
            // 
            // lstSlaves
            // 
            this.lstSlaves.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(28)))), ((int)(((byte)(40)))));
            this.lstSlaves.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.lstSlaves.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstSlaves.Font = new System.Drawing.Font("Consolas", 8F);
            this.lstSlaves.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(210)))), ((int)(((byte)(240)))));
            this.lstSlaves.ItemHeight = 19;
            this.lstSlaves.Location = new System.Drawing.Point(0, 24);
            this.lstSlaves.Name = "lstSlaves";
            this.lstSlaves.Size = new System.Drawing.Size(130, 492);
            this.lstSlaves.TabIndex = 0;
            this.lstSlaves.SelectedIndexChanged += new System.EventHandler(this.lstSlaves_SelectedIndexChanged);
            // 
            // lblScanStatus
            // 
            this.lblScanStatus.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.lblScanStatus.Font = new System.Drawing.Font("Segoe UI", 7.5F);
            this.lblScanStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblScanStatus.Location = new System.Drawing.Point(0, 516);
            this.lblScanStatus.Name = "lblScanStatus";
            this.lblScanStatus.Size = new System.Drawing.Size(130, 22);
            this.lblScanStatus.TabIndex = 1;
            this.lblScanStatus.Text = "대기 중";
            this.lblScanStatus.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btnAddNode
            // 
            this.btnAddNode.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(100)))), ((int)(((byte)(60)))));
            this.btnAddNode.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.btnAddNode.FlatAppearance.BorderSize = 0;
            this.btnAddNode.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnAddNode.Font = new System.Drawing.Font("Segoe UI", 8F, System.Drawing.FontStyle.Bold);
            this.btnAddNode.ForeColor = System.Drawing.Color.White;
            this.btnAddNode.Location = new System.Drawing.Point(0, 538);
            this.btnAddNode.Name = "btnAddNode";
            this.btnAddNode.Size = new System.Drawing.Size(130, 28);
            this.btnAddNode.TabIndex = 2;
            this.btnAddNode.Text = "+ Add Node";
            this.btnAddNode.UseVisualStyleBackColor = false;
            this.btnAddNode.Click += new System.EventHandler(this.btnAddNode_Click);
            // 
            // btnScan
            // 
            this.btnScan.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(50)))), ((int)(((byte)(100)))), ((int)(((byte)(170)))));
            this.btnScan.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.btnScan.FlatAppearance.BorderSize = 0;
            this.btnScan.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnScan.Font = new System.Drawing.Font("Segoe UI", 8.5F, System.Drawing.FontStyle.Bold);
            this.btnScan.ForeColor = System.Drawing.Color.White;
            this.btnScan.Location = new System.Drawing.Point(0, 566);
            this.btnScan.Name = "btnScan";
            this.btnScan.Size = new System.Drawing.Size(130, 30);
            this.btnScan.TabIndex = 3;
            this.btnScan.Text = "Node Scan";
            this.btnScan.UseVisualStyleBackColor = false;
            this.btnScan.Click += new System.EventHandler(this.btnScan_Click);
            // 
            // lblSlaveNodes
            // 
            this.lblSlaveNodes.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(25)))), ((int)(((byte)(30)))), ((int)(((byte)(48)))));
            this.lblSlaveNodes.Dock = System.Windows.Forms.DockStyle.Top;
            this.lblSlaveNodes.Font = new System.Drawing.Font("Segoe UI", 8F, System.Drawing.FontStyle.Bold);
            this.lblSlaveNodes.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(190)))), ((int)(((byte)(220)))));
            this.lblSlaveNodes.Location = new System.Drawing.Point(0, 0);
            this.lblSlaveNodes.Name = "lblSlaveNodes";
            this.lblSlaveNodes.Size = new System.Drawing.Size(130, 24);
            this.lblSlaveNodes.TabIndex = 4;
            this.lblSlaveNodes.Text = "Slave Nodes";
            this.lblSlaveNodes.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // tabMain
            // 
            this.tabMain.Controls.Add(this.tabLedCtrl);
            this.tabMain.Controls.Add(this.tabCalib);
            this.tabMain.Controls.Add(this.tabProdInfo);
            this.tabMain.Controls.Add(this.tabPwmDiag);
            this.tabMain.Controls.Add(this.tabParamTuning);
            this.tabMain.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.tabMain.Location = new System.Drawing.Point(145, 74);
            this.tabMain.Name = "tabMain";
            this.tabMain.SelectedIndex = 0;
            this.tabMain.Size = new System.Drawing.Size(994, 560);
            this.tabMain.TabIndex = 0;
            // 
            // tabLedCtrl
            // 
            this.tabLedCtrl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.tabLedCtrl.Controls.Add(this.grpLedTx);
            this.tabLedCtrl.Controls.Add(this.grpLedColor);
            this.tabLedCtrl.Controls.Add(this.grpEfficiency);
            this.tabLedCtrl.Controls.Add(this.grpSF);
            this.tabLedCtrl.Location = new System.Drawing.Point(4, 31);
            this.tabLedCtrl.Name = "tabLedCtrl";
            this.tabLedCtrl.Size = new System.Drawing.Size(986, 525);
            this.tabLedCtrl.TabIndex = 0;
            this.tabLedCtrl.Text = "  LED Ctrl  ";
            // 
            // grpLedTx
            // 
            this.grpLedTx.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(14)))), ((int)(((byte)(16)))), ((int)(((byte)(26)))));
            this.grpLedTx.Controls.Add(this.btnLedSend);
            this.grpLedTx.Controls.Add(this.btnLedStart);
            this.grpLedTx.Controls.Add(this.lblFramePreview);
            this.grpLedTx.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpLedTx.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(200)))), ((int)(((byte)(160)))));
            this.grpLedTx.Location = new System.Drawing.Point(6, 146);
            this.grpLedTx.Name = "grpLedTx";
            this.grpLedTx.Size = new System.Drawing.Size(870, 82);
            this.grpLedTx.TabIndex = 0;
            this.grpLedTx.TabStop = false;
            this.grpLedTx.Text = "[ LIN TX ]";
            // 
            // btnLedSend
            // 
            this.btnLedSend.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(130)))), ((int)(((byte)(70)))));
            this.btnLedSend.FlatAppearance.BorderSize = 0;
            this.btnLedSend.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnLedSend.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnLedSend.ForeColor = System.Drawing.Color.White;
            this.btnLedSend.Location = new System.Drawing.Point(8, 26);
            this.btnLedSend.Name = "btnLedSend";
            this.btnLedSend.Size = new System.Drawing.Size(110, 32);
            this.btnLedSend.TabIndex = 0;
            this.btnLedSend.Text = "▶ 1회 전송";
            this.btnLedSend.UseVisualStyleBackColor = false;
            this.btnLedSend.Click += new System.EventHandler(this.btnLedSend_Click);
            // 
            // btnLedStart
            // 
            this.btnLedStart.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(100)))), ((int)(((byte)(180)))));
            this.btnLedStart.FlatAppearance.BorderSize = 0;
            this.btnLedStart.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnLedStart.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnLedStart.ForeColor = System.Drawing.Color.White;
            this.btnLedStart.Location = new System.Drawing.Point(126, 26);
            this.btnLedStart.Name = "btnLedStart";
            this.btnLedStart.Size = new System.Drawing.Size(140, 32);
            this.btnLedStart.TabIndex = 1;
            this.btnLedStart.Text = "▶ LED Ctrl 시작";
            this.btnLedStart.UseVisualStyleBackColor = false;
            this.btnLedStart.Click += new System.EventHandler(this.btnLedStart_Click);
            // 
            // lblFramePreview
            // 
            this.lblFramePreview.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblFramePreview.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(200)))), ((int)(((byte)(160)))));
            this.lblFramePreview.Location = new System.Drawing.Point(8, 62);
            this.lblFramePreview.Name = "lblFramePreview";
            this.lblFramePreview.Size = new System.Drawing.Size(852, 18);
            this.lblFramePreview.TabIndex = 3;
            this.lblFramePreview.Text = "--";
            // 
            // grpLedColor
            // 
            this.grpLedColor.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpLedColor.Controls.Add(this.lblLedCX);
            this.grpLedColor.Controls.Add(this.nudColorX);
            this.grpLedColor.Controls.Add(this.lblLedCY);
            this.grpLedColor.Controls.Add(this.nudColorY);
            this.grpLedColor.Controls.Add(this.lblLedBr);
            this.grpLedColor.Controls.Add(this.nudBright);
            this.grpLedColor.Controls.Add(this.lblLedFI);
            this.grpLedColor.Controls.Add(this.nudFadeIn);
            this.grpLedColor.Controls.Add(this.lblLedFO);
            this.grpLedColor.Controls.Add(this.nudFadeOut);
            this.grpLedColor.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpLedColor.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpLedColor.Location = new System.Drawing.Point(6, 72);
            this.grpLedColor.Name = "grpLedColor";
            this.grpLedColor.Size = new System.Drawing.Size(870, 66);
            this.grpLedColor.TabIndex = 1;
            this.grpLedColor.TabStop = false;
            this.grpLedColor.Text = "[ Color / Brightness% / Fade ]";
            // 
            // lblLedCX
            // 
            this.lblLedCX.AutoSize = true;
            this.lblLedCX.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblLedCX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblLedCX.Location = new System.Drawing.Point(8, 34);
            this.lblLedCX.Name = "lblLedCX";
            this.lblLedCX.Size = new System.Drawing.Size(80, 22);
            this.lblLedCX.TabIndex = 0;
            this.lblLedCX.Text = "ColorX:";
            // 
            // nudColorX
            // 
            this.nudColorX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudColorX.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudColorX.ForeColor = System.Drawing.Color.White;
            this.nudColorX.Location = new System.Drawing.Point(72, 30);
            this.nudColorX.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudColorX.Name = "nudColorX";
            this.nudColorX.Size = new System.Drawing.Size(62, 29);
            this.nudColorX.TabIndex = 0;
            this.nudColorX.Value = new decimal(new int[] {
            101,
            0,
            0,
            0});
            this.nudColorX.ValueChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // lblLedCY
            // 
            this.lblLedCY.AutoSize = true;
            this.lblLedCY.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblLedCY.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblLedCY.Location = new System.Drawing.Point(158, 34);
            this.lblLedCY.Name = "lblLedCY";
            this.lblLedCY.Size = new System.Drawing.Size(80, 22);
            this.lblLedCY.TabIndex = 1;
            this.lblLedCY.Text = "ColorY:";
            // 
            // nudColorY
            // 
            this.nudColorY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudColorY.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudColorY.ForeColor = System.Drawing.Color.White;
            this.nudColorY.Location = new System.Drawing.Point(222, 30);
            this.nudColorY.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudColorY.Name = "nudColorY";
            this.nudColorY.Size = new System.Drawing.Size(62, 29);
            this.nudColorY.TabIndex = 1;
            this.nudColorY.Value = new decimal(new int[] {
            58,
            0,
            0,
            0});
            this.nudColorY.ValueChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // lblLedBr
            // 
            this.lblLedBr.AutoSize = true;
            this.lblLedBr.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblLedBr.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblLedBr.Location = new System.Drawing.Point(308, 34);
            this.lblLedBr.Name = "lblLedBr";
            this.lblLedBr.Size = new System.Drawing.Size(90, 22);
            this.lblLedBr.TabIndex = 2;
            this.lblLedBr.Text = "Bright%:";
            // 
            // nudBright
            // 
            this.nudBright.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudBright.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudBright.ForeColor = System.Drawing.Color.White;
            this.nudBright.Location = new System.Drawing.Point(378, 30);
            this.nudBright.Name = "nudBright";
            this.nudBright.Size = new System.Drawing.Size(62, 29);
            this.nudBright.TabIndex = 2;
            this.nudBright.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            this.nudBright.ValueChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // lblLedFI
            // 
            this.lblLedFI.AutoSize = true;
            this.lblLedFI.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblLedFI.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblLedFI.Location = new System.Drawing.Point(444, 34);
            this.lblLedFI.Name = "lblLedFI";
            this.lblLedFI.Size = new System.Drawing.Size(80, 22);
            this.lblLedFI.TabIndex = 3;
            this.lblLedFI.Text = "FadeIn:";
            // 
            // nudFadeIn
            // 
            this.nudFadeIn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudFadeIn.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudFadeIn.ForeColor = System.Drawing.Color.White;
            this.nudFadeIn.Location = new System.Drawing.Point(514, 30);
            this.nudFadeIn.Maximum = new decimal(new int[] {
            254,
            0,
            0,
            0});
            this.nudFadeIn.Name = "nudFadeIn";
            this.nudFadeIn.Size = new System.Drawing.Size(62, 29);
            this.nudFadeIn.TabIndex = 3;
            this.nudFadeIn.ValueChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // lblLedFO
            // 
            this.lblLedFO.AutoSize = true;
            this.lblLedFO.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblLedFO.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(190)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblLedFO.Location = new System.Drawing.Point(580, 34);
            this.lblLedFO.Name = "lblLedFO";
            this.lblLedFO.Size = new System.Drawing.Size(90, 22);
            this.lblLedFO.TabIndex = 4;
            this.lblLedFO.Text = "FadeOut:";
            // 
            // nudFadeOut
            // 
            this.nudFadeOut.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(64)))));
            this.nudFadeOut.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudFadeOut.ForeColor = System.Drawing.Color.White;
            this.nudFadeOut.Location = new System.Drawing.Point(650, 30);
            this.nudFadeOut.Maximum = new decimal(new int[] {
            254,
            0,
            0,
            0});
            this.nudFadeOut.Name = "nudFadeOut";
            this.nudFadeOut.Size = new System.Drawing.Size(62, 29);
            this.nudFadeOut.TabIndex = 4;
            this.nudFadeOut.ValueChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // grpEfficiency
            // 
            this.grpEfficiency.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpEfficiency.Controls.Add(this.lblUsedLed);
            this.grpEfficiency.Controls.Add(this.nudUsedLed);
            this.grpEfficiency.Controls.Add(this.btnDevGet);
            this.grpEfficiency.Controls.Add(this.btnDevSet);
            this.grpEfficiency.Controls.Add(this.lblOverlap);
            this.grpEfficiency.Controls.Add(this.lblDevStatus);
            this.grpEfficiency.Controls.Add(this.lblEff0);
            this.grpEfficiency.Controls.Add(this.nudOverlap);
            this.grpEfficiency.Controls.Add(this.nudEff0);
            this.grpEfficiency.Controls.Add(this.lblEff1);
            this.grpEfficiency.Controls.Add(this.lblOlActive);
            this.grpEfficiency.Controls.Add(this.nudEff1);
            this.grpEfficiency.Controls.Add(this.nudMaxInt);
            this.grpEfficiency.Controls.Add(this.lblEff2);
            this.grpEfficiency.Controls.Add(this.nudOlActive);
            this.grpEfficiency.Controls.Add(this.nudEff2);
            this.grpEfficiency.Controls.Add(this.lblMaxInt);
            this.grpEfficiency.Controls.Add(this.lblEff3);
            this.grpEfficiency.Controls.Add(this.lblCurSrc);
            this.grpEfficiency.Controls.Add(this.nudEff3);
            this.grpEfficiency.Controls.Add(this.nudCurSrc);
            this.grpEfficiency.Controls.Add(this.lblEff4);
            this.grpEfficiency.Controls.Add(this.nudEff4);
            this.grpEfficiency.Controls.Add(this.lblEff5);
            this.grpEfficiency.Controls.Add(this.nudEff5);
            this.grpEfficiency.Controls.Add(this.lblEff6);
            this.grpEfficiency.Controls.Add(this.nudEff6);
            this.grpEfficiency.Controls.Add(this.lblEff7);
            this.grpEfficiency.Controls.Add(this.nudEff7);
            this.grpEfficiency.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpEfficiency.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpEfficiency.Location = new System.Drawing.Point(6, 234);
            this.grpEfficiency.Name = "grpEfficiency";
            this.grpEfficiency.Size = new System.Drawing.Size(960, 172);
            this.grpEfficiency.TabIndex = 1;
            this.grpEfficiency.TabStop = false;
            this.grpEfficiency.Text = "[ (1) LED Efficiency  GET=읽기  SET=쓰기 ]";
            // 
            // lblUsedLed
            // 
            this.lblUsedLed.AutoSize = true;
            this.lblUsedLed.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblUsedLed.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblUsedLed.Location = new System.Drawing.Point(8, 79);
            this.lblUsedLed.Name = "lblUsedLed";
            this.lblUsedLed.Size = new System.Drawing.Size(100, 22);
            this.lblUsedLed.TabIndex = 0;
            this.lblUsedLed.Text = "Used LED:";
            // 
            // nudUsedLed
            // 
            this.nudUsedLed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudUsedLed.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudUsedLed.ForeColor = System.Drawing.Color.White;
            this.nudUsedLed.Location = new System.Drawing.Point(114, 76);
            this.nudUsedLed.Maximum = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.nudUsedLed.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudUsedLed.Name = "nudUsedLed";
            this.nudUsedLed.Size = new System.Drawing.Size(68, 27);
            this.nudUsedLed.TabIndex = 0;
            this.nudUsedLed.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // btnDevGet
            // 
            this.btnDevGet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(110)))), ((int)(((byte)(60)))));
            this.btnDevGet.FlatAppearance.BorderSize = 0;
            this.btnDevGet.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnDevGet.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnDevGet.ForeColor = System.Drawing.Color.White;
            this.btnDevGet.Location = new System.Drawing.Point(12, 128);
            this.btnDevGet.Name = "btnDevGet";
            this.btnDevGet.Size = new System.Drawing.Size(50, 26);
            this.btnDevGet.TabIndex = 0;
            this.btnDevGet.Text = "GET";
            this.btnDevGet.UseVisualStyleBackColor = false;
            this.btnDevGet.Click += new System.EventHandler(this.btnDevGet_Click);
            // 
            // btnDevSet
            // 
            this.btnDevSet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(110)))), ((int)(((byte)(60)))), ((int)(((byte)(40)))));
            this.btnDevSet.FlatAppearance.BorderSize = 0;
            this.btnDevSet.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnDevSet.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnDevSet.ForeColor = System.Drawing.Color.White;
            this.btnDevSet.Location = new System.Drawing.Point(66, 128);
            this.btnDevSet.Name = "btnDevSet";
            this.btnDevSet.Size = new System.Drawing.Size(50, 26);
            this.btnDevSet.TabIndex = 1;
            this.btnDevSet.Text = "SET";
            this.btnDevSet.UseVisualStyleBackColor = false;
            this.btnDevSet.Click += new System.EventHandler(this.btnDevSet_Click);
            // 
            // lblOverlap
            // 
            this.lblOverlap.AutoSize = true;
            this.lblOverlap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblOverlap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblOverlap.Location = new System.Drawing.Point(194, 79);
            this.lblOverlap.Name = "lblOverlap";
            this.lblOverlap.Size = new System.Drawing.Size(90, 22);
            this.lblOverlap.TabIndex = 1;
            this.lblOverlap.Text = "Overlap:";
            // 
            // lblDevStatus
            // 
            this.lblDevStatus.AutoSize = true;
            this.lblDevStatus.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblDevStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblDevStatus.Location = new System.Drawing.Point(128, 133);
            this.lblDevStatus.Name = "lblDevStatus";
            this.lblDevStatus.Size = new System.Drawing.Size(0, 21);
            this.lblDevStatus.TabIndex = 2;
            // 
            // lblEff0
            // 
            this.lblEff0.AutoSize = true;
            this.lblEff0.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff0.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff0.Location = new System.Drawing.Point(25, 25);
            this.lblEff0.Name = "lblEff0";
            this.lblEff0.Size = new System.Drawing.Size(45, 19);
            this.lblEff0.TabIndex = 3;
            this.lblEff0.Text = "LED0";
            // 
            // nudOverlap
            // 
            this.nudOverlap.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudOverlap.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudOverlap.ForeColor = System.Drawing.Color.White;
            this.nudOverlap.Location = new System.Drawing.Point(293, 76);
            this.nudOverlap.Maximum = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.nudOverlap.Name = "nudOverlap";
            this.nudOverlap.Size = new System.Drawing.Size(68, 27);
            this.nudOverlap.TabIndex = 0;
            // 
            // nudEff0
            // 
            this.nudEff0.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff0.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff0.ForeColor = System.Drawing.Color.White;
            this.nudEff0.Location = new System.Drawing.Point(26, 41);
            this.nudEff0.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff0.Name = "nudEff0";
            this.nudEff0.Size = new System.Drawing.Size(70, 27);
            this.nudEff0.TabIndex = 0;
            this.nudEff0.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // lblEff1
            // 
            this.lblEff1.AutoSize = true;
            this.lblEff1.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff1.Location = new System.Drawing.Point(120, 25);
            this.lblEff1.Name = "lblEff1";
            this.lblEff1.Size = new System.Drawing.Size(45, 19);
            this.lblEff1.TabIndex = 4;
            this.lblEff1.Text = "LED1";
            // 
            // lblOlActive
            // 
            this.lblOlActive.AutoSize = true;
            this.lblOlActive.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblOlActive.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblOlActive.Location = new System.Drawing.Point(379, 79);
            this.lblOlActive.Name = "lblOlActive";
            this.lblOlActive.Size = new System.Drawing.Size(110, 22);
            this.lblOlActive.TabIndex = 2;
            this.lblOlActive.Text = "OL Active:";
            // 
            // nudEff1
            // 
            this.nudEff1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff1.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff1.ForeColor = System.Drawing.Color.White;
            this.nudEff1.Location = new System.Drawing.Point(115, 41);
            this.nudEff1.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff1.Name = "nudEff1";
            this.nudEff1.Size = new System.Drawing.Size(70, 27);
            this.nudEff1.TabIndex = 1;
            this.nudEff1.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // nudMaxInt
            // 
            this.nudMaxInt.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudMaxInt.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudMaxInt.ForeColor = System.Drawing.Color.White;
            this.nudMaxInt.Location = new System.Drawing.Point(862, 78);
            this.nudMaxInt.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudMaxInt.Name = "nudMaxInt";
            this.nudMaxInt.Size = new System.Drawing.Size(68, 27);
            this.nudMaxInt.TabIndex = 0;
            this.nudMaxInt.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // lblEff2
            // 
            this.lblEff2.AutoSize = true;
            this.lblEff2.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff2.Location = new System.Drawing.Point(215, 25);
            this.lblEff2.Name = "lblEff2";
            this.lblEff2.Size = new System.Drawing.Size(45, 19);
            this.lblEff2.TabIndex = 5;
            this.lblEff2.Text = "LED2";
            // 
            // nudOlActive
            // 
            this.nudOlActive.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudOlActive.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudOlActive.ForeColor = System.Drawing.Color.White;
            this.nudOlActive.Location = new System.Drawing.Point(497, 76);
            this.nudOlActive.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudOlActive.Name = "nudOlActive";
            this.nudOlActive.Size = new System.Drawing.Size(68, 27);
            this.nudOlActive.TabIndex = 0;
            this.nudOlActive.Value = new decimal(new int[] {
            4,
            0,
            0,
            0});
            // 
            // nudEff2
            // 
            this.nudEff2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff2.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff2.ForeColor = System.Drawing.Color.White;
            this.nudEff2.Location = new System.Drawing.Point(210, 41);
            this.nudEff2.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff2.Name = "nudEff2";
            this.nudEff2.Size = new System.Drawing.Size(70, 27);
            this.nudEff2.TabIndex = 2;
            this.nudEff2.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // lblMaxInt
            // 
            this.lblMaxInt.AutoSize = true;
            this.lblMaxInt.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblMaxInt.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblMaxInt.Location = new System.Drawing.Point(756, 79);
            this.lblMaxInt.Name = "lblMaxInt";
            this.lblMaxInt.Size = new System.Drawing.Size(100, 22);
            this.lblMaxInt.TabIndex = 4;
            this.lblMaxInt.Text = "Max Int.:";
            // 
            // lblEff3
            // 
            this.lblEff3.AutoSize = true;
            this.lblEff3.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff3.Location = new System.Drawing.Point(310, 25);
            this.lblEff3.Name = "lblEff3";
            this.lblEff3.Size = new System.Drawing.Size(45, 19);
            this.lblEff3.TabIndex = 6;
            this.lblEff3.Text = "LED3";
            // 
            // lblCurSrc
            // 
            this.lblCurSrc.AutoSize = true;
            this.lblCurSrc.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblCurSrc.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblCurSrc.Location = new System.Drawing.Point(580, 77);
            this.lblCurSrc.Name = "lblCurSrc";
            this.lblCurSrc.Size = new System.Drawing.Size(90, 22);
            this.lblCurSrc.TabIndex = 3;
            this.lblCurSrc.Text = "Cur(mA):";
            // 
            // nudEff3
            // 
            this.nudEff3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff3.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff3.ForeColor = System.Drawing.Color.White;
            this.nudEff3.Location = new System.Drawing.Point(305, 41);
            this.nudEff3.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff3.Name = "nudEff3";
            this.nudEff3.Size = new System.Drawing.Size(70, 27);
            this.nudEff3.TabIndex = 3;
            this.nudEff3.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // nudCurSrc
            // 
            this.nudCurSrc.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCurSrc.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCurSrc.ForeColor = System.Drawing.Color.White;
            this.nudCurSrc.Location = new System.Drawing.Point(676, 76);
            this.nudCurSrc.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudCurSrc.Name = "nudCurSrc";
            this.nudCurSrc.Size = new System.Drawing.Size(68, 27);
            this.nudCurSrc.TabIndex = 0;
            this.nudCurSrc.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // lblEff4
            // 
            this.lblEff4.AutoSize = true;
            this.lblEff4.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff4.Location = new System.Drawing.Point(405, 25);
            this.lblEff4.Name = "lblEff4";
            this.lblEff4.Size = new System.Drawing.Size(45, 19);
            this.lblEff4.TabIndex = 7;
            this.lblEff4.Text = "LED4";
            // 
            // nudEff4
            // 
            this.nudEff4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff4.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff4.ForeColor = System.Drawing.Color.White;
            this.nudEff4.Location = new System.Drawing.Point(400, 41);
            this.nudEff4.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff4.Name = "nudEff4";
            this.nudEff4.Size = new System.Drawing.Size(70, 27);
            this.nudEff4.TabIndex = 4;
            this.nudEff4.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // lblEff5
            // 
            this.lblEff5.AutoSize = true;
            this.lblEff5.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff5.Location = new System.Drawing.Point(500, 25);
            this.lblEff5.Name = "lblEff5";
            this.lblEff5.Size = new System.Drawing.Size(45, 19);
            this.lblEff5.TabIndex = 8;
            this.lblEff5.Text = "LED5";
            // 
            // nudEff5
            // 
            this.nudEff5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff5.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff5.ForeColor = System.Drawing.Color.White;
            this.nudEff5.Location = new System.Drawing.Point(495, 41);
            this.nudEff5.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff5.Name = "nudEff5";
            this.nudEff5.Size = new System.Drawing.Size(70, 27);
            this.nudEff5.TabIndex = 5;
            this.nudEff5.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // lblEff6
            // 
            this.lblEff6.AutoSize = true;
            this.lblEff6.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff6.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff6.Location = new System.Drawing.Point(595, 25);
            this.lblEff6.Name = "lblEff6";
            this.lblEff6.Size = new System.Drawing.Size(45, 19);
            this.lblEff6.TabIndex = 9;
            this.lblEff6.Text = "LED6";
            // 
            // nudEff6
            // 
            this.nudEff6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff6.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff6.ForeColor = System.Drawing.Color.White;
            this.nudEff6.Location = new System.Drawing.Point(590, 41);
            this.nudEff6.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff6.Name = "nudEff6";
            this.nudEff6.Size = new System.Drawing.Size(70, 27);
            this.nudEff6.TabIndex = 6;
            this.nudEff6.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // lblEff7
            // 
            this.lblEff7.AutoSize = true;
            this.lblEff7.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblEff7.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblEff7.Location = new System.Drawing.Point(690, 25);
            this.lblEff7.Name = "lblEff7";
            this.lblEff7.Size = new System.Drawing.Size(45, 19);
            this.lblEff7.TabIndex = 10;
            this.lblEff7.Text = "LED7";
            // 
            // nudEff7
            // 
            this.nudEff7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudEff7.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudEff7.ForeColor = System.Drawing.Color.White;
            this.nudEff7.Location = new System.Drawing.Point(685, 41);
            this.nudEff7.Maximum = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nudEff7.Name = "nudEff7";
            this.nudEff7.Size = new System.Drawing.Size(70, 27);
            this.nudEff7.TabIndex = 7;
            this.nudEff7.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // grpSF
            // 
            this.grpSF.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpSF.Controls.Add(this.chkSF1);
            this.grpSF.Controls.Add(this.chkSF2);
            this.grpSF.Controls.Add(this.chkSF3);
            this.grpSF.Controls.Add(this.chkSF4);
            this.grpSF.Controls.Add(this.chkSF5);
            this.grpSF.Controls.Add(this.chkSF6);
            this.grpSF.Controls.Add(this.chkSF7);
            this.grpSF.Controls.Add(this.chkSF8);
            this.grpSF.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpSF.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpSF.Location = new System.Drawing.Point(6, 8);
            this.grpSF.Name = "grpSF";
            this.grpSF.Size = new System.Drawing.Size(700, 56);
            this.grpSF.TabIndex = 2;
            this.grpSF.TabStop = false;
            this.grpSF.Text = "[ DATA1  SlaveFlag ]";
            // 
            // chkSF1
            // 
            this.chkSF1.AutoSize = true;
            this.chkSF1.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF1.Location = new System.Drawing.Point(8, 26);
            this.chkSF1.Name = "chkSF1";
            this.chkSF1.Size = new System.Drawing.Size(62, 24);
            this.chkSF1.TabIndex = 0;
            this.chkSF1.Text = "SF1";
            this.chkSF1.UseVisualStyleBackColor = false;
            this.chkSF1.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // chkSF2
            // 
            this.chkSF2.AutoSize = true;
            this.chkSF2.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF2.Location = new System.Drawing.Point(72, 26);
            this.chkSF2.Name = "chkSF2";
            this.chkSF2.Size = new System.Drawing.Size(62, 24);
            this.chkSF2.TabIndex = 1;
            this.chkSF2.Text = "SF2";
            this.chkSF2.UseVisualStyleBackColor = false;
            this.chkSF2.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // chkSF3
            // 
            this.chkSF3.AutoSize = true;
            this.chkSF3.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF3.Location = new System.Drawing.Point(136, 26);
            this.chkSF3.Name = "chkSF3";
            this.chkSF3.Size = new System.Drawing.Size(62, 24);
            this.chkSF3.TabIndex = 2;
            this.chkSF3.Text = "SF3";
            this.chkSF3.UseVisualStyleBackColor = false;
            this.chkSF3.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // chkSF4
            // 
            this.chkSF4.AutoSize = true;
            this.chkSF4.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF4.Location = new System.Drawing.Point(200, 26);
            this.chkSF4.Name = "chkSF4";
            this.chkSF4.Size = new System.Drawing.Size(62, 24);
            this.chkSF4.TabIndex = 3;
            this.chkSF4.Text = "SF4";
            this.chkSF4.UseVisualStyleBackColor = false;
            this.chkSF4.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // chkSF5
            // 
            this.chkSF5.AutoSize = true;
            this.chkSF5.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF5.Location = new System.Drawing.Point(264, 26);
            this.chkSF5.Name = "chkSF5";
            this.chkSF5.Size = new System.Drawing.Size(62, 24);
            this.chkSF5.TabIndex = 4;
            this.chkSF5.Text = "SF5";
            this.chkSF5.UseVisualStyleBackColor = false;
            this.chkSF5.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // chkSF6
            // 
            this.chkSF6.AutoSize = true;
            this.chkSF6.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF6.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF6.Location = new System.Drawing.Point(328, 26);
            this.chkSF6.Name = "chkSF6";
            this.chkSF6.Size = new System.Drawing.Size(62, 24);
            this.chkSF6.TabIndex = 5;
            this.chkSF6.Text = "SF6";
            this.chkSF6.UseVisualStyleBackColor = false;
            this.chkSF6.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // chkSF7
            // 
            this.chkSF7.AutoSize = true;
            this.chkSF7.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF7.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF7.Location = new System.Drawing.Point(392, 26);
            this.chkSF7.Name = "chkSF7";
            this.chkSF7.Size = new System.Drawing.Size(62, 24);
            this.chkSF7.TabIndex = 6;
            this.chkSF7.Text = "SF7";
            this.chkSF7.UseVisualStyleBackColor = false;
            this.chkSF7.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // chkSF8
            // 
            this.chkSF8.AutoSize = true;
            this.chkSF8.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.chkSF8.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(220)))), ((int)(((byte)(240)))));
            this.chkSF8.Location = new System.Drawing.Point(462, 26);
            this.chkSF8.Name = "chkSF8";
            this.chkSF8.Size = new System.Drawing.Size(62, 24);
            this.chkSF8.TabIndex = 7;
            this.chkSF8.Text = "SF8";
            this.chkSF8.UseVisualStyleBackColor = false;
            this.chkSF8.CheckedChanged += new System.EventHandler(this.OnLedCtrlChanged);
            // 
            // tabCalib
            // 
            this.tabCalib.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.tabCalib.Controls.Add(this.grpCalib);
            this.tabCalib.Controls.Add(this.dgvCalib);
            this.tabCalib.Location = new System.Drawing.Point(4, 31);
            this.tabCalib.Name = "tabCalib";
            this.tabCalib.Size = new System.Drawing.Size(986, 525);
            this.tabCalib.TabIndex = 2;
            this.tabCalib.Text = "  Calibration  ";
            // 
            // grpCalib
            // 
            this.grpCalib.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(26)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.grpCalib.Controls.Add(this.lblCalibLed);
            this.grpCalib.Controls.Add(this.nudCalibLedSel);
            this.grpCalib.Controls.Add(this.lblCalibBrightR);
            this.grpCalib.Controls.Add(this.lblCalibBrightG);
            this.grpCalib.Controls.Add(this.lblCalibBrightB);
            this.grpCalib.Controls.Add(this.lblCalibRX);
            this.grpCalib.Controls.Add(this.nudCalibRX);
            this.grpCalib.Controls.Add(this.lblCalibRY);
            this.grpCalib.Controls.Add(this.nudCalibRY);
            this.grpCalib.Controls.Add(this.lblCalibRZ);
            this.grpCalib.Controls.Add(this.nudCalibRZ);
            this.grpCalib.Controls.Add(this.lblCalibGX);
            this.grpCalib.Controls.Add(this.nudCalibGX);
            this.grpCalib.Controls.Add(this.lblCalibGY);
            this.grpCalib.Controls.Add(this.nudCalibGY);
            this.grpCalib.Controls.Add(this.lblCalibGZ);
            this.grpCalib.Controls.Add(this.nudCalibGZ);
            this.grpCalib.Controls.Add(this.lblCalibBX);
            this.grpCalib.Controls.Add(this.nudCalibBX);
            this.grpCalib.Controls.Add(this.lblCalibBY);
            this.grpCalib.Controls.Add(this.nudCalibBY);
            this.grpCalib.Controls.Add(this.lblCalibBZ);
            this.grpCalib.Controls.Add(this.nudCalibBZ);
            this.grpCalib.Controls.Add(this.btnCalibGet);
            this.grpCalib.Controls.Add(this.btnCalibGetAll);
            this.grpCalib.Controls.Add(this.btnCalibSet);
            this.grpCalib.Controls.Add(this.lblCalibStatus);
            this.grpCalib.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpCalib.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpCalib.Location = new System.Drawing.Point(6, 8);
            this.grpCalib.Name = "grpCalib";
            this.grpCalib.Size = new System.Drawing.Size(960, 204);
            this.grpCalib.TabIndex = 0;
            this.grpCalib.TabStop = false;
            this.grpCalib.Text = "[ Calibration RGB Data ]";
            // 
            // lblCalibLed
            // 
            this.lblCalibLed.AutoSize = true;
            this.lblCalibLed.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblCalibLed.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(180)))), ((int)(((byte)(200)))));
            this.lblCalibLed.Location = new System.Drawing.Point(8, 26);
            this.lblCalibLed.Name = "lblCalibLed";
            this.lblCalibLed.Size = new System.Drawing.Size(50, 22);
            this.lblCalibLed.TabIndex = 0;
            this.lblCalibLed.Text = "LED:";
            // 
            // nudCalibLedSel
            // 
            this.nudCalibLedSel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibLedSel.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibLedSel.ForeColor = System.Drawing.Color.White;
            this.nudCalibLedSel.Location = new System.Drawing.Point(54, 25);
            this.nudCalibLedSel.Maximum = new decimal(new int[] {
            7,
            0,
            0,
            0});
            this.nudCalibLedSel.Name = "nudCalibLedSel";
            this.nudCalibLedSel.Size = new System.Drawing.Size(50, 27);
            this.nudCalibLedSel.TabIndex = 1;
            this.nudCalibLedSel.ValueChanged += new System.EventHandler(this.nudCalibLedSel_ValueChanged);
            // 
            // lblCalibBrightR
            // 
            this.lblCalibBrightR.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblCalibBrightR.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(200)))), ((int)(((byte)(150)))));
            this.lblCalibBrightR.Location = new System.Drawing.Point(110, 26);
            this.lblCalibBrightR.Name = "lblCalibBrightR";
            this.lblCalibBrightR.Size = new System.Drawing.Size(240, 18);
            this.lblCalibBrightR.TabIndex = 2;
            this.lblCalibBrightR.Text = "Bright:--  x:--  y:--";
            // 
            // lblCalibBrightG
            // 
            this.lblCalibBrightG.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblCalibBrightG.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(200)))), ((int)(((byte)(150)))));
            this.lblCalibBrightG.Location = new System.Drawing.Point(460, 26);
            this.lblCalibBrightG.Name = "lblCalibBrightG";
            this.lblCalibBrightG.Size = new System.Drawing.Size(240, 18);
            this.lblCalibBrightG.TabIndex = 3;
            this.lblCalibBrightG.Text = "Bright:--  x:--  y:--";
            // 
            // lblCalibBrightB
            // 
            this.lblCalibBrightB.Font = new System.Drawing.Font("Consolas", 8F);
            this.lblCalibBrightB.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(200)))), ((int)(((byte)(150)))));
            this.lblCalibBrightB.Location = new System.Drawing.Point(720, 26);
            this.lblCalibBrightB.Name = "lblCalibBrightB";
            this.lblCalibBrightB.Size = new System.Drawing.Size(240, 18);
            this.lblCalibBrightB.TabIndex = 4;
            this.lblCalibBrightB.Text = "Bright:--  x:--  y:--";
            // 
            // lblCalibRX
            // 
            this.lblCalibRX.AutoSize = true;
            this.lblCalibRX.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibRX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(80)))), ((int)(((byte)(80)))));
            this.lblCalibRX.Location = new System.Drawing.Point(8, 56);
            this.lblCalibRX.Name = "lblCalibRX";
            this.lblCalibRX.Size = new System.Drawing.Size(60, 22);
            this.lblCalibRX.TabIndex = 5;
            this.lblCalibRX.Text = "R  X:";
            // 
            // nudCalibRX
            // 
            this.nudCalibRX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibRX.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibRX.ForeColor = System.Drawing.Color.White;
            this.nudCalibRX.Location = new System.Drawing.Point(70, 55);
            this.nudCalibRX.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibRX.Name = "nudCalibRX";
            this.nudCalibRX.Size = new System.Drawing.Size(75, 27);
            this.nudCalibRX.TabIndex = 0;
            this.nudCalibRX.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibRY
            // 
            this.lblCalibRY.AutoSize = true;
            this.lblCalibRY.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibRY.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(80)))), ((int)(((byte)(80)))));
            this.lblCalibRY.Location = new System.Drawing.Point(168, 56);
            this.lblCalibRY.Name = "lblCalibRY";
            this.lblCalibRY.Size = new System.Drawing.Size(30, 22);
            this.lblCalibRY.TabIndex = 6;
            this.lblCalibRY.Text = "Y:";
            // 
            // nudCalibRY
            // 
            this.nudCalibRY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibRY.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibRY.ForeColor = System.Drawing.Color.White;
            this.nudCalibRY.Location = new System.Drawing.Point(204, 55);
            this.nudCalibRY.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibRY.Name = "nudCalibRY";
            this.nudCalibRY.Size = new System.Drawing.Size(75, 27);
            this.nudCalibRY.TabIndex = 0;
            this.nudCalibRY.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibRZ
            // 
            this.lblCalibRZ.AutoSize = true;
            this.lblCalibRZ.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibRZ.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(80)))), ((int)(((byte)(80)))));
            this.lblCalibRZ.Location = new System.Drawing.Point(306, 56);
            this.lblCalibRZ.Name = "lblCalibRZ";
            this.lblCalibRZ.Size = new System.Drawing.Size(30, 22);
            this.lblCalibRZ.TabIndex = 7;
            this.lblCalibRZ.Text = "Z:";
            // 
            // nudCalibRZ
            // 
            this.nudCalibRZ.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibRZ.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibRZ.ForeColor = System.Drawing.Color.White;
            this.nudCalibRZ.Location = new System.Drawing.Point(342, 56);
            this.nudCalibRZ.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibRZ.Name = "nudCalibRZ";
            this.nudCalibRZ.Size = new System.Drawing.Size(75, 27);
            this.nudCalibRZ.TabIndex = 0;
            this.nudCalibRZ.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibGX
            // 
            this.lblCalibGX.AutoSize = true;
            this.lblCalibGX.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibGX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(200)))), ((int)(((byte)(80)))));
            this.lblCalibGX.Location = new System.Drawing.Point(8, 96);
            this.lblCalibGX.Name = "lblCalibGX";
            this.lblCalibGX.Size = new System.Drawing.Size(60, 22);
            this.lblCalibGX.TabIndex = 8;
            this.lblCalibGX.Text = "G  X:";
            // 
            // nudCalibGX
            // 
            this.nudCalibGX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibGX.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibGX.ForeColor = System.Drawing.Color.White;
            this.nudCalibGX.Location = new System.Drawing.Point(70, 95);
            this.nudCalibGX.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibGX.Name = "nudCalibGX";
            this.nudCalibGX.Size = new System.Drawing.Size(75, 27);
            this.nudCalibGX.TabIndex = 0;
            this.nudCalibGX.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibGY
            // 
            this.lblCalibGY.AutoSize = true;
            this.lblCalibGY.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibGY.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(200)))), ((int)(((byte)(80)))));
            this.lblCalibGY.Location = new System.Drawing.Point(168, 96);
            this.lblCalibGY.Name = "lblCalibGY";
            this.lblCalibGY.Size = new System.Drawing.Size(30, 22);
            this.lblCalibGY.TabIndex = 9;
            this.lblCalibGY.Text = "Y:";
            // 
            // nudCalibGY
            // 
            this.nudCalibGY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibGY.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibGY.ForeColor = System.Drawing.Color.White;
            this.nudCalibGY.Location = new System.Drawing.Point(204, 95);
            this.nudCalibGY.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibGY.Name = "nudCalibGY";
            this.nudCalibGY.Size = new System.Drawing.Size(75, 27);
            this.nudCalibGY.TabIndex = 0;
            this.nudCalibGY.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibGZ
            // 
            this.lblCalibGZ.AutoSize = true;
            this.lblCalibGZ.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibGZ.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(200)))), ((int)(((byte)(80)))));
            this.lblCalibGZ.Location = new System.Drawing.Point(306, 96);
            this.lblCalibGZ.Name = "lblCalibGZ";
            this.lblCalibGZ.Size = new System.Drawing.Size(30, 22);
            this.lblCalibGZ.TabIndex = 10;
            this.lblCalibGZ.Text = "Z:";
            // 
            // nudCalibGZ
            // 
            this.nudCalibGZ.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibGZ.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibGZ.ForeColor = System.Drawing.Color.White;
            this.nudCalibGZ.Location = new System.Drawing.Point(342, 96);
            this.nudCalibGZ.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibGZ.Name = "nudCalibGZ";
            this.nudCalibGZ.Size = new System.Drawing.Size(75, 27);
            this.nudCalibGZ.TabIndex = 0;
            this.nudCalibGZ.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibBX
            // 
            this.lblCalibBX.AutoSize = true;
            this.lblCalibBX.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibBX.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(130)))), ((int)(((byte)(255)))));
            this.lblCalibBX.Location = new System.Drawing.Point(8, 136);
            this.lblCalibBX.Name = "lblCalibBX";
            this.lblCalibBX.Size = new System.Drawing.Size(60, 22);
            this.lblCalibBX.TabIndex = 11;
            this.lblCalibBX.Text = "B  X:";
            // 
            // nudCalibBX
            // 
            this.nudCalibBX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibBX.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibBX.ForeColor = System.Drawing.Color.White;
            this.nudCalibBX.Location = new System.Drawing.Point(70, 135);
            this.nudCalibBX.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibBX.Name = "nudCalibBX";
            this.nudCalibBX.Size = new System.Drawing.Size(75, 27);
            this.nudCalibBX.TabIndex = 0;
            this.nudCalibBX.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibBY
            // 
            this.lblCalibBY.AutoSize = true;
            this.lblCalibBY.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibBY.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(130)))), ((int)(((byte)(255)))));
            this.lblCalibBY.Location = new System.Drawing.Point(168, 136);
            this.lblCalibBY.Name = "lblCalibBY";
            this.lblCalibBY.Size = new System.Drawing.Size(30, 22);
            this.lblCalibBY.TabIndex = 12;
            this.lblCalibBY.Text = "Y:";
            // 
            // nudCalibBY
            // 
            this.nudCalibBY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibBY.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibBY.ForeColor = System.Drawing.Color.White;
            this.nudCalibBY.Location = new System.Drawing.Point(204, 135);
            this.nudCalibBY.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibBY.Name = "nudCalibBY";
            this.nudCalibBY.Size = new System.Drawing.Size(75, 27);
            this.nudCalibBY.TabIndex = 0;
            this.nudCalibBY.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // lblCalibBZ
            // 
            this.lblCalibBZ.AutoSize = true;
            this.lblCalibBZ.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCalibBZ.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(130)))), ((int)(((byte)(255)))));
            this.lblCalibBZ.Location = new System.Drawing.Point(306, 136);
            this.lblCalibBZ.Name = "lblCalibBZ";
            this.lblCalibBZ.Size = new System.Drawing.Size(30, 22);
            this.lblCalibBZ.TabIndex = 13;
            this.lblCalibBZ.Text = "Z:";
            // 
            // nudCalibBZ
            // 
            this.nudCalibBZ.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudCalibBZ.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.nudCalibBZ.ForeColor = System.Drawing.Color.White;
            this.nudCalibBZ.Location = new System.Drawing.Point(342, 136);
            this.nudCalibBZ.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nudCalibBZ.Name = "nudCalibBZ";
            this.nudCalibBZ.Size = new System.Drawing.Size(75, 27);
            this.nudCalibBZ.TabIndex = 0;
            this.nudCalibBZ.ValueChanged += new System.EventHandler(this.OnCalibXyzChanged);
            // 
            // btnCalibGet
            // 
            this.btnCalibGet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(110)))), ((int)(((byte)(60)))));
            this.btnCalibGet.FlatAppearance.BorderSize = 0;
            this.btnCalibGet.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCalibGet.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnCalibGet.ForeColor = System.Drawing.Color.White;
            this.btnCalibGet.Location = new System.Drawing.Point(8, 162);
            this.btnCalibGet.Name = "btnCalibGet";
            this.btnCalibGet.Size = new System.Drawing.Size(70, 26);
            this.btnCalibGet.TabIndex = 14;
            this.btnCalibGet.Text = "GET";
            this.btnCalibGet.UseVisualStyleBackColor = false;
            this.btnCalibGet.Click += new System.EventHandler(this.btnCalibGet_Click);
            // 
            // btnCalibGetAll
            // 
            this.btnCalibGetAll.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(80)))), ((int)(((byte)(130)))));
            this.btnCalibGetAll.FlatAppearance.BorderSize = 0;
            this.btnCalibGetAll.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCalibGetAll.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnCalibGetAll.ForeColor = System.Drawing.Color.White;
            this.btnCalibGetAll.Location = new System.Drawing.Point(84, 162);
            this.btnCalibGetAll.Name = "btnCalibGetAll";
            this.btnCalibGetAll.Size = new System.Drawing.Size(80, 26);
            this.btnCalibGetAll.TabIndex = 15;
            this.btnCalibGetAll.Text = "GET All";
            this.btnCalibGetAll.UseVisualStyleBackColor = false;
            this.btnCalibGetAll.Click += new System.EventHandler(this.btnCalibGetAll_Click);
            // 
            // btnCalibSet
            // 
            this.btnCalibSet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(110)))), ((int)(((byte)(60)))), ((int)(((byte)(40)))));
            this.btnCalibSet.FlatAppearance.BorderSize = 0;
            this.btnCalibSet.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCalibSet.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnCalibSet.ForeColor = System.Drawing.Color.White;
            this.btnCalibSet.Location = new System.Drawing.Point(170, 162);
            this.btnCalibSet.Name = "btnCalibSet";
            this.btnCalibSet.Size = new System.Drawing.Size(70, 26);
            this.btnCalibSet.TabIndex = 16;
            this.btnCalibSet.Text = "SET";
            this.btnCalibSet.UseVisualStyleBackColor = false;
            this.btnCalibSet.Click += new System.EventHandler(this.btnCalibSet_Click);
            // 
            // lblCalibStatus
            // 
            this.lblCalibStatus.AutoSize = true;
            this.lblCalibStatus.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblCalibStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblCalibStatus.Location = new System.Drawing.Point(250, 166);
            this.lblCalibStatus.Name = "lblCalibStatus";
            this.lblCalibStatus.Size = new System.Drawing.Size(0, 21);
            this.lblCalibStatus.TabIndex = 17;
            // 
            // dgvCalib
            // 
            this.dgvCalib.AllowUserToAddRows = false;
            this.dgvCalib.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(28)))), ((int)(((byte)(38)))));
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(60)))), ((int)(((byte)(90)))));
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            dataGridViewCellStyle1.ForeColor = System.Drawing.Color.White;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.dgvCalib.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.dgvCalib.ColumnHeadersHeight = 22;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(28)))), ((int)(((byte)(35)))), ((int)(((byte)(48)))));
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(210)))), ((int)(((byte)(240)))));
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvCalib.DefaultCellStyle = dataGridViewCellStyle2;
            this.dgvCalib.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(50)))), ((int)(((byte)(70)))));
            this.dgvCalib.Location = new System.Drawing.Point(6, 220);
            this.dgvCalib.Name = "dgvCalib";
            this.dgvCalib.ReadOnly = true;
            this.dgvCalib.RowHeadersVisible = false;
            this.dgvCalib.RowHeadersWidth = 62;
            this.dgvCalib.RowTemplate.Height = 20;
            this.dgvCalib.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dgvCalib.Size = new System.Drawing.Size(960, 220);
            this.dgvCalib.TabIndex = 1;
            // 
            // tabProdInfo
            // 
            this.tabProdInfo.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.tabProdInfo.Controls.Add(this.grpProdWrite);
            this.tabProdInfo.Controls.Add(this.grpUtcLotWrite);
            this.tabProdInfo.Controls.Add(this.lblNADCap);
            this.tabProdInfo.Controls.Add(this.lblNADVal);
            this.tabProdInfo.Controls.Add(this.lblSlaveNADCap);
            this.tabProdInfo.Controls.Add(this.lblMasterNADVal);
            this.tabProdInfo.Controls.Add(this.lblSFCap);
            this.tabProdInfo.Controls.Add(this.lblSFVal);
            this.tabProdInfo.Controls.Add(this.lblColorCap);
            this.tabProdInfo.Controls.Add(this.lblColorTblVal);
            this.tabProdInfo.Controls.Add(this.lblUTCCap);
            this.tabProdInfo.Controls.Add(this.lblUTCVal);
            this.tabProdInfo.Controls.Add(this.lblLOTCap);
            this.tabProdInfo.Controls.Add(this.lblLOTVal);
            this.tabProdInfo.Controls.Add(this.lblTrimCap);
            this.tabProdInfo.Controls.Add(this.lblTrimVal);
            this.tabProdInfo.Controls.Add(this.panelFlagBits);
            this.tabProdInfo.Location = new System.Drawing.Point(4, 31);
            this.tabProdInfo.Name = "tabProdInfo";
            this.tabProdInfo.Size = new System.Drawing.Size(986, 525);
            this.tabProdInfo.TabIndex = 3;
            this.tabProdInfo.Text = "  Product Info  ";
            // 
            // grpProdWrite
            // 
            this.grpProdWrite.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(24)))), ((int)(((byte)(38)))));
            this.grpProdWrite.Controls.Add(this.lblWrNADCap);
            this.grpProdWrite.Controls.Add(this.nudWrNAD);
            this.grpProdWrite.Controls.Add(this.lblWrMstNADCap);
            this.grpProdWrite.Controls.Add(this.nudWrMstNAD);
            this.grpProdWrite.Controls.Add(this.lblWrSFCap);
            this.grpProdWrite.Controls.Add(this.nudWrSlaveFlag);
            this.grpProdWrite.Controls.Add(this.lblWrColorTblCap);
            this.grpProdWrite.Controls.Add(this.nudWrColorTbl);
            this.grpProdWrite.Controls.Add(this.btnProdWrite);
            this.grpProdWrite.Controls.Add(this.lblProdWriteStatus);
            this.grpProdWrite.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpProdWrite.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(220)))), ((int)(((byte)(180)))));
            this.grpProdWrite.Location = new System.Drawing.Point(16, 317);
            this.grpProdWrite.Name = "grpProdWrite";
            this.grpProdWrite.Size = new System.Drawing.Size(800, 80);
            this.grpProdWrite.TabIndex = 16;
            this.grpProdWrite.TabStop = false;
            this.grpProdWrite.Text = "[ SET_NAD_INFO  (CMD=0xA0)  –  NAD / MasterNAD / SlaveFlag / ColorTable 쓰기 ]";
            // 
            // lblWrNADCap
            // 
            this.lblWrNADCap.AutoSize = true;
            this.lblWrNADCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblWrNADCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblWrNADCap.Location = new System.Drawing.Point(8, 30);
            this.lblWrNADCap.Name = "lblWrNADCap";
            this.lblWrNADCap.Size = new System.Drawing.Size(50, 22);
            this.lblWrNADCap.TabIndex = 0;
            this.lblWrNADCap.Text = "NAD:";
            // 
            // nudWrNAD
            // 
            this.nudWrNAD.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrNAD.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrNAD.ForeColor = System.Drawing.Color.White;
            this.nudWrNAD.Hexadecimal = true;
            this.nudWrNAD.Location = new System.Drawing.Point(52, 27);
            this.nudWrNAD.Maximum = new decimal(new int[] {
            125,
            0,
            0,
            0});
            this.nudWrNAD.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudWrNAD.Name = "nudWrNAD";
            this.nudWrNAD.Size = new System.Drawing.Size(58, 29);
            this.nudWrNAD.TabIndex = 1;
            this.nudWrNAD.Value = new decimal(new int[] {
            24,
            0,
            0,
            0});
            // 
            // lblWrMstNADCap
            // 
            this.lblWrMstNADCap.AutoSize = true;
            this.lblWrMstNADCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblWrMstNADCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblWrMstNADCap.Location = new System.Drawing.Point(120, 30);
            this.lblWrMstNADCap.Name = "lblWrMstNADCap";
            this.lblWrMstNADCap.Size = new System.Drawing.Size(80, 22);
            this.lblWrMstNADCap.TabIndex = 2;
            this.lblWrMstNADCap.Text = "MstNAD:";
            // 
            // nudWrMstNAD
            // 
            this.nudWrMstNAD.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrMstNAD.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrMstNAD.ForeColor = System.Drawing.Color.White;
            this.nudWrMstNAD.Hexadecimal = true;
            this.nudWrMstNAD.Location = new System.Drawing.Point(186, 27);
            this.nudWrMstNAD.Maximum = new decimal(new int[] {
            127,
            0,
            0,
            0});
            this.nudWrMstNAD.Name = "nudWrMstNAD";
            this.nudWrMstNAD.Size = new System.Drawing.Size(58, 29);
            this.nudWrMstNAD.TabIndex = 3;
            this.nudWrMstNAD.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // lblWrSFCap
            // 
            this.lblWrSFCap.AutoSize = true;
            this.lblWrSFCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblWrSFCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblWrSFCap.Location = new System.Drawing.Point(254, 30);
            this.lblWrSFCap.Name = "lblWrSFCap";
            this.lblWrSFCap.Size = new System.Drawing.Size(110, 22);
            this.lblWrSFCap.TabIndex = 4;
            this.lblWrSFCap.Text = "SlaveFlag:";
            // 
            // nudWrSlaveFlag
            // 
            this.nudWrSlaveFlag.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrSlaveFlag.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrSlaveFlag.ForeColor = System.Drawing.Color.White;
            this.nudWrSlaveFlag.Hexadecimal = true;
            this.nudWrSlaveFlag.Location = new System.Drawing.Point(340, 27);
            this.nudWrSlaveFlag.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudWrSlaveFlag.Name = "nudWrSlaveFlag";
            this.nudWrSlaveFlag.Size = new System.Drawing.Size(58, 29);
            this.nudWrSlaveFlag.TabIndex = 5;
            this.nudWrSlaveFlag.Value = new decimal(new int[] {
            8,
            0,
            0,
            0});
            // 
            // lblWrColorTblCap
            // 
            this.lblWrColorTblCap.AutoSize = true;
            this.lblWrColorTblCap.Font = new System.Drawing.Font("Consolas", 9F);
            this.lblWrColorTblCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));
            this.lblWrColorTblCap.Location = new System.Drawing.Point(408, 30);
            this.lblWrColorTblCap.Name = "lblWrColorTblCap";
            this.lblWrColorTblCap.Size = new System.Drawing.Size(100, 22);
            this.lblWrColorTblCap.TabIndex = 6;
            this.lblWrColorTblCap.Text = "ColorTbl:";
            // 
            // nudWrColorTbl
            // 
            this.nudWrColorTbl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrColorTbl.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrColorTbl.ForeColor = System.Drawing.Color.White;
            this.nudWrColorTbl.Location = new System.Drawing.Point(490, 27);
            this.nudWrColorTbl.Maximum = new decimal(new int[] {
            4,
            0,
            0,
            0});
            this.nudWrColorTbl.Name = "nudWrColorTbl";
            this.nudWrColorTbl.Size = new System.Drawing.Size(50, 29);
            this.nudWrColorTbl.TabIndex = 7;
            this.nudWrColorTbl.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // btnProdWrite
            // 
            this.btnProdWrite.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(130)))), ((int)(((byte)(70)))), ((int)(((byte)(30)))));
            this.btnProdWrite.FlatAppearance.BorderSize = 0;
            this.btnProdWrite.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnProdWrite.Font = new System.Drawing.Font("Consolas", 9.5F, System.Drawing.FontStyle.Bold);
            this.btnProdWrite.ForeColor = System.Drawing.Color.White;
            this.btnProdWrite.Location = new System.Drawing.Point(550, 24);
            this.btnProdWrite.Name = "btnProdWrite";
            this.btnProdWrite.Size = new System.Drawing.Size(120, 32);
            this.btnProdWrite.TabIndex = 8;
            this.btnProdWrite.Text = "▶ WRITE";
            this.btnProdWrite.UseVisualStyleBackColor = false;
            this.btnProdWrite.Click += new System.EventHandler(this.btnProdWrite_Click);
            // 
            // lblProdWriteStatus
            // 
            this.lblProdWriteStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblProdWriteStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(220)))), ((int)(((byte)(180)))));
            this.lblProdWriteStatus.Location = new System.Drawing.Point(680, 30);
            this.lblProdWriteStatus.Name = "lblProdWriteStatus";
            this.lblProdWriteStatus.Size = new System.Drawing.Size(110, 18);
            this.lblProdWriteStatus.TabIndex = 9;
            // 
            // grpUtcLotWrite
            // 
            this.grpUtcLotWrite.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(24)))), ((int)(((byte)(38)))));
            this.grpUtcLotWrite.Controls.Add(this.lblWrUTCCap);
            this.grpUtcLotWrite.Controls.Add(this.lblUtcPreview);
            this.grpUtcLotWrite.Controls.Add(this.btnWrUTC);
            this.grpUtcLotWrite.Controls.Add(this.lblWrLOTCap);
            this.grpUtcLotWrite.Controls.Add(this.nudWrLOT0);
            this.grpUtcLotWrite.Controls.Add(this.nudWrLOT1);
            this.grpUtcLotWrite.Controls.Add(this.nudWrLOT2);
            this.grpUtcLotWrite.Controls.Add(this.nudWrLOT3);
            this.grpUtcLotWrite.Controls.Add(this.btnWrLOT);
            this.grpUtcLotWrite.Controls.Add(this.lblUtcLotStatus);
            this.grpUtcLotWrite.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpUtcLotWrite.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(200)))), ((int)(((byte)(100)))));
            this.grpUtcLotWrite.Location = new System.Drawing.Point(16, 408);
            this.grpUtcLotWrite.Name = "grpUtcLotWrite";
            this.grpUtcLotWrite.Size = new System.Drawing.Size(900, 114);
            this.grpUtcLotWrite.TabIndex = 17;
            this.grpUtcLotWrite.TabStop = false;
            this.grpUtcLotWrite.Text = "[ UTC / LOT 번호 기록  (최초 1회만 – CMD=0xC0/0xC1) ]";
            // 
            // lblWrUTCCap
            // 
            this.lblWrUTCCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblWrUTCCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.lblWrUTCCap.Location = new System.Drawing.Point(6, 14);
            this.lblWrUTCCap.Name = "lblWrUTCCap";
            this.lblWrUTCCap.Size = new System.Drawing.Size(60, 18);
            this.lblWrUTCCap.TabIndex = 0;
            this.lblWrUTCCap.Text = "UTC";
            // 
            // lblUtcPreview
            // 
            this.lblUtcPreview.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblUtcPreview.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblUtcPreview.Location = new System.Drawing.Point(4, 35);
            this.lblUtcPreview.Name = "lblUtcPreview";
            this.lblUtcPreview.Size = new System.Drawing.Size(450, 35);
            this.lblUtcPreview.TabIndex = 1;
            this.lblUtcPreview.Text = "--  (버튼 클릭 시 현재 PC 시각으로 자동 설정)";
            // 
            // btnWrUTC
            // 
            this.btnWrUTC.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(70)))), ((int)(((byte)(20)))));
            this.btnWrUTC.FlatAppearance.BorderSize = 0;
            this.btnWrUTC.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnWrUTC.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnWrUTC.ForeColor = System.Drawing.Color.White;
            this.btnWrUTC.Location = new System.Drawing.Point(6, 76);
            this.btnWrUTC.Name = "btnWrUTC";
            this.btnWrUTC.Size = new System.Drawing.Size(223, 32);
            this.btnWrUTC.TabIndex = 14;
            this.btnWrUTC.Text = "▶ 현재시각으로 UTC 기록";
            this.btnWrUTC.UseVisualStyleBackColor = false;
            this.btnWrUTC.Click += new System.EventHandler(this.btnWrUTC_Click);
            // 
            // lblWrLOTCap
            // 
            this.lblWrLOTCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblWrLOTCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.lblWrLOTCap.Location = new System.Drawing.Point(460, 28);
            this.lblWrLOTCap.Name = "lblWrLOTCap";
            this.lblWrLOTCap.Size = new System.Drawing.Size(90, 18);
            this.lblWrLOTCap.TabIndex = 15;
            this.lblWrLOTCap.Text = "LOT (LE 4B)";
            // 
            // nudWrLOT0
            // 
            this.nudWrLOT0.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrLOT0.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrLOT0.ForeColor = System.Drawing.Color.White;
            this.nudWrLOT0.Location = new System.Drawing.Point(554, 24);
            this.nudWrLOT0.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudWrLOT0.Name = "nudWrLOT0";
            this.nudWrLOT0.Size = new System.Drawing.Size(54, 29);
            this.nudWrLOT0.TabIndex = 15;
            this.nudWrLOT0.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // nudWrLOT1
            // 
            this.nudWrLOT1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrLOT1.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrLOT1.ForeColor = System.Drawing.Color.White;
            this.nudWrLOT1.Location = new System.Drawing.Point(612, 24);
            this.nudWrLOT1.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudWrLOT1.Name = "nudWrLOT1";
            this.nudWrLOT1.Size = new System.Drawing.Size(54, 29);
            this.nudWrLOT1.TabIndex = 16;
            // 
            // nudWrLOT2
            // 
            this.nudWrLOT2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrLOT2.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrLOT2.ForeColor = System.Drawing.Color.White;
            this.nudWrLOT2.Location = new System.Drawing.Point(670, 24);
            this.nudWrLOT2.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudWrLOT2.Name = "nudWrLOT2";
            this.nudWrLOT2.Size = new System.Drawing.Size(54, 29);
            this.nudWrLOT2.TabIndex = 17;
            // 
            // nudWrLOT3
            // 
            this.nudWrLOT3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudWrLOT3.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudWrLOT3.ForeColor = System.Drawing.Color.White;
            this.nudWrLOT3.Location = new System.Drawing.Point(728, 24);
            this.nudWrLOT3.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudWrLOT3.Name = "nudWrLOT3";
            this.nudWrLOT3.Size = new System.Drawing.Size(54, 29);
            this.nudWrLOT3.TabIndex = 18;
            // 
            // btnWrLOT
            // 
            this.btnWrLOT.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(70)))), ((int)(((byte)(20)))));
            this.btnWrLOT.FlatAppearance.BorderSize = 0;
            this.btnWrLOT.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnWrLOT.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnWrLOT.ForeColor = System.Drawing.Color.White;
            this.btnWrLOT.Location = new System.Drawing.Point(464, 76);
            this.btnWrLOT.Name = "btnWrLOT";
            this.btnWrLOT.Size = new System.Drawing.Size(100, 32);
            this.btnWrLOT.TabIndex = 19;
            this.btnWrLOT.Text = "▶ LOT WRITE";
            this.btnWrLOT.UseVisualStyleBackColor = false;
            this.btnWrLOT.Click += new System.EventHandler(this.btnWrLOT_Click);
            // 
            // lblUtcLotStatus
            // 
            this.lblUtcLotStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblUtcLotStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(220)))), ((int)(((byte)(180)))));
            this.lblUtcLotStatus.Location = new System.Drawing.Point(460, 52);
            this.lblUtcLotStatus.Name = "lblUtcLotStatus";
            this.lblUtcLotStatus.Size = new System.Drawing.Size(430, 18);
            this.lblUtcLotStatus.TabIndex = 20;
            // 
            // lblNADCap
            // 
            this.lblNADCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblNADCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(130)))), ((int)(((byte)(160)))));
            this.lblNADCap.Location = new System.Drawing.Point(12, 20);
            this.lblNADCap.Name = "lblNADCap";
            this.lblNADCap.Size = new System.Drawing.Size(120, 18);
            this.lblNADCap.TabIndex = 0;
            this.lblNADCap.Text = "NAD";
            // 
            // lblNADVal
            // 
            this.lblNADVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblNADVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblNADVal.Location = new System.Drawing.Point(140, 20);
            this.lblNADVal.Name = "lblNADVal";
            this.lblNADVal.Size = new System.Drawing.Size(500, 18);
            this.lblNADVal.TabIndex = 1;
            this.lblNADVal.Text = "--";
            // 
            // lblSlaveNADCap
            // 
            this.lblSlaveNADCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblSlaveNADCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(130)))), ((int)(((byte)(160)))));
            this.lblSlaveNADCap.Location = new System.Drawing.Point(12, 56);
            this.lblSlaveNADCap.Name = "lblSlaveNADCap";
            this.lblSlaveNADCap.Size = new System.Drawing.Size(120, 18);
            this.lblSlaveNADCap.TabIndex = 2;
            this.lblSlaveNADCap.Text = "Master NAD";
            // 
            // lblMasterNADVal
            // 
            this.lblMasterNADVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblMasterNADVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblMasterNADVal.Location = new System.Drawing.Point(140, 56);
            this.lblMasterNADVal.Name = "lblMasterNADVal";
            this.lblMasterNADVal.Size = new System.Drawing.Size(500, 18);
            this.lblMasterNADVal.TabIndex = 3;
            this.lblMasterNADVal.Text = "--";
            // 
            // lblSFCap
            // 
            this.lblSFCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblSFCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(130)))), ((int)(((byte)(160)))));
            this.lblSFCap.Location = new System.Drawing.Point(12, 92);
            this.lblSFCap.Name = "lblSFCap";
            this.lblSFCap.Size = new System.Drawing.Size(120, 18);
            this.lblSFCap.TabIndex = 4;
            this.lblSFCap.Text = "Slave Flag";
            // 
            // lblSFVal
            // 
            this.lblSFVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblSFVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblSFVal.Location = new System.Drawing.Point(140, 92);
            this.lblSFVal.Name = "lblSFVal";
            this.lblSFVal.Size = new System.Drawing.Size(500, 18);
            this.lblSFVal.TabIndex = 5;
            this.lblSFVal.Text = "--";
            // 
            // lblColorCap
            // 
            this.lblColorCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblColorCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(130)))), ((int)(((byte)(160)))));
            this.lblColorCap.Location = new System.Drawing.Point(12, 128);
            this.lblColorCap.Name = "lblColorCap";
            this.lblColorCap.Size = new System.Drawing.Size(120, 18);
            this.lblColorCap.TabIndex = 6;
            this.lblColorCap.Text = "Color Table";
            // 
            // lblColorTblVal
            // 
            this.lblColorTblVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblColorTblVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblColorTblVal.Location = new System.Drawing.Point(140, 128);
            this.lblColorTblVal.Name = "lblColorTblVal";
            this.lblColorTblVal.Size = new System.Drawing.Size(500, 18);
            this.lblColorTblVal.TabIndex = 7;
            this.lblColorTblVal.Text = "--";
            // 
            // lblUTCCap
            // 
            this.lblUTCCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblUTCCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(130)))), ((int)(((byte)(160)))));
            this.lblUTCCap.Location = new System.Drawing.Point(12, 164);
            this.lblUTCCap.Name = "lblUTCCap";
            this.lblUTCCap.Size = new System.Drawing.Size(120, 18);
            this.lblUTCCap.TabIndex = 8;
            this.lblUTCCap.Text = "UTC";
            // 
            // lblUTCVal
            // 
            this.lblUTCVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblUTCVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblUTCVal.Location = new System.Drawing.Point(140, 164);
            this.lblUTCVal.Name = "lblUTCVal";
            this.lblUTCVal.Size = new System.Drawing.Size(500, 18);
            this.lblUTCVal.TabIndex = 9;
            this.lblUTCVal.Text = "--";
            // 
            // lblLOTCap
            // 
            this.lblLOTCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblLOTCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(130)))), ((int)(((byte)(160)))));
            this.lblLOTCap.Location = new System.Drawing.Point(12, 200);
            this.lblLOTCap.Name = "lblLOTCap";
            this.lblLOTCap.Size = new System.Drawing.Size(120, 18);
            this.lblLOTCap.TabIndex = 10;
            this.lblLOTCap.Text = "LOT";
            // 
            // lblLOTVal
            // 
            this.lblLOTVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblLOTVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblLOTVal.Location = new System.Drawing.Point(140, 200);
            this.lblLOTVal.Name = "lblLOTVal";
            this.lblLOTVal.Size = new System.Drawing.Size(500, 18);
            this.lblLOTVal.TabIndex = 11;
            this.lblLOTVal.Text = "--";
            // 
            // lblTrimCap
            // 
            this.lblTrimCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblTrimCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(130)))), ((int)(((byte)(160)))));
            this.lblTrimCap.Location = new System.Drawing.Point(12, 236);
            this.lblTrimCap.Name = "lblTrimCap";
            this.lblTrimCap.Size = new System.Drawing.Size(120, 18);
            this.lblTrimCap.TabIndex = 12;
            this.lblTrimCap.Text = "Trim";
            // 
            // lblTrimVal
            // 
            this.lblTrimVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblTrimVal.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(210)))), ((int)(((byte)(230)))), ((int)(((byte)(255)))));
            this.lblTrimVal.Location = new System.Drawing.Point(140, 236);
            this.lblTrimVal.Name = "lblTrimVal";
            this.lblTrimVal.Size = new System.Drawing.Size(500, 18);
            this.lblTrimVal.TabIndex = 13;
            this.lblTrimVal.Text = "--";
            // 
            // panelFlagBits
            // 
            this.panelFlagBits.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(18)))), ((int)(((byte)(24)))), ((int)(((byte)(36)))));
            this.panelFlagBits.Location = new System.Drawing.Point(12, 272);
            this.panelFlagBits.Name = "panelFlagBits";
            this.panelFlagBits.Size = new System.Drawing.Size(500, 26);
            this.panelFlagBits.TabIndex = 14;
            this.panelFlagBits.Paint += new System.Windows.Forms.PaintEventHandler(this.panelFlagBits_Paint);
            // 
            // tabPwmDiag
            // 
            this.tabPwmDiag.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.tabPwmDiag.Controls.Add(this.grpPwmCtrl);
            this.tabPwmDiag.Controls.Add(this.grpPwmMax);
            this.tabPwmDiag.Controls.Add(this.grpDiagRead);
            this.tabPwmDiag.Controls.Add(this.lblPwmDiagStatus);
            this.tabPwmDiag.Location = new System.Drawing.Point(4, 31);
            this.tabPwmDiag.Name = "tabPwmDiag";
            this.tabPwmDiag.Size = new System.Drawing.Size(986, 525);
            this.tabPwmDiag.TabIndex = 4;
            this.tabPwmDiag.Text = "  PWM / Diag  ";
            // 
            // tabParamTuning
            // 
            this.tabParamTuning.BackColor = System.Drawing.Color.FromArgb(22, 28, 42);
            this.tabParamTuning.Controls.Add(this.btnOpenParamTuning);
            this.tabParamTuning.Location = new System.Drawing.Point(4, 31);
            this.tabParamTuning.Name = "tabParamTuning";
            this.tabParamTuning.Size = new System.Drawing.Size(986, 525);
            this.tabParamTuning.TabIndex = 5;
            this.tabParamTuning.Text = "  Param Tuning  ";
            // 
            // btnOpenParamTuning
            // 
            this.btnOpenParamTuning.Location = new System.Drawing.Point(340, 200);
            this.btnOpenParamTuning.Size = new System.Drawing.Size(300, 60);
            this.btnOpenParamTuning.Text = "📂 Parameter Tuning 창 열기\r\n(MAP 파일 업로드 후 변수 Read/Write)";
            this.btnOpenParamTuning.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            this.btnOpenParamTuning.BackColor = System.Drawing.Color.FromArgb(61, 133, 198);
            this.btnOpenParamTuning.ForeColor = System.Drawing.Color.White;
            this.btnOpenParamTuning.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnOpenParamTuning.Click += new System.EventHandler(this.btnOpenParamTuning_Click);
            // 
            // grpPwmCtrl
            // 
            this.grpPwmCtrl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(24)))), ((int)(((byte)(38)))));
            this.grpPwmCtrl.Controls.Add(this.lblPwmPort);
            this.grpPwmCtrl.Controls.Add(this.nudPwmPort);
            this.grpPwmCtrl.Controls.Add(this.lblPwmDuty);
            this.grpPwmCtrl.Controls.Add(this.nudPwmDuty);
            this.grpPwmCtrl.Controls.Add(this.btnSetPwm);
            this.grpPwmCtrl.Controls.Add(this.btnGetPwm);
            this.grpPwmCtrl.Controls.Add(this.lblPwmValCap);
            this.grpPwmCtrl.Controls.Add(this.lblPwmVal);
            this.grpPwmCtrl.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpPwmCtrl.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpPwmCtrl.Location = new System.Drawing.Point(16, 16);
            this.grpPwmCtrl.Name = "grpPwmCtrl";
            this.grpPwmCtrl.Size = new System.Drawing.Size(640, 90);
            this.grpPwmCtrl.TabIndex = 0;
            this.grpPwmCtrl.TabStop = false;
            this.grpPwmCtrl.Text = "[ PWM 수동 제어  (CMD=0xAA SET_PWM / CMD=0x50 GET_PWM) ]";
            // 
            // lblPwmPort
            // 
            this.lblPwmPort.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblPwmPort.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.lblPwmPort.Location = new System.Drawing.Point(10, 30);
            this.lblPwmPort.Name = "lblPwmPort";
            this.lblPwmPort.Size = new System.Drawing.Size(70, 18);
            this.lblPwmPort.TabIndex = 0;
            this.lblPwmPort.Text = "Port (0~7)";
            // 
            // nudPwmPort
            // 
            this.nudPwmPort.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudPwmPort.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudPwmPort.ForeColor = System.Drawing.Color.White;
            this.nudPwmPort.Location = new System.Drawing.Point(84, 26);
            this.nudPwmPort.Maximum = new decimal(new int[] {
            7,
            0,
            0,
            0});
            this.nudPwmPort.Name = "nudPwmPort";
            this.nudPwmPort.Size = new System.Drawing.Size(50, 29);
            this.nudPwmPort.TabIndex = 0;
            // 
            // lblPwmDuty
            // 
            this.lblPwmDuty.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblPwmDuty.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.lblPwmDuty.Location = new System.Drawing.Point(148, 30);
            this.lblPwmDuty.Name = "lblPwmDuty";
            this.lblPwmDuty.Size = new System.Drawing.Size(80, 18);
            this.lblPwmDuty.TabIndex = 1;
            this.lblPwmDuty.Text = "Duty (0~255)";
            // 
            // nudPwmDuty
            // 
            this.nudPwmDuty.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudPwmDuty.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudPwmDuty.ForeColor = System.Drawing.Color.White;
            this.nudPwmDuty.Location = new System.Drawing.Point(232, 26);
            this.nudPwmDuty.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudPwmDuty.Name = "nudPwmDuty";
            this.nudPwmDuty.Size = new System.Drawing.Size(60, 29);
            this.nudPwmDuty.TabIndex = 1;
            this.nudPwmDuty.Value = new decimal(new int[] {
            128,
            0,
            0,
            0});
            // 
            // btnSetPwm
            // 
            this.btnSetPwm.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(80)))), ((int)(((byte)(130)))));
            this.btnSetPwm.FlatAppearance.BorderSize = 0;
            this.btnSetPwm.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSetPwm.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnSetPwm.ForeColor = System.Drawing.Color.White;
            this.btnSetPwm.Location = new System.Drawing.Point(302, 24);
            this.btnSetPwm.Name = "btnSetPwm";
            this.btnSetPwm.Size = new System.Drawing.Size(110, 32);
            this.btnSetPwm.TabIndex = 2;
            this.btnSetPwm.Text = "▶ SET PWM";
            this.btnSetPwm.UseVisualStyleBackColor = false;
            this.btnSetPwm.Click += new System.EventHandler(this.btnSetPwm_Click);
            // 
            // btnGetPwm
            // 
            this.btnGetPwm.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(80)))), ((int)(((byte)(60)))));
            this.btnGetPwm.FlatAppearance.BorderSize = 0;
            this.btnGetPwm.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGetPwm.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnGetPwm.ForeColor = System.Drawing.Color.White;
            this.btnGetPwm.Location = new System.Drawing.Point(420, 24);
            this.btnGetPwm.Name = "btnGetPwm";
            this.btnGetPwm.Size = new System.Drawing.Size(110, 32);
            this.btnGetPwm.TabIndex = 3;
            this.btnGetPwm.Text = "▶ GET PWM";
            this.btnGetPwm.UseVisualStyleBackColor = false;
            this.btnGetPwm.Click += new System.EventHandler(this.btnGetPwm_Click);
            // 
            // lblPwmValCap
            // 
            this.lblPwmValCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblPwmValCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(160)))), ((int)(((byte)(120)))));
            this.lblPwmValCap.Location = new System.Drawing.Point(540, 30);
            this.lblPwmValCap.Name = "lblPwmValCap";
            this.lblPwmValCap.Size = new System.Drawing.Size(40, 18);
            this.lblPwmValCap.TabIndex = 4;
            this.lblPwmValCap.Text = "→ 값:";
            // 
            // lblPwmVal
            // 
            this.lblPwmVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblPwmVal.ForeColor = System.Drawing.Color.LimeGreen;
            this.lblPwmVal.Location = new System.Drawing.Point(540, 50);
            this.lblPwmVal.Name = "lblPwmVal";
            this.lblPwmVal.Size = new System.Drawing.Size(90, 18);
            this.lblPwmVal.TabIndex = 5;
            this.lblPwmVal.Text = "--";
            // 
            // grpPwmMax
            // 
            this.grpPwmMax.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(24)))), ((int)(((byte)(38)))));
            this.grpPwmMax.Controls.Add(this.lblPwmMaxMask);
            this.grpPwmMax.Controls.Add(this.nudPwmMaxMask0);
            this.grpPwmMax.Controls.Add(this.nudPwmMaxMask1);
            this.grpPwmMax.Controls.Add(this.nudPwmMaxMask2);
            this.grpPwmMax.Controls.Add(this.btnSetPwmMax);
            this.grpPwmMax.Controls.Add(this.lblPwmMaxStatus);
            this.grpPwmMax.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpPwmMax.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(100)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.grpPwmMax.Location = new System.Drawing.Point(16, 120);
            this.grpPwmMax.Name = "grpPwmMax";
            this.grpPwmMax.Size = new System.Drawing.Size(640, 80);
            this.grpPwmMax.TabIndex = 1;
            this.grpPwmMax.TabStop = false;
            this.grpPwmMax.Text = "[ 포트별 최대 PWM 설정  (CMD=0x51 SET_PWM_MAX  –  비트마스크 3바이트) ]";
            // 
            // lblPwmMaxMask
            // 
            this.lblPwmMaxMask.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblPwmMaxMask.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(200)))), ((int)(((byte)(255)))));
            this.lblPwmMaxMask.Location = new System.Drawing.Point(10, 32);
            this.lblPwmMaxMask.Name = "lblPwmMaxMask";
            this.lblPwmMaxMask.Size = new System.Drawing.Size(120, 18);
            this.lblPwmMaxMask.TabIndex = 0;
            this.lblPwmMaxMask.Text = "Mask[0] [1] [2]";
            // 
            // nudPwmMaxMask0
            // 
            this.nudPwmMaxMask0.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudPwmMaxMask0.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudPwmMaxMask0.ForeColor = System.Drawing.Color.White;
            this.nudPwmMaxMask0.Location = new System.Drawing.Point(134, 28);
            this.nudPwmMaxMask0.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudPwmMaxMask0.Name = "nudPwmMaxMask0";
            this.nudPwmMaxMask0.Size = new System.Drawing.Size(60, 29);
            this.nudPwmMaxMask0.TabIndex = 0;
            this.nudPwmMaxMask0.Value = new decimal(new int[] {
            255,
            0,
            0,
            0});
            // 
            // nudPwmMaxMask1
            // 
            this.nudPwmMaxMask1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudPwmMaxMask1.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudPwmMaxMask1.ForeColor = System.Drawing.Color.White;
            this.nudPwmMaxMask1.Location = new System.Drawing.Point(198, 28);
            this.nudPwmMaxMask1.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudPwmMaxMask1.Name = "nudPwmMaxMask1";
            this.nudPwmMaxMask1.Size = new System.Drawing.Size(60, 29);
            this.nudPwmMaxMask1.TabIndex = 1;
            this.nudPwmMaxMask1.Value = new decimal(new int[] {
            255,
            0,
            0,
            0});
            // 
            // nudPwmMaxMask2
            // 
            this.nudPwmMaxMask2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudPwmMaxMask2.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudPwmMaxMask2.ForeColor = System.Drawing.Color.White;
            this.nudPwmMaxMask2.Location = new System.Drawing.Point(262, 28);
            this.nudPwmMaxMask2.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.nudPwmMaxMask2.Name = "nudPwmMaxMask2";
            this.nudPwmMaxMask2.Size = new System.Drawing.Size(60, 29);
            this.nudPwmMaxMask2.TabIndex = 2;
            this.nudPwmMaxMask2.Value = new decimal(new int[] {
            255,
            0,
            0,
            0});
            // 
            // btnSetPwmMax
            // 
            this.btnSetPwmMax.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(40)))), ((int)(((byte)(80)))), ((int)(((byte)(130)))));
            this.btnSetPwmMax.FlatAppearance.BorderSize = 0;
            this.btnSetPwmMax.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSetPwmMax.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnSetPwmMax.ForeColor = System.Drawing.Color.White;
            this.btnSetPwmMax.Location = new System.Drawing.Point(332, 26);
            this.btnSetPwmMax.Name = "btnSetPwmMax";
            this.btnSetPwmMax.Size = new System.Drawing.Size(130, 32);
            this.btnSetPwmMax.TabIndex = 3;
            this.btnSetPwmMax.Text = "▶ SET PWM MAX";
            this.btnSetPwmMax.UseVisualStyleBackColor = false;
            this.btnSetPwmMax.Click += new System.EventHandler(this.btnSetPwmMax_Click);
            // 
            // lblPwmMaxStatus
            // 
            this.lblPwmMaxStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblPwmMaxStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(220)))), ((int)(((byte)(180)))));
            this.lblPwmMaxStatus.Location = new System.Drawing.Point(474, 32);
            this.lblPwmMaxStatus.Name = "lblPwmMaxStatus";
            this.lblPwmMaxStatus.Size = new System.Drawing.Size(160, 18);
            this.lblPwmMaxStatus.TabIndex = 4;
            // 
            // grpDiagRead
            // 
            this.grpDiagRead.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(24)))), ((int)(((byte)(38)))));
            this.grpDiagRead.Controls.Add(this.btnGetPnVolt);
            this.grpDiagRead.Controls.Add(this.lblPnVoltCap);
            this.grpDiagRead.Controls.Add(this.lblPnVoltVal);
            this.grpDiagRead.Controls.Add(this.nudPnVoltOpt);
            this.grpDiagRead.Controls.Add(this.btnGetCurrent);
            this.grpDiagRead.Controls.Add(this.lblCurrentCap);
            this.grpDiagRead.Controls.Add(this.lblCurrentVal);
            this.grpDiagRead.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.grpDiagRead.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(100)))));
            this.grpDiagRead.Location = new System.Drawing.Point(16, 216);
            this.grpDiagRead.Name = "grpDiagRead";
            this.grpDiagRead.Size = new System.Drawing.Size(640, 110);
            this.grpDiagRead.TabIndex = 2;
            this.grpDiagRead.TabStop = false;
            this.grpDiagRead.Text = "[ 진단 읽기  (CMD=0xA7 GET_LED_PN_VOLT / CMD=0xA9 GET_CURRENT) ]";
            // 
            // btnGetPnVolt
            // 
            this.btnGetPnVolt.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(80)))), ((int)(((byte)(40)))));
            this.btnGetPnVolt.FlatAppearance.BorderSize = 0;
            this.btnGetPnVolt.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGetPnVolt.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnGetPnVolt.ForeColor = System.Drawing.Color.White;
            this.btnGetPnVolt.Location = new System.Drawing.Point(10, 24);
            this.btnGetPnVolt.Name = "btnGetPnVolt";
            this.btnGetPnVolt.Size = new System.Drawing.Size(130, 32);
            this.btnGetPnVolt.TabIndex = 0;
            this.btnGetPnVolt.Text = "▶ GET PN VOLT";
            this.btnGetPnVolt.UseVisualStyleBackColor = false;
            this.btnGetPnVolt.Click += new System.EventHandler(this.btnGetPnVolt_Click);
            // 
            // lblPnVoltCap
            // 
            this.lblPnVoltCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblPnVoltCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(160)))), ((int)(((byte)(120)))));
            this.lblPnVoltCap.Location = new System.Drawing.Point(10, 62);
            this.lblPnVoltCap.Name = "lblPnVoltCap";
            this.lblPnVoltCap.Size = new System.Drawing.Size(130, 16);
            this.lblPnVoltCap.TabIndex = 1;
            this.lblPnVoltCap.Text = "PN Option(0~3):";
            // 
            // lblPnVoltVal
            // 
            this.lblPnVoltVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblPnVoltVal.ForeColor = System.Drawing.Color.LimeGreen;
            this.lblPnVoltVal.Location = new System.Drawing.Point(210, 30);
            this.lblPnVoltVal.Name = "lblPnVoltVal";
            this.lblPnVoltVal.Size = new System.Drawing.Size(420, 18);
            this.lblPnVoltVal.TabIndex = 2;
            this.lblPnVoltVal.Text = "--";
            // 
            // nudPnVoltOpt
            // 
            this.nudPnVoltOpt.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(36)))), ((int)(((byte)(42)))), ((int)(((byte)(56)))));
            this.nudPnVoltOpt.Font = new System.Drawing.Font("Consolas", 9F);
            this.nudPnVoltOpt.ForeColor = System.Drawing.Color.White;
            this.nudPnVoltOpt.Location = new System.Drawing.Point(148, 58);
            this.nudPnVoltOpt.Maximum = new decimal(new int[] {
            3,
            0,
            0,
            0});
            this.nudPnVoltOpt.Name = "nudPnVoltOpt";
            this.nudPnVoltOpt.Size = new System.Drawing.Size(50, 29);
            this.nudPnVoltOpt.TabIndex = 1;
            // 
            // btnGetCurrent
            // 
            this.btnGetCurrent.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(80)))), ((int)(((byte)(40)))));
            this.btnGetCurrent.FlatAppearance.BorderSize = 0;
            this.btnGetCurrent.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGetCurrent.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.btnGetCurrent.ForeColor = System.Drawing.Color.White;
            this.btnGetCurrent.Location = new System.Drawing.Point(10, 66);
            this.btnGetCurrent.Name = "btnGetCurrent";
            this.btnGetCurrent.Size = new System.Drawing.Size(130, 32);
            this.btnGetCurrent.TabIndex = 2;
            this.btnGetCurrent.Text = "▶ GET CURRENT";
            this.btnGetCurrent.UseVisualStyleBackColor = false;
            this.btnGetCurrent.Click += new System.EventHandler(this.btnGetCurrent_Click);
            // 
            // lblCurrentCap
            // 
            this.lblCurrentCap.Font = new System.Drawing.Font("Segoe UI", 8F);
            this.lblCurrentCap.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(120)))), ((int)(((byte)(160)))), ((int)(((byte)(120)))));
            this.lblCurrentCap.Location = new System.Drawing.Point(148, 75);
            this.lblCurrentCap.Name = "lblCurrentCap";
            this.lblCurrentCap.Size = new System.Drawing.Size(60, 16);
            this.lblCurrentCap.TabIndex = 3;
            this.lblCurrentCap.Text = "→ 값:";
            // 
            // lblCurrentVal
            // 
            this.lblCurrentVal.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            this.lblCurrentVal.ForeColor = System.Drawing.Color.LimeGreen;
            this.lblCurrentVal.Location = new System.Drawing.Point(210, 75);
            this.lblCurrentVal.Name = "lblCurrentVal";
            this.lblCurrentVal.Size = new System.Drawing.Size(420, 18);
            this.lblCurrentVal.TabIndex = 4;
            this.lblCurrentVal.Text = "--";
            // 
            // lblPwmDiagStatus
            // 
            this.lblPwmDiagStatus.Font = new System.Drawing.Font("Consolas", 8.5F);
            this.lblPwmDiagStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(220)))), ((int)(((byte)(180)))));
            this.lblPwmDiagStatus.Location = new System.Drawing.Point(16, 340);
            this.lblPwmDiagStatus.Name = "lblPwmDiagStatus";
            this.lblPwmDiagStatus.Size = new System.Drawing.Size(600, 18);
            this.lblPwmDiagStatus.TabIndex = 3;
            // 
            // FormNodeScan
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 22F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(28)))), ((int)(((byte)(42)))));
            this.ClientSize = new System.Drawing.Size(1164, 646);
            this.Controls.Add(this.tabMain);
            this.Controls.Add(this.pnlLeft);
            this.Controls.Add(this.pnlInfo);
            this.Font = new System.Drawing.Font("Consolas", 9F);
            this.MinimumSize = new System.Drawing.Size(900, 560);
            this.Name = "FormNodeScan";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "MLM Node Scan  |  OEM Diagnostic Tool";
            this.pnlInfo.ResumeLayout(false);
            this.pnlLeft.ResumeLayout(false);
            this.tabMain.ResumeLayout(false);
            this.tabLedCtrl.ResumeLayout(false);
            this.grpLedTx.ResumeLayout(false);
            this.grpLedColor.ResumeLayout(false);
            this.grpLedColor.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudColorY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudBright)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeIn)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudFadeOut)).EndInit();
            this.grpEfficiency.ResumeLayout(false);
            this.grpEfficiency.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudUsedLed)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudOverlap)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff0)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudMaxInt)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudOlActive)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff3)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCurSrc)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff4)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff5)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff6)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudEff7)).EndInit();
            this.grpSF.ResumeLayout(false);
            this.grpSF.PerformLayout();
            this.tabCalib.ResumeLayout(false);
            this.grpCalib.ResumeLayout(false);
            this.grpCalib.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibLedSel)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibRX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibRY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibRZ)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibGX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibGY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibGZ)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibBX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibBY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudCalibBZ)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvCalib)).EndInit();
            this.tabProdInfo.ResumeLayout(false);
            this.grpProdWrite.ResumeLayout(false);
            this.grpProdWrite.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrNAD)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrMstNAD)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrSlaveFlag)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrColorTbl)).EndInit();
            this.grpUtcLotWrite.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT0)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudWrLOT3)).EndInit();
            this.tabPwmDiag.ResumeLayout(false);
            this.tabParamTuning.ResumeLayout(false);
            this.grpPwmCtrl.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmPort)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmDuty)).EndInit();
            this.grpPwmMax.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmMaxMask0)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmMaxMask1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudPwmMaxMask2)).EndInit();
            this.grpDiagRead.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nudPnVoltOpt)).EndInit();
            this.ResumeLayout(false);

        }
        #endregion

        // ── 기존 컨트롤 ──────────────────────────────────────────────────────
        private System.Windows.Forms.Panel pnlInfo;
        private System.Windows.Forms.Label lblTypCap;
        private System.Windows.Forms.Label lblTypVal;
        private System.Windows.Forms.Label lblProjCap;
        private System.Windows.Forms.Label lblProjVal;
        private System.Windows.Forms.Label lblSdkCap;
        private System.Windows.Forms.Label lblSdkVal;
        private System.Windows.Forms.Label lblRelCap;
        private System.Windows.Forms.Label lblReleaseVal;
        private System.Windows.Forms.Label lblMLMCap;
        private System.Windows.Forms.Label lblMLMVal;
        private System.Windows.Forms.Label lblMLSCap;
        private System.Windows.Forms.Label lblMLSVal;
        private System.Windows.Forms.Label lblFlagCap;
        private System.Windows.Forms.Label lblFlagVal;
        private System.Windows.Forms.Label lblSerialCap;
        private System.Windows.Forms.Label lblSerialVal;
        private System.Windows.Forms.Label lblTempCap;
        private System.Windows.Forms.Label lblTempVal;
        private System.Windows.Forms.Label lblVoltCap;
        private System.Windows.Forms.Label lblVoltVal;
        private System.Windows.Forms.Label lblDtcOpen;
        private System.Windows.Forms.Label lblDtcClose;
        private System.Windows.Forms.Label lblDtcCrit;
        private System.Windows.Forms.Label lblDtcOk;
        private System.Windows.Forms.Panel pnlLeft;
        private System.Windows.Forms.Label lblSlaveNodes;
        private System.Windows.Forms.ListBox lstSlaves;
        private System.Windows.Forms.Button btnScan;
        private System.Windows.Forms.Button btnAddNode;
        private System.Windows.Forms.Label lblScanStatus;
        private System.Windows.Forms.TabControl tabMain;
        private System.Windows.Forms.TabPage tabLedCtrl;
        private System.Windows.Forms.GroupBox grpSF;
        private System.Windows.Forms.CheckBox chkSF1;
        private System.Windows.Forms.CheckBox chkSF2;
        private System.Windows.Forms.CheckBox chkSF3;
        private System.Windows.Forms.CheckBox chkSF4;
        private System.Windows.Forms.CheckBox chkSF5;
        private System.Windows.Forms.CheckBox chkSF6;
        private System.Windows.Forms.CheckBox chkSF7;
        private System.Windows.Forms.CheckBox chkSF8;
        private System.Windows.Forms.GroupBox grpLedColor;
        private System.Windows.Forms.Label lblLedCX;
        private System.Windows.Forms.NumericUpDown nudColorX;
        private System.Windows.Forms.Label lblLedCY;
        private System.Windows.Forms.NumericUpDown nudColorY;
        private System.Windows.Forms.Label lblLedBr;
        private System.Windows.Forms.NumericUpDown nudBright;
        private System.Windows.Forms.Label lblLedFI;
        private System.Windows.Forms.NumericUpDown nudFadeIn;
        private System.Windows.Forms.Label lblLedFO;
        private System.Windows.Forms.NumericUpDown nudFadeOut;
        //private System.Windows.Forms.Panel pnlColorPreview;
        private System.Windows.Forms.GroupBox grpLedTx;
        private System.Windows.Forms.Button btnLedSend;
        private System.Windows.Forms.Button btnLedStart;
        private System.Windows.Forms.Label lblFramePreview;
        private System.Windows.Forms.GroupBox grpEfficiency;
        private System.Windows.Forms.Label lblEff0;
        private System.Windows.Forms.NumericUpDown nudEff0;
        private System.Windows.Forms.Label lblEff1;
        private System.Windows.Forms.NumericUpDown nudEff1;
        private System.Windows.Forms.Label lblEff2;
        private System.Windows.Forms.NumericUpDown nudEff2;
        private System.Windows.Forms.Label lblEff3;
        private System.Windows.Forms.NumericUpDown nudEff3;
        private System.Windows.Forms.Label lblEff4;
        private System.Windows.Forms.NumericUpDown nudEff4;
        private System.Windows.Forms.Label lblEff5;
        private System.Windows.Forms.NumericUpDown nudEff5;
        private System.Windows.Forms.Label lblEff6;
        private System.Windows.Forms.NumericUpDown nudEff6;
        private System.Windows.Forms.Label lblEff7;
        private System.Windows.Forms.NumericUpDown nudEff7;
        private System.Windows.Forms.Button btnDevGet;
        private System.Windows.Forms.Button btnDevSet;
        private System.Windows.Forms.Label lblDevStatus;
        private System.Windows.Forms.Label lblUsedLed;
        private System.Windows.Forms.NumericUpDown nudUsedLed;
        private System.Windows.Forms.Label lblOverlap;
        private System.Windows.Forms.NumericUpDown nudOverlap;
        private System.Windows.Forms.Label lblOlActive;
        private System.Windows.Forms.NumericUpDown nudOlActive;
        private System.Windows.Forms.Label lblCurSrc;
        private System.Windows.Forms.NumericUpDown nudCurSrc;
        private System.Windows.Forms.Label lblMaxInt;
        private System.Windows.Forms.NumericUpDown nudMaxInt;
        private System.Windows.Forms.TabPage tabCalib;
        private System.Windows.Forms.GroupBox grpCalib;
        private System.Windows.Forms.Label lblCalibLed;
        private System.Windows.Forms.NumericUpDown nudCalibLedSel;
        private System.Windows.Forms.Label lblCalibBrightR;
        private System.Windows.Forms.Label lblCalibBrightG;
        private System.Windows.Forms.Label lblCalibBrightB;
        private System.Windows.Forms.Label lblCalibRX;
        private System.Windows.Forms.NumericUpDown nudCalibRX;
        private System.Windows.Forms.Label lblCalibRY;
        private System.Windows.Forms.NumericUpDown nudCalibRY;
        private System.Windows.Forms.Label lblCalibRZ;
        private System.Windows.Forms.NumericUpDown nudCalibRZ;
        private System.Windows.Forms.Label lblCalibGX;
        private System.Windows.Forms.NumericUpDown nudCalibGX;
        private System.Windows.Forms.Label lblCalibGY;
        private System.Windows.Forms.NumericUpDown nudCalibGY;
        private System.Windows.Forms.Label lblCalibGZ;
        private System.Windows.Forms.NumericUpDown nudCalibGZ;
        private System.Windows.Forms.Label lblCalibBX;
        private System.Windows.Forms.NumericUpDown nudCalibBX;
        private System.Windows.Forms.Label lblCalibBY;
        private System.Windows.Forms.NumericUpDown nudCalibBY;
        private System.Windows.Forms.Label lblCalibBZ;
        private System.Windows.Forms.NumericUpDown nudCalibBZ;
        private System.Windows.Forms.Button btnCalibGet;
        private System.Windows.Forms.Button btnCalibGetAll;
        private System.Windows.Forms.Button btnCalibSet;
        private System.Windows.Forms.Label lblCalibStatus;
        private System.Windows.Forms.DataGridView dgvCalib;
        private System.Windows.Forms.TabPage tabProdInfo;
        private System.Windows.Forms.Label lblNADCap;
        private System.Windows.Forms.Label lblNADVal;
        private System.Windows.Forms.Label lblSlaveNADCap;
        private System.Windows.Forms.Label lblMasterNADVal;
        private System.Windows.Forms.Label lblSFCap;
        private System.Windows.Forms.Label lblSFVal;
        private System.Windows.Forms.Panel panelFlagBits;
        private System.Windows.Forms.Label lblColorCap;
        private System.Windows.Forms.Label lblColorTblVal;
        private System.Windows.Forms.Label lblUTCCap;
        private System.Windows.Forms.Label lblUTCVal;
        private System.Windows.Forms.Label lblLOTCap;
        private System.Windows.Forms.Label lblLOTVal;
        private System.Windows.Forms.Label lblTrimCap;
        private System.Windows.Forms.Label lblTrimVal;
        private System.Windows.Forms.GroupBox grpProdWrite;
        private System.Windows.Forms.Label lblWrNADCap;
        private System.Windows.Forms.NumericUpDown nudWrNAD;
        private System.Windows.Forms.Label lblWrMstNADCap;
        private System.Windows.Forms.NumericUpDown nudWrMstNAD;
        private System.Windows.Forms.Label lblWrSFCap;
        private System.Windows.Forms.NumericUpDown nudWrSlaveFlag;
        private System.Windows.Forms.Label lblWrColorTblCap;
        private System.Windows.Forms.NumericUpDown nudWrColorTbl;
        private System.Windows.Forms.Button btnProdWrite;
        private System.Windows.Forms.Label lblProdWriteStatus;
        // UTC / LOT Write
        private System.Windows.Forms.GroupBox grpUtcLotWrite;
        private System.Windows.Forms.Label lblWrUTCCap;
        private System.Windows.Forms.Label lblUtcPreview;
        private System.Windows.Forms.Button btnWrUTC;
        private System.Windows.Forms.Label lblWrLOTCap;
        private System.Windows.Forms.NumericUpDown nudWrLOT0;
        private System.Windows.Forms.NumericUpDown nudWrLOT1;
        private System.Windows.Forms.NumericUpDown nudWrLOT2;
        private System.Windows.Forms.NumericUpDown nudWrLOT3;
        private System.Windows.Forms.Button btnWrLOT;
        private System.Windows.Forms.Label lblUtcLotStatus;
        // PWM / Diag Tab
        private System.Windows.Forms.TabPage tabPwmDiag;
        private System.Windows.Forms.TabPage tabParamTuning;
        private System.Windows.Forms.Button btnOpenParamTuning;
        private System.Windows.Forms.GroupBox grpPwmCtrl;
        private System.Windows.Forms.Label lblPwmPort;
        private System.Windows.Forms.NumericUpDown nudPwmPort;
        private System.Windows.Forms.Label lblPwmDuty;
        private System.Windows.Forms.NumericUpDown nudPwmDuty;
        private System.Windows.Forms.Button btnSetPwm;
        private System.Windows.Forms.Button btnGetPwm;
        private System.Windows.Forms.Label lblPwmValCap;
        private System.Windows.Forms.Label lblPwmVal;
        private System.Windows.Forms.GroupBox grpPwmMax;
        private System.Windows.Forms.Label lblPwmMaxMask;
        private System.Windows.Forms.NumericUpDown nudPwmMaxMask0;
        private System.Windows.Forms.NumericUpDown nudPwmMaxMask1;
        private System.Windows.Forms.NumericUpDown nudPwmMaxMask2;
        private System.Windows.Forms.Button btnSetPwmMax;
        private System.Windows.Forms.Label lblPwmMaxStatus;
        private System.Windows.Forms.GroupBox grpDiagRead;
        private System.Windows.Forms.Button btnGetPnVolt;
        private System.Windows.Forms.Label lblPnVoltCap;
        private System.Windows.Forms.Label lblPnVoltVal;
        private System.Windows.Forms.NumericUpDown nudPnVoltOpt;
        private System.Windows.Forms.Button btnGetCurrent;
        private System.Windows.Forms.Label lblCurrentCap;
        private System.Windows.Forms.Label lblCurrentVal;
        private System.Windows.Forms.Label lblPwmDiagStatus;
    }
}
