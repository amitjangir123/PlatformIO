#include <Arduino.h>


#define LED_BUILTIN PC13

// Configure this to the digital pin where the IR receiver output is connected.
// Change if you wired the receiver to a different pin.
const uint8_t IR_IN_PIN = PA0;
const uint8_t IR_OUT_PIN = PB0; // Pin to indicate IR signal detection (e.g., an LED)
volatile bool IR_state = false; // Variable to hold the state of the IR signal
uint64_t irCheckTimeStamp = 0;
uint8_t i = 0;
bool Samples[10] = {false, false, false, false, false, false, false, false, false, false};


constexpr uint8_t MOTOR1_DIR_PIN = PA5;
constexpr uint8_t MOTOR1_STEP_PIN = PA6;
constexpr uint8_t MOTOR1_ENABLE_PIN = PA7;

constexpr uint8_t MOTOR2_STEP_PIN = PA8;
constexpr uint8_t MOTOR2_DIR_PIN = PA9;
constexpr uint8_t MOTOR2_ENABLE_PIN = PB15;

constexpr uint8_t DRIVER_ENABLE_LEVEL = LOW;
constexpr uint8_t DRIVER_DISABLE_LEVEL = HIGH;

int32_t leftSpeed = 0;
int32_t rightSpeed = 0;
uint32_t leftSteps = 0;
uint32_t rightSteps = 0;

HardwareTimer motor1StepTimer(TIM3);
HardwareTimer motor2StepTimer(TIM1);

HardwareSerial SerialUART(PA3, PA2); // RX, TX

volatile uint32_t motor1StepsDone = 0;
volatile uint32_t motor2StepsDone = 0;
volatile bool motor1StepPinHigh = false;
volatile bool motor2StepPinHigh = false;

void enableDrivers() {
  digitalWrite(MOTOR1_ENABLE_PIN, DRIVER_ENABLE_LEVEL);
  digitalWrite(MOTOR2_ENABLE_PIN, DRIVER_ENABLE_LEVEL);
}

void disableLeftDriver()
{
  digitalWrite(MOTOR2_ENABLE_PIN, DRIVER_DISABLE_LEVEL);    
}

void disableRightDriver()
{
  digitalWrite(MOTOR1_ENABLE_PIN, DRIVER_DISABLE_LEVEL);
}
void motor1StepTimerCallback() {
  if (motor1StepPinHigh) {
    digitalWrite(MOTOR1_STEP_PIN, LOW);
    motor1StepPinHigh = false;

    if (motor1StepsDone >= rightSteps) {
      motor1StepTimer.pause();
      disableLeftDriver();

    }
    return;
  }

  if (motor1StepsDone < rightSteps) {
    digitalWrite(MOTOR1_STEP_PIN, HIGH);
    motor1StepPinHigh = true;
    motor1StepsDone++;
  } else {
    motor1StepTimer.pause();
    digitalWrite(MOTOR1_STEP_PIN, LOW);
  }
}

void motor2StepTimerCallback() {
  if (motor2StepPinHigh) {
    digitalWrite(MOTOR2_STEP_PIN, LOW);
    motor2StepPinHigh = false;

    if (motor2StepsDone >= leftSteps) {
      motor2StepTimer.pause();
      disableRightDriver();
    }
    return;
  }

  if (motor2StepsDone < leftSteps) {
    digitalWrite(MOTOR2_STEP_PIN, HIGH);
    motor2StepPinHigh = true;
    motor2StepsDone++;
  } else {
    motor2StepTimer.pause();
    digitalWrite(MOTOR2_STEP_PIN, LOW);

  }
}

void startMotorLeft(uint32_t steps, uint32_t speed) {
  enableDrivers();
  motor2StepTimer.pause();
  motor2StepTimer.detachInterrupt();

  leftSteps = steps;

  motor2StepsDone = 0;
  motor2StepPinHigh = false;
  digitalWrite(MOTOR2_STEP_PIN, LOW);

  motor2StepTimer.setOverflow(speed * 2, HERTZ_FORMAT);
  motor2StepTimer.attachInterrupt(motor2StepTimerCallback);
  motor2StepTimer.refresh();
  motor2StepTimer.resume();
}

void startMotorRight(uint32_t steps, uint32_t speed) {
  enableDrivers();
  motor1StepTimer.pause();
  motor1StepTimer.detachInterrupt();

  rightSteps = steps;
  motor1StepsDone = 0;
  motor1StepPinHigh = false;
  digitalWrite(MOTOR2_STEP_PIN, LOW);

  motor1StepTimer.setOverflow(speed * 2, HERTZ_FORMAT);
  motor1StepTimer.attachInterrupt(motor1StepTimerCallback);
  motor1StepTimer.refresh();
  motor1StepTimer.resume();
}

void parse_line(int32_t &leftSpeed, int32_t &rightSpeed, uint32_t &leftSteps, uint32_t &rightSteps, const String &line) {
  int lIndex = line.indexOf("L:");
  int rIndex = line.indexOf("R:");
  int lsIndex = line.indexOf("LS:");
  int rsIndex = line.indexOf("RS:");

  if (lIndex != -1 && rIndex != -1) {
    leftSpeed = line.substring(lIndex + 2, lsIndex).toInt();
    rightSpeed = line.substring(rIndex + 2, rsIndex).toInt();
    leftSteps = line.substring(lsIndex + 3, rsIndex).toInt();
    rightSteps = line.substring(rsIndex + 3).toInt();
  }
}

void blinkLED(uint8_t pin, uint32_t duration, uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    digitalWrite(pin, LOW);
    delay(duration);
    digitalWrite(pin, HIGH);
    delay(duration);
  }
}

void drive_motors(int32_t leftSpeed, int32_t rightSpeed, uint32_t leftSteps, uint32_t rightSteps) {
  if (leftSpeed > 0) {
    digitalWrite(MOTOR2_DIR_PIN, HIGH);
    startMotorLeft(leftSteps, leftSpeed);
  } else if (leftSpeed < 0) {
    digitalWrite(MOTOR2_DIR_PIN, LOW);
    startMotorLeft(leftSteps, -leftSpeed);
  }
  else
  {
    disableLeftDriver();
  }

  if (rightSpeed > 0) {
    digitalWrite(MOTOR1_DIR_PIN, HIGH);
    startMotorRight(rightSteps, rightSpeed);
  } else if (rightSpeed < 0) {
    digitalWrite(MOTOR1_DIR_PIN, LOW);
    startMotorRight(rightSteps, -rightSpeed);
  }
  else
  {
    disableRightDriver();
  }
}

void check_ir_state() 
{

  if((millis() - irCheckTimeStamp) >= 3) // every 3 milliseconds, check the IR state
  {
    irCheckTimeStamp = millis(); // Update the last check time
    Samples[i] = !digitalRead(IR_IN_PIN);
    if(i == 9)
    {
        i = 0; // Reset index for the next set of samples
        // Check if at least 3 out of 10 samples are HIGH
        int highCount = 0;
        for(int j = 0; j < 10; j++) {
          if(Samples[j]) {
            highCount++;
          }
        }

        //Reset Samples array for the next set of readings
        for(int j = 0; j < 10; j++) {
          Samples[j] = false;
        }

        if(highCount >= 3)
        {
          digitalWrite(LED_BUILTIN, LOW); // Turn on the built-in LED to indicate IR signal detection
          digitalWrite(IR_OUT_PIN, HIGH); // Turn on the output pin (e.g., LED)
        }
        else
        {
          digitalWrite(LED_BUILTIN, HIGH); // Turn off the built-in LED
          digitalWrite(IR_OUT_PIN, LOW); // Turn off the output pin (e.g., LED)
        }
    }

    i++;
  }
}

void setup() {
  pinMode(MOTOR1_DIR_PIN, OUTPUT);
  pinMode(MOTOR1_STEP_PIN, OUTPUT);
  pinMode(MOTOR1_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR2_STEP_PIN, OUTPUT);
  pinMode(MOTOR2_DIR_PIN, OUTPUT);
  pinMode(MOTOR2_ENABLE_PIN, OUTPUT);
  pinMode(PC13, OUTPUT);
  pinMode(IR_IN_PIN, INPUT);
  pinMode(IR_OUT_PIN, OUTPUT);

  disableLeftDriver();
  disableRightDriver();

  SerialUART.begin(115200);

  digitalWrite(MOTOR1_STEP_PIN, LOW);
  digitalWrite(MOTOR2_STEP_PIN, LOW);
  digitalWrite(MOTOR1_DIR_PIN, HIGH);
  digitalWrite(MOTOR2_DIR_PIN, HIGH);

  digitalWrite(PC13, HIGH);
 
}


void loop() {

  if(SerialUART.available())
  {
    //blinkLED(PC13, 100, 3);
    String line = SerialUART.readStringUntil('\n');
    line.trim(); // Remove any leading/trailing whitespace

    parse_line(leftSpeed, rightSpeed, leftSteps, rightSteps, line);
    drive_motors(leftSpeed, rightSpeed, leftSteps, rightSteps);
    
  }

  check_ir_state();

  //delay(1000);
}
