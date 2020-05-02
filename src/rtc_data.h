
#ifndef GATE_SENSOR_RTC_DATA_H
#define GATE_SENSOR_RTC_DATA_H

#include <esp_attr.h>
#include <cstdint>
#include <ctime>

// Metrics and counters
namespace rd {
    RTC_DATA_ATTR static uint32_t sent_events = 0;
    RTC_DATA_ATTR static uint32_t sent_telemetry = 0;

    RTC_DATA_ATTR static uint64_t cpu_starts = 0;
    RTC_DATA_ATTR static uint64_t ulp_sensor_checks = 0;

    RTC_DATA_ATTR static tm ts_start = {};
    RTC_DATA_ATTR static tm ts_current = {};
}

#endif //GATE_SENSOR_RTC_DATA_H
