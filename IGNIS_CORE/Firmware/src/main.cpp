#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// NeoPixel pin
#define NEOPIXEL_PIN PB8
#define NUM_PIXELS 1

Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("\n🔥 NeoPixel LED Test 🔥");
  
  strip.begin();
  strip.setBrightness(50);
  strip.show();
  
  Serial.println("Testing LED colors...");
}

void loop() {
  Serial.println("RED");
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.show();
  delay(1000);
  
  Serial.println("GREEN");
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.show();
  delay(1000);
  
  Serial.println("BLUE");
  strip.setPixelColor(0, strip.Color(0, 0, 255));
  strip.show();
  delay(1000);
  
  Serial.println("OFF");
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  delay(1000);
}