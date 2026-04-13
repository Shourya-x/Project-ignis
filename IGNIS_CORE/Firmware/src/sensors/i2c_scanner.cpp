#include "i2c_scanner.h"
#include <Wire.h>

// I2C pins for STM32F411
#define I2C_SDA PB7
#define I2C_SCL PB6

// Known sensor addresses
const uint8_t KNOWN_ADDRESSES[] = {0x68, 0x69, 0x76, 0x77};
const int KNOWN_COUNT = sizeof(KNOWN_ADDRESSES) / sizeof(KNOWN_ADDRESSES[0]);

void scanI2CBus() {
  byte error, address;
  int deviceCount = 0;
  int knownCount = 0;
  
  Serial.println("\n========================================");
  Serial.println("🔥 Scanning I2C Bus...");
  Serial.println("========================================\n");
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      deviceCount++;
      Serial.print("  ✅ Found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // Check if it's a known sensor
      bool isKnown = false;
      for (int i = 0; i < KNOWN_COUNT; i++) {
        if (address == KNOWN_ADDRESSES[i]) {
          isKnown = true;
          knownCount++;
          break;
        }
      }
      
      if (isKnown) {
        if (address == 0x68 || address == 0x69) 
          Serial.print(" -> MPU6500 ✓");
        else if (address == 0x76 || address == 0x77) 
          Serial.print(" -> MS5611 ✓");
      } else {
        Serial.print(" -> Unknown");
      }
      Serial.println();
    }
  }
  
  Serial.println("\n========================================");
  Serial.print("📊 Total devices: ");
  Serial.println(deviceCount);
  Serial.print("🎯 Known IGNIS sensors: ");
  Serial.print(knownCount);
  Serial.println("/2 expected (MPU6500 + MS5611)");
  Serial.println("========================================\n");
  
  if (deviceCount == 0) {
    Serial.println("⚠️  No I2C devices found!");
    Serial.println("   Check wiring and pull-up resistors!\n");
  }
}

void printDeviceInfo() {
  Serial.println("\n📡 IGNIS-CORE I2C Information:");
  Serial.println("   SDA Pin: PB7");
  Serial.println("   SCL Pin: PB6");
  Serial.println("   Expected sensors:");
  Serial.println("     - MPU6500 at 0x68 or 0x69");
  Serial.println("     - MS5611 at 0x76 or 0x77");
  Serial.println("   Pull-up resistors: 4.7kΩ to 3.3V\n");
}