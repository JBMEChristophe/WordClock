#include <Adafruit_NeoPixel.h>
#include <time.h>
#include "words.h"


#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 3

#define NUM_LEDS 110
#define ROWS 10
#define COLUMNS 11
#define BRIGHTNESS 255

Adafruit_NeoPixel strip;

const byte neopixel_grid[10][11] = {
 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
 { 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11 },
 { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 },
 { 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33 },
 { 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54 },
 { 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55 },
 { 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76 },
 { 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77 },
 { 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98 },
 { 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99 }
};


struct tm t;

void setup() {
  strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);
  
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);

  //perform a self to check whether all the LEDS are working.
  self_test(&strip, neopixel_grid, ROWS, COLUMNS, 0xFFFFFFFF, 50); 
  
  t.tm_sec = 0;
  t.tm_min = 50;
  t.tm_hour = 11;
}

void loop() {
    strip.clear();

    float temp = 73.0;

    displayTemperature(&strip, neopixel_grid, temp, strip.Color(128,0,128,0));
    //displayTime(&t, &strip, neopixel_grid, 0x00FF0000);

    strip.show();

    // t.tm_min += 1;
    // if(t.tm_min == 60) { 
    //   t.tm_min = 0; 
    //   t.tm_hour += 1;
    // }
    // if (t.tm_hour == 24) {
    //   t.tm_hour = 0;
    // }
    delay(5000); 
  }

