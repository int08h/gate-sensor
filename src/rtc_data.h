
#ifndef GATE_SENSOR_RTC_DATA_H
#define GATE_SENSOR_RTC_DATA_H

#include <esp_attr.h>
#include <cstdint>
#include <ctime>
#include "jwt.h"
#include "gate.h"

// ensure enums are 32-bit in size to conform to ULP's word size
static_assert(sizeof(GateState) == 4, "enum not expected size");

// Metrics and counters stored in the RTC slow data section that are
// accessible by the ULP and retain their value across CPU restarts
namespace rtcd {
    RTC_DATA_ATTR static GateState last_state = UNKNOWN;

    RTC_DATA_ATTR static uint32_t sent_events = 0;
    RTC_DATA_ATTR static uint32_t sent_telemetry = 0;

    RTC_DATA_ATTR static uint64_t cpu_wakeups = 0;
    RTC_DATA_ATTR static uint64_t ulp_sensor_checks = 0;

    RTC_DATA_ATTR static time_t ts_boot = 0;
    RTC_DATA_ATTR static time_t ts_wake = 0;
    RTC_DATA_ATTR static time_t ts_start_sleep = 0;
    RTC_DATA_ATTR static time_t ts_next_telemetry = 0;

    RTC_DATA_ATTR static Jwt jwt;
}

#endif //GATE_SENSOR_RTC_DATA_H
