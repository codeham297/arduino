/* Fill in information from Blynk Device Info here the includes tends to call on these templates and ID's*/
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
#define SCK_PIN 18         // SPI Clock → ESP32 D18
#define MISO_PIN 19        // SPI MISO → ESP32 D19
#define MOSI_PIN 23        // SPI MOSI → ESP32 D23
#define SS_PIN 5           // RFID SDA (SS) → ESP32 D5
#define RST_PIN 4          // RFID Reset → ESP32 D4
#define FLOW_SENSOR_PIN 14 // Flow sensor data pin (GPIO 14)
#define SOLENOID_VALVE 16  // Solenoid valve control pin (GPIO 16)
#define RED_LED 33         // RFID authentification failure LED pin (GPIO 33)
#define GREEN_LED 26       // RFID authentification success LED pin (GPIO 26)
#define BLUE_LED 32        // System is running and connected to the internet LED pin (GPIO 32)
#define YELLOW_LED 25      // Water flow indicator LED pin (GPIO 25)
#define WHITE_LED 27       // Blinks rapidly when water is leaking (GPIO 27)

// Declare task handles
TaskHandle_t waterFlowTaskHandle = NULL;       // Handle for water flow task
TaskHandle_t rfidScannerTaskHandle = NULL;     // Handle for RFID task
TaskHandle_t checkConnectionTaskHandle = NULL; // Handle for connection check task

// Define functions
void checkConnection(void *pvParameters); // Function to check Blynk connection status
void waterFlow(void *pvParameters);       // Function to handle water flow sensor data
void rfidScanner(void *pvParameters);     // Function to handle RFID scanning
void blynkTask(void *pvParameters);       // Forward declaration for blynkTask
void getUserData(String scannedUID);      // Function to get user data based on scanned UID
void saveUserData();                      // Function to save user data to preferences

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{

  pinMode(RED_LED, OUTPUT);           // Set RED LED pin as output
  pinMode(GREEN_LED, OUTPUT);         // Set GREEN LED pin as output
  pinMode(BLUE_LED, OUTPUT);          // Set BLUE LED pin as output
  pinMode(YELLOW_LED, OUTPUT);        // Set YELLOW LED pin as output
  pinMode(WHITE_LED, OUTPUT);         // Set WHITE LED pin as output
  pinMode(FLOW_SENSOR_PIN, INPUT);    // Set flow sensor pin as input
  pinMode(SOLENOID_VALVE, OUTPUT);    // Set solenoid valve control pin as output
  digitalWrite(SOLENOID_VALVE, HIGH); // Ensure solenoid valve is initially closed

  // Debug console
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  Serial.println("Connecting to Blynk...");
  Serial.println("Starting SPI Communication...");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN); // Manually assign SPI pins
  Serial.println("Initializing RFID Module...");
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial(); // Print RFID module version
  Serial.println("RFID Scanner Ready...");
  // saveUserData(); // Save initial user data to preferences
  // Parallel running tasks;
  xTaskCreate(waterFlow, "WaterFlowTask", 2048, NULL, 2, &waterFlowTaskHandle);                   // Create water flow task
  xTaskCreate(rfidScanner, "RFIDScannerTask", 2048, NULL, 2, &rfidScannerTaskHandle);             // Create RFID scanner task
  xTaskCreate(checkConnection, "CheckConnectionTask", 2048, NULL, 1, &checkConnectionTaskHandle); // Create connection check task
  xTaskCreate(blynkTask, "BlynkTask", 2048, NULL, 3, NULL);                                       // Highest priority for Blynk
}

void loop()
{
  // Not used since tasks handle all functionality
}

// Define a task for Blynk.run()
void blynkTask(void *pvParameters)
{
  while (true)
  {
    Blynk.run();                          // Maintain Blynk connection
    vTaskDelay(100 / portTICK_PERIOD_MS); // Run every 100ms
  }
}

// Update checkConnection() to use vTaskDelay
void checkConnection(void *pvParameters)
{
  bool connected_message_sent = false;    // Flag to track if connection message has been sent
  bool disconnected_message_sent = false; // Flag to track if disconnection message has been sent

  while (true)
  {
    if (Blynk.connected())
    {
      digitalWrite(BLUE_LED, HIGH); // Turn on blue LED if connected
      if (!connected_message_sent)
      {
        Serial.println("Blynk is connected.");
        connected_message_sent = true;     // Set flag to true after sending notification
        disconnected_message_sent = false; // Reset disconnection flag
      }
      vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for 5 seconds
    }
    else
    {
      digitalWrite(BLUE_LED, LOW); // Turn off blue LED if not connected
      if (!disconnected_message_sent)
      {
        Serial.println("Blynk is not connected. Attempting to reconnect...");
        disconnected_message_sent = true; // Set flag to true after sending notification
        connected_message_sent = false;   // Reset connection flag
      }
      Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for 5 seconds
  }
}

// Update rfidScanner() to handle multiple cards and card removal
void rfidScanner(void *pvParameters)
{
  String cardUID = ""; // Variable to store the UID of the scanned card

  while (true)
  {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
      vTaskDelay(200 / portTICK_PERIOD_MS); // Wait before checking again
      continue;
    }
    cardUID = ""; // Reset cardUID for each new card
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      if (mfrc522.uid.uidByte[i] < 0x10)
      {
        cardUID += "0"; // Add leading zero for single digit hex values
      }
      cardUID += String(mfrc522.uid.uidByte[i], HEX); // Append UID byte to cardUID string
    }
    cardUID.toUpperCase(); // Convert UID to uppercase for consistency
    getUserData(cardUID);  // Retrieve user data based on scanned UID
    // Add logic to handle card actions here
    vTaskDelay(100 / portTICK_PERIOD_MS); // Delay before scanning again
  }
}

// Update waterFlow() to use vTaskDelay
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
    vTaskDelay(100 / portTICK_PERIOD_MS); // Run every 100ms
  }
}

// Userdata storage using Preferences library
Preferences preferences; // Create a Preferences object

struct UserData
{
  String userName;  // Store the user's name
  String cardUID;   // Store the UID of the card
  float waterUsage; // Store the water usage in liters
  float balance;    // Store the user's balance
};

// Function to save user data to preferences
UserData users[10] = {
    {"Alice", "33AD9C14", 50.0, 0.0},
    {"Bob", "4C870802", 75.0, 0.0},
    {"Charlie", "DB05219F", 100.0, 0.0},
    {"David", "5B06F79E", 120.0, 0.0},
    {"Eve", "DE6BD09F", 80.0, 0.0},
    {"Frank", "9B5FFE9E", 60.0, 0.0},
    {"Grace", "7BDD219F", 90.0, 0.0},
    {"Heidi", "CBCF199F", 110.0, 0.0},
    {"Ivan", "CB9C099F", 130.0, 0.0},
    {"Judy", "0BDE229F", 140.0, 0.0}};

// Function to save user data to preferences
void saveUserData()
{
  preferences.begin("user_data", false);               // Open preferences in read-write mode
  preferences.putBytes("users", users, sizeof(users)); // Save the users array to preferences
  Serial.println("User data saved to preferences.");
  preferences.end(); // Close preferences
}

void getUserData(String scannedUID)
{
  bool user_found = false; // Flag to check if user is found
  int user_index = -1;     // Variable to store user index if found
  // UserData users[10];                                  // Array to hold user data
  // preferences.begin("user_data", true);                // Open preferences in read-only mode
  // preferences.getBytes("users", users, sizeof(users)); // Retrieve the users array from preferences
  for (int i = 0; i < 10; i++)
  {
    if (users[i].cardUID == scannedUID)
    {
      user_found = true; // Set flag to true if user is found
      user_index = i;    // Store the index of the found user
      break;             // Exit loop if user is found
    }
  }
  if (user_found)
  {
    Serial.println("User: " + users[user_index].userName + ", UID: " + users[user_index].cardUID + ", Water Usage: " + String(users[user_index].waterUsage) + "L, Balance: " + String(users[user_index].balance) + "USD");
    digitalWrite(GREEN_LED, HIGH);         // Turn on green LED for successful authentication;
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for 1 second
    digitalWrite(GREEN_LED, LOW);          // Turn off green LED after 1 second
  }
  else
  {
    Serial.println("User not found for UID: " + scannedUID);
    digitalWrite(RED_LED, HIGH);           // Turn on red LED for authentication failure
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for 1 second
    digitalWrite(RED_LED, LOW);            // Turn off red LED after 1 second
  }

  // preferences.end(); // Close preferences
}