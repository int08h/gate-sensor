#ifndef GATE_SENSOR_WIFI_UTIL_H
#define GATE_SENSOR_WIFI_UTIL_H

#include <Arduino.h>
#include "WiFiClientSecure.h"
#include "constants.h"
#include "mlog.h"

void waitForWifi() {
    uint32_t attempt = 1;

    while (!WiFi.isConnected() && attempt < c::WIFI_MAX_ATTEMPTS) {
        LI("wifi connect attempt %2d/%d", attempt, c::WIFI_MAX_ATTEMPTS);
        delay(250);
        attempt++;
    }
}

boolean connectWifi() {
    const char *ssid = "channing-iot";
    const char *pass = "welcome to iot!";

    WiFi.config(addr::LOCAL, addr::GW, addr::SUBNET, addr::DNS1, addr::DNS2);
    WiFi.begin(ssid, pass);

    waitForWifi();

    if (!WiFi.isConnected()) {
        LE("wifi connect failed");
        return false;
    }

    LI("wifi connected ssid '%s', rssi %d, bssid %s",
       WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.BSSIDstr().c_str());
    return true;
}

#endif //GATE_SENSOR_WIFI_UTIL_H
