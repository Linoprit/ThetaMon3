
#ifndef SENSORS_MEASURE_TASK_H_
#define SENSORS_MEASURE_TASK_H_

#include "Measurement.h"
#include "Sensors/MeasurementPivot.h"

namespace msmnt {

class MeasurementTask {

public:
  MeasurementTask(){};
  ~MeasurementTask(){};

  void init(void);
  static MeasurementTask &instance(void);
  void initHardware(void);

  void cycle(void);

	msmnt::MeasurementPivot* GetMeasurementPivot() { return &_measurementPivot; }

  // 	uint8_t getFoundDS1820(void)
  // 	{
  // 		return _foundDS1820Count;
  // 	}
  

private:
  // the central data-structure
  msmnt::MeasurementPivot _measurementPivot;

  // 	uint8_t _foundDS1820Count;
};

} // namespace msmnt

#endif // SENSORS_MEASURE_TASK_H_