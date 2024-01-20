/*
 * GpioLedTask.cpp
 *
 *  Created on: Dec 14, 2023
 *      Author: harald
 */

#include "TasksCommon.h"

// Todo remove
void led_task_gpio(void* unused_arg) {

    uint8_t passed_value_buffer = 0;

    pinMode(LED_1_PIN, OUTPUT);

    while (true) {
        // Serial.print("SMP: ExtLed task running on core: ");
        // Serial.println(xPortGetCoreID());

        if (xQueueReceive(ledQueue, &passed_value_buffer, portMAX_DELAY) == pdPASS) {
            if (passed_value_buffer) Serial.println(" GPIO LED FLASH\n");
            digitalWrite(LED_1_PIN, passed_value_buffer == 1 ? 0 : 1);
        }


    }
}
