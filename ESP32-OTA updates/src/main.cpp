#include "ota.h"
#define RED_LED 2 // GPIO pin for RED LED

void blinkingLEDTask(void *pvParameters);

void setup()
{
  initOTA(); // Initialize OTA server
  xTaskCreate(blinkingLEDTask, "Blinking LED Task", 1024, NULL, 1, NULL);
}

void loop()
{
  startOTAserver(); // Handle OTA updates
}

void blinkingLEDTask(void *pvParameters)
{
  pinMode(RED_LED, OUTPUT);
  while (true)
  {
    Serial.println("RED LED ON");
    digitalWrite(RED_LED, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Keep RED LED ON for 1 second
    Serial.println("RED LED OFF");
    digitalWrite(RED_LED, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Keep RED LED OFF for 1 second
  }
}