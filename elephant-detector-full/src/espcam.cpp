#include "espcam.h"
#include "pins.h"
#include <Arduino.h>

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);   // Initialize built-in LED pin
    digitalWrite(LED_BUILTIN, LOW); // Turn off the LED initially
}

void loop()
{
}