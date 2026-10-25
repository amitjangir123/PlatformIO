#include <Arduino.h>

HardwareTimer *timer1 = new HardwareTimer(TIM1);

// Set this to your board's onboard LED pin, if it has one.
// #define LED_PIN PC13

void setup()
{
#ifdef LED_PIN
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
#endif
}

void loop()
{
    // 38 kHz PWM on PA8, 50% duty cycle
    timer1->setPWM(
        1,       // TIM1 channel 1
        PA8,     // output pin
        38000,   // frequency
        50       // duty cycle
    );
    delay(15);
    timer1->setPWM(
        1,       // TIM1 channel 1
        PA8,     // output pin
        38000,   // frequency
        0       // duty cycle
    );
    delay(15);
}
