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

typedef struct clockFaceColor{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t w;
	uint32_t c;
} clockFaceColor;

typedef struct clockFace{
    uint8_t y;
    uint8_t x;
    uint8_t len;
} clockFace;

enum display_state {
	Time,
	Temperature
};

struct state_times {
	enum display_state state;
	uint8_t start;
	uint8_t end;
	void (*displayFunction)();
}state_times

#endif //TYPES_H