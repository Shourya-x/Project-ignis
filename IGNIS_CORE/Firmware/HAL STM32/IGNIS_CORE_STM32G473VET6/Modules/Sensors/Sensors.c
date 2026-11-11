#include "Sensors.h"
#include "i2c.h"
#include "USB.h"
#include <stdio.h>
#include <math.h>

void I2C2_ScanBus(void)
{
    char msg[96];
    uint8_t found = 0;

    USB_Print("Scanning I2C2 bus (SCL=PC4, SDA=PA8)...\r\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++)
    {
        if (HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(addr << 1), 2, 10) == HAL_OK)
        {
            const char *name = "Unknown";
            if (addr == 0x68 || addr == 0x69) name = "MPU6500 IMU";
            else if (addr == 0x77 || addr == 0x76) name = "MS5611 Barometer";
            else if (addr == 0x1E) name = "HMC5883L Magnetometer";
            else if (addr == 0x0D) name = "QMC5883L Magnetometer";
            else if (addr >= 0x50 && addr <= 0x57) name = "24LC16B EEPROM";

            snprintf(msg, sizeof(msg), "  [FOUND] 0x%02X -> %s\r\n", addr, name);
            USB_Print(msg);
            found++;
        }
    }

    if (found == 0)
    {
        USB_Print("  [WARN] No I2C devices responded on I2C2!\r\n");
    }
}

void HMC5883L_Init(void)
{
    uint8_t id[3] = {0};
    char msg[64];

    if (HAL_I2C_Mem_Read(&hi2c2, (HMC5883L_ADDR << 1), HMC5883L_REG_ID_A, 1, id, 3, 50) == HAL_OK)
    {
        snprintf(msg, sizeof(msg), "HMC5883L ID = %c%c%c (expected H43)\r\n", id[0], id[1], id[2]);
        USB_Print(msg);
    }

    /* Config A: 8-sample average, 15 Hz default output rate, normal measurement */
    uint8_t cfgA = 0x70;
    HAL_I2C_Mem_Write(&hi2c2, (HMC5883L_ADDR << 1), HMC5883L_REG_CONFIG_A, 1, &cfgA, 1, 50);

    /* Config B: Gain = 1.3 Ga (±1.3 Gauss, 1090 LSB/Gauss) */
    uint8_t cfgB = 0x20;
    HAL_I2C_Mem_Write(&hi2c2, (HMC5883L_ADDR << 1), HMC5883L_REG_CONFIG_B, 1, &cfgB, 1, 50);

    /* Mode: Continuous measurement mode (0x00) */
    uint8_t mode = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, (HMC5883L_ADDR << 1), HMC5883L_REG_MODE, 1, &mode, 1, 50);
}

void HMC5883L_ReadRaw(int16_t *mx, int16_t *my, int16_t *mz)
{
    uint8_t buf[6] = {0};
    /* Note: HMC5883L register order is X, Z, Y! */
    if (HAL_I2C_Mem_Read(&hi2c2, (HMC5883L_ADDR << 1), HMC5883L_REG_DATA_X_M, 1, buf, 6, 50) == HAL_OK)
    {
        *mx = (int16_t)((buf[0] << 8) | buf[1]);
        *mz = (int16_t)((buf[2] << 8) | buf[3]);
        *my = (int16_t)((buf[4] << 8) | buf[5]);
    }
    else
    {
        *mx = 0;
        *my = 0;
        *mz = 0;
    }
}

float HMC5883L_GetHeadingDeg(void)
{
    int16_t mx, my, mz;
    HMC5883L_ReadRaw(&mx, &my, &mz);
    float heading = atan2f((float)my, (float)mx) * 57.29578f;
    if (heading < 0.0f) heading += 360.0f;
    return heading;
}

void Sensors_Init(void)
{
    I2C2_ScanBus();
    HMC5883L_Init();
}
