
#ifndef GATE_SENSOR_RTC_DATA_H
#define GATE_SENSOR_RTC_DATA_H

#include <esp_attr.h>
#include <cstdint>
#include <ctime>
#include "jwt.h"

// Metrics and counters stored in the RTC slow data section that are
// accessible by the ULP and retain their value across CPU restarts
namespace rd {
    RTC_DATA_ATTR static uint32_t sent_events = 0;
    RTC_DATA_ATTR static uint32_t sent_telemetry = 0;

    RTC_DATA_ATTR static uint64_t cpu_starts = 0;
    RTC_DATA_ATTR static uint64_t ulp_sensor_checks = 0;

    RTC_DATA_ATTR static time_t ts_start = 0;
    RTC_DATA_ATTR static time_t ts_current = 0;

    RTC_DATA_ATTR static Jwt jwt;
}

#endif //GATE_SENSOR_RTC_DATA_H
