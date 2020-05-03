#ifndef GATE_SENSOR_TIME_UTIL_H
#define GATE_SENSOR_TIME_UTIL_H

#include <Arduino.h>
#include "addr.h"
#include "mlog.h"
#include "constants.h"

time_t pollTime() {
    configTime(0, 0, addr::time3, addr::time2, addr::time1);

    time_t now = 0;
    uint64_t deadline = millis() + c::NTP_MAX_WAIT_MILLIS;

    while (millis() < deadline) {
        time(&now);
        if (now > 0) {
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
