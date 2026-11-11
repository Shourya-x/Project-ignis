#ifndef BAROMETER_H
#define BAROMETER_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MS5611_ADDR          0x77
#define MS5611_RESET         0x1E
#define MS5611_READ_ADC      0x00
#define MS5611_CONV_D1       0x40
#define MS5611_CONV_D2       0x50

void Barometer_Init(void);
void Barometer_ReadCalibration(void);
void Barometer_Read(int32_t *temp, int32_t *pres);

#ifdef __cplusplus
}
#endif

#endif /* BAROMETER_H */
