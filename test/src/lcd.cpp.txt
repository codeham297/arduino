#include "lcd.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void initLCD()
{
    Serial.begin(115200);
    Serial.println(F("Initializing TFT Display..."));
    tft.initR(INITR_MINI160x80); // 160x80 display initialization
    tft.fillScreen(ST77XX_BLACK);
    Serial.println(F("TFT Initialized"));
}

void displayMessage(const char *message)
{
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 40); // Adjust position
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println(message);
}