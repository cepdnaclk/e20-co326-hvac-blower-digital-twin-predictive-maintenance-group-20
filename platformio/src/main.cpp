// // const char* ssid = "Dulanga";
// // const char* password = "dul1.pun2/";

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Nokia 7.2";
const char* password = "abcd1234";

const char* mqtt_server = "192.168.97.194";
const int mqtt_port = 1883;

const char* control_topic = "misitha/hvacblower/control";
const char* status_topic  = "misitha/hvacblower/status";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastStatusMsg = 0;
bool fanState = false;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishStatus(const char* msg) {
  if (client.publish(status_topic, msg)) {
    Serial.print("Published status: ");
    Serial.println(msg);
  } else {
    Serial.println("Failed to publish status");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Command received: ");
  Serial.println(message);

  if (message == "ON") {
    fanState = true;
    publishStatus("Fan command applied: ON");
  } 
  else if (message == "OFF") {
    fanState = false;
    publishStatus("Fan command applied: OFF");
  } 
  else {
    publishStatus("Unknown command");
  }
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");

    String clientId = "esp32-misitha-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected!");

      client.subscribe(control_topic);
      Serial.print("Subscribed to: ");
      Serial.println(control_topic);

      publishStatus("ESP32 online");
    } else {
      Serial.print("failed, state = ");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 MQTT subscribe test");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  unsigned long now = millis();
  if (now - lastStatusMsg > 10000) {
    lastStatusMsg = now;

    if (fanState) {
      publishStatus("Current fan state: ON");
    } else {
      publishStatus("Current fan state: OFF");
    }
  }
}

