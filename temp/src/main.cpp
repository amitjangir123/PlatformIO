#include <Arduino.h>
#include <U8g2lib.h>

// OLED Display (0.91" SSD1306) - I2C: PB6 (SCL), PB7 (SDA)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ PB6, /* data=*/ PB7, /* reset=*/ U8X8_PIN_NONE);

// Rotary Encoder Pins
#define ENCODER_CLK PA2
#define ENCODER_DT PA1
#define ENCODER_SW PA0

// Encoder variables
volatile int encoderValue = 0;
volatile int lastCLK = 0;
volatile int lastDT = 0;
int buttonPressed = 0;
unsigned long lastButtonTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);
  
  // Initialize OLED display
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(15, 20, "Encoder Test");
  u8g2.drawStr(20, 40, "Initializing...");
  u8g2.sendBuffer();
  delay(2000);
  
  // Setup encoder pins
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  // Read initial state
  lastCLK = digitalRead(ENCODER_CLK);
  lastDT = digitalRead(ENCODER_DT);
  
  Serial.println("\n=== Rotary Encoder + OLED Test ===");
  Serial.println("Rotate the encoder or press the button...");
  Serial.println("Values will display on OLED + Serial Monitor");
}

void displayUpdate() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 12, "ENCODER VALUE:");
  
  // Display encoder value in large font
  u8g2.setFont(u8g2_font_ncenB24_tr);
  char valueStr[16];
  sprintf(valueStr, "%d", encoderValue);
  u8g2.drawStr(20, 45, valueStr);
  
  // Display button count
  u8g2.setFont(u8g2_font_ncenB08_tr);
  sprintf(valueStr, "Button: %d", buttonPressed);
  u8g2.drawStr(0, 64, valueStr);
  
  u8g2.sendBuffer();
}

void loop() {
  // Read encoder pins
  int currentCLK = digitalRead(ENCODER_CLK);
  int currentDT = digitalRead(ENCODER_DT);
  
  // Detect rotation
  if (currentCLK != lastCLK) {
    if (currentCLK == LOW) {  // CLK goes LOW
      if (currentDT == LOW) {
        encoderValue++;  // Clockwise
        Serial.print(">>> Clockwise | Value: ");
        Serial.println(encoderValue);
      } else {
        encoderValue--;  // Counter-Clockwise
        Serial.print("<<< Counter-Clockwise | Value: ");
        Serial.println(encoderValue);
      }
      displayUpdate();  // Update OLED when encoder moves
    }
    lastCLK = currentCLK;
  }
  
  lastDT = currentDT;
  
  // Check button press with debouncing
  if (digitalRead(ENCODER_SW) == LOW) {
    if (millis() - lastButtonTime > DEBOUNCE_DELAY) {
      buttonPressed++;
      lastButtonTime = millis();
      Serial.print("!!! Button Pressed! Total Count: ");
      Serial.println(buttonPressed);
      displayUpdate();  // Update OLED when button pressed
    }
  }
  
  delay(5);  // Small delay to avoid overwhelming the loop
}


