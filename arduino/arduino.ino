#include <Servo.h>
#include <Ultrasonic.h>
#include <L298N.h>



int distance = 0;
const int trigPin = 10;
const int echoPin = 11;
Ultrasonic ultrasonic(trigPin, echoPin);



const int servoPin = 9;
Servo servo;

const int ENA = 3;
const int ENB = 8;

const int IN1 = 5;
const int IN2 = 4;
const int IN3 = 7;
const int IN4 = 6;
L298N leftMotor(ENA, IN1, IN2);
// Right motor can not backward
L298N rightMotor(ENB, IN3, IN4);

void setup() {
  // put your setup code here, to run once:
  servo.attach(servoPin);
  Serial.begin(9600);  // Starts the serial communication
}

void loop() {
  // put your main code here, to run repeatedly:
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
