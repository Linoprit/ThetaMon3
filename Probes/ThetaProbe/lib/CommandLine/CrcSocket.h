/*
 * CrcSocket.h
 *
 *  Created on: 03.10.2020
 *      Author: harald
 */

#ifndef SOCKETS_CRCSOCKET_H_
#define SOCKETS_CRCSOCKET_H_

#include "CalcCrc.h"
#include <Arduino.h>
#include <cstdint>
#include <esp32/rom/crc.h>

constexpr uint8_t UINT32_BYTELEN = sizeof(uint32_t);

class CrcSocket {
public:
  CrcSocket(){};
  virtual ~CrcSocket(){};

  //////////////////////
  // void resetCrc(void) { actCrc = DEFAULT_CRC_INITVALUE; }

  // uint32_t calcCrcUpdate(uint32_t data) {
  //   uint8_t i;
  //   actCrc = actCrc ^ data;
  //   for (i = 0; i < 32; i++) {
  //     if (actCrc & 0x80000000)
  //       actCrc = (actCrc << 1) ^ DEFAULT_CRC32_POLY;
  //     else
  //       actCrc <<= 1;
  //   }
  //   return actCrc;
  // }

  // uint32_t HAL_CRC_Accumulate(uint32_t pBuffer[], uint32_t BufferLength) {
  //   uint32_t i;

  //   for (i = 0U; i < BufferLength; i++) {
  //     calcCrcUpdate(pBuffer[i]);
  //   }
  //   return actCrc;
  // }

  static uint32_t HAL_CRC_Calculate(uint32_t pBuffer[], uint32_t BufferLength) {
    CalcCrc calcCrc;

    calcCrc.resetCrc();
    for (uint32_t i = 0U; i < BufferLength; i++) {
      calcCrc.calcCrcUpdate(pBuffer[i]);
    }
    return calcCrc.GetActCrc();
  }
  ////////////////////////////
  /**
   * Calls the CRC-Engine and returns a 8-bit CRC, which is
   * simply the truncation of the 32-bit result.
   * !! dataLen MUST BE DIVIDABLE BY 4 !!
   * Params:
   * data: pointer to array
   * dataLen: size of the Array in bytes
   */
  static uint8_t calcChksum(uint8_t *data, uint8_t dataByteLen) {
    if ((dataByteLen % 4) != 0) {
      return 0;
    }
    uint32_t crc =
        HAL_CRC_Calculate(reinterpret_cast<uint32_t *>(data), dataByteLen / 4);
    return static_cast<uint8_t>(crc);
  };

  /**
   * Calculates the size in uint32 multiples. Takes in the size in Bytes and
   * returns how many uint32 values these are, if rounded up. i.E. char[3] = 3
   * bytes = 1 uint32; char[5] = 5 bytes = 2 uint32;
   */
  static uint32_t calcUint32Len(uint32_t sizeInBytes) {
    uint32_t len32 = (sizeInBytes + 3) / 4;
    return len32;
  };

  /**
   * When we calculate the length for strings, we have to ignore the 
	 * trailing '\0'. It should not be part of the checksum, i.e. when
	 * calculating hashes for command-strings.
   */
  static uint32_t calcUint32LenString(uint32_t sizeInBytes) {
    // We only get strings in here, so we must ignore '\0' at the end.
    sizeInBytes = sizeInBytes - 1;
    return calcUint32Len(sizeInBytes);
  };

  /**
   * Takes in data of any size, buffers it in a uint32-array and pads the
   * remainig bytes. The resulting array can directly be passed to the
   * CRC-engine.
   */
  static uint32_t calcBufferedChkSum32(uint8_t *data, uint32_t dataByteLen) {
    uint32_t uint32len = calcUint32LenString(dataByteLen);
    uint32_t data32[uint32len];
    uint8_t *uint8Data = reinterpret_cast<uint8_t *>(data32);

    // forget memset, it works bytewise
    for (uint_fast8_t i = 0; i < uint32len; i++) {
      data32[i] = 0;
    }
    for (uint_fast8_t i = 0; i < dataByteLen; i++) {
      uint8Data[i] = data[i];
    }
    return CrcSocket::calcChksum32(data32, uint32len);
  };
  /**
   * MainFunction, passes the data to the crc-engine
   */
  static uint32_t calcChksum32(uint32_t *data, uint32_t dataLen32) {
    uint32_t crc = HAL_CRC_Calculate(data, dataLen32);
    return crc;
  };

private:
  uint32_t actCrc; // current crc-value
  static constexpr uint32_t DEFAULT_CRC32_POLY = 0x04C11DB7U;
  static constexpr uint32_t DEFAULT_CRC_INITVALUE = 0xFFFFFFFFU;
};

#endif /* SOCKETS_CRCSOCKET_H_ */
