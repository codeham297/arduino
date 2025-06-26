#include "rfid_reader.h"

// Define user data struct
String current_user_uid = "00000000"; // Stores UID of current card

struct UserData
{
    String userName;
    String cardUID;
    float waterUsage;
    float balance;
};

// Sample user database
UserData users[10] = {
    {"Alice", "33AD9C14", 8.0, 72.5},
    {"Bob", "4C870802", 15.0, 94.3},
    {"Charlie", "DB05219F", 3.0, 29.1},
    {"David", "5B06F79E", 19.0, 67.2},
    {"Eve", "DE6BD09F", 12.0, 88.6},
    {"Frank", "9B5FFE9E", 7.0, 41.7},
    {"Grace", "7BDD219F", 4.0, 99.8},
    {"Heidi", "CBCF199F", 18.0, 55.4},
    {"Ivan", "CB9C099F", 2.0, 23.9},
    {"Judy", "0BDE229F", 10.0, 76.1}};

// Create MFRC522 instance for RFID scanning
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Retrieve user data based on scanned UID
UserData getUserData(String scannedUID)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].cardUID == scannedUID)
        {
            return users[i]; // Return matching user data
        }
    }
    return {"Unknown", "", 0.0, 0.0}; // Default for unknown card
}

void initRFID()
{
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    mfrc522.PCD_Init(); // Initialize the MFRC522 RFID reader
    Serial.println("RFID Reader initialized.");
    mfrc522.PCD_Init();
    mfrc522.PCD_DumpVersionToSerial(); // Dump the reader version to
    Serial.println("RFID Scanner Ready...");
}
void rfidManagerTask(void *pvParameters)
{
    initRFID(); // Initialize RFID reader
    while (true)
    {
        rfidScanner(NULL); // Start the RFID scanning task
    }
}

void rfidScanner(void *pvParameters)
{
    String cardUID = "";
    bool cardPresent = false;
    int cardAbsenceCounter = 0; // Counts absence occurrences before confirming removal

    while (true)
    {
        // No new card detected or failed to read card: check for removal
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
        {
            if (cardPresent)
            {
                cardAbsenceCounter++; // Increase counter if card was previously read
                if (cardAbsenceCounter >= 5)
                {
                    // Do not update user balance here—the waterFlowTask already handles realtime updates.
                    Serial.println("Card removed. Total water used during session: " + String(totalWaterUsed) + " L");

                    // Reset session-specific variables.
                    current_user_uid = "00000000";
                    pulseCount = 0;
                    totalWaterUsed = 0.0;
                    lastPulseCount = 0;
                    digitalWrite(SOLENOID_VALVE, LOW); // Ensure valve is closed.
                    digitalWrite(YELLOW_LED, LOW);
                    cardPresent = false;
                    cardAbsenceCounter = 0;
                }
            }
            vTaskDelay(200 / portTICK_PERIOD_MS);
            continue;
        }

        // Card is detected; reset absence counter.
        cardAbsenceCounter = 0;
        cardUID = "";
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            if (mfrc522.uid.uidByte[i] < 0x10)
            {
                cardUID += "0";
            }
            cardUID += String(mfrc522.uid.uidByte[i], HEX);
        }
        cardUID.toUpperCase();

        UserData user = getUserData(cardUID);

        // Process a new or changed card detection.
        if (!cardPresent || current_user_uid != user.cardUID)
        {
            if (user.userName != "Unknown")
            {
                Serial.println("User: " + user.userName + ", UID: " + user.cardUID +
                               ", Water Usage: " + String(user.waterUsage) + "L, Balance: " + String(user.balance) + "L");
                digitalWrite(GREEN_LED, HIGH);
                if (user.balance > 0.0)
                {
                    digitalWrite(SOLENOID_VALVE, HIGH); // Open valve for users with positive balance.
                    digitalWrite(YELLOW_LED, HIGH);
                    Serial.println("Solenoid valve opened for: " + user.userName + " (Balance: " + String(user.balance) + "L)");
                }
                else
                {
                    Serial.println("Insufficient balance for user: " + user.userName);
                    digitalWrite(SOLENOID_VALVE, LOW); // Close valve due to insufficient balance.
                    digitalWrite(YELLOW_LED, LOW);
                }
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(GREEN_LED, LOW);
            }
            else
            {
                Serial.println("Card not recognized: " + cardUID);
                digitalWrite(RED_LED, HIGH);
                digitalWrite(SOLENOID_VALVE, LOW); // Ensure valve remains closed.
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(RED_LED, LOW);
            }
            // Set the active card UID.
            current_user_uid = user.cardUID;
            cardPresent = true;
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
