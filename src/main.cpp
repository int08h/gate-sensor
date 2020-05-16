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

#include <driver/adc.h>
#include <driver/rtc_io.h>
#include <soc/rtc.h>

#include "mlog.h"
#include "rtc_data.h"
#include "net_util.h"

void handleBoot();
void handleWakeup();
void enterDeepSleep();
bool isInitialBoot();
bool sendIotTelemetry(WiFiClientSecure &client, time_t now);
bool sendPushoverEvent(WiFiClientSecure &client);

// Program always starts (and ends) here; loop() is never used.
void setup() {
//    esp_log_level_set("*", ESP_LOG_VERBOSE);
    Serial.begin(115200);

    rtcd::cpu_wakeups++;

    if (isInitialBoot()) {
        handleBoot();
    } else {
        handleWakeup();
    }

    enterDeepSleep();
}

__unused void loop() {}

bool isInitialBoot() {
    return rtcd::ts_boot == 0;
}

void handleBoot() {
    LI("initial boot");

    if (!connectWifi()) {
        return;
    }

    time_t now = setNtpTime();
    rtcd::ts_boot = now;

    if (rtcd::jwt.isExpired(now)) {
        rtcd::jwt.regenerate(now);
    }

    rtcd::ts_next_telemetry = now + c::TELEMETRY_SEND_SEC;
}

void handleWakeup() {
    LI("wakeup");

    Gate gate{};
    SensorReading reading = gate.measure();
    if (reading == CLOSED) {

    }

    if (!connectWifi()) {
        return;
    }

    time_t now = setNtpTime();
    LI("last wake %s, now %s", timeStr(rtcd::ts_start_sleep).c_str(), timeStr(now).c_str());

    if (rtcd::jwt.isExpired(now)) {
        rtcd::jwt.regenerate(now);
    }

    WiFiClientSecure client = WiFiClientSecure();
    client.setTimeout(/*secs*/10);

    if (now > rtcd::ts_next_telemetry) {
        LI("Sending telemetry (%s > %s)",
                timeStr(now).c_str(), timeStr(rtcd::ts_next_telemetry).c_str());
        sendIotTelemetry(client, now);
        rtcd::ts_next_telemetry = now + c::TELEMETRY_SEND_SEC;
    }
//    sendPushoverEvent(client);

    client.stop();
}

void enterDeepSleep() {
    LI("preparing for deep sleep");
    WiFi.disconnect(true);

    // Suppress boot messages
    esp_deep_sleep_disable_rom_logging();

    // Prevent current drain from internal pull ups on flash controller
    rtc_gpio_isolate(GPIO_NUM_12);
    rtc_gpio_isolate(GPIO_NUM_15);

    // Prepare ADC1 to be read by ULP
    // GPIO33 is ADC1_CH5, 0 = gate closed, 4095 = gate open
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_ulp_enable();

    LI("entering deep sleep");
    // todo: start ULP program - ulp_run()
    ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup() );
    time(&rtcd::ts_start_sleep);
    esp_deep_sleep(c::DEEP_SLEEP_USEC);
}

// Send telemetry to GCP IoT Core
bool sendIotTelemetry(WiFiClientSecure &client, time_t now) {
    client.setCACert(keys::google_root_bundle);

    HTTPClient req;
    req.setConnectTimeout(/*ms*/10 * 1000);

    String url = String(addr::GOOGLE_IOT_URL) + String(c::DEVICE_ID_GOOGLE) + ":publishEvent";
    req.begin(client, url);
    req.addHeader("Authorization", rtcd::jwt.asHeader());
    req.addHeader("Content-Type", "application/json");
    req.addHeader("Cache-Control", "no-cache");

    char ptmp[256];
    snprintf(ptmp, sizeof(ptmp), c::GCP_STATE_FMT, now, WiFi.RSSI(), 0, rtcd::sent_events,
             rtcd::sent_telemetry, rtcd::cpu_wakeups, rtcd::ulp_sensor_checks);
    char b64tmp[512];
    base64url_encode((unsigned char *) ptmp, strlen(ptmp), b64tmp);

    String payload = String("{\"binary_data\":\"") + b64tmp + "\"}";

    int code = req.POST(payload);
    rtcd::sent_telemetry++;
    LI("POST to Google complete, code = %d", code);

    if (code != 200) {
        dumpResponse(client);
    }

    return code == 200;
}

// Send mobile alert to Pushover
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
    rtcd::sent_events++;
    LI("POST to Pushover complete, code = %d", code);

    if (code != 200) {
        dumpResponse(client);
    }

    return code == 200;
}


