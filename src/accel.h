#ifndef GATE_SENSOR_SRC_ACCEL_H_
#define GATE_SENSOR_SRC_ACCEL_H_

#include <SparkFun_ADXL345.h>

#include "mlog.h"

class Adxl343 {
 public:
  Adxl343() : accel() {}

  boolean init() {
    accel.powerOn();
    accel.setRangeSetting(8);
    accel.setRate(25);
    accel.setLowPower(true);

    int x, y, z;
    while (true) {
      accel.readAccel(&x, &y, &z);
      LI("X: %d, Y: %d, Z: %d", x, y, z);
      delay(100);
    }
    return true;
  }

 private:
  ADXL345 accel;
};

#endif //GATE_SENSOR_SRC_ACCEL_H_
