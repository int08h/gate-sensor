#ifndef GATE_SENSOR_TIME_UTIL_H
#define GATE_SENSOR_TIME_UTIL_H

#include <Arduino.h>
#include "addr.h"
#include "mlog.h"
#include "constants.h"

void updateTime(tm* dst) {
    configTime(0, 0, addr::time3, addr::time2, addr::time1);
    if (!getLocalTime(dst, /*timeout ms*/ c::MAX_NTP_WAIT_MILLIS)) {
        LE("failed to obtain time");
    }
}

String timeStr(const tm& now) {
    char buf[32] = {};
    strftime(buf, sizeof(buf), c::TIME_FMT, &now);
    return String(buf);
}

#endif //GATE_SENSOR_TIME_UTIL_H
