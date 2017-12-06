
#ifndef DOMOTIC_H
#define DOMOTIC_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "mydebug.h"

#define CANCELLO_PIN 0
#define BASCULANTE_PIN 2
#define TRIGGER_DELAY 800

// *******************************************************************
#define TOPIC_CANCELLO_APRI "cantina/cancello/apri"
#define TOPIC_CANCELLO_TRIGGER "cantina/cancello/trigger"
#define TOPIC_BASCULANTE_APRI "cantina/basculante/apri"
#define TOPIC_BASCULANTE_TRIGGER "cantina/basculante/trigger"
#define TOPIC_BASCULANTE_CHIUDI "cantina/basculante/chiudi"
#define TOPIC_BASCULANTE_STATO "cantina/basculante/stato"
#define PAYLOAD_BASCULANTE_APERTO "aperto"
#define PAYLOAD_BASCULANTE_CHIUSO "chiuso"
// *******************************************************************

#define APERTO 1
#define CHIUSO 0
#define SCONOSCIUTO -1
int basculante_stato = SCONOSCIUTO;


static void triggerPin(int pin) {
  digitalWrite(pin, LOW);
  delay(TRIGGER_DELAY);
  digitalWrite(pin, HIGH);
}

bool isBasculanteAperto() {
  return basculante_stato == APERTO;
}

bool isBasculanteChiuso() {
  return !isBasculanteAperto();
}

void apriCancello() {
  triggerPin(CANCELLO_PIN);
}

void apriBasculante(bool soloSeChiuso) {
  if (!soloSeChiuso || isBasculanteChiuso())
    triggerPin(BASCULANTE_PIN);
}

void apriBasculante() {
  apriBasculante(true);
}

void chiudiBasculante() {
  if (isBasculanteAperto())
    triggerPin(BASCULANTE_PIN);
}

void esegui(char* topic, char* message) {
  blinkLed(500);  
  if (strcmp(topic,TOPIC_CANCELLO_APRI) == 0 || strcmp(topic, TOPIC_CANCELLO_TRIGGER) == 0) {
    apriCancello();
  } else if (strcmp(topic, TOPIC_BASCULANTE_APRI) == 0) {
    apriBasculante();
  } else if (strcmp(topic, TOPIC_BASCULANTE_TRIGGER) == 0) {
    apriBasculante(false);
  } else if (strcmp(topic, TOPIC_BASCULANTE_CHIUDI) == 0) {
    chiudiBasculante();
  } else if (strcmp(topic, TOPIC_BASCULANTE_STATO) == 0) {
    if (strcmp(message, PAYLOAD_BASCULANTE_APERTO) == 0) {
      basculante_stato = APERTO;
    } else if (strcmp(message, PAYLOAD_BASCULANTE_CHIUSO) == 0) {
      basculante_stato = CHIUSO;
    }
  } else {
    char data[100];
    sprintf(data, "Invalid topic found! (%s)\n", topic);
    DEBUG_PRINT(data);
  }
}

void checkBasculante(PubSubClient& client, int* state) {
    bool curState = isBasculanteAperto();
    if (curState != *state) {
        blinkLed(500);
        // notify app
        *state = curState;
        DEBUG_PRINT("Basculante is ");
        if (*state) {
          DEBUG_PRINT("opened! Publishing... ");          
          client.publish(TOPIC_BASCULANTE_STATO, PAYLOAD_BASCULANTE_APERTO, true);
          DEBUG_PRINT("OK\n");
        } else {
          DEBUG_PRINT("closed! Publishing... ");
          client.publish(TOPIC_BASCULANTE_STATO, PAYLOAD_BASCULANTE_CHIUSO, true);
          DEBUG_PRINT("OK\n");
        }
    }
}

#endif
