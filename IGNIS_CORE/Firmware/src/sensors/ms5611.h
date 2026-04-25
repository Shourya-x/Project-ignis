#ifndef MS5611_H
#define MS5611_H

#include <Arduino.h>

// Function declarations
bool initMS5611();
void readMS5611();

// Variable declarations
extern float temperature;
extern float pressure;
extern float altitude;
extern float groundAltitude;

#endif