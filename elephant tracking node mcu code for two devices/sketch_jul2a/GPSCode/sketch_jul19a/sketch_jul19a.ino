#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// -- Pin Definitions --
// The pins you connected the GPS TX/RX to
static const int RXPin = 4;  // NodeMCU D2 (GPIO4)
static const int TXPin = 5;  // NodeMCU D1 (GPIO5)

// -- GPS Baud Rate --
// The default baud rate for most NEO-M8N modules is 9600
static const uint32_t GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a SoftwareSerial object to communicate with the GPS
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  // Start the hardware serial for debugging
  Serial.begin(9600);
  
  // Start the software serial for the GPS
  ss.begin(GPSBaud);

  Serial.println("NodeMCU NEO-M8N GPS Test");
  Serial.println("Searching for GPS satellites...");
}

void loop() {
  // This is the magic loop that feeds characters from the GPS to the TinyGPS++ object.
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      displayInfo();
    }
  }

  // If no GPS data is received for 5 seconds, print a message
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS data detected: check wiring or antenna placement.");
    while(true); // Halt the program
  }
}

void displayInfo() {
  Serial.print(F("Location: ")); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6); // Latitude in degrees (double)
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6); // Longitude in degrees (double)
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  |  Satellites: "));
  if (gps.satellites.isValid()) {
    Serial.print(gps.satellites.value());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  |  Altitude: "));
  if (gps.altitude.isValid()) {
    Serial.print(gps.altitude.meters());
    Serial.print(F("m"));
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
  delay(500);
}