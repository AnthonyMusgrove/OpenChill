namespace OpenChill_SerialMon
{
    partial class frmMain
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.wcViewerTemperature = new ChartDirector.WinChartViewer();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.wcViewerHumidity = new ChartDirector.WinChartViewer();
            this.label3 = new System.Windows.Forms.Label();
            this.wcViewerWaterTemperature = new ChartDirector.WinChartViewer();
            this.label4 = new System.Windows.Forms.Label();
            this.lblDewPoint = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.lblChillerStatus = new System.Windows.Forms.Label();
            this.btnSettings = new System.Windows.Forms.Button();
            this.txtLog = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.wcViewerTemperature)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.wcViewerHumidity)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.wcViewerWaterTemperature)).BeginInit();
            this.SuspendLayout();
            // 
            // wcViewerTemperature
            // 
            this.wcViewerTemperature.Location = new System.Drawing.Point(24, 12);
            this.wcViewerTemperature.Name = "wcViewerTemperature";
            this.wcViewerTemperature.Size = new System.Drawing.Size(200, 200);
            this.wcViewerTemperature.TabIndex = 0;
            this.wcViewerTemperature.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Segoe UI Semibold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.label1.Location = new System.Drawing.Point(14, 194);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(211, 25);
            this.label1.TabIndex = 1;
            this.label1.Text = "Room Temperature (°C)";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Segoe UI Semibold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.label2.Location = new System.Drawing.Point(249, 194);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(180, 25);
            this.label2.TabIndex = 3;
            this.label2.Text = "Room Humidity (%)";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // wcViewerHumidity
            // 
            this.wcViewerHumidity.Location = new System.Drawing.Point(239, 12);
            this.wcViewerHumidity.Name = "wcViewerHumidity";
            this.wcViewerHumidity.Size = new System.Drawing.Size(200, 200);
            this.wcViewerHumidity.TabIndex = 2;
            this.wcViewerHumidity.TabStop = false;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Segoe UI Semibold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.label3.Location = new System.Drawing.Point(449, 194);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(212, 25);
            this.label3.TabIndex = 5;
            this.label3.Text = "Water Temperature (°C)";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // wcViewerWaterTemperature
            // 
            this.wcViewerWaterTemperature.Location = new System.Drawing.Point(454, 12);
            this.wcViewerWaterTemperature.Name = "wcViewerWaterTemperature";
            this.wcViewerWaterTemperature.Size = new System.Drawing.Size(200, 200);
            this.wcViewerWaterTemperature.TabIndex = 4;
            this.wcViewerWaterTemperature.TabStop = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Segoe UI Semibold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.label4.Location = new System.Drawing.Point(14, 235);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(140, 25);
            this.label4.TabIndex = 6;
            this.label4.Text = "Dew Point (°C):";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblDewPoint
            // 
            this.lblDewPoint.AutoSize = true;
            this.lblDewPoint.Font = new System.Drawing.Font("Segoe UI Semibold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.lblDewPoint.Location = new System.Drawing.Point(174, 235);
            this.lblDewPoint.Name = "lblDewPoint";
            this.lblDewPoint.Size = new System.Drawing.Size(50, 25);
            this.lblDewPoint.TabIndex = 7;
            this.lblDewPoint.Text = "0.00";
            this.lblDewPoint.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Segoe UI Semibold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.label5.Location = new System.Drawing.Point(299, 235);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(129, 25);
            this.label5.TabIndex = 8;
            this.label5.Text = "Chiller Status:";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblChillerStatus
            // 
            this.lblChillerStatus.AutoSize = true;
            this.lblChillerStatus.Font = new System.Drawing.Font("Segoe UI Semibold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.lblChillerStatus.Location = new System.Drawing.Point(454, 235);
            this.lblChillerStatus.Name = "lblChillerStatus";
            this.lblChillerStatus.Size = new System.Drawing.Size(44, 25);
            this.lblChillerStatus.TabIndex = 9;
            this.lblChillerStatus.Text = "Off.";
            this.lblChillerStatus.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btnSettings
            // 
            this.btnSettings.Font = new System.Drawing.Font("Segoe UI Semibold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.btnSettings.Location = new System.Drawing.Point(598, 237);
            this.btnSettings.Name = "btnSettings";
            this.btnSettings.Size = new System.Drawing.Size(56, 23);
            this.btnSettings.TabIndex = 10;
            this.btnSettings.Text = "Setup";
            this.btnSettings.UseVisualStyleBackColor = true;
            this.btnSettings.Click += new System.EventHandler(this.btnSettings_Click);
            // 
            // txtLog
            // 
            this.txtLog.Location = new System.Drawing.Point(667, 12);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtLog.Size = new System.Drawing.Size(564, 244);
            this.txtLog.TabIndex = 11;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(658, 274);
            this.Controls.Add(this.txtLog);
            this.Controls.Add(this.btnSettings);
            this.Controls.Add(this.lblChillerStatus);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.lblDewPoint);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.wcViewerWaterTemperature);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.wcViewerHumidity);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.wcViewerTemperature);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "frmMain";
            this.Text = "OpenChill Stats";
            this.Load += new System.EventHandler(this.frmMain_Load);
            ((System.ComponentModel.ISupportInitialize)(this.wcViewerTemperature)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.wcViewerHumidity)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.wcViewerWaterTemperature)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private ChartDirector.WinChartViewer wcViewerTemperature;
        private Label label1;
        private Label label2;
        private ChartDirector.WinChartViewer wcViewerHumidity;
        private Label label3;
        private ChartDirector.WinChartViewer wcViewerWaterTemperature;
        private Label label4;
        private Label lblDewPoint;
        private Label label5;
        private Label lblChillerStatus;
        private Button btnSettings;
        private TextBox txtLog;
    }
}