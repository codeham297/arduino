#include <Arduino.h>

#ifndef PINS_H
#define PINS_H

#define SDA_PIN 18 // SDA (I2C Data) for Vibration Sensor and LCD
#define SCL_PIN 5  // SCL (I2C Clock) for Vibration Sensor and LCD

#define CAM_SDA 15 // The right way is 15 here and 14 below
#define CAM_SCL 14

// SIM800L Module Pins
#define SIM800_RST 4
#define SIM800_RXD 17 // ESP32 TX to SIM800 RX
#define SIM800_TXD 16 // ESP32 RX from SIM800 TX

// Pin definitions for DFPlayer Mini on UART1
#define PIN_MP3_TX 14 // ESP32 TX → DFPlayer RX
#define PIN_MP3_RX 13 // ESP32 RX ← DFPlayer TX

// Buzzer Pin
#define BUZZER 32

// Vibration Sensor Pin
#define LED_BUILTIN 2        // Built-in LED pin for ESP32
#define LED_BUILTIN_ESPCAM 4 // Built-in LED pin for ESP32-CAM

extern String message;
extern String received_message;
extern int environment;
// Only DECLARE creds here—defined once in blynk.cpp
extern const char *ssid;
extern const char *pass;
extern int current_environment;

#endif
