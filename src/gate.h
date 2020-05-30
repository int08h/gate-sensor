#ifndef GATE_SENSOR_GATE_H
#define GATE_SENSOR_GATE_H

#include <driver/adc.h>

enum GateState {
  UNKNOWN = 0x01,
  CLOSED = 0x02,
  OPEN = 0x04
};

class Gate {
 public:
  // rarely, but occasionally, the ADC reading from the US5881 will be
  // above 0 but below 4095 full scale. Sensor is spec'ed min 3.5V but we're
  // driving it with 3.3V ... also bypass caps are indicated in the datasheet
  // but those aren't being used here either.
  static constexpr uint32_t THRESHOLD_LOW = 100;
  static constexpr uint32_t THRESHOLD_HIGH = 4000;

 public:
  Gate() {
    // GPIO33 is ADC1_CH5, 0 = gate closed, 4095 = gate open
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_WIDTH_BIT_12);
  }

  GateState current_state() {
    uint32_t value = current_value();

    if (value < THRESHOLD_LOW) {
      return CLOSED;
    } else if (value > THRESHOLD_HIGH) {
      return OPEN;
    } else {
      return UNKNOWN;
    }
  }

  uint32_t current_value() {
    return adc1_get_raw(ADC1_CHANNEL_5) & 0xffff;
  }

  static const char *to_str(GateState state) {
    switch (state) {
      case OPEN:
        return "OPEN";
      case CLOSED:
        return "CLOSED";
      case UNKNOWN:
      default:
        return "UNKNOWN";
    }
  }

};

#endif //GATE_SENSOR_GATE_H
