/*  OpenChill V3.0 Firmware 

    Required Hardware
    =================


    1 x  ESP32 Dev Module (or the official PCB & Bill of Materials [still being developed, use an ESP32 Dev Module for simplicity])

    1 x  OLED Display; this firmware uses the SSD1306  128x64 OLED display, they're plentiful on ebay.  Its the 4-wire OLED screen (SCL,SCA,3.3V,GND)

    1 x Solid State Relay that can switch on/off with minimum 3VDC (eg, the GEYA Solid State Relay Module SSR-10A) which can switch on and off using 3-32 VDC, and can switch 24-480 VAC
        (I'll list this item in the documentation on GITHUB, you can select any relay provided that it has the same electrical properties)

    1 x DHT11 Module (Temperature & Humidity Sensor)

    1 x Temperature Sensor Module (I'm using a Jaycar XC4494, but any analog thermistor sensor which changes resistance with temperature)

    1 x Potentiometer (and knob, because can be used to calibrate the water reservior temperature, eventually this will be completely digital)

    1 x Vero board (just something to attach connectors to that can be cut to fit into the slots in the 3d printed case, this is for the plugin sockets etc)

    Some sockets / plugs (TBA after 3d printed case is completed)

    1 x full 3d printed OpenChill V3.0 case

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {

  Serial.begin(9600);
  delay(500);
  while (!Serial);

  Serial.println("OpenChill V3.0 DBG Serial");

  Serial.println("Initialising OLED Display ...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println(F("OpenChill V3.0"));

  display.display();

}

void loop() {

}
