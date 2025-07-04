#ifndef BLYNK_H
#define BLYNK_H

#include <Arduino.h>
#include "globals.h"

// Function declarations
void initBlynk();
void sendData(String data);
void BlynkManagerTask(void *pvParameters);
void sendMeterData(String user_name, float current_usage, float total_usage, float Balance);
String usersArrayToString();
void sendUserDatabase();

#endif