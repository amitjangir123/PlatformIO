#include <Arduino.h>
#include <math.h>

#define PWM_PIN PA0
#define OUT_VOLTAGE PA1


HardwareSerial Serial1(PA10, PA9);

uint64_t global_counter = 2000;



float readActualVoltage() {
  float sum = 0.0f;
  float read_value = 0.0f;

  for(int i=0;i<1000;i++){
    read_value = (float)analogRead(PA1);
    sum += read_value;
  }

  sum = (sum / 1000.0f); // Average the 1000 readings
  return (sum * 3.3) / (4096.0f * 0.192); // Convert ADC reading to voltage
}

float readActualCurrent() {
  float sum = 0.0f;

  for(int i=0;i<2000;i++){
    sum += (float)analogRead(PB0);
  }

  sum = (sum / 2000.0f) - 14; // Average the 2000 readings
  return ((((sum * 3.3)/4095.0f)/0.109) * 1000.0f); // Convert ADC reading to current in mA
}




uint64_t set_voltage(float target_voltage) {

  uint64_t counter = global_counter; // Start with a mid-range duty cycle (for 10-bit resolution);
  float voltage = 0.0f;
  int64_t error = 0;
  float tolerance = 50.0f; // Adjust tolerance as needed for voltage matching
  float delayValue = 1000; // Initial delay value in milliseconds

  float p = 30.0f; // Proportional gain for control loop


  Serial1.print("Setting voltage to: ");
  Serial1.print(target_voltage, 2);
  Serial1.println(" V");

  delay(1000); // Wait for 1 second before starting voltage adjustment


  // Convert target to mV for better precision in calculations
  target_voltage *= 1000.0f;

  // Check Error
  voltage = readActualVoltage() * 1000.0f; // Convert to mV for error calculation
  error = (int64_t)((target_voltage - voltage));

  if (error <= tolerance) {
    return counter; // Voltage is already close enough to target
  }

  while (true) {
    voltage = readActualVoltage() * 1000.0f; // Convert to mV for error calculation

    Serial1.print("Current voltage: ");
    Serial1.print(voltage, 2);
    Serial1.println(" mV");
    error = (int64_t)((target_voltage - voltage));

    Serial1.print("Voltage error: ");
    Serial1.print(error, 2);
    Serial1.println(" mV");

    Serial1.print("Error: ");
    Serial1.print(fabsf(error), 2);
    Serial1.println(" mV");

    if (error <= tolerance) {
      return counter; // Voltage is close enough to target
    }

    // Proportional control adjustment
    int64_t adjustment = (int64_t)(p * error/1000.0f); // Scale error for adjustment

    counter = counter - adjustment;
    
    Serial1.print("Updated counter value:");
    Serial1.println(counter);

    analogWrite(PA0, counter);

    if(fabsf(error) >= 2000) {
      delayValue = 1000; // Larger error, faster response
    } else if (fabsf(error) >= 1000) {
      delayValue = 500; // Moderate error, moderate response
    } else {
      delayValue = 500; // Small error, fine-tuning
    }
    delay(delayValue); // Small delay to allow voltage to stabilize

    Serial1.println("=============");
  }

  
}

uint64_t set_current(float target_current, float max_voltage) {

  Serial1.print("Setting current to: ");
  Serial1.print(target_current, 2);
  Serial1.println(" mA");
  const float tolerance = 5; // Adjust tolerance as needed for current matching
  float current = 0.0;
  float voltage = 0.0;
  uint16_t counter = global_counter; // Start with a mid-range duty cycle (for 10-bit resolution);
  uint64_t readValue = 0;

  // Check if current voltage is already close enough to target
  current = readActualCurrent();

  if (fabsf(current - target_current) <= tolerance) {
    return counter; // Current is already close enough to target
  }

  while (true) {

    current = readActualCurrent();
    voltage = readActualVoltage();

    if(voltage > max_voltage) {
      Serial1.println("Voltage exceeded max limit, stopping current adjustment.");
      return counter; // Stop if voltage exceeds max limit
    }
    
    if (fabsf(current - target_current) <= tolerance) {
      break; // Current is close enough to target
    }
    
    if (current > target_current) {
      counter = (counter < 4000) ? counter + 10 : 4095;
    } else if (current < target_current) {
      counter = (counter > 10) ? counter - 10 : 0;
    }
    
    analogWrite(PA0, counter);
    delay(10); // Small delay to allow voltage to stabilize
  }


  // fine-tuning loop to get as close as possible to target voltage
  while (true) {

    current = readActualCurrent();

    if (fabsf(current - target_current) <= tolerance) {
      break; // Current is close enough to target
    }
    if (current > target_current) {
      counter = (counter < 4095) ? counter + 1 : 4095;
    } else if (current < target_current) {
      counter = (counter > 0) ? counter - 1 : 0;
    }
    analogWrite(PA0, counter);
    delay(10); // Small delay to allow voltage to stabilize
  }

  return counter;
}


void battery_charging() {

  Serial1.println("Starting battery charging...");

  delay(5000); // Wait for 5 seconds before starting charging

  //digitalWrite(PA5, LOW); // Turn on the load (assuming active low)
  float voltage = readActualVoltage();
  float current = readActualCurrent();

  //digitalWrite(PA5, HIGH); // Turn on the load (assuming active high)

  Serial1.print("Voltage: ");
  Serial1.print(voltage, 2);
  Serial1.println(" V");

  Serial1.print("Current: ");
  Serial1.print(current, 2);
  Serial1.println(" mA");

  delay(2000); // Wait for 1 second before starting charging

  if(voltage < 4.2) {

    while(voltage < 4.2) {
      set_current(250, 4.2); // Set target current to 250 mA
      voltage = readActualVoltage();
      current = readActualCurrent();

      Serial1.print("Voltage_cc: ");
      Serial1.print(voltage, 2);
      Serial1.println(" V");

      Serial1.print("Current_cc: ");
      Serial1.print(current, 2);
      Serial1.println(" mA");

      delay(2000); // Wait for 2 seconds before next reading
    }
    
  }
  else {
    // CV mode: Set voltage to 4.2V and let current taper off
    while(current > 50) { // Continue until current drops below 50 mA
      set_voltage(4.2); // Set target voltage to 4.2V
      voltage = readActualVoltage();
      current = readActualCurrent();
      Serial1.print("Voltage: ");
      Serial1.print(voltage, 2);
      Serial1.println(" V");
      Serial1.print("Current: ");
      Serial1.print(current, 2);
      Serial1.println(" mA");
      delay(2000); // Wait for 2 seconds before next reading
    }
  }
}

void setup() {
  pinMode(PA0, OUTPUT);
  pinMode(PA1, INPUT);
  pinMode(PB0, INPUT);
  pinMode(PA5, OUTPUT);
  analogWriteFrequency(50000); // 50 kHz PWM
  analogWriteResolution(12);   // 12-bit duty range (0..4095)
  analogReadResolution(12);

  digitalWrite(PA5, HIGH); // Turn on the load (assuming active high)

  
  //set_voltage(4.2); // Pre-charge to 4.2V before starting the main charging loop
  analogWrite(PA0, 2500); // Start with 0% duty cycle

  Serial1.begin(115200);
}

uint64_t counter = 600;
void loop() {
  // uint64_t readValue = 0;
  float Current = 0.0f;
  float Voltage = 0.0f;
  uint64_t delayValue = 1000;


  //global_counter =  set_voltage(8.2); // Set target voltage to 4.2V

  //global_counter = set_voltage(2.9); // Set target voltage to 4.2V

  analogWrite(PA0,counter);

  delay(100); // Wait for 2 seconds before next reading

  Voltage = readActualVoltage();
  Serial1.print("Voltage: ");
  Serial1.print(Voltage, 2);
  Serial1.print(" V ===");

  counter = counter + 1;

  Serial1.print("Counter: ");
  Serial1.println(counter);


  


  
  // Serial1.print("Actual voltage: ");
  // Voltage = readActualVoltage();
  // Serial1.print(Voltage, 2);
  // Serial1.println(" V");

  // Serial1.print("Current : ");
  // Current = readActualCurrent();
  // Serial1.print(Current, 2);
  // Serial1.println(" mA");

  //delay(10); // Wait for 5 seconds before next reading

  //battery_charging();

  //set_voltage(4.2); // Set target voltage to 4.2V
  
  // float voltage = readActualVoltage();
  // float current = readActualCurrent();
  // Serial1.print("Voltage: ");
  // Serial1.print(voltage, 2);
  // Serial1.println(" V");

  // delay(1000); // Wait for 2 seconds before next reading


  
}
