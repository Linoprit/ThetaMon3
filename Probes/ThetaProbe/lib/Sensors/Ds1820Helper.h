#ifndef DS1820HELPER_H
#define DS1820HELPER_H

#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include <MeasurementPivot.h>
#include <Measurement.h>

namespace msmnt {

class Ds1820Helper {
public:
  Ds1820Helper(Measurement::SensorChannel channel, uint8_t oneWirePin, uint8_t resolution,
               MeasurementPivot *measurementPivot);
  ~Ds1820Helper(){};

  void initHardware();
  void cycle();

private:
  OneWire _oneWireCh;
  DallasTemperature _sensorsCh;  
  Measurement::SensorChannel _channel;
  MeasurementPivot *_measurementPivot;
};
} // namespace msmnt
#endif