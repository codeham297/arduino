#ifndef VIBRATION_H
#define VIBRATION_H

#include <pins_and_globals.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

void initVibrationSensor();
void classifyVibration(float change);
void initVibrationTask(void *pvParameters);
extern float vibration;
#endif