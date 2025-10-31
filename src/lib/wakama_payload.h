#pragma once
#include <Arduino.h>

static String wakamaBuildPayload(
  const char *zone,
  const char *field,
  const char *coop,
  float tempC,
  float humidity,
  float soil,
  float co2ppm
) {
  // ISO time placeholder
  unsigned long ts_ms = millis();
  String json = "{";
  json += "\"type\":\"wakama.sensor.record\",";
  json += "\"version\":1,";
  json += "\"source\":\"esp32\",";
  json += "\"zone\":\"";  json += zone;  json += "\",";
  json += "\"field\":\""; json += field; json += "\",";
  json += "\"coop\":\"";  json += coop;  json += "\",";
  json += "\"readings\":{";
  json += "\"tempC\":";        json += String(tempC, 1);      json += ",";
  json += "\"humidity\":";     json += String(humidity, 1);   json += ",";
  json += "\"soilMoisture\":"; json += String(soil, 1);       json += ",";
  json += "\"co2ppm\":";       json += String(co2ppm, 0);
  json += "},";
  json += "\"ts_ms\":";        json += ts_ms;
  json += "}";
  return json;
}
