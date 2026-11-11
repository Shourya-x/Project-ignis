#ifndef IMU_H
#define IMU_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MPU6500_ADDR         0x68
#define MPU6500_WHO_AM_I     0x75
#define MPU6500_PWR_MGMT_1   0x6B
#define MPU6500_ACCEL_CONFIG 0x1C
#define MPU6500_GYRO_CONFIG  0x1B
#define MPU6500_ACCEL_XOUT_H 0x3B
#define MPU6500_GYRO_XOUT_H  0x43

void IMU_Init(void);
uint8_t IMU_ReadByte(uint8_t reg);
void IMU_ReadRaw(int16_t *ax, int16_t *ay, int16_t *az,
                 int16_t *gx, int16_t *gy, int16_t *gz);
void IMU_Calibrate(void);
void IMU_GetScaled(float *ax, float *ay, float *az,
                   float *gx, float *gy, float *gz);

#ifdef __cplusplus
}
#endif

#endif /* IMU_H */
