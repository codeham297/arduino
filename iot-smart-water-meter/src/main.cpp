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
void ledPowerTest();                      // Function to test LED functionality
void blynkTask(void *pvParameters);       // Forward declaration for blynkTask

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{

  pinMode(RED_LED, OUTPUT);        // Set RED LED pin as output
  pinMode(GREEN_LED, OUTPUT);      // Set GREEN LED pin as output
  pinMode(BLUE_LED, OUTPUT);       // Set BLUE LED pin as output
  pinMode(YELLOW_LED, OUTPUT);     // Set YELLOW LED pin as output
  pinMode(WHITE_LED, OUTPUT);      // Set WHITE LED pin as output
  pinMode(FLOW_SENSOR_PIN, INPUT); // Set flow sensor pin as input
  pinMode(SOLENOID_VALVE, OUTPUT); // Set solenoid valve control pin as output

  // Debug console
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  Serial.println("Connecting to Blynk...");
  Serial.println("Starting SPI Communication...");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN); // Manually assign SPI pins
  Serial.println("Initializing RFID Module...");
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial(); // Print RFID module version
  Serial.println("RFID Scanner Ready...");

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

// Update ledPowerTest() to test LEDs individually
void ledPowerTest()
{
  int leds[] = {WHITE_LED, BLUE_LED, RED_LED, GREEN_LED, YELLOW_LED};
  for (int i = 0; i < 5; i++)
  {
    digitalWrite(SOLENOID_VALVE, HIGH); // Ensure solenoid valve is off during test
    digitalWrite(leds[i], HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS); // LED on for 500 ms
    digitalWrite(leds[i], LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS); // LED off for 500 ms
  }
  Serial.println("LED test completed.");
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
    Serial.println();
    Serial.print(" UID tag: ");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      if (mfrc522.uid.uidByte[i] < 0x10)
      {
        cardUID += "0"; // Add leading zero for single digit hex values
      }
      cardUID += String(mfrc522.uid.uidByte[i], HEX); // Append UID byte to cardUID string
    }
    cardUID.toUpperCase(); // Convert UID to uppercase for consistency
    Serial.print(cardUID);
    Serial.println();
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
