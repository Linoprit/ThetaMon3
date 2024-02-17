#ifndef MQLOG_H
#define MQLOG_H

#include <stdint.h>

#pragma once

int MqLog(const char *format, ...);
int MqLogCycle(void);
// NOT threadsave
void MqPutchar(char character);

#endif