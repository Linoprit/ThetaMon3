#include "Bme280Helper.h"
#include <Wire.h>

namespace msmnt {

#define SEALEVELPRESSURE_HPA (1013.25)

Bme280Helper::Bme280Helper(Measurement::SensorChannel channel,
                           MeasurementPivot *measurementPivot)
    : _channel(channel), _measurementPivot(measurementPivot) {

  if (BME280_I2CADDR == 0x76) {
    _temperatureId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BME_TEMP_76);
    _humidityId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BME_HUMIDITY_76);
    _pressureId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BME_PRESS_76);
  } else { // 0x77
    _temperatureId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BME_TEMP_77);
    _humidityId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BME_HUMIDITY_77);
    _pressureId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BME_PRESS_77);
  }

  _measurementPivot->StoreSensId(_temperatureId, _channel);
  _measurementPivot->StoreSensId(_humidityId, _channel,
                                 Measurement::SensorType::HUMIDITY);
  _measurementPivot->StoreSensId(_pressureId, _channel,
                                 Measurement::SensorType::PRESS);
}

void Bme280Helper::initHardware() {

  bool status = _bme280.begin(BME280_I2CADDR);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, "
                   "address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(_bme280.sensorID(), 16);
  }

  Serial.print("BME280 temperature Id: ");
  Serial.println(Measurement::DumpSensId(_temperatureId).c_str());
  Serial.print("BME280 humidity    Id: ");
  Serial.println(Measurement::DumpSensId(_humidityId).c_str());
  Serial.print("BME280 preassure   Id: ");
  Serial.println(Measurement::DumpSensId(_pressureId).c_str());

  // TODO set channelstatus to unconnected
}

void Bme280Helper::cycle() {
  // TODO if !channelstatus return

  _measurementPivot->UpdateValue(_temperatureId, _bme280.readTemperature());
  _measurementPivot->UpdateValue(_humidityId, _bme280.readHumidity());
  _measurementPivot->UpdateValue(_pressureId, _bme280.readPressure() / 100.0F);
}

} // namespace msmnt
