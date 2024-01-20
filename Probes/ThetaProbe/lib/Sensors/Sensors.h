#ifndef APPLICATION_SENSORS_SENSORS_H_
#define APPLICATION_SENSORS_SENSORS_H_

#include <Arduino.h>
#include <Ds1820Helper.h>
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
  // Todo Ds1820Helper _ds1820Ch2;
  // Todo BMS280 _bms280;
};

} // namespace msmnt
#endif