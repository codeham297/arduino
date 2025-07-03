
// #define FLOW_SENSOR_PIN 14

// #define SOLENOID_VALVE 16

// extern String message;
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// GPIO pins
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23
#define SS_PIN 5
#define RST_PIN 4
#define FLOW_SENSOR_PIN 14
#define SOLENOID_VALVE 16

// // LED pins
// #define RED_LED 25
// #define GREEN_LED 26
// #define BLUE_LED 27
// #define YELLOW_LED 32
// #define WHITE_LED 33
// Constants
// #define PULSES_PER_LITER 459

#define RED_LED 26           // Card not recognized
#define GREEN_LED 33         // Card recognized
#define BLUE_LED 25          // Blynk connected
#define YELLOW_LED 32        // Indicator for solenoid open / water flow
#define WHITE_LED 27         // Leaking water detected
#define PULSES_PER_LITER 459 // Adjust based on sensor specs

// Shared variables
extern volatile unsigned long pulseCount;
extern float totalWaterUsed;
extern String current_user_uid;

// User data structure
struct UserData
{
    String userName;
    String cardUID;
    float waterUsage;
    float balance;
};

// Add to existing globals.h
extern UserData users[10]; // Declaration

// Pulse counter ISR
void IRAM_ATTR countPulse();

#endif
