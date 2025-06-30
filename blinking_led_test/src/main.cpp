#include <Arduino.h>
#define RED_LED 2

void setup()
{
  Serial.begin(115200);
  pinMode(RED_LED, OUTPUT);
}

void loop()
{

  Serial.println("RED LED ON");
  digitalWrite(RED_LED, HIGH);
  delay(1000); // Keep RED LED ON for 1 second
  Serial.println("RED LED OFF");
  digitalWrite(RED_LED, LOW);
  delay(1000); // Keep RED LED OFF for 1 second
}
