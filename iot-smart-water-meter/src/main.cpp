#include <Arduino.h>
#include "globals.h"
#include "my_wifi.h"
#include "blynk.h"
#include "ota.h"
#include "waterflow.h"
#include "rfid.h"

volatile unsigned long pulseCount = 0;
float totalWaterUsed = 0.0;
String current_user_uid = "00000000";

void IRAM_ATTR countPulse()
{
  pulseCount++;
}

// // Function declarations
// void waterFlowTask(void *pvParameters);
// void rfidScanner(void *pvParameters);
// UserData getUserData(String scannedUID); // Returns user data for a recognized card
// void IRAM_ATTR countPulse();
// void resetRFID(); // Resets the RFID reader

void setup()
{
  Serial.begin(115200);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  pinMode(SOLENOID_VALVE, OUTPUT);
  digitalWrite(SOLENOID_VALVE, LOW); // Ensure solenoid valve is initially closed

  // Initialize WiFi first
  if (!initWiFi())
  {
    Serial.println("Failed to connect to WiFi");
  }
  // Create FreeRTOS tasks with 4096 bytes of stack for each task.
  xTaskCreate(rfidTask, "RFID", 4096, NULL, 2, NULL);
  xTaskCreate(waterFlowTask, "WaterFlow", 4096, NULL, 2, NULL);
  xTaskCreate(BlynkManagerTask, "BlynkTask", 4096, NULL, 3, NULL);
  xTaskCreate(initOTATask, "OTATask", 4096, NULL, 1, NULL);          // Initialize OTA task then delete it
  xTaskCreate(startOTAserver, "OTAServerTask", 4096, NULL, 1, NULL); // Start OTA server task
}

void loop()
{
  maintainWiFi(); // Optional periodic WiFi maintenance
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // Not needed, as tasks handle execution.
}