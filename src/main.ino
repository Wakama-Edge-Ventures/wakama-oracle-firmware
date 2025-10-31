// Wakama Oracle Firmware (stub)
// ESP32 demo – generates fake sensor data and prints JSON every 10s
// Target: milestone 2 weeks (Solana Foundation)
// Author: Wakama Edge Ventures

#include <WiFi.h>

// optional: if you later want to POST to n8n / ingest
// #include <HTTPClient.h>

// --- WiFi (leave blank for now, doc will say: fill your SSID/PASS) ---
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASS";

// fake site info (same as dashboard demo)
const char* ZONE  = "Bouake";
const char* FIELD = "KouassiKongokro";
const char* COOP  = "Etra";

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println();
  Serial.println("Wakama Oracle – ESP32 stub starting...");

  // optional WiFi connect (non blocking)
  if (strlen(WIFI_SSID) > 0) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
      delay(200);
      Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("WiFi OK: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("WiFi not connected (demo will still stream JSON on Serial).");
    }
  }
}

float fakeTemp() {
  // around 28–32°C
  return 28.0 + (float)(rand() % 40) / 10.0;
}

float fakeHum() {
  // around 60–75%
  return 60.0 + (float)(rand() % 150) / 10.0;
}

float fakeSoil() {
  // around 35–55%
  return 35.0 + (float)(rand() % 200) / 10.0;
}

float fakeCO2() {
  // ppm
  return 480.0 + (float)(rand() % 200);
}

void loop() {
  // build a Wakama-like JSON (close to your sensor_batch)
  // IMPORTANT: keep "type" and "source" so the publisher can classify later
  unsigned long nowMs = millis();
  char buf[512];

  // we don’t have RTC here, so we send millis() – backend will overwrite
  snprintf(
    buf,
    sizeof(buf),
    "{"
      "\"type\":\"wakama.sensor.record\","
      "\"version\":1,"
      "\"source\":\"esp32-demo\","
      "\"site\":{"
        "\"zone\":\"%s\","
        "\"field\":\"%s\","
        "\"coop\":\"%s\""
      "},"
      "\"readings\":{"
        "\"tempC\":%.1f,"
        "\"humidity\":%.1f,"
        "\"soilMoisture\":%.1f,"
        "\"co2ppm\":%.1f"
      "},"
      "\"ts_ms\":%lu"
    "}",
    ZONE,
    FIELD,
    COOP,
    fakeTemp(),
    fakeHum(),
    fakeSoil(),
    fakeCO2(),
    nowMs
  );

  Serial.println(buf);

  // later: send HTTP POST to your ingest if WiFi is up
  // if (WiFi.status() == WL_CONNECTED) {
  //   HTTPClient http;
  //   http.begin("http://192.168.1.50:5678/ingest"); // your local n8n / express
  //   http.addHeader("Content-Type", "application/json");
  //   int code = http.POST((uint8_t*)buf, strlen(buf));
  //   http.end();
  // }

  delay(10 * 1000); // every 10s
}
