#include <Servo.h>
#include <Ultrasonic.h>
#include <L298N.h>



int distance = 0;
const int trigPin = 10;
const int echoPin = 11;
Ultrasonic ultrasonic(trigPin, echoPin);



const int servoPin = 9;
Servo servo;

const int IN1 = 5;
const int IN2 = 4;
const int IN3 = 7;
const int IN4 = 6;
L298N leftMotor(IN1, IN2);
// Right motor can not backward
L298N rightMotor(IN3, IN4);

void setup() {
  // put your setup code here, to run once:
  servo.attach(servoPin);
  Serial.begin(9600);  // Starts the serial communication
}

void loop() {
  // put your main code here, to run repeatedly:
  // Read distance
  distance = ultrasonic.read();
  Serial.print("Front: ");
  Serial.println(distance);
  if (distance <= 30) {
    stopAllMotors();
    goBack();
    stopAllMotors();
    int max = 0;
    int continueAngle = 90;
    for (int i = 0; i <= 180; i+= 30) {
      servo.write(i);
      delay(200);
      distance = ultrasonic.read();
      Serial.println(distance);
      int currentAngle = 90;
      if (distance > max) {
        max = distance;
        continueAngle = i;
      }
    }
    servo.write(90);
    int delta = continueAngle - 90;
    if (delta > 0) {
      int count = delta / 30;
      for (int i = 0; i < count; i++) {
        turnLeft();
      }
    } else if (delta < 0) {
      int count = -delta / 30;
      for (int i = 0; i < count; i++) {
        turnRight();
      }
    } else {
      goForward();
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
  delay(400);
}

void turnRight() {
  leftMotor.forward();
  delay(400);
}

int lookLeft() {
  servo.write(120);
  delay(200);
  int leftDistance = ultrasonic.read();
  Serial.print("Left: ");
  Serial.println(leftDistance);
  return leftDistance;
}

int lookRight() {
  servo.write(60);
  delay(200);
  int rightDistance = ultrasonic.read();
  Serial.print("Right: ");
  Serial.println(rightDistance);
  return rightDistance;
}

void lookFront() {
  servo.write(90);
  delay(200);
}
