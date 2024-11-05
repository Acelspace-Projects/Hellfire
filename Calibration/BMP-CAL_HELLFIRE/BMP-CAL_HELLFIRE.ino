// Arduino nano + tensionometre + bmp085
// v1.0
// author : Mixcraftio

#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

#define PRINT(item) Serial.print(item)

#define DELAY_MESURE 50

Adafruit_BMP085 bmp;

uint32_t timestamp;
uint32_t prev_mes_timestamp;
uint16_t pressVal;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Initialising...");
  
  Wire.begin(0b11101110);
  Wire.setClock(400000L);
  bmp.begin(BMP085_ULTRALOWPOWER, &Wire);

  Serial.println("Setup done!");
  prev_mes_timestamp = millis();

  String caldat = bmp.retCalDat();
  Serial.println(caldat);
}

void loop() {
  timestamp = millis();
  if ((timestamp - prev_mes_timestamp) >= DELAY_MESURE) {
    pressVal = bmp.readRawPressure();

    PRINT(timestamp);
    PRINT("\t");
    PRINT(pressVal);
    PRINT("\n");
    prev_mes_timestamp += DELAY_MESURE;
  }
}
