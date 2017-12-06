
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <EEPROM.h>
#include <AsyncDelay.h>
#include <ArduinoJson.h>


//#define TEST_MODE

#include "boards.h"
#include "mydebug.h"
#include "commands.h"
#include "timeutils.h"
#include "wifiutils.h"
#include "ledutils.h"
#include "mqttutils.h"
#include "otautils.h"

// *****************************************************************************
#ifdef MQTT_SSL
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif

PubSubClient client(espClient);
// *****************************************************************************

// *****************************************************************************
//  State of the board
// *****************************************************************************
#define STATE_ON 1
#define STATE_OFF 0
int state;

// *****************************************************************************
//  Button data
// *****************************************************************************
#define OPEN 0
#define CLICKED 1
#define UNINITIALIZED 2
#define BUTTON_DEBOUNCE_TIME 1000
#ifdef BUTTON_TWO_STATES
  int lastButtonState;
#else
  AsyncDelay lastClickDelay;
#endif

// *****************************************************************************
//  Timer works
// *****************************************************************************
bool timedSwitchActive;
AsyncDelay timedSwitchDelay;


// *****************************************************************************
//  Time utils
// *****************************************************************************
MyTimeUtils myTimeUtils;

// *****************************************************************************
//  Ota utils
// *****************************************************************************
OtaHttpUpdater otaHttpUpdater;

// *****************************************************************************
//  ESP initialization
// *****************************************************************************
void setup() {
  // Communicating with host
  SERIAL_INIT(115200);
  DEBUG_PRINT("\nDomotic Controller Esp8266\nMarco Panato\n2017/08/09\n");
  DEBUG_PRINT("Initializing board...\n");
  yield();

  // Initializing variables
  client.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  client.setCallback(mqttCallback);

  setPins();
  resetDelays();

  // Disable Wifi Access Point
  WiFi.mode(WIFI_STA);

  DEBUG_PRINT("Switching to Loop\n");
}

void loop() {
    myCheckWifi();
    myTimeUtils.callMeInLoop(); // NTP update
    if (isWifiOk())
        checkMQTT(client);
    checkButton();
    checkTimedEvent();
    otaHttpUpdater.loop();

    yield();
    delay(10);
}

// *****************************************************************************
//  INIT Things
// *****************************************************************************
void resetDelays() {
    #ifndef BUTTON_TWO_STATES
      lastClickDelay.expire();
    #endif

    // Timer switch time
    timedSwitchActive = false;
    timedSwitchDelay.expire();
    DEBUG_PRINT("Relays ok\n");
}

void setPins() {
    // Relay pin
    pinMode(RELAYPIN, OUTPUT);
    #ifndef TEST_MODE
      #ifdef RELAY_INVERTED
        digitalWrite(RELAYPIN, state == STATE_ON ? LOW : HIGH);
      #else
        digitalWrite(RELAYPIN, state == STATE_ON ? HIGH : LOW);
      #endif
    #else
      #ifdef RELAY_INVERTED
        digitalWrite(RELAYPIN, HIGH);
      #else
        digitalWrite(RELAYPIN, LOW);
      #endif
    #endif

    // Pins
    pinMode(BUTTONPIN, INPUT_PULLUP);
    digitalWrite(BUTTONPIN, HIGH);
    #ifdef ENABLE_GND_BUTTON
      pinMode(BUTTONPINGND, OUTPUT);
      digitalWrite(BUTTONPINGND, LOW);
    #endif
    #ifdef BUTTON_TWO_STATES
      lastButtonState = UNINITIALIZED;
    #endif

    DEBUG_PRINT("Pins ok\n");
}

// *****************************************************************************
//  Command Things
// *****************************************************************************
void executeCommand(const char* cmd, unsigned long payload) {
  PRINTF("executeCommand(%s, %ul)\n", cmd, payload);
  if (strcmp(cmd, ON_CMD)== 0) {
      DEBUG_PRINT("Switching state to enabled\n");
      if (state == STATE_OFF) performStateSwitch();
      timedSwitchActive = false; // Clear timers!
  } else if (strcmp(cmd, OFF_CMD)== 0) {
      DEBUG_PRINT("Switching state to disabled\n");
      if (state == STATE_ON) performStateSwitch();
      timedSwitchActive = false; // Clear timers!
  } else if (strcmp(cmd, SWITCH_CMD)== 0) {
      if (state == STATE_OFF) DEBUG_PRINT("Switching state to enabled\n");
      else DEBUG_PRINT("Switching state to disabled\n");
      performStateSwitch();
      timedSwitchActive = false; // Clear timers!
  } else if (strcmp(cmd, ON_TIME_CMD)== 0) {
      if (state == STATE_OFF) {
        DEBUG_PRINT("Timed on switch: ");
        DEBUG_PRINT(payload);
        DEBUG_PRINT(" secs\n");
        prepareTimedEvent(payload);
      } else {
        DEBUG_PRINT("Timed on switch: already ON\n");
      }
  } else if (strcmp(cmd, OFF_TIME_CMD)== 0) {
      if (state == STATE_ON) {
        DEBUG_PRINT("Timed off switch: ");
        DEBUG_PRINT(payload);
        DEBUG_PRINT(" secs\n");
        prepareTimedEvent(payload);
      } else {
        DEBUG_PRINT("Timed off switch: already OFF\n");
      }
  } else {
      DEBUG_PRINT("Received unexpected command!\n");
  }
}



// *****************************************************************************
//  State Things
// *****************************************************************************
void performStateSwitch() {
  DEBUG_PRINT("Performing state switch\n");
  state = !state;
  #ifndef TEST_MODE
  #ifdef RELAY_INVERTED
    digitalWrite(RELAYPIN, state == STATE_ON ? LOW : HIGH);
  #else
    digitalWrite(RELAYPIN, state == STATE_ON ? HIGH : LOW);
  #endif
  saveCurrentState();
  publishCurrentState();
  #endif
}

// *****************************************************************************
//  Button Things
// *****************************************************************************
void checkButton() {
  #ifdef BUTTON_TWO_STATES
    // Fix for uninitialized button
    if (lastButtonState == UNINITIALIZED) {
      lastButtonState = getButtonState();
      return;
    }
    if (getButtonState() != lastButtonState) {
          lastButtonState = lastButtonState == OPEN ? CLICKED : OPEN;
  #else
    if (lastClickDelay.isExpired() && getButtonState() == CLICKED) {
      lastClickDelay.start(BUTTON_DEBOUNCE_TIME, AsyncDelay::MILLIS);
  #endif
          DEBUG_PRINT("Button pressed!\n");
          executeCommand(SWITCH_CMD, 0);
    }
}

inline int getButtonState() {
  return digitalRead(BUTTONPIN) == LOW ? CLICKED : OPEN;
}

// *****************************************************************************
//  Timer Things
// *****************************************************************************
void checkTimedEvent() {
  if (!timedSwitchActive) return; // No events!

  if (timedSwitchDelay.isExpired()) {
    DEBUG_PRINT("Timer expired!\n");
    executeCommand(SWITCH_CMD, 0);
  }
}

void prepareTimedEvent(unsigned long secs) {
  if (secs == 0) return;

  // Change status
  executeCommand(SWITCH_CMD, 0);

  // Prepare data
  timedSwitchDelay.start(secs*1000, AsyncDelay::MILLIS);
  timedSwitchActive = true;
}


// *****************************************************************************
//  MQTT Things
// *****************************************************************************
void publishCurrentState() {
    if (isWifiOk()) {
        DEBUG_PRINT("Publishing current state\n");
        client.publish(BOARD_TOPIC, buildJsonString().c_str(), true);
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, BOARD_TOPIC) != 0) {
        DEBUG_PRINT("mqttCallback: unknown topic");
        return;
    }

    // it's my topic (could be retained message or a command)
    char* message = (char*) payload;
    message[length] = '\0';
    PRINTF("mqttCallback(%s, %s, %ul)\n", topic, message, length);
    parseJson(message);
}

// *****************************************************************************
//  JSON Things
// *****************************************************************************
const size_t bufferSize = JSON_OBJECT_SIZE(2) + 100;
DynamicJsonBuffer jsonBuffer(bufferSize);

void parseJson(const char* message) {
    JsonObject& root = jsonBuffer.parseObject(message);

    if (root.containsKey("stato") && root.containsKey("ts")) {
        // retained message
        const char* stato = root["stato"]; // "ON"
        long ts = root["ts"]; // 1351824120

        if ( // invalid state, update
            (strcmp(stato, "ON") == 0 && state == STATE_OFF) ||
            (strcmp(stato, "OFF") == 0 && state == STATE_ON) ||
            (strcmp(stato, "ON") != 0 && strcmp(stato, "OFF") != 0)
           ) {
            publishCurrentState();
        }
    } else if (root.containsKey("cmd")) {
        // command message
        const char* cmd = root["cmd"];
        if (root.containsKey("payload")) {
            unsigned long payload = root["payload"];
            executeCommand(cmd, payload);
        } else {
            executeCommand(cmd, 0);
        }
    } else {
        publishCurrentState(); // fix status
    }
}

String buildJsonString() {
    StaticJsonBuffer<JSON_OBJECT_SIZE(3)> jsonBuf;
    JsonObject& root = jsonBuf.createObject();
    root["ts"] = myTimeUtils.getEpoch();
    root["stato"] = state == STATE_ON ? "ON" : "OFF";
    String res;
    root.printTo(res);
    return res;
}


// *****************************************************************************
//  EEPROM Things
// *****************************************************************************
void loadOldState() {
    DEBUG_PRINT("Loading old state from memory...");
    state = EEPROM.read(EEPROM_ADDRESS);
    if (state != STATE_ON && state != STATE_OFF) { // EEPROM uninitialized
      state = STATE_OFF;
      saveCurrentState();
      DEBUG_PRINT("First boot! State now is disabled\n");
    }
}

void saveCurrentState() {
    EEPROM.write(EEPROM_ADDRESS, state);
    EEPROM.commit();
}
