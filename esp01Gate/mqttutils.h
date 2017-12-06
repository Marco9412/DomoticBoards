
#ifndef MQTTUTILS_H
#define MQTTUTILS_H

#include "wifiutils.h"
#include "mydebug.h"
#include "ledutils.h"
#include "domotic.h"

#define MQTT_BROKER_HOST "192.168.0.239"
#ifdef MQTT_SSL
  #define MQTT_BROKER_PORT 8883
#else
  #define MQTT_BROKER_PORT 1883
#endif

#define HOST_ID "esp01_cancello_client"
#define USERNAME "user"
#define PASSWORD "password"

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    char* message = (char*) payload;
    message[length] = '\0';
    DEBUG_PRINT("MQTT Message (");
    DEBUG_PRINT(topic);
    DEBUG_PRINT(",");
    DEBUG_PRINT(message);
    DEBUG_PRINT(")\n");
    esegui(topic, message);
}

void mqttConnect(PubSubClient& client) {
    DEBUG_PRINT("Attempting MQTT connection... ");
    
    while (!client.connect(HOST_ID, USERNAME, PASSWORD)) {
        DEBUG_PRINT("Cannot connect! Probably WiFi isn't connected!\n");
        DEBUG_PRINT("MQTT State -> ");
        DEBUG_PRINT(client.state());
        DEBUG_PRINT("\n");
        checkWifi();

        blinkLed(500);
        yield();
    }
    DEBUG_PRINT("\nMQTT connected! Subscribing... ");
    client.subscribe(TOPIC_CANCELLO_APRI);
    client.subscribe(TOPIC_CANCELLO_TRIGGER);
    client.subscribe(TOPIC_BASCULANTE_APRI);
    client.subscribe(TOPIC_BASCULANTE_TRIGGER);
    client.subscribe(TOPIC_BASCULANTE_CHIUDI);    
    client.subscribe(TOPIC_BASCULANTE_STATO);
    
    DEBUG_PRINT("Ok\n");

    turnOnLed();
    delay(500);
    turnOffLed();
}

void checkMQTT(PubSubClient& client) {
    yield();
    if (!client.connected()) {
        DEBUG_PRINT("MQTT Client disconnected!\n");
        mqttConnect(client);
    }

    client.loop();
}

#endif
