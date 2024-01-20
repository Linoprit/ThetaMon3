
#ifndef INSTANCES_CONFIG_H_
#define INSTANCES_CONFIG_H_

#include <stdint.h>

// ******* One Wire *******
constexpr uint8_t ONE_WIRE_CH1_PIN = 32;
constexpr uint8_t ONE_WIRE_CH2_PIN = 33;

// ******* Digital IO *******
constexpr uint8_t LED_ALIVE_PIN = 4;
constexpr uint8_t LED_CONNECTED_PIN = 2;
constexpr uint8_t RELAY_CH1_PIN = 27;
constexpr uint8_t RELAY_CH2_PIN = 12;

// ******* DS18B20 device *******
// Conversion precision in bit
constexpr uint8_t DS18B20_PRECISION = 12;

constexpr uint8_t DS18B20_SEARCH_RETRIES = 6;
constexpr uint8_t DS18B20_MAX_DEVICES = 4; // in one channel

// [ms] doAllMeasure(): how long to wait until all devices finished conversion
// If this time is passed, sensor-value is set to NAN
constexpr uint32_t DS18B20_CONVERSION_TIMEOUT_MS = 5000;

// time in [ms], the measureTask delays
constexpr uint32_t MEASURETASK_CYCLE = 5000;

// how many sensors can be present on a device. Should be the same in the whole
// network. (DS1820 Sensors * Channels + BME280 Temp/Humi/Press + RelayStates)
constexpr uint8_t MAX_SENSORS = (DS18B20_MAX_DEVICES * 2 + 3 + 1);

// ******* BME280 *******
// constexpr uint8_t BME280_I2CADDR = 0xEC		// 0x76<<1	SDO ->
// GND
constexpr uint8_t BME280_I2CADDR = 0xEE; // 0x77<<1	SDO -> VCC
constexpr uint8_t BME280_SDA_PIN = 21;
constexpr uint8_t BME280_SCL_PIN = 22;
// float getSealevelForAltitude()
//    const {return (float) _bme280_data.pressure / 100.0 / pow(1.0 - (_altitude
//    / 44330.0), 5.255);}

#endif /* INSTANCES_CONFIG_H_ */