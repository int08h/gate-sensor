#ifndef GATE_SENSOR_NET_UTIL_H
#define GATE_SENSOR_NET_UTIL_H

#include <Arduino.h>
#include "WiFiClientSecure.h"
#include "lwip/apps/sntp.h"

#include "addr.h"
#include "constants.h"
#include "mlog.h"
#include "secrets.h"

String timeStr(time_t now) {
  char buf[32] = {};

  tm *ts = localtime(&now);
  strftime(buf, sizeof(buf), c::TIME_FMT, ts);

  return String(buf);
}

void waitForWifi() {
  uint32_t attempt = 1;

  while (!WiFi.isConnected() && attempt < c::WIFI_MAX_ATTEMPTS) {
    LI("wifi connect attempt %2d/%d", attempt, c::WIFI_MAX_ATTEMPTS);
    delay(c::WIFI_WAIT_MILLIS);
    attempt++;
  }
}

boolean connectWifi() {
  if (WiFi.isConnected()) {
    return true;
  }

  const char *ssid = "channing-iot";
  const char *pass = "welcome to iot!";

  WiFi.config(secrets::LOCAL, addr::GW, addr::SUBNET, addr::DNS1, addr::DNS2);
  WiFi.begin(ssid, pass);

  waitForWifi();

  if (!WiFi.isConnected()) {
    LE("wifi connect failed");
    return false;
  }

  LI("wifi connected ssid '%s', rssi %d, bssid %s",
     WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.BSSIDstr().c_str());
  return true;
}

time_t pollTime() {
  configTime(0, 0, addr::TIME3, addr::TIME2, addr::TIME1);

  time_t now = 0;
  uint64_t deadline = millis() + c::NTP_MAX_WAIT_MILLIS;

  while (millis() < deadline) {
    now = time(nullptr);
    if (now > c::TIME_MIN_EPOCH) {
      // set clock; arduino esp-idf 3.x doesn't have SNTP mode immediate
      timeval tv{};
      tv.tv_sec = now;
      tv.tv_usec = 0;
      if (settimeofday(&tv, nullptr) != 0) {
        LE("settimeofday failed? errno=%d", errno);
      }
      break;
    }
    delay(25);
  }
  LI("NTP time %s", timeStr(now).c_str());
  return now;
}

time_t setNtpTime() {
  time_t now;
  time_t prev_time;

  prev_time = time(nullptr);

  uint32_t attempt = 1;
  while (attempt < c::NTP_MAX_ATTEMPTS) {
    LI("NTP attempt %d/%d", attempt, c::NTP_MAX_ATTEMPTS);
    now = pollTime();
    if (now > c::TIME_MIN_EPOCH) {
      LI("Clock diff %ld seconds", now - prev_time);
      return now;
    }
    attempt++;
  }

  return 0;
}

void dumpResponse(WiFiClientSecure &client) {
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  Serial.flush();
}

#endif //GATE_SENSOR_NET_UTIL_H
