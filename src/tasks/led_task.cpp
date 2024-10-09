// led_task.cpp
#include "led_task.h"
#include "task_manager.h"
#include <stdio.h>

// Convert hue to RGB values
void hueToRGB(uint hue, uint8_t* red, uint8_t* green, uint8_t* blue) {
    float s = 1.0;  // Maximum saturation
    float v = 200.0;  // Maximum brightness 255
    float h = hue / 60.0;
    int i = (int)h;
    float f = h - (float)i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    switch (i) {
    case 0:
        *red = v, *green = t, *blue = p;
        break;
    case 1:
        *red = q, *green = v, *blue = p;
        break;
    case 2:
        *red = p, *green = v, *blue = t;
        break;
    case 3:
        *red = p, *green = q, *blue = v;
        break;
    case 4:
        *red = t, *green = p, *blue = v;
        break;
    case 5:
    default:
        *red = v, *green = p, *blue = q;
        break;
    }

    *red = (uint8_t)*red;
    *green = (uint8_t)*green;
    *blue = (uint8_t)*blue;
}

// Function to run the LED task (snake animation)
void run_led_task(LEDs& ledStrip) {
    uint hue = 0;  // Start with red for the snake task

    // Task: Snake animation
    for (int i = 0; i < NUM_LEDS; ++i) {
        // Clear all LEDs
        ledStrip.clear();

        // Set the colors of the snake
        for (int j = 0; j < SNAKE_LENGTH; ++j) {
            int led_index = (i + j) % NUM_LEDS;
            // Calculate RGB from Hue
            uint8_t red, green, blue;
            hueToRGB(hue + j * 30, &red, &green, &blue);  // Gradually change the hue along the snake

            // Print RGB values to debug
            printf("LED %d: RGB(%u, %u, %u)\n", led_index, red, green, blue);

            ledStrip.setColor(led_index, red, green, blue);
        }

        volatile int no_op_delay = 0;  // Add this before sleep or delay calls
        no_op_delay++;  // Increment to ensure it is not optimized out

        // Update LEDs to show the set colors
        ledStrip.update();
        sleep_ms(50);  // Update rate controls the speed of the snake

        // Increment hue for the next cycle to change colors
        hue = (hue + 1) % 360;

        // If task was switched, break out of the loop
        if (current_task != LED_TASK) {
            break;
        }
    }
}
