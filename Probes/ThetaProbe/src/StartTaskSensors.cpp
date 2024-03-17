#include "TasksCommon.h"
#include "Config.h"
#include <Sensors/Sensors.h>

void startSensorsTask(void *unused_arg)
{
  msmnt::Sensors::instance().init();
  msmnt::Sensors::instance().initHardware();

  while (true)
  {
    msmnt::Sensors::instance().cycle();
    delay(MEASURETASK_CYCLE);
  }
}