// #include "Config.h"
// #include "TasksCommon.h"
// #include <Arduino.h>
// #include <DigitalIo/GpioInOut.h>
// #include <FileSystem/LittleFsHelpers.h>
// #include <OsHelpers.h>
// #include <Sensors/Measurement.h>
// #include <Sensors/MeasurementPivot.h>
// #include <Wifi/MqLog.h>
// #include <Wifi/MqttHelper.h>

// // Common tasks and queues definition
// TaskHandle_t sensorTaskHandle = NULL;
// TaskHandle_t comLineTaskHandle = NULL;

// void setup() {
//   Serial.begin(115200);
//   Serial.println();

//   xTaskCreate(startComLineTask, "ComLineTask", 3024, NULL, 1,
//               &comLineTaskHandle);
//   delay(500);

//   gpio::GpioInOut::instance().init();
//   gpio::GpioInOut::instance().initHardware();

//   nvm::LittleFsHelpers::instance().init();
//   nvm::LittleFsHelpers::instance().initHardware();

//   // Init sensor task. The sensor init also inits Wifi and Mqtt
//   // Because we push commands to the interpreter-queue, this must happen
//   // in sequence.
//   xTaskCreate(startSensorsTask, "SENSOR_TASK", 3024, NULL, 1,
//               &sensorTaskHandle);

// // Todo remove
//   pinMode(18, OUTPUT);
// }

// void loop() {
//   static uint_fast8_t aliveLedCycleCount = 0;

//   MqLogCycle();

//   aliveLedCycleCount++;
//   if (aliveLedCycleCount > 100) {
//     aliveLedCycleCount = 0;
//     gpio::GpioInOut::instance().tglLedAlive();
//   }

//   delay(10);
// }

////// BEGIN Code for testing
#include "DallasTemperature.h"
#include <Config.h>
#include <OneWire.h>
#include <OneWire_direct_gpio.h>
#include <Sensors/Measurement.h>

using namespace msmnt;
using namespace std;

OneWire oneWireCh1;
OneWire oneWireCh2;
DallasTemperature* _sensorsCh;

void setup() {
  Serial.begin(115200);
  oneWireCh1.begin(ONE_WIRE_CH1_PIN_OUT, ONE_WIRE_CH1_PIN_IN);
  oneWireCh2.begin(ONE_WIRE_CH2_PIN_OUT, ONE_WIRE_CH2_PIN_IN);
  _sensorsCh = new DallasTemperature(&oneWireCh1);
  _sensorsCh->setResolution(12);
  //pinMode(18, OUTPUT);
}

void loop() {

  // directWriteLow(ONE_WIRE_CH1_PIN_OUT);
  // delayMicroseconds(65);
  // directWriteHigh(ONE_WIRE_CH1_PIN_OUT);

  // directWriteLow(ONE_WIRE_CH2_PIN_OUT);
  // delayMicroseconds(65);
  // directWriteHigh(ONE_WIRE_CH2_PIN_OUT);

  // bool pPulse = oneWireCh1.reset();
  // if (pPulse) {
  //   Serial.printf("Got pPulse.\n");
  // } else {
  //   Serial.printf("No pPulse.\n");
  // }

  // DeviceAddress address;
  // oneWireCh1.reset_search();

  // while (oneWireCh1.search(address)) {
  //   std::string id = msmnt::Measurement::DumpSensIdArray(address);
  //   Serial.printf("found: %s\n", id.c_str());

  //   if (OneWire::crc8(address, 7) != address[7]) {
  //     MqLog("CRC is not valid!\n");
  //     continue;
  //   }
  // }


  uint8_t address[8];
  uint8_t senorCount = 0;
  _sensorsCh->begin();
  oneWireCh1.reset_search();

  // MqLog("Searching oneWire on channel %s\n",
  //       Measurement::DumpSensChannel(_channel).c_str());

  while (oneWireCh1.search(address)) {
    Serial.printf("ROM = %s\n", Measurement::DumpSensIdArray(address).c_str());

    if (OneWire::crc8(address, 7) != address[7]) {
      Serial.printf("CRC is not valid!\n");
      continue;
    }
    //_measurementPivot->StoreSensId(address, _channel);
    senorCount++;
  }
  Serial.printf("Found sensors: %i\n", senorCount);

  delay(500);
}

////// END Code for testing

/*
mosquitto_sub -h localhost -t Lager/sens
mosquitto_sub -h localhost -t Lager/log
mosquitto_pub -t 'Lager/cmd' -m 'printMeasures'
*/