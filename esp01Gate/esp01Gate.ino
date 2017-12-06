
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>



// *******************************************************************
//#define VERBOSE
//#define MQTT_SSL
// *******************************************************************

#include "wifiutils.h"
#include "ledutils.h"
#include "mydebug.h"
#include "domotic.h"
#include "mqttutils.h"

// *******************************************************************
#ifdef MQTT_SSL
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif

PubSubClient client(espClient);
// *******************************************************************

void setup() {
  // First thing! Disable relays
  pinMode(CANCELLO_PIN, OUTPUT);
  pinMode(BASCULANTE_PIN, OUTPUT);
  digitalWrite(CANCELLO_PIN, HIGH);
  digitalWrite(BASCULANTE_PIN, HIGH);

  // Communicating with host
  SERIAL_INIT(115200);
  delay(100);
  DEBUG_PRINT("\nDomotic Controller Esp01\nMarco Panato\n2017/08/01\n");
  DEBUG_PRINT("Initializing board...\n");

  // Initializing variables
  client.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  client.setCallback(mqttCallback);

  DEBUG_PRINT("Switching to Loop\n");

  // Disable Wifi Access Point
  WiFi.mode(WIFI_STA);
}

void loop() {
    checkWifi();
    checkMQTT(client);
    
    delay(1000);
    yield();
}

