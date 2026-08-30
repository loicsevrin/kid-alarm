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

#define awake_status_unknown 0 
#define awake_status_awake 1 
#define awake_status_asleep 2 

int awake_status = awake_status_unknown;

void display_awake_alarm_on_wake_up() {
  tft.pushImage((SCREEN_W-130)/2,30, 130,150, pumbaa_hat);
  if (awake_status == awake_status_asleep) {
    play_alarm();
  }
  awake_status = awake_status_awake;
}

void display_awake_no_alarm() {
  tft.pushImage((SCREEN_W-130)/2,30, 130,150, pumbaa_hat);
  awake_status = awake_status_awake;
}


void display_asleep() {
  tft.pushImage((SCREEN_W-119)/2,30, 119,150, pumbaa_asleep);
  if (awake_status == awake_status_awake) {
  }
  awake_status = awake_status_asleep;
}

void play_alarm() {
  for(int i=0; i<5; i++) {
    magic();
    delay(1000);
  }
}

void update_screen() {

  int day = timeClient.getDay()%7;
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  float time = ((float) hours) + ((float) minutes) / 100.0;
  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);
  display_time();
  if(day>=1 || day<=5) { // monday to thursday
    if (time > 7.15 && time < 19.30) {
      display_awake_alarm_on_wake_up();
    } else {
      display_asleep();
    }
  } else if(day == 0 || day == 6) { // saturday & sunday
    if (time > 9.00 && time < 19.30) {
      display_awake_no_alarm();      
    } else {
      display_asleep();
    }  
  }
}

void display_time() {
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

  bool res = timeClient.update();
  if (!res) {
    Serial.println("Failed to update time in get time offset");
  }

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

void connectToWifi() {
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
}


const int buzzer_gpio = 22;

void play1(char *note, int nb_demi_temps) {
  // mettre le la en bas
  int freq = 2000;
  if(strcmp(note, "do") == 0) {
    freq = 261;
  } else if(strcmp(note, "re") == 0) {
    freq = 293;
  } else if(strcmp(note, "mi") == 0) {
    freq = 329;
  } else if(strcmp(note, "fa") == 0) {
    freq = 349;
  } else if(strcmp(note, "sol") == 0) {
    freq = 392;
  } else if(strcmp(note, "la") == 0) {
    freq = 440;
  } else if(strcmp(note, "si") == 0) {
    freq = 493;
  }
  tone(buzzer_gpio, freq);   // 1 kHz tone
  // Serial.println("buzzer %s %d", note, freq);
  int base_temps_ms = 200;
  delay(nb_demi_temps * base_temps_ms);
  noTone(buzzer_gpio);   // 1 kHz tone
  delay(base_temps_ms / 5);
}



void play(char *note, int nb_demi_temps) {
  int freq = 2000;
  if(strcmp(note, "la") == 0) {
    freq = 220;
  } else if(strcmp(note, "si") == 0) {
    freq = 246;
  } else if(strcmp(note, "do") == 0) {
    freq = 261;
  } else if(strcmp(note, "re") == 0) {
    freq = 293;
  } else if(strcmp(note, "mi") == 0) {
    freq = 329;
  } else if(strcmp(note, "fa") == 0) {
    freq = 349;
  } else if(strcmp(note, "sol") == 0) {
    freq = 392;
  }
  if(strcmp(note, "") != 0) {
   tone(buzzer_gpio, freq);   // 1 kHz tone
  }
  // Serial.println("buzzer %s %d", note, freq);
  int base_temps_ms = 200;
  delay(nb_demi_temps * base_temps_ms);
  noTone(buzzer_gpio);   // 1 kHz tone
  delay(base_temps_ms / 5);
}




void magic() {
  play("la", 2);
  play("la", 1);
  play("do", 1);
  play("do", 1);
  play("mi", 1);
  play("mi", 1);
  play("fa", 2);
  play("", 1);
  play("fa", 1);
  play("fa", 1);
  play("fa", 1);
  play("re", 1);
  play("re", 1);
  play("mi", 1);
  play("mi", 1);
  play("mi", 1);
  play("mi", 1);
  play("mi", 2);
  play("do", 2);
  play("re", 2);
  play("", 1);
  play("re", 1);
  play("re", 1);
  play("mi", 1);
  play("re", 1);
  play("do", 1);
  play("la", 1);
}


void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0); //This is the display in landscape

  connectToWifi();

  timeClient.begin();
  
  timeClient.setTimeOffset(getTimeOffsetSeconds());

  initBrightness();
  setBrightness(20);

  WiFi.disconnect();
}


void loop() {
  
  static int i = 0;
  i++;
  int delay_hours = 12;
  int hours = timeClient.getHours();
  if(i > 3 * 60 * delay_hours && hours > 10 && hours < 18) {
    i = 0;
    connectToWifi();

    bool res = timeClient.update();
    if (!res) {
      Serial.println("Failed to update time");
    } else {
      Serial.println("Time updated");
    }

    WiFi.disconnect();
  }
  Serial.println(timeClient.getFormattedTime());

  update_screen();

  delay(20*1000);
}