#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial
#define SerialAT Serial1
#define TINY_GSM_DEBUG SerialMon
#define GSM_PIN ""

#define ADMIN_NUMBER "+255759469432"

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

// ESP32 and SIM800L pins
#define MODEM_TX 16
#define MODEM_RX 17

unsigned long lastCheckTime = 0; // Timer for checking network & signal

// Function to check network status
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
      return "Not Connected";
    }
  }
  else
  {
    SerialMon.println(" No response from modem.");
    return "Unknown";
  }
}

// Function to check signal strength
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

void setup()
{
  SerialMon.begin(115200);
  delay(1000);

  SerialMon.println("Wait ...");
  SerialAT.begin(9600, SERIAL_8N1, MODEM_TX, MODEM_RX);
  delay(3000);
  SerialMon.println("Initializing modem...");
  modem.restart();

  if (GSM_PIN && modem.getSimStatus() != 3)
  {
    modem.simUnlock(GSM_PIN);
  }

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected())
  {
    SerialMon.println("Network connected");
  }

  SerialAT.print("AT+CNMI=2,2,0,0,0\r");
  delay(1000);
  String smsMessage = "Hello From ESP32";
  SerialMon.println(smsMessage);
  modem.sendSMS(ADMIN_NUMBER, smsMessage);
}

void loop()
{
  // Check network and signal strength every 10 seconds
  if (millis() - lastCheckTime >= 10000)
  {
    lastCheckTime = millis();

    String networkStatus = checkNetwork();
    SerialMon.println("Current Network Status: " + networkStatus);

    String signalStrength = getSignalStrength();
    SerialMon.println("Current Signal Strength: " + signalStrength);
  }
}