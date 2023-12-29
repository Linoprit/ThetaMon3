#include "Sensors/MeasureTask.h"
#include <new>
#include "TasksCommon.h"

namespace msmnt
{
    void UpdateTask::init(void)
    {
        new (&instance()) UpdateTask();
    }

    void UpdateTask::initHardware(void)
    {
        // Todo
    }

    UpdateTask &UpdateTask::instance(void)
    {
        static UpdateTask measureTask;
        return measureTask;
    }

    void UpdateTask::cycle(void)
    {
        MeasurementType actMeasurement;

        // Todo
        // copy all queue-contents into _measurementArray
        if (xSemaphoreTake(measurementArraySmphr, (TickType_t)100) == pdTRUE)
        {

            while (xQueueReceive(measurementQueue,
                                 &(actMeasurement),
                                 (TickType_t)10) == pdPASS)
            {
                // update measurement in array1
                // if a msmt is not in array... what to do?

            }

            /* ... */
            xSemaphoreGive(measurementArraySmphr);
        }
    }
}
