#include "mpu6500.h"
#include <MPU6500_WE.h>
#include <Wire.h>

// Create MPU6500 object
MPU6500_WE mpu6500 = MPU6500_WE(&Wire, 0x68);

// Define variables
float accelX = 0, accelY = 0, accelZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
float pitch = 0, roll = 0, yaw = 0;
float compPitch = 0, compRoll = 0, compYaw = 0;
float alpha = 0.98;
unsigned long lastMPUReadTime = 0;

bool initMPU6500() {
  Serial.println("Initializing MPU6500...");
  
  if (!mpu6500.init()) {
    Serial.println("❌ MPU6500 not found!");
    return false;
  }
  
  Serial.println("✅ MPU6500 found!");
  
  // Configure MPU6500
  mpu6500.autoOffsets();
  mpu6500.enableGyrDLPF();
  mpu6500.setGyrDLPF(MPU6500_DLPF_6);
  mpu6500.setSampleRateDivider(5);
  mpu6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  mpu6500.setAccRange(MPU6500_ACC_RANGE_2G);
  mpu6500.enableAccDLPF(true);
  mpu6500.setAccDLPF(MPU6500_DLPF_6);
  
  lastMPUReadTime = micros();
  Serial.println("✅ MPU6500 configured!");
  return true;
}

void readMPU6500() {
  unsigned long now = micros();
  float dt = (now - lastMPUReadTime) / 1000000.0;
  if (dt > 0.1) dt = 0.01;
  if (dt < 0.0001) dt = 0.01;
  lastMPUReadTime = now;
  
  // Read accelerometer
  xyzFloat accRaw = mpu6500.getAccRawValues();
  accelX = accRaw.x;
  accelY = accRaw.y;
  accelZ = accRaw.z;
  
  // Read gyroscope (degrees per second)
  xyzFloat gyroRaw = mpu6500.getGyrValues();
  gyroX = gyroRaw.x;
  gyroY = gyroRaw.y;
  gyroZ = gyroRaw.z;
  
  // Calculate accelerometer angles (in degrees)
  float accPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180 / PI;
  float accRoll = atan2(accelY, accelZ) * 180 / PI;
  float accYaw = atan2(-accelX, accelZ) * 180 / PI;
  
  // Integrate gyroscope
  pitch += gyroY * dt;
  roll += gyroX * dt;
  yaw += gyroZ * dt;
  
  // Complementary filter
  compPitch = alpha * (compPitch + gyroY * dt) + (1 - alpha) * accPitch;
  compRoll = alpha * (compRoll + gyroX * dt) + (1 - alpha) * accRoll;
  compYaw = alpha * (compYaw + gyroZ * dt) + (1 - alpha) * accYaw;
}

void calibrateMPU6500() {
  Serial.println("Calibrating MPU6500...");
  mpu6500.autoOffsets();
  delay(100);
  
  // Reset angles
  pitch = 0;
  roll = 0;
  yaw = 0;
  compPitch = 0;
  compRoll = 0;
  compYaw = 0;
  
  Serial.println("✅ Calibration complete!");
}