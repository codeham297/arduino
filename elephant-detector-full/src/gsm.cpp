#include "gsm.h"

// Define GSM pins
#define SIM800_TXD 16
#define SIM800_RXD 17

#define ADMIN_NUMBER "+255759469432"

#define SerialMon Serial
#define SerialAT Serial1

TinyGsm modem(SerialAT);

void initGSM()
{
    SerialMon.begin(115200);
    delay(1000);

    SerialMon.println("Wait ...");
    SerialAT.begin(9600, SERIAL_8N1, SIM800_TXD, SIM800_RXD);
    delay(3000);

    SerialMon.println("Initializing modem ...");
    modem.restart();

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected())
    {
        SerialMon.println("Network connected");
    }

    SerialAT.print("AT+CNMI=2,2,0,0,0\r");
    delay(1000);
}

void sendMessage(const char *message)
{
    int retryCount = 0;
    const int maxRetries = 5;

    while (retryCount < maxRetries)
    {
        SerialMon.print("Attempting to send SMS (Try ");
        SerialMon.print(retryCount + 1);
        SerialMon.println(" of 5)...");

        bool sent = modem.sendSMS(ADMIN_NUMBER, message);

        if (sent)
        {
            SerialMon.println("SMS sent successfully!");
            return;
            retryCount = 0; // Reset retry count on success
        }
        else
        {
            SerialMon.println("SMS failed to send.");
            retryCount++;

            SerialMon.println("Checking network before retry...");
            String networkStatus = checkNetwork();

            if (networkStatus.indexOf("Not Connected") >= 0)
            {
                SerialMon.println("Reinitializing GSM module...");
                initGSM();
            }

            delay(5000); // Wait before retrying
        }
    }

    SerialMon.println("Failed to send SMS after 5 attempts.");
}

// Function to check network status

// Usage
//  String networkStatus = checkNetwork();
//  SerialMon.println("Current Network Status: " + networkStatus);

String checkNetwork()
{
    SerialMon.print("Checking network status...");
    SerialAT.print("AT+CREG?\r");
    delay(2000);

    if (SerialAT.available())
    {
        String networkStatus = SerialAT.readString();
        SerialMon.println(" Network Status: " + networkStatus);

        if (networkStatus.indexOf("+CREG: 0,1") > 0 || networkStatus.indexOf("+CREG: 0,5") > 0)
        {
            return "Connected";
        }
        else
        {
            return "Not Connected, trying to connect...";
            initGSM(); // Reinitialize GSM to try reconnecting
        }
    }
    else
    {
        SerialMon.println(" No response from modem.");
        return "Unknown";
    }
}

// Function to check signal strength
// Usage
// String signalStrength = getSignalStrength();
// SerialMon.println("Current Signal Strength: " + signalStrength);

String getSignalStrength()
{
    SerialMon.print("Checking signal strength...");
    SerialAT.print("AT+CSQ\r");
    delay(1000);

    if (SerialAT.available())
    {
        String response = SerialAT.readString();
        SerialMon.println(" Signal Strength Response: " + response);
        return response;
    }
    else
    {
        return "No response from modem.";
    }
}