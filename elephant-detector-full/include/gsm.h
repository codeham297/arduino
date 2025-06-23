#ifndef GSM_H
#define GSM_H

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include "pins_and_globals.h"
#include "esp_task_wdt.h"
#include "lcd.h"
#include "blynk.h"

void initGSM();
void sendMessage(const char *message);
String checkNetwork();
String getSignalStrength();

#endif