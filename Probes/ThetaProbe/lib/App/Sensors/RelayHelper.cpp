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

  bool rel1_val = 
    gpio::GpioInOut::instance().getRelayState(msmnt::Measurement::REL_01);
  bool rel2_val = 
    gpio::GpioInOut::instance().getRelayState(msmnt::Measurement::REL_02);

  _measurementPivot->UpdateValue(_idRelayNr1, rel1_val);
  _measurementPivot->UpdateValue(_idRelayNr2, rel2_val);
}

} // namespace msmnt