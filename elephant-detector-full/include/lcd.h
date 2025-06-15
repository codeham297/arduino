#ifndef LCD_H
#define LCD_H

#include "pins.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "pins.h" // Centralized pin definitions

void initLCD();
void displayMessage(const char *message);

#endif