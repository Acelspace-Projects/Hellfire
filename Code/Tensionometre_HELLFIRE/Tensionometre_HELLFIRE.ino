// Arduino nano + tensionometre + bmp085
// v1.0
// author : Mixcraftio

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>

#define DEBUG
#ifdef DEBUG 
  #define PRINT(item) Serial.print(item)
#else
  #define PRINT(item) dataFile.print(item)
#endif

#define SD_CHIP_SELECT 10
#define TENSIONOMETRE_PIN A1
#define DELAY_MESURE_TENSIO 10
#define RAPPORT_PRESS_TENSION 500/DELAY_MESURE_TENSIO

File dataFile;
Adafruit_BMP085 bmp;

uint32_t timestamp;
uint32_t prev_mes_timestamp;
uint16_t tensioVal;
uint16_t pressVal;
uint8_t count = 0;
uint8_t flushCount = 0;

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Initialising...");
  #endif

  SD.begin(SD_CHIP_SELECT);
  dataFile = SD.open("DATA.TXT", FILE_WRITE);
  Wire.begin(0b11101110);
  Wire.setClock(400000L);
  bmp.begin(BMP085_ULTRALOWPOWER, &Wire);
  pinMode(TENSIONOMETRE_PIN, INPUT);

  #ifdef DEBUG
    Serial.println("Setup done!");
  #endif
  prev_mes_timestamp = millis();
}

void loop() {
  timestamp = millis();
  if ((timestamp - prev_mes_timestamp) >= DELAY_MESURE_TENSIO) {
    tensioVal = analogRead(TENSIONOMETRE_PIN);  
    if(count >= RAPPORT_PRESS_TENSION){
      pressVal = bmp.readRawPressure();
    }

    PRINT(timestamp);
    PRINT("\t");
    PRINT(tensioVal);
    if(count >= RAPPORT_PRESS_TENSION){
      count = 0;
      PRINT("\t");
      PRINT(pressVal);
    }
    #ifndef DEBUG
      if(flushCount >= 200){
        flushCount = 0;
        dataFile.flush();
      }
    #endif
    PRINT("\n");

    ++count;
    ++flushCount;
    prev_mes_timestamp += DELAY_MESURE_TENSIO;
  }
}
