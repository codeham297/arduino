#ifndef BLYNK_H
#define BLYNK_H

#include <Arduino.h>
#include "globals.h"
#include <HTTPClient.h>

void initBlynk();
void checkBlynkConnection();
void sendData(String data);
void BlynkManagerTask(void *pvParameters);
void sendMeterData(String user_name, float current_usage, String database, float balance);
String formatUserData();

#endif