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
struct tm timestruct = { 0 } ;
u8 firstTimerecv = 0;


uint8_t rtnval;
uint8_t* rtnptr;
uint8_t connectionAttempts;

uint32_t elapsed_time_s[3] = { 0 };
uint8_t reportOnce = 0;
int count = 0;

//this should correct any jitter
void addsecond(struct tm* currentTime) {
  time_t unixTime = mktime(currentTime);
  unixTime += 1;
  *currentTime = *(localtime(&unixTime));
}

time_t oldTime = 0;
uint8_t once = 0;
uint8_t minuteChanged(struct tm* currentTime) {
  time_t c_currentTime = mktime(currentTime);
  if(c_currentTime % 60 == 0) {
    if(oldTime % 60 != 0 ){
      oldTime = c_currentTime;
      return 1;
    }
  }  
  return 0;
}

//is triggered at 200hz
void timer0_isr(void){
  if(count > TIM_POSTSCALER) //triggers once a sec
  {
    String s = "Time after increment: " + String(asctime(&timestruct));
    reportln(s, DEBUG);
    elapsed_time_s[0]++;
    count = 0;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    addsecond(&timestruct);
  }
  count++;
}


void setup() {
  //set the internal LED as output
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //initialise the serial port
  Serial.begin(115200);

  //initialise the LED strip
  strip.setBrightness(BRIGHTNESS);
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

  #ifdef ESP8266

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
  reportln("Connected", INFO);

  //intialise the time server
  reportln("--------------connecting to NTP-----------------", INFO);
  rtnval = ntp_init(TIMEZONE, DAYLIGHTSAVING);
  if(rtnval != 0) { reportln("cannot connect to Network Time Server", FATAL); }

  //init the interrupt which is triggered 1/
  noInterrupts();
  timer1_isr_init();
  timer1_enable(TIM_DIV265, TIM_EDGE, TIM_LOOP);
  timer1_attachInterrupt(timer0_isr);
  timer1_write(F_CPU/TIM_PRESCALER/TIM_POSTSCALER); // 1/200 sec
    #endif
  
  if(rtnval != 0 || rtnptr == NULL){
    reportln("An error was encountered: Halting", ERROR);
    while(1){}
  }

  reportln("----------GETTING EVERYTHING SET UP-------------", INFO);
  delay(5000); //give everything the time to get up and running and make de neccessary connections.
  yield();

  reportln("--------------STARTING CLOCK--------------------", INFO);
  interrupts();
}

void loop() {
  //interval based retrieval of the Time
  if(elapsed_time_s[0] > TIME_RETRIEVAL_INTERVAL_M * 60 || !firstTimerecv) {
    reportln("Retrieving time from using NTP", INFO);
    rtnval = ntp_getTime(&timestruct);
    if(rtnval != 0) {
      reportln("Could not sync time with the NTS", WARN);
    }
    elapsed_time_s[0] = 0;
    firstTimerecv = 1;
  }

  if(minuteChanged(&timestruct)) {
    strip.clear();
    reportln("A minute has passed, updating clockface", DEBUG);
    displayTime(&timestruct, &wclock, wclock.strip->Color(0,255,0,0));
    strip.show();
  }
}

