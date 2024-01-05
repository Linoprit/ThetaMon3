#include "TasksCommon.h"
#include "Config.h"
#include "Sensors/Measurement.h"
#include "Sensors/MeasurementTask.h"

void startUpdateTask(void *unused_arg)
{
  msmnt::MeasurementTask::instance().init();
  msmnt::MeasurementTask::instance().initHardware();

  while (true)
  {
    msmnt::MeasurementTask::instance().cycle();
    delay(MEASURETASK_CYCLE);
  }
}