#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <math.h>

#include "types.h"
#include "configuration.h"

#ifdef __AVR__
  #include <avr/power.h>
#elif ESP8266
  #include "WifiLocation.h"
#endif

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

#ifdef ESP8266
#warning "Compiling for the ESP8266!"
  const String googleApiKey = "AIzaSyDnDX92K9ZC6eTqhDzHmzCltHPHuRT6MFM";
  const String owmApiKey = "d816a08dddeb2df937174ddcd3d4b5a3";
  WifiLocation location(googleApiKey);

  const char ssid[] = L_SSID;
  const char passwd[] = L_PASSW;
#endif


byte neopixel_grid[ROWS][COLUMNS] = {
 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
 { 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11 },
 { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 },
 { 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33 },
 { 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54 },
 { 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55 },
 { 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76 },
 { 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77 },
 { 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98 },
 { 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99 }
};

wordClock wclock;
struct tm timestruct;
float temperature; 

uint8_t rtnval;
uint8_t* rtnptr;
uint8_t connectionAttempts;

long previous[3] = { 0 };
uint8_t reportOnce = 0;


//this should correct any jitter
void addsecond(struct tm* currentTime, uint32_t milliseconds) {
  time_t unixTime = mktime(currentTime);
  unixTime += (uint32_t)round((float)milliseconds/1000);
  *currentTime = *(localtime(&unixTime));
}

void setup() { 
  //initialise the LED strip
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show();
  wclock.rows = ROWS;
  wclock.columns = COLUMNS;
  wclock.strip = &strip;
  rtnptr = (byte*)memcpy(wclock.grid, neopixel_grid, sizeof(neopixel_grid[0][0])*COLUMNS*ROWS);
  if(rtnptr == NULL){ reportln("system out of memory!", FATAL); }

  //perform a self to check whether all the LEDS are working.
  reportln("--------------PERFORMING SELFTEST---------------", INFO);
  self_test(&wclock, 0xFFFFFFFF, 50); 
  reportln("--------------FINISHING SELFTEST----------------", INFO);

  //initialise the serial port
  Serial.begin(115200);

  delay(1000);

#ifdef ESP8266
  connectionAttempts = 0;

  //attempt to connect to WiFi
  Serial.println("--------------connecting to WiFI:---------------");
  Serial.print("attempting to connect to: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
      connectionAttempts++;
      Serial.print(".");
      delay(500);
      if(connectionAttempts > CONNECTIONATTEMPTS)
      {
        wclock.strip->setPixelColor(0, wclock.strip->Color(255,0,0,0));
        if(!reportOnce){
          reportln("could not connect to WiFi, I will keep trying", WARN);
          reportOnce = true;
        }
      }
  }
  reportln(" Connected", INFO);

  //intialise the time server
  rtnval = ntp_init(TIMEZONE, DAYLIGHTSAVING);
  if(rtnval != 0) { reportln("cannot connect to Network Time Server", FATAL); }
  rtnval = ntp_getTime(&timestruct);
  if(rtnval != 0) { reportln("cannot retrieve the time from Network Time Server", ERROR); }
#endif

  reportln("--------------STARTING CLOCK--------------------", INFO);

  if(rtnval != 0 || rtnptr != NULL){
    reportln("An error was encountered: Halting", ERROR);
    while(1){}
  }
}

void loop() {
  strip.clear();

  long current = millis();
  //interval based retrieval of the Time
  if(current - previous[0] > TIME_RETRIEVAL_INTERVAL_MS) {
    reportln("Retrieving time from using NTP", INFO);
    rtnval = ntp_getTime(&timestruct);
    if(rtnval != 0) {
      reportln("Could not sync time with the NTS", WARN);
    }
  }
  //interval based retrieval of the temperature
  if(current - previous[1] > TEMP_RETRIEVAL_INTERVAL_MS) {
    String httpResponse = getTemperature(&location, owmApiKey);
    temperature = extractTemperature(httpResponse);
    if(temperature == NULL) {
      reportln("Could not retrieve retrieve temperature from Openweathermap.com", WARN);
    }
  }

  //display temperature between 10 and 15 and between 40 and 45
  uint8_t interval1 = timestruct.tm_sec > T_DISPLAY1 && timestruct.tm_sec < (T_DISPLAY1 + T_DISPLAY_DUR);
  uint8_t interval2 = timestruct.tm_sec > T_DISPLAY2 && timestruct.tm_sec < (T_DISPLAY2 + T_DISPLAY_DUR);
  if(interval1 || interval2) {
    reportln("Displaying temperature", INFO);
    displayTemperature(&wclock, temperature, wclock.strip->Color(128,0,128,0));
  }
  else {
    displayTime(&timestruct, &wclock, wclock.strip->Color(0,255,0,0));
  }

  if(current - previous[2] > SECOND_INTERVAL_MS) {
    addsecond(&timestruct, current - previous[2]);
  }

  strip.show();
}

