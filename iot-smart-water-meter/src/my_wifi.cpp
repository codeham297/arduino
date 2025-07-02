#include "my_wifi.h"

const char *wifiSSID = "ESP32_AP";
const char *wifiPassword = "541754175417";

bool initWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPassword);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }

    Serial.println("\nWiFi connection failed");
    return false;
}

bool isWiFiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void maintainWiFi()
{
    // Placeholder for any periodic WiFi maintenance
    // Could implement auto-reconnect if needed
}