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

uint8 red = 76;
uint8 green = 0;
uint8 blue = 126;
uint8 white = 0;
uint32 color = 0;

uint8 m_red = 126;
uint8 m_green = 0;
uint8 m_blue = 76;
uint8 m_white = 0;
uint32 m_color = 0;

uint8_t auto_brightness_en;
uint8_t man_brightness;
uint8_t spare_minutes_en = 1;

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
u8 firstTimerecv = 0;


uint8_t rtnval;
uint8_t* rtnptr;
uint8_t connectionAttempts;

uint32_t elapsed_time_s[3] = { 0 };
uint8_t reportOnce = 0;
int count = 0;

#ifdef ESP_BLYNK

BLYNK_WRITE(V0){ //zeRGBa control
  red = param[0].asInt();
  green = param[1].asInt();
  blue = param[2].asInt();
  white = 0;
  if(red == 255 && green == 255 && blue == 255){
    red = 0; green = 0; blue = 0; white = 255;
  }

  String s = "red: " + String(red);
  reportln(s, DEBUG);
  s = "blue: " + String(blue);
  reportln(s, DEBUG);
  s = "green: "  + String(green);
  reportln(s, DEBUG);


  #ifdef ESP_EEPROM
  EEPROM.write(E_MEM_RED, red);
  EEPROM.write(E_MEM_GREEN, green);
  EEPROM.write(E_MEM_BLUE, blue);
  EEPROM.write(E_MEM_WHITE, white);
  EEPROM.commit();
  #endif //ESP_EEPROM

  setClockDisplay();
}

BLYNK_WRITE(V1){ //TEMPERATURE_DISPLAY

}

BLYNK_WRITE(V2){ //SELF_TEST
  reportln("Blynk self_test", DEBUG);
  strip.begin();
  self_test(&wclock, 0xFFFFFFFF, 50);
  setClockDisplay();
}

BLYNK_WRITE(V3){ //MINUTE_DISPLAY I/O
  spare_minutes_en = param.asInt();
  EEPROM.write(E_MEM_SPARE_MIN_EN, spare_minutes_en);
  setClockDisplay();
}

BLYNK_WRITE(V4){ //MINUTE_DISPLAY RGB
  m_red = (uint8_t)param[0].asInt();
  m_blue = (uint8_t)param[1].asInt();
  m_green = (uint8_t)param[2].asInt();
  m_white = 0;
  if(m_red == 255 && m_green == 255 && m_blue == 255){
    m_red = 0; m_green = 0; m_blue = 0; m_white = 255;
  }

  String s = "m_red: " + String(m_red);
  reportln(s, DEBUG);
  s = "m_blue: " + String(m_blue);
  reportln(s, DEBUG);
  s = "m_green: "  + String(m_green);
  reportln(s, DEBUG);


  #ifdef ESP_EEPROM
  EEPROM.write(E_MEM_MRED, m_red);
  EEPROM.write(E_MEM_MGREEN, m_blue);
  EEPROM.write(E_MEM_MBLUE, m_green);
  EEPROM.write(E_MEM_MWHITE, m_white);
  EEPROM.commit();
  #endif //ESP_EEPROM

  setClockDisplay();
}

BLYNK_WRITE(V5) { //AUTO_BRIGHTNESS I/O
  auto_brightness_en = param.asInt();
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
  strip.clear();

  //color = liminousityCorrection(&wclock, red, green, blue, white);
  color = wclock.strip->Color(red, green, blue, white);
  uint32 m_color = wclock.strip->Color(m_red,m_blue,m_green,m_white);
  //color = wclock.strip->Color(65,77,0,0);
  displayTime(&timestruct, &wclock, color, spare_minutes_en, m_color);
  strip.show();
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
    if(prevTime.tm_sec == 59 && timestruct.tm_sec == 00) {
        reportln("A minute has passed", DEBUG);
        setClockDisplay();
    }
    count = 0;
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

  red = EEPROM.read(E_MEM_RED);
  green = EEPROM.read(E_MEM_GREEN);
  blue = EEPROM.read(E_MEM_BLUE);
  white = EEPROM.read(E_MEM_WHITE);

  m_red = EEPROM.read(E_MEM_MRED);
  m_blue = EEPROM.read(E_MEM_MGREEN);
  m_green = EEPROM.read(E_MEM_MBLUE);
  m_white = EEPROM.read(E_MEM_MWHITE);

  spare_minutes_en = EEPROM.read(E_MEM_SPARE_MIN_EN);
#endif //ESP_EEPROM

  //set the internal LED as output
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

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
  if(rtnptr == NULL){ reportln("system out of memory!", FATAL); }

  //perform a self to check whether all the LEDS are working.
  reportln("--------------PERFORMING SELFTEST---------------", INFO);
  self_test(&wclock, 0xFFFFFFFF, 50); 
  reportln("--------------FINISHING SELFTEST----------------", INFO);


  delay(1000);

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

  //intialise the time server
  reportln("--------------connecting to NTP-----------------", INFO);
  rtnval = ntp_init(TIMEZONE, DAYLIGHTSAVING);
  if(rtnval != 0) { reportln("cannot connect to Network Time Server", FATAL); }

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
    float v1 = brightness;
    brightness = pow(brightness, 1/GAMMA) / pow(MAX_BRIGHTNESS, 1/GAMMA);
    brightness = brightness * MAX_BRIGHTNESS;
    float v2 = brightness;
    uint8_t b = map(brightness, 0, MAX_BRIGHTNESS, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    uint8_t v3 = b;
    wclock.strip->setBrightness(b);
    setClockDisplay();
    wclock.strip->show(); 
#ifdef ESP_BLYNK
  }
#endif //ESP_BLYNK
}
