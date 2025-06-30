#ifndef OTA_H
#define OTA_H
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>

extern const char* ssid;
extern const char* password;
extern WebServer server;

void initOTA();
void startOTAserver();

#endif // OTA_H