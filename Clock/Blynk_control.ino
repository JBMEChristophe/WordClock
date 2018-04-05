#ifdef ESP_BLYNK
  #include <BlynkSimpleEsp8266.h>
#endif //ESP_BLYNK

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

BLYNK_WRITE(V1){ //TEMPERATURE_DISPLAY
  temperature_display_en = param.asInt();
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