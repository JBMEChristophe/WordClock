#include <Adafruit_NeoPixel.h>
#include <time.h>
#include "types.h"
#include "configuration.h"

#ifdef __AVR__
  #include <avr/power.h>
#elif ESP8266
  #include "WifiLocation.h"
#endif


Adafruit_NeoPixel strip;

#ifdef ESP8266
  const String googleApiKey = "AIzaSyDnDX92K9ZC6eTqhDzHmzCltHPHuRT6MFM";
  const String owmApiKey = "d816a08dddeb2df937174ddcd3d4b5a3";
  WifiLocation location(googleApiKey);
#endif


byte neopixel_grid[ROWS][COLUMNS] = {
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

wordClock wclock;
struct tm t;  

void setup() {
  //strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

  delay(1000);
  
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(115200);
  
  wclock.rows = ROWS;
  wclock.columns = COLUMNS;
  wclock.strip = &strip;
  byte* rtn = (byte*)memcpy(wclock.grid, neopixel_grid, sizeof(neopixel_grid[0][0])*COLUMNS*ROWS);
  if(rtn == NULL){
    Serial.println("FATAL ERROR system out of memory!");
  }

  //perform a self to check whether all the LEDS are working.
  Serial.println("--------------PERFORMING SELFTEST---------------");
  self_test(&wclock, ROWS, COLUMNS, 0xFFFFFFFF, 50); 
  Serial.println("--------------FINISHING SELFTEST----------------");
}

void loop() {
    strip.clear();

    //float temperature = 73.0;

    //displayTemperature(&wclock, temperature, strip.Color(128,0,128,0));
    //displayTemperature(&strip, neopixel_grid, temperature, strip.Color(128,0,128,0));
    //displayTime(&t, &strip, neopixel_grid, 0x00FF0000);

    strip.show();

    delay(5000); 
  }

