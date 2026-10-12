namespace LINMaster.Forms
{
    partial class FormMacChange
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
                components.Dispose();
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        private void InitializeComponent()
        {
            this.grpRead        = new System.Windows.Forms.GroupBox();
            this.lblCurrentMac  = new System.Windows.Forms.Label();
            this.lblCurrentNode = new System.Windows.Forms.Label();
            this.lblCurMacLbl   = new System.Windows.Forms.Label();
            this.lblCurNodeLbl  = new System.Windows.Forms.Label();
            this.btnRead        = new System.Windows.Forms.Button();
            this.btnReadOtp     = new System.Windows.Forms.Button();

            this.grpWrite       = new System.Windows.Forms.GroupBox();
            this.lblNewMacLbl   = new System.Windows.Forms.Label();
            this.txtNewMac      = new System.Windows.Forms.TextBox();
            this.lblNewNodeLbl  = new System.Windows.Forms.Label();
            this.nudNewNode     = new System.Windows.Forms.NumericUpDown();
            this.btnWriteMac    = new System.Windows.Forms.Button();
            this.btnWriteNode   = new System.Windows.Forms.Button();
            this.btnWriteAll    = new System.Windows.Forms.Button();
            this.btnProgramOtp  = new System.Windows.Forms.Button();

            this.grpNode        = new System.Windows.Forms.GroupBox();
            this.lblNADLbl      = new System.Windows.Forms.Label();
            this.nudNAD         = new System.Windows.Forms.NumericUpDown();

            this.lblStatus      = new System.Windows.Forms.Label();
            this.pnlNote        = new System.Windows.Forms.Panel();
            this.lblNote        = new System.Windows.Forms.Label();

            this.grpRead.SuspendLayout();
            this.grpWrite.SuspendLayout();
            this.grpNode.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudNewNode)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudNAD)).BeginInit();
            this.pnlNote.SuspendLayout();
            this.SuspendLayout();

            // ── 폼 배경색 ─────────────────────────────────────────
            System.Drawing.Color bg       = System.Drawing.Color.FromArgb(26, 26, 38);
            System.Drawing.Color grpBg    = System.Drawing.Color.FromArgb(34, 34, 50);
            System.Drawing.Color accent   = System.Drawing.Color.FromArgb(60, 140, 200);
            System.Drawing.Color accentGr = System.Drawing.Color.FromArgb(40, 160, 100);
            System.Drawing.Color accentOr = System.Drawing.Color.FromArgb(180, 100, 40);
            System.Drawing.Color accentPu = System.Drawing.Color.FromArgb(100, 60, 160);
            System.Drawing.Color textCol  = System.Drawing.Color.FromArgb(220, 220, 230);
            System.Drawing.Color valCol   = System.Drawing.Color.FromArgb(140, 220, 255);
            var font9b   = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Bold);
            var font9    = new System.Drawing.Font("Consolas", 9F);
            var font10b  = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Bold);
            var font11b  = new System.Drawing.Font("Consolas", 11F, System.Drawing.FontStyle.Bold);

            // ─────────────────────────────────────────────────────
            // grpNode – NAD 선택
            // ─────────────────────────────────────────────────────
            this.grpNode.BackColor = grpBg;
            this.grpNode.ForeColor = System.Drawing.Color.FromArgb(160, 160, 180);
            this.grpNode.Font      = font9;
            this.grpNode.Text      = "[ 대상 디바이스 NAD ]";
            this.grpNode.Location  = new System.Drawing.Point(12, 12);
            this.grpNode.Size      = new System.Drawing.Size(460, 60);
            this.grpNode.TabIndex  = 0;

            this.lblNADLbl.Text      = "NAD (0x01~0x7F):";
            this.lblNADLbl.ForeColor = textCol;
            this.lblNADLbl.Font      = font9;
            this.lblNADLbl.Location  = new System.Drawing.Point(12, 26);
            this.lblNADLbl.AutoSize  = true;
            this.lblNADLbl.TabIndex  = 0;

            this.nudNAD.Minimum     = 1;
            this.nudNAD.Maximum     = 127;
            this.nudNAD.Value       = 1;
            this.nudNAD.Hexadecimal = true;
            this.nudNAD.BackColor   = System.Drawing.Color.FromArgb(40, 40, 58);
            this.nudNAD.ForeColor   = valCol;
            this.nudNAD.Font        = font9b;
            this.nudNAD.Location    = new System.Drawing.Point(190, 22);
            this.nudNAD.Size        = new System.Drawing.Size(80, 22);
            this.nudNAD.TabIndex    = 1;

            this.grpNode.Controls.Add(this.lblNADLbl);
            this.grpNode.Controls.Add(this.nudNAD);

            // ─────────────────────────────────────────────────────
            // grpRead – 현재 값 읽기
            // ─────────────────────────────────────────────────────
            this.grpRead.BackColor = grpBg;
            this.grpRead.ForeColor = System.Drawing.Color.FromArgb(160, 160, 180);
            this.grpRead.Font      = font9;
            this.grpRead.Text      = "[ 현재 값 읽기 ]";
            this.grpRead.Location  = new System.Drawing.Point(12, 84);
            this.grpRead.Size      = new System.Drawing.Size(460, 130);
            this.grpRead.TabIndex  = 1;

            this.lblCurMacLbl.Text      = "MAC Address:";
            this.lblCurMacLbl.ForeColor = textCol;
            this.lblCurMacLbl.Font      = font9;
            this.lblCurMacLbl.Location  = new System.Drawing.Point(12, 32);
            this.lblCurMacLbl.AutoSize  = true;

            this.lblCurrentMac.Text      = "─  (Read 버튼 클릭)";
            this.lblCurrentMac.ForeColor = valCol;
            this.lblCurrentMac.Font      = font11b;
            this.lblCurrentMac.Location  = new System.Drawing.Point(140, 28);
            this.lblCurrentMac.Size      = new System.Drawing.Size(300, 26);

            this.lblCurNodeLbl.Text      = "PLCA Node ID:";
            this.lblCurNodeLbl.ForeColor = textCol;
            this.lblCurNodeLbl.Font      = font9;
            this.lblCurNodeLbl.Location  = new System.Drawing.Point(12, 66);
            this.lblCurNodeLbl.AutoSize  = true;

            this.lblCurrentNode.Text      = "─  (Read 버튼 클릭)";
            this.lblCurrentNode.ForeColor = valCol;
            this.lblCurrentNode.Font      = font11b;
            this.lblCurrentNode.Location  = new System.Drawing.Point(140, 62);
            this.lblCurrentNode.Size      = new System.Drawing.Size(300, 26);

            this.btnRead.Text          = "▶  Read  (현재 값 읽기)";
            this.btnRead.BackColor     = accent;
            this.btnRead.ForeColor     = System.Drawing.Color.White;
            this.btnRead.FlatStyle     = System.Windows.Forms.FlatStyle.Flat;
            this.btnRead.FlatAppearance.BorderSize = 0;
            this.btnRead.Font          = font9b;
            this.btnRead.Location      = new System.Drawing.Point(12, 95);
            this.btnRead.Size          = new System.Drawing.Size(200, 26);
            this.btnRead.TabIndex      = 0;
            this.btnRead.Click        += new System.EventHandler(this.btnRead_Click);

            this.btnReadOtp.Text          = "▶  Read OTP  (OTP 값 읽기)";
            this.btnReadOtp.BackColor     = System.Drawing.Color.FromArgb(60, 110, 90);
            this.btnReadOtp.ForeColor     = System.Drawing.Color.White;
            this.btnReadOtp.FlatStyle     = System.Windows.Forms.FlatStyle.Flat;
            this.btnReadOtp.FlatAppearance.BorderSize = 0;
            this.btnReadOtp.Font          = font9b;
            this.btnReadOtp.Location      = new System.Drawing.Point(222, 95);
            this.btnReadOtp.Size          = new System.Drawing.Size(220, 26);
            this.btnReadOtp.TabIndex      = 6;
            this.btnReadOtp.Click        += new System.EventHandler(this.btnReadOtp_Click);

            this.grpRead.Controls.Add(this.lblCurMacLbl);
            this.grpRead.Controls.Add(this.lblCurrentMac);
            this.grpRead.Controls.Add(this.lblCurNodeLbl);
            this.grpRead.Controls.Add(this.lblCurrentNode);
            this.grpRead.Controls.Add(this.btnRead);
            this.grpRead.Controls.Add(this.btnReadOtp);

            // ─────────────────────────────────────────────────────
            // grpWrite – 새 값 입력 및 쓰기
            // ─────────────────────────────────────────────────────
            this.grpWrite.BackColor = grpBg;
            this.grpWrite.ForeColor = System.Drawing.Color.FromArgb(160, 160, 180);
            this.grpWrite.Font      = font9;
            this.grpWrite.Text      = "[ 변경 값 입력 및 쓰기  (OspBridge: MAC 레지스터 + 재초기화) ]";
            this.grpWrite.Location  = new System.Drawing.Point(12, 228);
            this.grpWrite.Size      = new System.Drawing.Size(460, 240);
            this.grpWrite.TabIndex  = 2;

            // MAC 입력
            this.lblNewMacLbl.Text      = "새 MAC Address:";
            this.lblNewMacLbl.ForeColor = textCol;
            this.lblNewMacLbl.Font      = font9;
            this.lblNewMacLbl.Location  = new System.Drawing.Point(12, 34);
            this.lblNewMacLbl.AutoSize  = true;

            this.txtNewMac.BackColor    = System.Drawing.Color.FromArgb(40, 40, 58);
            this.txtNewMac.ForeColor    = System.Drawing.Color.White;
            this.txtNewMac.Font         = font10b;
            this.txtNewMac.Location     = new System.Drawing.Point(170, 30);
            this.txtNewMac.Size         = new System.Drawing.Size(200, 24);
            this.txtNewMac.Text         = "00:E0:22:FE:70:01";
            this.txtNewMac.TabIndex     = 0;
            this.txtNewMac.BorderStyle  = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtNewMac.Leave       += new System.EventHandler(this.txtNewMac_Leave);
            this.txtNewMac.Enter       += new System.EventHandler(this.txtNewMac_Enter);

            // Node 입력
            this.lblNewNodeLbl.Text      = "새 PLCA Node ID:";
            this.lblNewNodeLbl.ForeColor = textCol;
            this.lblNewNodeLbl.Font      = font9;
            this.lblNewNodeLbl.Location  = new System.Drawing.Point(12, 70);
            this.lblNewNodeLbl.AutoSize  = true;

            this.nudNewNode.Minimum     = 0;
            this.nudNewNode.Maximum     = 254;
            this.nudNewNode.Value       = 1;
            this.nudNewNode.BackColor   = System.Drawing.Color.FromArgb(40, 40, 58);
            this.nudNewNode.ForeColor   = System.Drawing.Color.White;
            this.nudNewNode.Font        = font10b;
            this.nudNewNode.Location    = new System.Drawing.Point(170, 66);
            this.nudNewNode.Size        = new System.Drawing.Size(80, 24);
            this.nudNewNode.TabIndex    = 1;

            // 버튼들
            this.btnWriteMac.Text          = "💾  MAC만 쓰기";
            this.btnWriteMac.BackColor     = accentGr;
            this.btnWriteMac.ForeColor     = System.Drawing.Color.White;
            this.btnWriteMac.FlatStyle     = System.Windows.Forms.FlatStyle.Flat;
            this.btnWriteMac.FlatAppearance.BorderSize = 0;
            this.btnWriteMac.Font          = font9b;
            this.btnWriteMac.Location      = new System.Drawing.Point(12, 110);
            this.btnWriteMac.Size          = new System.Drawing.Size(130, 28);
            this.btnWriteMac.TabIndex      = 2;
            this.btnWriteMac.Click        += new System.EventHandler(this.btnWriteMac_Click);

            this.btnWriteNode.Text          = "💾  Node만 쓰기";
            this.btnWriteNode.BackColor     = accentOr;
            this.btnWriteNode.ForeColor     = System.Drawing.Color.White;
            this.btnWriteNode.FlatStyle     = System.Windows.Forms.FlatStyle.Flat;
            this.btnWriteNode.FlatAppearance.BorderSize = 0;
            this.btnWriteNode.Font          = font9b;
            this.btnWriteNode.Location      = new System.Drawing.Point(152, 110);
            this.btnWriteNode.Size          = new System.Drawing.Size(140, 28);
            this.btnWriteNode.TabIndex      = 3;
            this.btnWriteNode.Click        += new System.EventHandler(this.btnWriteNode_Click);

            this.btnWriteAll.Text          = "🔒  MAC + Node 동시 쓰기";
            this.btnWriteAll.BackColor     = accentPu;
            this.btnWriteAll.ForeColor     = System.Drawing.Color.White;
            this.btnWriteAll.FlatStyle     = System.Windows.Forms.FlatStyle.Flat;
            this.btnWriteAll.FlatAppearance.BorderSize = 0;
            this.btnWriteAll.Font          = font9b;
            this.btnWriteAll.Location      = new System.Drawing.Point(12, 150);
            this.btnWriteAll.Size          = new System.Drawing.Size(220, 28);
            this.btnWriteAll.TabIndex      = 4;
            this.btnWriteAll.Click        += new System.EventHandler(this.btnWriteAll_Click);

            this.btnProgramOtp.Text          = "OTP One-Time Write";
            this.btnProgramOtp.BackColor     = System.Drawing.Color.FromArgb(170, 50, 50);
            this.btnProgramOtp.ForeColor     = System.Drawing.Color.White;
            this.btnProgramOtp.FlatStyle     = System.Windows.Forms.FlatStyle.Flat;
            this.btnProgramOtp.FlatAppearance.BorderSize = 0;
            this.btnProgramOtp.Font          = font9b;
            this.btnProgramOtp.Location      = new System.Drawing.Point(242, 150);
            this.btnProgramOtp.Size          = new System.Drawing.Size(200, 28);
            this.btnProgramOtp.TabIndex      = 5;
            this.btnProgramOtp.Click        += new System.EventHandler(this.btnProgramOtp_Click);

            this.grpWrite.Controls.Add(this.lblNewMacLbl);
            this.grpWrite.Controls.Add(this.txtNewMac);
            this.grpWrite.Controls.Add(this.lblNewNodeLbl);
            this.grpWrite.Controls.Add(this.nudNewNode);
            this.grpWrite.Controls.Add(this.btnWriteMac);
            this.grpWrite.Controls.Add(this.btnWriteNode);
            this.grpWrite.Controls.Add(this.btnWriteAll);
            this.grpWrite.Controls.Add(this.btnProgramOtp);

            // ─────────────────────────────────────────────────────
            // 상태 표시줄
            // ─────────────────────────────────────────────────────
            this.lblStatus.Text      = "준비";
            this.lblStatus.ForeColor = System.Drawing.Color.FromArgb(120, 180, 120);
            this.lblStatus.Font      = font9;
            this.lblStatus.Location  = new System.Drawing.Point(12, 480);
            this.lblStatus.Size      = new System.Drawing.Size(460, 22);
            this.lblStatus.AutoSize  = false;
            this.lblStatus.TabIndex  = 3;

            // ─────────────────────────────────────────────────────
            // 안내 패널
            // ─────────────────────────────────────────────────────
            this.pnlNote.BackColor  = System.Drawing.Color.FromArgb(30, 60, 40);
            this.pnlNote.Location   = new System.Drawing.Point(12, 506);
            this.pnlNote.Size       = new System.Drawing.Size(460, 80);
            this.pnlNote.TabIndex   = 4;
            this.pnlNote.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;

            this.lblNote.Text =
                "ℹ  MAC 쓰기: OspBridge_SetMac → AD3301 LCE + E2B/OSP 재초기화. 전원 OFF 후에는 DIP 스위치 값이 다시 적용될 수 있습니다.\n" +
                "   Node 쓰기: PLCA Node ID만 EAL로 갱신합니다.\n" +
                "   이 창의 Read/Write 는 메인 폼의 FT4222(USB) 연결을 사용합니다 (LIN 장치와 별개).";
            this.lblNote.ForeColor  = System.Drawing.Color.FromArgb(160, 220, 160);
            this.lblNote.Font       = new System.Drawing.Font("Consolas", 8F);
            this.lblNote.Location   = new System.Drawing.Point(8, 6);
            this.lblNote.Size       = new System.Drawing.Size(444, 68);
            this.lblNote.AutoSize   = false;

            this.pnlNote.Controls.Add(this.lblNote);

            // ─────────────────────────────────────────────────────
            // FormMacChange 본체 설정
            // ─────────────────────────────────────────────────────
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 22F);
            this.AutoScaleMode       = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor           = bg;
            this.ClientSize          = new System.Drawing.Size(484, 598);
            this.Font                = font9;
            this.FormBorderStyle     = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox         = false;
            this.MinimizeBox         = false;
            this.Name                = "FormMacChange";
            this.Text                = "ADI MAC Address / PLCA Node ID 설정";
            this.StartPosition       = System.Windows.Forms.FormStartPosition.Manual;

            this.Controls.Add(this.grpNode);
            this.Controls.Add(this.grpRead);
            this.Controls.Add(this.grpWrite);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.pnlNote);

            this.grpRead.ResumeLayout(false);
            this.grpWrite.ResumeLayout(false);
            this.grpNode.ResumeLayout(false);
            this.grpNode.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudNewNode)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudNAD)).EndInit();
            this.pnlNote.ResumeLayout(false);
            this.ResumeLayout(false);
        }

        #endregion

        // ── 컨트롤 선언 ───────────────────────────────────────────
        private System.Windows.Forms.GroupBox       grpNode;
        private System.Windows.Forms.Label          lblNADLbl;
        private System.Windows.Forms.NumericUpDown  nudNAD;

        private System.Windows.Forms.GroupBox       grpRead;
        private System.Windows.Forms.Label          lblCurMacLbl;
        private System.Windows.Forms.Label          lblCurrentMac;
        private System.Windows.Forms.Label          lblCurNodeLbl;
        private System.Windows.Forms.Label          lblCurrentNode;
        private System.Windows.Forms.Button         btnRead;
        private System.Windows.Forms.Button         btnReadOtp;

        private System.Windows.Forms.GroupBox       grpWrite;
        private System.Windows.Forms.Label          lblNewMacLbl;
        private System.Windows.Forms.TextBox        txtNewMac;
        private System.Windows.Forms.Label          lblNewNodeLbl;
        private System.Windows.Forms.NumericUpDown  nudNewNode;
        private System.Windows.Forms.Button         btnWriteMac;
        private System.Windows.Forms.Button         btnWriteNode;
        private System.Windows.Forms.Button         btnWriteAll;
        private System.Windows.Forms.Button         btnProgramOtp;

        private System.Windows.Forms.Label          lblStatus;
        private System.Windows.Forms.Panel          pnlNote;
        private System.Windows.Forms.Label          lblNote;
    }
}
