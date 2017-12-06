
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <EEPROM.h>

// Enable Esp8266 SDK functions
#ifdef ESP8266
extern "C" {
  #include "user_interface.h"
}
#endif

// *******************************************************************
#define VERBOSE
//#define MEASURE_TIME
//#define MQTT_SSL
//#define DISABLE_LEDS
#define FIXED_IP
// *******************************************************************

// Publishing times
// With fixed IP -> 4.2 secs
// With DHCP     -> 5.1 secs

// *******************************************************************
#define BASCULANTE_IN_PIN     D1      // GPIO 5, no problems!
//#define SENSOR_REQUIRES_GND
#define SENSOR_GND_PIN        14
#define SLEEP_TIME_MS         5000
#define EEPROM_ADDRESS        0
// *******************************************************************

#include "wifiutils.h"
#include "ledutils.h"
#include "mydebug.h"
#include "mqttutils.h"
#include "resetutils.h"

// *******************************************************************
#define OPENED 1
#define CLOSED 0
int basculanteState;
#ifdef MEASURE_TIME
  int publish_time;
#endif
// *******************************************************************

// *******************************************************************
#ifdef MQTT_SSL
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif

PubSubClient client(espClient);
// *******************************************************************

void setup() {
  // Communicating with host
  SERIAL_INIT(115200);
  DEBUG_PRINT("\nBasculante Reader Sleep\nMarco Panato\n2017/07/02\n");
  DEBUG_PRINT("Initializing board...\n");

  // Initializing variables
  pinMode(BASCULANTE_IN_PIN, INPUT_PULLUP);
  #ifdef SENSOR_REQUIRES_GND
    pinMode(SENSOR_GND_PIN, OUTPUT);
    digitalWrite(SENSOR_GND_PIN, LOW);
  #endif

  yield();

  if (isAwakeFromDeepSleep()) {
    DEBUG_PRINT("Wake from deep sleep boot\n");
    EEPROM.begin(4); // minimum storage
    loadBasculantePreviousState();
  } else {
    DEBUG_PRINT("First boot\n");
    basculanteState = -1;
  }

  yield();
  
  DEBUG_PRINT("Checking basculante\n");
  checkBasculante();

  yield();

  DEBUG_PRINT("All done!");
  espDeepSleepMs(SLEEP_TIME_MS);
}

void loop() {
}

void checkBasculante() {
  volatile int stato = digitalRead(BASCULANTE_IN_PIN) == HIGH ? OPENED : CLOSED;
  if (basculanteState != stato) {
    basculanteState = stato;
    basculanteChanged();
  }
}

void basculanteChanged() {
  DEBUG_PRINT("Basculante is ");
  DEBUG_PRINT(basculanteState ? "opened\n" : "closed\n");

  startMeasuring();
  if (connectWifi()) {
    publishValue();
    disconnectWifi();    
  } else {
    basculanteState = -1; // error, should republish
  }  
  stopMeasuring();

  saveState();
}

void publishValue() {
  if (mqttConnect(client)) {
    if (!mqttPublish(client, basculanteState)) {
      DEBUG_PRINT("Cannot publish!\n");
      basculanteState = -1; // error, should republish!
    } else {      
      DEBUG_PRINT("Published!\n");
    }
    mqttDisconnect(client);
  }
}

// *******************************************************************
// Publish time utility
// *******************************************************************
inline void startMeasuring() {
  #ifdef MEASURE_TIME
    publish_time = millis();
  #endif
}

inline void stopMeasuring() {
  #ifdef MEASURE_TIME
    volatile int t_now = millis();
    DEBUG_PRINT("Publishing took ");
    DEBUG_PRINT(t_now - publish_time);
    DEBUG_PRINT("ms\n");
  #endif
}

// *******************************************************************
// EEPROM Utility
// *******************************************************************
inline void loadBasculantePreviousState() {
  DEBUG_PRINT("Loading previous state: ");
  volatile int st = EEPROM.read(EEPROM_ADDRESS);
  if (st == 0 || st == 1) {
    basculanteState = st;
    if (st == 1) DEBUG_PRINT("opened\n");
    else         DEBUG_PRINT("closed\n");
  } else {
    basculanteState = -1;
    DEBUG_PRINT("unknown\n");
  }
}

inline void saveState() {
  EEPROM.write(EEPROM_ADDRESS, basculanteState);
  EEPROM.commit();
}

