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

    if (!connectWifi()) {
        return;
    }

    time_t now = pollTime();
    rd::ts_start = now;
    rd::ts_current = now;
    LI("NTP time %s", timeStr(rd::ts_start).c_str());

    if (rd::jwt.isExpired(now)) {
        rd::jwt.regenerate(now);
    }

    LI("jwt = %s", rd::jwt.value().c_str());
}

void onWake() {
    LI("start");

    if (!connectWifi()) {
        return;
    }

    time_t ts_prev = rd::ts_current;
    rd::ts_current = pollTime();
    LI("starts %llu, last wake %s, now %s",
       rd::cpu_starts, timeStr(ts_prev).c_str(), timeStr(rd::ts_current).c_str());
}

void deepSleep() {
    LI("preparing for deep sleep");
    WiFi.disconnect(true);

    LI("entering deep sleep");
    esp_deep_sleep(c::DEEP_SLEEP_USEC);
}

void setup() {
    Serial.begin(115200);

    rd::cpu_starts++;

    if (rd::ts_start == 0) {
        onBoot();
    } else {
        onWake();
    }

    deepSleep();
}

void loop() {}

