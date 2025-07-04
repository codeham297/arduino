#ifndef MY_WIFI_H
#define MY_WIFI_H

#include <WiFi.h>

// WiFi credentials
extern const char *wifiSSID;
extern const char *wifiPassword;

// WiFi control functions
bool initWiFi();
bool isWiFiConnected();
void maintainWiFi();

#endif