#include <SPI.h>
#include <Wire.h>
// #include <arduinolib.h>
// #include <NorthernWidget.h>
#include <DS3231_Logger.h>
#include <MCP3421.h>
#include <MS5803.h>
// #include <SD.h>
#include <SdFat.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

//////////////////////////////////USER DEFINE//////////////////////////////////
const unsigned int LogInterval = 5; //Number of seconds between each logging event
//#define BARO //Uncomment this line if barometer is connected to logger

// int Log_Interval_Seconds = 5; //Valid range is 0-59 seconds
// int Log_Interval_Minutes = 0; //Valid range is 0-59 minutes
// int Log_Interval_Hours = 0; //Valid range is 0-23 hours

/////////////////////////////////HARDWARE PINS//////////////////////////////////
const uint8_t VRef_Pin = 2;
const uint8_t ThermSense_Pin = 1;
const uint8_t BatSense_Pin = 0;

const uint8_t VSwitch_Pin = 3;
const uint8_t SD_CD = 1;

const uint8_t Ext3v3Ctrl = 12;
const uint8_t ExtInt = 11;
const uint8_t RTCInt = 10;
const uint8_t LogInt = 2; 
////////////////////////////////////////LED//////////////////////////////////

const uint8_t BuiltInLED = 19;

// const uint8_t RED = 13;
// const uint8_t GREEN = 15;
// const uint8_t BLUE = 14;

// const uint8_t RGB[] = {RED, GREEN, BLUE};

const uint8_t RedLED = 13;
const uint8_t GreenLED = 15;
const uint8_t BlueLED = 14;

const unsigned long RED = 0xFFFF0000L;
const unsigned long GREEN = 0xFF00FF00L;
const unsigned long BLUE = 0xFF0000FFL;
const unsigned long MAROON = 0xFF800000L;
const unsigned long GOLD = 0xFFFFD700L;
const unsigned long ORANGE = 0xFFFFA500L;
const unsigned long PURPLE = 0xFF800080L;
const unsigned long CYAN = 0xFF00FFFF;
const unsigned long BLACK_ALERT = 0x802019FF;
const unsigned long OFF = 0x00;

////////////////////////////////////I2C////////////////////////////////////////
const int ClockADR = 0x68;
const int TempPresADR = 0x77;
const int TempADR = 0x6A;
const int BaroADR = 0x76;
const int I2C_ADR[4] = {ClockADR, TempPresADR, TempADR, BaroADR}; //Keep array of all addresses for system test

////////////////////////////////////SD////////////////////////////////////////
const int SD_CS = 4;
char FileNameC[11]; //Used for file handling
char FileNameTestC[11]; //Used for file handling
boolean SD_Init = false;
SdFat SD;
byte  keep_SPCR;

////////////////////////////////////CLOCK//////////////////////////////////////
DS3231_Logger RTC;
// RTClib RTC;
// bool Century=false;
// bool h12;
// bool PM;
// byte ADay, AHour, AMinute, ASecond, ABits;
// bool ADy, A12h, Apm;

const char compile_date[] = __DATE__ " " __TIME__;
String CompileDateTime = String(compile_date);
///////////////////////////////////GLOBALS/////////////////////////////////////
bool GlobalError = false;
String Data; //Data logging string
bool LogEvent = false; //Used to test if logging should begin yet
bool NewLog = false; //Used to tell system to start a new log
int AwakeCount = 0;

//////////////////////////////////TP Downhole//////////////////////////////////
MCP3421 adc;
MS5803 Downhole(ADDRESS_LOW, 5);
MS5803 Baro(ADDRESS_HIGH, 2);

/////////////////////////////////THERMISTOR////////////////////////////////////
float A = 0.003354016;
float B = 0.0003074038;
float C = 1.019153E-05;
float D = 9.093712E-07;

///////////////////////////////////SENSOR VALS/////////////////////////////////
String LogTimeDate = "2063/04/05 20:00:00";
float Temp[5] = {0}; //Temp Downhole, Temp on board, Temp RTC, Temp Baro
float Pressure[2] = {0}; //Downhole pressure, Atmospheric pressure (if applicable)
float BatVoltage = 0;


void setup () {
	Init(); 
	I2CTest();
	ClockTest(); //Fix clock test function! 
	// PowerTest(); //Problems with power turned off using I2C
	SDTest();
  	
  	digitalWrite(BuiltInLED, HIGH); 

	if(GlobalError) LED_Color(RED);
	else(LED_Color(GREEN));

	Serial.print("\n\nLog When Ready...");

	delay(2000);

	LED_Color(OFF);
	// RTC.SetAlarm(LogInterval); //Setup and turn on an alarm for a given number of seconds 
}

void loop() {

	if(NewLog) {
		LogEvent = true;
		RTC.SetAlarm(LogInterval);
		// SetAlarm(Log_Interval_Hours, Log_Interval_Minutes, Log_Interval_Seconds); //Setup alarm when log button is pressed
		InitLogFile(); //Start a new file each time log button is pressed
		// Log(); //Log a value at that instant as well as set up future logs
		NewLog = false;
    Blink();
	}

	if(LogEvent) {
		Update();
		LogStr(Data);
		Serial.println("LoggingDone"); //DEBUG!
		LogEvent = false; //Clear log flag
	}

//  Serial.println("AlarmTest"); //DEBUG!
	if(!digitalRead(RTCInt)) {
//    Serial.println("Reset Alarm"); //DEBUG!
//    RTC.ClearAlarm(); //
		RTC.SetAlarm(LogInterval); //Turn alarm back on 
	}

	AwakeCount++;

	if(AwakeCount > 5) {
//    AwakeCount = 0;
		sleepNow();
	}
	delay(1);	
}

void Blink() {
  for(int i = 0; i < 5; i++) {
    digitalWrite(BlueLED, LOW);
    delay(500);
    digitalWrite(BlueLED, HIGH);
    delay(500);
  }
}

void I2CTest() {
	int Error = 0;
	bool I2C_Test = true;
	#if defined(BARO)
		int NumADR = 4;
	#else
		int NumADR = 3;
	#endif

	Serial.print("I2C: ");

	for(int i = 0; i < NumADR; i++) {
		Wire.beginTransmission(I2C_ADR[i]);
		// Wire.write(0xFF);
    	Error = Wire.endTransmission();
    	if(Error != 0) {
    		if(I2C_Test) Serial.println(" Fail");
    		Serial.print("   Fail At: ");
    		Serial.println(I2C_ADR[i], HEX);
    		// Serial.println(Error); //DEBUG!
    		I2C_Test = false;
    		GlobalError = true;
		}
	}

	if(I2C_Test) Serial.println("PASS");
}

void SDTest() {
	bool SDError = false;
	// bool SD_Test = true;

	pinMode(SD_CD, INPUT);
	bool CardPressent = digitalRead(SD_CD);

	Serial.print("SD: ");

	if(CardPressent) Serial.println(" NO CARD");

	if (!SD.begin(SD_CS)) {
    	SDError = true;
    	GlobalError = true;
  	}

	String FileNameTest = "HWTest";
	(FileNameTest + ".txt").toCharArray(FileNameTestC, 11);
	SD.remove(FileNameTestC); //Remove any previous files

	randomSeed(analogRead(A7)); //Seed with a random number to try to endsure randomness
	int RandVal = random(30557); //Generate a random number between 0 and 30557 (the number of words in Hamlet)
	char RandDigits[6] = {0};
	sprintf(RandDigits, "%d", RandVal); //Convert RandVal into a series of digits
	int RandLength = (int)((ceil(log10(RandVal))+1)*sizeof(char)); //Find the length of the values in the array

	File DataWrite = SD.open(FileNameTestC, FILE_WRITE);
	if(DataWrite) {
	DataWrite.println(RandVal);
	DataWrite.println("\nHe was a man. Take him for all in all.");
	DataWrite.println("I shall not look upon his like again.");
	DataWrite.println("-Hamlet, Act 1, Scene 2");
	}
	DataWrite.close();

	char TestDigits[6] = {0};
	File DataRead = SD.open(FileNameTestC, FILE_READ);
	if(DataRead) {
	DataRead.read(TestDigits, RandLength);

	for(int i = 0; i < RandLength - 1; i++){ //Test random value string
	  if(TestDigits[i] != RandDigits[i]) {
	    SDError = true;
	    GlobalError = true;
	  }
	}
	}
	DataRead.close();

	//Print SD file info??
	//  if(!SDError && !GlobalError) {


	keep_SPCR=SPCR; 
  
	if(SDError && !CardPressent) Serial.println("FAIL");
  else Serial.println("PASS");

	//  SPI.end();
	//  digitalWrite(SD_CS, LOW);
	//  pinMode(5, OUTPUT);
	//  digitalWrite(5, LOW);
	//  pinMode(6, OUTPUT);
	//  digitalWrite(6, LOW);
	//  pinMode(7, OUTPUT);
	//  digitalWrite(7, LOW);
}

void InitLogFile(){
    String FileName = "Log";
    int FileNum = 1;
    String NumString = "01";
    (FileName + "01"+ ".txt").toCharArray(FileNameC, 11);
    while(SD.exists(FileNameC)) {
      FileNum += 1;
      NumString = String(FileNum, DEC);
      (FileName + NumString + ".txt").toCharArray(FileNameC, 11);
    }
    (FileName + NumString + ".txt").toCharArray(FileNameC, 11);
  
    #if defined(BARO)
	//      LogStr("Test");
	    LogStr("Pressure [mBar], Baro [mBar], TempDownhole [C], TempDownholePres [C], TempBoard [C], TempRTC [C], TempBaro [C], Bat [V], Time [UTC]"); 
    #else
	//        String Demo = "Demo";
	//        LogStr(Demo);
		LogStr("Pressure [mBar], TempDownhole [C], TempDownholePres [C], TempBoard [C], TempRTC [C], Bat [V], Time [UTC]"); 
    #endif
}

void ClockTest() {
	int Error = 1;

	// digitalWrite(Ext3v3Ctrl, HIGH); //Turn off power to outputs
  delay(1000);
	Serial.print("Clock: ");
	Wire.beginTransmission(I2C_ADR[0]);
  Wire.write(0xFF);
	Error = Wire.endTransmission();
	if(Error == 0) Serial.println(" PASS");

	if(Error != 0) Serial.println(" FAIL");
	
	// digitalWrite(Ext3v3Ctrl, LOW); //Turn power back on
}

void PowerTest() {
	int Error = 0;

	digitalWrite(Ext3v3Ctrl, HIGH); //Turn off power to outputs

	Serial.print("Power: ");
	Wire.beginTransmission(I2C_ADR[1]);
	Error = Wire.endTransmission();
	if(Error == 0) Serial.println(" FAIL");

	if(Error != 0) Serial.println(" PASS");

	digitalWrite(Ext3v3Ctrl, LOW); //Turn power back on
}

void LED_Color(unsigned long Val) {
	int Red = 0; //Red led color
	int Green = 0;  //Green led color
	int Blue = 0;  //Blue led color
	int Lum = 0;  //Luminosity

	//Parse all values from single Val
	Blue = Val & 0xFF;
	Green = (Val >> 8) & 0xFF;
	Red = (Val >> 16) & 0xFF;
	Lum = (Val >> 24) & 0xFF;
	//  Lum = 255 - Lum; //Invert since LEDs are open drain

	analogWrite(RedLED, 255 - (Red * Lum)/0xFF);
	analogWrite(GreenLED, 255 - (Green * Lum)/0xFF);
	analogWrite(BlueLED, 255 - (Blue * Lum)/0xFF);
}

int LogStr(String Val) {
  File DataFile = SD.open(FileNameC, FILE_WRITE);

  // if the file is available, write to it:
  if (DataFile) {
    DataFile.println(Val);
	   // return 0;
  }
  // if the file isn't open, pop up an error:
  else {
	   // return -1;
  }

  DataFile.close();
}

void Log() {
	//Write global Data to SD
	LogEvent = true; //Set flag for a log event
	AwakeCount = 0; 
}

void Init() {
  //Sets up basic initialization required for the system


  pinMode(Ext3v3Ctrl, OUTPUT);
  digitalWrite(Ext3v3Ctrl, LOW); //Turn on power by default, turn off later to sleep

  pinMode(RedLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);

  LED_Color(OFF);
  
  Serial.begin(38400); //DEBUG!
	//  Serial.println(CompileDateTime);
	//  Serial.println(millis());
  Serial.println("Initializing...\n\n"); //DEBUG!
  pinMode(BuiltInLED, OUTPUT);

  Wire.begin();
  pinMode(SD_CS, OUTPUT);
  // SPI.setDataMode(SPI_MODE0);
  // SPI.setClockDivider(SPI_CLOCK_DIV2); //Sts SPI clock to 4 MHz for an 8 MHz system clock

	pinMode(RTCInt, INPUT_PULLUP);
  	pinMode(LogInt, INPUT);
  	attachInterrupt(digitalPinToInterrupt(RTCInt), Log, FALLING); //Attach an interrupt driven by the interrupt from RTC, logs data
  	attachInterrupt(digitalPinToInterrupt(LogInt), StartLog, FALLING);	//Attach an interrupt driven by the manual log button, sets logging flag and logs data

  RTC.Begin(); //Initalize RTC
	RTC.ClearAlarm(); //
	Downhole.reset();
	Downhole.begin();
  #if defined(BARO) 
    Baro.reset();
    Baro.begin();
  #endif
}

void Update() {
	//Collect new values and write those values to global Data
	int NumTemp = 4;
	int NumPres = 1;
	GetTPDownholeVals(); //FIX!
	GetOnBoardVals();
	GetTime(); //FIX!
	#if defined(BARO)
		GetBaroVals();
		NumTemp = 5;
		NumPres = 2;
	#endif

	Data = ""; //Clear the data string
	for(int i = 0; i < NumPres; i++) { //Load pressure vals into data string
		Data = Data + String(Pressure[i]) + ",";
	}
	for(int i = 0; i < NumTemp; i++) { //Load temp vals into data string
		Data = Data + String(Temp[i]) + ",";
	}
	Data = Data + BatVoltage + ","; //Load battery voltage into data string
	Data = Data + LogTimeDate; //Load time into data string
}

void GetTime() {
	//Update global time string
	// DateTime TimeStamp = RTC.now();
	// LogTimeDate = String(TimeStamp.year()) + "/" + String(TimeStamp.month()) + "/" + String(TimeStamp.day()) + " " + String(TimeStamp.hour()) + ":" + String(TimeStamp.minute()) + ":" + String(TimeStamp.second());  
	LogTimeDate = RTC.GetTime(0);
}

// void SetAlarm(byte hours, byte minutes, byte seconds) {

// 	tmElements_t tm;
//     tm.Hour = 00;                   //set the RTC to an arbitrary time
//     tm.Minute = 00;
//     tm.Second = 00;
//     tm.Day = 16;
//     tm.Month = 9;
//     tm.Year = 2017 - 1970;          //tmElements_t.Year is the offset from 1970
//     write(tm);                  //set the RTC from the tm structure
// 	//Sets alarm on RTC based on user desired frequency
// 	setAlarm(ALM1_MATCH_SECONDS, seconds, minutes, hours, 0);
//     //clear the alarm flag
//     alarm(ALARM_1);
//     //configure the INT/SQW pin for "interrupt" operation (disable square wave output)
//     squareWave(SQWAVE_NONE);
//     //enable interrupt output for Alarm 1
//     alarmInterrupt(ALARM_1, true);

// //
//   // byte AlarmBits = 0b01001000;
//   // Clock.turnOffAlarm(1); //Turn off alarms before setting.
//   // Clock.turnOffAlarm(2);

//   // Clock.checkIfAlarm(1); //Clear alarm flags, do I need to do this here?
//   // Clock.checkIfAlarm(2); //Clear alarm flags

//   // // This is the primary alarm
//   // Clock.setA1Time(0, _hours, _minutes, _seconds, AlarmBits, true, false, false);
//   // delay(2);

//   // // This is a backup alarm that will wake the logger in case it misses the
//   // // first alarm for some unknown reason
//   // uint8_t _hours_backup = _hours;
//   // uint8_t _minutes_backup = _minutes+2;

//   // if(_minutes_backup > 59){_minutes_backup = _minutes_backup - 60; _hours_backup++;}
//   // if(_hours_backup > 23){_hours_backup = _hours_backup - 24;}
//   // Clock.setA2Time(0, _hours_backup, _minutes_backup, AlarmBits, true, false, false);  //setting as backup wake function
//   // delay(2);
//   // Clock.turnOnAlarm(1); //Turn on alarms.
//   // delay(1);
//   // Clock.turnOnAlarm(2);
//   // delay(1);

//   // // Serial.print(' ');
//   // Serial.write(2); // write ASCII STX; somehow Serial comes to the rescue again!
//   // Serial.write(2); // STX just as a character that doesn't take much space in
//   //                  // the Serial monitor
//   // Serial.flush();
// //

// }

// byte write(tmElements_t &tm)
// {
//     Wire.beginTransmission(RTC_ADDR);
//     Wire.write((uint8_t)RTC_SECONDS);
//     Wire.write(dec2bcd(tm.Second));
//     Wire.write(dec2bcd(tm.Minute));
//     Wire.write(dec2bcd(tm.Hour));         //sets 24 hour format (Bit 6 == 0)
//     Wire.write(tm.Wday);
//     Wire.write(dec2bcd(tm.Day));
//     Wire.write(dec2bcd(tm.Month));
//     Wire.write(dec2bcd(tmYearToY2k(tm.Year))); 
//     byte ret = Wire.endTransmission();
//     uint8_t s = readRTC(RTC_STATUS);        //read the status register
//     writeRTC( RTC_STATUS, s & ~_BV(OSF) );  //clear the Oscillator Stop Flag
//     return ret;
// }

// void setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate)
// {
//     uint8_t addr;
    
//     seconds = dec2bcd(seconds);
//     minutes = dec2bcd(minutes);
//     hours = dec2bcd(hours);
//     daydate = dec2bcd(daydate);
//     if (alarmType & 0x01) seconds |= _BV(A1M1);
//     if (alarmType & 0x02) minutes |= _BV(A1M2);
//     if (alarmType & 0x04) hours |= _BV(A1M3);
//     if (alarmType & 0x10) daydate |= _BV(DYDT);
//     if (alarmType & 0x08) daydate |= _BV(A1M4);
    
//     if ( !(alarmType & 0x80) ) {    //alarm 1
//         addr = ALM1_SECONDS;
//         writeRTC(addr++, seconds);
//     }
//     else {
//         addr = ALM2_MINUTES;
//     }
//     writeRTC(addr++, minutes);
//     writeRTC(addr++, hours);
//     writeRTC(addr++, daydate);
// }

// void alarmInterrupt(byte alarmNumber, bool interruptEnabled)
// {
//     uint8_t controlReg, mask;
    
//     controlReg = readRTC(RTC_CONTROL);
//     mask = _BV(A1IE) << (alarmNumber - 1);
//     if (interruptEnabled)
//         controlReg |= mask;
//     else
//         controlReg &= ~mask;
//     writeRTC(RTC_CONTROL, controlReg); 
// }

// bool alarm(byte alarmNumber)
// {
//     uint8_t statusReg, mask;
    
//     statusReg = readRTC(RTC_STATUS);
//     mask = _BV(A1F) << (alarmNumber - 1);
//     if (statusReg & mask) {
//         statusReg &= ~mask;
//         writeRTC(RTC_STATUS, statusReg);
//         return true;
//     }
//     else {
//         return false;
//     }
// }

// void squareWave(SQWAVE_FREQS_t freq)
// {
//     uint8_t controlReg;

//     controlReg = readRTC(RTC_CONTROL);
//     if (freq >= SQWAVE_NONE) {
//         controlReg |= _BV(INTCN);
//     }
//     else {
//         controlReg = (controlReg & 0xE3) | (freq << RS1);
//     }
//     writeRTC(RTC_CONTROL, controlReg);
// }

// byte writeRTC(byte addr, byte *values, byte nBytes)
// {
//     Wire.beginTransmission(0x68);
//     Wire.write(addr);
//     for (byte i=0; i<nBytes; i++) Wire.write(values[i]);
//     return Wire.endTransmission();
// }

// byte writeRTC(byte addr, byte value)
// {
//     return ( writeRTC(addr, &value, 1) );
// }

// byte readRTC(byte addr, byte *values, byte nBytes)
// {
// 	Wire.beginTransmission(RTC_ADDR);
//     Wire.write(addr);
//     if ( byte e = Wire.endTransmission() ) return e;
//     Wire.requestFrom( (uint8_t)RTC_ADDR, nBytes );
//     for (byte i=0; i<nBytes; i++) values[i] = Wire.read();
//     return 0;
// }

// byte readRTC(byte addr)
// {
//     byte b;
    
//     readRTC(addr, &b, 1);
//     return b;
// }



void GetTPDownholeVals() {
  //Update global values for TPDownhole
  //Get pressure, get temp1, get temp2
  	adc.SetResolution(18);
	Pressure[0] = Downhole.getPressure(ADC_4096);
// Serial.println(Pressure[0]); //DEBUG!
	Temp[0] = Downhole.getTemperature(CELSIUS, ADC_512);
	float Vcc = 3.3;
	float Val = Vcc - adc.GetVoltage();
  	float TempData = TempConvert(Val, Vcc, 47000.0, A, B, C, D, 10000.0);
  	Temp[1] = TempData - 273.15; //Get temp from on board thermistor 
}

void GetBaroVals() {
  //Get values from barometer
  //Get pressure, and temp
  	Pressure[1] = Baro.getPressure(ADC_4096);
	Temp[3] = Baro.getTemperature(CELSIUS, ADC_512);
}

void GetOnBoardVals() {
  //Get onboard temp, RTC temp, and battery voltage, referance voltage
  // float VRef = analogRead(VRef_Pin);
  float Vcc = 3.3; //(1.8/VRef)*3.3; //Compensate for Vcc using VRef
  // Serial.println(Vcc); //DEBUG!


  float Val = float(analogRead(ThermSense_Pin))*(Vcc/1024.0);
//  float Vout = Vcc - Val;
//  Serial.println(Val); //DEBUG!
//  Serial.println(Vout);  //DEBUG!
  float TempData = TempConvert(Val, Vcc, 10000.0, A, B, C, D, 10000.0);
  Temp[2] = TempData - 273.15; //Get temp from on board thermistor 

  BatVoltage = analogRead(BatSense_Pin)*9.0*(Vcc/1024.0); //Get battery voltage, Include voltage divider in math

  // Temp[3] = Clock.getTemperature(); //Get tempreture from RTC //FIX!
  Temp[3] = RTC.GetTemp();
}

void StartLog() {
//ISR to respond to pressing log button and waking device from sleep and starting log
// Serial.println("Begin Logging"); //DEBUG!
	NewLog = true; //Set initial logging flag
}

float TempConvert(float V, float Vcc, float R, float A, float B, float C, float D, float R25){
//  Serial.print("R = "); //DEBUG!
//  Serial.println(R); //DEBUG!
  float Rt = ((Vcc/V)*R) - R;
//  Serial.print("Rt = "); //DEBUG!
//  Serial.println(Rt); //DEBUG!
  float LogRt = log(Rt/R25);
//  Serial.print("LogRt = "); //DEBUG!
//  Serial.println(LogRt); //DEBUG!
  float T = 1.0/(A + B*LogRt + C*pow(LogRt, 2.0) + D*pow(LogRt, 3.0));
  return T;
}

void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */  
    // MCUCR = bit (BODS) | bit (BODSE);
  	// MCUCR = bit (BODS);
	wdt_disable();
	// power_adc_disable(); // ADC converter
	// // power_spi_disable(); // SPI
	// power_usart0_disable();// Serial (USART) 
	// power_timer1_disable();// Timer 1
	// power_timer2_disable();// Timer 2
	// ADCSRA = 0;
	turnOffSDcard();
	// digitalWrite(Ext3v3Ctrl, HIGH); //Turn off extenral rail
	// SPI.end(); //Turn off SPI
	// digitalWrite(SD_CS, LOW);
	// pinMode(SD_CS, INPUT); //Disconnect SD chip slect pin
	// pinMode(5, INPUT); //Set all SPI pins as inputs, will be reversed be beginning SPI again
	// pinMode(6, INPUT);
	// pinMode(7, INPUT);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
 
    // attachInterrupt(0, wakeUpNow, FALLING); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
//    sleep();              //WIRING MODE!
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    // detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
//    ADCSRA = 1; //Turn ADC back on
    // digitalWrite(Ext3v3Ctrl, LOW); //turn external rail back on
    // digitalWrite(SD_CS, HIGH);
    // SPI.begin();
    // SD.begin(SD_CS);
    turnOnSDcard();
    // pinMode(SD_CS, OUTPUT); //Disconnect SD chip slect pin
 
}

void turnOffSDcard() 
 {
delay(6);
                                       // disable SPI
// power_spi_disable();                     // disable SPI clock
// DDRB &= ~((1<<DDB5) | (1<<DDB7) | (1<<DDB6) | (1<<DDB4));   // set All SPI pins to INPUT
// pinMode(SD_CD, INPUT);
// DDRC &= ~((1<<DDC0) | (1<<DDC1));
pinMode(16, INPUT);
pinMode(17, INPUT);
// digitalWrite(16, HIGH);
// digitalWrite(17, HIGH);
digitalWrite(SD_CS, HIGH);
// digitalWrite(5, LOW);
// // Note: you must disconnect the LED on pin 13 or you’ll bleed current through the limit resistor
// // LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // wait 1 second before pulling the plug!
delay(6);
digitalWrite(Ext3v3Ctrl, HIGH);  //turn off BJT
delay(20);
// SPCR = SPCR & 0b11101111;
SPCR = 0;  
power_spi_disable(); 
// SPI.end();
delay(10);
// pinMode(5, OUTPUT);
// digitalWrite(5, LOW);
// DDRB &= ~((1<<DDB5));
// PORTB &= ~(1<<PORTB5); //Set port B5 (MOSI) LOW
// DDRB &= ~((1<<DDB5) | (1<<DDB7) | (1<<DDB6) | (1<<DDB4)); 
// PORTB |= ((1<<DDB5) | (1<<DDB7) | (1<<DDB6) | (1<<DDB4));     // set ALL SPI pins HIGH (~30k pullup)
// digitalWrite(SD_CS, LOW);
// pinMode(SD_CS, INPUT);
delay(6); 
} 

void turnOnSDcard() 
 {
// pinMode(SD_CS, OUTPUT);
// SPI.begin();
// SD.begin(SD_CS);
// DDRB |= ((1<<DDB5));
// digitalWrite(SD_CS, HIGH);
delay(6);                                            // let the card settle
// some cards will fail on power-up unless SS is pulled up  ( &  D0/MISO as well? )
// DDRC = DDRC | ((1<<DDC0) | (1<<DDC1));
// DDRB = DDRB | (1<<DDB7) | (1<<DDB5) | (1<<DDB4); // set SCLK(D13), MOSI(D11) & SS(D10) as OUTPUT
// Note: | is an OR operation so  the other pins stay as they were.                (MISO stays as INPUT) 
// PORTB = PORTB & ~(1<<DDB7);  // disable pin 13 SCLK pull-up – leave pull-up in place on the other 3 lines
power_spi_enable();                      // enable the SPI clock 
SPCR=keep_SPCR;                          // enable SPI peripheral
delay(20);
digitalWrite(Ext3v3Ctrl, LOW); //turn on the BJT on SD ground line
delay(10);  

}


// uint8_t dec2bcd(uint8_t n)
// {
//     return n + 6 * (n / 10);
// }





