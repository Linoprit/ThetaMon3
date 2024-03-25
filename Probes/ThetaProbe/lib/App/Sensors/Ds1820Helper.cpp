#include "Ds1820Helper.h"
#include <Wifi/MqLog.h>

namespace msmnt {

using namespace std;

Ds1820Helper::Ds1820Helper(Measurement::SensorChannel channel,
                           uint8_t oneWirePinOut, uint8_t oneWirePinIn,
                           uint8_t resolution,
                           MeasurementPivot *measurementPivot)
    : _oneWireCh(oneWirePinOut, oneWirePinIn), _sensorsCh(&_oneWireCh),
      _channel(channel), _measurementPivot(measurementPivot) {
  _sensorsCh.setResolution(resolution);
}

void Ds1820Helper::initHardware(void) {
  uint8_t address[8];
  uint8_t senorCount = 0;
  _sensorsCh.begin();
  _oneWireCh.reset_search();

  MqLog("Searching oneWire on channel %s\n",
        Measurement::DumpSensChannel(_channel).c_str());

  while (_oneWireCh.search(address)) {
    MqLog("ROM = %s\n", Measurement::DumpSensIdArray(address).c_str());

    if (OneWire::crc8(address, 7) != address[7]) {
      MqLog("CRC is not valid!\n");
      continue;
    }
    _measurementPivot->StoreSensId(address, _channel);
    senorCount++;
  }
  MqLog("Found sensors: %i\n", senorCount);
}

void Ds1820Helper::cycle() {
  DallasTemperature::request_t req = _sensorsCh.requestTemperatures();
  // delay(200);


  if (!req.result){
    MqLog("request Temp failed.");
  }


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
      MqLog("Updating value failed: %s / %s. Value: %.02f\n", idStr.c_str(),
            shortName.c_str(), temperature);
    }
  }
}

} // namespace msmnt