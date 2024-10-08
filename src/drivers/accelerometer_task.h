#ifndef ACCELEROMETER_TASK_H
#define ACCELEROMETER_TASK_H

#include "lis3dh.h"
#include "leds.h"

// Function to implement a digital spirit level using the LIS3DH and LED strip
void accelerometer_spirit_level(LIS3DH& lis3dh, LEDs& ledStrip);

#endif // ACCELEROMETER_TASK_H
