#include <pins.h>
#include <dfplayer.h>
#include <lcd.h>
#include <gsm.h>
#include <espcam.h>

#include <Arduino.h>
#include "gsm.h"

void setup()
{
  Serial.begin(115200);
  initGSM();                             // Initialize GSM module
  sendMessage("GSM Module is working!"); // Send initial message
  initDFPlayer();                        // Initialize DFPlayer Mini
  initLCD();
  displayMessage("SYSTEM INITIALIZED"); // Display message
}

void loop()
{
}