#ifndef GPIOINOUT_H
#define GPIOINOUT_H

#include <Arduino.h>
#include <Config.h>
#include <Sensors/Measurement.h>
#include <Sensors/MeasurementPivot.h>

#pragma once

namespace gpio {

using namespace msmnt;

class GpioInOut {
public:
  void init(void) { new (&instance()) GpioInOut(); };
  static GpioInOut &instance(void) {
    static GpioInOut gpioInOut;
    return gpioInOut;
  };
  void initHardware();

  GpioInOut() : _isTestTimerActive{false} {};
  virtual ~GpioInOut(){};

  void calcRelayStates(msmnt::MeasurementPivot *measurementPivot);

  void setLedAlive() { digitalWrite(LED_ALIVE_PIN, HIGH); }
  void clrLedAlive() { digitalWrite(LED_ALIVE_PIN, LOW); }
  void tglLedAlive() {
    digitalWrite(LED_ALIVE_PIN, !digitalRead(LED_ALIVE_PIN));
  }

  void setLedConnected() { digitalWrite(LED_CONNECTED_PIN, HIGH); }
  void clrLedConnected() { digitalWrite(LED_CONNECTED_PIN, LOW); }
  void tglLedConnected() {
    digitalWrite(LED_CONNECTED_PIN, !digitalRead(LED_CONNECTED_PIN));
  }

  void setRelayState(Measurement::RelayChannel relayNumber, bool state);
  bool getRelayState(Measurement::RelayChannel relayNumber);
  
  void setRelayCh1();
  void clrRelayCh1();
  void tglRelayCh1();

  void setRelayCh2();
  void clrRelayCh2();
  void tglRelayCh2();

  void tstRelay(uint8_t relayChannel, bool on);
  void setFlagTestTimer() { _isTestTimerActive = true; };
  void clrFlagTestTimer() { _isTestTimerActive = false; };

  void setRelayTestTimer();
  bool calcState(Measurement::RelayChannel relayNumber,
                 MeasurementPivot *measurementPivot);
  uint_fast8_t getRelayChannelCount(Measurement::RelayChannel relayNumber,
                                    MeasurementPivot *measurementPivot);

private:
  bool _isTestTimerActive;

};

} // namespace gpio

#endif