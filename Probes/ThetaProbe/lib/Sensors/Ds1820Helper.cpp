#include "Ds1820Helper.h"

namespace msmnt {

using namespace std;

Ds1820Helper::Ds1820Helper(Measurement::SensorChannel channel,
                           uint8_t oneWirePin, uint8_t resolution,
                           MeasurementPivot *measurementPivot)
    : _oneWireCh(oneWirePin), _sensorsCh(&_oneWireCh), _channel(channel),
      _measurementPivot(measurementPivot) {
  _sensorsCh.setResolution(resolution);
}

void Ds1820Helper::initHardware(void) {
  uint8_t address[8];
  _sensorsCh.begin();
  _oneWireCh.reset_search();

  Serial.printf("Searching oneWire on channel %s\n",
                Measurement::DumpSensChannel(_channel).c_str());

  while (_oneWireCh.search(address)) {
    Serial.print("ROM = ");
    Serial.println(Measurement::DumpSensIdArray(address).c_str());

    if (OneWire::crc8(address, 7) != address[7]) {
      Serial.println("CRC is not valid!");
      continue;
    }
    _measurementPivot->StoreSensId(address, _channel);
  }
}

void Ds1820Helper::cycle() {
  _sensorsCh.requestTemperatures();
  delay(200);

  _measurementPivot->ResetIter();

  Measurement *actMeasurement;
  for (uint_fast8_t i = 0; i < MAX_SENSORS; i++) {
    actMeasurement = _measurementPivot->GetNextMeasurement();
    if (actMeasurement == nullptr) {

      break;
    }
    if (actMeasurement->sensChan != _channel) {
      continue;
    }

    Measurement::SensorId sensId = actMeasurement->sensorId;
    float temperature =
        _sensorsCh.getTempC(Measurement::CastSensIdToArray(&sensId));
    if (temperature <= DEVICE_DISCONNECTED_C) {
      continue;
    }

    bool updateRes = _measurementPivot->UpdateValue(sensId, temperature);
    if (!updateRes) {

      string idStr = Measurement::DumpSensId(sensId);
      string shortName = string(actMeasurement->shortname);
      Serial.printf("Updating value failed: %s / %s", idStr.c_str(),
                    shortName.c_str());
    }
  }
}

} // namespace msmnt