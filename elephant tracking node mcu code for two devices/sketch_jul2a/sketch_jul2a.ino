#include <ESP8266WiFi.h>
#include <ESP8266Firebase.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// === WiFi & Firebase Config ===
#define WIFI_SSID "TCL20Y"
#define WIFI_PASSWORD "F55BuIvV"
#define FIREBASE_URL "https://elephant-tracker-44677-default-rtdb.firebaseio.com/"
Firebase firebase(FIREBASE_URL);

// The pins you connected the GPS TX/RX to
static const int RXPin = 4;  // NodeMCU D2 (GPIO4)
static const int TXPin = 5;  // NodeMCU D1 (GPIO5)

// === Device ID ===
#define DEVICE_ID "E01"

// -- GPS Baud Rate --
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

// Create a SoftwareSerial object to communicate with the GPS
SoftwareSerial ss(RXPin, TXPin);

// // === NTP Client Setup ===
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // UTC+5:30

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // Start the software serial for the GPS
  ss.begin(GPSBaud);
  Serial.println("Booting...");

  Serial.println("NodeMCU NEO-M8N GPS Test");
  Serial.println("Searching for GPS satellites...");
  
  timeClient.begin();
  timeClient.update();

  //Define the path for firebase realtime database
  firebase.setString("/elephants/" + String(DEVICE_ID) + "/id", DEVICE_ID);
  Serial.println("Firebase initialized");
}

void loop() {

   while (ss.available() > 0) {
    gps.encode(ss.read());
  }

    // If no GPS data is received for 5 seconds, print a message
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS data detected: check wiring or antenna placement.");
    while(true);
  }
  

  if (gps.location.isValid()) {
    float lat = gps.location.lat();
    float lng = gps.location.lng();

    timeClient.update();
    String formattedTime = timeClient.getFormattedTime();
    unsigned long epoch = timeClient.getEpochTime();

    String basePath = "/elephants/" + String(DEVICE_ID) + "/position";

    firebase.setFloat(basePath + "/lat", lat);
    firebase.setFloat(basePath + "/lng", lng);
    firebase.setString(basePath + "/time", formattedTime);
    firebase.setInt(basePath + "/timestamp", epoch);

    Serial.println("Lat: " + String(lat, 6));
    Serial.println("Lng: " + String(lng, 6));
    Serial.println("Time: " + formattedTime);
    Serial.println("------------------------");


  } else {
    Serial.println("Waiting for GPS fix...");

  }
}
