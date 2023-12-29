/*
 * TaksCommon.h
 *
 *  Created on: Dec 14, 2023
 *      Author: harald
 */

#ifndef SRC_TASKS_TASKSCOMMON_H_
#define SRC_TASKS_TASKSCOMMON_H_


#ifdef __cplusplus
extern "C" {
#endif

void led_task_gpio(void* unused_arg);
void led_task_pico(void* unused_arg);
void ReadSensors(void* unused_arg);
void startUpdateTask(void* unused_arg);

#ifdef __cplusplus
}           // extern "C"
#endif

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_task_wdt.h"

// act measured values are put in here
extern volatile QueueHandle_t measurementQueue;
extern SemaphoreHandle_t measurementArraySmphr;


// from examples, to be deleted.
#define         LED_1_PIN           32
#define         LED_2_PIN           33
extern volatile QueueHandle_t ledQueue;
const TickType_t ms_delay = 1000 / portTICK_PERIOD_MS;

#endif /* SRC_TASKS_TASKSCOMMON_H_ */
