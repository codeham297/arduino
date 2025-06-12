#include "DFRobotDFPlayerMini.h"

// Use ESP32's Hardware Serial (UART2)
#define PIN_MP3_TX 18        // Connects to module's RX
#define PIN_MP3_RX 17        // Connects to module's TX
HardwareSerial mp3Serial(2); // Use UART2
DFRobotDFPlayerMini player;

void setup()
{
  Serial.begin(115200);                                      // Debugging
  mp3Serial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX); // ESP32 UART

  if (player.begin(mp3Serial))
  {
    Serial.println("DFPlayer Mini initialized!");
    player.volume(30);
    player.play(1);
  }
  else
  {
    Serial.println("Failed to connect to DFPlayer Mini!");
  }
}

void loop()
{
  player.volume(20); // Set volume to 20
  player.play(2);
  delay(5000); // Play the second track every 5 seconds
  player.play(3);
  delay(5000);       // Play the third track every 5 seconds
  player.volume(30); // Increase volume to 30
  player.play(1);
  delay(50000); // Play the first track every 50 seconds
}