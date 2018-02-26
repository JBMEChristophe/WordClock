#include <ESP8266WiFi.h>
#include <time.h>

uint8_t ntp_init(float timezone_hours, float daylightOffset_hours) {
	if(WiFi.status() != WL_CONNECTED) { 
		return 1;
	}
	configTime(timezone_hours*3600, daylightOffset_hours, NTP_SERVER_1, NTP_SERVER_2);
	while(!time(nullptr)) {
	}
	return 0;
}

uint8_t ntp_getTime(struct tm* t) {
	if(WiFi.status() != WL_CONNECTED)
	{
		return 1;
	}
	time_t now = time(nullptr);
	t = localtime(&now);
  	return 0;
}