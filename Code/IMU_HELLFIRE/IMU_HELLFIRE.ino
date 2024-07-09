// Adafruit M0 Adalogger + LSM6DSOX & LIS3MDL wing + LSM6DSO32 sensor
// v0.1
// author : Mixcraftio

#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>

#define DEBUG
#ifdef DEBUG 
  #define PRINT(item) Serial.print(item)
#else
  #define PRINT(item) dataFile.print(item)
#endif

#define SD_CHIP_SELECT 4
#define DELAY_MESURE 10

File dataFile;
String dataString;
Adafruit_LIS3MDL lis3mdl;
Adafruit_LSM6DSOX sox;
uint32_t timestamp;
uint32_t prev_mes_timestamp = millis();

void setup(){
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) delay(10);
  #endif

  SD.begin(SD_CHIP_SELECT);
  dataFile = SD.open("DATA.TXT", FILE_WRITE);

  sox.begin_I2C();
  sox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
  sox.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
  sox.setAccelDataRate(LSM6DS_RATE_6_66K_HZ);
  sox.setGyroDataRate(LSM6DS_RATE_6_66K_HZ);

  lis3mdl.begin_I2C();

  #ifdef DEBUG
    Serial.println("Setup done!");
  #endif
}

void loop(){
  timestamp = millis();
  if ((timestamp - prev_mes_timestamp) >= DELAY_MESURE) {
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent(&accel, &gyro, &temp);

    PRINT(temp.temperature); PRINT("\t");
    PRINT(accel.acceleration.x); PRINT("\t");
    PRINT(accel.acceleration.y); PRINT("\t");
    PRINT(accel.acceleration.z); PRINT("\t");
    PRINT(gyro.gyro.x); PRINT("\t");
    PRINT(gyro.gyro.y); PRINT("\t");
    PRINT(gyro.gyro.z); PRINT("\t");

    sensors_event_t mag; 
    lis3mdl.getEvent(&mag);
    PRINT(mag.magnetic.x); PRINT("\t");
    PRINT(mag.magnetic.y); PRINT("\t");
    PRINT(mag.magnetic.z); PRINT("\t");

    dataString = "";
    
    PRINT(dataString);
    PRINT("\n");
    #ifndef DEBUG
      dataFile.flush();
    #endif
    prev_mes_timestamp += DELAY_MESURE;
  }
}