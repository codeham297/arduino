/* Fill in information from Blynk Device Info */
#define BLYNK_TEMPLATE_ID "TMPL2fA5FeDds"
#define BLYNK_TEMPLATE_NAME "METER"
#define BLYNK_AUTH_TOKEN "-TSP4WITnmcJv5D1xOlsfmGiNeFmN9wX"

char ssid[] = "ABDULHAMEED";
char pass[] = "04012024ABDUL";

#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
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
#define RED_LED 33           // Card not recognized
#define GREEN_LED 26         // Card recognized
#define BLUE_LED 32          // Blynk connected
#define YELLOW_LED 25        // Indicator for solenoid open / water flow
#define WHITE_LED 27         // Leaking water detected
#define PULSES_PER_LITER 450 // Adjust based on sensor specs

volatile unsigned long pulseCount = 0;
static float totalWaterUsed = 0.0;
static unsigned long lastPulseCount = 0; // For delta pulse tracking

// Declare task handles
TaskHandle_t waterFlowTaskHandle = NULL;
TaskHandle_t rfidScannerTaskHandle = NULL;
TaskHandle_t checkConnectionTaskHandle = NULL;

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
    {"Alice", "33AD9C14", 50.0, 0.4},
    {"Bob", "4C870802", 75.0, 0.3},
    {"Charlie", "DB05219F", 100.0, 0.0},
    {"David", "5B06F79E", 120.0, 0.3},
    {"Eve", "DE6BD09F", 80.0, 0.0},
    {"Frank", "9B5FFE9E", 60.0, 10.0},
    {"Grace", "7BDD219F", 90.0, 0.0},
    {"Heidi", "CBCF199F", 110.0, 0.0},
    {"Ivan", "CB9C099F", 130.0, 0.0},
    {"Judy", "0BDE229F", 140.0, 0.0}};

// Create MFRC522 instance for RFID scanning
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Function declarations
void checkConnection(void *pvParameters);
void waterFlowTask(void *pvParameters);
void rfidScanner(void *pvParameters);
void blynkTask(void *pvParameters);
UserData getUserData(String scannedUID); // Returns user data for a recognized card
void IRAM_ATTR countPulse();

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

void checkConnection(void *pvParameters)
{
  bool connected_message_sent = false;
  bool disconnected_message_sent = false;

  while (true)
  {
    if (Blynk.connected())
    {
      digitalWrite(BLUE_LED, HIGH);
      if (!connected_message_sent)
      {
        Serial.println("Blynk is connected.");
        connected_message_sent = true;
        disconnected_message_sent = false;
      }
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    else
    {
      digitalWrite(BLUE_LED, LOW);
      if (!disconnected_message_sent)
      {
        Serial.println("Blynk is not connected. Attempting to reconnect...");
        disconnected_message_sent = true;
        connected_message_sent = false;
      }
      Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
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
          digitalWrite(SOLENOID_VALVE, HIGH); // Ensure valve is closed.
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
          digitalWrite(SOLENOID_VALVE, LOW); // Open valve for users with positive balance.
          digitalWrite(YELLOW_LED, HIGH);
          Serial.println("Solenoid valve opened for: " + user.userName + " (Balance: " + String(user.balance) + "L)");
        }
        else
        {
          Serial.println("Insufficient balance for user: " + user.userName);
          digitalWrite(SOLENOID_VALVE, HIGH); // Close valve due to insufficient balance.
          digitalWrite(YELLOW_LED, LOW);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        digitalWrite(GREEN_LED, LOW);
      }
      else
      {
        Serial.println("Card not recognized: " + cardUID);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(SOLENOID_VALVE, HIGH); // Ensure valve remains closed.
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
  while (true)
  {
    // Proceed only if a valid user session is active.
    if (current_user_uid != "00000000")
    {
      unsigned long newPulses = pulseCount - lastPulseCount;  // New pulses since last cycle
      float deltaWater = newPulses / (float)PULSES_PER_LITER; // New water usage (L)
      totalWaterUsed = pulseCount / (float)PULSES_PER_LITER;  // Cumulative water usage

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
              digitalWrite(SOLENOID_VALVE, HIGH);
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
      Blynk.virtualWrite(V1, currentUser.userName);
      Blynk.virtualWrite(V2, currentUser.waterUsage);
      Blynk.virtualWrite(V3, totalWaterUsed);
      Blynk.virtualWrite(V4, currentUser.balance);
    }
    else
    {
      // If no active user, send default values.
      Blynk.virtualWrite(V1, "None");
      Blynk.virtualWrite(V2, 0);
      Blynk.virtualWrite(V3, 0);
      Blynk.virtualWrite(V4, 0);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void IRAM_ATTR countPulse()
{
  pulseCount++; // Each rising edge increments the pulse count.
}

void blynkTask(void *pvParameters)
{
  while (true)
  {
    Blynk.run();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
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
  digitalWrite(SOLENOID_VALVE, HIGH); // Ensure solenoid valve is initially closed

  // Start the interrupt for water flow pulses.
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), countPulse, RISING);

  Serial.begin(9600);
  Serial.println("Starting SPI Communication...");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  Serial.println("Initializing RFID Module...");
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println("RFID Scanner Ready...");

  // Create FreeRTOS tasks with 4096 bytes of stack for each task.
  xTaskCreate(rfidScanner, "RFIDScannerTask", 4096, NULL, 2, &rfidScannerTaskHandle);
  xTaskCreate(waterFlowTask, "WaterFlowMonitor", 4096, NULL, 2, &waterFlowTaskHandle);
  xTaskCreate(checkConnection, "CheckConnectionTask", 4096, NULL, 1, &checkConnectionTaskHandle);
  xTaskCreate(blynkTask, "BlynkTask", 4096, NULL, 3, NULL);
}

void loop()
{
  // Not needed, as tasks handle execution.
}