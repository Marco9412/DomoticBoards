
#include "ledutils.h"
#include "boards.h"
#include "mydebug.h"

void turnOnLed() {
  DEBUG_PRINT("turnOnLed()\n");
  #if defined(ESP01) && ! defined(VERBOSE)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  #endif
}

void turnOffLed() {
  DEBUG_PRINT("turnOnLed()\n");
  #if defined(ESP01) && ! defined(VERBOSE)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  #endif
}

void blinkLed(int timeoutMs) {
  PRINTF("blinkLed(%d)\n", timeoutMs);
  turnOnLed();
  delay(timeoutMs);
  turnOffLed();
}
