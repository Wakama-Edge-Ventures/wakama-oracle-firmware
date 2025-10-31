# wakama-oracle-firmware

Firmware ESP32/ESP32-CAM pour **Wakama Oracle**.

---

## 1. Description

Minimal ESP32 firmware used to demonstrate the **“device → JSON → publisher → dashboard”** flow for **Wakama.farm**.

This repo does **not** try to be production-ready. It only proves that:

1. An ESP32 can produce **Wakama-shaped JSON** (close to `wakama.sensor.record`).
2. The JSON contains the **same site metadata** used in the dashboard (**Bouake / KouassiKongokro / Etra**).
3. It can later be **POSTed to an ingest endpoint** (n8n, Express, Wakama ingest).
4. The code is **small enough** to be flashed from Arduino IDE or PlatformIO.

This repo is just to show Solana Foundation / Amira that the **device side exists** and speaks the **same JSON** as the publisher.

---

## 2. Repo structure

```text
wakama-oracle-firmware/
├── README.md
├── platformio.ini
├── src/
│   ├── main.ino
│   └── config.h
├── src/lib/
│   └── wakama_payload.h
├── src/payloads/
│   └── example-sensor.json
└── docs/
    └── wiring.md
3. JSON shape (target)
The firmware emits JSON like:

json
Copier le code
{
  "type": "wakama.sensor.record",
  "version": 1,
  "source": "esp32-demo",
  "site": {
    "zone": "Bouake",
    "field": "KouassiKongokro",
    "coop": "Etra"
  },
  "readings": {
    "tempC": 29.1,
    "humidity": 64.5,
    "soilMoisture": 42.7,
    "co2ppm": 512
  },
  "ts_ms": 123456789
}
This is intentionally similar to the JSON we push to IPFS and to the dashboard.

4. Files
4.1 src/config.h
cpp
Copier le code
#pragma once

// put your local WiFi creds here
#define WAKAMA_WIFI_SSID "YOUR_WIFI_SSID"
#define WAKAMA_WIFI_PASS "YOUR_WIFI_PASS"

// identify this device in logs
#define WAKAMA_DEVICE_ID "esp32-bouake-001"

// later we can POST to this URL
// e.g. http://192.168.1.50:5678/ingest or your n8n webhook
#define WAKAMA_INGEST_URL ""
4.2 src/lib/wakama_payload.h
cpp
Copier le code
#pragma once
#include <Arduino.h>

static String wakamaBuildPayload(
  const char* zone,
  const char* field,
  const char* coop,
  float tempC,
  float humidity,
  float soil,
  float co2ppm,
  unsigned long ts_ms
) {
  // build JSON using Arduino String (ok for demo)
  String j = "{";
  j += "\"type\":\"wakama.sensor.record\",";
  j += "\"version\":1,";
  j += "\"source\":\"esp32-demo\",";
  j += "\"site\":{";
    j += "\"zone\":\"";  j += zone;  j += "\",";
    j += "\"field\":\""; j += field; j += "\",";
    j += "\"coop\":\"";  j += coop;  j += "\"";
  j += "},";
  j += "\"readings\":{";
    j += "\"tempC\":";        j += String(tempC, 1);     j += ",";
    j += "\"humidity\":";     j += String(humidity, 1);  j += ",";
    j += "\"soilMoisture\":"; j += String(soil, 1);      j += ",";
    j += "\"co2ppm\":";       j += String(co2ppm, 0);
  j += "},";
  j += "\"ts_ms\":"; j += String(ts_ms);
  j += "}";
  return j;
}
4.3 src/payloads/example-sensor.json
json
Copier le code
{
  "type": "wakama.sensor.record",
  "version": 1,
  "source": "esp32-demo",
  "site": {
    "zone": "Bouake",
    "field": "KouassiKongokro",
    "coop": "Etra"
  },
  "readings": {
    "tempC": 29.4,
    "humidity": 63.2,
    "soilMoisture": 43.8,
    "co2ppm": 505
  },
  "ts_ms": 1730371200000
}
This is only to show on GitHub: “this is the format we send”.

4.4 src/main.ino (improved version)
cpp
Copier le code
#include <WiFi.h>
#include "config.h"
#include "lib/wakama_payload.h"

const char* ZONE  = "Bouake";
const char* FIELD = "KouassiKongokro";
const char* COOP  = "Etra";

void connectWifi() {
  if (strlen(WAKAMA_WIFI_SSID) == 0) {
    Serial.println("WiFi SSID empty – skipping connect.");
    return;
  }
  Serial.print("WiFi connecting to: ");
  Serial.println(WAKAMA_WIFI_SSID);
  WiFi.begin(WAKAMA_WIFI_SSID, WAKAMA_WIFI_PASS);
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
    Serial.println("WiFi failed (demo will still print JSON).");
  }
}

float fakeTemp()    { return 28.0 + (float)(rand() % 35) / 10.0; }
float fakeHum()     { return 60.0 + (float)(rand() % 200) / 10.0; }
float fakeSoil()    { return 35.0 + (float)(rand() % 200) / 10.0; }
float fakeCO2()     { return 470.0 + (float)(rand() % 250); }

void setup() {
  Serial.begin(115200);
  delay(1200);
  Serial.println();
  Serial.println("Wakama Oracle – ESP32 demo firmware");
  Serial.print("Device: ");
  Serial.println(WAKAMA_DEVICE_ID);
  connectWifi();
}

void loop() {
  float t   = fakeTemp();
  float h   = fakeHum();
  float sm  = fakeSoil();
  float co2 = fakeCO2();
  unsigned long ts = millis();

  String payload = wakamaBuildPayload(ZONE, FIELD, COOP, t, h, sm, co2, ts);
  Serial.println(payload);

  // later:
  // if (WiFi.status() == WL_CONNECTED && strlen(WAKAMA_INGEST_URL) > 0) {
  //   // HTTP POST here
  // }

  delay(10 * 1000);
}
4.5 docs/wiring.md
markdown
Copier le code
# Wiring notes (demo)

For the 2-week milestone we do **NOT** require real sensors, but the target MCU is an ESP32 DevKit (common board).

If you want to plug real sensors:

- **DHT22** → VCC 3.3V, GND, DATA on GPIO 4 (can be changed)
- **DS18B20** → 3.3V, GND, DATA on GPIO 5, 4.7k pull-up
- **Soil sensor (analog)** → 3.3V, GND, A0 (ADC1_CH0)

In code we currently **generate fake values**. Replacing them is straightforward:

1. read from DHT22
2. pass real values to `wakamaBuildPayload(...)`
3. print / POST
4.6 platformio.ini (optional but clean)
ini
Copier le code
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
src_dir = src
lib_deps =
  bblanchon/ArduinoJson @ ^6.21.3
5. How it connects to the rest
This firmware produces the JSON.

The JSON is the same as the one you put in wakama-rwa-templates/templates/*.json.

Then the publisher (wakama-oracle-publisher) uploads it to Pinata (IPFS) and emits a Solana Devnet memo.

Then the dashboard (wakama-dashboard) shows it in /now-playing.

So pour le milestone 2 semaines, ça prouve le flux complet device → JSON → IPFS → Devnet → dashboard.

6. License
MIT – Wakama Edge Ventures
