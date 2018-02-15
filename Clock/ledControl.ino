#include <Adafruit_NeoPixel.h>
#include <time.h>
#include "words.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

typedef struct wordClock {
	uint8_t rows;
	uint8_t columns
	byte grid[ROWS]
}

#define SW(w) setWord(strip, neopixel_grid, w, strip->Color(255,0,0,0));
#define SWC(w, c) setWord(strip, neopixel_grid, w, c);

//TESTING FOR INITIALISATION
void self_test(Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], int rows, int columns, uint32_t color, int d) {
  for(int x = 0; x < rows; x++) {
    for(int y = 0; y < columns; y++) {
      strip->setPixelColor(grid[x][y], color);
      strip->show();
    }
    delay(d);
    for(int y = 0; y < columns; y++) {
      strip->setPixelColor(grid[x][y], 0x00000000);
      strip->show();
    }
  }
}

void setPixelRange(Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], int x, int y, byte pixels, uint32_t color)
{
	Serial.print("pixels: ");
	Serial.println(pixels, BIN);
	for (int i = sizeof(pixels)*8; i > 0; --i) {
		if(pixels & (1 << i)) {
			Serial.print("Pixel: ");
			Serial.print(i);
			Serial.print(" : ");
			Serial.println(pixels & (1 << i));
			//this is working
			// strip->setPixelColor(grid[y][x+i], color);
			strip->setPixelColor(grid[y][x-i], color);
		}
		else {
			//turn the other LEDs, in the range, off
			strip->setPixelColor(grid[y][x-i], 0x00000000);
		}
	}
}

//sets a word defined in words.h
void setWord(Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], clockFace w, uint32_t color) {
  for(int i = 0; i < w.len; i++) {
    strip->setPixelColor(grid[w.x][w.y+i], color);
  }
}