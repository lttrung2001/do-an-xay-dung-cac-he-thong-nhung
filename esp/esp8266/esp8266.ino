#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <string.h>

#include <Servo.h>
#include <Ultrasonic.h>
#include <L298N.h>

#define WIFI_SSID "THANHTRUNG"
#define WIFI_PASSWORD "12345678"

#define MQTT_SERVER "4a05b8e1d6604c8c8c84238e32f702e8.s2.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "n19dccn214"
#define MQTT_PASSWORD "ejURUQaYZWn2@t"

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

int distance = 0;
const int trigPin = D6;
const int echoPin = D5;
Ultrasonic ultrasonic(trigPin, echoPin);

const int servoPin = D7;
Servo servo;

const int IN1 = D1;
const int IN2 = D2;
const int IN3 = D3;
const int IN4 = D4;
L298N leftMotor(IN1, IN2);
// Right motor can not backward
L298N rightMotor(IN3, IN4);

void setup() {
  pinMode(LED_BUILTIN, HIGH);
  // put your setup code here, to run once:
  setupWifi();
  wifiClient.setInsecure();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  connectToBroker();

  servo.attach(servoPin);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // client.loop();
  int leftDistance = 0;
  int rightDistance = 0;
  // Read distance
  distance = ultrasonic.read();
  delay(100);
  Serial.print("Front: ");
  Serial.println(distance);
  if (distance <= 30) {
    stopAllMotors();
    // goBack();
    // stopAllMotors();
    leftDistance = lookLeft();
    rightDistance = lookRight();
    lookFront();
    if (leftDistance > rightDistance) {
      turnLeft();
    } else {
      turnRight();
    }
  } else {
    goForward();
  }
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

void stopAllMotors() {
  leftMotor.stop();
  rightMotor.stop();
  delay(100);
}

void goForward() {
  leftMotor.forward();
  rightMotor.forward();
}

void goBack() {
  leftMotor.backward();
  rightMotor.backward();
  delay(500);
}

void turnLeft() {
  leftMotor.backward();
  delay(500);
}

void turnRight() {
  leftMotor.forward();
  delay(500);
}

int lookLeft() {
  servo.write(120);
  delay(200);
  int leftDistance = ultrasonic.read();
  delay(100);
  Serial.print("Left: ");
  Serial.println(leftDistance);
  return leftDistance;
}

int lookRight() {
  servo.write(60);
  delay(200);
  int rightDistance = ultrasonic.read();
  delay(100);
  Serial.print("Right: ");
  Serial.println(rightDistance);
  return rightDistance;
}

void lookFront() {
  servo.write(90);
  delay(500);
}
