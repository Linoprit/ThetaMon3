#include "Config.h"
#include "TasksCommon.h"
#include <Arduino.h>
#include <Wifi/MqLog.h>
//#include <CommandLine/ComLineConfig.h>
//#include <CommandLine/CommandLine.h>
#include <DigitalIo/GpioInOut.h>
#include <FileSystem/LittleFsHelpers.h>
#include <OsHelpers.h>
#include <Sensors/Measurement.h>
#include <Sensors/MeasurementPivot.h>
#include <Wifi/MqttHelper.h>

// Common tasks and queues definition
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t comLineTaskHandle = NULL;

void setup() {
  Serial.begin(115200);
  Serial.println();

  xTaskCreate(startComLineTask, "ComLineTask", 3024, NULL, 1,
              &comLineTaskHandle);
  delay(500);

  gpio::GpioInOut::instance().init();
  gpio::GpioInOut::instance().initHardware();

  nvm::LittleFsHelpers::instance().init();
  nvm::LittleFsHelpers::instance().initHardware();

  // Init sensor task. The sensor init also inits Wifi and Mqtt
  // Because we push commands to the interpreter-queue, this must happen
  // in sequence.
  xTaskCreate(startSensorsTask, "SENSOR_TASK", 3024, NULL, 1,
              &sensorTaskHandle);
}

void loop() {
  static uint_fast8_t aliveLedCycleCount = 0;

  MqLogCycle();

  aliveLedCycleCount++;
  if (aliveLedCycleCount > 100) {
    aliveLedCycleCount = 0;
    gpio::GpioInOut::instance().tglLedAlive();
  }

  delay(10);
}

/*
mosquitto_sub -h localhost -t Lager/sens
mosquitto_sub -h localhost -t Lager/log
mosquitto_pub -t 'Lager/cmd' -m 'printMeasures'



*/