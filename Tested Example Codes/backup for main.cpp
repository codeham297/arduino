/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL2fA5FeDds"
#define BLYNK_TEMPLATE_NAME "METER"
#define BLYNK_AUTH_TOKEN "-TSP4WITnmcJv5D1xOlsfmGiNeFmN9wX"

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ABDULHAMEED";
char pass[] = "04012024ABDUL";

// Manually define SPI pins used on RFID module
// These pins are connected to the ESP32
#define SCK_PIN 18  // SPI Clock → ESP32 D18
#define MISO_PIN 19 // SPI MISO → ESP32 D19
#define MOSI_PIN 23 // SPI MOSI → ESP32 D23
#define SS_PIN 5    // RFID SDA (SS) → ESP32 D5
#define RST_PIN 4   // RFID Reset → ESP32 D4

// Define GPIO pins for various components
#define FLOW_SENSOR_PIN 14 // Flow sensor data pin (GPIO 14)
#define SOLENOID_VALVE 16  // Solenoid valve control pin (GPIO 16)
#define RED_LED 33         // RFID authentification failure LED pin (GPIO 33)
#define GREEN_LED 26       // RFID authentification success LED pin (GPIO 26)
#define BLUE_LED 32        // System is running and connected to the internet LED pin (GPIO 32)
#define YELLOW_LED 25      // Water flow indicator LED pin (GPIO 25)
#define WHITE_LED 27       // Blinks rapidly when water is leaking (GPIO 27)

// Define functions
void checkConnection(); // Function to check Blynk connection status
void ledPowerTest();    // Function to test LED functionality

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
  ledPowerTest();    // Test LEDs to ensure they are working
  checkConnection(); // Check Blynk connection status
  delay(2000);       // Allow ESP32 time to stabilize
  Serial.println("Starting SPI Communication...");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN); // Manually assign SPI pins
  Serial.println("Initializing RFID Module...");
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial(); // Print RFID module version
  Serial.println("RFID Scanner Ready...");
}

void loop()
{
  Blynk.run();
  checkConnection(); // Check Blynk connection status
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag: ");
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  // Here you can add code to send the UID to Blynk or perform other actions
  delay(2000); // Delay before scanning again
}

void checkConnection()
{
  if (Blynk.connected())
  {
    digitalWrite(BLUE_LED, HIGH); // Turn on blue LED if connected
    Serial.println("Blynk is connected.");
  }
  else
  {
    digitalWrite(BLUE_LED, LOW); // Turn off blue LED if not connected
    Serial.println("Blynk is not connected. Attempting to reconnect...");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  }
}

void ledPowerTest()
{
  // Blink the white LED rapidly to indicate a test
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(SOLENOID_VALVE, HIGH); // Ensure solenoid valve is off during test
    digitalWrite(WHITE_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    delay(1000); // LED on for 100 ms
    digitalWrite(WHITE_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    delay(1000); // LED off for 100 ms
  }
  Serial.println("LED test completed.");
}