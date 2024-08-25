#include <AppTypes.h>
#include <Arduino.h>
#include <Config.h>
#include <DigitalIo/GpioInOut.h>
#include <unity.h>

using namespace msmnt;

bool updateMsmntConf(msmnt::MeasurementPivot *msmntPvt, uint64_t sensorId,
                     float minVal, float maxVal, uint8_t sensorType,
                     uint8_t relayNr, std::string shortname) {
  SensorConfigType sensConf;

  sensConf.sensorId = sensorId;
  sensConf.minVal = minVal;
  sensConf.maxVal = maxVal;
  sensConf.sensType = static_cast<Measurement::SensorType>(sensorType);
  sensConf.relayNr = static_cast<Measurement::RelayChannel>(relayNr);
  msmnt::Measurement::CopyToShortname(sensConf.shortname, shortname.c_str());

  return msmntPvt->UpdateConfig(sensConf);
}

void fillMsmnt(msmnt::MeasurementPivot *msmntPvt) {
  msmntPvt->StoreSensId(131072u, Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 131072, -12.5, -12.7, 0, 0,
                  std::string("WrkstTmp"));

  msmntPvt->StoreSensId(5333131681338097448u, Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 5333131681338097448u, -5.0, -3.0, 0, 1,
                  std::string("INNEN   "));

  msmntPvt->StoreSensId(16789727009993785128u,
                        Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 16789727009993785128u, -5.0, -3.0, 0, 0,
                  std::string("Test 003"));

  msmntPvt->StoreSensId(9872197974920462120u, Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 9872197974920462120u, -5.0, -3.0, 0, 1,
                  std::string("Test 004"));

  msmntPvt->StoreSensId(7638694032459628328u, Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 7638694032459628328u, -5.0, -3.0, 0, 1,
                  std::string("Test 005"));

  msmntPvt->StoreSensId(5332846427711201320u, Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 5332846427711201320u, -5.0, -3.0, 0, 1,
                  std::string("Test 010"));

  msmntPvt->StoreSensId(9728353066963701288u, Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 9728353066963701288u, -5.0, -3.0, 0, 0,
                  std::string("LGR_U2  "));

  msmntPvt->StoreSensId(15060624921359795240u,
                        Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 15060624921359795240u, -5.0, -3.0, 0, 0,
                  std::string("LGRASN_O"));

  msmntPvt->StoreSensId(2018190504903736616u, Measurement::SensorChannel::CH_1);
  updateMsmntConf(msmntPvt, 2018190504903736616u, -5.0, -3.0, 0, 1,
                  std::string("LGRASN_U"));

  // ---------------------------------------

  msmntPvt->StoreSensId(288379986015149092u, Measurement::SensorChannel::CH_2);
  updateMsmntConf(msmntPvt, 288379986015149092u, -12.5, -12.7, 0, 0,
                  std::string("ESP1Tmp "));

  msmntPvt->StoreSensId(11169503848206398760u,
                        Measurement::SensorChannel::CH_2);
  updateMsmntConf(msmntPvt, 11169503848206398760u, 3.0, 5.0, 0, 2,
                  std::string("Test 011"));

  msmntPvt->StoreSensId(17798800300682535208u,
                        Measurement::SensorChannel::CH_2);
  updateMsmntConf(msmntPvt, 17798800300682535208u, 3.0, 5.0, 0, 2,
                  std::string("INNEN_U2"));

  msmntPvt->StoreSensId(4179915027513875496u, Measurement::SensorChannel::CH_2);
  updateMsmntConf(msmntPvt, 4179915027513875496u, 3.0, 5.0, 0, 2,
                  std::string("INNEN_O2"));

  msmntPvt->StoreSensId(14484146576875228968u,
                        Measurement::SensorChannel::CH_2);
  updateMsmntConf(msmntPvt, 14484146576875228968u, 3.0, 5.0, 0, 2,
                  std::string("LGR_O2  "));

  msmntPvt->StoreSensId(17078224360303526696u,
                        Measurement::SensorChannel::CH_2);
  updateMsmntConf(msmntPvt, 17078224360303526696u, 3.0, 5.0, 0, 2,
                  std::string("WST_O2  "));

  msmntPvt->StoreSensId(72627769288089640u, Measurement::SensorChannel::CH_2);
  updateMsmntConf(msmntPvt, 72627769288089640u, 3.0, 5.0, 0, 0,
                  std::string("WST_U2  "));
}

void updateValuesCh1(msmnt::MeasurementPivot *msmntPvt, float theta)
{
  for (uint_fast8_t i = 0; i < Measurement::VALUES_BUFF_LEN; i++) {
    msmntPvt->UpdateValue(131072u, theta);
    msmntPvt->UpdateValue(5333131681338097448u, theta);
    msmntPvt->UpdateValue(16789727009993785128u, theta);
    msmntPvt->UpdateValue(9872197974920462120u, theta);
    msmntPvt->UpdateValue(7638694032459628328u, theta);
    msmntPvt->UpdateValue(5332846427711201320u, theta);
    msmntPvt->UpdateValue(9728353066963701288u, theta);
    msmntPvt->UpdateValue(15060624921359795240u, theta);
    msmntPvt->UpdateValue(2018190504903736616u, theta);
  }
}

void updateValuesCh2(msmnt::MeasurementPivot *msmntPvt, float theta)
{
  for (uint_fast8_t i = 0; i < Measurement::VALUES_BUFF_LEN; i++) {
    msmntPvt->UpdateValue(288379986015149092u, theta);
    msmntPvt->UpdateValue(11169503848206398760u, theta);
    msmntPvt->UpdateValue(17798800300682535208u, theta);
    msmntPvt->UpdateValue(4179915027513875496u, theta);
    msmntPvt->UpdateValue(14484146576875228968u, theta);
    msmntPvt->UpdateValue(17078224360303526696u, theta);
    msmntPvt->UpdateValue(72627769288089640u, theta);
  }
}

void updateValue(msmnt::MeasurementPivot *msmntPvt, uint64_t sensId,
                 float theta) {
  for (uint_fast8_t i = 0; i < Measurement::VALUES_BUFF_LEN; i++) {
    msmntPvt->UpdateValue(sensId, theta);
  }
}

bool testCycle(msmnt::MeasurementPivot *msmntPvt, int levelCh1, int levelCh2) {
  gpio::GpioInOut::instance().calcRelayStates(msmntPvt);

  Serial.printf("RelNr 1: expect %i / actual %i\n", levelCh1,
                digitalRead(RELAY_CH1_PIN));
  Serial.printf("RelNr 2: expect %i / actual %i\n", levelCh2,
                digitalRead(RELAY_CH2_PIN));

  if ((digitalRead(RELAY_CH1_PIN) == levelCh1) &&
      (digitalRead(RELAY_CH2_PIN) == levelCh2)) {
    return true;
  }
  return false;
}

void CalcRelayStates_UT() {
  msmnt::MeasurementPivot msmntPvt;
  fillMsmnt(&msmntPvt);

  gpio::GpioInOut gpioInOut = gpio::GpioInOut::instance();
  gpioInOut.init();
  gpioInOut.initHardware();

  updateValuesCh1(&msmntPvt, 0.0);
  updateValuesCh1(&msmntPvt, 4.0);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, HIGH, HIGH));
  TEST_ASSERT_FALSE(
    gpio::GpioInOut::instance().getRelayState(msmnt::Measurement::REL_01));
  TEST_ASSERT_FALSE(
      gpio::GpioInOut::instance().getRelayState(msmnt::Measurement::REL_02));
  TEST_ASSERT_FALSE(
    gpio::GpioInOut::instance().calcState(msmnt::Measurement::REL_01, &msmntPvt));


  updateValue(&msmntPvt, 72627769288089640u, 2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, HIGH, HIGH));

  updateValue(&msmntPvt, 5333131681338097448u, -5.1);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, HIGH));

  updateValue(&msmntPvt, 17798800300682535208u, 2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));

  updateValuesCh1(&msmntPvt, -5.1);
  updateValuesCh2(&msmntPvt, 2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));

  updateValue(&msmntPvt, 5332846427711201320u, -2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));
  updateValue(&msmntPvt, 14484146576875228968u, 5.1);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));

  updateValue(&msmntPvt, 9872197974920462120u, -2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));
  updateValue(&msmntPvt, 17798800300682535208u, 5.1);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));

  updateValue(&msmntPvt, 7638694032459628328u, -2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));
  updateValue(&msmntPvt, 4179915027513875496u, 5.1);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, LOW, LOW));

  updateValuesCh1(&msmntPvt, -2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, HIGH, LOW));
  updateValuesCh2(&msmntPvt, 5.1);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, HIGH, HIGH));

  updateValue(&msmntPvt, 15060624921359795240u, -5.1);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, HIGH, HIGH));

  updateValue(&msmntPvt, 16789727009993785128u, 2.9);
  TEST_ASSERT_TRUE(testCycle(&msmntPvt, HIGH, HIGH));
}
