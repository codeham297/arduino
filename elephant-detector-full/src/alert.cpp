#include "alert.h"

void triggerAlert(const char *message)
{
    Serial.println("ALERT: Sending emergency notification...");
    sendMessage(message);                   // Send an emergency SMS
    displayMessage("ALERT: Check System!"); // Show alert on LCD
    activateBuzzer();                       // Trigger buzzer for an audible warning
}

void activateBuzzer()
{
    Serial.println("Buzzer activated!");
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH); // Turn buzzer on
    delay(1000);                // Alert duration
    digitalWrite(BUZZER, LOW);  // Turn buzzer off
}