#include <SPI.h>
#include <MFRC522.h>

// **Updated pin connections for your setup**
#define SS_PIN  5   // RFID SDA (SS) → ESP32 D5
#define RST_PIN 4   // RFID Reset → ESP32 D4

// Manually define SPI pins
#define SCK_PIN  18  // SPI Clock → ESP32 D18
#define MISO_PIN 19  // SPI MISO → ESP32 D19
#define MOSI_PIN 23  // SPI MOSI → ESP32 D23

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
    Serial.begin(9600);
    delay(2000);  // Allow ESP32 time to stabilize

    Serial.println("Starting SPI Communication...");
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // Manually assign SPI pins

    Serial.println("Initializing RFID Module...");
    mfrc522.PCD_Init();  
    mfrc522.PCD_DumpVersionToSerial();  // Print RFID module version

    Serial.println("RFID Scanner Ready...");
}

void loop() {
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    // Show UID on serial monitor
    Serial.println();
    Serial.print(" UID tag: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    
    delay(2000);  // Delay before scanning again
}