#include "alert.h"

void initializeAlertSystem()
{
    pinMode(BUZZER, OUTPUT);          // Set buzzer pin as output
    pinMode(VIBRATION_SENSOR, INPUT); // Set vibration sensor pin as input
    digitalWrite(BUZZER, LOW);        // Ensure buzzer is off initially
    Serial.println("Alert system initialized.");
}

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
    digitalWrite(BUZZER, HIGH); // Turn buzzer on
    delay(1000);                // Alert duration
    digitalWrite(BUZZER, LOW);  // Turn buzzer off
    delay(1000);                // Wait before next alert
    Serial.println("Buzzer deactivated.");
}