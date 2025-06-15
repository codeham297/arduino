#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define ESPNOW_WIFI_CHANNEL 6

uint8_t allowedSender[] = {0x68, 0x25, 0xDD, 0x33, 0xAA, 0xC4};

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len)
{
    if (memcmp(mac, allowedSender, 6) != 0)
    {
        Serial.println("Ignored unknown sender");
        return;
    }

    Serial.printf("Received from %02X:%02X:%02X:%02X:%02X:%02X → ",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.printf("Message: %.*s\n", len, data);
}

void initWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
}

void initEspNow()
{
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        while (true)
            ;
    }

    esp_now_register_recv_cb(onDataRecv);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, allowedSender, 6);
    peerInfo.channel = ESPNOW_WIFI_CHANNEL;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;

    if (!esp_now_is_peer_exist(allowedSender))
    {
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            Serial.println("Failed to register sender peer");
            while (true)
                ;
        }
    }
}

void setup()
{
    Serial.begin(115120);
    Serial.println("ESP-NOW Receiver");
    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

    initWiFi();
    initEspNow();
}

void loop()
{
    delay(1000);
}