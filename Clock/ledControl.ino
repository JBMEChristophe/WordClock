#include <Adafruit_NeoPixel.h>
#include <time.h>
#include "numbers.h"
#include "words.h"
#include "types.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define SW(w) setWord(clock, w, strip->Color(255,0,0,0));
#define SWC(w, c) setWord(clock, w, c);

//TESTING FOR INITIALISATION
void self_test(wordClock* clock, uint32_t color, int ms) {
  for(int x = 0; x < clock->rows; x++) {
    for(int y = 0; y < clock->columns; y++) {
      clock->strip->setPixelColor(clock->grid[x][y], color);
      clock->strip->show();
    }
    delay(ms);
    for(int y = 0; y < clock->columns; y++) {
      clock->strip->setPixelColor(clock->grid[x][y], 0x00000000);
      clock->strip->show();
    }
  }
}

void setPixelRange(wordClock* clock, int x, int y, byte pixels, uint32_t color)
{
	for (int i = 3; i <= 7; i++) {

    //this may seem like magic
    //5 is the width of the letter
    //x is of offset accross the screen
    //3 is offset caused by the matrix
    //1 is a correction because arrays
    int pixelLoc = 5+x-i+3-1;
		if(pixels & (1 << i)) {
			//this is working
			clock->strip->setPixelColor(clock->grid[y][pixelLoc], color);
      Serial.print(1);
		}
		else {
			//turn the other LEDs, in the range, off
			clock->strip->setPixelColor(clock->grid[y][pixelLoc], 0x00000000);
      Serial.print(0);
		}
	}
  Serial.println();
}

//sets a word defined in words.h
void setWord(wordClock* clock, clockFace w, uint32_t color) {
  for(int i = 0; i < w.len; i++) {
    clock->strip->setPixelColor(clock->grid[w.x][w.y+i], color);
  }
}

uint32_t liminousityCorrection(wordClock* clock, uint8 red, uint8 green, uint8 blue, uint8 white){
  float RMCDavg = ((R_MCD_L+R_MCD_H)/2);
  float GMCDavg = ((G_MCD_L+G_MCD_H)/2);
  float BMCDavg = ((B_MCD_L+B_MCD_H)/2);

  long Cgreen = green * RMCDavg;
  long Cred = red * GMCDavg;
  long Cblue = blue * BMCDavg;
  uint8_t lcred   = (uint8_t)map(Cred, 0, 255*RMCDavg, 0, 255);
  uint8_t lcgreen = (uint8_t)map(Cgreen, 0, 255*GMCDavg, 0, 255);
  uint8_t lcblue = (uint8_t)map(Cblue, 0, 255*BMCDavg, 0, 255);

  return clock->strip->Color(lcred,lcblue,lcgreen,white);
}

 uint8_t gammaCorrection(wordClock* clock, uint8 red, uint8 green, uint8 blue, uint8 white){
  float Cred = pow(red/255, 1/clock->strip->gamma8(1)) * 255;
  float Cgreen = pow(green/255, 1/clock->strip->gamma8(1)) * 255;
  float Cblue = pow(blue/255, 1/clock->strip->gamma8(1)) * 255;
  float Cwhite = pow(white/255, 1/clock->strip->gamma8(1)) * 255;
  uint8_t c = (Cred + Cgreen + Cblue + Cwhite) / 4;
  return c;
}
