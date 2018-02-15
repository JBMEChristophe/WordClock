#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <string.h>
#include "numbers.h"


uint8_t* cutTemp(float temperature) {
	if(temperature > 99 || temperature < -99){
		return NULL;
	}
	static uint8_t t[2];


	temperature *= 1;

	t[0] = (uint8_t)roundf(temperature) / 10;
	t[1] = (uint8_t)roundf(temperature) % 10;

	return t;
}

void showNumber(wordClock* clock, uint8_t numberToDisplay, uint8_t wordHeight, uint8_t xOffset, uint32_t color) {
	for (uint8_t i = 0; i < wordHeight; ++i) {
		setPixelRange(clock, xOffset, wordHeight-i, IMAGES[numberToDisplay][i], color);
	}
}

void displayTemperature(wordClock* clock, float temperature, uint32_t color) {
	uint8_t* temp = cutTemp(temperature);
	//draw a minus symbol above the numbers
	if(temperature < 0)	{
		SWC(((clockFace){0,10,5}), color);
	}

	if (temp[0] != 0) {
	 	showNumber(clock, temp[0], 8, -4, color);
	}
	showNumber (clock, temp[1], 8, 2, color);
}