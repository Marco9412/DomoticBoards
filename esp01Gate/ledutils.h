
#ifndef LEDUTILS_H
#define LEDUTILS_H

#include <Arduino.h>

// Only use with LED_BUILTIN
// Available only if Serial is disabled on ESP01

void turnOnLed() {
  #ifndef VERBOSE
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  #endif
}

void turnOffLed() {
  #ifndef VERBOSE
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  #endif
}

void blinkLed(int timeoutMs) {
  #ifndef VERBOSE
  turnOnLed();
  delay(timeoutMs);
  turnOffLed();
  #endif
}

#endif
