#ifndef RFID_H
#define RFID_H

#include "globals.h"

// Task interface
void rfidTask(void *pvParameters);

// Helper functions
void rfidInit();
void rfidRun();

#endif