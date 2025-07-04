#include "my_time.h"
#include "my_wifi.h"

void setup()
{
  Serial.begin(115200);
  initWiFi(); // Initialize WiFi first
  xTaskCreate(initTime, "InitTime", 4096, NULL, 1, NULL);
  xTaskCreate(runTime, "RunTime", 4096, NULL, 1, NULL);
}
void loop()
{
  // The loop is intentionally left empty as the time tasks run independently
  vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay to avoid busy-waiting
}