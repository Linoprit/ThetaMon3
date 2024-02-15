#ifndef RELAYHELPER_H
#define RELAYHELPER_H

#pragma once

#include "Measurement.h"
#include "MeasurementPivot.h"
namespace msmnt {

class RelayHelper {
public:
  RelayHelper(MeasurementPivot *measurementPivot);
  virtual ~RelayHelper(){};

  void cycle();

private:
  MeasurementPivot *_measurementPivot;
  Measurement::SensorId _idRelayNr1;
  Measurement::SensorId _idRelayNr2;
};
} // namespace msmnt
#endif