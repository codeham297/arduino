#ifndef I2C_INIT_H
#define I2C_INIT_H

#include <pins.h>
#include <Wire.h>

// Define I2C instances for multiple buses
extern TwoWire I2C_ADXL;
extern TwoWire I2C_LCD;

void initI2CBuses(); // Function to initialize I2C buses

#endif