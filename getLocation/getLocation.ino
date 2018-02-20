/*********************************************************************
  This is the code for Fetching your location from Google Geolocation API
  
  This code is provided by 
  techiesms
*********************************************************************/

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"

char myssid[] = "Guest";         // your network SSID (name)
char mypass[] = "gastennetwerk";          // your network password


//Credentials for Google GeoLocation API...
const char* Host = "www.googleapis.com";
String thisPage = "/geolocation/v1/geolocate?key=";
String GoogleMapsGeolocationAPIkey = "AIzaSyDnDX92K9ZC6eTqhDzHmzCltHPHuRT6MFM";
String GoogleMapsGeoCodingAPIkey = "AIzaSyDhfgDgOxdC1sEq4hQ7QwidYXKaHeiWTk8";

int status = WL_IDLE_STATUS;
String jsonString = "{\n";
int more_text = 0;

double latitude    = 0.0;
double longitude   = 0.0;
double accuracy    = 0.0;

DynamicJsonBuffer* builtPOSTrequest(DynamicJsonBuffer* json, Wifi* wifi) {
  int n = WiFi.scanNetworks();
  // now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"homeMobileCountryCode\": 234,\n"; // this is a real UK MCC
  jsonString += "\"homeMobileNetworkCode\": 27,\n";  // and a real UK MNC
  jsonString += "\"radioType\": \"gsm\",\n";         // for gsm
  jsonString += "\"carrier\": \"Vodafone\",\n";      // associated with Vodafone
  jsonString += "\"wifiAccessPoints\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString += "{\n";
    jsonString += "\"macAddress\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signalStrength\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1)
    {
      jsonString += "},\n";
    }
    else
    {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
}

void setup()   {
  Serial.begin(9600);


  Serial.println("Start");
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(myssid, mypass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");
}


void loop() {
  DynamicJsonBuffer jsonBuffer;

  //--------------------------------------------------------------------
  WiFiClientSecure client;
  if (client.connect(Host, 443)) {
    client.println("POST " + thisPage + GoogleMapsGeolocationAPIkey + " HTTP/1.1");
    client.println("Host: " + (String)Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.print(jsonString);
    delay(500);
  }

  //Read and parse all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (more_text) {
      Serial.print(line);
    }
    JsonObject& root = jsonBuffer.parseObject(line);
    if (root.success()) {
      latitude    = root["location"]["lat"];
      longitude   = root["location"]["lng"];
      accuracy    = root["accuracy"];
    }
  }
  client.stop();
}
