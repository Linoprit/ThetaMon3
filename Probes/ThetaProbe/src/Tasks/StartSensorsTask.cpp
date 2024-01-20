#include "TasksCommon.h"
#include "Config.h"
#include "Sensors.h"
//#include "Sensors/MeasurementTask.h"

void startSensorsTask(void *unused_arg)
{
  msmnt::Sensors::instance().init();
  msmnt::Sensors::instance().initHardware();

  while (true)
  {
    msmnt::Sensors::instance().cycle();
    // delay(MEASURETASK_CYCLE); // TODO enable, if deployed
    delay(2000);
  }
}