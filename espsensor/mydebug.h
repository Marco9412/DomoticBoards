/*
 * Debug functions
 */

#ifndef MYDEBUG_H
#define MYDEBUG_H

#include <Arduino.h>

#ifdef VERBOSE
    #define SERIAL_INIT(baud) Serial.begin(baud)
    #define DEBUG_PRINT(x,...) Serial.print(x, ##__VA_ARGS__)
#else
    #define SERIAL_INIT(baud)
    #define DEBUG_PRINT(x,...)
#endif


#endif
