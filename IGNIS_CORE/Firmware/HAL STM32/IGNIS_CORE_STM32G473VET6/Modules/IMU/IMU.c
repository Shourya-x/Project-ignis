#include "IMU.h"
#include "i2c.h"
#include "USB.h"
#include <stdio.h>
#include <string.h>

static int16_t accel_offset[3] = {0, 0, 0};
static int16_t gyro_offset[3]  = {0, 0, 0};

void IMU_Init(void)
{
    uint8_t whoami = IMU_ReadByte(MPU6500_WHO_AM_I);
    char buf[64];

    snprintf(buf, sizeof(buf), "MPU6500 WHO_AM_I = 0x%02X (expected 0x70)\r\n", whoami);
    USB_Print(buf);

    /* Wake up MPU6500 (disable sleep mode) */
    uint8_t data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, (MPU6500_ADDR << 1), MPU6500_PWR_MGMT_1, 1, &data, 1, 100);

    /* Configure accelerometer (±8g = 4096 LSB/g) */
    data = 0x10;
    HAL_I2C_Mem_Write(&hi2c2, (MPU6500_ADDR << 1), MPU6500_ACCEL_CONFIG, 1, &data, 1, 100);

    /* Configure gyroscope (±1000°/s = 32.8 LSB/(°/s)) */
    data = 0x10;
    HAL_I2C_Mem_Write(&hi2c2, (MPU6500_ADDR << 1), MPU6500_GYRO_CONFIG, 1, &data, 1, 100);
}

uint8_t IMU_ReadByte(uint8_t reg)
{
    uint8_t data = 0;
    HAL_I2C_Mem_Read(&hi2c2, (MPU6500_ADDR << 1), reg, 1, &data, 1, 100);
    return data;
}

void IMU_ReadRaw(int16_t *ax, int16_t *ay, int16_t *az,
                 int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t buf[14] = {0};
    HAL_I2C_Mem_Read(&hi2c2, (MPU6500_ADDR << 1), MPU6500_ACCEL_XOUT_H, 1, buf, 14, 100);

    *ax = (int16_t)((buf[0] << 8) | buf[1]);
    *ay = (int16_t)((buf[2] << 8) | buf[3]);
    *az = (int16_t)((buf[4] << 8) | buf[5]);
    *gx = (int16_t)((buf[8] << 8) | buf[9]);
    *gy = (int16_t)((buf[10] << 8) | buf[11]);
    *gz = (int16_t)((buf[12] << 8) | buf[13]);
}

void IMU_Calibrate(void)
{
    int32_t sum_accel[3] = {0, 0, 0};
    int32_t sum_gyro[3]  = {0, 0, 0};
    const int samples = 500;
    char buf[100];

    for (int i = 0; i < samples; i++)
    {
        int16_t ax, ay, az, gx, gy, gz;
        IMU_ReadRaw(&ax, &ay, &az, &gx, &gy, &gz);
        sum_accel[0] += ax;
        sum_accel[1] += ay;
        sum_accel[2] += az;
        sum_gyro[0]  += gx;
        sum_gyro[1]  += gy;
        sum_gyro[2]  += gz;
        HAL_Delay(2);
    }

    accel_offset[0] = (int16_t)(sum_accel[0] / samples);
    accel_offset[1] = (int16_t)(sum_accel[1] / samples);
    accel_offset[2] = (int16_t)((sum_accel[2] / samples) - 4096);  /* Expect 1g (4096 LSB at ±8g) on Z */

    gyro_offset[0] = (int16_t)(sum_gyro[0] / samples);
    gyro_offset[1] = (int16_t)(sum_gyro[1] / samples);
    gyro_offset[2] = (int16_t)(sum_gyro[2] / samples);

    snprintf(buf, sizeof(buf), "Offsets: A(%d,%d,%d) G(%d,%d,%d)\r\n",
             accel_offset[0], accel_offset[1], accel_offset[2],
             gyro_offset[0], gyro_offset[1], gyro_offset[2]);
    USB_Print(buf);
}

void IMU_GetScaled(float *ax, float *ay, float *az,
                   float *gx, float *gy, float *gz)
{
    int16_t rax, ray, raz, rgx, rgy, rgz;

    IMU_ReadRaw(&rax, &ray, &raz, &rgx, &rgy, &rgz);

    rax -= accel_offset[0];
    ray -= accel_offset[1];
    raz -= accel_offset[2];
    rgx -= gyro_offset[0];
    rgy -= gyro_offset[1];
    rgz -= gyro_offset[2];

    /* Accel: ±8g = 4096 LSB/g */
    *ax = rax / 4096.0f;
    *ay = ray / 4096.0f;
    *az = raz / 4096.0f;

    /* Gyro: ±1000 deg/s = 32.8 LSB/(deg/s) */
    *gx = rgx / 32.8f;
    *gy = rgy / 32.8f;
    *gz = rgz / 32.8f;
}
