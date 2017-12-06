
#include "otautils.h"

#ifdef ENABLE_OTA

OtaHttpUpdater::OtaHttpUpdater() :
    httpServer(80),
    httpUpdater(),
    running(false)
{
}

void OtaHttpUpdater::begin() {
    if (running) return;
    DEBUG_PRINT("OtaHttpUpdater::begin()\n");
    httpUpdater.setup(&httpServer, OTA_PATH, OTA_USERNAME, OTA_PASSWORD);
    MDNS.begin(OTA_HOSTNAME);
    httpServer.begin();
    MDNS.addService("http", "tcp", 80);
    running = true;
}

void OtaHttpUpdater::loop() {
    if (!running) begin();
    httpServer.handleClient();
}

#else

OtaHttpUpdater::OtaHttpUpdater()
{
}

void OtaHttpUpdater::begin() {
}

void OtaHttpUpdater::loop() {
}

#endif
