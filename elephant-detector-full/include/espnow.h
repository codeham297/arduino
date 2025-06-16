#ifndef ESPNOW_H
#define ESPNOW_H

#include <WiFi.h>
#include <esp_now.h> // 🔧 Add this
#include <stdint.h>

void initESPNow();
void sendESPNowTrigger();
void onESPNowReceive(const uint8_t *mac_addr, const uint8_t *data, int len);
void onESPNowSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void sendESPNowMessage(String message);
#endif                          // ESPNOW_H
extern String received_message; // Global variable to store received message
