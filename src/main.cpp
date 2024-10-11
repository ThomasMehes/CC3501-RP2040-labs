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