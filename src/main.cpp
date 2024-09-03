#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "WS2812.pio.h"        // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/leds.h"        // Include the header for the LEDs control class

#define LED_PIN 14        // Pin where the LED data line is connected
#define NUM_LEDS 12        // Number of LEDs in the strip

int main() {
        stdio_init_all();   // Initialize all standard IO

    // Initialize PIO and state machine for controlling LEDs
    PIO pio = pio0;    // Use PIO0
    uint sm = 0;       // Use state machine 0

    // Create an instance of the LEDs class
    LEDs ledStrip(LED_PIN, NUM_LEDS, pio, sm);

    // Main loop
    while (true) {
        // Always set the 12th LED to blue before any updates
        ledStrip.setColor(11, 0, 0, 255);  // Set LED 12 to blue

        // Set other individual colors
        ledStrip.setColor(0, 255, 0, 0);   // Set LED 1 to red
        ledStrip.setColor(1, 0, 255, 0);   // Set LED 2 to green
        ledStrip.setColor(2, 0, 0, 255);   // Set LED 3 to blue

        // Update LEDs to show the set colors
        ledStrip.update();
        sleep_ms(1000);  // Wait for 1 second

        // Clear all LEDs except the 12th one
        ledStrip.clear();
        ledStrip.setColor(11, 0, 0, 255);  // Ensure LED 12 stays blue
        ledStrip.update();
        sleep_ms(1000);

        // Demonstrate setting and then updating individually
        ledStrip.setColor(0, 255, 255, 0);  // Change LED 1 to yellow
        ledStrip.setColor(1, 255, 0, 255);  // Change LED 2 to magenta
        ledStrip.setColor(2, 0, 255, 255);  // Change LED 3 to cyan
        ledStrip.update();
        sleep_ms(1000);

        // Clear all LEDs again except the 12th one
        ledStrip.clear();
        ledStrip.setColor(11, 0, 0, 255);  // Ensure LED 12 stays blue
        ledStrip.update();
        sleep_ms(1000);
    }

    return 0;
}
