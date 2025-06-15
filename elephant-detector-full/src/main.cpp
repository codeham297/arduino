#include "pins.h"
#include "dfplayer.h"
#include "lcd.h"
#include "gsm.h"
#include "alert.h"
#include "espnow.h"
#include "camera.h"

void setup()
{
  Serial.begin(115200);

  // Initialize communication modules
  initGSM();
  sendMessage("GSM Module is working!");
  initDFPlayer();
  initLCD();
  initializeAlertSystem();

  displayMessage("SYSTEM INITIALIZED");

  // Initialize ESP-NOW with receive callback
  initESPNow();

  Serial.println("System setup complete. Ready to operate.");
}

// Optionally in a shared header
struct SensorData
{
  bool vibrationDetected;
};

SensorData sensorData;

void loop()
{
  if (digitalRead(VIBRATION_SENSOR) == HIGH)
  {
    sensorData.vibrationDetected = true;
    sendESPNowTrigger(); // Send ESP-NOW alert
    Serial.println("Vibration detected, checking camera...");
    displayMessage("Vibration detected!");
  }
  else
  {
    sensorData.vibrationDetected = false;
    Serial.println("No vibration detected.");
    displayMessage("No vibration detected.");
  }

  delay(1000);
}