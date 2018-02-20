#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define TIMEZONE +1
#define DAYLIGHTSAVING 0

#define PIN 3
#define NUM_LEDS 110
#define ROWS 10
#define COLUMNS 11
#define BRIGHTNESS 255

#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.nist.gov"

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
		if(severity == FATAL) Serial.print("FATAL: ");
		else if (severity == ERROR)	Serial.print("ERROR: ");
		else if (severity == WARN)	Serial.print("WARN: ");
		else if (severity == INFO)	Serial.print("INFO: ");
		else if (severity == DEBUG)	Serial.print("DEBUG: ");
		else Serial.print("Undefined severity: ");
		Serial.println(s);
	}
}


#endif //CONFIGURATION_H