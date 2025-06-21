#include "blynk.h"

const char *ssid = "ESP32_AP";
const char *pass = "00000000";
constexpr char ESPNOW_SSID[] = "ESPNOW";
constexpr char ESPNOW_PASS[] = "00000000";

String oldmessage = "";
String message = "";

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL2Trz-zQTJ"
#define BLYNK_TEMPLATE_NAME "elephant detectot"
#define BLYNK_AUTH_TOKEN "4jEvDDqL5x31mbRyx8v-MY3QQcp-UYmi"

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
void myTimerEvent()
{
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    if (message != oldmessage)
    {
        sendData(message);
    }
}

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
        digitalWrite(LED_BUILTIN, HIGH);
        if (!connected_message_sent)
        {
            displayMessage("BLYNK IS CONNECTED");
            connected_message_sent = true;
            disconnected_message_sent = false;
        }
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
        if (!disconnected_message_sent)
        {
            displayMessage("BLYNK DISCONNECTED. ATTEMPTING RECONNECT...");
            disconnected_message_sent = true;
            connected_message_sent = false;
        }
        if (!WiFi.isConnected())
        {
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAP(ESPNOW_SSID, ESPNOW_PASS); // was not in receiver
            WiFi.begin(ssid, pass);
        }

        Blynk.config(BLYNK_AUTH_TOKEN);
        Blynk.connect(); // try to connect for 5 seconds max

        if (Blynk.connected())
        {
            if (!connected_message_sent)
            {
                digitalWrite(LED_BUILTIN, HIGH);
                displayMessage("BLYNK RECONNECTED");
                sendData("BLYNK IS CONNECTED SUCCESSFULLY");
                connected_message_sent = true;
                disconnected_message_sent = false;
            }
        }
    }
}

void sendData(String message)
{
    if (message != oldmessage && message != "")
    {
        Serial.println("DATA SENT: " + message);
        Blynk.virtualWrite(V0, message);
        oldmessage = message;
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
