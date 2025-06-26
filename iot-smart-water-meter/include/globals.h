// GPIO pin definitions for ESP32
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23
#define SS_PIN 5
#define RST_PIN 4

#define FLOW_SENSOR_PIN 14

#define SOLENOID_VALVE 16

#define RED_LED 33           // Card not recognized
#define GREEN_LED 26         // Card recognized
#define BLUE_LED 32          // Blynk connected
#define YELLOW_LED 25        // Indicator for solenoid open / water flow
#define WHITE_LED 27         // Leaking water detected
#define PULSES_PER_LITER 450 // Adjust based on sensor specs

extern String message;
