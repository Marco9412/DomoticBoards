
#include "mqttutils.h"
#include "ledutils.h"
#include "boards.h"
#include "mydebug.h"

void mqttConnect(PubSubClient& client) {
    DEBUG_PRINT("Attempting MQTT connection... ");

    if (!client.connect(MQTT_HOST_ID, USERNAME, PASSWORD)) {
        DEBUG_PRINT("Cannot connect! Probably WiFi isn't connected!\n");
        DEBUG_PRINT("MQTT State -> ");
        DEBUG_PRINT(client.state());
        DEBUG_PRINT("\n");

        blinkLed(500);
        yield();
        return;
    }
    DEBUG_PRINT("\nMQTT connected! Subscribing... ");
    client.subscribe(BOARD_TOPIC);

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
        return;
    }
    client.loop();
}
