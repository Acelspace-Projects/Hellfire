// M0 Lora Feather Code + LSM6DSOX sensor
// v0.1
// author : Mixcraftio

#define DEBUG True
#define GPSSerial Serial1 // HardwareSerial name GPS is connected to
#define PRINT(t)              \
  #if DEBUG == True           \
    Serial.print(t);          \
  #else                       \
    rf95.send(t, sizeof(t));  \
  #endif
#define PRINTLN(t)              \
  #if DEBUG == True           \
    Serial.println(t);          \
  #else                       \
    rf95.send(t, sizeof(t));  \
  #endif


#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_GPS.h>

Adafruit_LSM6DSOX sox;
Adafruit_GPS GPS(&GPSSerial);

uint32_t time;
uint32_t previousTimeLSM = millis();
const uint16_t LSMInterval = 10;
uint32_t previousTimeGPS = millis();
const uint16_t GPSInterval = 1000;

void setup(void) {
  #if DEBUG == True
    Serial.begin(115200);
    PRINTLN("Initialising...");
    while (!Serial);
  #endif
  
  // Set ranges and init sensors
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPS.println(PMTK_Q_RELEASE);
  sox.begin_I2C();
  sox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
  sox.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
  sox.setAccelDataRate(LSM6DS_RATE_6_66K_HZ);
  sox.setGyroDataRate(LSM6DS_RATE_6_66K_HZ);

  #if DEBUG == True
    PRINTLN("Initialised!");
    printRates();
  #endif
}

#if DEBUG == True
  void printRates(){
    PRINT("Accelerometer range set to: ");
    switch (sox.getAccelRange()) {
      case LSM6DS_ACCEL_RANGE_2_G:
        PRINTLN("+-2G");
        break;
      case LSM6DS_ACCEL_RANGE_4_G:
        PRINTLN("+-4G");
        break;
      case LSM6DS_ACCEL_RANGE_8_G:
        PRINTLN("+-8G");
        break;
      case LSM6DS_ACCEL_RANGE_16_G:
        PRINTLN("+-16G");
        break;
    }

    PRINT("Gyro range set to: ");
    switch (sox.getGyroRange()) {
      case LSM6DS_GYRO_RANGE_125_DPS:
        PRINTLN("125 degrees/s");
        break;
      case LSM6DS_GYRO_RANGE_250_DPS:
        PRINTLN("250 degrees/s");
        break;
      case LSM6DS_GYRO_RANGE_500_DPS:
        PRINTLN("500 degrees/s");
        break;
      case LSM6DS_GYRO_RANGE_1000_DPS:
        PRINTLN("1000 degrees/s");
        break;
      case LSM6DS_GYRO_RANGE_2000_DPS:
        PRINTLN("2000 degrees/s");
        break;
      case ISM330DHCX_GYRO_RANGE_4000_DPS:
        break; // unsupported range for the DSOX
    }

    PRINT("Accelerometer data rate set to: ");
    switch (sox.getAccelDataRate()) {
      case LSM6DS_RATE_SHUTDOWN:
        PRINTLN("0 Hz");
        break;
      case LSM6DS_RATE_12_5_HZ:
        PRINTLN("12.5 Hz");
        break;
      case LSM6DS_RATE_26_HZ:
        PRINTLN("26 Hz");
        break;
      case LSM6DS_RATE_52_HZ:
        PRINTLN("52 Hz");
        break;
      case LSM6DS_RATE_104_HZ:
        PRINTLN("104 Hz");
        break;
      case LSM6DS_RATE_208_HZ:
        PRINTLN("208 Hz");
        break;
      case LSM6DS_RATE_416_HZ:
        PRINTLN("416 Hz");
        break;
      case LSM6DS_RATE_833_HZ:
        PRINTLN("833 Hz");
        break;
      case LSM6DS_RATE_1_66K_HZ:
        PRINTLN("1.66 KHz");
        break;
      case LSM6DS_RATE_3_33K_HZ:
        PRINTLN("3.33 KHz");
        break;
      case LSM6DS_RATE_6_66K_HZ:
        PRINTLN("6.66 KHz");
        break;
    }

    PRINT("Gyro data rate set to: ");
    switch (sox.getGyroDataRate()) {
      case LSM6DS_RATE_SHUTDOWN:
        PRINTLN("0 Hz");
        break;
      case LSM6DS_RATE_12_5_HZ:
        PRINTLN("12.5 Hz");
        break;
      case LSM6DS_RATE_26_HZ:
        PRINTLN("26 Hz");
        break;
      case LSM6DS_RATE_52_HZ:
        PRINTLN("52 Hz");
        break;
      case LSM6DS_RATE_104_HZ:
        PRINTLN("104 Hz");
        break;
      case LSM6DS_RATE_208_HZ:
        PRINTLN("208 Hz");
        break;
      case LSM6DS_RATE_416_HZ:
        PRINTLN("416 Hz");
        break;
      case LSM6DS_RATE_833_HZ:
        PRINTLN("833 Hz");
        break;
      case LSM6DS_RATE_1_66K_HZ:
        PRINTLN("1.66 KHz");
        break;
      case LSM6DS_RATE_3_33K_HZ:
        PRINTLN("3.33 KHz");
        break;
      case LSM6DS_RATE_6_66K_HZ:
        PRINTLN("6.66 KHz");
        break;
    }
    return;
  }
#endif

void loop() {
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  #if DEBUG == True
    if (c) PRINT(c);
  #endif
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    // PRINT(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  time = millis();
  if (time - previousTimeGPS >= GPSInterval) {
    // "GPS";arduinotime;fix;quality;time;date;locationlat;locationlng;speed;angle;altitude;satellites

    #if DEBUG == True
      PRINT("GPS;");
      PRINT(millis()); PRINT(";");
      PRINT((int)GPS.fix); PRINT(";");
      PRINT((int)GPS.fixquality);
      if (GPS.fix) {
        PRINT(";");
        if (GPS.hour < 10) { PRINT('0'); }
        PRINT(GPS.hour, DEC); PRINT(':');
        if (GPS.minute < 10) { PRINT('0'); }
        PRINT(GPS.minute, DEC); PRINT(':');
        if (GPS.seconds < 10) { PRINT('0'); }
        PRINT(GPS.seconds, DEC); PRINT('.');
        if (GPS.milliseconds < 10) {
          PRINT("00");
        } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
          PRINT("0");
        }
        PRINT(GPS.milliseconds);
        PRINT(";");
        PRINT(GPS.day, DEC); PRINT('/');
        PRINT(GPS.month, DEC); PRINT("/20");
        PRINT(GPS.year, DEC);
        PRINT(";");
        PRINT(GPS.latitudeDegrees, 10);
        // PRINT(GPS.lat); (N or S)
        PRINT(";");
        PRINT(GPS.longitudeDegrees, 10);
        // PRINT(GPS.lon); (W or E)
        PRINT(";");
        PRINT(GPS.speed);
        PRINT(";");
        PRINT(GPS.angle);
        PRINT(";");
        PRINT(GPS.altitude);
        PRINT(";");
        PRINT((int)GPS.satellites);
        PRINT(";");
        PRINT((int)GPS.antenna);
      }
      PRINT("\n");
    // #else
    #endif

    previousTimeGPS = millis();
  }

  time = millis();
  if (time - previousTimeLSM >= LSMInterval) {
    // "IMU";time;temp;ax;ay;az;gx;gy;gz

    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent(&accel, &gyro, &temp);

    #if DEBUG == True
    PRINT("IMU;");
    previousTimeLSM = millis();
    PRINT(millis()); PRINT(";"); // ms
    PRINT(temp.temperature); PRINT(";"); // °C

    PRINT(accel.acceleration.x); PRINT(";"); // m.s^-2
    PRINT(accel.acceleration.y); PRINT(";"); // m.s^-2
    PRINT(accel.acceleration.z); PRINT(";"); // m.s^-2

    PRINT(gyro.gyro.x); PRINT(";"); // rad.s^-1
    PRINT(gyro.gyro.y); PRINT(";"); // rad.s^-1
    PRINT(gyro.gyro.z); PRINT("\n"); // rad.s^-1
    // #else
    #endif

  }
}