#ifndef OSHELPERS_H
#define OSHELPERS_H

//#pragma once

#include "esp_heap_caps_init.h"
#include <Arduino.h>
#include <WiFi.h>

class OsHelpers {
public:
  OsHelpers(){};
  ~OsHelpers(){};

  // buffer must point to a 6-byte array
  static void GetMacAddress(uint8_t *buffer) { WiFi.macAddress(buffer); }
  static inline uint32_t get_tick(void) { return millis(); };
  static inline void SYSTEM_REBOOT() { ESP.restart(); }
  static inline void SYSTEM_EXIT() { ESP.restart(); }
  // static inline void NOP(void) { asm("NOP"); } // Use built in

  // for ESP32
  static void *callocHeap(size_t size) {    
    return heap_caps_calloc(1, size, MALLOC_CAP_8BIT);
  }
  static void freeHeap(void *ptr) { heap_caps_free(ptr); }

private:
};

#endif