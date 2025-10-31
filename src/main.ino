#include <WiFi.h>
#include "config.h"
#include "lib/wakama_payload.h"

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WAKAMA_WIFI_SSID, WAKAMA_WIFI_PASS);
  Serial.print("WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("ok");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Wakama Oracle firmware stub");
  connectWifi();
}

void loop() {
  // fake sensor values for demo
  float tempC   = 29.1;
  float hum     = 64.5;
  float soil    = 42.7;
  float co2ppm  = 512;

  String payload = wakamaBuildPayload(
    "Bouake",
    "KouassiKongokro",
    "Etra",
    tempC,
    hum,
    soil,
    co2ppm
  );

  Serial.println("---- wakama payload ----");
  Serial.println(payload);
  Serial.println("------------------------");

  // TODO: POST to WAKAMA_INGEST_URL with WiFiClient
  // for the milestone, serial-only is OK

  delay(5000);
}
