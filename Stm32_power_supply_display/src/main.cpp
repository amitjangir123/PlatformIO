#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

const int analogPin = A0;      // PA0 on STM32 Bluepill (voltage)
const int currentPin = A1;     // PA1 on STM32 Bluepill (shunt)
const int adcMax = 4095;       // 12-bit ADC resolution for STM32
const uint32_t vref_mV = 3300; // STM32 Bluepill ADC reference in mV
const uint32_t dividerScale_milli = 5550; // 5.55x in milli scale
// Use U8x8 (text-only) to keep flash size small on F103C6
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);


static void formatVoltage(uint32_t mv, char *out, size_t outLen) {
  // Produces a fixed-width 5-char string: " 9.45" or "12.34"
  if (outLen < 6) {
    if (outLen > 0) out[0] = '\0';
    return;
  }
  uint32_t v = mv / 1000;
  uint32_t frac = (mv % 1000) / 10; // two decimals

  size_t i = 0;
  if (v >= 10) {
    out[i++] = '0' + (v / 10);
    out[i++] = '0' + (v % 10);
  } else {
    out[i++] = ' ';
    out[i++] = '0' + (v % 10);
  }
  out[i++] = '.';
  out[i++] = '0' + (frac / 10);
  out[i++] = '0' + (frac % 10);
  out[i++] = 'V';
  out[i] = '\0';
}

static void formatCurrent(uint32_t mA, char *out, size_t outLen) {
  // Produces a fixed-width 6-char string: " 1.23A" or "12.34A"
  if (outLen < 7) {
    if (outLen > 0) out[0] = '\0';
    return;
  }
  uint32_t centiA = mA / 10; // 0.01A units
  uint32_t a = centiA / 100;
  uint32_t frac = centiA % 100;

  size_t i = 0;
  if (a >= 10) {
    out[i++] = '0' + (a / 10);
    out[i++] = '0' + (a % 10);
  } else {
    out[i++] = ' ';
    out[i++] = '0' + a;
  }
  out[i++] = '.';
  out[i++] = '0' + (frac / 10);
  out[i++] = '0' + (frac % 10);
  out[i++] = 'A';
  out[i] = '\0';
}

void setup() {
  analogReadResolution(12);
  Wire.begin();   // default I2C pins: PB6=SCL, PB7=SDA on Bluepill
  u8x8.setI2CAddress(0x3C << 1);
  u8x8.begin();
  u8x8.setFont(u8x8_font_px437wyse700b_2x2_f);
}

void loop() {
  int raw = analogRead(analogPin);
  uint64_t num = (uint64_t)raw * vref_mV * dividerScale_milli;
  uint32_t mv = num / ((uint64_t)adcMax * 1000);

  int rawCurrent = analogRead(currentPin);
  uint32_t mvShunt = ((uint64_t)rawCurrent * vref_mV) / adcMax;
  uint32_t current_mA = mvShunt * 10; // 0.1 ohm shunt -> mA = mV * 10

  char lineV[16];
  char lineI[16];
  formatVoltage(mv, lineV, sizeof(lineV));
  formatCurrent(current_mA, lineI, sizeof(lineI));
  u8x8.setCursor(0, 0);
  u8x8.print(lineV);
  u8x8.setCursor(0, 2);
  u8x8.print(lineI);


  delay(500);
}
