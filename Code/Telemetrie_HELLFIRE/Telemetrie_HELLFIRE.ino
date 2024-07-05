// Heltec Wireless Tracker + LSM6DSOX sensor
// v0.1
// author : Mixcraftio

#include <Adafruit_LSM6DSOX.h>

#define DEBUG
#define GPSSerial Serial1

#define LSM_INTERVAL 10
#define GPS_INTERVAL 1000

Adafruit_LSM6DSOX sox;

uint32_t time;
uint32_t previousTimeLSM = millis();
uint32_t previousTimeGPS = millis();

void setup(void) {
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) delay(10);
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

  #ifdef DEBUG
    Serial.println("Setup done!");
  #endif
}

#ifdef DEBUG
void printRates(){
  Serial.print("Accelerometer range set to: ");
  switch (sox.getAccelRange()) {
    case LSM6DS_ACCEL_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case LSM6DS_ACCEL_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case LSM6DS_ACCEL_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case LSM6DS_ACCEL_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }

  Serial.print("Gyro range set to: ");
  switch (sox.getGyroRange()) {
    case LSM6DS_GYRO_RANGE_125_DPS:
      Serial.println("125 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_250_DPS:
      Serial.println("250 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_500_DPS:
      Serial.println("500 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_1000_DPS:
      Serial.println("1000 degrees/s");
      break;
    case LSM6DS_GYRO_RANGE_2000_DPS:
      Serial.println("2000 degrees/s");
      break;
    case ISM330DHCX_GYRO_RANGE_4000_DPS:
      break; // unsupported range for the DSOX
  }

  Serial.print("Accelerometer data rate set to: ");
  switch (sox.getAccelDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
      Serial.println("0 Hz");
      break;
    case LSM6DS_RATE_12_5_HZ:
      Serial.println("12.5 Hz");
      break;
    case LSM6DS_RATE_26_HZ:
      Serial.println("26 Hz");
      break;
    case LSM6DS_RATE_52_HZ:
      Serial.println("52 Hz");
      break;
    case LSM6DS_RATE_104_HZ:
      Serial.println("104 Hz");
      break;
    case LSM6DS_RATE_208_HZ:
      Serial.println("208 Hz");
      break;
    case LSM6DS_RATE_416_HZ:
      Serial.println("416 Hz");
      break;
    case LSM6DS_RATE_833_HZ:
      Serial.println("833 Hz");
      break;
    case LSM6DS_RATE_1_66K_HZ:
      Serial.println("1.66 KHz");
      break;
    case LSM6DS_RATE_3_33K_HZ:
      Serial.println("3.33 KHz");
      break;
    case LSM6DS_RATE_6_66K_HZ:
      Serial.println("6.66 KHz");
      break;
  }

  Serial.print("Gyro data rate set to: ");
  switch (sox.getGyroDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
      Serial.println("0 Hz");
      break;
    case LSM6DS_RATE_12_5_HZ:
      Serial.println("12.5 Hz");
      break;
    case LSM6DS_RATE_26_HZ:
      Serial.println("26 Hz");
      break;
    case LSM6DS_RATE_52_HZ:
      Serial.println("52 Hz");
      break;
    case LSM6DS_RATE_104_HZ:
      Serial.println("104 Hz");
      break;
    case LSM6DS_RATE_208_HZ:
      Serial.println("208 Hz");
      break;
    case LSM6DS_RATE_416_HZ:
      Serial.println("416 Hz");
      break;
    case LSM6DS_RATE_833_HZ:
      Serial.println("833 Hz");
      break;
    case LSM6DS_RATE_1_66K_HZ:
      Serial.println("1.66 KHz");
      break;
    case LSM6DS_RATE_3_33K_HZ:
      Serial.println("3.33 KHz");
      break;
    case LSM6DS_RATE_6_66K_HZ:
      Serial.println("6.66 KHz");
      break;
  }
  return;
}
#endif

void loop() {
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  #ifdef GPSECHO
    if (c) Serial.print(c);
  #endif
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    // Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  time = millis();
  if (time - previousTimeGPS >= GPS_INTERVAL) {
    // "GPS";arduinotime;fix;quality;time;date;locationlat;locationlng;speed;angle;altitude;satellites

    Serial.print("GPS;");
    Serial.print(millis()); Serial.print(";");
    Serial.print((int)GPS.fix); Serial.print(";");
    Serial.print((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print(";");
      if (GPS.hour < 10) { Serial.print('0'); }
      Serial.print(GPS.hour, DEC); Serial.print(':');
      if (GPS.minute < 10) { Serial.print('0'); }
      Serial.print(GPS.minute, DEC); Serial.print(':');
      if (GPS.seconds < 10) { Serial.print('0'); }
      Serial.print(GPS.seconds, DEC); Serial.print('.');
      if (GPS.milliseconds < 10) {
        Serial.print("00");
      } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
        Serial.print("0");
      }
      Serial.print(GPS.milliseconds);
      Serial.print(";");
      Serial.print(GPS.day, DEC); Serial.print('/');
      Serial.print(GPS.month, DEC); Serial.print("/20");
      Serial.print(GPS.year, DEC);
      Serial.print(";");
      Serial.print(GPS.latitudeDegrees, 10);
      // Serial.print(GPS.lat); (N or S)
      Serial.print(";");
      Serial.print(GPS.longitudeDegrees, 10);
      // Serial.print(GPS.lon); (W or E)
      Serial.print(";");
      Serial.print(GPS.speed);
      Serial.print(";");
      Serial.print(GPS.angle);
      Serial.print(";");
      Serial.print(GPS.altitude);
      Serial.print(";");
      Serial.print((int)GPS.satellites);
      Serial.print(";");
      Serial.print((int)GPS.antenna);
    }
    Serial.print("\n");

    previousTimeGPS = millis();
  }

  time = millis();
  if (time - previousTimeLSM >= LSM_INTERVAL) {
    // "IMU";time;temp;ax;ay;az;gx;gy;gz

    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent(&accel, &gyro, &temp);

    #ifdef DEBUG
    Serial.print("IMU;");
    Serial.print(millis()); Serial.print(";"); // ms
    Serial.print(temp.temperature); Serial.print(";"); // °C

    Serial.print(accel.acceleration.x); Serial.print(";"); // m.s^-2
    Serial.print(accel.acceleration.y); Serial.print(";"); // m.s^-2
    Serial.print(accel.acceleration.z); Serial.print(";"); // m.s^-2

    Serial.print(gyro.gyro.x); Serial.print(";"); // rad.s^-1
    Serial.print(gyro.gyro.y); Serial.print(";"); // rad.s^-1
    Serial.print(gyro.gyro.z); Serial.print("\n"); // rad.s^-1
    // #else
    #endif

    previousTimeLSM = millis();
  }
}