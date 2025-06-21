#include "espnow.h"
// #include <WiFi.h>
// #include "ESPAsyncWebServer.h"

int ESPNOW_CHANNEL;
constexpr char ESPNOW_SSID[] = "ESPNOW";
constexpr char ESPNOW_PASS[] = "00000000";
constexpr char WIFI_SSID[] = "ESP32_AP";
constexpr char WIFI_PASS[] = "00000000";

// List of peer MAC addresses
esp_now_peer_info_t peerInfo;
#ifdef ESPCAM_H
uint8_t broadcastAddress[] = {0x44, 0x25, 0xDD, 0x33, 0xAA, 0xC4};
#else
uint8_t broadcastAddress[] = {0x44, 0x1D, 0x64, 0xF3, 0xF5, 0xF8};
#endif

String received_message = ""; // Global variable to store received message

void onESPNowSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);

    Serial.println(
        String("ESPNOW SEND TO: ") + macStr +
        " IS " + (status == ESP_NOW_SEND_SUCCESS ? "SUCCESSFUL" : "FAILED"));
}

void onESPNowReceive(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.printf("ESPNOW RECEIVED FROM: %s | MESSAGE: %s\n", macStr, (char *)data);
}

void initWiFiForESPNOW()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Wi-Fi not connected. Attempting connection...");
#ifdef ESPCAM_H
        WiFi.mode(WIFI_STA);
        WiFi.begin(ESPNOW_SSID, ESPNOW_PASS);
        ESPNOW_CHANNEL = getWiFiChannel(ESPNOW_SSID);
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_promiscuous(false);
#else
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(ESPNOW_SSID, ESPNOW_PASS);
        WiFi.begin(WIFI_SSID, WIFI_PASS);

#endif
    }
}

void initESPNow()
{
    delay(1000);
    initWiFiForESPNOW();
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ERROR INITIALIZING ESPNOW");
        return;
    }
    Serial.println(current_environment);
#ifdef ESPCAM_H
    esp_now_register_send_cb(onESPNowSent);
#else
    esp_now_register_recv_cb(esp_now_recv_cb_t(onESPNowReceive));
#endif

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }
}

void sendESPNowMessage(String message)
{
    uint8_t data[message.length() + 1];
    message.getBytes(data, message.length() + 1);

    esp_err_t result = esp_now_send(broadcastAddress, data, sizeof(data));

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             broadcastAddress[0], broadcastAddress[1], broadcastAddress[2],
             broadcastAddress[3], broadcastAddress[4], broadcastAddress[5]);

    Serial.println(String("To ") + macStr + " | " +
                   (result == ESP_OK ? "✅ Sent" : "❌ Failed") +
                   " | Message: " + message);
}