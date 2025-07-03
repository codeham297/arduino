#include "blynk.h"
#include "my_wifi.h"

// Blynk configuration
#define BLYNK_TEMPLATE_ID "TMPL2ARmqqC39"
#define BLYNK_TEMPLATE_NAME "SMART WATER METER"
#define BLYNK_AUTH_TOKEN "yGrqJCuDshNBK5Bfr1LE3TIzXJlM7F79"
#define BLYNK_PRINT Serial

// // hamisisalum297@gmail.com
// #define BLYNK_TEMPLATE_ID "TMPL2fA5FeDds"
// #define BLYNK_TEMPLATE_NAME "METER"
// #define BLYNK_AUTH_TOKEN "-TSP4WITnmcJv5D1xOlsfmGiNeFmN9wX"
#include <BlynkSimpleEsp32.h>

BlynkTimer timer;

String oldmessage = "";
String message = "SYSTEM ONLINE";

String old_user_name = "";
float old_current_usage = 0.0;
float old_total_usage = 0.0;
float old_balance = 0.0;

String old_message = "";

BLYNK_WRITE(V0)
{
    int value = param.asInt();
    Blynk.virtualWrite(V1, value);
}

BLYNK_CONNECTED()
{
    Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
    Blynk.setProperty(V3, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
    Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
    digitalWrite(BLUE_LED, HIGH); // Turn on blue LED when Blynk connects
}

void initBlynk()
{
    pinMode(BLUE_LED, OUTPUT);   // Initialize BLUE_LED as output
    digitalWrite(BLUE_LED, LOW); // Start with LED off

    Blynk.config(BLYNK_AUTH_TOKEN);
    if (!Blynk.connect())
    {
        Serial.println("[Blynk] Initial connection failed");
        digitalWrite(BLUE_LED, LOW); // Ensure LED is off if connection fails
    }
}

void sendMeterData(String user_name, float current_usage, float total_usage, float Balance)
{
    if (user_name != old_user_name)
    {
        Blynk.virtualWrite(V1, user_name);
        old_user_name = user_name;
    }
    if (current_usage != old_current_usage)
    {
        Blynk.virtualWrite(V2, current_usage);
        old_current_usage = current_usage;
    }
    if (total_usage != old_total_usage)
    {
        Blynk.virtualWrite(V3, total_usage);
        old_total_usage = total_usage;
    }
    if (Balance != old_balance)
    {
        Blynk.virtualWrite(V4, Balance);
        old_balance = Balance;
    }
}

void sendData(String data)
{
    if (data != old_message)
    {
        Blynk.virtualWrite(V0, data);
        old_message = data;
    }
}

void BlynkManagerTask(void *pvParameters)
{
    initBlynk();

    while (true)
    {
        if (isWiFiConnected())
        {
            if (Blynk.connected())
            {
                Blynk.run();
                timer.run();
                digitalWrite(BLUE_LED, HIGH); // LED on when connected
            }
            else
            {
                digitalWrite(BLUE_LED, LOW); // LED off when disconnected
                Blynk.connect();             // Try to reconnect
            }
        }
        else
        {
            digitalWrite(BLUE_LED, LOW); // LED off when WiFi is disconnected
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}