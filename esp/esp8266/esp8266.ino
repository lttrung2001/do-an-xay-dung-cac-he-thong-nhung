#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <string.h>
#include <Wire.h>

#define WIFI_SSID "OPPO F11"
#define WIFI_PASSWORD "trung2lan"

#define MQTT_SERVER "4a05b8e1d6604c8c8c84238e32f702e8.s2.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "n19dccn214"
#define MQTT_PASSWORD "ejURUQaYZWn2@t"

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

void setup() {
  pinMode(LED_BUILTIN, HIGH);
  // put your setup code here, to run once:
  Serial.begin(9600);
  // Wire.begin(D1, D2);
  setupWifi();
  wifiClient.setInsecure();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  connectToBroker();
  Serial.println("Start transfer");
}

void loop() {
  client.loop();
  // put your main code here, to run repeatedly:
  // Wire.beginTransmission(8);
  // Wire.write("Hello Arduino");
  // Wire.endTransmission();
  // Wire.requestFrom(8,4); // 20 ky tu
  // delay(200);
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
  Serial.println("Connected");
  Serial.println(WiFi.localIP());
}

void connectToBroker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe("topic2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  Serial.println(topic);
  Serial.write(payload, length);
}
