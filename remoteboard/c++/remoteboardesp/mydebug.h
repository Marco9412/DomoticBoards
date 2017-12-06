/*
 * Debug functions
 */

#include "boards.h"

#ifndef DEBUG_H
#define DEBUG_H

#ifdef VERBOSE
  #define SERIAL_INIT(x) Serial.begin(x)
  #define DEBUG_PRINT(x,...) Serial.print(x, ##__VA_ARGS__)
  #define PRINTF(x,...) Serial.printf(x, ##__VA_ARGS__)
#else
  #define SERIAL_INIT(x)
  #define DEBUG_PRINT(x,...)
  #define PRINTF(x,...)
#endif

#endif
