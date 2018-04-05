#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "EEPROM_mem_location.h"

#define L_SSID "Guest"
#define L_PASSW "gastennetwerk"

#define ESP_BLYNK //comment out to disable feature
#define ESP_EEPROM // comment out to disable feature
//#define SECOND_BLINKING_INTERNAL_LED
//#define SELFTEST_ON_STARTUP
#define TIME_DISPLAY
//#define TEMPERATURE_DISPLAY
#define WIFI_NOT_CONNECT_LED

#define TIMEZONE 1
#define DAYLIGHTSAVING 1

#define BAUDRATE 115200

#define PIN D3
#define NUM_LEDS 110
#define ROWS 10
#define COLUMNS 11
#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 1

#define T1M_POSTSCALER 200
#define T1M_PRESCALER 256

#define TIME_RETRIEVAL_INTERVAL_M  15		
#define TEMP_RETRIEVAL_INTERVAL_M  15				
#define CONNECTIONATTEMPTS 60			  //try connecting for 30 secs

#define T_DISPLAY1_START 10
#define T_DISPLAY1_END T_DISPLAY1_START + T_DISPLAY_DUR

#define T_DISPLAY2_START 40
#define T_DISPLAY2_END T_DISPLAY2_START + T_DISPLAY_DUR

#define T_DISPLAY_DUR 5

#define NTP_SERVER_1 "nl.pool.ntp.org"
#define NTP_SERVER_2 "europe.pool.ntp.org"

//LED gamma correction
#define GAMMA 0.17
//these are the luminousity values of the individual leds
#define R_MCD_L 550
#define R_MCD_H 700
#define G_MCD_L 1100
#define G_MCD_H 1400
#define B_MCD_L 200
#define B_MCD_H 400

#define KELVIN 273.15

#define TIME 0
#define MINUTES 1
#define TEMPERATURE 2

//reporting is defined by levels 1 upto 5 with 5 reporting everything
//when putting 0 in the reporting level nothing will be printed
#define REPORTING_LEVEL 5

#define DEBUG 5
#define INFO 4
#define WARN 3
#define ERROR 2
#define FATAL 1

void reportln(String s, int severity) {
	if(severity <= REPORTING_LEVEL) {
		switch (severity){
			case FATAL:		Serial.print("FATAL: "); break;
			case ERROR:		Serial.print("ERROR: "); break;
			case WARN:		Serial.print("WARNING: "); break;
			case INFO:		Serial.print("INFO: "); break;
			case DEBUG:		Serial.print("DEBUG: "); break;
			default:		Serial.print("Undefined severity: "); break;
		}
		Serial.println(s);
	}
}


#endif //CONFIGURATION_H
