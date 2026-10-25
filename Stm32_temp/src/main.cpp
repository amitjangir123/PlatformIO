#include <Arduino.h>

#define LED_BUILTIN PC13

// Configure this to the digital pin where the IR receiver output is connected.
// Change if you wired the receiver to a different pin.
const uint8_t IR_PIN = PA0;
bool IR_state = false;
uint32_t high_counter = 0;
uint32_t low_counter = 0;

void setup() {
  // IR receiver typically pulls LOW when a signal is detected.
  pinMode(IR_PIN, INPUT_PULLUP);

  // Built-in LED (board-specific). PlatformIO/Arduino should provide LED_BUILTIN.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PB0, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PB0, LOW); // Turn on the LED to indicate the program is running
}

void loop() {

    IR_state = !digitalRead(IR_PIN);

    if(IR_state)
    {
      high_counter += 1;
      low_counter = 0;
      if(high_counter > 80)
      {
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(PB0, HIGH); // Turn on the LED to indicate IR signal detected
        high_counter = 0;
        low_counter = 0;
      }
    }
    else
    {
      low_counter += 1;
      if(low_counter > 120)
      {
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(PB0, LOW); // Turn off the LED to indicate no IR signal detected
        low_counter = 0;
      }
    }

    delay(1); // Delay for 1 millisecond to avoid busy-waiting

}