#ifndef MQLOG_H
#define MQLOG_H

#include <stdint.h>

#pragma once

int MqLog(const char *format, ...);
int MqLogCycle(void);

// class MqLog
// {
// public:
//     MqLog();
//     virtual ~MqLog(){};

// private:

// };

#endif