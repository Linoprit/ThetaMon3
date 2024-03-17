#include "FileSystem/LittleFsHelpers.h"
#include "Sensors.h"
#include <DigitalIo/GpioInOut.h>
#include <sys/time.h>

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
      _bme280(Measurement::I2C_1, &_measurementPivot),
      _relays(&_measurementPivot) {}

void Sensors::initHardware(void) {
  wifi::MqttHelper::instance().init();
  wifi::MqttHelper::instance().MqttSetup();
  delay(1000);

  _ds1820Ch1.initHardware();
  _ds1820Ch2.initHardware();
  _bme280.initHardware();
  delay(100);

  // MqttSetup and readIdTable push commands to the interpreter-queue.
  // Thats, why this must happen in the same task.
  nvm::LittleFsHelpers::instance().readIdTable();
  _measurementPivot.ResetConfigChangedFlags();

}

void Sensors::cycle(void) {
    _ds1820Ch1.cycle();
    _ds1820Ch2.cycle();
    _bme280.cycle();
    _relays.cycle();

  _updateCount++;
  if (_updateCount >= Measurement::VALUES_BUFF_LEN) {
    wifi::MqttHelper::instance().pubishMeasurements(&_measurementPivot);
    _updateCount = 0;
  }
}

bool Sensors::saveSensorIdTable() {
  return nvm::LittleFsHelpers::instance().saveSensIdTable(&_measurementPivot);
}

} // namespace msmnt
