[![DOI](https://zenodo.org/badge/123350026.svg)](https://zenodo.org/badge/latestdoi/123350026)

# Project-Margay

![Margay Andy hand Dec 2020](Documentation/images/MargayAndyHand_cropped_2020-02-16_19.22.39.png)

***Margay data logger v2.2.*** *Back side with full-size SD card holder and pinout labels. Standard sized SD card for data storage and scale.*

Project Margay is a micro scale environmental data logger designed based on the ALog series, it is designed to trade IO capabilities for cost and size, allowing for a very simple, but very useful data logger

## Namesake

Due to its small size and adaptability to its environment, Project Margay owes its name to the small Margay cat native to the forests of Central and South America.

![A margay, yawning](https://upload.wikimedia.org/wikipedia/commons/4/45/Leopardus_wiedii%2C_R%C3%A9serve_Zoologique_de_Calviac.jpg)

***A yawning margay cat***

## Technical Specifications

### Electronic Hardware
This includes a description of the PCB and component functionality

#### Features:
* ATMega1284p Processor
* 3.3v Logic
* On board full size SD card (for ease of field use)
* Extremely low sleep current
* Input voltage designed for use with easy to find alkaline batteries
* 0.1" Pitch headers can be populated with header pins and placed on a breadboard for prototyping

### Current iteration:

![Margay v2.2 interior elements labeled](Documentation/images/Margay_v220_top_annotated_interior_20200428.png)

***Significant components on the Margay v2.2.***

#### v2.2
* Microcontroller (computer) core
  * ATMega1284p
  * Arduino compatible
  * programmable using C++
  * 8 MHz
* Real-time clock
  * DS3231M
  * Temperature-compensated crystal for high accuracy (±5 ppm)
* Sensor and general-purpose connections
  * Combined digital humidity and pressure sensor
    * BME280
* Data storage and return
  * Local storage: SD card
* Power
  * Coin cell battery - CR1220, 3V
  * Power consumption
    * I<sub>Q</sub> = <2.5&mu;A (Estimated)
    * I<sub>out</sub> = 200mA, max (Regulated power supplied to sensors)
    * V<sub>in</sub> = 3.3 ~ 5.5v (Reverse polarity protected)
    <!-- Absolute max voltage 6.0V; we recommend not exceeding 5.5V -->
* User interface
  * "Log" button
  * "Reset" button
  * Auxiliary LED
  * RGB Status LED

**IO**
* 1 ADC, 18 bit
* 1 I<sup>2</sup>C Bus
* 1 UART Channel
* 2 GPIO Pins (one configurable as an 8 bit ADC)
* 1 PWM Channel (output configurable to 3.3v or VBat via a jumper on bottom of board)
* RGB Status LED
* Auxiliary LED
* Log Button
* Reset Button

### Past Iterations:

#### Features:
* ATMega644p Processor
* 3.3v Logic
* On board full size SD card (for ease of field use)
* Extremely low sleep current
* Input voltage designed for use with easy to find alkaline batteries
* 0.1" Pitch headers can be populated with header pins and placed on a breadboard for prototyping

#### v0.0 (Retired)
**Features** <br>
I<sub>Q</sub> = 1.6mA <br>
I<sub>out</sub> = 50mA, max (Regulated power supplied to sensors) <br>
V<sub>in</sub> = 3.3 ~ 5.5v <br>

**IO**:
* 1 I2C Bus
* 1 SPI Bus, with up to 2 CS pins
* 1 PWM Channel (output configurable to 3.3v or VBat via a jumper on bottom of board)
* RGB Status LED
* Auxiliary LED
* Reset Button
* Reconfigurable Button

#### v1.0
**Features** <br>
I<sub>Q</sub> = 2.5&mu;A <br>
I<sub>out</sub> = 50mA, max (Regulated power supplied to sensors) <br>
V<sub>in</sub> = 3.3 ~ 5.5v (Reverse polarity protected) <br>

**IO**
* 1 ADC, 18 bit
* 1 I<sup>2</sup>C Bus
* 1 SPI Bus, with up to 2 CS pins
* 1 PWM Channel (output configurable to 3.3v or VBat via a jumper on bottom of board)
* RGB Status LED
* Auxiliary LED
* Reset Button
* Reconfigurable Button

#### v2.0
**Features** <br>
I<sub>Q</sub> = <2.5&mu;A (Estimated) <br>
I<sub>out</sub> = 200mA, max (Regulated power supplied to sensors) <br>
V<sub>in</sub> = 3.3 ~ 5.5v (Reverse polarity protected) <br>

**IO**
* 1 ADC, 18 bit
* 1 I<sup>2</sup>C Bus
* 1 UART Channel
* 2 GPIO Pins (one configurable as an 8 bit ADC)
* 1 PWM Channel (output configurable to 3.3v or VBat via a jumper on bottom of board)
* RGB Status LED
* Auxiliary LED
* Reset Button
* Reconfigurable Button

### Electronic Software and Firmware

* Programmable using the Arduino IDE https://www.arduino.cc/en/main/software
* Custom bootloader and board definition for ATMega1284p available via https://github.com/NorthernWidget/Arduino_Boards
* Custom libraries from Northern Widget and the open-source community
  * Primary data-logger functions
  * Libraries available with each sensor, exposing a standard interface
* Open-source licensing via GNU GPL 3.0

### Pinout and board interfaces
Pinout is listed on bottom of board, and shown here for v2.2.

![Margay v2.2, top annotated](Documentation/images/Margay_v220_top_annotated_20200428.png "Margay v2.2, top annotated")

***Margay data logger with pins, connectors, and interactive items annotated, top side.***

**Pins are colored following this convention:**
* Ground: black
* Positive voltage: red
* Universal Serial Asynchronous Receiver--Transmitter (UART or USART) bus: blue
* Digital I/O: gray
* Analog measurement system: green
* I2C bus: purple

*The jumper on the front of the board, to the right of "B+" and "Ax", connects an on-board reference resistor (10 kOhm in our builds) with the analog pin Ax to create a voltage divider in which the sensor between Ax and REF is on the GND side of the voltage divider.*

![Margay v2.2, bottom annotated](Documentation/images/Margay_v220_bottom_annotated_20200428.png "Margay v2.2, bottom annotated")

***Margay data logger annotated, bottom side.*** This is where the labels for the above pins are printed.

In this pinout, the name of each pin is shown, as well as the group of pins which it belongs to, a detailed description of the pins and callouts follows:

#### Pins:
* 3v3, the switched 3.3v output rail, this rail can be turned on and off to disconnect power consumptive external devices
* GND, the main output ground
* BAT+, the positive connection for the battery line, voltage range 3.3v ~ 5.5v
* BAT-, Negative battery connection, ***note: this pin is NOT interchangeable with GND, as BAT- is reverse voltage protected to prevent damage from plugging the battery in backwards***
* Vs, this is the switched PWM output, the voltage of this output is determined by a solder jumper on the bottom of the board, controlled by **D3**

* MOSI, this is the master out, slave in, pin for the SPI bus, which doubles as **D5** when SPI and SD card are not used
* MISO, this is the master in, slave out, pin for the SPI bus, which doubles as **D6** when SPI and SD card are not used
* SCK, this is the clock line for the SPI bus, which doubles as **D7** when SPI and SD card are not used
* CS, this is the chip select pin for the SPI bus, which doubles as both **D11** and **INT1** when external SPI is not used (Can be used even when SD card is used)

* SCL, this is the dedicated serial clock line for the I2C bus
* SDA, this is the dedicated serial data line for the I2C bus

#### Callouts:
1. This is the auxiliary LED, which indicate TX and RX status using red and blue channels, green channel is controlled by **D19**
2. This is the micro USB connection, which can be used for both programing via the Arduino IDE, and serial monitoring
3. This is the primary RGB status LED, where the individual channels are controlled by **D15**, **D14**, and **D13**
4. This is the reconfigurable push-button, generally used to initiate logging, it is read by **D2**, which is also **INT2**
5. This is the hardware reset button, which will force the micro to return to the initial state after code upload
6. This is the ICSP header, which can be used to burn the bootloader to the board or write programs to the chip without using the USB and bootloader

#### Onboard Pins:
Pin Name | Pin Number (v0.0) | Pin Number (v1.0) | Function
-------- | ----------------- | ----------------- | --------
`SD_CS` | D4 | D4 | Chip select pin on SD card
`SD_CD` | D1 | D1 | Card detection pin for SD card, active low, AUX power must be on to use
`BuiltInLED` | D19 | D20 | Connected to green channel on **AUX** LED, on when pin is low
`RedLED` | D13 | D13 | Connected to red channel on **STAT** LED, on when pin is low
`GreenLED` | D15 | D15 | Connected to green channel on **STAT** LED, on when pin is low
`BlueLED` | D14 | D14 | Connected to blue channel on **STAT** LED, on when pin is low
`VRef_Pin` | A2 | A2 | Analog pin to read 1.8v on board reference, AUX power must be on to use
`ThermSensePin` | A1 | A1 | Analog pin to read thermistor voltage divider, AUX power must be on to use
`BatSensePin` | A0 | A0 | Analog pin to read battery voltage divider, AUX power must be on to use
`VSwitch_Pin` | D3 | D3 | Connected to MOSFET driver for **Vs** pin, active low
`Ext3v3Ctrl` | D12 | D19 | Turns on AUX power, active low
`I2C_SW` | N/A | D12 | Switches between on board I2C and external I2C
`PG` | D18 | D18 | Power good pin from core 3v3, can be used to test if core 3v3 is stable, pulled low when power is not stable
`ExtInt` | D11 | D11 | External interrupt and external chip select pin
`RTCInt` | D10 | D10 | Interrupt (`INT0`) connected to RTC /INT line, active low
`LogInt` | D2 | D2 | Interrupt (`INT2`) connected to **LOG** button, active low


## On-board Devices

### I2C

| **Device**                                                	| **Default address** 	| **Reprogrammable address** 	| **Function**                                                     	|
|-----------------------------------------------------------	|---------------------	|----------------------------	|------------------------------------------------------------------	|
| [DS3231](https://github.com/NorthernWidget/DS3231_Logger) 	| 0x68                	| N                          	| Real-time clock                                                  	|
| [MCP3421](https://github.com/NorthernWidget/MCP3421)      	| 0x6A                	| N                          	| ADC                                                              	|
| [BME280](https://github.com/NorthernWidget/BME_Library)   	| 0x77                	| N                          	| On-board barometric pressure, temperature, and relative humidity 	|

### SPI

* SD Card

### UART (USART)

* FTDI USB-Serial converter

## Assembly

Assembling this data logger is possible by hand with sufficient skill and the following tools:
* Temperature-controlled soldering iron
* Hot-air rework station
* Equipment for stenciling with solder paste
* ESD-safe tweezers and workstation
* Solder wick

Mechanized assembly by a professional circuit-board assembly house, which is available in many parts of the world, may be preferred due to the complexity of this data logger board.

## Programming

### Downloading and installing the Arduino IDE

Go to https://www.arduino.cc/en/main/software. Choose the proper IDE version for your computer. For Windows, we suggest the non-app version to have more control over Arduino; this might change in the future. You will have to add custom libraries, so the web version will not work (at least, as of the time of writing). Download and install the Arduino IDE. Open it to begin the next steps.

***Note: Users should see the code included below in the README and follow the directions indicated in http://northernwidget.com/tutorial. Some of the below information is deprecated.***

***Note: Currently, users should modify their Arduino libraries to include the most recent version 1 of SdFat; Margay is incompatible at present with version 2.***

### Installing the Bootloader

Before a data logger can receive programs via the convenient USB port, it must have a *bootloader* that tells it to expect to receive new programs that way.  You can read more about bootloaders in general here: https://www.arduino.cc/en/Hacking/Bootloader.

Because you can't upload the bootloader via USB, you use the 2x3-pin 6-pin ICSP (also called ISP) header with a special device called an "in-circuit system programmer" (or just "in-system programmer; yup, that's what the acronym stands for).

Many devices exist to upload a bootloader including:
* The official [AVR ISP mkII](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42093-AVR-ISP-mkII_UserGuide.pdf) (no longer produced but available used)
* Using an [Arduino as an ISP](https://www.arduino.cc/en/tutorial/arduinoISP)
* The versatile [Olimex AVR-ISP-MK2](https://www.olimex.com/Products/AVR/Programmers/AVR-ISP-MK2/open-source-hardware)
* The [Adafruit USBtinyISP](https://www.adafruit.com/product/46)

***Important note for Linux users:*** You must supply permissions to the Arduino IDE for it to be able to use the ICSP, or you will have to run it using `sudo`. The former option is better; the latter is easier in the moment.

To upload the bootloader, do the following:

1. Open the Arduino IDE. If you have not yet installed the Northern Widget board definitions, find and install them here (instructions provided): https://github.com/NorthernWidget/Arduino_Boards - the Margay board should be run using the "TLog v1" board definition.
2. Select the desired board -- most likely ***ATMega1284p 8MHz*** under *Northern Widget Boards*.
3. Plug the data logger into your computer via a micro-USB cable to provide power.
4. Plug your ISP of choice into your computer (via a USB cable) and onto the 6-pin header. There are two ways to place it on; the header is aligned such that the ribbon cable should be facing away from the board while programming. If this fails without being able to upload, try flipping the header around.
5. Go to Tools --> Programmer and select the appropriate programmer based on what you are using.
6. Go to Tools --> Burn bootloader. Typically, within a few seconds, you learn whether you succeeded or failed. Hopefully it worked!

***Note: Be sure to download and/or update drivers for your ISP.***

### Using Northern Widget Software
This includes a usage guide to demo software which is provided to both test the hardware features of the board in general (the MargayHardwareTest file), and to set the device up as a logger using a [TP-Downhole](https://github.com/NorthernWidget/TP-DownHole) device as a sensor which runs on I2C. This logger demo tests all hardware on the board and ensures all required systems are connected and indicates the result of these tests using the Status LED on startup. A green light indicated all systems check out, and the device is ready to log, otherwise a red light indicates there is a problem, it is recommended to open up a serial monitor to determine which system is failing. If a green light is indicated, when the system is ready to log, logging is initiated by pressing the log button, which makes an initial log and starts a sequence of logs which will continue to occur every 15 minutes (by default) or by a different user defined time. In between logging events, the system is put to sleep to save power. Each time the log button is pressed, a new SD card file is created named "Logx.txt" where x increments with each button press, and each one of these individual files is initiated with a header to inform the user of the data columns used by the CSV type file.

To use this software the following libraries must be installed in the Arduino IDE:
* [SdFat](https://github.com/greiman/SdFat)
* [DS3231_Logger](https://github.com/NorthernWidget/DS3231_Logger)
* [MCP3421](https://github.com/NorthernWidget/MCP3421)
* [MS5803](https://github.com/NorthernWidget/MS5803)

Information on library instillation can be found on the [Arduino](https://www.arduino.cc/en/Guide/Libraries) site, the "Manual" instillation method should be used to ensure success

### Setting the serial number

If you need to set a serial number on your Margay board, upload [this sketch](https://github.com/NorthernWidget-Skunkworks/Project-Margay/tree/master/Software/Margay_HardwareTest_v2) available in the "Software" folder within this repository.

Once the above testing sketch has been uploaded

Type `SN Set`, followed by either a carriage return or a linefeed/newline character, to enter a prompt to set the serial number. This will give you three fields into which you can enter 4-digit hexadecimal (0-F) numbers:
* Board type; at Northern Widget, this is `0x4D02` for the Margay v2.2. If you are working with one of our boards, please use this number and enter a group ID and uniuqe ID as instructed. If you have built your own board, please do *not* use this number, as we would like to keep our series separate for the sake of recordkeeping (and not track every single 3rd-party built board out there)
* Group ID: We use this to denote collaborative projects vs. internal use vs. general sales
* Unique ID: This is a monotonically increasing number.

If you wish to later read the serial number, type `SN Read`.

>> @awickert @bschulz: Document other testing capabilities within the sketch?

### Using Custom Software (Developer)
As we provide all information about on board pins and their functionality, it is easy for a user to write their own code in the Arduino IDE to leverage the hardware capabilities of the Margay to whatever degree is desired. To do this, the Northern Widget board file can be used (as described above), or the **[MightyCore](https://github.com/MCUdude/MightyCore)** Board files can be used. These are the board files the Northern Widget ones were based on, but allow for more compilation options for the user. Full instructions for instillation and use are provided on the MightyCore GitHub page.

For the Margay (any model), the recommended settings are as follows:

Setting | Value
--------|------
`Board` | `ATmega644`
`Pinout` | `Standard`
`Clock` | `8MHz External`
`Compiler LTO` | `Disabled`
`Variant` | `644P/644PA`
`BOD` | `2.7v`

#### Sample code

`Margay_NoSensors.ino`

```c++
#include "Margay.h"
// Include any sensor libraries.
// The Northern Widget standard interface is demonstrated here.
//Sensor mySensor;

// Instantiate classes
// Sensor mySensor; (for any Northern Widget standard sensor library)
Margay Logger;

// Empty header to start; will include sensor labels and information
String header;

// I2CVals for sensors
// Add these for any sensors that you attach
// These are used in the I2C device check (for the warning light)
// But at the time of writing, the logger should still work without this.
uint8_t I2CVals[] = {};

// Number of seconds between readings
// The Watchdog timer will automatically reset the logger after approximately 36 minutes.
// We recommend logging intervals of 30 minutes (1800 s) or less.
// Intervals that divide cleanly into hours are strongly preferable.
uint32_t updateRate = 60;

void setup(){
    // No sensors attached; header may remain empty.
    // header = header + mySensor.getHeader(); // + nextSensor.getHeader() + ...
    Logger.begin(I2CVals, sizeof(I2CVals), header);
}

void loop(){
    Logger.run(update, updateRate);
}

String update() {
    initialize();
    //return mySensor.getString(); // If a sensor were attached
    return ""; // Empty string for this example: no sensors attached
}

void initialize(){
    //mySensor.begin(); // For any Northern Widget sensor
                        // Other libraries may have different standards
}
```

### Reference

A full index of the public variables and functions within the Margay data logger library is available at https://github.com/NorthernWidget-Skunkworks/Margay_Library. (currently under construction)

## Field operator's guide

***Note: Logger will not be able to wake up unless the clock (RTC) is powered.***

### Logging Start
Logging begins automatically once power is applied. If error conditions are found, they will be indicated by status lights, but the logger will attempt to continue if possible. The following should represent the light sequence.

- On power application
	- `AUX` light will illuminate **green**, will stay on while testing
- After testing is complete
	- `AUX` light will turn **off**
	- `STAT` light will illuminate with various colors depending on the status of the logger
    - See "Status Codes" under "Troubleshooting" below for details
	- `STAT` light will blink blue quickly once to indicate logging (or attempted logging) has begun

### Troubleshooting
If an error code is received try the following steps:

**General**
* Disconnect and reconnect power, both USB and battery ([Turn it off and back on again](https://i.imgur.com/Yj6dB3W.gif))
* Verify the quality of all screw terminal connections by gently tugging on the wires and making sure they stay in place, if not, remove and re-tighten the connection
* Ensure sensors and/or cables are not damaged, this can result in shorts or other problems
* Make sure batteries have sufficient voltage to run the logger, when the battery voltage drops below *3.3v*, malfunctions can occur

#### Status Codes

The second LED (STAT) will have one of the colors below.

**Green**: All systems check out OK, logging will proceed

**Orange**: A sensor system is not registering properly, some sensor data may be missing or incorrect
* Verify correct polarity of sensor connection
* Ensure the right sensor is connected
* Verify the screw terminals are well connected to the wires (a loose connection can cause a failure)
* Make sure battery power is applied, some sensors can fail otherwise

**Cyan**: Clock time is incorrect, but logger is otherwise working correctly
* Connect the logger to a computer and reset the clock using the [Northern Widget Time Set GUI](https://github.com/NorthernWidget/SetTime_GUI)
* Note and record the wrong time if the logger has been out in the field, alongside the current (correct) time, to correct the prior measurements

**Pink**: (looks like purple to some people): SD card is not inserted
* Insert the SD card, or make sure card is fully seated

**Red**: Critical on-board component is not functioning correctly, such as SD card or clock; Logging will likely not be able to proceed
* Attempt power cycle
* Try different SD card
* Disconnect all sensors
* If none of the previous steps remove the red light, contact [Northern Widget](http://www.northernwidget.com/contact/) for further support

**Yellow, Fast Blinking**: <50% battery capacity
* Replace batteries

**Red, Fast Blinking**: Batteries <3.3V, voltage too low to properly function
* *If* this error occurs while also connected over USB, check proper connection of batteries
* Replace batteries

# Developer Notes

+ (**<= Mrk 1.0**) When using power from the external rail (the 3v3 on the screw terminals) it is always advised to have a battery connected to the board, even if connected via USB. The USB connection is able to power the core components, but not the external rail. We know this is annoying, but fear not, the Northern Widget team is [working tirelessly](https://i.imgur.com/Kx97N0l.gifv) to fix this for the Mrk 2.0!
+ (**<= Mrk 1.0**) When using I<sup>2</sup>C on the device, external pullups (4.7k&Omega; ~ 10k&Omega;) are required. If using device strictly onboard the internal pullups on the ATMEGA seem sufficient, but if you add capacitance of a cable attaching to an external sensor, etc, this is often too much, since the internal pullups are very weak. This is also being fixed in the Mrk 2.0 version with dedicated switchable on board pullups.
+ (**All models**) The external power rails and the switched battery rail should be enabled in hardware by default, however, it is our recommendation to explicitly define these pins (`Ext3v3Ctrl`) as outputs and drive them `LOW` even if you never intend to switch them on and off. This prevents the rails from inadvertently being turned off due to a transient on the floating control line.

# Acknowledgments

Support for this project provided by:

<img src="https://pbs.twimg.com/profile_images/1445421246045360133/zQtKhpkT_400x400.jpg" alt="UMN ESCI" width="240px">

<img src="https://www.nsf.gov/news/mmg/media/images/nsf_logo_f_ba321daf-8607-41d7-94bc-1db6039d7893.jpg" alt="NSF" width="240px">

<br/>
<br/>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
