#ifndef APPLICATION_SENSORS_MEASUREMENTS_H_
#define APPLICATION_SENSORS_MEASUREMENTS_H_

#include "Config.h"
#include <array>
#include <stdint.h>

namespace msmnt {
typedef std::array<Measurement, MAX_SENSORS> MeasurementArray;

// used to update measurement in the array
typedef struct {
  uint8_t sensorIdHash[8];
  float actValue;
  uint32_t lastUpdateTick;
} MeasurementType;

class __attribute__((__packed__)) Measurement {
public:
  static constexpr uint16_t SHORTNAME_LEN = 8;
  static constexpr uint16_t INVLD_TEMPERATURE = 255;
  static constexpr uint16_t MIN_TEMP_HYSTERESIS = 1;
  static constexpr uint16_t SENSOR_ID_LEN = 8;

  enum SensorType {
    TEMP = 0,
    HUMIDITY = 1,
    PRESS = 2,
    RELAY = 3,
  };
  enum RelayChannel {
    NONE = 0,
    REL_01 = 1,
    REL_02 = 2,
  };

  uint8_t sensorIdHash[8]; // 8 bytes
  float actValue;
  uint32_t lastUpdateTick; // in seconds.
  float minVal;            // switch relay on, if actValue below this.
  float maxVal;            // switch relay off, if actValue above this.
  bool isValid;            // timeout, out of range, etc.
  SensorType sensType;
  RelayChannel relayNr; // which channel to switch; 0 = no relay.
  char shortname[Measurement::SHORTNAME_LEN]; // without \0.

  Measurement(){};
  virtual ~Measurement() {}
};

} // namespace msmnt

#endif /* APPLICATION_SENSORS_MEASUREMENTS_H_ */