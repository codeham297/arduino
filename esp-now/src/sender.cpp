#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define ESPNOW_WIFI_CHANNEL 6

uint8_t receiverMac[] = {0x44, 0x1D, 0x64, 0xF3, 0xF5, 0xF8};
uint32_t msg_count = 0;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
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

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = ESPNOW_WIFI_CHANNEL;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(receiverMac))
  {
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
      while (true)
        ;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP-NOW Sender");
  Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

  initWiFi();
  initEspNow();
}

void loop()
{
  char msg[32];
  snprintf(msg, sizeof(msg), "Data #%lu", msg_count++);
  Serial.printf("Sending: %s\n", msg);

  esp_now_send(receiverMac, (uint8_t *)msg, sizeof(msg));
  delay(5000);
}