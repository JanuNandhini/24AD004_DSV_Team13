#include <AccelStepper.h>
#include <ps5Controller.h>
#include <ESP32Servo.h>

#define STEP_PIN 14
#define DIR_PIN 13
#define SERVO_PIN 26

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
Servo servo;

float stepsPerMM = 50;
long targetPosition = 0;

int servoPos = 0;
const int servoMin = 0;
const int servoMax = 68;

void setup() {

  Serial.begin(115200);

  stepper.setCurrentPosition(0);

  stepper.setMaxSpeed(1500);
  stepper.setAcceleration(1000);

  servo.attach(SERVO_PIN);
  servo.write(servoPos);

  ps5.begin("90:B6:85:64:59:2B");

  Serial.println("Enter distance in mm (negative for reverse):");
}

void loop() {

  if (Serial.available()) {

    String input = Serial.readStringUntil('\n');
    float distance = input.toFloat();

    long steps = distance * stepsPerMM;

    targetPosition += steps;

    stepper.enableOutputs();   // Enable motor before movement
    stepper.moveTo(targetPosition);

    Serial.print("Distance entered: ");
    Serial.print(distance);
    Serial.println(" mm");

    Serial.print("Steps to move: ");
    Serial.println(steps);
  }

  stepper.run();

  // Disable motor when it reaches the target
  if (stepper.distanceToGo() == 0) {
    stepper.disableOutputs();
  }

  if (ps5.isConnected()) {

    if (ps5.Up()) {
      if (servoPos < servoMax) {
        servoPos++;
        servo.write(servoPos);
        delay(20);
      }
    }

    if (ps5.Down()) {
      if (servoPos > servoMin) {
        servoPos--;
        servo.write(servoPos);
        delay(20);
      }
    }
  }
}