// // Animates white pixels to simulate flying through a star field

// #include <Arduino.h>
// #include "TasksCommon.h"

// volatile QueueHandle_t ledQueue = NULL;
// TaskHandle_t gpioTaskHandle = NULL;
// TaskHandle_t picoTaskHandle = NULL;
// TaskHandle_t sensorTaskHandle = NULL;
// TaskHandle_t displayTaskHandle = NULL;

// void setup()
// {
//   esp_task_wdt_init(30, false);

//   setupDisplay();

//   xTaskCreate(led_task_pico, "PICO_LED_TASK", 1024, NULL, 1,
//   &picoTaskHandle); xTaskCreate(led_task_gpio, "GPIO_LED_TASK", 1024, NULL,
//   1, &gpioTaskHandle); xTaskCreate(ReadSensors, "READ_SENSORS_TASK", 1024,
//   NULL, 1, &sensorTaskHandle); xTaskCreate(DisplayTask, "DisplayTask", 1024,
//   NULL, 1, &displayTaskHandle);

//   ledQueue = xQueueCreate(4, sizeof(uint8_t));
// }

// int value = 0;
// void loop()
// {
//   vTaskDelay(500);
// }
/*
  Rui Santos
  Complete project details at
  https://RandomNerdTutorials.com/esp32-mqtt-publish-ds18b20-temperature-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>

#include "Config.h"
#include "TasksCommon.h"
#include "Wifi/MqLog.h"
#include <CommandLine/ComLineConfig.h>
#include <CommandLine/CommandLine.h>
#include <FileSystem/LittleFsHelpers.h>
#include <OsHelpers.h>
#include <Sensors/Measurement.h>
#include <Sensors/MeasurementPivot.h>
#include <Wifi/MqttHelper.h>

// ToDo
// invalid measurements, timeout + out of range
// Relays. Check relays. Relay as sensor
// Channels. Configure channel x off/on
// messages to mqtt
// mqtt to command

// Common tasks and queues definition
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t printTaskHandle = NULL;
SemaphoreHandle_t mqBuffSemHandle = NULL;

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Todo move to gpio_init
  pinMode(LED_ALIVE_PIN, OUTPUT);
  pinMode(LED_CONNECTED_PIN, OUTPUT);
  pinMode(RELAY_CH1_PIN, OUTPUT);
  pinMode(RELAY_CH2_PIN, OUTPUT);

  cLine::CommandLine::instance().init();
  delay(500);
  cLine::CommandLine::instance().splash();

  nvm::LittleFsHelpers::instance().init();
  nvm::LittleFsHelpers::instance().initHardware();

  // Init sensor task. The sensor init also inits Wifi and Mqtt
  // Because we push commands to the interpreter-queue, this must happen
  // in sequence.
  xTaskCreate(startSensorsTask, "SENSOR_TASK", 3024, NULL, 1,
              &sensorTaskHandle);

  // used in MqLog
  mqBuffSemHandle = xSemaphoreCreateBinary();
  xSemaphoreGive(mqBuffSemHandle);

  // measurementArraySmphr = xSemaphoreCreateCounting(5, 0);

  // print measurementPivot
  // xTaskCreate(startPrintTask, "PRINT_TASK", 2048, NULL, 1, &printTaskHandle);

  // Read file line by line
  // littleFsTest.init();
  // littleFsTest.listDir("/", 3);
  // File file = LittleFS.open("/ID_Table_U64.txt");
  // if (!file || file.isDirectory()) {
  //   Serial.println("- failed to open file for reading");
  //   return;
  // }
  // while (file.available()) {
  //   String MySecret = file.readStringUntil('\n');
  //   Serial.print(MySecret.c_str());
  // }
  // file.close();
  //
}

void loop() {
  static uint_fast8_t mqLogCycleCount = 0;

  // CommandLine loop
  int incomingByte = Serial.read();
  while (incomingByte != -1) {
    cLine::CommandLine::instance().putChar(incomingByte);
    incomingByte = Serial.read();
  }
  cLine::CommandLine::instance().cycle();

  mqLogCycleCount++;
  if (mqLogCycleCount > 3) {
    MqLogCycle();
    mqLogCycleCount = 0;
  }

  //
  //
  // digitalWrite(LED_ALIVE_PIN, 1);
  // digitalWrite(LED_CONNECTED_PIN, 1);
  // digitalWrite(RELAY_CH1_PIN, 1);
  // digitalWrite(RELAY_CH2_PIN, 1);
  // delay(1000);

  // digitalWrite(LED_ALIVE_PIN, 0);
  // digitalWrite(LED_CONNECTED_PIN, 0);
  // digitalWrite(RELAY_CH1_PIN, 0);
  // digitalWrite(RELAY_CH2_PIN, 0);

  delay(10);
}
