#include <WifiLocation.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

WifiLocation location(googleApiKey);

String builtOWMrequest(location_t* loc, String weatherAppKey){
  String host = "api.openweathermap.org";
  host += "/data/2.5/weather?lat=";
  host += String(loc->lat, 7);
  host += "&lon=";
  host += String(loc->lon, 7);
  host += "&APPID=";
  host += weatherAppKey;
  return host;
}

float getTemperature(String weatherAppKey) { 
  location_t loc = location.getGeoFromWiFi();
  String request = builtOWMrequest(&loc, weatherAppKey);
  String payload = "";
  HTTPClient http;  //Declare an object of class HTTPClient
 
  http.begin("http://jsonplaceholder.typicode.com/users/1");
  int httpCode = http.GET();
  if (httpCode > 0) { 
    payload = http.getString();
    Serial.println(payload);
  }
  http.end();   //Close connection
  return -1;
}


