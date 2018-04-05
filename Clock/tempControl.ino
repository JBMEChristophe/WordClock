#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <string.h>
#include "numbers.h"


uint8_t* cutTemp(float celsius) {
	if(celsius > 99 || celsius < -99){
		return NULL;
	}
	static uint8_t t[2];


	celsius = fabs(celsius);

	t[0] = (uint8_t)roundf(celsius) / 10;
	t[1] = (uint8_t)roundf(celsius) % 10;

	return t;
}

void showNumber(wordClock* clock, uint8_t numberToDisplay, uint8_t wordHeight, uint8_t xOffset, uint32_t color) {
	for (uint8_t i = 0; i < wordHeight; ++i) {
		setPixelRange(clock, xOffset, wordHeight - i - 1, NUMBERS[numberToDisplay][i], color);
	}
}

uint8_t displayTemperature(wordClock* clock, float temperature, uint32_t color) {
	Serial.println("temperature from displaytemperature:" +  String(temperature));
	uint8_t* temp = cutTemp(temperature);
	if(temp == NULL){
		return 1;
	}
	Serial.println("in displayTemperature");
	//draw a minus symbol above the numbers
	if(temperature < 0)	{
		clock->strip->setPixelColor(clock->grid[9][0], color);
		clock->strip->setPixelColor(clock->grid[9][1], color);
		clock->strip->setPixelColor(clock->grid[9][2], color);
	}
	else{
		clock->strip->setPixelColor(clock->grid[9][0], 0x00000000);
		clock->strip->setPixelColor(clock->grid[9][1], 0x00000000);
		clock->strip->setPixelColor(clock->grid[9][2], 0x00000000);
	}

	if (temp[0] != 0) {
	 	showNumber(clock, temp[0], 8, 0, color);
	}
	showNumber (clock, temp[1], 8, 6, color);
	return 0;
}