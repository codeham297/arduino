#ifndef RFID_READER_H
#define RFID_READER_H

#include <Arduino.h>
#include "globals.h"

#include <SPI.h>
#include <MFRC522.h>

void rfidScanner(void *pvParameters);
void initRFID();
void rfidManagerTask(void *pvParameters);
#endif