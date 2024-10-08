#include <stdio.h>
#include <cmath>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "WS2812.pio.h"         // Include WS2812 LED driver header
#include "drivers/leds.h"       // Include the header for the LEDs control class
#include "drivers/lis3dh.h"     // Include the header for the Accelerator control class
#include "drivers/accelerometer_task.h"  // Include the header for the accelerometer task
#include "drivers/microphone.h" // Include the header for the mircophone task
#include "tasks/microphone_task.h" 
#include "arm_math.h"
#include "arm_const_structs.h"  // Include the CMSIS-DSP library for FFT


#define LED_PIN 14              // Pin where the LED data line is connected
#define NUM_LEDS 12             // Number of LEDs in the strip
#define I2C_PORT i2c0           // Define the I2C port
#define I2C_SDA_PIN 16          // Define the SDA pin for I2C
#define I2C_SCL_PIN 17          // Define the SCL pin for I2C
#define LIS3DH_I2C_ADDRESS 0x19 // The I2C address of the LIS3DH
#define BUTTON_PIN 15           // GPIO pin for the button (SWI)
#define SNAKE_LENGTH 6          // Length of the "snake"
#define BUFFER_SIZE 1024        // Define buffer size for reading samples
#define FFT_SIZE 1024           // Define the size of the FFT (must be a power of 2)
#define DC_OFFSET 2048          // Example DC offset value, modify based on your microphone

// Task identifiers
enum Tasks {
    SNAKE_TASK,
    ACCELEROMETER_TASK,
    MICROPHONE_TASK,
    PLACEHOLDER_TASK, 
    NUM_TASKS
};

// Global variable to track the current task
volatile Tasks current_task = SNAKE_TASK;

// Define the Hanning window in Q15 format as a global constant
const q15_t hanning_window[BUFFER_SIZE] = {
    /* Insert the Q15 formatted Hanning window values generated from MATLAB here */
    0, 0, 1, 3, 5, 8, 11, 15, 20, 25, 31, 37, 44, 52, 61, 69, 79, 89, 100, 111, 123, 136, 149, 163, 178, 193, 208, 225, 242, 259, 277, 296, 315, 335, 356, 377, 399, 421, 444, 468, 492, 517, 542, 568, 595, 622, 650, 678, 707, 736, 767, 797, 829, 860, 893, 926, 960, 994, 1029, 1064, 1100, 1137, 1174, 1211, 1250, 1288, 1328, 1368, 1408, 1449, 1491, 1533, 1576, 1619, 1663, 1708, 1753, 1798, 1844, 1891, 1938, 1986, 2034, 2083, 2133, 2182, 2233, 2284, 2335, 2387, 2440, 2493, 2547, 2601, 2656, 2711, 2766, 2823, 2879, 2937, 2994, 3053, 3111, 3171, 3230, 3291, 3351, 3413, 3474, 3536, 3599, 3662, 3726, 3790, 3855, 3920, 3985, 4051, 4118, 4185, 4252, 4320, 4388, 4457, 4526, 4596, 4666, 4737, 4808, 4879, 4951, 5023, 5096, 5169, 5243, 5317, 5391, 5466, 5541, 5617, 5693, 5769, 5846, 5923, 6001, 6079, 6158, 6236, 6316, 6395, 6475, 6555, 6636, 6717, 6799, 6880, 6962, 7045, 7128, 7211, 7295, 7379, 7463, 7547, 7632, 7717, 7803, 7889, 7975, 8062, 8148, 8236, 8323, 8411, 8499, 8587, 8676, 8765, 8854, 8944, 9033, 9123, 9214, 9304, 9395, 9486, 9578, 9670, 9761, 9854, 9946, 10039, 10132, 10225, 10318, 10412, 10505, 10599, 10694, 10788, 10883, 10978, 11073, 11168, 11264, 11359, 11455, 11551, 11648, 11744, 11841, 11937, 12034, 12131, 12229, 12326, 12424, 12521, 12619, 12717, 12815, 12914, 13012, 13111, 13209, 13308, 13407, 13506, 13605, 13704, 13804, 13903, 14003, 14102, 14202, 14302, 14401, 14501, 14601, 14701, 14802, 14902, 15002, 15102, 15203, 15303, 15403, 15504, 15604, 15705, 15806, 15906, 16007, 16107, 16208, 16309, 16409, 16510, 16610, 16711, 16812, 16912, 17013, 17113, 17214, 17314, 17415, 17515, 17616, 17716, 17816, 17916, 18017, 18117, 18217, 18317, 18416, 18516, 18616, 18716, 18815, 18915, 19014, 19113, 19213, 19312, 19411, 19509, 19608, 19707, 19805, 19904, 20002, 20100, 20198, 20296, 20393, 20491, 20588, 20685, 20782, 20879, 20976, 21072, 21169, 21265, 21361, 21457, 21552, 21647, 21743, 21838, 21932, 22027, 22121, 22216, 22309, 22403, 22497, 22590, 22683, 22776, 22868, 22961, 23053, 23144, 23236, 23327, 23418, 23509, 23599, 23690, 23780, 23869, 23959, 24048, 24136, 24225, 24313, 24401, 24489, 24576, 24663, 24750, 24836, 24922, 25008, 25093, 25178, 25263, 25347, 25431, 25515, 25599, 25682, 25764, 25847, 25929, 26010, 26091, 26172, 26253, 26333, 26413, 26492, 26571, 26650, 26728, 26806, 26883, 26960, 27037, 27113, 27189, 27265, 27340, 27414, 27488, 27562, 27636, 27708, 27781, 27853, 27925, 27996, 28067, 28137, 28207, 28276, 28345, 28414, 28482, 28550, 28617, 28683, 28750, 28815, 28881, 28946, 29010, 29074, 29137, 29200, 29263, 29325, 29386, 29447, 29508, 29568, 29627, 29686, 29745, 29803, 29860, 29917, 29974, 30029, 30085, 30140, 30194, 30248, 30301, 30354, 30407, 30458, 30510, 30560, 30611, 30660, 30709, 30758, 30806, 30853, 30900, 30947, 30993, 31038, 31083, 31127, 31170, 31213, 31256, 31298, 31339, 31380, 31420, 31460, 31499, 31538, 31576, 31613, 31650, 31686, 31722, 31757, 31791, 31825, 31859, 31891, 31924, 31955, 31986, 32017, 32046, 32076, 32104, 32132, 32160, 32187, 32213, 32239, 32264, 32288, 32312, 32335, 32358, 32380, 32402, 32422, 32443, 32462, 32481, 32500, 32518, 32535, 32551, 32567, 32583, 32598, 32612, 32625, 32638, 32651, 32662, 32673, 32684, 32694, 32703, 32712, 32720, 32727, 32734, 32740, 32746, 32751, 32755, 32759, 32762, 32764, 32766, 32767, 32767, 32767, 32767, 32766, 32764, 32762, 32759, 32755, 32751, 32746, 32740, 32734, 32727, 32720, 32712, 32703, 32694, 32684, 32673, 32662, 32651, 32638, 32625, 32612, 32598, 32583, 32567, 32551, 32535, 32518, 32500, 32481, 32462, 32443, 32422, 32402, 32380, 32358, 32335, 32312, 32288, 32264, 32239, 32213, 32187, 32160, 32132, 32104, 32076, 32046, 32017, 31986, 31955, 31924, 31891, 31859, 31825, 31791, 31757, 31722, 31686, 31650, 31613, 31576, 31538, 31499, 31460, 31420, 31380, 31339, 31298, 31256, 31213, 31170, 31127, 31083, 31038, 30993, 30947, 30900, 30853, 30806, 30758, 30709, 30660, 30611, 30560, 30510, 30458, 30407, 30354, 30301, 30248, 30194, 30140, 30085, 30029, 29974, 29917, 29860, 29803, 29745, 29686, 29627, 29568, 29508, 29447, 29386, 29325, 29263, 29200, 29137, 29074, 29010, 28946, 28881, 28815, 28750, 28683, 28617, 28550, 28482, 28414, 28345, 28276, 28207, 28137, 28067, 27996, 27925, 27853, 27781, 27708, 27636, 27562, 27488, 27414, 27340, 27265, 27189, 27113, 27037, 26960, 26883, 26806, 26728, 26650, 26571, 26492, 26413, 26333, 26253, 26172, 26091, 26010, 25929, 25847, 25764, 25682, 25599, 25515, 25431, 25347, 25263, 25178, 25093, 25008, 24922, 24836, 24750, 24663, 24576, 24489, 24401, 24313, 24225, 24136, 24048, 23959, 23869, 23780, 23690, 23599, 23509, 23418, 23327, 23236, 23144, 23053, 22961, 22868, 22776, 22683, 22590, 22497, 22403, 22309, 22216, 22121, 22027, 21932, 21838, 21743, 21647, 21552, 21457, 21361, 21265, 21169, 21072, 20976, 20879, 20782, 20685, 20588, 20491, 20393, 20296, 20198, 20100, 20002, 19904, 19805, 19707, 19608, 19509, 19411, 19312, 19213, 19113, 19014, 18915, 18815, 18716, 18616, 18516, 18416, 18317, 18217, 18117, 18017, 17916, 17816, 17716, 17616, 17515, 17415, 17314, 17214, 17113, 17013, 16912, 16812, 16711, 16610, 16510, 16409, 16309, 16208, 16107, 16007, 15906, 15806, 15705, 15604, 15504, 15403, 15303, 15203, 15102, 15002, 14902, 14802, 14701, 14601, 14501, 14401, 14302, 14202, 14102, 14003, 13903, 13804, 13704, 13605, 13506, 13407, 13308, 13209, 13111, 13012, 12914, 12815, 12717, 12619, 12521, 12424, 12326, 12229, 12131, 12034, 11937, 11841, 11744, 11648, 11551, 11455, 11359, 11264, 11168, 11073, 10978, 10883, 10788, 10694, 10599, 10505, 10412, 10318, 10225, 10132, 10039, 9946, 9854, 9761, 9670, 9578, 9486, 9395, 9304, 9214, 9123, 9033, 8944, 8854, 8765, 8676, 8587, 8499, 8411, 8323, 8236, 8148, 8062, 7975, 7889, 7803, 7717, 7632, 7547, 7463, 7379, 7295, 7211, 7128, 7045, 6962, 6880, 6799, 6717, 6636, 6555, 6475, 6395, 6316, 6236, 6158, 6079, 6001, 5923, 5846, 5769, 5693, 5617, 5541, 5466, 5391, 5317, 5243, 5169, 5096, 5023, 4951, 4879, 4808, 4737, 4666, 4596, 4526, 4457, 4388, 4320, 4252, 4185, 4118, 4051, 3985, 3920, 3855, 3790, 3726, 3662, 3599, 3536, 3474, 3413, 3351, 3291, 3230, 3171, 3111, 3053, 2994, 2937, 2879, 2823, 2766, 2711, 2656, 2601, 2547, 2493, 2440, 2387, 2335, 2284, 2233, 2182, 2133, 2083, 2034, 1986, 1938, 1891, 1844, 1798, 1753, 1708, 1663, 1619, 1576, 1533, 1491, 1449, 1408, 1368, 1328, 1288, 1250, 1211, 1174, 1137, 1100, 1064, 1029, 994, 960, 926, 893, 860, 829, 797, 767, 736, 707, 678, 650, 622, 595, 568, 542, 517, 492, 468, 444, 421, 399, 377, 356, 335, 315, 296, 277, 259, 242, 225, 208, 193, 178, 163, 149, 136, 123, 111, 100, 89, 79, 69, 61, 52, 44, 37, 31, 25, 20, 15, 11, 8, 5, 3, 1, 0, 0 
};


// LED Control FFT Bins 
const int led_bins[NUM_LEDS + 1] = {6, 8, 11, 16, 24, 35, 51, 75, 110, 161, 237, 349, 513};


// Function to handle button presses (interrupt handler)
void button_callback(uint gpio, uint32_t events) {
    // Change task when button is pressed
    current_task = (Tasks)((current_task + 1) % NUM_TASKS);  // Increment the task and wrap around
    printf("Button pressed, switching to task %d\n", current_task);
}

void hueToRGB(uint hue, uint8_t* red, uint8_t* green, uint8_t* blue);


int main() {
    stdio_init_all();   // Initialize all standard IO
    printf("Starting program...\n");

    // Initialize GPIO for button input
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);                    // Set button pin as input
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);  // Interrupt on falling edge (button press)

    // Initialize PIO and state machine for controlling LEDs
    PIO pio = pio0;    // Use PIO0
    uint sm = 0;       // Use state machine 0
    LEDs ledStrip(LED_PIN, NUM_LEDS, pio, sm);

    // Create an instance of the LIS3DH accelerometer class
    LIS3DH lis3dh(I2C_PORT, LIS3DH_I2C_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN);
    if (!lis3dh.init()) {
        printf("LIS3DH initialization failed!\n");
        return -1;
    }

    printf("LIS3DH initialized.\n");

    uint16_t buffer[BUFFER_SIZE];  // Create a buffer to hold ADC samples

    uint hue = 0;  // Start with red for the snake task

    // Main loop
    while (true) {
        switch (current_task) {
            case SNAKE_TASK:
                // Task 1: Snake animation
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
                    if (current_task != SNAKE_TASK) {
                        break;
                    }
                }
                break;

            case ACCELEROMETER_TASK: {
                // Task 2: Accelerometer-based LED control
                float x_g, y_g, z_g;

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

                // Check if task was switched
                if (current_task != ACCELEROMETER_TASK) {
                    break;
                }
                break;
            }

            case MICROPHONE_TASK: {
                run_microphone_task();  // Call the modified microphone task function
                break;
            }

            case PLACEHOLDER_TASK:
                // Placeholder task: Currently does nothing but can be expanded
                printf("Placeholder task running...\n");
                sleep_ms(1000);  // Sleep for a short while to simulate a task
                break;


            default:
                break;
        }
    }
    return 0;
}

void hueToRGB(uint hue, uint8_t* red, uint8_t* green, uint8_t* blue) {
    float s = 1.0;  // Maximum saturation
    float v = 200.0;  // Maximum brightness 255
    float h = hue / 60.0;
    int i = (int)h;
    float f = h - (float)i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    switch(i) {
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