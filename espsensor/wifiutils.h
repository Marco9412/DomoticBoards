
#ifndef WIFIUTILS_H
#define WIFIUTILS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ledutils.h"
#include "mydebug.h"

// WiFi parameters
#define SSID "ssid"
#define PASS "passworddd"

bool connectWifi() {
  turnOnLed();

  WiFi.mode(WIFI_STA);
  #ifdef FIXED_IP
    WiFi.config(IPAddress(192,168,0,230),IPAddress(192,168,0,1),IPAddress(255,255,255,0),IPAddress(192,168,0,1));
    yield();
  #endif
  WiFi.begin(SSID, PASS);
  yield();
  
  volatile int count = 0;
  DEBUG_PRINT("Attempting WiFi connection...\n");
  while ((WiFi.status() != WL_CONNECTED)) {
    delay(250);
    yield();
    blinkLed2(250);
    if (count++ == 20) { // 10 seconds timeout
      turnOffLed();
      DEBUG_PRINT("WiFi connection timed-out!\n");
      yield();
      return false;
    }
  }
  DEBUG_PRINT("WiFi connected\n");

  turnOffLed();
  yield();
  return true;
}

void disconnectWifi() {
  DEBUG_PRINT("Shutting off WiFi...\n");
  WiFi.disconnect(true); // disconnect AND disable wifi radio
  yield();
}

#endif
