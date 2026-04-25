#ifndef MPU6500_H
#define MPU6500_H

#include <Arduino.h>

// Function declarations
bool initMPU6500();
void readMPU6500();
void calibrateMPU6500();

// Variable declarations
extern float accelX, accelY, accelZ;
extern float gyroX, gyroY, gyroZ;
extern float pitch, roll, yaw;
extern float compPitch, compRoll, compYaw;

#endif