#include <Arduino.h>
#include "globals.h"
#include "blynk.h"

String oldusername = "";
float oldwaterused = 0.0;
float oldwaterusage = 0.0;
float oldbalance = 0.0;

#include <SPI.h>
#include <MFRC522.h>

#include <Preferences.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// GPIO pin definitions for ESP32
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23
#define SS_PIN 5
#define RST_PIN 4
#define FLOW_SENSOR_PIN 14
#define SOLENOID_VALVE 16

volatile unsigned long pulseCount = 0;
static float totalWaterUsed = 0.0;
static unsigned long lastPulseCount = 0; // For delta pulse tracking

// Declare task handles
TaskHandle_t waterFlowTaskHandle = NULL;
TaskHandle_t rfidScannerTaskHandle = NULL;

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
    {"Alice", "33AD9C14", 8.0, 1000},
    {"Bob", "4C870802", 15.0, 10000},
    {"Charlie", "DB05219F", 0.0, 98000.0},
    {"David", "5B06F79E", 19.0, 8000},
    {"Eve", "DE6BD09F", 12.0, 72000},
    {"Frank", "9B5FFE9E", 0.0, 100.0},
    {"Grace", "7BDD219F", 4.0, 3000},
    {"Heidi", "CBCF199F", 18.0, 40000},
    {"Ivan", "CB9C099F", 2.0, 75000},
    {"Judy", "0BDE229F", 10.0, 80000}};

// Create MFRC522 instance for RFID scanning
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Function declarations
void waterFlowTask(void *pvParameters);
void rfidScanner(void *pvParameters);
UserData getUserData(String scannedUID); // Returns user data for a recognized card
void IRAM_ATTR countPulse();
void resetRFID(); // Resets the RFID reader

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
          resetRFID(); // Reset RFID reader to handle next card detection.
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

void waterFlowTask(void *pvParameters)
{
  float deltaWater = 0.0; // Water usage in liters for the current cycle
  while (true)
  {
    // Proceed only if a valid user session is active.
    if (current_user_uid != "00000000")
    {
      unsigned long newPulses = pulseCount - lastPulseCount; // New pulses since last cycle
      deltaWater = newPulses / (float)PULSES_PER_LITER;      // New water usage (L)
      totalWaterUsed = pulseCount / (float)PULSES_PER_LITER; // Cumulative water usage

      if (newPulses > 0)
      {
        Serial.println("Delta Water Usage: " + String(deltaWater) + " L");
        Serial.println("Total Water Usage: " + String(totalWaterUsed) + " L");

        // Deduct delta from the current user's balance and add to waterUsage.
        for (int i = 0; i < 10; i++)
        {
          if (users[i].cardUID == current_user_uid)
          {
            users[i].balance -= deltaWater;
            users[i].waterUsage += deltaWater;
            if (users[i].balance <= 0)
            {
              Serial.println("Balance depleted for " + users[i].userName + "! Closing solenoid valve...");
              digitalWrite(SOLENOID_VALVE, LOW);
              digitalWrite(YELLOW_LED, LOW);
              current_user_uid = "00000000";
              pulseCount = 0;
              totalWaterUsed = 0.0;
              lastPulseCount = 0;
              break;
            }
            break;
          }
        }
      }
      // Update lastPulseCount for the next delta calculation.
      lastPulseCount = pulseCount;

      // --- Update Blynk Virtual Pins ---
      // V1 -> Username, V2 -> Total Water Used, V3 -> Latest Delta, V4 -> Current Balance
      UserData currentUser = getUserData(current_user_uid);
      sendMeterData(currentUser.userName, totalWaterUsed, deltaWater, currentUser.balance);
    }
    else
    {
      // If no active user, send default values.
      if (oldusername != "NO AUTHORIZED USER")
      {
        sendMeterData("NO AUTHORIZED USER", 0.0, 0.0, 0.0);
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void IRAM_ATTR countPulse()
{
  pulseCount++; // Each rising edge increments the pulse count.
}

void setup()
{
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  pinMode(SOLENOID_VALVE, OUTPUT);
  digitalWrite(SOLENOID_VALVE, LOW); // Ensure solenoid valve is initially closed

  // Start the interrupt for water flow pulses.
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), countPulse, RISING);

  Serial.begin(115200);
  Serial.println("Starting SPI Communication...");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  Serial.println("Initializing RFID Module...");
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println("RFID Scanner Ready...");

  // Create FreeRTOS tasks with 4096 bytes of stack for each task.
  xTaskCreate(rfidScanner, "RFIDScannerTask", 4096, NULL, 2, &rfidScannerTaskHandle);
  xTaskCreate(waterFlowTask, "WaterFlowMonitor", 4096, NULL, 2, &waterFlowTaskHandle);
  xTaskCreate(BlynkManagerTask, "BlynkTask", 4096, NULL, 3, NULL);
}

void loop()
{
  // Not needed, as tasks handle execution.
}

void resetRFID()
{
  Serial.println("[RFID] Resetting reader...");
  mfrc522.PCD_Reset(); // issue soft reset
  delay(50);           // give it a moment
  mfrc522.PCD_Init();  // re-init SPI, antenna, config
  Serial.println("[RFID] Reader re-initialized");
}