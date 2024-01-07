#include "Config.h"
#include "Sensors_UT/Measurement_UT.cpp"
#include "CommandLine_UT/CrcSocket_UT.cpp"
#include "Wifi/Credentials.h"
#include <Arduino.h>
#include <WiFi.h>
#include <unity.h>

//#define UNITY_FLOAT_PRECISION 3
//#include "../src/  MeasurementPivot.cpp"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_gpios(void) {
  TEST_ASSERT_EQUAL(4, LED_ALIVE_PIN);
  TEST_ASSERT_EQUAL(2, LED_CONNECTED_PIN);
  TEST_ASSERT_EQUAL(27, RELAY_CH1_PIN);
  TEST_ASSERT_EQUAL(12, RELAY_CH2_PIN);

  digitalWrite(LED_ALIVE_PIN, HIGH);
  TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_ALIVE_PIN));
  delay(500);
  digitalWrite(LED_ALIVE_PIN, LOW);
  TEST_ASSERT_EQUAL(LOW, digitalRead(LED_ALIVE_PIN));
  delay(500);

  digitalWrite(LED_CONNECTED_PIN, HIGH);
  TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_CONNECTED_PIN));
  delay(500);
  digitalWrite(LED_CONNECTED_PIN, LOW);
  TEST_ASSERT_EQUAL(LOW, digitalRead(LED_CONNECTED_PIN));
  delay(500);

  digitalWrite(RELAY_CH1_PIN, HIGH);
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH1_PIN));
  delay(500);
  digitalWrite(RELAY_CH1_PIN, LOW);
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH1_PIN));
  delay(500);

  digitalWrite(RELAY_CH2_PIN, HIGH);
  TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_CH2_PIN));
  delay(500);
  digitalWrite(RELAY_CH2_PIN, LOW);
  TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_CH2_PIN));
  delay(500);
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // TODO move this to a kind of GPIO_Init
  pinMode(LED_ALIVE_PIN, OUTPUT);
  pinMode(LED_CONNECTED_PIN, OUTPUT);
  pinMode(RELAY_CH1_PIN, OUTPUT);
  pinMode(RELAY_CH2_PIN, OUTPUT);

  UNITY_BEGIN(); // IMPORTANT LINE!
  // RUN_TEST(test_gpios);
}

void loop() {
  RUN_TEST(test_gpios);
  // delay(500);

  RUN_TEST(Measurement_UT);
  RUN_TEST(MeasurementPivot_UT);
  RUN_TEST(MeasurementPivotUpdateValue_UT);

  RUN_TEST(MeasurementPivotUpdateConfig_UT);
  RUN_TEST(MeasurementPivotCreateId_UT);

  RUN_TEST(CrcAlgorithmTest_1);
  RUN_TEST(CrcAlgorithmTest_2);
  RUN_TEST(CrcCode_1);
  RUN_TEST(CrcCode_2);
  RUN_TEST(CrcCode_3);
  RUN_TEST(CrcCode_4);
  RUN_TEST(CrcCode_5);
  RUN_TEST(CrcCode_6);
  RUN_TEST(CrcCode_7);



  UNITY_END(); // stop unit testing
}
