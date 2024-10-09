// task_manager.h
#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

// Task identifiers
enum Tasks {
    LED_TASK,
    ACCELEROMETER_TASK,
    MICROPHONE_TASK,
    BLUETOOTH_TASK,
    NUM_TASKS
};

// Global variable to track the current task
extern volatile Tasks current_task;

#endif // TASK_MANAGER_H
