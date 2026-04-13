#include "ms5611.h"
#include <MS5611.h>
#include <Wire.h>

// Create MS5611 object (address 0x77)
MS5611 ms5611(0x77);

// Define variables
float temperature = 0;
float pressure = 0;
float altitude = 0;
float groundAltitude = 0;

bool initMS5611() {
  Serial.println("Initializing MS5611...");
  
  if (!ms5611.begin()) {
    Serial.println("❌ MS5611 not found!");
    return false;
  }
  
  Serial.println("✅ MS5611 found!");
  
  // Calibrate ground altitude (average 10 readings)
  Serial.println("Calibrating ground altitude...");
  float sum = 0;
  for (int i = 0; i < 10; i++) {
    ms5611.read();
    sum += ms5611.getAltitude();
    delay(100);
  }
  groundAltitude = sum / 10;
  
  Serial.print("✅ Ground altitude: ");
  Serial.print(groundAltitude);
  Serial.println(" meters");
  
  return true;
}

void readMS5611() {
  ms5611.read();
  temperature = ms5611.getTemperature();
  pressure = ms5611.getPressure();
  altitude = ms5611.getAltitude();
}