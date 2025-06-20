#include "gsm.h"
#include "dfplayer.h"

// Define GSM pins
#define SIM800_TXD 16
#define SIM800_RXD 17
#define ADMIN_NUMBER "+255759469432"

#define SerialMon Serial
#define SerialAT Serial1

TinyGsm modem(SerialAT);

// Optional: Configure TinyGSM timeout
#define MODEM_TIMEOUT_MS 10000

void initGSM()
{
    delay(1000);
    SerialMon.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    SerialMon.println("Wait ...");
    SerialAT.begin(9600, SERIAL_8N1, SIM800_TXD, SIM800_RXD);
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    SerialMon.println("Initializing modem ...");
    SerialAT.setTimeout(MODEM_TIMEOUT_MS);
    modem.restart(); // Blocking, but OK with delay after

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork(30)) // Timeout after 30s
    {
        SerialMon.println(" fail");
        displayMessage("NETWORK FAILURE");
        playTrack(3);
        checkBlynkConnection();
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected())
    {
        SerialMon.println("Network connected");
        displayMessage("CONNECTED");
        sendMessage("CONNECTED");
        // sendData("CONNECTED");
    }

    SerialAT.print("AT+CNMI=2,2,0,0,0\r");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void sendMessage(const char *message)
{
    // sendData(message);
    bool sent = modem.sendSMS(ADMIN_NUMBER, message);

    if (sent)
    {
        displayMessage("SMS SENT");
        return;
    }

    // String networkStatus = checkNetwork();
    // if (networkStatus.indexOf("Not Connected") >= 0)
    // {
    //     SerialMon.println("Skipping GSM reinitialization to avoid watchdog conflict.");
    //     // Optionally: set a flag here for main task to reinit
    // }

    esp_task_wdt_reset();                  // Feed the watchdog
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait before retrying
    displayMessage("NOT SENT");

    SerialMon.println("Failed to send SMS after retries.");
}

String checkNetwork()
{
    SerialMon.println("Checking network status...");
    SerialAT.print("AT+CREG?\r");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    if (SerialAT.available())
    {
        String networkStatus = SerialAT.readString();
        SerialMon.println(" Network Status: " + networkStatus);

        if (networkStatus.indexOf("+CREG: 0,1") > 0 || networkStatus.indexOf("+CREG: 0,5") > 0)
        {
            return "Connected";
        }
        return "Not Connected, trying to connect...";
    }
    else
    {
        SerialMon.println("No response from modem.");
        return "Unknown";
    }
}

String getSignalStrength()
{
    SerialMon.println("Checking signal strength...");
    SerialAT.print("AT+CSQ\r");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    if (SerialAT.available())
    {
        String response = SerialAT.readString();
        SerialMon.println(" Signal Strength Response: " + response);
        return response;
    }
    return "No response from modem.";
}