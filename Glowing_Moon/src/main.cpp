#include <Arduino.h>

#define RED PB1
#define BLUE PB3
#define GREEN PB5

// Initialize LED pins
void initLED() {
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(GREEN, LOW);
}

// Set LED brightness (0-255)
void setLEDBrightness(uint8_t ledColor, uint8_t brightness) {
  uint8_t pwmValue = brightness;
  
  if (ledColor == 0) {  // RED
    analogWrite(RED, pwmValue);
  } else if (ledColor == 1) {  // BLUE
    analogWrite(BLUE, pwmValue);
  } else if (ledColor == 2) {  // GREEN
    analogWrite(GREEN, pwmValue);
  }
}

void setup() {
  initLED();
  
  // Set initial brightness
  setLEDBrightness(0, 90);   // BLUE: 90
  setLEDBrightness(1, 1);    // GREEN: 1
  setLEDBrightness(2, 1);    // RED: 1
}

void pattern_random_colours_smooth()
{

  uint64_t delayTime = 10;
  uint8_t oldBlueBrightness = 0;
  uint8_t oldGreenBrightness = 0;
  uint8_t oldRedBrightness = 0;
  while(1)
  {

    uint8_t BlueBrightness = random(0, 255);
    uint8_t GreenBrightness = random(0, 255);
    uint8_t RedBrightness = random(0, 255);
  

    // Smooth transition
    if(oldBlueBrightness > BlueBrightness)
    {
      for(oldBlueBrightness; oldBlueBrightness > BlueBrightness; oldBlueBrightness--)
      {
        setLEDBrightness(0, oldBlueBrightness);
        delay(delayTime);
      }
    }
    else
    {
      for(oldBlueBrightness; oldBlueBrightness <= BlueBrightness; oldBlueBrightness++)
      {
        setLEDBrightness(0, oldBlueBrightness);
        delay(delayTime);
      }
    }

    if(oldGreenBrightness > GreenBrightness)
    {
      for(oldGreenBrightness; oldGreenBrightness > GreenBrightness; oldGreenBrightness--)
      {
        setLEDBrightness(1, oldGreenBrightness);
        delay(delayTime);
      }
    }
    else
    {
      for(oldGreenBrightness; oldGreenBrightness <= GreenBrightness; oldGreenBrightness++)
      {
        setLEDBrightness(1, oldGreenBrightness);
        delay(delayTime);
      }
    }

    if(oldRedBrightness > RedBrightness)
    {
      for(oldRedBrightness; oldRedBrightness > RedBrightness; oldRedBrightness--)
      {
        setLEDBrightness(2, oldRedBrightness);
        delay(delayTime);
      }
    }
    else
    {
      for(oldRedBrightness; oldRedBrightness <= RedBrightness; oldRedBrightness++)
      {
        setLEDBrightness(2, oldRedBrightness);
        delay(delayTime);
      }
    }
    

    oldBlueBrightness = BlueBrightness;
    oldGreenBrightness = GreenBrightness;
    oldRedBrightness = RedBrightness;

  }
}
void pattern_full_white_moon()
{
  setLEDBrightness(0, 250);   // BLUE: 255
  setLEDBrightness(1, 255);    // GREEN: 255
  setLEDBrightness(2, 255);    // RED: 255
}

void loop() {
  pattern_full_white_moon();
  //pattern_bloody_moon();
  //pattern_random_colours_smooth();
}