// // Animates white pixels to simulate flying through a star field

// #include <Arduino.h>
// #include "TasksCommon.h"

// volatile QueueHandle_t ledQueue = NULL;
// TaskHandle_t gpioTaskHandle = NULL;
// TaskHandle_t picoTaskHandle = NULL;
// TaskHandle_t sensorTaskHandle = NULL;
// TaskHandle_t displayTaskHandle = NULL;

// void setup()
// {
//   esp_task_wdt_init(30, false);

//   setupDisplay();

//   xTaskCreate(led_task_pico, "PICO_LED_TASK", 1024, NULL, 1,
//   &picoTaskHandle); xTaskCreate(led_task_gpio, "GPIO_LED_TASK", 1024, NULL,
//   1, &gpioTaskHandle); xTaskCreate(ReadSensors, "READ_SENSORS_TASK", 1024,
//   NULL, 1, &sensorTaskHandle); xTaskCreate(DisplayTask, "DisplayTask", 1024,
//   NULL, 1, &displayTaskHandle);

//   ledQueue = xQueueCreate(4, sizeof(uint8_t));
// }

// int value = 0;
// void loop()
// {
//   vTaskDelay(500);
// }
/*
  Rui Santos
  Complete project details at
  https://RandomNerdTutorials.com/esp32-mqtt-publish-ds18b20-temperature-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include "Config.h"
#include "Measurement.h"
#include "TasksCommon.h"


// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(192, 168, 178, 24)
// For a cloud MQTT broker, type the domain name
// #define MQTT_HOST "example.com"
#define MQTT_PORT 1883

// Temperature MQTT Topic
#define MQTT_PUB_TEMP "esp32/ds18b20/temperature"

// GPIO where the DS18B20 is connected to
const int oneWireBus = 25;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
// Temperature value
float temp;

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

unsigned long previousMillis = 0; // Stores last time temperature was published
const long interval = 10000; // Interval at which to publish sensor readings

// Common tasks and queues definition
TaskHandle_t measureTaskHandle = NULL;
volatile QueueHandle_t measurementQueue = NULL;
SemaphoreHandle_t measurementArraySmphr = NULL;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    connectToMqtt();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    xTimerStop(
        mqttReconnectTimer,
        0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
    break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

/*void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}*/

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  // Start the DS18B20 sensor
  sensors.begin();

  Serial.begin(115200);
  Serial.println();

  // init common tasks and queues
  xTaskCreate(startUpdateTask, "MEASURE_TASK", 1024, NULL, 1,
              &measureTaskHandle);
  measurementQueue = xQueueCreate(MAX_SENSORS, sizeof(msmnt::MeasurementType));
  measurementArraySmphr = xSemaphoreCreateCounting(5, 0);

  //
  //
  //

  mqttReconnectTimer =
      xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer =
      xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  // mqttClient.onSubscribe(onMqttSubscribe);
  // mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them
  // below mqttClient.setCredentials("REPlACE_WITH_YOUR_USER",
  // "REPLACE_WITH_YOUR_PASSWORD");
  connectToWifi();
}

void loop() {
  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds)
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    // New temperature readings
    sensors.requestTemperatures();
    // Temperature in Celsius degrees
    temp = sensors.getTempCByIndex(0);
    // Temperature in Fahrenheit degrees
    // temp = sensors.getTempFByIndex(0);

    // Publish an MQTT message on topic esp32/ds18b20/temperature
    uint16_t packetIdPub1 =
        mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(temp).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: ", MQTT_PUB_TEMP);
    Serial.println(packetIdPub1);
    Serial.printf("Message: %.2f /n", sensors.getTempCByIndex(0));
  }
}
