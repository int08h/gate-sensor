#ifndef GATE_SENSOR_WIFI_UTIL_H
#define GATE_SENSOR_WIFI_UTIL_H

#include <Arduino.h>
#include "WiFi.h"
#include "constants.h"
#include "mlog.h"

boolean waitForWifi() {
    uint32_t attempt = 1;

    while (!WiFi.isConnected() && attempt < c::MAX_WIFI_ATTEMPTS) {
        LI("wifi connect attempt %2d/%d", attempt, c::MAX_WIFI_ATTEMPTS);
        delay(250);
        attempt++;
    }

    return WiFi.isConnected();
}

void connectWifi() {
    const char *ssid = "-iot";
    const char *pass = "iot!";

    WiFi.config(addr::local, addr::gw, addr::subnet, addr::dns1, addr::dns2);
    WiFi.begin(ssid, pass);
    LI("wifi connect ssid '%s', rssi %d, bssid %s",
       WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.BSSIDstr().c_str());

    if (!waitForWifi()) {
        LE("wifi failed to start");
        return;
    }
}


#endif //GATE_SENSOR_WIFI_UTIL_H
