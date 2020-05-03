
#ifndef GATE_SENSOR_ADDR_H
#define GATE_SENSOR_ADDR_H

#include <IPAddress.h>

namespace addr {
    IPAddress gw = IPAddress(192, 168, 1, 1);
    IPAddress local = IPAddress(192, 168, 1, 230);
    IPAddress subnet = IPAddress(255, 255, 255, 0);

    IPAddress dns1 = IPAddress(8, 8, 8, 8);
    IPAddress dns2 = IPAddress(1, 1, 1, 1);

    const char* time1 = "time1.google.com";
    const char* time2 = "time2.google.com";
    const char* time3 = "time3.google.com";

    const char* google_mqtt = "mqtt.2030.ltsapis.goog";
}

#endif //GATE_SENSOR_ADDR_H
