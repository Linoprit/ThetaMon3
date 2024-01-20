/*
 * LedTask.cpp
 *
 *  Created on: Dec 14, 2023
 *      Author: harald
 */

#include "TasksCommon.h"

// Todo remove
void led_task_pico(void* unused_arg) {

    uint8_t pico_led_state = 0;
    pinMode(LED_2_PIN, OUTPUT);

    while (true) {
    	// Serial.print("IntLed task running on core: ");
        // Serial.println(xPortGetCoreID());

        pico_led_state = 1;
        digitalWrite(LED_2_PIN, pico_led_state);
        xQueueSendToBack(ledQueue, &pico_led_state, 0);
        vTaskDelay(ms_delay);

        pico_led_state = 0;
        digitalWrite(LED_2_PIN, pico_led_state);
        xQueueSendToBack(ledQueue, &pico_led_state, 0);
        vTaskDelay(ms_delay);
    }
}
