
#ifndef GATE_SENSOR_MAIN_H
#define GATE_SENSOR_MAIN_H

#include <driver/adc.h>
#include <driver/rtc_io.h>

#include "Arduino.h"
#include "HTTPClient.h"
#include "WiFiClientSecure.h"

#include "accel.h"
#include "certs.h"
#include "secrets.h"
#include "mlog.h"
#include "net_util.h"
#include "rtc_data.h"
#include "ulp_prog.h"

void handleBoot();
void handleWakeup();
void prepareUlp();
void handleGateChange(GateState state);
void handleSendTelemetry();
void enterDeepSleep(uint32_t duration_usec);

bool isInitialBoot();
bool sendIotTelemetry(WiFiClientSecure &client);
bool sendPushoverEvent(WiFiClientSecure &client, GateState state);

const char *wake_reason();

inline uint16_t ulp_read(uint32_t val) {
    return val & 0xffff;
}

#endif //GATE_SENSOR_MAIN_H
