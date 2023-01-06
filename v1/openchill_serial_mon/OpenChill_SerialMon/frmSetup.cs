using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace OpenChill_SerialMon
{
    public partial class frmSetup : Form
    {
        private frmMain parentForm;

        public frmSetup(frmMain parentForm)
        {
            this.parentForm = parentForm;
            InitializeComponent();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Hide();
        }

        private void frmSetup_Load(object sender, EventArgs e)
        {
            this.updateSerialPortList();
        }

        private void updateSerialPortList()
        {
            this.cboSerialPortList.Items.Clear();

            try
            {
                string[] allAvailablePorts = SerialPort.GetPortNames();

                foreach (string portName in allAvailablePorts)
                    this.cboSerialPortList.Items.Add(portName);
            }
            catch(Exception ex)
            {
                MessageBox.Show("Could not enumerate system serial ports: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

        }

        private void btnRefreshSerialPorts_Click(object sender, EventArgs e)
        {
            this.updateSerialPortList();
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if(this.cboSerialPortList.Text == "")
            {
                MessageBox.Show("Please select a valid Serial Port from the dropdown list.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            this.parentForm.ConnectToOpenChill(this.cboSerialPortList.Text);

            this.Hide();

        }
    }
}
