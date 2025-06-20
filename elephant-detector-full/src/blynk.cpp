#include "blynk.h"

const char *ssid = "ESP32_AP";
const char *pass = "00000000";
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

void checkBlynkConnection()
{
    static bool connected_message_sent = false;
    static bool disconnected_message_sent = false;

    if (Blynk.connected())
    {
        if (!connected_message_sent)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            connected_message_sent = true;
            disconnected_message_sent = false;
            displayMessage("BLYNK WAS ALREADY CONNECTED");
        }
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
        if (!disconnected_message_sent)
        {
            // Debug console
            if (!Blynk.connected())
            {
                Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
                // You can also specify server:
                // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
                // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

                // Setup a function to be called every second
                timer.setInterval(1000L, myTimerEvent);

                Blynk.run();
                timer.run();
                sendData("BLYNK IS CONNECTED SUCCESSFULLY");
                displayMessage("BLYNK IS CONNECTED SUCCESSFULLY");
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
