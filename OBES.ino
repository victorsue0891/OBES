/* 
 *  OBES : Open Source Battery based Environment Sensor
 *  Auther : victorsue0891@gmail.com
 */
//Include Library
#include <SPI.h>
#include <Wire.h>
#include "BME280_I2C.h"
#include <SdFat.h>            // Arduino FAT16-FAT32 Library https://github.com/greiman/SdFat
#include <LowPower.h>         // Lightweight low power library for Arduino https://github.com/rocketscream/Low-Power
#include <RTClib.h>           // real time clock library for Arduino  https://github.com/MrAlvin/RTClib


#define DS3231_I2C_ADDRESS 0x68
#define MOSIpin 11
#define MISOpin 12
#define SampleIntervalMinutes 1
#define SEALEVELPRESSURE_HPA (hgInches/0.02952998751)      // hPa=(inches Hg)/0.02952998751

//Global object
SdFat SD;
RTC_DS3231 RTC;
BME280_I2C bme1(0x77); // I2C address 0x77 
BME280_I2C bme2(0x76); // I2C address 0x76 

//Global Variable
const int chipSelect = 10;    //CS moved to pin 10 on the arduino
//--DS3231 RTC 
const int RTC_INTERRUPT_PIN = 2; 
byte Alarmhour;
byte Alarmminute;
byte Alarmday;
volatile boolean clockInterrupt = false;  //this flag is set to true when the RTC interrupt handler is executed
const float hgInches = 29.99;                   // Enter the sealevel barometric pressure here (xx.xx inches Hg)

/*
 Function    :
 Description : 
*/
void rtcISR() 
{
    clockInterrupt = true;
}

/*
 Function    :
 Description : 
*/
void upgradeWaitSession()
{
  int repeatTimes = 300; // wait 30 sec for Arduino IDE upload process

  Serial.begin(9600);  
  
  while ( repeatTimes > 0 )
  {
    if ( Serial.available() > 0 ) // if open the USB serial, enter to upgrade mode
    {
      Serial.println(F("======================="));
      Serial.println(F(" Enter to Upgrade Mode"));
      Serial.println(F("======================="));
      // Initial I2C interface  
      Wire.begin();          
      // Initial 
      RTC.begin();           
      // Set RTC tim as same as PC
      RTC.adjust(DateTime((__DATE__), (__TIME__)));
      Serial.println(F("Update time to :"));
      Serial.println((__DATE__));
      Serial.println((__TIME__));
      while(1)
      {
        delay(5000);
        Serial.print(".");
      }
    }
    repeatTimes = repeatTimes -1 ;
    delay(100);
  }
  Serial.println(F("booting..."));
}

/*
 Function    :
 Description : 
*/
void setup() {

  // Wait 30sec, if any input to Serial during this session,
  // the board will enter to USB Upgrade mode
  upgradeWaitSession();
	
  // Setting up SPI CS pin
  pinMode(chipSelect, OUTPUT); digitalWrite(chipSelect, HIGH);
  delay(1);
  
  // Initial BME280 sensor
  if (!bme1.begin()) 
  { 
    while (1)
    {
      Serial.println("BME280 : 01 initial fail");   
      delay(3000);
    }
  } 
  if (!bme2.begin()) 
  { 
    while (1)
    {
      Serial.println("BME280 : 02 initial fail");   
      delay(3000);
    }        
  } 
  
  // Initial 
  RTC.begin();           

  // Checking if the card is present and can be initialized  
  Serial.println(F("Finding SD card ..."));
  if (!SD.begin(chipSelect)) 
  {
    while (1)
    {
      Serial.println(F("SD : No card"));
      delay(3000);
    }
    return;
  }
  Serial.println(F("SD : OK"));  

  // Create the log file
  File dataFile = SD.open("OBES.log", FILE_WRITE);
  if (dataFile) 
  {
    dataFile.println("===[OBES]===");
    dataFile.close();
  }
  else 
  {
    Serial.println(F("File : Open Error"));
  }
  
}

/*
 Function    :
 Description : 
*/
void loop() {
  //variable for BME280
  float BMEt;
  float BMEh;
  float BMEp;
  
  // Read RTC time
  char TmeStrng[ ] = "0000/00/00,00:00:00"; //16 ascii characters (without seconds) [clf added seconds]
  DateTime now = RTC.now();
  sprintf(TmeStrng, "%04d/%02d/%02d,%02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second()); // [added seconds]
  
  // Set the RTC ISR
  if (clockInterrupt) 
  {
    //Checking the RTC alarm still on
	if (RTC.checkIfAlarm(1)) 
	{
    //turn off the RTC alarm
	  RTC.turnOffAlarm(1);
  }

    Serial.print(F("RTC Alarm on INT-0 triggered at "));
    Serial.println(TmeStrng);
    Serial.print(F("RTC utc Time: "));
    Serial.println(now.unixtime());
    clockInterrupt = false;                //reset the interrupt flag to false
  }

  // read BME280 sensor value
  bme1.readSensor(); 
  BMEt = bme1.getTemperature_C();
  BMEh = bme1.getHumidity();
  BMEp = bme1.getPressure_MB();
  String dataString01 = "";
  dataString01 += "[01]";
  dataString01 += TmeStrng;
  dataString01 += ",";
  dataString01 += String(now.unixtime());
  dataString01 += ","; 
  dataString01 += String(BMEt);
  dataString01 += ",";     
  dataString01 += String(BMEh);
  dataString01 += ",";     
  dataString01 += String(BMEp);

  bme2.readSensor(); 
  BMEt = bme2.getTemperature_C();
  BMEh = bme2.getHumidity();
  BMEp = bme2.getPressure_MB();
  String dataString02 = "";
  dataString02 += "[02]";
  dataString02 += TmeStrng;
  dataString02 += ",";
  dataString02 += String(now.unixtime());
  dataString02 += ","; 
  dataString02 += String(BMEt);
  dataString02 += ",";     
  dataString02 += String(BMEh);
  dataString02 += ",";     
  dataString02 += String(BMEp);  

  //Write Sensor Data to SD card
  File dataFile = SD.open("OBES.log", FILE_WRITE);
  if (dataFile) 
  {
    dataFile.println(dataString01);
    delay(300);
    dataFile.println(dataString02);
    delay(300);
    dataFile.close();
    delay(500);
  }
  else 
  {
    Serial.println(F("File : write error!"));
  }
  delay(100);
  Serial.println(F("Write to SD"));
  delay(100);
    
  //Set RTC the next alarm time
  Alarmhour = now.hour();
  Alarmminute = now.minute() + SampleIntervalMinutes;
  Alarmday = now.day();
  if (Alarmminute > 59) 
  {
    Alarmminute = 0;
    Alarmhour = Alarmhour + 1;
    if (Alarmhour > 23) 
    {
      Alarmhour = 0;
    }
  }
  RTC.setAlarm1Simple(Alarmhour, Alarmminute);
  RTC.turnOnAlarm(1);
  if (RTC.checkAlarmEnabled(1)) 
  {
    //should nothing
  }

  // After sleep
  // Enable interrupt on pin2 & attach it to rtcISR function:
  attachInterrupt(0, rtcISR, LOW);
  // Enter power down state with ADC module disabled to save power:
  //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  //processor starts HERE AFTER THE RTC ALARM WAKES IT UP
  detachInterrupt(0); // immediately disable the interrupt on waking

}


