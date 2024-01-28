#ifndef OSHELPERS_H
#define OSHELPERS_H

//#pragma once

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

private:
};

#endif