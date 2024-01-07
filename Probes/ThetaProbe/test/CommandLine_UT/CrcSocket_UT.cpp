/*
 * Unit_CrcSocket.cpp
 *
 *  Created on: 14.03.2021
 *      Author: harald
 */

#include "ComLineConfig.h"
#include "CrcSocket.h"
#include <Arduino.h>
#include <unity.h>

#include <esp32/rom/crc.h>

void printUint64(uint64_t val) {
  // Serial.printf("%08lx%08lx\r\n", ((uint32_t)((val >> 32) & 0xFFFFFFFF)),
  //               ((uint32_t)(val & 0xFFFFFFFF)));
  Serial.printf("%08lx\r\n", val);
}

void CrcAlgorithmTest_1() {
  uint8_t data = 0xC1;
  uint8_t poly = 0xCB;
  uint8_t initialCrc = 0xFF;

  // Result in single steps
  // 0x3E 0x7C 0xF8 0x3B 0x76 0xEC 0x13 0x26 0x4C
  uint8_t i;
  uint8_t actCrc = initialCrc ^ data;
  for (i = 0; i < 8; i++) {
    if (actCrc & 0x80)
      actCrc = (actCrc << 1) ^ poly;
    else
      actCrc <<= 1;
  }
  TEST_ASSERT_EQUAL(actCrc, 0x4CU);
}

void CrcAlgorithmTest_2() {
  CalcCrc calcCrc;
  calcCrc.resetCrc();

  uint32_t data = 0x11223344U;

  uint32_t crc = calcCrc.calcCrcUpdate(data);

  // crc = ~crc32_le((uint32_t)~0x00000000, (uint8_t *)&data, UINT32_BYTELEN);
  // printUint64(crc);
  // // CCITT - FALSE,
  // crc = ~crc32_be((uint32_t)~0xffffffff, (uint8_t *)&data, UINT32_BYTELEN);
  // printUint64(crc);
  // // X25,
  // crc = (~crc32_le((uint32_t)~0xffffffff, (uint8_t *)&data, UINT32_BYTELEN))
  // ^
  //       0xffffffff;
  // printUint64(crc);
  // // XMODEM,
  // crc = ~crc32_be((uint32_t)~0x00000000, (uint8_t *)&data, UINT32_BYTELEN);
  // printUint64(crc);
  // printUint64(0xB14257CCU);

  TEST_ASSERT_EQUAL(crc, 0xB14257CCU);
}

void CrcCode_1() {
  uint32_t result = 0;
  cLine::CmdBufferType cmdBuffer;
  cmdBuffer.fill('\0');

  uint8_t test1[] = "e2dumpjournal";
  volatile uint32_t uint8Len_test1 = sizeof(test1);

  memcpy(cmdBuffer.data(), test1, uint8Len_test1);

  result = CrcSocket::HAL_CRC_Calculate(
      reinterpret_cast<uint32_t *>(cmdBuffer.data()),
      CrcSocket::calcUint32LenString(sizeof(test1)));

  TEST_ASSERT_EQUAL(result, 1884395996U);
}

void CrcCode_2() {
  uint32_t result = 0;
  cLine::CmdBufferType cmdBuffer;
  cmdBuffer.fill('\0');

  char test1[] = "e2init";
  volatile uint32_t uint8Len_test1 = sizeof(test1);
  memcpy(cmdBuffer.data(), test1, uint8Len_test1);
  result = CrcSocket::HAL_CRC_Calculate(
      reinterpret_cast<uint32_t *>(cmdBuffer.data()),
      CrcSocket::calcUint32LenString(sizeof(test1)));

  TEST_ASSERT_EQUAL(result, 3743823511U);
}

void CrcCode_3() {
  uint32_t result = 0;
  cLine::CmdBufferType cmdBuffer;
  cmdBuffer.fill('\0');

  char test1[] = "e2dumpjournalmap";
  volatile uint32_t uint8Len_test1 = sizeof(test1);
  memcpy(cmdBuffer.data(), test1, uint8Len_test1);

  result = CrcSocket::HAL_CRC_Calculate(
      reinterpret_cast<uint32_t *>(cmdBuffer.data()),
      CrcSocket::calcUint32LenString(sizeof(test1)));

  TEST_ASSERT_EQUAL(result, 2362017329U);
}

void CrcCode_4() {
  uint32_t result = 0;
  cLine::CmdBufferType cmdBuffer;
  cmdBuffer.fill('\0');

  char test1[] = "e2erase";
  volatile uint32_t uint8Len_test1 = sizeof(test1);
  memcpy(cmdBuffer.data(), test1, uint8Len_test1);
  result = CrcSocket::HAL_CRC_Calculate(
      reinterpret_cast<uint32_t *>(cmdBuffer.data()),
      CrcSocket::calcUint32LenString(sizeof(test1)));

  TEST_ASSERT_EQUAL(result, 3704805521U);
}

void CrcCode_5() {
  uint32_t result = 0;
  cLine::CmdBufferType cmdBuffer;
  cmdBuffer.fill('\0');

  char test1[] = "e2dumpcalib";
  volatile uint32_t uint8Len_test1 = sizeof(test1);
  memcpy(cmdBuffer.data(), test1, uint8Len_test1);
  result = CrcSocket::HAL_CRC_Calculate(
      reinterpret_cast<uint32_t *>(cmdBuffer.data()),
      CrcSocket::calcUint32LenString(sizeof(test1)));

  TEST_ASSERT_EQUAL(result, 2821683926U);
}

void CrcCode_6() {
  uint32_t result = 0;
  cLine::CmdBufferType cmdBuffer;
  cmdBuffer.fill('\0');

  char test1[] = "e2dumpmem";
  volatile uint32_t uint8Len_test1 = sizeof(test1);
  memcpy(cmdBuffer.data(), test1, uint8Len_test1);
  result = CrcSocket::HAL_CRC_Calculate(
      reinterpret_cast<uint32_t *>(cmdBuffer.data()),
      CrcSocket::calcUint32LenString(sizeof(test1)));

  TEST_ASSERT_EQUAL(result, 1704472323U);
}

void CrcCode_7() {
  uint32_t result = 0;
  cLine::CmdBufferType cmdBuffer;
  cmdBuffer.fill('\0');

  char test1[] = "e2dumpstart";
  volatile uint32_t uint8Len_test1 = sizeof(test1);
  memcpy(cmdBuffer.data(), test1, uint8Len_test1);
  result = CrcSocket::HAL_CRC_Calculate(
      reinterpret_cast<uint32_t *>(cmdBuffer.data()),
      CrcSocket::calcUint32LenString(sizeof(test1)));

  TEST_ASSERT_EQUAL(result, 302299815U);
}
