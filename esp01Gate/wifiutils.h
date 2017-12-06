
#ifndef WIFIUTILS_H
#define WIFIUTILS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ledutils.h"
#include "mydebug.h"

// WiFi parameters
#define SSID "ssid"
#define PASS "password"

void connectWifi() {
  turnOnLed();

  WiFi.begin(SSID, PASS);
  while ((WiFi.status() != WL_CONNECTED )) {
    delay(1000);
    DEBUG_PRINT("Attempting WiFi connection...\n");

    turnOffLed();
    delay(500);
    turnOnLed();
    delay(500);
  }
  DEBUG_PRINT("WiFi connected: ");
  DEBUG_PRINT(WiFi.localIP());
  DEBUG_PRINT("\n");

  turnOffLed();
}

void checkWifi() {
  yield();
  if (WiFi.status() == WL_CONNECTED) {
    return; // all right
  }
  DEBUG_PRINT("System offline!\n");
  connectWifi();
}

#endif
