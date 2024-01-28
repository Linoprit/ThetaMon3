//#include <Application/ThetaSensors/ID_Table.h>
#include <Sensors/Measurement.h>
#include <Arduino.h>
#include <CommandLine/ComLineConfig.h>
#include <Interpreter/Lexer.h>
#include <Interpreter/Token.h>
#include <unity.h>

// UInt64 0 to 18,446,744,073,709,551,615
// UInt32 0 to 4,294,967,295
using namespace cLine;

void LexerCommandTest() {
  CmdBufferType comLine;
  char command[] = "  e2dumpjournalmap abc";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  CmdToken *cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getVal(), 2362017329U);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
}

void LexerUInt32Test() {
  CmdBufferType comLine;
  char command[] = "  12345 abc";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  UInt64Token *cmdToken = (UInt64Token *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getVal(), 12345U);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::UInt64);
}

void LexerUInt32AsUInt64Test() {
  CmdBufferType comLine;
  char command[] = "  4294967295 abc";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  UInt64Token *cmdToken = (UInt64Token *)lex.getNextToken();

  //
  Serial.printf("in Test Expect: %lu Actual: %llu\r\n", 429496725,
                cmdToken->getVal());
  TEST_ASSERT_TRUE(cmdToken->getVal() == 4294967295U);
  //

  TEST_ASSERT_EQUAL(cmdToken->getVal(), 4294967295U);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::UInt64);
}

void LexerUInt64Test() {
  CmdBufferType comLine;
  char command[] = "  4294967296 abc";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  UInt64Token *cmdToken = (UInt64Token *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getVal(), 4294967296U);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::UInt64);
}

void LexerSignInt32Test() {
  CmdBufferType comLine;
  char command[] = "  -12345 abc";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  SInt32Token *cmdToken = (SInt32Token *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getVal(), -12345);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::SInt32);
}

void LexerFloatTest() {
  CmdBufferType comLine;
  char command[] = "  12345.5678 abc";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  FltToken *cmdToken = (FltToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL_FLOAT(cmdToken->getVal(), 12345.5678F);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Float);
}

void LexerNegFloatTest() {
  CmdBufferType comLine;
  char command[] = "  -12345.5678 abc";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  FltToken *cmdToken = (FltToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL_FLOAT(cmdToken->getVal(), -12345.5678F);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Float);
}

void LexerStringTest() {
  CmdBufferType comLine;
  char command[] = "  \"einszwo\"  ";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  ChrToken *cmdToken = (ChrToken *)lex.getNextToken();
  std::string result(cmdToken->getVal());
  std::string ref("einszwo");
  TEST_ASSERT_TRUE(ref == result);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::String);
}

void LexerStringAllChars() {
  CmdBufferType comLine;
  char command[] = "  \"#&[]()@+{};:<>|~\"  ";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  ChrToken *cmdToken = (ChrToken *)lex.getNextToken();
  std::string result(cmdToken->getVal());
  std::string ref("#&[]()@+{};:<>|~");
  TEST_ASSERT_TRUE(ref == result);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::String);
}

void LexerStringOverflow() {
  CmdBufferType comLine;
  char command[] = "  \"einszwodreivierfuenfsechssiebenacht\"  ";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  ChrToken *cmdToken = (ChrToken *)lex.getNextToken();
  std::string result(cmdToken->getVal());
  std::string ref("einszwodreivierfuenfsechssieben");
  TEST_ASSERT_TRUE(ref == result);
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::String);
}

void LexerStringNotClosed() {
  CmdBufferType comLine;
  char command[] = "  \"einszwodrei  vier";
  memcpy(comLine.data(), command, sizeof(command));

  Lexer lex(&comLine);
  ChrToken *cmdToken = (ChrToken *)lex.getNextToken();
  TEST_ASSERT_TRUE(*cmdToken->getVal() == '\0');
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::String);
}

void printU64(uint64_t value) {
  uint32_t *valPtr = (uint32_t *)&value;
  Serial.printf("%08lx %08lx\n", valPtr[0], valPtr[1]);
}

bool compareU64(uint64_t left, uint64_t right) {
  uint32_t *leftPtr = (uint32_t *)&left;
  uint32_t *rightPtr = (uint32_t *)&right;

  return (leftPtr[0] == rightPtr[0]) && (leftPtr[1] == rightPtr[1]);
}

msmnt::SensorConfigType getThetaStruct(Lexer *lex) {
  msmnt::SensorConfigType sens;

  UInt64Token *int64Token = (UInt64Token *)lex->getNextToken();
  TEST_ASSERT_EQUAL(int64Token->getType(), Token::UInt64);
  sens.sensorId = (uint64_t)int64Token->getVal();

  FltToken *fltToken = (FltToken *)lex->getNextToken();
  TEST_ASSERT_EQUAL_FLOAT(fltToken->getType(), Token::Float);
  sens.minVal = fltToken->getVal();

  fltToken = (FltToken *)lex->getNextToken();
  TEST_ASSERT_EQUAL_FLOAT(fltToken->getType(), Token::Float);
  sens.maxVal = fltToken->getVal();

  UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
  TEST_ASSERT_EQUAL(intToken->getType(), Token::UInt64);
  sens.sensType =
      static_cast<msmnt::Measurement::SensorType>(intToken->getVal());

  intToken = (UInt64Token *)lex->getNextToken();
  TEST_ASSERT_EQUAL(intToken->getType(), Token::UInt64);
  sens.relayNr =
      static_cast<msmnt::Measurement::RelayChannel>(intToken->getVal());

  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  TEST_ASSERT_EQUAL(chrToken->getType(), Token::String);  
  sens.CopyToShortname(chrToken->getVal());

  return sens;
}

void HelpCopyToShortname_UT(int testNr, char *testChArr, string expect) {
  char shortname[msmnt::Measurement::SHORTNAME_LEN] = {'\0'};

  msmnt::Measurement::CopyToShortname(shortname, testChArr);
  std::string testStr = std::string(shortname, msmnt::Measurement::SHORTNAME_LEN);

  Serial.printf("shortnames %i: Expect='%s' / Act='%s'\n", testNr,
                expect.c_str(), testStr.c_str());
  TEST_ASSERT_TRUE(expect == testStr);
}

void CopyToShortname_UT() {
  HelpCopyToShortname_UT(1, (char*) "INNEN   ", "INNEN   ");
  HelpCopyToShortname_UT(2, (char*) "INNENRaumSensor", "INNENRau");
  HelpCopyToShortname_UT(3, (char*) "INNEN\0   ", "INNEN   ");
  HelpCopyToShortname_UT(4, (char*) "INNEN", "INNEN   ");
}

void LexerCommandLine() {
  CmdBufferType comLine;

  Serial.print("Test 01: ");
  char command1[] = " setSensId 3103548024 -10.0	10.0	0	0 \"INNEN\" ";
  memcpy(comLine.data(), command1, sizeof(command1));
  Lexer lex(&comLine);

  CmdToken *cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  uint32_t cmd = cmdToken->getVal();
  TEST_ASSERT_EQUAL(2358927868U, cmd);

  msmnt::SensorConfigType sens = getThetaStruct(&lex);
  Serial.printf("Expected %lu, Actual %llu\n", 3103548024U, sens.sensorId);
  TEST_ASSERT_TRUE(3103548024U == sens.sensorId);
  TEST_ASSERT_EQUAL_FLOAT(-10.0F, sens.minVal);
  TEST_ASSERT_EQUAL_FLOAT(10.0F, sens.maxVal);
  TEST_ASSERT_EQUAL(0, sens.sensType);
  TEST_ASSERT_EQUAL(0, sens.relayNr);
  std::string value1(sens.shortname, msmnt::Measurement::SHORTNAME_LEN);
  std::string ref1("INNEN   ", msmnt::Measurement::SHORTNAME_LEN);  
  TEST_ASSERT_TRUE(ref1 == value1);

  Serial.print("Test 02: ");
  char command2[] = " setSensId 1294211458 -2.0 10.0  0 1 \"WST_O\" ";
  comLine.fill('\0');
  memcpy(comLine.data(), command2, sizeof(command2));
  lex.setComLine(&comLine);

  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  cmd = cmdToken->getVal();
  TEST_ASSERT_EQUAL(2358927868U, cmd);

  sens = getThetaStruct(&lex);
  Serial.printf("Expected %lu, Actual %llu\n", 1294211458U, sens.sensorId);
  TEST_ASSERT_TRUE(1294211458U == sens.sensorId);
  TEST_ASSERT_EQUAL_FLOAT(-2.0F, sens.minVal);
  TEST_ASSERT_EQUAL_FLOAT(10.0F, sens.maxVal);
  TEST_ASSERT_EQUAL(0, sens.sensType);
  TEST_ASSERT_EQUAL(1, sens.relayNr);
  std::string value2(sens.shortname, msmnt::Measurement::SHORTNAME_LEN);
  std::string ref2("WST_O   ", msmnt::Measurement::SHORTNAME_LEN);
  TEST_ASSERT_TRUE(ref2 == value2);

  Serial.print("Test 03: ");
  char command3[] = "setSensId 3932845497 -4.2 9.8 0 1 \"WST_U\" ";
  comLine.fill('\0');
  memcpy(comLine.data(), command3, sizeof(command3));
  lex.setComLine(&comLine);

  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  cmd = cmdToken->getVal();
  TEST_ASSERT_EQUAL(2358927868U, cmd);

  sens = getThetaStruct(&lex);
  Serial.printf("Expected %lu, Actual %llu\n", 3932845497, sens.sensorId);
  TEST_ASSERT_TRUE(3932845497U == sens.sensorId);
  TEST_ASSERT_EQUAL_FLOAT(-4.2F, sens.minVal);
  TEST_ASSERT_EQUAL_FLOAT(9.8F, sens.maxVal);
  TEST_ASSERT_EQUAL(0, sens.sensType);
  TEST_ASSERT_EQUAL(1, sens.relayNr);
  std::string value3(sens.shortname, msmnt::Measurement::SHORTNAME_LEN);
  std::string ref3("WST_U   ", msmnt::Measurement::SHORTNAME_LEN);
  TEST_ASSERT_TRUE(ref3 == value3);

  Serial.print("Test 04: ");
  char command4[] = "setSensId	3159888747 -8.56 7.75 10 1	\"GGE_O\"";
  comLine.fill('\0');
  memcpy(comLine.data(), command4, sizeof(command4));
  lex.setComLine(&comLine);

  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  cmd = cmdToken->getVal();
  TEST_ASSERT_EQUAL(2358927868U, cmd);

  sens = getThetaStruct(&lex);
  Serial.printf("Expected %lu, Actual %llu\n", 3159888747, sens.sensorId);
  TEST_ASSERT_TRUE(3159888747U == sens.sensorId);
  TEST_ASSERT_EQUAL_FLOAT(-8.56F, sens.minVal);
  TEST_ASSERT_EQUAL_FLOAT(7.75F, sens.maxVal);
  TEST_ASSERT_EQUAL(10, sens.sensType);
  TEST_ASSERT_EQUAL(1, sens.relayNr);
  std::string value4(sens.shortname, msmnt::Measurement::SHORTNAME_LEN);
  std::string ref4("GGE_O   ", msmnt::Measurement::SHORTNAME_LEN);
  TEST_ASSERT_TRUE(ref4 == value4);
}

void LexerPrintCmdHashes() {
  char getSensIdTable[] = "getSensIdTable";
  char setStationId[] = "setStationId";
  char getStationId[] = "getStationId";
  char reboot[] = "reboot";
  char clrSensIdTable[] = "clrSensIdTable";
  char calcHash[] = "calcHash";

  CmdBufferType comLine;
  comLine.fill('\0');
  memcpy(comLine.data(), getSensIdTable, sizeof(getSensIdTable));
  Lexer lex(&comLine);

  CmdToken *cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  Serial.printf("getSensIdTable: %u\n", cmdToken->getVal());

  comLine.fill('\0');
  memcpy(comLine.data(), setStationId, sizeof(setStationId));
  lex.setComLine(&comLine);
  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  Serial.printf("setStationId: %u\n", cmdToken->getVal());

  comLine.fill('\0');
  memcpy(comLine.data(), getStationId, sizeof(getStationId));
  lex.setComLine(&comLine);
  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  Serial.printf("getStationId: %u\n", cmdToken->getVal());

  comLine.fill('\0');
  memcpy(comLine.data(), reboot, sizeof(reboot));
  lex.setComLine(&comLine);
  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  Serial.printf("reboot: %u\n", cmdToken->getVal());

  comLine.fill('\0');
  memcpy(comLine.data(), clrSensIdTable, sizeof(clrSensIdTable));
  lex.setComLine(&comLine);
  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  Serial.printf("clrSensIdTable: %u\n", cmdToken->getVal());

  comLine.fill('\0');
  memcpy(comLine.data(), calcHash, sizeof(calcHash));
  lex.setComLine(&comLine);
  cmdToken = (CmdToken *)lex.getNextToken();
  TEST_ASSERT_EQUAL(cmdToken->getType(), Token::Command);
  Serial.printf("calcHash: %u\n", cmdToken->getVal());
}
