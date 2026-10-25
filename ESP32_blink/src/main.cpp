#include <Arduino.h>

// Simple blink example for ESP32 (uses built-in LED pin)
const uint8_t LED_PIN = LED_BUILTIN;
const unsigned long BLINK_INTERVAL_MS = 500;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_INTERVAL_MS);
  digitalWrite(LED_PIN, LOW);
  delay(BLINK_INTERVAL_MS);
}