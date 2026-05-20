#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------------- Pin Definitions ----------------
// LEDs & Indicators
#define LED_PIN PC13       // Onboard LED (active low)
#define NEOPIXEL_PIN PB8   // WS2812 NeoPixel data pin
#define BUZZER_PIN PA8

// Servos (for future TVC)
#define X_SERVO_PIN PA0
#define Y_SERVO_PIN PA1

// SD Card (SPI1)
#define SD_CS PA4

// I2C (for MPU6500 & MS5611)
#define I2C_SDA PB7
#define I2C_SCL PB6

// GPS (UART1)
#define GPS_SERIAL Serial1
#define GPS_BAUD 9600

// ---------------- Sensor Addresses ----------------
#define MS5611_ADDRESS 0x77
#define MPU6500_ADDRESS 0x68

// ---------------- Filter Constants ----------------
#define COMPLEMENTARY_ALPHA 0.96f
#define LOOP_DELAY_MS 10

// ---------------- Servo Limits ----------------
#define SERVO_MIN 60
#define SERVO_MAX 120
#define SERVO_CENTER 90

// ---------------- Altitude Thresholds (meters) ----------------
#define ALTITUDE_LOW 20.0f
#define ALTITUDE_MEDIUM 50.0f
#define ALTITUDE_HIGH 100.0f

#endif