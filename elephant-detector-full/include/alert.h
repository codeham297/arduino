#ifndef ALERT_H
#define ALERT_H

#include "gsm.h"
#include "lcd.h"
#include "pins_and_globals.h"
#include "dfplayer.h"

// Function prototypes
void triggerAlert(const char *message); // Send an alert message via GSM
void activateBuzzer();                  // Trigger the buzzer for a warning signal
void initializeAlertSystem();           // Initialize alert system components

#endif