#ifndef APPLICATION_SENSORS_MEASUREMENTS_H_
#define APPLICATION_SENSORS_MEASUREMENTS_H_

#include "Config.h"
#include <Arduino.h>
#include <array>
#include <cstring>
#include <limits>
#include <math.h>
#include <stdint.h>

namespace msmnt {

/* We have to make sure:
    - Every instance in initialised with INVALID_SENS_ID.
    - A measurement with INVALID_SENS_ID contains no valid data.
*/
class __attribute__((__packed__)) Measurement {
public:
  static constexpr uint16_t SHORTNAME_LEN = 8;
  static constexpr uint16_t SENS_ID_LEN = 8;
  static constexpr uint16_t INVLD_TEMPERATURE = 255;
  static constexpr uint16_t MIN_TEMP_HYSTERESIS = 1;
  static constexpr uint16_t SENSOR_ID_LEN = 8;
  static constexpr uint16_t VALUES_BUFF_LEN = 5;
  static constexpr uint64_t INVALID_SENS_ID =
      std::numeric_limits<uint64_t>::max();
  typedef uint8_t
      SensorIdArray[SENS_ID_LEN]; // same as DallasTemperature::DeviceAddress
  typedef uint64_t SensorId;      // SensorIdArray casted to Integer

  enum SensorType {
    TEMP = 0,
    HUMIDITY = 1,
    PRESS = 2,
    RELAY = 3,
  };
  enum RelayChannel {
    RL_NONE = 0,
    REL_01 = 1,
    REL_02 = 2,
  };
  enum SensorChannel {
    SC_NONE = 0,
    CH_1 = 1, // OneWire Channel 1
    CH_2 = 2, // OneWire Channwl 2
    I2C = 3,  // I2C (BME280)
    GPIO = 4, // Digital Pin (Relay)
  };

  SensorId sensId = INVALID_SENS_ID; // 8 bytes integer.
  float meanValue = NAN;             // Mean value of values array.
  uint32_t lastUpdateTick = 0;       // in seconds.
  float minVal = 0.0f;               // switch relay on, if actValue below this.
  float maxVal = 0.0f;        // switch relay off, if actValue above this.
  SensorType sensType = TEMP; // Which type is the sensor.
  SensorChannel sensChan =
      SC_NONE; // Where the sensor is connected to. Relevant to OW devices.
  RelayChannel relayNr = RL_NONE; // which channel to switch; 0 = no relay.
  float values[VALUES_BUFF_LEN] = {0.0f}; // last measurement i.
  uint8_t valueIndex = 0; // pos, where the next value is stored to.
  char shortname[Measurement::SHORTNAME_LEN] = {'-'}; // without \0.

  Measurement() { InitValueArray(); }
  virtual ~Measurement() {}

  void UpdateValue(float value) {
    values[valueIndex] = value;
    IncrementValueIdx();

    float sum = 0.0f;
    float count = 0.0f;
    for (uint_fast8_t i = 0; i < VALUES_BUFF_LEN; i++) {
      if (isnanf(values[i])) {
        break;
      }
      sum += values[i];
      count += 1.0f;
    }
    if (count > 0.0) {
      meanValue = sum / count;
    }
  }

  float GetMeanValue() { return meanValue; }

  bool IsInvalidSensId() { return sensId == INVALID_SENS_ID; }

  static bool IsInvalidSensId(SensorId id) { return id == INVALID_SENS_ID; }

  static SensorId CastArrayToSensId(uint8_t *address) {
    SensorId *temp = (SensorId *)address;
    return *temp;
  }

  static bool CompareSensorId(SensorIdArray address_left,
                              SensorIdArray address_right) {
    for (uint_fast8_t i = 0; i < SENS_ID_LEN; i++) {
      if (address_left[i] != address_right[i]) {
        return false;
      }
    }
    return true;
  }

  std::string GetShortname() { return std::string(shortname, SHORTNAME_LEN); }

  static std::string DumpSensId(SensorId sensId) {
    SensorIdArray *sensIdArray = (SensorIdArray *)&sensId;
    return DumpSensIdArray(*sensIdArray);
  }
  static std::string DumpSensIdArray(SensorIdArray sensIdArray) {
    char buffer[25];
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", sensIdArray[0],
            sensIdArray[1], sensIdArray[2], sensIdArray[3], sensIdArray[4],
            sensIdArray[5], sensIdArray[6], sensIdArray[7]);
    return std::string(buffer);
  }

  void Dump() {
    Serial.print("sensId = ");
    Serial.print(sensId);
    Serial.print(" (");
    uint8_t *sensIdArray = (uint8_t *)&sensId;
    for (uint_fast8_t i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(sensIdArray[i], HEX);
    }
    Serial.println(")");

    Serial.print("meanValue = ");
    Serial.println(meanValue, 2);

    Serial.print("lastUpdateTick = ");
    Serial.println(lastUpdateTick);

    Serial.print("minVal = ");
    Serial.println(minVal, 2);

    Serial.print("maxVal = ");
    Serial.println(maxVal, 2);

    Serial.print("sensType = ");
    switch (sensType) {
    case SensorType::TEMP:
      Serial.println("TEMP");
      break;
    case SensorType::HUMIDITY:
      Serial.println("HUMID");
      break;
    case SensorType::PRESS:
      Serial.println("PRESS");
      break;
    case SensorType::RELAY:
      Serial.println("RELAY");
      break;
    }

    Serial.print("sensChan = ");
    switch (sensType) {
    case SensorChannel::CH_1:
      Serial.println("CH_1");
      break;
    case SensorChannel::CH_2:
      Serial.println("CH_2");
      break;
    case SensorChannel::GPIO:
      Serial.println("GPIO");
      break;
    case SensorChannel::I2C:
      Serial.println("I2C");
      break;
    case SensorChannel::SC_NONE:
      Serial.println("NONE");
      break;
    }

    Serial.print("relayNr = ");
    Serial.println(relayNr);

    Serial.print("values = ");
    for (uint_fast8_t i = 0; i < VALUES_BUFF_LEN; i++) {
      Serial.println(values[i], 2);
      Serial.print(" ");
    }
    Serial.println();

    Serial.print("valueIndex = ");
    Serial.println(valueIndex);

    Serial.print("shortname = ");
    Serial.println(GetShortname().c_str());
  }

private:
  void IncrementValueIdx() {
    valueIndex++;
    if (valueIndex >= VALUES_BUFF_LEN) {
      valueIndex = 0;
    }
  }

  void InitValueArray() {
    for (uint_fast8_t i = 0; i < VALUES_BUFF_LEN; i++) {
      values[i] = NAN;
    }
  }
};

// used to update measurement in the array
typedef struct {
  Measurement::SensorId sensorId;
  float value;
  uint32_t lastUpdateTick;
} MeasurementType;

typedef struct {
  Measurement::SensorId sensorId;
  float minVal;
  float maxVal;
  Measurement::SensorType sensType;
  Measurement::RelayChannel relayNr; // 0 = no relay
  char shortname[Measurement::SHORTNAME_LEN];
} SensorConfigType;

} // namespace msmnt
#endif /* APPLICATION_SENSORS_MEASUREMENTS_H_ */