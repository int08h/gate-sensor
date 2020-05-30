// Contains portions of https://github.com/boarchuz/HULP
// Copyright (c) 2019 Matt
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GATE_SENSOR_ULP_INST_H
#define GATE_SENSOR_ULP_INST_H

#include <driver/gpio.h>
#include <driver/rtc_io.h>
#include <esp32/ulp.h>
#include <soc/rtc.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/rtc_io_reg.h>
#include <soc/sens_reg.h>
#include <soc/rtc_periph.h>
#include <soc/rtc_i2c_reg.h>

/**
 * Read a bit from peripheral register into R0
 *
 * This instruction can access RTC_CNTL_, RTC_IO_, SENS_, and RTC_I2C peripheral registers.
 */
#define I_RD_REG_BIT(reg, shift) I_RD_REG(reg, shift, shift)

/**
 * Check if RTC controller is ready to wake the SoC
 */
#define I_GET_WAKE_READY() I_RD_REG_BIT(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP_S)

/**
 * I_WAKE when the SoC is ready, waiting if necessary
 */
#define M_WAKE_WHEN_READY() \
    I_GET_WAKE_READY(), \
    I_BL(-1, 1), \
    I_WAKE()

#define RTC_WORD_OFFSET(x) ((uint32_t*)&(x) - RTC_SLOW_MEM)

/**
 * Load the value of a ulp_var_t into a general purpose register.
 *
 *  reg_dest: General purpise register to load value into (R0-R3).
 *  reg_zero: General purpise register containing 0. May be the same as reg_dest.
 *  var: A variable of type ulp_var_t or similar.
 *
 *  eg.
 *      I_MOVI(R3, 0),                  // A register must have known value 0 to use I_GET/I_PUT.
 *      I_GET(R0, R3, ulp_my_variable), // Get ulp_my_variable's value into R0.
 *      I_ADDI(R0, R0, 1),              // R0 = R0 + 1
 *      I_PUT(R0, R3, ulp_my_variable), // Put the value of R0 into ulp_my_variable.
 */
#define I_GET(reg_dest, reg_zero, var) \
    I_GETO(reg_dest, reg_zero, 0, var)

/**
 * A more flexible variant of I_GET that allows specifying the value (val_offset) of the offset register (reg_offset).
 *  Note: undefined behaviour may occur if val_offset > offset of var. This is not a concern if your program is at default address 0.
 *
 *  reg_dest: General purpise register to load value into (R0-R3).
 *  reg_offset: General purpose register containing a known value. May be the same as reg_src.
 *  val_offset: The value of reg_offset.
 *  var: A 4-byte, word-aligned variable in RTC_SLOW_MEM. Use HULP's ulp_var_t family for convenience.
 *
 */
#define I_GETO(reg_dest, reg_offset, val_offset, var) \
    I_LD(reg_dest, reg_offset, (uint16_t)(RTC_WORD_OFFSET(var) - (val_offset)))

/**
 * Store the value of a general purpose register into ulp_var_t.
 *
 *  reg_src: General purpise register containing the value to be stored (R0-R3)
 *  reg_zero: General purpise register containing 0. May be the same as reg_src (if storing 0).
 *  var: a ulp_var_t or similar.
 *
 *  eg.
 *      I_MOVI(R3, 0),                          // A register must have known value 0 to use I_GET/I_PUT.
 *      I_MOVI(R0, 123),
 *      I_PUT(R0, R3, ulp_my_variable),         // Put R0's value in ulp_my_variable.
 *      I_MOVI(R1, 1),
 *      I_PUT(R1, R3, ulp_another_variable),    // Put R1's value in ulp_another_variable.
 */
#define I_PUT(reg_src, reg_zero, var) \
    I_PUTO(reg_src, reg_zero, 0, var)

/**
 * A more flexible variant of I_PUT that allows specifying the value (val_offset) of the offset register (reg_offset).
 *  Note: undefined behaviour may occur if val_offset > offset of var. This is not a concern if your program is at default address 0.
 *
 *  reg_src: General purpose register containing the value to be stored (R0-R3)
 *  reg_offset: General purpose register containing a known value. May be the same as reg_src.
 *  val_offset: The value of reg_offset.
 *  var: A 4-byte, word-aligned variable in RTC_SLOW_MEM. Use HULP's ulp_var_t family for convenience.
 */
#define I_PUTO(reg_src, reg_offset, val_offset, var) \
    I_ST(reg_src, reg_offset, (uint16_t)(RTC_WORD_OFFSET(var) - (val_offset)))


#define I_ADC_POWER_OFF() I_WR_REG(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR_S, SENS_FORCE_XPD_SAR_S + 1, SENS_FORCE_XPD_SAR_PD)

#define I_ADC_POWER_ON() I_WR_REG(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR_S, SENS_FORCE_XPD_SAR_S + 1, SENS_FORCE_XPD_SAR_PU)


#endif //GATE_SENSOR_ULP_INST_H
