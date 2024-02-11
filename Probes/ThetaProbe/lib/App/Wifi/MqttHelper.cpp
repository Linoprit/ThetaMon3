#include "MqttHelper.h"
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <FileSystem/LittleFsHelpers.h>
#include <WiFi.h>

namespace wifi {

using namespace msmnt;

/// begin Mqtt callbacks and timer
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  //Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.printf("WIFI is connected, local IP: ");
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
  Serial.print("Connected to MQTT. Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.printf("Pub ack. Id: %lu\n", packetId);
}

void connectToWifi() {
  Serial.println("Reconnecting to Wi-Fi...");
  nvm::LittleFsHelpers::instance().readMqttConf();
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}
/// end Mqtt callbacks and timer

MqttHelper::MqttHelper() {
  _mqttPubLog = new char[MQTT_PATHLEN];
  _mqttPubSens = new char[MQTT_PATHLEN];
  _mqttSubCmd = new char[MQTT_PATHLEN];

  char noneBuff[] = "none";
  strcpy(_mqttPubLog, noneBuff);
  strcpy(_mqttPubSens, noneBuff);
  strcpy(_mqttSubCmd, noneBuff);
}

void MqttHelper::MqttSetup() {
  mqttReconnectTimer =
      xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer =
      xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  // mqttClient.onSubscribe(onMqttSubscribe);
  // mqttClient.onUnsubscribe(onMqttUnsubscribe);
  // mqttClient.onPublish(onMqttPublish);

  // Connection to Wifi is done by reading the MqttConfig file.
  nvm::LittleFsHelpers::instance().readMqttConf();

  mqttClient.setServer(_mqttHost, _mqttPort);
  // If your broker requires authentication (username and password), set them
  // below mqttClient.setCredentials("REPlACE_WITH_YOUR_USER",
  // "REPLACE_WITH_YOUR_PASSWORD");

  WiFi.onEvent(WiFiEvent);
}

void MqttHelper::printMqttConf() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWIFI is connected, local IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.printf("\nWifi is not  connected.\n");
  }

  Serial.printf("MqttHost: %i.%i.%i.%i:%lu\n", _mqttHost[0], _mqttHost[1],
                _mqttHost[2], _mqttHost[3], _mqttPort);

  Serial.printf("Mqtt publish sensordata: '%s'\n", _mqttPubSens);
  Serial.printf("Mqtt publish logs: '%s'\n", _mqttPubLog);
  Serial.printf("Mqtt subscribe commands: '%s'\n", _mqttSubCmd);
}

void MqttHelper::PubishMeasurements(MeasurementPivot *measurementPivot) {

  // uint64: 20, float: 6, spaces: 1, EOL: 1
  char buff[30];

  measurementPivot->ResetIter();
  Measurement *actMeasurement = measurementPivot->GetNextMeasurement();

  while (actMeasurement != nullptr) {
    sprintf(buff, "%llu %.02f;\0", actMeasurement->sensorId,
            actMeasurement->meanValue);

    uint16_t packetIdPub1 = mqttClient.publish(_mqttPubSens, 1, true, buff);
    //Serial.printf("Pub on topic '%s' at QoS 1, Id: %lu\n", _mqttPubSens,
    //              packetIdPub1);

    actMeasurement = measurementPivot->GetNextMeasurement();
  }
}

} // namespace wifi