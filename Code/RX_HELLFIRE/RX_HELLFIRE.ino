// Heltec Wireless Tracker + SD
// v1.0
// author : Mixcraftio

#include <SPI.h>
#include <SD.h>
#include <RadioLib.h>

#define PRINT(item)             \
          Serial.print(item);   \ 
          dataFile.print(item)

#define RADIO_DIO_1             14
#define RADIO_NSS               8
#define RADIO_RESET             12
#define RADIO_BUSY              13
#define SD_CHIP_SELECT          45
#define FREQUENCY               869.138 //MHz
#define TX_POWER                5 //dBm

SX1262 radio = new Module(RADIO_NSS, RADIO_DIO_1, RADIO_RESET, RADIO_BUSY);
String str;
File dataFile;
String infoString;
volatile bool receivedFlag = false;

#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

void init_SD() {
  if(!SD.begin(SD_CHIP_SELECT)) {
    PRINT("Card Mount Failed\n");
    delay(1000);
    init_SD();
  }
}

void init_File() {
  dataFile = SD.open("/DATA.TXT", FILE_WRITE);
  if(!dataFile) {
    PRINT("Opening file failed\n");
    delay(1000);
    init_File();
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  init_SD();
  init_File();

  int state = radio.begin(FREQUENCY, 125., 9, 7, 18, TX_POWER);
  if (state != RADIOLIB_ERR_NONE) {
    PRINT("Lora init failed with error code : ");
    PRINT(state); PRINT("\n");
    while (true);
  }

  radio.setPacketReceivedAction(setFlag);

  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    PRINT(F("success!\n"));
  } else {
    PRINT(F("failed, code "));
    PRINT(state); PRINT("\n");
    while (true);
  }

  PRINT("Setup done\n");
  dataFile.flush();
}

void setFlag(void) {
  receivedFlag = true;
}

void loop() {
  // check if the flag is set
  if(receivedFlag) {
    receivedFlag = false;

    int state = radio.readData(str);

    if(!dataFile) {
      Serial.println("SD card Error");
    }

    if (state == RADIOLIB_ERR_NONE) {
      infoString = "INFO;"+String(radio.getRSSI())+";"+String(radio.getSNR())+";"+String(radio.getFrequencyError())+"\n";
      PRINT(infoString);
      if(str.startsWith("FX10;")) {
        str = str.substring(5);
        PRINT(str);
      }
    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      PRINT(F("CRC error!\n"));

    } else {
      PRINT(F("failed, code "));
      PRINT(state); PRINT("\n");
    }
    dataFile.flush();
  }
}
