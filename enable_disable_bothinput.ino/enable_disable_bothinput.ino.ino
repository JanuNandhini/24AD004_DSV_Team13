#include <AccelStepper.h>
#include <ps5Controller.h>
#include <ESP32Servo.h>

#define STEP_PIN 14
#define DIR_PIN 13
#define SERVO_PIN 26

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
Servo servo;

float stepsPerMM = 6.25;
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

  Serial.println("Commands:");
  Serial.println("M<number>  → Move stepper in mm");
  Serial.println("S<number>  → Set servo angle");
}

void loop() {

  if (Serial.available()) {

    String input = Serial.readStringUntil('\n');

    // Servo command
    if (input.startsWith("S")) {

      int angle = input.substring(1).toInt();

      if (angle >= servoMin && angle <= servoMax) {

        servoPos = angle;
        servo.write(servoPos);

        Serial.print("Servo angle set to: ");
        Serial.println(servoPos);

      } else {

        Serial.println("Angle out of range (0-68)");
      }
    }

    // Stepper command
    if (input.startsWith("M")) {

      float distance = input.substring(1).toFloat();

      long steps = distance * stepsPerMM;

      targetPosition += steps;

      stepper.enableOutputs();
      stepper.moveTo(targetPosition);

      Serial.print("Moving stepper: ");
      Serial.print(distance);
      Serial.println(" mm");
    }
  }

  stepper.run();

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
