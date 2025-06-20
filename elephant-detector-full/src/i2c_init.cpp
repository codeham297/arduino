#include "i2c_init.h"

void initI2CBuses()
{
    // Initialize I2C communication
    Wire.begin(SDA_PIN, SCL_PIN); // Default I2C bus for vibration sensor and LCD
}