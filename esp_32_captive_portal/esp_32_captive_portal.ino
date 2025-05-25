#include <WiFi.h> // Include the Wi-Fi library for ESP32
#include <HTTPClient.h> // Include the HTTP client library for ESP32
#include <WebServer.h> // Include the Web Server library for ESP32

const char* ssid = "ABDULHAMEED"; // Wi-Fi name
const char* password = "04012024ABDUL"; // Wi-Fi password
const char* ap_ssid = "ESP32_Captive_Portal"; // Access Point SSID
const char* ap_password = "12345678"; // Access Point password


void setup() {
    Serial.begin(9600); // Start serial communication at 9600 baud rate
    delay(1000); // Wait for a second to ensure serial is ready
    WiFi.mode(WIFI_AP_STA); // Set the ESP32 to Access Point mode
    while(WiFi.status() != WL_CONNECTED) {
        Serial.println("Connecting to Wi-Fi...");
        WiFi.begin(ssid, password); // Start the connection to the Wi-Fi network
        delay(5000); // Wait for 5 seconds before retrying
        Serial.println(WiFi.status()); // Print the current Wi-Fi status
    }

// WiFi.getMode(); // Get the current Wi-Fi mode

        Serial.println("Setting up Access Point...");
        WiFi.softAP(ap_ssid, ap_password); // Start the Access Point with SSID and password
        delay(5000); // Wait for a second to ensure AP is set up
}

void loop() {
    if(WiFi.status() == WL_CONNECTED) { // Check if the ESP32 is connected to Wi-Fi
        Serial.println("Connected to Wi-Fi");
        HTTPClient http; // Create an HTTP client object
        http.begin("https://www.example.com"); // Specify the URL to connect to
        int httpCode = http.GET(); // Make a GET request
        if(httpCode > 0) { // Check if the request was successful
            String payload = http.getString(); // Get the response payload
            Serial.println(payload); // Print the response payload
            Serial.println(WiFi.getMode());
        } else {
            Serial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end(); // Close the HTTP connection
    } else {
        Serial.println("Wi-Fi not connected");
    }
}