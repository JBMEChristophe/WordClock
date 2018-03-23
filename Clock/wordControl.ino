#include <Adafruit_NeoPixel.h>

#include "words.h"
#include "numbers.h"
#include <time.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

void handle_minutes(struct tm* t, wordClock* clock, uint32_t color) {
    int minutes = t->tm_min - (t->tm_min % 5); //floor to nearest 5

    switch(minutes) {
      case 0:
        SWC(w_OCLOCK, color);
        break;
      case 5:
        SWC(m_FIVE, color);
        SWC(s_PAST, color);
        break;
      case 10:
        SWC(m_TEN, color);
        SWC(s_PAST, color);
        break;
      case 15:
        SWC(m_QUARTER, color);
        SWC(h_PAST, color);
        break;
      case 20:
        SWC(m_TEN, color);
        SWC(s_TO, color);
        SWC(s_HALF, color);
        break;
      case 25:
        SWC(m_FIVE, color);
        SWC(s_TO, color);
        SWC(s_HALF, color);
        break;
      case 30:
        SWC(s_HALF, color);
        break;
      case 35:
        SWC(m_FIVE, color);
        SWC(s_PAST, color);
        SWC(s_HALF, color);
        break;
      case 40:
        SWC(m_TEN, color);
        SWC(s_PAST, color);
        SWC(s_HALF, color);
        break;
      case 45:
        SWC(m_QUARTER, color);
        SWC(h_TO, color);
        break;
      case 50:
        SWC(m_TEN, color);
        SWC(s_TO, color);
        break;
      case 55:
        SWC(m_FIVE, color);
        SWC(s_TO, color);
        break;
    }
}

void handle_minutes_spare(struct tm* t, wordClock* clock, uint32_t color)
{
  int mod_min = t->tm_min % 5;

  switch (mod_min) {
  case 4:
    clock->strip->setPixelColor(clock->grid[4][6], color);
  case 3:
    clock->strip->setPixelColor(clock->grid[4][4], color);
  case 2:
    clock->strip->setPixelColor(clock->grid[5][6], color);
  case 1:
    clock->strip->setPixelColor(clock->grid[5][4], color);
    break;
  default:
    break;
  }
}

void handle_hours(struct tm* t, wordClock* clock, uint32_t color)
{
  int minutes = t->tm_min - (t->tm_min % 5); //floor to nearest 5
  int hours   = t->tm_hour;

  if (hours == 0) {
    hours = 12;
  }
  if (minutes > 15 ) {
       hours++;
  }
  if (hours > 12) {
    hours -= 12;
  }

  SWC(TEXT_NUMBERS[hours -1], color);
}

void displayTime(struct tm* t, wordClock* clock, uint32_t color)
{
  SWC(w_IT, color);
  SWC(w_IS, color);
  handle_minutes(t, clock, color);
  handle_minutes_spare(t, clock, clock->strip->Color(0,0,255,0));
  handle_hours(t, clock, color);
}
