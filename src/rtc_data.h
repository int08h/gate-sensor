#ifndef GATE_SENSOR_RTC_DATA_H
#define GATE_SENSOR_RTC_DATA_H

#include <esp_attr.h>
#include <cstdint>
#include <ctime>
#include "jwt.h"
#include "gate.h"

// Metrics and counters stored in the RTC slow data section that are
// accessible by the ULP and retain their value across CPU restarts
namespace rtcd {
// Accumulate sensor check counts while ULP is running; cleared on wake
RTC_DATA_ATTR static uint32_t ulp_check_acc = 0;
// Accumulate number out-of-range measurements
RTC_DATA_ATTR static uint32_t ulp_oor_acc = 0;
// Most recent ADC measurement value
RTC_DATA_ATTR static uint32_t ulp_a1c5_value = 999;

// Most recent gate state
RTC_DATA_ATTR static GateState last_state = UNKNOWN;

// Count of telemetry and gate state events sent
RTC_DATA_ATTR static uint32_t sent_events = 0;
RTC_DATA_ATTR static uint32_t sent_telemetry = 0;

// How many times the CPU has woken (deep sleep or ULP)
RTC_DATA_ATTR static uint64_t cpu_wakeups = 0;
// Total number of ULP sensor checks
RTC_DATA_ATTR static uint64_t ulp_sensor_checks = 0;
// Total number of ULP sensor measurements THRESHOLD_LOW < val < THRESHOLD_HIGH
RTC_DATA_ATTR static uint64_t ulp_out_of_range = 0;

// Timestamps of various events
RTC_DATA_ATTR static time_t ts_boot = 0;
RTC_DATA_ATTR static time_t ts_wake = 0;
RTC_DATA_ATTR static time_t ts_start_sleep = 0;
RTC_DATA_ATTR static time_t ts_next_telemetry = 0;

// JWT to authenticate to GCP
RTC_DATA_ATTR static Jwt jwt;
}

// ensure enums are 32-bit in size to conform to ULP's word size
static_assert(sizeof(GateState) == 4, "enum not expected size");

#endif //GATE_SENSOR_RTC_DATA_H
