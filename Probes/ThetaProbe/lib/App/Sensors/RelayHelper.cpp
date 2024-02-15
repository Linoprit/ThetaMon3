#include "RelayHelper.h"
#include <DigitalIo/GpioInOut.h>

namespace msmnt {

RelayHelper::RelayHelper(MeasurementPivot *measurementPivot):
    _measurementPivot(measurementPivot) {
  _idRelayNr1 =
      MeasurementPivot::CreateId(MeasurementPivot::SensorKindType::RELAY_01);
  _idRelayNr2 =
      MeasurementPivot::CreateId(MeasurementPivot::SensorKindType::RELAY_02);

  _measurementPivot->StoreSensId(_idRelayNr1, Measurement::SensorChannel::GPIO);
  _measurementPivot->StoreSensId(_idRelayNr2, Measurement::SensorChannel::GPIO);
}

void RelayHelper::cycle() {
  gpio::GpioInOut::instance().calcRelayStates(_measurementPivot);
  _measurementPivot->UpdateValue(_idRelayNr1, digitalRead(RELAY_CH1_PIN));
  _measurementPivot->UpdateValue(_idRelayNr2, digitalRead(RELAY_CH2_PIN));
}

} // namespace msmnt