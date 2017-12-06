
#ifndef LEDUTILS_H
#define LEDUTILS_H

#include <Arduino.h>

// Only use with LED_BUILTIN
// Available only if Serial is disabled on ESP01

void turnOnLed();
void turnOffLed();
void blinkLed(int timeoutMs);

#endif
