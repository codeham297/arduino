#include <Arduino.h>
#define RED_LED 26    // Card not recognized
#define GREEN_LED 33  // Card recognized
#define BLUE_LED 25   // Blynk connected
#define YELLOW_LED 32 // Indicator for solenoid open / water flow
#define WHITE_LED 27  // Leaking water detected

void setup()
{
  pinMode(RED_LED, OUTPUT);    // green
  pinMode(GREEN_LED, OUTPUT);  // orange blue
  pinMode(BLUE_LED, OUTPUT);   // red green
  pinMode(YELLOW_LED, OUTPUT); // blue
  pinMode(WHITE_LED, OUTPUT);  // white
  Serial.begin(115200);
  Serial.println("Starting LED Blink Example...");
}

void loop()
{

  Serial.println("RED LED ON");
  digitalWrite(RED_LED, HIGH);
  delay(1000); // Keep RED LED ON for 1 second
  Serial.println("RED LED OFF");
  digitalWrite(RED_LED, LOW);
  delay(1000); // Keep RED LED OFF for 1 second
  Serial.println("GREEN LED ON");
  digitalWrite(GREEN_LED, HIGH);
  delay(1000); // Keep GREEN LED ON for 1 second
  Serial.println("GREEN LED OFF");
  digitalWrite(GREEN_LED, LOW);
  delay(1000); // Keep GREEN LED OFF for 1 second
  Serial.println("BLUE LED ON");
  digitalWrite(BLUE_LED, HIGH);
  delay(1000); // Keep BLUE LED ON for 1 second
  Serial.println("BLUE LED OFF");
  digitalWrite(BLUE_LED, LOW);
  delay(1000); // Keep BLUE LED OFF for 1 second
  Serial.println("YELLOW LED ON");
  digitalWrite(YELLOW_LED, HIGH);
  delay(1000); // Keep YELLOW LED ON for 1 second
  Serial.println("YELLOW LED OFF");
  digitalWrite(YELLOW_LED, LOW);
  delay(1000); // Keep YELLOW LED OFF for 1 second
  Serial.println("WHITE LED ON");
  digitalWrite(WHITE_LED, HIGH);
  delay(1000); // Keep WHITE LED ON for 1 second
  Serial.println("WHITE LED OFF");
  digitalWrite(WHITE_LED, LOW);
  delay(1000); // Keep WHITE LED OFF for 1 second
  Serial.println("Looping through LEDs...");
  delay(1000); // Delay before the next loop iteration
}
