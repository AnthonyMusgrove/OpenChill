# OpenChill
OpenChill is an open source laser chiller firmware and hardware project using readily available parts from JayCar (Australia)

## Why OpenChill?
<p>A Co2 laser tube requires water cooling to maintain power, longevity and safety of the equipment.  The issue is, if the water can't be kept cool enough, the laser starts to lose power and over time, degrades and becomes unusable quicker.</p>  
<p>Why can't we just pump icy cold water through it then using a fridge or freezer?  Well, great question, it turns out that condensation buildup on and inside the laser tube causes 'arcing' and 'shorting' which can be harmful or fatal to YOU, the operator, and your laser.</p><p>  This is where OpenChill comes into play.</p>
<br/>
<p>OpenChill monitors the room's Humidity and Temperature using a sensor connected to the control board.  It then calculates the room's "dew point", which is the temperature in which the tubing will begin to generate condensation. </p><p> The firmware allows you to configure various settings (via wifi/web browser, and eventually a windows app), such as 'Set temperature of chiller dynamic to 3 degrees celsius ABOVE dew point', etcetera. <p></p> It will allow for remote monitoring and configuration, and eventually a touch screen, mounts, and more. It will also incorporate the K40 laser 'Cooler Warning' signal wire to shut the laser down incase certain conditions are met.</p>
<br/>
<p>The sky is truly the limit with OpenChill and I can't wait to see the progress over time of this awesome project.</p>
<br/>
<p>Images, instructions, firmware, etc all in progress of ALPHA, once ready for initial commit we can begin group collaboration.</p>

## Parts List

| Jaycar Code | Description | QTY Needed |
| ----------- | ----------- | ---------- |
| XC4419 | RELAY BOARD MODULE (5V) ARDUINO   | 1 |
|XC4520 |  TEMPERATURE HUMIDITY SENSOR MODULE ARDUINO | 1 |
|XC4411 |  Arduino Uno With WIFI (includes an onboard ESP8266) | 1 |
|SY4040 |  12VDC 30A@250VAC SPST Relay | 1 |
|XC4514 |  Arduino Compatible DC Voltage Regulator | 1 |
|PT4626 |  SPADE Sockets 6.8mm Insulated (for 240v relay connections) | 1 pack |


## Non standard bits and pieces

| A water cooler with a working condensor/fridge mechanism.  We will be removing the thermostat that it has and installing OpenChill. |
| A BBQ temperature probe, something waterproof, metal casing preferred that will sit in the water cooler reservoir to monitor the temperature of the chilled water |


## Downloads 
OpenChill Uno Firmware
OpenChill ESP Firmware
OpenChill Windows Monitoring Tool
