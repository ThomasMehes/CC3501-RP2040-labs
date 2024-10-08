#ifndef LIS3DH_H
#define LIS3DH_H

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>

class LIS3DH {
public:
    LIS3DH(i2c_inst_t* i2c_instance, uint8_t i2c_address, uint8_t sda_pin, uint8_t scl_pin);
    
    // Initialization function
    bool init();  
    
    // Function to read data from a register
    bool read_register(uint8_t reg, uint8_t* data, uint8_t length);
    
    // Function to write data to a register
    bool write_register(uint8_t reg, uint8_t data);
    
    // Function to read acceleration data for X, Y, and Z axes
    bool read_acceleration(int16_t* x, int16_t* y, int16_t* z);

        // Function to read acceleration data for X, Y, and Z axes
    bool read_acceleration_g(float* x_g, float* y_g, float* z_g);

private:
    i2c_inst_t* i2c_instance;  // I2C instance to be used
    uint8_t i2c_address;       // I2C address of the device
    uint8_t sda_pin;           // SDA pin number
    uint8_t scl_pin;           // SCL pin number
};

#endif // LIS3DH_H
