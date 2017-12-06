
#ifndef OTAUTILS_H
#define OTAUTILS_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include "boards.h"
#include "mydebug.h"

class OtaHttpUpdater {
public:
    OtaHttpUpdater();
    void begin();
    void loop();

private:
    ESP8266WebServer httpServer;
    ESP8266HTTPUpdateServer httpUpdater;
    bool running;
};


#endif
