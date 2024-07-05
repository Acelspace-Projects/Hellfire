// Adafruit M0 Adalogger + LSM6DSOX & LIS3MDL wing + LSM6DSO32 sensor
// v0.1
// author : Mixcraftio

#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
// #include <Adafruit_LSM6DSO32.h>

#define DEBUG
#define PRINT(item)                 \
          #ifdef DEBUG              \
            Serial.print(item)      \
          #else                     \
            dataFile.print(item)    \
          #endif

#define SD_CHIP_SELECT 4

File dataFile;
String dataString;
Adafruit_LIS3MDL lis3mdl;
Adafruit_LSM6DSOX sox;



void setup(){
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) delay(10);
  #endif

  SD.begin(SD_CHIP_SELECT);
  dataFile = SD.open("DATA.TXT", FILE_WRITE);
  sox.begin_I2C();
  lis3mdl.begin_I2C();

  #ifdef DEBUG
    Serial.println("Setup done!");
  #endif
}

void loop(){
  dataString = "";
  
  PRINT(dataString);
  PRINT("\n");
  dataFile.flush();
}