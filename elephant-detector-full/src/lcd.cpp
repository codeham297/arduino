#include <lcd.h>
// Define LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

void initLCD()
{

    // Initialize LCD
    lcd.init();
    lcd.backlight(); // Turn on LCD backlight
    lcd.clear();     // Clear display
    lcd.setCursor(0, 0);
    lcd.print("LCD Ready!"); // Test message
    delay(1000);
    lcd.clear();
}

void displayMessage(const char *message)
{
    lcd.clear(); // Clear previous message

    // Print first line (up to 16 characters)
    lcd.setCursor(0, 0);
    for (int i = 0; i < 16 && message[i] != '\0'; i++)
    {
        lcd.print(message[i]);
    }

    // Print second line if there is more text
    if (strlen(message) > 16)
    {
        lcd.setCursor(0, 1);
        for (int i = 16; i < 32 && message[i] != '\0'; i++)
        {
            lcd.print(message[i]);
        }
    }
    delay(1000); // Wait a bit before clearing or changing
}
void clearLCD()
{
    lcd.clear();         // Clear the LCD display
    lcd.setCursor(0, 0); // Reset cursor position to the top-left corner
}