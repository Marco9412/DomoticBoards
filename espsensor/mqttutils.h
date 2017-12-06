
#ifndef MQTTUTILS_H
#define MQTTUTILS_H

#include "wifiutils.h"
#include "mydebug.h"
#include "ledutils.h"

#define MQTT_BROKER_HOST "192.168.0.239"
#ifdef MQTT_SSL
  #define MQTT_BROKER_PORT 8883
#else
  #define MQTT_BROKER_PORT 1883
#endif

#define HOST_ID "esp8266_domotic_bascu_sensor"
#define USERNAME "user"
#define PASSWORD "password"

#define TOPIC_BASCULANTE_STATO "atopic"
#define PAYLOAD_BASCULANTE_APERTO "open_value"
#define PAYLOAD_BASCULANTE_CHIUSO "closed_value"

bool mqttConnect(PubSubClient& client) {
  DEBUG_PRINT("Attempting MQTT connection...\n");
  
  client.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);    
  while (!client.connect(HOST_ID, USERNAME, PASSWORD)) {
      DEBUG_PRINT("Cannot connect!\n");
      DEBUG_PRINT("MQTT State -> ");
      DEBUG_PRINT(client.state());
      DEBUG_PRINT("\n");
      yield();
      return false;
  }
  DEBUG_PRINT("MQTT connected!\n");
  yield();
  return true;
}

void mqttDisconnect(PubSubClient& client) {
  DEBUG_PRINT("MQTT disconnected!\n");
  client.disconnect();
  yield();
}

bool mqttPublish(PubSubClient& client, int stato) {
  blinkLed(500);
  yield();
  return client.publish(TOPIC_BASCULANTE_STATO, (stato == 1 ? PAYLOAD_BASCULANTE_APERTO : PAYLOAD_BASCULANTE_CHIUSO), true);
}

#endif
