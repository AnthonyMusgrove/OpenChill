/* 
 *    O P E N C H I L L
 *    Controller Firmware
 *    
 *    Contributors:
 *    Anthony Musgrove  (Project founder, since February 2022)  anthony@labworx.au
 *  
 *    OpenChill is an open source laser chiller firmware and hardware project using readily available parts from JayCar (Australia)
 */

  /*
   *  User Configuration - This data WILL EVENTUALLY be retrieved from the onboard ESP chip EEPROM, so eventually these
   *  variables will no longer be configurable here.
   */


   /* Ver 2.0.0 
   
      Released to the public January 6th, 2023.

      Major change:   #define MKS_BASE_1_4 to use OpenChill on an MKS Base 1.4 3D Printer Board (Extremely stable).  Not recommended at this stage to 
                      build OpenChill board (old version) as new version with much better layout and component choice will be available to the public
                      shortly.

                      Note:  If you have another 3D Printer board laying around that you'd like to see working with OpenChill, please send a ticket on GitHub
                             with the board details including any exact model numbers, brands etc, and a photo or image of the board in question.
   */

//#include <LibPrintf.h>
#include <LiquidCrystal.h>


// #define DEBUG
// OPTIONS:   DEBUG, DEBUGSERIAL, FRIDGECIRCUITDEBUG, SERIALMODE, PRODUCTION (production doesn't do anything yet)
//            ADDITIONAL OPTION FOR ESP8266:  WIFICONSOLE  - with this enabled with OPENCHILL_ESP8266 as the 
//                                                         main board, OpenChill will attempt to connect to the WIFI settings
//                                                         you've specified and publish a basic (for now) outputs page.
#define WIFICONSOLE


#define OPENCHILL_ESP8266


/* OpenChill ESP8266 CPU MAP  (based on WEMOS D1 MINI, ref https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/) */
#ifdef OPENCHILL_ESP8266 

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/* eek, not enough pins on an 8266 to do serial LCD, besides we have wifi on an 8266! */
#define HAS_WIFI

#define LCD_RS -1
#define LCD_EN -1
#define LCD_D4 -1
#define LCD_D5 -1
#define LCD_D6 -1
#define LCD_D7 -1

#define SAFETY_CUTOFF_SIGNAL_PIN D6 /* OUTPUT Signal wire for OpenChill to direct laser to cease operating  */
#define LASER_IN_USE_PIN D7 /* INPUT Signal wire from laser to advise OpenChill that laser is currently in use */


// Available pins:  D0, D1, D2
// Used pins:       D3, D4, D5, D6, D7, D8, A0

#define FRIDGE_RELAY_PIN D4 /* Arduino Digital Pin for the Fridge Relay Circuit */
#define DHT11_PIN D8   /* Arduino Digital Pin for the Signal Wire of the DHT11 Temperature & Humidity Sensor */
#define BUZZER_PIN D5 /* Arduino Digital PWM Piezzo/Buzzer Pin */
#define RESERVOIR_TEMP_PROBE_PIN A0  /* Arduino Analog Pin for the Chiller Temperature Probe Circuit Reading, only ADC on ESP8266 */

#define OVERRIDE_PIN D3


#endif


/*  OpenChill Hardware V1 CPU MAP */
#ifdef OPENCHILL_HARDWARE_V1

#define HAS_LCD

#define LCD_RS 12
#define LCD_EN  11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

#define SAFETY_CUTOFF_SIGNAL_PIN 6 /* OUTPUT Signal wire for OpenChill to direct laser to cease operating  */
#define LASER_IN_USE_PIN 7 /* INPUT Signal wire from laser to advise OpenChill that laser is currently in use */
#define FRIDGE_RELAY_PIN 8 /* Arduino Digital Pin for the Fridge Relay Circuit */
#define DHT11_PIN 9   /* Arduino Digital Pin for the Signal Wire of the DHT11 Temperature & Humidity Sensor */
#define BUZZER_PIN 10 /* Arduino Digital PWM Piezzo/Buzzer Pin */
#define RESERVOIR_TEMP_PROBE_PIN A3  /* Arduino Analog Pin for the Chiller Temperature Probe Circuit Reading */

#define OVERRIDE_PIN -1 /* not yet defined for this hardware */

#endif

/* GT2560_REV_A Geetech GT2560 Rev A Plus CPU MAP - LCD not working yet, I need to look into it, pretty sure its I2C */
#ifdef GT2560_REV_A_PLUS

// should be the same as the ramps boards.. 
#define HAS_LCD

#define LCD_RS 16
#define LCD_EN 17
#define LCD_D4 23
#define LCD_D5 25
#define LCD_D6 27
#define LCD_D7 29

#define SAFETY_CUTOFF_SIGNAL_PIN 6 /* OUTPUT Signal wire for OpenChill to direct laser to cease operating  */
#define LASER_IN_USE_PIN 7 /* INPUT Signal wire from laser to advise OpenChill that laser is currently in use */
#define FRIDGE_RELAY_PIN 4 /* Arduino Digital Pin for the Fridge Relay Circuit | Hot bed */
#define DHT11_PIN 22   /* Arduino Digital Pin for the Signal Wire of the DHT11 Temperature & Humidity Sensor | GT2650 Rev A - Z MIN */
#define BUZZER_PIN 18 /* Arduino Digital PWM Piezzo/Buzzer Pin */
#define RESERVOIR_TEMP_PROBE_PIN 8  /* Arduino Analog Pin for the Chiller Temperature Probe Circuit Reading  | GT2650 Rev A - Temperature Probe 0*/

#define OVERRIDE_PIN 3 /* X MIN */
#endif


/*  MKS Base v1/1.2 < 1.4 CPU MAP */
#ifdef MKS_BASE_1_0

#define HAS_LCD

#define LCD_RS 16
#define LCD_EN  17
#define LCD_D4 23
#define LCD_D5 25
#define LCD_D6 27
#define LCD_D7 29

#define SAFETY_CUTOFF_SIGNAL_PIN 6 /* OUTPUT Signal wire for OpenChill to direct laser to cease operating  */
#define LASER_IN_USE_PIN 7 /* INPUT Signal wire from laser to advise OpenChill that laser is currently in use */
#define FRIDGE_RELAY_PIN 8 /* Arduino Digital Pin for the Fridge Relay Circuit */
#define DHT11_PIN 18   /* Arduino Digital Pin for the Signal Wire of the DHT11 Temperature & Humidity Sensor */
#define BUZZER_PIN 37 /* Arduino Digital PWM Piezzo/Buzzer Pin */
#define RESERVOIR_TEMP_PROBE_PIN A15  /* Arduino Analog Pin for the Chiller Temperature Probe Circuit Reading */

#define OVERRIDE_PIN 3 /* X MIN */

#endif



/*  MKS Base v1.4 CPU MAP */
#ifdef MKS_BASE_1_4

#define HAS_LCD

#define LCD_RS 16
#define LCD_EN  17
#define LCD_D4 23
#define LCD_D5 25
#define LCD_D6 27
#define LCD_D7 29

#define SAFETY_CUTOFF_SIGNAL_PIN 6 /* OUTPUT Signal wire for OpenChill to direct laser to cease operating  */
#define LASER_IN_USE_PIN 7 /* INPUT Signal wire from laser to advise OpenChill that laser is currently in use */
#define FRIDGE_RELAY_PIN 8 /* Arduino Digital Pin for the Fridge Relay Circuit */
#define DHT11_PIN 18   /* Arduino Digital Pin for the Signal Wire of the DHT11 Temperature & Humidity Sensor */
#define BUZZER_PIN 37 /* Arduino Digital PWM Piezzo/Buzzer Pin */
#define RESERVOIR_TEMP_PROBE_PIN A15  /* Arduino Analog Pin for the Chiller Temperature Probe Circuit Reading */

#define OVERRIDE_PIN 3 /* X MIN */

#endif



#ifdef HAS_LCD
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif 

#ifdef HAS_WIFI

#define WIFI_SSID     "YourWIFISSIDHere"
#define WIFI_PASSWD   "YourWIFIPasswordHere"

#define IS_STATIC_IP

/* the following only takes affect if IS_STATIC_IP is defined above */

/* Portal IP Address (Example: 192.168.1.75) */
#define IP_0  192
#define IP_1  168
#define IP_2  1
#define IP_3  85

/* DNS IP Address (Example: 192.168.1.1) */
#define DNS_0 192
#define DNS_1 168
#define DNS_2 1
#define DNS_3 1

/* Gateway/Router Address (Example: 192.168.1.1) */
#define GW_0 192
#define GW_1 168
#define GW_2 1
#define GW_3 1

/* Subnet Mask (Example: 255.255.255.0) */
#define SUBN_0 255
#define SUBN_1 255
#define SUBN_2 255
#define SUBN_3 0

#define WEB_PORT      80
  
ESP8266WebServer oc_portal(WEB_PORT);

#endif

 /* This value was used arbitrarily as my temperature probe on the MKS 1.4 board was always 8 degrees celsius higher than it should be.   
  * 
  * Calibration of the temperature probe using the MKS 1.4 board will be trial and error until I get some type of software calibration
  * working properly.  If your water temperature probe isn't accurate ( DO NOT SOLELY RELY ON THE MKS 4 TEMPERATURE PROBE WITHOUT CALIBRATING ),
  * submit a ticket so I can get some feedback on the best way to move forward with calibration.
 */
#define TEMP_CALIB_DIFF 7

#define DEGREES_ABOVE_DEWPOINT 1  /* OpenChill will aim to maintain a chiller temperature of x degrees ABOVE dewpoint */
#define MAX_TEMPERATURE_ALERT 26.5 /* Currently, OpenChill will emit an audible beep when chiller temperature exceeds this value */


/*  Instead of a single degree value for target above dewpoint, lets introduce a range.  For a few reasons - to stop the
 *   need for cutting in and out when the temperature fluctuates even by error, without having to introduce new code to
 *   detect and ignore (yet).  -anthony@labworx.au
*/

#define TARGET_TEMPERATURE_MIN_ABOVE_DEWPOINT 1
#define TARGET_TEMPERATURE_MAX_ABOVE_DEWPOINT 1


#define DHTTYPE DHT11   // DHT 11

#include "DHT.h"
DHT dht(DHT11_PIN, DHTTYPE);

//
/* variables to store the current DHT11 Temperature & Humidity reading, along with calculated Dew Point */
float currentEnvironmentTemperature;
float currentEnvironmentHumidity;
float currentEnvironmentDewPoint;

/* variables to store Chiller Reservoir Temperature & Analog Read value */
float currentReservoirTemperature = -9999; /* set to really icy cold low, so we can ensure not to act until atleast first sample is taken */
int currentReservoirVoltage;

#define WATER_TEMP_TOTAL_SAMPLES 50

/* variables to control rolling log of measurements for averaging */
float reservoir_temperature_log[WATER_TEMP_TOTAL_SAMPLES]; 
int cur_log_value = -1;

/* Keep track of Fridge circuit Status */
bool FridgeOn = false;

bool OverRide = false;

/* Setup Control Board */
void setup() {
  Serial.begin(115200);  /* Baud Rate MUST match Baud Rate of onboard ESP chip, tip:  Leave both set to 115200 */
  pinMode(BUZZER_PIN, OUTPUT); /* Define Buzzer Pin as an output */
  pinMode(FRIDGE_RELAY_PIN, OUTPUT); /* Define Fridge Relay Pin as an output */

  pinMode(OVERRIDE_PIN, INPUT_PULLUP);


  digitalWrite(FRIDGE_RELAY_PIN, LOW); /* Make sure initially we have the Fridge relay off */
  FridgeOn = false; /* Keep track of fridge circuit status, we've switched it off */

  pinMode(SAFETY_CUTOFF_SIGNAL_PIN, OUTPUT); /* OUTPUT Signal wire for OpenChill to direct laser to cease operating  */
  pinMode(LASER_IN_USE_PIN, INPUT); /* INPUT Signal wire from laser to advise OpenChill that laser is currently in use */

  #ifdef HAS_LCD
    lcd.begin(20, 4);
    setup_lcd();
  #endif 

  #ifdef HAS_WIFI
    setupWifi();
  #endif

  dht.begin();

}


void setupWifi()
{
  #ifdef HAS_WIFI

  Serial.println("Setting up WIFI (ESP8266) ...");
  Serial.print("Connecting to SSID ");
  Serial.print(WIFI_SSID);

  #ifdef IS_STATIC_IP
    IPAddress portal_ip_addr(IP_0, IP_1, IP_2, IP_3);
    IPAddress dns_addr(DNS_0, DNS_1, DNS_2, DNS_3);
    IPAddress gateway_addr(GW_0, GW_1, GW_2, GW_3);
    IPAddress subnet_addr(SUBN_0, SUBN_1, SUBN_2, SUBN_3);

    WiFi.config(portal_ip_addr, gateway_addr, subnet_addr, dns_addr);
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WIFI, IP Address = ");
  Serial.println(WiFi.localIP());

  setupWebPortal();

  #endif
}

#ifdef HAS_WIFI
void showSimpleWebPortal()
{
  String portal_Html = "<!DOCTYPE html><html><head>";
  portal_Html += "<title>OpenChill Simple Web Portal</title>"
   "</head><body style='font-family: helvetica, sans-serif;'>"
   " <script type='text/javascript'>"
   " function query_server_dashboard_data() {"
   "     var xmlhttp = new XMLHttpRequest();"
    
    "    xmlhttp.onreadystatechange = function() {"
    "        if (xmlhttp.readyState == XMLHttpRequest.DONE) {"
    "           if (xmlhttp.status == 200) {"
    "               var dashboard_data_arr = JSON.parse(xmlhttp.responseText); "
    "               var denvtemp = document.getElementById('environment.temperature').innerText = dashboard_data_arr.environment.temperature + '°C'; "
    "               var denvhumidity = document.getElementById('environment.humidity').innerText = dashboard_data_arr.environment.humidity + '%'; "
    "               var denvdewpoint = document.getElementById('environment.dewpoint').innerText = dashboard_data_arr.environment.dewpoint + '°C'; "
    "               var dwatertemp = document.getElementById('water.temperature').innerText = dashboard_data_arr.reservior_temperature + '°C'; "
    "               var dfridgestatus = document.getElementById('fridge.status').innerText = dashboard_data_arr.fridge_status; "
    "               var doverridestatus = document.getElementById('override.status').innerText = dashboard_data_arr.override_status; "
    "           }"
    "           else if (xmlhttp.status == 400) {"
    "             /* alert('There was an error 400'); */" 
    "           }"
    "           else {"
    "               /* alert('something else other than 200 was returned'); */" 
    "           }"
    "        }"
    "    };"
    
    "    xmlhttp.open('GET', 'stats', true);"
    "    xmlhttp.send();"
    " }"

    " setInterval(get_dashboard_data, 1000);"
     
     "function get_dashboard_data() { "
      "query_server_dashboard_data();"    
     "}"

    "</script>"


 "<h2>OpenChill Simple Web Portal</h2>"

 "<table width='100%'>"

 "<tr><td colspan=2 style='background-color:navy; color: white;'><strong>Environment</strong></td></tr>"
  "<tr><td style='width:10%;'>Temperature</td><td id='environment.temperature'>" + String(currentEnvironmentTemperature) +  "</td></tr>"
  "<tr><td style='width:10%;'>Humidity</td><td id='environment.humidity'>" + String(currentEnvironmentHumidity) + "</td></tr>"
  "<tr><td style='width:10%;'>DewPoint</td><td id='environment.dewpoint'>" + String(currentEnvironmentDewPoint) + "</td></tr>"

  "<tr><td colspan=2 style='background-color:navy; color: white;'><strong>Reservoir</strong></td></tr>"
  "<tr><td style='width:10%;'>Coolant Temp</td><td id='water.temperature'>" + String(currentReservoirTemperature) + "</td></tr>"

  "<tr><td colspan=2 style='background-color:navy; color: white;'><strong>Chiller/Compressor/Fridge</strong></td></tr>"
  "<tr><td style='width:10%;'>Status</td><td id='fridge.status'>" + (FridgeOn ? ("On") : ("Off")) + "</td></tr>"

  "<tr><td colspan=2 style='background-color:navy; color: white;'><strong>Override Manual Mode</strong></td></tr>"
  "<tr><td style='width:10%;'>Status</td><td id='override.status'>" + (OverRide ? ("On") : ("Off")) + "</td></tr>"

  "<table></body></html>";

  oc_portal.send(200, "text/html", portal_Html);

}

void sendStatsData()
{
  oc_portal.sendHeader("Access-Control-Allow-Origin", "*");

  String json_Packet = "{"

  "\"environment\":"

  "{"

  "\"temperature\": \"" + String(currentEnvironmentTemperature) + "\","
  "\"humidity\": \"" + String(currentEnvironmentHumidity) + "\","
  "\"dewpoint\": \"" + String(currentEnvironmentDewPoint) + "\""

  "},"

  "\"reservior_temperature\":"

  "\"" + String(currentReservoirTemperature) + "\","

  "\"fridge_status\":";

  if(FridgeOn)
  {
    json_Packet += "\"On\"";
  }
  else
  {
    json_Packet += "\"Off\", ";
  }

  json_Packet += "\"override_status\":";

  if(OverRide)
  {
    json_Packet += "\"On\"";
  }
  else
  {
    json_Packet += "\"Off\"";
  }

  json_Packet += "}";

  oc_portal.send(200, "application/json;charset=utf-8", json_Packet);
}

void setupWebPortal()
{
  //oc_portal
  oc_portal.on("/", HTTP_GET, showSimpleWebPortal);
  oc_portal.on("/stats", HTTP_GET, sendStatsData);

  oc_portal.begin();
}
#endif HAS_WIFI

 void setEnvironmentAlert()
{

  //TODO:  Define a config flag for whether or not to send signal to laser to pause on alert(s)
  // Pin defined as SAFETY_CUTOFF_SIGNAL_PIN (digital pin 6)
    
  if(currentReservoirTemperature > MAX_TEMPERATURE_ALERT)
  {
    tone(BUZZER_PIN, 1000);
    delay(1000);
    noTone(BUZZER_PIN);
    delay(500);
  }
}

/*

  In the very near future this function is going to be rewritten with debouncing/idle time in mind, also
  instead of taking and acting on every temperature reading, it will be an average over x readings, so that
  the noise will be filtered out better.
*/

void setFridgeStatus()
{
/*
 * #define TARGET_TEMPERATURE_MIN_ABOVE_DEWPOINT 1
#define TARGET_TEMPERATURE_MAX_ABOVE_DEWPOINT 5
 */

  int override_openchill = digitalRead(OVERRIDE_PIN);

  if(override_openchill == LOW)
  {
      digitalWrite(FRIDGE_RELAY_PIN, HIGH);
      FridgeOn = true;
      OverRide = true;
      return;
  }
  else
  {
      FridgeOn = false;
      OverRide = false;
  }

  /*
   * Check current temperature is between min and max target, if its between the range, fridge goes off if its on..

    if its between the range and fridge is off, leave it off

    if its below the range, definitely off.

    if its above range, definitely on
   */

   if(currentReservoirTemperature < -9998)
   {
    #ifdef DEBUG
        Serial.println("Waiting for first water temperature sample before calculating fridge conditions ..");
    #endif

     return;
   }

  float min_target_temperature = currentEnvironmentDewPoint + TARGET_TEMPERATURE_MIN_ABOVE_DEWPOINT;  //  eg  dp=11, min above=1, min temp = 12
  float max_target_temperature = currentEnvironmentDewPoint + TARGET_TEMPERATURE_MAX_ABOVE_DEWPOINT;  //  eg  dp=11, max above=5, max temp = 16
  
  // check if current temperature is above min, but below max..
 if((currentReservoirTemperature >= min_target_temperature) && (currentReservoirTemperature <= max_target_temperature))
 {
   //if its in range, fridge can be turned off if its on
   if(FridgeOn)
   {
    
    #ifndef PRODUCTION
          Serial.println("Temperature in target range, fridge is on, so switching it off ..");
    #endif
          //fridge off
          digitalWrite(FRIDGE_RELAY_PIN, LOW);
          FridgeOn = false;
       }
       
       return;
     }
     else if(currentReservoirTemperature > max_target_temperature)
     {
    
    #ifdef DEBUG
        Serial.println("Temperature is above maximum target, switching fridge on ..");
    #endif
    
        //fridge on
        digitalWrite(FRIDGE_RELAY_PIN, HIGH);
        FridgeOn = true;
     }
     else if(currentReservoirTemperature < min_target_temperature)
     {

    #ifdef DEBUG
        Serial.println("Temperature is below minimum target, switching fridge off ..");
    #endif
    
        //fridge off
        digitalWrite(FRIDGE_RELAY_PIN, LOW);
        FridgeOn = false;
     }
     else
     {

      #ifdef DEBUG
        Serial.println("Condition met, but unknown yet why.");
      #endif
        //do nothing (yet) 
     }
}

void readReservoirProbeTemperature()
{
  currentReservoirVoltage = analogRead(RESERVOIR_TEMP_PROBE_PIN);

/*
 variables to control rolling log of measurements for averaging 
float reservoir_temperature_log[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
int cur_log_value = -1;
*/
  cur_log_value++;
  
  #ifdef MKS_BASE_1_0 //MKS_BASE_1_4 || MKS_BASE_1_0

    reservoir_temperature_log[cur_log_value] = map(currentReservoirVoltage, 1023, 0, -50, 50) - TEMP_CALIB_DIFF;
//    reservoir_temperature_log[cur_log_value] = map(currentReservoirVoltage, 0, 512, 10, 50); // - TEMP_CALIB_DIFF;
 //   reservoir_temperature_log[cur_log_value] = map(currentReservoirVoltage, 1023, 0, 0, 50);// - TEMP_CALIB_DIFF;

  #else
  
	  float R1 = 10000;
	  float logR2, R2, T;
	  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

	  R2 = R1 * (1023.0 / (float)currentReservoirVoltage - 1.0);
	  logR2 = log(R2);
	  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
	  T = T - 273.15;
	  T = (T * 9.0)/ 5.0 + 32.0; 

	  /* set Current Reservoir Temperature to current temperature in Celsius */
	  //currentReservoirTemperature = T;

    reservoir_temperature_log[cur_log_value] = T;

  #endif

  float total_for_avg = 0;

  if(cur_log_value == WATER_TEMP_TOTAL_SAMPLES)
  {
    for(int x=0; x< WATER_TEMP_TOTAL_SAMPLES; x++)
    {
        total_for_avg += reservoir_temperature_log[x];
    }

    currentReservoirTemperature = total_for_avg / WATER_TEMP_TOTAL_SAMPLES;

    //#ifdef MKS_BASE_1_0
    //  currentReservoirTemperature = currentReservoirTemperature / 2;
    //#endif

    cur_log_value = -1;
  }

}

void readEnvironment()
{
//  int chk = DHT.read11(DHT11_PIN);
  dht.read();
  currentEnvironmentHumidity = dht.readHumidity();        //  read humitity as %f
  currentEnvironmentTemperature = dht.readTemperature();     //  read temperature as Celsius (default)
  currentEnvironmentDewPoint = (currentEnvironmentTemperature - (100 - currentEnvironmentHumidity) / 5);
}

void debug_Out(bool fridge_test_mode)
{
  Serial.print("Total Samples: ");
  Serial.print(cur_log_value);
  Serial.print(" | Temp: ");
  Serial.print(currentEnvironmentTemperature);
  Serial.print(" | Humid: ");
  Serial.print(currentEnvironmentHumidity);
  Serial.print(" | Dew Point: ");
  Serial.print(currentEnvironmentDewPoint);
  Serial.print(" | Reservoir Temp: ");
  Serial.print(currentReservoirTemperature);
  Serial.print(" | Fridge Status: ");

  if(fridge_test_mode)
  {
    Serial.print(FridgeOn);
    Serial.println(" [Fridge Circuit Debug Active]");
  }
  else
  {
    Serial.println(FridgeOn);
  }
  
}

void serialMon_Update()
{
  Serial.print("|T:");
  Serial.print(currentEnvironmentTemperature);
  Serial.print(",H:");
  Serial.print(currentEnvironmentHumidity);
  Serial.print(",D:");
  Serial.print(currentEnvironmentDewPoint);
  Serial.print(",R:");
  Serial.print(currentReservoirTemperature);
  Serial.print(",F:");
  Serial.print(FridgeOn);
  Serial.print(",");
  Serial.println("^");

  // Output example:  |T:16.00,H:63.00,DP:5.00,RT:11.75,FR:0 or 1 (fridge compressor circuit on or off)^
  
}

void fridge_relay_debug()
{

  if(FridgeOn)
  {
    digitalWrite(FRIDGE_RELAY_PIN, LOW);
    FridgeOn = false;
  }
  else
  {
    digitalWrite(FRIDGE_RELAY_PIN, HIGH);
    FridgeOn = true;
  }

  delay(1500);
}

void setup_lcd()
{

  #ifdef HAS_LCD

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OpenChill System v2");

  lcd.setCursor(0, 1);
  lcd.print("Env:");

  //update environment temperature
  lcd.setCursor(5,1);
  lcd.print("??");
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(10, 1);
  lcd.print("Hum:");

  //update humidity
  lcd.setCursor(15, 1);
  lcd.print("??%");
  
  lcd.setCursor(0,2);
  lcd.print("DPt:");

  //update dewpoint
  lcd.setCursor(5,2);
  lcd.print("??");
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(10,2);

  lcd.print("Wtr:");

  //update water/reservior temperature
  lcd.setCursor(15,2);
  lcd.print("??");
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0,3);
  lcd.print("Chiller: ");

  //update fridge/chiller status
  lcd.setCursor(9,3);
  lcd.print("??");

  lcd.setCursor(13,3);
  lcd.print("      ");

  #endif
}

void update_lcd()
{

  #ifdef HAS_LCD

  lcd.setCursor(5,1);

  if(!isnan(currentEnvironmentTemperature))
  {
    lcd.print(currentEnvironmentTemperature,0);
  }
  else
  {
    lcd.print("!?");
  }

  lcd.print((char)223);
  lcd.print("C");

  //update humidity
  lcd.setCursor(15, 1);

  if(!isnan(currentEnvironmentHumidity))
  {
    lcd.print(currentEnvironmentHumidity,0);
  }
  else
  {
    lcd.print("!?");
  }

  lcd.print("%");
  
  //update dewpoint
  lcd.setCursor(5,2);

  if(!isnan(currentEnvironmentDewPoint))
  {
   lcd.print(currentEnvironmentDewPoint,0);
  }
  else
  {
    lcd.print("!?");
  }
  
  lcd.print((char)223);
  lcd.print("C");

  //update water/reservior temperature
  lcd.setCursor(15,2);

  if(currentReservoirTemperature > -9999)
  {
    lcd.print(currentReservoirTemperature,0);
    lcd.print((char)223);
    lcd.print("C");
  }
  else
  {
    lcd.print("**");
    lcd.print((char)223);
    lcd.print("C");
     
  }

  lcd.setCursor(9,3);
  lcd.print(FridgeOn);

  if(OverRide == true)
  {
    lcd.setCursor(13,3);
    lcd.print("[lock]");
  }
  else
  {
    lcd.setCursor(13,3);
    lcd.print("      ");
  }

/*
  Serial.print("|T:");
  Serial.print(currentEnvironmentTemperature);
  Serial.print(",H:");
  Serial.print(currentEnvironmentHumidity);
  Serial.print(",D:");
  Serial.print(currentEnvironmentDewPoint);
  Serial.print(",R:");
  Serial.print(currentReservoirTemperature);
  Serial.print(",F:");
  Serial.print(FridgeOn);
  Serial.print(",");
  Serial.println("^");
  */

  #endif

}

/* Control Board Heartbeat */
void loop () 
{
//  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
//  lcd.print(millis() / 1000);
  
  readEnvironment();   /* Read Environment */
  readReservoirProbeTemperature(); /* Read Reservoir Temperature */

#ifndef FRIDGECIRCUITDEBUG
  setFridgeStatus(); /* Set Fridge Status on or off based on above data */
#endif

  setEnvironmentAlert(); /* Create any alerts based on environment data, such as max chiller temp reached */

#ifdef DEBUG
  debug_Out(false);
#endif

#ifdef FRIDGECIRCUITDEBUG
  fridge_relay_debug();
  debug_Out(true);
#endif

#ifdef SERIALMODE
  serialMon_Update();
#endif

#ifdef PRODUCTION
#endif

#ifdef HAS_LCD
  update_lcd();
#endif

#ifdef HAS_WIFI
  oc_portal.handleClient();
#endif

  delay(250);

}
