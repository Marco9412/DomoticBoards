
#include "wifiutils.h"
#include "ledutils.h"
#include "mydebug.h"

void connectWifi() {
  static long lastConnTime = -1;
  volatile long now = millis();
  if (lastConnTime != -1 && (now - lastConnTime) < CONN_TIME_MS) return;
  
  DEBUG_PRINT("System offline!\n");
  DEBUG_PRINT("Attempting WiFi connection...\n");
  #ifdef BOARD_IP
    WiFi.config(BOARD_IP,IPAddress(192,168,0,1),IPAddress(255,255,255,0),IPAddress(192,168,0,1));
    yield();
  #endif
  
  lastConnTime = millis();
  WiFi.begin(MY_SSID, MY_PASS);
  yield();  
}

void myCheckWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return; // all right
  }
  connectWifi();
}

bool isWifiOk() {
  yield();
  return WiFi.status() == WL_CONNECTED;
}
