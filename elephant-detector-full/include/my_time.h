#ifndef MY_TIME_H
#define MY_TIME_H

#include <time.h>
#include <Arduino.h>

void initTime(void *pvParameters);
void runTime(void *pvParameters);
String getTimeInfo();

#endif // MY_TIME_H