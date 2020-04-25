#include "internet.h"

Internet::Internet(const char* ssid, const char* passwd)
  : ssid(ssid)
  , passwd(passwd)
{

}

bool Internet::connect() {
    if (WiFiClass::status() == WL_CONNECTED) {
        ESP_LOGI("net", "connected to %s", ssid);
        return false;
    }

    WiFiClass::mode(WIFI_MODE_STA);

    bool ok = WiFi.config(localAddr, gatewayAddr, subnetMask, primaryDns, secondaryDns);
    if (!ok) {
        ESP_LOGE("net", "wifi config failed");
        return false;
    }

    WiFi.begin(ssid, passwd);

    while (WiFiClass::status() != WL_CONNECTED) {
        ESP_LOGI("net", "awaiting connection to AP");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    ESP_LOGI("net", "connected to %s at %d", WiFi.BSSIDstr().c_str(), WiFi.RSSI());
    return true;
}

uint32_t Internet::sendAll() {
    return 0;
}