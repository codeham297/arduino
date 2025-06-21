#include <Arduino.h> // Needed for FreeRTOS
#include "i2c_init.h"
#include "pins.h"
#include "dfplayer.h"
#include "lcd.h"
#include "gsm.h"
#include "alert.h"
#include "espnow.h"
#include "blynk.h"
#include "vibration.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
int current_environment = 1;

/* RTOS Task Handle */
TaskHandle_t vibrationTaskHandle;

void initESPNowTask(void *pv)
{
  initESPNow();
  for (;;)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
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
  xTaskCreate(initESPNowTask, "ESPNowTask", 4096, NULL, 1, NULL);
}

void loop()
{
  checkBlynkConnection();
  // Serial.println("message: " + received_message);
  delay(1000);
  Serial.println("Current env: " + current_environment);
  Serial.println(String("ESPNOW IS ON CHANNEL: ") + WiFi.channel());
  sendESPNowMessage("THIS MESSAGE IS FROM MAIN");

  if (received_message.length() > 0)
  {
    Serial.println(received_message);
    displayMessage(received_message.c_str());
  }

  if (vibration > 1)
  {
    if (received_message.length() > 0)
    {
      // Process the received message
      displayMessage(received_message.c_str()); // Display message on LCD
      Serial.println(received_message);
      // sendData(received_message);

      if (received_message == " Lion" || received_message == " Elephant" || received_message == "Lion" || received_message == "Elephant")
      {
        triggerAlert(received_message.c_str()); // Trigger alert for specific messages
      }
      received_message = ""; // Clear the message after processing
    }
  }
}