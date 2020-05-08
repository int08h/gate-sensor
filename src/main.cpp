/**
 * ESP32 based sensor for detecting open/close condition of a Hall-effect sensor
 * and communicating events via Wifi to Google Cloud IoT Core.
 *
 * Copyright (c) 2020 int08h LLC.
 * Apache 2.0 licensed https://www.apache.org/licenses/LICENSE-2.0
 */

#include <HTTPClient.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "WiFiClientSecure.h"

#include "mlog.h"
#include "rtc_data.h"
#include "time_util.h"
#include "wifi_util.h"

using rd::jwt;

void onBoot() {
    LI("boot");

    if (!connectWifi()) {
        return;
    }

    time_t now = pollTime();
    rd::ts_start = now;
    rd::ts_current = now;
    LI("NTP time %s", timeStr(rd::ts_start).c_str());

    if (jwt.isExpired(now)) {
        jwt.regenerate(now);
    }
}
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void onWake() {
    LI("wake");

    if (!connectWifi()) {
        return;
    }

    time_t now = pollTime();
    time_t ts_prev = rd::ts_current;
    rd::ts_current = now;
    LI("starts %llu, last wake %s, now %s",
       rd::cpu_starts, timeStr(ts_prev).c_str(), timeStr(rd::ts_current).c_str());

    if (jwt.isExpired(now)) {
        jwt.regenerate(now);
    }

    WiFiClientSecure client = WiFiClientSecure();
    client.setCACert(keys::google_bundle);
    client.setTimeout(/*secs*/15);
    client.connect(addr::GOOGLE_HOST, 443);

    PubSubClient pubsub{addr::GOOGLE_HOST, 443, client};
    pubsub.setCallback(callback);

    const char* devid = "projects/int08h-blog/locations/us-central1/registries/home-automation/devices/sensor-dev-1";
    if (!pubsub.connect(devid, "unused", jwt.value())) {
        LE("connect fail: %d", pubsub.state());
        return;
    }
    LI("connect completed");

    String payload = String("{\"timestamp\":") + now + "}";
    LI("payload = %s", payload.c_str());
    
    if (!pubsub.publish("/devices/sensor-dev-1/state", payload.c_str())) {
        LE("publish failed");
        return;
    }
    LI("publish sent");
    pubsub.disconnect();
    LI("disconnect");

    client.stop();
}

void sendToPushover() {
    HTTPClient req;
    req.setConnectTimeout(/*ms*/10 * 1000);

    req.begin(addr::PO_URL);
    req.addHeader("Host", "api.pushover.net");
    req.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String payload = String("message=This+is+a+test+2");
    payload += "&token=";
    payload += keys::po_token;
    payload += "&user=";
    payload += keys::po_user;
    payload += "&device=sensor-dev";

    int code = req.POST(payload);
    LI("HTTP request response %d", code);
}

void deepSleep() {
    LI("preparing for deep sleep");
    WiFi.disconnect(true);

    LI("entering deep sleep");
    esp_deep_sleep(c::DEEP_SLEEP_USEC);
}

void setup() {
    esp_log_level_set("*", ESP_LOG_VERBOSE);
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

