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
    SerialMon.println("Sending SMS...");

    bool sent = modem.sendSMS(ADMIN_NUMBER, message);

    if (sent)
    {
        SerialMon.println("SMS sent successfully!");
    }
    else
    {
        SerialMon.println("SMS failed to send.");
    }
}