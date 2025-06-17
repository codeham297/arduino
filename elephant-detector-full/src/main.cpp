#include "pins.h"
#include "dfplayer.h"
#include "lcd.h"
#include "gsm.h"
#include "alert.h"
#include "espnow.h"
#include "camera.h"

#include <Arduino.h> // Needed for FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void initESPNowTask(void *pvParameters)
{
  initESPNow();
  vTaskDelete(NULL);
}

void initGSMTask(void *pvParameters)
{
  initGSM();
  sendMessage("GSM Module is working!");
  vTaskDelete(NULL);
}

void initDFPlayerTask(void *pvParameters)
{
  initDFPlayer();
  vTaskDelete(NULL);
}

void initLCDTask(void *pvParameters)
{
  initLCD();
  vTaskDelete(NULL);
}

void initAlertTask(void *pvParameters)
{
  initializeAlertSystem();
  vTaskDelete(NULL);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing system...");

  // Start initialization tasks in parallel
  xTaskCreate(initESPNowTask, "ESPNowTask", 2048, NULL, 1, NULL);
  xTaskCreate(initGSMTask, "GSMTask", 2048, NULL, 1, NULL);
  xTaskCreate(initDFPlayerTask, "DFPlayerTask", 2048, NULL, 1, NULL);
  xTaskCreate(initLCDTask, "LCDTask", 2048, NULL, 1, NULL);
  xTaskCreate(initAlertTask, "AlertTask", 2048, NULL, 1, NULL);

  Serial.println("System setup complete. Ready to operate.");
}

struct SensorData
{
  bool vibrationDetected;
};

SensorData sensorData;

void loop()
{
  if (received_message.length() > 0)
  {
    sendMessage(received_message.c_str());
  }

  received_message = ""; // Clear the message after processing

  if (digitalRead(VIBRATION_SENSOR) == HIGH)
  {
    sensorData.vibrationDetected = true;
    Serial.println("Vibration detected, checking camera...");
    displayMessage("Vibration detected!");
    sendMessage("Vibration detected! Checking camera...");
  }
  else
  {
    sensorData.vibrationDetected = false;
  }

  delay(1000);
}