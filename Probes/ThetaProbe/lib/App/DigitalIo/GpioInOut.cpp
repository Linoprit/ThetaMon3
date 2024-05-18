#include "GpioInOut.h"
#include <AppTypes.h>

namespace gpio {

TimerHandle_t relayTestTimer;

void relayTestTimerCallback() {
  GpioInOut::instance().clrFlagTestTimer();
  GpioInOut::instance().clrRelayCh1();
  GpioInOut::instance().clrRelayCh2();
}

void GpioInOut::initHardware() {
  pinMode(LED_ALIVE_PIN, OUTPUT);
  pinMode(LED_CONNECTED_PIN, OUTPUT);
  pinMode(LED_DEBUG_PIN, OUTPUT);
  pinMode(RELAY_CH1_PIN, OUTPUT);
  pinMode(RELAY_CH2_PIN, OUTPUT);

  relayTestTimer = xTimerCreate(
      "RelayTestTimer", pdMS_TO_TICKS(4000), pdFALSE, (void *)0,
      reinterpret_cast<TimerCallbackFunction_t>(relayTestTimerCallback));
}

void GpioInOut::calcRelayStates(msmnt::MeasurementPivot *measurementPivot) {
  bool stateRelay1 =
      calcState(Measurement::RelayChannel::REL_01, measurementPivot);
  setRelayState(Measurement::RelayChannel::REL_01, stateRelay1);

  bool stateRelay2 =
      calcState(Measurement::RelayChannel::REL_02, measurementPivot);
  setRelayState(Measurement::RelayChannel::REL_02, stateRelay2);
}



void GpioInOut::setRelayState(Measurement::RelayChannel relayNumber,
                              bool state) {
  switch (relayNumber) {
  case Measurement::RelayChannel::REL_01:
    if (state == true) {
      setRelayCh1();
    } else {
      clrRelayCh1();
    }
    break;
  case Measurement::RelayChannel::REL_02:
    if (state == true) {
      setRelayCh2();
    } else {
      clrRelayCh2();
    }
    break;
  default:
    break;
  }
}

bool GpioInOut::getRelayState(Measurement::RelayChannel relayNumber) {
  switch (relayNumber) {
  case Measurement::RelayChannel::REL_01:
    return static_cast<bool>(digitalRead(RELAY_CH1_PIN));
    break;
  case Measurement::RelayChannel::REL_02:
    return static_cast<bool>(digitalRead(RELAY_CH2_PIN));
    break;
  default:
    return false;
    break;
  }
}

bool GpioInOut::calcState(Measurement::RelayChannel relayNumber,
                          msmnt::MeasurementPivot *measurementPivot) {
  if (relayNumber == 0) {
    return _off_;
  }

  bool currentState = getRelayState(relayNumber);

  uint_fast8_t relayMaxCount = 0;
  uint_fast8_t relayChannelCount =
      getRelayChannelCount(relayNumber, measurementPivot);

  measurementPivot->ResetIter();
  Measurement *actMeasurement = measurementPivot->GetNextMeasurement();
  while (actMeasurement != nullptr) {
      if ((actMeasurement->relayNr == 0) ||
          (actMeasurement->relayNr != relayNumber)) {
        actMeasurement = measurementPivot->GetNextMeasurement();
        continue;
      }
      if (actMeasurement->meanValue < actMeasurement->minVal) {
        return _on_;
      }
      if (actMeasurement->meanValue > actMeasurement->maxVal) {
        relayMaxCount++;
      }
    actMeasurement = measurementPivot->GetNextMeasurement();
  }
  if ((relayMaxCount >= relayChannelCount) && (currentState == _on_)) {
    return _off_;
  }

  return currentState;
}

uint_fast8_t
GpioInOut::getRelayChannelCount(Measurement::RelayChannel relayNmber,
                                msmnt::MeasurementPivot *measurementPivot) {
  uint_fast8_t result = 0;

  measurementPivot->ResetIter();
  Measurement *actMeasurement = measurementPivot->GetNextMeasurement();
  while (actMeasurement != nullptr) {
    if (actMeasurement->relayNr == relayNmber) {
      result++;
    }
    actMeasurement = measurementPivot->GetNextMeasurement();
  }
  return result;
}

void GpioInOut::setRelayCh1() {
  if (_isTestTimerActive) {
    return;
  }
  digitalWrite(RELAY_CH1_PIN, HIGH);
}
void GpioInOut::clrRelayCh1() {
  if (_isTestTimerActive) {
    return;
  }
  digitalWrite(RELAY_CH1_PIN, LOW);
}
void GpioInOut::tglRelayCh1() {
  if (_isTestTimerActive) {
    return;
  }
  digitalWrite(RELAY_CH1_PIN, !digitalRead(RELAY_CH1_PIN));
}

void GpioInOut::setRelayCh2() {
  if (_isTestTimerActive) {
    return;
  }
  digitalWrite(RELAY_CH2_PIN, HIGH);
}
void GpioInOut::clrRelayCh2() {
  if (_isTestTimerActive) {
    return;
  }
  digitalWrite(RELAY_CH2_PIN, LOW);
}
void GpioInOut::tglRelayCh2() {
  if (_isTestTimerActive) {
    return;
  }
  digitalWrite(RELAY_CH2_PIN, !digitalRead(RELAY_CH2_PIN));
}

void GpioInOut::tstRelay(uint8_t relayChannel, bool on) {

  setRelayTestTimer();

  switch (relayChannel) {
  case 1:
    if (on) {
      digitalWrite(RELAY_CH1_PIN, HIGH);
    } else {
      digitalWrite(RELAY_CH1_PIN, LOW);
    }
    break;
  case 2:
    if (on) {
      digitalWrite(RELAY_CH2_PIN, HIGH);
    } else {
      digitalWrite(RELAY_CH2_PIN, LOW);
    }
    break;
  default:
    break;
  }
}

void GpioInOut::setRelayTestTimer() {
  _isTestTimerActive = true;
  xTimerStop(relayTestTimer, 0);
  xTimerStart(relayTestTimer, 0);
}

} // namespace gpio
