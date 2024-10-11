// board.h

#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <cmath>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "arm_math.h"
#include "arm_const_structs.h"  // Include the CMSIS-DSP library for FFT

#include "WS2812.pio.h"   // Include WS2812 LED driver header
#include "drivers/leds.h"               // DRIVER HEADERS
#include "drivers/lis3dh.h"     
#include "drivers/accelerometer.h"     
#include "drivers/microphone.h"        

#include "tasks/led_task.h"             // TASK HEADERS
#include "tasks/accelerometer_task.h" 
#include "tasks/microphone_task.h" 
#include "tasks/bluetooth_task.h"
#include "tasks/task_manager.h"  


// Hardware Configuration Definitions
#define LED_PIN 14              // Pin where the LED data line is connected
#define NUM_LEDS 12             // Number of LEDs in the strip
#define I2C_PORT i2c0           // Define the I2C port
#define I2C_SDA_PIN 16          // Define the SDA pin for I2C
#define I2C_SCL_PIN 17          // Define the SCL pin for I2C
#define LIS3DH_I2C_ADDRESS 0x19 // The I2C address of the LIS3DH
#define BUTTON_PIN 15           // GPIO pin for the button (SWI)
#define BUFFER_SIZE 1024        // Define buffer size for reading samples
#define FFT_SIZE 1024           // Define the size of the FFT (must be a power of 2)
#define DC_OFFSET 2048          // Example DC offset value, modify based on your microphone

// Global Variables
extern volatile Tasks current_task;

// Function Prototypes
void button_callback(uint gpio, uint32_t events);  // Callback function for button press

#endif // BOARD_H
