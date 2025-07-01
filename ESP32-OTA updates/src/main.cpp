#include "ota.h"

#define RED_LED 2

// WiFi credentials (now defined in main.cpp)
const char *ssid = "ESP32_AP";
const char *pass = "541754175417";

void blinkingLEDTask(void *pvParameters);

void setup()
{
  Serial.begin(115200);
  xTaskCreate(blinkingLEDTask, "Blinking LED Task", 1024, NULL, 1, NULL);
  xTaskCreate(initOTATask, "OTATask", 4096, NULL, 1, NULL);            // Start OTA task
  xTaskCreate(startOTAserver, "OTA Server Task", 4096, NULL, 1, NULL); // Start OTA server task
}

void loop()
{
  // The main loop is intentionally left empty as tasks handle execution.
  // This allows the FreeRTOS scheduler to manage task switching.
}
void blinkingLEDTask(void *pvParameters)
{
  pinMode(RED_LED, OUTPUT);
  while (true)
  {
    digitalWrite(RED_LED, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(RED_LED, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}