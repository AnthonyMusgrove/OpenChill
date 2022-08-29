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

#include <LibPrintf.h>

// #define DEBUG
// OPTIONS:   DEBUG, FRIDGECIRCUITDEBUG, SERIALMODE, PRODUCTION (production doesn't do anything yet)
#define DEBUG

 
#define DEGREES_ABOVE_DEWPOINT 1  /* OpenChill will aim to maintain a chiller temperature of x degrees ABOVE dewpoint */
#define MAX_TEMPERATURE_ALERT 26.5 /* Currently, OpenChill will emit an audible beep when chiller temperature exceeds this value */

/*
 * PIN Configuration
 */
 
#define DHT11_PIN 9   /* Arduino Digital Pin for the Signal Wire of the DHT11 Temperature & Humidity Sensor */
#define RESERVOIR_TEMP_PROBE_PIN A3  /* Arduino Analog Pin for the Chiller Temperature Probe Circuit Reading */
#define FRIDGE_RELAY_PIN 8 /* Arduino Digital Pin for the Fridge Relay Circuit */
#define BUZZER_PIN 10 /* Arduino Digital PWM Piezzo/Buzzer Pin */

#define DEBUG_RELAY_PIN 3 /* Debug Relay Pin for Firmware ALPHA, not used in O/S release. */

#include <dht.h>
dht DHT;

/* variables to store the current DHT11 Temperature & Humidity reading, along with calculated Dew Point */
float currentEnvironmentTemperature;
float currentEnvironmentHumidity;
float currentEnvironmentDewPoint;

/* variables to store Chiller Reservoir Temperature & Analog Read value */
float currentReservoirTemperature;
int currentReservoirVoltage;

/* Keep track of Fridge circuit Status */
bool FridgeOn = false;

/* variables to store communication parameters  for 
 * communication between onboard Control Chip & ESP Wifi Chip 
 */
String commsRecieved = "";
bool commsIn = false;

/* Setup Control Board */
void setup() {
  Serial.begin(115200);  /* Baud Rate MUST match Baud Rate of onboard ESP chip, tip:  Leave both set to 115200 */
  pinMode(BUZZER_PIN, OUTPUT); /* Define Buzzer Pin as an output */
  pinMode(FRIDGE_RELAY_PIN, OUTPUT); /* Define Fridge Relay Pin as an output */
  digitalWrite(FRIDGE_RELAY_PIN, LOW); /* Make sure initially we have the Fridge relay off */
  FridgeOn = false; /* Keep track of fridge circuit status, we've switched it off */

  pinMode(DEBUG_RELAY_PIN, OUTPUT); /* Debug Relay Pin for Firmware ALPHA, not used in O/S release */
}

/*
 *   checkCommsIn() checks for communications pending between ESP wifi chip and main control chip
 *   
 *   OpenChill Internal ESP<-->ControlChip Protocol Definition
 *   
 *   get|all   -   return all environment information, including fridge circuit status
 *   get|x     -   where x is definition of data item, eg, get|fridge_status, get|humidity, get|chiller_temperature
 *   
 *   set|var|val
 *   
 *   Frame Format
 *   
 *   STARTBIT | ADDRESS       | FUNCTION    | TXID                  | DATA        | ENDBIT
 *   0x02     | 8-bit target  | 8 Bit Cmd   | 8-bit Transaction ID  | n x8 Bits   | 0x03
 *   
 */

#define THIS_CHIP_ADDR 0x031 /* address = 1 */
#define COMMS_STARTBIT 0x02 /* StartBit = SOT */
#define COMMS_ENDBIT 0x03 /* EndBit = EOT */

#define REG_ROOM_TEMPERATURE      0x20
#define REG_ROOM_HUMIDITY         0x21
#define REG_ROOM_DEWPOINT         0x22
#define REG_RESERVOIR_TEMPERATURE 0x23
#define REG_FRIDGE_STATUS         0x24


void checkCommsIn()
{
    while (Serial.available())
    {
      char c = Serial.read(); //read it

      if(c == COMMS_STARTBIT)  /* start of frame */
      {
//        Serial.println("Got Start bit!");
        /* we've recieved a valid start bit, clear any current frame/buffer and start from beginning */
        commsRecieved = "";
        commsIn = true;
      }
      else if(commsIn && c != COMMS_ENDBIT) // not start of frame, not end of frame, part of frame
      {
//        Serial.println("Got Some Data!");
        commsRecieved += c;
      }
      else if(commsIn && c == COMMS_ENDBIT) // end of frame, ready to parse frame
      {
//        Serial.println("Got End Bit and valid frame!");
        processCommsFrame(String(commsRecieved));
        commsRecieved = "";
        commsIn = false;
      }

      delay(70);
    }
}

#define CMD_READ 0x05  /* Read/request data */
#define CMD_WRITE 0x06 /* Write/request write data */
#define CMD_CONTROL 0x07 /* Control functions, ie, Switch on/off pump, fridge, etc */

/* process frame incoming */

/* Incoming frame is the entire frame MINUS the StartBit and EndBit 

 *   STARTBIT |[ ADDRESS       | FUNCTION    | TXID           | DATA       ]| ENDBIT
 *   0x02     |[ 8-bit target  | 8 Bit Cmd   | 16 Bit TX ID    | n x8 Bits   ]| 0x03
 *   
 *   
 *   ( High Byte x 256 ) + Low Byte = 16 Bit Value

( 10 * 256 ) + 196 = 2,756
 *   

*/
void processCommsFrame(String commsDataFrameIn)
{

  char deviceAddress = commsDataFrameIn.charAt(0);

  if(deviceAddress != THIS_CHIP_ADDR && deviceAddress != 0x30) // datapacket not for me, because address isn't my unique address or broadcast address of 0, drop & don't complain!
    return;

  char Cmd = commsDataFrameIn.charAt(1);
  int TxId_HIGH = commsDataFrameIn.charAt(2);
  int TxId_LOW = commsDataFrameIn.charAt(3);
  int TxId = 100;

//  Serial.print("COMMAND RECEIVED: ");
//  Serial.println(Cmd, HEX);

  switch(Cmd)
  {
      case 5:
        // Data part is from 2 to end of frame.
//        Serial.println("READ COMMAND RECEIVED!");
        cmd_read(TxId, commsDataFrameIn.substring(4));
      break;

      case CMD_WRITE:
        cmd_write(TxId, commsDataFrameIn.substring(4));
      break;

      case CMD_CONTROL:
        cmd_control(TxId, commsDataFrameIn.substring(4));
      break;

      default:
        //Serial.println("INVALID CMD DETECTED!");
      /* Invalid command specified, simply drop and ignore, do not complain! */
      return;
      break;
  }


}

#define READ_STATS 0x31
#define READ_SINGLE 0x32
#define READ_MULTI 0x33

void cmd_read(long TransactionId, String data)
{
  /* READ DATA FORMAT:

      Byte 1    Read Type,      <1 (0x31)> Statistics Poll     
                                <2 (0x32)> Single Value    <Single Value ID>
                                <3 (0x33)> Multi Value     <Multi Value ID, Multi Value ID, Multi Value ID>
      
  */

//  Serial.print("READ IN, Char at 0 = |");
//  Serial.print(data.charAt(0));
//  Serial.println("|");

  if(data.charAt(0) == 1)
  {
    // statistics poll
    reply_statistics_poll(TransactionId);
    return;
  }
  
  
  //READ DATA REQUESTED.... DATA given: 234567
  //Serial.write("READ DATA REQUESTED.... DATA given: ");
  //Serial.println(data);
}

void reply_statistics_poll(long TransactionId)
{
  // Format response:   STARTBIT [TRANSACTIONID] <ID> <VALUE> <SEPARATOR> <ID> <VALUE> <SEPARATOR> <ID> <VALUE> <SEPARATOR>

    int roomTempEnc = currentEnvironmentTemperature * 100;
    int roomHumidEnc = currentEnvironmentHumidity * 100;
    int roomDewPointEnc = currentEnvironmentDewPoint * 100;
    int resvTempEnc = currentReservoirTemperature * 100;

  // RESPONSE FRAME FORMAT:   STARTBIT [TRANSACTIONID] <ID> <VALUE> <SEPARATOR> <ID> <VALUE> <SEPARATOR> <ID> <VALUE> <SEPARATOR>

//    STARTBIT  TRANSACTIONID CMD SUBCMD DATA  ENDBIT

    printf("%c%02x%02x%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x%c", COMMS_STARTBIT, CMD_READ, READ_STATS, TransactionId, REG_ROOM_TEMPERATURE, roomTempEnc, REG_ROOM_HUMIDITY, roomHumidEnc, REG_ROOM_DEWPOINT, roomDewPointEnc, REG_RESERVOIR_TEMPERATURE, resvTempEnc, REG_FRIDGE_STATUS, FridgeOn, COMMS_ENDBIT);
}

void cmd_write(char TransactionId, String data)
{
  
}

void cmd_control(char TransactionId, String data)
{
  
}

void cmd_set_debug_relay(String Flag)
{
  
}
 

void setEnvironmentAlert()
{
  if(currentReservoirTemperature > MAX_TEMPERATURE_ALERT)
  {
    tone(BUZZER_PIN, 1000);
    delay(1000);
    noTone(BUZZER_PIN);
    delay(500);
  }
}

void setFridgeStatus()
{
  if(currentReservoirTemperature <= (currentEnvironmentDewPoint + DEGREES_ABOVE_DEWPOINT))
  {
    //fridge off.
    digitalWrite(FRIDGE_RELAY_PIN, LOW);
    FridgeOn = false;
  }
  else
  {
    //fridge on
    digitalWrite(FRIDGE_RELAY_PIN, HIGH);
    FridgeOn = true;
  }
}

void readReservoirProbeTemperature()
{
  currentReservoirVoltage = analogRead(RESERVOIR_TEMP_PROBE_PIN);

  float R1 = 10000;
  float logR2, R2, T;
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

  R2 = R1 * (1023.0 / (float)currentReservoirVoltage - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0; 

  /* set Current Reservoir Temperature to current temperature in Celsius */
  currentReservoirTemperature = T;
}

void readEnvironment()
{
  int chk = DHT.read11(DHT11_PIN);
  currentEnvironmentHumidity = DHT.humidity;        //  read humitity as %f
  currentEnvironmentTemperature = DHT.temperature;     //  read temperature as Celsius (default)
  currentEnvironmentDewPoint = (currentEnvironmentTemperature - (100 - currentEnvironmentHumidity) / 5);
}

void debug_Out(bool fridge_test_mode)
{
  Serial.print("Temp: ");
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
  Serial.print(",DP:");
  Serial.print(currentEnvironmentDewPoint);
  Serial.print(",RT:");
  Serial.print(currentReservoirTemperature);
  Serial.print(",FR:");
  Serial.print(FridgeOn);
  Serial.print("^");

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

/* Control Board Heartbeat */
void loop () 
{

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


  checkCommsIn(); /* Check to see if ESP is trying to query or write values to/from control chip */
  
  delay(250);
}
