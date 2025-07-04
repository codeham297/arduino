#include <WiFi.h>
#include <time.h>

// WiFi credentials
const char *ssid = "ESP32_AP";
const char *password = "541754175417";

// NTP settings
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3 * 3600; // GMT offset in seconds (0 for UTC)
const int daylightOffset_sec = 0;    // Daylight offset in seconds (3600 for CEST)

void setup()
{
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

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
}

void loop()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  // Print time on Serial Monitor
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  delay(1000); // Update every second
}