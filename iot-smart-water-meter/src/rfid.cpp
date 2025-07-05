#include "rfid.h"
#include "waterflow.h"
#include "blynk.h"
#include <SPI.h>
#include <MFRC522.h>

float deltaWater = 0.0; // Delta water usage in liters
// RFID reader instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void rfidInit()
{
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    mfrc522.PCD_Init();
    Serial.println("RFID Scanner Ready...");
}

UserData getUserData(String scannedUID)
{
    for (int i = 0; i < 10; i++)
    {
        if (users[i].cardUID == scannedUID)
        {
            return users[i];
        }
    }
    return {"Unknown", "", 0.0, 0.0};
}

void resetRFID()
{
    mfrc522.PCD_Reset();
    delay(50);
    mfrc522.PCD_Init();
}

void rfidRun()
{
    static String cardUID = "";
    static bool cardPresent = false;
    static int cardAbsenceCounter = 0;
    static unsigned long lastPulseCountAtRemoval = 0; // Track pulses when card is removed

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        if (cardPresent)
        {
            cardAbsenceCounter++;
            if (cardAbsenceCounter >= 5)
            {
                // Update user balance before resetting
                for (int i = 0; i < 10; i++)
                {
                    if (users[i].cardUID == current_user_uid)
                    {
                        // Calculate water used since last check
                        unsigned long pulsesUsed = pulseCount - lastPulseCountAtRemoval;
                        float waterUsed = pulsesUsed / (float)PULSES_PER_LITER;

                        users[i].balance -= waterUsed;
                        users[i].waterUsage += waterUsed;
                        break;
                    }
                }

                Serial.println("Card removed. Total water used: " + String(totalWaterUsed) + " L");
                current_user_uid = "00000000";
                lastPulseCountAtRemoval = pulseCount; // Store current pulse count
                digitalWrite(SOLENOID_VALVE, LOW);
                digitalWrite(YELLOW_LED, LOW);
                cardPresent = false;
                cardAbsenceCounter = 0;
                resetRFID();
                sendMeterData("NO USER", 0.0, 0.0, 0.0); // Send data for unknown card
                sendUserDatabase();                      // Update user database
            }
        }
        return;
    }

    // Rest of the RFID logic (unchanged)
    cardAbsenceCounter = 0;
    cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        if (mfrc522.uid.uidByte[i] < 0x10)
            cardUID += "0";
        cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase();

    UserData user = getUserData(cardUID);

    if (!cardPresent || current_user_uid != user.cardUID)
    {
        if (user.userName != "Unknown")
        {
            Serial.println("User: " + user.userName + ", UID: " + user.cardUID +
                           ", Water Usage: " + String(user.waterUsage) + "L, Balance: " + String(user.balance) + "L");
            sendMeterData(user.userName, deltaWater, user.waterUsage, user.balance);

            digitalWrite(GREEN_LED, HIGH);
            if (user.balance > 0.0)
            {
                digitalWrite(SOLENOID_VALVE, HIGH);
                digitalWrite(YELLOW_LED, HIGH);
                Serial.println("Valve opened for: " + user.userName);
                lastPulseCountAtRemoval = pulseCount; // Reset pulse tracking
            }
            else
            {
                Serial.println("Insufficient balance for: " + user.userName);
                digitalWrite(SOLENOID_VALVE, LOW);
                digitalWrite(YELLOW_LED, LOW);
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            digitalWrite(GREEN_LED, LOW);
        }
        else
        {
            Serial.println("Unknown card: " + cardUID);
            digitalWrite(RED_LED, HIGH);
            digitalWrite(SOLENOID_VALVE, LOW);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            digitalWrite(RED_LED, LOW);
            sendMeterData("Unknown", 0.0, 0.0, 0.0); // Send data for unknown card
        }
        current_user_uid = user.cardUID;
        cardPresent = true;
    }
}

void rfidTask(void *pvParameters)
{
    rfidInit(); // One-time initialization

    while (true)
    {
        rfidRun(); // Continuous operation
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}