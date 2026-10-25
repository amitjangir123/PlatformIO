// Read analog voltage on an ESP32 ADC pin and print it.
#include <Arduino.h>
// Choose an ADC-capable pin. GPIO34 is input-only and safe for ADC.
const int ADC_PIN = 34;
const float ADC_MAX = 4095.0f;  // 12-bit default
const float VREF = 3.3f;        // Adjust if your board uses a different reference

void setup() {
  Serial.begin(115200);
  delay(100);

  // Configure ADC attenuation to allow up to ~3.3V range.
  analogSetPinAttenuation(ADC_PIN, ADC_11db);
}

void loop() {
  int sum = 0;
  int raw = 0;

  for (int i = 0; i < 100; i++) {
    raw = analogRead(ADC_PIN);
    sum += raw;
  }

  raw = sum / 100; // Average of 10 readings
    
  float voltage = (raw / ADC_MAX) * VREF * 5.8f; // Scale to voltage based on attenuation

  Serial.print("ADC raw: ");
  Serial.print(raw);
  Serial.print("  Voltage: ");
  Serial.print(voltage, 3);
  Serial.println(" V");

  delay(1000);
}
