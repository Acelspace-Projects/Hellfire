// Heltec Wireless Tracker + LSM6DSOX sensor
// v1.0
// author : Mixcraftio

// #include <SD.h>
#include <RadioLib.h>
#include <TinyGPSPlus.h>
// #include <Adafruit_LSM6DSOX.h>

// #define DEBUG
#ifdef DEBUG 
  #define PRINT(item) Serial.print(item)
#else
  #define PRINT(item) /*dataFile.print(item)*/
#endif

#define RADIO_DIO_1       14
#define RADIO_NSS         8
#define RADIO_RESET       12
#define RADIO_BUSY        13
#define SD_CHIP_SELECT    4
#define LSM_CHIP_SELECT   5
#define VGNSS_CTRL        3
#define DELAY_MESURE      20
#define FREQUENCY         869.138 //MHz
#define BANDWIDTH         125. //KHz
#define TX_POWER          20 //dBm

TinyGPSPlus GPS;
// Adafruit_LSM6DSOX sox;
// File dataFile;
SX1262 radio = new Module(RADIO_NSS, RADIO_DIO_1, RADIO_RESET, RADIO_BUSY);
// uint32_t timestamp;
// uint32_t prev_mes_timestamp = 0;
String dataString;
// String radioString;

void init_radio() {
  int state = radio.begin(FREQUENCY, BANDWIDTH, 9, 7, 18, TX_POWER);
  if (state != RADIOLIB_ERR_NONE) {
    PRINT("Lora init failed with error code : ");
    PRINT(state); PRINT("\n");
    delay(1000);
    init_radio();
  }
}

// void init_SD() {
//   if(!SD.begin(SD_CHIP_SELECT)) {
//     PRINT("Card Mount Failed\n");
//     delay(1000);
//     init_SD();
//   }
// }

// void init_File() {
//   dataFile = SD.open("/DATA.TXT", FILE_WRITE);
//   if(!dataFile) {
//     PRINT("Opening file failed\n");
//     delay(1000);
//     init_File();
//   }
// }

void init_GNSS() {
  pinMode(VGNSS_CTRL,OUTPUT);
  digitalWrite(VGNSS_CTRL,HIGH);
  Serial1.begin(115200,SERIAL_8N1,33,34);    
  while(!Serial1);
}

// void init_IMU() {
//   if (!sox.begin_SPI(LSM_CHIP_SELECT)) {
//     PRINT("LSM6DSOX init failed\n");
//     delay(1000);
//     init_IMU();
//   }
// }

void setup(void) {
  pinMode(18,OUTPUT);
  digitalWrite(18,HIGH);
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) delay(10);
  #endif

  init_radio();
  // init_SD();
  // init_File();
  init_GNSS();
  String mes="FX10;Connected\n";
  radio.transmit(mes);
  // init_IMU();
  // sox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
  // sox.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
  // sox.setAccelDataRate(LSM6DS_RATE_208_HZ);
  // sox.setGyroDataRate(LSM6DS_RATE_208_HZ);

  #ifdef DEBUG
    Serial.println("Setup done!");
  #endif
  // prev_mes_timestamp = millis();
}

void loop() {
  // timestamp = millis();
  // if ((timestamp - prev_mes_timestamp) >= DELAY_MESURE) {
  //   // "FX10";"IMU";time;temp;ax;ay;az;gx;gy;gz
  //   sensors_event_t accel;
  //   sensors_event_t gyro;
  //   sensors_event_t temp;
  //   sox.getEvent(&accel, &gyro, &temp);

  //   PRINT("FX10;IMU;");
  //   PRINT(temp.temperature); PRINT(";");
  //   PRINT(accel.acceleration.x); PRINT(";");
  //   PRINT(accel.acceleration.y); PRINT(";");
  //   PRINT(accel.acceleration.z); PRINT(";");
  //   PRINT(gyro.gyro.x); PRINT(";");
  //   PRINT(gyro.gyro.y); PRINT(";");
  //   PRINT(gyro.gyro.z);
  //   PRINT("\n");

    // radioString = "IMU;"+String(temp.temperature)+";"+
    //               String(accel.acceleration.x)+";"+
    //               String(accel.acceleration.y)+";"+
    //               String(accel.acceleration.z)+";"+
    //               String(gyro.gyro.x)+";"+
    //               String(gyro.gyro.y)+";"+
    //               String(gyro.gyro.z)+"\n";
    // radio.transmit(radioString);

  //   prev_mes_timestamp += DELAY_MESURE;
  // }
  
  if(Serial1.available()>0) {
    // "FX10";"GPS";fix;quality;time;date;locationlat;locationlng;speed;angle;altitude;satellites
    if(Serial1.peek()!='\n') {
      GPS.encode(Serial1.read());
    } else {
      Serial1.read();
      if(GPS.location.isUpdated()) {
        dataString = "FX10;GPS;;;"+String(GPS.time.hour())+":"+String(GPS.time.minute())+":"+String(GPS.time.second())+";;"+String(GPS.location.lat(),5)+";"+String(GPS.location.lng(),5)+"\n";
        PRINT(dataString);
        radio.transmit(dataString);
        // #ifndef DEBUG
        //   dataFile.flush();
        // #endif
      }
    }
	}
}