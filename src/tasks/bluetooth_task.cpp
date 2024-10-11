// bluetooth_task.cpp

#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "bluetooth_task.h"  // Include the header file for this task (create bluetooth_task.h if it doesn't exist)
#include "accelerometer_task.h"  // Include the accelerometer task
#include "drivers/leds.h"
#include "drivers/lis3dh.h"
#include "task_manager.h"
#include "board.h"

// Define UART settings for Bluetooth communication
#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 8
#define UART_RX_PIN 9

// Function to initialize UART for Bluetooth communication
void init_bluetooth_uart() {
    // Initialize UART with the desired baud rate
    uart_init(UART_ID, BAUD_RATE);

    // Set the GPIO pins for UART function (TX and RX)
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}


// Function to run the Bluetooth task
void run_bluetooth_task() {
    // Initialize UART for Bluetooth communication
    init_bluetooth_uart();

    float x_g, y_g, z_g;
    char buffer[100];  // Buffer to hold the formatted string for UART transmission

    // Initialize PIO and state machine for controlling LEDs
    PIO pio = pio0;    // Use PIO0
    uint sm = 0;       // Use state machine 0
    LEDs ledStrip(LED_PIN, NUM_LEDS, pio, sm);

    // Create an instance of the LIS3DH accelerometer class
    LIS3DH lis3dh(I2C_PORT, LIS3DH_I2C_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN);

    // Loop for the Bluetooth task
    while (true) {
        // Read acceleration data from the accelerometer
        if (!lis3dh.read_acceleration_g(&x_g, &y_g, &z_g)) {
            printf("Failed to read acceleration data\n");
            continue;
        }

        // Format the accelerometer data as a string
        snprintf(buffer, sizeof(buffer), "X: %.3f g, Y: %.3f g, Z: %.3f g\n", x_g, y_g, z_g);

        // Send the formatted string over UART to the Bluetooth module
        uart_puts(UART_ID, buffer);

        // Print the data to the terminal for debugging purposes
        printf("Sent over Bluetooth: %s", buffer);

        // Map the accelerometer data to the LED display
        int led_x = (int)((x_g + 1) * 2);  // Map x from -1g to 1g onto LEDs 0-3
        int led_y = 4 + (int)((y_g + 1) * 2);  // Map y from -1g to 1g onto LEDs 4-7
        int led_z = 8 + (int)((z_g + 1) * 2);  // Map z from -1g to 1g onto LEDs 8-11

        // Ensure the indices are within bounds
        led_x = led_x < 0 ? 0 : (led_x >= 4 ? 3 : led_x);
        led_y = led_y < 4 ? 4 : (led_y >= 8 ? 7 : led_y);
        led_z = led_z < 8 ? 8 : (led_z >= 12 ? 11 : led_z);

        // Clear the LED strip
        ledStrip.clear();
        sleep_ms(1);

        // Set the appropriate LEDs to indicate tilt for each axis
        ledStrip.setColor(led_x, 255, 0, 0);  // X axis tilt in red
        ledStrip.setColor(led_y, 0, 255, 0);  // Y axis tilt in green
        ledStrip.setColor(led_z, 0, 0, 255);  // Z axis tilt in blue

        // Update the LED strip to show the new state
        ledStrip.update();

        // Add a delay to ensure data is readable on the Bluetooth terminal
        sleep_ms(500);  // Adjust the delay as needed

        // Check if the task has been switched; if so, break out of the loop
        if (current_task != BLUETOOTH_TASK) {
            break;
        }
    }
}

