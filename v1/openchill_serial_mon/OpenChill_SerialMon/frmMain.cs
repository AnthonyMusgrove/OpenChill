namespace OpenChill_SerialMon
{
    using ChartDirector;
    using System.IO.Ports;

    public partial class frmMain : Form
    {

        public SerialPort srpConn = new SerialPort();
        public frmSetup setupForm;

        public frmMain()
        {
            InitializeComponent();
            this.setupForm = new frmSetup(this);
        }

        // Output example:  T:16.00,H:63.00,DP:5.00,RT:11.75,FR:0 or 1 (fridge compressor circuit on or off)
        // BAUD Rate of OpenChill 115200

        public void addToLog(String data)
        {
            if(this.txtLog.InvokeRequired)
            {
                this.txtLog.Invoke(new Action(() => this.txtLog.Text += Environment.NewLine + data));
            }
            else
            {
                this.txtLog.Text += Environment.NewLine + data;
            }
        }


        private void updateTemperatureChart(double temperature_value)
        {
            ChartDirector.AngularMeter mtrTemperature = new AngularMeter(200, 200);
            mtrTemperature.setMeter(100, 100, 90, -145, 145);

            mtrTemperature.addRing(0, 80, 0xeeeeee);

            mtrTemperature.setScale(0, 40, 10, 5, 1);

            mtrTemperature.setLabelStyle("Arial Italic", 10);
            mtrTemperature.setTickLength(-12, -9, -6);
            mtrTemperature.setLineWidth(0, 2, 1, 1);

            //mtrTemperature.addText(125, 175, "Temperature", "Arial Italic", 15, Chart.TextColor, Chart.Center);

            mtrTemperature.addPointer2(temperature_value, 0xff0000);

            this.wcViewerTemperature.Chart = mtrTemperature;
        }


        private void updateHumidityChart(double humidity_value)
        {
            ChartDirector.AngularMeter mtrHumidity = new AngularMeter(200, 200);
            mtrHumidity.setMeter(100, 100, 90, -145, 145);

            mtrHumidity.addRing(0, 80, 0xeeeeee);

            mtrHumidity.setScale(0, 100, 10, 5, 1);

            mtrHumidity.setLabelStyle("Arial Italic", 10);
            mtrHumidity.setTickLength(-12, -9, -6);
            mtrHumidity.setLineWidth(0, 2, 1, 1);

            //mtrTemperature.addText(125, 175, "Temperature", "Arial Italic", 15, Chart.TextColor, Chart.Center);

            mtrHumidity.addPointer2(humidity_value, 0xff0000);

            this.wcViewerHumidity.Chart = mtrHumidity;

        }

        private void updateWaterTemperatureChart(double water_temperature)
        {
            ChartDirector.AngularMeter mtrWaterTemperature = new AngularMeter(200, 200);
            mtrWaterTemperature.setMeter(100, 100, 90, -145, 145);

            mtrWaterTemperature.addRing(0, 80, 0xeeeeee);

            mtrWaterTemperature.setScale(0, 40, 10, 5, 1);

            mtrWaterTemperature.setLabelStyle("Arial Italic", 10);
            mtrWaterTemperature.setTickLength(-12, -9, -6);
            mtrWaterTemperature.setLineWidth(0, 2, 1, 1);

            //mtrTemperature.addText(125, 175, "Temperature", "Arial Italic", 15, Chart.TextColor, Chart.Center);

            mtrWaterTemperature.addPointer2(water_temperature, 0xff0000);

            this.wcViewerWaterTemperature.Chart = mtrWaterTemperature;
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            this.updateTemperatureChart(0);
            this.updateHumidityChart(0);
            this.updateWaterTemperatureChart(0);
        }

        public void ConnectToOpenChill(String SerialPortName)
        {
            try
            {
                this.srpConn = new SerialPort(SerialPortName, 115200);
                this.srpConn.Open();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Could not connect to selected serial port: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }

            //assume connection open

            this.srpConn.DataReceived += SrpConn_DataReceived;

        }

        //byte[] inBuffer = new byte[2];
        byte[] inBuffer = new byte[40];
        String curDataToProcess = "";


        private void SrpConn_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            //  // Output example:  |T:16.00,H:63.00,DP:5.00,RT:11.75,FR:0 or 1 (fridge compressor circuit on or off)^
            this.srpConn.Read(inBuffer, 0, this.srpConn.BytesToRead);
            this.curDataToProcess += System.Text.Encoding.UTF8.GetString(inBuffer);

            this.processData();

        }

        private void processData()
        {
            string curFrame = "";
            Boolean hasStartedFrame = false;
            
            for(int t=0; t<this.curDataToProcess.Length; t++)
            {
                if (this.curDataToProcess[t] == '|')
                {
                    //Start of frame
                    if (hasStartedFrame)
                        curFrame = "";

                    hasStartedFrame = true;
                }
                else if(this.curDataToProcess[t] == '^')
                {
                    //End of frame
                    this.ProcessCompleteDataFrame(curFrame);
                    curFrame = "";
                    hasStartedFrame = false;
                }
                else
                {
                    if (hasStartedFrame)
                        curFrame += this.curDataToProcess[t];
                }
            }

            //will need to remove any data that was processed from curDataToProcess , but for now, lets just clear it.
            this.curDataToProcess = "";
        }

        private void ProcessCompleteDataFrame(String DataFrame)
        {
            if (DataFrame.Length < 10)
                return;

            //T:12.00,H:68.00,DP:5.60,RT:15.50,FR:1^|T:12.00,H:68.00,DP:5.60,RT:15.74,FR:1
            String[] frameParts = DataFrame.Split(',');

            foreach (String framePart in frameParts)
            {
                String[] segmentParts = framePart.Split(':');

                switch(segmentParts[0].ToUpper())
                {
                    case "T":  //room temperature
                        //addToLog("received Room Temperature value: " + segmentParts[1]);
                        this.updateTemperatureChart(double.Parse(segmentParts[1]));
                        this.wcViewerTemperature.updateViewPort(true, false);
                        break;

                    case "H":  //room humidity

                        this.updateHumidityChart(double.Parse(segmentParts[1]));
                        this.wcViewerHumidity.updateViewPort(true, false);

                        break;

                    case "DP": // dew point
                        this.lblDewPoint.Text = segmentParts[1];
                        break;

                    case "RT": // reservior / water temperature
                        this.updateWaterTemperatureChart(double.Parse(segmentParts[1]));
                        this.wcViewerWaterTemperature.updateViewPort(true, false);

                        break;

                    case "FR": // Fridge compressor status

                        if(segmentParts[1] == "0")
                        {
                            this.lblChillerStatus.Text = "Off.";
                        }
                        else
                        {
                            this.lblChillerStatus.Text = "On.";
                        }

                        break;
                }

            }



            //addToLog("Data Frame Received: " + DataFrame);
        }

        private void btnSettings_Click(object sender, EventArgs e)
        {
            this.setupForm.Show();
        }
    }
}