
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


#ifndef TIME_UTILS_H
#define TIME_UTILS_H

class MyTimeUtils {

  public:
    MyTimeUtils(): ntpUDP(), timeClient(ntpUDP, 120000), timeStarted(false) {}
    void initTime();
    void closeTime();
    void updateCurrentTime();
    unsigned long getEpoch();

    void callMeInLoop();

  private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;
    bool timeStarted;
};

#endif
