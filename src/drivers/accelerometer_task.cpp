#include "lis3dh.h"
#include "leds.h"
#define NUM_LEDS 12             // Number of LEDs in the strip

// Function to implement a digital spirit level using the LIS3DH and LED strip
void accelerometer_spirit_level(LIS3DH& lis3dh, LEDs& ledStrip) {
    float x_g, y_g, z_g;

    printf("Entering accelerometer_spirit_level\n");

    // Replace `while(true)` with a finite loop, e.g., 10 iterations
    for (int i = 0; i < 3; i++) {
        printf("Iteration %d: Reading acceleration...\n", i);
        // Read acceleration data in g
        if (!lis3dh.read_acceleration_g(&x_g, &y_g, &z_g)) {
            printf("Failed to read acceleration data\n");
            continue;
        }

        // Print the acceleration values to the terminal
        printf("X: %.3f g, Y: %.3f g, Z: %.3f g\n", x_g, y_g, z_g);

        // Map the X and Y axis tilt to LED positions
        int led_x = (int)((x_g + 1) * 6);  // Map x from -1g to 1g onto LEDs 0-12
        int led_y = (int)((y_g + 1) * 6);  // Map y from -1g to 1g onto LEDs 0-12

        // Ensure the indices are within bounds
        led_x = led_x < 0 ? 0 : (led_x >= NUM_LEDS ? NUM_LEDS - 1 : led_x);
        led_y = led_y < 0 ? 0 : (led_y >= NUM_LEDS ? NUM_LEDS - 1 : led_y);

        printf("Mapped LED positions: led_x = %d, led_y = %d\n", led_x, led_y);

        // Clear the LED strip
        ledStrip.clear();

        // Set the appropriate LEDs to indicate tilt
        ledStrip.setColor(led_x, 255, 0, 0);  // X axis tilt in red
        ledStrip.setColor(led_y, 0, 255, 0);  // Y axis tilt in green

        printf("Updating LED strip\n");

        // Update the LED strip to show the new state
        ledStrip.update();

        // Small delay before the next read
        sleep_ms(500);
    }
    printf("Exiting accelerometer_spirit_level\n");
}

