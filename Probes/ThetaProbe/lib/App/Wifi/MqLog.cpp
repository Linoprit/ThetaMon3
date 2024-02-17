#include "MqLog.h"
#include "Wifi/MqttHelper.h"
#include <AppTypes.h>
#include <Arduino.h>
#include <cstring>
#include <stdarg.h>
#include <stdio.h>

constexpr uint16_t TX_BUFF_LEN = 1024;

constexpr uint16_t TMP_BUFF_LEN = 512;
uint8_t txBuff[TX_BUFF_LEN];
uint16_t tx_act_pos = 0;

extern SemaphoreHandle_t mqBuffSemHandle;

uint16_t MqLogBuffFreeBytes(void) { return TX_BUFF_LEN - tx_act_pos; }

int MqLogCycle(void) {

  if (tx_act_pos > 0) {
    uint8_t result = _FAIL_;

    if (xSemaphoreTake(mqBuffSemHandle, 10) != pdTRUE) {
      return _FAIL_;
    }
    result = wifi::MqttHelper::instance().publishLog(&txBuff[0], tx_act_pos);    
    xSemaphoreGive(mqBuffSemHandle);

    // buffer is in transmission, we must not clear it!
    tx_act_pos = 0;
    return result;
  }
  return _SUCCESS_;
}

// NOT threadsave!!
void MqPutchar(char character) {
  txBuff[tx_act_pos] = character;
  tx_act_pos++;
  if (tx_act_pos >= (TX_BUFF_LEN - 1)) {
    tx_act_pos = TX_BUFF_LEN - 1;
  }
}


int MqLog(const char *format, ...) {
  uint8_t tmpBuff[TMP_BUFF_LEN];

  va_list arg;
  va_start(arg, format);
  uint16_t tmpLen = vsnprintf((char *)&tmpBuff, TX_BUFF_LEN, format, arg);
  va_end(arg);

  if (MqLogBuffFreeBytes() < tmpLen)
    return _FAIL_;

  if (xSemaphoreTake(mqBuffSemHandle, 50) != pdTRUE) {
    return _FAIL_;
  }
  std::memcpy(&txBuff[tx_act_pos], tmpBuff, tmpLen);
  tx_act_pos += tmpLen;
  xSemaphoreGive(mqBuffSemHandle);

  return _SUCCESS_;
}
