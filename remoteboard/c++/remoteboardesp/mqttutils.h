
#ifndef MQTTUTILS_H
#define MQTTUTILS_H

#include "boards.h"

#ifdef MQTT_SSL
  #define MQTT_BROKER_PORT 8883
#else
  #define MQTT_BROKER_PORT 1883
#endif

#define USERNAME "username"
#define PASSWORD "password"

#include <PubSubClient.h>

void mqttConnect(PubSubClient& client);
void checkMQTT(PubSubClient& client);

#endif
