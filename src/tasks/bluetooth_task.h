// bluetooth_task.h

#ifndef BLUETOOTH_TASK_H
#define BLUETOOTH_TASK_H

// Include the necessary headers for LIS3DH and LEDs
#include "drivers/lis3dh.h"  // Include the LIS3DH accelerometer class
#include "drivers/leds.h"    // Include the LEDs control class

// Function declaration with parameters
void run_bluetooth_task();

#endif // BLUETOOTH_TASK_H
