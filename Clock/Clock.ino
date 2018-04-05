#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <EEPROM.h>
#include <math.h>

#include "types.h"
#include "configuration.h"

#ifdef __AVR__
  #include <avr/power.h>
#elif ESP8266
  #include "WifiLocation.h"
#endif

#ifdef ESP_BLYNK
  #include <BlynkSimpleEsp8266.h>
#endif //ESP_BLYNK

#ifdef ESP_EEPROM
  #include <EEPROM.h>
#endif //ESP_EEPROM

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

const String googleApiKey = "AIzaSyDnDX92K9ZC6eTqhDzHmzCltHPHuRT6MFM";
const String owmApiKey = "d816a08dddeb2df937174ddcd3d4b5a3";
const String blynkApiKey = "741a7e8900c04fd8ada0297a956b1c77";
WifiLocation location(googleApiKey);

display_state prevDisplayState;
display_state displayState = Time;

const char ssid[] = L_SSID;
const char passwd[] = L_PASSW;

clockFaceColor color[] = {
  { .r = 76, .g = 0, .b = 126, .w = 0 }, //TIME
  { .r = 76, .g = 0, .b = 126, .w = 0 }, //MINUTES
  { .r = 76, .g = 0, .b = 126, .w = 0 }  //TEMPERATURE
};

uint8_t auto_brightness_en;
uint8_t man_brightness;
uint8_t spare_minutes_en = 1;
uint8_t temperature_display_en = 1;

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
struct tm timestruct = { 0 };
struct tm prevTime   = { 0 };
float temperature;
location_t loc;

uint8_t displayUpdate = 1;

uint8_t rtnval;
uint8_t* rtnptr;

uint32_t elapsed_time_s = 999999;
int count = 0;
String s;

//this should correct any jitter
void addsecond(struct tm* currentTime) {
  time_t unixTime = mktime(currentTime);
  unixTime += 1;
  *currentTime = *(localtime(&unixTime));
}

void setClockDisplay() {
  #ifdef TIME_DISPLAY
  reportln("updating clockface", DEBUG);
  wclock.strip->clear();

  color[TIME].c     = wclock.strip->Color( color[TIME].r,
                                           color[TIME].g,
                                           color[TIME].b,
                                           color[TIME].w);

  color[MINUTES].c  = wclock.strip->Color( color[MINUTES].r, 
                                        color[MINUTES].g,
                                        color[MINUTES].b,
                                        color[MINUTES].w );

  displayTime(&timestruct, &wclock, color[TIME].c, spare_minutes_en, color[MINUTES].c);
  strip.show();
  #endif //TIME_DISPLAY
}

void setTemperatureDisplay() {
  #ifdef TEMPERATURE_DISPLAY
  reportln("showing temperature", DEBUG);

  color[TEMPERATURE].c = wclock.strip->Color( color[TEMPERATURE].r, 
                                        color[TEMPERATURE].g,
                                        color[TEMPERATURE].b,
                                        color[TEMPERATURE].w );
  reportln("updating color", DEBUG);
  rtnval = displayTemperature(&wclock, temperature, color[TEMPERATURE].c);
  reportln("updating temperature display", DEBUG);
  if(rtnval != 0) {
    reportln("error encountered displaying temperature", ERROR);
  }
  strip.show();
  #endif //TEMPERATURE_DISPLAY
}

void updateDisplay(display_state ds){
  wclock.strip->clear();
  switch (ds){
    case Time:
      #ifdef TIME_DISPLAY
      setClockDisplay();
      #endif //TIME_DISPLAY
      break;
    case Temperature:
      #ifdef TEMPERATURE_DISPLAY
      setTemperatureDisplay();
      #endif //TEMPERATURE_DISPLAY
      break;
    default:
      reportln("updateDisplay received an invalid call", DEBUG);
      break;
  }
}

//is triggered at 200hz
void timer1_isr(void){
  if(count++ > T1M_POSTSCALER) //triggers once a sec
  {
    #ifdef SECOND_BLINKING_INTERNAL_LED
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    #endif //SECOND_BLINKING_INTERNAL_LED

    s = "Time: " + String(asctime(&timestruct));
    reportln(s, DEBUG);

    //interval based retrieval of the Time
    if(elapsed_time_s++ > TIME_RETRIEVAL_INTERVAL_M * 60) {
      reportln("Retrieving time from using NTP", INFO);
      rtnval = ntp_getTime(&timestruct);
      if(rtnval != 0) {
        reportln("Could not sync time with the NTS", WARN);
      }
/*
      reportln("Retrieving temperature from OpenWeatherMap", INFO);
      String temperatureRespons = getTemperature(loc, owmApiKey);
      temperature = extractTemperature(temperatureRespons);
      temperature = temperature - KELVIN;
      s = "temperature from OWM: "  +  String(temperature);
      reportln(s, DEBUG);
*/
      elapsed_time_s = 0;
      displayUpdate = true;
    }

    prevTime = timestruct;
    addsecond(&timestruct);
    if(prevTime.tm_sec == 59 && timestruct.tm_sec == 00) {
      reportln("A minute has passed", DEBUG);
      displayUpdate = true;

    }

    if(timestruct.tm_sec >= T_DISPLAY1_START  && timestruct.tm_sec < T_DISPLAY1_END){
      displayState = Temperature;
    }
    else if(timestruct.tm_sec >= T_DISPLAY2_START  && timestruct.tm_sec < T_DISPLAY2_END){
      displayState = Temperature;
    }
    else{
      displayState = Time;
    }

    if(displayUpdate == true || prevDisplayState != displayState){
      displayUpdate = false;
      prevDisplayState = displayState;

      reportln("updating display", DEBUG);
      updateDisplay(displayState);
    }
    count = 0;
  }
}

void setup() {
  #ifdef ESP_BLYNK
  Blynk.begin(blynkApiKey.c_str(), ssid, passwd);
  #endif //ESP_BLYNK
  #ifdef ESP_EEPROM
  EEPROM.begin(512);

  color[TIME].r = EEPROM.read(E_MEM_RED);
  color[TIME].g = EEPROM.read(E_MEM_GREEN);
  color[TIME].b = EEPROM.read(E_MEM_BLUE);
  color[TIME].w = EEPROM.read(E_MEM_WHITE);

  color[TIME].r = EEPROM.read(E_MEM_MRED);
  color[TIME].g = EEPROM.read(E_MEM_MGREEN);
  color[TIME].b = EEPROM.read(E_MEM_MBLUE);
  color[TIME].w = EEPROM.read(E_MEM_MWHITE);

  spare_minutes_en = EEPROM.read(E_MEM_SPARE_MIN_EN);
  #endif //ESP_EEPROM

  //set the internal LED as output
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //initialise the serial port
  Serial.begin(BAUDRATE);

  //initialise the LED strip
  strip.setBrightness(MAX_BRIGHTNESS);
  strip.begin();
  strip.show();

  //initialise the clock struct
  wclock.rows = ROWS;
  wclock.columns = COLUMNS;
  wclock.strip = &strip;
  rtnptr = (byte*)memcpy(wclock.grid, neopixel_grid, sizeof(neopixel_grid[0][0])*COLUMNS*ROWS);
  if(rtnptr == NULL){
    reportln("system out of memory!", FATAL);
  }
  #ifdef SELFTEST_ON_STARTUP
  //perform a self to check whether all the LEDS are working.
  reportln("--------------PERFORMING SELFTEST---------------", INFO);
  self_test(&wclock, 0xFFFFFFFF, 50); 
  reportln("--------------FINISHING SELFTEST----------------", INFO);
  delay(1000);
  #endif //SELFTEST_ON_STARTUP

  //attempt to connect to WiFi
  uint8_t connectionAttempts = 0;
  uint8_t reportOnce = 0;
  reportln("--------------connecting to WiFI:---------------", INFO);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passwd);
  delay(500);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      if(connectionAttempts++ > CONNECTIONATTEMPTS)
      {
        #ifdef WIFI_NOT_CONNECT_LED
        wclock.strip->setPixelColor(0, wclock.strip->Color(255,0,0,0));
        #endif //WIFI_NOT_CONNECT_LED
        if(!reportOnce){
          reportln("The device could not connect to the provided SSID", WARN);
          reportOnce = true;
          strip.show();
        }
     }
  }

  reportln("--------------Retrieving location---------------", INFO);
  location_t loc = location.getGeoFromWiFi();
  s = "Lattitude received: " + String(loc.lat);
  reportln(s, DEBUG);
  s = "Longitude received: " + String(loc.lon);
  reportln(s, DEBUG);
  

  reportln("------------Retrieving weather data-------------", INFO);
  String temperatureRespons = getTemperature(loc, owmApiKey);
  temperature = extractTemperature(temperatureRespons);
  temperature = temperature - KELVIN;
  s = "Temperature: " + String(temperature) + " Degrees celsius";
  reportln(s, DEBUG);

  reportln("------------Connection established--------------", INFO);
  s = "IP-Address: " + WiFi.localIP().toString();
  reportln(s, DEBUG);

  //intialise the time server
  reportln("--------------connecting to NTP-----------------", INFO);
  rtnval = ntp_init(TIMEZONE, DAYLIGHTSAVING);
  if(rtnval != 0) {
    reportln("cannot connect to Network Time Server", FATAL);
  }

  //init the interrupt which is triggered 1/
  noInterrupts();
  timer1_isr_init();
  timer1_enable(TIM_DIV265, TIM_EDGE, TIM_LOOP);
  timer1_attachInterrupt(timer1_isr);
  timer1_write(F_CPU/T1M_PRESCALER/T1M_POSTSCALER); // 1/200 sec

  reportln("--------------STARTING CLOCK--------------------", INFO);
  interrupts(); //starting the interrupt after all initialisation is done
}

void loop() {
  #ifdef ESP_BLYNK
  Blynk.run();
  if(auto_brightness_en) {
  #endif //ESP_BLYNK
    float brightness = analogRead(A0);
    brightness = map(brightness, 0, 1024, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    brightness = pow(brightness, 1/GAMMA) / pow(MAX_BRIGHTNESS, 1/GAMMA);
    brightness = brightness * MAX_BRIGHTNESS;
    wclock.strip->setBrightness(map(brightness, 0, MAX_BRIGHTNESS, MIN_BRIGHTNESS, MAX_BRIGHTNESS));
    updateDisplay(displayState);
    wclock.strip->show(); 
  #ifdef ESP_BLYNK
  }
  #endif //ESP_BLYNK
}
