// Arduino nano + tensionometre + bmp085
// v0.1
// author : Mixcraftio

#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>

#define DEBUG

#define SD_CHIP_SELECT 10
#define TENSIONOMETRE_PIN A1
#define DELAY_MESURE 10 // ms

File dataFile;
Adafruit_BMP085 bmp;

uint32_t prev_mes_timestamp;
uint32_t timestamp;
uint16_t tensioVal;
uint16_t pressVal;

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Initialising...");
  #endif

  SD.begin(SD_CHIP_SELECT);
  dataFile = SD.open("DATA.TXT", FILE_WRITE);
  bmp.begin();
  pinMode(TENSIONOMETRE_PIN, INPUT);

  #ifdef DEBUG
    Serial.println("Setup done!");
  #endif
}

void loop() {
  timestamp = millis();
  if ((timestamp - prev_mes_timestamp) >= DELAY_MESURE){
    tensioVal = analogRead(TENSIONOMETRE_PIN);
    pressVal = bmp.readPressure();

    #ifdef DEBUG
      if (dataFile) {
        dataFile.print(timestamp);
        dataFile.print("\t");
        dataFile.print(tensioVal);
        dataFile.print("\t");
        dataFile.println(pressVal);
        dataFile.flush();
      }
      else {
        Serial.println("ERROR: Failed to open DATA.TXT");
      }
      Serial.print(timestamp);
      Serial.print("\t");
      Serial.print(tensioVal);
      Serial.print("\t");
      Serial.println(pressVal);
    #else
      dataFile.print(timestamp);
      dataFile.print("\t");
      dataFile.print(tensioVal);
      dataFile.print("\t");
      dataFile.println(pressVal);
      dataFile.flush();
    #endif
    prev_mes_timestamp = timestamp;
  }
}
