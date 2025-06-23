#include "alert.h"

void initializeAlertSystem()
{
    pinMode(BUZZER, OUTPUT);   // Set buzzer pin as output
    digitalWrite(BUZZER, LOW); // Ensure buzzer is off initially
    Serial.println("Alert system initialized.");
}
void triggerAlert(const char *message)
{
    Serial.println("ALERT: Sending emergency notification...");

    String alertMsg = "DANGER ";
    alertMsg += message;
    alertMsg.toUpperCase();       // Convert entire message to uppercase
    String smsMessage = alertMsg; // Copy for SMS/data
    alertMsg += " ALERTING AUTHORITIES";

    displayMessage(alertMsg.c_str()); // For LCD
    playTrack(1);
    sendMessage(smsMessage.c_str()); // Send SMS
    sendData(smsMessage.c_str());    // Transmit data
    activateBuzzer();                // Audible alert
    playTrack(4);
}
void activateBuzzer()
{
    Serial.println("Buzzer activated!");
    digitalWrite(BUZZER, HIGH); // Turn buzzer on
    delay(1000);                // Alert duration
    digitalWrite(BUZZER, LOW);  // Turn buzzer off
    delay(1000);                // Wait before next alert
    Serial.println("Buzzer deactivated.");
}