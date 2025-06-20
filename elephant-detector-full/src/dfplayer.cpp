#include "dfplayer.h"

HardwareSerial mp3Serial(2); // Use UART2 for DFPlayer Mini
DFRobotDFPlayerMini player;

int currentTrack = 4;
unsigned long previousMillis = 0;
const unsigned long interval = 5000; // 5 seconds between tracks
bool playerReady = false;

void tryToConnectDFPlayer(int maxAttempts)
{
    for (int attempt = 1; attempt <= maxAttempts; attempt++)
    {
        Serial.print("Attempt ");
        Serial.print(attempt);
        Serial.println(" to connect to DFPlayer...");

        if (player.begin(mp3Serial))
        {
            Serial.println("DFPlayer Mini initialized!");
            player.volume(30);
            player.play(2); // Optional: play startup track
            playerReady = true;
            return;
        }
        delay(1000);
    }
    Serial.println("Unable to connect to DFPlayer after several attempts.");
    sendMessage("DFPlayer connection failed after several attempts.");
    displayMessage("DFPlayer Failed to Connect");
}

void initDFPlayer()
{
    delay(5000); // Give GSM time to process first SMS

    Serial.println("Initializing DFPlayer...");
    mp3Serial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX); // RX, TX
    tryToConnectDFPlayer();
    previousMillis = millis();
}

void playNextTrack()
{
    if (!playerReady)
        return;

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

void playTrack(int track)
{
    if (!playerReady)
        return;
    if (player.readState() == 1)
    {
        return;
    }
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        player.play(track);
        Serial.println("Now playing track " + track);

        previousMillis = currentMillis;
    }
}