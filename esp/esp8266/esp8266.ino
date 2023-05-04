#include <ESP8266WiFi.h>
#include <SocketIoClient.h>
#include <string>
#include <string.h>
#include <Wire.h>

#define WIFI_SSID "THANHTRUNG"
#define WIFI_PASSWORD "12345678"

char host[] = "192.168.39.114";
int port = 3000;

SocketIoClient webSocket;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(D1, D2);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());
  webSocket.begin(host, port);
}

void loop() {
  // put your main code here, to run repeatedly:
  pinMode(LED_BUILTIN, HIGH);
  Wire.beginTransmission(8);
  Wire.write("Hello Arduino");
  Wire.endTransmission();

  Wire.requestFrom(8,4); // 20 ky tu
  delay(200);
}
