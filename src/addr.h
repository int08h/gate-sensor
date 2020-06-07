
#ifndef GATE_SENSOR_ADDR_H
#define GATE_SENSOR_ADDR_H

#include <IPAddress.h>

namespace addr {
// local addr in secrets.h
IPAddress GW = IPAddress(192, 168, 1, 1);
IPAddress SUBNET = IPAddress(255, 255, 255, 0);

IPAddress DNS1 = IPAddress(8, 8, 8, 8);
IPAddress DNS2 = IPAddress(1, 1, 1, 1);

const char *TIME1 = "time1.google.com";
const char *TIME2 = "time2.google.com";
const char *TIME3 = "time3.google.com";

const char *GOOGLE_IOT_URL =
    "https://cloudiotdevice.googleapis.com/v1/projects/int08h-blog/locations/us-central1/registries/home-automation/devices/";

const char *PO_URL = "https://api.pushover.net/1/messages.json";
}

#endif //GATE_SENSOR_ADDR_H
