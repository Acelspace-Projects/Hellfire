
#include <Adafruit_GPS.h>

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Comment to turn off echoing the GPS data to the Serial console
// Uncomment if you want to debug and listen to the raw GPS sentences
// #define GPSECHO

uint32_t timer = millis();


void setup()
{
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic parsing test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

void loop() // run over and over again
{
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

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer

    // "GPS";fix;quality;time;date;locationlat;locationlng;speed;angle;altitude;satellites

    Serial.print("GPS;");
    Serial.print((int)GPS.fix);
    Serial.print(";");
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
      // Serial.print(GPS.lat);
      Serial.print(";");
      Serial.print(GPS.longitudeDegrees, 10);
      // Serial.print(GPS.lon);
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
  }
}