/**
 * ESP32 based sensor for detecting open/close condition of a Hall-effect sensor
 * and communicating events via Wifi to Google Cloud IoT Core.
 *
 * Copyright (c) 2020 int08h LLC.
 * Apache 2.0 licensed https://www.apache.org/licenses/LICENSE-2.0
 */

#include <HTTPClient.h>
#include "Arduino.h"
#include "WiFiClientSecure.h"

#include "mlog.h"
#include "rtc_data.h"
#include "net_util.h"

using rd::jwt;

void handleBoot();
void handleWakeup();
void enterDeepSleep();
bool isInitialBoot();
bool sendIotTelemetry(WiFiClientSecure &client, time_t now);
bool sendPushoverEvent(WiFiClientSecure &client);

// Program always starts (and ends) here; loop() is never used.
void setup() {
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    Serial.begin(115200);

    rd::cpu_starts++;

    if (isInitialBoot()) {
        handleBoot();
    } else {
        handleWakeup();
    }

    enterDeepSleep();
}

__unused void loop() {}

bool isInitialBoot() {
    return rd::ts_start == 0;
}

void handleBoot() {
    LI("initial boot");

    if (!connectWifi()) {
        return;
    }

    time_t now = setTime();
    rd::ts_start = now;
    rd::ts_current = now;
    LI("NTP time %s", timeStr(rd::ts_start).c_str());

    if (jwt.isExpired(now)) {
        jwt.regenerate(now);
    }
}

void handleWakeup() {
    LI("wakeup");

    if (!connectWifi()) {
        return;
    }

    time_t now = setTime();
    time_t ts_prev = rd::ts_current;
    rd::ts_current = now;
    LI("starts %llu, last wake %s, now %s",
       rd::cpu_starts, timeStr(ts_prev).c_str(), timeStr(rd::ts_current).c_str());

    if (jwt.isExpired(now)) {
        jwt.regenerate(now);
    }

    WiFiClientSecure client = WiFiClientSecure();
    client.setTimeout(/*secs*/10);

    sendIotTelemetry(client, now);
//    sendPushoverEvent(client);

    client.stop();
}

void enterDeepSleep() {
    LI("preparing for deep sleep");
    WiFi.disconnect(true);

    LI("entering deep sleep");
    esp_deep_sleep(c::DEEP_SLEEP_USEC);
}

bool sendIotTelemetry(WiFiClientSecure &client, time_t now) {
    client.setCACert(keys::google_root_bundle);

    HTTPClient req;
    req.setConnectTimeout(/*ms*/10 * 1000);

    String url = String(addr::GOOGLE_IOT_URL) + String(c::DEVICE_ID_GOOGLE) + ":publishEvent";
    req.begin(client, url);
    req.addHeader("Authorization", jwt.asHeader());
    req.addHeader("Content-Type", "application/json");
    req.addHeader("Cache-Control", "no-cache");

    char ptmp[256];
    snprintf(ptmp, sizeof(ptmp), c::GCP_STATE_FMT, now, WiFi.RSSI(), 0, rd::sent_events,
             rd::sent_telemetry, rd::cpu_starts, rd::ulp_sensor_checks);
    char b64tmp[512];
    base64url_encode((unsigned char *) ptmp, strlen(ptmp), b64tmp);

    String payload = String("{\"binary_data\":\"") + b64tmp + "\"}";

    int code = req.POST(payload);
    rd::sent_telemetry++;
    LI("POST to google compete, code = %d", code);

    if (code != 200) {
        dumpResponse(client);
    }

    return code == 200;
}

bool sendPushoverEvent(WiFiClientSecure &client) {
    client.setCACert(keys::digicert_ca);

    HTTPClient req;
    req.setConnectTimeout(/*ms*/10 * 1000);

    req.begin(client, addr::PO_URL);
    req.addHeader("Content-Type", "application/x-www-form-urlencoded");
    req.addHeader("Cache-Control", "no-cache");

    String payload = String("message=") + String(c::DEVICE_ID_HUMAN) + " has been opened.";
    payload.replace(' ', '+');

    payload += "&token=";
    payload += keys::po_token;
    payload += "&user=";
    payload += keys::po_user;
    payload += "&device=";
    payload += c::DEVICE_ID_PO;

    int code = req.POST(payload);
    rd::sent_events++;
    LI("Pushover request response %d", code);

    if (code != 200) {
        dumpResponse(client);
    }

    return code == 200;
}


