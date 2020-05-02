/**
 * ESP32 based sensor for detecting open/close condition of a Hall-effect sensor
 * and communicating events via Wifi to Google Cloud IoT Core.
 *
 * Copyright (c) 2020 int08h LLC.
 * Apache 2.0 licensed https://www.apache.org/licenses/LICENSE-2.0
 */

#include "Arduino.h"
#include "WiFi.h"
#include "my-log.h"

//time.google.com has address 216.239.35.8
//time.google.com has address 216.239.35.4
//time.google.com has address 216.239.35.12
//time.google.com has address 216.239.35.0

namespace metric {
    RTC_DATA_ATTR static uint32_t sent_events = 0;
    RTC_DATA_ATTR static uint32_t sent_telemetry = 0;

    RTC_DATA_ATTR static uint64_t cpu_starts = 0;
    RTC_DATA_ATTR static uint64_t ulp_sensor_checks = 0;

    RTC_DATA_ATTR static uint64_t ts_start = 0;
    RTC_DATA_ATTR static uint64_t ts_last_wake = 0;
}

boolean waitForWifi() {
    const int MAX_ATTEMPTS = 10;
    int attempt = 1;

    while (WiFi.status() != WL_CONNECTED && attempt < MAX_ATTEMPTS) {
        delay(200);
        LI("wifi connect attempt %d/%d", attempt, MAX_ATTEMPTS);
        attempt++;
    }

    return WiFi.status() == WL_CONNECTED;
}

void initialStart() {
    LI("initial startup");

    if (!waitForWifi()) {
        LI("wifi failed to start, sleeping");
        return;
    }

    metric::ts_start = millis();
}

void setup() {
    const char *ssid = "-iot";
    const char *pass = "iot!";

    Serial.begin(115200);

    metric::cpu_starts++;

    WiFi.begin(ssid, pass);

    if (metric::ts_start == 0) {
        initialStart();
    } else {
        uint64_t now = millis();
        LI("starts %llu, last wake %llu, now %llu", metric::cpu_starts, metric::ts_last_wake, now);
        metric::ts_last_wake += now;
    }

    WiFi.disconnect(true);
    esp_deep_sleep(2000000);
}

void loop() {
}
