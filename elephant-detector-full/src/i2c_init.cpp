#include "i2c_init.h"

String received_message = ""; // Global variable to store received message
String message = "";

void initSlave()
{
    // Serial.begin(115200);
    // Serial.setDebugOutput(true);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Wire.begin((uint8_t)I2C_DEV_ADDR, CAM_SDA, CAM_SCL, 100000);
}

void initI2CBuses()
{
    // Initialize I2C communication
    Wire.begin(SDA_PIN, SCL_PIN); // Default I2C bus for vibration sensor and LCD
}

int i = 0;

void onRequest()
{
    String sendStr = message;

    // Pad the message to avoid leftover characters
    while (sendStr.length() < 32)
        sendStr += ' ';

    // Send up to 32 characters safely
    size_t limit = (sendStr.length() < 32) ? sendStr.length() : 32;
    for (size_t i = 0; i < limit; i++)
    {
        Wire.write(sendStr[i]);
    }
}

void onReceive(int len)
{
    Serial.printf("onReceive[%d]: ", len);
    while (Wire.available())
    {
        Serial.write(Wire.read());
    }
    Serial.println();
}

void testSlave()
{

    // Write message to the slave
    Wire.beginTransmission(I2C_DEV_ADDR);
    Wire.printf("Hello World! %lu", i++);
    uint8_t error = Wire.endTransmission(true);
    Serial.printf("endTransmission: %u\n", error);

    // Read 16 bytes from the slave
    uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR, 16);

    Serial.printf("requestFrom: %u\n", bytesReceived);
    if ((bool)bytesReceived)
    { // If received more than zero bytes
        uint8_t temp[bytesReceived];
        Wire.readBytes(temp, bytesReceived);
        log_print_buf(temp, bytesReceived);
    }
}
String getMessageFromSlave(uint8_t addr, uint8_t len)
{
    String result = "";
    Wire.requestFrom(addr, len);
    while (Wire.available())
    {
        result += (char)Wire.read();
    }
    return result;
}
