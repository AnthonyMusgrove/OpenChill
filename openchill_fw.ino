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

#include <LibPrintf.h>
#include <LiquidCrystal.h>

#define MKS_BASE_1_4



/*  OpenChill Hardware V1 CPU MAP */
#ifdef OPENCHILL_HARDWARE_V1
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

#endif

/*  MKS Base v1.4 CPU MAP */
#ifdef MKS_BASE_1_4
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

#endif

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// #define DEBUG
// OPTIONS:   DEBUG, DEBUGSERIAL, FRIDGECIRCUITDEBUG, SERIALMODE, PRODUCTION (production doesn't do anything yet)
#define DEBUG

 
 /* This value was used arbitrarily as my temperature probe on the MKS 1.4 board was always 8 degrees celsius higher than it should be.   
  * 
  * Calibration of the temperature probe using the MKS 1.4 board will be trial and error until I get some type of software calibration
  * working properly.  If your water temperature probe isn't accurate ( DO NOT SOLELY RELY ON THE MKS 4 TEMPERATURE PROBE WITHOUT CALIBRATING ),
  * submit a ticket so I can get some feedback on the best way to move forward with calibration.
 */
#define TEMP_CALIB_DIFF 8

#define DEGREES_ABOVE_DEWPOINT 1  /* OpenChill will aim to maintain a chiller temperature of x degrees ABOVE dewpoint */
#define MAX_TEMPERATURE_ALERT 26.5 /* Currently, OpenChill will emit an audible beep when chiller temperature exceeds this value */


/*  Instead of a single degree value for target above dewpoint, lets introduce a range.  For a few reasons - to stop the
 *   need for cutting in and out when the temperature fluctuates even by error, without having to introduce new code to
 *   detect and ignore (yet).  -anthony@labworx.au
*/

#define TARGET_TEMPERATURE_MIN_ABOVE_DEWPOINT 1
#define TARGET_TEMPERATURE_MAX_ABOVE_DEWPOINT 3



//#define DHTPIN 2

#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

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


/* Setup Control Board */
void setup() {
  Serial.begin(115200);  /* Baud Rate MUST match Baud Rate of onboard ESP chip, tip:  Leave both set to 115200 */
  pinMode(BUZZER_PIN, OUTPUT); /* Define Buzzer Pin as an output */
  pinMode(FRIDGE_RELAY_PIN, OUTPUT); /* Define Fridge Relay Pin as an output */
  digitalWrite(FRIDGE_RELAY_PIN, LOW); /* Make sure initially we have the Fridge relay off */
  FridgeOn = false; /* Keep track of fridge circuit status, we've switched it off */

  pinMode(SAFETY_CUTOFF_SIGNAL_PIN, OUTPUT); /* OUTPUT Signal wire for OpenChill to direct laser to cease operating  */
  pinMode(LASER_IN_USE_PIN, INPUT); /* INPUT Signal wire from laser to advise OpenChill that laser is currently in use */

  lcd.begin(20, 4);
//  lcd.print("OpenChill.");
  dht.begin();

  setup_lcd();

}

 

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
  
  #ifdef MKS_BASE_1_4

    reservoir_temperature_log[cur_log_value] = map(currentReservoirVoltage, 1023, 0, -50, 50) - TEMP_CALIB_DIFF;

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


}

void update_lcd()
{

  //update environment temperature
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


#ifdef DEBUGSERIAL
  tmp_increment_counter();
#endif

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

  update_lcd();

  delay(250);

}
