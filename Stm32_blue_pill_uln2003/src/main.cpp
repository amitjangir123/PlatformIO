#include <Arduino.h>

// ULN2003 driver (full-step) for STM32 Blue Pill
// Wiring: ULN2003 IN1..IN4 -> PB3, PB4, PB5, PB6 respectively

const uint8_t motorPins[4] = { PB3, PB4, PB5, PB6 };

const uint8_t motor2Pins[4] = { PA8, PA9, PA10, PA11 }; // 2nd motor

// Half-step sequence (8 states): alternates single- and two-coil states
// States: 1000,1100,0100,0110,0010,0011,0001,1001
const int SEQ_LEN = 8;
const uint8_t halfStep[SEQ_LEN][4] = {
  { 1, 0, 0, 0 },
  { 1, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 1, 0 },
  { 0, 0, 1, 1 },
  { 0, 0, 0, 1 },
  { 1, 0, 0, 1 }
};

// per-motor step indices so each motor can be driven independently
int stepIndex1 = 0; // motor 1 (0..7)
int stepIndex2 = 0; // motor 2 (0..7)

// Step a specific motor pins array
void stepMotor(const uint8_t pins[4], int &index, int steps, int dir, unsigned int stepDelayUs) {
  for (int s = 0; s < steps; ++s) {
    index = (index + dir + SEQ_LEN) % SEQ_LEN;
    for (int i = 0; i < 4; ++i) {
      digitalWrite(pins[i], halfStep[index][i] ? HIGH : LOW);
    }
    delayMicroseconds(stepDelayUs);
  }
}

// Step both motors together (write both pin sets each sub-step)
void stepBothMotors(int steps, int dir1, int dir2, unsigned int stepDelayUs) {
  for (int s = 0; s < steps; ++s) {
    stepIndex1 = (stepIndex1 + dir1 + SEQ_LEN) % SEQ_LEN;
    stepIndex2 = (stepIndex2 + dir2 + SEQ_LEN) % SEQ_LEN;

    for (int i = 0; i < 4; ++i) {
      digitalWrite(motorPins[i], halfStep[stepIndex1][i] ? HIGH : LOW);
      digitalWrite(motor2Pins[i], halfStep[stepIndex2][i] ? HIGH : LOW);
    }

    delayMicroseconds(stepDelayUs);
  }
}

void setup() {
  // Initialize serial for quick debugging
  Serial.begin(115200);
  delay(50);
  Serial.println("ULN2003 half-step driver starting");

  // Configure pins
  for (int i = 0; i < 4; ++i) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
    pinMode(motor2Pins[i], OUTPUT);
    digitalWrite(motor2Pins[i], LOW);
  }
}

void loop() {
  // Example: rotate 512 steps (one or two revolutions depending on motor) each direction
  const unsigned int stepDelay = 1000; // µs per step (adjust speed)
  const int stepsPerMove = 2048 * 2;

  Serial.println("Rotating CW both motors");
  stepBothMotors(stepsPerMove, 1, 1, stepDelay);
  //delay(500);

  
  //Serial.println("Rotating CCW both motors");
  //stepBothMotors(stepsPerMove, -1, -1, stepDelay);
  //delay(500);
}