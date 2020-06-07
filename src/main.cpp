/**
 * ESP32 based sensor for detecting open/close condition of a Hall-effect sensor
 * and communicating events via Wifi to Google Cloud IoT Core.
 *
 * Copyright (c) 2020 int08h LLC.
 * Apache 2.0 licensed https://www.apache.org/licenses/LICENSE-2.0
 */

#include "main.h"

// Program always starts (and ends) here; loop() is never used.
void setup() {
  rtcd::ts_wake = time(nullptr);
  rtcd::cpu_wakeups += 1;
  rtcd::ulp_sensor_checks += ulp_read(rtcd::ulp_check_acc);
  rtcd::ulp_out_of_range += ulp_read(rtcd::ulp_oor_acc);
  rtcd::ulp_check_acc = 0;
  rtcd::ulp_oor_acc = 0;

  Serial.begin(115200);

  if (isInitialBoot()) {
    handleBoot();
  }

  handleWakeup();
  enterDeepSleep(c::DEEP_SLEEP_USEC);
}

bool isInitialBoot() {
  return rtcd::ts_boot == 0;
}

void handleBoot() {
  LI("initial boot");

  if (!connectWifi()) {
    return;
  }

  time_t now = setNtpTime();
  rtcd::ts_boot = now;
  rtcd::ts_wake = now;
  rtcd::ts_next_telemetry = now + 1;

  Gate gate{};
  rtcd::last_state = gate.current_state();
  rtcd::ulp_a1c5_value = gate.current_value();

  prepareUlp();
}

void handleWakeup() {
  time_t sleep_dur = rtcd::ts_wake - rtcd::ts_start_sleep;
  LI("wake cpu:%llu ulp:%llu (oor %llu); slept %d sec (reason: %s)",
     rtcd::cpu_wakeups, rtcd::ulp_sensor_checks, rtcd::ulp_out_of_range, sleep_dur, wake_reason());

  Gate gate{};
  GateState curr_state = gate.current_state();

  if (curr_state != rtcd::last_state) {
    LI("Gate change %s -> %s", gate.to_str(rtcd::last_state), gate.to_str(curr_state));
    rtcd::last_state = curr_state;
    handleGateChange(curr_state);
  } else {
    LI("Gate remains %s", gate.to_str(curr_state));
  }

  time_t now = time(nullptr);
  if (now > rtcd::ts_next_telemetry) {
    LI("Deadline to send telemetry (now %s > %s)",
       timeStr(now).c_str(), timeStr(rtcd::ts_next_telemetry).c_str());
    rtcd::ts_next_telemetry = now + c::TELEMETRY_SEND_SEC;
    handleSendTelemetry();
  }
}

void handleGateChange(GateState state) {
  if (!connectWifi()) {
    return;
  }

  time_t now = setNtpTime();
  LI("Sending Pushover event %s at %s", Gate::to_str(state), timeStr(now).c_str());

  WiFiClientSecure client = WiFiClientSecure();
  client.setTimeout(/*secs*/10);

  sendPushoverEvent(client, state);

  client.stop();
}

void handleSendTelemetry() {
  if (!connectWifi()) {
    return;
  }

  time_t now = setNtpTime();

  if (rtcd::jwt.isExpired(now)) {
    rtcd::jwt.regenerate(now);
  }

  WiFiClientSecure client = WiFiClientSecure();
  client.setTimeout(/*secs*/10);

  sendIotTelemetry(client);

  client.stop();
}

// Send telemetry to GCP IoT Core
bool sendIotTelemetry(WiFiClientSecure &client) {
  client.setCACert(certs::google_root_bundle);

  HTTPClient req;
  req.setConnectTimeout(/*ms*/10 * 1000);

  String url = String(addr::GOOGLE_IOT_URL) + String(secrets::DEVICE_ID_GOOGLE) + ":publishEvent";
  req.begin(client, url);
  req.addHeader("Authorization", rtcd::jwt.asHeader());
  req.addHeader("Content-Type", "application/json");
  req.addHeader("Cache-Control", "no-cache");

  time_t now = time(nullptr);
  char ptmp[256];
  snprintf(ptmp, sizeof(ptmp), c::GCP_STATE_FMT, now, WiFi.RSSI(),
           rtcd::last_state, rtcd::sent_events, rtcd::sent_telemetry,
           rtcd::cpu_wakeups, rtcd::ulp_sensor_checks, rtcd::ulp_out_of_range);
  char b64tmp[512];
  base64url_encode((unsigned char *) ptmp, strlen(ptmp), b64tmp);

  String payload = String("{\"binary_data\":\"") + b64tmp + "\"}";

  int code = req.POST(payload);
  rtcd::sent_telemetry++;
  LI("POST to Google complete, code = %d", code);

  if (code != 200) {
    dumpResponse(client);
  }

  return code == 200;
}

// Send mobile alert to Pushover
bool sendPushoverEvent(WiFiClientSecure &client, GateState state) {
  client.setCACert(certs::digicert_ca);

  HTTPClient req;
  req.setConnectTimeout(/*ms*/10 * 1000);

  req.begin(client, addr::PO_URL);
  req.addHeader("Content-Type", "application/x-www-form-urlencoded");
  req.addHeader("Cache-Control", "no-cache");

  String payload = String("message=")
      + String(secrets::DEVICE_ID_HUMAN)
      + " is now "
      + Gate::to_str(state);

  payload.replace(' ', '+');

  payload += "&token=";
  payload += secrets::PO_TOKEN;
  payload += "&user=";
  payload += secrets::PO_USER;
  payload += "&device=";
  payload += secrets::DEVICE_ID_PO;

  int code = req.POST(payload);
  rtcd::sent_events++;
  LI("POST to Pushover complete, code = %d", code);

  if (code != 200) {
    dumpResponse(client);
  }

  return code == 200;
}

void prepareUlp() {
  size_t inst_size = sizeof(ulp_insn_t);
  size_t num_words = sizeof(gate_ulp::program) / inst_size;
  ESP_ERROR_CHECK(ulp_process_macros_and_load(0, gate_ulp::program, &num_words));
  LI("loaded %u byte ULP program", num_words * inst_size);
}

void enterDeepSleep(uint32_t duration_usec) {
  LI("preparing for %d us deep sleep", duration_usec);
  WiFi.disconnect(true);

  // Suppress boot messages
  esp_deep_sleep_disable_rom_logging();

  // Prevent current drain from internal pull up on flash controller
  rtc_gpio_isolate(GPIO_NUM_12);
  rtc_gpio_isolate(GPIO_NUM_15);

  // Prepare ADC1 to be read by ULP; GPIO33 is ADC1_CH5
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_ulp_enable();

  LI("starting ULP");
  ESP_ERROR_CHECK(ulp_set_wakeup_period(0, c::ULP_SLEEP_USEC));
  ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());
  ESP_ERROR_CHECK(ulp_run(0));

  LI("entering deep sleep");
  time(&rtcd::ts_start_sleep);
  esp_deep_sleep(duration_usec);
}

const char *wake_reason() {
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_ULP) {
    return "ULP wakeup";
  }

  switch (esp_reset_reason()) {
    case ESP_RST_POWERON:
      return "Reset due to power-on event";
    case ESP_RST_EXT:
      return "Reset by external pin";
    case ESP_RST_SW:
      return "Software reset via esp_restart";
    case ESP_RST_PANIC:
      return "Software reset due to exception/panic";
    case ESP_RST_INT_WDT:
      return "Reset (software or hardware) due to interrupt watchdog";
    case ESP_RST_TASK_WDT:
      return "Reset due to task watchdog";
    case ESP_RST_WDT:
      return "Reset due to other watchdogs";
    case ESP_RST_DEEPSLEEP:
      return "Reset after exiting deep sleep mode";
    case ESP_RST_BROWNOUT:
      return "Brownout reset (software or hardware)";
    case ESP_RST_SDIO:
      return "Reset over SDIO";
    case ESP_RST_UNKNOWN:
    default:
      return "Reset reason can not be determined";
  }
}

__unused void loop() {}
