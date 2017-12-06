
#ifndef WIFIUTILS_H
#define WIFIUTILS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "boards.h"

// WiFi parameters
#define MY_SSID "ssid"
#define MY_PASS "password"

#define CONN_TIME_MS 10000

void connectWifi();
void myCheckWifi();
bool isWifiOk();

#endif
