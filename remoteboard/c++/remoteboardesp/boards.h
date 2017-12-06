/*
 * Boards params
 *
 * BOARD_TOPIC
 *    The topic of this board (used in commands)
 *
 * RELAY_INVERTED
 *    Tells whether the power is connected to NC pin of the relay.
 *      Connect it to:
 *        NC: this device will be ON for the most of the time.
 *        NO: this device will be OFF for the most of the time.
 *
 * ENABLE_GND_BUTTON
 *    Enables GND pin for button pushes
 *
 * BUTTON_TWO_STATES
 *    Button as switch or button as a two-state
 *
 */

// ONCE a TIME!

#ifndef BOARDS_H
#define BOARDS_H

//#define LAMPADASTUDIO
//#define WEMOS_LAMPADASTUDIO // IP static!

#ifdef LAMPADASTUDIO
  #define BOARD_TOPIC "studio/lampada"
  #define RELAYPIN 2
  #define ESP01
  //#define LEDPIN 1 // using LED_BUILTIN of ESP_01 //USING BUILTIN
  #define BUTTONPIN 0 // RX
  //#define BUTTONPINGND 0
  #define EEPROM_ADDRESS 0
  #define MQTT_HOST_ID "esp01_lampada_client"
  #define MQTT_BROKER_HOST "192.168.0.239"
#endif

#ifdef WEMOS_LAMPADASTUDIO
  //#define VERBOSE

  #define RELAYPIN D5
  #define BUTTONPIN D6
  #define BUTTONPINGND D7
  #define ENABLE_GND_BUTTON

  #define EEPROM_ADDRESS 0

  #define BOARD_TOPIC "studio/lampada"
  //#define MQTT_SSL
  #define MQTT_HOST_ID "espwemos_lampada_studio_client"
  #define MQTT_BROKER_HOST "192.168.0.239"

  #define ENABLE_OTA
  #define OTA_PATH "/update"
  #define OTA_USERNAME "admin"
  #define OTA_PASSWORD "passworddddddd"
  #define OTA_HOSTNAME "espwemos_lampada_studio_ota"

  #define BOARD_IP IPAddress(192, 168, 0, 205)
#endif

#endif
