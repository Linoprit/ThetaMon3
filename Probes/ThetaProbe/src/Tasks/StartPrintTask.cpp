#include "TasksCommon.h"
#include <Sensors.h>

void startPrintTask(void *unused_arg) {

  while (true) {
    msmnt::MeasurementPivot *measurementPivot =
        msmnt::Sensors::instance().getMeasurementPivot();

    if (measurementPivot == nullptr) {
      Serial.println("measurementPivot is null.");
    } else {
      measurementPivot->Dump();
    }
    Serial.println("---------------\n");

    delay(2000);
  }
}