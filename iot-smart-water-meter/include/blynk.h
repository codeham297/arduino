#ifndef BLYNK_H
#define BLYNK_H

#include <Arduino.h>
#include "globals.h"
#include <HTTPClient.h>

void initBlynk();
void checkBlynkConnection();
void sendData(String message);
void BlynkManagerTask(void *pvParameters);

#endif