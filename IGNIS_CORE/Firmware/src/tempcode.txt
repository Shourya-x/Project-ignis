/*
 * IGNIS-CORE - Main Orchestrator with LED Status
 */

#include <Wire.h>
#include "sensors/i2c_scanner.h"
#include "sensors/mpu6500.h"
#include "sensors/ms5611.h"
#include "output_devices/led_status.h"

// I2C pins
#define I2C_SDA PB7
#define I2C_SCL PB6

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastSerialPrint = 0;

bool sensorsOK = false;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  // Initialize LED first (for visual feedback)
  initLED();
  indicateStartup();
  
  // Initialize I2C bus
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();
  Wire.setClock(400000);
  
  // Print header
  Serial.println("\n\n🔥 IGNIS-CORE System Starting 🔥");
  Serial.println("=================================");
  
  // I2C info
  printDeviceInfo();
  
  // Scan I2C bus
  scanI2CBus();
  
  // Initialize sensors
  Serial.println("\n📡 Initializing Sensors...");
  
  bool mpuOK = initMPU6500();
  bool ms5611OK = initMS5611();
  
  sensorsOK = (mpuOK && ms5611OK);
  
  if (sensorsOK) {
    Serial.println("\n✅ All sensors ready!");
    indicateSuccess();
  } else {
    Serial.println("\n⚠️ Some sensors failed to initialize!");
    indicateError(1);
  }
  
  Serial.println("\n🚀 System ready!");
  Serial.println("Commands:");
  Serial.println("  scan     - Scan I2C bus");
  Serial.println("  status   - Show all sensor readings");
  Serial.println("  calibrate - Calibrate MPU6500");
  Serial.println("  led on   - Turn LED white");
  Serial.println("  led off  - Turn LED off");
  Serial.println("  help     - Show this menu\n");
  
  lastSensorRead = millis();
  lastSerialPrint = millis();
  
  // Final LED indication
  setLEDColor(COLOR_GREEN);  // Solid green = ready
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensors every 10ms (100Hz)
  if (currentTime - lastSensorRead >= 10) {
    readMPU6500();
    readMS5611();
    lastSensorRead = currentTime;
  }
  
  // Update LED based on rocket attitude
  updateLEDStatus(compPitch, compRoll, sensorsOK);
  
  // Print status every 1 second
  if (currentTime - lastSerialPrint >= 1000) {
    float altAGL = altitude - groundAltitude;
    
    Serial.print("📊 P:");
    Serial.print(compPitch, 1);
    Serial.print("° R:");
    Serial.print(compRoll, 1);
    Serial.print("° Y:");
    Serial.print(compYaw, 1);
    Serial.print("° | T:");
    Serial.print(temperature, 1);
    Serial.print("°C | A:");
    Serial.print(altAGL, 1);
    Serial.println("m");
    
    lastSerialPrint = currentTime;
  }
  
  // Handle serial commands
  if (Serial.available()) {
    String cmd = Serial.readString();
    cmd.toLowerCase();
    cmd.trim();
    
    if (cmd == "scan") {
      scanI2CBus();
    }
    else if (cmd == "status") {
      float altAGL = altitude - groundAltitude;
      
      Serial.println("\n--- IGNIS-CORE Status ---");
      Serial.print("  📊 Attitude: P:");
      Serial.print(compPitch, 1);
      Serial.print("° R:");
      Serial.print(compRoll, 1);
      Serial.print("° Y:");
      Serial.print(compYaw, 1);
      Serial.println("°");
      Serial.print("  🌡️ Temperature: ");
      Serial.print(temperature, 1);
      Serial.println("°C");
      Serial.print("  📊 Pressure: ");
      Serial.print(pressure, 0);
      Serial.println(" mbar");
      Serial.print("  🏔️ Altitude: ");
      Serial.print(altAGL, 1);
      Serial.println(" m AGL");
      Serial.print("  💡 LED Status: ");
      if (isBlinking) {
        Serial.println("Blinking");
      } else {
        Serial.println("Solid");
      }
      Serial.println("------------------------\n");
    }
    else if (cmd == "calibrate") {
      setLEDBlink(COLOR_YELLOW, 500);
      calibrateMPU6500();
      setLEDColor(COLOR_GREEN);
    }
    else if (cmd == "led on") {
      setLEDColor(COLOR_WHITE);
      Serial.println("LED turned ON");
    }
    else if (cmd == "led off") {
      setLEDColor(COLOR_OFF);
      Serial.println("LED turned OFF");
    }
    else if (cmd == "help") {
      Serial.println("\nCommands:");
      Serial.println("  scan      - Scan I2C bus");
      Serial.println("  status    - Show all sensor readings");
      Serial.println("  calibrate - Calibrate MPU6500");
      Serial.println("  led on    - Turn LED white");
      Serial.println("  led off   - Turn LED off");
      Serial.println("  help      - Show this menu\n");
    }
  }
  
  delay(5);
}