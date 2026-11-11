#ifndef SENSORS_H
#define SENSORS_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== HMC5883L Magnetometer (I2C2) ==================== */
#define HMC5883L_ADDR        0x1E
#define HMC5883L_REG_CONFIG_A 0x00
#define HMC5883L_REG_CONFIG_B 0x01
#define HMC5883L_REG_MODE     0x02
#define HMC5883L_REG_DATA_X_M 0x03
#define HMC5883L_REG_ID_A     0x0A

void Sensors_Init(void);
void I2C2_ScanBus(void);

void HMC5883L_Init(void);
void HMC5883L_ReadRaw(int16_t *mx, int16_t *my, int16_t *mz);
float HMC5883L_GetHeadingDeg(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSORS_H */
