#ifndef GATE_SENSOR_CONSTANTS_H
#define GATE_SENSOR_CONSTANTS_H

#include <cstdint>

namespace c {
    // Device name
    const char* DEVICE_ID_HUMAN = "Gate Sensor";
    // GCP device id
    const char* DEVICE_ID_GOOGLE = "sensor-dev-1";
    // Pushover device id
    const char* DEVICE_ID_PO = "sensor-dev";

    // How long to deep sleep in the ULP
    const uint32_t DEEP_SLEEP_SEC = 5;
    const uint32_t DEEP_SLEEP_USEC = DEEP_SLEEP_SEC * 1000 * 1000;

    // Interval between sending telemetry to GCP
    const uint32_t TELEMETRY_SEND_SEC = 30;

    // This many tries to get an NTP answer
    const uint32_t NTP_MAX_ATTEMPTS = 3;
    // Wait this long for each NTP response
    const uint32_t NTP_MAX_WAIT_MILLIS = 2000;

    // This many attempts to join WiFi
    const uint32_t WIFI_MAX_ATTEMPTS = 10;
    // Wait this long in between waits to join
    const uint32_t WIFI_WAIT_MILLIS = 300;

    // Format to log timestamp
    const char *TIME_FMT = "%Y-%m-%d %H:%M:%S Z";
    // If NTP time is prior to this, it's invalid
    const time_t TIME_MIN_EPOCH = 1551398400;

    // Valid for 23 hours
    const time_t JWT_VALID_SEC = 23 * 60 * 60;
    // base64url(`{"alg":"RS256","typ":"JWT"}`)
    const char *JWT_RSA_HEADER_B64 = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.";
    // JWT claims
    const char *JWT_CLAIMS_FMT = R"({"iat":%u,"exp":%u,"aud":"%s"})";

    // GCP project
    const char *GCP_PROJECT = "int08h-blog";

    // GCP IoT telemetry format
    const char *GCP_STATE_FMT = R"({"ts":%u,"rssi":%d,"g":%u,"evt":%u,"tmt":%u,"cpu":"%llu","ulp":"%llu"})";
}

#endif //GATE_SENSOR_CONSTANTS_H
