/**
 * ESP32 based sensor for detecting open/close condition of a Hall-effect sensor
 * and communicating events via Wifi to Google Cloud IoT Core.
 *
 * Copyright (c) 2020 int08h LLC.
 * Apache 2.0 licensed https://www.apache.org/licenses/LICENSE-2.0
 */

#include "Arduino.h"
#include "internet.h"

class Telemetry {
public:
    Telemetry() : wakeCount(0) {}

    uint64_t wakeCount;
};


RTC_DATA_ATTR static Telemetry telemetry = Telemetry();
RTC_DATA_ATTR static Internet net = Internet("foo-iot", "foo");

void setup() {
    telemetry.wakeCount++;
    Serial.begin(115200);
    ESP_LOGI("loop", ". %u", telemetry.wakeCount);

    net.connect();

    esp_wifi_stop();
    esp_deep_sleep(1000000);
}

void loop() {
}
