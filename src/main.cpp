/**
 * ESP32 based sensor for detecting open/close condition of a Hall-effect sensor
 * and communicating events via Wifi to Google Cloud IoT Core.
 *
 * Copyright (c) 2020 int08h LLC.
 * Apache 2.0 licensed https://www.apache.org/licenses/LICENSE-2.0
 */

#include "Arduino.h"
#include "WiFi.h"
#include "mlog.h"
#include "rtc_data.h"
#include "time_util.h"
#include "wifi_util.h"


void onBoot() {
    LI("start");

    connectWifi();
    updateTime(&rd::ts_start);
    LI("NTP time %s", timeStr(rd::ts_start).c_str());

    LI("end");
}

void onWake() {
    LI("start");

    connectWifi();
    tm ts_prev = rd::ts_current;
    updateTime(&rd::ts_current);
    LI("starts %llu, last wake %s, now %s",
       rd::cpu_starts, timeStr(ts_prev).c_str(), timeStr(rd::ts_current).c_str());

    LI("end");
}

void setup() {
    Serial.begin(115200);

    rd::cpu_starts++;

    if (rd::ts_start.tm_mon == 0) {
        onBoot();
    } else {
        onWake();
    }

    WiFi.disconnect(true);
    esp_deep_sleep(c::DEEP_SLEEP_USEC);
}

void loop() {
}

