
#include "timeutils.h"
#include "wifiutils.h"
#include "boards.h"
#include "mydebug.h"

void MyTimeUtils::initTime() {
    timeClient.begin();
    timeStarted = true;
}

void MyTimeUtils::closeTime() {
    timeClient.end();
    timeStarted = false;
}

void MyTimeUtils::updateCurrentTime() {
    timeClient.update();
}

unsigned long MyTimeUtils::getEpoch() {
    return timeClient.getEpochTime();
}

void MyTimeUtils::callMeInLoop() {
    volatile bool wState = isWifiOk();

    if (!timeStarted && wState) {
        initTime();
        updateCurrentTime();
    } else if (wState) {
        updateCurrentTime();
    } else {
        closeTime();
    }
}
