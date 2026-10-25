#include <Arduino.h>

// Pin definitions for STM32 Bluepill (PB3-PB7)
// Motor A: IN1 = PB3, IN2 = PB4, ENA = PB5
// Motor B: IN3 = PB6, IN4 = PB7, ENB = PB5 (if using same enable, else PB5 for A, PB8 for B)
#define IN1 PB3
#define IN2 PB4
#define ENA PA0
#define IN3 PB5
#define IN4 PB6
#define ENB PA1 // If you have ENB on PB8, else set to ENA if both motors share enable

// Motor control functions with speed
void motorA_forward(uint8_t speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
}

void motorA_backward(uint8_t speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
}

void motorA_stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void motorB_forward(uint8_t speed) {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speed);
}

void motorB_backward(uint8_t speed) {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, speed);
}

void motorB_stop() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

// Example: drive both motors forward at a given speed
void motors_forward(uint8_t speedA, uint8_t speedB) {
  motorA_forward(speedA);
  motorB_forward(speedB);
}


void motors_backward(uint8_t speedA, uint8_t speedB) {
  motorA_backward(speedA);
  motorB_backward(speedB);
}

void motors_left(uint8_t speedA, uint8_t speedB) {
  motorA_backward(speedA);
  motorB_forward(speedB);
}

void motors_right(uint8_t speedA, uint8_t speedB) {
  motorA_forward(speedA);
  motorB_backward(speedB);
}

void motors_stop() {
  motorA_stop();
  motorB_stop();
}

// You may need to adjust ENB if both motors share the same enable pin (L298N variant)
// If only one enable pin is available, set ENB to ENA

void setup() {
  // Initialize motor control pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Stop motors at startup
  motors_stop();
}

void loop() {
  // Example: Move forward at half speed for 2 seconds
  motors_forward(50, 50);
  delay(2000);

  /*
  // Move backward at full speed for 2 seconds
  motors_backward(50, 50);
  delay(2000);

  // Turn left for 1 second (left motor backward, right motor forward)
  motors_left(50, 50);
  delay(1000);

  // Turn right for 1 second (left motor forward, right motor backward)
  motors_right(50, 50);
  delay(1000);

  // Stop for 2 seconds
  motors_stop();
  delay(2000);
  */
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}