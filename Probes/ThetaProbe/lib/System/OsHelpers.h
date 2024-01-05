#ifndef OSHELPERS_H
#define OSHELPERS_H

#pragma once

#include <WiFi.h>

class OsHelpers
{
public:
    OsHelpers() {};
    ~OsHelpers() {};

    // Wifi must be startet with Wifi.begin(...)
    // buffer must point to a 6-byte array
    static void GetMacAddress(uint8_t* buffer){        
        WiFi.macAddress(buffer);
    }


private:

};

#endif