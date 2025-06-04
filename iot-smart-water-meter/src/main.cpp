/* Fill in information from Blynk Device Info */
#define BLYNK_TEMPLATE_ID "TMPL2fA5FeDds"
#define BLYNK_TEMPLATE_NAME "METER"
#define BLYNK_AUTH_TOKEN "-TSP4WITnmcJv5D1xOlsfmGiNeFmN9wX"

char ssid[] = "ABDULHAMEED";
char pass[] = "04012024ABDUL";

#include <Arduino.h>
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
#define RED_LED 33    // Card not recognized
#define GREEN_LED 26  // Card recognized
#define BLUE_LED 32   // Blynk connected
#define YELLOW_LED 25 // Solenoid valve open
#define WHITE_LED 27  // Leaking water detected

// Declare task handles
TaskHandle_t waterFlowTaskHandle = NULL;
TaskHandle_t rfidScannerTaskHandle = NULL;
TaskHandle_t checkConnectionTaskHandle = NULL;

// Define user data struct
String current_user_uid = "00000000"; // To store the UID of the currently scanned user

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
    {"Frank", "9B5FFE9E", 60.0, 5.0},
    {"Grace", "7BDD219F", 90.0, 0.0},
    {"Heidi", "CBCF199F", 110.0, 0.0},
    {"Ivan", "CB9C099F", 130.0, 0.0},
    {"Judy", "0BDE229F", 140.0, 0.0}};

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Function declarations (unchanged)
void checkConnection(void *pvParameters);
void waterFlow(void *pvParameters);
void rfidScanner(void *pvParameters);
void blynkTask(void *pvParameters);
UserData getUserData(String scannedUID); // Now returns user data instead of handling logic
void saveUserData();

// Function to retrieve user data based on scanned UID
UserData getUserData(String scannedUID)
{
  for (int i = 0; i < 10; i++)
  {
    if (users[i].cardUID == scannedUID)
    {
      return users[i]; // Return the user data if found
    }
  }
  return {"Unknown", "", 0.0, 0.0}; // Default object for unknown users
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
  digitalWrite(SOLENOID_VALVE, HIGH);

  Serial.begin(9600);
  Serial.println("Starting SPI Communication...");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  Serial.println("Initializing RFID Module...");
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println("RFID Scanner Ready...");

  // xTaskCreate(waterFlow, "WaterFlowTask", 2048, NULL, 2, &waterFlowTaskHandle);
  xTaskCreate(rfidScanner, "RFIDScannerTask", 2048, NULL, 2, &rfidScannerTaskHandle);
  // xTaskCreate(checkConnection, "CheckConnectionTask", 2048, NULL, 1, &checkConnectionTaskHandle);
  // xTaskCreate(blynkTask, "BlynkTask", 2048, NULL, 3, NULL);
}

void loop() {}

// Define a task for Blynk.run()
void blynkTask(void *pvParameters)
{
  while (true)
  {
    Blynk.run();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
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
  int cardAbsenceCounter = 0; // Track how many times a card is missing before confirming removal

  while (true)
  {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
      if (cardPresent)
      {
        cardAbsenceCounter++; // Increment counter only when card was previously detected

        if (cardAbsenceCounter >= 5) // Wait for multiple failed checks before confirming removal
        {
          Serial.println("Card removed. Resetting current_user_uid...");
          current_user_uid = "00000000";
          digitalWrite(SOLENOID_VALVE, HIGH); // Close the solenoid valve
          digitalWrite(YELLOW_LED, LOW);      // Turn off the yellow LED
          cardPresent = false;
          cardAbsenceCounter = 0;
        }
      }
      vTaskDelay(200 / portTICK_PERIOD_MS);
      continue;
    }

    cardAbsenceCounter = 0; // Reset absence counter if card is detected

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

    if (!cardPresent || current_user_uid != user.cardUID)
    {
      if (user.userName != "Unknown")
      {

        Serial.println("User: " + user.userName + ", UID: " + user.cardUID +
                       ", Water Usage: " + String(user.waterUsage) + "L, Balance: " + String(user.balance) + "USD");
        digitalWrite(user.cardUID != "" ? GREEN_LED : RED_LED, HIGH);

        if (user.balance > 0.0)
        {
          digitalWrite(SOLENOID_VALVE, LOW); // Open the solenoid valve for the user
          digitalWrite(YELLOW_LED, HIGH);    // Turn on the yellow LED to indicate water flow
          Serial.println("Solenoid valve opened for user: " + user.userName + String(user.balance) + "USD");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        digitalWrite(user.cardUID != "" ? GREEN_LED : RED_LED, LOW);
      }
      else
      {
        Serial.println("Card not recognized: " + cardUID);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(SOLENOID_VALVE, HIGH); // Close the solenoid valve
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        digitalWrite(RED_LED, LOW);
      }

      current_user_uid = user.cardUID;
      cardPresent = true;
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void waterFlow(void *pvParameters)
{
  static unsigned long lastFlowTime = 0;
  static unsigned long flowCount = 0;

  while (true)
  {
    int flowState = digitalRead(FLOW_SENSOR_PIN);

    if (flowState == HIGH)
    {
      digitalWrite(YELLOW_LED, HIGH);
      if (millis() - lastFlowTime > 1000)
      {
        flowCount++;
        Serial.print("Water Flow Count: ");
        Serial.println(flowCount);
        lastFlowTime = millis();
      }
    }
    else
    {
      digitalWrite(YELLOW_LED, LOW);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}