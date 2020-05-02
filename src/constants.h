#ifndef GATE_SENSOR_CONSTANTS_H
#define GATE_SENSOR_CONSTANTS_H

#include <cstdint>

namespace c {
    const uint32_t DEEP_SLEEP_SEC = 2;
    const uint32_t DEEP_SLEEP_USEC = DEEP_SLEEP_SEC * 1000 * 1000;

    const uint32_t MAX_NTP_WAIT_MILLIS = 2000;

    const uint32_t MAX_WIFI_ATTEMPTS = 10;

    const char *TIME_FMT = "%Y-%m-%d %H:%M:%S Z";
}

#endif //GATE_SENSOR_CONSTANTS_H
