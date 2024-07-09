#include <RadioLib.h>

#define RADIO_DIO_1             14
#define RADIO_NSS               8
#define RADIO_RESET             12
#define RADIO_BUSY              13
SX1262 radio = new Module(RADIO_NSS, RADIO_DIO_1, RADIO_RESET, RADIO_BUSY);

#define FREQUENCY               869.5 //MHz
#define TX_POWER                5 //dBm

volatile bool receivedFlag = false;
  #if defined(ESP8266) || defined(ESP32)
    ICACHE_RAM_ATTR
  #endif

void setup() {
  Serial.begin(115200);
  while (!Serial);

  int state = radio.begin(FREQUENCY, 125., 9, 7, 18, TX_POWER);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Lora init failed with error code : ");
    Serial.println(state);
    while (true);
  }

  // set the function that will be called
  // when new packet is received
  radio.setPacketReceivedAction(setFlag);

  // start listening for LoRa packets
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  Serial.println("Setup done");
}

// flag to indicate that a packet was received

void setFlag(void) {
  // we got a packet, set the flag
  receivedFlag = true;
}

void loop() {
  // check if the flag is set
  if(receivedFlag) {
    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    // you can also read received data as byte array
    /*
      byte byteArr[8];
      int numBytes = radio.getPacketLength();
      int state = radio.readData(byteArr, numBytes);
    */

    if (state == RADIOLIB_ERR_NONE) {
      infoString
      

      // // print data of the packet
      // Serial.print(F("[SX1262] Data:\t\t"));
      Serial.print(str);

      // // print RSSI (Received Signal Strength Indicator)
      // Serial.print(F("[SX1262] RSSI:\t\t"));
      // Serial.print(radio.getRSSI());
      // Serial.println(F(" dBm"));

      // // print SNR (Signal-to-Noise Ratio)
      // Serial.print(F("[SX1262] SNR:\t\t"));
      // Serial.print(radio.getSNR());
      // Serial.println(F(" dB"));

      // // print frequency error
      // Serial.print(F("[SX1262] Frequency error:\t"));
      // Serial.print(radio.getFrequencyError());
      // Serial.println(F(" Hz"));

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);

    }
  }
}
