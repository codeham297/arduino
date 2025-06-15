#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "espnow.h"

#define ESPNOW_CHANNEL 6

// Update this with the actual receiver MAC address (for sender nodes)
uint8_t peerMac[] = {0x44, 0x1D, 0x64, 0xF3, 0xF5, 0xF8};

void onESPNowSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("ESP-NOW send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void onESPNowReceive(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    Serial.println("ESP-NOW message received.");
    if (data != nullptr && len > 0 && data[0] == 1)
    {
        Serial.println("Trigger signal received. Executing action...");
        // Here you can add the action to be executed when the trigger is received
        // For example, you can call a function to capture an image or send an alert
        // captureImage(); // Example function call
    }
    else
    {
        Serial.println("Received data is invalid or not a trigger signal.");
    }
}

void initWiFiForESPNOW()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
}

void initESPNow()
{
    initWiFiForESPNOW();

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        while (true)
            ;
    }

    esp_now_register_send_cb(onESPNowSent);
    esp_now_register_recv_cb(onESPNowReceive);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerMac, 6);
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;

    if (!esp_now_is_peer_exist(peerMac))
    {
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            Serial.println("Failed to register ESP-NOW peer");
            while (true)
                ;
        }
    }
}

void sendESPNowTrigger()
{
    uint8_t message[1] = {1}; // Trigger signal
    if (esp_now_send(peerMac, message, sizeof(message)) != ESP_OK)
    {
        Serial.println("ESP-NOW send failed");
    }
    else
    {
        Serial.println("ESP-NOW trigger sent.");
    }
}