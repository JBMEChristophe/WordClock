#include <Adafruit_NeoPixel.h>
#include <time.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

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

void setWord(Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], clockFace w, uint32_t color)
{
  for(int i = 0; i < w.len; i++)
  {
    strip->setPixelColor(grid[w.x][w.y+i], color);
  }
}

void handle_minutes(struct tm* t, Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], uint32_t color) {
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

void handle_minutes_spare(struct tm* t, Adafruit_NeoPixel* strip, byte grid[ROWS][COLUMNS], uint32_t color)
{
  int mod_min = t->tm_min % 5;

  switch (mod_min) {
  case 4:
    strip->setPixelColor(grid[5][5], color);
  case 3:
    strip->setPixelColor(grid[5][4], color);
  case 2:
    strip->setPixelColor(grid[4][5], color);
  case 1:
    strip->setPixelColor(grid[4][4], color);
    break;
  default:
    break;
  }
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
