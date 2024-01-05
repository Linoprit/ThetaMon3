#include "Config.h"
#include "Sensors/MeasurementTask.h"
#include "TasksCommon.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#include <new>


namespace msmnt {
  
OneWire oneWireCh1(ONE_WIRE_CH1_PIN);
DallasTemperature sensorsCh1(&oneWireCh1);


void MeasurementTask::init(void) { new (&instance()) MeasurementTask(); }

void MeasurementTask::initHardware(void) {

  uint8_t address[8];
  sensorsCh1.begin();
  oneWireCh1.reset_search();
  while (oneWireCh1.search(address)) {
    Serial.print("ROM =");
    for (uint_fast8_t i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(address[i], HEX);
    }
    if (OneWire::crc8(address, 7) != address[7]) {
      Serial.println("CRC is not valid!");
    }

    sensorsCh1.setResolution(12);
  }
}

MeasurementTask &MeasurementTask::instance(void) {
  static MeasurementTask measureTask;
  return measureTask;
}

void MeasurementTask::cycle(void) {

  while (1) {
    Serial.print("Requesting temperatures...");
    sensorsCh1.requestTemperatures();
    Serial.println("DONE");
    delay(1000);

    // for (int i = 0; i < oneWireCount; i++) {
      
    //   float temperature = sensorsCh1.getTempC(const uint8_t *);


    //   //float temperature = sensor[i].getTempCByIndex(0);
    //   Serial.print("Temperature for the sensor ");
    //   Serial.print(i);
    //   Serial.print(" is ");
    //   Serial.println(temperature);
    // }
    // Serial.println();
    // delay(1000);
  }

  // MeasurementType actMeasurement;

  // // Todo
  // // copy all queue-contents into _measurementArray
  // if (xSemaphoreTake(measurementArraySmphr, (TickType_t)100) == pdTRUE) {

  //   while (xQueueReceive(measurementQueue, &(actMeasurement), (TickType_t)10)
  //   ==
  //          pdPASS) {
  //     // update measurement in array1
  //     // if a msmt is not in array... what to do?
  //   }

  //   /* ... */
  //   xSemaphoreGive(measurementArraySmphr);
  // }
}
} // namespace msmnt
