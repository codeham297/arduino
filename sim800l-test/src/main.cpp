// Define modem type
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>

// Define ESP32-S3 UART Pins for SIM800L
#define SIM800_RST  21
#define SIM800_RXD  19  // TX from SIM800L
#define SIM800_TXD  20  // RX to SIM800L

// Use Hardware Serial for SIM800L
#define SerialMon Serial      // Debug output (USB Serial)
#define SerialAT Serial1      // SIM800L AT commands

TinyGsm modem(SerialAT);

void setup() {
    SerialMon.begin(115200);     // Start serial for debugging
    SerialAT.begin(9600, SERIAL_8N1, SIM800_RXD, SIM800_TXD);  // Start serial for SIM800L
    delay(3000);

    // Initialize modem
    SerialMon.println("Initializing SIM800L...");
    if (!modem.restart()) {
        SerialMon.println("Failed to restart modem");
        return;
    }

    SerialMon.println("Modem Initialized.");
}

// Send SMS Function
void sendSMS(const char *number, const char *message) {
    bool success = modem.sendSMS(number, message);
    SerialMon.println(success ? "✅ SMS Sent!" : "❌ SMS Failed.");
}

// Make a Call Function
void makeCall(const char *number) {
    bool success = modem.callNumber(number);
    SerialMon.println(success ? "📞 Calling..." : "❌ Call Failed.");
}

// Send USSD Function (e.g., balance check)
void sendUSSD(const char *code) {
    String response = modem.sendUSSD(code);
    SerialMon.println("📡 USSD Response: " + response);
}

// Get IMEI Function
void getIMEI() {
    String imei = modem.getIMEI();
    SerialMon.println("🔢 IMEI: " + imei);
}

// Get Signal Strength Function
void getSignalQuality() {
    int signal = modem.getSignalQuality();
    SerialMon.println("📶 Signal Strength: " + String(signal));
}

void loop() {
    SerialMon.println("\n🔄 Running GSM Tests...");
    
    sendSMS("+1234567890", "Hello from ESP32-S3 & SIM800L!");
    makeCall("+1234567890");
    sendUSSD("*111#");  // Change USSD code for balance check
    getIMEI();
    getSignalQuality();
    delay(5000); // Run every 60 seconds
}