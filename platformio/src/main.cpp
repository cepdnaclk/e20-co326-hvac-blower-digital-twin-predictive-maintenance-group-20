#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "fan_anomaly_model.h"

// Initialize the model directly 
Eloquent::ML::Port::OneClassSVM ai_model;

const char* ssid = "SLT-Fiber-7E6E";
const char* password = "145@7e6e";

const char* mqtt_server = "192.168.1.3";
const int mqtt_port = 1883;

const char* data_topic = "sensors/group20/hvac-blower/data";
const char* alert_topic = "alerts/group20/hvac-blower/status";

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

unsigned long lastMsgTime = 0;
int t = 0;
float history[5] = {0,0,0,0,0};
int history_idx = 0;
int history_count = 0;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    String clientId = "esp32-hvac-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  timeClient.begin();
  
  // Seed random
  randomSeed(analogRead(0));
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  timeClient.update();

  unsigned long now = millis();
  if (now - lastMsgTime > 2000) { // 2s interval matching python default
    lastMsgTime = now;

    int cycle_t = t % 180;
    
    float base = 3.0 + 0.3 * sin((float)cycle_t / 10.0);
    float noise = random(-100, 100) / 1000.0; // -0.1 to 0.1
    float drift = 0.001 * cycle_t;

    float current = base + noise + drift;

    if (random(100) < 5) {
      current += random(80, 150) / 100.0;
    }

    if (cycle_t > 80 && cycle_t < 110) {
      current += 1.0;
    }

    history[history_idx] = current;
    history_idx = (history_idx + 1) % 5;
    if (history_count < 5) history_count++;

    float moving_avg = 0;
    for (int i = 0; i < history_count; i++) {
      moving_avg += history[i];
    }
    moving_avg /= history_count;

    float features[2] = {current, moving_avg};
    
    // micromlgen predicted 0 for decision > 0 (inlier)
    int prediction = ai_model.predict(features);

    String status = "ANOMALY";
    String message = "Anomalous blower fan behavior detected";
    if (prediction == 0) {
      status = "NORMAL";
      message = "Fan operating normally";
    }

    unsigned long epochTime = timeClient.getEpochTime();

    // Setup JSON documents
    // Note: Use a larger size object and allocate DynamicJsonDocument/StaticJsonDocument appropriately.
    // 256 bytes is enough for our payload.
    StaticJsonDocument<256> sensor_payload;
    sensor_payload["timestamp"] = epochTime;
    sensor_payload["device"] = "fan01";
    // round to 2 decimals
    sensor_payload["current"] = round(current * 100.0) / 100.0;
    sensor_payload["moving_avg"] = round(moving_avg * 100.0) / 100.0;
    sensor_payload["unit"] = "A";

    StaticJsonDocument<256> alert_payload;
    alert_payload["timestamp"] = epochTime;
    alert_payload["device"] = "fan01";
    alert_payload["status"] = status;
    alert_payload["message"] = message;

    char sensorMsg[256];
    serializeJson(sensor_payload, sensorMsg);
    
    char alertMsg[256];
    serializeJson(alert_payload, alertMsg);

    client.publish(data_topic, sensorMsg);
    client.publish(alert_topic, alertMsg);

    Serial.print("DATA : ");
    Serial.println(sensorMsg);
    Serial.print("ALERT: ");
    Serial.println(alertMsg);

    t++;
  }
}
