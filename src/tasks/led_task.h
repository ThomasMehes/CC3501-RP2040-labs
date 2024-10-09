// led_task.h
#ifndef LED_TASK_H
#define LED_TASK_H

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "drivers/leds.h"

// Define constants specific to the LED task
#define LED_PIN 14
#define NUM_LEDS 12
#define SNAKE_LENGTH 4 // Length of the "snake" led pattern

// Function prototype for running the LED task
void run_led_task(LEDs& ledStrip);

#endif // LED_TASK_H
