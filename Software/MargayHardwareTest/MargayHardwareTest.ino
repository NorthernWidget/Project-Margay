#include <SPI.h>
#include <Wire.h>
#include <DS3231.h>
#include <MCP3421.h>
#include <SD.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

MCP3421 adc;

boolean Debug = true;
char ReadArray[25] = {0};

const uint8_t VRef_Pin = 3;
const uint8_t ThermSense_Pin = 1;
const uint8_t BatSense_Pin = 2;

const uint8_t VSwitch_Pin = 12;
const uint8_t SD_CD = 1;

const uint8_t Ext3v3Ctrl = 19;
const uint8_t ExtInt = 11;

const uint8_t BatSwitch = 22;

const uint8_t BuiltInLED = 20;

const uint8_t RED = 13;
const uint8_t GREEN = 15;
const uint8_t BLUE = 14;

const uint8_t RGB[] = {RED, GREEN, BLUE};

const int SD_CS = 4;
char FileNameC[11];
boolean SD_Init = false;

DS3231 Clock;

void setup() {
  pinMode(Ext3v3Ctrl, OUTPUT);
  pinMode(BatSwitch, OUTPUT);
  digitalWrite(BatSwitch, HIGH);
  digitalWrite(Ext3v3Ctrl, HIGH);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);

  pinMode(BuiltInLED, OUTPUT);
  digitalWrite(BuiltInLED, HIGH);

  Serial.begin(38400);
  Serial.print("Loading...");

  Wire.begin();
  adc.SetResolution(18);

  Serial.println("Done.");
  Serial.println("Enter When Ready...");
}

void loop() {
  static int ReadLength = 0;
  String ReadString;

  if(Serial.available() > 0) {
    char Input = Serial.read();

    if(Input != 13 || Input != 10) {
      ReadArray[ReadLength] = Input;
      ReadLength++;
    }

    if(Input == 13 || Input == 10) {
      ReadString = String(ReadArray);
      ReadString.trim();
      memset(ReadArray, 0, sizeof(ReadArray));
      ReadLength = 0;

      if(ReadString == "SD") {
        Serial.println("\nTesting SD");
        Serial.println(bool2pf(TestSD()));
      }

      if(ReadString == "Clock") {
        Serial.println("\nTesting Clock");
        Serial.println(bool2pf(TestClock()));
      }

      if(ReadString == "I2C") {
        Serial.println("\nI2C Test");
        I2CTest();
        Serial.println("End");
      }

      if(ReadString == "ADC Disp") {
        Serial.println("\nBegin ADC Test");
        ADCDisp();
        Serial.println("...End ADC Test");
      }

      if(ReadString == "IO") {
        Serial.println("\nIO Test");
        IOTest();
        Serial.println("End");
      }

      if(ReadString == "PG") {
        Serial.println("\nBegin PG Test");
        Serial.print("PG: ");
        Serial.println(bool2pf(PGTest()));
      }

      if(ReadString == "Power") {
        Serial.println("\nPower Test");
        PowerTest();
        Serial.println("End");
      }

      if(ReadString == "Button") {
        Serial.println("\nBegin Button Test");
        ButtonTest();
      }

      if(ReadString == "LED") {
        Serial.println("\nLED Test");
        LEDTest();
        Serial.println("End");
      }

      if(ReadString == "SN Set") {
        char Data[5] = {0};
        while(Serial.available() > 0) {
          int x = Serial.read();
        }
        Serial.print("Enter Board Type... ");
        while(Serial.available() == 0);
        String DeviceIDStr = Serial.readString();
        DeviceIDStr.toCharArray(Data, 5);
        uint16_t DeviceID = (int)strtol(Data, NULL, 16);
        Serial.println(DeviceID, HEX);

        Serial.print("Enter Group ID... ");
        while(Serial.available() == 0);
        String GroupIDStr = Serial.readString();
        GroupIDStr.toCharArray(Data, 5);
        uint16_t GroupID = (int)strtol(Data, NULL, 16);
        Serial.println(GroupID, HEX);

        Serial.print("Enter Unique ID... ");
        while(Serial.available() == 0);
        String UniIDStr = Serial.readString();
        UniIDStr.toCharArray(Data, 5);
        uint16_t UniID = (int)strtol(Data, NULL, 16);
        Serial.println(UniID, HEX);

        int Location = EEPROM.length() - 8;

        boolean Fault = false;
        uint16_t FirmwareID = 0x0000;
        uint16_t SerialNumber[] = {DeviceID, GroupID, UniID, FirmwareID};

        for(int i = 0; i < 4; i++) {
          EEPROM.write(Location, (SerialNumber[i] & 0xFF00) >> 8);
          EEPROM.write(Location + 1, SerialNumber[i] & 0xFF);
          Location += 2;
        }
        Location = EEPROM.length() - 8;

        for(int i = 0; i < 4; i++) {
          if(EEPROM.read(Location) != (SerialNumber[i] & 0xFF00) >> 8 ||
             EEPROM.read(Location + 1) != (SerialNumber[i] & 0xFF)) {
            Fault = true;
          }
          Location += 2;
        }

        if(Fault == true) {
          Serial.println("CRITICAL ERROR: Readback Failed, try again");
        } else {
          Serial.println("\n...Serial Number Write Completed Without Errors.");
        }
      }

      if(ReadString == "SN Read") {
        int Location = EEPROM.length() - 8;
        for(int i = 0; i < 8; i++) {
          Serial.println(EEPROM.read(Location + i), HEX);
        }
      }

      if(ReadString == "ItsAUnixSystem") {
        Debug = true;
      }
    }
  }
}

void IOTest() {
  pinMode(VSwitch_Pin, OUTPUT);
  pinMode(ExtInt, OUTPUT);

  while(Serial.read() != '\r') {
    digitalWrite(VSwitch_Pin, LOW);
    digitalWrite(ExtInt, LOW);
    delay(1000);
    digitalWrite(VSwitch_Pin, HIGH);
    digitalWrite(ExtInt, HIGH);
    delay(1000);
  }
}

boolean PGTest() {
  pinMode(18, INPUT);
  if(digitalRead(18) == HIGH) return false;
  else return true;
}

void PowerTest() {
  pinMode(Ext3v3Ctrl, OUTPUT);

  while(Serial.read() != '\r') {
    digitalWrite(Ext3v3Ctrl, LOW);
    delay(1000);
    digitalWrite(Ext3v3Ctrl, HIGH);
    delay(1000);
  }

  pinMode(Ext3v3Ctrl, INPUT);
}

void I2CTest() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if(error == 0) {
      Serial.print("I2C device found at address 0x");
      if(address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if(error == 4) {
      Serial.print("Unknown error at address 0x");
      if(address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if(nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void LEDTest() {
  pinMode(BuiltInLED, OUTPUT);
  digitalWrite(BuiltInLED, HIGH);
  for(int p = 0; p < 3; p++) {
    pinMode(RGB[p], OUTPUT);
    digitalWrite(RGB[p], HIGH);
  }

  while(Serial.read() != '\r') {
    for(int i = 0; i < 3; i++) {
      digitalWrite(RGB[i], LOW);
      delay(1000);
      digitalWrite(RGB[i], HIGH);
      delay(1000);
    }
    digitalWrite(BuiltInLED, LOW);
    delay(1000);
    digitalWrite(BuiltInLED, HIGH);
    delay(1000);
  }
}

void ADCDisp() {
  while(Serial.read() != '\r') {
    Serial.print("Ref = ");
    Serial.print(analogRead(VRef_Pin) * (3.3 / 1024.0));
    Serial.println(" V");

    Serial.print("Therm = ");
    Serial.print(analogRead(ThermSense_Pin) * (3.3 / 1024.0));
    Serial.println(" V");

    Serial.print("Bat = ");
    Serial.print(analogRead(BatSense_Pin) * (3.3 / 1024.0) / 10.0);
    Serial.println(" V");

    Serial.print("Ax = ");
    Serial.print(adc.GetVoltage() * (3.3 / 1024.0));
    Serial.println(" V");

    Serial.print("\n\n");
  }
}

boolean TestSD() {
  boolean SDError = false;
  pinMode(SD_CS, OUTPUT);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  if(!SD_Init) {
    if(!SD.begin(SD_CS)) {
      if(Debug) Serial.println("Failed Init");
      SDError = true;
      return SDError;
    }
    if(Debug) Serial.println("Initialized");
    SD_Init = true;
  }

  String FileName = "HWTest";
  (FileName + ".txt").toCharArray(FileNameC, 11);
  SD.remove(FileNameC);

  randomSeed(millis());
  int RandVal = random(30557); // number of words in Hamlet
  char RandDigits[5] = {0};
  sprintf(RandDigits, "%d", RandVal);
  int RandLength = (int)((ceil(log10(RandVal)) + 1) * sizeof(char));

  if(Debug) {
    Serial.print("Random Value = ");
    Serial.println(RandVal);
  }

  File DataWrite = SD.open(FileNameC, FILE_WRITE);
  if(DataWrite) {
    DataWrite.println(RandVal);
    DataWrite.println("\nHe was a man. Take him for all in all.");
    DataWrite.println("I shall not look upon his like again.");
    DataWrite.println("-Hamlet, Act 1, Scene 2");
  }
  DataWrite.close();

  char TestDigits[5] = {0};
  File DataRead = SD.open(FileNameC, FILE_READ);
  if(DataRead) {
    DataRead.read(TestDigits, RandLength);

    if(Debug) Serial.print("Received Value = ");
    for(int i = 0; i < RandLength - 1; i++) {
      if(Debug) Serial.print(TestDigits[i]);
      if(TestDigits[i] != RandDigits[i]) {
        SDError = true;
      }
    }
    if(Debug) Serial.println("");

    if(Debug) {
      Serial.println("SD Test Data:");
      while(DataRead.available()) {
        Serial.write(DataRead.read());
      }
    }
  }
  DataRead.close();

  if(Debug) Serial.println("");

  return SDError;
}

boolean TestClock() {
  boolean ClockError = false;
  boolean DumbFalse = false;
  Clock.setClockMode(false); // 24h
  byte DateSet[] = {63, 4, 5, 4, 20, 0, 0};
  byte DateRead[7] = {0};

  Clock.setYear(DateSet[6]);
  Clock.setMonth(DateSet[5]);
  Clock.setDate(DateSet[4]);
  Clock.setDoW(DateSet[3]);
  Clock.setHour(DateSet[2]);
  Clock.setMinute(DateSet[1]);
  Clock.setSecond(DateSet[0]);

  delay(5000);

  DateRead[6] = Clock.getYear();
  DateRead[5] = Clock.getMonth(DumbFalse);
  DateRead[4] = Clock.getDate();
  DateRead[3] = Clock.getDoW();
  DateRead[2] = Clock.getHour(DumbFalse, DumbFalse);
  DateRead[1] = Clock.getMinute();
  DateRead[0] = Clock.getSecond();

  for(int i = 6; i > 0; i--) {
    if(DateRead[i] != DateSet[i]) ClockError = true;
  }
  if(DateRead[0] != DateSet[0] + 5 && DateRead[0] != DateSet[0] + 4 && DateRead[0] != DateSet[0] + 6)
    ClockError = true;

  if(Debug) {
    Serial.println("Values Set:");
    Serial.print("Year = "); Serial.print("20"); Serial.println(DateSet[6]);
    Serial.print("Month = "); Serial.println(DateSet[5]);
    Serial.print("Day = "); Serial.println(DateSet[4]);
    Serial.print("DoW = "); Serial.println(DateSet[3]);
    Serial.print("Hour = "); Serial.println(DateSet[2]);
    Serial.print("Minute = "); Serial.println(DateSet[1]);
    Serial.print("Second = "); Serial.println(DateSet[0]);

    Serial.println("\nValues Read:");
    Serial.print("Year = "); Serial.print("20"); Serial.println(DateRead[6]);
    Serial.print("Month = "); Serial.println(DateRead[5]);
    Serial.print("Day = "); Serial.println(DateRead[4]);
    Serial.print("DoW = "); Serial.println(DateRead[3]);
    Serial.print("Hour = "); Serial.println(DateRead[2]);
    Serial.print("Minute = "); Serial.println(DateRead[1]);
    Serial.print("Second = "); Serial.println(DateRead[0]);
    Serial.println("");
  }
  return ClockError;
}

void ButtonTest() {
  pinMode(2, INPUT);
  long Timeout = 2000;
  long Time = millis();
  while(millis() - Time < Timeout) {
    if(digitalRead(2) == LOW) {
      Serial.println("Log Button Pushed");
      while(digitalRead(2) == LOW);
    }
  }
  Serial.println("Test Complete");
}

String bool2pf(boolean TestResult) {
  if(TestResult) return "FAIL";
  return "PASS";
}
