#ifndef GSM_H
#define GSM_H

#define TINY_GSM_MODEM_SIM800
#include <Arduino.h>
#include <TinyGsmClient.h>
#include "pins.h"

void initGSM();
void sendMessage(const char *message);

#endif