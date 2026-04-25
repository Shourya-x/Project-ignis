#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Function declarations
void initLED();
void setLEDColor(uint8_t r, uint8_t g, uint8_t b);
void setLEDColor(uint32_t color);
void setLEDBlink(uint8_t r, uint8_t g, uint8_t b, int interval);
void setLEDBlink(uint32_t color, int interval);
void updateLEDStatus(float pitch, float roll, bool sensorsOK);
void indicateStartup();
void indicateError(uint8_t errorCode);
void indicateSuccess();

// Pre-defined colors
extern uint32_t COLOR_OFF;
extern uint32_t COLOR_RED;
extern uint32_t COLOR_GREEN;
extern uint32_t COLOR_BLUE;
extern uint32_t COLOR_YELLOW;
extern uint32_t COLOR_CYAN;
extern uint32_t COLOR_MAGENTA;
extern uint32_t COLOR_WHITE;

// Add this - so main.cpp can check LED status
extern bool isBlinking;

#endif