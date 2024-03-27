
#ifndef INSTANCES_CONFIG_H_
#define INSTANCES_CONFIG_H_

#include <stdint.h>

// ******* One Wire *******
constexpr bool OW_LINE_INVERTED = true;
// #define OW_LINE_INVERTED true

// constexpr uint8_t ONE_WIRE_CH1_PIN = 32;
// constexpr uint8_t ONE_WIRE_CH2_PIN = 33;
constexpr uint8_t ONE_WIRE_CH1_PIN_IN = 25;
constexpr uint8_t ONE_WIRE_CH1_PIN_OUT = 26;
constexpr uint8_t ONE_WIRE_CH2_PIN_IN = 32;
constexpr uint8_t ONE_WIRE_CH2_PIN_OUT = 33;

// ******* Digital IO *******
constexpr uint8_t LED_ALIVE_PIN = 4;
constexpr uint8_t LED_CONNECTED_PIN = 2;
constexpr uint8_t RELAY_CH1_PIN = 27;
constexpr uint8_t RELAY_CH2_PIN = 12;

// ******* DS18B20 device *******
// Conversion precision in bit
constexpr uint8_t DS18B20_PRECISION = 12;
// constexpr uint8_t DS18B20_PRECISION = 9;

constexpr uint8_t DS18B20_SEARCH_RETRIES = 6;
constexpr uint8_t DS18B20_MAX_DEVICES = 4; // in one channel

// [ms] doAllMeasure(): how long to wait until all devices finished conversion
// If this time is passed, sensor-value is set to NAN
constexpr uint32_t DS18B20_CONVERSION_TIMEOUT_MS = 5000;

// time in [ms], the measureTask delays
constexpr uint32_t MEASURETASK_CYCLE = 60000;

// how many sensors can be present on a device (probe). Should be the same in
// the whole network. (DS1820 Sensors * Channels + BME280 Temp/Humi/Press +
// RelayStates)
constexpr uint8_t MAX_SENSORS = (DS18B20_MAX_DEVICES * 2 + 3 + 1);

// Measurement timeout. If a sensor fails to be read for this time (in seconds),
// it is marked as invalid.
constexpr uint32_t MEASUREMENT_TIMEOUT_SEC = 300;

// ******* BME280 *******
// constexpr uint8_t BME280_I2CADDR = 0xEC // 0x76<<1	SDO -> GND
constexpr uint8_t BME280_I2CADDR = 0x77; // 0x77<<1	SDO -> VCC
// constexpr uint8_t BME280_I2C_CHANNEL = 1; // not used, I2C_1 is default
// I2C_1, SDA_PIN = 21, SCL_PIN = 22

// float getSealevelForAltitude()
//    const {return (float) _bme280_data.pressure / 100.0 / pow(1.0 - (_altitude
//    / 44330.0), 5.255);}

// ******* Filenames *******
constexpr char idTableFile[] = "/ID_Table_U64.txt";

// ******* MQTT *******
constexpr char mqttConfFile[] = "/MqttConfig.txt";
constexpr char MQTT_SENS[] = "/sens";
constexpr char MQTT_LOG[] = "/log";
constexpr char MQTT_CMD[] = "/cmd";
// Length of memory, allocatet for the mqtt-paths. Must hold i.e.
// 'werkstatt/sens'. If the spotname is too long, the path will be truncated, so
// mqtt will not work.
constexpr uint8_t MQTT_PATHLEN = 20;
// constexpr uint8_t WIFI_SSIDLEN = 20;

#endif /* INSTANCES_CONFIG_H_ */