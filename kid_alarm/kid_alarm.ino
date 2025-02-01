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

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0); //This is the display in landscape
  #if 0
  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);
  
  tft.pushImage((SCREEN_W-119)/2,30, 119,150, pumbaa_asleep);
  tft.pushImage((SCREEN_W-130)/2,30, 130,150, pumbaa_hat);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextDatum(BC_DATUM);
  int y = 250;
  int line_height = 30;
  tft.drawString("THIBAULT", SCREEN_W/2, y, 1); // Left Aligned
  y += line_height;
  tft.drawString("SAMEDI", SCREEN_W/2, y, 1); // Left Aligned
  y += line_height;
  tft.drawString("14:50", SCREEN_W/2, y, 1); // Left Aligned
  #endif

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
  Serial.print("You're connected to the network");

  timeClient.begin();
  
  timeClient.setTimeOffset(3600);
  
}


void loop() {
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  time_t cet = timeClient.getEpochTime();

  // int day_of_week = ((cet / (3600*24)) + 4) % 7;
  Serial.println("day");
  Serial.println(timeClient.getDay());

  Serial.println("hour");
  Serial.println(timeClient.getHours());

  Serial.println("minutes");
  Serial.println(timeClient.getMinutes());


  delay(5000);
}