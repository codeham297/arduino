#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <DFRobotDFPlayerMini.h>

// For the ESP32-C3, we will use the built-in RGB LED on pin 48
#define PIN 48 // ESP32-C3 built-in RGB LED pin
#define NUMPIXELS 1
#define DELAYVAL 500

// For the TFT display, we will use the following pins
#define TFT_CS 39
#define TFT_DC 38
#define TFT_MOSI 35
#define TFT_SCLK 36
#define TFT_RST 37 // Reset pin for the TFT display
#define TFT_BLK 40 // Backlight pin for the TFT display

// For the SIM800 module, we will use the following pins
#define SIM800_RST 21
#define SIM800_RXD 19 // TX pin for SIM800
#define SIM800_TXD 20 // RX pin for SIM800

// For the MP3 module(mp3-tf-16p v3.0), we will use the following pins
#define MP3_TX 17
#define MP3_RX 18 // RX pin for MP3 module

// For the ESP32-cam the following pins will be used
#define ESP32_CAM_TX 43 // TX pin for ESP32-CAM
#define ESP32_CAM_RX 44 // RX pin for ESP32-CAM

// For the buzzer the following pin will be used
#define BUZZER 16
// For the neopixel led
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// For the MP3 module;
HardwareSerial mp3Serial(2); // Use UART2
DFRobotDFPlayerMini player;

float p = 3.1415926;
int size = 2; // Text size multiplier

void testdrawtext(char *text, uint16_t color)
{
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
}

void setup()
{
  Serial.begin(9600);
  mp3Serial.begin(9600, SERIAL_8N1, MP3_RX, MP3_TX); // ESP32 UART

  tft.initR(INITR_MINI160x80_PLUGIN); // Init ST7735S chip, black tab
  tft.setRotation(3);                 // Set rotation to 1 (portrait mode)
  pixels.begin();
  pixels.clear();

  Serial.println(F("Initialized"));

  tft.fillScreen(ST77XX_BLACK);
  testdrawtext("ELEPHANT DETECTOR INITIALIZING", ST77XX_WHITE);
  if (player.begin(mp3Serial))
  {
    Serial.println("DFPlayer Mini initialized!");
    player.volume(20);
    player.play(3);
  }
  else
  {
    Serial.println("Failed to connect to DFPlayer Mini!");
  }

  delay(1000);
}

void loop()
{
  Serial.println("Testing NeoPixel colors...");
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red
    pixels.show();
    delay(DELAYVAL);
    pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green
    pixels.show();
    delay(DELAYVAL);
    pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // Blue
    pixels.show();
    delay(DELAYVAL);
  }

  // tft.invertDisplay(true);
  // delay(500);
  // tft.invertDisplay(false);
  // delay(500);
}
