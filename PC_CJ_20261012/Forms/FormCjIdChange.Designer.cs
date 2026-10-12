namespace LINMaster.Forms
{
    partial class FormCjIdChange
    {
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.Label lblTitle;
        private System.Windows.Forms.Label lblCurrentCaption;
        private System.Windows.Forms.Label lblCurrentId;
        private System.Windows.Forms.Label lblNewCaption;
        private System.Windows.Forms.Label lblHexPrefix;
        private System.Windows.Forms.NumericUpDown nudNewChairId;
        private System.Windows.Forms.Label lblGuide;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.Button btnCancel;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.lblTitle = new System.Windows.Forms.Label();
            this.lblCurrentCaption = new System.Windows.Forms.Label();
            this.lblCurrentId = new System.Windows.Forms.Label();
            this.lblNewCaption = new System.Windows.Forms.Label();
            this.lblHexPrefix = new System.Windows.Forms.Label();
            this.nudNewChairId = new System.Windows.Forms.NumericUpDown();
            this.lblGuide = new System.Windows.Forms.Label();
            this.btnApply = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.nudNewChairId)).BeginInit();
            this.SuspendLayout();
            //
            // lblTitle
            //
            this.lblTitle.AutoSize = true;
            this.lblTitle.Font = new System.Drawing.Font("Consolas", 13F, System.Drawing.FontStyle.Bold);
            this.lblTitle.ForeColor = System.Drawing.Color.FromArgb(100, 220, 255);
            this.lblTitle.Location = new System.Drawing.Point(20, 18);
            this.lblTitle.Name = "lblTitle";
            this.lblTitle.Size = new System.Drawing.Size(160, 22);
            this.lblTitle.TabIndex = 0;
            this.lblTitle.Text = "CJ 의자 ID 변경";
            //
            // lblCurrentCaption
            //
            this.lblCurrentCaption.AutoSize = true;
            this.lblCurrentCaption.Location = new System.Drawing.Point(22, 65);
            this.lblCurrentCaption.Name = "lblCurrentCaption";
            this.lblCurrentCaption.Size = new System.Drawing.Size(75, 16);
            this.lblCurrentCaption.TabIndex = 1;
            this.lblCurrentCaption.Text = "현재 ID";
            //
            // lblCurrentId
            //
            this.lblCurrentId.AutoSize = true;
            this.lblCurrentId.Font = new System.Drawing.Font("Consolas", 11F, System.Drawing.FontStyle.Bold);
            this.lblCurrentId.ForeColor = System.Drawing.Color.FromArgb(120, 200, 255);
            this.lblCurrentId.Location = new System.Drawing.Point(146, 64);
            this.lblCurrentId.Name = "lblCurrentId";
            this.lblCurrentId.Size = new System.Drawing.Size(48, 18);
            this.lblCurrentId.TabIndex = 2;
            this.lblCurrentId.Text = "0x0A";
            //
            // lblNewCaption
            //
            this.lblNewCaption.AutoSize = true;
            this.lblNewCaption.Location = new System.Drawing.Point(22, 103);
            this.lblNewCaption.Name = "lblNewCaption";
            this.lblNewCaption.Size = new System.Drawing.Size(75, 16);
            this.lblNewCaption.TabIndex = 3;
            this.lblNewCaption.Text = "변경 ID";
            //
            // lblHexPrefix
            //
            this.lblHexPrefix.AutoSize = true;
            this.lblHexPrefix.Font = new System.Drawing.Font("Consolas", 10F);
            this.lblHexPrefix.Location = new System.Drawing.Point(146, 103);
            this.lblHexPrefix.Name = "lblHexPrefix";
            this.lblHexPrefix.Size = new System.Drawing.Size(24, 17);
            this.lblHexPrefix.TabIndex = 4;
            this.lblHexPrefix.Text = "0x";
            //
            // nudNewChairId
            //
            this.nudNewChairId.BackColor = System.Drawing.Color.FromArgb(35, 48, 66);
            this.nudNewChairId.ForeColor = System.Drawing.Color.White;
            this.nudNewChairId.Hexadecimal = true;
            this.nudNewChairId.Location = new System.Drawing.Point(174, 100);
            this.nudNewChairId.Maximum = new decimal(new int[] { 13, 0, 0, 0 });
            this.nudNewChairId.Minimum = new decimal(new int[] { 10, 0, 0, 0 });
            this.nudNewChairId.Name = "nudNewChairId";
            this.nudNewChairId.Size = new System.Drawing.Size(68, 23);
            this.nudNewChairId.TabIndex = 5;
            this.nudNewChairId.Value = new decimal(new int[] { 10, 0, 0, 0 });
            //
            // lblGuide
            //
            this.lblGuide.ForeColor = System.Drawing.Color.FromArgb(170, 180, 200);
            this.lblGuide.Location = new System.Drawing.Point(22, 139);
            this.lblGuide.Name = "lblGuide";
            this.lblGuide.Size = new System.Drawing.Size(305, 37);
            this.lblGuide.TabIndex = 6;
            this.lblGuide.Text = "0x0A~0x0D 중 선택. 다른 보드와 ID가 중복되면 개별 진단 확인은 불가합니다.";
            //
            // btnApply
            //
            this.btnApply.BackColor = System.Drawing.Color.FromArgb(45, 105, 175);
            this.btnApply.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnApply.ForeColor = System.Drawing.Color.White;
            this.btnApply.Location = new System.Drawing.Point(149, 190);
            this.btnApply.Name = "btnApply";
            this.btnApply.Size = new System.Drawing.Size(86, 32);
            this.btnApply.TabIndex = 7;
            this.btnApply.Text = "변경";
            this.btnApply.UseVisualStyleBackColor = false;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            //
            // btnCancel
            //
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCancel.ForeColor = System.Drawing.Color.White;
            this.btnCancel.Location = new System.Drawing.Point(242, 190);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(86, 32);
            this.btnCancel.TabIndex = 8;
            this.btnCancel.Text = "취소";
            this.btnCancel.UseVisualStyleBackColor = true;
            //
            // FormCjIdChange
            //
            this.AcceptButton = this.btnApply;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(23, 30, 48);
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(350, 240);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.lblGuide);
            this.Controls.Add(this.nudNewChairId);
            this.Controls.Add(this.lblHexPrefix);
            this.Controls.Add(this.lblNewCaption);
            this.Controls.Add(this.lblCurrentId);
            this.Controls.Add(this.lblCurrentCaption);
            this.Controls.Add(this.lblTitle);
            this.Font = new System.Drawing.Font("Consolas", 9F);
            this.ForeColor = System.Drawing.Color.White;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormCjIdChange";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "CJ 의자 ID 변경";
            ((System.ComponentModel.ISupportInitialize)(this.nudNewChairId)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();
        }
    }
}
