#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Define the RX and TX pins for SoftwareSerial
// NodeMCU D1 (GPIO5) -> GPS TX (NodeMCU will transmit to GPS)
// NodeMCU D2 (GPIO4) -> GPS RX (NodeMCU will receive from GPS)
static const int RXPin = D4, TXPin = D5; // Corrected for common usage. D4 is GPIO2, D5 is GPIO14 on ESP8266.
                                        // Let's use D2 (GPIO4) and D1 (GPIO5) as per the wiring diagram for clarity.
                                        // D2 (GPIO4) -> GPS RX (NodeMCU receives)
                                        // D1 (GPIO5) -> GPS TX (NodeMCU transmits - though not strictly needed for just reading)

// Let's re-align the pins for clearer NodeMCU to GPS connection based on common advice:
// NodeMCU D2 (GPIO4) as SoftwareSerial RX (GPS TX -> D2)
// NodeMCU D1 (GPIO5) as SoftwareSerial TX (NodeMCU D1 -> GPS RX)
static const int GPS_RX_PIN = D2; // Connect GPS TX to NodeMCU D2 (GPIO4)
static const int GPS_TX_PIN = D1; // Connect GPS RX to NodeMCU D1 (GPIO5) - needed if you want to send commands to GPS

static const long GPSBaud = 9600; // Most common baud rate for Neo-M8N/Neo-6M

// Create SoftwareSerial object for GPS communication
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN); // RX, TX

// Create TinyGPS++ object
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200); // Standard baud rate for Serial Monitor
  ss.begin(GPSBaud);     // Start SoftwareSerial communication with GPS

  Serial.println(F("NodeMCU GPS Logger with Neo-M8N/Neo-6M"));
  Serial.println(F("Waiting for GPS data..."));
  Serial.println(F("Ensure GPS module has a clear view of the sky."));
}

void loop() {
  // While there is data available from the GPS module
  while (ss.available() > 0) {
    // Feed the NMEA characters to the TinyGPS++ library
    if (gps.encode(ss.read())) {
      // If a complete GPRMC or GPGGA sentence has been successfully parsed,
      // you can now access the data.
      displayInfo();
    }
  }

  // If no new data is available for a while, TinyGPS++ can tell you if it's stale
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS data received: check wiring."));
    while(true); // Halt execution if no data
  }
}

void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6); // Latitude with 6 decimal places
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6); // Longitude with 6 decimal places
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Altitude: "));
  if (gps.altitude.isValid()) {
    Serial.print(gps.altitude.meters());
    Serial.print(F("m"));
  } else {
    Serial.print(F("INVALID"));
  }


  Serial.print(F("  Satellites: "));
  if (gps.satellites.isValid()) {
    Serial.print(gps.satellites.value());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println(); // New line for the next set of data
}