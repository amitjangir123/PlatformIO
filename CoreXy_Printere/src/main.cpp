
#include <Arduino.h>
#include <AccelStepper.h>

// DRV8825 control pins on port C
// PC15 = ENABLE (active LOW), PC14 = STEP, PC13 = DIRECTION
const int PIN_ENABLE = PC15;
const int PIN_STEP = PC14;
const int PIN_DIR = PC13;

// AccelStepper in DRIVER mode uses (step, dir)
AccelStepper stepper(AccelStepper::DRIVER, PIN_STEP, PIN_DIR);

// Default parameters
float stepsPerSecond = 200.0f;
float maxSpeed = 1000.0f; // maximum allowed speed (steps/sec)
float acceleration = 2000.0f; // steps/sec^2

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_ENABLE, HIGH); // disable initially (DRV8825 active LOW)

  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(acceleration);

  Serial.println("AccelStepper DRV8825 on PC15=EN, PC14=STEP, PC13=DIR");
  Serial.println("Commands:");
  Serial.println("  r <Hz>   - set stepping rate in steps/sec (e.g. r 400)");
  Serial.println("  d <0|1>  - set direction (0=forward, 1=reverse)");
  Serial.println("  e <0|1>  - enable/disable motor (0=disable, 1=enable)");
  Serial.println("  a <accel> - set acceleration (steps/sec^2)");
  Serial.println("  m <max>   - set max speed (steps/sec)");
}

unsigned long lastPrint = 0;

void handleSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;
  char cmd = line.charAt(0);
  String arg = line.substring(1);
  arg.trim();
  if (cmd == 'r' || cmd == 'R') {
    float v = arg.toFloat();
    if (v > 0) stepsPerSecond = v;
    Serial.print("Rate set to: "); Serial.println(stepsPerSecond);
    // set direction by sign when running below
  } else if (cmd == 'd' || cmd == 'D') {
    int v = arg.toInt();
    if (v == 0) stepper.setSpeed(abs(stepsPerSecond));
    else stepper.setSpeed(-abs(stepsPerSecond));
    Serial.print("Direction: "); Serial.println(v);
  } else if (cmd == 'e' || cmd == 'E') {
    int v = arg.toInt();
    digitalWrite(PIN_ENABLE, v ? LOW : HIGH); // active LOW
    Serial.print("Enabled: "); Serial.println(v ? "YES" : "NO");
  } else if (cmd == 'a' || cmd == 'A') {
    float v = arg.toFloat();
    if (v > 0) {
      acceleration = v;
      stepper.setAcceleration(acceleration);
      Serial.print("Acceleration set: "); Serial.println(acceleration);
    }
  } else if (cmd == 'm' || cmd == 'M') {
    float v = arg.toFloat();
    if (v > 0) {
      maxSpeed = v;
      stepper.setMaxSpeed(maxSpeed);
      Serial.print("Max speed set: "); Serial.println(maxSpeed);
    }
  } else {
    Serial.println("Unknown command");
  }
}

void loop() {
  handleSerial();

  // If enabled, run at configured speed
  if (digitalRead(PIN_ENABLE) == LOW) {
    // Use current speed sign; if speed not set via direction, set now
    if (stepper.speed() == 0) {
      stepper.setSpeed(stepsPerSecond);
    }
    // ensure maxSpeed is respected
    if (abs(stepper.speed()) > stepper.maxSpeed()) {
      stepper.setSpeed((stepper.speed() > 0 ? 1 : -1) * stepper.maxSpeed());
    }
    stepper.runSpeed();
  } else {
    // disabled: small idle delay and periodic status print
    if (millis() - lastPrint > 2000) {
      Serial.print("Disabled. Rate="); Serial.println(stepsPerSecond);
      lastPrint = millis();
    }
    delay(10);
  }
}


