#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(115200);

  // Initialize I2C with custom SDA and SCL pins
  Wire.begin(22, 23); // SDA on GPIO 22, SCL on GPIO 23

  // Initialize LCD
  lcd.init();
  lcd.backlight(); // Turn on LCD backlight
  lcd.clear();     // Clear display
  lcd.setCursor(0, 0);
  lcd.print("LCD Ready!"); // Test message
}

void loop()
{
  lcd.print("WoW I2C LCD");   // Display static text on the first line
  delay(1000);                // Delay to allow the message to be read
  lcd.clear();                // Clear the display
  lcd.print("Hello, World!"); // Display static text on the second line
  delay(1000);                // Delay to allow the message to be read
  lcd.clear();                // Clear the display again
  // Display dynamic text from Serial Monitor
  if (Serial.available())
  {
    delay(100); // Allow time for message to arrive
    lcd.clear();
    lcd.print("Received:"); // Static part of the message
    while (Serial.available() > 0)
    {
      lcd.write(Serial.read()); // Display received characters
    }
  }
}