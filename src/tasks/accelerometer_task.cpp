// accelerometer_task.cpp
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "accelerometer_task.h"
#include "hardware/gpio.h"
#include "drivers/leds.h"
#include "drivers/lis3dh.h"
#include "task_manager.h"

void run_accelerometer_task(LIS3DH &lis3dh, LEDs &ledStrip) {
    float x_g, y_g, z_g;

    // Loop for the accelerometer task
    while (true) {
        // Read acceleration data
        if (!lis3dh.read_acceleration_g(&x_g, &y_g, &z_g)) {
            printf("Failed to read acceleration data\n");
            continue;
        }

        // Print the acceleration values to the terminal
        printf("X: %.3f g, Y: %.3f g, Z: %.3f g\n", x_g, y_g, z_g);

        // Map the X, Y, and Z axis tilt to LED positions
        int led_x = (int)((x_g + 1) * 2);  // Map x from -1g to 1g onto LEDs 0-3
        int led_y = 4 + (int)((y_g + 1) * 2);  // Map y from -1g to 1g onto LEDs 4-7
        int led_z = 8 + (int)((z_g + 1) * 2);  // Map z from -1g to 1g onto LEDs 8-11

        // Ensure the indices are within bounds
        led_x = led_x < 0 ? 0 : (led_x >= 4 ? 3 : led_x);
        led_y = led_y < 4 ? 4 : (led_y >= 8 ? 7 : led_y);
        led_z = led_z < 8 ? 8 : (led_z >= 12 ? 11 : led_z);

        printf("Mapped LED positions: led_x = %d, led_y = %d, led_z = %d\n", led_x, led_y, led_z);

        // Clear the LED strip
        ledStrip.clear();
        sleep_ms(1);

        // Set the appropriate LEDs to indicate tilt for each axis
        ledStrip.setColor(led_x, 255, 0, 0);  // X axis tilt in red
        ledStrip.setColor(led_y, 0, 255, 0);  // Y axis tilt in green
        ledStrip.setColor(led_z, 0, 0, 255);  // Z axis tilt in blue

        // Update the LED strip to show the new state
        ledStrip.update();
        printf("LEDs updated: led_x = %d, led_y = %d, led_z = %d\n", led_x, led_y, led_z);  // Debug output

        // Add a delay after the update to ensure LEDs remain visible
        sleep_ms(100);  // Adjust the delay as needed to give enough time for the LEDs to be visible

        // If task was switched, break out of the loop
        if (current_task != ACCELEROMETER_TASK) {
            break;
        }
    }
}
