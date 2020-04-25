#ifndef GATESENSOR_INTERNET_H
#define GATESENSOR_INTERNET_H

#include "esp_wifi.h"

#include "Arduino.h"
#include "WiFi.h"

class Internet {
public:
    Internet(const char* ssid, const char* passwd);

    bool connect();
    uint32_t sendAll();

private:
    const char* ssid;
    const char* passwd;

private:
    const IPAddress localAddr{192, 168, 1, 230};
    const IPAddress gatewayAddr{192, 168, 1, 1};
    const IPAddress subnetMask{255, 255, 255, 0};
    const IPAddress primaryDns{8, 8, 8, 8};
    const IPAddress secondaryDns{8, 8, 4, 4};
};

#endif //GATESENSOR_INTERNET_H
