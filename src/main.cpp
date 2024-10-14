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

#include "board.h" // Include board-specific configurations

// Global variable to track the current task
volatile Tasks current_task = LED_TASK;

// Function to handle button presses (interrupt handler)
void button_callback(uint gpio, uint32_t events) {
    // Change task when button is pressed
    current_task = (Tasks)((current_task + 1) % NUM_TASKS);  // Increment the task and wrap around
    printf("Button pressed, switching to task %d\n", current_task);
}

int main() {
    stdio_init_all();   // Initialize all standard IO

    // Initialize GPIO for button input
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);                    // Set button pin as input
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);  // Interrupt on falling edge (button press)

    // Main loop
    while (true) {
        switch (current_task) {
            case LED_TASK:
                run_led_task();  // Call the modularized LED task function
                break;

            case ACCELEROMETER_TASK: {
                run_accelerometer_task();
                break;
            }

            case MICROPHONE_TASK: {
                run_microphone_task();  
                break;
            }

            case BLUETOOTH_TASK:
                run_bluetooth_task(); 
                break;

            default:
                break;
        }
    }
    return 0;
}