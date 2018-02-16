#include <Adafruit_NeoPixel.h>
#include <time.h>
#include "types.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define SW(w) setWord(clock, w, strip->Color(255,0,0,0));
#define SWC(w, c) setWord(clock, w, c);

//TESTING FOR INITIALISATION
void self_test(wordClock* clock, int rows, int columns, uint32_t color, int ms) {
  for(int x = 0; x < rows; x++) {
    for(int y = 0; y < columns; y++) {
      clock->strip->setPixelColor(clock->grid[x][y], color);
      clock->strip->show();
    }
    delay(ms);
    for(int y = 0; y < columns; y++) {
      clock->strip->setPixelColor(clock->grid[x][y], 0x00000000);
      clock->strip->show();
    }
  }
}

void setPixelRange(wordClock* clock, int x, int y, byte pixels, uint32_t color)
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
			clock->strip->setPixelColor(clock->grid[y][x-i], color);
		}
		else {
			//turn the other LEDs, in the range, off
			clock->strip->setPixelColor(clock->grid[y][x-i], 0x00000000);
		}
	}
}

//sets a word defined in words.h
void setWord(wordClock* clock, clockFace w, uint32_t color) {
  for(int i = 0; i < w.len; i++) {
    clock->strip->setPixelColor(clock->grid[w.x][w.y+i], color);
  }
<<<<<<< HEAD
}
=======
}

void handle_hours(struct tm* t, Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], uint32_t color)
{
  int minutes = t->tm_min - (t->tm_min % 5); //floor to nearest 5
  int hours   = t->tm_hour;
  if(minutes > 15) {
    hours += 1;
  }

  if(hours > 12) {
      hours -= 12;
  }

  if(hours - 1 < 0) { hours = 12; }
  Serial.print("hours ");
  Serial.println(hours);
  SWC(NUMBERS[hours -1], color);
}

void displayTime(struct tm* t, Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], uint32_t color)
{
  SWC(w_IT, color);
  SWC(w_IS, color);
  handle_minutes(t, strip, grid, color);
  handle_minutes_spare(t, strip, grid, strip->Color(0,0,255,0));
  handle_hours(t, strip, grid, color);
}
>>>>>>> parent of 6bc0879... [BUG] hours
