#include <RadioLib.h>

// Wireless Tracker has the following connections:
#define RADIO_DIO_1             14
#define RADIO_NSS               8
#define RADIO_RESET             12
#define RADIO_BUSY              13
SX1262 radio = new Module(RADIO_NSS, RADIO_DIO_1, RADIO_RESET, RADIO_BUSY);

#define FREQUENCY               869.5 //MHz
#define TX_POWER                20 //dBm

void setup() {
  Serial.begin(9600);
  // while (!Serial);

  int state = radio.begin(FREQUENCY, 125., 9, 7, 18, TX_POWER);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Lora init failed with error code : ");
    Serial.println(state);
    while (true);
  }

  Serial.println("Setup done");
}

// counter to keep track of transmitted packets
int count = 0;

void loop() {

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  String str = "Hello World! #" + String(count++);
  int state = radio.transmit(str);

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
    int state = radio.transmit(byteArr, 8);
  */

  if (state == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F("success!"));

    // print measured data rate
    Serial.print(F("[SX1262] Datarate:\t"));
    Serial.print(radio.getDataRate());
    Serial.println(F(" bps"));

  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }

  // wait for a second before transmitting again
  delay(1000);
}
