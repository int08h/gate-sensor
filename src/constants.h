#ifndef GATE_SENSOR_CONSTANTS_H
#define GATE_SENSOR_CONSTANTS_H

#include <cstdint>

namespace c {
    const uint32_t DEEP_SLEEP_SEC = 2;
    const uint32_t DEEP_SLEEP_USEC = DEEP_SLEEP_SEC * 1000 * 1000;

    const uint32_t NTP_MAX_WAIT_MILLIS = 2000;

    const uint32_t WIFI_MAX_ATTEMPTS = 10;

    const char *TIME_FMT = "%Y-%m-%d %H:%M:%S Z";
    const time_t TIME_MIN_EPOCH = 1551398400;

    // GCP project
    const char *GCP_PROJECT = "int08h-blog";

    // Valid for 23 hours
    const time_t JWT_VALID_SEC = 23 * 60 * 60;

    // base64url(`{"alg":"ES256","typ":"JWT"}`)
    const char *JWT_EC_HEADER_B64 = "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.";
    // base64url(`{"alg":"RS256","typ":"JWT"}`)
    const char *JWT_RSA_HEADER_B64 = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.";
    // JWT claims
    const char *JWT_CLAIMS_FMT = R"({"iat":%u,"exp":%u,"aud":"%s"})";
}

#endif //GATE_SENSOR_CONSTANTS_H
