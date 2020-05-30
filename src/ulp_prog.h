#ifndef GATE_SENSOR_ULP_PROG_H
#define GATE_SENSOR_ULP_PROG_H

#include "ulp_inst.h"

namespace gate_ulp {

enum labels {
  L_HALT = 1,
  L_WAKE = 2,
  L_SENSOR_HIGH = 3,
  L_SENSOR_LOW = 4,
};

const ulp_insn_t program[]{
    // R3 serves as the zero register for HULP macros
    I_MOVI(R3, 0),

    // Increment the ULP wake accumulator
    I_GET(R0, R3, rtcd::ulp_check_acc),
    I_ADDI(R0, R0, 1),
    I_PUT(R0, R3, rtcd::ulp_check_acc),

    // Load previous ADC value into R2
    I_GET(R2, R3, rtcd::ulp_a1c5_value),

    I_ADC_POWER_ON(),

    // Measure ADC1 channel 5 into R1
    I_ADC(R1, 0, ADC1_CHANNEL_5),

    I_ADC_POWER_OFF(),

    // Store measured value
    I_PUT(R1, R3, rtcd::ulp_a1c5_value),
    // Copy measurement to R0 for branch instructions
    I_MOVR(R0, R1),

    // if val > high threshold
    M_BGE(L_SENSOR_HIGH, Gate::THRESHOLD_HIGH),
    // if val < low threshold
    M_BL(L_SENSOR_LOW, Gate::THRESHOLD_LOW),
    // else increment the ULP out-of-range accumulator
    I_GET(R0, R3, rtcd::ulp_oor_acc),
    I_ADDI(R0, R0, 1),
    I_PUT(R0, R3, rtcd::ulp_oor_acc),
    // and halt
    M_BX(L_HALT),

    M_LABEL(L_SENSOR_HIGH),
    I_MOVI(R1, GateState::OPEN),
    I_GET(R0, R3, rtcd::last_state),
    I_ANDR(R0, R0, R1),
    // if (cur & prev != 0) then no change
    M_BGE(L_HALT, 1),
    // state *has* changed
    M_BX(L_WAKE),

    M_LABEL(L_SENSOR_LOW),
    I_MOVI(R1, GateState::CLOSED),
    I_GET(R0, R3, rtcd::last_state),
    I_ANDR(R0, R0, R1),
    // if (cur & prev != 0) then no change
    M_BGE(L_HALT, 1),
    // state *has* changed
    M_BX(L_WAKE),

    M_LABEL(L_WAKE),
    // Cancel the ULP wake timer
    I_END(),
    // And wake CPU
    M_WAKE_WHEN_READY(),

    // Halt the ULP
    M_LABEL(L_HALT),
    I_HALT()
};
}

#endif //GATE_SENSOR_ULP_PROG_H

