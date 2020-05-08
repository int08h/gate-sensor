#ifndef GATE_SENSOR_TIME_UTIL_H
#define GATE_SENSOR_TIME_UTIL_H

#include <Arduino.h>
#include "addr.h"
#include "mlog.h"
#include "constants.h"
#include "lwip/apps/sntp.h"

time_t pollTime() {
    configTime(0, 0, addr::TIME3, addr::TIME2, addr::TIME1);

    time_t now = 0;
    uint64_t deadline = millis() + c::NTP_MAX_WAIT_MILLIS;

    while (millis() < deadline) {
        time(&now);
        if (now > c::TIME_MIN_EPOCH) {
            // set clock; arduino esp-idf 3.x doesn't have SNTP mode immediate
            timeval tv{};
            tv.tv_sec = now;
            tv.tv_usec = 0;
            if (settimeofday(&tv, nullptr) != 0) {
                LE("settimeofday failed? errno=%d", errno);
            }
            break;
        }
        delay(25);
    }

    return now;
}

String timeStr(time_t now) {
    char buf[32] = {};

    tm *ts = localtime(&now);
    strftime(buf, sizeof(buf), c::TIME_FMT, ts);

    return String(buf);
}

#endif //GATE_SENSOR_TIME_UTIL_H
