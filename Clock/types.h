#ifndef TYPES_H
#define TYPES_H

#include <Adafruit_NeoPixel.h>
#include "configuration.h"

typedef struct letter
{
	byte b0 : 1;
	byte b1 : 1;
	byte b2 : 1;
	byte b3 : 1;
	byte b4 : 1;
} letter;

typedef struct wordClock {
	int rows;
	int columns;
	byte grid[ROWS][COLUMNS];
	Adafruit_NeoPixel* strip;
} wordClock;

typedef struct clockFace{
    uint8_t y;
    uint8_t x;
    uint8_t len;
} clockFace;

#endif //TYPES_H