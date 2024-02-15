#include "CommandLine_UT/CrcSocket_UT.cpp"
#include "CommandLine_UT/Lexer_UT.cpp"
#include "Config.h"
#include "DigitalIo_UT/CheckRelays_UT.cpp"
#include "DigitalIo_UT/DigitalIo_UT.cpp"
#include "Sensors_UT/Measurement_UT.cpp"
#include <Arduino.h>
#include <WiFi.h>
#include <unity.h>

// To get printed output, run with: pio test -v

void setUp(void) { // setup stuff
}

void tearDown(void) { // clean stuff
}

void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  Serial.begin(115200);

  UNITY_BEGIN(); // IMPORTANT LINE!
}

void loop() {
  RUN_TEST(Measurement_UT);
  RUN_TEST(OutOfRanges_UT);
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

  RUN_TEST(LexerCommandTest);
  RUN_TEST(LexerUInt32Test);
  RUN_TEST(LexerUInt32AsUInt64Test);
  RUN_TEST(LexerUInt64Test);
  RUN_TEST(LexerSignInt32Test);
  RUN_TEST(LexerFloatTest);
  RUN_TEST(LexerNegFloatTest);
  RUN_TEST(LexerStringTest);
  RUN_TEST(LexerStringAllChars);
  RUN_TEST(LexerStringOverflow);
  RUN_TEST(LexerStringNotClosed);
  RUN_TEST(CopyToShortname_UT);
  RUN_TEST(LexerCommandLine);
  RUN_TEST(LexerPrintCmdHashes);

  RUN_TEST(DigitalIo_UT);
  RUN_TEST(CalcRelayStates_UT);

  UNITY_END(); // stop unit testing
}
