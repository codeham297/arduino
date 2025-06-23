#ifndef BLYNK_H
#define BLYNK_H

#include <Arduino.h>
#include "lcd.h"
#include "pins_and_globals.h"
#include <HTTPClient.h>

void initBlynk();
void checkBlynkConnection();
void sendData(String message);
void BlynkManagerTask(void *pvParameters);

#endif