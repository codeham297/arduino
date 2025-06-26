#include "blynk.h"
#include "globals.h"
#include "rfid_reader.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting RFID Water Meter System...");

  // Initialize GPIO pins
  pinMode(SOLENOID_VALVE, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  xTaskCreate(BlynkManagerTask, "BlynkManagerTask", 20048, NULL, 1, NULL);

  // Initialize RFID reader
  // initRFIDReader();

  // Initialize Blynk
  // initBlynk();

  // // Start tasks
  // xTaskCreate(waterFlowTask, "WaterFlowTask", 2048, NULL, 1, &waterFlowTaskHandle);
  // xTaskCreate(rfidScanner, "RFIDScanner", 2048, NULL, 1, &rfidScannerTaskHandle);
}
void loop()
{
  // Main loop can be empty as tasks handle the logic
  delay(1000); // Just to avoid busy-waiting
  // sendData("System is running..."); // Example data sending
}