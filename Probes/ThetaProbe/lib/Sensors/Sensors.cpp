#include "Sensors.h"

namespace msmnt {

void Sensors::init(void) { new (&instance()) Sensors(); }

Sensors &Sensors::instance(void) {
  static Sensors sensors;
  return sensors;
}

Sensors::Sensors()
    : _ds1820Ch1(Measurement::CH_1, ONE_WIRE_CH1_PIN, DS18B20_PRECISION,
                 &_measurementPivot),
      _ds1820Ch2(Measurement::CH_2, ONE_WIRE_CH2_PIN, DS18B20_PRECISION,
                 &_measurementPivot),
      _bme280(Measurement::I2C_1, &_measurementPivot) {}

void Sensors::initHardware(void) {
  _ds1820Ch1.initHardware();
  _ds1820Ch2.initHardware();
  _bme280.initHardware();
}

void Sensors::cycle(void) {
  _ds1820Ch1.cycle();
  _ds1820Ch2.cycle();
  _bme280.cycle();
}

} // namespace msmnt
