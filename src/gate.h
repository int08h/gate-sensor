#ifndef GATE_SENSOR_GATE_H
#define GATE_SENSOR_GATE_H

#include <driver/adc.h>

enum GateState {
    UNKNOWN = 0,
    CLOSED = 1,
    OPEN = 2
};

class Gate {
public:
    Gate() {
        // GPIO33 is ADC1_CH5, 0 = gate closed, 4095 = gate open
        adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
        adc1_config_width(ADC_WIDTH_BIT_12);
    }

    GateState measure() const {
        int value = adc1_get_raw(ADC1_CHANNEL_5);

        if (value < THRESHOLD_LOW) {
            return CLOSED;
        } else if (value > THRESHOLD_HIGH) {
            return OPEN;
        } else {
            return UNKNOWN;
        }
    }

    static const char* to_str(GateState state) {
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

private:
    // rarely, but occasionally, the ADC reading from the US5881 will be
    // above 0 but below 4095 full scale. Sensor is spec'ed min 3.5V but we're
    // driving it with 3.3V ... also bypass caps are indicated in the datasheet
    // but those aren't being used here either.
    static const int THRESHOLD_LOW = 200;
    static const int THRESHOLD_HIGH = 3000;
};

#endif //GATE_SENSOR_GATE_H
