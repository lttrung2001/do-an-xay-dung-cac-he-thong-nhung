#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <string.h>

#include <Servo.h>
#include <Ultrasonic.h>
#include <L298N.h>

// WIFI parameters
#define WIFI_SSID "THANHTRUNG"
#define WIFI_PASSWORD "12345678"

// MQTT HiveMQ parameters
#define MQTT_SERVER "4a05b8e1d6604c8c8c84238e32f702e8.s2.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "n19dccn214"
#define MQTT_PASSWORD "ejURUQaYZWn2@t"

// Using to connect mqtt broker.
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// Using to store distance value in centimeter.
int distance = 0;
// Define trigger and echo pins of ultrasonic sensor.
const int trigPin = D6;
const int echoPin = D5;
// Declare ultrasonic object
Ultrasonic ultrasonic(trigPin, echoPin);

// Define data pin of servo.
const int servoPin = D7;
// Declare servo object.
// 90 degree is look forward.
// 60 degree is look left.
// 120 degree is look right.
Servo servo;

// Left back motor.
const int IN1 = D1;
const int IN2 = D2;

// Right back motor.
const int IN3 = D3;
const int IN4 = D4;

// Declare motor objects.
L298N leftMotor(IN1, IN2);
L298N rightMotor(IN3, IN4);

// Status of car.
// true: self-driving mode.
// false: mobile control mode.
bool isSelfDriving = true;

void setup() {
  pinMode(LED_BUILTIN, HIGH);
  setupWifi();
  // Why to set insecure to wifi client here?
  wifiClient.setInsecure();
  // Set server parameters to connect to mqtt service in the future.
  client.setServer(MQTT_SERVER, MQTT_PORT);
  // Set callback.
  client.setCallback(callback);
  // Connect to mqtt broker.
  connectToBroker();

  // Attach servo using servo pin as defined previously.
  servo.attach(servoPin);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
  // If status is self-driving mode.
  if (isSelfDriving) {
    // Then create 2 int store left and right distance value.
    int leftDistance = 0;
    int rightDistance = 0;
    // Read front distance.
    distance = ultrasonic.read();
    delay(100);
    // If front distance is smaller than the value we defined (30 cm).
    if (distance < 30) {
      // Stop all the motors.
      stopAllMotors();
      // Then look left and right to get left and right distance.
      leftDistance = lookLeft();
      rightDistance = lookRight();
      // After look left and right then make servo look straight forward to reset the state.
      lookFront();
      // Which direction has a longer distance, then go in that direction.
      if (leftDistance > rightDistance) {
        turnLeft();
      } else {
        turnRight();
      }
    }
    // If distance is equals or greater than defined value then forward normally. 
    else {
      goForward();
    }
  }
  // If status is mobile control mode then stop all the motors and wait for control signals.
  else {
    stopAllMotors();
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
  // Try to connect mqtt broker until client connected.
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266";
    clientId += String(random(0xffff), HEX);
    // Client connected
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      // Then subscribe needed topics.
      Serial.println("connected");
      // We subscribe "topic2" to receive....
      client.subscribe("topic2");
      // We subscribe "control" to receive control signals.
      client.subscribe("control");
    } 
    // If client connect failed, try again after 2 seconds.
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

// This method use to listen incoming data from subcribed topics.
void callback(char* topic, byte* payload, unsigned int length) {
  // Topic "control".
  // Receive value:
  // 1: Left
  // 2: Forward
  // 3: Right
  // 4: Backward
  // Other values: switch control mode.
  if (strcmp(topic, "control") == 0) {
    // Parse received value
    char value[1];
    value[0] = payload[0];
    // If value is 1 then the car move to left.
    if (String(value).substring(0,1) == "1") {
      leftMotor.backward();
      rightMotor.forward();
      delay(100);
    } 
    // Move to right.
    else if (String(value).substring(0,1) == "3") {
      leftMotor.forward();
      rightMotor.backward();
      delay(100);
    } 
    // Move forward.
    else if (String(value).substring(0,1) == "2") {
      leftMotor.forward();
      rightMotor.forward();
      delay(100);
    } 
    // Move backward.
    else if (String(value).substring(0,1) == "4") {
      leftMotor.backward();
      rightMotor.backward();
      delay(100);
    } 
    // Switch mode.
    else {
      isSelfDriving = !isSelfDriving;
    }
    return;
  }
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
  delay(100);
  int leftDistance = ultrasonic.read();
  delay(100);
  return leftDistance;
}

int lookRight() {
  servo.write(60);
  delay(100);
  int rightDistance = ultrasonic.read();
  delay(100);
  return rightDistance;
}

void lookFront() {
  servo.write(90);
  delay(100);
}
