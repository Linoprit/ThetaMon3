#ifndef BME280HELPER_H
#define BME280HELPER_H

//#pragma once

#include <Adafruit_BME280.h>
#include "Measurement.h"
#include "MeasurementPivot.h"

namespace msmnt {

class Bme280Helper {
public:
  Bme280Helper(Measurement::SensorChannel channel, MeasurementPivot *measurementPivot);
  virtual ~Bme280Helper(){};

  void initHardware();
  void cycle();

private:
  Adafruit_BME280 _bme280; // I2C
  Measurement::SensorChannel _channel;
  MeasurementPivot *_measurementPivot;
  Measurement::SensorId _temperatureId;
  Measurement::SensorId _humidityId;
  Measurement::SensorId _pressureId;
};
} // namespace msmnt
#endif