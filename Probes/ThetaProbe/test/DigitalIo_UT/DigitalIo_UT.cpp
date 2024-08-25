#include <AppTypes.h>
#include <Arduino.h>
#include <Config.h>
#include <DigitalIo/GpioInOut.h>
#include <unity.h>

void RelayTestTimerOn() {
  gpio::GpioInOut::instance().tstRelay(1, _on_);
  delay(500);
  gpio::GpioInOut::instance().clrRelayCh1();
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH1_PIN));

  gpio::GpioInOut::instance().tstRelay(2, _on_);
  delay(500);
  gpio::GpioInOut::instance().clrRelayCh2();
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH1_PIN));
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH2_PIN));

  gpio::GpioInOut::instance().tstRelay(2, _off_);
  delay(500);
  gpio::GpioInOut::instance().clrRelayCh1();
  gpio::GpioInOut::instance().setRelayCh2();
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH1_PIN));
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH2_PIN));

  gpio::GpioInOut::instance().tstRelay(1, _off_);
  delay(500);
  gpio::GpioInOut::instance().setRelayCh1();
  gpio::GpioInOut::instance().setRelayCh2();
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH1_PIN));
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH2_PIN));

  delay(3600);
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH1_PIN));
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH2_PIN));
  gpio::GpioInOut::instance().clrFlagTestTimer();
}

void TestSimpleOnOff() {
  gpio::GpioInOut::instance().setLedAlive();
  TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_ALIVE_PIN));
  delay(500);
  gpio::GpioInOut::instance().clrLedAlive();
  TEST_ASSERT_EQUAL(LOW, digitalRead(LED_ALIVE_PIN));
  delay(500);

  gpio::GpioInOut::instance().setLedConnected();
  TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_CONNECTED_PIN));
  delay(500);
  gpio::GpioInOut::instance().clrLedConnected();
  TEST_ASSERT_EQUAL(LOW, digitalRead(LED_CONNECTED_PIN));
  delay(500);

  TEST_ASSERT_EQUAL(LOW, gpio::GpioInOut::instance().isTestTimerActive());
  gpio::GpioInOut::instance().setRelayCh1();
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH1_PIN));
  delay(500);
  gpio::GpioInOut::instance().clrRelayCh1();
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH1_PIN));
  delay(500);

  gpio::GpioInOut::instance().setRelayCh2();
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH2_PIN));
  delay(500);
  gpio::GpioInOut::instance().clrRelayCh2();
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH2_PIN));
  delay(500);
}

void DigitalIo_UT() {

  gpio::GpioInOut::instance().init();
  gpio::GpioInOut::instance().initHardware();

  TEST_ASSERT_EQUAL(4, LED_ALIVE_PIN);
  TEST_ASSERT_EQUAL(2, LED_CONNECTED_PIN);
  TEST_ASSERT_EQUAL(27, RELAY_CH1_PIN);
  TEST_ASSERT_EQUAL(12, RELAY_CH2_PIN);

  RelayTestTimerOn();
  TestSimpleOnOff();
}
