
#ifndef SENSORS_MEASURE_TASK_H_
#define SENSORS_MEASURE_TASK_H_

#include "Measurement.h"


namespace msmnt
{

	class UpdateTask
	{

	public:
		UpdateTask(){};
		~UpdateTask(){};

		void init(void);
		static UpdateTask& instance(void);
		void initHardware(void);
		bool isInitDone(void)
		{
			return _initIsDone;
		}
		void cycle(void);

		uint8_t getFoundDS1820(void)
		{
			return _foundDS1820Count;
		}
		msmnt::MeasurementArray *getMeasurements(void)
		{
			return &_measurementArray;
		}		
		
	private:
		bool _initIsDone;
		uint8_t _foundDS1820Count;
		msmnt::MeasurementArray _measurementArray;

	};

}

#endif // SENSORS_MEASURE_TASK_H_