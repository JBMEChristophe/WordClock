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

const char ssid[] = L_SSID;
const char passwd[] = L_PASSW;


wordClock     wclock;
uint8_t       auto_brightness_en;
uint8_t       man_brightness;
uint8_t       spare_minutes_en = 1;
uint8_t       temperature_display_en = 0;
float         temperature;
//location_t    loc;
display_state state;

struct state_times state_transitions[] = {
  { 
    .state              = Time,        
    .start              = 0,
    .end                = 1,
    .displayFunction    = setClockDisplay 
  },
  { 
    .state              = Temperature,
    .start              = T_DISPLAY1, 
    .end                = T_DISPLAY1 + T_DISPLAY_DUR,
    .displayFunction    = setTemperatureDisplay 
  },
  { 
    .state              = Temperature,
    .start              = T_DISPLAY2, 
    .end                = T_DISPLAY2 + T_DISPLAY_DUR,
    .displayFunction    = setTemperatureDisplay
  },
}
clockFaceColor color[] = {
  { .r = 76, .g = 0, .b = 126, .w = 0 }, //TIME
  { .r = 76, .g = 0, .b = 126, .w = 0 }, //MINUTES
  { .r = 0, .g = 0, .b = 0, .w = 255 }  //TEMPERATURE
};
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

struct tm timestruct = { 0 };
struct tm prevTime   = { 0 };

uint8_t*    rtnptr;
uint8_t     rtnval;
uint8_t     firstTimerecv = 0;
uint8_t     connectionAttempts;

uint32_t    elapsed_time_s[3] = { 0 };
uint8_t     reportOnce = 0;
uint32_t    count = 0;

#ifdef ESP_BLYNK

BLYNK_WRITE(V0){ //zeRGBa control
  color[TIME].r = param[0].asInt();
  color[TIME].g = param[1].asInt();
  color[TIME].b = param[2].asInt();
  color[TIME].w = 0;

  if(color[TIME].r == 255 && color[TIME].g == 255 && color[TIME].b == 255){
    color[TIME].r = 0;
    color[TIME].g = 0;
    color[TIME].b = 0;
    color[TIME].w = 255;
  }

  String s = "color[TIME].r: " + String(color[TIME].r);
  reportln(s, DEBUG);
  s = "green: "  + String(color[TIME].g);
  reportln(s, DEBUG);
  s = "blue: " + String(color[TIME].b);
  reportln(s, DEBUG);

  #ifdef ESP_EEPROM
  EEPROM.write(E_MEM_RED,   color[TIME].r);
  EEPROM.write(E_MEM_GREEN, color[TIME].g);
  EEPROM.write(E_MEM_BLUE,  color[TIME].b);
  EEPROM.write(E_MEM_WHITE, color[TIME].w);
  EEPROM.commit();
  #endif //ESP_EEPROM

  setClockDisplay();
}

BLYNK_WRITE(V1){ //TEMPERATURE_DISPLAY I/O
  temperature_display_en = param.asInt();
  EEPROM.write(E_MEM_TEMPERATURE_DIS_EN, temperature_display_en);
}

BLYNK_WRITE(V2){ //SELF_TEST
  reportln("Blynk self_test", DEBUG);
  strip.begin();
  reportln("--------------PERFORMING SELFTEST---------------", INFO);
  self_test(&wclock, 0xFFFFFFFF, 50);
  reportln("--------------FINISHING SELFTEST----------------", INFO);
  setClockDisplay();
}

BLYNK_WRITE(V3){ //MINUTE_DISPLAY I/O
  spare_minutes_en = param.asInt();
  EEPROM.write(E_MEM_SPARE_MIN_EN, spare_minutes_en);
  setClockDisplay();
}

BLYNK_WRITE(V4){ //MINUTE_DISPLAY RGB
  color[MINUTES].r = (uint8_t)param[0].asInt();
  color[MINUTES].g = (uint8_t)param[1].asInt();
  color[MINUTES].b = (uint8_t)param[2].asInt();
  color[MINUTES].w = 0;

  if(color[MINUTES].r == 255 && color[MINUTES].g == 255 && color[MINUTES].b == 255){
    color[MINUTES].r = 0;
    color[MINUTES].g = 0;
    color[MINUTES].b = 0;
    color[MINUTES].w = 255;
  }

  String s = "m_red: " + String(color[MINUTES].r);
  reportln(s, DEBUG);
  s = "m_green: " + String(color[MINUTES].g);
  reportln(s, DEBUG);
  s = "m_blue: " + String(color[MINUTES].b);
  reportln(s, DEBUG);


  #ifdef ESP_EEPROM
  EEPROM.write(E_MEM_MRED,    color[MINUTES].r);
  EEPROM.write(E_MEM_MGREEN,  color[MINUTES].g);
  EEPROM.write(E_MEM_MBLUE,   color[MINUTES].b);
  EEPROM.write(E_MEM_MWHITE,  color[MINUTES].w);
  EEPROM.commit();
  #endif //ESP_EEPROM

  setClockDisplay();
}

BLYNK_WRITE(V5) { //AUTO_BRIGHTNESS I/O
  auto_brightness_en = param.asInt();
  #ifdef ESP_EEPROM
  EEPROM.write(E_MEM_AUTO_BRIGHTNESS, auto_brightness_en);
  #endif //ESP_EEPROM
}

BLYNK_WRITE(V6){ //MANUAL_BRIGHTNESS VAL
  if(!auto_brightness_en) {
    uint8_t temp = param.asInt();
    if(man_brightness != temp) {
      man_brightness = temp;
      wclock.strip->setBrightness(man_brightness);
      wclock.strip->show(); 
    }
  }
}

#endif //ESP_BLYNK

//this should correct any jitter
void addsecond(struct tm* currentTime) {
  time_t unixTime = mktime(currentTime);
  unixTime += 1;
  *currentTime = *(localtime(&unixTime));
}

void setClockDisplay() {
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
}

uint8_t InTime(struct state_times, struct tm t) {
  if(t.tm_sec >= state_times.start && t.tm_sec < state_times.start + state_times.duration){
    return 1;
  }
  return 0;
}

void setTemperatureDisplay() {
  wclock->strip->clear();
  color[TEMPERATURE].c = wclock.strip->Color( color[TEMPERATURE].r,
                                              color[TEMPERATURE].g,
                                              color[TEMPERATURE].b,
                                              color[TEMPERATURE].w);
  if(temperature_display_en){
    if(displayTemperature(&wclock, temperature, color[TEMPERATURE].c) != 0){
      reportln("displayTemperature failed", ERROR);
    }
  }
}

//is triggered at 200hz
void timer1_isr(void){
  if(count > TIM_POSTSCALER) //triggers once a sec
  {
    String s = "Time" + String(asctime(&timestruct));
    reportln(s, DEBUG);

    //interval based retrieval of the Time
    if(elapsed_time_s[0]++ > TIME_RETRIEVAL_INTERVAL_M * 60 || !firstTimerecv) {
      reportln("Retrieving time from using NTP", INFO);
      rtnval = ntp_getTime(&timestruct);
      if(rtnval != 0) {
        reportln("Could not sync time with the NTS", WARN);
      }
      elapsed_time_s[0] = 0;
      setClockDisplay();
      firstTimerecv = 1;
    }

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    prevTime = timestruct;
    addsecond(&timestruct);
/*
    if(prevTime.tm_sec == 59 && timestruct.tm_sec == 00) {
        reportln("A minute has passed", DEBUG);
        setClockDisplay();
    }

    if(prevTime.tm_sec == T_DISPLAY1 && currentTime.tm_sec < T_DISPLAY1 + T_DISPLAY_DUR){
      reportln("displaying temperature", DEBUG);
      setTemperatureDisplay();
    }
    else if (prevTime.tm_sec == T_DISPLAY1 + T_DISPLAY_DUR && currentTime.tm_sec == T_DISPLAY1 + T_DISPLAY1 + 1){
      reportln("displaying time", DEBUG);
      setClockDisplay();
    }
*/
    display_state ds = NULL;
    for(int i = 0; i < display_states_length; i++) {
      if(ds != states[i].state) {
        ds = states[i].state;
        if(states[i].start  => currentTime.tm_sec &&
           states[i].end    < currentTime.tm_sec) {
            states[i]->displayFunction();
        }
        else {
          states[DEFAULT_DISPLAY_STATE]->displayFunction();
        }
      }
    }

    if(timestruct){
      count = 0;
    }
  }
  else {
    count++;
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

  spare_minutes_en        = EEPROM.read(E_MEM_SPARE_MIN_EN);
  temperature_display_en  = EEPROM.read(E_MEM_TEMPERATURE_DIS_EN);
  auto_brightness_en      = EEPROM.read(E_MEM_AUTO_BRIGHTNESS);


  #endif //ESP_EEPROM

  //set the internal LED as output
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //initialise the serial port
  Serial.begin(BAUDRATE);

  //initialise the clock struct
  wclock.rows = ROWS;
  wclock.columns = COLUMNS;
  wclock.strip = &strip;
  rtnptr = (byte*)memcpy(wclock.grid, neopixel_grid, sizeof(neopixel_grid[0][0])*COLUMNS*ROWS);
  if(rtnptr == NULL){ reportln("system out of memory!", FATAL); }

  #ifdef SELFTEST_ON_STARTUP
  //perform a self to check whether all the LEDS are working.
  reportln("--------------PERFORMING SELFTEST---------------", INFO);
  self_test(&wclock, 0xFFFFFFFF, 50); 
  reportln("--------------FINISHING SELFTEST----------------", INFO);

  #endif //SELFTEST_ON_STARTUP

  delay(1000);

  //initialise the LED strip
  strip.setBrightness(MAX_BRIGHTNESS);
  strip.begin();
  strip.show();

  //attempt to connect to WiFi
  connectionAttempts = 0;
  reportln("--------------connecting to WiFI:---------------", INFO);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passwd);
  delay(500);
  while (WiFi.status() != WL_CONNECTED) {
      connectionAttempts++;
      Serial.print(".");
      delay(500);
      if(connectionAttempts > CONNECTIONATTEMPTS)
      {
        wclock.strip->setPixelColor(0, wclock.strip->Color(255,0,0,0));
        if(!reportOnce){
          reportln("The device could not connect to the provided SSID", WARN);
          reportOnce = true;
          strip.show();
        }
      }
  }
  String s = "-----------Connection established--------------";
  reportln(s, INFO);
  s = "IP-Address: " + WiFi.localIP().toString();
  reportln(s, INFO);

  reportln("--------------Retrieving location--------------");
  //loc = location->getGeoFromWiFi();

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
  timer1_write(F_CPU/TIM_PRESCALER/TIM_POSTSCALER); // 1/200 sec
  
  if(rtnval != 0 || rtnptr == NULL){
    reportln("An error was encountered: Halting", ERROR);
    while(1){}
  }

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
    setClockDisplay();
    wclock.strip->show(); 
  #ifdef ESP_BLYNK
  }
  #endif //ESP_BLYNK
}
