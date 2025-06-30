#include "blynk.h"

String oldmessage = "";
String message = "SYSTEM ONLINE"; // Initial message to send
#include <Arduino.h>

// /* Fill-in information from Blynk Device Info here */
// // pwanielectrical@gmail.com
// #define BLYNK_TEMPLATE_ID "TMPL2ARmqqC39"
// #define BLYNK_TEMPLATE_NAME "SMART WATER METER"
// #define BLYNK_AUTH_TOKEN "yGrqJCuDshNBK5Bfr1LE3TIzXJlM7F79"

// hamisisalum297@gmail.com
#define BLYNK_TEMPLATE_ID "TMPL2fA5FeDds"
#define BLYNK_TEMPLATE_NAME "METER"
#define BLYNK_AUTH_TOKEN "-TSP4WITnmcJv5D1xOlsfmGiNeFmN9wX"

char ssid[] = "ESP32_AP";
char pass[] = "541754175417";

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
    // Set incoming value from pin V0 to a variable
    int value = param.asInt();

    // Update state
    Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
    // Change Web Link Button message to "Congratulations!"
    Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
    Blynk.setProperty(V3, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
    Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
// void myTimerEvent()
// {
//     // You can send any value at any time.
//     // Please don't send more that 10 values per second.
//     if (message != oldmessage)
//     {
//         // sendData(message);
//     }
// }

void initBlynk()
{
    checkBlynkConnection();
}

bool connected_message_sent = false;
bool disconnected_message_sent = false;
void checkBlynkConnection()
{
    if (Blynk.connected())
    {
        digitalWrite(BLUE_LED, HIGH);
        if (!connected_message_sent)
        {
            // displayMessage("BLYNK IS CONNECTED");
            connected_message_sent = true;
            disconnected_message_sent = false;
        }
    }
    else
    {
        digitalWrite(BLUE_LED, LOW);
        if (!disconnected_message_sent)
        {
            // displayMessage("BLYNK DISCONNECTED. ATTEMPTING RECONNECT...");
            disconnected_message_sent = true;
            connected_message_sent = false;
        }
        if (!WiFi.isConnected())
        {
            WiFi.mode(WIFI_STA);
            WiFi.disconnect(); // Ensure we are disconnected before reconnecting
            WiFi.begin(ssid, pass);
        }

        Blynk.config(BLYNK_AUTH_TOKEN);
        Blynk.connect(); // try to connect for 5 seconds max

        if (Blynk.connected())
        {
            if (!connected_message_sent)
            {
                digitalWrite(BLUE_LED, HIGH);
                // displayMessage("BLYNK RECONNECTED");
                sendData("SYSTEM ONLINE | Blynk Reconnected");
                connected_message_sent = true;
                disconnected_message_sent = false;
            }
        }
    }
}

String old_user_name = "";
float old_current_usage = 0.0;
String old_database = "";
float old_balance = 0.0;

void sendMeterData(String user_name, float current_usage, String database, float Balance)
{
    if (user_name != old_user_name)
    {
        Blynk.virtualWrite(V1, user_name);
        old_user_name = user_name;
    }
    else if (current_usage != old_current_usage)
    {
        Blynk.virtualWrite(V2, current_usage);
        old_current_usage = current_usage;
    }
    else if (database != old_database)
    {
        Blynk.virtualWrite(V3, database);
        old_database = database;
    }
    else if (Balance != old_balance)
    {
        Blynk.virtualWrite(V4, Balance);
        old_balance = Balance;
    }
}

String old_message = "";
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
    initBlynk(); // Initial connection setup

    while (true)
    {
        Blynk.run();
        timer.run();
        vTaskDelay(10 / portTICK_PERIOD_MS); // Run frequently, but yield
    }
}
