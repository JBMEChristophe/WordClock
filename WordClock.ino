#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 3
#define BRIGHTNESS 32

#define ROWLENGTH 11
#define COLORS 4

Adafruit_NeoPixel strip;



void setup()
{
	strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

	strip.setBrightneww(BRIGHTNESS);
	strip.begin();
	strip.show();
}

void loop()
{

}

