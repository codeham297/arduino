// ota.h
#ifndef OTA_H
#define OTA_H

#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

// Declare the task function
void initOTATask(void *parameters);

void startOTAserver(void *parameters); // Handle client requests

#endif