#include <Arduino.h> // Needed for FreeRTOS

#include "i2c_init.h"
#include "pins_and_globals.h"
#include "dfplayer.h"
#include "lcd.h"
#include "gsm.h"
#include "alert.h"
#include "blynk.h"
#include "vibration.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
int environment = 1;

/* RTOS Task Handle */
TaskHandle_t vibrationTaskHandle;

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
  delay(1000);
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
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Initializing system...");
  xTaskCreate(initLCDTask, "LCDTask", 4096, NULL, 1, NULL);
  initI2CBuses(); // Initialize I2C buses for LCD and vibration sensor
  xTaskCreate(BlynkManagerTask, "BlynkTask", 4096, NULL, 1, NULL);
  // xTaskCreate(initGSMTask, "GSMTask", 8192, NULL, 1, NULL);
  xTaskCreate(initDFPlayerTask, "DFPlayerTask", 2048, NULL, 1, NULL);
  initGSM();
  xTaskCreate(initVibrationTask, "VibrationTask", 4096, NULL, 1, &vibrationTaskHandle);
  xTaskCreate(initAlertTask, "AlertTask", 2048, NULL, 1, NULL);

  Serial.println("System setup complete. Ready to operate.");
  playTrack(5);
  // xTaskCreate(initESPNowTask, "ESPNowTask", 4096, NULL, 1, NULL);
}

void loop()
{
  String new_message = getMessageFromSlave(I2C_DEV_ADDR, 32);
  new_message.trim();

  // Only update if it's a new, non-empty message
  if (new_message.length() > 0 && new_message != received_message)
  {
    received_message = new_message;
  }

  // Process only when both conditions are met
  if (vibration > 1 && received_message.length() > 0)
  {
    displayMessage(received_message.c_str());
    Serial.println(received_message);

    if (received_message == "LION" || received_message == "ELEPHANT")
    {
      triggerAlert(received_message.c_str());
    }

    received_message = ""; // Now safe to clear after processing
  }
}