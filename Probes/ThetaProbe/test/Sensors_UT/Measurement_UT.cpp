#include "OsHelpers.h"
#include <Arduino.h>
#include <Config.h>
#include <Sensors/Measurement.h>
#include <Sensors/MeasurementPivot.h>
#include <unity.h>

using namespace msmnt;

void Measurement_UT() {
  msmnt::Measurement msmnt;

  TEST_ASSERT_TRUE(isnan(msmnt.GetMeanValue()));

  msmnt.UpdateValue(1.0f);
  TEST_ASSERT_EQUAL_FLOAT(1.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(2.0f);
  TEST_ASSERT_EQUAL_FLOAT(1.50f, msmnt.GetMeanValue());

  msmnt.UpdateValue(3.0f);
  TEST_ASSERT_EQUAL_FLOAT(2.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(4.0f);
  TEST_ASSERT_EQUAL_FLOAT(2.50f, msmnt.GetMeanValue());

  msmnt.UpdateValue(-56.0f);
  TEST_ASSERT_EQUAL_FLOAT(2.50f, msmnt.GetMeanValue());

  msmnt.UpdateValue(56.0f);
  TEST_ASSERT_EQUAL_FLOAT(2.50f, msmnt.GetMeanValue());

  msmnt.UpdateValue(5.0f);
  TEST_ASSERT_EQUAL_FLOAT(3.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(6.0f);
  TEST_ASSERT_EQUAL_FLOAT(4.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(7.0f);
  TEST_ASSERT_EQUAL_FLOAT(5.00f, msmnt.GetMeanValue());

  TEST_ASSERT_TRUE(msmnt.IsInvalidSensId());
  TEST_ASSERT_TRUE(msmnt.IsInvalidSensId(msmnt::Measurement::INVALID_SENS_ID));

  uint8_t sensIdArray[] = {0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90};
  msmnt::Measurement::SensorId sensId = 0x9078563412EFCDAB; // little endian
  TEST_ASSERT_EQUAL(sensId, msmnt::Measurement::CastArrayToSensId(sensIdArray));

  // 'pio test -v' to see the print-out;
  char buffer[50];
  sprintf(buffer, "array: %s / id: %s",
          msmnt::Measurement::DumpSensIdArray(sensIdArray).c_str(),
          msmnt::Measurement::DumpSensId(sensId).c_str());
  Serial.println(buffer);

  TEST_ASSERT_TRUE(
      msmnt::Measurement::CompareSensorId(sensIdArray, sensIdArray));
  uint8_t sensIdArrayOther[] = {0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x80};
  TEST_ASSERT_FALSE(
      msmnt::Measurement::CompareSensorId(sensIdArray, sensIdArrayOther));
}

void OutOfRanges_UT() {
  msmnt::Measurement msmnt;
  uint_fast8_t i = 0;

  msmnt.sensType = msmnt::Measurement::SensorType::PRESS;
  for (i = 0; i < msmnt::Measurement::VALUES_BUFF_LEN; i++) {
    msmnt.UpdateValue(600.0f);
  }
  TEST_ASSERT_EQUAL_FLOAT(600.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(400.0f);
  TEST_ASSERT_EQUAL_FLOAT(600.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(1200.0f);
  TEST_ASSERT_EQUAL_FLOAT(600.00f, msmnt.GetMeanValue());

  msmnt.sensType = msmnt::Measurement::SensorType::HUMIDITY;
  for (i = 0; i < msmnt::Measurement::VALUES_BUFF_LEN; i++) {
    msmnt.UpdateValue(60.0f);
  }
  TEST_ASSERT_EQUAL_FLOAT(60.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(4.0f);
  TEST_ASSERT_EQUAL_FLOAT(60.00f, msmnt.GetMeanValue());

  msmnt.UpdateValue(100.0f);
  TEST_ASSERT_EQUAL_FLOAT(60.00f, msmnt.GetMeanValue());
}

void MeasurementPivot_UT() {
  msmnt::MeasurementPivot msmntPvt;

  uint8_t sensIdArray[] = {0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x90};
  msmnt::Measurement::SensorId sensId = 0x9078563412EFCDAB; // little endian
  TEST_ASSERT_TRUE(
      msmntPvt.StoreSensId(sensIdArray, Measurement::SensorChannel::CH_1));
  TEST_ASSERT_FALSE(
      msmntPvt.StoreSensId(sensId, Measurement::SensorChannel::CH_1));
  for (uint_fast8_t i = 1; i <= 5; i++) {
    TEST_ASSERT_TRUE(
        msmntPvt.StoreSensId(sensId + i, Measurement::SensorChannel::CH_1));
  }

  msmntPvt.ResetIter();
  for (uint_fast8_t i = 1; i <= 5; i++) {
    TEST_ASSERT_TRUE(msmntPvt.GetNextMeasurement() != nullptr);
  }

  Measurement *freeSlot = msmntPvt.FindNextFreeSlot();
  TEST_ASSERT_TRUE(freeSlot->IsInvalidSensId());

  for (uint_fast8_t i = 0; i <= 5; i++) {
    TEST_ASSERT_TRUE(msmntPvt.FindSensIdOrEmpty(sensId + i) != nullptr);
  }
  TEST_ASSERT_FALSE(msmntPvt.FindSensIdOrEmpty(sensId + 6) != nullptr);
}

void MeasurementPivotUpdateValue_UT() {
  msmnt::MeasurementPivot msmntPvt;
  msmnt::Measurement::SensorId sensId = 0x9078563412EFCDAB; // little endian
  for (uint_fast8_t i = 0; i <= 5; i++) {
    TEST_ASSERT_TRUE(
        msmntPvt.StoreSensId(sensId + i, Measurement::SensorChannel::CH_1));
  }

  timeval timeVal;
  gettimeofday(&timeVal, NULL);

  TEST_ASSERT_TRUE(msmntPvt.UpdateValue(sensId, 1.0f));
  Measurement *msmnt = msmntPvt.FindSensIdOrEmpty(sensId);
  TEST_ASSERT_EQUAL_FLOAT(1.00f, msmnt->GetMeanValue());
  TEST_ASSERT_EQUAL(timeVal.tv_sec, msmnt->lastUpdateTick);
  TEST_ASSERT_TRUE(timeVal.tv_sec != 0);
}

void MeasurementPivotUpdateConfig_UT() {
  msmnt::MeasurementPivot msmntPvt;
  msmnt::Measurement::SensorId sensId = 0x9078563412EFCDAB; // little endian
  for (uint_fast8_t i = 0; i <= 5; i++) {
    TEST_ASSERT_TRUE(
        msmntPvt.StoreSensId(sensId + i, Measurement::SensorChannel::CH_1));
  }

  SensorConfigType config;
  config.sensorId = sensId + 2;
  config.minVal = 1.0f;
  config.maxVal = 3.0f;
  config.sensType = Measurement::SensorType::PRESS;
  config.relayNr = Measurement::RelayChannel::REL_02;
  memcpy(config.shortname, "blahblah", Measurement::SHORTNAME_LEN);

  TEST_ASSERT_TRUE(msmntPvt.UpdateConfig(config));

  Measurement *msmnt = msmntPvt.FindSensIdOrEmpty(sensId + 2);
  TEST_ASSERT_EQUAL_FLOAT(1.0f, msmnt->minVal);
  TEST_ASSERT_EQUAL_FLOAT(3.0f, msmnt->maxVal);
  TEST_ASSERT_EQUAL(Measurement::SensorType::PRESS, msmnt->sensType);
  TEST_ASSERT_EQUAL(Measurement::RelayChannel::REL_02, msmnt->relayNr);
  TEST_ASSERT_TRUE(std::string("blahblah", Measurement::SHORTNAME_LEN) ==
                   msmnt->GetShortname());
}

void MeasurementPivotCreateId_UT() {
  Measurement::SensorId sensId = MeasurementPivot::CreateId(
      MeasurementPivot::SensorKindType::BME_PRESS_76);

  uint8_t *maskedIdPtr = (uint8_t *)&sensId;
  // Serial.println(maskedIdPtr[7]);
  TEST_ASSERT_EQUAL(3, maskedIdPtr[7]);

  msmnt::Measurement::SensorId macMask = 0x00FFFFFFFFFFFFFF; // little endian
  Measurement::SensorId macMaskedId = sensId & macMask;
  uint64_t macSensId = 0;
  uint8_t *mac = (uint8_t *)&macSensId; // the last two bytes should stay empty
  OsHelpers::GetMacAddress(mac);
  // Serial.println(Measurement::DumpSensId(macSensId).c_str());
  TEST_ASSERT_EQUAL(macSensId, macMaskedId);
}