#include "Barometer.h"
#include "i2c.h"
#include "USB.h"
#include <stdio.h>
#include <string.h>

static uint16_t C[8];

void Barometer_Init(void)
{
    uint8_t reset = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, (MS5611_ADDR << 1), MS5611_RESET, 1, &reset, 1, 100);
    HAL_Delay(10);

    Barometer_ReadCalibration();
}

void Barometer_ReadCalibration(void)
{
    uint8_t buf[2] = {0};
    char msg[64];

    USB_Print("Reading MS5611 calibration coefficients...\r\n");

    for (int i = 1; i <= 6; i++)
    {
        uint8_t cmd = (uint8_t)(0xA0 + (i * 2));
        HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
        HAL_I2C_Master_Receive(&hi2c2, (MS5611_ADDR << 1), buf, 2, 100);
        C[i] = (uint16_t)(((uint16_t)buf[0] << 8) | buf[1]);

        snprintf(msg, sizeof(msg), "  C[%d] = %u\r\n", i, C[i]);
        USB_Print(msg);
    }

    USB_Print("Calibration done.\r\n");
}

void Barometer_Read(int32_t *temp, int32_t *pres)
{
    uint8_t buf[3] = {0};
    uint32_t D1, D2;

    /* Pressure conversion (OSR=4096) */
    uint8_t cmd = MS5611_CONV_D1 + 0x08;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_Delay(10);

    cmd = MS5611_READ_ADC;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_I2C_Master_Receive(&hi2c2, (MS5611_ADDR << 1), buf, 3, 100);
    D1 = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];

    /* Temperature conversion (OSR=4096) */
    cmd = MS5611_CONV_D2 + 0x08;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_Delay(10);

    cmd = MS5611_READ_ADC;
    HAL_I2C_Master_Transmit(&hi2c2, (MS5611_ADDR << 1), &cmd, 1, 100);
    HAL_I2C_Master_Receive(&hi2c2, (MS5611_ADDR << 1), buf, 3, 100);
    D2 = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];

    int32_t dT = (int32_t)(D2 - ((uint32_t)C[5] << 8));
    int32_t TEMP = (int32_t)(2000 + ((int64_t)dT * C[6]) / 8388608);

    int64_t OFF  = (int64_t)C[2] * 65536 + ((int64_t)C[4] * dT) / 128;
    int64_t SENS = (int64_t)C[1] * 32768 + ((int64_t)C[3] * dT) / 256;

    if (TEMP < 2000)
    {
        int32_t T2    = (int32_t)(((int64_t)dT * dT) / 0x800000);
        int64_t OFF2  = 5LL * (TEMP - 2000) * (TEMP - 2000) / 2;
        int64_t SENS2 = 5LL * (TEMP - 2000) * (TEMP - 2000) / 4;

        TEMP -= T2;
        OFF  -= OFF2;
        SENS -= SENS2;
    }

    int64_t P = ((int64_t)D1 * SENS / 2097152 - OFF) / 32768;

    *temp = TEMP;
    *pres = (int32_t)P;
}
