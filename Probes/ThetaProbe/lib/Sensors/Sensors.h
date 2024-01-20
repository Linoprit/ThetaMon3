#ifndef APPLICATION_SENSORS_SENSORS_H_
#define APPLICATION_SENSORS_SENSORS_H_

#include <Arduino.h>
#include <Ds1820Helper.h>
#include <Bme280Helper.h>
#include <MeasurementPivot.h>

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

private:
  MeasurementPivot _measurementPivot;
  Ds1820Helper _ds1820Ch1;
  Ds1820Helper _ds1820Ch2;
  Bme280Helper _bme280;
  // Todo BME280 _bme280;
};

} // namespace msmnt
#endif