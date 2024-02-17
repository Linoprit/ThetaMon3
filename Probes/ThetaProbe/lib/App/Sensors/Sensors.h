#ifndef APPLICATION_SENSORS_SENSORS_H_
#define APPLICATION_SENSORS_SENSORS_H_

#include "Bme280Helper.h"
#include "Ds1820Helper.h"
#include "MeasurementPivot.h"
#include "RelayHelper.h"
#include "Wifi/MqttHelper.h"
#include <Arduino.h>

namespace msmnt {

class Sensors {
public:
  Sensors();
  virtual ~Sensors(){};

  void init(void);
  void initHardware(void);
  static Sensors &instance(void);

  void cycle(void);
  MeasurementPivot *getMeasurementPivot() { return &_measurementPivot; };
  bool saveSensorIdTable();

private:
  MeasurementPivot _measurementPivot;
  Ds1820Helper _ds1820Ch1;
  Ds1820Helper _ds1820Ch2;
  Bme280Helper _bme280;
  RelayHelper _relays;
  uint_fast8_t _updateCount;
};

} // namespace msmnt
#endif