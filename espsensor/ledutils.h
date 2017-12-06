
#ifndef LEDUTILS_H
#define LEDUTILS_H

#include <Arduino.h>


#ifdef DISABLE_LEDS

  void turnOnLed() {}
  void turnOffLed() {}
  void blinkLed(int timeoutMs) {
    delay(timeoutMs);  
  }
  void blinkLed2(int timeoutMs) {
    delay(timeoutMs);  
  }

#else

  void turnOnLed() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);;
  }
  
  void turnOffLed() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  
  void blinkLed(int timeoutMs) {
    turnOnLed();
    delay(timeoutMs);
    turnOffLed();
  }
  
  void blinkLed2(int timeoutMs) {
    turnOffLed();
    delay(timeoutMs);
    turnOnLed();
  }

#endif

#endif
