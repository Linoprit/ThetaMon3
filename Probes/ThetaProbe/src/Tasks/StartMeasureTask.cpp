#include "TasksCommon.h"
#include "Config.h"
#include "Measurement.h"
#include "Sensors/MeasureTask.h"

void startUpdateTask(void *unused_arg)
{
  msmnt::UpdateTask::instance().init();
  msmnt::UpdateTask::instance().initHardware();

  while (true)
  {
    msmnt::UpdateTask::instance().cycle();
    delay(MEASURETASK_CYCLE);
  }
}