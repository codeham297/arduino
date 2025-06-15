#include "DFRobotDFPlayerMini.h"

// Pin definitions for DFPlayer Mini on UART1
#define PIN_MP3_TX 13 // ESP32 TX → DFPlayer RX
#define PIN_MP3_RX 14 // ESP32 RX ← DFPlayer TX

HardwareSerial mp3Serial(1); // Use UART1
DFRobotDFPlayerMini player;

int currentTrack = 1;
unsigned long previousMillis = 0;
const unsigned long interval = 5000; // 5 seconds between tracks
bool playerReady = false;

void tryToConnectDFPlayer(int maxAttempts = 5)
{
  for (int attempt = 1; attempt <= maxAttempts; attempt++)
  {
    Serial.print("Attempt ");
    Serial.print(attempt);
    Serial.println(" to connect to DFPlayer...");

    if (player.begin(mp3Serial))
    {
      Serial.println("DFPlayer Mini initialized!");
      player.volume(25);         // Set initial volume once
      player.play(currentTrack); // Optional: play startup track
      playerReady = true;
      return;
    }

    delay(1000);
  }

  Serial.println("Unable to connect to DFPlayer after several attempts.");
}

void setup()
{
  Serial.begin(115200); // Debug output
  delay(1500);          // Allow DFPlayer to boot

  mp3Serial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX); // RX, TX
  tryToConnectDFPlayer();
  previousMillis = millis();
}

void loop()
{
  if (!playerReady)
    return; // Skip loop if player isn't ready

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    currentTrack++;
    if (currentTrack > 3)
      currentTrack = 1;

    player.play(currentTrack);
    Serial.print("Now playing track ");
    Serial.println(currentTrack);

    previousMillis = currentMillis;
  }
}