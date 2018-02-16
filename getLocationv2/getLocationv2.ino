#include <ArduinoJson.h>

#include <ESP8266WiFi.h>

String googleApiKey = "AIzaSyDnDX92K9ZC6eTqhDzHmzCltHPHuRT6MFM";
String owmApiKey = "d816a08dddeb2df937174ddcd3d4b5a3";
String ssid = "Guest";
String passwd = "gastennetwerk";

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passwd);
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    while (WiFi.status() != WL_CONNECTED) {
        // wait 5 seconds for connection:
        Serial.print(".");
        delay(500);
    }
    Serial.println("connected");

    Serial.print("temperature");
    Serial.println(getTemperature(owmApiKey));
    
}

void loop() {

}
