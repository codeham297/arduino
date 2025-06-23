#ifndef LCD_H
#define LCD_H

#include "pins_and_globals.h" // Centralized pin definitions
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

void initLCD();
void displayMessage(const char *message);
void clearLCD();

#endif