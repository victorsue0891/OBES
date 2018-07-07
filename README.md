# OBES : Open Source Battery based Environment Sensor

Arduino based sensor system for Cloud Observation

## Getting Started

These code can build with Arduino IDE 1.8.4.

### Dependencies

* Arduino IDE >= 1.6
* SdFat library    (Arduino FAT16-FAT32 Library https://github.com/greiman/SdFat)
* LowPower library (Lightweight low power library for Arduino https://github.com/rocketscream/Low-Power)
* RTClib           (Real Time clock library for Arduino  https://github.com/MrAlvin/RTClib)

### Hardware Connection

-------------------------
  Nano  <->  RTC DS3231
-------------------------
*   D2    <->  SQQ
*   A5    <->  SCL
*   A4    <->  SDA
*   5V    <->  VCC
*   GND   <->  GND

-------------------------
  Nano  <->  BME280 x2
-------------------------
*   A5    <->  SCK/SCL
*   A4    <->  SDI/SDA
*   5V    <->  VCC
*   GND   <->  GND

-------------------------
  Nano  <->  SD card 
-------------------------
*   CS    <->  D10
*   SCK   <->  D13
*   MOSI  <->  D11
*   MISO  <->  D12
*   5V    <->  VCC
*   GND   <->  GND

-------------------------
  Nano  <->  Battery
-------------------------
*   VIN   <->  VCC
*   GND   <->  GND
