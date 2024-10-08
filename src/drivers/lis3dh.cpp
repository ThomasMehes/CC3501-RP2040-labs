#include "lis3dh.h"
#include <stdio.h>  // Required for printf

// Define the debug message flag
#define DEBUG_MESSAGES 0  // Set to 1 to enable debug messages, 0 to disable

// Robust DEBUG_PRINT macro definition using a do-while loop for better handling
#if DEBUG_MESSAGES
    #define DEBUG_PRINT(fmt, ...) do { printf(fmt, ##__VA_ARGS__); } while (0)
#else
    #define DEBUG_PRINT(fmt, ...) do { } while (0)  // No-operation macro
#endif

// Constructor
LIS3DH::LIS3DH(i2c_inst_t* i2c_instance, uint8_t i2c_address, uint8_t sda_pin, uint8_t scl_pin)
    : i2c_instance(i2c_instance), i2c_address(i2c_address), sda_pin(sda_pin), scl_pin(scl_pin) {}

// Function to initialize the accelerometer
bool LIS3DH::init() {
    // (a) Initialize the I2C interface
    i2c_init(i2c_instance, 400000);  // 400 kHz baud rate

    // (b) Set GPIO pins for SDA and SCL
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    // (c) Read the WHO_AM_I register to check communication
    uint8_t who_am_i;
    if (!read_register(0x0F, &who_am_i, 1)) {
        DEBUG_PRINT("Failed to read WHO_AM_I register\n");
        return false;
    }

    if (who_am_i != 0x33) {
        DEBUG_PRINT("WHO_AM_I register mismatch: expected 0x33, got 0x%02X\n", who_am_i);
        return false;
    }

    DEBUG_PRINT("WHO_AM_I register verified: 0x33\n");

    // (d) Configure accelerometer settings (example: set range and sample rate)
    if (!write_register(0x20, 0x57)) {  // Example: Set CTRL_REG1 for 100 Hz, normal mode
        DEBUG_PRINT("Failed to configure accelerometer\n");
        return false;
    }

    // Set measurement range to ±2g (CTRL_REG4)
    if (!write_register(0x23, 0x00)) {  // Set range to ±2g
        DEBUG_PRINT("Failed to configure accelerometer range\n");
        return false;
    }

    return true;
}

// Helper function to read from a register
bool LIS3DH::read_register(uint8_t reg, uint8_t* data, uint8_t length) {
    if (i2c_write_blocking(i2c_instance, i2c_address, &reg, 1, true) != 1) {
        DEBUG_PRINT("Failed to select register address\n");
        return false;
    }

    int bytes_read = i2c_read_blocking(i2c_instance, i2c_address, data, length, false);
    if (bytes_read != length) {
        DEBUG_PRINT("Failed to read data\n");
        return false;
    }

    return true;
}

// Helper function to write to a register
bool LIS3DH::write_register(uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = { reg, data };
    if (i2c_write_blocking(i2c_instance, i2c_address, buffer, 2, false) != 2) {
        DEBUG_PRINT("Failed to write data\n");
        return false;
    }

    return true;
}

// Function to read acceleration data for X, Y, and Z axes
bool LIS3DH::read_acceleration(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t raw_data[6];  // 2 bytes for each axis (X, Y, Z)
    
    // Perform a multi-byte read starting from the OUT_X_L register (0x28)
    if (!read_register(0x28 | 0x80, raw_data, 6)) {  // OR with 0x80 to enable auto-increment for multi-byte read
        DEBUG_PRINT("Failed to read acceleration data\n");
        return false;
    }

    // Combine the raw data into 16-bit signed integers (convert two 8-bit values into a 16-bit value)
    *x = (int16_t)(raw_data[0] | (raw_data[1] << 8)) >> 4;  // Right shift by 4 as discussed
    *y = (int16_t)(raw_data[2] | (raw_data[3] << 8)) >> 4;  // Right shift by 4
    *z = (int16_t)(raw_data[4] | (raw_data[5] << 8)) >> 4;  // Right shift by 4

    return true;
}

// Function to read acceleration data for X, Y, and Z axes and convert to g
bool LIS3DH::read_acceleration_g(float* x_g, float* y_g, float* z_g) {
    int16_t x_raw, y_raw, z_raw;

    // Read raw acceleration data
    DEBUG_PRINT("Reading raw acceleration data...\n");
    if (!read_acceleration(&x_raw, &y_raw, &z_raw)) {
        DEBUG_PRINT("Failed to read raw acceleration data\n");
        return false;
    }
    DEBUG_PRINT("Raw data read successfully: X_raw = %d, Y_raw = %d, Z_raw = %d\n", x_raw, y_raw, z_raw);

    // Sensitivity for ±2g range (default): 1 mg/digit or 0.001 g/digit
    const float sensitivity = 0.001; // 0.001 g/LSB (for ±2g range)

    // Convert raw values to g by multiplying with sensitivity
    *x_g = x_raw * sensitivity;
    *y_g = y_raw * sensitivity;
    *z_g = z_raw * sensitivity;

    return true;
}
