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

XPT2046_Bitbang ts(XPT2046_MOSI, XPT2046_MISO, XPT2046_CLK, XPT2046_CS);

TFT_eSPI tft = TFT_eSPI();

TFT_eSPI_Button key[6];

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0); //This is the display in landscape
  
  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int x = 5;
  int y = 10;
  int fontNum = 2; 
  tft.drawString("THIBAULT", x, y, fontNum); // Left Aligned
  x = 320 /2;
  y += 16;
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawCentreString("NOEMIE", x, y, fontNum);

  #define SIZE 2000 
  uint8_t bitmap[SIZE];

  // rouge bleu rouge bleu changement tous les 5 pixels
  // for(int i = 0; i<SIZE-2; i+=2) {
  //   bitmap[i] = 0xE0;
  //   bitmap[i+1] = 0x0F;
  // }

 
  // for(int i = 0; i<SIZE-3; i+=3) {
  //   bitmap[i] = 0xE0;
  //   bitmap[i+1] = 0x0F;
  //   bitmap[i+2] = 0xE0;
  // }

  for(int i = 0; i<SIZE-3; i+=3) {
    bitmap[i] = 0b00011000;
    bitmap[i+1] = 0b00011000;
    bitmap[i+2] = 0b00011000;
  }

  //tft.drawBitmap( 10, 10, bitmap, 200, 1, 0b1111100000000000, 0b0000000000011111);

  #define SIZE_16 2000 
  uint16_t image_16[SIZE_16];

  for(int i = 0; i<SIZE_16-3; i+=3) {
    image_16[i] = 0x1F << 11;
    image_16[i+1] = 0x1F << 5;
    image_16[i+2] = 0x1F << 0;
  }


  // tft.pushImage(10, 10, 100, 200, simba);

  // tft.pushImage(0,0, 215,170, pumbaa_asleep);
  tft.pushImage(0,0, 200,230, pumbaa_hat);

  //tft.image(logo, x, y);
}


void drawButtons() {
  uint16_t bWidth = TFT_HEIGHT/3;
  uint16_t bHeight = TFT_WIDTH/2;
  // Generate buttons with different size X deltas
  for (int i = 0; i < 6; i++) {
    key[i].initButton(&tft,
                      bWidth * (i%3) + bWidth/2,
                      bHeight * (i/3) + bHeight/2,
                      bWidth,
                      bHeight,
                      TFT_BLACK, // Outline
                      TFT_BLUE, // Fill
                      TFT_BLACK, // Text
                      "",
                      1);

    key[i].drawButton(false, String(i+1));
  }
}

void loop() {
}