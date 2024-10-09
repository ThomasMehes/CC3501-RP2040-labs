// accelerometer_task.h
#ifndef ACCELEROMETER_TASK_H
#define ACCELEROMETER_TASK_H

#include "drivers/lis3dh.h"
#include "drivers/leds.h"

void run_accelerometer_task(LIS3DH &lis3dh, LEDs &ledStrip);

#endif // ACCELEROMETER_TASK_H
