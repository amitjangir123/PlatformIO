#include <Arduino.h>

// DRV8825 Stepper Motor Driver Pin Configuration for ESP32
// Adjust pins according to your wiring

// Motor 1 pins (you can add more motors by adding more pin definitions)
#define STEP_PIN_1    21  // GPIO21 - STEP signal
#define DIR_PIN_1     19  // GPIO19 - DIRECTION signal
#define ENABLE_PIN_1  18  // GPIO18 - ENABLE signal (active low)
#define SLP_PIN_1     22  // GPIO22 - SLEEP signal (HIGH = active, LOW = sleep)
#define RST_PIN_1     23  // GPIO23 - RESET signal (HIGH = normal, LOW = reset)

// Motor parameters
#define MICROSTEPS 1     // DRV8825 can do 1, 2, 4, 8, or 16 microsteps
#define STEPS_PER_REV 200 // NEMA 17 or most steppers: 200 steps per revolution
#define SPEED 80       // Speed in microseconds between steps (smaller = faster)

// Variables
volatile int stepCount = 0;
int direction = 1; // 1 for forward, -1 for backward

// Function to move motor by specified number of steps
void moveMotor(int steps, int dir) {
  // Set direction
  if (dir > 0) {
    digitalWrite(DIR_PIN_1, HIGH);
  } else {
    digitalWrite(DIR_PIN_1, LOW);
  }
  
  // Create step pulses
  for (int i = 0; i < abs(steps); i++) {
    digitalWrite(STEP_PIN_1, HIGH);
    delayMicroseconds(5);  // Pulse width
    digitalWrite(STEP_PIN_1, LOW);
    delayMicroseconds(SPEED);
    stepCount += dir;
  }
}

// Rotate motor by specified number of revolutions
void rotateRevolutions(float revolutions, int dir) {
  int totalSteps = (int)(revolutions * STEPS_PER_REV * MICROSTEPS / 16);
  moveMotor(totalSteps, dir);
}

// Rotate motor by specified degrees
void rotateDegrees(float degrees, int dir) {
  float revolutions = degrees / 360.0;
  rotateRevolutions(revolutions, dir);
}

void setup() {
  Serial.begin(115200);
  delay(2000); // Wait for Serial to initialize
  
  Serial.println("\n\n=================================");
  Serial.println("ESP32 Stepper Motor Test (DRV8825)");
  Serial.println("=================================\n");
  
  // Configure motor pins
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(ENABLE_PIN_1, OUTPUT);
  pinMode(SLP_PIN_1, OUTPUT);
  pinMode(RST_PIN_1, OUTPUT);
  
  // Initialize pins
  digitalWrite(STEP_PIN_1, LOW);
  digitalWrite(DIR_PIN_1, HIGH);
  digitalWrite(ENABLE_PIN_1, LOW);   // Enable the motor (active low)
  digitalWrite(SLP_PIN_1, HIGH);     // Wake up the driver (active high)
  digitalWrite(RST_PIN_1, HIGH);     // Normal operation (active high)
  
  Serial.println("Motor initialized and enabled");
  Serial.println("Testing sequence starting...\n");
  
  delay(1000);
}

void loop() {
  // Rotate 360 degrees clockwise
  //rotateDegrees(360, 1);  // Rotate 360 degrees clockwise
  moveMotor(2000, 1);
  delay(50);             // Pause between rotations
  
  // Rotate 360 degrees counter-clockwise
  //rotateDegrees(360, -1); // Rotate 360 degrees counter-clockwise
  moveMotor(2000, -1);
  delay(50);             // Pause between rotations
}