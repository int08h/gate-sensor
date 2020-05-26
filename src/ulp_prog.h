#ifndef GATE_SENSOR_ULP_PROG_H
#define GATE_SENSOR_ULP_PROG_H

#include "ulp_inst.h"

namespace gate_ulp {

    const uint32_t LABEL_HALT = 1;

    const ulp_insn_t program[]{
            // R3 serves as the zero register for HULP macros
            I_MOVI(R3, 0),

            // Increment the ULP wake accumulator
            I_GET(R0, R3, rtcd::ulp_acc),
            I_ADDI(R0, R0, 1),
            I_PUT(R0, R3, rtcd::ulp_acc),

            // Load last ADC value into R2
            I_GET(R2, R3, rtcd::ulp_adc5_value),

            // Measure ADC1 channel 5 into R0
            I_ADC(R0, 0, ADC1_CHANNEL_5),

            I_ADC_POWER_OFF(),

            // Store measured value
            I_PUT(R0, R3, rtcd::ulp_adc5_value),

            // Compare prior gate state (R2) to current value (R0)
            I_SUBR(R0, R2, R0),
            // If unchanged state (R2-R0=0), jump to LABEL_HALT
            M_BXZ(LABEL_HALT),
              // On state change, disable ULP program timer
              I_END(),
              // And wake CPU
              M_WAKE_WHEN_READY(),

            // Halt the ULP
            M_LABEL(LABEL_HALT),
            I_HALT()
    };
}

#endif //GATE_SENSOR_ULP_PROG_H

