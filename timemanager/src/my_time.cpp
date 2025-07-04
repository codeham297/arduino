#include "my_time.h"

// NTP settings
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3 * 3600; // GMT offset in seconds (0 for UTC)
const int daylightOffset_sec = 0;    // Daylight offset in seconds (3600 for CEST)

void initTime(void *pvParameters)
{
    // Configure and initialize NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    // Wait for time to be set
    Serial.println("Waiting for NTP time sync...");
    time_t now;
    struct tm timeinfo;
    while (time(&now) < 8 * 3600 * 2)
    { // Wait until time is set (after 1970)
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nTime synchronized");
    vTaskDelete(NULL); // Delete this task after initialization
}

void runTime(void *pvParameters)
{
    while (true)
    {
        String currentTime = getTimeInfo();
        Serial.println(currentTime);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Use FreeRTOS delay
    }
}

String getTimeInfo()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return "Time not available";
    }

    char timeString[64];
    strftime(timeString, sizeof(timeString), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    return String(timeString);
}