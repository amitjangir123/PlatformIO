#include <Arduino.h>

HardwareSerial uartPort(2);

const int uartRxPin = 16;
const int uartTxPin = 17;
const unsigned long sendIntervalMs = 2000;
unsigned long previousSendMs = 0;

void setup() {
  uartPort.begin(115200, SERIAL_8N1, uartRxPin, uartTxPin);
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMs = millis();

  if (currentMs - previousSendMs >= sendIntervalMs) {
    previousSendMs = currentMs;
    uartPort.println("Hello");
    Serial.println("Sent: Hello");
  }

  if(uartPort.available()) 
  {
    String receivedData = uartPort.readStringUntil('\n');
    Serial.println(receivedData);
  }
}
