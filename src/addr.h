
#ifndef GATE_SENSOR_ADDR_H
#define GATE_SENSOR_ADDR_H

#include <IPAddress.h>

namespace addr {
    IPAddress GW = IPAddress(192, 168, 1, 1);
    IPAddress LOCAL = IPAddress(192, 168, 1, 230);
    IPAddress SUBNET = IPAddress(255, 255, 255, 0);

    IPAddress DNS1 = IPAddress(8, 8, 8, 8);
    IPAddress DNS2 = IPAddress(1, 1, 1, 1);

    const char *TIME1 = "time1.google.com";
    const char *TIME2 = "time2.google.com";
    const char *TIME3 = "time3.google.com";

    // mqtt.2030.ltsapis.goog
    const char *GOOGLE_HOST = "mqtt.2030.ltsapis.goog";
    IPAddress GOOGLE_MQTT = IPAddress(74, 125, 69, 206);

    const char *PO_HOST = "api.pushover.net";
    const char *PO_URL = "https://api.pushover.net/1/messages.json";
}

#endif //GATE_SENSOR_ADDR_H
