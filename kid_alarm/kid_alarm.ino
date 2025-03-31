/*******************************************************************
    TFT_eSPI button example for the ESP32 Cheap Yellow Display.

    https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display

    Written by Claus Näveke
    Github: https://github.com/TheNitek
 *******************************************************************/

// Make sure to copy the UserSetup.h file into the library as
// per the Github Instructions. The pins are defined in there.

// ----------------------------
// Standard Libraries
// ----------------------------

#include <SPI.h>


#include <Time.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>

#include ".env.h"
#include "pumbaa_asleep.h"
#include "pumbaa_hat.h"

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <XPT2046_Bitbang.h>
// A library for interfacing with the touch screen
//
// Can be installed from the library manager (Search for "XPT2046 Slim")
// https://github.com/TheNitek/XPT2046_Bitbang_Arduino_Library

#include <TFT_eSPI.h>
// A library for interfacing with LCD displays
//
// Can be installed from the library manager (Search for "TFT_eSPI")
// https://github.com/Bodmer/TFT_eSPI


int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiUDP ntpUDP;
// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);
// ----------------------------
// Touch Screen pins
// ----------------------------

// The CYD touch uses some non default
// SPI pins

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
// ----------------------------

#define SCREEN_W 240
#define SCREEN_H 320

void printWifiData();
void printCurrentNet();

XPT2046_Bitbang ts(XPT2046_MOSI, XPT2046_MISO, XPT2046_CLK, XPT2046_CS);

TFT_eSPI tft = TFT_eSPI();

TFT_eSPI_Button key[6];

void display_awake() {
  tft.pushImage((SCREEN_W-130)/2,30, 130,150, pumbaa_hat);
}

void display_asleep() {
  tft.pushImage((SCREEN_W-119)/2,30, 119,150, pumbaa_asleep);
}


void update_screen() {

  int day = timeClient.getDay()%7;
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  float time = ((float) hours) + ((float) minutes) / 100.0;
  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);
  if(day>=1 || day<=4) { // monday to thursday
    if (time > 6.50 && time < 19.45) {
      display_awake();      
    } else {
      display_asleep();
    }
  } else if(day == 0) { // sunday
    if (time > 7.30 && time < 19.45) {
      display_awake();      
    } else {
      display_asleep();
    }  
  } else if(day == 5) { // friday
    if (time > 6.50 && time < 20.15) {
      display_awake();      
    } else {
      display_asleep();
    }  
  } else if(day == 6) { // saturday
    if (time > 7.30 && time < 20.15) {
      display_awake();      
    } else {
      display_asleep();
    }  
  }

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextDatum(BC_DATUM);
  int y = 250;
  int line_height = 30;
  tft.drawString("THIBAULT", SCREEN_W/2, y, 1); // Left Aligned
  y += line_height;
  char *days[] = {"DIMANCHE", "LUNDI", "MARDI", "MERCREDI", "JEUDI","VENDREDI","SAMEDI"};
  tft.drawString(days[timeClient.getDay()%7], SCREEN_W/2, y, 1); // Left Aligned
  y += line_height;
  char time_str[10];
  sprintf(time_str, "%02d:%02d", timeClient.getHours(), timeClient.getMinutes());
  tft.drawString(time_str, SCREEN_W/2, y, 1); // Left Aligned
}

void initBrightness() {
    pinMode(21, OUTPUT);
}

void setBrightness(int percent) {
  int command = percent * 255 / 100;
  analogWrite(21, command);
}

int getTimeOffsetSeconds() {
  int offset_s = 3600; // Default offset for CET (Central European Time)

  String location_x = "4.8";
  String location_y = "45.5";

  long int timestamp = (long int) timeClient.getEpochTime();

  HTTPClient http;
  char url[512];
  sprintf(url, 
    "https://maps.googleapis.com/maps/api/timezone/json?location=%s,%s&timestamp=%d&key=%s",
    location_y, 
    location_x,
    timestamp,
    google_api_key);
  Serial.println(url);
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println(payload);
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      offset_s = doc["rawOffset"].as<int>() + doc["dstOffset"].as<int>();
    } else {
      Serial.println("Failed to parse JSON");
    }
  } else {
    Serial.println("HTTP request failed");
  }

  http.end();
  return offset_s; 
}

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0); //This is the display in landscape

  // Connect to WPA/WPA2 network:
  Serial.println("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  delay(5000);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("wifi status");
    Serial.println(WiFi.status());
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  timeClient.begin();
  
  timeClient.update();

  timeClient.setTimeOffset(getTimeOffsetSeconds());

  initBrightness();
  setBrightness(20);
}


void loop() {
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  update_screen();

  delay(20*1000);
}