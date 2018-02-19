#include "WifiLocation.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

WifiLocation location(googleApiKey);

String builtOWMrequest(float lat, float lon, String weatherAppKey){
  String host = "api.openweathermap.org";
  host += "/data/2.5/weather?lat=";
  host += String(lat, 7);
  host += "&lon=";
  host += String(lon, 7);
  host += "&APPID=";
  host += weatherAppKey;
  return host;
}

String getTemperature(String weatherAppKey) {
  location_t loc = location.getGeoFromWiFi();
  String request = builtOWMrequest(loc.lat, loc.lon, weatherAppKey);
  String payload = "";
  HTTPClient http;  //Declare an object of class HTTPClient
 
  http.begin(request);
  int httpCode = http.GET();
  if (httpCode > 0) { 
    payload = http.getString();
    Serial.println(payload);
  }
  http.end();   //Close connection
  return payload;
}


