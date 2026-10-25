#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Common variables and definitions
static QueueHandle_t moveQueue = NULL;

struct MoveCommand {
  uint8_t dir; // 0 for backward, 1 for forward, 2 right turn, 3 left turn
  uint32_t distance; // distance in mm for forward/backward, angle in degrees for turns
};

const char* WIFI_SSID = "Airtel_Jangir_2G";
const char* WIFI_PASS = "12341234";
const uint16_t WIFI_PORT = 80;

WebServer server(WIFI_PORT);

// New WiFi/task + web UI to receive stepper 
/*
void handleRoot() {
  const char* page =
    "<!doctype html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<title>Stepper Control</title></head><body>"
    "<h3>Send Step Command</h3>"
    "<form action=\"/move\" method=\"get\">"
    "Steps: <input type=\"number\" name=\"steps\" value=\"200\" min=\"1\"><br><br>"
    "Direction: <select name=\"dir\"><option value=\"1\">Forward</option><option value=\"0\">Backward</option></select><br><br>"
    "<input type=\"submit\" value=\"Send\">"
    "</form></body></html>";
  server.send(200, "text/html", page);
}

void handleMove() {
  StepCommand cmd;
  if (server.hasArg("steps")) {
    cmd.steps = server.arg("steps").toInt();
  } else {
    cmd.steps = 0;
  }
  if (server.hasArg("dir")) {
    cmd.dir = server.arg("dir") == "1";
  } else {
    cmd.dir = true;
  }

  if (stepsQueue != NULL) {
    if (xQueueSend(stepsQueue, &cmd, pdMS_TO_TICKS(100)) == pdTRUE) {
      server.send(200, "text/plain", "Command queued");
    } else {
      server.send(500, "text/plain", "Queue full");
    }
  } else {
    server.send(500, "text/plain", "Queue not available");
  }
}

void WiFiTask(void *pv) {
  (void)pv;
  // set a fixed IP (use addresses valid for your network)
  IPAddress localIP(192,168,1,50);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  if (!WiFi.config(localIP, gateway, subnet)) {
    Serial.println("Static IP config failed");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Connecting to WiFi...");
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    vTaskDelay(pdMS_TO_TICKS(500));
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed");
    // continue to start server on AP-less mode (may fail)
  }

  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.begin();
  Serial.printf("HTTP server started on port %d\n", WIFI_PORT);

  for (;;) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  vTaskDelete(NULL);
}
*/

void ExecuteMove(void *pv) 
{
  (void)pv;

  const uint8_t m1Pins[4] = { 13, 12, 14, 27 };
  const uint8_t m2Pins[4] = { 15, 2, 4, 5};

  MoveCommand cmd;
  
  uint8_t step_sequence[8][4] = {
      {1,0,0,0},
      {1,1,0,0},
      {0,1,0,0},
      {0,1,1,0},
      {0,0,1,0},
      {0,0,1,1},
      {0,0,0,1},
      {1,0,0,1}};

  for (int i = 0; i < 4; i++) {
    pinMode(m1Pins[i], OUTPUT);
    pinMode(m2Pins[i], OUTPUT);
  }

  while (1)
  {
    if (moveQueue != NULL && xQueueReceive(moveQueue, &cmd, portMAX_DELAY) == pdTRUE) {
      // Execute the move command
      for (uint32_t s = 0; s < cmd.distance; s++)
      {
        switch(cmd.dir)
        {
          case 0:
            // Rotate forward
            for(int i = 0; i < 8; i++) 
            {
              for(int j = 0; j < 4; j++) 
              {
                digitalWrite(m1Pins[j], step_sequence[i][j]);
                digitalWrite(m2Pins[j], step_sequence[i][j]);
              }
              vTaskDelay(pdMS_TO_TICKS(1)); // Delay between steps (adjust as needed)
            }
            break;
          case 1:
            
            // Rotate backward
            for(int i = 7; i >= 0; i--) 
            {
              for(int j = 0; j < 4; j++) 
              {
                digitalWrite(m1Pins[j], step_sequence[i][j]);
                digitalWrite(m2Pins[j], step_sequence[i][j]);
              }
              vTaskDelay(pdMS_TO_TICKS(1)); // Delay between steps (adjust as needed)
            }
            break;
          case 2:
            // Right turn (only move one stepper)
            for(int i = 0; i < 8; i++) 
            {
              for(int j = 0; j < 4; j++) 
              {
                digitalWrite(m1Pins[j], step_sequence[i][j]);
                digitalWrite(m2Pins[j], step_sequence[7-i][j]);
              }
              vTaskDelay(pdMS_TO_TICKS(1)); // Delay between steps (adjust as needed)
            }
            break;
          case 3:
            // Left turn (only move one stepper)
            for(int i = 7; i >= 0; i--) 
            {
              for(int j = 0; j < 4; j++) 
              {
                digitalWrite(m1Pins[j], step_sequence[i][j]);
                digitalWrite(m2Pins[j], step_sequence[7-i][j]);
              }
              vTaskDelay(pdMS_TO_TICKS(1)); // Delay between steps (adjust as needed)
            }
            break;
        }
      }
    }
  }
}

uint32_t get_random_value(uint32_t min, uint32_t max) {
  return esp_random() % (max - min + 1) + min;
}

void ExecuteMove_n20(void *pv) 
{
  (void)pv;
  MoveCommand cmd;

  while (1)
  {
    if (moveQueue != NULL && xQueueReceive(moveQueue, &cmd, portMAX_DELAY) == pdTRUE) {
      // Execute the move command
      Serial.printf("Executing command: Direction: %d, Distance: %d\n", cmd.dir, cmd.distance);
      switch (cmd.dir)
      {
      case 0: // Forward
        digitalWrite(5, HIGH);
        digitalWrite(18, LOW);
        digitalWrite(19, HIGH);
        digitalWrite(21, LOW);
        vTaskDelay(pdMS_TO_TICKS(cmd.distance)); // Assuming each step takes 10ms, adjust as needed
        digitalWrite(5, LOW);
        digitalWrite(18, LOW);
        digitalWrite(19, LOW);
        digitalWrite(21, LOW);
        break;
      case 1: // Backward
        digitalWrite(5, LOW);
        digitalWrite(18, HIGH);
        digitalWrite(19, LOW);
        digitalWrite(21, HIGH);

        vTaskDelay(pdMS_TO_TICKS(cmd.distance)); // Assuming each step takes 10ms, adjust as needed
        digitalWrite(5, LOW);
        digitalWrite(18, LOW);
        digitalWrite(19, LOW);
        digitalWrite(21, LOW);
        break;

      case 2: // Right turn
        digitalWrite(5, HIGH);
        digitalWrite(18, LOW);
        digitalWrite(19, LOW);
        digitalWrite(21, HIGH);
        vTaskDelay(pdMS_TO_TICKS(cmd.distance)); // Assuming each step takes 10ms, adjust as needed
        digitalWrite(5, LOW);
        digitalWrite(18, LOW);
        digitalWrite(19, LOW);
        digitalWrite(21, LOW);
        break;
      case 3: // Left turn
        digitalWrite(5, LOW);
        digitalWrite(18, HIGH);
        digitalWrite(19, HIGH);
        digitalWrite(21, LOW);
        vTaskDelay(pdMS_TO_TICKS(cmd.distance)); // Assuming each step takes 10ms, adjust as needed
        digitalWrite(5, LOW);
        digitalWrite(18, LOW);
        digitalWrite(19, LOW);
        digitalWrite(21, LOW);
        break;
      
      default:
        break;
      }


    }
  }
}
void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting Setup");

  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(21, OUTPUT);

  moveQueue = xQueueCreate(4, sizeof(MoveCommand));

  // Create the idle FreeRTOS task for future stepper control, pinned to core 1
  //xTaskCreatePinnedToCore(MoveSteppersTask,"Move Steppers", 2048, NULL, 2, NULL, 1);
  //xTaskCreatePinnedToCore(MoveStepper2_random, "Move Steppers Random", 2048, NULL, 1, NULL, 1);
  //xTaskCreatePinnedToCore(MoveStepper1_random, "Move Stepper 1 Random", 2048, NULL, 1, NULL, 1);
  //xTaskCreatePinnedToCore(ExecuteMove, "Execute Move", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(ExecuteMove_n20, "Execute Move", 2048, NULL, 1, NULL, 1);
  //xTaskCreatePinnedToCore(CheckSensors, "Check Sensors", 2048, NULL, 1, NULL, 1);

  // Create the WiFi and web server task, pinned to core 0
  // xTaskCreatePinnedToCore(WiFiTask, "WiFi Task", 8192, NULL, 1, NULL, 0);

    
  
}

void sendCommand(uint8_t dir, uint32_t distance) {
  MoveCommand cmd;
  cmd.dir = dir;
  cmd.distance = distance;

  if (moveQueue != NULL) {
    if (xQueueSend(moveQueue, &cmd, pdMS_TO_TICKS(100)) == pdTRUE) {
      //Serial.println("Command queued");
      //Serial.printf("Direction: %d, Distance: %d\n", cmd.dir, cmd.distance);
    } else {
      //Serial.println("Queue full");
    }
  }
}


void loop() {

    
    MoveCommand cmd;
    cmd.dir = 0; // Forward
    cmd.distance = 100; // 500 steps

    while(1)
    {
      if(digitalRead(23) == LOW || (20 == get_random_value(0, 50))) // Assuming LOW means the button is pressed
      {
        
        sendCommand(1, 500); // Move backward 500 steps
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay before sending the next command
        sendCommand(get_random_value(2, 2), get_random_value(200, 1800)); // Change direction to right turn and set random distance
        vTaskDelay(pdMS_TO_TICKS(500)); // Debounce delay
      }
      else
      {
        cmd.dir = 0; // Forward
        cmd.distance = 100; // Fixed distance of 100
      }
      sendCommand(cmd.dir, cmd.distance);
      vTaskDelay(pdMS_TO_TICKS(100)); // Delay before sending the next command
    }

}